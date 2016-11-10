

/*****************************************************************************
  1 头文件包含
*****************************************************************************/


/*****************************************************************************
  1 Header File Include
*****************************************************************************/
#include "CnasHrmMain.h"
#include "CnasHrmCtx.h"
#include "CnasHrmCommFunc.h"
#include "CnasHrmProcCas.h"
#include "CnasHrmProcTtf.h"
#include "CnasHrmProcHsm.h"
#include "CnasHrmProcHlu.h"
#include "CnasHrmProcHsd.h"
#include "CnasMntn.h"


#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_CNAS_HRM_MAIN_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/*****************************************************************************
  3 函数定义
*****************************************************************************/
/*lint -save -e958*/
#ifdef __PS_WIN32_RECUR__

VOS_UINT32 CNAS_HRM_RestorePcReplayFixedContext(
    struct MsgCB                       *pstMsg
)
{
    CNAS_MNTN_LOG_PC_REPLAY_CONTEXT_STRU                   *pstRcvMsg      = VOS_NULL_PTR;
    CNAS_HRM_PC_REPLAY_FIXED_CONTEXT_MSG_STRU                        *pstFixedCtxMsg = VOS_NULL_PTR;
    CNAS_HRM_CTX_STRU                                      *pstCnasHrmCtx = VOS_NULL_PTR;

    pstCnasHrmCtx   = CNAS_HRM_GetCtxAddr();
    pstRcvMsg       = (CNAS_MNTN_LOG_PC_REPLAY_CONTEXT_STRU*)pstMsg;
    pstFixedCtxMsg  = (CNAS_HRM_PC_REPLAY_FIXED_CONTEXT_MSG_STRU*)pstRcvMsg->aucDataCtx;

    pstCnasHrmCtx->ucIsNeedWaitResApplied = pstFixedCtxMsg->ucIsNeedWaitResApplied;
    pstCnasHrmCtx->enDsdsType             = pstFixedCtxMsg->enDsdsType;

    return VOS_TRUE;
}
#endif


VOS_VOID  CNAS_HRM_RcvHrmMsg(
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulMsgName;

    ulMsgName = *((VOS_UINT32 *)(pstMsg->aucValue));

    switch (ulMsgName)
    {
        case ID_CNAS_MNTN_LOG_PC_REPLAY_FIXED_CONTEXT:
#ifdef __PS_WIN32_RECUR__
            CNAS_HRM_RestorePcReplayFixedContext(pstMsg);
#endif
            break;

        default:
            break;
     }

     return;
}


VOS_VOID CNAS_HRM_MsgProc (struct MsgCB* pstMsg)
{

    if (VOS_NULL_PTR == pstMsg)
    {
        return;
    }

    CNAS_HRM_LogHrmCtx();

    switch (pstMsg->ulSenderPid)
    {
        case UEPS_PID_HALMP:
            CNAS_HRM_RcvCasMsg(pstMsg);
            break;

        case UEPS_PID_HSM:
            CNAS_HRM_RcvHsmMsg(pstMsg);
            break;

        case UEPS_PID_HLU:
            CNAS_HRM_RcvHluMsg(pstMsg);
            break;

        case UEPS_PID_HSD:
            CNAS_HRM_RcvHsdMsg(pstMsg);
            break;

        case CTTF_PID_HRPD_SIG:
            CNAS_HRM_RcvFsigMsg(pstMsg);
            break;
        case UEPS_PID_HRM:
            CNAS_HRM_RcvHrmMsg(pstMsg);
            break;
        default:
            break;
    }

    CNAS_HRM_LogHrmCtx();

    return;
}


VOS_UINT32  CNAS_HRM_InitTask(VOS_VOID)
{
    /* 初始化HRM上下文 */
    CNAS_HRM_InitCtx();

    return VOS_OK;
}


VOS_UINT32 CNAS_HRM_InitPid(
    enum VOS_INIT_PHASE_DEFINE         enInitPhase
)
{
    switch (enInitPhase)
    {
        case VOS_IP_LOAD_CONFIG:
            /* 调用HRM模块初始化函数 */
            CNAS_HRM_InitTask();
            break;

        case VOS_IP_INITIAL:
        case VOS_IP_FARMALLOC:
        case VOS_IP_ENROLLMENT:
        case VOS_IP_LOAD_DATA:
        case VOS_IP_FETCH_DATA:
        case VOS_IP_STARTUP:
        case VOS_IP_RIVAL:
        case VOS_IP_KICKOFF:
        case VOS_IP_STANDBY:
        case VOS_IP_BROADCAST_STATE:
        case VOS_IP_RESTART:
        case VOS_IP_BUTT:
            break;

        default:
            break;
    }

    return VOS_OK;
}

/*lint -restore*/


#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */




