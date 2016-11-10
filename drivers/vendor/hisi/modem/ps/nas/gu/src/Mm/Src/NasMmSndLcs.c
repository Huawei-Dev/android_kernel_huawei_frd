

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "MM_Ext.h"
#include "NasMmSndLcs.h"
#include "NasMmCtx.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NAS_MM_SND_LCS_C
/*lint +e767*/

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/

VOS_VOID NAS_MM_SndLcsEstCnf(
    VOS_UINT32                          ulTransactionId
)
{
    MM_LCS_EST_CNF_STRU                *pstLcsEstCnf;

    if (NAS_MM_GetConnCtrlInfo(MM_CONN_CTRL_LCS)->RcvXXEstReq.ulTransactionId == ulTransactionId)
    {
        /* 因为即使在IDLE NORMAL状态下，在收到EST_REQ时，也始终把缓存标志设置了，
           所以在处理结束后，需要清除消息缓存标志  */
        NAS_MM_GetConnCtrlInfo(MM_CONN_CTRL_LCS)->RcvXXEstReq.ucFlg = MM_FALSE;

        /* 更新CS域业务链接状态和缓存状态 */
        NAS_MM_UpdateCsServiceConnStatusFlg();
        NAS_MM_UpdateCsServiceBufferStatusFlg();
    }

    /* 构造消息 */
    pstLcsEstCnf = (MM_LCS_EST_CNF_STRU *)NAS_MM_ALLOC_MSG_WITH_HDR(sizeof(MM_LCS_EST_CNF_STRU));
    if( VOS_NULL_PTR == pstLcsEstCnf )
    {
        NAS_ERROR_LOG(WUEPS_PID_MM, "NAS_MM_SndLcsEstCnf: Alloc Message Failed!");
        return;
    }

    /* 消息初始化 */
    NAS_MM_CLR_MSG_ENTITY(pstLcsEstCnf);

    /* 填写消息头 */
    NAS_MM_CFG_MSG_HDR(pstLcsEstCnf, WUEPS_PID_LCS, ID_MM_LCS_EST_CNF);

    /* 填写消息内容 */
    pstLcsEstCnf->ulTi = ulTransactionId;

    /* 发送消息 */
    if (VOS_OK != NAS_MM_SEND_MSG(pstLcsEstCnf))
    {
        NAS_ERROR_LOG(WUEPS_PID_MM, "NAS_MM_SndLcsEstCnf: Send Message Failed!");
    }

    return;
}


VOS_VOID NAS_MM_SndLcsDataInd(
    VOS_UINT32                          ulTransactionId,
    VOS_VOID                           *pNasMsg,
    VOS_UINT32                          ulNasMsgLen
)
{
    MM_LCS_DATA_IND_STRU               *pstLcsDataInd = VOS_NULL_PTR;
    VOS_UINT32                          ulLength;
    VOS_UINT32                          ulMsgPointer;

    NAS_SS_ReportN2MOtaMsg((SS_MSG_STRU *)((VOS_UINT8 *)pNasMsg - 4));

    /* 计算消息长度 */
    if (4 < ulNasMsgLen)
    {
        ulLength = sizeof(MM_LCS_DATA_IND_STRU) - 4 + ulNasMsgLen;
    }
    else
    {
        ulLength = sizeof(MM_LCS_DATA_IND_STRU);
    }

    /* 构造消息 */
    pstLcsDataInd = (MM_LCS_DATA_IND_STRU *)NAS_MM_ALLOC_MSG_WITH_HDR(ulLength);
    if( VOS_NULL_PTR == pstLcsDataInd )
    {
        NAS_ERROR_LOG(WUEPS_PID_MM, "NAS_MM_SndLcsDataInd: Alloc Message Failed!");
        return;
    }

    /* 消息初始化 */
    NAS_MM_CLR_MSG_ENTITY(pstLcsDataInd);

    /* 填写消息头 */
    NAS_MM_CFG_MSG_HDR(pstLcsDataInd, WUEPS_PID_LCS, ID_MM_LCS_DATA_IND);

    /* 填写消息内容 */
    pstLcsDataInd->ulTi                  = ulTransactionId;
    pstLcsDataInd->stLcsMsg.ulLcsMsgSize = ulNasMsgLen;
    ulMsgPointer = (VOS_UINT32)pstLcsDataInd->stLcsMsg.aucLcsMsg;
    PS_MEM_CPY((VOS_UINT8*)ulMsgPointer, pNasMsg, ulNasMsgLen);

    /* 发送消息 */
    if (VOS_OK != NAS_MM_SEND_MSG(pstLcsDataInd))
    {
        NAS_ERROR_LOG(WUEPS_PID_MM, "NAS_MM_SndLcsDataInd: Send Message Failed!");
    }

    return;
}


VOS_VOID NAS_MM_SndLcsRelInd(
    VOS_UINT32                          ulTransactionId,
    NAS_MMCM_REL_CAUSE_ENUM_UINT32      enRelCause
)
{
    MM_LCS_REL_IND_STRU                 *pstLcsRelInd;

    /* 释放缓存Ti的处理 */
    if (NAS_MM_GetConnCtrlInfo(MM_CONN_CTRL_LCS)->RcvXXEstReq.ulTransactionId == ulTransactionId)
    {
        NAS_MM_GetConnCtrlInfo(MM_CONN_CTRL_LCS)->RcvXXEstReq.ucFlg = MM_FALSE;

        /* 更新CS域业务链接状态和缓存状态 */
        NAS_MM_UpdateCsServiceConnStatusFlg();
        NAS_MM_UpdateCsServiceBufferStatusFlg();

#if (FEATURE_ON == FEATURE_LTE)
        if (NAS_MML_CSFB_SERVICE_STATUS_MO_NORMAL_LCS_EXIST ==  NAS_MML_GetCsfbServiceStatus())
        {
            /* 需要终止缓存Ti触发的CSFB */
            NAS_MM_ProcCsfbServiceStatusChange(NAS_MML_GetCsfbServiceStatus(), NAS_MML_CSFB_SERVICE_STATUS_NOT_EXIST);
        }
#endif
    }

    /* 清空建链中Ti的标志 */
    if (NAS_MM_GetConnCtrlInfo(MM_CONN_CTRL_LCS)->ucMMConnEstingTI == ulTransactionId)
    {
        NAS_MM_GetConnCtrlInfo(MM_CONN_CTRL_LCS)->ucMMConnEstingTI = NO_MM_CONN_ESTING;
    }

    /* 构造消息 */
    pstLcsRelInd = (MM_LCS_REL_IND_STRU *)NAS_MM_ALLOC_MSG_WITH_HDR(sizeof(MM_LCS_REL_IND_STRU));
    if( VOS_NULL_PTR == pstLcsRelInd )
    {
        NAS_ERROR_LOG(WUEPS_PID_MM, "NAS_MM_SndLcsRelInd: Alloc Message Failed!");
        return;
    }

    /* 消息初始化 */
    NAS_MM_CLR_MSG_ENTITY(pstLcsRelInd);

    /* 填写消息头 */
    NAS_MM_CFG_MSG_HDR(pstLcsRelInd, WUEPS_PID_LCS, ID_MM_LCS_REL_IND);

    /* 填写消息内容 */
    pstLcsRelInd->ulTi          = ulTransactionId;
    pstLcsRelInd->ulRelCause    = enRelCause;

    /* 发送消息 */
    if (VOS_OK != NAS_MM_SEND_MSG(pstLcsRelInd))
    {
        NAS_ERROR_LOG(WUEPS_PID_MM, "NAS_MM_SndLcsRelInd: Send Message Failed!");
    }

    return;
}











#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

