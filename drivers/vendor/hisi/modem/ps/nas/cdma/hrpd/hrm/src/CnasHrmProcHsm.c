

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "CnasHrmProcHsm.h"
#include "CnasHrmSndCas.h"
#include "CnasHrmSndTtf.h"
#include "CnasHrmSndHsm.h"
#include "CnasHrmCtx.h"
#include "CnasHrmCommFunc.h"
#include "CnasMntn.h"

#include "hsm_hrm_pif.h"
#include "Nas_Mem.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#define THIS_FILE_ID                    PS_FILE_ID_CNAS_HRM_PROC_HSM_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/
/*lint -save -e958*/


VOS_VOID CNAS_HRM_ProcHsmDataReqCommProcess(
    HSM_HRM_DATA_REQ_STRU              *pstDataReq
)
{
    CNAS_HRM_DATA_REQ_BUFF_STRU        *pstReqBuff = VOS_NULL_PTR;
    VOS_UINT16                          usOpId;
    VOS_UINT32                          ulMsgLen;
    VOS_UINT8                           ucIndex;

    ulMsgLen        =  pstDataReq->ulLength - sizeof(pstDataReq->enDataReqType);
    ucIndex         = CNAS_HRM_GetMsgBuffIndexByDataReqType(pstDataReq->enDataReqType);

    pstReqBuff = CNAS_HRM_GetDataReqBuffAddr(ucIndex);

    /* 收到CNAS_HRM_DATA_REQ_UATI_REQUEST(DSDS2.0)
            CNAS_HRM_DATA_REQ_UATI_CMPL(DSDS2.0)
            CNAS_HRM_DATA_REQ_HARDWARE_ID
            CNAS_HRM_DATA_REQ_MO_KEEP_ALIVE(DSDS2.0)
            CNAS_HRM_DATA_REQ_MT_KEEP_ALIVE
            CNAS_HRM_DATA_REQ_SESSION_CLOSE时，
        对应的缓存如果被使用或者资源未释放认为异常 */
    if (VOS_TRUE == pstReqBuff->stReqMsg.ulIsReqMsgExisted)
    {
        /* 缓存被使用，异常 */
        CNAS_WARNING_LOG(UEPS_PID_HRM, "CNAS_HRM_ProcHsmDataReqCommProcess: buff msg is existed");

        CNAS_HRM_ClearMsgInSpecifiedDataReqBuff(ucIndex);
    }

    if ((CNAS_HRM_RES_APPLY_STATE_APPLY_SUCC == pstReqBuff->stResInfo.enResApplyState)
     || (CNAS_HRM_RES_APPLY_STATE_APPLYING == pstReqBuff->stResInfo.enResApplyState))
    {
        /* 资源未释放，异常，但不重复申请 */
        CNAS_WARNING_LOG(UEPS_PID_HRM, "CNAS_HRM_ProcHsmDataReqCommProcess: res isn't released");
    }
    else
    {
        CNAS_HRM_SndCasHrpdResApplyReq(CNAS_HRM_GetTaskTypeByDataReqType(pstDataReq->enDataReqType));

        /* 判断是否需要等资源申请结果，如果不需要等结果将请求发给SNP，否则等收到CNF之后处理 */
        if (VOS_FALSE == CNAS_HRM_IsNeedWaitResApplied())
        {
            pstReqBuff->stResInfo.enResApplyState = CNAS_HRM_RES_APPLY_STATE_APPLY_SUCC;
        }
        else
        {
            pstReqBuff->stResInfo.enResApplyState = CNAS_HRM_RES_APPLY_STATE_APPLYING;
        }
    }


    /* 置使用标志 */
    pstReqBuff->stReqMsg.ulIsReqMsgExisted  = VOS_TRUE;

    if (CNAS_HRM_RES_APPLY_STATE_APPLY_SUCC == pstReqBuff->stResInfo.enResApplyState)
    {
        usOpId = CNAS_HRM_GetOpId();

        /* 发送请求给SNP */
        CNAS_HRM_SndSnpDataReq(usOpId, ulMsgLen, &(pstDataReq->stDataReqCommContent));

        /* 如果是UATI CMPL，需要立即给HSM回复UATI CMPL消息已发送给SNP */
        if (CNAS_HRM_DATA_REQ_UATI_CMPL == pstDataReq->enDataReqType)
        {
            CNAS_HRM_SndHsmUatiCmplSndInd(pstDataReq->usOpId);
        }

        pstReqBuff->stReqMsg.usHrmOpId = usOpId;
    }

    pstReqBuff->stReqMsg.usReqOpId = pstDataReq->usOpId;

    pstReqBuff->stReqMsg.pucMsg = (VOS_UINT8 *)PS_MEM_ALLOC(UEPS_PID_HRM, pstDataReq->ulLength + VOS_MSG_HEAD_LENGTH);

    NAS_MEM_CPY_S(pstReqBuff->stReqMsg.pucMsg, pstDataReq->ulLength + VOS_MSG_HEAD_LENGTH,
                  pstDataReq, pstDataReq->ulLength + VOS_MSG_HEAD_LENGTH);

}


VOS_VOID CNAS_HRM_ProcHsmDataReqCommProcessDsds1(
    HSM_HRM_DATA_REQ_STRU              *pstDataReq
)
{
    CNAS_HRM_DATA_REQ_BUFF_STRU        *pstDataReqBuff = VOS_NULL_PTR;
    VOS_UINT16                          usOpId;
    VOS_UINT32                          ulMsgLen;
    VOS_UINT8                           ucBuffIndex;

    ulMsgLen       = pstDataReq->ulLength - sizeof(pstDataReq->enDataReqType);
    ucBuffIndex    = CNAS_HRM_GetMsgBuffIndexByDataReqType(pstDataReq->enDataReqType);

    pstDataReqBuff     = CNAS_HRM_GetDataReqBuffAddr(ucBuffIndex);

    /* UATI:
        DSDS1.0在收到ID_HSM_HRM_SESSION_BEGIN_NTF时已经申请资源，
        如果不需要等申请结果，已置使用和资源申请标志，
        如果需要等申请结果，在收到ID_CAS_RFUSER_HRPD_RESOURCE_APPLY_CNF时置使用和资源申请标志
        网络可能在AT为发UATI请求时下发UATI ASSIGN，所以UATI REQ和UATI CMPL流程相同
        UATI CMPL发送后，需要立即给HSM回复UATI CMPL SND IND

        MO KEEP ALVIE:
        DSDS1.0在收到ID_HSM_HRM_SESSION_BEGIN_NTF时已经申请资源，
        如果不需要等申请结果，已置使用和资源申请标志，
        如果需要等申请结果，在收到ID_CAS_RFUSER_HRPD_RESOURCE_APPLY_CNF时置使用和资源申请标志
     */

    /* 判断是否需要等资源申请结果 */
    if (VOS_TRUE == CNAS_HRM_IsNeedWaitResApplied())
    {
        /* 如果资源已申请到 */
        if (CNAS_HRM_RES_APPLY_STATE_APPLY_SUCC == pstDataReqBuff->stResInfo.enResApplyState)
        {
            usOpId = CNAS_HRM_GetOpId();

            /* 发送请求给SNP */
            CNAS_HRM_SndSnpDataReq(usOpId, ulMsgLen, &(pstDataReq->stDataReqCommContent));

            /* 如果是UATI CMPL，需要立即给HSM回复UATI CMPL消息已发送给SNP */
            if (CNAS_HRM_DATA_REQ_UATI_CMPL == pstDataReq->enDataReqType)
            {
                CNAS_HRM_SndHsmUatiCmplSndInd(pstDataReq->usOpId);
            }

            /* 需缓存消息，用于SNP回复处理 */
            pstDataReqBuff->stReqMsg.usHrmOpId = usOpId;
        }

        /* 如果当前资源是被抢占的
             (可能场景:
              UATI REQ被抢占，但是SNP的data cnf是成功的，此时会有UATI CMPL) */
        if (CNAS_HRM_RES_OCCUPY_TYPE_NON != pstDataReqBuff->stResInfo.enResOccupyType)
        {
            if (CNAS_HRM_RES_APPLY_STATE_APPLYING != pstDataReqBuff->stResInfo.enResApplyState)
            {
                CNAS_HRM_SndCasHrpdResApplyReq(CNAS_HRM_GetTaskTypeByDataReqType(pstDataReq->enDataReqType));

                pstDataReqBuff->stResInfo.enResApplyState = CNAS_HRM_RES_APPLY_STATE_APPLYING;
            }

            pstDataReqBuff->stResInfo.enResOccupyType = CNAS_HRM_RES_OCCUPY_TYPE_NON;
        }

        /* 如果资源申请失败 */
        if (CNAS_HRM_RES_APPLY_STATE_APPLY_FAIL == pstDataReqBuff->stResInfo.enResApplyState)
        {
            /* NO RF为HRM的内部原因值，非SNP回复原因值，reason填butt */
            CNAS_HRM_SndHsmDataCnf(pstDataReq->usOpId, HRM_HSM_DATA_CNF_RSLT_NO_RF, CTTF_HRPD_SNP_DATA_FAIL_REASON_ENUM_BUTT);

            return;
        }

        /* 其它状态，只需后续缓存 */
    }
    else
    {
        /* 不需要申请资源，如果未置资源申请标志，异常 */
        if (CNAS_HRM_RES_APPLY_STATE_APPLY_SUCC != pstDataReqBuff->stResInfo.enResApplyState)
        {
            CNAS_ERROR_LOG(UEPS_PID_HRM, "CNAS_HRM_ProcHsmUatiCommProcessDsds1: res isn't applied");
        }

        usOpId = CNAS_HRM_GetOpId();

        /* 发送请求给SNP */
        CNAS_HRM_SndSnpDataReq(usOpId, ulMsgLen, &(pstDataReq->stDataReqCommContent));

        /* 如果是UATI CMPL，需要立即给HSM回复UATI CMPL消息已发送给SNP */
        if (CNAS_HRM_DATA_REQ_UATI_CMPL == pstDataReq->enDataReqType)
        {
            CNAS_HRM_SndHsmUatiCmplSndInd(pstDataReq->usOpId);
        }

        /* 需缓存消息 */
        pstDataReqBuff->stReqMsg.usHrmOpId = usOpId;
    }

    /* 缓存消息 */
    pstDataReqBuff->stReqMsg.ulIsReqMsgExisted = VOS_TRUE;

    pstDataReqBuff->stReqMsg.usReqOpId         = pstDataReq->usOpId;

    if (VOS_NULL_PTR != pstDataReqBuff->stReqMsg.pucMsg)
    {
        CNAS_WARNING_LOG(UEPS_PID_HRM, "CNAS_HRM_ProcHsmDataReqCommProcessDsds1: buff msg is existed");

        PS_MEM_FREE(UEPS_PID_HRM, pstDataReqBuff->stReqMsg.pucMsg);

        pstDataReqBuff->stReqMsg.pucMsg = VOS_NULL_PTR;

        pstDataReqBuff->stReqMsg.usHrmOpId = 0;
    }

    pstDataReqBuff->stReqMsg.pucMsg = (VOS_UINT8 *)PS_MEM_ALLOC(UEPS_PID_HRM, pstDataReq->ulLength + VOS_MSG_HEAD_LENGTH);

    NAS_MEM_CPY_S(pstDataReqBuff->stReqMsg.pucMsg, pstDataReq->ulLength + VOS_MSG_HEAD_LENGTH,
                  pstDataReq, pstDataReq->ulLength + VOS_MSG_HEAD_LENGTH);

}



VOS_VOID CNAS_HRM_RcvHsmUatiBeginNtf(VOS_VOID)
{
    CNAS_HRM_DATA_REQ_BUFF_STRU        *pstUatiReqBuff = VOS_NULL_PTR;
    CNAS_HRM_DATA_REQ_BUFF_STRU        *pstUatiCmplBuff = VOS_NULL_PTR;
    CNAS_HRM_DSDS_TYPE_ENUM_UINT8       enDsdsType;

    enDsdsType      = CNAS_HRM_GetDsdsType();

    pstUatiReqBuff  = CNAS_HRM_GetDataReqBuffAddr(CNAS_HRM_DATA_REQ_BUFF_INDEX_UATI_REQ);

    pstUatiCmplBuff = CNAS_HRM_GetDataReqBuffAddr(CNAS_HRM_DATA_REQ_BUFF_INDEX_UATI_CMPL);

    /* 如果是DSDS1.0，UATI保护整个流程，需要立即发送资源申请请求
        如果是DSDS2.0, 只保护SNP_DATA_REQ和CNF,所以此处不做处理 */
    if (CNAS_HRM_DSDS_TYPE_1 != enDsdsType)
    {
        return;
    }

    /* 收到ID_HSM_HRM_SESSION_BEGIN_NTF时，对应的缓存如果被使用或者资源未释放认为异常 */
    if ((VOS_TRUE == pstUatiReqBuff->stReqMsg.ulIsReqMsgExisted)
     || (VOS_TRUE == pstUatiCmplBuff->stReqMsg.ulIsReqMsgExisted))
    {
        /* 缓存被使用，异常 */
        CNAS_ERROR_LOG(UEPS_PID_HRM, "CNAS_HRM_RcvHsmUatiBeginNtf: buff msg is existed");
    }

    if ((CNAS_HRM_RES_APPLY_STATE_APPLY_SUCC == pstUatiReqBuff->stResInfo.enResApplyState)
     || (CNAS_HRM_RES_APPLY_STATE_APPLY_SUCC == pstUatiCmplBuff->stResInfo.enResApplyState)
     || (CNAS_HRM_RES_APPLY_STATE_APPLYING == pstUatiReqBuff->stResInfo.enResApplyState)
     || (CNAS_HRM_RES_APPLY_STATE_APPLYING == pstUatiCmplBuff->stResInfo.enResApplyState))
    {
        /* 资源未释放，异常打印，不重复申请资源 */
        CNAS_WARNING_LOG(UEPS_PID_HRM, "CNAS_HRM_RcvHsmUatiBeginNtf: res isn't released");

        return;
    }

    CNAS_HRM_SndCasHrpdResApplyReq(RFUSER_CAS_HRPD_TASK_TYPE_UATI);

    /* 判断是否需要等资源申请结果，如果不需要等结果置标志，否则等收到CNF之后处理 */
    if (VOS_FALSE == CNAS_HRM_IsNeedWaitResApplied())
    {
        pstUatiReqBuff->stResInfo.enResApplyState  = CNAS_HRM_RES_APPLY_STATE_APPLY_SUCC;
        pstUatiCmplBuff->stResInfo.enResApplyState = CNAS_HRM_RES_APPLY_STATE_APPLY_SUCC;
    }
    else
    {
        pstUatiReqBuff->stResInfo.enResApplyState  = CNAS_HRM_RES_APPLY_STATE_APPLYING;
        pstUatiCmplBuff->stResInfo.enResApplyState = CNAS_HRM_RES_APPLY_STATE_APPLYING;
    }

    return;
}


VOS_VOID CNAS_HRM_RcvHsmMoKeepAliveBeginNtf(VOS_VOID)
{
    CNAS_HRM_DATA_REQ_BUFF_STRU        *pstMoKeepAliveReqBuff = VOS_NULL_PTR;
    CNAS_HRM_DSDS_TYPE_ENUM_UINT8       enDsdsType;

    enDsdsType      = CNAS_HRM_GetDsdsType();

    pstMoKeepAliveReqBuff  = CNAS_HRM_GetDataReqBuffAddr(CNAS_HRM_DATA_REQ_BUFF_INDEX_MO_KEEP_ALIVE);

    /* 如果是DSDS1.0，MO KEEP ALVIE 保护整个流程，需要立即发送资源申请请求
        如果是DSDS2.0, 只保护SNP_DATA_REQ和CNF,所以此处不做处理 */
    if (CNAS_HRM_DSDS_TYPE_1 != enDsdsType)
    {
        return;
    }

    /* 收到ID_HSM_HRM_SESSION_BEGIN_NTF时，对应的缓存如果被使用或者资源未释放认为异常 */
    if (VOS_TRUE == pstMoKeepAliveReqBuff->stReqMsg.ulIsReqMsgExisted)
    {
        /* 缓存被使用，异常 */
        CNAS_ERROR_LOG(UEPS_PID_HRM, "CNAS_HRM_RcvHsmMoKeepAliveBeginNtf: buff msg is existed");
    }

    if ((CNAS_HRM_RES_APPLY_STATE_APPLY_SUCC == pstMoKeepAliveReqBuff->stResInfo.enResApplyState)
     || (CNAS_HRM_RES_APPLY_STATE_APPLYING == pstMoKeepAliveReqBuff->stResInfo.enResApplyState))
    {
        /* 资源未释放，异常打印，不重复申请资源 */
        CNAS_WARNING_LOG(UEPS_PID_HRM, "CNAS_HRM_RcvHsmMoKeepAliveBeginNtf: res isn't released");

        return;
    }

    CNAS_HRM_SndCasHrpdResApplyReq(RFUSER_CAS_HRPD_TASK_TYPE_MO_KEEP_ALIVE);

    /* 判断是否需要等资源申请结果，如果不需要等结果置标志，否则等收到CNF之后处理 */
    if (VOS_FALSE == CNAS_HRM_IsNeedWaitResApplied())
    {
        pstMoKeepAliveReqBuff->stResInfo.enResApplyState  = CNAS_HRM_RES_APPLY_STATE_APPLY_SUCC;
    }
    else
    {
        pstMoKeepAliveReqBuff->stResInfo.enResApplyState  = CNAS_HRM_RES_APPLY_STATE_APPLYING;
    }

    return;

}


VOS_VOID CNAS_HRM_RcvHsmSessionBeginNtf(
    HSM_HRM_SESSION_BEGIN_NTF_STRU     *pstSessionBeginNtf
)
{
    switch(pstSessionBeginNtf->enSessionType)
    {
        case HSM_HRM_SESSION_TYPE_UATI:
            CNAS_HRM_RcvHsmUatiBeginNtf();
            break;

        case HSM_HRM_SESSION_TYPE_MO_KEEP_ALIVE:
            CNAS_HRM_RcvHsmMoKeepAliveBeginNtf();
            break;

        default:
            CNAS_ERROR_LOG(UEPS_PID_HRM, "CNAS_HRM_RcvHsmSessionBeginNtf: invalid session type ");
            break;
    }
}


VOS_VOID CNAS_HRM_RcvHsmUatiEndNtf(VOS_VOID)
{
    CNAS_HRM_DSDS_TYPE_ENUM_UINT8       enDsdsType;
    CNAS_HRM_DATA_REQ_BUFF_STRU        *pstUatiReqBuff = VOS_NULL_PTR;
    CNAS_HRM_DATA_REQ_BUFF_STRU        *pstUatiCmplBuff = VOS_NULL_PTR;

    enDsdsType = CNAS_HRM_GetDsdsType();

    pstUatiReqBuff  = CNAS_HRM_GetDataReqBuffAddr(CNAS_HRM_DATA_REQ_BUFF_INDEX_UATI_REQ);

    pstUatiCmplBuff = CNAS_HRM_GetDataReqBuffAddr(CNAS_HRM_DATA_REQ_BUFF_INDEX_UATI_CMPL);

    /* 如果是DSDS1.0，UATI保护整个流程，释放资源时清掉缓存 */
    if (CNAS_HRM_DSDS_TYPE_1 != enDsdsType)
    {
        return;
    }

    if ((CNAS_HRM_RES_APPLY_STATE_APPLY_SUCC == pstUatiReqBuff->stResInfo.enResApplyState)
     || (CNAS_HRM_RES_APPLY_STATE_APPLY_SUCC == pstUatiCmplBuff->stResInfo.enResApplyState)
     || (CNAS_HRM_RES_APPLY_STATE_APPLYING == pstUatiReqBuff->stResInfo.enResApplyState)
     || (CNAS_HRM_RES_APPLY_STATE_APPLYING == pstUatiCmplBuff->stResInfo.enResApplyState))
    {
        /* 资源不重复释放 */
        CNAS_HRM_SndCasHrpdResRelNtf(RFUSER_CAS_HRPD_TASK_TYPE_UATI);
    }

    CNAS_HRM_ClearResInfoInSpecifiedDataReqBuff(CNAS_HRM_DATA_REQ_BUFF_INDEX_UATI_REQ);
    CNAS_HRM_ClearResInfoInSpecifiedDataReqBuff(CNAS_HRM_DATA_REQ_BUFF_INDEX_UATI_CMPL);

    return;
}


VOS_VOID CNAS_HRM_RcvHsmMoKeepAliveEndNtf(VOS_VOID)
{
    CNAS_HRM_DSDS_TYPE_ENUM_UINT8       enDsdsType;
    CNAS_HRM_DATA_REQ_BUFF_STRU        *pstMoKeepAliveReqBuff = VOS_NULL_PTR;

    enDsdsType = CNAS_HRM_GetDsdsType();

    pstMoKeepAliveReqBuff  = CNAS_HRM_GetDataReqBuffAddr(CNAS_HRM_DATA_REQ_BUFF_INDEX_MO_KEEP_ALIVE);

    /* 如果是DSDS1.0，UATI保护整个流程，释放资源时清掉缓存 */
    if (CNAS_HRM_DSDS_TYPE_1 != enDsdsType)
    {
        return;
    }

    if ((CNAS_HRM_RES_APPLY_STATE_APPLY_SUCC == pstMoKeepAliveReqBuff->stResInfo.enResApplyState)
     || (CNAS_HRM_RES_APPLY_STATE_APPLYING == pstMoKeepAliveReqBuff->stResInfo.enResApplyState))
    {
        /* 资源不重复释放 */
        CNAS_HRM_SndCasHrpdResRelNtf(RFUSER_CAS_HRPD_TASK_TYPE_MO_KEEP_ALIVE);
    }

    CNAS_HRM_ClearResInfoInSpecifiedDataReqBuff(CNAS_HRM_DATA_REQ_BUFF_INDEX_MO_KEEP_ALIVE);

    return;

}


VOS_VOID CNAS_HRM_RcvHsmSessionEndNtf(
    HSM_HRM_SESSION_END_NTF_STRU       *pstSessionEndNtf
)
{
    switch(pstSessionEndNtf->enSessionType)
    {
        case HSM_HRM_SESSION_TYPE_UATI:
            CNAS_HRM_RcvHsmUatiEndNtf();
            break;

        case HSM_HRM_SESSION_TYPE_MO_KEEP_ALIVE:
            CNAS_HRM_RcvHsmMoKeepAliveEndNtf();
            break;

        default:
            CNAS_ERROR_LOG(UEPS_PID_HRM, "CNAS_HRM_RcvHsmSessionEndNtf: invalid session type ");
            break;
    }
}



VOS_VOID CNAS_HRM_ProcHsmUatiReqDsds1(
    HSM_HRM_DATA_REQ_STRU              *pstDataReq
)
{
    CNAS_HRM_ProcHsmDataReqCommProcessDsds1(pstDataReq);

}


VOS_VOID CNAS_HRM_ProcHsmUatiReqDsds2(
    HSM_HRM_DATA_REQ_STRU              *pstDataReq
)
{
    CNAS_HRM_ProcHsmDataReqCommProcess(pstDataReq);
}



VOS_VOID CNAS_HRM_RcvHsmUatiReq(
    HSM_HRM_DATA_REQ_STRU              *pstDataReq
)
{
    CNAS_HRM_DSDS_TYPE_ENUM_UINT8       enDsdsType;

    enDsdsType = CNAS_HRM_GetDsdsType();

    if (CNAS_HRM_DSDS_TYPE_1 == enDsdsType)
    {
        CNAS_HRM_ProcHsmUatiReqDsds1(pstDataReq);
        return;
    }

    if (CNAS_HRM_DSDS_TYPE_2 == enDsdsType)
    {
        CNAS_HRM_ProcHsmUatiReqDsds2(pstDataReq);
        return;
    }
}


VOS_VOID CNAS_HRM_ProcHsmUatiCmplDsds1(
    HSM_HRM_DATA_REQ_STRU              *pstDataReq
)
{
    CNAS_HRM_ProcHsmDataReqCommProcessDsds1(pstDataReq);
}


VOS_VOID CNAS_HRM_ProcHsmUatiCmplDsds2(
    HSM_HRM_DATA_REQ_STRU              *pstDataReq
)
{
    CNAS_HRM_ProcHsmDataReqCommProcess(pstDataReq);
}



VOS_VOID CNAS_HRM_RcvHsmUatiCmpl(
    HSM_HRM_DATA_REQ_STRU              *pstDataReq
)
{
    CNAS_HRM_DSDS_TYPE_ENUM_UINT8       enDsdsType;

    enDsdsType = CNAS_HRM_GetDsdsType();

    if (CNAS_HRM_DSDS_TYPE_1 == enDsdsType)
    {
        CNAS_HRM_ProcHsmUatiCmplDsds1(pstDataReq);
        return;
    }

    if (CNAS_HRM_DSDS_TYPE_2 == enDsdsType)
    {
        CNAS_HRM_ProcHsmUatiCmplDsds2(pstDataReq);
        return;
    }
}


VOS_VOID CNAS_HRM_RcvHsmHarewareId(
    HSM_HRM_DATA_REQ_STRU              *pstDataReq
)
{
    CNAS_HRM_ProcHsmDataReqCommProcess(pstDataReq);
}


VOS_VOID CNAS_HRM_ProcHsmMoKeepAliveReqDsds1(
    HSM_HRM_DATA_REQ_STRU              *pstDataReq
)
{
    CNAS_HRM_ProcHsmDataReqCommProcessDsds1(pstDataReq);
}


VOS_VOID CNAS_HRM_ProcHsmMoKeepAliveReqDsds2(
    HSM_HRM_DATA_REQ_STRU              *pstDataReq
)
{
    CNAS_HRM_ProcHsmDataReqCommProcess(pstDataReq);
}



VOS_VOID CNAS_HRM_RcvHsmMoKeepAlive(
    HSM_HRM_DATA_REQ_STRU              *pstDataReq
)
{
    CNAS_HRM_DSDS_TYPE_ENUM_UINT8       enDsdsType;

    enDsdsType = CNAS_HRM_GetDsdsType();

    if (CNAS_HRM_DSDS_TYPE_1 == enDsdsType)
    {
        CNAS_HRM_ProcHsmMoKeepAliveReqDsds1(pstDataReq);
        return;
    }

    if (CNAS_HRM_DSDS_TYPE_2 == enDsdsType)
    {
        CNAS_HRM_ProcHsmMoKeepAliveReqDsds2(pstDataReq);
        return;
    }
}


VOS_VOID CNAS_HRM_RcvHsmMtKeepAlive(
    HSM_HRM_DATA_REQ_STRU              *pstDataReq
)
{
    CNAS_HRM_ProcHsmDataReqCommProcess(pstDataReq);
}


VOS_VOID CNAS_HRM_RcvHsmSessionClose(
    HSM_HRM_DATA_REQ_STRU              *pstDataReq
)
{
    CNAS_HRM_ProcHsmDataReqCommProcess(pstDataReq);
}



VOS_VOID CNAS_HRM_RcvHsmDataReq(
    HSM_HRM_DATA_REQ_STRU              *pstDataReq
)
{
    switch (pstDataReq->enDataReqType)
    {
        case CNAS_HRM_DATA_REQ_UATI_REQUEST:
            CNAS_HRM_RcvHsmUatiReq(pstDataReq);
            break;

        case CNAS_HRM_DATA_REQ_UATI_CMPL:
            CNAS_HRM_RcvHsmUatiCmpl(pstDataReq);
            break;

        case CNAS_HRM_DATA_REQ_HARDWARE_ID:
            CNAS_HRM_RcvHsmHarewareId(pstDataReq);
            break;

        case CNAS_HRM_DATA_REQ_MO_KEEP_ALIVE:
            CNAS_HRM_RcvHsmMoKeepAlive(pstDataReq);
            break;

        case CNAS_HRM_DATA_REQ_MT_KEEP_ALIVE:
            CNAS_HRM_RcvHsmMtKeepAlive(pstDataReq);
            break;

        case CNAS_HRM_DATA_REQ_SESSION_CLOSE:
            CNAS_HRM_RcvHsmSessionClose(pstDataReq);
            break;

        default:
            break;
     }

}


VOS_VOID CNAS_HRM_RcvHsmDataCancelNtf(
    HSM_HRM_DATA_CANCEL_NTF_STRU       *pstDataCancelNtf
)
{
    VOS_UINT8                           ucBuffIndex;
    CNAS_HRM_DATA_REQ_BUFF_STRU        *pstReqBuff = VOS_NULL_PTR;
    VOS_UINT8                           ucIsNeedRelRes;

    ucIsNeedRelRes    = VOS_TRUE;
    ucBuffIndex       = CNAS_HRM_GetMsgBuffIndexByDataReqType(pstDataCancelNtf->enDataReqType);
    pstReqBuff        = CNAS_HRM_GetDataReqBuffAddr(ucBuffIndex);


    /* UATI和MO KEEP ALIVE只有在DSDS2.0时，才释放资源。DSDS1.0在收到HSM的ID_HSM_HRM_SESSION_END_NTF时释放资源
        其它类型，释放资源 */
    if (VOS_TRUE == CNAS_HRM_IsDsds1FullFlowResProtect(ucBuffIndex))
    {
        ucIsNeedRelRes = VOS_FALSE;
    }

    /* 无对应的的缓存消息 */
    if (VOS_TRUE != pstReqBuff->stReqMsg.ulIsReqMsgExisted)
    {
        return;
    }

    /* 非抢占资源申请成功，短抢正在申请资源，需取消SNP数据发送 */
    if (((CNAS_HRM_RES_OCCUPY_TYPE_NON == pstReqBuff->stResInfo.enResOccupyType)
       && (CNAS_HRM_RES_APPLY_STATE_APPLY_SUCC == pstReqBuff->stResInfo.enResApplyState))
     || ((CNAS_HRM_RES_OCCUPY_TYPE_SHORT == pstReqBuff->stResInfo.enResOccupyType)
       && (CNAS_HRM_RES_APPLY_STATE_APPLYING == pstReqBuff->stResInfo.enResApplyState)))
    {
        CNAS_HRM_SndSnpDataCancelReq(pstReqBuff->stReqMsg.usHrmOpId);
    }

    if (VOS_TRUE == ucIsNeedRelRes)
    {
        /* 释放资源 */
        if ((CNAS_HRM_RES_APPLY_STATE_APPLY_SUCC == pstReqBuff->stResInfo.enResApplyState)
         || (CNAS_HRM_RES_APPLY_STATE_APPLYING == pstReqBuff->stResInfo.enResApplyState))
        {
            CNAS_HRM_SndCasHrpdResRelNtf(CNAS_HRM_GetTaskTypeByDataReqType(pstDataCancelNtf->enDataReqType));
        }

        /* 清缓存 */
        CNAS_HRM_ClearMsgInSpecifiedDataReqBuff(ucBuffIndex);
        CNAS_HRM_ClearResInfoInSpecifiedDataReqBuff(ucBuffIndex);
    }
    else
    {
        CNAS_HRM_ClearMsgInSpecifiedDataReqBuff(ucBuffIndex);
    }
}


VOS_VOID CNAS_HRM_RcvHsmMsg(
    struct MsgCB                       *pstMsg
)
{

    VOS_UINT16                          usMsgName;

    usMsgName = *((VOS_UINT16 *)(pstMsg->aucValue));

    switch (usMsgName)
    {
        case ID_HSM_HRM_SESSION_BEGIN_NTF:
            CNAS_HRM_RcvHsmSessionBeginNtf((HSM_HRM_SESSION_BEGIN_NTF_STRU*)pstMsg);
            break;

        case ID_HSM_HRM_SESSION_END_NTF:
            CNAS_HRM_RcvHsmSessionEndNtf((HSM_HRM_SESSION_END_NTF_STRU*)pstMsg);
            break;

        case ID_HSM_HRM_DATA_REQ:
            CNAS_HRM_RcvHsmDataReq((HSM_HRM_DATA_REQ_STRU*)pstMsg);
            break;

        case ID_HSM_HRM_DATA_CANCEL_NTF:
            CNAS_HRM_RcvHsmDataCancelNtf((HSM_HRM_DATA_CANCEL_NTF_STRU*)pstMsg);
            break;

        default:
            break;
     }

     return;
}


/*lint -restore*/

#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif



