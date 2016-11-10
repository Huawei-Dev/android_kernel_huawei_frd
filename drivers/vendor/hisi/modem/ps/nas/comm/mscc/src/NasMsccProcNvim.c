

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "PsCommonDef.h"
#include "NasMsccProcNvim.h"
#if (FEATURE_ON == FEATURE_IMS)
#include "ImsaNvInterface.h"
#include "LPsNvInterface.h"
#include "LNvCommon.h"
#endif
#include "NVIM_Interface.h"
#include "NasNvInterface.h"

#include "NasMntn.h"
#include "NasMsccTimerMgmt.h"
#include "NasMsccComFunc.h"

#if (FEATURE_ON == FEATURE_TDS_WCDMA_DYNAMIC_LOAD)
#include "NasDynLoad.h"
#endif


#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_NAS_MSCC_PROC_NVIM_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/*****************************************************************************
  3 函数定义
*****************************************************************************/


VOS_VOID NAS_MSCC_ReadNvim(VOS_VOID)
{
    /* INit the Platform RAT cap structure */
    NAS_MSCC_ReadPlatformRatCapNvim();

    /* Init the IMS Config Parameter Structure */
    NAS_MSCC_ReadImsCfgInfoNvim();

    /* Read the NV System Acquire Info */
    NAS_MSCC_ReadMmssNvimInfo();

#if (FEATURE_ON == FEATURE_TDS_WCDMA_DYNAMIC_LOAD)
    NAS_DYNLOAD_ReadDynloadCtrlNvim();

    NAS_DYNLOAD_ReadDynloadExceptionCtrlNvim();
#endif

    NAS_MSCC_ReadEmcCallBackCfgNvim();

    NAS_MSCC_Read1xServiceClSysAcqStrategyCfgNvim();

    NAS_MSCC_ReadClSysAcqDsdsStrategyCfgNvim();

#if (FEATURE_PTM == FEATURE_ON)
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    NAS_MSCC_ReadErrLogMainActiveReportControlNvim();

    NAS_MSCC_ReadErrorLogActiveReportCdmaControlNvim();
#endif
#endif

    NAS_MSCC_ReadSysAcqBsrCtrlNvim();

    NAS_MSCC_ReadTestConfigNvim();
}


VOS_VOID NAS_MSCC_ReadImsCfgInfoNvim(VOS_VOID)
{
    /* Read NVIM to update the IMS RAT support */
    NAS_MSCC_ReadImsRatSupportNvim();

    /* Init the Voice domain */
    NAS_MSCC_ReadVoiceDomainNvim();

    /* Init the Ims Voice Cap Timer Len */
    NAS_MSCC_ReadWaitImsVoiceCapTimerLenNvim();
}


VOS_VOID NAS_MSCC_ReadImsRatSupportNvim(VOS_VOID)
{
#if (FEATURE_ON == FEATURE_IMS)
    VOS_UINT32                          ulLength;
    IMSA_NV_IMS_RAT_SUPPORT_STRU        stImsSupport;
#endif

    NAS_MSCC_IMS_CONFIG_PARA_STRU      *pstImsCfgInfo;

    pstImsCfgInfo = NAS_MSCC_GetImsCfgInfo();

    /* IMS宏关闭时，默认初始化为不支持IMS */
    PS_MEM_SET(&(pstImsCfgInfo->stImsRatSupport), 0, sizeof(NAS_MSCC_IMS_RAT_SUPPORT_STRU));

    pstImsCfgInfo->stImsRatSupport.ucWifiImsSupportFlag    = VOS_FALSE;
    pstImsCfgInfo->stImsRatSupport.ucUtranImsSupportFlag   = VOS_FALSE;
    pstImsCfgInfo->stImsRatSupport.ucLteImsSupportFlag     = VOS_FALSE;
    pstImsCfgInfo->stImsRatSupport.ucWifiEmsSupportFlag    = VOS_FALSE;
    pstImsCfgInfo->stImsRatSupport.ucUtranEmsSupportFlag   = VOS_FALSE;
    pstImsCfgInfo->stImsRatSupport.ucLteEmsSupportFlag     = VOS_FALSE;
    pstImsCfgInfo->stImsRatSupport.ucRoamingImsSupportFlag = VOS_FALSE;

    /* IMS能力只有在FEATURE_IMS打开时，才有可能设置为开启 */
#if (FEATURE_ON == FEATURE_IMS)
    /* 先获取NV的长度 */
    ulLength = 0;
    (VOS_VOID)NV_GetLength(EN_NV_ID_IMS_RAT_SUPPORT, &ulLength);

    if (ulLength > sizeof(IMSA_NV_IMS_RAT_SUPPORT_STRU))
    {
        NAS_TRACE_HIGH("NAS_MSCC_ReadImsRatSupportNvim(): EN_NV_ID_IMS_RAT_SUPPORT length Error");

        return;
    }

    /* 读NV项EN_NV_ID_IMS_RAT_SUPPORT，失败，直接返回 */
    if (NV_OK != NV_Read(EN_NV_ID_IMS_RAT_SUPPORT, &stImsSupport, ulLength))
    {

        NAS_TRACE_HIGH("NAS_MSCC_ReadImsRatSupportNvim(): read EN_NV_ID_IMS_RAT_SUPPORT Error");

        return;
    }

    /* 赋值到全局变量中 */
    if (VOS_TRUE == stImsSupport.ucWifiImsSupportFlag)
    {
        pstImsCfgInfo->stImsRatSupport.ucWifiImsSupportFlag = VOS_TRUE;
    }

    if (VOS_TRUE == stImsSupport.ucUtranImsSupportFlag)
    {
        pstImsCfgInfo->stImsRatSupport.ucUtranImsSupportFlag = VOS_TRUE;
    }

    if (VOS_TRUE == stImsSupport.ucLteImsSupportFlag)
    {
        pstImsCfgInfo->stImsRatSupport.ucLteImsSupportFlag = VOS_TRUE;
    }

    if (VOS_TRUE == stImsSupport.ucWifiEmsSupportFlag)
    {
        pstImsCfgInfo->stImsRatSupport.ucWifiEmsSupportFlag = VOS_TRUE;
    }

    if (VOS_TRUE == stImsSupport.ucUtranEmsSupportFlag)
    {
        pstImsCfgInfo->stImsRatSupport.ucUtranEmsSupportFlag = VOS_TRUE;
    }

    if (VOS_TRUE == stImsSupport.ucLteEmsSupportFlag)
    {
        pstImsCfgInfo->stImsRatSupport.ucLteEmsSupportFlag = VOS_TRUE;
    }

    if (VOS_FALSE == stImsSupport.ucRoamingImsNotSupportFlag)
    {
        pstImsCfgInfo->stImsRatSupport.ucRoamingImsSupportFlag = VOS_TRUE;
    }
#endif

    return;
}


VOS_VOID NAS_MSCC_ReadVoiceDomainNvim(VOS_VOID)
{
#if (FEATURE_ON == FEATURE_IMS)
    VOS_UINT32                          ulLength;
    LNAS_LMM_NV_VOICE_DOMAIN_STRU       stVoiceDomain;
#endif

    NAS_MSCC_IMS_CONFIG_PARA_STRU      *pstImsCfgInfo;

    pstImsCfgInfo = NAS_MSCC_GetImsCfgInfo();

    /* NV关闭时，默认初始化为CS ONLY */
    pstImsCfgInfo->enVoiceDomain = NAS_MSCC_VOICE_DOMAIN_CS_ONLY;

#if (FEATURE_ON == FEATURE_IMS)
    pstImsCfgInfo->enVoiceDomain = NAS_MSCC_VOICE_DOMAIN_IMS_PS_PREFERRED;

    /* 先获取NV的长度 */
    ulLength = 0;
    (VOS_VOID)NV_GetLength(EN_NV_ID_UE_VOICE_DOMAIN, &ulLength);

    if (ulLength > sizeof(LNAS_LMM_NV_VOICE_DOMAIN_STRU))
    {
        NAS_TRACE_HIGH("NAS_MSCC_ReadVoiceDomainNvim(): EN_NV_ID_UE_VOICE_DOMAIN length Error");

        return;
    }

    /* 读NV项EN_NV_ID_UE_VOICE_DOMAIN，失败，直接返回 */
    if (NV_OK != NV_Read(EN_NV_ID_UE_VOICE_DOMAIN,
                         &stVoiceDomain, ulLength))
    {

        NAS_TRACE_HIGH("NAS_MSCC_ReadVoiceDomainNvim(): read EN_NV_ID_UE_VOICE_DOMAIN Error");

        return;
    }

    /* NV项激活，并且取值合法时，赋值到全局变量中 */
    if ( (VOS_TRUE                              == stVoiceDomain.bitOpVoicDomain)
      && (NAS_LMM_VOICE_DOMAIN_IMS_PS_PREFERRED >= stVoiceDomain.enVoicDomain) )
    {
        pstImsCfgInfo->enVoiceDomain = stVoiceDomain.enVoicDomain;
    }
#endif

    return;
}


VOS_VOID NAS_MSCC_ReadWaitImsVoiceCapTimerLenNvim(VOS_VOID)
{
#if (FEATURE_ON == FEATURE_IMS)
    NAS_NVIM_WAIT_IMS_VOICE_AVAIL_TIMER_LEN_STRU            stImsVoiceAvailTimerLen;
    VOS_UINT32                                              ulLength;
#endif
    NAS_MSCC_IMS_CONFIG_PARA_STRU                          *pstImsCfgInfo;

    pstImsCfgInfo = NAS_MSCC_GetImsCfgInfo();

    /* 定时器时长与WIFI关联 */
    if (VOS_TRUE == pstImsCfgInfo->stImsRatSupport.ucWifiImsSupportFlag)
    {
        pstImsCfgInfo->ulWaitImsVoiceAvailTimerLen = TI_NAS_MSCC_WAIT_IMSA_IMS_WITH_WIFI_VOICE_AVAIL_IND_LEN;
    }
    else
    {
        pstImsCfgInfo->ulWaitImsVoiceAvailTimerLen = TI_NAS_MSCC_WAIT_IMSA_IMS_VOICE_AVAIL_IND_LEN;
    }

#if (FEATURE_ON == FEATURE_IMS)
    ulLength = 0;

    /* 先获取NV的长度 */
    (VOS_VOID)NV_GetLength(en_NV_Item_WAIT_IMS_VOICE_AVAIL_Timer_Len, &ulLength);

    if (ulLength > sizeof(NAS_NVIM_WAIT_IMS_VOICE_AVAIL_TIMER_LEN_STRU))
    {
        NAS_TRACE_HIGH("NAS_MSCC_ReadWaitImsVoiceCapTimerLenNvim(): en_NV_Item_WAIT_IMS_VOICE_AVAIL_Timer_Len length Error");
        return;
    }

    /* 读NV失败 */
    if (NV_OK != NV_Read(en_NV_Item_WAIT_IMS_VOICE_AVAIL_Timer_Len,
                         &stImsVoiceAvailTimerLen,
                         ulLength))
    {
        NAS_TRACE_HIGH("NAS_MSCC_ReadWaitImsVoiceCapTimerLenNvim(): en_NV_Item_WAIT_IMS_VOICE_AVAIL_Timer_Len error");
        return;
    }

    if (VOS_TRUE == stImsVoiceAvailTimerLen.ucNvimActiveFlag)
    {
        /* NV的长度不为0且支持IMS WIFI,NV的单位为S,需要转换为毫秒 */
        if ((VOS_TRUE == pstImsCfgInfo->stImsRatSupport.ucWifiImsSupportFlag)
         && (0 != stImsVoiceAvailTimerLen.ucWaitImsWithWifiVoiceAvailTimerLen))
        {
            pstImsCfgInfo->ulWaitImsVoiceAvailTimerLen = stImsVoiceAvailTimerLen.ucWaitImsWithWifiVoiceAvailTimerLen * NAS_MSCC_THOUSAND_MILLISECOND_UNIT;
        }

        /* NV的长度不为0且不支持IMS WIFI,NV的单位为S,需要转换为毫秒 */
        if ((VOS_FALSE == pstImsCfgInfo->stImsRatSupport.ucWifiImsSupportFlag)
         && (0 != stImsVoiceAvailTimerLen.ucWaitImsVoiceAvailTimerLen))
        {
            pstImsCfgInfo->ulWaitImsVoiceAvailTimerLen = stImsVoiceAvailTimerLen.ucWaitImsVoiceAvailTimerLen * NAS_MSCC_THOUSAND_MILLISECOND_UNIT;
        }
    }
#endif

    return;
}




VOS_VOID NAS_MSCC_ReadPlatformRatCapNvim(VOS_VOID)
{
    VOS_UINT32                          i;
    VOS_UINT32                          j;
    PLATAFORM_RAT_CAPABILITY_STRU       stNvPlatformRatCap;
    VOS_RATMODE_ENUM_UINT32             enNasMsccPlatformRat;
    NAS_MSCC_PLATFORM_RAT_CAP_STRU     *pstPlatformRatCap;

    pstPlatformRatCap = NAS_MSCC_GetPlatformRatCap();

    /* Initialize the Platform RAT cap structure to contain only GSM */
    pstPlatformRatCap->ucRatNum      = 1;
    pstPlatformRatCap->aenRatList[0] = VOS_RATMODE_GSM;
    for (i = 1; i < VOS_RATMODE_BUTT; i++)
    {
        pstPlatformRatCap->aenRatList[i] = VOS_RATMODE_BUTT;
    }

    /* MemSet the stNvPlatformRatCap local structure to zero. Read NV 9203 to get the platform RAT capability supported*/
    PS_MEM_SET(&stNvPlatformRatCap, 0x0, sizeof(stNvPlatformRatCap));
    if (NV_OK != NV_Read(en_NV_Item_Platform_RAT_CAP,
                         &stNvPlatformRatCap,
                         sizeof(PLATAFORM_RAT_CAPABILITY_STRU)))
    {
        NAS_TRACE_HIGH("NAS_MSCC_ReadPlatformRatCapNvim():ERROR: read en_NV_Item_PLATFORM_RAT_CAP Error");

        return;
    }

    /* If the Number of RAT read from NVIM is greater than maximum, set the Number of Rat to the max value */
    if (stNvPlatformRatCap.usRatNum >= PLATFORM_MAX_RAT_NUM)
    {
        NAS_TRACE_HIGH("NAS_MSCC_ReadPlatformRatCapNvim():ERROR: en_NV_Item_PLATFORM_RAT_CAP Invalid: Number of Platforms greater than MAX");

        return;
    }

    /* Check if the NVIM data is valid */
    if (VOS_FALSE == NAS_MSCC_IsPlatformRatCapNvimValid(&stNvPlatformRatCap))
    {
        NAS_TRACE_HIGH("NAS_MSCC_ReadPlatformRatCapNvim():ERROR: en_NV_Item_PLATFORM_RAT_CAP Invalid");

        return;
    }

    j = 0;

    /* Read the NVIM data and convert the NVIM format to MSCC Enum format. Skip the BUTT plarform types */
    for (i = 0; i < stNvPlatformRatCap.usRatNum; i++)
    {
        enNasMsccPlatformRat = NAS_MSCC_ConvertNvPlatformRatToMsccType(stNvPlatformRatCap.aenRatList[i]);
        if (VOS_RATMODE_BUTT != enNasMsccPlatformRat)
        {
            pstPlatformRatCap->aenRatList[j] = enNasMsccPlatformRat;
            j ++;
        }
    }

    /* If atleast one entry is copied into the pstPlatformRatCap, update the number of RATs to the number read. */
    if (j > 0)
    {
        pstPlatformRatCap->ucRatNum      = (VOS_UINT8)j;
    }

    return;
}


VOS_VOID NAS_MSCC_ReadTestConfigNvim(VOS_VOID)
{
    CNAS_NVIM_NO_CARD_MODE_CFG_STRU     stNoCardModeCfg;

    PS_MEM_SET(&stNoCardModeCfg, 0x0, sizeof(stNoCardModeCfg));

    if (NV_OK != NV_Read(en_NV_Item_NO_CARD_MODE_CFG,
                         &stNoCardModeCfg, sizeof(stNoCardModeCfg)))
    {
        NAS_ERROR_LOG(UEPS_PID_MSCC, "NAS_MSCC_ReadTestConfig: Read NO card mode cfg Nvim Failed");

        NAS_MSCC_SetNoCardModeCfgFlag(VOS_FALSE);
    }
    else
    {
        if (VOS_FALSE == stNoCardModeCfg.ulEnableFlag)
        {
            NAS_MSCC_SetNoCardModeCfgFlag(VOS_FALSE);
        }
        else
        {
            NAS_MSCC_SetNoCardModeCfgFlag(VOS_TRUE);
        }
    }

}



VOS_VOID NAS_MSCC_ReadMmssNvimInfo(VOS_VOID)
{
    /* Read the MSCC Location Info From NVIM */
    NAS_MSCC_ReadLastLocationInfoNvim();

    /* Read the CL System Acquire Cfg NVIM */
    NAS_MSCC_ReadMmssSystemAcquireCfgNvim();
}



VOS_VOID NAS_MSCC_ReadMmssSystemAcquireCfgNvim(VOS_VOID)
{
    NAS_NVIM_MMSS_SYSTEM_ACQUIRE_CFG_STRU                   stNvimMmssSystemAcquireCfg;
    NAS_MSCC_MMSS_SYSACQ_CFG_STRU                          *pstMmssSysAcqCfgInfo = VOS_NULL_PTR;

    PS_MEM_SET(&stNvimMmssSystemAcquireCfg, 0x0, sizeof(NAS_NVIM_MMSS_SYSTEM_ACQUIRE_CFG_STRU));

    /* Get the MMSS Sys acq Cfg Info global variable address */
    pstMmssSysAcqCfgInfo = NAS_MSCC_GetMmssSysAcqCfgInfoAddr();

    if (NV_OK != NV_Read(en_Nv_Item_Mmss_System_Acquire_Cfg,
                     &stNvimMmssSystemAcquireCfg,
                     sizeof(NAS_NVIM_MMSS_SYSTEM_ACQUIRE_CFG_STRU)))
    {
        NAS_ERROR_LOG(UEPS_PID_MSCC, "NAS_MSCC_ReadCLSystemAcquireCfgNvim: NV Read Failed!");

        return;
    }

    NAS_MSCC_LogReadNVInfo(en_Nv_Item_Mmss_System_Acquire_Cfg,
                           sizeof(NAS_NVIM_MMSS_SYSTEM_ACQUIRE_CFG_STRU),
                           UEPS_PID_MSCC,
                           (VOS_UINT8 *)&stNvimMmssSystemAcquireCfg);

    /* Log the MMSS System acquire Cfg */
    NAS_MSCC_MNTN_LogNvimMmssCfgInfo(&stNvimMmssSystemAcquireCfg);

    /* Read the Search LTE on HSD Sync Ind Control Flag from NVIM */
    /* Read the NV Value if the NV value is true or False */
    if ((stNvimMmssSystemAcquireCfg.ucReAcqLteOnHrpdSyncIndFlag == VOS_TRUE)
      ||(stNvimMmssSystemAcquireCfg.ucReAcqLteOnHrpdSyncIndFlag == VOS_FALSE))
    {
        pstMmssSysAcqCfgInfo->ucReAcqLteOnHrpdSyncIndFlag = stNvimMmssSystemAcquireCfg.ucReAcqLteOnHrpdSyncIndFlag;

    }
    /* If the NV Value is neither true nor false, then use the default value of true */
    else
    {
        NAS_ERROR_LOG(UEPS_PID_MSCC, "NAS_MSCC_ReadMsccSearchLteOnHrpdSyncIndSwitch: Invalid Value!");

        /* Initialize to FALSE */
        pstMmssSysAcqCfgInfo->ucReAcqLteOnHrpdSyncIndFlag = VOS_FALSE;
    }

    /* Read the CDMA 1x Preferred flag from NVIM */
    /* Read the NV Value if the NV value is true or False */
    if ((stNvimMmssSystemAcquireCfg.ucIs1xLocInfoPrefThanLte == VOS_TRUE)
      ||(stNvimMmssSystemAcquireCfg.ucIs1xLocInfoPrefThanLte == VOS_FALSE))
    {
        pstMmssSysAcqCfgInfo->ucIs1xLocInfoPrefThanLte = stNvimMmssSystemAcquireCfg.ucIs1xLocInfoPrefThanLte;

    }
    /* If the NV Value is neither true nor false, then use the default value of true */
    else
    {
        NAS_ERROR_LOG(UEPS_PID_MSCC, "NAS_MSCC_ReadCdma1xLocationInfoPreferredFlag: Invalid Value!");

        /* Initialize the flag to contain CDMA 1x preferred */
        pstMmssSysAcqCfgInfo->ucIs1xLocInfoPrefThanLte = VOS_TRUE;

    }

    /* Save the Sys Acq timer Cfg from NVIM */
    NAS_MSCC_SaveSysAcqTimerCfg(&(stNvimMmssSystemAcquireCfg.stMmssSysAcqTimerCfg),
                                &(pstMmssSysAcqCfgInfo->stSysAcqTimerCfg));

    return;
}




VOS_VOID NAS_MSCC_ReadLastLocationInfoNvim(VOS_VOID)
{
    NAS_NVIM_MMSS_LAST_LOCATION_INFO_STRU                   stNvimMmssLocationInfo;
    NAS_MSCC_MMSS_LOCATION_INFO_STRU                       *pstMmssLocationInfo = VOS_NULL_PTR;

    PS_MEM_SET(&stNvimMmssLocationInfo, 0x0, sizeof(NAS_NVIM_MMSS_LAST_LOCATION_INFO_STRU));

    /* Get the MMSS Location Info Global variable address */
    pstMmssLocationInfo = NAS_MSCC_GetMmssLastLocationInfoAddr();

    /* Read the NV Item */
    if (NV_OK != NV_Read(en_Nv_Item_MmssLastLocationInfo,
                         &stNvimMmssLocationInfo,
                         sizeof(NAS_NVIM_MMSS_LAST_LOCATION_INFO_STRU)))
    {
        NAS_ERROR_LOG(UEPS_PID_MSCC, "NAS_MSCC_ReadLastLocationInfoNvim: Read NV Failed!");

        return;
    }

    NAS_MSCC_LogReadNVInfo(en_Nv_Item_MmssLastLocationInfo,
                           sizeof(NAS_NVIM_MMSS_LAST_LOCATION_INFO_STRU),
                           UEPS_PID_MSCC,
                           (VOS_UINT8 *)&stNvimMmssLocationInfo);

    /* Log the MMSS Last Location Info */
    NAS_MSCC_MNTN_LogNvimMmssLastLocInfo(&stNvimMmssLocationInfo);

    /* If the MMSS Location Info valid flag is not TRUE, use the default values written in the previous step */
    if (VOS_TRUE != stNvimMmssLocationInfo.ucIsLocInfoUsedInSwitchOn)

    {
        NAS_ERROR_LOG(UEPS_PID_MSCC, "NAS_MSCC_ReadLastLocationInfoNvim: Location Info NVIM Flag is not active!");

        return;
    }
    /* Read the NVIM Values if the Valid flag is TRUE */
    else
    {

        if (NAS_NVIM_LC_RAT_COMBINED_GUL == stNvimMmssLocationInfo.enSysAcqMode)
        {
            /* Set the System Acquire Mode as GUL in System Acquire Ctrl global variable and return*/
            pstMmssLocationInfo->enSysAcqMode = NAS_MSCC_LC_RAT_COMBINED_GUL;

            /* Set the location info as valid */
            pstMmssLocationInfo->ucIsLocInfoUsedInSwitchOn = VOS_TRUE;
        }
        /* Update the global values to the NV values if Sys Acq Mode is CL */
        else if (NAS_NVIM_LC_RAT_COMBINED_CL == stNvimMmssLocationInfo.enSysAcqMode)
        {
            /* Set the System Acquire Mode as CL */
            pstMmssLocationInfo->enSysAcqMode = NAS_MSCC_LC_RAT_COMBINED_CL;

            /* Update the global values to the NV values */
            pstMmssLocationInfo->ucCdma1xActiveFlag         = stNvimMmssLocationInfo.ucIs1xLocInfoValid;
            pstMmssLocationInfo->ucLteActiveFlag            = stNvimMmssLocationInfo.ucIsLteLocInfoValid;

            /* Update the 1x location info */
            pstMmssLocationInfo->st1xLocationInfo.ulMcc = stNvimMmssLocationInfo.st1xLocInfo.ulMcc;
            pstMmssLocationInfo->st1xLocationInfo.ulMnc = stNvimMmssLocationInfo.st1xLocInfo.ulMnc;
            pstMmssLocationInfo->st1xLocationInfo.usSid = stNvimMmssLocationInfo.st1xLocInfo.usSid;
            pstMmssLocationInfo->st1xLocationInfo.usNid = stNvimMmssLocationInfo.st1xLocInfo.usNid;

            pstMmssLocationInfo->st1xLocationInfo.en1xPrioClass = stNvimMmssLocationInfo.st1xLocInfo.en1xPrioClass;
            pstMmssLocationInfo->st1xLocationInfo.enAIPrioClass = stNvimMmssLocationInfo.st1xLocInfo.enAIPrioClass;

            /* Update the LTE location info */
            pstMmssLocationInfo->stLteLocationInfo.ulMcc = stNvimMmssLocationInfo.st3gppLocInfo.ulMcc;
            pstMmssLocationInfo->stLteLocationInfo.ulMnc = stNvimMmssLocationInfo.st3gppLocInfo.ulMnc;

            pstMmssLocationInfo->stLteLocationInfo.enPrioClass = stNvimMmssLocationInfo.st3gppLocInfo.enPrioClass;

            /* Set the location info as valid */
            pstMmssLocationInfo->ucIsLocInfoUsedInSwitchOn = VOS_TRUE;

        }
        /* Make error log if System Acquire mode is invalid */
        else
        {
            NAS_ERROR_LOG(UEPS_PID_MSCC, "NAS_MSCC_ReadLastLocationInfoNvim: System acquire mode is invalid!");
        }
    }

    return;
}




VOS_VOID NAS_MSCC_WriteMmssLocationInfoNvim(
    NAS_MSCC_MMSS_LOCATION_INFO_STRU                       *pstMmssLocationInfo
)
{
    NAS_NVIM_MMSS_LAST_LOCATION_INFO_STRU    stNvimMmssLocationInfo;

    PS_MEM_SET(&stNvimMmssLocationInfo, 0x0, sizeof(NAS_NVIM_MMSS_LAST_LOCATION_INFO_STRU));

    /* Construct the NVIM MMSS Location info */
    if (NAS_MSCC_LC_RAT_COMBINED_CL == pstMmssLocationInfo->enSysAcqMode)
    {
        stNvimMmssLocationInfo.enSysAcqMode = NAS_NVIM_LC_RAT_COMBINED_CL;
    }
    else
    {
        stNvimMmssLocationInfo.enSysAcqMode = NAS_NVIM_LC_RAT_COMBINED_GUL;
    }

    stNvimMmssLocationInfo.ucIs1xLocInfoValid  = pstMmssLocationInfo->ucCdma1xActiveFlag;
    stNvimMmssLocationInfo.ucIsLteLocInfoValid = pstMmssLocationInfo->ucLteActiveFlag;

    /* Update the 1x Location Info */
    stNvimMmssLocationInfo.st1xLocInfo.ulMcc   = pstMmssLocationInfo->st1xLocationInfo.ulMcc;
    stNvimMmssLocationInfo.st1xLocInfo.ulMnc   = pstMmssLocationInfo->st1xLocationInfo.ulMnc;
    stNvimMmssLocationInfo.st1xLocInfo.usSid   = pstMmssLocationInfo->st1xLocationInfo.usSid;
    stNvimMmssLocationInfo.st1xLocInfo.usNid   = pstMmssLocationInfo->st1xLocationInfo.usNid;
    stNvimMmssLocationInfo.st1xLocInfo.en1xPrioClass = pstMmssLocationInfo->st1xLocationInfo.en1xPrioClass;
    stNvimMmssLocationInfo.st1xLocInfo.enAIPrioClass = pstMmssLocationInfo->st1xLocationInfo.enAIPrioClass;

    /* Update the LTE Location Info */
    stNvimMmssLocationInfo.st3gppLocInfo.ulMcc  = pstMmssLocationInfo->stLteLocationInfo.ulMcc;
    stNvimMmssLocationInfo.st3gppLocInfo.ulMnc  = pstMmssLocationInfo->stLteLocationInfo.ulMnc;
    stNvimMmssLocationInfo.st3gppLocInfo.enPrioClass = pstMmssLocationInfo->stLteLocationInfo.enPrioClass;

    /* Set the NVIM location info as valid */
    stNvimMmssLocationInfo.ucIsLocInfoUsedInSwitchOn = VOS_TRUE;

    if (NV_OK != NV_Write(en_Nv_Item_MmssLastLocationInfo,
                         &stNvimMmssLocationInfo, sizeof(NAS_NVIM_MMSS_LAST_LOCATION_INFO_STRU)))
    {
        /* Make Error log is NV Write Fails */
        NAS_ERROR_LOG(UEPS_PID_MSCC, "NAS_MSCC_WriteMmssLocationInfoNvim:Write Nvim Failed");

        return;
    }

    return;
}



VOS_UINT8 NAS_MSCC_ReadMlplMsplNvim(
    NAS_MSCC_NVIM_MLPL_MSPL_STRU                            *pstNvimMlplMspl
)
{
    if (NV_OK != NV_Read(en_NV_Item_MLPL_MSPL_FILE, pstNvimMlplMspl,
                                            sizeof(NAS_MSCC_NVIM_MLPL_MSPL_STRU)))
    {
        NAS_ERROR_LOG(UEPS_PID_MSCC, "NAS_MSCC_ReadAndParseMlplMsplNvim:ERROR: Read Nvim Failed");

        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_VOID NAS_MSCC_ReadEmcCallBackCfgNvim(VOS_VOID)
{
    CNAS_NVIM_1X_CALLBACK_CFG_STRU      stNvCallBackCfg;

    PS_MEM_SET(&stNvCallBackCfg, 0x00, sizeof(CNAS_NVIM_1X_CALLBACK_CFG_STRU));

    if (NV_OK != NV_Read(en_Nv_Item_EMC_CALLBACK_CFG,
                         &stNvCallBackCfg, sizeof(CNAS_NVIM_1X_CALLBACK_CFG_STRU)))
    {
        NAS_ERROR_LOG(UEPS_PID_MSCC, "NAS_MSCC_ReadCallBackCfgNvim:Read Nvim Failed");

        /* 读取失败 默认关闭CallBack */
        NAS_MSCC_SetEmcCallBackEnableFlag(VOS_FALSE);

        return;
    }

    NAS_MSCC_SetEmcCallBackEnableFlag(stNvCallBackCfg.ulCallBackEnableFlg);

    return;
}


VOS_VOID NAS_MSCC_Read1xServiceClSysAcqStrategyCfgNvim(VOS_VOID)
{
    NAS_NVIM_1X_SERVICE_CL_SYSTEM_ACQUIRE_STRATEGY_CFG_STRU stNvim1xSrvClSysAcqStrategyCfg;
    NAS_MSCC_1X_SERVICE_CL_SYSTEM_ACQUIRE_PATTERN_CFG_STRU *pst1xSrvClSysAcqPatternCfgPhaseOne;
    NAS_MSCC_1X_SERVICE_CL_SYSTEM_ACQUIRE_PATTERN_CFG_STRU *pst1xSrvClSysAcqPatternCfgPhaseTwo;
    NAS_MSCC_1X_SERVICE_CL_SYSTEM_ACQUIRE_CTRL_STRU        *pst1xSrvClSysAcqCtrl;

    VOS_UINT32                                              ulLength;

    PS_MEM_SET(&stNvim1xSrvClSysAcqStrategyCfg, 0x0, sizeof(NAS_NVIM_1X_SERVICE_CL_SYSTEM_ACQUIRE_STRATEGY_CFG_STRU));

    /* 先获取NV的长度 */
    ulLength = 0;
    (VOS_VOID)NV_GetLength(en_NV_Item_1X_SERVICE_CL_SYSTEM_ACQUIRE_STRATEGY_CFG, &ulLength);

    if (ulLength > sizeof(NAS_NVIM_1X_SERVICE_CL_SYSTEM_ACQUIRE_STRATEGY_CFG_STRU))
    {
        NAS_ERROR_LOG(UEPS_PID_MSCC, "NAS_MSCC_Read1xServiceClSysAcqStrategyCfgNvim(): en_NV_Item_Oos_Plmn_Search_Strategy_Cfg length Error");

        return;
    }

    if (NV_OK != NV_Read(en_NV_Item_1X_SERVICE_CL_SYSTEM_ACQUIRE_STRATEGY_CFG,
                         &stNvim1xSrvClSysAcqStrategyCfg,
                         sizeof(NAS_NVIM_1X_SERVICE_CL_SYSTEM_ACQUIRE_STRATEGY_CFG_STRU)))
    {
        NAS_ERROR_LOG(UEPS_PID_MSCC, "NAS_MSCC_Read1xServiceClSysAcqStrategyCfgNvim: Read NV Failed!");

        return;
    }

    NAS_MSCC_LogReadNVInfo(en_NV_Item_1X_SERVICE_CL_SYSTEM_ACQUIRE_STRATEGY_CFG,
                           sizeof(NAS_NVIM_1X_SERVICE_CL_SYSTEM_ACQUIRE_STRATEGY_CFG_STRU),
                           UEPS_PID_MSCC,
                           (VOS_UINT8 *)&stNvim1xSrvClSysAcqStrategyCfg);

    pst1xSrvClSysAcqPatternCfgPhaseOne = NAS_MSCC_Get1xServiceClSysAcqPatternOneCfgAddr();
    pst1xSrvClSysAcqPatternCfgPhaseTwo = NAS_MSCC_Get1xServiceClSysAcqPatternTwoCfgAddr();

    pst1xSrvClSysAcqCtrl = NAS_MSCC_Get1xServiceClSysAcqCtrlAddr();

    pst1xSrvClSysAcqCtrl->uc1xBsrLteActiveFlg = stNvim1xSrvClSysAcqStrategyCfg.stCtrlInfo.uc1xBsrLteActiveFlg;

    if (0 != stNvim1xSrvClSysAcqStrategyCfg.stCtrlInfo.uc1xBsrLteTimerLen)
    {
        pst1xSrvClSysAcqCtrl->uc1xBsrLteTimerLen = stNvim1xSrvClSysAcqStrategyCfg.stCtrlInfo.uc1xBsrLteTimerLen;
    }

    pst1xSrvClSysAcqCtrl->ucSrlte1xBsrLteEnableFlag = stNvim1xSrvClSysAcqStrategyCfg.stCtrlInfo.ucSrlte1xBsrLteEnableFlg;

    /* 每个阶段的历史搜和全频搜次数不能同时配置为0 */
    if ((0 == stNvim1xSrvClSysAcqStrategyCfg.stPhaseOnePatternCfg.ucLteFullBandSrchNum)
     && (0 == stNvim1xSrvClSysAcqStrategyCfg.stPhaseOnePatternCfg.ucLteHistorySrchNum))
    {
        NAS_ERROR_LOG(UEPS_PID_MSCC, "NAS_MSCC_Read1xServiceClSysAcqStrategyCfgNvim: invalid phase one pattern setttings for srch num!");

        return;
    }

    if ((0 == stNvim1xSrvClSysAcqStrategyCfg.stPhaseTwoPatternCfg.ucLteFullBandSrchNum)
     && (0 == stNvim1xSrvClSysAcqStrategyCfg.stPhaseTwoPatternCfg.ucLteHistorySrchNum))
    {
        NAS_ERROR_LOG(UEPS_PID_MSCC, "NAS_MSCC_Read1xServiceClSysAcqStrategyCfgNvim: invalid phase two pattern setttings for srch num!");

        return;
    }

    /* sleep timer长度为0则取默认值 */
    if (0 == stNvim1xSrvClSysAcqStrategyCfg.stPhaseOnePatternCfg.usSleepTimeLen)
    {
        pst1xSrvClSysAcqPatternCfgPhaseOne->usSleepTimeLen = TI_NAS_MSCC_DEFAULT_1X_SERVICE_AVAILABLE_TIMER_PHASE_ONE_SLEEP_TIMER_LEN;
    }
    else
    {
        pst1xSrvClSysAcqPatternCfgPhaseOne->usSleepTimeLen = stNvim1xSrvClSysAcqStrategyCfg.stPhaseOnePatternCfg.usSleepTimeLen;
    }

    pst1xSrvClSysAcqPatternCfgPhaseOne->ucLteFullBandSrchNum = stNvim1xSrvClSysAcqStrategyCfg.stPhaseOnePatternCfg.ucLteFullBandSrchNum;
    pst1xSrvClSysAcqPatternCfgPhaseOne->ucLteHistorySrchNum  = stNvim1xSrvClSysAcqStrategyCfg.stPhaseOnePatternCfg.ucLteHistorySrchNum;
    pst1xSrvClSysAcqPatternCfgPhaseOne->usTotalTimeLen    = stNvim1xSrvClSysAcqStrategyCfg.stPhaseOnePatternCfg.usTotalTimeLen;

    /* 2阶段信息 */
    if (0 == stNvim1xSrvClSysAcqStrategyCfg.stPhaseTwoPatternCfg.usSleepTimeLen)
    {
        pst1xSrvClSysAcqPatternCfgPhaseTwo->usSleepTimeLen = TI_NAS_MSCC_DEFAULT_1X_SERVICE_AVAILABLE_TIMER_PHASE_TWO_SLEEP_TIMER_LEN;
    }
    else
    {
        pst1xSrvClSysAcqPatternCfgPhaseTwo->usSleepTimeLen = stNvim1xSrvClSysAcqStrategyCfg.stPhaseTwoPatternCfg.usSleepTimeLen;
    }

    pst1xSrvClSysAcqPatternCfgPhaseTwo->ucLteFullBandSrchNum = stNvim1xSrvClSysAcqStrategyCfg.stPhaseTwoPatternCfg.ucLteFullBandSrchNum;
    pst1xSrvClSysAcqPatternCfgPhaseTwo->ucLteHistorySrchNum  = stNvim1xSrvClSysAcqStrategyCfg.stPhaseTwoPatternCfg.ucLteHistorySrchNum;
    pst1xSrvClSysAcqPatternCfgPhaseTwo->usTotalTimeLen    = stNvim1xSrvClSysAcqStrategyCfg.stPhaseTwoPatternCfg.usTotalTimeLen;

    /* reserve:目前只读取第一和第二阶段的信息,后续的扩展使用 */

    NAS_MSCC_MNTN_LogClSystemAcquireStrategyRelatedInfo();

    return;
}


VOS_VOID NAS_MSCC_ReadClSysAcqDsdsStrategyCfgNvim(VOS_VOID)
{
    NAS_NVIM_CL_SYSTEM_ACQUIRE_DSDS_STRATEGY_CFG_STRU       stNvimClSysAcqDsdsStrategyCfg;
    VOS_UINT32                                              ulLength;
    NAS_MSCC_CL_SYSTEM_ACQUIRE_DSDS_STRATEGY_STRU          *pstClSysAcqDsdsStrategy;

    PS_MEM_SET(&stNvimClSysAcqDsdsStrategyCfg, 0x0, sizeof(NAS_NVIM_CL_SYSTEM_ACQUIRE_DSDS_STRATEGY_CFG_STRU));

    /* 先获取NV的长度 */
    ulLength = 0;
    (VOS_VOID)NV_GetLength(en_NV_Item_CL_SYSTEM_ACQUIRE_DSDS_STRATEGY_CFG, &ulLength);

    if (ulLength > sizeof(NAS_NVIM_CL_SYSTEM_ACQUIRE_DSDS_STRATEGY_CFG_STRU))
    {
        NAS_ERROR_LOG(UEPS_PID_MSCC, "NAS_MSCC_ReadClSysAcqDsdsStrategyCfgNvim(): en_NV_Item_CL_SYSTEM_ACQUIRE_DSDS_STRATEGY_CFG length Error");

        return;
    }

    if (NV_OK != NV_Read(en_NV_Item_CL_SYSTEM_ACQUIRE_DSDS_STRATEGY_CFG,
                         &stNvimClSysAcqDsdsStrategyCfg,
                         sizeof(NAS_NVIM_CL_SYSTEM_ACQUIRE_DSDS_STRATEGY_CFG_STRU)))
    {
        NAS_ERROR_LOG(UEPS_PID_MSCC, "NAS_MSCC_ReadClSysAcqDsdsStrategyCfgNvim: Read NV Failed!");

        return;
    }

    NAS_MSCC_LogReadNVInfo(en_NV_Item_CL_SYSTEM_ACQUIRE_DSDS_STRATEGY_CFG,
                           sizeof(NAS_NVIM_CL_SYSTEM_ACQUIRE_DSDS_STRATEGY_CFG_STRU),
                           UEPS_PID_MSCC,
                           (VOS_UINT8 *)&stNvimClSysAcqDsdsStrategyCfg);

    pstClSysAcqDsdsStrategy = NAS_MSCC_GetClSysAcqDsdsStrategyCfgAddr();

    /* disable则不读写其他值 */
    if (0 == stNvimClSysAcqDsdsStrategyCfg.ucReAcqLteWithNoRfEnable)
    {
        pstClSysAcqDsdsStrategy->ucReAcqLteWithNoRfEnable = VOS_FALSE;

        return;
    }

    pstClSysAcqDsdsStrategy->ucReAcqLteWithNoRfEnable = VOS_TRUE;

    if (0 != stNvimClSysAcqDsdsStrategyCfg.ucReAcqLteWithNoRfDelayTime)
    {
        pstClSysAcqDsdsStrategy->ucReAcqLteWithNoRfDelayTime = stNvimClSysAcqDsdsStrategyCfg.ucReAcqLteWithNoRfDelayTime;
    }
    else
    {
        pstClSysAcqDsdsStrategy->ucReAcqLteWithNoRfDelayTime = TI_NAS_MSCC_WAIT_MMC_RF_AVAILABLE_IND_LEN;
    }

    if (NAS_NVIM_CL_SYS_ACQ_DSDS_STRATEGY_SCENE_SWITCH_ON == stNvimClSysAcqDsdsStrategyCfg.enReAcqLteWithNoRfScene)
    {
        pstClSysAcqDsdsStrategy->enReAcqLteWithNoRfScene  = NAS_MSCC_CL_SYS_ACQ_DSDS_STRATEGY_SCENE_SWITCH_ON;
    }
    else if (NAS_NVIM_CL_SYS_ACQ_DSDS_STRATEGY_SCENE_SWITCH_ON_AND_SYSCFG_SET == stNvimClSysAcqDsdsStrategyCfg.enReAcqLteWithNoRfScene)
    {
        pstClSysAcqDsdsStrategy->enReAcqLteWithNoRfScene  = NAS_MSCC_CL_SYS_ACQ_DSDS_STRATEGY_SCENE_SWITCH_ON_AND_SYSCFG_SET;
    }
    else
    {
        pstClSysAcqDsdsStrategy->enReAcqLteWithNoRfScene  = NAS_MSCC_CL_SYS_ACQ_DSDS_STRATEGY_SCENE_ANY;
    }

    return;
}


#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
#if (FEATURE_PTM == FEATURE_ON)

VOS_VOID NAS_MSCC_ReadErrLogMainActiveReportControlNvim(VOS_VOID)
{
    NV_ID_ERR_LOG_CTRL_INFO_STRU                            stErrLogCtrlInfo;
    VOS_UINT32                                              ulResult;
    VOS_UINT32                                              ulLength;

    ulLength = 0;

    NAS_MEM_SET_S(&stErrLogCtrlInfo, sizeof(NV_ID_ERR_LOG_CTRL_INFO_STRU), 0x00, sizeof(NV_ID_ERR_LOG_CTRL_INFO_STRU));

    (VOS_VOID)NV_GetLength(en_NV_Item_ErrLogCtrlInfo, &ulLength);

    ulResult = NV_Read(en_NV_Item_ErrLogCtrlInfo,
                       &stErrLogCtrlInfo,
                       ulLength);

    if (NV_OK == ulResult)
    {
        if (VOS_FALSE == stErrLogCtrlInfo.ucAlmStatus)
        {
            NAS_ERRLOG_CCB_SetMmaMainActiveReportFlag(VOS_FALSE);
            NAS_ERRLOG_CCB_SetXccMainActiveReportFlag(VOS_FALSE);
            NAS_ERRLOG_CCB_SetHsmMainActiveReportFlag(VOS_FALSE);
            NAS_ERRLOG_CCB_SetXregMainActiveReportFlag(VOS_FALSE);
            NAS_ERRLOG_CCB_SetXsmsMainActiveReportFlag(VOS_FALSE);
        }
        else
        {
            NAS_ERRLOG_CCB_SetMmaMainActiveReportFlag(VOS_TRUE);
            NAS_ERRLOG_CCB_SetXccMainActiveReportFlag(VOS_TRUE);
            NAS_ERRLOG_CCB_SetHsmMainActiveReportFlag(VOS_TRUE);
            NAS_ERRLOG_CCB_SetXregMainActiveReportFlag(VOS_TRUE);
            NAS_ERRLOG_CCB_SetXsmsMainActiveReportFlag(VOS_TRUE);
        }

        NAS_ERRLOG_CCB_SetMmaMainActiveReportLevel(stErrLogCtrlInfo.ucAlmLevel);
        NAS_ERRLOG_CCB_SetXccMainActiveReportLevel(stErrLogCtrlInfo.ucAlmLevel);
        NAS_ERRLOG_CCB_SetHsmMainActiveReportLevel(stErrLogCtrlInfo.ucAlmLevel);
        NAS_ERRLOG_CCB_SetXregMainActiveReportLevel(stErrLogCtrlInfo.ucAlmLevel);
        NAS_ERRLOG_CCB_SetXsmsMainActiveReportLevel(stErrLogCtrlInfo.ucAlmLevel);

        NAS_MSCC_LogReadNVInfo(en_NV_Item_ErrLogCtrlInfo,
                                sizeof(NV_ID_ERR_LOG_CTRL_INFO_STRU),
                                UEPS_PID_MSCC,
                                (VOS_UINT8 *)&stErrLogCtrlInfo);
    }
    else
    {
        NAS_ERRLOG_CCB_SetMmaMainActiveReportFlag(VOS_FALSE);
        NAS_ERRLOG_CCB_SetXccMainActiveReportFlag(VOS_FALSE);
        NAS_ERRLOG_CCB_SetHsmMainActiveReportFlag(VOS_FALSE);
        NAS_ERRLOG_CCB_SetXregMainActiveReportFlag(VOS_FALSE);
        NAS_ERRLOG_CCB_SetXsmsMainActiveReportFlag(VOS_FALSE);

        NAS_ERRLOG_CCB_SetMmaMainActiveReportLevel(NAS_ERR_LOG_CTRL_LEVEL_CRITICAL);
        NAS_ERRLOG_CCB_SetXccMainActiveReportLevel(NAS_ERR_LOG_CTRL_LEVEL_CRITICAL);
        NAS_ERRLOG_CCB_SetHsmMainActiveReportLevel(NAS_ERR_LOG_CTRL_LEVEL_CRITICAL);
        NAS_ERRLOG_CCB_SetXregMainActiveReportLevel(NAS_ERR_LOG_CTRL_LEVEL_CRITICAL);
        NAS_ERRLOG_CCB_SetXsmsMainActiveReportLevel(NAS_ERR_LOG_CTRL_LEVEL_CRITICAL);

        NAS_ERROR_LOG(UEPS_PID_MSCC, "NAS_MSCC_ReadErrLogMainActiveReportControlNvim: Read Nvim Failed");
    }
}


VOS_VOID NAS_MSCC_ConfigurateErrorLogCdma1xOosActiveReportFlag(
    NAS_NVIM_CDMA_ERR_LOG_ACTIVE_REPORT_CONTRL_STRU         stTafErrLogActiveReportControl
)
{
    if (VOS_TRUE == stTafErrLogActiveReportControl.uc1xOosReportEnable)
    {
        NAS_ERRLOG_CCB_SetCdma1xOosActiveReportFlag(VOS_TRUE);
    }
    else
    {
        NAS_ERRLOG_CCB_SetCdma1xOosActiveReportFlag(VOS_FALSE);
    }

    return;
}


VOS_VOID NAS_MSCC_ConfigurateErrorLogCdma1xMtServiceExceptionActiveReportFlag(
    NAS_NVIM_CDMA_ERR_LOG_ACTIVE_REPORT_CONTRL_STRU         stTafErrLogActiveReportControl
)
{
    if (VOS_TRUE == stTafErrLogActiveReportControl.uc1xMtServiceExceptionReportEnable)
    {
        NAS_ERRLOG_CCB_SetCdma1xMtServiceExceptionActiveReportFlag(VOS_TRUE);
    }
    else
    {
        NAS_ERRLOG_CCB_SetCdma1xMtServiceExceptionActiveReportFlag(VOS_FALSE);
    }

    return;
}


VOS_VOID NAS_MSCC_ConfigurateErrorLogCdmaHrpdUatiFailActiveReportFlag(
    NAS_NVIM_CDMA_ERR_LOG_ACTIVE_REPORT_CONTRL_STRU         stTafErrLogActiveReportControl
)
{
    if (VOS_TRUE == stTafErrLogActiveReportControl.ucHrpdUatiFailReportEnable)
    {
        NAS_ERRLOG_CCB_SetCdmaHrpdUatiFailActiveReportFlag(VOS_TRUE);
    }
    else
    {
        NAS_ERRLOG_CCB_SetCdmaHrpdUatiFailActiveReportFlag(VOS_FALSE);
    }

    return;
}


VOS_VOID NAS_MSCC_ConfigurateErrorLogCdmaHrpdSessionFailActiveReportFlag(
    NAS_NVIM_CDMA_ERR_LOG_ACTIVE_REPORT_CONTRL_STRU         stTafErrLogActiveReportControl
)
{
    if (VOS_TRUE == stTafErrLogActiveReportControl.ucHrpdSessionFailReportEnable)
    {
        NAS_ERRLOG_CCB_SetCdmaHrpdSessionFailActiveReportFlag(VOS_TRUE);
    }
    else
    {
        NAS_ERRLOG_CCB_SetCdmaHrpdSessionFailActiveReportFlag(VOS_FALSE);
    }

    return;
}


VOS_VOID NAS_MSCC_ConfigurateErrorLogCdmaHrpdSessionExcepDeactActiveReportFlag(
    NAS_NVIM_CDMA_ERR_LOG_ACTIVE_REPORT_CONTRL_STRU         stTafErrLogActiveReportControl
)
{
    if (VOS_TRUE == stTafErrLogActiveReportControl.ucHrpdSessionExceptionDeactReportEnable)
    {
        NAS_ERRLOG_CCB_SetCdmaHrpdSessionExcepDeactActiveReportFlag(VOS_TRUE);
    }
    else
    {
        NAS_ERRLOG_CCB_SetCdmaHrpdSessionExcepDeactActiveReportFlag(VOS_FALSE);
    }

    return;
}


VOS_VOID NAS_MSCC_ConfigurateErrorLogCdmaHrpdOrLteOosReportFlag(
    NAS_NVIM_CDMA_ERR_LOG_ACTIVE_REPORT_CONTRL_STRU         stTafErrLogActiveReportControl
)
{
    if (VOS_TRUE == stTafErrLogActiveReportControl.uctHrpdOrLteOosReportEnable)
    {
        NAS_ERRLOG_CCB_SetCdmaHrpdOrLteOosReportFlag(VOS_TRUE);
    }
    else
    {
        NAS_ERRLOG_CCB_SetCdmaHrpdOrLteOosReportFlag(VOS_FALSE);
    }
    return;
}


VOS_VOID NAS_MSCC_ConfigurateErrorLogCdmaXregFailReportFlag(
    NAS_NVIM_CDMA_ERR_LOG_ACTIVE_REPORT_CONTRL_STRU         stTafErrLogActiveReportControl
)
{
    if (VOS_TRUE == stTafErrLogActiveReportControl.uctXregResltReportEnable)
    {
        NAS_ERRLOG_CCB_SetCdmaXregResltReportFlag(VOS_TRUE);
    }
    else
    {
        NAS_ERRLOG_CCB_SetCdmaXregResltReportFlag(VOS_FALSE);
    }

    return;
}



VOS_VOID NAS_MSCC_ConfigurateErrorLogCdmaXsmsReportFlag(
    NAS_NVIM_CDMA_ERR_LOG_ACTIVE_REPORT_CONTRL_STRU         stTafErrLogActiveReportControl
)
{
    if (VOS_TRUE == stTafErrLogActiveReportControl.ucXsmsReportEnable)
    {
        NAS_ERRLOG_CCB_SetCdmaXsmsReportFlag(VOS_TRUE);
    }
    else
    {
        NAS_ERRLOG_CCB_SetCdmaXsmsReportFlag(VOS_FALSE);
    }

    return;
}


VOS_VOID NAS_MSCC_ReadErrorLogActiveReportCdmaControlNvim(VOS_VOID)
{
    NAS_NVIM_CDMA_ERR_LOG_ACTIVE_REPORT_CONTRL_STRU         stTafErrLogActiveReportControl;

    PS_MEM_SET(&stTafErrLogActiveReportControl, 0x00, sizeof(NAS_NVIM_CDMA_ERR_LOG_ACTIVE_REPORT_CONTRL_STRU));

    if (NV_OK != NV_Read(en_NV_Item_CDMA_ERR_LOG_ACTIVE_REPORT_CONTROL,
                         (VOS_VOID *)&stTafErrLogActiveReportControl,
                         sizeof(NAS_NVIM_CDMA_ERR_LOG_ACTIVE_REPORT_CONTRL_STRU)))
    {
        NAS_ERROR_LOG(UEPS_PID_MSCC,"TAF_MMA_ReadErrorLogActiveReportCdmaControlNvim: return error");

        /* 读取nv失败，X模CHR上报功能默认为关闭 */
        NAS_ERRLOG_CCB_SetCdma1xOosActiveReportFlag(VOS_FALSE);

        NAS_ERRLOG_CCB_SetCdma1xMtServiceExceptionActiveReportFlag(VOS_FALSE);

        NAS_ERRLOG_CCB_SetCdmaHrpdUatiFailActiveReportFlag(VOS_FALSE);

        NAS_ERRLOG_CCB_SetCdmaHrpdSessionFailActiveReportFlag(VOS_FALSE);

        NAS_ERRLOG_CCB_SetCdmaHrpdSessionExcepDeactActiveReportFlag(VOS_FALSE);
        NAS_ERRLOG_CCB_SetCdmaHrpdOrLteOosReportFlag(VOS_FALSE);
        NAS_ERRLOG_CCB_SetCdmaXregResltReportFlag(VOS_FALSE);

        NAS_ERRLOG_CCB_SetCdmaXregResltReportFlag(VOS_FALSE);

        return;
    }

    NAS_MSCC_LogReadNVInfo(en_NV_Item_CDMA_ERR_LOG_ACTIVE_REPORT_CONTROL,
                          sizeof(NAS_NVIM_CDMA_ERR_LOG_ACTIVE_REPORT_CONTRL_STRU),
                          UEPS_PID_MSCC,
                          (VOS_UINT8 *)&stTafErrLogActiveReportControl);

    /* 读取nv成功，根据NV配置X模CHR上报功能 */
    NAS_MSCC_ConfigurateErrorLogCdma1xOosActiveReportFlag(stTafErrLogActiveReportControl);

    NAS_MSCC_ConfigurateErrorLogCdma1xMtServiceExceptionActiveReportFlag(stTafErrLogActiveReportControl);

    NAS_MSCC_ConfigurateErrorLogCdmaHrpdUatiFailActiveReportFlag(stTafErrLogActiveReportControl);

    NAS_MSCC_ConfigurateErrorLogCdmaHrpdSessionFailActiveReportFlag(stTafErrLogActiveReportControl);

    NAS_MSCC_ConfigurateErrorLogCdmaHrpdSessionExcepDeactActiveReportFlag(stTafErrLogActiveReportControl);
    NAS_MSCC_ConfigurateErrorLogCdmaHrpdOrLteOosReportFlag(stTafErrLogActiveReportControl);
    NAS_MSCC_ConfigurateErrorLogCdmaXregFailReportFlag(stTafErrLogActiveReportControl);

    NAS_MSCC_ConfigurateErrorLogCdmaXsmsReportFlag(stTafErrLogActiveReportControl);

    return;
}

#endif
#endif


VOS_VOID NAS_MSCC_ReadSysAcqBsrCtrlNvim(VOS_VOID)
{
    NAS_NVIM_SYS_ACQ_BSR_CTRL_STRU                          stNvimSysAcqBsrCtrl;
    NAS_MSCC_SYS_ACQ_BSR_CTRL_STRU                         *pstSysAcqBsrCtrl = VOS_NULL_PTR;
    VOS_UINT32                                              ulLength;
    VOS_UINT32                                              i;
    VOS_UINT32                                              ulBsrTimerLen;
    VOS_UINT32                                              ulBsrTimerLenWithNoMatchedMsplRec;

    NAS_MEM_SET_S(&stNvimSysAcqBsrCtrl, sizeof(stNvimSysAcqBsrCtrl), 0x0, sizeof(stNvimSysAcqBsrCtrl));

    pstSysAcqBsrCtrl = NAS_MSCC_GetBsrCtrl();
    i                                 = 0;
    ulLength                          = 0;
    ulBsrTimerLen                     = TI_NAS_MSCC_BSR_TIMER_LEN;
    ulBsrTimerLenWithNoMatchedMsplRec = TI_NAS_MSCC_DEFAULT_BSR_TIMER_LEN_WITH_UNMATCHED_MSPL_REC;

    /* 先获取NV的长度 */
    (VOS_VOID)NV_GetLength(en_NV_Item_SYS_ACQ_BSR_CTRL_CFG, &ulLength);

    if (ulLength > sizeof(NAS_NVIM_SYS_ACQ_BSR_CTRL_STRU))
    {
        NAS_ERROR_LOG(UEPS_PID_MSCC, "NAS_MSCC_ReadSysAcqBsrCtrlNvim(): en_NV_Item_SYS_ACQ_BSR_CTRL_CFG length Error");

        return;
    }

    if (NV_OK != NV_Read(en_NV_Item_SYS_ACQ_BSR_CTRL_CFG,
                         &stNvimSysAcqBsrCtrl,
                         sizeof(NAS_NVIM_SYS_ACQ_BSR_CTRL_STRU)))
    {
        NAS_ERROR_LOG(UEPS_PID_MSCC, "NAS_MSCC_ReadSysAcqBsrCtrlNvim: Read NV Failed!");

        return;
    }

    NAS_MSCC_LogReadNVInfo(en_NV_Item_SYS_ACQ_BSR_CTRL_CFG,
                           sizeof(NAS_NVIM_SYS_ACQ_BSR_CTRL_STRU),
                           UEPS_PID_MSCC,
                           (VOS_UINT8 *)&stNvimSysAcqBsrCtrl);

    if (0 == stNvimSysAcqBsrCtrl.ucEhrpdConnBsrActiveFlg)
    {
        pstSysAcqBsrCtrl->ucEhrpdConnBsrActiveFlg = VOS_FALSE;
    }
    else
    {
        pstSysAcqBsrCtrl->ucEhrpdConnBsrActiveFlg = VOS_TRUE;
    }

    if (0 == stNvimSysAcqBsrCtrl.ucHrpdConnBsrActiveFlg)
    {
        pstSysAcqBsrCtrl->ucHrpdConnBsrActiveFlg = VOS_FALSE;
    }
    else
    {
        pstSysAcqBsrCtrl->ucHrpdConnBsrActiveFlg = VOS_TRUE;
    }

    if (VOS_TRUE != stNvimSysAcqBsrCtrl.ucBsrTimerActivateFlag)
    {
        pstSysAcqBsrCtrl->ucBsrTimerActivateFlag = VOS_FALSE;

        return;
    }

    pstSysAcqBsrCtrl->ucBsrTimerActivateFlag = VOS_TRUE;

    if (0 == stNvimSysAcqBsrCtrl.ucBsrPhaseNum)
    {
        return;
    }

    pstSysAcqBsrCtrl->ucBsrPhaseNum          = ((stNvimSysAcqBsrCtrl.ucBsrPhaseNum > NAS_MSCC_MAX_BSR_PHASE_NUM) ? NAS_MSCC_MAX_BSR_PHASE_NUM : stNvimSysAcqBsrCtrl.ucBsrPhaseNum);

    for (i = 0; i < pstSysAcqBsrCtrl->ucBsrPhaseNum; i++)
    {
        ulBsrTimerLen                     = TI_NAS_MSCC_BSR_TIMER_LEN;
        ulBsrTimerLenWithNoMatchedMsplRec = TI_NAS_MSCC_DEFAULT_BSR_TIMER_LEN_WITH_UNMATCHED_MSPL_REC;

        if ((0 == stNvimSysAcqBsrCtrl.astBsrTimerInfo[i].ucMaxHistorySrchTimes)
         && (0 == stNvimSysAcqBsrCtrl.astBsrTimerInfo[i].ucMaxPrefBandSrchTimes))
        {
            NAS_ERROR_LOG(UEPS_PID_MSCC, "NAS_MSCC_ReadSysAcqBsrCtrlNvim: both history srch times and pref band srch times is zero!");

            continue;
        }

        pstSysAcqBsrCtrl->astBsrTimerInfo[i].ucMaxHistorySrchTimes  = stNvimSysAcqBsrCtrl.astBsrTimerInfo[i].ucMaxHistorySrchTimes;
        pstSysAcqBsrCtrl->astBsrTimerInfo[i].ucMaxPrefBandSrchTimes = stNvimSysAcqBsrCtrl.astBsrTimerInfo[i].ucMaxPrefBandSrchTimes;

        if (0 != stNvimSysAcqBsrCtrl.astBsrTimerInfo[i].usBsrTimerLen)
        {
            ulBsrTimerLen = NAS_MSCC_THOUSAND_MILLISECOND_UNIT * stNvimSysAcqBsrCtrl.astBsrTimerInfo[i].usBsrTimerLen;
        }

        if (0 != stNvimSysAcqBsrCtrl.astBsrTimerInfo[i].usBsrTimerLenWithNoMatchedMsplRec)
        {
            ulBsrTimerLenWithNoMatchedMsplRec = NAS_MSCC_THOUSAND_MILLISECOND_UNIT * stNvimSysAcqBsrCtrl.astBsrTimerInfo[i].usBsrTimerLenWithNoMatchedMsplRec;
        }

        pstSysAcqBsrCtrl->astBsrTimerInfo[i].ulBsrTimerLen  = ulBsrTimerLen;
        pstSysAcqBsrCtrl->astBsrTimerInfo[i].ulBsrTimerLenWithNoMatchedMsplRec = ulBsrTimerLenWithNoMatchedMsplRec;
    }

    return;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */



