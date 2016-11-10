

/*****************************************************************************
  1 Header File Include
*****************************************************************************/
#include "CnasHsmFsmSessionDeact.h"
#include "CnasHsmComFunc.h"
#include "CnasHsmCtx.h"
#include "CnasHsmFsmTbl.h"
#include "CnasHsmSndInternalMsg.h"
#include "CnasHsmSndAs.h"
#include "nas_cttf_hrpd_rsnp_pif.h"
#include "CnasHsmDecode.h"
#include "cttf_hrpd_sig_public_pif.h"
#include "CnasHsmEncode.h"
#include "CnasHsmSndTtf.h"
#include "CnasMntn.h"
#include "CnasHsmMntn.h"
#include "CnasTimerMgmt.h"
#include "CnasHsmProcNvim.h"
#include "Nas_Mem.h"
#include "CnasHsmSndHrm.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_CNAS_HSM_FSM_SESSION_DEACT_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 The Define Of The Gloabal Variable
*****************************************************************************/

/*****************************************************************************
  3 Function Define
*****************************************************************************/

VOS_VOID CNAS_HSM_QuitFsmScpDeactiveReq(VOS_VOID)
{
    CNAS_HSM_SESSION_DEACT_REASON_ENUM_UINT8                enSessionDeactReason;

    enSessionDeactReason = CNAS_HSM_GetSessionDeactReason_SessionDeact();

    /* No Rf失败原因值需要在session deactive后上报hsd,而且这两个原因之失败不会发起重试,
       所以这里不能替换成abort */
    if ((VOS_TRUE == CNAS_HSM_GetAbortFlag_SessionDeact())
     && (CNAS_HSM_SESSION_DEACT_REASON_AMP_FAIL_UATI_SNP_DATA_CNF_NO_RF != enSessionDeactReason)
     && (CNAS_HSM_SESSION_DEACT_REASON_SCP_NO_RF                        != enSessionDeactReason)
     && (CNAS_HSM_SESSION_DEACT_REASON_CAS_NEG_RSLT_IND_SCP_NO_RF       != enSessionDeactReason))
    {
        enSessionDeactReason = CNAS_HSM_SESSION_DEACT_REASON_SESSION_DEACT_ABORT;
    }

    /* clear coverity warning */
    if (CNAS_HSM_SESSION_DEACT_REASON_BUTT == enSessionDeactReason)
    {
        CNAS_WARNING_LOG(UEPS_PID_HSM, "CNAS_HSM_QuitFsmScpDeactiveReq(): session deact reason is BUTT!");
    }

    CNAS_HSM_InitSessionDeactiveFsmCtx(CNAS_HSM_GetSessionDeactiveFsmCtxAddr());

    CNAS_HSM_SetCurrSubState(CNAS_HSM_SS_VACANT);

    CNAS_HSM_SetScpActiveFlag(VOS_FALSE);

    CNAS_HSM_SndInternalSessionDeactiveResultInd(enSessionDeactReason);

    return;
}


VOS_UINT8 CNAS_HSM_IsNeedSendSessionClose_SessionDeact(
    CNAS_HSM_SESSION_CLOSE_PROTOCOL_REASON_ENUM_UINT8       enCloseReasonSendAn
)
{
    VOS_UINT8                                               ucIsScpActive;
    CNAS_HSM_HRPD_CAS_STATUS_ENUM_UINT16                    enHrpdConvertedCasStatus;

    ucIsScpActive            = CNAS_HSM_GetScpActiveFlag();
    enHrpdConvertedCasStatus = CNAS_HSM_GetHrpdConvertedCasStatus();

    /* 有网状态下才能下发sessionclose消息 */

    if ((CNAS_HSM_HRPD_CAS_STATUS_ENUM_IDLE != enHrpdConvertedCasStatus)
     && (CNAS_HSM_HRPD_CAS_STATUS_ENUM_CONN != enHrpdConvertedCasStatus))
    {
        return VOS_FALSE;
    }

    /*UATI过程被打断的原因值可以映射出发给AN的SessionClose原因值,
           但是Setup态不需要给网络发SessionClose消息,增加屏蔽 */

    if (((VOS_TRUE == ucIsScpActive)
     && (CNAS_HSM_SESSION_CLOSE_PROTOCOL_REASON_BUTT != enCloseReasonSendAn))
    || (CNAS_HSM_SESSION_CLOSE_PROTOCOL_REASON_REPLY == enCloseReasonSendAn))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 CNAS_HSM_RcvInterSessionDeactReq_SessionDeact_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_HSM_SESSION_CLOSE_MSG_STRU                         stSessionClose;
    CNAS_HSM_HSM_SESSION_DEACTIVE_REQ_STRU                 *pstSessionDeactReq;
    CNAS_HSM_SESSION_CLOSE_PROTOCOL_REASON_ENUM_UINT8       enCloseReasonSendAn;
    VOS_UINT8                                               ucNeedSendSessionClose;
    CNAS_HSM_SESSION_DEACT_REVISE_TIMER_SCENE_ENUM_UINT8    enReviseTimerScene;



    pstSessionDeactReq = (CNAS_HSM_HSM_SESSION_DEACTIVE_REQ_STRU *)pstMsg;

    /* Save session deactive reason to session deactive fsm ctx */
    CNAS_HSM_SetSessionDeactReason_SessionDeact(pstSessionDeactReq->enSessionDeactReason);

    enReviseTimerScene = CNAS_HSM_GetStartReviseTimerSceneBySessionDeactReason_SessionDeact(pstSessionDeactReq->enSessionDeactReason);
    CNAS_HSM_SetReviseTimerScene_SessionDeact(enReviseTimerScene);

    CNAS_HSM_GetCloseReasonSendAnByDeactReason(pstSessionDeactReq->enSessionDeactReason,
                                               &enCloseReasonSendAn);
    ucNeedSendSessionClose = VOS_FALSE;



    /* 如果是在连接状态机中收到了网络发的SessionClose，在预处理中已经发了SessionClose，
     * deactive reason为CNAS_HSM_SESSION_DEACT_REASON_INTERNAL_SESSION_CLOSE_IN_CONN_MNMT。
     * 如果deactive reason为CNAS_HSM_SESSION_DEACT_REASON_INTERNAL_SESSION_CLOSE_IN_CONN_MNMT，
     * 并且TI_CNAS_HSM_SESSION_CLOSE_WAIT_SNP_DATA_CNF在运行，说明预处理中发送的SessionClose未收到SNP_DATA_CNF，
     * 将子状态设置为TI_CNAS_HSM_SESSION_CLOSE_WAIT_SNP_DATA_CNF */
    if (CNAS_HSM_SESSION_DEACT_REASON_INTERNAL_SESSION_CLOSE_IN_CONN_MNMT
                                                == pstSessionDeactReq->enSessionDeactReason)
    {
        if (CNAS_TIMER_STATUS_RUNNING == CNAS_GetSpecifiedTimerStatus(UEPS_PID_HSM,
                                                TI_CNAS_HSM_SESSION_CLOSE_WAIT_SNP_DATA_CNF, 0))
        {
            CNAS_HSM_SetCurrSubState(CNAS_HSM_SS_SESSION_DEACT_WAIT_SESSION_CLOSE_SNP_DATA_CNF);

            return CNAS_HSM_FSM_EVENT_HANDLED;
        }
    }
    else
    {
        ucNeedSendSessionClose = CNAS_HSM_IsNeedSendSessionClose_SessionDeact(enCloseReasonSendAn);
    }

    if (VOS_FALSE == ucNeedSendSessionClose)
    {
        /* 与CAS讨论结果，只要激活过SCP则在去激活过程中无条件断链，勿须判断链接状态,
           但是uati或者session激活等流程被suspend打断时，不发送断链指示到cas */
        if (VOS_TRUE == CNAS_HSM_GetScpActiveFlag())
        {
            /* suspend打断时，尽量控制不要往cas下发断链指示，cas发起suspend时，下层是没有链路的，
               直接发送scp deactive req给cas */
            if ((CNAS_HSM_SESSION_DEACT_REASON_UATI_ABORT_BY_SUSPEND        == pstSessionDeactReq->enSessionDeactReason)
             || (CNAS_HSM_SESSION_DEACT_REASON_SESSION_ACT_ABORT_BY_SUSPEND == pstSessionDeactReq->enSessionDeactReason))
            {
                CNAS_HSM_SndCasHrpdScpDeActiveReq();

                CNAS_HSM_StartTimer(TI_CNAS_HSM_WAIT_SCP_DEACTIVE_CNF,
                                    CNAS_HSM_GetScpDeactiveWaitScpDeactiveCnfTimerLen_SessionDeact());

                CNAS_HSM_SetCurrSubState(CNAS_HSM_SS_SESSION_DEACT_WAIT_SCP_DEACTIVE_CNF);
            }
            else
            {
                CNAS_HSM_SndCasHrpdConnCloseReq(VOS_TRUE);

                CNAS_HSM_StartTimer(TI_CNAS_HSM_WAIT_HRPD_CONN_CLOSE_IND,
                                    CNAS_HSM_GetConnCloseWaitConnCloseIndTimerLen_SessionDeact());

                CNAS_HSM_SetCurrSubState(CNAS_HSM_SS_SESSION_DEACT_WAIT_CONN_CLOSE_IND);
            }

            return CNAS_HSM_FSM_EVENT_HANDLED;
        }

        CNAS_HSM_QuitFsmScpDeactiveReq();

    }
    else
    {

        NAS_MEM_SET_S(&stSessionClose, sizeof(stSessionClose), 0x00, sizeof(CNAS_HSM_SESSION_CLOSE_MSG_STRU));

        stSessionClose.enCloseReason = enCloseReasonSendAn;
        stSessionClose.usType        = CNAS_HSM_GetScpActFailProcType();
        stSessionClose.usSubType     = CNAS_HSM_GetScpActFailProcSubtype();

        CNAS_HSM_SndSessionCloseMsg(&stSessionClose);

        CNAS_HSM_StartTimer(TI_CNAS_HSM_SESSION_CLOSE_WAIT_SNP_DATA_CNF,
                            CNAS_HSM_GetSessionCloseWaitSnpDataCnfTimerLen_SessionDeact());

        CNAS_HSM_SetCurrSubState(CNAS_HSM_SS_SESSION_DEACT_WAIT_SESSION_CLOSE_SNP_DATA_CNF);
    }

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvHsdPowerOffReq_SessionDeact_WaitSessionCloseSnpDataCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulRemainLen;

    /* 缓存power off, 记录被打断但是流程继续，流程结束时返回结果为abort，
       等流程结束后，处理power off。如果已经在等待SNP DATA CNF,且此前的定
       时器剩余时长大于2s，修改定时器时长为2S，重新启动，否则不重新启动 */

    ulRemainLen = CNAS_GetSpecifiedTimerRemainLen(UEPS_PID_HSM, TI_CNAS_HSM_SESSION_CLOSE_WAIT_SNP_DATA_CNF, 0);
    if (TI_CNAS_HSM_REVISE_WAIT_SNP_DATA_CNF_LEN < ulRemainLen)
    {
        CNAS_HSM_StopTimer(TI_CNAS_HSM_SESSION_CLOSE_WAIT_SNP_DATA_CNF);

        CNAS_HSM_StartTimer(TI_CNAS_HSM_SESSION_CLOSE_WAIT_SNP_DATA_CNF ,
                            TI_CNAS_HSM_REVISE_WAIT_SNP_DATA_CNF_LEN);
    }

    CNAS_HSM_SetAbortFlag_SessionDeact(VOS_TRUE);

    CNAS_HSM_SaveCacheMsg(ulEventType, pstMsg);

    CNAS_HSM_SetReviseTimerScene_SessionDeact(CNAS_HSM_SESSION_DEACT_REVISE_TIMER_SCENE_POWER_OFF);

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvTiWaitSnpDataCnfTimerExpire_SessionDeact_WaitSessionCloseSnpDataCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_HSM_SESSION_CLOSE_MSG_STRU                        *pstStoreSessionClose = VOS_NULL_PTR;
    CNAS_HSM_SESSION_DEACT_REASON_ENUM_UINT8                enSessionDeactReason;

    pstStoreSessionClose = CNAS_HSM_GetStoreSessionCloseAddr();

    CNAS_WARNING_LOG(UEPS_PID_HSM, "CNAS_HSM_RcvTiWaitSnpDataCnfTimerExpire_SessionDeact_WaitSessionCloseSnpDataCnf: timer out");

    /* session close流程被abort也是要等到data cnf或者保护定时器超时,
       所以只需要在保护定时器超时的时候取消发送cancel, 打断消息处不需要发送 */
    CNAS_HSM_SndHrmDataCancelNtf(CNAS_HRM_DATA_REQ_SESSION_CLOSE);

    CNAS_HSM_ResetSessionCloseRetryTimes();

    NAS_MEM_SET_S(pstStoreSessionClose, sizeof(CNAS_HSM_SESSION_CLOSE_MSG_STRU), 0x0, sizeof(CNAS_HSM_SESSION_CLOSE_MSG_STRU));

    /* 与CAS讨论结果，只要激活过SCP则在去激活过程中无条件断链，勿须判断链接状态，
       但是uati或者session激活等流程被suspend打断时，不发送断链指示到cas  */
    if (VOS_TRUE == CNAS_HSM_GetScpActiveFlag())
    {
        /* suspend打断时，尽量控制不要往cas下发断链指示，cas发起suspend时，下层是没有链路的，
           直接发送scp deactive req给cas */
        enSessionDeactReason = CNAS_HSM_GetSessionDeactReason_SessionDeact();

        if ((VOS_TRUE == CNAS_HSM_GetSuspendFlag_SessionDeact())
         || ((CNAS_HSM_SESSION_DEACT_REASON_UATI_ABORT_BY_SUSPEND        == enSessionDeactReason)
          || (CNAS_HSM_SESSION_DEACT_REASON_SESSION_ACT_ABORT_BY_SUSPEND == enSessionDeactReason)))
        {
            CNAS_HSM_SndCasHrpdScpDeActiveReq();

            CNAS_HSM_StartTimer(TI_CNAS_HSM_WAIT_SCP_DEACTIVE_CNF,
                                CNAS_HSM_GetScpDeactiveWaitScpDeactiveCnfTimerLen_SessionDeact());

            CNAS_HSM_SetCurrSubState(CNAS_HSM_SS_SESSION_DEACT_WAIT_SCP_DEACTIVE_CNF);
        }
        else
        {
            CNAS_HSM_SndCasHrpdConnCloseReq(VOS_TRUE);

            CNAS_HSM_StartTimer(TI_CNAS_HSM_WAIT_HRPD_CONN_CLOSE_IND,
                                CNAS_HSM_GetConnCloseWaitConnCloseIndTimerLen_SessionDeact());

            CNAS_HSM_SetCurrSubState(CNAS_HSM_SS_SESSION_DEACT_WAIT_CONN_CLOSE_IND);
        }

    }
    else
    {
        CNAS_HSM_QuitFsmScpDeactiveReq();
    }

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvHrmDataCnf_SessionDeact_WaitSessionCloseSnpDataCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT16                                              usSessionCloseOpId;
    HRM_HSM_DATA_CNF_STRU                                  *pstSnpDataCnf;
    CNAS_HSM_MNTN_LOG_DISCARD_MSG_REASON_UNION              unDiscardMsgReason;
    CNAS_HSM_SESSION_DEACT_REASON_ENUM_UINT8                enSessionDeactReason;

    pstSnpDataCnf       = (HRM_HSM_DATA_CNF_STRU *)pstMsg;
    usSessionCloseOpId  = CNAS_HSM_GetSessionCloseSnpDataReqOpId();

    unDiscardMsgReason.stDiscardSnpDataCnfReason.enMainState    = CNAS_HSM_GetCurrMainState();
    unDiscardMsgReason.stDiscardSnpDataCnfReason.enSubState     = CNAS_HSM_GetCurrSubState();
    unDiscardMsgReason.stDiscardSnpDataCnfReason.usRcvOpId      = pstSnpDataCnf->usOpId;
    unDiscardMsgReason.stDiscardSnpDataCnfReason.usSndOpId      = usSessionCloseOpId;

    if (usSessionCloseOpId != pstSnpDataCnf->usOpId)
    {
        CNAS_HSM_LogDiscardMsgInfo(CNAS_HSM_MNTN_LOG_DISCARD_MSG_SCENE_ERROR_SNP_DATA_CNF, &unDiscardMsgReason);

        CNAS_WARNING_LOG(UEPS_PID_HSM, "CNAS_HSM_RcvCttfSnpDataCnf_SessionDeact_WaitSessionCloseSnpDataCnf: OpId not as expected");

        return CNAS_HSM_FSM_EVENT_NOT_HANDLED;
    }

    /* 处理session close的snp data cnf，对于不再重发session close的场景，
        返回VOS_FALSE,接着做后续处理,反之重发session close,返回VOS_TRUE  */
    if (VOS_TRUE == CNAS_HSM_IsNeedRetransmitSessionClose_SessionDeact(pstSnpDataCnf->enRslt, pstSnpDataCnf->enFailReason))
    {
        return CNAS_HSM_FSM_EVENT_HANDLED;
    }

    /* 与CAS讨论结果，只要激活过SCP则在去激活过程中无条件断链，勿须判断链接状态，
       但是uati或者session激活等流程被suspend打断时，不发送断链指示到cas */
    if (VOS_TRUE == CNAS_HSM_GetScpActiveFlag())
    {
        /* suspend打断时，尽量控制不要往cas下发断链指示，cas发起suspend时，下层是没有链路的，
           直接发送scp deactive req给cas */
        enSessionDeactReason = CNAS_HSM_GetSessionDeactReason_SessionDeact();

        if ((VOS_TRUE == CNAS_HSM_GetSuspendFlag_SessionDeact())
         || ((CNAS_HSM_SESSION_DEACT_REASON_UATI_ABORT_BY_SUSPEND        == enSessionDeactReason)
          || (CNAS_HSM_SESSION_DEACT_REASON_SESSION_ACT_ABORT_BY_SUSPEND == enSessionDeactReason)))
        {
            CNAS_HSM_SndCasHrpdScpDeActiveReq();

            CNAS_HSM_StartTimer(TI_CNAS_HSM_WAIT_SCP_DEACTIVE_CNF,
                                CNAS_HSM_GetScpDeactiveWaitScpDeactiveCnfTimerLen_SessionDeact());

            CNAS_HSM_SetCurrSubState(CNAS_HSM_SS_SESSION_DEACT_WAIT_SCP_DEACTIVE_CNF);
        }
        else
        {
            CNAS_HSM_SndCasHrpdConnCloseReq(VOS_TRUE);

            CNAS_HSM_StartTimer(TI_CNAS_HSM_WAIT_HRPD_CONN_CLOSE_IND,
                                CNAS_HSM_GetConnCloseWaitConnCloseIndTimerLen_SessionDeact());

            CNAS_HSM_SetCurrSubState(CNAS_HSM_SS_SESSION_DEACT_WAIT_CONN_CLOSE_IND);
        }

    }
    else
    {
        CNAS_HSM_QuitFsmScpDeactiveReq();
    }

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvHsdPowerOffReq_SessionDeact_WaitConnCloseInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulRemainLen;

    /* 缓存power off, 记录被打断但是流程继续，流程结束时返回结果为abort，
       等流程结束后，处理power off。如果已经在等待conn close ind,且此前的定
       时器剩余时长大于1.5s，修改定时器时长为1.5S，重新启动，否则不重新启动 */

    ulRemainLen = CNAS_GetSpecifiedTimerRemainLen(UEPS_PID_HSM, TI_CNAS_HSM_WAIT_HRPD_CONN_CLOSE_IND, 0);
    if (TI_CNAS_HSM_REVISE_WAIT_HRPD_CONN_CLOSE_IND_LEN < ulRemainLen)
    {
        CNAS_HSM_StopTimer(TI_CNAS_HSM_WAIT_HRPD_CONN_CLOSE_IND);

        CNAS_HSM_StartTimer(TI_CNAS_HSM_WAIT_HRPD_CONN_CLOSE_IND ,
                            TI_CNAS_HSM_REVISE_WAIT_HRPD_CONN_CLOSE_IND_LEN);
    }

    CNAS_HSM_SetAbortFlag_SessionDeact(VOS_TRUE);

    CNAS_HSM_SaveCacheMsg(ulEventType, pstMsg);

    CNAS_HSM_SetReviseTimerScene_SessionDeact(CNAS_HSM_SESSION_DEACT_REVISE_TIMER_SCENE_POWER_OFF);

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvCasConnCloseInd_SessionDeact_WaitConnCloseInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_HSM_StopTimer(TI_CNAS_HSM_WAIT_HRPD_CONN_CLOSE_IND);

    /* 因为只有SCP激活的情况下才能发起断链，所以此处SCP肯定是激活状态 */
    CNAS_HSM_SndCasHrpdScpDeActiveReq();

    CNAS_HSM_StartTimer(TI_CNAS_HSM_WAIT_SCP_DEACTIVE_CNF,
                        CNAS_HSM_GetScpDeactiveWaitScpDeactiveCnfTimerLen_SessionDeact());

    CNAS_HSM_SetCurrSubState(CNAS_HSM_SS_SESSION_DEACT_WAIT_SCP_DEACTIVE_CNF);

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvTiWaitHrpdConnCloseIndTimerExpire_SessionDeact_WaitConnCloseInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_WARNING_LOG(UEPS_PID_HSM, "CNAS_HSM_RcvTiWaitHrpdConnCloseIndTimerExpire_SessionDeact_WaitConnCloseInd: timer out");

    CNAS_HSM_SetConnStatus(CNAS_HSM_HRPD_CONN_STATUS_CLOSE);

    /* 因为只有SCP激活的情况下才能发起断链，所以此处SCP肯定是激活状态 */
    CNAS_HSM_SndCasHrpdScpDeActiveReq();

    CNAS_HSM_StartTimer(TI_CNAS_HSM_WAIT_SCP_DEACTIVE_CNF,
                        CNAS_HSM_GetScpDeactiveWaitScpDeactiveCnfTimerLen_SessionDeact());

    CNAS_HSM_SetCurrSubState(CNAS_HSM_SS_SESSION_DEACT_WAIT_SCP_DEACTIVE_CNF);
    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvHsdPowerOffReq_SessionDeact_WaitScpDeactiveCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulRemainLen;

    /* 缓存power off, 记录被打断但是流程继续，流程结束时返回结果为abort，
       等流程结束后，处理power off。如果已经在等待scp deactive cnf,且此前的定
       时器剩余时长大于1.5s，修改定时器时长为1.5S，重新启动，否则不重新启动 */

    ulRemainLen = CNAS_GetSpecifiedTimerRemainLen(UEPS_PID_HSM, TI_CNAS_HSM_WAIT_SCP_DEACTIVE_CNF, 0);
    if (TI_CNAS_HSM_REVISE_WAIT_SCP_DEACTIVE_CNF_LEN < ulRemainLen)
    {
        CNAS_HSM_StopTimer(TI_CNAS_HSM_WAIT_SCP_DEACTIVE_CNF);

        CNAS_HSM_StartTimer(TI_CNAS_HSM_WAIT_SCP_DEACTIVE_CNF ,
                            TI_CNAS_HSM_REVISE_WAIT_SCP_DEACTIVE_CNF_LEN);
    }

    CNAS_HSM_SetAbortFlag_SessionDeact(VOS_TRUE);

    CNAS_HSM_SaveCacheMsg(ulEventType, pstMsg);

    CNAS_HSM_SetReviseTimerScene_SessionDeact(CNAS_HSM_SESSION_DEACT_REVISE_TIMER_SCENE_POWER_OFF);

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvTiWaitScpDeactiveCnfTimerExpire_SessionDeact_WaitScpDeactiveCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_WARNING_LOG(UEPS_PID_HSM, "CNAS_HSM_RcvTiWaitScpDeactiveCnfTimerExpire_SessionDeact_WaitScpDeactiveCnf: timer out");

    CNAS_HSM_QuitFsmScpDeactiveReq();

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvCasScpDeactiveCnf_SessionDeact_WaitScpDeactiveCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_HSM_StopTimer(TI_CNAS_HSM_WAIT_SCP_DEACTIVE_CNF);

    CNAS_HSM_QuitFsmScpDeactiveReq();

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvHsdSuspendInd_SessionDeact_WaitSessionCloseSnpDataCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_HSM_SetAbortFlag_SessionDeact(VOS_TRUE);

    CNAS_HSM_SetSuspendFlag_SessionDeact(VOS_TRUE);

    CNAS_HSM_SaveCacheMsg(ulEventType, pstMsg);

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvHsdPowerSaveReq_SessionDeact_WaitSessionCloseSnpDataCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          ulRemainLen;

    /* 缓存power save, 记录被打断但是流程继续，流程结束时返回结果为abort，
       等流程结束后，处理power save。如果已经在等待snp data cnf,且此前的定
       时器剩余时长大于2s，修改定时器时长为2S，重新启动，否则不重新启动 */

    ulRemainLen = CNAS_GetSpecifiedTimerRemainLen(UEPS_PID_HSM, TI_CNAS_HSM_SESSION_CLOSE_WAIT_SNP_DATA_CNF, 0);
    if (TI_CNAS_HSM_REVISE_WAIT_SNP_DATA_CNF_LEN < ulRemainLen)
    {
        CNAS_HSM_StopTimer(TI_CNAS_HSM_SESSION_CLOSE_WAIT_SNP_DATA_CNF);

        CNAS_HSM_StartTimer(TI_CNAS_HSM_SESSION_CLOSE_WAIT_SNP_DATA_CNF ,
                            TI_CNAS_HSM_REVISE_WAIT_SNP_DATA_CNF_LEN);
    }

    CNAS_HSM_SetAbortFlag_SessionDeact(VOS_TRUE);

    CNAS_HSM_SaveCacheMsg(ulEventType, pstMsg);

    CNAS_HSM_SetReviseTimerScene_SessionDeact(CNAS_HSM_SESSION_DEACT_REVISE_TIMER_SCENE_POWER_SAVE);

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvHsdResumeInd_SessionDeact_WaitSessionCloseSnpDataCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_HSM_SetAbortFlag_SessionDeact(VOS_TRUE);

    CNAS_HSM_SaveCacheMsg(ulEventType, pstMsg);

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvRrmStatusInd_SessionDeact_WaitSessionCloseSnpDataCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_HSM_SetAbortFlag_SessionDeact(VOS_TRUE);

    CNAS_HSM_SaveCacheMsg(ulEventType, pstMsg);

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSD_RcvCasNetworkLostIndProc_SessionDeact_WaitSessionCloseSnpDataCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_HSM_SESSION_CLOSE_MSG_STRU                        *pstStoreSessionClose = VOS_NULL_PTR;
    CNAS_HSM_SESSION_DEACT_REASON_ENUM_UINT8                enSessionDeactReason;

    pstStoreSessionClose = CNAS_HSM_GetStoreSessionCloseAddr();

    CNAS_HSM_StopTimer(TI_CNAS_HSM_SESSION_CLOSE_WAIT_SNP_DATA_CNF);

    CNAS_HSM_SndHrmDataCancelNtf(CNAS_HRM_DATA_REQ_SESSION_CLOSE);
    CNAS_HSM_ResetSessionCloseRetryTimes();

    NAS_MEM_SET_S(pstStoreSessionClose, sizeof(CNAS_HSM_SESSION_CLOSE_MSG_STRU), 0x0, sizeof(CNAS_HSM_SESSION_CLOSE_MSG_STRU));

    /* 与CAS讨论结果，只要激活过SCP则在去激活过程中无条件断链，勿须判断链接状态,
       但是uati或者session激活等流程被suspend打断时，不发送断链指示到cas */
    if (VOS_TRUE == CNAS_HSM_GetScpActiveFlag())
    {
        /* suspend打断时，尽量控制不要往cas下发断链指示，cas发起suspend时，下层是没有链路的，
           直接发送scp deactive req给cas */
        enSessionDeactReason = CNAS_HSM_GetSessionDeactReason_SessionDeact();

        if ((VOS_TRUE == CNAS_HSM_GetSuspendFlag_SessionDeact())
         || ((CNAS_HSM_SESSION_DEACT_REASON_UATI_ABORT_BY_SUSPEND        == enSessionDeactReason)
          || (CNAS_HSM_SESSION_DEACT_REASON_SESSION_ACT_ABORT_BY_SUSPEND == enSessionDeactReason)))
        {
            CNAS_HSM_SndCasHrpdScpDeActiveReq();

            CNAS_HSM_StartTimer(TI_CNAS_HSM_WAIT_SCP_DEACTIVE_CNF,
                                CNAS_HSM_GetScpDeactiveWaitScpDeactiveCnfTimerLen_SessionDeact());

            CNAS_HSM_SetCurrSubState(CNAS_HSM_SS_SESSION_DEACT_WAIT_SCP_DEACTIVE_CNF);
        }
        else
        {
            CNAS_HSM_SndCasHrpdConnCloseReq(VOS_TRUE);

            CNAS_HSM_StartTimer(TI_CNAS_HSM_WAIT_HRPD_CONN_CLOSE_IND,
                                CNAS_HSM_GetConnCloseWaitConnCloseIndTimerLen_SessionDeact());

            CNAS_HSM_SetCurrSubState(CNAS_HSM_SS_SESSION_DEACT_WAIT_CONN_CLOSE_IND);
        }

    }
    else
    {
        CNAS_HSM_QuitFsmScpDeactiveReq();
    }

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvHsdOhmInd_SessionDeact_WaitSessionCloseSnpDataCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_HSM_SaveCacheMsg(ulEventType, pstMsg);

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvHsdPowerSaveReq_SessionDeact_WaitConnCloseInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_HSM_SetAbortFlag_SessionDeact(VOS_TRUE);

    CNAS_HSM_SaveCacheMsg(ulEventType, pstMsg);

    CNAS_HSM_SetReviseTimerScene_SessionDeact(CNAS_HSM_SESSION_DEACT_REVISE_TIMER_SCENE_POWER_SAVE);

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvRrmStatusInd_SessionDeact_WaitConnCloseInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_HSM_SetAbortFlag_SessionDeact(VOS_TRUE);

    CNAS_HSM_SaveCacheMsg(ulEventType, pstMsg);

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvCasHrpdNetworkLostInd_SessionDeact_WaitConnCloseInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 在wait conn close ind状态收到cas上报的掉网指示消息，处理同收到conn close ind */
    CNAS_HSM_StopTimer(TI_CNAS_HSM_WAIT_HRPD_CONN_CLOSE_IND);

    CNAS_HSM_SndCasHrpdScpDeActiveReq();

    CNAS_HSM_StartTimer(TI_CNAS_HSM_WAIT_SCP_DEACTIVE_CNF,
                        CNAS_HSM_GetScpDeactiveWaitScpDeactiveCnfTimerLen_SessionDeact());

    CNAS_HSM_SetCurrSubState(CNAS_HSM_SS_SESSION_DEACT_WAIT_SCP_DEACTIVE_CNF);

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvHsdOhmInd_SessionDeact_WaitConnCloseInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_HSM_SaveCacheMsg(ulEventType, pstMsg);

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvHsdSuspendInd_SessionDeact_WaitConnCloseInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_HSM_SetAbortFlag_SessionDeact(VOS_TRUE);

    CNAS_HSM_SetSuspendFlag_SessionDeact(VOS_TRUE);

    CNAS_HSM_SaveCacheMsg(ulEventType, pstMsg);

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvHsdSuspendInd_SessionDeact_WaitScpDeactiveCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_HSM_SetAbortFlag_SessionDeact(VOS_TRUE);

    CNAS_HSM_SetSuspendFlag_SessionDeact(VOS_TRUE);

    CNAS_HSM_SaveCacheMsg(ulEventType, pstMsg);

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvHsdPowerSaveReq_SessionDeact_WaitScpDeactiveCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_HSM_SetAbortFlag_SessionDeact(VOS_TRUE);

    CNAS_HSM_SaveCacheMsg(ulEventType, pstMsg);

    CNAS_HSM_SetReviseTimerScene_SessionDeact(CNAS_HSM_SESSION_DEACT_REVISE_TIMER_SCENE_POWER_SAVE);

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvRrmStatusInd_SessionDeact_WaitScpDeactiveCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_HSM_SetAbortFlag_SessionDeact(VOS_TRUE);

    CNAS_HSM_SaveCacheMsg(ulEventType, pstMsg);

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvHsdSystemAcquiredInd_SessionDeact_WaitScpDeactiveCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* may rcv during implement SCP Deactive procedure in Slave State */
    CNAS_HSM_SetAbortFlag_SessionDeact(VOS_TRUE);

    CNAS_HSM_SaveCacheMsg(ulEventType, pstMsg);

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvHsdResumeInd_SessionDeact_WaitScpDeactiveCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* may rcv during implement SCP Deactive procedure in Slave State
       or rcv in Suspend ind invoke SCP Deactive procedure */
    CNAS_HSM_SetAbortFlag_SessionDeact(VOS_TRUE);

    CNAS_HSM_SaveCacheMsg(ulEventType, pstMsg);

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvHsdIratFromLteInd_SessionDeact_WaitScpDeactiveCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* may rcv during implement SCP Deactive procedure in Slave State */
    CNAS_HSM_SetAbortFlag_SessionDeact(VOS_TRUE);

    CNAS_HSM_SaveCacheMsg(ulEventType, pstMsg);

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvHsdPilotSrchFailNtf_SessionDeact_WaitScpDeactiveCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 在Slave态KeepAlive定时器超时后进行本地session deactive过程中网络可能下发Pilot Search Fail */
    CNAS_HSM_SetAbortFlag_SessionDeact(VOS_TRUE);

    CNAS_HSM_SaveCacheMsg(ulEventType, pstMsg);

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_RcvHsdOhmInd_SessionDeact_WaitScpDeactiveCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    CNAS_HSM_SaveCacheMsg(ulEventType, pstMsg);

    return CNAS_HSM_FSM_EVENT_HANDLED;
}


VOS_UINT32 CNAS_HSM_GetSessionCloseWaitSnpDataCnfTimerLen_SessionDeact(VOS_VOID)
{
    VOS_UINT32                                              ulTimerLen;
    CNAS_HSM_SESSION_DEACT_REVISE_TIMER_SCENE_ENUM_UINT8    enReviseTimerScene;

    enReviseTimerScene = CNAS_HSM_GetReviseTimerScene_SessionDeact();

    switch (enReviseTimerScene)
    {
        /* 由于power off、power save 打断导致的流程失败需要发送session close消息时，
           使用修正后的定时器时长2S, 否则使用正常定时器时长30S */
        case CNAS_HSM_SESSION_DEACT_REVISE_TIMER_SCENE_POWER_OFF:
        case CNAS_HSM_SESSION_DEACT_REVISE_TIMER_SCENE_POWER_SAVE:
            ulTimerLen = TI_CNAS_HSM_REVISE_WAIT_SNP_DATA_CNF_LEN;

            break;

        default :
            ulTimerLen = TI_CNAS_HSM_WAIT_SNP_DATA_CNF_LEN;

            break;
    }

    return ulTimerLen;
}


VOS_UINT32 CNAS_HSM_GetConnCloseWaitConnCloseIndTimerLen_SessionDeact(VOS_VOID)
{
    VOS_UINT32                                              ulTimerLen;
    CNAS_HSM_SESSION_DEACT_REVISE_TIMER_SCENE_ENUM_UINT8    enReviseTimerScene;

    enReviseTimerScene = CNAS_HSM_GetReviseTimerScene_SessionDeact();

    switch (enReviseTimerScene)
    {
        /* 由于power off打断导致的流程失败需要发送conn close req消息时，
           使用修正后的定时器时长1.5S, 否则使用正常定时器时长15S */

        case CNAS_HSM_SESSION_DEACT_REVISE_TIMER_SCENE_POWER_OFF:
            ulTimerLen = TI_CNAS_HSM_REVISE_WAIT_HRPD_CONN_CLOSE_IND_LEN;

            break;

        default :
            ulTimerLen = TI_CNAS_HSM_WAIT_HRPD_CONN_CLOSE_IND_LEN;

            break;
    }

    return ulTimerLen;
}


VOS_UINT32 CNAS_HSM_GetScpDeactiveWaitScpDeactiveCnfTimerLen_SessionDeact(VOS_VOID)
{
    VOS_UINT32                                              ulTimerLen;
    CNAS_HSM_SESSION_DEACT_REVISE_TIMER_SCENE_ENUM_UINT8    enReviseTimerScene;

    enReviseTimerScene = CNAS_HSM_GetReviseTimerScene_SessionDeact();

    switch (enReviseTimerScene)
    {
        /* 由于power off打断导致的流程失败需要发送scp deactive req消息时，
           使用修正后的定时器时长1.5S, 否则使用正常定时器时长3S */
        case CNAS_HSM_SESSION_DEACT_REVISE_TIMER_SCENE_POWER_OFF:
            ulTimerLen = TI_CNAS_HSM_REVISE_WAIT_SCP_DEACTIVE_CNF_LEN;

            break;

        default :
            ulTimerLen = TI_CNAS_HSM_WAIT_SCP_DEACTIVE_CNF_LEN;

            break;
    }

    return ulTimerLen;
}


CNAS_HSM_SESSION_DEACT_REVISE_TIMER_SCENE_ENUM_UINT8 CNAS_HSM_GetStartReviseTimerSceneBySessionDeactReason_SessionDeact(
    CNAS_HSM_SESSION_DEACT_REASON_ENUM_UINT8                enSessionDeactReason
)
{
    CNAS_HSM_SESSION_DEACT_REVISE_TIMER_SCENE_ENUM_UINT8    enReviseTimerScene;

    switch (enSessionDeactReason)
    {
        case CNAS_HSM_SESSION_DEACT_REASON_SESSION_ACT_ABORT_BY_POWER_OFF:
        case CNAS_HSM_SESSION_DEACT_REASON_UATI_ABORT_BY_POWER_OFF:
            enReviseTimerScene = CNAS_HSM_SESSION_DEACT_REVISE_TIMER_SCENE_POWER_OFF;

            break;

        case CNAS_HSM_SESSION_DEACT_REASON_SESSION_ACT_ABORT_BY_POWER_SAVE:
        case CNAS_HSM_SESSION_DEACT_REASON_UATI_ABORT_BY_POWER_SAVE:
            enReviseTimerScene = CNAS_HSM_SESSION_DEACT_REVISE_TIMER_SCENE_POWER_SAVE;

            break;

        default :
            enReviseTimerScene = CNAS_HSM_SESSION_DEACT_REVISE_TIMER_SCENE_BUTT;

            break;
    }

    return enReviseTimerScene;
}


VOS_UINT8 CNAS_HSM_IsNeedRetransmitSessionClose_SessionDeact(
    HRM_HSM_DATA_CNF_RSLT_ENUM_UINT32                       enSnpDataCnfRslt,
    CTTF_HRPD_SNP_DATA_FAIL_REASON_ENUM_UINT32              enSnpDataCnfFailReason
)
{
    CNAS_HSM_SESSION_CLOSE_MSG_STRU                        *pstStoreSessionClose;
    CNAS_HSM_HRPD_CAS_STATUS_ENUM_UINT16                    enConvertedCasStatus;
    VOS_UINT8                                               ucIsNeedRetransmit;

    pstStoreSessionClose     = CNAS_HSM_GetStoreSessionCloseAddr();
    enConvertedCasStatus     = CNAS_HSM_GetHrpdConvertedCasStatus();
    ucIsNeedRetransmit       = VOS_TRUE;

    if ((HRM_HSM_DATA_CNF_RSLT_SUCC  == enSnpDataCnfRslt)
     || (HRM_HSM_DATA_CNF_RSLT_NO_RF == enSnpDataCnfRslt))
    {
        CNAS_HSM_StopTimer(TI_CNAS_HSM_SESSION_CLOSE_WAIT_SNP_DATA_CNF);

        CNAS_HSM_SndTtfHrpdPASessionCloseInd();

        CNAS_HSM_ResetSessionCloseRetryTimes();

        NAS_MEM_SET_S(pstStoreSessionClose, sizeof(CNAS_HSM_SESSION_CLOSE_MSG_STRU), 0x0, sizeof(CNAS_HSM_SESSION_CLOSE_MSG_STRU));

        return VOS_FALSE;
    }

    /* 失败原因为CAN_NOT_TX和GENERIC_ERROR                                 -- 不再重发
        失败原因为SLP_RESET、AC_CHANNEL_NOT_EXIST、LINK_ACQUIRED和CONN_INIT -- 重发
        失败原因为CONN_CLOSE和RELEASE_ALL，若CNAS维护的CAS状态为INIT        --不再重发
        失败原因为CONN_CLOSE和RELEASE_ALL，若CNAS维护的CAS状态为IDLE或CONN  --重发      */
    switch (enSnpDataCnfFailReason)
    {
        case CTTF_HRPD_SNP_DATA_FAIL_REASON_CAN_NOT_TX:
        case CTTF_HRPD_SNP_DATA_FAIL_REASON_GENERIC_ERROR:

            CNAS_HSM_StopTimer(TI_CNAS_HSM_SESSION_CLOSE_WAIT_SNP_DATA_CNF);


            CNAS_HSM_ResetSessionCloseRetryTimes();

            NAS_MEM_SET_S(pstStoreSessionClose, sizeof(CNAS_HSM_SESSION_CLOSE_MSG_STRU), 0x0, sizeof(CNAS_HSM_SESSION_CLOSE_MSG_STRU));

            ucIsNeedRetransmit = VOS_FALSE;

            break;

        case CTTF_HRPD_SNP_DATA_FAIL_REASON_SLP_RESET:
        case CTTF_HRPD_SNP_DATA_FAIL_REASON_AC_CHANNEL_NOT_EXIST:
        case CTTF_HRPD_SNP_DATA_FAIL_REASON_LINK_ACQUIRED:
        case CTTF_HRPD_SNP_DATA_FAIL_REASON_CONN_INIT:

            if (CNAS_HSM_SESSION_CLOSE_MAX_RETRY_CNT <= CNAS_HSM_GetSessionCloseRetryTimes())
            {
                CNAS_HSM_StopTimer(TI_CNAS_HSM_SESSION_CLOSE_WAIT_SNP_DATA_CNF);


                CNAS_HSM_ResetSessionCloseRetryTimes();

                NAS_MEM_SET_S(pstStoreSessionClose, sizeof(CNAS_HSM_SESSION_CLOSE_MSG_STRU), 0x0, sizeof(CNAS_HSM_SESSION_CLOSE_MSG_STRU));

                ucIsNeedRetransmit = VOS_FALSE;
            }
            else
            {
                CNAS_HSM_SndSessionCloseMsg(pstStoreSessionClose);

                CNAS_HSM_IncreaseSessionCloseRetryTimes();

                ucIsNeedRetransmit = VOS_TRUE;
            }

            break;

       case CTTF_HRPD_SNP_DATA_FAIL_REASON_CONN_CLOSE:
       case CTTF_HRPD_SNP_DATA_FAIL_REASON_RELEASE_ALL:

            if (CNAS_HSM_HRPD_CAS_STATUS_ENUM_INIT == enConvertedCasStatus)
            {
                CNAS_HSM_StopTimer(TI_CNAS_HSM_SESSION_CLOSE_WAIT_SNP_DATA_CNF);


                CNAS_HSM_ResetSessionCloseRetryTimes();

                NAS_MEM_SET_S(pstStoreSessionClose, sizeof(CNAS_HSM_SESSION_CLOSE_MSG_STRU), 0x0, sizeof(CNAS_HSM_SESSION_CLOSE_MSG_STRU));

                ucIsNeedRetransmit = VOS_FALSE;
            }
            else if ((CNAS_HSM_HRPD_CAS_STATUS_ENUM_IDLE == enConvertedCasStatus)
                  || (CNAS_HSM_HRPD_CAS_STATUS_ENUM_CONN == enConvertedCasStatus))
            {
                if (CNAS_HSM_SESSION_CLOSE_MAX_RETRY_CNT <= CNAS_HSM_GetSessionCloseRetryTimes())
                {
                    CNAS_HSM_StopTimer(TI_CNAS_HSM_SESSION_CLOSE_WAIT_SNP_DATA_CNF);


                    CNAS_HSM_ResetSessionCloseRetryTimes();

                    NAS_MEM_SET_S(pstStoreSessionClose, sizeof(CNAS_HSM_SESSION_CLOSE_MSG_STRU), 0x0, sizeof(CNAS_HSM_SESSION_CLOSE_MSG_STRU));

                    ucIsNeedRetransmit = VOS_FALSE;
                }
                else
                {
                    CNAS_HSM_SndSessionCloseMsg(pstStoreSessionClose);

                    CNAS_HSM_IncreaseSessionCloseRetryTimes();

                    ucIsNeedRetransmit = VOS_TRUE;
                }

            }
            else
            {
                /* CNAS维护的CAS状态为BUTT, 异常保护 */
                CNAS_HSM_StopTimer(TI_CNAS_HSM_SESSION_CLOSE_WAIT_SNP_DATA_CNF);


                CNAS_HSM_ResetSessionCloseRetryTimes();

                NAS_MEM_SET_S(pstStoreSessionClose, sizeof(CNAS_HSM_SESSION_CLOSE_MSG_STRU), 0x0, sizeof(CNAS_HSM_SESSION_CLOSE_MSG_STRU));

                ucIsNeedRetransmit = VOS_FALSE;
            }

            break;

      default:

            CNAS_HSM_StopTimer(TI_CNAS_HSM_SESSION_CLOSE_WAIT_SNP_DATA_CNF);


            CNAS_HSM_ResetSessionCloseRetryTimes();

            NAS_MEM_SET_S(pstStoreSessionClose, sizeof(CNAS_HSM_SESSION_CLOSE_MSG_STRU), 0x0, sizeof(CNAS_HSM_SESSION_CLOSE_MSG_STRU));

            ucIsNeedRetransmit = VOS_FALSE;

            break;

    }

    return ucIsNeedRetransmit;
}


#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

