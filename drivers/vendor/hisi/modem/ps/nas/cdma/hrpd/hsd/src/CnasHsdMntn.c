

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "CnasMntn.h"
#include "CnasHsdMntn.h"
#include "PsLib.h"
#include "hsd_mscc_pif.h"
#include "cas_hrpd_airlinkmgmt_nas_pif.h"
#include "CnasHsdTimer.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_CNAS_HSD_MNTN_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* 消息打印记录 */
CNAS_HSD_LOG_MSG_STATUS_STRU            g_stCnasHsdLogMsg;

/* 从MSCC收到的消息白名单 */
CNAS_HSD_MNTN_WHITE_LIST_MSG_STRU       g_stCnasHsdMntnFromMsccMsgTbl[]=
{
    {UEPS_PID_MSCC,                     UEPS_PID_HSD,                           ID_MSCC_HSD_SYSTEM_ACQUIRE_REQ},
    {UEPS_PID_MSCC,                     UEPS_PID_HSD,                           ID_MSCC_HSD_DATA_CALL_REDIAL_SYS_ACQ_REQ},
    {UEPS_PID_MSCC,                     UEPS_PID_HSD,                           ID_MSCC_HSD_SYS_CFG_REQ},
    {UEPS_PID_MSCC,                     UEPS_PID_HSD,                           ID_MSCC_HSD_POWER_SAVE_REQ},
    {UEPS_PID_MSCC,                     UEPS_PID_HSD,                           ID_MSCC_HSD_CFREQLOCK_NTF},
    {UEPS_PID_MSCC,                     UEPS_PID_HSD,                           ID_MSCC_HSD_BG_SEARCH_REQ},
    {UEPS_PID_MSCC,                     UEPS_PID_HSD,                           ID_MSCC_HSD_STOP_BG_SEARCH_REQ}
};

/* 从CAS收到的消息白名单 */
CNAS_HSD_MNTN_WHITE_LIST_MSG_STRU       g_stCnasHsdMntnFromCasMsgTbl[]=
{
    {UEPS_PID_HALMP,                    UEPS_PID_HSD,                           ID_CAS_CNAS_HRPD_SUSPEND_IND},
    {UEPS_PID_HALMP,                    UEPS_PID_HSD,                           ID_CAS_CNAS_HRPD_RESUME_IND},
    {UEPS_PID_HALMP,                    UEPS_PID_HSD,                           ID_CAS_CNAS_HRPD_IRAT_FROM_LTE_NTF},
    {UEPS_PID_HALMP,                    UEPS_PID_HSD,                           ID_CAS_CNAS_HRPD_IRAT_TO_LTE_NTF},
    {UEPS_PID_HALMP,                    UEPS_PID_HSD,                           ID_CAS_CNAS_HRPD_SUSPEND_REL_CNF},
    {UEPS_PID_HALMP,                    UEPS_PID_HSD,                           ID_CAS_CNAS_HRPD_PILOT_SEARCH_REQ_IND},
    {UEPS_PID_HALMP,                    UEPS_PID_HSD,                           ID_CAS_CNAS_HRPD_PILOT_SEARCH_STOP_IND},
    {UEPS_PID_HALMP,                    UEPS_PID_HSD,                           ID_CAS_CNAS_HRPD_BSR_LTE_CNF},
    {UEPS_PID_HALMP,                    UEPS_PID_HSD,                           ID_CAS_CNAS_HRPD_STOP_BSR_LTE_CNF},
    {UEPS_PID_HALMP,                    UEPS_PID_HSD,                           ID_CAS_CNAS_HRPD_SYSTEM_SYNC_CNF},
    {UEPS_PID_HALMP,                    UEPS_PID_HSD,                           ID_CAS_CNAS_HRPD_START_CNF},
    {UEPS_PID_HALMP,                    UEPS_PID_HSD,                           ID_CAS_CNAS_HRPD_SUSPEND_CNF},
    {UEPS_PID_HALMP,                    UEPS_PID_HSD,                           ID_CAS_CNAS_HRPD_SYS_CFG_CNF}
};

/* 定时器超时的消息白名单 */
CNAS_HSD_MNTN_WHITE_LIST_MSG_STRU       g_stCnasHsdMntnFromTimerMsgTbl[]=
{
    {VOS_PID_TIMER,                     UEPS_PID_HSD,                           TI_CNAS_HSD_WAIT_CAS_SUSPEND_CNF},
    {VOS_PID_TIMER,                     UEPS_PID_HSD,                           TI_CNAS_HSD_WAIT_CAS_IRAT_OR_RESUME_IND},
    {VOS_PID_TIMER,                     UEPS_PID_HSD,                           TI_CNAS_HSD_WAIT_CAS_SUSPEND_REL_CNF},
    {VOS_PID_TIMER,                     UEPS_PID_HSD,                           TI_CNAS_HSD_WAIT_CAS_BG_SRCH_CNF},
    {VOS_PID_TIMER,                     UEPS_PID_HSD,                           TI_CNAS_HSD_WAIT_CAS_STOP_BG_SRCH_CNF}
};

/* 发送给MSCC的消息白名单 */
CNAS_HSD_MNTN_WHITE_LIST_MSG_STRU       g_stCnasHsdMntnToMsccMsgTbl[]=
{
    {UEPS_PID_HSD,                      UEPS_PID_MSCC,                          ID_HSD_MSCC_SYSTEM_ACQUIRE_START_IND},
    {UEPS_PID_HSD,                      UEPS_PID_MSCC,                          ID_HSD_MSCC_SYSTEM_ACQUIRE_CNF},
    {UEPS_PID_HSD,                      UEPS_PID_MSCC,                          ID_HSD_MSCC_SYSTEM_ACQUIRE_END_IND},
    {UEPS_PID_HSD,                      UEPS_PID_MSCC,                          ID_HSD_MSCC_DATA_CALL_REDIAL_SYS_ACQ_CNF},
    {UEPS_PID_HSD,                      UEPS_PID_MSCC,                          ID_HSD_MSCC_INTERSYS_START_IND},
    {UEPS_PID_HSD,                      UEPS_PID_MSCC,                          ID_HSD_MSCC_INTERSYS_END_IND},
    {UEPS_PID_HSD,                      UEPS_PID_MSCC,                          ID_HSD_MSCC_BG_SEARCH_CNF},
    {UEPS_PID_HSD,                      UEPS_PID_MSCC,                          ID_HSD_MSCC_SYSTEM_SYNC_IND},
    {UEPS_PID_HSD,                      UEPS_PID_MSCC,                          ID_HSD_MSCC_POWER_SAVE_CNF},
    {UEPS_PID_HSD,                      UEPS_PID_MSCC,                          ID_HSD_MSCC_STOP_BG_SEARCH_CNF},
    {UEPS_PID_HSD,                      UEPS_PID_MSCC,                          ID_HSD_MSCC_STOP_BG_SEARCH_IND}
};

/* 发送给CAS的消息白名单 */
CNAS_HSD_MNTN_WHITE_LIST_MSG_STRU       g_stCnasHsdMntnToCasMsgTbl[]=
{
    {UEPS_PID_HSD,                      UEPS_PID_HALMP,                         ID_CNAS_CAS_HRPD_PILOT_SEARCH_SUCC_NTF},
    {UEPS_PID_HSD,                      UEPS_PID_HALMP,                         ID_CNAS_CAS_HRPD_PILOT_SEARCH_FAIL_NTF},
    {UEPS_PID_HSD,                      UEPS_PID_HALMP,                         ID_CNAS_CAS_HRPD_SUSPEND_REQ},
    {UEPS_PID_HSD,                      UEPS_PID_HALMP,                         ID_CNAS_CAS_HRPD_SUSPEND_RSP},
    {UEPS_PID_HSD,                      UEPS_PID_HALMP,                         ID_CNAS_CAS_HRPD_SUSPEND_REL_REQ},
    {UEPS_PID_HSD,                      UEPS_PID_HALMP,                         ID_CNAS_CAS_HRPD_RESUME_RSP},
    {UEPS_PID_HSD,                      UEPS_PID_HALMP,                         ID_CNAS_CAS_HRPD_BSR_LTE_REQ},
    {UEPS_PID_HSD,                      UEPS_PID_HALMP,                         ID_CNAS_CAS_HRPD_STOP_BSR_LTE_REQ},
    {UEPS_PID_HSD,                      UEPS_PID_HALMP,                         ID_CNAS_CAS_HRPD_SYS_CFG_REQ}
};

/*****************************************************************************
  3 函数定义
*****************************************************************************/
/*lint -save -e545 -e413 */

VOS_VOID CNAS_HSD_LogScanFreqList(
    CNAS_HSD_HRPD_SCAN_FREQ_LIST_STRU           *pstScanFreqList,
    CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32           enMsgLogType
)
{
    VOS_UINT32                                   i;
    CNAS_HSD_LOG_SCAN_FREQ_LIST_MSG_STRU        *pstMsg = VOS_NULL_PTR;

    pstMsg = (CNAS_HSD_LOG_SCAN_FREQ_LIST_MSG_STRU *)PS_MEM_ALLOC(UEPS_PID_HSD,
                          (sizeof(CNAS_HSD_LOG_SCAN_FREQ_LIST_MSG_STRU)));

    if (VOS_NULL_PTR == pstMsg)
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_LogScanFreqList:Alloc Memory Failed");

        return;
    }
    pstMsg->stMsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderPid     = UEPS_PID_HSD;
    pstMsg->stMsgHeader.ulReceiverPid   = UEPS_PID_HSD;
    pstMsg->stMsgHeader.ulMsgName       = ID_CNAS_HSD_MNTN_LOG_SCAN_FREQ_LIST_IND;

    pstMsg->enMsgLogType                = enMsgLogType;
    pstMsg->usTotalItemNum              = pstScanFreqList->usAvailFreqNum;
    pstMsg->usCurScanIndex              = pstScanFreqList->usCurrIndex;
    pstMsg->ucLowPrioLevel              = pstScanFreqList->ucLowPrioLevel;

    for (i = 0; i < pstScanFreqList->usAvailFreqNum; i++)
    {
        if (i >= CNAS_HSD_MAX_LOG_FREQ_NUM_IN_SCAN_LIST)
        {
            break;
        }

        pstMsg->astScanFreqList[i] = pstScanFreqList->pstScanFreqItem[i];
    }

    pstMsg->stMsgHeader.ulLength        = PS_OFFSET_OF(CNAS_HSD_LOG_SCAN_FREQ_LIST_MSG_STRU, astScanFreqList)
                                          - VOS_MSG_HEAD_LENGTH
                                          + (pstMsg->usTotalItemNum * sizeof(CNAS_HSD_HRPD_SCAN_FREQ_ITEM_STRU));

    CNAS_MNTN_TraceIntMsg(UEPS_PID_HSD, sizeof(CNAS_HSD_LOG_SCAN_FREQ_LIST_MSG_STRU),(VOS_VOID *)pstMsg);

    PS_MEM_FREE(UEPS_PID_HSD, pstMsg);

    return;
}


VOS_VOID CNAS_HSD_LogSysRecList(
    CNAS_HSD_HRPD_SYS_LIST_STRU                  *pstSysRecList,
    CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32            enMsgLogType
)
{
    VOS_UINT32                                    i;
    CNAS_HSD_LOG_HRPD_SYS_LIST_MSG_STRU          *pstMsg = VOS_NULL_PTR;

    pstMsg = (CNAS_HSD_LOG_HRPD_SYS_LIST_MSG_STRU *)PS_MEM_ALLOC(UEPS_PID_HSD,
                          (sizeof(CNAS_HSD_LOG_HRPD_SYS_LIST_MSG_STRU)));

    if (VOS_NULL_PTR == pstMsg)
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_LogSysRecList:Alloc Memory Failed");

        return;
    }

    pstMsg->stMsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderPid     = UEPS_PID_HSD;
    pstMsg->stMsgHeader.ulReceiverPid   = UEPS_PID_HSD;
    pstMsg->stMsgHeader.ulMsgName       = ID_CNAS_HSD_MNTN_LOG_HRPD_SYS_LIST_IND;

    pstMsg->enMsgLogType                = enMsgLogType;

    pstMsg->usSysNum   = pstSysRecList->usHrpdSysNum;

    for (i = 0; i < pstSysRecList->usHrpdSysNum; i++)
    {
        if (i >= CNAS_HSD_MAX_LOG_HRPD_SYS_NUM)
        {
            break;
        }

        pstMsg->astSysRecList[i] = pstSysRecList->pstHrpdSysItem[i];
    }

    pstMsg->stMsgHeader.ulLength        = PS_OFFSET_OF(CNAS_HSD_LOG_HRPD_SYS_LIST_MSG_STRU, astSysRecList)
                                          - VOS_MSG_HEAD_LENGTH
                                          + (pstMsg->usSysNum* sizeof(CNAS_HSD_HRPD_SYS_REC_ITEM_STRU));

    CNAS_MNTN_TraceIntMsg(UEPS_PID_HSD, sizeof(CNAS_HSD_LOG_HRPD_SYS_LIST_MSG_STRU),(VOS_VOID *)pstMsg);

    PS_MEM_FREE(UEPS_PID_HSD, pstMsg);

    return;
}


VOS_VOID CNAS_HSD_LogMruList(
    CNAS_HSD_HRPD_MRU_LIST_STRU                  *pstMruList,
    CNAS_HSD_MNTN_MRU_LOG_TYPE_ENUM_UINT8         enMruLogType
)
{
    VOS_UINT32                                    i;
    CNAS_HSD_LOG_MRU_LIST_MSG_STRU               *pstMsg = VOS_NULL_PTR;

    pstMsg = (CNAS_HSD_LOG_MRU_LIST_MSG_STRU *)PS_MEM_ALLOC(UEPS_PID_HSD,
                          (sizeof(CNAS_HSD_LOG_MRU_LIST_MSG_STRU)));

    if (VOS_NULL_PTR == pstMsg)
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_LogMruList:Alloc Memory Failed");

        return;
    }

    pstMsg->stMsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderPid     = UEPS_PID_HSD;
    pstMsg->stMsgHeader.ulReceiverPid   = UEPS_PID_HSD;
    pstMsg->stMsgHeader.ulMsgName       = ID_CNAS_HSD_MNTN_LOG_MRU_LIST_IND;

    pstMsg->enMruLogType                = enMruLogType;

    pstMsg->ucSysNum    = pstMruList->ucCurrMruNum;

    for (i = 0; i < CNAS_MIN(pstMruList->ucCurrMruNum, CNAS_HSD_MAX_MRU_SYS_NUM); i++)
    {
        pstMsg->astSystem[i] = pstMruList->astHrpdSys[i];
    }

    pstMsg->stMsgHeader.ulLength        = PS_OFFSET_OF(CNAS_HSD_LOG_MRU_LIST_MSG_STRU, astSystem)
                                          - VOS_MSG_HEAD_LENGTH
                                          + (pstMsg->ucSysNum * sizeof(CNAS_PRL_HRPD_SYSTEM_STRU));

    CNAS_MNTN_TraceIntMsg(UEPS_PID_HSD, sizeof(CNAS_HSD_LOG_MRU_LIST_MSG_STRU),(VOS_VOID *)pstMsg);

    PS_MEM_FREE(UEPS_PID_HSD, pstMsg);

    return;
}



VOS_VOID CNAS_HSD_LogOocScheduleInfo(
    CNAS_NVIM_HRPD_OOC_TIMER_SCHEDULE_INFO_STRU            *pstOocTimerScheduleInfo
)
{
    VOS_UINT8                                               i;
    CNAS_HSD_LOG_OOC_TIMER_SCHEDULE_INFO_MSG_STRU          *pstMsg               = VOS_NULL_PTR;
    pstMsg = (CNAS_HSD_LOG_OOC_TIMER_SCHEDULE_INFO_MSG_STRU *)PS_MEM_ALLOC(UEPS_PID_HSD,
                          sizeof(CNAS_HSD_LOG_OOC_TIMER_SCHEDULE_INFO_MSG_STRU));

    if (VOS_NULL_PTR == pstMsg)
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_LogOocScheduleInfo:Alloc Memory Failed");

        return;
    }

    NAS_MEM_SET_S(pstMsg, sizeof(CNAS_HSD_LOG_OOC_TIMER_SCHEDULE_INFO_MSG_STRU), 0, sizeof(CNAS_HSD_LOG_OOC_TIMER_SCHEDULE_INFO_MSG_STRU));

    pstMsg->stMsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderPid     = UEPS_PID_HSD;
    pstMsg->stMsgHeader.ulReceiverPid   = UEPS_PID_HSD;
    pstMsg->stMsgHeader.ulMsgName       = ID_CNAS_HSD_MNTN_LOG_OOC_TIMER_SCHEDULE_IND;
    pstMsg->stMsgHeader.ulLength        = sizeof(CNAS_HSD_LOG_OOC_TIMER_SCHEDULE_INFO_MSG_STRU) - VOS_MSG_HEAD_LENGTH;

    pstMsg->ucMru0SearchTimerLen        = pstOocTimerScheduleInfo->ucMru0SearchTimerLen;
    pstMsg->ucPhaseNum                  = pstOocTimerScheduleInfo->ucPhaseNum;

    for (i = 0; i < pstOocTimerScheduleInfo->ucPhaseNum; i++)
    {
        pstMsg->astOocTimerInfo[i].usTimes    = pstOocTimerScheduleInfo->astOocTimerInfo[i].usTimes;
        pstMsg->astOocTimerInfo[i].usTimerLen = pstOocTimerScheduleInfo->astOocTimerInfo[i].usTimerLen;
    }

    CNAS_MNTN_TraceIntMsg(UEPS_PID_HSD, sizeof(CNAS_HSD_LOG_OOC_TIMER_SCHEDULE_INFO_MSG_STRU), (VOS_VOID *)pstMsg);

    PS_MEM_FREE(UEPS_PID_HSD, pstMsg);

    return;
}


VOS_VOID CNAS_HSD_LogOocCtxInfo(
    CNAS_HSD_OOC_CTX_INFO_STRU         *pstOocCtxInfo
)
{
    CNAS_HSD_LOG_OOC_CTX_INFO_MSG_STRU                     *pstMsg = VOS_NULL_PTR;

    pstMsg = (CNAS_HSD_LOG_OOC_CTX_INFO_MSG_STRU *)PS_MEM_ALLOC(UEPS_PID_HSD,
                                                                sizeof(CNAS_HSD_LOG_OOC_CTX_INFO_MSG_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_LogOocCtxInfo: Alloc Memory Failed!");

        return;
    }

    NAS_MEM_SET_S(pstMsg, sizeof(CNAS_HSD_LOG_OOC_CTX_INFO_MSG_STRU), 0, sizeof(CNAS_HSD_LOG_OOC_CTX_INFO_MSG_STRU));

    /* 更新消息头信息 */
    pstMsg->stMsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderPid     = UEPS_PID_HSD;
    pstMsg->stMsgHeader.ulReceiverPid   = UEPS_PID_HSD;
    pstMsg->stMsgHeader.ulMsgName       = ID_CNAS_HSD_MNTN_LOG_OOC_CTX_INFO_IND;
    pstMsg->stMsgHeader.ulLength        = sizeof(CNAS_HSD_LOG_OOC_CTX_INFO_MSG_STRU) - VOS_MSG_HEAD_LENGTH;

    /* 设置消息体内容 */
    pstMsg->ucWaitSearchFlag            = pstOocCtxInfo->ucWaitSearchFlag;
    pstMsg->ucCurrentPhase              = pstOocCtxInfo->ucCurrentPhase;
    pstMsg->usCurrentTimes              = pstOocCtxInfo->usCurrentTimes;
    pstMsg->ucSceneSetFlag              = pstOocCtxInfo->ucSceneSetFlag;

    CNAS_MNTN_TraceIntMsg(UEPS_PID_HSD, sizeof(CNAS_HSD_LOG_OOC_CTX_INFO_MSG_STRU), (VOS_VOID *)pstMsg);

    PS_MEM_FREE(UEPS_PID_HSD, pstMsg);

    return;
}



VOS_VOID CNAS_HSD_LogAvoidFreqList(
    CNAS_HSD_AVOID_FREQ_LIST_STRU      *pstAvoidFreqList
)
{
    VOS_UINT8                                               i;
    CNAS_HSD_LOG_AVOID_FREQ_LIST_MSG_STRU                  *pstMsg = VOS_NULL_PTR;

    pstMsg = (CNAS_HSD_LOG_AVOID_FREQ_LIST_MSG_STRU *)PS_MEM_ALLOC(UEPS_PID_HSD,
                                                                   sizeof(CNAS_HSD_LOG_AVOID_FREQ_LIST_MSG_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_LogAvoidFreqList: Alloc Memory Failed!");

        return;
    }

    NAS_MEM_SET_S(pstMsg, sizeof(CNAS_HSD_LOG_AVOID_FREQ_LIST_MSG_STRU), 0, sizeof(CNAS_HSD_LOG_AVOID_FREQ_LIST_MSG_STRU));

    pstMsg->stMsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderPid     = UEPS_PID_HSD;
    pstMsg->stMsgHeader.ulReceiverPid   = UEPS_PID_HSD;
    pstMsg->stMsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulMsgName       = ID_CNAS_HSD_MNTN_LOG_AVOID_FREQ_LIST_IND;
    pstMsg->stMsgHeader.ulLength        = sizeof(CNAS_HSD_LOG_AVOID_FREQ_LIST_MSG_STRU) - VOS_MSG_HEAD_LENGTH;

    pstMsg->ucAvoidItemUsedNum          = pstAvoidFreqList->ucAvoidItemUsedNum;

    for (i  = 0; i < CNAS_HSD_AVOID_REASON_MAX; i++)
    {
        pstMsg->aucCounter[i]           = pstAvoidFreqList->aucCounter[i];
    }

    for (i = 0; i < CNAS_HSD_MAX_AVOID_FREQ_NUM; i++)
    {
        pstMsg->astAvoidFreqInfo[i].enUsedFlag        = pstAvoidFreqList->astAvoidFreqInfo[i].enUsedFlag;
        pstMsg->astAvoidFreqInfo[i].stAvoidFreq       = pstAvoidFreqList->astAvoidFreqInfo[i].stAvoidFreq;
        pstMsg->astAvoidFreqInfo[i].ulStartSlice      = pstAvoidFreqList->astAvoidFreqInfo[i].ulStartSlice;
        pstMsg->astAvoidFreqInfo[i].ulExpiredSliceNum = pstAvoidFreqList->astAvoidFreqInfo[i].ulExpiredSliceNum;
    }

    CNAS_MNTN_TraceIntMsg(UEPS_PID_HSD, sizeof(CNAS_HSD_LOG_AVOID_FREQ_LIST_MSG_STRU), (VOS_VOID *)pstMsg);

    PS_MEM_FREE(UEPS_PID_HSD, pstMsg);

    return;
}


VOS_VOID CNAS_HSD_LogRedirectionFreqList(
    CNAS_HSD_REDIRECTION_STRU           *pstRedirection
)
{
    CNAS_HSD_LOG_REDIRCT_MSG_STRU      *pstMsg;

    pstMsg = (CNAS_HSD_LOG_REDIRCT_MSG_STRU *)PS_MEM_ALLOC(UEPS_PID_HSD,
                          (sizeof(CNAS_HSD_LOG_REDIRCT_MSG_STRU)));

    if (VOS_NULL_PTR == pstMsg)
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_LogRedirectionFreqList:Alloc Memory Failed");

        return;
    }

    pstMsg->ulSenderCpuId           = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverCpuId         = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid             = UEPS_PID_HSD;
    pstMsg->ulReceiverPid           = UEPS_PID_HSD;
    pstMsg->enMsgId                 = ID_CNAS_HSD_MNTN_LOG_REDIRECT_FREQ_LIST_IND;

    pstMsg->enNetworkLostReason     = CAS_CNAS_HRPD_NETWORK_LOST_REDIRECT;

    pstMsg->ucChanNum               = pstRedirection->ucChanNum;

    NAS_MEM_CPY_S(pstMsg->aulChannel,
                  sizeof(CAS_CNAS_HRPD_DERECTION_CHANNEL_STRU) * CAS_CNAS_HRPD_MAX_REDIRECT_CH_NUM,
                  pstRedirection->aulChannel,
                  sizeof(CAS_CNAS_HRPD_DERECTION_CHANNEL_STRU) * pstMsg->ucChanNum);

    pstMsg->ulLength        = PS_OFFSET_OF(CNAS_HSD_LOG_REDIRCT_MSG_STRU, aulChannel)
                                           - VOS_MSG_HEAD_LENGTH
                                           + (pstMsg->ucChanNum * sizeof(CAS_CNAS_HRPD_DERECTION_CHANNEL_STRU));

    CNAS_MNTN_TraceIntMsg(UEPS_PID_HSD, sizeof(CNAS_HSD_LOG_REDIRCT_MSG_STRU),(VOS_VOID *)pstMsg);

    PS_MEM_FREE(UEPS_PID_HSD, pstMsg);

    return;
}


VOS_VOID CNAS_HSD_LogOperLockSysWhiteList(
    CNAS_CCB_OPER_LOCK_SYS_WHITE_LIST_STRU                 *pstMsg
)
{
    CNAS_HSD_LOG_OPER_LOCK_SYS_WHITE_LIST_STRU             *pstOperLockSysWhiteList = VOS_NULL_PTR;
    VOS_UINT32                                              i;

    pstOperLockSysWhiteList = (CNAS_HSD_LOG_OPER_LOCK_SYS_WHITE_LIST_STRU *)PS_MEM_ALLOC(UEPS_PID_HSD,
                                                                sizeof(CNAS_HSD_LOG_OPER_LOCK_SYS_WHITE_LIST_STRU));
    if (VOS_NULL_PTR == pstOperLockSysWhiteList)
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_LogOperLockSysWhiteList: Alloc Memory Failed!");

        return;
    }

    NAS_MEM_SET_S(pstOperLockSysWhiteList, sizeof(CNAS_HSD_LOG_OPER_LOCK_SYS_WHITE_LIST_STRU), 0, sizeof(CNAS_HSD_LOG_OPER_LOCK_SYS_WHITE_LIST_STRU));

    /* 更新消息头信息 */
    pstOperLockSysWhiteList->stMsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstOperLockSysWhiteList->stMsgHeader.ulSenderPid     = UEPS_PID_HSD;
    pstOperLockSysWhiteList->stMsgHeader.ulReceiverPid   = UEPS_PID_HSD;
    pstOperLockSysWhiteList->stMsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstOperLockSysWhiteList->stMsgHeader.ulMsgName       = ID_CNAS_HSD_MNTN_LOG_OPER_LOCK_WHITE_SID_LIST_IND;
    pstOperLockSysWhiteList->stMsgHeader.ulLength        = sizeof(CNAS_HSD_LOG_OPER_LOCK_SYS_WHITE_LIST_STRU) - VOS_MSG_HEAD_LENGTH;

    /* 设置消息体内容 */
    pstOperLockSysWhiteList->ucEnable      = pstMsg->ucEnable;

    pstOperLockSysWhiteList->usWhiteSysNum = CNAS_MIN(pstMsg->usWhiteSysNum, CNAS_CCB_MAX_WHITE_LOCK_SYS_NUM);

    for (i = 0; i < pstOperLockSysWhiteList->usWhiteSysNum;i++)
    {
        pstOperLockSysWhiteList->astSysInfo[i].usStartSid = pstMsg->astSysInfo[i].usStartSid;
        pstOperLockSysWhiteList->astSysInfo[i].usEndSid   = pstMsg->astSysInfo[i].usEndSid;
        pstOperLockSysWhiteList->astSysInfo[i].ulMcc      = pstMsg->astSysInfo[i].ulMcc;
    }

    CNAS_MNTN_TraceIntMsg(UEPS_PID_HSD, sizeof(CNAS_HSD_LOG_OPER_LOCK_SYS_WHITE_LIST_STRU), (VOS_VOID *)pstOperLockSysWhiteList);

    PS_MEM_FREE(UEPS_PID_HSD, pstOperLockSysWhiteList);

    return;
}


VOS_VOID CNAS_HSD_LogCTCCCustomizeFreqList(
    CNAS_CCB_CTCC_CUSTOMIZE_FREQ_LIST_STRU                 *pstMsg
)
{
    CNAS_HSD_LOG_CTCC_CUSTOMIZE_FREQ_LIST_STRU             *pstCustFreqList = VOS_NULL_PTR;
    VOS_UINT32                                              i;

    pstCustFreqList = (CNAS_HSD_LOG_CTCC_CUSTOMIZE_FREQ_LIST_STRU *)PS_MEM_ALLOC(UEPS_PID_HSD,
                                                                sizeof(CNAS_HSD_LOG_CTCC_CUSTOMIZE_FREQ_LIST_STRU));
    if (VOS_NULL_PTR == pstCustFreqList)
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_LogCTCCCustomizeFreqList: Alloc Memory Failed!");

        return;
    }

    NAS_MEM_SET_S(pstCustFreqList, sizeof(CNAS_HSD_LOG_CTCC_CUSTOMIZE_FREQ_LIST_STRU), 0, sizeof(CNAS_HSD_LOG_CTCC_CUSTOMIZE_FREQ_LIST_STRU));

    /* 更新消息头信息 */
    pstCustFreqList->stMsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstCustFreqList->stMsgHeader.ulSenderPid     = UEPS_PID_HSD;
    pstCustFreqList->stMsgHeader.ulReceiverPid   = UEPS_PID_HSD;
    pstCustFreqList->stMsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstCustFreqList->stMsgHeader.ulMsgName       = ID_CNAS_HSD_MNTN_LOG_CTCC_CUSTOMIZE_FREQ_LIST_IND;
    pstCustFreqList->stMsgHeader.ulLength        = sizeof(CNAS_HSD_LOG_CTCC_CUSTOMIZE_FREQ_LIST_STRU) - VOS_MSG_HEAD_LENGTH;

    /* 设置消息体内容 */
    pstCustFreqList->ucEnableFlg = pstMsg->ucEnableFlg;
    pstCustFreqList->usFreqNum   = CNAS_MIN(pstMsg->usFreqNum, CNAS_CCB_MAX_FREQ_NUM);

    for (i = 0; i < pstCustFreqList->usFreqNum;i++)
    {
        pstCustFreqList->astFreqList[i].usChannel   = pstMsg->astFreqList[i].usChannel;
    }

    CNAS_MNTN_TraceIntMsg(UEPS_PID_HSD, sizeof(CNAS_HSD_LOG_CTCC_CUSTOMIZE_FREQ_LIST_STRU), (VOS_VOID *)pstCustFreqList);

    PS_MEM_FREE(UEPS_PID_HSD, pstCustFreqList);

    return;
}


VOS_VOID CNAS_HSD_LogPrlSourceType(
    CNAS_HSD_PRL_SOURCE_TYPE_ENUM_UINT8 enPrlSrcType
)
{
    CNAS_HSD_LOG_PRL_SOURCE_TYPE_MSG_STRU                  *pstMsg = VOS_NULL_PTR;

    pstMsg = (CNAS_HSD_LOG_PRL_SOURCE_TYPE_MSG_STRU *)PS_MEM_ALLOC(UEPS_PID_HSD,
                                                                   sizeof(CNAS_HSD_LOG_PRL_SOURCE_TYPE_MSG_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_LogPrlSourceType: Alloc Memory Failed!");

        return;
    }

    NAS_MEM_SET_S(pstMsg, sizeof(CNAS_HSD_LOG_PRL_SOURCE_TYPE_MSG_STRU), 0, sizeof(CNAS_HSD_LOG_PRL_SOURCE_TYPE_MSG_STRU));

    pstMsg->stMsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderPid     = UEPS_PID_HSD;
    pstMsg->stMsgHeader.ulReceiverPid   = UEPS_PID_HSD;
    pstMsg->stMsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulMsgName       = ID_CNAS_HSD_MNTN_LOG_PRL_SOURCE_TYPE_IND;
    pstMsg->stMsgHeader.ulLength        = sizeof(CNAS_HSD_LOG_PRL_SOURCE_TYPE_MSG_STRU) - VOS_MSG_HEAD_LENGTH;

    pstMsg->enPrlSrcType                = enPrlSrcType;

    CNAS_MNTN_TraceIntMsg(UEPS_PID_HSD, sizeof(CNAS_HSD_LOG_PRL_SOURCE_TYPE_MSG_STRU), (VOS_VOID *)pstMsg);

    PS_MEM_FREE(UEPS_PID_HSD, pstMsg);

    return;
}


VOS_VOID  CNAS_HSD_LogDrxTimerStatus(
    VOS_UINT8                           ucDrxTimerStatus
)
{
    CNAS_HSD_LOG_DRX_TIMER_STATUS_IND_STRU                  *pstMsg = VOS_NULL_PTR;

    pstMsg = (CNAS_HSD_LOG_DRX_TIMER_STATUS_IND_STRU *)PS_MEM_ALLOC(UEPS_PID_HSD,
                                                                   sizeof(CNAS_HSD_LOG_DRX_TIMER_STATUS_IND_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        CNAS_ERROR_LOG(UEPS_PID_HSD, "CNAS_HSD_LogDrxTimerStatus: Alloc Memory Failed!");

        return;
    }

    NAS_MEM_SET_S(pstMsg, sizeof(CNAS_HSD_LOG_DRX_TIMER_STATUS_IND_STRU), 0, sizeof(CNAS_HSD_LOG_DRX_TIMER_STATUS_IND_STRU));

    pstMsg->stMsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderPid     = UEPS_PID_HSD;
    pstMsg->stMsgHeader.ulReceiverPid   = UEPS_PID_HSD;
    pstMsg->stMsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulMsgName       = ID_CNAS_HSD_MNTN_LOG_DRX_TIMER_STATUS_IND;
    pstMsg->stMsgHeader.ulLength        = sizeof(CNAS_HSD_LOG_DRX_TIMER_STATUS_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    pstMsg->ucDrxTimerStatus            = ucDrxTimerStatus;

    CNAS_MNTN_TraceIntMsg(UEPS_PID_HSD, sizeof(CNAS_HSD_LOG_DRX_TIMER_STATUS_IND_STRU), (VOS_VOID *)pstMsg);

    PS_MEM_FREE(UEPS_PID_HSD, pstMsg);

    return;
}


VOS_UINT32 CNAS_HSD_MNTN_IsNeedRecordForRcvMsg(
    VOS_UINT32                          ulSenderPid,
    VOS_UINT32                          ulReceiverPid,
    VOS_UINT32                          ulMsgName
)
{
    VOS_UINT32                          i;

    switch (ulSenderPid)
    {
        case UEPS_PID_MSCC:
        case I1_UEPS_PID_MSCC:
            for (i = 0; i < (sizeof(g_stCnasHsdMntnFromMsccMsgTbl)/sizeof(CNAS_HSD_MNTN_WHITE_LIST_MSG_STRU)); i++)
            {
                if (ulMsgName == g_stCnasHsdMntnFromMsccMsgTbl[i].ulMsgName)
                {
                    return VOS_TRUE;
                }
            }
            break;

        case UEPS_PID_HALMP:
            for (i = 0; i < (sizeof(g_stCnasHsdMntnFromCasMsgTbl)/sizeof(CNAS_HSD_MNTN_WHITE_LIST_MSG_STRU)); i++)
            {
                if (ulMsgName == g_stCnasHsdMntnFromCasMsgTbl[i].ulMsgName)
                {
                    return VOS_TRUE;
                }
            }
            break;

        case VOS_PID_TIMER:
            for (i = 0; i < (sizeof(g_stCnasHsdMntnFromTimerMsgTbl)/sizeof(CNAS_HSD_MNTN_WHITE_LIST_MSG_STRU)); i++)
            {
                if (ulMsgName == g_stCnasHsdMntnFromTimerMsgTbl[i].ulMsgName)
                {
                    return VOS_TRUE;
                }
            }
            break;

         default:
            break;
    }

    return VOS_FALSE;
}


VOS_UINT32 CNAS_HSD_MNTN_IsNeedRecordForSndMsg(
    VOS_UINT32                          ulSenderPid,
    VOS_UINT32                          ulReceiverPid,
    VOS_UINT32                          ulMsgName
)
{
    VOS_UINT32                          i;

    switch (ulReceiverPid)
    {
        case UEPS_PID_MSCC:
        case I1_UEPS_PID_MSCC:
            for (i = 0; i < (sizeof(g_stCnasHsdMntnToMsccMsgTbl)/sizeof(CNAS_HSD_MNTN_WHITE_LIST_MSG_STRU)); i++)
            {
                if (ulMsgName == g_stCnasHsdMntnToMsccMsgTbl[i].ulMsgName)
                {
                    return VOS_TRUE;
                }
            }
            break;

        case UEPS_PID_HALMP:
            for (i = 0; i < (sizeof(g_stCnasHsdMntnToCasMsgTbl)/sizeof(CNAS_HSD_MNTN_WHITE_LIST_MSG_STRU)); i++)
            {
                if (ulMsgName == g_stCnasHsdMntnToCasMsgTbl[i].ulMsgName)
                {
                    return VOS_TRUE;
                }
            }
            break;

         default:
            break;
    }

    return VOS_FALSE;
}


CNAS_HSD_LOG_MSG_STATUS_STRU* CNAS_HSD_GetMntnMsgLog(VOS_VOID)
{
    return  &g_stCnasHsdLogMsg;
}


VOS_VOID CNAS_HSD_AddLogMsgState(
    VOS_UINT32                          ulSendPid,
    VOS_UINT32                          ulReceivePid,
    VOS_UINT16                          usMsgName
)
{
    CNAS_HSD_LOG_MSG_STATUS_STRU       *pstLogMsgState;
    VOS_UINT32                          ulIndex;

    pstLogMsgState = CNAS_HSD_GetMntnMsgLog();

    if (pstLogMsgState->ulLatestIndex >= CNAS_HSD_MAX_LOG_MSG_STATE_NUM)
    {
        pstLogMsgState->ulLatestIndex = 0;
    }

    ulIndex = pstLogMsgState->ulLatestIndex;

    NAS_MEM_SET_S(&(pstLogMsgState->stMsgState[ulIndex]), sizeof(CNAS_HSD_MSG_STATE_STRU), 0, sizeof(CNAS_HSD_MSG_STATE_STRU));

    pstLogMsgState->stMsgState[ulIndex].ulReceiveTime   = VOS_GetTick();
    pstLogMsgState->stMsgState[ulIndex].usSendPid       = (VOS_UINT16)ulSendPid;
    pstLogMsgState->stMsgState[ulIndex].usReceivePid    = (VOS_UINT16)ulReceivePid;
    pstLogMsgState->stMsgState[ulIndex].ucHsdFsmId      = (VOS_UINT8)CNAS_HSD_GetCurrFsmId();
    pstLogMsgState->stMsgState[ulIndex].ucHsdState      = (VOS_UINT8)CNAS_HSD_GetFsmTopState();
    pstLogMsgState->stMsgState[ulIndex].usMsgName       = usMsgName;

    pstLogMsgState->ulLatestIndex = ulIndex + 1;

    return;
}


VOS_VOID CNAS_HSD_LogMsgInfo(
    MSG_HEADER_STRU                    *pstMsgHeader
)
{
    VOS_UINT32                          ulSenderPid;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulMsgName;
    VOS_UINT32                          ulRcvRet;
    VOS_UINT32                          ulSndRet;

    /* 判断发送id是否是定时器 */
    if (VOS_PID_TIMER == pstMsgHeader->ulSenderPid)
    {
        ulSenderPid      =    ((REL_TIMER_MSG*)pstMsgHeader)->ulSenderPid;
        ulReceiverPid    =    ((REL_TIMER_MSG*)pstMsgHeader)->ulReceiverPid;
        ulMsgName        =    ((REL_TIMER_MSG*)pstMsgHeader)->ulName;
    }
    else
    {
        ulSenderPid      =    pstMsgHeader->ulSenderPid;
        ulReceiverPid    =    pstMsgHeader->ulReceiverPid;

        /* 目前消息都是只用到2个字节，只取2字节内容 */
        ulMsgName        =    (pstMsgHeader->ulMsgName & 0xFFFF);
    }


    ulRcvRet = CNAS_HSD_MNTN_IsNeedRecordForRcvMsg(ulSenderPid,
                                                   ulReceiverPid,
                                                   ulMsgName);

    ulSndRet = CNAS_HSD_MNTN_IsNeedRecordForSndMsg(ulSenderPid,
                                                   ulReceiverPid,
                                                   ulMsgName);

    if ((VOS_FALSE == ulRcvRet)
     && (VOS_FALSE == ulSndRet))
    {
        return;
    }

    /* 打印消息 */
    CNAS_HSD_AddLogMsgState(ulSenderPid, ulReceiverPid, (VOS_UINT16)ulMsgName);

    return;

}

/*lint -restore*/
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */




