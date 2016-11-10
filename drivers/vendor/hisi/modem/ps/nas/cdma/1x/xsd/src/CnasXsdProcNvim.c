

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include  "CnasXsdProcNvim.h"
#include  "CnasXsdCtx.h"
#include  "NVIM_Interface.h"
#include  "CnasMntn.h"
#include  "CnasXsdMntn.h"
#include  "NasNvInterface.h"
#include  "msp_diag_comm.h"

#include  "CnasXsdTimer.h"
#include  "CasNvInterface.h"
#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_CNAS_XSD_PROC_NVIM_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/*****************************************************************************
  3 函数定义
*****************************************************************************/
/*lint -save -e958*/

VOS_VOID CNAS_XSD_ReadMruNvim(VOS_VOID)
{
    CNAS_XSD_MRU_LIST_STRU             *pstMruList = VOS_NULL_PTR;
    CNAS_NVIM_1X_MRU_LIST_STRU          stNvMruList;
    VOS_UINT16                          i;

    pstMruList      = CNAS_XSD_GetMruList();

    NAS_MEM_SET_S(&stNvMruList, sizeof(stNvMruList), 0x0, sizeof(CNAS_NVIM_1X_MRU_LIST_STRU));

    if (NV_OK != NV_Read(en_NV_Item_1X_MRU_LIST,
                         &stNvMruList, sizeof(CNAS_NVIM_1X_MRU_LIST_STRU)))
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_ReadMruNvim:Read Nvim Failed");
        return;
    }

    CNAS_MNTN_LogReadNVInfo(en_NV_Item_1X_MRU_LIST,
                            sizeof(CNAS_NVIM_1X_MRU_LIST_STRU),
                            UEPS_PID_XSD,
                            (VOS_UINT8 *)&stNvMruList);

    pstMruList->ucSysNum = stNvMruList.ucSysNum;

    for (i = 0; i < stNvMruList.ucSysNum; i++)
    {
        pstMruList->astSystem[i].stFreq.enBandClass = stNvMruList.astSystem[i].usBandClass;
        pstMruList->astSystem[i].stFreq.usChannel   = stNvMruList.astSystem[i].usChannel;
        pstMruList->astSystem[i].usNid              = stNvMruList.astSystem[i].usNid;
        pstMruList->astSystem[i].usSid              = stNvMruList.astSystem[i].usSid;
    }

    CNAS_XSD_LogMruList(pstMruList);
}


VOS_VOID CNAS_XSD_WriteMruNvim(
    CNAS_XSD_MRU_LIST_STRU             *pstMruList
)
{
    CNAS_NVIM_1X_MRU_LIST_STRU          stNvMruList;
    VOS_UINT16                          i;

    NAS_MEM_SET_S(&stNvMruList, sizeof(stNvMruList), 0x0, sizeof(CNAS_NVIM_1X_MRU_LIST_STRU));

    stNvMruList.ucSysNum = pstMruList->ucSysNum;

    for (i = 0; i < pstMruList->ucSysNum; i++)
    {
        stNvMruList.astSystem[i].usBandClass        = pstMruList->astSystem[i].stFreq.enBandClass;
        stNvMruList.astSystem[i].usChannel          = pstMruList->astSystem[i].stFreq.usChannel;
        stNvMruList.astSystem[i].usNid              = pstMruList->astSystem[i].usNid;
        stNvMruList.astSystem[i].usSid              = pstMruList->astSystem[i].usSid;
    }

    if (NV_OK != NV_Write(en_NV_Item_1X_MRU_LIST,
                         &stNvMruList, sizeof(CNAS_NVIM_1X_MRU_LIST_STRU)))
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_WriteMruNvim:Write Nvim Failed");
        return;
    }
}


VOS_VOID CNAS_XSD_ReadPrlNvim(VOS_VOID)
{
    CNAS_NVIM_1X_EVDO_PRL_LIST_STRU                        *pPrlData = VOS_NULL_PTR;
    VOS_UINT32                                              ulRslt;

    /* 读取PRL前，为PRL码流分配临时内存空间 */
    pPrlData = (CNAS_NVIM_1X_EVDO_PRL_LIST_STRU *)PS_MEM_ALLOC(UEPS_PID_XSD, sizeof(CNAS_NVIM_1X_EVDO_PRL_LIST_STRU));

    if (VOS_NULL_PTR != pPrlData)
    {
        NAS_MEM_SET_S(pPrlData, sizeof(CNAS_NVIM_1X_EVDO_PRL_LIST_STRU), 0, sizeof(CNAS_NVIM_1X_EVDO_PRL_LIST_STRU));

        if (NV_OK != NV_Read(en_NV_Item_PRL_LIST, pPrlData, sizeof(CNAS_NVIM_1X_EVDO_PRL_LIST_STRU)))
        {
            CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_ReadPrlNvim: Read Nvim Failed");

            /* 释放内存 */
            PS_MEM_FREE(UEPS_PID_XSD, pPrlData);

            return;
        }

        if (VOS_FALSE == CNAS_XSD_GetTestConfig()->ucPrlCombinedFlag)
        {
            /* 默认尝试用EPRL方式解析PRL码流 */
            ulRslt = CNAS_PRL_ProcPrlInfo(&(pPrlData->ucPrlData[0]), sizeof(CNAS_NVIM_1X_EVDO_PRL_LIST_STRU), CNAS_PRL_SSPR_P_REV_3);
        }
        else
        {
            /* 为测试需要增加对NV解析处理，如果当前ucPrlCombinedFlag不为FALSE，则统一按照版本1，走级联PRL解析 */
            ulRslt = CNAS_PRL_ProcPrlInfo(&(pPrlData->ucPrlData[0]), sizeof(CNAS_NVIM_1X_EVDO_PRL_LIST_STRU), CNAS_PRL_SSPR_P_REV_1);
        }

        if (VOS_TRUE == ulRslt)
        {
            CNAS_XSD_LogPrlSourceType(CNAS_CCB_PRL_TYPE_NVRAM);
            CNAS_CCB_SetPrlSrcType(CNAS_CCB_PRL_TYPE_NVRAM);
        }
        else
        {
            CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_ReadPrlNvim: parse Nvim PRL Failed");
        }

        /* 释放内存 */
        PS_MEM_FREE(UEPS_PID_XSD, pPrlData);
    }

    return;
}




VOS_VOID CNAS_XSD_ReadBandClassNvim(VOS_VOID)
{
#if 0
    VOS_UINT32                          ulNvimBand;

    NAS_MEM_SET_S(&ulNvimBand, 0x0, sizeof(VOS_UINT32));

    if (NV_OK != NV_Read(en_NV_Item_1X_BANDCLASS_LIST,
                         &ulNvimBand, sizeof(VOS_UINT32)))
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_ReadBandClassNvim:Read Nvim Failed");
        return;
    }

    CNAS_XSD_SetUeSupportedBandClass(ulNvimBand);

    CNAS_XSD_LogUeSupportedBandClass(ulNvimBand);
#endif
    return;
}


VOS_VOID CNAS_XSD_ReadHomeSidNidNvim(VOS_VOID)
{
    CNAS_CCB_1X_HOME_SID_NID_LIST_STRU                     *pstHomeSidNidList = VOS_NULL_PTR;
    CNAS_NVIM_1X_HOME_SID_NID_LIST_STRU                     stNvHomeSidNidList;
    VOS_UINT16                                              i;

    pstHomeSidNidList       = CNAS_CCB_GetHomeSidNidList();

    NAS_MEM_SET_S(&stNvHomeSidNidList, sizeof(stNvHomeSidNidList), 0x0, sizeof(CNAS_NVIM_1X_HOME_SID_NID_LIST_STRU));

    if (NV_OK != NV_Read(en_NV_Item_1X_HOME_SID_NID_LIST,
                        &stNvHomeSidNidList, sizeof(CNAS_NVIM_1X_HOME_SID_NID_LIST_STRU)))
    {
        /* NVIM先读，如果读取失败，初始化该变量 */
        NAS_MEM_SET_S(pstHomeSidNidList, sizeof(CNAS_CCB_1X_HOME_SID_NID_LIST_STRU), 0, sizeof(CNAS_CCB_1X_HOME_SID_NID_LIST_STRU));

        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_ReadHomeSidNidNvim:Read Nvim Failed");
        return;
    }

    CNAS_MNTN_LogReadNVInfo(en_NV_Item_1X_HOME_SID_NID_LIST,
                            sizeof(CNAS_NVIM_1X_HOME_SID_NID_LIST_STRU),
                            UEPS_PID_XSD,
                            (VOS_UINT8 *)&stNvHomeSidNidList);

    pstHomeSidNidList->ucSysNum = (VOS_UINT8)CNAS_MIN(stNvHomeSidNidList.ucSysNum, CNAS_NVIM_MAX_1X_HOME_SID_NID_NUM);

    for(i = 0; i < pstHomeSidNidList->ucSysNum; i++)
    {
        pstHomeSidNidList->astHomeSidNid[i].usSid   = stNvHomeSidNidList.astHomeSidNid[i].usSid;
        pstHomeSidNidList->astHomeSidNid[i].usNid   = stNvHomeSidNidList.astHomeSidNid[i].usNid;
        pstHomeSidNidList->astHomeSidNid[i].usBand  = stNvHomeSidNidList.astHomeSidNid[i].usBand;
    }

    CNAS_XSD_LogHomeSidNidList(pstHomeSidNidList);
}




VOS_VOID CNAS_XSD_Read1xOosSysAcqStrategyCfgNvim(VOS_VOID)
{
    CNAS_XSD_OOC_SCHEDULE_INFO_STRU                   *pstOocScheduleInfo = VOS_NULL_PTR;
    CNAS_NVIM_1X_OOS_SYS_ACQ_STRATEGY_CFG_STRU         stNvimOosSysAcqStrategyCfg;
    VOS_UINT16                                         i;

    pstOocScheduleInfo       = CNAS_XSD_GetOocScheduleInfo();

    PS_MEM_SET(&stNvimOosSysAcqStrategyCfg,
              0x0,
              sizeof(CNAS_NVIM_1X_OOS_SYS_ACQ_STRATEGY_CFG_STRU));

    if (NV_OK != NV_Read(en_NV_Item_1X_OOS_SYS_ACQ_STRATEGY_CFG,
                        &stNvimOosSysAcqStrategyCfg, sizeof(CNAS_NVIM_1X_OOS_SYS_ACQ_STRATEGY_CFG_STRU)))
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_ReadOosSysAcqStrategyCfgNvim:Read Nvim Failed");

        return;
    }

    CNAS_MNTN_LogReadNVInfo(en_NV_Item_1X_OOS_SYS_ACQ_STRATEGY_CFG,
                            sizeof(CNAS_NVIM_1X_OOS_SYS_ACQ_STRATEGY_CFG_STRU),
                            UEPS_PID_XSD,
                            (VOS_UINT8 *)&stNvimOosSysAcqStrategyCfg);

    pstOocScheduleInfo->stStrategyCfg.ucPhaseNum           = (VOS_UINT8)CNAS_MIN(CNAS_XSD_MAX_OOC_SCHEDULE_PHASE_NUM, stNvimOosSysAcqStrategyCfg.stCtrlInfo.ucPhaseNum);

    /* 总阶段数如果设置为0,则取默认值 */
    if (0 == stNvimOosSysAcqStrategyCfg.stCtrlInfo.ucPhaseNum)
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_ReadOosSysAcqStrategyCfgNvim:total phase num is zero!");

        return;
    }

    for (i = 0; i < pstOocScheduleInfo->stStrategyCfg.ucPhaseNum; i++)
    {
        pstOocScheduleInfo->stStrategyCfg.astPhaseCfgInfo[i].ucSrchNum      = stNvimOosSysAcqStrategyCfg.astSysAcqPhaseInfo[i].ucSrchNum;

        /* 搜索次数不能为0,否则取1阶段的默认值 */
        if (0 == pstOocScheduleInfo->stStrategyCfg.astPhaseCfgInfo[i].ucSrchNum)
        {
            CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_ReadOosSysAcqStrategyCfgNvim:current phase srch num is zero!");

            pstOocScheduleInfo->stStrategyCfg.astPhaseCfgInfo[i].ucSrchNum = CNAS_XSD_DEFAULT_OOS_SYS_ACQ_PHASE_ONE_MAX_NUM_ONE_CYCLE;
        }

        /* sleep时长可以为0，代表不间隔连续搜索 */
        pstOocScheduleInfo->stStrategyCfg.astPhaseCfgInfo[i].usSleepTimeLen = stNvimOosSysAcqStrategyCfg.astSysAcqPhaseInfo[i].usSleepTimeLen;

        /* 总时长为0代表不控制搜索总时长 */
        pstOocScheduleInfo->stStrategyCfg.astPhaseCfgInfo[i].usTotalTimeLen = stNvimOosSysAcqStrategyCfg.astSysAcqPhaseInfo[i].usTotalTimeLen;
    }

    pstOocScheduleInfo->stStrategyCfg.usMru0SearchTimerLen = stNvimOosSysAcqStrategyCfg.stCtrlInfo.usMru0SearchTimerLen;
    pstOocScheduleInfo->stStrategyCfg.enOocScanStrategy          = stNvimOosSysAcqStrategyCfg.stCtrlInfo.enChanRepeatScanStrategy;
    pstOocScheduleInfo->stStrategyCfg.uc1xOocDoTchPhase1TimerLen = stNvimOosSysAcqStrategyCfg.stCtrlInfo.uc1xOocDoTchPhase1TimerLen;
    pstOocScheduleInfo->stStrategyCfg.uc1xOocDoTchPhase2TimerLen = stNvimOosSysAcqStrategyCfg.stCtrlInfo.uc1xOocDoTchPhase2TimerLen;

    CNAS_XSD_LogOocScheduleInfo(&stNvimOosSysAcqStrategyCfg);
}



VOS_VOID CNAS_XSD_ReadTestConfigNvim(VOS_VOID)
{
    CNAS_XSD_TEST_CONFIG_STRU          *pstTestConfig = VOS_NULL_PTR;
    CNAS_NVIM_TEST_CONFIG_STRU          stTestConfig;

    pstTestConfig      = CNAS_XSD_GetTestConfig();

    NAS_MEM_SET_S(&stTestConfig, sizeof(stTestConfig), 0x0, sizeof(stTestConfig));

    if (NV_OK != NV_Read(en_NV_Item_TEST_CONFIG,
                         &stTestConfig, sizeof(stTestConfig)))
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_ReadTestConfigNvim:Read Nvim Failed");

        pstTestConfig->ucReadNvPrlDirectly    = VOS_FALSE;
        pstTestConfig->ucReadDefaultPrl       = VOS_FALSE;
        pstTestConfig->ucIsMod1xAvailTimerLen = VOS_FALSE;

        pstTestConfig->ucPrlCombinedFlag      = VOS_FALSE;

        return;
    }

    CNAS_MNTN_LogReadNVInfo(en_NV_Item_TEST_CONFIG,
                            sizeof(CNAS_NVIM_TEST_CONFIG_STRU),
                            UEPS_PID_XSD,
                            (VOS_UINT8 *)&stTestConfig);

    pstTestConfig->ucReadNvPrlDirectly    = stTestConfig.ucReadNvPrlDirectly;
    pstTestConfig->ucReadDefaultPrl       = stTestConfig.ucReadDefaultPrl;
    pstTestConfig->ucIsMod1xAvailTimerLen = stTestConfig.ucIsMod1xAvailTimerLen;

    pstTestConfig->ucPrlCombinedFlag      = stTestConfig.ucNvPrlCombinedFlag;

}


VOS_VOID CNAS_XSD_ReadAddAvoidListCfgNvim(VOS_VOID)
{
    CNAS_XSD_ADD_AVOID_LIST_CFG_STRU                       *pstAddAvoidListCfg = VOS_NULL_PTR;
    CNAS_NVIM_1X_ADD_AVOID_LIST_CFG_STRU                    stAddAvoidListCfg;

    pstAddAvoidListCfg      = &(CNAS_XSD_Get1xSysAcqNvimConfig()->stAddAvoidListCfg);

    NAS_MEM_SET_S(&stAddAvoidListCfg, sizeof(stAddAvoidListCfg), 0x0, sizeof(stAddAvoidListCfg));

    if (NV_OK != NV_Read(en_NV_Item_1X_ADD_AVOID_LIST_CFG,
                         &stAddAvoidListCfg, sizeof(stAddAvoidListCfg)))
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_ReadAddAvoidFreqCfgNvim:Read Nvim Failed");

        pstAddAvoidListCfg->ucIsNegSysAdd = VOS_FALSE;
        return;
    }

    pstAddAvoidListCfg->ucIsNegSysAdd = stAddAvoidListCfg.ucIsNegSysAdd;

}


VOS_VOID CNAS_XSD_ReadNegPrefSysCmpCtrlNvim(VOS_VOID)
{
    CNAS_XSD_NEG_PREF_SYS_CMP_CTRL_STRU                   *pstNegPrefSysCmpCtrl = VOS_NULL_PTR;
    CNAS_NVIM_1X_NEG_PREF_SYS_CMP_CTRL_STRU                stNegPrefSysCmpCtrl;

    pstNegPrefSysCmpCtrl      = &(CNAS_XSD_Get1xSysAcqNvimConfig()->stNegPrefSysCmpCtrl);

    NAS_MEM_SET_S(&stNegPrefSysCmpCtrl, sizeof(stNegPrefSysCmpCtrl), 0x0, sizeof(stNegPrefSysCmpCtrl));

    if (NV_OK != NV_Read(en_NV_Item_1X_NEG_PREF_SYS_CMP_CTRL,
                         &stNegPrefSysCmpCtrl, sizeof(stNegPrefSysCmpCtrl)))
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_ReadNegPrefSysCmpCtrlNvim:Read Nvim Failed");

        pstNegPrefSysCmpCtrl->enNegPrefSysCmpType = CNAS_XSD_NEG_PREF_SYS_CMP_BAND_CHAN_AMBIGUOUS_MATCH;
        return;
    }

    pstNegPrefSysCmpCtrl->enNegPrefSysCmpType = stNegPrefSysCmpCtrl.enNegPrefSysCmpType;

}


VOS_VOID CNAS_XSD_ReadCallBackCfgNvim( VOS_VOID )
{
    CNAS_XSD_CALLBACK_CFG_STRU         *pstCallBackCfg = VOS_NULL_PTR;
    CNAS_NVIM_1X_CALLBACK_CFG_STRU      stNvCallBackCfg;

    pstCallBackCfg                      = CNAS_XSD_GetCallBackCfg();

    NAS_MEM_SET_S(&stNvCallBackCfg, sizeof(stNvCallBackCfg), 0x00, sizeof(CNAS_NVIM_1X_CALLBACK_CFG_STRU));

    if (NV_OK != NV_Read(en_Nv_Item_EMC_CALLBACK_CFG,
                         &stNvCallBackCfg, sizeof(CNAS_NVIM_1X_CALLBACK_CFG_STRU)))
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_ReadCallBackCfgNvim:Read Nvim Failed");

        /* 读取失败 默认关闭CallBack */
        pstCallBackCfg->ulCallBackEnableFlg     = VOS_FALSE;
        pstCallBackCfg->ulCallBackModeTimerLen  = 0;

        return;
    }

    pstCallBackCfg->ulCallBackEnableFlg     = stNvCallBackCfg.ulCallBackEnableFlg;
    pstCallBackCfg->ulCallBackModeTimerLen  = stNvCallBackCfg.ulCallBackModeTimerLen * 1000;

    CNAS_XSD_LogCallBackConfig(pstCallBackCfg);

    return;
}

/* CNAS_XSD_ReadHomeSidNidDependOnCfgNvim */


VOS_VOID CNAS_XSD_ReadOperLockSysWhiteListInfoNvim(VOS_VOID)
{
    CNAS_NVIM_OPER_LOCK_SYS_WHITE_LIST_STRU                 stOperLockSysWhiteList;
    CNAS_CCB_OPER_LOCK_SYS_WHITE_LIST_STRU                 *pstOperLockSysWhiteList = VOS_NULL_PTR;
    VOS_UINT32                                              i;

    pstOperLockSysWhiteList = CNAS_CCB_GetOperLockSysWhiteList();

    NAS_MEM_SET_S(&stOperLockSysWhiteList, sizeof(stOperLockSysWhiteList), 0x0, sizeof(stOperLockSysWhiteList));
    NAS_MEM_SET_S(pstOperLockSysWhiteList, sizeof(CNAS_CCB_OPER_LOCK_SYS_WHITE_LIST_STRU), 0x0, sizeof(CNAS_CCB_OPER_LOCK_SYS_WHITE_LIST_STRU));

    if (NV_OK != NV_Read(en_NV_Item_OPER_LOCK_SYS_WHITE_LIST_INFO,
                         &stOperLockSysWhiteList, sizeof(stOperLockSysWhiteList)))
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_ReadOperLockSysWhiteListInfoNvim:Read Nvim Failed");
        return;
    }

    pstOperLockSysWhiteList->ucEnable      = stOperLockSysWhiteList.ucEnable;
    pstOperLockSysWhiteList->usWhiteSysNum = CNAS_MIN(stOperLockSysWhiteList.usWhiteSysNum, CNAS_CCB_MAX_WHITE_LOCK_SYS_NUM);

    for (i = 0; i < pstOperLockSysWhiteList->usWhiteSysNum;i++ )
    {
        pstOperLockSysWhiteList->astSysInfo[i].usStartSid = stOperLockSysWhiteList.astSysInfo[i].usStartSid;
        pstOperLockSysWhiteList->astSysInfo[i].usEndSid   = stOperLockSysWhiteList.astSysInfo[i].usEndSid;
        pstOperLockSysWhiteList->astSysInfo[i].ulMcc      = stOperLockSysWhiteList.astSysInfo[i].ulMcc;
    }

    CNAS_XSD_LogOperLockSysWhiteList(pstOperLockSysWhiteList);
}


VOS_VOID CNAS_XSD_ReadCTCCCustomizeFreqNvim(VOS_VOID)
{
    CNAS_NVIM_CDMA_HRPD_CUSTOMIZE_PREF_CHANNELS_STRU        stCustFreqList;
    CNAS_CCB_CTCC_CUSTOMIZE_FREQ_LIST_STRU                 *pstCustFreqList = VOS_NULL_PTR;
    VOS_UINT32                                              i;

    pstCustFreqList = CNAS_CCB_GetCTCCCustomizeFreqList();

    NAS_MEM_SET_S(&stCustFreqList, sizeof(stCustFreqList), 0x0, sizeof(stCustFreqList));
    NAS_MEM_SET_S(pstCustFreqList, sizeof(CNAS_CCB_CTCC_CUSTOMIZE_FREQ_LIST_STRU), 0x0, sizeof(CNAS_CCB_CTCC_CUSTOMIZE_FREQ_LIST_STRU));

    if (NV_OK != NV_Read(en_NV_Item_CDMA_HRPD_CUSTOMIZE_PREF_CHANNELS_INFO,
                         &stCustFreqList, sizeof(stCustFreqList)))
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_ReadCTCCCustomizeFreqNvim:Read Nvim Failed");
        return;
    }

    pstCustFreqList->ucEnableFlg = stCustFreqList.ucEnableFlg;
    pstCustFreqList->usFreqNum   = CNAS_MIN(stCustFreqList.usFreqNum, CNAS_CCB_MAX_FREQ_NUM);

    for (i = 0; i < pstCustFreqList->usFreqNum;i++ )
    {
        pstCustFreqList->astFreqList[i].usChannel   = stCustFreqList.astFreqList[i].usChannel;
    }

    CNAS_XSD_LogCTCCCustomizeFreqList(pstCustFreqList);
}



VOS_UINT32 CNAS_XSD_WriteOperLockWhiteSidListInfoNvim(
    MSCC_XSD_OPER_LOCK_SYS_WHITE_STRU  *pstOPerLockWhiteSid
)
{
    CNAS_NVIM_OPER_LOCK_SYS_WHITE_LIST_STRU                 stOperLockSysWhiteListNvim;

    NAS_MEM_CPY_S(&stOperLockSysWhiteListNvim, sizeof(stOperLockSysWhiteListNvim), pstOPerLockWhiteSid, sizeof(CNAS_NVIM_OPER_LOCK_SYS_WHITE_LIST_STRU));

    if (NV_OK != NV_Write(en_NV_Item_OPER_LOCK_SYS_WHITE_LIST_INFO,
                         &stOperLockSysWhiteListNvim, sizeof(CNAS_NVIM_OPER_LOCK_SYS_WHITE_LIST_STRU)))
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_WriteOperLockWhiteSidInfoNvim:Write Nvim Failed");

        return VOS_FALSE;
    }

    return VOS_TRUE;
}



VOS_VOID CNAS_XSD_ReadCdmaStandardChannelsNvim(VOS_VOID)
{
    CNAS_NVIM_CDMA_STANDARD_CHANNELS_STRU                  stNvimCdmaStandardChan;
    CNAS_CCB_CDMA_STANDARD_CHANNELS_STRU                   *pstCdmaStandardChan = VOS_NULL_PTR;

    pstCdmaStandardChan       = CNAS_CCB_GetCdmaStandardChannels();

    NAS_MEM_SET_S(&stNvimCdmaStandardChan, sizeof(stNvimCdmaStandardChan), 0x0, sizeof(CNAS_NVIM_CDMA_STANDARD_CHANNELS_STRU));

    if (NV_OK != NV_Read(en_NV_Item_CDMA_STANDARD_CHANNLES_INFO,
                         &stNvimCdmaStandardChan,
                         sizeof(CNAS_NVIM_CDMA_STANDARD_CHANNELS_STRU)))
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_ReadCdmaStandardChannelsNvim: Read Nvim Failed");
    }
    else
    {
        pstCdmaStandardChan->usPrimaryA   = stNvimCdmaStandardChan.usPrimaryA;

        pstCdmaStandardChan->usPrimaryB   = stNvimCdmaStandardChan.usPrimaryB;

        pstCdmaStandardChan->usSecondaryA = stNvimCdmaStandardChan.usSecondaryA;

        pstCdmaStandardChan->usSecondaryB = stNvimCdmaStandardChan.usSecondaryB;
    }

    CNAS_XSD_LogCdmaStandardChannels(pstCdmaStandardChan);
}



VOS_VOID CNAS_XSD_Read1xSupportBandClassMaskNvim(VOS_VOID)
 {
    VOS_UINT32                                            *pul1xSupportBandClassMask = VOS_NULL_PTR;
    CNAS_NVIM_1X_SUPPORT_BANDCLASS_MASK_STRU               st1xSupportBandClassMask;

    pul1xSupportBandClassMask = &(CNAS_XSD_GetXsdCtxAddr()->stSysAcqCtrl.unUeSupportedBand.ulBand);

    NAS_MEM_SET_S(&st1xSupportBandClassMask, sizeof(st1xSupportBandClassMask), 0x0, sizeof(st1xSupportBandClassMask));

    if (NV_OK != NV_Read(en_NV_Item_CDMA_SUPPORT_BANDCLASS_MASK,
                         &st1xSupportBandClassMask, sizeof(st1xSupportBandClassMask)))
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_Read1xSupportBandClassMaskNvim:Read Nvim Failed");

        return;
    }

    *pul1xSupportBandClassMask = st1xSupportBandClassMask.ulSupportBandclassMask;

}



VOS_VOID CNAS_XSD_ReadCdma1XCustomPrefChannelsNvim( VOS_VOID )
{
    CNAS_NVIM_CDMA_1X_CUSTOMIZE_PREF_CHANNELS_STRU          stNvimCdma1XCustomPrefChan;
    CNAS_XSD_CDMA_1X_CUSTOM_PREF_CHANNELS_STRU             *pstCdma1XCustomPrefChan    = VOS_NULL_PTR;
    VOS_UINT32                                              i;

    pstCdma1XCustomPrefChan       = CNAS_XSD_GetCdma1XCustomPrefChannels();

    NAS_MEM_SET_S(&stNvimCdma1XCustomPrefChan, sizeof(stNvimCdma1XCustomPrefChan), 0x0, sizeof(CNAS_NVIM_CDMA_1X_CUSTOMIZE_PREF_CHANNELS_STRU));
    NAS_MEM_SET_S(pstCdma1XCustomPrefChan, sizeof(CNAS_XSD_CDMA_1X_CUSTOM_PREF_CHANNELS_STRU), 0x00, sizeof(CNAS_XSD_CDMA_1X_CUSTOM_PREF_CHANNELS_STRU));

    if (NV_OK != NV_Read(en_NV_Item_CDMA_1X_CUSTOMIZE_PREF_CHANNELS_INFO,
                         &stNvimCdma1XCustomPrefChan,
                         sizeof(CNAS_NVIM_CDMA_1X_CUSTOMIZE_PREF_CHANNELS_STRU)))
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_ReadCdma1XCustomPrefChannelsNvim: Read Nvim Failed");

        return;
    }

    pstCdma1XCustomPrefChan->ucEnableFlg  = stNvimCdma1XCustomPrefChan.ucEnableFlg;
    pstCdma1XCustomPrefChan->usFreqNum    = stNvimCdma1XCustomPrefChan.usFreqNum;
    stNvimCdma1XCustomPrefChan.usFreqNum  = CNAS_MIN(CNAS_XSD_MAX_CDMA_1X_CUSTOM_PREF_CHANNELS_NUM, stNvimCdma1XCustomPrefChan.usFreqNum);

    for (i = 0; i < stNvimCdma1XCustomPrefChan.usFreqNum; i++)
    {
        pstCdma1XCustomPrefChan->astFreqList[i].usChannel   = stNvimCdma1XCustomPrefChan.astFreqList[i].usChannel;
        pstCdma1XCustomPrefChan->astFreqList[i].enBandClass = (CNAS_PRL_BAND_CLASS_ENUM_UINT16)stNvimCdma1XCustomPrefChan.astFreqList[i].usBandClass;
    }

#ifdef DMT
    pstCdma1XCustomPrefChan->ucEnableFlg  = VOS_FALSE;
#endif

    CNAS_XSD_LogCdma1XCustomPrefChannels(pstCdma1XCustomPrefChan);

    return;
}


VOS_VOID CNAS_XSD_WriteCdma1XPrefChannelsNvim(
    CNAS_PRL_FREQENCY_CHANNEL_STRU     *pstFreq
)
{
    CNAS_NVIM_CDMA_1X_CUSTOMIZE_PREF_CHANNELS_STRU          stNvimCdma1XCustomPrefChan;

    NAS_MEM_SET_S(&stNvimCdma1XCustomPrefChan, sizeof(CNAS_NVIM_CDMA_1X_CUSTOMIZE_PREF_CHANNELS_STRU), 0x00, sizeof(CNAS_NVIM_CDMA_1X_CUSTOMIZE_PREF_CHANNELS_STRU));

    if (VOS_NULL_PTR == pstFreq)
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_WriteCdma1XPrefChannelsNvim: NULL PTR");

        return;
    }

    /* 读取当前的NV数值 */
    if (NV_OK != NV_Read(en_NV_Item_CDMA_1X_CUSTOMIZE_PREF_CHANNELS_INFO,
                         &stNvimCdma1XCustomPrefChan,
                         sizeof(CNAS_NVIM_CDMA_1X_CUSTOMIZE_PREF_CHANNELS_STRU)))
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_WriteCdma1XPrefChannelsNvim: Read Nvim Failed");

        return;
    }

    if (CNAS_NVIM_MAX_CDMA_1X_CUSTOMIZE_PREF_CHANNELS_NUM <= stNvimCdma1XCustomPrefChan.usFreqNum)
    {
        CNAS_WARNING_LOG(UEPS_PID_XSD, "CNAS_XSD_WriteCdma1XPrefChannelsNvim: NV Full Already!");

        return;
    }

    stNvimCdma1XCustomPrefChan.astFreqList[stNvimCdma1XCustomPrefChan.usFreqNum].usChannel      = pstFreq->usChannel;
    stNvimCdma1XCustomPrefChan.astFreqList[stNvimCdma1XCustomPrefChan.usFreqNum].usBandClass    = (VOS_UINT16)pstFreq->enBandClass;
    stNvimCdma1XCustomPrefChan.usFreqNum++;

    CNAS_XSD_LogCdma1XCustomPrefChannels((CNAS_XSD_CDMA_1X_CUSTOM_PREF_CHANNELS_STRU *)&stNvimCdma1XCustomPrefChan);

    if (NV_OK != NV_Write(en_NV_Item_CDMA_1X_CUSTOMIZE_PREF_CHANNELS_INFO,
                         &stNvimCdma1XCustomPrefChan,
                         sizeof(CNAS_NVIM_CDMA_1X_CUSTOMIZE_PREF_CHANNELS_STRU)))
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_WriteCdma1XPrefChannelsNvim: Write Nvim Failed");

        return;
    }

    return;
}


VOS_VOID CNAS_XSD_ReadNoCardModeCfgNvim(VOS_VOID)
{
    CNAS_NVIM_NO_CARD_MODE_CFG_STRU     stNoCardModeCfg;

    NAS_MEM_SET_S(&stNoCardModeCfg, sizeof(stNoCardModeCfg), 0x0, sizeof(stNoCardModeCfg));

    if (NV_OK != NV_Read(en_NV_Item_NO_CARD_MODE_CFG,
                         &stNoCardModeCfg, sizeof(stNoCardModeCfg)))
    {
        CNAS_XSD_SetNoCardModeCfgFlg(VOS_FALSE);

        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_ReadNoCardModeCfgNvim: Read Nvim Failed");

        return;
    }

    CNAS_XSD_SetNoCardModeCfgFlg((VOS_UINT8)stNoCardModeCfg.ulEnableFlag);

    return;
}



VOS_VOID CNAS_XSD_ReadEmcRedialSysAcqCfgNvim(VOS_VOID)
{
    VOS_UINT32                                              i;
    CNAS_XSD_EMC_REDIAL_SYS_ACQ_CFG_STRU                   *pstEmcRedialSysAcqCfg   = VOS_NULL_PTR;

    pstEmcRedialSysAcqCfg = CNAS_XSD_GetEmcRedialSysAcqCfgInfo();

    if (NV_OK != NV_Read(en_NV_Item_EMC_REDIAL_SYS_ACQ_CFG,
                         pstEmcRedialSysAcqCfg->aucRedialTimes, sizeof(pstEmcRedialSysAcqCfg->aucRedialTimes)))
    {
        pstEmcRedialSysAcqCfg->aucRedialTimes[0] = 4;
        pstEmcRedialSysAcqCfg->aucRedialTimes[1] = 3;

        for (i = 2; i < CNAS_XSD_MAX_MRU_SYS_NUM; i++)
        {
            pstEmcRedialSysAcqCfg->aucRedialTimes[i] = 2;
        }

        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_ReadEmcRedialSysAcqCfgNvim: Read Nvim Failed");

    }

    return;
}



VOS_VOID CNAS_XSD_Read1xAvoidScheduleInfoNvim(VOS_VOID)
{
    CNAS_NVIM_1X_AVOID_SCHEDULE_INFO_STRU                  stAvoidScheduleInfo;
    CNAS_XSD_AVOID_SCHEDULE_INFO_STRU                     *pstAvoidScheduleInfo  = VOS_NULL_PTR;
    VOS_UINT32                                             i;
    VOS_UINT32                                             j;

    NAS_MEM_SET_S(&stAvoidScheduleInfo, sizeof(stAvoidScheduleInfo), 0x0, sizeof(stAvoidScheduleInfo));

    pstAvoidScheduleInfo = CNAS_XSD_GetAvoidScheduInfoAddr();

    if (NV_OK != NV_Read(en_NV_Item_1X_AVOID_SCHEDULE_INFO,
                         &stAvoidScheduleInfo, sizeof(stAvoidScheduleInfo)))
    {
        for(i = 0; i < CNAS_XSD_AVOID_REASON_MAX; i++)
        {
            if ((CNAS_XSD_AVOID_REG_REJECTED                   == i)
             || (CNAS_XSD_AVOID_REDIRECTION_FAIL               == i)
             || (CNAS_XSD_AVOID_REDIRECTION_WITH_INVALID_GSRDM == i))
            {
                for(j = 0; j < CNAS_XSD_AVOID_MAX_PHASE_NUM; j++)
                {
                    pstAvoidScheduleInfo->aucAvoidTimerLen[i][j] = 30;
                }
            }
            else
            {
                for(j = 0; j < CNAS_XSD_AVOID_MAX_PHASE_NUM; j++)
                {
                    pstAvoidScheduleInfo->aucAvoidTimerLen[i][j] = 60;
                }
            }
        }

        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_Read1xAvoidScheduleInfoNvim: Read Nvim Failed");

        return;

    }

    for(i = 0; i < CNAS_XSD_AVOID_REASON_MAX; i++)
    {
        for (j = 0; j < CNAS_XSD_AVOID_MAX_PHASE_NUM; j++)
        {
            pstAvoidScheduleInfo->aucAvoidTimerLen[i][j] = stAvoidScheduleInfo.astAvoidPhaseNum[i].aucAvoidTimerLen[j];
        }
    }

    return;
}


VOS_VOID CNAS_XSD_Read1xPowerOffCampOnCtrlFlgNvim(VOS_VOID)
{
    CNAS_NVIM_1X_POWER_OFF_CAMP_ON_CTRL_STRU                st1xPowerOffCampOnCtrl;

    NAS_MEM_SET_S(&st1xPowerOffCampOnCtrl, sizeof(st1xPowerOffCampOnCtrl), 0, sizeof(st1xPowerOffCampOnCtrl));

    if (NV_OK != NV_Read(en_NV_Item_1X_POWER_OFF_CAMP_ON_CTRL,
                         &st1xPowerOffCampOnCtrl, sizeof(st1xPowerOffCampOnCtrl)))
    {
        CNAS_CCB_SetPowerOffCampOnCtrlFlg(VOS_FALSE);

        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_Read1xPowerOffCampOnCtrlFlgNvim: Read Nvim Failed");

        return;

    }

    CNAS_CCB_SetPowerOffCampOnCtrlFlg(st1xPowerOffCampOnCtrl.ucPowerOffCampOnCtrlFlg);

    return;
}



VOS_VOID CNAS_XSD_ReadMru0SwitchOocStrategyCfgNvim(VOS_VOID)
{

    CNAS_NVIM_MRU0_SWITCH_ON_OOC_STRATEGY_CFG_STRU          stNvimMru0StategyCfg;
    VOS_UINT32                                              ulLength;

    NAS_MEM_SET_S(&stNvimMru0StategyCfg, sizeof(stNvimMru0StategyCfg), 0x0, sizeof(stNvimMru0StategyCfg));

    /* 先获取NV的长度 */
    ulLength = 0;
    (VOS_VOID)NV_GetLength(en_NV_Item_MRU0_SWITCH_ON_OOC_STRATEGY_CFG, &ulLength);

    if (ulLength > sizeof(CNAS_NVIM_MRU0_SWITCH_ON_OOC_STRATEGY_CFG_STRU))
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_ReadMru0SwitchOocStrategyCfgNvim(): en_NV_Item_MRU0_SWITCH_ON_OOC_STRATEGY_CFG length Error");

        return;
    }

    if (NV_OK != NV_Read(en_NV_Item_MRU0_SWITCH_ON_OOC_STRATEGY_CFG,
                         &stNvimMru0StategyCfg,
                         sizeof(CNAS_NVIM_MRU0_SWITCH_ON_OOC_STRATEGY_CFG_STRU)))
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_ReadMru0SwitchOocStrategyCfgNvim: Read NV Failed!");

        return;
    }

    CNAS_MNTN_LogReadNVInfo(en_NV_Item_MRU0_SWITCH_ON_OOC_STRATEGY_CFG,
                            sizeof(CNAS_NVIM_MRU0_SWITCH_ON_OOC_STRATEGY_CFG_STRU),
                            UEPS_PID_XSD,
                            (VOS_UINT8 *)&stNvimMru0StategyCfg);

    if (stNvimMru0StategyCfg.usMru0Times >= CNAS_XSD_MAX_INSERTED_MRU0_NUM)
    {
        stNvimMru0StategyCfg.usMru0Times = CNAS_XSD_MAX_INSERTED_MRU0_NUM;
    }

    CNAS_XSD_SetSwitchOnOocMru0EnableFlg(stNvimMru0StategyCfg.ucEnable);
    CNAS_XSD_SetSwitchOnOocMru0Times(stNvimMru0StategyCfg.usMru0Times);

    return;
}



VOS_VOID CNAS_XSD_Read1xPRLRoamIndStrategyNvim(VOS_VOID)
{
    CNAS_NVIM_1X_PRL_ROAM_IND_STRATEGY_CFG_STRU             st1xPRLRoamIndStrategy;

    NAS_MEM_SET_S(&st1xPRLRoamIndStrategy, sizeof(CNAS_NVIM_1X_PRL_ROAM_IND_STRATEGY_CFG_STRU), 0x00, sizeof(CNAS_NVIM_1X_PRL_ROAM_IND_STRATEGY_CFG_STRU));

    if (NV_OK != NV_Read(en_NV_Item_1X_PRL_ROAM_IND_STRATEGY_CFG,
                        &st1xPRLRoamIndStrategy, sizeof(CNAS_NVIM_1X_PRL_ROAM_IND_STRATEGY_CFG_STRU)))
    {
        CNAS_XSD_SetIsConsider1xRoamIndInSysDeterminationFlg(VOS_FALSE);

        CNAS_ERROR_LOG4(UEPS_PID_XSD, "CNAS_XSD_Read1xPRLRoamIndStrategyNvim: Read Nvim Failed", 9, 1, 0, 0);

        return;
    }

    if (VOS_TRUE != st1xPRLRoamIndStrategy.ucIsConsiderRoamIndInPRLFlg)
    {
        CNAS_XSD_SetIsConsider1xRoamIndInSysDeterminationFlg(VOS_FALSE);

        return;
    }

    CNAS_XSD_SetIsConsider1xRoamIndInSysDeterminationFlg(VOS_TRUE);

    return;
}


VOS_VOID CNAS_XSD_Read1xDoSysAcqNoRfProtectTimerCfgNvim(VOS_VOID)
{
    CNAS_NVIM_1X_DO_SYS_ACQ_NO_RF_PROTECT_TIMER_CFG_STRU                st1xDoSysAcqNoRfPrtTimerCfg;

    NAS_MEM_SET_S(&st1xDoSysAcqNoRfPrtTimerCfg, sizeof(st1xDoSysAcqNoRfPrtTimerCfg), 0, sizeof(st1xDoSysAcqNoRfPrtTimerCfg));

    if (NV_OK != NV_Read(en_NV_Item_1X_DO_SYS_ACQ_NO_RF_PROTECT_TIMER_CFG,
                         &st1xDoSysAcqNoRfPrtTimerCfg, sizeof(st1xDoSysAcqNoRfPrtTimerCfg)))
    {
        CNAS_XSD_SetSysAcqNoRfProtectTimerLen(TI_CNAS_XSD_SYS_ACQ_NO_RF_PROTECT_TIMER_LEN);
        CNAS_XSD_SetRedirNoRfProtectTimerLen(TI_CNAS_XSD_REDIR_NO_RF_PROTECT_TIMER_LEN);

        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_Read1xDoSysAcqNoRfProtectTimerCfgNvim: Read Nvim Failed");

        return;

    }

    if (0 == st1xDoSysAcqNoRfPrtTimerCfg.ul1xSysAcqNoRfProtectTimerLen)
    {
        CNAS_XSD_SetSysAcqNoRfProtectTimerLen(TI_CNAS_XSD_SYS_ACQ_NO_RF_PROTECT_TIMER_LEN);
    }
    else
    {
        CNAS_XSD_SetSysAcqNoRfProtectTimerLen(st1xDoSysAcqNoRfPrtTimerCfg.ul1xSysAcqNoRfProtectTimerLen);
    }

    if (0 == st1xDoSysAcqNoRfPrtTimerCfg.ul1xRedirNoRfProtectTimerLen)
    {
        CNAS_XSD_SetRedirNoRfProtectTimerLen(TI_CNAS_XSD_REDIR_NO_RF_PROTECT_TIMER_LEN);
    }
    else
    {
        CNAS_XSD_SetRedirNoRfProtectTimerLen(st1xDoSysAcqNoRfPrtTimerCfg.ul1xRedirNoRfProtectTimerLen);
    }

    return;
}


VOS_VOID CNAS_XSD_ReadLteOrDoConn1xSysAcqSyncDelayInfoNvim(VOS_VOID)
{
    CNAS_NVIM_LTE_OR_DO_CONN_1X_SYS_ACQ_SYNC_DELAY_INFO_STRU                    st1xSysAcqSyncDelayInfo;

    NAS_MEM_SET_S(&st1xSysAcqSyncDelayInfo, sizeof(st1xSysAcqSyncDelayInfo), 0, sizeof(st1xSysAcqSyncDelayInfo));

    if (NV_OK != NV_Read(en_NV_Item_1X_SYS_ACQ_SYNC_DELAY_INFO_WHEN_LTE_OR_DO_CONN,
                         &st1xSysAcqSyncDelayInfo, sizeof(st1xSysAcqSyncDelayInfo)))
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_ReadLteOrDoConn1xSysAcqSyncDelayInfoNvim: Read Nvim Failed");

        return;
    }

    if (VOS_TRUE == st1xSysAcqSyncDelayInfo.uc1xSysAcqSyncDelayEnable)
    {
        CNAS_XSD_SetLteOrDoConn1xSysAcqSyncDelayTimerLen(st1xSysAcqSyncDelayInfo.ul1xSysAcqDelayTimerLen);
        CNAS_XSD_SetLteOrDoConn1xSysAcqSyncDelayFlag(VOS_TRUE);
    }
    return;
}



VOS_VOID CNAS_XSD_ReadCbtPrlNvim(VOS_VOID)
{
    CNAS_NVIM_CBT_PRL_LIST_STRU                            *pPrlData = VOS_NULL_PTR;
    VOS_UINT32                                              ulRslt;

    /* 读取PRL前，为PRL码流分配临时内存空间 */
    pPrlData = (CNAS_NVIM_CBT_PRL_LIST_STRU *)PS_MEM_ALLOC(UEPS_PID_XSD, sizeof(CNAS_NVIM_CBT_PRL_LIST_STRU));

    if (VOS_NULL_PTR != pPrlData)
    {
        NAS_MEM_SET_S(pPrlData, sizeof(CNAS_NVIM_CBT_PRL_LIST_STRU), 0, sizeof(CNAS_NVIM_CBT_PRL_LIST_STRU));

        if (NV_OK != NV_Read(en_NV_Item_CBT_PRL_LIST, pPrlData, sizeof(CNAS_NVIM_CBT_PRL_LIST_STRU)))
        {
            CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_ReadCbtPrlNvim: Read Nvim Failed");

            /* 释放内存 */
            PS_MEM_FREE(UEPS_PID_XSD, pPrlData);

            return;
        }

        /* 默认尝试用EPRL方式解析PRL码流 */
        ulRslt = CNAS_PRL_ProcPrlInfo(&(pPrlData->ucPrlData[0]), sizeof(CNAS_NVIM_CBT_PRL_LIST_STRU), CNAS_PRL_SSPR_P_REV_3);

        if (VOS_TRUE == ulRslt)
        {
            CNAS_XSD_LogPrlSourceType(CNAS_CCB_PRL_TYPE_NVRAM);
            CNAS_CCB_SetPrlSrcType(CNAS_CCB_PRL_TYPE_NVRAM);
        }
        else
        {
            CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_ReadCbtPrlNvim: parse Nvim PRL Failed");
        }

        /* 释放内存 */
        PS_MEM_FREE(UEPS_PID_XSD, pPrlData);
    }

    return;
}

VOS_VOID CNAS_XSD_ReadSrlteSyncOneChanAddTimerLen( VOS_VOID )
{
    CAS_1X_NVIM_TIMER_LENGTH_STRU       stCas1xNvimTimerLen;

    NAS_MEM_SET_S(&stCas1xNvimTimerLen, sizeof(stCas1xNvimTimerLen), 0, sizeof(stCas1xNvimTimerLen));

    if (NV_OK != NV_Read(en_NV_Item_CAS_1X_NVIM_TIMER_LENGTH,
                         &stCas1xNvimTimerLen, sizeof(CAS_1X_NVIM_TIMER_LENGTH_STRU)))
    {
        CNAS_ERROR_LOG(UEPS_PID_XSD, "CNAS_XSD_ReadSrlteSyncOneChanAddTimerLen: Read Nvim Failed");

        return;
    }

    /* 代码中区分SR或者SV,因此0也是一个有效值，如果是0表示SRLTE下不增加同步时长 */
    CNAS_XSD_GetXsdCtxAddr()->stSysAcqCtrl.ulSrlteSyncOneChanAddTimerLen = stCas1xNvimTimerLen.ucWaitSrlteSysSyncTimerAddLen * 1000;

    return;
}

/*lint -restore*/


#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */




