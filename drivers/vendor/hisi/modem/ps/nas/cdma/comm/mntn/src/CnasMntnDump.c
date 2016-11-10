

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "CnasMntnDump.h"
#include "mdrv.h"

#include "msp_diag_comm.h"
#include "NasOmTrans.h"

#include "Nas_Mem.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_CNAS_MNTN_DUMP_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

VOS_UINT8                              *gpucCnasExcAddr = VOS_NULL_PTR;

/*****************************************************************************
  3 函数定义
*****************************************************************************/
/*lint -save -e958*/


VOS_VOID CNAS_MNTN_SaveCnasLogMsgState(
    CNAS_MNTN_SAVE_EXC_LOG_STRU        *pstCnasMntnSaveExcLog
)
{
    CNAS_CCB_LOG_MSG_STATUS_STRU       *pstCnasLogMsgState = VOS_NULL_PTR;

    pstCnasLogMsgState = &(CNAS_CCB_GetCcbCtxAddr()->stMntnInfo.stLogMsgState);

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasLogMsgState),
                  sizeof(CNAS_CCB_LOG_MSG_STATUS_STRU),
                  pstCnasLogMsgState,
                  sizeof(CNAS_CCB_LOG_MSG_STATUS_STRU));

    return;
}


VOS_VOID CNAS_MNTN_SaveCnasCcbGlobalInfo(
    CNAS_MNTN_SAVE_EXC_LOG_STRU        *pstCnasMntnSaveExcLog
)
{
    pstCnasMntnSaveExcLog->stCnasCcbMntnSaveExcLog.ucIsMtCallInRoamingAcc  = CNAS_CCB_GetCcbCtxAddr()->stCallInfoCfg.ucIsMtCallInRoamingAcc;

    pstCnasMntnSaveExcLog->stCnasCcbMntnSaveExcLog.ucPowerOffCampOnCtrlFlg = CNAS_CCB_GetCcbCtxAddr()->stCustomCfg.ucPowerOffCampOnCtrlFlg;

    pstCnasMntnSaveExcLog->stCnasCcbMntnSaveExcLog.ucHrpdNetWorkSrchingFlg = CNAS_CCB_GetCcbCtxAddr()->stHrpdNetWorkSrchInfo.ucHrpdNetWorkSrchingFlg;

    pstCnasMntnSaveExcLog->stCnasCcbMntnSaveExcLog.enPrlSrcType            = CNAS_CCB_GetCcbCtxAddr()->enPrlSrcType;

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasCcbMntnSaveExcLog.stNwInfo),
                  sizeof(CNAS_CCB_NW_INFO_STRU),
                  &(CNAS_CCB_GetCcbCtxAddr()->stNwInfo),
                  sizeof(CNAS_CCB_NW_INFO_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasCcbMntnSaveExcLog.stHomeSidNidList),
                  sizeof(CNAS_CCB_1X_HOME_SID_NID_LIST_STRU),
                  &(CNAS_CCB_GetCcbCtxAddr()->stHomeSidNidList),
                  sizeof(CNAS_CCB_1X_HOME_SID_NID_LIST_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasCcbMntnSaveExcLog.stModemInfo),
                  sizeof(CNAS_CCB_MODEM_INFO_STRU),
                  &(CNAS_CCB_GetCcbCtxAddr()->stModemInfo),
                  sizeof(CNAS_CCB_MODEM_INFO_STRU));

    pstCnasMntnSaveExcLog->stCnasCcbMntnSaveExcLog.enCurrPsRatType = CNAS_CCB_GetCcbCtxAddr()->enCurrPsRatType;

    pstCnasMntnSaveExcLog->stCnasCcbMntnSaveExcLog.enPrevPsRatType = CNAS_CCB_GetCcbCtxAddr()->enPrevPsRatType;

    NAS_MEM_CPY_S(pstCnasMntnSaveExcLog->stCnasCcbMntnSaveExcLog.ast1xCallState,
                  CNAS_CCB_1X_CALL_STATE_MAX_NUM * sizeof(CNAS_CCB_1X_CALL_STATE_STRU),
                  CNAS_CCB_GetCcbCtxAddr()->ast1xCallState,
                  CNAS_CCB_1X_CALL_STATE_MAX_NUM * sizeof(CNAS_CCB_1X_CALL_STATE_STRU));

    pstCnasMntnSaveExcLog->stCnasCcbMntnSaveExcLog.en1xReturnCause    = CNAS_CCB_GetCcbCtxAddr()->en1xReturnCause;

    pstCnasMntnSaveExcLog->stCnasCcbMntnSaveExcLog.uc1XCallExistCount = CNAS_CCB_GetCcbCtxAddr()->uc1XCallExistCount;

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasCcbMntnSaveExcLog.stOperLockSysWhiteList),
                  sizeof(CNAS_CCB_OPER_LOCK_SYS_WHITE_LIST_STRU),
                  &(CNAS_CCB_GetCcbCtxAddr()->stOperLockSysWhiteList),
                  sizeof(CNAS_CCB_OPER_LOCK_SYS_WHITE_LIST_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasCcbMntnSaveExcLog.stCTCCCustInfo),
                  sizeof(CNAS_CCB_CTCC_CUSTOMIZE_INFO_STRU),
                  &(CNAS_CCB_GetCcbCtxAddr()->stCTCCCustInfo),
                  sizeof(CNAS_CCB_CTCC_CUSTOMIZE_INFO_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasCcbMntnSaveExcLog.stCdmaStandardChan),
                sizeof(CNAS_CCB_CDMA_STANDARD_CHANNELS_STRU),
               &(CNAS_CCB_GetCcbCtxAddr()->stCdmaStandardChan),
               sizeof(CNAS_CCB_CDMA_STANDARD_CHANNELS_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasCcbMntnSaveExcLog.stOhmFreq),
                  sizeof(CNAS_PRL_FREQENCY_CHANNEL_STRU),
                  &(CNAS_CCB_GetCcbCtxAddr()->stOhmFreq),
                  sizeof(CNAS_PRL_FREQENCY_CHANNEL_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasCcbMntnSaveExcLog.stCardInfo),
                  sizeof(CNAS_CCB_CARD_INFO_STRU),
                  &(CNAS_CCB_GetCcbCtxAddr()->stCardInfo),
                  sizeof(CNAS_CCB_CARD_INFO_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasCcbMntnSaveExcLog.st1xSrvInfo),
                  sizeof(CNAS_CCB_1X_SRV_INFO_STRU),
                  &(CNAS_CCB_GetCcbCtxAddr()->st1XSrvInfo),
                  sizeof(CNAS_CCB_1X_SRV_INFO_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasCcbMntnSaveExcLog.stMsSysCfg),
                  sizeof(CNAS_CCB_MS_SYS_CFG_INFO_STRU),
                  &(CNAS_CCB_GetCcbCtxAddr()->stMsSysCfgInfo),
                  sizeof(CNAS_CCB_MS_SYS_CFG_INFO_STRU));
}


VOS_VOID CNAS_MNTN_SaveCnasXsdGlobalInfo(
    CNAS_MNTN_SAVE_EXC_LOG_STRU        *pstCnasMntnSaveExcLog
)
{
    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasXsdMntnSaveExcLog.stRedirInfo),
                  sizeof(CNAS_XSD_REDIRECTION_INFO_STRU),
                  &(CNAS_XSD_GetXsdCtxAddr()->stSysAcqCtrl.stRedirInfo),
                  sizeof(CNAS_XSD_REDIRECTION_INFO_STRU));

    pstCnasMntnSaveExcLog->stCnasXsdMntnSaveExcLog.stMntnAvoidFreqList.ucAvoidFreqNum = CNAS_XSD_GetXsdCtxAddr()->stSysAcqCtrl.stAvoidFreqList.ucAvoidFreqNum;
    NAS_MEM_CPY_S(pstCnasMntnSaveExcLog->stCnasXsdMntnSaveExcLog.stMntnAvoidFreqList.astAvoidFreqInfo,
                  CNAS_MNTN_DUMP_XSD_REDIRECTION_INFO_CNT * sizeof(CNAS_XSD_AVOID_FREQ_INFO_STRU),
                  CNAS_XSD_GetXsdCtxAddr()->stSysAcqCtrl.stAvoidFreqList.astAvoidFreqInfo,
                  CNAS_MNTN_DUMP_XSD_REDIRECTION_INFO_CNT * sizeof(CNAS_XSD_AVOID_FREQ_INFO_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasXsdMntnSaveExcLog.stMruList),
                  sizeof(CNAS_XSD_MRU_LIST_STRU),
                  &(CNAS_XSD_GetXsdCtxAddr()->stSysAcqCtrl.stMruList),
                  sizeof(CNAS_XSD_MRU_LIST_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasXsdMntnSaveExcLog.stChanScanList),
                  sizeof(CNAS_XSD_SCAN_CHANNEL_LIST_STRU),
                  &(CNAS_XSD_GetXsdCtxAddr()->stSysAcqCtrl.stChanScanList),
                  sizeof(CNAS_XSD_SCAN_CHANNEL_LIST_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasXsdMntnSaveExcLog.stGeoListSrchInfo),
                  sizeof(CNAS_XSD_GEO_LIST_SRCH_INFO_STRU),
                  &(CNAS_XSD_GetXsdCtxAddr()->stSysAcqCtrl.stGeoListSrchInfo),
                  sizeof(CNAS_XSD_GEO_LIST_SRCH_INFO_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasXsdMntnSaveExcLog.stCurCampedSysInfo),
                  sizeof(CNAX_XSD_CAMPED_SYS_INFO_STRU),
                  &(CNAS_XSD_GetXsdCtxAddr()->stSysAcqCtrl.stCurCampedSysInfo),
                  sizeof(CNAX_XSD_CAMPED_SYS_INFO_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasXsdMntnSaveExcLog.stCallRedialInfo),
                  sizeof(CNAS_XSD_CALL_REDIAL_CTRL_INFO_STRU),
                  &(CNAS_XSD_GetXsdCtxAddr()->stSysAcqCtrl.stCallRedialInfo),
                  sizeof(CNAS_XSD_CALL_REDIAL_CTRL_INFO_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasXsdMntnSaveExcLog.stFreqLockPara),
                  sizeof(CNAS_XSD_CFREQ_LOCK_SET_PARA_STRU),
                  &(CNAS_XSD_GetXsdCtxAddr()->stSysAcqCtrl.stFreqLockPara),
                  sizeof(CNAS_XSD_CFREQ_LOCK_SET_PARA_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasXsdMntnSaveExcLog.stOocScheduleInfo),
                  sizeof(CNAS_XSD_OOC_SCHEDULE_INFO_STRU),
                  &(CNAS_XSD_GetXsdCtxAddr()->stSysAcqCtrl.stOocScheduleInfo),
                  sizeof(CNAS_XSD_OOC_SCHEDULE_INFO_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasXsdMntnSaveExcLog.stEmcCallInfo),
                  sizeof(CNAS_XSD_EMC_CALL_CTRL_INFO_STRU),
                  &(CNAS_XSD_GetXsdCtxAddr()->stSysAcqCtrl.stEmcCallInfo),
                  sizeof(CNAS_XSD_EMC_CALL_CTRL_INFO_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasXsdMntnSaveExcLog.stCdma1XCustomPrefChan),
                  sizeof(CNAS_XSD_CDMA_1X_CUSTOM_PREF_CHANNELS_STRU),
                  &(CNAS_XSD_GetXsdCtxAddr()->stSysAcqCtrl.stCdma1XCustomPrefChan),
                  sizeof(CNAS_XSD_CDMA_1X_CUSTOM_PREF_CHANNELS_STRU));

    pstCnasMntnSaveExcLog->stCnasXsdMntnSaveExcLog.ucSrvAcqFlg = CNAS_XSD_GetXsdCtxAddr()->stSysAcqCtrl.ucSrvAcqFlg;

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasXsdMntnSaveExcLog.stTestConfig),
                  sizeof(CNAS_XSD_TEST_CONFIG_STRU),
                  &(CNAS_XSD_GetXsdCtxAddr()->stTestConfig),
                  sizeof(CNAS_XSD_TEST_CONFIG_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasXsdMntnSaveExcLog.stSysAcqNvimCfg),
                  sizeof(CNAS_XSD_SYS_ACQ_NVIM_CFG_STRU),
                  &(CNAS_XSD_GetXsdCtxAddr()->stSysAcqNvimCfg),
                  sizeof(CNAS_XSD_SYS_ACQ_NVIM_CFG_STRU));
}


VOS_VOID CNAS_MNTN_SaveCnasXccGlobalInfo(
    CNAS_MNTN_SAVE_EXC_LOG_STRU        *pstCnasMntnSaveExcLog
)
{
    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasXccMntnSaveExcLog.stMainCtrlCtx),
                  sizeof(CNAS_XCC_MAIN_CTRL_CTX_STRU),
                  &(CNAS_XCC_GetCtxAddr()->stMainCtrlCtx),
                  sizeof(CNAS_XCC_MAIN_CTRL_CTX_STRU));

    NAS_MEM_CPY_S(pstCnasMntnSaveExcLog->stCnasXccMntnSaveExcLog.astSridInfo,
                  CNAS_MNTN_DUMP_XCC_MAX_SRID_CNT * sizeof(CNAS_XCC_SRID_INFO_STRU),
                  CNAS_XCC_GetCtxAddr()->astSridInfo,
                  CNAS_MNTN_DUMP_XCC_MAX_SRID_CNT * sizeof(CNAS_XCC_SRID_INFO_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasXccMntnSaveExcLog.stOrigCallIndexOrder),
                  sizeof(CNAS_XCC_RECORD_ORIG_CALL_INDEX_ORDER_STRU),
                  &(CNAS_XCC_GetCtxAddr()->stOrigCallIndexOrder),
                  sizeof(CNAS_XCC_RECORD_ORIG_CALL_INDEX_ORDER_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasXccMntnSaveExcLog.stCallNvimCfg),
                  sizeof(CNAS_XCC_CALL_NVIM_CFG_STRU),
                  &(CNAS_XCC_GetCtxAddr()->stCallNvimCfg),
                  sizeof(CNAS_XCC_CALL_NVIM_CFG_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasXccMntnSaveExcLog.stPagingRspSoCfg),
                  sizeof(CNAS_XCC_1X_PAGING_RSP_SO_CFG_STRU),
                  &(CNAS_XCC_GetCtxAddr()->stPagingRspSoCfg),
                  sizeof(CNAS_XCC_1X_PAGING_RSP_SO_CFG_STRU));

    pstCnasMntnSaveExcLog->stCnasXccMntnSaveExcLog.ucIsAlreadyNtfLmmCallStart = CNAS_XCC_GetCtxAddr()->ucIsAlreadyNtfLmmCallStart;

}


VOS_VOID CNAS_MNTN_SaveCnasHsmGlobalInfo(
    CNAS_MNTN_SAVE_EXC_LOG_STRU        *pstCnasMntnSaveExcLog
)
{
    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasHsmMntnSaveExcLog.stHrpdConnCtrlInfo),
                  sizeof(CNAS_HSM_HRPD_CONN_CTRL_STRU),
                  &(CNAS_HSM_GetHsmCtxAddr()->stHrpdConnCtrlInfo),
                  sizeof(CNAS_HSM_HRPD_CONN_CTRL_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasHsmMntnSaveExcLog.stSessionCtrlInfo),
                  sizeof(CNAS_HSM_SESSION_CTRL_STRU),
                  &(CNAS_HSM_GetHsmCtxAddr()->stSessionCtrlInfo),
                  sizeof(CNAS_HSM_SESSION_CTRL_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasHsmMntnSaveExcLog.stHrpdAmpNegAttibInfo),
                  sizeof(CNAS_HSM_HRPD_AMP_NEG_ATTRIB_STRU),
                  &(CNAS_HSM_GetHsmCtxAddr()->stHrpdAmpNegAttibInfo),
                  sizeof(CNAS_HSM_HRPD_AMP_NEG_ATTRIB_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasHsmMntnSaveExcLog.stKeepAliveCtrlCtx),
                  sizeof(CNAS_HSM_KEEP_ALIVE_CTRL_CTX_STRU),
                  &(CNAS_HSM_GetHsmCtxAddr()->stKeepAliveCtrlCtx),
                  sizeof(CNAS_HSM_KEEP_ALIVE_CTRL_CTX_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasHsmMntnSaveExcLog.stMultiModeCtrlInfo),
                  sizeof(CNAS_HSM_MULTI_MODE_CTRL_INFO_STRU),
                  &(CNAS_HSM_GetHsmCtxAddr()->stMultiModeCtrlInfo),
                  sizeof(CNAS_HSM_MULTI_MODE_CTRL_INFO_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasHsmMntnSaveExcLog.stSnpDataReqCtrlInfo),
                  sizeof(CNAS_HSM_SNP_DATA_REQ_OPID_CTRL_STRU),
                  &(CNAS_HSM_GetHsmCtxAddr()->stSnpDataReqCtrlInfo),
                  sizeof(CNAS_HSM_SNP_DATA_REQ_OPID_CTRL_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasHsmMntnSaveExcLog.stLowPowerCtrlInfo),
                  sizeof(CNAS_HSM_LOW_POWER_CTRL_STRU),
                  &(CNAS_HSM_GetHsmCtxAddr()->stLowPowerCtrlInfo),
                  sizeof(CNAS_HSM_LOW_POWER_CTRL_STRU));
}


VOS_VOID CNAS_MNTN_SaveCnasEHsmGlobalInfo(
    CNAS_MNTN_SAVE_EXC_LOG_STRU        *pstCnasMntnSaveExcLog
)
{
    CNAS_EHSM_MNTN_SAVE_EXC_LOCAL_PDN_INFO_STRU            *pstLoaclPdnInfo;
    CNAS_EHSM_MNTN_SAVE_EXC_LTE_PDN_INFO_STRU              *pstLtePdnInfo;
    VOS_UINT8                                               ucIndex;

    pstCnasMntnSaveExcLog->stCnasEHsmMntnSaveExcLog.enEhrpdState     = CNAS_EHSM_GetEhsmCtxAddr()->enEhrpdState;
    pstCnasMntnSaveExcLog->stCnasEHsmMntnSaveExcLog.enSessionType    = CNAS_EHSM_GetEhsmCtxAddr()->enSessionType;
    pstCnasMntnSaveExcLog->stCnasEHsmMntnSaveExcLog.ulWorkMode       = CNAS_EHSM_GetEhsmCtxAddr()->enWorkMode;
    pstCnasMntnSaveExcLog->stCnasEHsmMntnSaveExcLog.ulAirLinkExist   = CNAS_EHSM_GetEhsmCtxAddr()->ulAirLinkExist;
    pstCnasMntnSaveExcLog->stCnasEHsmMntnSaveExcLog.ulAutoAttachFlag = CNAS_EHSM_GetEhsmCtxAddr()->ulAutoAttachFlag;

    for (ucIndex = 0; ucIndex < CNAS_MNTN_DUMP_EHSM_PDN_CNT; ucIndex++)
    {
        pstLoaclPdnInfo = &(pstCnasMntnSaveExcLog->stCnasEHsmMntnSaveExcLog.astLocalPdnBearInfo[ucIndex]);
        pstLtePdnInfo   = &(pstCnasMntnSaveExcLog->stCnasEHsmMntnSaveExcLog.astLtePdnBearInfo[ucIndex]);

        NAS_MEM_SET_S(pstLoaclPdnInfo, sizeof(CNAS_EHSM_MNTN_SAVE_EXC_LOCAL_PDN_INFO_STRU), 0, sizeof(CNAS_EHSM_MNTN_SAVE_EXC_LOCAL_PDN_INFO_STRU));
        NAS_MEM_SET_S(pstLtePdnInfo, sizeof(CNAS_EHSM_MNTN_SAVE_EXC_LTE_PDN_INFO_STRU), 0, sizeof(CNAS_EHSM_MNTN_SAVE_EXC_LTE_PDN_INFO_STRU));

        pstLoaclPdnInfo->ucPdnId         = CNAS_EHSM_GetEhsmCtxAddr()->astLocalPdnBearInfo[ucIndex].ucPdnId;
        pstLoaclPdnInfo->ucInUsed        = CNAS_EHSM_GetEhsmCtxAddr()->astLocalPdnBearInfo[ucIndex].ucInUsed;
        pstLoaclPdnInfo->ucCid           = CNAS_EHSM_GetEhsmCtxAddr()->astLocalPdnBearInfo[ucIndex].ucCid;
        pstLoaclPdnInfo->ucIsPdnActive   = CNAS_EHSM_GetEhsmCtxAddr()->astLocalPdnBearInfo[ucIndex].ucIsPdnActive;
        pstLoaclPdnInfo->ulEpsbId        = CNAS_EHSM_GetEhsmCtxAddr()->astLocalPdnBearInfo[ucIndex].ulEpsbId;
        pstLoaclPdnInfo->ucApnLen        = CNAS_EHSM_GetEhsmCtxAddr()->astLocalPdnBearInfo[ucIndex].stApn.ucApnLen;
        pstLoaclPdnInfo->enAttachType    = CNAS_EHSM_GetEhsmCtxAddr()->astLocalPdnBearInfo[ucIndex].enAttachType;
        pstLoaclPdnInfo->enPdnType       = CNAS_EHSM_GetEhsmCtxAddr()->astLocalPdnBearInfo[ucIndex].enPdnType;
        pstLoaclPdnInfo->usMtu           = CNAS_EHSM_GetEhsmCtxAddr()->astLocalPdnBearInfo[ucIndex].usMtu;
        pstLoaclPdnInfo->enAuthType      = CNAS_EHSM_GetEhsmCtxAddr()->astLocalPdnBearInfo[ucIndex].enAuthType;
        pstLoaclPdnInfo->ucRetryTotalCnt = CNAS_EHSM_GetEhsmCtxAddr()->astLocalPdnBearInfo[ucIndex].ucRetryTotalCnt;

        NAS_MEM_CPY_S(&(pstLoaclPdnInfo->stPdnAddr),
                      sizeof(CNAS_EHSM_PDN_ADDR_STRU),
                      &(CNAS_EHSM_GetEhsmCtxAddr()->astLocalPdnBearInfo[ucIndex].stPdnAddr),
                      sizeof(CNAS_EHSM_PDN_ADDR_STRU));

        NAS_MEM_CPY_S(pstLoaclPdnInfo->aucApn,
                      sizeof(pstLoaclPdnInfo->aucApn),
                      CNAS_EHSM_GetEhsmCtxAddr()->astLocalPdnBearInfo[ucIndex].stApn.aucApn,
                      sizeof(pstLoaclPdnInfo->aucApn));

        pstLtePdnInfo->ucInUsed  = CNAS_EHSM_GetEhsmCtxAddr()->astLtePdnBearInfo[ucIndex].ucInUsed;
        pstLtePdnInfo->ucCid     = CNAS_EHSM_GetEhsmCtxAddr()->astLtePdnBearInfo[ucIndex].ucCid;
        pstLtePdnInfo->enPdnType = CNAS_EHSM_GetEhsmCtxAddr()->astLtePdnBearInfo[ucIndex].enPdnType;
        pstLtePdnInfo->ulEpsbId  = CNAS_EHSM_GetEhsmCtxAddr()->astLtePdnBearInfo[ucIndex].ulEpsbId;
        pstLtePdnInfo->ucApnLen  = CNAS_EHSM_GetEhsmCtxAddr()->astLtePdnBearInfo[ucIndex].stApn.ucApnLen;

        NAS_MEM_CPY_S(&(pstLtePdnInfo->stPdnAddr),
                      sizeof(CNAS_EHSM_PDN_ADDR_STRU),
                      &(CNAS_EHSM_GetEhsmCtxAddr()->astLtePdnBearInfo[ucIndex].stPdnAddr),
                      sizeof(CNAS_EHSM_PDN_ADDR_STRU));

        NAS_MEM_CPY_S(pstLtePdnInfo->aucApn,
                      sizeof(pstLtePdnInfo->aucApn),
                      CNAS_EHSM_GetEhsmCtxAddr()->astLtePdnBearInfo[ucIndex].stApn.aucApn,
                      sizeof(pstLtePdnInfo->aucApn));

    }

}


VOS_VOID CNAS_MNTN_SaveCnasHsdGlobalInfo(
    CNAS_MNTN_SAVE_EXC_LOG_STRU        *pstCnasMntnSaveExcLog
)
{
    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasHsdMntnSaveExcLog.stCnasHsdLogMsg),
                  sizeof(CNAS_HSD_LOG_MSG_STATUS_STRU),
                  CNAS_HSD_GetMntnMsgLog(),
                  sizeof(CNAS_HSD_LOG_MSG_STATUS_STRU));

    pstCnasMntnSaveExcLog->stCnasHsdMntnSaveExcLog.enModeType = CNAS_HSD_GetHsdCtxAddr()->stSysAcqCtrl.enModeType;

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasHsdMntnSaveExcLog.stMruList),
                  sizeof(CNAS_HSD_HRPD_MRU_LIST_STRU),
                  &(CNAS_HSD_GetHsdCtxAddr()->stSysAcqCtrl.stMruList),
                  sizeof(CNAS_HSD_HRPD_MRU_LIST_STRU));

    pstCnasMntnSaveExcLog->stCnasHsdMntnSaveExcLog.stAvoidFreqList.ucAvoidItemUsedNum = \
                                        CNAS_HSD_GetHsdCtxAddr()->stSysAcqCtrl.stAvoidFreqList.ucAvoidItemUsedNum;

    pstCnasMntnSaveExcLog->stCnasHsdMntnSaveExcLog.stAvoidFreqList.enCurrAvoidReason  = \
                                        CNAS_HSD_GetHsdCtxAddr()->stSysAcqCtrl.stAvoidFreqList.enCurrAvoidReason;

    NAS_MEM_CPY_S(pstCnasMntnSaveExcLog->stCnasHsdMntnSaveExcLog.stAvoidFreqList.astAvoidFreqInfo,
                  CNAS_MNTN_DUMP_HSD_AVOID_INFO_CNT * sizeof(CNAS_HSD_AVOID_FREQ_INFO_STRU),
                  CNAS_HSD_GetHsdCtxAddr()->stSysAcqCtrl.stAvoidFreqList.astAvoidFreqInfo,
                  CNAS_MNTN_DUMP_HSD_AVOID_INFO_CNT * sizeof(CNAS_HSD_AVOID_FREQ_INFO_STRU));

    pstCnasMntnSaveExcLog->stCnasHsdMntnSaveExcLog.stRedirection.ucChanNum = \
                                        CNAS_HSD_GetHsdCtxAddr()->stSysAcqCtrl.stRedirection.ucChanNum;

    NAS_MEM_CPY_S(pstCnasMntnSaveExcLog->stCnasHsdMntnSaveExcLog.stRedirection.aulChannel,
                  CNAS_MNTN_DUMP_HSD_REDIRECTION_INFO_CNT * sizeof(CAS_CNAS_HRPD_DERECTION_CHANNEL_STRU),
                  CNAS_HSD_GetHsdCtxAddr()->stSysAcqCtrl.stRedirection.aulChannel,
                  CNAS_MNTN_DUMP_HSD_REDIRECTION_INFO_CNT * sizeof(CAS_CNAS_HRPD_DERECTION_CHANNEL_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasHsdMntnSaveExcLog.stHighPriority),
                  sizeof(CNAS_HSD_HIGHPRIORITY_STRU),
                  &(CNAS_HSD_GetHsdCtxAddr()->stSysAcqCtrl.stHighPriority),
                  sizeof(CNAS_HSD_HIGHPRIORITY_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasHsdMntnSaveExcLog.stHrpdSysInfo),
                  sizeof(CNAS_HSD_HRPD_SYSTEM_INFO_STRU),
                  &(CNAS_HSD_GetHsdCtxAddr()->stSysAcqCtrl.stHrpdSysInfo),
                  sizeof(CNAS_HSD_HRPD_SYSTEM_INFO_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasHsdMntnSaveExcLog.stOocScheduleInfo),
                  sizeof(CNAS_HSD_OOC_SCHEDULE_INFO_STRU),
                  &(CNAS_HSD_GetHsdCtxAddr()->stSysAcqCtrl.stOocScheduleInfo),
                  sizeof(CNAS_HSD_OOC_SCHEDULE_INFO_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasHsdMntnSaveExcLog.stCasOhmHrpdSys),
                  sizeof(CNAS_PRL_HRPD_SYSTEM_STRU),
                  &(CNAS_HSD_GetHsdCtxAddr()->stSysAcqCtrl.stCasOhmHrpdSys),
                  sizeof(CNAS_PRL_HRPD_SYSTEM_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasHsdMntnSaveExcLog.stNetwkLostSysRec),
                  sizeof(CNAS_HSD_NETWORK_LOST_SYS_RECORD_STRU),
                  &(CNAS_HSD_GetHsdCtxAddr()->stSysAcqCtrl.stNetwkLostSysRec),
                  sizeof(CNAS_HSD_NETWORK_LOST_SYS_RECORD_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasHsdMntnSaveExcLog.stSyncFreq),
                  sizeof(CNAS_PRL_FREQENCY_CHANNEL_STRU),
                  &(CNAS_HSD_GetHsdCtxAddr()->stSysAcqCtrl.stSyncFreq),
                  sizeof(CNAS_PRL_FREQENCY_CHANNEL_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasHsdMntnSaveExcLog.stSysAcqSrlteInfo),
                  sizeof(CNAS_HSD_HRPD_SYS_ACQ_SRLTE_INFO_STRU),
                  &(CNAS_HSD_GetHsdCtxAddr()->stSysAcqCtrl.stSysAcqSrlteInfo),
                  sizeof(CNAS_HSD_HRPD_SYS_ACQ_SRLTE_INFO_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasHsdMntnSaveExcLog.stHrpdMatched1xSysInfo),
                  sizeof(CNAS_HSD_1X_SYSTEM_INFO_STRU),
                  &(CNAS_HSD_GetHsdCtxAddr()->stSysAcqCtrl.stHrpdMatched1xSysInfo),
                  sizeof(CNAS_HSD_1X_SYSTEM_INFO_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasHsdMntnSaveExcLog.stSysAssistInfo),
                  sizeof(CNAS_HSD_SYSTEM_ASSIST_INFO_STRU),
                  &(CNAS_HSD_GetHsdCtxAddr()->stSysAssistInfo),
                  sizeof(CNAS_HSD_SYSTEM_ASSIST_INFO_STRU));
}


VOS_VOID CNAS_MNTN_SaveCnasXregGlobalInfo(
    CNAS_MNTN_SAVE_EXC_LOG_STRU        *pstCnasMntnSaveExcLog
)
{
    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasXregMntnSaveExcLog.stCnasXregStateInfo),
                  sizeof(CNAS_XREG_REGSTATE_STRU),
                  CNAS_XREG_GetRegStateAddr(),
                  sizeof(CNAS_XREG_REGSTATE_STRU));

    NAS_MEM_CPY_S(&(pstCnasMntnSaveExcLog->stCnasXregMntnSaveExcLog.stCnasXregSysMsgCont),
                  sizeof(CNAS_XREG_SYS_MSG_CONTENT_STRU),
                  CNAS_XREG_GetSysMsgAddr(),
                  sizeof(CNAS_XREG_SYS_MSG_CONTENT_STRU));
}


VOS_VOID  CNAS_MNTN_SaveExcLog(
    VOS_UINT8                          *pucAddr,
    VOS_UINT32                          ulLength
)
{
    CNAS_MNTN_SAVE_EXC_LOG_STRU        *pstCnasMntnSaveExcLog = VOS_NULL_PTR;
    VOS_UINT32                          ulCnasSaveExcLength;

    /* 入参检查 */
    if ( ( VOS_NULL_PTR == pucAddr )
      || ( 0 == ulLength ))
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_MNTN_SaveExcLog(): No memory allocated");

        return;
    }

    ulCnasSaveExcLength = sizeof(CNAS_MNTN_SAVE_EXC_LOG_STRU);

    /* 确保CNAS_MNTN_SAVE_EXC_LOG_STRU结构的长度不会溢出底软分配给Nas的空间，如果超出需要调整 */
    if (ulLength < ulCnasSaveExcLength)
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_MNTN_SaveExcLog(): Memory allocated not enough");

        return;
    }

    pstCnasMntnSaveExcLog = (CNAS_MNTN_SAVE_EXC_LOG_STRU*)pucAddr;

    /* 标示存储的有效信息的开始和结束字段 */
    pstCnasMntnSaveExcLog->ulCnasBeginTag = CNAS_MNTN_DUMP_BEGIN_TAG;
    pstCnasMntnSaveExcLog->ulCnasEndTag   = CNAS_MNTN_DUMP_END_TAG;

    CNAS_NORMAL_LOG(UEPS_PID_XSD, "CNAS_MNTN_SaveExcLog(): Save CNAS related info before reset");

    /* 存储消息 */
    CNAS_MNTN_SaveCnasLogMsgState(pstCnasMntnSaveExcLog);

    pstCnasMntnSaveExcLog->ulCnasCcbInfoStart  = CNAS_MNTN_DUMP_CCB_INFO_START_TAG;

    /* 存储CNAS_CCB中的全局变量 */
    CNAS_MNTN_SaveCnasCcbGlobalInfo(pstCnasMntnSaveExcLog);

    pstCnasMntnSaveExcLog->ulCnasXsdInfoStart  = CNAS_MNTN_DUMP_XSD_INFO_START_TAG;

    /* 存储CNAS_XSD中的全局变量 */
    CNAS_MNTN_SaveCnasXsdGlobalInfo(pstCnasMntnSaveExcLog);

    pstCnasMntnSaveExcLog->ulCnasXccInfoStart  = CNAS_MNTN_DUMP_XCC_INFO_START_TAG;

    /* 存储CNAS_XCC中的全局变量 */
    CNAS_MNTN_SaveCnasXccGlobalInfo(pstCnasMntnSaveExcLog);

    pstCnasMntnSaveExcLog->ulCnasXregInfoStart = CNAS_MNTN_DUMP_XREG_INFO_START_TAG;

    /* 存储CNAS_XREG中的全局变量 */
    CNAS_MNTN_SaveCnasXregGlobalInfo(pstCnasMntnSaveExcLog);

    pstCnasMntnSaveExcLog->ulCnasHsmInfoStart  = CNAS_MNTN_DUMP_HSM_INFO_START_TAG;

    /* 存储CNAS_HSM中的全局变量 */
    CNAS_MNTN_SaveCnasHsmGlobalInfo(pstCnasMntnSaveExcLog);

    pstCnasMntnSaveExcLog->ulCnasEHsmInfoStart = CNAS_MNTN_DUMP_EHSM_INFO_START_TAG;

    /* 存储CNAS_EHSM中的全局变量 */
    CNAS_MNTN_SaveCnasEHsmGlobalInfo(pstCnasMntnSaveExcLog);

    pstCnasMntnSaveExcLog->ulCnasHsdInfoStart  = CNAS_MNTN_DUMP_HSD_INFO_START_TAG;

    /* 存储CNAS_HSD中的全局变量 */
    CNAS_MNTN_SaveCnasHsdGlobalInfo(pstCnasMntnSaveExcLog);

    return ;
}


VOS_VOID CNAS_MNTN_SaveCnasExcLog( VOS_VOID )
{
    VOS_UINT8                          *pucSaveAddr   = VOS_NULL_PTR;
    VOS_UINT32                          ulLeftLength  = 0;

    if ( VOS_NULL_PTR == gpucCnasExcAddr )
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_MNTN_SaveCnasExcLog(): No memory allocated");
        return;
    }

    pucSaveAddr     = gpucCnasExcAddr;
    ulLeftLength    = CNAS_MNTN_SAVE_EXC_LOG_LENGTH_MODEM;

    CNAS_MNTN_SaveExcLog( ( VOS_UINT8 *)pucSaveAddr, ulLeftLength);

    return;
}


VOS_VOID CNAS_MNTN_RegisterDumpCallBack( VOS_VOID )
{
    /* 调用底软接口进行申请内存 */
    gpucCnasExcAddr = (VOS_UINT8 *)mdrv_om_register_field(OM_CP_CNAS,
                                                          "CNAS dump",
                                                          VOS_NULL_PTR,
                                                          VOS_NULL_PTR,
                                                          CNAS_MNTN_SAVE_EXC_LOG_LENGTH_MODEM,
                                                          0);

    /* 申请失败打印异常信息 */
    if (VOS_NULL_PTR == gpucCnasExcAddr)
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_MNTN_RegisterDumpCallBack(): No memory allocated");
        return;
    }

    /* 保存CNAS复位前的临终遗言信息,注册回调函数 */
    (VOS_VOID)mdrv_om_register_callback("CNAS_EXCLOG", (dump_hook)CNAS_MNTN_SaveCnasExcLog);

    CNAS_NORMAL_LOG(UEPS_PID_XSD, "CNAS dump register call back function finished");

    return;
}

/*lint -restore*/
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */



