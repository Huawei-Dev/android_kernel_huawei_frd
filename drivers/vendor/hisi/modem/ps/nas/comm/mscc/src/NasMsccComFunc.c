

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "NasMsccCtx.h"
#include "PsCommonDef.h"
#include "NasMsccMntn.h"

#include "NasMsccMlplMsplApi.h"
#include "NasMsccComFunc.h"
#include "NasMsccSysAcqStrategy.h"
#include "NasComm.h"
#include "NasErrLogCcbCtx.h"
#include "NasMsccFsmSysAcq.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_NAS_MSCC_COM_FUNC_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/*****************************************************************************
  3 函数定义
*****************************************************************************/
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)


VOS_UINT32 NAS_MSCC_IsNeedPerformCLSysAcq(VOS_VOID)
{
    VOS_UINT32                                              ulIsHrpdAndLteBothSupported;
    VOS_UINT8                                               ucMlplMsplValidFlg;
    NAS_MSCC_PIF_CARD_STATUS_ENUM_UINT8                     enUsimCardStatus;
    NAS_MSCC_PIF_CARD_STATUS_ENUM_UINT8                     enCsimCardStatus;
    NAS_MSCC_LTE_CAPABILITY_STATUS_ENUM_UINT32              enLteCapSta;

    enLteCapSta = NAS_MSCC_GetLteCapbilityStatus();

    enUsimCardStatus                = NAS_MSCC_GetUsimCardStatus();
    enCsimCardStatus                = NAS_MSCC_GetCsimCardStatus();
    ulIsHrpdAndLteBothSupported     = NAS_MSCC_IsHrpdAndLteBothSupported();
    ucMlplMsplValidFlg              = NAS_MSCC_GetMlplMsplValidFlag();

    NAS_NORMAL_LOG2(UEPS_PID_MSCC, "NAS_MSCC_IsNeedPerformCLSysAcq:enUsimCardStatus and enCsimCardStatus",
                                    enUsimCardStatus, enCsimCardStatus);

    NAS_NORMAL_LOG3(UEPS_PID_MSCC, "NAS_MSCC_IsNeedPerformCLSysAcq:ulIsHrpdAndLteBothSupported, ucMlplMsplValidFlg and enLteCapSta",
                                    ulIsHrpdAndLteBothSupported, ucMlplMsplValidFlg, enLteCapSta);

    if ((VOS_TRUE                                           == ulIsHrpdAndLteBothSupported)
     && (VOS_TRUE                                           == ucMlplMsplValidFlg)
     && (NAS_MSCC_PIF_CARD_STATUS_USIM_PRESENT              == enUsimCardStatus)
     && (NAS_MSCC_PIF_CARD_STATUS_CSIM_PRESENT              == enCsimCardStatus)
     && (NAS_MSCC_LTE_CAPABILITY_STATUS_DISABLE_NOTIFIED_AS != enLteCapSta))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}



VOS_UINT32 NAS_MSCC_IsBgSearchHighPriSysAllowed(VOS_VOID)
{
    NAS_MSCC_PIF_CARD_STATUS_ENUM_UINT8 enUsimCardStatus;
    NAS_MSCC_PIF_CARD_STATUS_ENUM_UINT8 enCsimCardStatus;
    VOS_UINT8                           ucMlplMsplValidFlag;
    VOS_UINT32                          uc3gppNormalSrvFlg;
    VOS_UINT32                          ucHrpdNormalSrvFlg;

    enUsimCardStatus        = NAS_MSCC_GetUsimCardStatus();
    enCsimCardStatus        = NAS_MSCC_GetCsimCardStatus();
    ucMlplMsplValidFlag     = NAS_MSCC_GetMlplMsplValidFlag();

    uc3gppNormalSrvFlg  = NAS_MSCC_Is3gppNormalService();
    ucHrpdNormalSrvFlg  = NAS_MSCC_IsHrpdNormalService();

    NAS_NORMAL_LOG2(UEPS_PID_MSCC,"NAS_MSCC_IsBgSearchHighPriSysAllowed:enUsimCardStatus and enCsimCardStatus",
                                    enUsimCardStatus, enCsimCardStatus);

    NAS_NORMAL_LOG3(UEPS_PID_MSCC, "NAS_MSCC_IsBgSearchHighPriSysAllowed:ucMlplMsplValidFlag, uc3gppNormalSrvFlg and ucHrpdNormalSrvFlg",
                                    ucMlplMsplValidFlag, uc3gppNormalSrvFlg, ucHrpdNormalSrvFlg);

    if ((VOS_TRUE != NAS_MSCC_IsHrpdAndLteBothSupported())
     || (VOS_TRUE != ucMlplMsplValidFlag)
     || (NAS_MSCC_PIF_CARD_STATUS_USIM_PRESENT != enUsimCardStatus)
     || (NAS_MSCC_PIF_CARD_STATUS_CSIM_PRESENT != enCsimCardStatus))
    {
        return VOS_FALSE;
    }

    /* 在LTE网络注册是否被invalid for lte service原因值拒绝过 */
    if (VOS_FALSE == NAS_MSCC_GetSimPsRegStatus())
    {
        return VOS_FALSE;
    }



    /* LTE和HRPD都不是正常服务直接返回 */
    if ((VOS_FALSE == uc3gppNormalSrvFlg)
     && (VOS_FALSE == ucHrpdNormalSrvFlg))
    {
        return VOS_FALSE;
    }

    /* 当前网络是否已经是最高优先级网络 */
    if (VOS_TRUE == NAS_MSCC_IsCampOnHighestPrioritySystem())
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_IsCampOnHighestPrioritySystem(VOS_VOID)
{
    NAS_MSCC_3GPP_SYS_INFO_STRU                            *pst3gppSysInfo  = VOS_NULL_PTR;
    NAS_MSCC_HRPD_SYS_INFO_STRU                            *pstHrpdSysInfo  = VOS_NULL_PTR;
    NAS_MSCC_1X_SYS_INFO_STRU                              *pst1xSysInfo    = VOS_NULL_PTR;
    NAS_MSCC_MSPL_SYS_LIST_INFO_STRU                        stOutSysList;
    NAS_MSCC_MMSS_MATCH_3GPP_INPUT_INFO_STRU                st3gppInfo;
    NAS_MSCC_MMSS_MATCH_HRPD_INPUT_INFO_STRU                stHrpdInfo;
    NAS_MSCC_MMSS_MATCH_1X_INPUT_INFO_STRU                  st1xInfo;
    VOS_UINT32                                              ulReturnValue;
    VOS_UINT32                                              ulGetMsplListRslt;
    NAS_MSCC_MSPL_ID_STRU                                  *pstMatchedMsplTable = VOS_NULL_PTR;

    ulReturnValue       = VOS_TRUE;
    ulGetMsplListRslt   = VOS_FALSE;

    PS_MEM_SET(&stOutSysList, 0x00, sizeof(NAS_MSCC_MSPL_SYS_LIST_INFO_STRU));

    if (VOS_TRUE == NAS_MSCC_GetHrpdCampOnFlag())
    {
        pstHrpdSysInfo = NAS_MSCC_GetHrpdSysInfoAddr();

        stHrpdInfo.enPriClassCdma2000Hrpd = pstHrpdSysInfo->enHrpdPrioClass;
        stHrpdInfo.enPriClassCdma2000Ai   = pstHrpdSysInfo->enAIPrioClass;
        stHrpdInfo.ucIsMccValid           = VOS_TRUE;
        stHrpdInfo.ulMcc                  = pstHrpdSysInfo->ulMcc;
        stHrpdInfo.ucSubNetMask           = pstHrpdSysInfo->ucSubNetMask;

        PS_MEM_CPY(stHrpdInfo.aucSectorId,
                   pstHrpdSysInfo->aucSectorId,
                   NAS_MSCC_BYTES_IN_SUBNET);

        /* 当HRPD网络没有下发MCC时, 使用1X信息查询MLPL/MSPL表,
           再使用HRPD信息判断是否有LTE网络优先级高于当前HRPD网络 */
        if (NAS_MSCC_INVALID_MCC == pstHrpdSysInfo->ulMcc)
        {
            if (VOS_TRUE == NAS_MSCC_Get1xCampOnFlag())
            {
                stHrpdInfo.ucIsMccValid       = VOS_FALSE;

                pst1xSysInfo = NAS_MSCC_Get1xSysInfoAddr();

                st1xInfo.enPriClassCdma20001x = pst1xSysInfo->en1xPrioClass;
                st1xInfo.enPriClassCdma2000Ai = pst1xSysInfo->enAIPrioClass;
                st1xInfo.ulMcc                = pst1xSysInfo->ulMcc;
                st1xInfo.ulMnc                = pst1xSysInfo->ulMnc;
                st1xInfo.ucIsMccValid         = VOS_TRUE;
                st1xInfo.ucIsMncValid         = VOS_TRUE;
                st1xInfo.usNid                = pst1xSysInfo->usNid;
                st1xInfo.usSid                = pst1xSysInfo->usSid;

                ulGetMsplListRslt = NAS_MSCC_GetHigherPriMsplSysListThanHrpdBy1xLoc(&st1xInfo,
                                                        &stHrpdInfo, &stOutSysList);
            }

        }
        else
        {
            ulGetMsplListRslt = NAS_MSCC_GetHigherPriMsplSysListThanHrpd(&stHrpdInfo, &stOutSysList);
        }

        if (VOS_TRUE == ulGetMsplListRslt)
        {
            if (VOS_TRUE == NAS_MSCC_IsLteSysTypeExistInSysList(&stOutSysList))
            {
                ulReturnValue = VOS_FALSE;
            }
        }

        return ulReturnValue;
    }

    if (VOS_TRUE == NAS_MSCC_GetLteCampOnFlag())
    {
        pst3gppSysInfo = NAS_MSCC_Get3gppSysInfoAddr();

        st3gppInfo.stPlmnId.ulMcc = pst3gppSysInfo->stPlmnId.ulMcc;
        st3gppInfo.stPlmnId.ulMnc = pst3gppSysInfo->stPlmnId.ulMnc;
        st3gppInfo.enPriClass     = pst3gppSysInfo->enPrioClass;
        st3gppInfo.enSysRat       = VOS_RATMODE_LTE;

        pstMatchedMsplTable = NAS_MSCC_GetMatchedMsplTableBy3gppInfo(&st3gppInfo);

        if (VOS_NULL_PTR == pstMatchedMsplTable)
        {
            ulGetMsplListRslt = VOS_FALSE;
        }
        else
        {
            ulGetMsplListRslt = NAS_MSCC_GetHigherPriSysListWithMsplTable(&st3gppInfo,
                                                                          pstMatchedMsplTable,
                                                                          &stOutSysList);
        }

        if (VOS_TRUE == ulGetMsplListRslt)
        {
            if (VOS_TRUE == NAS_MSCC_IsHrpdSysTypeExistInSysList(&stOutSysList))
            {
                ulReturnValue = VOS_FALSE;
            }
        }

        return ulReturnValue;
    }

    return ulReturnValue;
}


VOS_UINT32 NAS_MSCC_GetBsrTimerLen(VOS_VOID)
{
    NAS_MSCC_MMSS_MATCH_3GPP_INPUT_INFO_STRU                st3gppInfo;
    NAS_MSCC_MMSS_MATCH_HRPD_INPUT_INFO_STRU                stHrpdInfo;
    NAS_MSCC_MMSS_MATCH_1X_INPUT_INFO_STRU                  st1xInfo;
    VOS_UINT32                                              ulBsrTimerLen;
    VOS_UINT32                                              ulGetBsrTimerLenRslt;
    NAS_MSCC_HRPD_SYS_INFO_STRU                            *pstHrpdSysInfo = VOS_NULL_PTR;
    NAS_MSCC_3GPP_SYS_INFO_STRU                            *pstLteSysInfo  = VOS_NULL_PTR;
    NAS_MSCC_1X_SYS_INFO_STRU                              *pst1xSysInfo   = VOS_NULL_PTR;


    ulBsrTimerLen           = 0;
    ulGetBsrTimerLenRslt    = VOS_FALSE;

    if (VOS_TRUE == NAS_MSCC_GetLteCampOnFlag())
    {
        pstLteSysInfo  = NAS_MSCC_Get3gppSysInfoAddr();

        st3gppInfo.enPriClass     = pstLteSysInfo->enPrioClass;
        st3gppInfo.stPlmnId.ulMcc = pstLteSysInfo->stPlmnId.ulMcc;
        st3gppInfo.stPlmnId.ulMnc = pstLteSysInfo->stPlmnId.ulMnc;
        st3gppInfo.enSysRat       = VOS_RATMODE_LTE;

        ulGetBsrTimerLenRslt = NAS_MSCC_GetCurr3gppSysBsrTimerLen(&st3gppInfo, &ulBsrTimerLen);
    }
    else if (VOS_TRUE == NAS_MSCC_GetHrpdCampOnFlag())
    {
        pstHrpdSysInfo = NAS_MSCC_GetHrpdSysInfoAddr();

        stHrpdInfo.enPriClassCdma2000Hrpd = pstHrpdSysInfo->enHrpdPrioClass;
        stHrpdInfo.enPriClassCdma2000Ai   = pstHrpdSysInfo->enAIPrioClass;
        stHrpdInfo.ucIsMccValid           = VOS_TRUE;
        stHrpdInfo.ulMcc                  = pstHrpdSysInfo->ulMcc;
        stHrpdInfo.ucSubNetMask           = pstHrpdSysInfo->ucSubNetMask;

        PS_MEM_CPY(stHrpdInfo.aucSectorId,
                   pstHrpdSysInfo->aucSectorId,
                   NAS_MSCC_BYTES_IN_SUBNET);

        /* 当HRPD网络没有下发MCC时, 先使用1X信息查询MLPL/MSPL表,
           再使用HRPD SysInfo获取 BSR定时器时长*/
        if (NAS_MSCC_INVALID_MCC == pstHrpdSysInfo->ulMcc)
        {
            if (VOS_TRUE == NAS_MSCC_Get1xCampOnFlag())
            {
                stHrpdInfo.ucIsMccValid       = VOS_FALSE;

                pst1xSysInfo   = NAS_MSCC_Get1xSysInfoAddr();

                st1xInfo.enPriClassCdma20001x = pst1xSysInfo->en1xPrioClass;
                st1xInfo.enPriClassCdma2000Ai = pst1xSysInfo->enAIPrioClass;
                st1xInfo.ulMcc                = pst1xSysInfo->ulMcc;
                st1xInfo.ulMnc                = pst1xSysInfo->ulMnc;
                st1xInfo.ucIsMccValid         = VOS_TRUE;
                st1xInfo.ucIsMncValid         = VOS_TRUE;
                st1xInfo.usNid                = pst1xSysInfo->usNid;
                st1xInfo.usSid                = pst1xSysInfo->usSid;

                ulGetBsrTimerLenRslt = NAS_MSCC_GetCurrHrpdSysBsrTimerLenBy1xLoc(&st1xInfo,
                                                                    &stHrpdInfo, &ulBsrTimerLen);
            }
        }
        else
        {
            ulGetBsrTimerLenRslt = NAS_MSCC_GetCurrHrpdSysBsrTimerLen(&stHrpdInfo, &ulBsrTimerLen);
        }
    }
    else
    {
        pst1xSysInfo   = NAS_MSCC_Get1xSysInfoAddr();

        st1xInfo.enPriClassCdma20001x = pst1xSysInfo->en1xPrioClass;
        st1xInfo.enPriClassCdma2000Ai = pst1xSysInfo->enAIPrioClass;
        st1xInfo.ulMcc                = pst1xSysInfo->ulMcc;
        st1xInfo.ulMnc                = pst1xSysInfo->ulMnc;
        st1xInfo.ucIsMccValid         = VOS_TRUE;
        st1xInfo.ucIsMncValid         = VOS_TRUE;
        st1xInfo.usNid                = pst1xSysInfo->usNid;
        st1xInfo.usSid                = pst1xSysInfo->usSid;

        ulGetBsrTimerLenRslt = NAS_MSCC_GetCurr1xSysBsrTimerLen(&st1xInfo, &ulBsrTimerLen);
    }

    if (VOS_FALSE == ulGetBsrTimerLenRslt)
    {
        ulBsrTimerLen   = 0;
    }

    return ulBsrTimerLen;
}


VOS_VOID NAS_MSCC_StartBsrTimer()
{
    VOS_UINT32                          ulBsrTimerLen;

    ulBsrTimerLen = NAS_MSCC_GetBsrTimerLen();

    if (0 == ulBsrTimerLen)
    {
        NAS_WARNING_LOG(UEPS_PID_MSCC, "NAS_MSCC_StartBsrTimer():NAS MSCC BSR Timer length is zero!");

        return;
    }

    NAS_MSCC_StartTimer(TI_NAS_MSCC_BSR_TIMER, ulBsrTimerLen);

    NAS_MSCC_SetCurrBsrTimerLen(ulBsrTimerLen);

    NAS_MSCC_SetOrigBsrTimerStartRat();

    return;
}


VOS_UINT32 NAS_MSCC_IsNeedStartBsrTimer(VOS_VOID)
{
    /* 非主状态机不启动BSR定时器，在退出子状态机后再判断是否需要启动BSR定时器 */
    if (NAS_MSCC_FSM_L1_MAIN != NAS_MSCC_GetCurrFsmId())
    {
        return VOS_FALSE;
    }

    if (NAS_MSCC_TIMER_STATUS_RUNING == NAS_MSCC_GetTimerStatus(TI_NAS_MSCC_PERIOD_TRYING_HIGH_PRI_SYSTEM_SEARCH))
    {
        return VOS_FALSE;
    }

    /* CPROC不支持L到C的BSR流程,在此暂时进行规避,待CPROC支持该功能后打开,
       PC上依然保证,只在单板上进行屏蔽 */
#if (VOS_WIN32 != VOS_OS_VER)
    if (VOS_TRUE == NAS_MSCC_Get3gppCampOnFlag())
    {
        return VOS_FALSE;
    }
#endif

    return NAS_MSCC_IsBgSearchHighPriSysAllowed();
}


VOS_UINT32 NAS_MSCC_IsNeedRestartBsrTimer(VOS_VOID)
{
    VOS_UINT32                                              ulOldBsrTimerLen;
    VOS_UINT32                                              ulNewBsrTimerLen;

    ulOldBsrTimerLen = NAS_MSCC_GetCurrBsrTimerLen();
    ulNewBsrTimerLen = NAS_MSCC_GetBsrTimerLen();

    if (NAS_MSCC_TIMER_STATUS_RUNING != NAS_MSCC_GetTimerStatus(TI_NAS_MSCC_BSR_TIMER))
    {
        return VOS_FALSE;
    }

    if (VOS_TRUE == NAS_MSCC_GetHrpdCampOnFlag())
    {
        /* 如果定时器长度发生变化,重启 */
        /* 如果当前制式与启动定时器时的制式已经不同,重启 */
        if ((VOS_RATMODE_HRPD != NAS_MSCC_GetOrigBsrTimerStartRat())
         || (ulOldBsrTimerLen != ulNewBsrTimerLen))
        {
            return VOS_TRUE;
        }
    }

    if (VOS_TRUE == NAS_MSCC_Get3gppCampOnFlag())
    {
        if ((VOS_RATMODE_LTE  != NAS_MSCC_GetOrigBsrTimerStartRat())
         || (ulOldBsrTimerLen != ulNewBsrTimerLen))
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


NAS_MSCC_SERVICE_STATUS_ENUM_UINT8 NAS_MSCC_Convert1xServiceStatusToMsccFormat(
    NAS_MSCC_PIF_SERVICE_STATUS_ENUM_UINT32                 en1xPsServiceStatus
)
{
    NAS_MSCC_SERVICE_STATUS_ENUM_UINT8  enPsSrvStatus;

    switch (en1xPsServiceStatus)
    {
        case NAS_MSCC_PIF_SERVICE_STATUS_NORMAL_SERVICE:

            enPsSrvStatus = NAS_MSCC_SERVICE_STATUS_NORMAL_SERVICE;
            break;

        case NAS_MSCC_PIF_SERVICE_STATUS_LIMITED_SERVICE:
        case NAS_MSCC_PIF_SERVICE_STATUS_LIMITED_SERVICE_REGION:
        case NAS_MSCC_PIF_SERVICE_STATUS_NO_IMSI:

            enPsSrvStatus = NAS_MSCC_SERVICE_STATUS_LIMITED_SERVICE;
            break;

        case NAS_MSCC_PIF_SERVICE_STATUS_NO_CHANGE:
        case NAS_MSCC_PIF_SERVICE_STATUS_DEEP_SLEEP:
        case NAS_MSCC_PIF_SERVICE_STATUS_NO_SERVICE:

            enPsSrvStatus = NAS_MSCC_SERVICE_STATUS_NO_SERVICE;
            break;

        default:

            enPsSrvStatus =  NAS_MSCC_SERVICE_STATUS_BUTT;
            break;
    }

    return enPsSrvStatus;
}


NAS_MSCC_PIF_PRI_CLASS_ENUM_UINT8 NAS_MSCC_ConvertHsdSysPriClassToMsccFormat(
    MSCC_HSD_PRI_CLASS_ENUM_UINT8       enPriClass
)
{
    NAS_MSCC_PIF_PRI_CLASS_ENUM_UINT8   enMsccSysPriClass;

    switch (enPriClass)
    {
        case MSCC_HSD_PRI_CLASS_HOME:

            enMsccSysPriClass = NAS_MSCC_PIF_PRI_CLASS_HOME;
            break;

        case MSCC_HSD_PRI_CLASS_PREF:

            enMsccSysPriClass = NAS_MSCC_PIF_PRI_CLASS_PREF;
            break;

        case MSCC_HSD_PRI_CLASS_ANY :

            enMsccSysPriClass = NAS_MSCC_PIF_PRI_CLASS_ANY;
            break;

        default:

            enMsccSysPriClass = NAS_MSCC_PIF_PRI_CLASS_ENUM_BUTT;
            break;
    }

    return enMsccSysPriClass;
}







VOS_VOID NAS_MSCC_Update1xSysInfo(
    XSD_MSCC_1X_SYSTEM_SERVICE_INFO_IND_STRU               *pst1xSrvInfoInd
)
{
    NAS_MSCC_1X_SYS_INFO_STRU          *pst1xSysInfo;

    pst1xSysInfo = NAS_MSCC_Get1xSysInfoAddr();

    if ((NAS_MSCC_PIF_SERVICE_STATUS_NORMAL_SERVICE == pst1xSrvInfoInd->stSysSrvInfo.enServiceStatus)
     || (NAS_MSCC_PIF_SERVICE_STATUS_LIMITED_SERVICE == pst1xSrvInfoInd->stSysSrvInfo.enServiceStatus))
    {
        pst1xSysInfo->ucCampOnFlg   = VOS_TRUE;

        pst1xSysInfo->en1xPrioClass = pst1xSrvInfoInd->stSysSrvInfo.en1xPriClass;

        pst1xSysInfo->enAIPrioClass = pst1xSrvInfoInd->stSysSrvInfo.enAIPriClass;

        /* Update the location info in the Camped Location Info global variable */

        /* 判定1x位置有效才进行MCC的判定 */
        if (PS_TRUE == pst1xSrvInfoInd->stSysSrvInfo.enServiceInfoIncl)
        {
            pst1xSysInfo->ulMcc = pst1xSrvInfoInd->stSysSrvInfo.stServiceInfo.ulMcc;
            pst1xSysInfo->ulMnc = pst1xSrvInfoInd->stSysSrvInfo.stServiceInfo.usMnc;
        }

        pst1xSysInfo->usSid = pst1xSrvInfoInd->stSysSrvInfo.usSid;
        pst1xSysInfo->usNid = pst1xSrvInfoInd->stSysSrvInfo.usNid;
    }
    else
    {
        pst1xSysInfo->ucCampOnFlg  = VOS_FALSE;
    }

    return;
}



VOS_VOID NAS_MSCC_UpdateHrpdSysInfo(
    HSD_MSCC_OVERHEAD_MSG_IND_STRU     *pstHsdOverHeadInd
)
{
    NAS_MSCC_HRPD_SYS_INFO_STRU        *pstHrpdSysInfo;

    pstHrpdSysInfo = NAS_MSCC_GetHrpdSysInfoAddr();

    pstHrpdSysInfo->ucCampOnFlg     = VOS_TRUE;
    pstHrpdSysInfo->ulMcc           = pstHsdOverHeadInd->ulMcc;
    pstHrpdSysInfo->ucSubNetMask    = pstHsdOverHeadInd->ucSubNetMask;
    pstHrpdSysInfo->enAIPrioClass   = NAS_MSCC_ConvertHsdSysPriClassToMsccFormat(pstHsdOverHeadInd->ucPriClass_AI);
    pstHrpdSysInfo->enHrpdPrioClass = NAS_MSCC_ConvertHsdSysPriClassToMsccFormat(pstHsdOverHeadInd->ucPriClass_HRPD);

    PS_MEM_CPY(pstHrpdSysInfo->aucSectorId, pstHsdOverHeadInd->aucSectorId, NAS_MSCC_BYTES_IN_SUBNET);

    NAS_MSCC_SetLastCampOnLTEOrDoRat(NAS_MSCC_PIF_NET_RAT_TYPE_HRPD);

    return;
}


VOS_VOID NAS_MSCC_GetHighPriSysInfoInHrpdSysInfoListWithPriClass(
    VOS_UINT8                                               ucHrpdSysNum,
    MSCC_HSD_HRPD_SYS_INFO_STRU                            *pstHrpdSysInfoList,
    NAS_MSCC_MSPL_SYS_PRI_CLASS_ENUM_UINT8                  enSysPriClass,
    NAS_MSCC_HIGH_PRIORITY_SYSTEM_INFO_STRU                *pstHighPriSysInfo
)
{
    VOS_UINT8                           ucPriClassMatched;
    VOS_UINT8                           i;
    VOS_UINT8                           j;

    ucPriClassMatched = VOS_FALSE;

    ucHrpdSysNum = NAS_COMM_MIN(MSCC_HSD_BG_HRPD_MAX_SYS_NUM, ucHrpdSysNum);

    for (i = 0, j = 0; i < ucHrpdSysNum; i++)
    {
        if ((NAS_MSCC_PIF_PREF_NEG_SYS_NEG == pstHrpdSysInfoList[i].enPrefNegSys)
         || (VOS_TRUE == pstHrpdSysInfoList[i].ucIsAvoided)
         || (VOS_FALSE == pstHrpdSysInfoList[i].ucIsExistedInPrl))
        {
            continue;
        }

        if ((NAS_MSCC_MSPL_SYS_PRI_CLASS_HOME == enSysPriClass)
         && (MSCC_HSD_PRI_CLASS_HOME == pstHrpdSysInfoList[i].enPriClass))
        {
            ucPriClassMatched = VOS_TRUE;
        }

        if ((NAS_MSCC_MSPL_SYS_PRI_CLASS_HOME_AND_PREF == enSysPriClass)
         && ((MSCC_HSD_PRI_CLASS_HOME == pstHrpdSysInfoList[i].enPriClass)
          || (MSCC_HSD_PRI_CLASS_PREF == pstHrpdSysInfoList[i].enPriClass)))
        {
            ucPriClassMatched = VOS_TRUE;
        }

        if (NAS_MSCC_MSPL_SYS_PRI_CLASS_ANY == enSysPriClass)
        {
            ucPriClassMatched = VOS_TRUE;
        }

        if (VOS_TRUE == ucPriClassMatched)
        {
            /* save high pri sys info */
            pstHighPriSysInfo->astFreqInfo[j].stFreq.enBandClass         = pstHrpdSysInfoList[i].stHrpdSysList.usBandClass;
            pstHighPriSysInfo->astFreqInfo[j].stFreq.usChannel           = pstHrpdSysInfoList[i].stHrpdSysList.usFreq;

            pstHighPriSysInfo->astFreqInfo[j].stHrpdSysItem.usSysIndex   = pstHrpdSysInfoList[i].stHrpdSysItem.usSysIndex;
            pstHighPriSysInfo->astFreqInfo[j].stHrpdSysItem.usAcqIndex   = pstHrpdSysInfoList[i].stHrpdSysItem.usAcqIndex;
            pstHighPriSysInfo->astFreqInfo[j].stHrpdSysItem.ucPrioLevel  = pstHrpdSysInfoList[i].stHrpdSysItem.ucPrioLevel;
            pstHighPriSysInfo->astFreqInfo[j].stHrpdSysItem.enPrefNegSys = pstHrpdSysInfoList[i].stHrpdSysItem.enPrefNegSys;

            j++;
            pstHighPriSysInfo->ucFreqNum = j;

            ucPriClassMatched = VOS_FALSE;
        }

    }

}



VOS_UINT32 NAS_MSCC_GetCurrentCampedSysMsplIndex(
    VOS_UINT8                          *pucMatchedMsplIndex,
    VOS_UINT8                          *pucDefMsplRec
)
{
    NAS_MSCC_MLPL_STRU                                     *pstMlpl;
    NAS_MSCC_MLPL_MATCH_LVL_ENUM_UINT8                      enMlplMatchLvl;
    NAS_MSCC_MMSS_MATCH_3GPP_INPUT_INFO_STRU                stIn3gppInfo;
    NAS_MSCC_MMSS_MATCH_HRPD_INPUT_INFO_STRU                stInHrpdInfo;
    NAS_MSCC_HRPD_SYS_INFO_STRU                            *pstHrpdSysInfo;
    NAS_MSCC_3GPP_SYS_INFO_STRU                            *pst3gppSysInfo;

    *pucDefMsplRec = VOS_FALSE;

    /* 如果MLPL MSPL不可用，直接返回VOS_FALSE */
    if (VOS_FALSE == NAS_MSCC_GetMlplMsplValidFlag())
    {
        return VOS_FALSE;
    }

    enMlplMatchLvl       = NAS_MSCC_MLPL_MATCH_LVL_NOT_MATCH;
    *pucMatchedMsplIndex = 0;

    pstMlpl = NAS_MSCC_GetMlplInfoAddr();

    if (VOS_TRUE == NAS_MSCC_Get3gppCampOnFlag())
    {
        pst3gppSysInfo = NAS_MSCC_Get3gppSysInfoAddr();

        PS_MEM_SET(&stIn3gppInfo, 0x00 , sizeof(NAS_MSCC_MMSS_MATCH_3GPP_INPUT_INFO_STRU));

        stIn3gppInfo.stPlmnId.ulMcc = pst3gppSysInfo->stPlmnId.ulMcc;
        stIn3gppInfo.stPlmnId.ulMnc = pst3gppSysInfo->stPlmnId.ulMnc;
        stIn3gppInfo.enPriClass     = pst3gppSysInfo->enPrioClass;
        stIn3gppInfo.enSysRat       = VOS_RATMODE_LTE;

        if (VOS_FALSE == NAS_MSCC_GetMsplIndexBy3gppSys(&stIn3gppInfo, pstMlpl,
                                                        pucMatchedMsplIndex, &enMlplMatchLvl))
        {
            /* 如果在MLPL列表中找不到匹配的MLPL记录，并且Default MLPL可用，获取Default MLPL的MSPL_INDEX */
            if (VOS_FALSE == pstMlpl->ucIsContainDefRec)
            {
                return VOS_FALSE;
            }

            *pucMatchedMsplIndex = pstMlpl->ucMsplIdxInFirstDefRec;

            *pucDefMsplRec = VOS_TRUE;

            return VOS_TRUE;
        }

        return VOS_TRUE;
    }

    if (VOS_TRUE == NAS_MSCC_GetHrpdCampOnFlag())
    {
        pstHrpdSysInfo = NAS_MSCC_GetHrpdSysInfoAddr();

        PS_MEM_SET(&stInHrpdInfo, 0x00 , sizeof(NAS_MSCC_MMSS_MATCH_HRPD_INPUT_INFO_STRU));

        stInHrpdInfo.enPriClassCdma2000Hrpd = pstHrpdSysInfo->enHrpdPrioClass;
        stInHrpdInfo.enPriClassCdma2000Ai   = pstHrpdSysInfo->enAIPrioClass;
        stInHrpdInfo.ucIsMccValid           = VOS_TRUE;
        stInHrpdInfo.ulMcc                  = pstHrpdSysInfo->ulMcc;
        stInHrpdInfo.ucSubNetMask           = pstHrpdSysInfo->ucSubNetMask;

        PS_MEM_CPY(stInHrpdInfo.aucSectorId,
                   pstHrpdSysInfo->aucSectorId,
                   NAS_MSCC_BYTES_IN_SUBNET);

        if (VOS_FALSE == NAS_MSCC_GetMsplIndexByHrpdSys(&stInHrpdInfo, pstMlpl,
                                                        pucMatchedMsplIndex, &enMlplMatchLvl))
        {
            /* 如果在MLPL列表中找不到匹配的MLPL记录，并且Default MLPL可用，获取Default MLPL的MSPL_INDEX */
            if (VOS_FALSE == pstMlpl->ucIsContainDefRec)
            {
                return VOS_FALSE;
            }

            *pucMatchedMsplIndex = pstMlpl->ucMsplIdxInFirstDefRec;

            *pucDefMsplRec = VOS_TRUE;

            return VOS_TRUE;
        }

        return VOS_TRUE;
    }

    return VOS_FALSE;
}



VOS_VOID NAS_MSCC_GetPlmnListAccordingtoSpecMsplIndex(
    VOS_UINT8                                               ucOrigSysMsplIndex,
    NAS_MSCC_PIF_SEARCHED_PLMN_LIST_INFO_STRU              *pstSrcPlmnList,
    NAS_MSCC_PIF_SEARCHED_PLMN_LIST_INFO_STRU              *pstDestPlmnList
)
{
    NAS_MSCC_MLPL_STRU                                     *pstMlpl;
    NAS_MSCC_MLPL_MATCH_LVL_ENUM_UINT8                      enMlplMatchLvl;
    NAS_MSCC_MMSS_MATCH_3GPP_INPUT_INFO_STRU                stIn3gppInfo;
    VOS_UINT8                                               i;
    VOS_UINT8                                               j;
    VOS_UINT8                                               ucCurSysMsplIndex;

    /* 如果MLPL MSPL不可用，直接返回VOS_FALSE */
    if (VOS_FALSE == NAS_MSCC_GetMlplMsplValidFlag())
    {
        PS_MEM_CPY(pstDestPlmnList, pstSrcPlmnList, sizeof(NAS_MSCC_PIF_SEARCHED_PLMN_LIST_INFO_STRU));

        return;
    }

    enMlplMatchLvl = NAS_MSCC_MLPL_MATCH_LVL_NOT_MATCH;

    pstMlpl = NAS_MSCC_GetMlplInfoAddr();

    for (i = 0,j = 0; i < pstSrcPlmnList->ucHighPlmnNum; i++)
    {
        PS_MEM_SET(&stIn3gppInfo, 0x00 , sizeof(NAS_MSCC_MMSS_MATCH_3GPP_INPUT_INFO_STRU));

        stIn3gppInfo.stPlmnId.ulMcc = pstSrcPlmnList->astHighPlmnID[i].ulMcc;
        stIn3gppInfo.stPlmnId.ulMnc = pstSrcPlmnList->astHighPlmnID[i].ulMnc;
        stIn3gppInfo.enPriClass     = NAS_MSCC_PIF_PRI_CLASS_ANY;
        stIn3gppInfo.enSysRat       = VOS_RATMODE_LTE;

        ucCurSysMsplIndex = 0;

        if (VOS_TRUE == NAS_MSCC_GetMsplIndexBy3gppSys(&stIn3gppInfo, pstMlpl,
                                                        &ucCurSysMsplIndex, &enMlplMatchLvl))
        {
            if (ucCurSysMsplIndex == ucOrigSysMsplIndex)
            {
                pstDestPlmnList->astHighPlmnID[j] = pstSrcPlmnList->astHighPlmnID[i];
                j++;
            }
        }
    }

    pstDestPlmnList->ucHighPlmnNum = j;

    for (i = 0,j = 0; i < pstSrcPlmnList->ucLowPlmnNum; i++)
    {
        PS_MEM_SET(&stIn3gppInfo, 0x00 , sizeof(NAS_MSCC_MMSS_MATCH_3GPP_INPUT_INFO_STRU));

        stIn3gppInfo.stPlmnId.ulMcc = pstSrcPlmnList->astLowPlmnID[i].ulMcc;
        stIn3gppInfo.stPlmnId.ulMnc = pstSrcPlmnList->astLowPlmnID[i].ulMnc;
        stIn3gppInfo.enPriClass     = NAS_MSCC_PIF_PRI_CLASS_ANY;
        stIn3gppInfo.enSysRat       = VOS_RATMODE_LTE;

        ucCurSysMsplIndex = 0;

        if (VOS_TRUE == NAS_MSCC_GetMsplIndexBy3gppSys(&stIn3gppInfo, pstMlpl,
                                                       &ucCurSysMsplIndex, &enMlplMatchLvl))
        {
            if (ucCurSysMsplIndex == ucOrigSysMsplIndex)
            {
                pstDestPlmnList->astLowPlmnID[j] = pstSrcPlmnList->astLowPlmnID[i];
                j++;
            }
        }
    }

    pstDestPlmnList->ucLowPlmnNum = j;

    pstDestPlmnList->enRat        = pstSrcPlmnList->enRat;
}



VOS_UINT32 NAS_MSCC_IsNeedAcqLte(VOS_VOID)
{
    VOS_UINT32                                              ulIsLteSupported;
    VOS_UINT8                                               ucAllowSrchLteFlg;
    NAS_MSCC_PIF_CARD_STATUS_ENUM_UINT8                     enUsimCardStatus;
    VOS_UINT8                                               ucSimPsRegStatus;
    VOS_UINT32                                              ulEpsAttachAllowFlg;

    ulIsLteSupported    = NAS_MSCC_IsSpecRatSupported(NAS_MSCC_PIF_NET_RAT_TYPE_LTE);
    ucAllowSrchLteFlg   = NAS_MSCC_GetAllowSrchLteFlg();
    enUsimCardStatus    = NAS_MSCC_GetUsimCardStatus();
    ucSimPsRegStatus    = NAS_MSCC_GetSimPsRegStatus();
    ulEpsAttachAllowFlg = NAS_MSCC_GetEpsAttachAllowFlg();

    NAS_NORMAL_LOG2(UEPS_PID_MSCC,"NAS_MSCC_IsNeedAcqLte:ulIsLteSupported and ucAllowSrchLteFlg",
                                    ulIsLteSupported, ucAllowSrchLteFlg);

    NAS_NORMAL_LOG3(UEPS_PID_MSCC, "NAS_MSCC_IsNeedAcqLte:enUsimCardStatus, ucSimPsRegStatus and ulEpsAttachAllowFlg",
                                    enUsimCardStatus, ucSimPsRegStatus, ulEpsAttachAllowFlg);

    /* LTE not supported by syscfg,could not acquire */
    if (VOS_FALSE == ulIsLteSupported)
    {
        return VOS_FALSE;
    }

    if (VOS_FALSE == ucAllowSrchLteFlg)
    {
        return VOS_FALSE;
    }

    /* USIMM卡应用为absent时， 不搜lte */
    if (NAS_MSCC_PIF_CARD_STATUS_ABSENT == enUsimCardStatus)
    {
        return VOS_FALSE;
    }

    /* SIM卡时不搜lte */
    if (NAS_MSCC_PIF_CARD_STATUS_SIM_PRESENT == enUsimCardStatus)
    {
        return VOS_FALSE;
    }

    /* sim ps invalid,could not acquire lte */
    if (VOS_FALSE == ucSimPsRegStatus)
    {
        return VOS_FALSE;
    }

    if (VOS_FALSE == ulEpsAttachAllowFlg)
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_IsNeedAcqHrpd(VOS_VOID)
{
    VOS_UINT32                                              ulIsHrpdSupported;
    NAS_MSCC_PIF_CARD_STATUS_ENUM_UINT8                     enCsimCardStatus;

    VOS_UINT32                                              ulNoCardEnableFlag;

    ulIsHrpdSupported = NAS_MSCC_IsSpecRatSupported(NAS_MSCC_PIF_NET_RAT_TYPE_HRPD);
    enCsimCardStatus  = NAS_MSCC_GetCsimCardStatus();

    ulNoCardEnableFlag = NAS_MSCC_GetNoCardModeCfgFlag();

    NAS_NORMAL_LOG3(UEPS_PID_MSCC,"NAS_MSCC_IsNeedAcqHrpd:ulIsHrpdSupported, enCsimCardStatus and ulNoCardEnableFlag",
                                    ulIsHrpdSupported, enCsimCardStatus, ulNoCardEnableFlag);

    if (VOS_FALSE == ulIsHrpdSupported)
    {
        return VOS_FALSE;
    }

    if ((NAS_MSCC_PIF_CARD_STATUS_ABSENT == enCsimCardStatus)
     && (VOS_TRUE                        == ulNoCardEnableFlag))
    {
        return VOS_TRUE;
    }

    if (NAS_MSCC_PIF_CARD_STATUS_ABSENT == enCsimCardStatus)
    {
       return VOS_FALSE;
    }

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_IsNeedStartScanTimerUponSyscfg(VOS_VOID)
{
    if (VOS_TRUE == NAS_MSCC_IsGULModeConfigured())
    {
        return VOS_FALSE;
    }

    if (VOS_TRUE == NAS_MSCC_Is3gppNormalService())
    {
        return VOS_FALSE;
    }

    if (VOS_TRUE == NAS_MSCC_IsHrpdNormalService())
    {
        return VOS_FALSE;
    }

    if (VOS_TRUE == NAS_MSCC_Is1xNormalService())
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}



NAS_MSCC_PIF_PLMN_PRIORITY_CLASS_ENUM_UINT8 NAS_MSCC_ConvertMsplPriClassToPIfFormat(
    NAS_MSCC_MSPL_SYS_PRI_CLASS_ENUM_UINT8   enPriClass
)
{
    switch(enPriClass)
    {
        case NAS_MSCC_MSPL_SYS_PRI_CLASS_ANY:
            return NAS_MSCC_PIF_PLMN_PRIORITY_ANY;

        case NAS_MSCC_MSPL_SYS_PRI_CLASS_HOME_AND_PREF:
            return NAS_MSCC_PIF_PLMN_PRIORITY_PREF;

        case NAS_MSCC_MSPL_SYS_PRI_CLASS_HOME:
            return NAS_MSCC_PIF_PLMN_PRIORITY_HOME;

        default:
            return NAS_MSCC_PIF_PLMN_PRIORITY_BUTT;
    }
}


VOS_UINT32 NAS_MSCC_IsNeedPeformBsrUpOn3gppRfAvailable(VOS_VOID)
{
    NAS_MSCC_TIMER_STATUS_ENUM_UINT8    enBsrTimerStats;

    enBsrTimerStats      = NAS_MSCC_GetTimerStatus(TI_NAS_MSCC_BSR_TIMER);

    /* MMC RF可用时,如果BSR相关定时器在运行,为保证尽快BSR到LTE下去,此处进行BSR允许的判定 */
    if (NAS_MSCC_TIMER_STATUS_RUNING == enBsrTimerStats)
    {
        if (VOS_TRUE == NAS_MSCC_IsBgSearchHighPriSysAllowed())
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}



VOS_UINT32 NAS_MSCC_IsNeedEnableLteUpOnAttachPs(VOS_VOID)
{
    if (VOS_FALSE == NAS_MSCC_IsPlatformSupportLte())
    {
        return VOS_FALSE;
    }

    if (VOS_FALSE == NAS_MSCC_GetSimPsRegStatus())
    {
        return VOS_FALSE;
    }

    if (VOS_FALSE == NAS_MSCC_GetEpsAttachAllowFlg())
    {
        return VOS_FALSE;
    }

    /* 已经enable LTE后不需要再次enable LTE */
    if (NAS_MSCC_LTE_CAPABILITY_STATUS_REENABLE_NOTIFIED_AS == NAS_MSCC_GetLteCapbilityStatus())
    {
        return VOS_FALSE;
    }

    if (NAS_MSCC_LTE_CAPABILITY_STATUS_DISABLE_NOTIFIED_AS == NAS_MSCC_GetLteCapbilityStatus())
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 NAS_MSCC_IsNeedDisableLte(
    NAS_MSCC_DISABLE_LTE_REASON_ENUM_UINT32                *penDisableLReason
)
{
    VOS_UINT32                                              ulIsPlaforSupportLte;
    NAS_MSCC_PIF_CARD_STATUS_ENUM_UINT8                     enUsimCardStatus;
    NAS_MSCC_PIF_CARD_STATUS_ENUM_UINT8                     enCsimCardStatus;
    NAS_MSCC_LTE_CAPABILITY_STATUS_ENUM_UINT32              enLteCapStatus;
    VOS_UINT8                                               ucSimPsRegStatus;
    VOS_UINT32                                              ulEpsAttachAllowFlg;

    ulIsPlaforSupportLte = NAS_MSCC_IsPlatformSupportLte();
    enUsimCardStatus     = NAS_MSCC_GetUsimCardStatus();
    enCsimCardStatus     = NAS_MSCC_GetCsimCardStatus();
    enLteCapStatus       = NAS_MSCC_GetLteCapbilityStatus();
    ucSimPsRegStatus     = NAS_MSCC_GetSimPsRegStatus();
    ulEpsAttachAllowFlg  = NAS_MSCC_GetEpsAttachAllowFlg();

    NAS_NORMAL_LOG3(UEPS_PID_MSCC, "NAS_MSCC_IsNeedDisableLte: ulIsPlaforSupportLte, enUsimCardStatus, enCsimCardStatus",
        ulIsPlaforSupportLte, enUsimCardStatus, enCsimCardStatus);

    NAS_NORMAL_LOG3(UEPS_PID_MSCC, "NAS_MSCC_IsNeedDisableLte: enLteCapStatus, ucSimPsRegStatus, ulEpsAttachAllowFlg",
        enLteCapStatus, ucSimPsRegStatus, ulEpsAttachAllowFlg);

    *penDisableLReason = NAS_MSCC_DISABLE_LTE_REASON_BUTT;

    if (VOS_FALSE == ulIsPlaforSupportLte)
    {
        return VOS_FALSE;
    }

    /* UIM卡时disable lte */
    if ((NAS_MSCC_PIF_CARD_STATUS_ABSENT      == enUsimCardStatus)
     && (NAS_MSCC_PIF_CARD_STATUS_UIM_PRESENT == enCsimCardStatus))
    {
        *penDisableLReason = NAS_MSCC_DISABLE_LTE_REASON_UIM_CARD_SWITCH_ON;

        return VOS_TRUE;
    }

    /* 已经disable L不用重复通知 */
    if (NAS_MSCC_LTE_CAPABILITY_STATUS_DISABLE_NOTIFIED_AS == enLteCapStatus)
    {
        return VOS_FALSE;
    }

    if (VOS_FALSE == ucSimPsRegStatus)
    {
        *penDisableLReason = NAS_MSCC_DISABLE_LTE_REASON_PS_USIM_INVALID;

        return VOS_TRUE;
    }

    if (VOS_FALSE == ulEpsAttachAllowFlg)
    {
        *penDisableLReason = NAS_MSCC_DISABLE_LTE_REASON_PS_USER_DETACH;

        return VOS_TRUE;
    }
    return VOS_FALSE;
}




VOS_UINT32 NAS_MSCC_IsNeedAcqLteUponAttachPs(VOS_VOID)
{
    /* 如果可以进行多模搜网,可以等待DO捕获失败或available超时触发搜LTE */
    if (VOS_TRUE == NAS_MSCC_IsNeedPerformCLSysAcq())
    {
        return VOS_FALSE;
    }

    /* lte处于激活态,可以让MMC控制搜LTE */
    if (VOS_TRUE == NAS_MSCC_Get3gppActiveFlg())
    {
        return VOS_FALSE;
    }

    /* 如果需要优先捕获lte则搜LTE;否则不搜LTE */
    if (VOS_TRUE == NAS_MSCC_IsNeedAcqLte())
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}




VOS_UINT32 NAS_MSCC_IsNeedStart1xBsrLteTimer(VOS_VOID)
{
    NAS_MSCC_1X_SERVICE_CL_SYSTEM_ACQUIRE_CTRL_STRU        *pst1xSrvClSysAcqCtrl;
    VOS_UINT32                                              ul1xNormalSrvFlg;
    VOS_UINT32                                              ulHrpdNormalSrvFlg;
    VOS_UINT32                                              ul3gppNormalSrvFlg;
    VOS_UINT32                                              ulDataCallSwitchTo1x;
    NAS_MSCC_FSM_ID_ENUM_UINT32                             enCurFsmId;

    pst1xSrvClSysAcqCtrl    = NAS_MSCC_Get1xServiceClSysAcqCtrlAddr();
    ul1xNormalSrvFlg        = NAS_MSCC_Is1xNormalService();
    ulHrpdNormalSrvFlg      = NAS_MSCC_IsHrpdNormalService();
    ul3gppNormalSrvFlg      = NAS_MSCC_Is3gppNormalService();
    ulDataCallSwitchTo1x    = NAS_MSCC_GetDataCallSwitchTo1xFlg();
    enCurFsmId              = NAS_MSCC_GetCurrFsmId();

    NAS_NORMAL_LOG3(UEPS_PID_MSCC, "NAS_MSCC_IsNeedAcqLte:NAS_MSCC_IsNeedStart1xBsrLteTimer, ul1xNormalSrvFlg and ulHrpdNormalSrvFlg and ul3gppNormalSrvFlg",
                                    ul1xNormalSrvFlg, ulHrpdNormalSrvFlg, ul3gppNormalSrvFlg);

    NAS_NORMAL_LOG3(UEPS_PID_MSCC, "NAS_MSCC_IsNeedAcqLte:NAS_MSCC_IsNeedStart1xBsrLteTimer, ulDataCallSwitchTo1x and enCurFsmId and current 1x bsr lte active flag",
                                    ulDataCallSwitchTo1x, enCurFsmId, pst1xSrvClSysAcqCtrl->uc1xBsrLteActiveFlg);

    if (VOS_FALSE == pst1xSrvClSysAcqCtrl->uc1xBsrLteActiveFlg)
    {
        return VOS_FALSE;
    }

    if (VOS_FALSE == ul1xNormalSrvFlg)
    {
        return VOS_FALSE;
    }

    if (VOS_FALSE == ulDataCallSwitchTo1x)
    {
        return VOS_FALSE;
    }

    if (NAS_MSCC_FSM_L1_MAIN != enCurFsmId)
    {
        return VOS_FALSE;
    }

    if (VOS_TRUE == ulHrpdNormalSrvFlg)
    {
        return VOS_FALSE;
    }

    if (VOS_TRUE == ul3gppNormalSrvFlg)
    {
        return VOS_FALSE;
    }

    if (VOS_TRUE == NAS_MSCC_IsNeedPerformCLSysAcq())
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}



VOS_UINT32 NAS_MSCC_IsUe1xDataServiceExist(VOS_VOID)
{
    NAS_MSCC_PIF_1X_SERVICE_TYPE_ENUM_UINT8                 en1xServiceType;

    en1xServiceType = NAS_MSCC_Get1xUeServiceType();

    if ((NAS_MSCC_PIF_1X_CAS_TCH_STATE   == NAS_MSCC_Get1xUeMainState())
     && (NAS_MSCC_PIF_1X_SERVICE_TYPE_PS == en1xServiceType))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 NAS_MSCC_IsSrlte1xDataServiceState(VOS_VOID)
{
    VOS_UINT32                          ul1xDataServiceFlag;

    ul1xDataServiceFlag = NAS_MSCC_IsUe1xDataServiceExist();

    if ((VOS_TRUE == NAS_MSCC_IsSrlte())
     && (VOS_TRUE == ul1xDataServiceFlag))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 NAS_MSCC_IsNeedPerfromBsrHistorySrch(VOS_VOID)
{
    NAS_MSCC_SYS_ACQ_BSR_CTRL_STRU     *pstSysAcqBsrCtrl = VOS_NULL_PTR;
    VOS_UINT8                           ucCurBsrHistorySrchTimes;
    VOS_UINT8                           ucCurBsrPrefBandSrchTimes;
    VOS_UINT8                           ucCurBsrPhase;

    pstSysAcqBsrCtrl = NAS_MSCC_GetBsrCtrl();
    ucCurBsrPhase    = NAS_MSCC_GetCurBsrTimerPhase();

    ucCurBsrHistorySrchTimes    = NAS_MSCC_GetCurBsrHistorySrchTimes();
    ucCurBsrPrefBandSrchTimes   = NAS_MSCC_GetCurBsrPrefBandSrchTimes();

    NAS_NORMAL_LOG3(UEPS_PID_MSCC, "NAS_MSCC_IsNeedPerfromBsrHistorySrch Enter:ucCurBsrPhase, ucCurBsrHistorySrchTimes and ucCurBsrPrefBandSrchTimes",
                                    ucCurBsrPhase, ucCurBsrHistorySrchTimes, ucCurBsrPrefBandSrchTimes);

    NAS_NORMAL_LOG2(UEPS_PID_MSCC, "NAS_MSCC_IsNeedPerfromBsrHistorySrch Enter:ucNvimMaxBsrHistorySrchTimes and ucNvimMaxBsrPrefBandSrchTimes",
                                    pstSysAcqBsrCtrl->astBsrTimerInfo[ucCurBsrPhase].ucMaxHistorySrchTimes, pstSysAcqBsrCtrl->astBsrTimerInfo[ucCurBsrPhase].ucMaxPrefBandSrchTimes);

    /* 当前阶段NV配置的历史搜次数为0不做历史搜 */
    if (0 == pstSysAcqBsrCtrl->astBsrTimerInfo[ucCurBsrPhase].ucMaxHistorySrchTimes)
    {
        return VOS_FALSE;
    }

    /* 当前阶段NV配置的pref band搜的最大次数为0，历史搜的次数不为0,直接做历史搜 */
    if (0 == pstSysAcqBsrCtrl->astBsrTimerInfo[ucCurBsrPhase].ucMaxPrefBandSrchTimes)
    {
        return VOS_TRUE;
    }

    /* 先判定pref band搜的次数是否达到最大,如果未达到最大,再判定history搜的次数是否达到最大 */
    if (ucCurBsrPrefBandSrchTimes < pstSysAcqBsrCtrl->astBsrTimerInfo[ucCurBsrPhase].ucMaxPrefBandSrchTimes)
    {
        if (ucCurBsrHistorySrchTimes < pstSysAcqBsrCtrl->astBsrTimerInfo[ucCurBsrPhase].ucMaxHistorySrchTimes)
        {
            /* 都未达到最大,做历史搜,递增历史搜次数 */
            return VOS_TRUE;
        }
    }
    else
    {
        /* 此时必然是最后一个阶段,因为BSR结束时,阶段已经修改 */
        /* 当前pref band搜次数达到最大后，不论当前是否在最大阶段,都是先做历史搜 */
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 NAS_MSCC_IsNeedGotoNextBsrSrchPhase(VOS_VOID)
{
    NAS_MSCC_SYS_ACQ_BSR_CTRL_STRU     *pstSysAcqBsrCtrl = VOS_NULL_PTR;
    VOS_UINT8                           ucCurBsrHistorySrchTimes;
    VOS_UINT8                           ucCurBsrPrefBandSrchTimes;
    VOS_UINT8                           ucCurBsrPhase;

    pstSysAcqBsrCtrl = NAS_MSCC_GetBsrCtrl();
    ucCurBsrPhase    = NAS_MSCC_GetCurBsrTimerPhase();

    ucCurBsrHistorySrchTimes    = NAS_MSCC_GetCurBsrHistorySrchTimes();
    ucCurBsrPrefBandSrchTimes   = NAS_MSCC_GetCurBsrPrefBandSrchTimes();

    if ((ucCurBsrHistorySrchTimes  >= pstSysAcqBsrCtrl->astBsrTimerInfo[ucCurBsrPhase].ucMaxHistorySrchTimes)
     && (ucCurBsrPrefBandSrchTimes >= pstSysAcqBsrCtrl->astBsrTimerInfo[ucCurBsrPhase].ucMaxPrefBandSrchTimes))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_VOID NAS_MSCC_UpdateBsrSrchCtrlInfoUponBsrSrchFinished(VOS_VOID)
{
    NAS_MSCC_SYS_ACQ_BSR_CTRL_STRU     *pstSysAcqBsrCtrl    = VOS_NULL_PTR;
    VOS_UINT8                           ucCurBsrHistorySrchTimes;
    VOS_UINT8                           ucCurBsrPrefBandSrchTimes;
    VOS_UINT8                           ucCurBsrPhase;

    pstSysAcqBsrCtrl = NAS_MSCC_GetBsrCtrl();
    ucCurBsrPhase    = NAS_MSCC_GetCurBsrTimerPhase();

    ucCurBsrHistorySrchTimes    = NAS_MSCC_GetCurBsrHistorySrchTimes();
    ucCurBsrPrefBandSrchTimes   = NAS_MSCC_GetCurBsrPrefBandSrchTimes();

    NAS_NORMAL_LOG3(UEPS_PID_MSCC, "NAS_MSCC_UpdateBsrSrchCtrlInfoUponBsrSrchFinished Enter:ucCurBsrPhase, ucCurBsrHistorySrchTimes and ucCurBsrPrefBandSrchTimes",
                                    ucCurBsrPhase, ucCurBsrHistorySrchTimes, ucCurBsrPrefBandSrchTimes);

    if (NAS_MSCC_BSR_SRCH_TYPE_HISTORY == NAS_MSCC_GetCurBsrSrchType())
    {
        ucCurBsrHistorySrchTimes++;
        NAS_MSCC_SetCurBsrHistorySrchTimes(ucCurBsrHistorySrchTimes);
    }

    if (NAS_MSCC_BSR_SRCH_TYPE_PREF_BAND == NAS_MSCC_GetCurBsrSrchType())
    {
        ucCurBsrPrefBandSrchTimes++;
        NAS_MSCC_SetCurBsrPrefBandSrchTimes(ucCurBsrPrefBandSrchTimes);
    }

    if (VOS_TRUE == NAS_MSCC_IsNeedGotoNextBsrSrchPhase())
    {
        NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_UpdateBsrSrchCtrlInfoUponBsrSrchFinished enter next phase.");

        NAS_MSCC_SetCurBsrHistorySrchTimes(0);
        NAS_MSCC_SetCurBsrPrefBandSrchTimes(0);

        if ((ucCurBsrPhase + 1) < pstSysAcqBsrCtrl->ucBsrPhaseNum)
        {
            NAS_MSCC_SetCurBsrTimerPhase(ucCurBsrPhase + 1);
        }
    }
    else
    {
        /* 开始pref band搜索,清除history搜索次数 */
        if (NAS_MSCC_BSR_SRCH_TYPE_PREF_BAND == NAS_MSCC_GetCurBsrSrchType())
        {
            NAS_MSCC_SetCurBsrHistorySrchTimes(0);
        }
    }

    NAS_NORMAL_LOG3(UEPS_PID_MSCC, "NAS_MSCC_UpdateBsrSrchCtrlInfoUponBsrSrchFinished Quit:ucCurBsrPhase, ucCurBsrHistorySrchTimes and ucCurBsrPrefBandSrchTimes",
                                ucCurBsrPhase, ucCurBsrHistorySrchTimes, ucCurBsrPrefBandSrchTimes);
}




VOS_VOID NAS_MSCC_GetAllowSrchedClEplmnlistUpon1xSrvExist(
    NAS_MSCC_EQUPLMN_INFO_STRU         *pstEplmnList
)
{
    NAS_MSCC_PIF_SIM_OPERPLMN_INFO_STRU                    *pstOPlmnInfo;
    NAS_MSCC_PIF_SIM_USERPLMN_INFO_STRU                    *pstUplmnInfo;
    NAS_MSCC_PIF_SIM_EHPLMN_INFO_STRU                      *pstEhplmnInfo;
    VOS_UINT32                                              i;
    VOS_UINT8                                               ucIndex;
    VOS_UINT32                                              ulCurMcc;
    NAS_MSCC_PIF_PLMN_PRIORITY_CLASS_ENUM_UINT8             enLteSysPriClass;

    if (VOS_FALSE == NAS_MSCC_GetAllowSrchLteFlg())
    {
        return;
    }

    enLteSysPriClass  = NAS_MSCC_GetAllowSrchLteSysPriClass();
    pstEhplmnInfo     = NAS_MSCC_GetEhPlmnInfoAddr();
    pstOPlmnInfo      = NAS_MSCC_GetOperPlmnInfoAddr();
    pstUplmnInfo      = NAS_MSCC_GetUserPlmnInfoAddr();

    ulCurMcc = NAS_MSCC_Get1xSysInfoAddr()->ulMcc;

    for (i = 0, ucIndex = 0; i < pstEhplmnInfo->ucEhPlmnNum; i ++)
    {
        if (ulCurMcc == pstEhplmnInfo->astEhPlmnInfo[i].stPlmnId.ulMcc)
        {
            pstEplmnList->astEquPlmnAddr[ucIndex] = pstEhplmnInfo->astEhPlmnInfo[i].stPlmnId;

            ucIndex++;

            if (ucIndex >= MSCC_MMC_MAX_EPLMN_ID_NUM)
            {
                pstEplmnList->ucEquPlmnNum = ucIndex;

                return;
            }
        }
    }

    if (NAS_MSCC_PIF_PLMN_PRIORITY_HOME == enLteSysPriClass)
    {
        pstEplmnList->ucEquPlmnNum = ucIndex;

        return;
    }

    for (i = 0; i < pstUplmnInfo->ucUserPlmnNum; i ++)
    {
        if (ulCurMcc == pstUplmnInfo->astUserPlmnInfo[i].stPlmnId.ulMcc)
        {
            pstEplmnList->astEquPlmnAddr[ucIndex] = pstUplmnInfo->astUserPlmnInfo[i].stPlmnId;

            ucIndex++;

            if (ucIndex >= MSCC_MMC_MAX_EPLMN_ID_NUM)
            {
                pstEplmnList->ucEquPlmnNum = ucIndex;

                return;
            }
        }
    }

    for (i = 0; i < pstOPlmnInfo->usOperPlmnNum; i ++)
    {
        if (ulCurMcc == pstOPlmnInfo->astOperPlmnInfo[i].stPlmnId.ulMcc)
        {
            pstEplmnList->astEquPlmnAddr[ucIndex] = pstOPlmnInfo->astOperPlmnInfo[i].stPlmnId;

            ucIndex++;

            if (ucIndex >= MSCC_MMC_MAX_EPLMN_ID_NUM)
            {
                pstEplmnList->ucEquPlmnNum = ucIndex;

                return;
            }
        }
    }

    pstEplmnList->ucEquPlmnNum = ucIndex;

    return;
}


VOS_VOID NAS_MSCC_GetAllowSrchedClEplmnlistUponDoSrvExist(
    NAS_MSCC_EQUPLMN_INFO_STRU         *pstEplmnList
)
{
    NAS_MSCC_PIF_SIM_OPERPLMN_INFO_STRU                    *pstOPlmnInfo;
    NAS_MSCC_PIF_SIM_USERPLMN_INFO_STRU                    *pstUplmnInfo;
    NAS_MSCC_PIF_SIM_EHPLMN_INFO_STRU                      *pstEhplmnInfo;
    VOS_UINT32                                              i;
    VOS_UINT8                                               ucIndex;
    VOS_UINT32                                              ulCurMcc;

    pstEhplmnInfo     = NAS_MSCC_GetEhPlmnInfoAddr();
    pstOPlmnInfo      = NAS_MSCC_GetOperPlmnInfoAddr();
    pstUplmnInfo      = NAS_MSCC_GetUserPlmnInfoAddr();

    ulCurMcc = NAS_MSCC_GetHrpdSysInfoAddr()->ulMcc;

    for (i = 0, ucIndex = 0; i < pstEhplmnInfo->ucEhPlmnNum; i ++)
    {
        if (ulCurMcc == pstEhplmnInfo->astEhPlmnInfo[i].stPlmnId.ulMcc)
        {
            pstEplmnList->astEquPlmnAddr[ucIndex] = pstEhplmnInfo->astEhPlmnInfo[i].stPlmnId;

            ucIndex++;

            if (ucIndex >= MSCC_MMC_MAX_EPLMN_ID_NUM)
            {
                pstEplmnList->ucEquPlmnNum = ucIndex;

                return;
            }
        }
    }

    for (i = 0; i < pstUplmnInfo->ucUserPlmnNum; i ++)
    {
        if (ulCurMcc == pstUplmnInfo->astUserPlmnInfo[i].stPlmnId.ulMcc)
        {
            pstEplmnList->astEquPlmnAddr[ucIndex] = pstUplmnInfo->astUserPlmnInfo[i].stPlmnId;

            ucIndex++;

            if (ucIndex >= MSCC_MMC_MAX_EPLMN_ID_NUM)
            {
                pstEplmnList->ucEquPlmnNum = ucIndex;

                return;
            }
        }
    }

    for (i = 0; i < pstOPlmnInfo->usOperPlmnNum; i ++)
    {
        if (ulCurMcc == pstOPlmnInfo->astOperPlmnInfo[i].stPlmnId.ulMcc)
        {
            pstEplmnList->astEquPlmnAddr[ucIndex] = pstOPlmnInfo->astOperPlmnInfo[i].stPlmnId;

            ucIndex++;

            if (ucIndex >= MSCC_MMC_MAX_EPLMN_ID_NUM)
            {
                pstEplmnList->ucEquPlmnNum = ucIndex;

                return;
            }
        }
    }

    pstEplmnList->ucEquPlmnNum = ucIndex;

    return;
}


VOS_VOID NAS_MSCC_GetAllowSrchedClEplmnlist(
    NAS_MSCC_EQUPLMN_INFO_STRU         *pstEplmnList
)
{
    if (VOS_TRUE == NAS_MSCC_Get1xCampOnFlag())
    {
        NAS_MSCC_GetAllowSrchedClEplmnlistUpon1xSrvExist(pstEplmnList);
    }
    else if (VOS_TRUE == NAS_MSCC_GetHrpdCampOnFlag())
    {
        NAS_MSCC_GetAllowSrchedClEplmnlistUponDoSrvExist(pstEplmnList);
    }
    else
    {
        return ;
    }

    return;
}



#endif



VOS_UINT32 NAS_MSCC_IsHrpdAndLteBothSupported(VOS_VOID)
{
    /* syscfg set both support HRPD and LTE, we consider it's CL mode */
    VOS_UINT32                          ulLteSupport;

    ulLteSupport = NAS_MSCC_IsSpecRatSupported(NAS_MSCC_PIF_NET_RAT_TYPE_LTE);

    if ((VOS_TRUE == NAS_MSCC_IsSpecRatSupported(NAS_MSCC_PIF_NET_RAT_TYPE_HRPD))
     && (VOS_TRUE == ulLteSupport))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}



VOS_UINT32 NAS_MSCC_IsGULModeConfigured(VOS_VOID)
{
    /* syscfg set not support HRPD and 1X, we consider it's GUL mode */
    VOS_UINT32                          ulSupport1x;
    VOS_UINT32                          ulSupportHrpd;

    ulSupport1x   = NAS_MSCC_IsSpecRatSupported(NAS_MSCC_PIF_NET_RAT_TYPE_1X);
    ulSupportHrpd = NAS_MSCC_IsSpecRatSupported(NAS_MSCC_PIF_NET_RAT_TYPE_HRPD);

    if ((VOS_FALSE == ulSupportHrpd)
     && (VOS_FALSE == ulSupport1x))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_VOID  NAS_MSCC_ConvertSimPlmnToNasPlmn(
    NAS_MSCC_SIM_FORMAT_PLMN_ID_STRU   *pstInSimPlmn,
    NAS_MSCC_PIF_PLMN_ID_STRU          *pstOutNasPlmn
)
{
    VOS_UINT32                          ulTempData;
    NAS_MSCC_PIF_PLMN_ID_STRU           stPlmn;

    /*
       PLMN BCD 编码 方式

       MCC digit 2,  MCC digit 1, ----OCTET 1
       MNC digit 3,  MCC digit 3, ----OCTET 2
       MNC digit 2,  MNC digit 1, ----OCTET 3
    */
    ulTempData = NAS_MSCC_OCTET_LOW_FOUR_BITS & (pstInSimPlmn->aucSimPlmn[0]);
    stPlmn.ulMcc = ulTempData;
    ulTempData = (NAS_MSCC_OCTET_HIGH_FOUR_BITS & (pstInSimPlmn->aucSimPlmn[0])) >> NAS_MSCC_OCTET_MOVE_FOUR_BITS;
    stPlmn.ulMcc |= ulTempData << NAS_MSCC_OCTET_MOVE_EIGHT_BITS;

    ulTempData = NAS_MSCC_OCTET_LOW_FOUR_BITS & (pstInSimPlmn->aucSimPlmn[1]);
    stPlmn.ulMcc |= ulTempData << NAS_MSCC_OCTET_MOVE_SIXTEEN_BITS;
    ulTempData = (NAS_MSCC_OCTET_HIGH_FOUR_BITS & (pstInSimPlmn->aucSimPlmn[1])) >> NAS_MSCC_OCTET_MOVE_FOUR_BITS;

    stPlmn.ulMnc = ulTempData << NAS_MSCC_OCTET_MOVE_SIXTEEN_BITS;
    ulTempData = NAS_MSCC_OCTET_LOW_FOUR_BITS & (pstInSimPlmn->aucSimPlmn[2]);
    stPlmn.ulMnc |= ulTempData;
    ulTempData = (NAS_MSCC_OCTET_HIGH_FOUR_BITS & (pstInSimPlmn->aucSimPlmn[2])) >> NAS_MSCC_OCTET_MOVE_FOUR_BITS;
    stPlmn.ulMnc |= ulTempData << NAS_MSCC_OCTET_MOVE_EIGHT_BITS;

    pstOutNasPlmn->ulMcc = stPlmn.ulMcc;
    pstOutNasPlmn->ulMnc = stPlmn.ulMnc;
}


VOS_VOID  NAS_MSCC_ConvertNasPlmnToSimFormat(
    NAS_MSCC_PIF_PLMN_ID_STRU          *pstNasPlmn,
    NAS_MSCC_SIM_FORMAT_PLMN_ID_STRU   *pstSimPlmn
)
{
    pstSimPlmn->aucSimPlmn[0]
      = (VOS_UINT8)(0x0000000F & pstNasPlmn->ulMcc);
    pstSimPlmn->aucSimPlmn[0]
     |= (VOS_UINT8)((0x00000F00 & pstNasPlmn->ulMcc) >> 4);

    pstSimPlmn->aucSimPlmn[1]
     = (VOS_UINT8)((0x000F0000 & pstNasPlmn->ulMcc) >> 16);

    pstSimPlmn->aucSimPlmn[1]
     |= (VOS_UINT8)((0x000F0000 & pstNasPlmn->ulMnc) >> 12);

    pstSimPlmn->aucSimPlmn[2]
     = (VOS_UINT8)(0x0000000F & pstNasPlmn->ulMnc);
    pstSimPlmn->aucSimPlmn[2]
     |= (VOS_UINT8)((0x00000F00 & pstNasPlmn->ulMnc) >> 4);

    return;
}




VOS_UINT8 NAS_MSCC_ComparePlmn(
    NAS_MSCC_PIF_PLMN_ID_STRU          *pstSrcPlmn,
    NAS_MSCC_PIF_PLMN_ID_STRU          *pstDesPlmn
)
{
    if (pstSrcPlmn->ulMcc != pstDesPlmn->ulMcc)
    {
        return VOS_FALSE;
    }

    if ((NAS_MSCC_NAS_STYLE_WILDCARD_MNC == pstSrcPlmn->ulMnc)
     || (NAS_MSCC_NAS_STYLE_WILDCARD_MNC == pstDesPlmn->ulMnc))
    {
        return VOS_TRUE;
    }

    if (pstSrcPlmn->ulMnc == pstDesPlmn->ulMnc)
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT8 NAS_MSCC_IsPlmnIdValid(
    NAS_MSCC_PIF_PLMN_ID_STRU          *pstPlmnId
)
{
    VOS_UINT8                                               ucResult;

    /*
    The coding of this field is the responsibility of each administration but
    BCD coding shall be used. The MNC shall consist of 2 or 3 digits.
    If a network operator decides to use only two digits in the MNC,
    bits 5 to 8 of octet 7 shall be coded as "1111".
    MCC digit 2     MCC digit 1
    MNC digit 3     MCC digit 3
    MNC digit 2     MNC digit 1
    */

    ucResult = VOS_TRUE;

    if ( ((pstPlmnId->ulMcc & NAS_MSCC_BYTE_MASK) > NAS_MSCC_MAX_DIGIT_VALUE)
      || (((pstPlmnId->ulMcc >> 8) & NAS_MSCC_BYTE_MASK) > NAS_MSCC_MAX_DIGIT_VALUE)
      || (((pstPlmnId->ulMcc >> 16) & NAS_MSCC_BYTE_MASK) > NAS_MSCC_MAX_DIGIT_VALUE))
    {
        ucResult = VOS_FALSE;
    }

    if ((((pstPlmnId->ulMnc) & NAS_MSCC_BYTE_MASK) > NAS_MSCC_MAX_DIGIT_VALUE)
     || (((pstPlmnId->ulMnc >> 8) & NAS_MSCC_BYTE_MASK) > NAS_MSCC_MAX_DIGIT_VALUE))
    {
        ucResult = VOS_FALSE;
    }

    if ((((pstPlmnId->ulMnc >> 16) & NAS_MSCC_BYTE_MASK) > NAS_MSCC_MAX_DIGIT_VALUE)
     && (((pstPlmnId->ulMnc >> 16) & NAS_MSCC_BYTE_MASK) != NAS_MSCC_LOW_BYTE_INVALID))
    {
        ucResult = VOS_FALSE;
    }

    return ucResult;
}



NAS_MSCC_SERVICE_STATUS_ENUM_UINT8 NAS_MSCC_ConvertMmcServiceStatusToMsccFormat(
    NAS_MSCC_PIF_SERVICE_STATUS_ENUM_UINT32                 enMmcPsServiceStatus
)
{
    NAS_MSCC_SERVICE_STATUS_ENUM_UINT8  enPsSrvStatus;

    switch ((VOS_UINT8)enMmcPsServiceStatus)
    {
        case NAS_MSCC_PIF_SERVICE_STATUS_NORMAL_SERVICE:

            enPsSrvStatus = NAS_MSCC_SERVICE_STATUS_NORMAL_SERVICE;
            break;

        case NAS_MSCC_PIF_SERVICE_STATUS_LIMITED_SERVICE:
        case NAS_MSCC_PIF_SERVICE_STATUS_LIMITED_SERVICE_REGION:
        case NAS_MSCC_PIF_SERVICE_STATUS_NO_IMSI:

            enPsSrvStatus = NAS_MSCC_SERVICE_STATUS_LIMITED_SERVICE;
            break;

        case NAS_MSCC_PIF_SERVICE_STATUS_NO_CHANGE:
        case NAS_MSCC_PIF_SERVICE_STATUS_DEEP_SLEEP:
        case NAS_MSCC_PIF_SERVICE_STATUS_NO_SERVICE:

            enPsSrvStatus = NAS_MSCC_SERVICE_STATUS_NO_SERVICE;
            break;

        default:

            enPsSrvStatus =  NAS_MSCC_SERVICE_STATUS_BUTT;
            break;
    }

    return enPsSrvStatus;
}




NAS_MSCC_PIF_PRI_CLASS_ENUM_UINT8 NAS_MSCC_ConvertMmcSysPriClassToMsccFormat(
    NAS_MSCC_PIF_PLMN_PRIORITY_CLASS_ENUM_UINT8             enPriClass
)
{
    NAS_MSCC_PIF_PRI_CLASS_ENUM_UINT8   enMsccSysPriClass;
    switch (enPriClass)
    {
        case NAS_MSCC_PIF_PLMN_PRIORITY_HOME:

            enMsccSysPriClass = NAS_MSCC_PIF_PRI_CLASS_HOME;
            break;

        case NAS_MSCC_PIF_PLMN_PRIORITY_PREF:

            enMsccSysPriClass = NAS_MSCC_PIF_PRI_CLASS_PREF;
            break;

        case NAS_MSCC_PIF_PLMN_PRIORITY_ANY :

            enMsccSysPriClass = NAS_MSCC_PIF_PRI_CLASS_ANY;
            break;

        default:

            enMsccSysPriClass = NAS_MSCC_PIF_PRI_CLASS_ENUM_BUTT;
            break;
    }

    return enMsccSysPriClass;
}



VOS_RATMODE_ENUM_UINT32 NAS_MSCC_ConvertNvPlatformRatToMsccType(
    PLATFORM_RAT_TYPE_ENUM_UINT16       enNvPlatformRatCap
)
{
    VOS_RATMODE_ENUM_UINT32             enNasMsccPlatformRat;

    switch (enNvPlatformRatCap)
    {
        case PLATFORM_RAT_GSM:
            enNasMsccPlatformRat = VOS_RATMODE_GSM;
            break;

        case PLATFORM_RAT_WCDMA:
            enNasMsccPlatformRat = VOS_RATMODE_WCDMA;
            break;

#if (FEATURE_ON == FEATURE_UE_MODE_TDS)
        case PLATFORM_RAT_TDS:
            enNasMsccPlatformRat = VOS_RATMODE_TDS;
            break;
#endif

#if   (FEATURE_ON == FEATURE_LTE)
        case PLATFORM_RAT_LTE:
            enNasMsccPlatformRat = VOS_RATMODE_LTE;
            break;
#endif

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
        case PLATFORM_RAT_1X:
            enNasMsccPlatformRat = VOS_RATMODE_1X;
            break;

        case PLATFORM_RAT_HRPD:
            enNasMsccPlatformRat = VOS_RATMODE_HRPD;
            break;

#endif

        default:
            enNasMsccPlatformRat = VOS_RATMODE_BUTT;
            break;
    }

    return enNasMsccPlatformRat;
}




VOS_UINT32 NAS_MSCC_IsLtePlmnIdChanged(VOS_VOID)
{
    NAS_MSCC_3GPP_SYS_INFO_STRU        *pst3gppSysInfo          = VOS_NULL_PTR;
    NAS_MSCC_MMSS_LOCATION_INFO_STRU   *pstMmssLastLocationInfo = VOS_NULL_PTR;

    /* Get the current camped 3GPP Sys Info address */
    pst3gppSysInfo = NAS_MSCC_Get3gppSysInfoAddr();

    if (NAS_MSCC_SYS_MODE_LTE == pst3gppSysInfo->enSysMode)
    {
        /* Get the MMSS Location info address which stores the MCC And MNC of the last used LTE system */
        pstMmssLastLocationInfo = NAS_MSCC_GetMmssLastLocationInfoAddr();

        if (VOS_FALSE == pstMmssLastLocationInfo->ucLteActiveFlag)
        {
            /* If LTE Active Flag is false previously, then return VOS_TRUE */
            return VOS_TRUE;
        }

        if ((pst3gppSysInfo->stPlmnId.ulMcc != pstMmssLastLocationInfo->stLteLocationInfo.ulMcc)
         || (pst3gppSysInfo->stPlmnId.ulMnc != pstMmssLastLocationInfo->stLteLocationInfo.ulMnc))
        {
            return VOS_TRUE;
        }

        return VOS_FALSE;
    }

    return VOS_FALSE;
}



VOS_VOID NAS_MSCC_SaveSysAcqTimerCfg(
    NAS_NVIM_MSCC_SYS_ACQ_TIMER_CFG_STRU                   *pstNvimSysAcqTimerCfg,
    NAS_MSCC_SYSACQ_TIMER_CFG_STRU                         *pstSysAcqTimerCfgInfo
)
{
    if (0 != pstNvimSysAcqTimerCfg->ulFirstSearchAvailTimerCount)
    {
        /* Read the NVIM Value if the count is not zero */
        pstSysAcqTimerCfgInfo->ulAvailableTimerCountFirstSearch = pstNvimSysAcqTimerCfg->ulFirstSearchAvailTimerCount;
    }

    if (0 != pstNvimSysAcqTimerCfg->ulFirstSearchAvailTimerLen)
    {
        /* Read the NVIM Value if the timer length is not zero */
        pstSysAcqTimerCfgInfo->ulAvailableTimerLengthFirstSearch = pstNvimSysAcqTimerCfg->ulFirstSearchAvailTimerLen
                                                                                  * NAS_MSCC_THOUSAND_MILLISECOND_UNIT;
    }

    if (0 != pstNvimSysAcqTimerCfg->ulDeepSearchAvailTimerLen)
    {
        /* Read the NVIM Value if the timer length is not zero */
        pstSysAcqTimerCfgInfo->ulAvailableTimerLengthDeepSearch = pstNvimSysAcqTimerCfg->ulDeepSearchAvailTimerLen
                                                                                  * NAS_MSCC_THOUSAND_MILLISECOND_UNIT;
    }

    if (0 != pstNvimSysAcqTimerCfg->ulScanTimerLen)
    {
        /* Read the NVIM Value if the timer length is not zero */
        pstSysAcqTimerCfgInfo->ulScanTimerLen                   = pstNvimSysAcqTimerCfg->ulScanTimerLen * NAS_MSCC_THOUSAND_MILLISECOND_UNIT;
    }

    if (0 != pstNvimSysAcqTimerCfg->ulSleepTimerLen)
    {
        /* Read the NVIM Value if the timer length is not zero */
        pstSysAcqTimerCfgInfo->ulSleepTimerLen                  = pstNvimSysAcqTimerCfg->ulSleepTimerLen * NAS_MSCC_THOUSAND_MILLISECOND_UNIT;;
    }


    /* Read the Timer length value from NVIM if the NVIM Active flag is true */

    return;
}




VOS_VOID NAS_MSCC_UpdateLocationInfoToNvim(VOS_VOID)
{
    NAS_MSCC_MMSS_LOCATION_INFO_STRU                       *pstMmssLocationInfo = VOS_NULL_PTR;
    NAS_MSCC_1X_SYS_INFO_STRU                              *pst1xSysInfo = VOS_NULL_PTR;
    NAS_MSCC_3GPP_SYS_INFO_STRU                            *pst3gppSysInfo = VOS_NULL_PTR;

    /* Get 1x Current camped Sys info global variable address */
    pst1xSysInfo = NAS_MSCC_Get1xSysInfoAddr();

    /* Get 3gpp Current camped sys info global variable address */
    pst3gppSysInfo = NAS_MSCC_Get3gppSysInfoAddr();

    /* Get the MMSS Location Info Global variable address */
    pstMmssLocationInfo = NAS_MSCC_GetMmssLastLocationInfoAddr();

    /* If CL Mode is active, update to NV */
    if (VOS_TRUE == NAS_MSCC_IsGULModeConfigured())
    {
        /* Update the Last Location info as Valid and Sys Acq mode as GUL */
        pstMmssLocationInfo->ucIsLocInfoUsedInSwitchOn = VOS_TRUE;
        pstMmssLocationInfo->enSysAcqMode              = NAS_MSCC_LC_RAT_COMBINED_GUL;

        /* Update the MMSS LOcation info with active flags as FALSE and Clear the 1x and LTE location info */
        pstMmssLocationInfo->ucCdma1xActiveFlag = VOS_FALSE;
        pstMmssLocationInfo->ucLteActiveFlag    = VOS_FALSE;
        PS_MEM_SET(&(pstMmssLocationInfo->st1xLocationInfo), 0x00, sizeof(NAS_MSCC_1X_LOC_INFO_STRU));
        PS_MEM_SET(&(pstMmssLocationInfo->stLteLocationInfo), 0x00, sizeof(NAS_MSCC_LTE_LOC_INFO_STRU));
    }
    else
    {
        /* Update the Last Location info as Invalid and Sys Acq mode as GUL */
        pstMmssLocationInfo->ucIsLocInfoUsedInSwitchOn = VOS_TRUE;
        pstMmssLocationInfo->enSysAcqMode              = NAS_MSCC_LC_RAT_COMBINED_CL;

        /* Update the 1x location info */
        if (VOS_TRUE == pst1xSysInfo->ucCampOnFlg)
        {
            pstMmssLocationInfo->ucCdma1xActiveFlag = VOS_TRUE;
            pstMmssLocationInfo->st1xLocationInfo.ulMcc = pst1xSysInfo->ulMcc;
            pstMmssLocationInfo->st1xLocationInfo.ulMnc = pst1xSysInfo->ulMnc;
            pstMmssLocationInfo->st1xLocationInfo.usSid = pst1xSysInfo->usSid;
            pstMmssLocationInfo->st1xLocationInfo.usNid = pst1xSysInfo->usNid;

            pstMmssLocationInfo->st1xLocationInfo.en1xPrioClass = pst1xSysInfo->en1xPrioClass;
            pstMmssLocationInfo->st1xLocationInfo.enAIPrioClass = pst1xSysInfo->enAIPrioClass;
        }
        else
        {
            /* If 1x system is not camped on, Set 1x active flag to FALSE and clear the 1x location info */
            pstMmssLocationInfo->ucCdma1xActiveFlag = VOS_FALSE;
            PS_MEM_SET(&(pstMmssLocationInfo->st1xLocationInfo), 0x00, sizeof(NAS_MSCC_1X_LOC_INFO_STRU));
        }

        /* Update LTE Location info */
        if ((NAS_MSCC_SYS_MODE_LTE == pst3gppSysInfo->enSysMode)
         && (VOS_TRUE              == pst3gppSysInfo->ucCampOnFlg))
        {
            pstMmssLocationInfo->ucLteActiveFlag = VOS_TRUE;
            pstMmssLocationInfo->stLteLocationInfo.ulMcc = pst3gppSysInfo->stPlmnId.ulMcc;
            pstMmssLocationInfo->stLteLocationInfo.ulMnc = pst3gppSysInfo->stPlmnId.ulMnc;

            pstMmssLocationInfo->stLteLocationInfo.enPrioClass = pst3gppSysInfo->enPrioClass;
        }
        else
        {
            pstMmssLocationInfo->ucLteActiveFlag = VOS_FALSE;
            PS_MEM_SET(&(pstMmssLocationInfo->stLteLocationInfo), 0x00, sizeof(NAS_MSCC_LTE_LOC_INFO_STRU));
        }
    }

    /* Write the location info to NVIM */
    NAS_MSCC_WriteMmssLocationInfoNvim(pstMmssLocationInfo);
}



MSCC_MMA_SYSTEM_ACQUIRE_RESULT_ENUM_UINT32 NAS_MSCC_ConvertMmcPlmnSelRsltToMmaSysAcqRlst(
    NAS_MSCC_PIF_PLMN_SELECTION_RESULT_ENUM_UINT32          enMmcPlmnSelRslt
)
{
    MSCC_MMA_SYSTEM_ACQUIRE_RESULT_ENUM_UINT32              enMmaSysAcqRlst;

    switch (enMmcPlmnSelRslt)
    {
        case NAS_MSCC_PIF_PLMN_SELECTION_RESULT_NORMAL_SERVICE :
            enMmaSysAcqRlst = MSCC_MMA_SYSTEM_ACQUIRE_RESULT_NORMAL_SERVICE;
            break;

        case NAS_MSCC_PIF_PLMN_SELECTION_RESULT_LIMITED_SERVICE :
            enMmaSysAcqRlst = MSCC_MMA_SYSTEM_ACQUIRE_RESULT_LIMITED_SERVICE;
            break;

        case NAS_MSCC_PIF_PLMN_SELECTION_RESULT_INTERRUPT :
            enMmaSysAcqRlst = MSCC_MMA_SYSTEM_ACQUIRE_RESULT_INTERRUPT;
            break;

        case NAS_MSCC_PIF_PLMN_SELECTION_RESULT_NO_SERVICE :
            enMmaSysAcqRlst = MSCC_MMA_SYSTEM_ACQUIRE_RESULT_NO_SERVICE;
            break;

        case NAS_MSCC_PIF_PLMN_SELECTION_RESULT_NO_RF :
            enMmaSysAcqRlst = MSCC_MMA_SYSTEM_ACQUIRE_RESULT_NO_RF;
            break;

        default:
            enMmaSysAcqRlst = MSCC_MMA_SYSTEM_ACQUIRE_RESULT_BUTT;
            NAS_WARNING_LOG(UEPS_PID_MSCC, "NAS_MSCC_ConvertMmcPlmnSelRsltToMmaSysAcqRlst: unknown PlmnSelRslt!");
            break;
    }

    return enMmaSysAcqRlst;
}



VOS_VOID NAS_MSCC_Update3gppSysInfo(
    MMC_MSCC_SYS_INFO_IND_STRU         *pstMmcSysInfo
)
{
    NAS_MSCC_3GPP_SYS_INFO_STRU        *pst3gppSysInfo = VOS_NULL_PTR;

    pst3gppSysInfo = NAS_MSCC_Get3gppSysInfoAddr();

    /* 更新当前的系统信息 */
    pst3gppSysInfo->enSysMode       = pstMmcSysInfo->enCurNetWork;
    pst3gppSysInfo->enLmmAccessType = pstMmcSysInfo->enLmmAccessType;
    pst3gppSysInfo->stPlmnId.ulMcc  = pstMmcSysInfo->stPlmnId.ulMcc;
    pst3gppSysInfo->stPlmnId.ulMnc  = pstMmcSysInfo->stPlmnId.ulMnc;
    pst3gppSysInfo->usLac           = pstMmcSysInfo->usLac;
    pst3gppSysInfo->ucRac           = pstMmcSysInfo->ucRac;
    pst3gppSysInfo->ulCellId        = pstMmcSysInfo->stCellId.aulCellId[0];
    pst3gppSysInfo->ucRoamFlag      = pstMmcSysInfo->ucRoamFlag;
    pst3gppSysInfo->ucCampOnFlg     = VOS_TRUE;

    if (NAS_MSCC_PIF_NET_RAT_TYPE_LTE == pstMmcSysInfo->enCurNetWork)
    {
        pst3gppSysInfo->enPrioClass = NAS_MSCC_ConvertMmcSysPriClassToMsccFormat(pstMmcSysInfo->enPrioClass);

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
        NAS_MSCC_SetLastCampOnLTEOrDoRat(NAS_MSCC_PIF_NET_RAT_TYPE_LTE);
#endif
    }

    return;
}


VOS_VOID NAS_MSCC_GetPlmnListInPlmnPriClassListWithPriClass(
    NAS_MSCC_PIF_PLMN_PRI_CLASS_LIST_INFO_STRU             *pstPlmnPriClassListInfo,
    NAS_MSCC_MSPL_SYS_PRI_CLASS_ENUM_UINT8                  enSysPriClass,
    NAS_MSCC_PIF_SEARCHED_PLMN_LIST_INFO_STRU              *pstDestPlmnList
)
{
    VOS_UINT8                           i;
    VOS_UINT8                           j;
    VOS_UINT8                           ucPriClassMatched;
    VOS_UINT8                           ucPlmnNum;

    ucPriClassMatched = VOS_FALSE;
    ucPlmnNum         = NAS_COMM_MIN(NAS_MSCC_PIF_MAX_HIGH_QUALITY_PLMN_NUM, pstPlmnPriClassListInfo->ucHighPlmnNum);

    for (i = 0, j = 0; i < ucPlmnNum; i++)
    {
        /* for forbidden plmn, we should skip */
        if (VOS_TRUE == pstPlmnPriClassListInfo->astHighPlmnPriClassInfo[i].ucIsForbidden)
        {
            continue;
        }

        if (VOS_FALSE == pstPlmnPriClassListInfo->astHighPlmnPriClassInfo[i].ucAllowSrchInCLMode)
        {
            continue;
        }

        if (VOS_TRUE == NAS_MSCC_IsPlmnPriClassMatchMsplSysPriClass(pstPlmnPriClassListInfo->astHighPlmnPriClassInfo[i].enPriClass,
                                                                    enSysPriClass))
        {
            ucPriClassMatched = VOS_TRUE;
        }

        if (VOS_TRUE == ucPriClassMatched)
        {
            pstDestPlmnList->astHighPlmnID[j] = pstPlmnPriClassListInfo->astHighPlmnPriClassInfo[i].stPlmnId;
            j++;

            ucPriClassMatched = VOS_FALSE;
        }
    }

    pstDestPlmnList->ucHighPlmnNum = j;

    ucPriClassMatched = VOS_FALSE;
    ucPlmnNum         = NAS_COMM_MIN(NAS_MSCC_PIF_MAX_LOW_QUALITY_PLMN_NUM, pstPlmnPriClassListInfo->ucLowPlmnNum);

    for (i = 0, j = 0; i < ucPlmnNum; i++)
    {
        /* for forbidden plmn, we should skip */
        if (VOS_TRUE == pstPlmnPriClassListInfo->astLowPlmnPriClassInfo[i].ucIsForbidden)
        {
            continue;
        }

        if (VOS_FALSE == pstPlmnPriClassListInfo->astLowPlmnPriClassInfo[i].ucAllowSrchInCLMode)
        {
            continue;
        }

        if (VOS_TRUE == NAS_MSCC_IsPlmnPriClassMatchMsplSysPriClass(pstPlmnPriClassListInfo->astLowPlmnPriClassInfo[i].enPriClass,
                                                                    enSysPriClass))
        {
            ucPriClassMatched = VOS_TRUE;
        }

        if (VOS_TRUE == ucPriClassMatched)
        {
            pstDestPlmnList->astLowPlmnID[j] = pstPlmnPriClassListInfo->astLowPlmnPriClassInfo[i].stPlmnId;
            j++;

            ucPriClassMatched = VOS_FALSE;
        }
    }

    pstDestPlmnList->ucLowPlmnNum = j;

    pstDestPlmnList->enRat = pstPlmnPriClassListInfo->enRat;

    return;
}


VOS_RATMODE_ENUM_UINT32 NAS_MSCC_TransferMsccToVosRat(
    NAS_MSCC_PIF_NET_RAT_TYPE_ENUM_UINT8         enRatType
)
{
    switch (enRatType)
    {
        case NAS_MSCC_PIF_NET_RAT_TYPE_GSM:
            return VOS_RATMODE_GSM;

        case NAS_MSCC_PIF_NET_RAT_TYPE_WCDMA:
            return VOS_RATMODE_WCDMA;

        case NAS_MSCC_PIF_NET_RAT_TYPE_LTE:
            return VOS_RATMODE_LTE;

        case NAS_MSCC_PIF_NET_RAT_TYPE_1X:
            return VOS_RATMODE_1X;

        case NAS_MSCC_PIF_NET_RAT_TYPE_HRPD:
            return VOS_RATMODE_HRPD;

        default:
            return VOS_RATMODE_BUTT;
    }
}




VOS_UINT32 NAS_MSCC_Is3gppNormalService(VOS_VOID)
{
    if (VOS_FALSE == NAS_MSCC_Get3gppCampOnFlag())
    {
        return VOS_FALSE;
    }

    if (NAS_MSCC_SERVICE_STATUS_NORMAL_SERVICE == NAS_MSCC_GetCurr3gppPsServiceStatus())
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}



VOS_UINT32 NAS_MSCC_IsHrpdNormalService(VOS_VOID)
{
    if (VOS_FALSE == NAS_MSCC_GetHrpdCampOnFlag())
    {
        return VOS_FALSE;
    }

    if (NAS_MSCC_SERVICE_STATUS_NORMAL_SERVICE == NAS_MSCC_GetCurrHrpdServiceStatus())
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}



VOS_UINT32 NAS_MSCC_Is1xNormalService(VOS_VOID)
{
    if (VOS_FALSE == NAS_MSCC_Get1xCampOnFlag())
    {
        return VOS_FALSE;
    }

    if (NAS_MSCC_SERVICE_STATUS_NORMAL_SERVICE == NAS_MSCC_GetCurr1xServiceStatus())
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}



VOS_UINT32 NAS_MSCC_IsPlmnPriClassMatchMsplSysPriClass(
    NAS_MSCC_PIF_PLMN_PRIORITY_CLASS_ENUM_UINT8             enPriClass,
    NAS_MSCC_MSPL_SYS_PRI_CLASS_ENUM_UINT8                  enSysPriClass
)
{
    if (NAS_MSCC_MSPL_SYS_PRI_CLASS_ANY == enSysPriClass)
    {
        return VOS_TRUE;
    }

    if ((NAS_MSCC_MSPL_SYS_PRI_CLASS_HOME_AND_PREF == enSysPriClass)
     && ((NAS_MSCC_PIF_PLMN_PRIORITY_HOME          == enPriClass)
      || (NAS_MSCC_PIF_PLMN_PRIORITY_PREF          == enPriClass)))
    {
        return VOS_TRUE;
    }

    if ((NAS_MSCC_MSPL_SYS_PRI_CLASS_HOME == enSysPriClass)
     && (NAS_MSCC_PIF_PLMN_PRIORITY_HOME  == enPriClass))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}



VOS_VOID NAS_MSCC_SoftReBoot_WithLineNoAndFileID(
    VOS_UINT32                          ulLineNO,
    VOS_UINT32                          ulFileID,
    NAS_MSCC_REBOOT_SCENE_ENUM_UINT8    ucRebootScene,
    VOS_UINT32                          ulPid
)
{
    VOS_UINT32                          ulSlice;
    VOS_UINT32                          ulTaskTcb;
    VOS_UINT32                          enFsmId;
    VOS_UINT32                          enFsmTopState;

    ulSlice       = VOS_GetSlice();
    ulTaskTcb     = VOS_GetTCBFromPid(ulPid);

    /* clear coverity warning */
    enFsmTopState = NAS_MSCC_GetFsmTopState();
    if (0 < enFsmTopState)
    {
        NAS_NORMAL_LOG1(UEPS_PID_MSCC, "NAS_MSCC_SoftReBoot_WithLineNoAndFileID: NAS_MSCC_GetFsmTopState:", enFsmTopState);
    }

    enFsmId       = NAS_MSCC_GetCurrFsmId();
    if (NAS_MSCC_BUTT == enFsmId)
    {
        NAS_WARNING_LOG(UEPS_PID_MSCC, "NAS_MSCC_SoftReBoot_WithLineNoAndFileID: NAS_MSCC_GetCurrFsmId return NAS_MSCC_BUTT");
    }

    mdrv_om_system_error( NAS_REBOOT_MOD_ID_MSCC | (VOS_INT)ucRebootScene
                          | (VOS_INT)(enFsmTopState << 8) | (VOS_INT)(enFsmId << 16),
                      (VOS_INT)ulTaskTcb,
                      (VOS_INT)((ulFileID << 16) | ulLineNO),
                      (VOS_CHAR *)(&ulSlice),
                      sizeof(ulSlice) );

    return;

}



VOS_UINT32 NAS_MSCC_IsEplmnListChanged(
    NAS_MSCC_EQUPLMN_INFO_STRU         *pstOldEplmnList,
    NAS_MSCC_EQUPLMN_INFO_STRU         *pstCurEplmnList
)
{
    VOS_UINT32                          i;
    VOS_UINT32                          j;

    if (pstCurEplmnList->ucEquPlmnNum != pstOldEplmnList->ucEquPlmnNum)
    {
        return VOS_TRUE;
    }

    for (i = 0; i < pstOldEplmnList->ucEquPlmnNum; i++)
    {
        for (j = 0; j < pstCurEplmnList->ucEquPlmnNum; j++)
        {
            if ((pstOldEplmnList->astEquPlmnAddr[i].ulMcc != pstCurEplmnList->astEquPlmnAddr[j].ulMcc)
             || (pstOldEplmnList->astEquPlmnAddr[i].ulMnc != pstCurEplmnList->astEquPlmnAddr[j].ulMnc))
            {
                return VOS_TRUE;
            }
        }
    }

    return VOS_FALSE;
}




#if (FEATURE_PTM == FEATURE_ON)
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

NAS_ERR_LOG_CL_SEARCH_RSLT_ENUM_UINT32 NAS_MSCC_ConversionHrpdAcqRsltToClOosSrchRslt(
    VOS_UINT32                          ulSrchRslt
)
{
    NAS_ERR_LOG_CL_SEARCH_RSLT_ENUM_UINT32                  enErrLogSrchRslt;

    switch (ulSrchRslt)
    {
        case NAS_MSCC_PIF_ACQUIRED_RESULT_SUCC:
            enErrLogSrchRslt = NAS_ERR_LOG_CL_SEARCH_RSLT_HRPD_SUCC;
            break;
        case NAS_MSCC_PIF_ACQUIRED_RESULT_FAIL:
            enErrLogSrchRslt = NAS_ERR_LOG_CL_SEARCH_RSLT_HRPD_FAIL;
            break;
        case NAS_MSCC_PIF_ACQUIRED_RESULT_ABORTED:
            enErrLogSrchRslt = NAS_ERR_LOG_CL_SEARCH_RSLT_HRPD_ABORTED;
             break;
        case NAS_MSCC_PIF_ACQUIRED_RESULT_NO_RF:
            enErrLogSrchRslt = NAS_ERR_LOG_CL_SEARCH_RSLT_HRPD_NO_RF;
            break;
        case NAS_MSCC_PIF_ACQUIRED_RESULT_REG_NO_RF:
            enErrLogSrchRslt = NAS_ERR_LOG_CL_SEARCH_RSLT_HRPD_REG_NO_RF;
            break;
        default:
            enErrLogSrchRslt = NAS_ERR_LOG_CL_SEARCH_RSLT_BUTT;
            break;
        }

    return enErrLogSrchRslt;
}


NAS_ERR_LOG_CL_SEARCH_RSLT_ENUM_UINT32 NAS_MSCC_ConvertLteAcqRsltToClOosSrchRslt(
    VOS_UINT32                          ulSrchRslt
)
{
    NAS_ERR_LOG_CL_SEARCH_RSLT_ENUM_UINT32                  enErrLogSrchRslt;

    switch (ulSrchRslt)
    {
        case NAS_MSCC_PIF_PLMN_SELECTION_RESULT_NORMAL_SERVICE:
            enErrLogSrchRslt = NAS_ERR_LOG_CL_SEARCH_RSLT_LTE_NORMAL_SRV;
            break;
        case NAS_MSCC_PIF_PLMN_SELECTION_RESULT_LIMITED_SERVICE:
            enErrLogSrchRslt = NAS_ERR_LOG_CL_SEARCH_RSLT_LTE_LIMITED_SRV;
            break;
        case NAS_MSCC_PIF_PLMN_SELECTION_RESULT_NO_SERVICE:
            enErrLogSrchRslt = NAS_ERR_LOG_CL_SEARCH_RSLT_LTE_NO_SRV;
            break;
        case NAS_MSCC_PIF_PLMN_SELECTION_RESULT_INTERRUPT:
            enErrLogSrchRslt = NAS_ERR_LOG_CL_SEARCH_RSLT_LTE_INTERUPT;
            break;
        case NAS_MSCC_PIF_PLMN_SELECTION_RESULT_NO_RF:
            enErrLogSrchRslt = NAS_ERR_LOG_CL_SEARCH_RSLT_LTE_NO_RF;
            break;
        case NAS_MSCC_PIF_PLMN_SELECTION_RESULT_L2C:
            enErrLogSrchRslt = NAS_ERR_LOG_CL_SEARCH_RSLT_LTE_L2C;
            break;
        default:
            enErrLogSrchRslt = NAS_ERR_LOG_CL_SEARCH_RSLT_BUTT;
            break;
        }

    return enErrLogSrchRslt;
}


VOS_VOID NAS_MSCC_SaveErrLogCcbSysAcqStartInfo(
    VOS_RATMODE_ENUM_UINT32                                 enRatMode,
    NAS_MSCC_PIF_PLMN_SEARCH_TYPE_ENUM_UINT32               enLteSrchType
)
{
    VOS_UINT32                          ulIndex;
    VOS_UINT32                          ulIsNeedClSysAcq;

    /* 开始统计其它信息前，先调用该函数，对ulSearchRecordCnt加1 */
    NAS_ERRLOG_CCB_InCreaseClOosSrchInfoRecordCnt();
    ulIndex = NAS_ERRLOG_CCB_CalcClOosSrchInfoRecordIndex();

    /* 搜网记录全局变量初始化 */
    NAS_ERRLOG_CCB_InitClOosSrchInfoRecord(ulIndex);

    NAS_ERRLOG_CCB_SetClOosSrchInfoRecordIndex(ulIndex, NAS_ERRLOG_CCB_GetClOosSrchInfoRecordCnt());
    NAS_ERRLOG_CCB_SetClOosSrchInfoStartTime(ulIndex, VOS_GetSlice());
    NAS_ERRLOG_CCB_SetClOosSrchInfoSearchRat(ulIndex, enRatMode);

    /* 搜网类型只在LTE搜网时有效 */
    NAS_ERRLOG_CCB_SetClOosSrchInfoSearchType(ulIndex, enLteSrchType);

    ulIsNeedClSysAcq = NAS_MSCC_IsNeedPerformCLSysAcq();
    if (VOS_FALSE == ulIsNeedClSysAcq)
    {
        NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_SaveErrLogCcbSysAcqStartInfo(): not cl sys acq scene!");
    }

    NAS_ERRLOG_CCB_SetClOosSrchInfoIsHrpdAndLteBothSupport(ulIsNeedClSysAcq);
}


VOS_VOID NAS_MSCC_SaveErrLogCcbSysAcqEndInfo(
    VOS_RATMODE_ENUM_UINT32                                 enRatMode,
    NAS_ERR_LOG_CL_SEARCH_RSLT_ENUM_UINT32                  enErrLogSrchRslt
)
{
    VOS_UINT32                                             ulIndex;
    NAS_ERRLOG_CCB_CL_OOS_SEARCH_RECORD_STRU              *pstClOosSrchRecord;

    ulIndex = NAS_ERRLOG_CCB_CalcClOosSrchInfoRecordIndex();
    /*  有效性检查，如果没有搜网开始记录，认为异常，丢弃结束记录 */
    pstClOosSrchRecord = NAS_ERRLOG_CCB_GetClOosSrchInfoRecord(ulIndex);
    if ((0 == pstClOosSrchRecord->ulStartTimer)
     || (enRatMode != pstClOosSrchRecord->ulSearchRat))
    {
        return;
    }

    NAS_ERRLOG_CCB_SetClOosSrchInfoEndTime(ulIndex, VOS_GetSlice());

    /* CHR定义的结果  */
    NAS_ERRLOG_CCB_SetClOosSrchInfoSearchRslt(ulIndex, enErrLogSrchRslt);
}
#endif
#endif


VOS_UINT32 NAS_MSCC_IsSrlte(VOS_VOID)
{
    if (PS_RRM_VERSION_SRLTE == RRM_GetVersionType())
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */



