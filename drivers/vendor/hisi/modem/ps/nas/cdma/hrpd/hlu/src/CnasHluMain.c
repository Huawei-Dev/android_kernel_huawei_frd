

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "vos.h"
#include "CnasHluMain.h"
#include "CnasHluComm.h"
#include "CnasHluSndHrm.h"

#include "CnasHluSndHsm.h"
#include "CnasMntn.h"

#include "CnasHluProcNvim.h"

#include "cnas_cttf_hrpd_pa_pif.h"

#include "CnasCcb.h"

#include "Nas_Mem.h"



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_CNAS_HLU_MAIN_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/
/*lint -save -e958*/
#ifdef __PS_WIN32_RECUR__

VOS_UINT32 CNAS_HLU_RestorePcReplayFixedContext(
    struct MsgCB                       *pstMsg
)
{
    CNAS_MNTN_LOG_PC_REPLAY_CONTEXT_STRU                   *pstRcvMsg      = VOS_NULL_PTR;
    CNAS_HLU_PC_REPLAY_FIXED_CONTEXT_MSG_STRU                        *pstFixedCtxMsg = VOS_NULL_PTR;
    CNAS_HLU_CTX_STRU                                      *pstCnasHluCtx = VOS_NULL_PTR;

    pstCnasHluCtx   = CNAS_HLU_GetLocCtxAddr();
    pstRcvMsg       = (CNAS_MNTN_LOG_PC_REPLAY_CONTEXT_STRU*)pstMsg;
    pstFixedCtxMsg  = (CNAS_HLU_PC_REPLAY_FIXED_CONTEXT_MSG_STRU*)pstRcvMsg->aucDataCtx;

    NAS_MEM_CPY_S(&(pstCnasHluCtx->stLocCtx), sizeof(CNAS_HLU_LOC_STRU), &(pstFixedCtxMsg->stLocCtx), sizeof(CNAS_HLU_LOC_STRU));
    NAS_MEM_CPY_S(&(pstCnasHluCtx->stBLOBCtx), sizeof(CNAS_HLU_BLOB_STRU), &(pstFixedCtxMsg->stBLOBCtx), sizeof(CNAS_HLU_BLOB_STRU));

    return VOS_TRUE;
}
#endif

VOS_VOID CNAS_HLU_LogPcReplayFixedContext(VOS_VOID)
{
    CNAS_HLU_CTX_STRU                                      *pstCnasHluCtx = VOS_NULL_PTR;
    CNAS_HLU_PC_REPLAY_FIXED_CONTEXT_MSG_STRU                         stFixedCtxData;

    pstCnasHluCtx      = CNAS_HLU_GetLocCtxAddr();

    NAS_MEM_SET_S(&stFixedCtxData, sizeof(stFixedCtxData), 0, sizeof(stFixedCtxData));

    NAS_MEM_CPY_S(&(stFixedCtxData.stLocCtx), sizeof(CNAS_HLU_LOC_STRU), &(pstCnasHluCtx->stLocCtx), sizeof(CNAS_HLU_LOC_STRU));
    NAS_MEM_CPY_S(&(stFixedCtxData.stBLOBCtx), sizeof(CNAS_HLU_BLOB_STRU), &(pstCnasHluCtx->stBLOBCtx), sizeof(CNAS_HLU_BLOB_STRU));

    CNAS_MNTN_LogPcReplayFixedContext(sizeof(CNAS_HLU_PC_REPLAY_FIXED_CONTEXT_MSG_STRU), UEPS_PID_HLU, (VOS_UINT8 *)&stFixedCtxData);
}


VOS_VOID CNAS_HLU_HndlHsmHandoffReq(
    HSM_HLU_1X_TO_HRPD_HANDOFF_REQ_STRU     *pstHsmHandOffReq
)
{
    CNAS_HLU_MSG_TX_OPTION_INFO         stMsgTxInfo;
    CNAS_HLU_LOC_VALUE_STRU             stHandOffLocVal;

    if (CNAS_HLU_RAN_HANDOFF_IS_SET == CNAS_HLU_GetRANHandoff())
    {

        CNAS_HLU_StartTimer(TI_CNAS_HLU_WAIT_HRM_DATA_CNF, TI_CNAS_HLU_WAIT_HRM_DATA_CNF_LEN);

        /* 设置HandOff标记 */
        CNAS_HLU_SetHandOffFlg(VOS_TRUE);


        /* SNP_DATA_REQ msg transmission info.  */
        stMsgTxInfo.enChannelType  = CTTF_HRPD_CHANNEL_AC_OR_RTC;
        stMsgTxInfo.enDeliveryType = CTTF_HRPD_DELIVERY_TYPE_RELIABLE;
        stMsgTxInfo.enAddressType  = CTTF_HRPD_ADDRESS_UNICAST;
        stMsgTxInfo.ucPriority     = CNAS_HLU_LOC_MSG_PRIORITY;

        /* Check and fill HANDOFF msg details received from AT. */
        if (CNAS_HLU_IS_EQUAL(CNAS_HLU_SID_INVALID, pstHsmHandOffReq->usSid)
         || CNAS_HLU_IS_EQUAL(CNAS_HLU_NID_INVALID, pstHsmHandOffReq->usNid)
         || CNAS_HLU_IS_EQUAL(CNAS_HLU_PACKET_ZONE_ID_INVALID, pstHsmHandOffReq->ucPktZoneId))
        {
            CNAS_WARNING_LOG3(UEPS_PID_HLU,"CNAS_HLU_HndlHsmHandoffReq:Invalid "
                              "Location value received from AT. HLU location "
                              "values will be used in HandOff.",
                              pstHsmHandOffReq->usSid,
                              pstHsmHandOffReq->usNid,
                              pstHsmHandOffReq->ucPktZoneId);

            CNAS_HLU_SendLocNotification(&stMsgTxInfo, VOS_TRUE, VOS_NULL_PTR, CNAS_HRM_DATA_REQ_HANDOFF_LOC_NTF);
        }
        else
        {
            stHandOffLocVal.usSID          = pstHsmHandOffReq->usSid;
            stHandOffLocVal.usNID          = pstHsmHandOffReq->usNid;
            stHandOffLocVal.ucPacketZoneID = pstHsmHandOffReq->ucPktZoneId;

            /* Send notification mesage to FSIG */
            CNAS_HLU_SendLocNotification(&stMsgTxInfo, VOS_TRUE, &stHandOffLocVal, CNAS_HRM_DATA_REQ_HANDOFF_LOC_NTF);

            /* When g_bCnasRANHandoff is enabled, CNAS_HLU_SndHsmHandoffCnf will be
               invoked in CNAS_HLU_DispatchFsigMsg. */
        }
    }
    else
    {
        CNAS_HLU_SndHsmHandoffCnf(CNAS_HLU_HSM_HANDOFF_UNSOLICIT_NOT_ALLOWED);

        CNAS_ERROR_LOG1(UEPS_PID_HLU, "CNAS_HLU_HndlHsmHandoffReq:" \
                                     "AT not allowed to send LocationNotification "
                                     "in unsolicited manner."
                                     "RANHandoff is disabled.",
                                     CNAS_HLU_GetRANHandoff());


    }

    return;
}



VOS_VOID CNAS_HLU_HndlHsmSessChngInd(
    HSM_HLU_SESSION_CHANGED_IND_STRU   *pstHsmSessChngInd
)
{
    CNAS_HLU_MSG_TX_OPTION_INFO         stMsgTxInfo;
    CNAS_CCB_PS_RATTYPE_ENUM_UINT32     ulPrevDataMode;
    HSM_HLU_SESSION_RAT_MODE_ENUM_UINT8 ulCurrDataMode;

    ulCurrDataMode = pstHsmSessChngInd->enRatMode;
    ulPrevDataMode = CNAS_CCB_GetPrevPsRatType();

    /* Check the cause of session change - this is judged and handled in HSM */
    if ((CNAS_HLU_RAN_HANDOFF_IS_SET == CNAS_HLU_GetRANHandoff())
     && (((CNAS_CCB_PS_RATTYPE_EHRPD == ulPrevDataMode) && (HSM_HLU_SESSION_RAT_MODE_EHRPD == ulCurrDataMode))
      || ((CNAS_CCB_PS_RATTYPE_HRPD == ulPrevDataMode) && (HSM_HLU_SESSION_RAT_MODE_HRPD == ulCurrDataMode))
      || ((CNAS_CCB_PS_RATTYPE_1X == ulPrevDataMode) && (HSM_HLU_SESSION_RAT_MODE_EHRPD == ulCurrDataMode))))
    {
        /* Send notification mesage to FSIG */
        stMsgTxInfo.enChannelType       = CTTF_HRPD_CHANNEL_AC_OR_RTC; /* need to confirm */
        stMsgTxInfo.enDeliveryType      = CTTF_HRPD_DELIVERY_TYPE_BEST_EFFORT; /* need to confirm */
        stMsgTxInfo.enAddressType       = CTTF_HRPD_ADDRESS_UNICAST;
        stMsgTxInfo.ucPriority          = CNAS_HLU_LOC_MSG_PRIORITY;


        CNAS_HLU_StartTimer(TI_CNAS_HLU_WAIT_HRM_DATA_CNF, TI_CNAS_HLU_WAIT_HRM_DATA_CNF_LEN);

        CNAS_HLU_SendLocNotification(&stMsgTxInfo, VOS_FALSE, VOS_NULL_PTR, CNAS_HRM_DATA_REQ_LOC_NTF);

    }
    else
    {
        CNAS_ERROR_LOG1(UEPS_PID_HLU,
                        "CNAS_HLU_HndlHsmSessChngInd:Location update procedures could not be performed,RANHandoff value is ",
                        CNAS_HLU_GetRANHandoff());
    }

    return;
}




VOS_VOID CNAS_HLU_HndlPowerOnReq(VOS_VOID)
{
    CNAS_HLU_SetHandOffFlg(VOS_FALSE);

    CNAS_HLU_GetLocCtxAddr()->ucOpId = 0;

    /* Read Last stored LocationValue from NV */
    CNAS_HLU_ReadLocInfoNvim();

    /* Read Last stored StorageBLOB from NV */
    CNAS_HLU_ReadStrgBlobNvim();

#ifndef __PS_WIN32_RECUR__
    CNAS_HLU_LogPcReplayFixedContext();
#endif

    /* Return Start confirm to HSD */
    CNAS_HLU_SndHsdStartCnf();

    return;
}


VOS_VOID CNAS_HLU_HndlPowerOffReq(VOS_VOID)
{
    CNAS_HLU_SetHandOffFlg(VOS_FALSE);

    CNAS_HLU_GetLocCtxAddr()->ucOpId = 0;

    CNAS_HLU_StopTimer(TI_CNAS_HLU_WAIT_HRM_DATA_CNF);

    CNAS_HLU_SndHsdPowerOffCnf();

    return;
}


VOS_VOID CNAS_HLU_HndlRpaCommitInd(
    CTTF_CNAS_HRPD_PA_COMMITTED_IND_STRU *pstCommitInd
)
{
    CNAS_HLU_SetProtocolType(pstCommitInd->enProtocolType);

    if ((pstCommitInd->ucRanHandoff != CNAS_HLU_RAN_HANDOFF_IS_SET)
     && (pstCommitInd->ucRanHandoff != CNAS_HLU_RAN_HANDOFF_IS_NOT_SET))
    {
        CNAS_ERROR_LOG1(UEPS_PID_HLU,
                        "CNAS_HLU_HndlRpaCommitInd:Invalid ucRanHandoff attribute value ",
                        pstCommitInd->ucRanHandoff);
        return;
    }

    CNAS_HLU_SetRANHandoff(pstCommitInd->ucRanHandoff);

    return;
}


VOS_VOID CNAS_HLU_DispatchHrmMsg(
    struct MsgCB                       *pstMsg
)
{
    HRM_HLU_DATA_IND_STRU              *pstHrmDataInd;
    HRM_HLU_DATA_CNF_STRU              *pstHrmAckMsg;

    CNAS_HLU_HSM_1X_TO_HRPD_HANDOFF_RSLT_ENUM_UINT32        enHandOffRslt;
    VOS_UINT8                                               ucOpId;
    VOS_UINT16                                              usMsgName;

    usMsgName = *((VOS_UINT16 *)(pstMsg->aucValue));

    switch (usMsgName)
    {
        case ID_HRM_HLU_DATA_IND:
        {

            pstHrmDataInd = (HRM_HLU_DATA_IND_STRU *)pstMsg;

            if (CTTF_HRPD_SNP_PROTOCOL_TYPE_BUTT == CNAS_HLU_GetProtocolType())
            {
                CNAS_HLU_SetProtocolType(pstHrmDataInd->enProtocolType);
            }

            /* Validate Instance type. */
            if (pstHrmDataInd->enInstanceType != CTTF_HRPD_INSTANCE_INUSE)
            {
                CNAS_ERROR_LOG1(UEPS_PID_HLU,
                                "CNAS_HLU_DispatchHrmMsg: Invalid instance type received.",
                                pstHrmDataInd->enInstanceType);
                return;
            }
            /* Decode and store the location details, if received.*/
            CNAS_HLU_ProcHrmDataInd(&pstHrmDataInd->stMsgData);

            break;
        }
        case ID_HRM_HLU_DATA_CNF:
        {
            CNAS_HLU_StopTimer(TI_CNAS_HLU_WAIT_HRM_DATA_CNF);


            /* 如果不在HandOff流程中,不需要回复HandOffRslt */
            if (VOS_FALSE == CNAS_HLU_GetHandOffFlg())
            {
                break;
            }

            ucOpId       = CNAS_HLU_GetHandOffOpId();
            pstHrmAckMsg = (HRM_HLU_DATA_CNF_STRU *)pstMsg;

            /* AT will not send HANDOFF_REQ again untill unless it receives
               the HANDOFF_CNF for last HANDOFF_REQ sent.
               Therefor, at any point of time, ucMsgId will correspond to
               last HANDOFF_REQ sent to FSIG and for which AT awaits the
               HANDOFF_CNF msg. */
            if (ucOpId == pstHrmAckMsg->usOpId)
            {
                enHandOffRslt = CNAS_HLU_GetHandOffRslt(pstHrmAckMsg->enRslt);

                /* Send HandOffCNF to HSM. */
                CNAS_HLU_SndHsmHandoffCnf(enHandOffRslt);

                CNAS_HLU_SetHandOffFlg(VOS_FALSE);
            }
            else
            {
                /* Discard the received ack msg from FSIG, It does not belong
                   to last HANDOFF reqest sent to FSIG. */
            }

            break;
        }
        default:
        {
            CNAS_ERROR_LOG1(UEPS_PID_HLU,
                            "CNAS_HLU_DispatchHrmMsg: Incorrect HRM msg received.",
                            usMsgName);


            break;
        }
    }

    return;
}



VOS_VOID CNAS_HLU_DispatchRpaMsg(
    struct MsgCB                       *pstMsg
)
{
    MSG_HEADER_STRU                                        *pstRpaMsg;
    CTTF_CNAS_HRPD_PA_COMMITTED_IND_STRU                   *pstCommitInd;

    pstRpaMsg = (MSG_HEADER_STRU *)pstMsg;

    switch (pstRpaMsg->ulMsgName)
    {
        case ID_CTTF_CNAS_HRPD_PA_COMMITTED_IND:
        {
            pstCommitInd = (CTTF_CNAS_HRPD_PA_COMMITTED_IND_STRU *)pstMsg;

            CNAS_HLU_HndlRpaCommitInd(pstCommitInd);
            break;
        }
        default:
        {
            CNAS_ERROR_LOG1(UEPS_PID_HLU,
                            "CNAS_HLU_DispatchRpaMsg: Incorrect FSIG msg received.",
                            pstRpaMsg->ulMsgName);
            break;
        }
    }

    return;
}



VOS_VOID  CNAS_HLU_DispatchHsdMsg(
    struct MsgCB                       *pstMsg
)
{
    MSG_HEADER_STRU                    *pstHdrMsg;

    pstHdrMsg = (MSG_HEADER_STRU *)pstMsg;

    switch (pstHdrMsg->ulMsgName)
    {
        case ID_HSD_HLU_START_REQ:
            /* HLU power on request.
               Return typr of CNAS_HLU_HandlePowerOnReq is not handled currently
               as there is no operation happening inside it.
               Return check can be added in future, if needed.
            */
            CNAS_HLU_HndlPowerOnReq();
            break;

        case ID_HSD_HLU_POWER_OFF_REQ:
            /* HLU power on request.
               Return typr of CNAS_HLU_HandlePowerOffReq is not handled currently
               as there is no operation happening inside it.
               Return check can be added in future, if needed.
            */
            CNAS_HLU_HndlPowerOffReq();
            break;

        default:
            CNAS_ERROR_LOG1(UEPS_PID_HLU,
                            "CNAS_HLU_DispatchHsdMsg: Incorrect FSIG msg received.",
                            pstHdrMsg->ulMsgName);
            break;
    }

    return;
}


VOS_VOID  CNAS_HLU_DispatchHsmMsg(
    struct MsgCB                       *pstMsg
)
{
    MSG_HEADER_STRU                    *pstHsmMsg;

    HSM_HLU_1X_TO_HRPD_HANDOFF_REQ_STRU     *pstHsmHandOffReq;

    HSM_HLU_SESSION_CHANGED_IND_STRU   *pstHsmSessChngInd;


    pstHsmMsg = (MSG_HEADER_STRU *)pstMsg;

    switch (pstHsmMsg->ulMsgName)
    {
        case ID_HSM_HLU_SESSION_CHANGED_IND:
        {
            pstHsmSessChngInd = (HSM_HLU_SESSION_CHANGED_IND_STRU *)pstMsg;

            /* Session change indication */
            CNAS_HLU_HndlHsmSessChngInd(pstHsmSessChngInd);


            break;
        }
        case ID_HSM_HLU_1X_TO_HRPD_HANDOFF_REQ:
        {
            /* 1x DO handoff request */
            pstHsmHandOffReq = (HSM_HLU_1X_TO_HRPD_HANDOFF_REQ_STRU *)pstMsg;

            CNAS_HLU_HndlHsmHandoffReq(pstHsmHandOffReq);


            break;
        }
        default:
            break;
    }

    return;
}


VOS_VOID  CNAS_HLU_DisPatchTimerMsg(
    struct MsgCB                       *pstMsg
)
{
    REL_TIMER_MSG                      *pstRcvTimerMsg;

    pstRcvTimerMsg = (REL_TIMER_MSG *)pstMsg;

    CNAS_HLU_StopTimer(pstRcvTimerMsg->ulName);

    switch (pstRcvTimerMsg->ulName)
    {
        case TI_CNAS_HLU_WAIT_HRM_DATA_CNF:
            /* 给 HRM 发 DATA CANCEL NTF */
            CNAS_HLU_SendDataCancelNtf();

            /* HANDOFF流程，需要给HSM返回结果 */
            if (VOS_TRUE == CNAS_HLU_GetHandOffFlg())
            {
                CNAS_HLU_SndHsmHandoffCnf(CNAS_HLU_HSM_HANDOFF_FAILURE);
            }

            break;
        default:
            break;
    }

    return;
}

VOS_VOID  CNAS_HLU_DispatchHluMsg(
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                                              ulMsgName;

    ulMsgName = *((VOS_UINT32 *)(pstMsg->aucValue));

    switch (ulMsgName)
    {
        case ID_CNAS_MNTN_LOG_PC_REPLAY_FIXED_CONTEXT:
#ifdef __PS_WIN32_RECUR__
            CNAS_HLU_RestorePcReplayFixedContext(pstMsg);
#endif
            break;

        default:
            break;
    }

    return;
}


VOS_VOID  CNAS_HLU_DispatchMsg(
    struct MsgCB                       *pstMsg
)
{
    if (VOS_NULL_PTR == pstMsg)
    {
        return;
    }

    switch (pstMsg->ulSenderPid)
    {
        case UEPS_PID_HSD:
            CNAS_HLU_DispatchHsdMsg(pstMsg);
            break;

        case UEPS_PID_HSM:
            CNAS_HLU_DispatchHsmMsg(pstMsg);
            break;
        case UEPS_PID_HRM:
            CNAS_HLU_DispatchHrmMsg(pstMsg);
            break;

        case CTTF_PID_HRPD_RPA:
            CNAS_HLU_DispatchRpaMsg(pstMsg);
            break;


        case VOS_PID_TIMER:
            CNAS_HLU_DisPatchTimerMsg(pstMsg);
            break;
        case UEPS_PID_HLU:
            CNAS_HLU_DispatchHluMsg(pstMsg);
            break;
        default:
            break;
    }

    return;
}



VOS_UINT32  CNAS_HLU_InitTask(VOS_VOID)
{
    /* Init global variable */
    NAS_MEM_SET_S((VOS_VOID *)(CNAS_HLU_GetLocCtxAddr()), sizeof(CNAS_HLU_CTX_STRU), 0, sizeof(CNAS_HLU_CTX_STRU));

    CNAS_HLU_SetRANHandoff(0);
    CNAS_HLU_SetProtocolType(CTTF_HRPD_SNP_PROTOCOL_TYPE_BUTT);


    return VOS_OK;
}


VOS_UINT32 CNAS_HLU_InitPid(
    enum VOS_INIT_PHASE_DEFINE         enInitPhase
)
{
    switch (enInitPhase)
    {
        case VOS_IP_LOAD_CONFIG:
            CNAS_HLU_InitTask();
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
    #endif
#endif




