

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "NasErrLogCcbCtx.h"
#include "Nas_Mem.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_NAS_ERRLOG_CCB_CTX_C


#if (FEATURE_PTM == FEATURE_ON)

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
NAS_ERRLOG_CCB_CTX_STRU g_stNasErrLogCcbCtx;

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
#if (FEATURE_ON == FEATURE_MULTI_MODEM)
extern NAS_ERRLOG_CCB_CTX_STRU* I1_NAS_ERRLOG_CCB_GetCcbCtxAddr(VOS_VOID);

#if (3 == MULTI_MODEM_NUMBER)
extern NAS_ERRLOG_CCB_CTX_STRU* I2_NAS_ERRLOG_CCB_GetCcbCtxAddr(VOS_VOID);
#endif
#endif

#endif /* (FEATURE_ON == FEATURE_UE_MODE_CDMA) */

/*****************************************************************************
  3 函数实现
*****************************************************************************/


NAS_ERRLOG_CCB_CTX_STRU* NAS_ERRLOG_CCB_GetCcbCtxAddr(VOS_VOID)
{
    return &(g_stNasErrLogCcbCtx);
}

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

#if defined(INSTANCE_1) || defined(INSTANCE_2)
/* NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr 函数本身已经区分Modem，这个函数单编   */
#else

NAS_ERRLOG_CCB_CDMA_CTX_STRU* NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr(VOS_VOID)
{
    if (MODEM_ID_0 == NAS_ERRLOG_CCB_GetCcbCtxAddr()->stErrLogCcbCdmaCtx.stModemInfo.enCdmaModeModemId)
    {
        return &(NAS_ERRLOG_CCB_GetCcbCtxAddr()->stErrLogCcbCdmaCtx);
    }
#if (FEATURE_ON == FEATURE_MULTI_MODEM)
    else if (MODEM_ID_1 == NAS_ERRLOG_CCB_GetCcbCtxAddr()->stErrLogCcbCdmaCtx.stModemInfo.enCdmaModeModemId)
    {
        return &(I1_NAS_ERRLOG_CCB_GetCcbCtxAddr()->stErrLogCcbCdmaCtx);
    }
#if (3 == MULTI_MODEM_NUMBER)
    else if (MODEM_ID_2 == NAS_ERRLOG_CCB_GetCcbCtxAddr()->stErrLogCcbCdmaCtx.stModemInfo.enCdmaModeModemId)
    {
        return &(I2_NAS_ERRLOG_CCB_GetCcbCtxAddr()->stErrLogCcbCdmaCtx);
    }
#endif
#endif
    else
    {
        return &(NAS_ERRLOG_CCB_GetCcbCtxAddr()->stErrLogCcbCdmaCtx);
    }
}
#endif


VOS_VOID NAS_ERRLOG_CCB_SetCdmaModemId(
    MODEM_ID_ENUM_UINT16                enModeModemId
)
{
    NAS_ERRLOG_CCB_GetCcbCtxAddr()->stErrLogCcbCdmaCtx.stModemInfo.enCdmaModeModemId = enModeModemId;
}


NAS_ERRLOG_CCB_CDMA_MAIN_ACTIVE_REPORT_CONTROL_STRU* NAS_ERRLOG_CCB_GetMainActiveReportControlCtxAddr(VOS_VOID)
{
    return &(NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbMainActiveReportControl);
}


NAS_ERRLOG_CCB_CDMA_SELF_ACTIVE_REPORT_CONTROL_STRU* NAS_ERRLOG_CCB_GetCdmaActiveReportControlCtxAddr(VOS_VOID)
{
    return &(NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbCdmaActiveReportControl);
}


NAS_ERRLOG_CCB_1X_COMMON_INFO_STRU* NAS_ERRLOG_CCB_Get1xCommonInfoCtxAddr(VOS_VOID)
{
    return &(NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcb1xCommonInfo);
}


NAS_ERRLOG_CCB_HRPD_COMMON_INFO_STRU* NAS_ERRLOG_CCB_GetHrpdCommonInfoCtxAddr(VOS_VOID)
{
    return &(NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbHrpdCommonInfo);
}


NAS_ERRLOG_CCB_1X_OOS_RECOVERY_STRU* NAS_ERRLOG_CCB_Get1xOosRecoveryCtxAddr(VOS_VOID)
{
    return &(NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcb1xOosRecovery);
}


NAS_ERRLOG_CCB_1X_OOS_SEARCH_STRU* NAS_ERRLOG_CCB_Get1xOosSearchCtxAddr(VOS_VOID)
{
    return &(NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcb1xOosSearchRecord);
}


NAS_ERRLOG_CCB_CDMA_PS_CALL_EXCEPTION_STRU* NAS_ERRLOG_CCB_GetCdmaPsCallExpCtxAddr(VOS_VOID)
{
    return &(NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbCdmaPsCallExp);
}


NAS_ERRLOG_CCB_CDMA_PS_DISC_INFO_STRU* NAS_ERRLOG_CCB_GetCdmaPsDiscCtxAddr(VOS_VOID)
{
    return &(NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbCdmaPsDiscInfo);
}


NAS_ERRLOG_CCB_HRPD_UATI_FAIL_STRU* NAS_ERRLOG_CCB_GetHrpdUatiFailCtxAddr(VOS_VOID)
{
    return &(NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogHrpdSessionInfo.stHrpdUatiFail);
}


NAS_ERRLOG_CCB_HRPD_SESSION_FAIL_STRU* NAS_ERRLOG_CCB_GetHrpdSessionFailCtxAddr(VOS_VOID)
{
    return &(NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogHrpdSessionInfo.stHrpdSessionFail);
}


NAS_ERRLOG_CCB_CL_OOS_SEARCH_INFO_STRU* NAS_ERRLOG_CCB_GetClOosSrchInfoCtxAddr(VOS_VOID)
{
    return &(NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbCLOosSearchInfo);
}


NAS_ERRLOG_CCB_MODEM_INFO_STRU* NAS_ERRLOG_CCB_GetModemInfoAddr(VOS_VOID)
{
    return &(NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stModemInfo);
}


VOS_VOID NAS_ERRLOG_CCB_InitCcbCtx(VOS_VOID)
{
    NAS_ERRLOG_CCB_InitModemInfo();

    NAS_ERRLOG_CCB_InitMainActiveReportControlCtx();

    NAS_ERRLOG_CCB_InitCdmaActiveReportControlCtx();

    NAS_ERRLOG_CCB_Init1xCommonInfoCtx();

    NAS_ERRLOG_CCB_InitHrpdCommonInfoCtx();

    NAS_ERRLOG_CCB_Init1xOosRecoveryCtx();

    NAS_ERRLOG_CCB_Init1xOosSearchCtx();

    NAS_ERRLOG_CCB_InitCdmaPsCallExpCtx();

    NAS_ERRLOG_CCB_InitCdmaPsDiscInfoCtx();

    NAS_ERRLOG_CCB_InitHrpdUatiFailCtx();

    NAS_ERRLOG_CCB_InitHrpdSessionFailCtx();

    NAS_ERRLOG_CCB_InitClOosSearchInfo();

    NAS_ERRLOG_CCB_Init1xCsCallExceptionCtx();

    NAS_ERRLOG_CCB_Init1xMtServiceExceptionCtx();

    NAS_ERRLOG_CCB_Init1xConversationExceptionCtx();

    NAS_ERRLOG_CCB_InitHrpdOosSearchCtx();
    NAS_ERRLOG_CCB_InitClLteCommonInfoCtx();
    NAS_ERRLOG_CCB_InitHrpdOrLteOosRecoveryCtx();
}



VOS_VOID NAS_ERRLOG_CCB_InitModemInfo(VOS_VOID)
{
    NAS_ERRLOG_CCB_MODEM_INFO_STRU     *pstModemInfo;

    pstModemInfo = NAS_ERRLOG_CCB_GetModemInfoAddr();
    pstModemInfo->enCdmaModeModemId = MODEM_ID_BUTT;
}


VOS_VOID NAS_ERRLOG_CCB_InitMainActiveReportControlCtx(VOS_VOID)
{
    NAS_ERRLOG_CCB_CDMA_MAIN_ACTIVE_REPORT_CONTROL_STRU    *pstMainActiveReportControl;

    pstMainActiveReportControl = NAS_ERRLOG_CCB_GetMainActiveReportControlCtxAddr();

    NAS_MEM_SET_S(pstMainActiveReportControl,
                  sizeof(NAS_ERRLOG_CCB_CDMA_MAIN_ACTIVE_REPORT_CONTROL_STRU),
                  0x00,
                  sizeof(NAS_ERRLOG_CCB_CDMA_MAIN_ACTIVE_REPORT_CONTROL_STRU));
}


VOS_VOID NAS_ERRLOG_CCB_InitCdmaActiveReportControlCtx(VOS_VOID)
{
    NAS_ERRLOG_CCB_CDMA_SELF_ACTIVE_REPORT_CONTROL_STRU    *pstCdmaActiveReportControl;

    pstCdmaActiveReportControl = NAS_ERRLOG_CCB_GetCdmaActiveReportControlCtxAddr();

    NAS_MEM_SET_S(pstCdmaActiveReportControl,
                  sizeof(NAS_ERRLOG_CCB_CDMA_SELF_ACTIVE_REPORT_CONTROL_STRU),
                  0x00,
                  sizeof(NAS_ERRLOG_CCB_CDMA_SELF_ACTIVE_REPORT_CONTROL_STRU));
}


VOS_VOID NAS_ERRLOG_CCB_Init1xCommonInfoCtx(VOS_VOID)
{
    NAS_ERRLOG_CCB_1X_COMMON_INFO_STRU                     *pst1xCommonInfo;

    pst1xCommonInfo = NAS_ERRLOG_CCB_Get1xCommonInfoCtxAddr();

    NAS_MEM_SET_S(pst1xCommonInfo, sizeof(NAS_ERRLOG_CCB_1X_COMMON_INFO_STRU), 0x00, sizeof(NAS_ERRLOG_CCB_1X_COMMON_INFO_STRU));

    pst1xCommonInfo->usSid    = NAS_ERRLOG_CCB_INVALID_SID;
    pst1xCommonInfo->usNid    = NAS_ERRLOG_CCB_INVALID_NID;
    pst1xCommonInfo->usBaseId = NAS_ERRLOG_CCB_INVALID_BASEID;
}


VOS_VOID NAS_ERRLOG_CCB_InitHrpdCommonInfoCtx(VOS_VOID)
{
    NAS_ERRLOG_CCB_HRPD_COMMON_INFO_STRU                   *pstHrpdCommonInfo;

    pstHrpdCommonInfo = NAS_ERRLOG_CCB_GetHrpdCommonInfoCtxAddr();

    NAS_MEM_SET_S(pstHrpdCommonInfo, sizeof(NAS_ERRLOG_CCB_HRPD_COMMON_INFO_STRU), 0x00, sizeof(NAS_ERRLOG_CCB_HRPD_COMMON_INFO_STRU));
}


VOS_VOID NAS_ERRLOG_CCB_Init1xOosRecoveryCtx(VOS_VOID)
{
    NAS_ERRLOG_CCB_1X_OOS_RECOVERY_STRU                    *pst1xOosRecovery;

    pst1xOosRecovery = NAS_ERRLOG_CCB_Get1xOosRecoveryCtxAddr();

    NAS_MEM_SET_S(pst1xOosRecovery, sizeof(NAS_ERRLOG_CCB_1X_OOS_RECOVERY_STRU), 0x00, sizeof(NAS_ERRLOG_CCB_1X_OOS_RECOVERY_STRU));
}


VOS_VOID NAS_ERRLOG_CCB_Init1xOosSearchCtx(VOS_VOID)
{
    NAS_ERRLOG_CCB_1X_OOS_SEARCH_STRU                      *pst1xOosSearch;

    pst1xOosSearch = NAS_ERRLOG_CCB_Get1xOosSearchCtxAddr();

    NAS_MEM_SET_S(pst1xOosSearch, sizeof(NAS_ERRLOG_CCB_1X_OOS_SEARCH_STRU), 0x00, sizeof(NAS_ERRLOG_CCB_1X_OOS_SEARCH_STRU));
}


VOS_VOID NAS_ERRLOG_CCB_InitCdmaPsCallExpCtx(VOS_VOID)
{
    NAS_ERRLOG_CCB_CDMA_PS_CALL_EXCEPTION_STRU             *pstCdmaPsCallExp;

    pstCdmaPsCallExp = NAS_ERRLOG_CCB_GetCdmaPsCallExpCtxAddr();

    NAS_MEM_SET_S(pstCdmaPsCallExp, sizeof(NAS_ERRLOG_CCB_CDMA_PS_CALL_EXCEPTION_STRU), 0x00, sizeof(NAS_ERRLOG_CCB_CDMA_PS_CALL_EXCEPTION_STRU));
}


VOS_VOID NAS_ERRLOG_CCB_InitCdmaPsDiscInfoCtx(VOS_VOID)
{
    NAS_ERRLOG_CCB_CDMA_PS_DISC_INFO_STRU                  *pstCdmaPsDiscInfo;

    pstCdmaPsDiscInfo = NAS_ERRLOG_CCB_GetCdmaPsDiscCtxAddr();

    NAS_MEM_SET_S(pstCdmaPsDiscInfo, sizeof(NAS_ERRLOG_CCB_CDMA_PS_DISC_INFO_STRU), 0x00, sizeof(NAS_ERRLOG_CCB_CDMA_PS_DISC_INFO_STRU));
}


VOS_VOID NAS_ERRLOG_CCB_InitHrpdUatiFailCtx(VOS_VOID)
{
    NAS_ERRLOG_CCB_HRPD_UATI_FAIL_STRU                     *pstHrpdUatiFail;

    pstHrpdUatiFail = NAS_ERRLOG_CCB_GetHrpdUatiFailCtxAddr();

    NAS_MEM_SET_S(pstHrpdUatiFail, sizeof(NAS_ERRLOG_CCB_HRPD_UATI_FAIL_STRU), 0x00, sizeof(NAS_ERRLOG_CCB_HRPD_UATI_FAIL_STRU));
}


VOS_VOID NAS_ERRLOG_CCB_InitHrpdSessionFailCtx(VOS_VOID)
{
    NAS_ERRLOG_CCB_HRPD_SESSION_FAIL_STRU                  *pstHrpdSessionFail;

    pstHrpdSessionFail = NAS_ERRLOG_CCB_GetHrpdSessionFailCtxAddr();

    NAS_MEM_SET_S(pstHrpdSessionFail, sizeof(NAS_ERRLOG_CCB_HRPD_SESSION_FAIL_STRU), 0x00, sizeof(NAS_ERRLOG_CCB_HRPD_SESSION_FAIL_STRU));
}


VOS_VOID NAS_ERRLOG_CCB_InitClOosSearchInfo(VOS_VOID)
{
    NAS_ERRLOG_CCB_CL_OOS_SEARCH_INFO_STRU                  *pstClOosSrchInfo;

    pstClOosSrchInfo = NAS_ERRLOG_CCB_GetClOosSrchInfoCtxAddr();

    NAS_MEM_SET_S(pstClOosSrchInfo, sizeof(NAS_ERRLOG_CCB_CL_OOS_SEARCH_INFO_STRU), 0x00, sizeof(NAS_ERRLOG_CCB_CL_OOS_SEARCH_INFO_STRU));
}


VOS_VOID NAS_ERRLOG_CCB_SetAlreadyReportChr1xOosFlag(
    VOS_UINT8                           ucAlreadyReportChrOosFlag
)
{
    NAS_ERRLOG_CCB_Get1xOosRecoveryCtxAddr()->ucAlreadyReportChrOosFlag = ucAlreadyReportChrOosFlag;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetAlreadyReportChr1xOosFlag(VOS_VOID)
{
    return NAS_ERRLOG_CCB_Get1xOosRecoveryCtxAddr()->ucAlreadyReportChrOosFlag;
}


VOS_VOID NAS_ERRLOG_CCB_SetAlreadyReportOosToApFlag(
    VOS_UINT8                           ucAlreadyReportOosToApFlag
)
{
    NAS_ERRLOG_CCB_Get1xOosRecoveryCtxAddr()->ucAlreadyReportOosToApFlag = ucAlreadyReportOosToApFlag;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetAlreadyReportOosToApFlag(VOS_VOID)
{
    return NAS_ERRLOG_CCB_Get1xOosRecoveryCtxAddr()->ucAlreadyReportOosToApFlag;
}


VOS_VOID NAS_ERRLOG_CCB_SetHrpdUatiFailStartTime(
    VOS_UINT32                          ulStartTime
)
{
    NAS_ERRLOG_CCB_GetHrpdUatiFailCtxAddr()->ulUatiStartTimer = ulStartTime;
}


VOS_UINT32 NAS_ERRLOG_CCB_GetHrpdUatiFailStartTime(VOS_VOID)
{
    return NAS_ERRLOG_CCB_GetHrpdUatiFailCtxAddr()->ulUatiStartTimer;
}


VOS_VOID NAS_ERRLOG_CCB_SetHrpdUatiFailEndTime(
    VOS_UINT32                          ulEndTime
)
{
    NAS_ERRLOG_CCB_GetHrpdUatiFailCtxAddr()->ulUatiEndTimer = ulEndTime;
}


VOS_UINT32 NAS_ERRLOG_CCB_GetHrpdUatiFailEndTime(VOS_VOID)
{
    return NAS_ERRLOG_CCB_GetHrpdUatiFailCtxAddr()->ulUatiEndTimer;
}


VOS_VOID NAS_ERRLOG_CCB_InCreaseHrpdUatiFailRecordCnt(VOS_VOID)
{
    NAS_ERRLOG_CCB_GetHrpdUatiFailCtxAddr()->ucRecordCnt++;
}


VOS_VOID NAS_ERRLOG_CCB_SetHrpdUatiAssignRecordStartTime(
    VOS_UINT32                          ulIndex,
    VOS_UINT32                          ulStartTime
)
{
    NAS_ERRLOG_CCB_GetHrpdUatiFailCtxAddr()->astUatiAssignFailRecord[ulIndex].ulUatiAssignStartTimer = ulStartTime;
}


VOS_VOID NAS_ERRLOG_CCB_SetHrpdUatiAssignRecordEndTime(
    VOS_UINT32                          ulIndex,
    VOS_UINT32                          ulEndTime
)
{
    NAS_ERRLOG_CCB_GetHrpdUatiFailCtxAddr()->astUatiAssignFailRecord[ulIndex].ulUatiAssignEndTimer = ulEndTime;
}


VOS_VOID NAS_ERRLOG_CCB_SetHrpdUatiAssignRecordRslt(
    VOS_UINT32                                              ulIndex,
    NAS_ERR_LOG_HRPD_UATI_ASSIGN_RSLT_ENUM_UINT32           enUatlAssignRlst
)
{
    NAS_ERRLOG_CCB_GetHrpdUatiFailCtxAddr()->astUatiAssignFailRecord[ulIndex].enUatiAssignRslt = enUatlAssignRlst;
}


VOS_VOID NAS_ERRLOG_CCB_SetHrpdUatiAssignRecordIndex(
    VOS_UINT32                          ulIndex
)
{
    NAS_ERRLOG_CCB_GetHrpdUatiFailCtxAddr()->astUatiAssignFailRecord[ulIndex].ulIndex = ulIndex;
}


VOS_UINT32 NAS_ERRLOG_CCB_CalcHrpdUatiAssignFailRecordIndex(VOS_VOID)
{
    VOS_UINT32                          ulIndex;

    if (0 == NAS_ERRLOG_CCB_GetHrpdUatiFailCtxAddr()->ucRecordCnt)
    {
        ulIndex = 0;
    }
    else
    {
        ulIndex = NAS_ERRLOG_CCB_GetHrpdUatiFailCtxAddr()->ucRecordCnt % NAS_ERR_LOG_HRPD_UATI_ASSIGN_MAX_CNT;
        if (0 == ulIndex)
        {
            ulIndex = NAS_ERR_LOG_HRPD_UATI_ASSIGN_MAX_CNT - 1;
        }
        else
        {
            ulIndex -= 1;
        }
    }

    return ulIndex;
}


VOS_VOID NAS_ERRLOG_CCB_SetHrpdUatiRcvSnpDataCnfTimer(
    VOS_UINT32                          ulIndex,
    VOS_UINT32                          ulUatiReqRcvSnpDataCnfTimer
)
{
    NAS_ERRLOG_CCB_GetHrpdUatiFailCtxAddr()->astUatiAssignFailRecord[ulIndex].ulUatiReqRcvSnpDataCnfTimer = ulUatiReqRcvSnpDataCnfTimer;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetHrpdUatiFailRecordCnt(VOS_VOID)
{
    return NAS_ERRLOG_CCB_GetHrpdUatiFailCtxAddr()->ucRecordCnt;
}


VOS_VOID NAS_ERRLOG_CCB_SetHrpdUatiFailTriggerCause(
    VOS_UINT8                           ucTriggerCause
)
{
    NAS_ERRLOG_CCB_GetHrpdUatiFailCtxAddr()->ucTriggerCause = ucTriggerCause;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetHrpdUatiFailTriggerCause(VOS_VOID)
{
    return NAS_ERRLOG_CCB_GetHrpdUatiFailCtxAddr()->ucTriggerCause;
}


VOS_VOID NAS_ERRLOG_CCB_SetHrpdUatiFailCause(
    VOS_UINT8                           ucUatiFailCause
)
{
    NAS_ERRLOG_CCB_GetHrpdUatiFailCtxAddr()->ucUATIFailCause = ucUatiFailCause;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetHrpdUatiFailCause(VOS_VOID)
{
    return NAS_ERRLOG_CCB_GetHrpdUatiFailCtxAddr()->ucUATIFailCause;
}


VOS_VOID NAS_ERRLOG_CCB_SetHrpdSessionFailStartTime(
    VOS_UINT32                          ulStartTime
)
{
    NAS_ERRLOG_CCB_GetHrpdSessionFailCtxAddr()->ulStartTimer = ulStartTime;
}


VOS_UINT32 NAS_ERRLOG_CCB_GetHrpdSessionFailStartTime(VOS_VOID)
{
    return NAS_ERRLOG_CCB_GetHrpdSessionFailCtxAddr()->ulStartTimer;
}


VOS_VOID NAS_ERRLOG_CCB_SetHrpdSessionFailEndTime(
    VOS_UINT32                          ulEndTime
)
{
    NAS_ERRLOG_CCB_GetHrpdSessionFailCtxAddr()->ulEndTimer = ulEndTime;
}


VOS_UINT32 NAS_ERRLOG_CCB_GetHrpdSessionFailEndTime(VOS_VOID)
{
    return NAS_ERRLOG_CCB_GetHrpdSessionFailCtxAddr()->ulEndTimer;
}


VOS_VOID NAS_ERRLOG_CCB_SetHrpdSessionFailActReason(
    VOS_UINT8                           ucSessionActReason
)
{
    NAS_ERRLOG_CCB_GetHrpdSessionFailCtxAddr()->ucSessionActReason = ucSessionActReason;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetHrpdSessionFailActReason(VOS_VOID)
{
    return NAS_ERRLOG_CCB_GetHrpdSessionFailCtxAddr()->ucSessionActReason;
}


VOS_VOID NAS_ERRLOG_CCB_SetHrpdSessionFailActType(
    VOS_UINT8                           ucSessionActType
)
{
    NAS_ERRLOG_CCB_GetHrpdSessionFailCtxAddr()->ucSessionActType = ucSessionActType;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetHrpdSessionFailActType(VOS_VOID)
{
    return NAS_ERRLOG_CCB_GetHrpdSessionFailCtxAddr()->ucSessionActType;
}


VOS_VOID NAS_ERRLOG_CCB_SetHrpdSessionFailSupportEhrpdFlg(
    VOS_UINT8                           ucSupportedEhrpdFlg
)
{
    NAS_ERRLOG_CCB_GetHrpdSessionFailCtxAddr()->ucSupportedEhrpdFlg = ucSupportedEhrpdFlg;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetHrpdSessionFailSupportEhrpdFlg(VOS_VOID)
{
    return NAS_ERRLOG_CCB_GetHrpdSessionFailCtxAddr()->ucSupportedEhrpdFlg;
}


VOS_VOID NAS_ERRLOG_CCB_SetHrpdSessionFailCause(
    VOS_UINT8                           ucSessionFailCause
)
{
    NAS_ERRLOG_CCB_GetHrpdSessionFailCtxAddr()->ucSessionFailCause = ucSessionFailCause;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetHrpdSessionFailCause(VOS_VOID)
{
    return NAS_ERRLOG_CCB_GetHrpdSessionFailCtxAddr()->ucSessionFailCause;
}


VOS_VOID NAS_ERRLOG_CCB_InCreaseClOosSrchInfoRecordCnt(VOS_VOID)
{
    NAS_ERRLOG_CCB_GetClOosSrchInfoCtxAddr()->ulSearchRecordCnt += 1;
}


VOS_UINT32 NAS_ERRLOG_CCB_GetClOosSrchInfoRecordCnt(VOS_VOID)
{
    return NAS_ERRLOG_CCB_GetClOosSrchInfoCtxAddr()->ulSearchRecordCnt;
}


VOS_UINT32 NAS_ERRLOG_CCB_CalcClOosSrchInfoRecordIndex(VOS_VOID)
{
    VOS_UINT32                          ulIndex;
    VOS_UINT32                          ulRecordCnt;

    ulRecordCnt = NAS_ERRLOG_CCB_GetClOosSrchInfoRecordCnt();

    /* 防护，不会出现该场景 */
    if (0 == ulRecordCnt)
    {
        ulIndex = 0;
    }
    else
    {
        ulIndex = ulRecordCnt % NAS_ERRLOG_CCB_CL_OOS_MAX_SRCH_RECORD_NUM;
        if (0 == ulIndex)
        {
            ulIndex = NAS_ERRLOG_CCB_CL_OOS_MAX_SRCH_RECORD_NUM - 1;
        }
        else
        {
            ulIndex -= 1;
        }
    }

    return ulIndex;
}


VOS_VOID NAS_ERRLOG_CCB_SetClOosSrchInfoStartTime(
    VOS_UINT32                          ulIndex,
    VOS_UINT32                          ulStartTime
)
{
    NAS_ERRLOG_CCB_GetClOosSrchInfoCtxAddr()->astSearchRecord[ulIndex].ulStartTimer = ulStartTime;
}


VOS_VOID NAS_ERRLOG_CCB_SetClOosSrchInfoEndTime(
    VOS_UINT32                          ulIndex,
    VOS_UINT32                          ulEndTime
)
{
    NAS_ERRLOG_CCB_GetClOosSrchInfoCtxAddr()->astSearchRecord[ulIndex].ulEndTimer = ulEndTime;
}


VOS_VOID NAS_ERRLOG_CCB_SetClOosSrchInfoRecordIndex(
    VOS_UINT32                          ulIndex,
    VOS_UINT32                          ulRecordCnt
)
{
    NAS_ERRLOG_CCB_GetClOosSrchInfoCtxAddr()->astSearchRecord[ulIndex].ulIndex = ulRecordCnt - 1;
}


VOS_VOID NAS_ERRLOG_CCB_SetClOosSrchInfoSearchRat(
    VOS_UINT32                          ulIndex,
    VOS_UINT32                          ulSearchRat
)
{
    NAS_ERRLOG_CCB_GetClOosSrchInfoCtxAddr()->astSearchRecord[ulIndex].ulSearchRat = ulSearchRat;
}


VOS_VOID NAS_ERRLOG_CCB_SetClOosSrchInfoSearchType(
    VOS_UINT32                          ulIndex,
    VOS_UINT32                          ulSearchType
)
{
    NAS_ERRLOG_CCB_GetClOosSrchInfoCtxAddr()->astSearchRecord[ulIndex].ulSearchType = ulSearchType;
}


VOS_VOID NAS_ERRLOG_CCB_SetClOosSrchInfoSearchRslt(
    VOS_UINT32                                              ulIndex,
    NAS_ERR_LOG_CL_SEARCH_RSLT_ENUM_UINT32                  enSearchRslt
)
{
    NAS_ERRLOG_CCB_GetClOosSrchInfoCtxAddr()->astSearchRecord[ulIndex].enSearchRslt = enSearchRslt;
}


VOS_VOID NAS_ERRLOG_CCB_SetClOosSrchInfoIsHrpdAndLteBothSupport(
    VOS_UINT32                          ulIsHrpdAndLteBothSupport
)
{
    NAS_ERRLOG_CCB_GetClOosSrchInfoCtxAddr()->ulIsHrpdAndLteBothSupport = ulIsHrpdAndLteBothSupport;
}


VOS_VOID NAS_ERRLOG_CCB_InitClOosSrchInfoRecord(
    VOS_UINT32                          ulIndex
)
{
    NAS_MEM_SET_S(&(NAS_ERRLOG_CCB_GetClOosSrchInfoCtxAddr()->astSearchRecord[ulIndex]),
                  sizeof(NAS_ERRLOG_CCB_CL_OOS_SEARCH_RECORD_STRU),
                  0x00,
                  sizeof(NAS_ERRLOG_CCB_CL_OOS_SEARCH_RECORD_STRU));
}


NAS_ERRLOG_CCB_CL_OOS_SEARCH_RECORD_STRU *NAS_ERRLOG_CCB_GetClOosSrchInfoRecord(
    VOS_UINT32                          ulIndex
)
{
    return &(NAS_ERRLOG_CCB_GetClOosSrchInfoCtxAddr()->astSearchRecord[ulIndex]);
}



VOS_VOID NAS_ERRLOG_CCB_InCreasePsCallExcepRedialCnt(VOS_VOID)
{
    NAS_ERRLOG_CCB_GetCdmaPsCallExpCtxAddr()->ulRedialCnt += 1;
}


VOS_UINT32 NAS_ERRLOG_CCB_GetCdmaPsCallExcepRedialCnt(VOS_VOID)
{
    return NAS_ERRLOG_CCB_GetCdmaPsCallExpCtxAddr()->ulRedialCnt;
}


VOS_UINT32 NAS_ERRLOG_CCB_CalcPsCallExcepRedialRecordIndex(VOS_VOID)
{
    VOS_UINT32                          ulIndex;
    VOS_UINT32                          ulRedialCnt;

    ulRedialCnt = NAS_ERRLOG_CCB_GetCdmaPsCallExcepRedialCnt();

    /* 防护，不会出现该场景 */
    if (0 == ulRedialCnt)
    {
        ulIndex = 0;
    }
    else
    {
        ulIndex = ulRedialCnt % NAS_ERR_LOG_MAX_CDMA_PS_CALL_EXCEP_REDIAL_NUM;
        if (0 == ulIndex)
        {
            ulIndex = NAS_ERR_LOG_MAX_CDMA_PS_CALL_EXCEP_REDIAL_NUM - 1;
        }
        else
        {
            ulIndex -= 1;
        }
    }

    return ulIndex;
}


VOS_VOID NAS_ERRLOG_CCB_SetCdmaPsCallExcepRedialRecordIndex(
    VOS_UINT32                          ulRedialIndex,
    VOS_UINT32                          ulRedialCnt
)
{
    NAS_ERRLOG_CCB_GetCdmaPsCallExpCtxAddr()->astRedialInfo[ulRedialIndex].ulIndex = ulRedialCnt - 1;
}


VOS_VOID NAS_ERRLOG_CCB_SetCdmaPsCallExcepRedialStartTime(
    VOS_UINT32                          ulRedialIndex,
    VOS_UINT32                          ulStartTime
)
{
    NAS_ERRLOG_CCB_GetCdmaPsCallExpCtxAddr()->astRedialInfo[ulRedialIndex].ulStartTimer = ulStartTime;
}


VOS_VOID NAS_ERRLOG_CCB_SetCdmaPsCallExcepRedialEndTime(
    VOS_UINT32                          ulRedialIndex,
    VOS_UINT32                          ulEndTime
)
{
    NAS_ERRLOG_CCB_GetCdmaPsCallExpCtxAddr()->astRedialInfo[ulRedialIndex].ulEndTimer = ulEndTime;
}


VOS_VOID NAS_ERRLOG_CCB_SetCdmaPsCallExcepRedialRatMode(
    VOS_UINT32                                              ulRedialIndex,
    NAS_ERR_LOG_PS_CALL_RAT_TYPE_ENUM_UINT32                enApsRatMode
)
{
    NAS_ERRLOG_CCB_GetCdmaPsCallExpCtxAddr()->astRedialInfo[ulRedialIndex].enRatMode = enApsRatMode;
}


VOS_VOID NAS_ERRLOG_CCB_SetCdmaPsCallExcepRedialRslt(
    VOS_UINT32                          ulRedialIndex,
    VOS_UINT32                          ulTafPsCause
)
{
    NAS_ERRLOG_CCB_GetCdmaPsCallExpCtxAddr()->astRedialInfo[ulRedialIndex].ulRedialRslt = ulTafPsCause;
}


VOS_VOID NAS_ERRLOG_CCB_SetCdmaPsCallExcepApsFsmInfo(
    VOS_UINT32                              ulMainFsmId,
    VOS_UINT32                              ulMainFsmState,
    VOS_UINT32                              ulSubFsmId,
    VOS_UINT32                              ulSubFsmState,
    VOS_UINT8                               ucPdpId
)
{
    NAS_ERRLOG_CCB_CDMA_PS_CALL_EXCEPTION_STRU              *pstPsCallExcep;

    pstPsCallExcep = NAS_ERRLOG_CCB_GetCdmaPsCallExpCtxAddr();

    pstPsCallExcep->stApsFsmInfo.stMainFsm.ulFsmId = ulMainFsmId;
    pstPsCallExcep->stApsFsmInfo.stMainFsm.ulState = ulMainFsmState;
    pstPsCallExcep->stApsFsmInfo.stSubFsm.ulFsmId  = ulSubFsmId;
    pstPsCallExcep->stApsFsmInfo.stSubFsm.ulState  = ulSubFsmState;
    pstPsCallExcep->stApsFsmInfo.ucPdpId           = ucPdpId;
}


VOS_VOID NAS_ERRLOG_CCB_SetCdmaPsCallExcepPsCause(
    VOS_UINT32                          ulPsCause
)
{
    NAS_ERRLOG_CCB_GetCdmaPsCallExpCtxAddr()->ulPsCause = ulPsCause;
}


VOS_VOID NAS_ERRLOG_CCB_InCreasePsDiscRecdCnt(VOS_VOID)
{
    NAS_ERRLOG_CCB_GetCdmaPsDiscCtxAddr()->ulCdmaPsDiscRecdCnt += 1;
}


VOS_UINT32 NAS_ERRLOG_CCB_GetCdmaPsDiscRecdCnt(VOS_VOID)
{
    return NAS_ERRLOG_CCB_GetCdmaPsDiscCtxAddr()->ulCdmaPsDiscRecdCnt;
}


VOS_UINT32 NAS_ERRLOG_CCB_CalcPsDiscRecdIndex(VOS_VOID)
{
    VOS_UINT32                          ulRecordIndex;
    VOS_UINT32                          ulRecordCnt;

    ulRecordCnt = NAS_ERRLOG_CCB_GetCdmaPsDiscRecdCnt();

    /* 防护，不会出现该场景 */
    if (0 == ulRecordCnt)
    {
        ulRecordIndex = 0;
    }
    else
    {
        ulRecordIndex = ulRecordCnt % NAS_ERR_LOG_MAX_CDMA_PS_DISC_RECORD_NUM;
        if (0 == ulRecordIndex)
        {
            ulRecordIndex = NAS_ERR_LOG_MAX_CDMA_PS_DISC_RECORD_NUM - 1;
        }
        else
        {
            ulRecordIndex -= 1;
        }
    }

    return ulRecordIndex;
}


VOS_VOID NAS_ERRLOG_CCB_SetCdmaPsDiscRecdIndex(
    VOS_UINT32                          ulRecdIndex,
    VOS_UINT32                          ulCdmaPsDiscRecdCnt
)
{
    NAS_ERRLOG_CCB_GetCdmaPsDiscCtxAddr()->astErrLogCdmaPsDiscRecd[ulRecdIndex].ulIndex
                                            = ulCdmaPsDiscRecdCnt - 1;
}


VOS_UINT32 NAS_ERRLOG_CCB_GetCdmaPsDiscRecdRedialCnt(VOS_VOID)
{
    VOS_UINT32                          ulRecdIndex;

    ulRecdIndex   = NAS_ERRLOG_CCB_CalcPsDiscRecdIndex();
    return NAS_ERRLOG_CCB_GetCdmaPsDiscCtxAddr()->astErrLogCdmaPsDiscRecd[ulRecdIndex].ulRedialCnt;
}


VOS_UINT32 NAS_ERRLOG_CCB_CalcCdmaPsDiscRecdRedialIndex(VOS_VOID)
{
    VOS_UINT32                          ulIndex;
    VOS_UINT32                          ulRedialCnt;

    ulRedialCnt = NAS_ERRLOG_CCB_GetCdmaPsDiscRecdRedialCnt();

    /* 防护，不会出现该场景 */
    if (0 == ulRedialCnt)
    {
        ulIndex = 0;
    }
    else
    {
        ulIndex = ulRedialCnt % NAS_ERR_LOG_MAX_CDMA_PS_CALL_EXCEP_REDIAL_NUM;
        if (0 == ulIndex)
        {
            ulIndex = NAS_ERR_LOG_MAX_CDMA_PS_CALL_EXCEP_REDIAL_NUM - 1;
        }
        else
        {
            ulIndex -= 1;
        }
    }

    return ulIndex;
}



VOS_VOID NAS_ERRLOG_CCB_InCreaseCdmaPsDiscRecdRedialCnt(VOS_VOID)
{
    VOS_UINT32                          ulRecdIndex;

    ulRecdIndex   = NAS_ERRLOG_CCB_CalcPsDiscRecdIndex();
    NAS_ERRLOG_CCB_GetCdmaPsDiscCtxAddr()->astErrLogCdmaPsDiscRecd[ulRecdIndex].ulRedialCnt += 1;
}


VOS_VOID NAS_ERRLOG_CCB_SetCdmaPsDiscRecdRedialRecordIndex(
    VOS_UINT32                          ulRecdIndex,
    VOS_UINT32                          ulRedialIndex,
    VOS_UINT32                          ulRedialCnt
)
{
    NAS_ERRLOG_CCB_GetCdmaPsDiscCtxAddr()->astErrLogCdmaPsDiscRecd[ulRecdIndex].astRedialInfo[ulRedialIndex].ulIndex
                                        = ulRedialCnt - 1;
}


VOS_VOID NAS_ERRLOG_CCB_SetCdmaPsDiscRecdRedialStartTime(
    VOS_UINT32                          ulRecdIndex,
    VOS_UINT32                          ulRedialIndex,
    VOS_UINT32                          ulStartTime
)
{
    NAS_ERRLOG_CCB_GetCdmaPsDiscCtxAddr()->astErrLogCdmaPsDiscRecd[ulRecdIndex].astRedialInfo[ulRedialIndex].ulStartTimer = ulStartTime;
}


VOS_VOID NAS_ERRLOG_CCB_SetCdmaPsDiscRecdRedialEndTime(
    VOS_UINT32                          ulRecdIndex,
    VOS_UINT32                          ulRedialIndex,
    VOS_UINT32                          ulEndTime
)
{
    NAS_ERRLOG_CCB_GetCdmaPsDiscCtxAddr()->astErrLogCdmaPsDiscRecd[ulRecdIndex].astRedialInfo[ulRedialIndex].ulEndTimer = ulEndTime;
}


VOS_VOID NAS_ERRLOG_CCB_SetCdmaPsDiscRecdRedialRatMode(
    VOS_UINT32                                              ulRecdIndex,
    VOS_UINT32                                              ulRedialIndex,
    NAS_ERR_LOG_PS_CALL_RAT_TYPE_ENUM_UINT32                enRatMode
)
{
    NAS_ERRLOG_CCB_GetCdmaPsDiscCtxAddr()->astErrLogCdmaPsDiscRecd[ulRecdIndex].astRedialInfo[ulRedialIndex].enRatMode = enRatMode;
}


VOS_VOID NAS_ERRLOG_CCB_SetCdmaPsDiscRecdRedialRslt(
    VOS_UINT32                          ulRecdIndex,
    VOS_UINT32                          ulRedialIndex,
    VOS_UINT32                          ulTafPsCause
)
{
    NAS_ERRLOG_CCB_GetCdmaPsDiscCtxAddr()->astErrLogCdmaPsDiscRecd[ulRecdIndex].astRedialInfo[ulRedialIndex].ulRedialRslt = ulTafPsCause;
}


VOS_VOID NAS_ERRLOG_CCB_SetCdmaPsDiscRecdApsFsmInfo(
    VOS_UINT32                              ulMainFsmId,
    VOS_UINT32                              ulMainFsmState,
    VOS_UINT32                              ulSubFsmId,
    VOS_UINT32                              ulSubFsmState,
    VOS_UINT8                               ucPdpId
)
{
    VOS_UINT32                                               ulRecdIndex;
    NAS_ERRLOG_CCB_CDMA_PS_DISC_RECORD_STRU                 *pstCdmaPsDiscRecord;

    ulRecdIndex   = NAS_ERRLOG_CCB_CalcPsDiscRecdIndex();

    pstCdmaPsDiscRecord = &(NAS_ERRLOG_CCB_GetCdmaPsDiscCtxAddr()->astErrLogCdmaPsDiscRecd[ulRecdIndex]);

    pstCdmaPsDiscRecord->stApsFsmInfo.stMainFsm.ulFsmId = ulMainFsmId;
    pstCdmaPsDiscRecord->stApsFsmInfo.stMainFsm.ulState = ulMainFsmState;
    pstCdmaPsDiscRecord->stApsFsmInfo.stSubFsm.ulFsmId  = ulSubFsmId;
    pstCdmaPsDiscRecord->stApsFsmInfo.stSubFsm.ulState  = ulSubFsmState;
    pstCdmaPsDiscRecord->stApsFsmInfo.ucPdpId           = ucPdpId;
}


VOS_VOID NAS_ERRLOG_CCB_SetCdmaPsDiscRecdPsCause(
    VOS_UINT32                          ulRecdIndex,
    TAF_PS_CAUSE_ENUM_UINT32            enPsCause
)
{
    NAS_ERRLOG_CCB_GetCdmaPsDiscCtxAddr()->astErrLogCdmaPsDiscRecd[ulRecdIndex].ulPsCause = enPsCause;
}


VOS_VOID NAS_ERRLOG_CCB_InitCdmaPsDiscExcepRecd(
    VOS_UINT32                          ulRecdIndex
)
{
    NAS_MEM_SET_S(&(NAS_ERRLOG_CCB_GetCdmaPsDiscCtxAddr()->astErrLogCdmaPsDiscRecd[ulRecdIndex]),
                  sizeof(NAS_ERRLOG_CCB_CDMA_PS_DISC_RECORD_STRU),
                  0x00,
                  sizeof(NAS_ERRLOG_CCB_CDMA_PS_DISC_RECORD_STRU));
}


VOS_VOID NAS_ERRLOG_CCB_SaveCdmaPsCallFailStartRecord(
    NAS_ERR_LOG_PS_CALL_RAT_TYPE_ENUM_UINT32                enApsRatType
)
{
    VOS_UINT32                          ulPsCallExpRedialIndex;
    /* 发起PPP建链，统计开始时间和接入制式 */

    /* PS呼叫失败信息统计 */
    /* 重拨统计开始前，先对重拨次数加1 */
    NAS_ERRLOG_CCB_InCreasePsCallExcepRedialCnt();
    ulPsCallExpRedialIndex = NAS_ERRLOG_CCB_CalcPsCallExcepRedialRecordIndex();

    NAS_ERRLOG_CCB_SetCdmaPsCallExcepRedialRecordIndex(ulPsCallExpRedialIndex,
                                    NAS_ERRLOG_CCB_GetCdmaPsCallExcepRedialCnt());

    NAS_ERRLOG_CCB_SetCdmaPsCallExcepRedialStartTime(ulPsCallExpRedialIndex, VOS_GetSlice());
    /* 设置发起的制式  */
    NAS_ERRLOG_CCB_SetCdmaPsCallExcepRedialRatMode(ulPsCallExpRedialIndex, enApsRatType);
}


VOS_VOID NAS_ERRLOG_CCB_SaveCdmaPsDiscExcepRedialStartRecord(
    NAS_ERR_LOG_PS_CALL_RAT_TYPE_ENUM_UINT32                enApsRatType
)
{
    VOS_UINT32                          ulRecdIndex;
    VOS_UINT32                          ulRedialIndex;
    VOS_UINT32                          ulRedialCnt;

    ulRedialCnt                         = 0;
    /* 发起PPP建链，统计开始时间和接入制式 */
    /* PS异常断链过程信息统计 */

    /* 每条异常信息中重拨次数加1 */
    NAS_ERRLOG_CCB_InCreaseCdmaPsDiscRecdRedialCnt();

    ulRecdIndex   = NAS_ERRLOG_CCB_CalcPsDiscRecdIndex();
    ulRedialIndex = NAS_ERRLOG_CCB_CalcCdmaPsDiscRecdRedialIndex();

    ulRedialCnt   = NAS_ERRLOG_CCB_GetCdmaPsDiscRecdRedialCnt();

    if (0 == ulRedialCnt)
    {
        NAS_WARNING_LOG(WUEPS_PID_TAF, "NAS_ERRLOG_CCB_SaveCdmaPsDiscExcepRedialStartRecord: ulRedialCnt is 0!");
    }

    NAS_ERRLOG_CCB_SetCdmaPsDiscRecdRedialRecordIndex(ulRecdIndex,
                                    ulRedialIndex,
                                    ulRedialCnt);

    NAS_ERRLOG_CCB_SetCdmaPsDiscRecdRedialStartTime(ulRecdIndex, ulRedialIndex, VOS_GetSlice());
    NAS_ERRLOG_CCB_SetCdmaPsDiscRecdRedialRatMode(ulRecdIndex, ulRedialIndex, enApsRatType);
}


VOS_VOID NAS_ERRLOG_CCB_CdmaPsDiscExcepStartRecord(
    VOS_UINT32                          ulCause,
    NAS_ERRLOG_PDP_ENTITY_FSM_CTX_STRU *pstPdpEntityFsmCtx
)
{
    VOS_UINT32                          ulRecdIndex;

    /* PS异常断链，开始统计，先初始化 */
    NAS_ERRLOG_CCB_InitCdmaPsDiscInfoCtx();

    /*  统计前，先对记录条数加1 */
    NAS_ERRLOG_CCB_InCreasePsDiscRecdCnt();

    ulRecdIndex = NAS_ERRLOG_CCB_CalcPsDiscRecdIndex();
    NAS_ERRLOG_CCB_SetCdmaPsDiscRecdIndex(ulRecdIndex, NAS_ERRLOG_CCB_GetCdmaPsDiscRecdCnt());

    /* 异常断链信息 */
    NAS_ERRLOG_CCB_SetCdmaPsDiscRecdPsCause(ulRecdIndex, ulCause);

    NAS_ERRLOG_CCB_SetCdmaPsDiscRecdApsFsmInfo(pstPdpEntityFsmCtx->ulMainFsmId,
                                               pstPdpEntityFsmCtx->ulMainState,
                                               pstPdpEntityFsmCtx->ulSubFsmId,
                                               pstPdpEntityFsmCtx->ulSubState,
                                               pstPdpEntityFsmCtx->ucPdpId);
}


VOS_VOID NAS_ERRLOG_CCB_SaveCdmaPsCreateLinkEndRecord(
    VOS_UINT32                          ulTafPsCause
)
{
    VOS_UINT32                          ulPsCallExpRedialIndex;
    VOS_UINT32                          ulPsDiscRecdIndex;
    VOS_UINT32                          ulPsDiscRedialIndex;

    /* PS呼叫失败中重拨信息 */
    /* PPP链接建立重拨结束统计 */
    if (0 != NAS_ERRLOG_CCB_GetCdmaPsCallExcepRedialCnt())
    {
        ulPsCallExpRedialIndex = NAS_ERRLOG_CCB_CalcPsCallExcepRedialRecordIndex();

        NAS_ERRLOG_CCB_SetCdmaPsCallExcepRedialEndTime(ulPsCallExpRedialIndex, VOS_GetSlice());
        /* 重拨结果统计 */
        NAS_ERRLOG_CCB_SetCdmaPsCallExcepRedialRslt(ulPsCallExpRedialIndex, ulTafPsCause);
    }
    /*  PS异常断链过程中，失败信息 */
    if ((0 != NAS_ERRLOG_CCB_GetCdmaPsDiscRecdCnt())
      &&(0 != NAS_ERRLOG_CCB_GetCdmaPsDiscRecdRedialCnt()))
    {
        ulPsDiscRecdIndex   = NAS_ERRLOG_CCB_CalcPsDiscRecdIndex();
        ulPsDiscRedialIndex = NAS_ERRLOG_CCB_CalcCdmaPsDiscRecdRedialIndex();

        NAS_ERRLOG_CCB_SetCdmaPsDiscRecdRedialEndTime(ulPsDiscRecdIndex, ulPsDiscRedialIndex, VOS_GetSlice());
        NAS_ERRLOG_CCB_SetCdmaPsDiscRecdRedialRslt(ulPsDiscRecdIndex, ulPsDiscRedialIndex, ulTafPsCause);
    }
}


VOS_UINT8 NAS_ERRLOG_CCB_GetMmaMainActiveReportFlag(VOS_VOID)
{
    return (NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbMainActiveReportControl.stNasErrLogCcbMmaActiveReportControl.ucErrLogActiveReportAlarmFlag);
}


VOS_VOID NAS_ERRLOG_CCB_SetMmaMainActiveReportFlag(
    VOS_UINT8                           ucErrLogActiveReportAlarmFlag
)
{
    NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbMainActiveReportControl.stNasErrLogCcbMmaActiveReportControl.ucErrLogActiveReportAlarmFlag = ucErrLogActiveReportAlarmFlag;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetMmaMainActiveReportLevel(VOS_VOID)
{
    return (NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbMainActiveReportControl.stNasErrLogCcbMmaActiveReportControl.ucErrLogActiveReportAlarmLevel);
}


VOS_VOID NAS_ERRLOG_CCB_SetMmaMainActiveReportLevel(
    VOS_UINT8                           ucErrLogActiveReportAlarmLevel
)
{
    NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbMainActiveReportControl.stNasErrLogCcbMmaActiveReportControl.ucErrLogActiveReportAlarmLevel = ucErrLogActiveReportAlarmLevel;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetXccMainActiveReportFlag(VOS_VOID)
{
    return (NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbMainActiveReportControl.stNasErrLogCcbXccActiveReportControl.ucErrLogActiveReportAlarmFlag);
}


VOS_VOID NAS_ERRLOG_CCB_SetXccMainActiveReportFlag(
    VOS_UINT8                           ucErrLogActiveReportAlarmFlag
)
{
    NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbMainActiveReportControl.stNasErrLogCcbXccActiveReportControl.ucErrLogActiveReportAlarmFlag = ucErrLogActiveReportAlarmFlag;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetXccMainActiveReportLevel(VOS_VOID)
{
    return (NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbMainActiveReportControl.stNasErrLogCcbXccActiveReportControl.ucErrLogActiveReportAlarmLevel);
}


VOS_VOID NAS_ERRLOG_CCB_SetXccMainActiveReportLevel(
    VOS_UINT8                           ucErrLogActiveReportAlarmLevel
)
{
    NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbMainActiveReportControl.stNasErrLogCcbXccActiveReportControl.ucErrLogActiveReportAlarmLevel = ucErrLogActiveReportAlarmLevel;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetHsmMainActiveReportFlag(VOS_VOID)
{
    return (NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbMainActiveReportControl.stNasErrLogCcbHsmActiveReportControl.ucErrLogActiveReportAlarmFlag);
}


VOS_VOID NAS_ERRLOG_CCB_SetHsmMainActiveReportFlag(
    VOS_UINT8                           ucErrLogActiveReportAlarmFlag
)
{
    NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbMainActiveReportControl.stNasErrLogCcbHsmActiveReportControl.ucErrLogActiveReportAlarmFlag = ucErrLogActiveReportAlarmFlag;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetHsmMainActiveReportLevel(VOS_VOID)
{
    return (NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbMainActiveReportControl.stNasErrLogCcbHsmActiveReportControl.ucErrLogActiveReportAlarmLevel);
}


VOS_VOID NAS_ERRLOG_CCB_SetHsmMainActiveReportLevel(
    VOS_UINT8                           ucErrLogActiveReportAlarmLevel
)
{
    NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbMainActiveReportControl.stNasErrLogCcbHsmActiveReportControl.ucErrLogActiveReportAlarmLevel = ucErrLogActiveReportAlarmLevel;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetXregMainActiveReportFlag(VOS_VOID)
{
    return (NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbMainActiveReportControl.stNasErrLogCcbXregActiveReportControl.ucErrLogActiveReportAlarmFlag);
}


VOS_VOID NAS_ERRLOG_CCB_SetXregMainActiveReportFlag(
    VOS_UINT8                           ucErrLogActiveReportAlarmFlag
)
{
    NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbMainActiveReportControl.stNasErrLogCcbXregActiveReportControl.ucErrLogActiveReportAlarmFlag = ucErrLogActiveReportAlarmFlag;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetXregMainActiveReportLevel(VOS_VOID)
{
    return (NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbMainActiveReportControl.stNasErrLogCcbXregActiveReportControl.ucErrLogActiveReportAlarmLevel);
}


VOS_VOID NAS_ERRLOG_CCB_SetXregMainActiveReportLevel(
    VOS_UINT8                           ucErrLogActiveReportAlarmLevel
)
{
    NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbMainActiveReportControl.stNasErrLogCcbXregActiveReportControl.ucErrLogActiveReportAlarmLevel = ucErrLogActiveReportAlarmLevel;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetXsmsMainActiveReportFlag(VOS_VOID)
{
    return (NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbMainActiveReportControl.stNasErrLogCcbXsmsActiveReportControl.ucErrLogActiveReportAlarmFlag);
}


VOS_VOID NAS_ERRLOG_CCB_SetXsmsMainActiveReportFlag(
    VOS_UINT8                           ucErrLogActiveReportAlarmFlag
)
{
    NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbMainActiveReportControl.stNasErrLogCcbXsmsActiveReportControl.ucErrLogActiveReportAlarmFlag = ucErrLogActiveReportAlarmFlag;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetXsmsMainActiveReportLevel(VOS_VOID)
{
    return (NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbMainActiveReportControl.stNasErrLogCcbXsmsActiveReportControl.ucErrLogActiveReportAlarmLevel);
}


VOS_VOID NAS_ERRLOG_CCB_SetXsmsMainActiveReportLevel(
    VOS_UINT8                           ucErrLogActiveReportAlarmLevel
)
{
    NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbMainActiveReportControl.stNasErrLogCcbXsmsActiveReportControl.ucErrLogActiveReportAlarmLevel = ucErrLogActiveReportAlarmLevel;
}



VOS_UINT8 NAS_ERRLOG_CCB_GetCdma1xOosActiveReportFlag(VOS_VOID)
{
    return (NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbCdmaActiveReportControl.uc1xOosReportEnable);
}


VOS_VOID NAS_ERRLOG_CCB_SetCdma1xOosActiveReportFlag(
    VOS_UINT8                           uc1xOosReportEnable
)
{
    NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbCdmaActiveReportControl.uc1xOosReportEnable = uc1xOosReportEnable;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetCdma1xMtServiceExceptionActiveReportFlag(VOS_VOID)
{
    return (NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbCdmaActiveReportControl.uc1xMtServiceExceptionReportEnable);
}


VOS_VOID NAS_ERRLOG_CCB_SetCdma1xMtServiceExceptionActiveReportFlag(
    VOS_UINT8                           uc1xMtServiceExceptionReportEnable
)
{
    NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbCdmaActiveReportControl.uc1xMtServiceExceptionReportEnable = uc1xMtServiceExceptionReportEnable;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetCdmaHrpdUatiFailActiveReportFlag(VOS_VOID)
{
    return (NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbCdmaActiveReportControl.ucHrpdUatiFailReportEnable);
}


VOS_VOID NAS_ERRLOG_CCB_SetCdmaHrpdUatiFailActiveReportFlag(
    VOS_UINT8                           ucHrpdUatiFailReportEnable
)
{
    NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbCdmaActiveReportControl.ucHrpdUatiFailReportEnable = ucHrpdUatiFailReportEnable;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetCdmaHrpdSessionFailActiveReportFlag(VOS_VOID)
{
    return (NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbCdmaActiveReportControl.ucHrpdSessionFailReportEnable);
}


VOS_VOID NAS_ERRLOG_CCB_SetCdmaHrpdSessionFailActiveReportFlag(
    VOS_UINT8                           ucHrpdSessionFailReportEnable
)
{
    NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbCdmaActiveReportControl.ucHrpdSessionFailReportEnable = ucHrpdSessionFailReportEnable;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetCdmaHrpdSessionExcepDeactActiveReportFlag(VOS_VOID)
{
    return (NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbCdmaActiveReportControl.ucHrpdSessionExcepDeactReportEnable);
}


VOS_VOID NAS_ERRLOG_CCB_SetCdmaHrpdSessionExcepDeactActiveReportFlag(
    VOS_UINT8                           ucHrpdSessionExcepDeactReportEnable
)
{
    NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbCdmaActiveReportControl.ucHrpdSessionExcepDeactReportEnable = ucHrpdSessionExcepDeactReportEnable;
}

VOS_UINT8 NAS_ERRLOG_CCB_GetCdmaHrpdOrLteOosReportFlag(VOS_VOID)
{
    return (NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbCdmaActiveReportControl.ucHrpdOrLteOosReportEnable);
}


VOS_VOID NAS_ERRLOG_CCB_SetCdmaHrpdOrLteOosReportFlag(
    VOS_UINT8                           ucHrpdOrLteOosReportEnable
)
{
    NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbCdmaActiveReportControl.ucHrpdOrLteOosReportEnable = ucHrpdOrLteOosReportEnable;
}

VOS_UINT8 NAS_ERRLOG_CCB_GetCdmaXregResltReportFlag(VOS_VOID)
{
    return (NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbCdmaActiveReportControl.ucXregResltReportEnable);
}


VOS_VOID NAS_ERRLOG_CCB_SetCdmaXregResltReportFlag(
    VOS_UINT8                           ucXregResltReportEnable
)
{
    NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbCdmaActiveReportControl.ucXregResltReportEnable = ucXregResltReportEnable;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetCdmaXsmsReportFlag(VOS_VOID)
{
    return (NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbCdmaActiveReportControl.ucXsmsReportEnable);
}


VOS_VOID NAS_ERRLOG_CCB_SetCdmaXsmsReportFlag(
    VOS_UINT8                           ucXsmsReportEnable
)
{
    NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbCdmaActiveReportControl.ucXsmsReportEnable = ucXsmsReportEnable;
}


NAS_ERRLOG_CCB_1X_CS_CALL_EXCEPTION_STRU *NAS_ERRLOG_CCB_Get1xCsCallExceptionAddr(VOS_VOID)
{
    return &(NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcb1xCsOrigCallException);
}


VOS_VOID NAS_ERRLOG_CCB_Init1xCsCallExceptionCtx(VOS_VOID)
{
   NAS_ERRLOG_CCB_1X_CS_CALL_EXCEPTION_STRU               *pstCsCallException = VOS_NULL_PTR;

   pstCsCallException = NAS_ERRLOG_CCB_Get1xCsCallExceptionAddr();

   NAS_MEM_SET_S(pstCsCallException, sizeof(NAS_ERRLOG_CCB_1X_CS_CALL_EXCEPTION_STRU),
                 0x00, sizeof(NAS_ERRLOG_CCB_1X_CS_CALL_EXCEPTION_STRU));

   return;
}


NAS_ERRLOG_CCB_1X_CS_CALL_EXCEPTION_STRU *NAS_ERRLOG_CCB_Get1xConversationExceptionAddr(VOS_VOID)
{
    return &(NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcb1xConversationException);
}


VOS_VOID NAS_ERRLOG_CCB_Init1xConversationExceptionCtx(VOS_VOID)
{
   NAS_ERRLOG_CCB_1X_CS_CALL_EXCEPTION_STRU               *pstConversationException = VOS_NULL_PTR;

   pstConversationException = NAS_ERRLOG_CCB_Get1xConversationExceptionAddr();

   NAS_MEM_SET_S(pstConversationException, sizeof(NAS_ERRLOG_CCB_1X_CS_CALL_EXCEPTION_STRU),
                 0x00, sizeof(NAS_ERRLOG_CCB_1X_CS_CALL_EXCEPTION_STRU));

   return;
}


NAS_ERRLOG_CCB_1X_MT_SERVICE_EXCEPTION_STRU *NAS_ERRLOG_CCB_Get1xMtServiceExceptionAddr(VOS_VOID)
{
    return &(NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcb1xMtServiceException);
}


VOS_VOID NAS_ERRLOG_CCB_Init1xMtServiceExceptionCtx(VOS_VOID)
{
    NAS_ERRLOG_CCB_1X_MT_SERVICE_EXCEPTION_STRU            *pst1xMtServiceException = VOS_NULL_PTR;

    pst1xMtServiceException = NAS_ERRLOG_CCB_Get1xMtServiceExceptionAddr();

    NAS_MEM_SET_S(pst1xMtServiceException, sizeof(NAS_ERRLOG_CCB_1X_MT_SERVICE_EXCEPTION_STRU),
                  0x00, sizeof(NAS_ERRLOG_CCB_1X_MT_SERVICE_EXCEPTION_STRU));

    return;
}


VOS_VOID NAS_ERRLOG_CCB_Init1xMtServiceExceptionRecord(
    NAS_ERR_LOG_1X_SERVICE_TYPE_ENUM_UINT32                 enServiceType
)
{
    VOS_UINT8                                               ucIndex;
    NAS_ERRLOG_CCB_1X_MT_SERVICE_EXCEPTION_STRU            *pst1xMtServiceException = VOS_NULL_PTR;

    pst1xMtServiceException = NAS_ERRLOG_CCB_Get1xMtServiceExceptionAddr();

    if (VOS_TRUE == NAS_ERRLOG_CCB_Get1xMtServiceExceptionRecordIndex(enServiceType, &ucIndex))
    {
        if (pst1xMtServiceException->ucRecordIndex - 1 == ucIndex)
        {
            PS_MEM_SET(&(pst1xMtServiceException->ast1xMtExceptionRecord[ucIndex]),
                       0x00,
                       sizeof(NAS_ERRLOG_CCB_1X_MT_EXCEPTION_RECORD_STRU));
        }
        else
        {
            PS_MEM_MOVE(&(pst1xMtServiceException->ast1xMtExceptionRecord[ucIndex]),
                                &(pst1xMtServiceException->ast1xMtExceptionRecord[ucIndex + 1]),
                                sizeof(NAS_ERRLOG_CCB_1X_MT_EXCEPTION_RECORD_STRU)  * (pst1xMtServiceException->ucRecordIndex - ucIndex - 1));

            PS_MEM_SET(&(pst1xMtServiceException->ast1xMtExceptionRecord[pst1xMtServiceException->ucRecordIndex - 1]),
                       0x00,
                       sizeof(NAS_ERRLOG_CCB_1X_MT_EXCEPTION_RECORD_STRU));
        }

        pst1xMtServiceException->ucRecordIndex--;
    }
}


VOS_VOID NAS_ERRLOG_CCB_SetCsCallStartTimer(
    VOS_UINT32                          ulStartTimer
)
{
    NAS_ERRLOG_CCB_1X_CS_CALL_EXCEPTION_STRU               *pstCsCallException = VOS_NULL_PTR;
    VOS_UINT32                                              ulIndex;

    pstCsCallException = NAS_ERRLOG_CCB_Get1xCsCallExceptionAddr();

    /* 获取当前记录重拨信息的结构体数组的索引 */
    ulIndex = NAS_ERRLOG_CCB_GetCurCsCallRedialRecordInfoIndex(pstCsCallException->ulRedialCnt);

    pstCsCallException->astRedialInfo[ulIndex].ulStartTimer = ulStartTimer;
    pstCsCallException->astRedialInfo[ulIndex].ulIndex      = pstCsCallException->ulRedialCnt;

    return;
}


VOS_VOID NAS_ERRLOG_CCB_SetCsCallEndTimer(
    VOS_UINT32                          ulEndTimer
)
{
    NAS_ERRLOG_CCB_1X_CS_CALL_EXCEPTION_STRU               *pstCsCallException = VOS_NULL_PTR;
    VOS_UINT32                                              ulIndex;

    pstCsCallException = NAS_ERRLOG_CCB_Get1xCsCallExceptionAddr();

    /* 获取当前记录重拨信息的结构体数组的索引 */
    ulIndex = NAS_ERRLOG_CCB_GetCurCsCallRedialRecordInfoIndex(pstCsCallException->ulRedialCnt);

    pstCsCallException->astRedialInfo[ulIndex].ulEndTimer = ulEndTimer;
    pstCsCallException->astRedialInfo[ulIndex].ulIndex    = pstCsCallException->ulRedialCnt;

    return;
}


VOS_UINT32 NAS_ERRLOG_CCB_GetCurCsCallRedialRecordInfoIndex(
    VOS_UINT32                          ulRedialCnt
)
{
    VOS_UINT32                          ulCnt;

    ulCnt = ulRedialCnt;

    return (ulCnt % NAS_ERRLOG_CCB_1X_CS_CALL_MAX_REDIAL_NUM);
}


VOS_VOID NAS_ERRLOG_CCB_SaveCsCallRsltCause(
    VOS_UINT32                          ulErrLogCallCause
)
{
    NAS_ERRLOG_CCB_1X_CS_CALL_EXCEPTION_STRU               *pstCsCallException = VOS_NULL_PTR;
    VOS_UINT32                                              ulIndex;

    pstCsCallException = NAS_ERRLOG_CCB_Get1xCsCallExceptionAddr();

    /* 获取当前记录重拨信息的结构体数组的索引 */
    ulIndex = NAS_ERRLOG_CCB_GetCurCsCallRedialRecordInfoIndex(pstCsCallException->ulRedialCnt);

    pstCsCallException->astRedialInfo[ulIndex].ulRedialRslt = ulErrLogCallCause;
    pstCsCallException->astRedialInfo[ulIndex].ulIndex      = pstCsCallException->ulRedialCnt;

    pstCsCallException->ulCcCause = ulErrLogCallCause;

    return;
}


VOS_VOID NAS_ERRLOG_CCB_SetConversationStartTimer(
    VOS_UINT32                          ulStartTimer
)
{
    NAS_ERRLOG_CCB_1X_CS_CALL_EXCEPTION_STRU               *pstConversationException = VOS_NULL_PTR;

    pstConversationException = NAS_ERRLOG_CCB_Get1xConversationExceptionAddr();

    pstConversationException->astRedialInfo[0].ulStartTimer = ulStartTimer;

    return;
}


VOS_VOID NAS_ERRLOG_CCB_SetConversationEndTimer(
    VOS_UINT32                          ulEndTimer
)
{
    NAS_ERRLOG_CCB_1X_CS_CALL_EXCEPTION_STRU               *pstConversationException = VOS_NULL_PTR;

    pstConversationException = NAS_ERRLOG_CCB_Get1xConversationExceptionAddr();

    pstConversationException->astRedialInfo[0].ulEndTimer = ulEndTimer;

    return;
}



VOS_VOID NAS_ERRLOG_CCB_SaveConversationRsltCause(
    NAS_ERR_LOG_1X_MT_SERVICE_EXCEPTION_CAUSE_ENUM_UINT32  enErrLogCallCause
)
{
    NAS_ERRLOG_CCB_1X_CS_CALL_EXCEPTION_STRU               *pstCsCallException = VOS_NULL_PTR;

    pstCsCallException = NAS_ERRLOG_CCB_Get1xConversationExceptionAddr();

    pstCsCallException->astRedialInfo[0].ulRedialRslt = enErrLogCallCause;

    pstCsCallException->ulCcCause = enErrLogCallCause;

    return;
}


VOS_VOID NAS_ERRLOG_CCB_IncreaseCsCallRedialCnt(VOS_VOID)
{
    NAS_ERRLOG_CCB_1X_CS_CALL_EXCEPTION_STRU               *pstCsCallException = VOS_NULL_PTR;

    pstCsCallException = NAS_ERRLOG_CCB_Get1xCsCallExceptionAddr();

    pstCsCallException->ulRedialCnt++;

    return;
}


VOS_UINT8 NAS_ERRLOG_CCB_IsCsCallRedialCntUpdata(VOS_VOID)
{
    NAS_ERRLOG_CCB_1X_CS_CALL_EXCEPTION_STRU               *pstCsCallException = VOS_NULL_PTR;
    NAS_ERRLOG_CCB_1X_CS_CALL_REDIAL_INFO_STRU              stRedialInfo;

    NAS_MEM_SET_S(&stRedialInfo, sizeof(NAS_ERRLOG_CCB_1X_CS_CALL_REDIAL_INFO_STRU),
                  0x00, sizeof(NAS_ERRLOG_CCB_1X_CS_CALL_REDIAL_INFO_STRU));

    pstCsCallException = NAS_ERRLOG_CCB_Get1xCsCallExceptionAddr();

    if (0 == PS_MEM_CMP(&stRedialInfo, pstCsCallException->astRedialInfo, sizeof(stRedialInfo)))
    {
        return VOS_FALSE;
    }
    else
    {
        return VOS_TRUE;
    }

}


VOS_VOID NAS_ERRLOG_CCB_SaveReceivedPagingTimer(
    NAS_ERR_LOG_1X_SERVICE_TYPE_ENUM_UINT32                 enServiceType,
    VOS_UINT32                                              ulTimer
)
{
    VOS_UINT8                                               ucIndex;
    NAS_ERRLOG_CCB_1X_MT_SERVICE_EXCEPTION_STRU            *pst1xMtServiceException = VOS_NULL_PTR;

    pst1xMtServiceException = NAS_ERRLOG_CCB_Get1xMtServiceExceptionAddr();

    if (VOS_TRUE == NAS_ERRLOG_CCB_Get1xMtServiceExceptionRecordIndex(enServiceType, &ucIndex))
    {
        pst1xMtServiceException->ast1xMtExceptionRecord[ucIndex].ulPagingTimer = ulTimer;
    }
}


VOS_VOID NAS_ERRLOG_CCB_SavePagingServiceType(
    NAS_ERR_LOG_1X_SERVICE_TYPE_ENUM_UINT32                 enServiceType
)
{
    VOS_UINT8                                               ucIndex;
    NAS_ERRLOG_CCB_1X_MT_SERVICE_EXCEPTION_STRU            *pst1xMtServiceException = VOS_NULL_PTR;

    ucIndex = 0;

    if ((enServiceType > NAS_ERR_LOG_1X_MT_SERVICE_TYPE)
     && (enServiceType < NAS_ERR_LOG_1X_MT_SERVICE_TYPE_BUTT))
    {
        /*已保存相同业务类型被叫,则不保存*/
        if (VOS_FALSE == NAS_ERRLOG_CCB_Get1xMtServiceExceptionRecordIndex(enServiceType, &ucIndex))
        {
            pst1xMtServiceException = NAS_ERRLOG_CCB_Get1xMtServiceExceptionAddr();

            pst1xMtServiceException->ast1xMtExceptionRecord[pst1xMtServiceException->ucRecordIndex].enPagingType = enServiceType;

            pst1xMtServiceException->ucRecordIndex++;
        }
    }

}


VOS_UINT8 NAS_ERRLOG_CCB_Get1xMtServiceExceptionRecordIndex(
    NAS_ERR_LOG_1X_SERVICE_TYPE_ENUM_UINT32                 enServiceType,
    VOS_UINT8                                              *pucIndex
)
{
    VOS_UINT8                                               i;
    NAS_ERRLOG_CCB_1X_MT_SERVICE_EXCEPTION_STRU            *pst1xMtServiceException = VOS_NULL_PTR;

    *pucIndex               = 0;
    pst1xMtServiceException = NAS_ERRLOG_CCB_Get1xMtServiceExceptionAddr();

    if (pst1xMtServiceException->ucRecordIndex > NAS_ERRLOG_CCB_1X_MT_MAX_EXCEPTION_RECORD_NUM)
    {
        pst1xMtServiceException->ucRecordIndex = NAS_ERRLOG_CCB_1X_MT_MAX_EXCEPTION_RECORD_NUM;
    }

    for (i = 0; i < pst1xMtServiceException->ucRecordIndex; i++)
    {
        if (enServiceType == pst1xMtServiceException->ast1xMtExceptionRecord[i].enPagingType)
        {
            *pucIndex = i;
            return VOS_TRUE;
        }
    }

    if (pst1xMtServiceException->ucRecordIndex < NAS_ERRLOG_CCB_1X_MT_MAX_EXCEPTION_RECORD_NUM)
    {
        *pucIndex  = pst1xMtServiceException->ucRecordIndex;
    }

    return VOS_FALSE;
}



VOS_VOID NAS_ERRLOG_CCB_SaveMtServiceExceptionCause(
    NAS_ERR_LOG_1X_SERVICE_TYPE_ENUM_UINT32                 enServiceType,
    NAS_ERR_LOG_1X_MT_SERVICE_EXCEPTION_CAUSE_ENUM_UINT32   enErrLogCallCause
)
{
    VOS_UINT8                                               ucIndex;
    NAS_ERRLOG_CCB_1X_MT_SERVICE_EXCEPTION_STRU            *pst1xMtServiceException = VOS_NULL_PTR;

    pst1xMtServiceException = NAS_ERRLOG_CCB_Get1xMtServiceExceptionAddr();

    if (VOS_TRUE == NAS_ERRLOG_CCB_Get1xMtServiceExceptionRecordIndex(enServiceType, &ucIndex))
    {
        pst1xMtServiceException->ast1xMtExceptionRecord[ucIndex].enCcCause = enErrLogCallCause;
    }
}


VOS_VOID NAS_ERRLOG_CCB_SaveMtServiceExceptionXccState(
    NAS_ERR_LOG_1X_SERVICE_TYPE_ENUM_UINT32                 enServiceType,
    VOS_UINT32                                              ulXccState
)
{
    VOS_UINT8                                               ucIndex;
    NAS_ERRLOG_CCB_1X_MT_SERVICE_EXCEPTION_STRU            *pst1xMtServiceException = VOS_NULL_PTR;

    pst1xMtServiceException = NAS_ERRLOG_CCB_Get1xMtServiceExceptionAddr();

    if (VOS_TRUE == NAS_ERRLOG_CCB_Get1xMtServiceExceptionRecordIndex(enServiceType, &ucIndex))
    {
        pst1xMtServiceException->ast1xMtExceptionRecord[ucIndex].ulXccState = ulXccState;
    }
}


VOS_VOID NAS_ERRLOG_CCB_Init1xOosSrchInfoRecord(
    VOS_UINT32                          ulIndex
)
{
    NAS_MEM_SET_S(&(NAS_ERRLOG_CCB_Get1xOosSearchCtxAddr()->ast1xOosSearchRecord[ulIndex]),
                  sizeof(NAS_ERRLOG_CCB_1X_OOS_SEARCH_RECORD_STRU),
                  0x00,
                  sizeof(NAS_ERRLOG_CCB_1X_OOS_SEARCH_RECORD_STRU));
}


VOS_VOID NAS_ERRLOG_CCB_InCrease1xOosSrchInfoRecordCnt(VOS_VOID)
{
    NAS_ERRLOG_CCB_Get1xOosSearchCtxAddr()->ulSearchRecordCnt += 1;
}


VOS_UINT32 NAS_ERRLOG_CCB_Get1xOosSrchInfoRecordCnt(VOS_VOID)
{
    return NAS_ERRLOG_CCB_Get1xOosSearchCtxAddr()->ulSearchRecordCnt;
}


VOS_UINT32 NAS_ERRLOG_CCB_Calc1xOosSrchInfoRecordIndex(VOS_VOID)
{
    VOS_UINT32                          ulIndex;
    VOS_UINT32                          ulRecordCnt;

    ulRecordCnt = NAS_ERRLOG_CCB_Get1xOosSrchInfoRecordCnt();

    /* 防护，不会出现该场景 */
    if (0 == ulRecordCnt)
    {
        ulIndex = 0;
    }
    else
    {
        ulIndex = ulRecordCnt % NAS_ERRLOG_CCB_1X_OOS_MAX_SEARCH_RECORD_NUM;
        if (0 == ulIndex)
        {
            ulIndex = NAS_ERRLOG_CCB_1X_OOS_MAX_SEARCH_RECORD_NUM - 1;
        }
        else
        {
            ulIndex -= 1;
        }
    }

    return ulIndex;
}


VOS_VOID NAS_ERRLOG_CCB_Set1xOosSrchInfoStartTime(
    VOS_UINT32                          ulIndex,
    VOS_UINT32                          ulStartTime
)
{
    NAS_ERRLOG_CCB_Get1xOosSearchCtxAddr()->ast1xOosSearchRecord[ulIndex].ulStartTimer = ulStartTime;
}


VOS_VOID NAS_ERRLOG_CCB_Set1xOosSrchInfoEndTime(
    VOS_UINT32                          ulIndex,
    VOS_UINT32                          ulEndTime
)
{
    NAS_ERRLOG_CCB_Get1xOosSearchCtxAddr()->ast1xOosSearchRecord[ulIndex].ulEndTimer = ulEndTime;
}


VOS_VOID NAS_ERRLOG_CCB_Set1xOosSrchInfoRecordIndex(
    VOS_UINT32                          ulIndex,
    VOS_UINT32                          ulRecordIndex
)
{
    NAS_ERRLOG_CCB_Get1xOosSearchCtxAddr()->ast1xOosSearchRecord[ulIndex].ulIndex = ulRecordIndex  - 1;
}


VOS_VOID NAS_ERRLOG_CCB_Set1xOosSrchInfoSearchRslt(
    VOS_UINT32                                              ulIndex,
    NAS_ERR_LOG_1X_SEARCH_RSLT_ENUM_UINT32                  enSearchRslt
)
{
    NAS_ERRLOG_CCB_Get1xOosSearchCtxAddr()->ast1xOosSearchRecord[ulIndex].enSearchRslt = enSearchRslt;
}


VOS_VOID NAS_ERRLOG_CCB_SndAcpuOmFaultErrLogInd(
    VOS_VOID                           *pData,
    VOS_UINT32                          ulDataLen,
    VOS_UINT8                           ucFaultId,
    NAS_ERR_LOG_ALM_ID_ENUM_U16         usAlmId,
    VOS_UINT32                          ulSenderPid
)
{
    OM_FAULT_ERR_LOG_CDMA_IND_STRU                         *pstFaultRptInd = VOS_NULL_PTR;
    VOS_UINT32                                              ulMsgLen;
    VOS_UINT32                                              ulContentAddr;
    NAS_ERR_LOG_UNSOLI_REPORT_TYPE_ENUM_U16                 enReportType;


    /* 申请消息结构内存 */
    /*lint -e961*/
    ulMsgLen     = sizeof(OM_FAULT_ERR_LOG_CDMA_IND_STRU) - VOS_MSG_HEAD_LENGTH - 4 + ulDataLen;
    /*lint +e961*/
    pstFaultRptInd = (OM_FAULT_ERR_LOG_CDMA_IND_STRU*)PS_ALLOC_MSG(ulSenderPid, ulMsgLen);

    if (VOS_NULL_PTR == pstFaultRptInd)
    {
        NAS_WARNING_LOG(WUEPS_PID_TAF, "NAS_ERRLOG_CCB_SndAcpuOmFaultErrLogInd:ERROR: Alloc msg fail.");
        return;
    }

    /* 构造消息结构体 */
    pstFaultRptInd->ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstFaultRptInd->ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstFaultRptInd->ulReceiverPid     = MSP_PID_DIAG_APP_AGENT;
    pstFaultRptInd->ulSenderPid       = ulSenderPid;
    pstFaultRptInd->ulMsgName         = ID_OM_FAULT_ERR_LOG_IND;

    enReportType = NAS_GetErrLogUnsoliReportType(usAlmId);

    if (NAS_ERR_LOG_FAULT_REPORT == enReportType)
    {
        pstFaultRptInd->usMsgType     = OM_ERR_LOG_MSG_FAULT_REPORT;
    }
    else if (NAS_ERR_LOG_ALARM_REPORT == enReportType)
    {
        pstFaultRptInd->usMsgType     = OM_ERR_LOG_MSG_ALARM_REPORT;
    }
    else
    {
        pstFaultRptInd->usMsgType     = OM_ERR_LOG_MSG_TYPE_BUTT;
    }

    pstFaultRptInd->ucFaultId         = ucFaultId;
    pstFaultRptInd->ucAlarmId         = (VOS_UINT8)usAlmId;
    pstFaultRptInd->ulMsgSN           = 0;
    pstFaultRptInd->ulRptlen          = ulDataLen;

    ulContentAddr                     = (VOS_UINT32)pstFaultRptInd->aucContent;
    NAS_MEM_CPY_S((VOS_UINT8 *)ulContentAddr, ulDataLen, pData, ulDataLen);

    /* 发送消息到ACPU OM模块 */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstFaultRptInd))
    {
        NAS_WARNING_LOG(WUEPS_PID_TAF, "NAS_ERRLOG_CCB_SndAcpuOmFaultErrLogInd:ERROR: SEND MSG FIAL.");
    }

    return;
}


VOS_VOID NAS_ERRLOG_CCB_SndAcpuOmFaultErrLogCnf(
    VOS_VOID                           *pData,
    VOS_UINT32                          ulDataLen,
    VOS_UINT8                           ucFaultId,
    VOS_UINT8                           ucAlarmId,
    VOS_UINT32                          ulSenderPid
)
{
    OM_ERR_LOG_RPT_CNF_STRU            *pstFaultRptCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgLen;
    VOS_UINT32                          ulContentAddr;

    /* 申请消息结构内存 */
    /*lint -e961*/
    ulMsgLen     = sizeof(OM_ERR_LOG_RPT_CNF_STRU) - VOS_MSG_HEAD_LENGTH - 4 + ulDataLen;
    /*lint +e961*/
    pstFaultRptCnf = (OM_ERR_LOG_RPT_CNF_STRU*)PS_ALLOC_MSG(ulSenderPid, ulMsgLen);

    if (VOS_NULL_PTR == pstFaultRptCnf)
    {
        NAS_WARNING_LOG(WUEPS_PID_TAF, "NAS_ERRLOG_CCB_SndAcpuOmFaultErrLogCnf:ERROR: Alloc msg fail.");
        return;
    }

    /* 构造消息结构体 */
    pstFaultRptCnf->ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstFaultRptCnf->ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstFaultRptCnf->ulReceiverPid     = MSP_PID_DIAG_APP_AGENT;
    pstFaultRptCnf->ulSenderPid       = ulSenderPid;
    pstFaultRptCnf->ulMsgName         = ID_OM_ERR_LOG_REPORT_CNF;
    pstFaultRptCnf->usMsgType         = OM_ERR_LOG_MSG_ERR_REPORT;
    pstFaultRptCnf->ucFaultId         = ucFaultId;
    pstFaultRptCnf->ucAlarmId         = ucAlarmId;
    pstFaultRptCnf->ulMsgSN           = 0;
    pstFaultRptCnf->ulRptlen          = ulDataLen;

    ulContentAddr                     = (VOS_UINT32)pstFaultRptCnf->aucContent;
    NAS_MEM_CPY_S((VOS_UINT8 *)ulContentAddr, ulDataLen, pData, ulDataLen);

    /* 发送消息到ACPU OM模块 */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstFaultRptCnf))
    {
        NAS_WARNING_LOG(WUEPS_PID_TAF, "NAS_ERRLOG_CCB_SndAcpuOmFaultErrLogCnf:ERROR: SEND MSG FIAL.");
    }

    return;
}


VOS_VOID NAS_ERRLOG_CCB_SetHrpdOosSrchInfoStartTime(
    VOS_UINT32                          ulIndex,
    VOS_UINT32                          ulStartTime
)
{
    NAS_ERRLOG_CCB_GetHrpdOosSearchCtxAddr()->astHrpdOosSearchRecord[ulIndex].ulStartTimer = ulStartTime;

    return;
}


VOS_VOID NAS_ERRLOG_CCB_SetHrpdOosSrchInfoEndTime(
    VOS_UINT32                          ulIndex,
    VOS_UINT32                          ulEndTime
)
{
    NAS_ERRLOG_CCB_GetHrpdOosSearchCtxAddr()->astHrpdOosSearchRecord[ulIndex].ulEndTimer = ulEndTime;

    return;
}


VOS_VOID NAS_ERRLOG_CCB_SetHrpdOosSrchInfoRecordIndex(
    VOS_UINT32                          ulIndex,
    VOS_UINT32                          ulRecordIndex
)
{
    NAS_ERRLOG_CCB_GetHrpdOosSearchCtxAddr()->astHrpdOosSearchRecord[ulIndex].ulIndex = ulRecordIndex  - 1;

    return;
}


VOS_VOID NAS_ERRLOG_CCB_SetHrpdOosSrchInfoSearchRslt(
    VOS_UINT32                                                ulIndex,
    NAS_ERR_LOG_HRPD_SEARCH_RSLT_ENUM_UINT32                  enSearchRslt
)
{
    NAS_ERRLOG_CCB_GetHrpdOosSearchCtxAddr()->astHrpdOosSearchRecord[ulIndex].enSearchRslt = enSearchRslt;

    return;
}

NAS_ERRLOG_CCB_HRPD_OR_LTE_OOS_RECOVERY_STRU* NAS_ERRLOG_CCB_GetHrpdOrLteOosRecoveryCtxAddr(VOS_VOID)
{
    return &(NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbHrpdOrLteOosRecovery);
}

NAS_ERRLOG_CCB_HRPD_OOS_SEARCH_STRU* NAS_ERRLOG_CCB_GetHrpdOosSearchCtxAddr(VOS_VOID)
{
    return &(NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbHrpdOosSearchRecord);
}



NAS_ERRLOG_CCB_HRPD_OOS_REPORT_STRU* NAS_ERRLOG_CCB_GetHrpdOosReportCtxAddr(VOS_VOID)
{
    return &(NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbHrpdOosRpt);
}


VOS_VOID NAS_ERRLOG_CCB_SetHrpdOosCause(
    NAS_ERR_LOG_HRPD_OOS_CAUSE_ENUM_UINT32                  enOosCause
)
{
    NAS_ERRLOG_CCB_GetHrpdOosReportCtxAddr()->enOosCause = enOosCause;

    return;
}


NAS_ERR_LOG_HRPD_OOS_CAUSE_ENUM_UINT32 NAS_ERRLOG_CCB_GetHrpdOosCause(VOS_VOID)
{
    return NAS_ERRLOG_CCB_GetHrpdOosReportCtxAddr()->enOosCause;
}


VOS_VOID NAS_ERRLOG_CCB_InitHrpdOosReportCtx(VOS_VOID)
{
   NAS_ERRLOG_CCB_HRPD_OOS_REPORT_STRU               *pstNasErrLogCcbHrpdOosRpt = VOS_NULL_PTR;

   pstNasErrLogCcbHrpdOosRpt = NAS_ERRLOG_CCB_GetHrpdOosReportCtxAddr();

   NAS_MEM_SET_S(pstNasErrLogCcbHrpdOosRpt, sizeof(NAS_ERRLOG_CCB_HRPD_OOS_REPORT_STRU),
                 0x00, sizeof(NAS_ERRLOG_CCB_HRPD_OOS_REPORT_STRU));

   return;
}

VOS_VOID NAS_ERRLOG_CCB_SetAlreadyReportChrHrpdOrLteOosFlag(
    VOS_UINT8                           ucAlreadyReportChrOosFlag
)
{
    NAS_ERRLOG_CCB_GetHrpdOrLteOosRecoveryCtxAddr()->ucAlreadyReportChrOosFlag = ucAlreadyReportChrOosFlag;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetAlreadyReportChrHrpdOrLteOosFlag(VOS_VOID)
{
    return NAS_ERRLOG_CCB_GetHrpdOrLteOosRecoveryCtxAddr()->ucAlreadyReportChrOosFlag;
}


VOS_VOID NAS_ERRLOG_CCB_SetAlreadyReportHrpdOrLteOosToApFlag(
    VOS_UINT8                           ucAlreadyReportOosToApFlag
)
{
    NAS_ERRLOG_CCB_GetHrpdOrLteOosRecoveryCtxAddr()->ucAlreadyReportOosToApFlag = ucAlreadyReportOosToApFlag;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetAlreadyReportHrpdOrLteOosToApFlag(VOS_VOID)
{
    return NAS_ERRLOG_CCB_GetHrpdOrLteOosRecoveryCtxAddr()->ucAlreadyReportOosToApFlag;
}


VOS_VOID NAS_ERRLOG_CCB_InitHrpdOrLteOosRecoveryCtx(VOS_VOID)
{

    NAS_ERRLOG_CCB_HRPD_OR_LTE_OOS_RECOVERY_STRU           *pstHrpdOrLteOosRecovery;

    pstHrpdOrLteOosRecovery = NAS_ERRLOG_CCB_GetHrpdOrLteOosRecoveryCtxAddr();

    NAS_MEM_SET_S(pstHrpdOrLteOosRecovery, sizeof(NAS_ERRLOG_CCB_HRPD_OR_LTE_OOS_RECOVERY_STRU), 0x00, sizeof(NAS_ERRLOG_CCB_HRPD_OR_LTE_OOS_RECOVERY_STRU));
}


VOS_VOID NAS_ERRLOG_CCB_InitHrpdOosSearchCtx(VOS_VOID)
{
    NAS_ERRLOG_CCB_HRPD_OOS_SEARCH_STRU                      *pstHrpdOosSearch;

    pstHrpdOosSearch = NAS_ERRLOG_CCB_GetHrpdOosSearchCtxAddr();

    NAS_MEM_SET_S(pstHrpdOosSearch, sizeof(NAS_ERRLOG_CCB_HRPD_OOS_SEARCH_STRU), 0x00, sizeof(NAS_ERRLOG_CCB_HRPD_OOS_SEARCH_STRU));
}



VOS_VOID NAS_ERRLOG_CCB_InitHrpdOosSrchInfoRecord(
    VOS_UINT32                          ulIndex
)
{
    NAS_MEM_SET_S(&(NAS_ERRLOG_CCB_GetHrpdOosSearchCtxAddr()->astHrpdOosSearchRecord[ulIndex]),
                  sizeof(NAS_ERRLOG_CCB_HRPD_OOS_SEARCH_RECORD_STRU),
                  0x00,
                  sizeof(NAS_ERRLOG_CCB_HRPD_OOS_SEARCH_RECORD_STRU));
}


VOS_VOID NAS_ERRLOG_CCB_IncreaseHrpdOosSrchInfoRecordCnt(VOS_VOID)
{
    NAS_ERRLOG_CCB_GetHrpdOosSearchCtxAddr()->ulSearchRecordCnt += 1;
}


VOS_UINT32 NAS_ERRLOG_CCB_GetHrpdOosSrchInfoRecordCnt(VOS_VOID)
{
    return NAS_ERRLOG_CCB_GetHrpdOosSearchCtxAddr()->ulSearchRecordCnt;
}


VOS_UINT32 NAS_ERRLOG_CCB_CalcHrpdOosSrchInfoRecordIndex(VOS_VOID)
{
    VOS_UINT32                          ulIndex;
    VOS_UINT32                          ulRecordCnt;

    ulRecordCnt = NAS_ERRLOG_CCB_GetHrpdOosSrchInfoRecordCnt();

    /* 防护，不会出现该场景 */
    if (0 == ulRecordCnt)
    {
        ulIndex = 0;
    }
    else
    {
        ulIndex = ulRecordCnt % NAS_ERRLOG_CCB_HRPD_OOS_MAX_SEARCH_RECORD_NUM;
        if (0 == ulIndex)
        {
            ulIndex = NAS_ERRLOG_CCB_HRPD_OOS_MAX_SEARCH_RECORD_NUM - 1;
        }
        else
        {
            ulIndex -= 1;
        }
    }

    return ulIndex;
}


NAS_ERRLOG_CCB_XREG_FAIL_INFO_STRU* NAS_ERRLOG_CCB_GetXregFailCtxAddr(VOS_VOID)
{
    return &(NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbXregFailInfo);
}


VOS_VOID NAS_ERRLOG_CCB_InitXregFailCtx(VOS_VOID)
{
    NAS_ERRLOG_CCB_SetXregFailType(NAS_ERR_LOG_XREG_ACCESS_BUTT);

    NAS_ERRLOG_CCB_SetXregFailReason(NAS_ERR_LOG_XREG_XREG_BUTT);
}


VOS_VOID NAS_ERRLOG_CCB_SetXregFailType(
    NAS_ERR_LOG_XREG_REGISTRATION_TYPE_ENUM_UINT8           enRegType
)
{
    NAS_ERRLOG_CCB_GetXregFailCtxAddr()->enRegType = enRegType;

    return;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetXregFailType(VOS_VOID)
{
    return NAS_ERRLOG_CCB_GetXregFailCtxAddr()->enRegType;
}


VOS_VOID NAS_ERRLOG_CCB_SetXregFailReason(
    NAS_ERR_LOG_XREG_FAIL_REASON_ENUM_UINT8           enRegFailReason
)
{
    NAS_ERRLOG_CCB_GetXregFailCtxAddr()->enRegFailReason = enRegFailReason;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetXreRegFailReason(VOS_VOID)
{
    return NAS_ERRLOG_CCB_GetXregFailCtxAddr()->enRegFailReason;
}


NAS_ERRLOG_CCB_XSMS_SND_FAIL_INFO_STRU* NAS_ERRLOG_CCB_GetXsmsSndCtxAddr(VOS_VOID)
{
    return &(NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbXsmsSndFailInfo);
}


VOS_VOID NAS_ERRLOG_CCB_SetXsmsXccCnf(VOS_UINT32 enLastXccCnfRslt)
{
    NAS_ERRLOG_CCB_GetXsmsSndCtxAddr()->enLastXccCnfRslt = enLastXccCnfRslt;

    return;
}


VOS_VOID NAS_ERRLOG_CCB_SetXsmsCasCnf(VOS_UINT8 enLastCasCnfRslt)
{
    NAS_ERRLOG_CCB_GetXsmsSndCtxAddr()->enLastCasCnfRslt = enLastCasCnfRslt;

    return;
}


VOS_VOID NAS_ERRLOG_CCB_SetXsmsTlAckCauseCode(VOS_UINT8 enLastTlAckCauseCode)
{
    NAS_ERRLOG_CCB_GetXsmsSndCtxAddr()->enLastTlAckCauseCode = enLastTlAckCauseCode;

    return;
}


VOS_VOID NAS_ERRLOG_CCB_SetXsmsNoTlAckCauseCode(NAS_ERR_LOG_XSMS_NO_TL_ACK_REASON_ENUM_UINT8 enNoTlAckReason)
{
    NAS_ERRLOG_CCB_GetXsmsSndCtxAddr()->enNoTlAckReason = enNoTlAckReason;

    return;
}


VOS_VOID NAS_ERRLOG_CCB_SetXsmsSndFailReason(NAS_ERR_LOG_XSMS_FAIL_REASON_ENUM_UINT8 enSndFailReason)
{
    NAS_ERRLOG_CCB_GetXsmsSndCtxAddr()->enSndFailReason = enSndFailReason;

    return;
}


VOS_VOID NAS_ERRLOG_CCB_SetXsmsSndFSMState(VOS_UINT32 enFailState)
{
    NAS_ERRLOG_CCB_GetXsmsSndCtxAddr()->enFailState = enFailState;

    return;
}


VOS_VOID NAS_ERRLOG_CCB_SetXsmsReSendTime(VOS_UINT32 ulReSendTime)
{
    NAS_ERRLOG_CCB_GetXsmsSndCtxAddr()->ulReSendTime = ulReSendTime;

    return;
}


NAS_ERRLOG_CCB_XSMS_RCV_FAIL_INFO_STRU* NAS_ERRLOG_CCB_GetXsmsRcvCtxAddr(VOS_VOID)
{
    return &(NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbXsmsRcvFailInfo);
}


NAS_ERR_LOG_XSMS_FAIL_REASON_ENUM_UINT8 NAS_ERRLOG_CCB_GetXsmsRcvFailReason(VOS_VOID)
{
    return NAS_ERRLOG_CCB_GetXsmsRcvCtxAddr()->enFailReason;
}


VOS_VOID NAS_ERRLOG_CCB_SetXsmsRcvFailReason(NAS_ERR_LOG_XSMS_FAIL_REASON_ENUM_UINT8 enFailReason)
{
    NAS_ERRLOG_CCB_GetXsmsRcvCtxAddr()->enFailReason = enFailReason;

    return;
}


VOS_VOID NAS_ERRLOG_CCB_SetXsmsRcvCasCnfRslt(VOS_UINT8 enLastCasCnfRslt)
{
    NAS_ERRLOG_CCB_GetXsmsRcvCtxAddr()->enLastCasCnfRslt = enLastCasCnfRslt;

    return;
}


VOS_UINT8 NAS_ERRLOG_CCB_GetXsmsRcvCasCnfRslt(VOS_VOID)
{
    return NAS_ERRLOG_CCB_GetXsmsRcvCtxAddr()->enLastCasCnfRslt ;
}


VOS_VOID NAS_ERRLOG_CCB_SetXsmsReRcvTime(VOS_UINT8 ucReRcvTime)
{
    NAS_ERRLOG_CCB_GetXsmsRcvCtxAddr()->ucReRcvTime = ucReRcvTime;

    return;
}


VOS_UINT32 NAS_ERRLOG_CCB_GetXsmsReRcvTime(VOS_VOID)
{
    return NAS_ERRLOG_CCB_GetXsmsRcvCtxAddr()->ucReRcvTime;
}


NAS_ERRLOG_CCB_CL_LTE_COMMON_INFO_STRU *NAS_ERRLOG_CCB_GetClLteCommonInfoCtxAddr(VOS_VOID)
{
    return  &(NAS_ERRLOG_CCB_GetCdmaCcbCtxAddr()->stNasErrLogCcbLteCommonInfo);
}


VOS_VOID NAS_ERRLOG_CCB_InitClLteCommonInfoCtx(VOS_VOID)
{
    NAS_ERRLOG_CCB_CL_LTE_COMMON_INFO_STRU                 *pstLteCommonInfo;

    pstLteCommonInfo = NAS_ERRLOG_CCB_GetClLteCommonInfoCtxAddr();

    NAS_MEM_SET_S(pstLteCommonInfo, sizeof(NAS_ERRLOG_CCB_CL_LTE_COMMON_INFO_STRU), 0x00, sizeof(NAS_ERRLOG_CCB_CL_LTE_COMMON_INFO_STRU));

    pstLteCommonInfo->stPlmn.ulMcc = NAS_ERRLOG_CCB_INVALID_MCC;
    pstLteCommonInfo->stPlmn.ulMnc = NAS_ERRLOG_CCB_INVALID_MNC;
}

#endif /*(FEATURE_ON == FEATURE_UE_MODE_CDMA)*/

#endif  /*(FEATURE_PTM == FEATURE_ON)*/

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
