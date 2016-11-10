

/*****************************************************************************
  1 Header File Include
*****************************************************************************/
#include "CnasEhsmPreProcAct.h"
#include "CnasEhsmCtx.h"
#include "CnasEhsmMntn.h"
#include "CnasEhsmSndHsd.h"
#include "CnasEhsmSndAps.h"
#include "CnasEhsmSndEsm.h"
#include "ehsm_hsd_pif.h"
#include "CnasHsmCtx.h"
#include "CnasEhsmSndRrm.h"
/* To Do: Need to add EHSM depending interfaces */
#include "CnasEhsmDecode.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_CNAS_EHSM_PRE_PROC_ACT_C

/*****************************************************************************
  2 The Define Of The Gloabal Variable
*****************************************************************************/


/*****************************************************************************
  3 Function Define
*****************************************************************************/
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)


VOS_UINT32 CNAS_EHSM_RcvApsPdnContextInfoNotify_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    APS_EHSM_PDN_CONTEXT_INFO_NOTIFY_STRU                  *pstApsPdnCtx = VOS_NULL_PTR;
    CNAS_EHSM_CTX_STRU                                     *pstEhsmCtx   = VOS_NULL_PTR;
    CNAS_EHSM_EHRPD_PDN_BEAR_INFO_STRU                     *pstPdnInfo   = VOS_NULL_PTR;
    VOS_UINT32                                              i;
    VOS_UINT8                                               aucIpv6Prefix[CNAS_EHSM_IPV6_ADDR_LEN-CNAS_EHSM_IPV6_IFID_LEN];


    pstApsPdnCtx    = (APS_EHSM_PDN_CONTEXT_INFO_NOTIFY_STRU *)pstMsg;
    pstEhsmCtx      = CNAS_EHSM_GetEhsmCtxAddr();

    for (i = 0; i < CNAS_EHSM_MAX_PDN_NUM; i++)
    {
        /* RabId作为APS与EHSM之间唯一的标示符 */
        if ((pstEhsmCtx->astLocalPdnBearInfo[i].ulEpsbId == pstApsPdnCtx->ucRabId)
         && (VOS_TRUE == pstEhsmCtx->astLocalPdnBearInfo[i].ucInUsed))
        {
            pstEhsmCtx->astLocalPdnBearInfo[i].ucCid = pstApsPdnCtx->ucCid;

            /* 实际使用中i与PdnId的值相等，但是不一定能使用APS消息中携带的值 */
            pstPdnInfo = &(pstEhsmCtx->astLocalPdnBearInfo[i]);

            if (1 == pstApsPdnCtx->stIpv4Dns.bitOpIpv4PrimDNS)
            {
                pstPdnInfo->stPcoIpv4Item.bitOpPriDns = 1;
                NAS_MEM_CPY_S(&(pstPdnInfo->stPcoIpv4Item.aucPriDns[0]),
                              CNAS_EHSM_IPV4_ADDR_LEN,
                              &(pstApsPdnCtx->stIpv4Dns.aucIpv4PrimDNS[0]),
                              EHSM_APS_IPV4_ADDR_LEN);
            }

            if (1 == pstApsPdnCtx->stIpv4Dns.bitOpIpv4SecDNS)
            {
                pstPdnInfo->stPcoIpv4Item.bitOpSecDns = 1;
                NAS_MEM_CPY_S(&(pstPdnInfo->stPcoIpv4Item.aucSecDns[0]),
                              CNAS_EHSM_IPV4_ADDR_LEN,
                              &(pstApsPdnCtx->stIpv4Dns.aucIpv4SecDNS[0]),
                              EHSM_APS_IPV4_ADDR_LEN);
            }

            if (1 == pstApsPdnCtx->stIpv6Dns.bitOpIpv6PrimDNS)
            {
                pstPdnInfo->stPcoIpv6Item.bitOpPriDns = 1;
                NAS_MEM_CPY_S(&(pstPdnInfo->stPcoIpv6Item.aucPriDns[0]),
                              CNAS_EHSM_IPV6_DNS_LEN,
                              &(pstApsPdnCtx->stIpv6Dns.aucIpv6PrimDNS[0]),
                              EHSM_APS_IPV6_ADDR_LEN);
            }

            if (1 == pstApsPdnCtx->stIpv6Dns.bitOpIpv6SecDNS)
            {
                pstPdnInfo->stPcoIpv6Item.bitOpSecDns = 1;
                NAS_MEM_CPY_S(&(pstPdnInfo->stPcoIpv6Item.aucSecDns[0]),
                              CNAS_EHSM_IPV6_DNS_LEN,
                              &(pstApsPdnCtx->stIpv6Dns.aucIpv6SecDNS[0]),
                              EHSM_APS_IPV6_ADDR_LEN);
            }

            if (1 == pstApsPdnCtx->stIpv6Prefix.bitOpIpv6Prefix)
            {
                PS_MEM_CPY(&(pstPdnInfo->stPdnAddr.aucIpv6Addr[0]), &(pstApsPdnCtx->stIpv6Prefix.aucIpv6Prefix[0]), (EHSM_APS_IPV6_ADDR_LEN-EHSM_APS_IPV6_IFID_LEN));
            }

            /*Send PDN sync ind to LNAS */
            if (EHSM_APS_PDN_OPTION_MODIFIED == pstApsPdnCtx->enOption)
            {
                if ((CNAS_EHSM_PDN_TYPE_IPV4V6 == pstPdnInfo->enPdnType)
                 || (CNAS_EHSM_PDN_TYPE_IPV6   == pstPdnInfo->enPdnType))
                {
                    PS_MEM_SET(aucIpv6Prefix, 0, (EHSM_APS_IPV6_ADDR_LEN-EHSM_APS_IPV6_IFID_LEN));

                    if ((0 != PS_MEM_CMP(&(pstPdnInfo->stPdnAddr.aucIpv6Addr[0]), aucIpv6Prefix, (EHSM_APS_IPV6_ADDR_LEN-EHSM_APS_IPV6_IFID_LEN)))
                     && (0 == pstPdnInfo->stPdnAddr.ucSyncToEsmIpv6Addr))
                    {
                        pstPdnInfo->stPdnAddr.ucSyncToEsmIpv6Addr = 1;
                        CNAS_EHSM_SndEsmSyncEhrpdPdnInfoInd(pstPdnInfo, VOS_FALSE, EHSM_ESM_PDN_OPT_MODIFIED);
                    }
                }
                else
                {
                    CNAS_EHSM_SndEsmSyncEhrpdPdnInfoInd(pstPdnInfo, VOS_FALSE, EHSM_ESM_PDN_OPT_MODIFIED);
                }
            }

            break;
        }

        /* C和L都在从模的时候，如果此时CID改变需要更新 */
        if ((pstEhsmCtx->astLtePdnBearInfo[i].ulEpsbId== pstApsPdnCtx->ucRabId)
         && (VOS_TRUE == pstEhsmCtx->astLtePdnBearInfo[i].ucInUsed))
        {
            pstEhsmCtx->astLtePdnBearInfo[i].ucCid = pstApsPdnCtx->ucCid;
        }
    }


    return VOS_TRUE;
}


VOS_UINT32 CNAS_EHSM_RcvHsmConnectInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_EHSM_SetAirLinkExistFlag(VOS_TRUE);

    if (CNAS_EHSM_EHRPD_DORMANT_STA == CNAS_EHSM_GetEhrpdState())
    {
        CNAS_EHSM_SetEhrpdState(CNAS_EHSM_EHRPD_ACTIVE_STA);
    }

    /* 进状态机处理 */
    return VOS_FALSE;
}


VOS_UINT32 CNAS_EHSM_RcvHsmDiscInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_EHSM_SetAirLinkExistFlag(VOS_FALSE);

    return VOS_FALSE;
}


VOS_UINT32 CNAS_EHSM_RcvPppModifyInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CTTF_CNAS_EHRPD_MODIFY_IND_STRU    *pstModifyInd;
    EHSM_APS_PDN_CHANGE_INFO_STRU       stPdnChgInfo;
    CNAS_EHSM_EHRPD_PDN_BEAR_INFO_STRU *pstEhrpdPdnAddr = VOS_NULL_PTR;
    CNAS_EHSM_PDN_ADDR_STRU             stPdnAddr;

    /* 初始化, 获取消息内容 */
    pstModifyInd    = (CTTF_CNAS_EHRPD_MODIFY_IND_STRU*)pstMsg;
    pstEhrpdPdnAddr = CNAS_EHSM_GetEhrpdLocPdnAddr(pstModifyInd->ucPdnId);
    NAS_MEM_SET_S(&stPdnChgInfo, sizeof(EHSM_APS_PDN_CHANGE_INFO_STRU), 0X00, sizeof(EHSM_APS_PDN_CHANGE_INFO_STRU));
    NAS_MEM_SET_S(&stPdnAddr, sizeof(CNAS_EHSM_PDN_ADDR_STRU), 0X00, sizeof(CNAS_EHSM_PDN_ADDR_STRU));

    /* 如果Modify的PdnId非激活态，则return掉不处理 */
    if (VOS_TRUE != CNAS_EHSM_IsPdnActived(pstModifyInd->ucPdnId))
    {
        return VOS_TRUE;
    }

    if (CNAS_EHSM_CAUSE_SUCCESS != CNAS_EHSM_DecodeNwPDNValue(0,
                                                             (VOS_UINT8 *)&(pstModifyInd->stPdnAddress),
                                                             &stPdnAddr))
    {
       CNAS_ERROR_LOG(UEPS_PID_EHSM, "CNAS_EHSM_RcvPppModifyInd_Active:Wrong Msg Entry");
       return VOS_TRUE;
    }

    stPdnChgInfo.stPdnAddr.enPdnType        = stPdnAddr.enPdnType;
    pstEhrpdPdnAddr->enPdnType              = stPdnAddr.enPdnType;
    pstEhrpdPdnAddr->stPdnAddr.enPdnType    = stPdnAddr.enPdnType;

    NAS_MEM_CPY_S(stPdnChgInfo.stPdnAddr.aucIpv4Addr, EHSM_APS_IPV4_ADDR_LEN, stPdnAddr.aucIpv4Addr, CNAS_CTTF_EHRPD_IPV4_ADDR_LEN);
    NAS_MEM_CPY_S(pstEhrpdPdnAddr->stPdnAddr.aucIpv4Addr, CNAS_EHSM_IPV4_ADDR_LEN, stPdnAddr.aucIpv4Addr, CNAS_CTTF_EHRPD_IPV4_ADDR_LEN);


    NAS_MEM_CPY_S(stPdnChgInfo.stPdnAddr.aucIpv6Addr, EHSM_APS_IPV6_ADDR_LEN, stPdnAddr.aucIpv6Addr, CNAS_CTTF_EHRPD_IPV6_ADDR_LEN);
    NAS_MEM_CPY_S(pstEhrpdPdnAddr->stPdnAddr.aucIpv6Addr, CNAS_EHSM_IPV6_ADDR_LEN, stPdnAddr.aucIpv6Addr, CNAS_CTTF_EHRPD_IPV6_ADDR_LEN);

    stPdnChgInfo.usMtu        = pstModifyInd->usMtu;
    pstEhrpdPdnAddr->usMtu    = pstModifyInd->usMtu;

    CNAS_EHSM_SndApsPdnChgInd(&stPdnChgInfo, pstModifyInd->ucPdnId);

    return VOS_TRUE;
}


VOS_UINT32 CNAS_EHSM_RcvEsmClearAllBearer_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                           ulPndNum;
    ulPndNum = CNAS_EHSM_GetLocalActivePdnConnNum();
    if ((CNAS_EHSM_HRPD_WORK_MODE_MASTER == CNAS_EHSM_GetHrpdWorkMode())
     && (0 != ulPndNum))
    {
        /* EHRPD已经attach成功了，不处理ESM CLEAR ALL消息 */
        CNAS_ERROR_LOG(UEPS_PID_EHSM, "Ehprd is Active!");
        return VOS_TRUE;
    }

    CNAS_EHSM_ClearAllLteLocalPdnInfo();

    return VOS_FALSE;
}


VOS_UINT32 CNAS_EHSM_RcvEsmSyncPdnInfoInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_EHSM_FSM_ID_ENUM_UINT32        enFsmId;
    VOS_UINT32                          ulState;
    ESM_EHSM_SYNC_PDN_INFO_IND_STRU    *pstPdnInfo   = VOS_NULL_PTR;

    pstPdnInfo      = (ESM_EHSM_SYNC_PDN_INFO_IND_STRU *)pstMsg;
    ulState         = CNAS_EHSM_GetFsmTopState();
    enFsmId         = CNAS_EHSM_GetCurrFsmId();

    /* 当前如果没有在主状态，或者在主状态，但是不为INACTIVE或者SUSPEND状态 */
    if ((VOS_TRUE == pstPdnInfo->ulAttachFlag)
     && ((CNAS_EHSM_FSM_L1_MAIN != enFsmId)
      || (CNAS_EHSM_L1_STA_ACTIVE == ulState)))
    {
        CNAS_EHSM_RecodeErrorLogInfo(pstPdnInfo->ulAttachFlag,
                                     pstPdnInfo->enPdnOption,
                                     pstPdnInfo->ulBitCid,
                                     pstPdnInfo->ulEpsbId,
                                     CNAS_EHSM_ERROR_CODE_PREPROC);
    }

    return VOS_FALSE;
}



VOS_UINT32 CNAS_EHSM_RcvRrmStatusInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_EHSM_FSM_CTX_STRU             *pstCurFsm = VOS_NULL_PTR;
    RRM_PS_STATUS_IND_STRU             *pstRrmStatusInd;

    pstRrmStatusInd = (RRM_PS_STATUS_IND_STRU *)pstMsg;

    if (RRM_PS_TASK_TYPE_HRPD_ATTACH != pstRrmStatusInd->enTaskType)
    {
        CNAS_WARNING_LOG(UEPS_PID_EHSM, "CNAS_EHSM_RcvRrmStatusInd_PreProc: Wrong Task Type");
    }

    /* 向RRM发送去注册消息 */
    CNAS_EHSM_SndRrmDeRegisterInd(RRM_PS_TASK_TYPE_HRPD_ATTACH);

    pstCurFsm   = CNAS_EHSM_GetCurFsmAddr();

    /* 只有在inactive态时，才处理资源可用的指示。在后续的状态机中处理 */
    if ((CNAS_EHSM_FSM_L1_MAIN     == pstCurFsm->enFsmId)
     && (CNAS_EHSM_L1_STA_INACTIVE == pstCurFsm->ulState))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_UINT32 CNAS_EHSM_RcvHsmSuspendInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_EHSM_SetHrpdWorkMode(CNAS_EHSM_HRPD_WORK_MODE_SLAVE);

    CNAS_EHSM_SetAirLinkExistFlag(VOS_FALSE);

    return VOS_FALSE;
}


VOS_UINT32 CNAS_EHSM_RcvHsmSessionInfoInd_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    HSM_EHSM_SESSION_INFO_IND_STRU     *pstHsmSessionInd = VOS_NULL_PTR;

    pstHsmSessionInd = (HSM_EHSM_SESSION_INFO_IND_STRU *)pstMsg;

    if ((EHSM_HSM_SESSION_TYPE_HRPD  == pstHsmSessionInd->enSessionType)
     || (EHSM_HSM_SESSION_TYPE_EHRPD == pstHsmSessionInd->enSessionType))
    {
        CNAS_EHSM_SetHrpdWorkMode(CNAS_EHSM_HRPD_WORK_MODE_MASTER);
    }

    if (EHSM_HSM_SESSION_TYPE_EHRPD != pstHsmSessionInd->enSessionType)
    {
        CNAS_EHSM_SetAirLinkExistFlag(VOS_FALSE);
    }


    /* 异常检测，如果当前的流程有问题，需要记录 */
    if (EHSM_HSM_SESSION_TYPE_EHRPD == pstHsmSessionInd->enSessionType)
    {
        CNAS_EHSM_PorcEhsmPdnInfoDifferWithAps();
    }


    /* 如果存在异常，勾包显示 */
    if (0 != CNAS_EHSM_GetCurrErrorLogICount())
    {
        CNAS_EHSM_MNTN_LogErrorInfo();
    }

    return VOS_FALSE;
}

#ifdef __PS_WIN32_RECUR__

VOS_UINT32 CNAS_EHSM_RestorePcReplayFixedContext_PreProc(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    CNAS_MNTN_LOG_PC_REPLAY_CONTEXT_STRU         *pstRcvMsg      = VOS_NULL_PTR;
    CNAS_EHSM_PC_REPLAY_FIXED_CONTEXT_MSG_STRU                       *pstFixedCtxMsg = VOS_NULL_PTR;
    CNAS_EHSM_CTX_STRU                                     *pstEhsmCtx = VOS_NULL_PTR;

    pstEhsmCtx     = CNAS_EHSM_GetEhsmCtxAddr();
    pstRcvMsg      = (CNAS_MNTN_LOG_PC_REPLAY_CONTEXT_STRU*)pstMsg;
    pstFixedCtxMsg = (CNAS_EHSM_PC_REPLAY_FIXED_CONTEXT_MSG_STRU*)pstRcvMsg->aucDataCtx;

    pstEhsmCtx->ulAutoAttachFlag = pstFixedCtxMsg->ulAutoAttachFlag;

    pstEhsmCtx->stRetryCtrlInfo.stRetryConnEstInfo.ulExpireTimerLen  = pstFixedCtxMsg->ulExpireTimerLenOfRetryConn;
    pstEhsmCtx->stRetryCtrlInfo.stRetryConnEstInfo.usMaxNoOfRetry    = pstFixedCtxMsg->usMaxNoOfRetryConn;

    pstEhsmCtx->stRetryCtrlInfo.stRetryPdnSetupInfo.ulExpireTimerLen = pstFixedCtxMsg->ulExpireTimerLenOfRetryPdn;
    pstEhsmCtx->stRetryCtrlInfo.stRetryPdnSetupInfo.usMaxNoOfRetry   = pstFixedCtxMsg->usMaxNoOfRetryPdn;

    return VOS_TRUE;
}
#endif


#endif /* FEATURE_UE_MODE_CDMA */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */



