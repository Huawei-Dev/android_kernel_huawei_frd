/******************************************************************************

   Copyright(C)2013,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : ImsaServiceManagement.c
  Description     : 该C文件实现服务管理模块的初始化，内部消息的处理和发送
  History           :
     1.sunbing 49683      2013-06-19  Draft Enact

******************************************************************************/

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include "ImsaServiceManagement.h"
#include "ImsaEntity.h"
#include "ImsaPublic.h"
#include "ImsaInterface.h"
#include "ImsaProcMsccMsg.h"
#include "ImsaCallManagement.h"
#include "ImsaProcAtMsg.h"
#include "ImsaProcSpmMsg.h"
#include "ImsaProcSmsMsg.h"
#include "ImsaProcUssdMsg.h"
#include "ImsaImsAdaption.h"

#include "ImsaLmmInterface.h"
#include "ImsaDomainManagement.h"


/*lint -e767*/
#define    THIS_FILE_ID          PS_FILE_ID_IMSASERVICEMANAGEMENT_C
#define    THIS_NAS_FILE_ID      NAS_FILE_ID_IMSASERVICEMANAGEMENT_C
/*lint +e767*/


/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if (FEATURE_ON == FEATURE_IMS)
/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/
IMSA_SUBSCRIPTION_INFO_LIST_STRU    g_stSubcriptionInfoList = {0};              /**< 订阅信息列表 */

extern VOS_VOID IMSA_CONTROL_InitConfigPara( VOS_VOID );
extern VOS_VOID IMSA_CONTROL_InitNetworkInfo( VOS_VOID );
extern VOS_VOID IMSA_SRV_RatFormatTransform
(
    IMSA_CAMPED_RAT_TYPE_ENUM_UINT8    *penImsaRat,
    MSCC_IMSA_RAT_TYPE_ENUM_UINT8        enMmaRat
);
extern VOS_UINT32 IMSA_SRV_IsCurRatSupportEms( VOS_VOID );
extern IMSA_NRM_SRV_CON_SAT_STATUE_ENMUM_UINT32 IMSA_SRV_IsNrmSrvConditonSatisfied( VOS_VOID );
extern VOS_UINT32 IMSA_SRV_IsMakeEmcCallInNrmSrvSatisfied( VOS_VOID );
extern VOS_UINT32 IMSA_SRV_IsEmcSrvConditonSatisfied( VOS_VOID );
/*extern VOS_VOID IMSA_CMCCDeregProc();*/
extern IMSA_NORMAL_REG_STATUS_ENUM_UINT32 IMSA_SRV_GetRegStatusFromSrvStatus
(
    IMSA_SRV_STATUS_ENUM_UINT8          enSrvStatus
);
extern VOS_UINT32 IMSA_SRV_IsSrvStatusEqual
(
    IMSA_SRV_TYPE_ENUM_UINT8            enSrvType,
    IMSA_SRV_STATUS_ENUM_UINT8          enImsaSrvStatus
);
extern VOS_VOID IMSA_SRV_SndConnSetupReq
(
    VOS_UINT32                          ulReEstablishFlag,
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_IMS_DOMAIN_ENUM_UINT8           enConnRatType
);
/*extern VOS_VOID IMSA_SRV_SndConnRelReq
(
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType,
    IMSA_CONN_SIP_PDP_TYPE_ENUM_UINT32      enSipPdpType,
    IMSA_IMS_DOMAIN_NUM_UINT8               enConnRatType
);*/

extern VOS_VOID IMSA_SRV_SndRegReregReq
(
    IMSA_REG_TYPE_ENUM_UINT8            enRegType,
    IMSA_REG_BEARER_TYPE_ENUM_UINT8     enRegBearerType
);

extern VOS_VOID IMSA_SRV_SndRegRegReq
(
    IMSA_REG_TYPE_ENUM_UINT8            enRegType,
    IMSA_REG_ADDR_PARAM_ENUM_UINT32     enAddrType
);
extern VOS_VOID IMSA_SRV_SndNrmRegStatusNotify
(
    const IMSA_SUBSCRIPTION_INFO_STRU  *pstSubsciptionInfo,
    IMSA_NORMAL_REG_STATUS_ENUM_UINT32  enNormalRegStatus
);
extern VOS_VOID IMSA_SRV_NrmRegStatusNotify
(
    IMSA_NORMAL_REG_STATUS_ENUM_UINT32  enNrmRegStatus
);

extern VOS_VOID IMSA_SRV_ProcConnSetupIndSucc
(
    const IMSA_CONN_SETUP_IND_STRU     *pstConnSetupIndMsg
);
extern VOS_VOID IMSA_SRV_ProcConnSetupIndFail
(
    const IMSA_CONN_SETUP_IND_STRU     *pstConnSetupIndMsg
);
extern VOS_VOID IMSA_SRV_ProcRegRegIndSucc
(
    const IMSA_REG_REG_IND_STRU        *pstRegRegIndMsg
);
extern VOS_VOID IMSA_SRV_ProcRegRegIndFail
(
    const IMSA_REG_REG_IND_STRU        *pstRegRegIndMsg
);
extern VOS_VOID IMSA_SRV_ProcEmcConnRelIndMsg
(
    IMSA_CONN_SIP_PDP_TYPE_ENUM_UINT32      enSipPdpType
);
extern VOS_VOID IMSA_SRV_ProcNormalConnSigalBearerRelIndMsg( VOS_VOID );
extern VOS_VOID IMSA_SRV_ProcNormalConnSignalBearerModifyIndMsg
(
    const IMSA_CONN_MODIFY_IND_STRU    *pstConnModifyIndMsg
);
extern VOS_VOID IMSA_SRV_ProcTransToNotSupportEms( VOS_VOID );
extern VOS_VOID IMSA_SRV_UpdateRat
(
    MSCC_IMSA_RAT_TYPE_ENUM_UINT8        enRat,
    VOS_UINT32                         *pulIsNotfiyNrmNoSrv,
    VOS_UINT32                         *pulIsNotfiyEmcNoSrv
);
extern VOS_VOID IMSA_SRV_UpdateImsVoPsStatus
(
    MSCC_IMSA_IMS_VOPS_INDICATOR_ENUM_UINT8      enImsVoPsInd
);
extern VOS_VOID IMSA_SRV_UpdateEmsStatus
(
    MSCC_IMSA_EMS_INDICATOR_ENUM_UINT8           enEmsInd
);
extern VOS_UINT32 IMSA_IsImsVoiceContidionSatisfied( VOS_VOID );

extern VOS_VOID IMSA_SndImsMsgServiceSuspendOrResumeSrvInfo
(
    IMSA_IMS_INPUT_SERVICE_REASON_ENUM_UINT32   enInputServeReason
);

extern VOS_VOID IMSA_SRV_RegConningStateProcRegDeregIndMsg
(
    IMSA_REG_TYPE_ENUM_UINT8            enRegType
);

extern VOS_VOID IMSA_SRV_SetImsRegForbidden
(
    IMSA_REG_TYPE_ENUM_UINT8       enRegType,
    IMSA_REG_RESULT_ENUM_UINT32    enResult
);

extern VOS_VOID IMSA_StartTempForbTimer(VOS_VOID);
extern VOS_UINT32 IMSA_IsTempForbTimerRunning(VOS_VOID);


extern VOS_UINT32 IMSA_SRV_IsReestablishEmcPdnConditonSatisfied( VOS_VOID );

/*extern VOS_UINT32 IMSA_SRV_GetImsVoiceCapability(MSCC_IMSA_IMS_VOICE_CAP_ENUM_UINT8 *penImsVoiceCap);
extern VOS_VOID IMSA_SRV_SndDomainSrvReadyInd();*/
extern VOS_VOID IMSA_SRV_SndDomainBearerRelnd(IMSA_IMS_DOMAIN_ENUM_UINT8 enNewPdnConnType);
extern VOS_VOID IMSA_SndOmNoRegCauseInfo
(
    IMSA_SRV_STATUS_ENUM_UINT8                  enImsaSrvStatue,
    IMSA_NRM_SRV_CON_SAT_STATUE_ENMUM_UINT32    enImsaSrvConSatStatue
);
extern VOS_VOID IMSA_SndOmServiceStateChangeInfo
(

    IMSA_SRV_TYPE_ENUM_UINT8            enSrvType,
    IMSA_SRV_STATUS_ENUM_UINT8          enNewImsaSrvStatue,
    IMSA_SRV_STATUS_ENUM_UINT8          enOldImsaSrvStatue
);

extern VOS_UINT32 IMSA_SRV_IsLteRegConditionSatisfied(VOS_VOID);

extern VOS_VOID IMSA_ProcConnHoCnfInHoDereg
(
    IMSA_CONN_HO_CNF_STRU* pstConnHoCnf
);
extern VOS_VOID IMSA_ProcConnHoCnfFailInHoReg
(
    IMSA_CONN_HO_CNF_STRU* pstConnHoCnf
);

extern VOS_VOID IMSA_ProcConnHoCnfSuccInHoReg
(
    IMSA_CONN_HO_CNF_STRU* pstConnHoCnf
);

extern VOS_VOID IMSA_SRV_DomainSwitchInReleasingDereg
(
    IMSA_DOMAIN_SWITCH_TYPE_ENUM_UINT8 enSwitchType,
    VOS_UINT8   ucHoFailReturn2OrgPdnFlag,
    VOS_UINT8   ucOrgPdnRlsFlag
);

extern VOS_VOID IMSA_SRV_DomainSwitchInIdleDereg
(
    IMSA_DOMAIN_SWITCH_TYPE_ENUM_UINT8 enSwitchType,
    VOS_UINT8   ucHoFailReturn2OrgPdnFlag,
    VOS_UINT8   ucOrgPdnRlsFlag,
    IMSA_IMS_DOMAIN_ENUM_UINT8     enConnRatType
);

extern VOS_VOID IMSA_SRV_DomainSwitchInConningDereg
(
    IMSA_DOMAIN_SWITCH_TYPE_ENUM_UINT8 enSwitchType,
    VOS_UINT8   ucHoFailReturn2OrgPdnFlag,
    VOS_UINT8   ucOrgPdnRlsFlag,
    IMSA_IMS_DOMAIN_ENUM_UINT8     enConnRatType
);
extern VOS_VOID IMSA_SRV_DomainSwitchInConningReg
(
    IMSA_DOMAIN_SWITCH_TYPE_ENUM_UINT8 enSwitchType,
    VOS_UINT8   ucHoFailReturn2OrgPdnFlag,
    VOS_UINT8   ucOrgPdnRlsFlag,
    IMSA_IMS_DOMAIN_ENUM_UINT8     enConnRatType
);

extern VOS_VOID IMSA_SRV_DomainSwitchInHoReg
(
    IMSA_DOMAIN_SWITCH_TYPE_ENUM_UINT8 enSwitchType,
    VOS_UINT8   ucHoFailReturn2OrgPdnFlag,
    VOS_UINT8   ucOrgPdnRlsFlag,
    IMSA_IMS_DOMAIN_ENUM_UINT8     enConnRatType
);

extern VOS_UINT32 IMSA_SRV_DomainSwitchCheckParmValid
(
    IMSA_DOMAIN_SWITCH_TYPE_ENUM_UINT8 enSwitchType,
    VOS_UINT8   ucHoFailReturn2OrgPdnFlag,
    VOS_UINT8   ucOrgPdnRlsFlag
);



/*****************************************************************************
  3 Function
*****************************************************************************/
/*lint -e961*/

VOS_VOID IMSA_CONTROL_InitConfigPara( VOS_VOID )
{
    IMSA_CONFIG_PARA_STRU              *pstImsaConfigPara = VOS_NULL_PTR;

    pstImsaConfigPara = IMSA_GetConfigParaAddress();

    pstImsaConfigPara->ucUtranImsSupportFlag        = VOS_FALSE;
    pstImsaConfigPara->ucLteImsSupportFlag          = VOS_FALSE;
    pstImsaConfigPara->ucUtranEmsSupportFlag        = VOS_FALSE;
    pstImsaConfigPara->ucLteEmsSupportFlag          = VOS_FALSE;
    pstImsaConfigPara->ucVoiceCallOnImsSupportFlag  = VOS_TRUE;
    pstImsaConfigPara->ucSmsOnImsSupportFlag        = VOS_TRUE;
    pstImsaConfigPara->ucNoCardEmcCallSupportFlag   = VOS_FALSE;
    pstImsaConfigPara->ucUeRelMediaPdpFLag          = VOS_FALSE;
    pstImsaConfigPara->ucImsLocDeregSigPdpRelFlag   = VOS_TRUE;
    pstImsaConfigPara->enVoiceDomain                = IMSA_VOICE_DOMAIN_IMS_PS_PREFERRED;
    pstImsaConfigPara->stCMCCCustomReq.ucCMCCCustomDeregFlag        = VOS_FALSE;
    pstImsaConfigPara->stCMCCCustomReq.ucCMCCCustomTcallFlag        = VOS_FALSE;
    pstImsaConfigPara->stCMCCCustomReq.ucCMCCCustomTqosFlag         = VOS_FALSE;
    pstImsaConfigPara->ucWifiEmsSupportFlag         = VOS_FALSE;
    pstImsaConfigPara->ucWifiImsSupportFlag         = VOS_FALSE;
}


VOS_VOID IMSA_CONTROL_InitNetworkInfo( VOS_VOID )
{
    IMSA_NETWORK_INFO_STRU             *pstImsaNetworkInfo = VOS_NULL_PTR;

    pstImsaNetworkInfo = IMSA_GetNetInfoAddress();

    pstImsaNetworkInfo->enImsaCampedRatType   = IMSA_CAMPED_RAT_TYPE_NULL;
    pstImsaNetworkInfo->enImsaImsVoPsStatus   = IMSA_IMS_VOPS_STATUS_BUTT;
    pstImsaNetworkInfo->enImsaEmsStatus       = IMSA_EMS_STAUTS_BUTT;
    pstImsaNetworkInfo->enImsaPsServiceStatus = IMSA_PS_SERVICE_STATUS_NO_SERVICE;
    pstImsaNetworkInfo->ucImsaRoamingFlg      = VOS_FALSE;
}


VOS_VOID IMSA_CONTROL_InitWifiNetInfo(VOS_VOID)
{
  IMSA_WIFI_NETWORK_INFO_STRU   *pstImsaWifiNetworkInfo = VOS_NULL_PTR;
  pstImsaWifiNetworkInfo = IMSA_GetWifiNetInfoAddress();
  IMSA_MEM_SET_S(   pstImsaWifiNetworkInfo,
                  sizeof(IMSA_WIFI_NETWORK_INFO_STRU),
                  0x0,
                 sizeof(IMSA_WIFI_NETWORK_INFO_STRU));
}


VOS_VOID IMSA_CONTROL_InitVoWifiPara(VOS_VOID)
{
  IMSA_VOWIFI_PARA_STRU   *pstImsaVowifiPara = VOS_NULL_PTR;
  pstImsaVowifiPara = IMSA_GetVoWifiParaAddress();
  IMSA_MEM_SET_S(   pstImsaVowifiPara,
                  sizeof(IMSA_VOWIFI_PARA_STRU),
                    0x0,
                    sizeof(IMSA_VOWIFI_PARA_STRU));

  pstImsaVowifiPara->ucWifiImsSupportFlag         = VOS_FALSE;
  pstImsaVowifiPara->ucHoSupportInNonActSrvFlag   = VOS_FALSE;
  pstImsaVowifiPara->ucW2LHoInActSrvFlag          = VOS_FALSE;
  pstImsaVowifiPara->ucL2WHoInActSrvFlag          = VOS_FALSE;
  pstImsaVowifiPara->ucVolteCtrlVowifiFlag        = VOS_FALSE;
  pstImsaVowifiPara->ucHoSupportInUnstableSrvFlag = VOS_FALSE;
  pstImsaVowifiPara->ucL2WHoDelayInrSRVCCFlag     = VOS_FALSE;

}





VOS_VOID IMSA_CONTROL_Init( VOS_VOID )
{
    IMSA_CONTROL_MANAGER_STRU          *pstImsaControlManager   = VOS_NULL_PTR;
    IMSA_SUBSCRIPTION_INFO_LIST_STRU   *pstSubcriptionInfoList  = VOS_NULL_PTR;

    pstSubcriptionInfoList  = IMSA_GetSubcriptionInfoListAddr();
    pstImsaControlManager   = IMSA_GetControlManagerAddress();

    IMSA_MEM_SET_S( pstImsaControlManager,
                    sizeof(IMSA_CONTROL_MANAGER_STRU),
                    0x0,
                    sizeof(IMSA_CONTROL_MANAGER_STRU));

    pstImsaControlManager->enImsaStatus                 = IMSA_STATUS_NULL;
    pstImsaControlManager->enImsaIsimStatus             = IMSA_ISIM_STATUS_ABSENT;

    pstImsaControlManager->enImsaCellularStatus         = IMSA_STATUS_NULL;
    pstImsaControlManager->enImsaWifiStatus             = IMSA_STATUS_NULL;
    pstImsaControlManager->enImsaDomainMode             = IMSA_IMS_DOMAIN_MODE_NULL;
    pstImsaControlManager->ucRcvServiceChangeIndFlag    = VOS_FALSE;

    pstImsaControlManager->bitCapImsDomainGSM           = 0;
    pstImsaControlManager->bitCapImsDomainUtran         = 0;
    pstImsaControlManager->bitCapImsDomainLTE           = 0;
    pstImsaControlManager->bitCapImsDomainWifi          = 0;
    pstImsaControlManager->enImsaSrvCurrentDomain       = IMSA_IMS_DOMAIN_MODE_NULL;

   /* 初始化Wifi网络配置参数 */
    IMSA_CONTROL_InitWifiNetInfo();
   /* 初始化VoWIFI配置参数 */
    IMSA_CONTROL_InitVoWifiPara();

    pstImsaControlManager->bitRegSuccProcTypeValid     = IMSA_OP_FALSE;
    pstImsaControlManager->enRegSuccProcType           = IMSA_MSCC_REG_PROC_BUTT;

    /* 初始化保护定时器 */
    pstImsaControlManager->stProtectTimer.phTimer       = VOS_NULL_PTR;
    pstImsaControlManager->stProtectTimer.ucMode        = VOS_RELTIMER_NOLOOP;
    pstImsaControlManager->stProtectTimer.usName        = TI_IMSA_START_OR_STOP;
    pstImsaControlManager->stProtectTimer.ulTimerLen    = TI_IMSA_START_OR_STOP_TIMER_LEN;

    /* 初始化配置信息 */
    IMSA_CONTROL_InitConfigPara();

    /* 初始化网络信息 */
    IMSA_CONTROL_InitNetworkInfo();

    /* 初始化周期性尝试IMS服务定时器 */
    pstImsaControlManager->stPeriodImsSrvTryTimer.phTimer   = VOS_NULL_PTR;
    pstImsaControlManager->stPeriodImsSrvTryTimer.usName    = TI_IMSA_PERIOD_TRY_IMS_SRV;
    pstImsaControlManager->stPeriodImsSrvTryTimer.ucMode    = VOS_RELTIMER_NOLOOP;
    pstImsaControlManager->stPeriodImsSrvTryTimer.ulTimerLen= TI_IMSA_PERIOD_IMS_SRV_TRY_TIMER_LEN;
    pstImsaControlManager->stPeriodImsSrvTryTimer.usPara    = (VOS_UINT16)IMSA_SRV_TYPE_NORMAL;

    pstImsaControlManager->stTempForbiddenTimer.phTimer     = VOS_NULL_PTR;
    pstImsaControlManager->stTempForbiddenTimer.usName      = TI_IMSA_TEMP_FORBIDDEN_TIMER;
    pstImsaControlManager->stTempForbiddenTimer.ucMode      = VOS_RELTIMER_NOLOOP;
    pstImsaControlManager->stTempForbiddenTimer.ulTimerLen  = LMM_Get3402TimerLen();
    pstImsaControlManager->stTempForbiddenTimer.usPara      = (VOS_UINT16)IMSA_SRV_TYPE_NORMAL;

    pstImsaControlManager->stWaitDeregTimer.phTimer         = VOS_NULL_PTR;
    pstImsaControlManager->stWaitDeregTimer.usName          = TI_IMSA_WAIT_DEREG_TIMER;
    pstImsaControlManager->stWaitDeregTimer.ucMode          = VOS_RELTIMER_NOLOOP;
    pstImsaControlManager->stWaitDeregTimer.ulTimerLen      = TI_IMSA_WAIT_DEREG_TIMER_LEN;
    pstImsaControlManager->stWaitDeregTimer.usPara          = (VOS_UINT16)IMSA_SRV_TYPE_NORMAL;

    pstImsaControlManager->stWaitReadfileCnfTimer.phTimer    = VOS_NULL_PTR;
    pstImsaControlManager->stWaitReadfileCnfTimer.usName     = TI_IMSA_WAIT_READFILE_CNF_TIMER;
    pstImsaControlManager->stWaitReadfileCnfTimer.ucMode     = VOS_RELTIMER_NOLOOP;
    pstImsaControlManager->stWaitReadfileCnfTimer.ulTimerLen = TI_IMSA_WAIT_READFILE_CNF_TIMER_LEN;
    pstImsaControlManager->stWaitReadfileCnfTimer.usPara     = (VOS_UINT16)IMSA_SRV_TYPE_NORMAL;

    /* delete PeriodImsEmcSrvTryTimer */

    pstImsaControlManager->ucInternalImsStackStartFlag = VOS_FALSE;
    pstImsaControlManager->enSwitchType                = IMSA_DOMAIN_SWITCH_TYPE_NO_SWITCH;
    pstImsaControlManager->ucHoFailReturn2OrgPdnFlag   = VOS_TRUE;
    pstImsaControlManager->ucOrgPdnRlsFlag             = VOS_TRUE;
    pstImsaControlManager->ucDomainRegSrvStatusFlag    = VOS_FALSE;

    pstImsaControlManager->enNrmSrvStatus   = IMSA_SRV_STATUS_IDLE_DEREG;
    pstImsaControlManager->enEmcSrvStatus   = IMSA_SRV_STATUS_IDLE_DEREG;

    pstImsaControlManager->enDeregCause     = IMSA_SRV_DEREG_CAUSE_BUTT;
    pstImsaControlManager->ucIsImsRegForbidden = IMSA_FALSE;
    /* 初始化上次普通注册状态 */
    pstSubcriptionInfoList->enLastNrmRegStatus  = IMSA_NORMAL_REG_STATUS_LTE_DEREG;

    /* 初始化上次普通AT注册状态 */
    pstSubcriptionInfoList->enLastNrmAtRegStatus  = IMSA_NORMAL_AT_REG_STATUS_DEREG;

    pstImsaControlManager->stGetSerProtectTimer.phTimer         = VOS_NULL_PTR;
    pstImsaControlManager->stGetSerProtectTimer.ucMode          = VOS_RELTIMER_NOLOOP;
    pstImsaControlManager->stGetSerProtectTimer.usName          = TI_IMSA_GET_SERVICE_PROTECT_TIMER;
    pstImsaControlManager->stGetSerProtectTimer.ulTimerLen      = TI_IMSA_GET_SERVICE_PROTECT_TIMER_LEN;

    pstImsaControlManager->stDeregSerProtectTimer.phTimer       = VOS_NULL_PTR;
    pstImsaControlManager->stDeregSerProtectTimer.ucMode        = VOS_RELTIMER_NOLOOP;
    pstImsaControlManager->stDeregSerProtectTimer.usName        = TI_IMSA_DEREG_SERVICE_PROTECT_TIMER;
    pstImsaControlManager->stDeregSerProtectTimer.ulTimerLen    = TI_IMSA_DEREG_SERVICE_PROTECT_TIMER_LEN;

    pstImsaControlManager->stImsaCommonInfo.enISIMState = IMSA_REALISIM_STATE_BUTT;

    pstImsaControlManager->ucNoRegEmcCallFlag = IMSA_FALSE;

    pstImsaControlManager->ucPsServStatusChangeFlag = VOS_FALSE;

    return ;
}


VOS_VOID IMSA_CONTROL_ClearResource( VOS_VOID )
{
    IMSA_CONTROL_MANAGER_STRU          *pstImsaControlManager   = VOS_NULL_PTR;
    IMSA_SUBSCRIPTION_INFO_LIST_STRU   *pstSubcriptionInfoList  = VOS_NULL_PTR;

    pstSubcriptionInfoList  = IMSA_GetSubcriptionInfoListAddr();
    pstImsaControlManager   = IMSA_GetControlManagerAddress();

    /* 如果注册状态发生变更，需要上报上层注册状态 */
    IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_IDLE_DEREG);
    IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_EMC, IMSA_SRV_STATUS_IDLE_DEREG);

    pstImsaControlManager->enImsaDomainMode             = IMSA_IMS_DOMAIN_MODE_NULL;

    pstImsaControlManager->enImsaStatus                 = IMSA_STATUS_NULL;
    pstImsaControlManager->enImsaWifiStatus             = IMSA_STATUS_NULL;
    pstImsaControlManager->enImsaCellularStatus         = IMSA_STATUS_NULL;

    pstImsaControlManager->ucRcvServiceChangeIndFlag    = VOS_FALSE;
    pstImsaControlManager->enImsaSrvCurrentDomain       = IMSA_IMS_DOMAIN_MODE_NULL;

    pstImsaControlManager->enIsImsSrvExist              = IMSA_SRV_CONN_STATUS_ISNOT_EXIST;

    /* 停止保护定时器 */
    IMSA_StopTimer(&pstImsaControlManager->stProtectTimer);
    pstImsaControlManager->stProtectTimer.phTimer       = VOS_NULL_PTR;
    pstImsaControlManager->stProtectTimer.ucMode        = VOS_RELTIMER_NOLOOP;
    pstImsaControlManager->stProtectTimer.usName        = TI_IMSA_START_OR_STOP;
    pstImsaControlManager->stProtectTimer.ulTimerLen    = TI_IMSA_START_OR_STOP_TIMER_LEN;

    /* 初始化网络信息 */
    IMSA_CONTROL_InitNetworkInfo();
    IMSA_CONTROL_InitWifiNetInfo();

    /* 停止周期性尝试IMS服务定时器 */
    IMSA_StopTimer(&pstImsaControlManager->stPeriodImsSrvTryTimer);
    pstImsaControlManager->stPeriodImsSrvTryTimer.phTimer   = VOS_NULL_PTR;
    pstImsaControlManager->stPeriodImsSrvTryTimer.usName    = TI_IMSA_PERIOD_TRY_IMS_SRV;
    pstImsaControlManager->stPeriodImsSrvTryTimer.ucMode    = VOS_RELTIMER_NOLOOP;
    pstImsaControlManager->stPeriodImsSrvTryTimer.ulTimerLen= TI_IMSA_PERIOD_IMS_SRV_TRY_TIMER_LEN;
    pstImsaControlManager->stPeriodImsSrvTryTimer.usPara    = (VOS_UINT16)IMSA_SRV_TYPE_NORMAL;

    IMSA_StopTimer(&pstImsaControlManager->stTempForbiddenTimer);
    pstImsaControlManager->stTempForbiddenTimer.phTimer     = VOS_NULL_PTR;
    pstImsaControlManager->stTempForbiddenTimer.usName      = TI_IMSA_TEMP_FORBIDDEN_TIMER;
    pstImsaControlManager->stTempForbiddenTimer.ucMode      = VOS_RELTIMER_NOLOOP;
    pstImsaControlManager->stTempForbiddenTimer.ulTimerLen  = LMM_Get3402TimerLen();
    pstImsaControlManager->stTempForbiddenTimer.usPara      = (VOS_UINT16)IMSA_SRV_TYPE_NORMAL;

    pstImsaControlManager->bitRegSuccProcTypeValid     = IMSA_OP_FALSE;
    pstImsaControlManager->enRegSuccProcType           = IMSA_MSCC_REG_PROC_BUTT;

    /* 停止等待去注册结果定时器 */
    IMSA_StopTimer(&pstImsaControlManager->stWaitDeregTimer);
    pstImsaControlManager->stWaitDeregTimer.phTimer         = VOS_NULL_PTR;
    pstImsaControlManager->stWaitDeregTimer.usName          = TI_IMSA_WAIT_DEREG_TIMER;
    pstImsaControlManager->stWaitDeregTimer.ucMode          = VOS_RELTIMER_NOLOOP;
    pstImsaControlManager->stWaitDeregTimer.ulTimerLen      = TI_IMSA_WAIT_DEREG_TIMER_LEN;
    pstImsaControlManager->stWaitDeregTimer.usPara          = (VOS_UINT16)IMSA_SRV_TYPE_NORMAL;

    IMSA_StopTimer(&pstImsaControlManager->stWaitReadfileCnfTimer);
    pstImsaControlManager->stWaitReadfileCnfTimer.phTimer    = VOS_NULL_PTR;
    pstImsaControlManager->stWaitReadfileCnfTimer.usName     = TI_IMSA_WAIT_READFILE_CNF_TIMER;
    pstImsaControlManager->stWaitReadfileCnfTimer.ucMode     = VOS_RELTIMER_NOLOOP;
    pstImsaControlManager->stWaitReadfileCnfTimer.ulTimerLen = TI_IMSA_WAIT_READFILE_CNF_TIMER_LEN;
    pstImsaControlManager->stWaitReadfileCnfTimer.usPara     = (VOS_UINT16)IMSA_SRV_TYPE_NORMAL;

    /* delete PeriodImsEmcSrvTryTimer */

    pstImsaControlManager->ucInternalImsStackStartFlag = VOS_FALSE;
    pstImsaControlManager->enSwitchType                = IMSA_DOMAIN_SWITCH_TYPE_NO_SWITCH;
    pstImsaControlManager->ucHoFailReturn2OrgPdnFlag   = VOS_TRUE;
    pstImsaControlManager->ucOrgPdnRlsFlag             = VOS_TRUE;
    pstImsaControlManager->ucDomainRegSrvStatusFlag    = VOS_FALSE;

    pstImsaControlManager->enNrmSrvStatus   = IMSA_SRV_STATUS_IDLE_DEREG;
    pstImsaControlManager->enEmcSrvStatus   = IMSA_SRV_STATUS_IDLE_DEREG;

    pstImsaControlManager->enDeregCause     = IMSA_SRV_DEREG_CAUSE_BUTT;

    pstImsaControlManager->ucIsImsRegForbidden = IMSA_FALSE;

    IMSA_MEM_SET_S( &pstImsaControlManager->stImsaIntraMsgQueue,
                    sizeof(IMSA_INTRA_MSG_QUEUE_STRU),
                    0x0,
                    sizeof(IMSA_INTRA_MSG_QUEUE_STRU));

    /* 初始化上次普通注册状态 */
    pstSubcriptionInfoList->enLastNrmRegStatus  = IMSA_NORMAL_REG_STATUS_LTE_DEREG;

    /* 初始化上次普通AT注册状态 */
    pstSubcriptionInfoList->enLastNrmAtRegStatus  = IMSA_NORMAL_AT_REG_STATUS_DEREG;

    pstImsaControlManager->stImsaCommonInfo.stImsaUeId.bitOpImpi = IMSA_OP_FALSE;

    IMSA_MEM_SET_S( &pstImsaControlManager->stImsaCommonInfo.stImsaUeId.stImpi,
                    sizeof(IMSA_IMPI_STRU),
                    0,
                    IMSA_MAX_IMPI_LEN +1);


    pstImsaControlManager->stImsaCommonInfo.stImsaUeId.bitOpTImpu = IMSA_OP_FALSE;

    IMSA_MEM_SET_S( &pstImsaControlManager->stImsaCommonInfo.stImsaUeId.stTImpu,
                    sizeof(IMSA_TIMPU_STRU),
                    0,
                    IMSA_MAX_TIMPU_LEN+1);

    pstImsaControlManager->stImsaCommonInfo.bitOpHomeNetDomainName = IMSA_OP_FALSE;

    IMSA_MEM_SET_S( &pstImsaControlManager->stImsaCommonInfo.stHomeNetDomainName,
                    sizeof(IMSA_HOME_NET_DOMAIN_NAME_STRU),
                    0,
                    IMSA_MAX_HOME_NET_DOMAIN_NAME_LEN +1);


    pstImsaControlManager->ucIsCardLocked = IMSA_FALSE;

    /* 清除从卡中读取的SIP URI */
    pstImsaControlManager->stImsaSipUri.ucNvReadSipUriFlag = VOS_FALSE;
    pstImsaControlManager->stImsaSipUri.ucReadSipUriFlag   = VOS_FALSE;
    IMSA_MEM_SET_S( pstImsaControlManager->stImsaSipUri.acSipUri,
                    IMSA_IMS_ALPHA_STRING_SZ + 1,
                    0,
                    IMSA_IMS_ALPHA_STRING_SZ + 1);

    pstImsaControlManager->ucPsServStatusChangeFlag = VOS_FALSE;

    return ;
}



VOS_VOID IMSA_SRV_RatFormatTransform
(
    IMSA_CAMPED_RAT_TYPE_ENUM_UINT8    *penImsaRat,
    MSCC_IMSA_RAT_TYPE_ENUM_UINT8        enMmaRat
)
{
    if (MSCC_IMSA_RAT_TYPE_GSM == enMmaRat)
    {
        *penImsaRat = IMSA_CAMPED_RAT_TYPE_GSM;
    }
    else if (MSCC_IMSA_RAT_TYPE_UTRAN == enMmaRat)
    {
        *penImsaRat = IMSA_CAMPED_RAT_TYPE_UTRAN;
    }
    else if (MSCC_IMSA_RAT_TYPE_LTE == enMmaRat)
    {
        *penImsaRat = IMSA_CAMPED_RAT_TYPE_EUTRAN;
    }
    else
    {
        IMSA_WARN_LOG("IMSA_SRV_RatFormatTransform:illegal rat type");
        TLPS_PRINT2LAYER_WARNING(IMSA_SRV_RatFormatTransform_ENUM, LNAS_PARAM_INVALID);
        *penImsaRat = IMSA_CAMPED_RAT_TYPE_NULL;
    }
}


VOS_UINT32 IMSA_SRV_IsCurRatSupportIms( VOS_VOID )
{
    IMSA_NETWORK_INFO_STRU             *pstImsaNwInfo       = VOS_NULL_PTR;
    IMSA_CONFIG_PARA_STRU              *pstImsaConfigPara   = VOS_NULL_PTR;

    pstImsaNwInfo       = IMSA_GetNetInfoAddress();
    pstImsaConfigPara   = IMSA_GetConfigParaAddress();

    #if 0
    if ((IMSA_CAMPED_RAT_TYPE_GSM == pstImsaNwInfo->enImsaCampedRatType)
        && (VOS_TRUE == pstImsaConfigPara->ucGsmImsSupportFlag))
    {
        IMSA_INFO_LOG("IMSA_SRV_IsCurRatSupportIms:GSM support ims!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsCurRatSupportIms_ENUM, 1);
        return IMSA_TRUE;
    }
    #endif

    if( IMSA_IMS_DOMAIN_LTE == IMSA_GetCurrentDomain())
    {
        if ((IMSA_CAMPED_RAT_TYPE_UTRAN == pstImsaNwInfo->enImsaCampedRatType)
            && (VOS_TRUE == pstImsaConfigPara->ucUtranImsSupportFlag)
        /* 中移动在TDD下不支持IMS， 所以不需要判断UTRAN_TDD */
            && (MSCC_IMSA_ACCESS_TYPE_UTRAN_FDD == pstImsaNwInfo->enAccessType))
        {
            IMSA_INFO_LOG("IMSA_SRV_IsCurRatSupportIms:UNTRAN support ims!");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsCurRatSupportIms_ENUM, 2);
            return IMSA_TRUE;
        }

        if ((IMSA_CAMPED_RAT_TYPE_EUTRAN == pstImsaNwInfo->enImsaCampedRatType)
            && (VOS_TRUE == pstImsaConfigPara->ucLteImsSupportFlag)
            && ((MSCC_IMSA_ACCESS_TYPE_EUTRAN_TDD == pstImsaNwInfo->enAccessType)
            || (MSCC_IMSA_ACCESS_TYPE_EUTRAN_FDD == pstImsaNwInfo->enAccessType)))
        {
            IMSA_INFO_LOG("IMSA_SRV_IsCurRatSupportIms:LTE support ims!");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsCurRatSupportIms_ENUM, 3);
            return IMSA_TRUE;
        }
    }

    if( (IMSA_IMS_DOMAIN_WIFI == IMSA_GetCurrentDomain())
        &&(VOS_TRUE == pstImsaConfigPara->ucWifiImsSupportFlag))
    {
        IMSA_INFO_LOG("IMSA_SRV_IsCurRatSupportIms:WIFI support ims!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsCurRatSupportIms_ENUM, 3);
        return IMSA_TRUE;
    }


    return IMSA_FALSE;
}


VOS_UINT32 IMSA_SRV_IsCurRatSupportEms( VOS_VOID )
{
    IMSA_NETWORK_INFO_STRU             *pstImsaNwInfo       = VOS_NULL_PTR;
    IMSA_CONFIG_PARA_STRU              *pstImsaConfigPara   = VOS_NULL_PTR;

    pstImsaNwInfo       = IMSA_GetNetInfoAddress();
    pstImsaConfigPara   = IMSA_GetConfigParaAddress();

    if ((IMSA_CAMPED_RAT_TYPE_UTRAN == pstImsaNwInfo->enImsaCampedRatType)
        && (VOS_TRUE == pstImsaConfigPara->ucUtranEmsSupportFlag)
    /* 中移动在TDD下不支持IMS， 所以不需要判断UTRAN_TDD */
        && (MSCC_IMSA_ACCESS_TYPE_UTRAN_FDD == pstImsaNwInfo->enAccessType))
    {
        IMSA_INFO_LOG("IMSA_SRV_IsCurRatSupportIms:UNTRAN support Ems!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsCurRatSupportEms_ENUM, 2);
        return IMSA_TRUE;
    }

    if ((IMSA_CAMPED_RAT_TYPE_EUTRAN == pstImsaNwInfo->enImsaCampedRatType)
        && (VOS_TRUE == pstImsaConfigPara->ucLteEmsSupportFlag)
        && ((MSCC_IMSA_ACCESS_TYPE_EUTRAN_TDD == pstImsaNwInfo->enAccessType)
        || (MSCC_IMSA_ACCESS_TYPE_EUTRAN_FDD == pstImsaNwInfo->enAccessType)))
    {
        IMSA_INFO_LOG("IMSA_SRV_IsCurRatSupportIms:LTE support Ems!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsCurRatSupportEms_ENUM, 3);
        return IMSA_TRUE;
    }

    return IMSA_FALSE;
}


VOS_UINT32 IMSA_SRV_IsNrmSrvIsimParaSatisfied( VOS_VOID )
{
    IMSA_COMMON_INFO_STRU              *pstImsaCommonInfo = VOS_NULL_PTR;
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;

    /* 判断USIM卡状态是否正常 */
    pstControlMagnaer                       = IMSA_GetControlManagerAddress();
    if ((IMSA_PS_SIMM_INVALID == pstControlMagnaer->ucImsaUsimStatus)
         && (IMSA_IMS_DOMAIN_LTE == IMSA_GetCurrentDomain()))
    {
        IMSA_INFO_LOG("IMSA_SRV_IsNrmSrvIsimParaSatisfied:USIM not available!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsNrmSrvIsimParaSatisfied_ENUM, 1);
        return IMSA_FALSE;
    }

    if (IMSA_ISIM_STATUS_AVAILABLE != IMSA_GetIsimStatus())
    {
        IMSA_INFO_LOG("IMSA_SRV_IsNrmSrvIsimParaSatisfied:ISIM not available!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsNrmSrvIsimParaSatisfied_ENUM, 2);
        return IMSA_FALSE;
    }

    pstImsaCommonInfo = IMSA_GetCommonInfoAddress();
    if (IMSA_OP_TRUE != pstImsaCommonInfo->bitOpHomeNetDomainName)
    {
        IMSA_INFO_LOG("IMSA_SRV_IsNrmSrvIsimParaSatisfied:No home network domain name!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsNrmSrvIsimParaSatisfied_ENUM, 3);
        return IMSA_FALSE;
    }

    if (IMSA_OP_TRUE != pstImsaCommonInfo->stImsaUeId.bitOpImpi)
    {
        IMSA_INFO_LOG("IMSA_SRV_IsNrmSrvIsimParaSatisfied:No impi!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsNrmSrvIsimParaSatisfied_ENUM, 4);
        return IMSA_FALSE;
    }

    if (IMSA_OP_TRUE != pstImsaCommonInfo->stImsaUeId.bitOpTImpu)
    {
        IMSA_INFO_LOG("IMSA_SRV_IsNrmSrvIsimParaSatisfied:No temporary IMPU!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsNrmSrvIsimParaSatisfied_ENUM, 5);
        return IMSA_FALSE;
    }

    if (IMSA_TRUE == pstControlMagnaer->ucIsCardLocked)
    {
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsNrmSrvIsimParaSatisfied_ENUM, 6);
        return IMSA_FALSE;
    }

    return IMSA_TRUE;
}


IMSA_NRM_SRV_CON_SAT_STATUE_ENMUM_UINT32 IMSA_SRV_IsNrmSrvConditonSatisfied( VOS_VOID )
{
    /* 当前接入技术支持IMS特性 */
    if (IMSA_FALSE == IMSA_SRV_IsCurRatSupportIms())
    {
        IMSA_INFO_LOG("IMSA_SRV_IsNrmSrvConditonSatisfied:cur rat not support ims!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsNrmSrvConditonSatisfied_ENUM, 1);
        return IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_RAT_NOT_SUPPORT;
    }

    if (IMSA_VOICE_DOMAIN_CS_ONLY == IMSA_GetVoiceDomain())
    {
        IMSA_INFO_LOG("IMSA_SRV_IsNrmSrvConditonSatisfied:voice not support!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsNrmSrvConditonSatisfied_ENUM, 2);

        /* VOICE DOMAIN为CS ONLY时，增加CHR的主动上报 */
        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_NotInitRegEventReport(IMSA_ERR_LOG_NOT_INIT_IMS_REG_REASON_VOICE_DAOMAIN_CS_ONLY);
        #endif

        return IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_VOICE_NOT_SUPPORT;
    }

    if (VOS_FALSE == IMSA_GetUeImsVoiceCap())
    {
        IMSA_INFO_LOG("IMSA_SRV_IsNrmSrvConditonSatisfied:voice not support!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsNrmSrvConditonSatisfied_ENUM, 8);

        /* UE不支持IMS语音时，增加CHR的主动上报 */
        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_NotInitRegEventReport(IMSA_ERR_LOG_NOT_INIT_IMS_REG_REASON_UE_NOT_SUPPORT_IMS_VOICE);
        #endif

        return IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_VOICE_NOT_SUPPORT;
    }

    if ((IMSA_IMS_VOPS_STATUS_NOT_SUPPORT == IMSA_GetNwImsVoiceCap())
        &&(IMSA_IMS_DOMAIN_LTE == IMSA_GetCurrentDomain()))
    {
        /* 如果网侧不支持IMS语音，但是UE支持IMS短信，且短信触发注册的开关
        打开时，需要继续判断其他条件是否满足 */
        if ((IMSA_FALSE == IMSA_GetUeSmsCapInitRegFlag())
            || (IMSA_FALSE == IMSA_GetUeImsSmsCap()))
        {
            IMSA_INFO_LOG("IMSA_SRV_IsNrmSrvConditonSatisfied:voice not support!");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsNrmSrvConditonSatisfied_ENUM, 9);

            /* 网侧不支持VOLTE时，增加CHR的主动上报 */
            #if (FEATURE_ON == FEATURE_PTM)
            IMSA_NotInitRegEventReport(IMSA_ERR_LOG_NOT_INIT_IMS_REG_REASON_NW_NOT_SUPPORT_VOPS);
            #endif
            return IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_VOICE_NOT_SUPPORT;
        }

    }

    #if 0
    /* IMS语音不支持，则返回条件不满足 */
    if ((IMSA_VOICE_DOMAIN_CS_ONLY == IMSA_GetVoiceDomain())
            || (VOS_FALSE == IMSA_GetUeImsVoiceCap())
            || ((IMSA_IMS_VOPS_STATUS_NOT_SUPPORT == IMSA_GetNwImsVoiceCap())
                &&(IMSA_IMS_DOMAIN_LTE == IMSA_GetCurrentDomain())))
    {
        IMSA_INFO_LOG("IMSA_SRV_IsNrmSrvConditonSatisfied:voice not support!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsNrmSrvConditonSatisfied_ENUM, 2);
        return IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_VOICE_NOT_SUPPORT;
    }
    #endif

    /* PS域服务状态为正常服务 */
    if ((IMSA_PS_SERVICE_STATUS_NORMAL_SERVICE != IMSA_GetPsServiceStatus())
        &&(IMSA_IMS_DOMAIN_LTE == IMSA_GetCurrentDomain()))
    {
        IMSA_INFO_LOG("IMSA_SRV_IsNrmSrvConditonSatisfied:Ps service not normal!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsNrmSrvConditonSatisfied_ENUM, 3);
        return IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_OTHERS;
    }


    /* 如果是在漫游网络下，且UE设置为不支持漫游网络下的注册，则直接退出，并且通知MMA VOICE不可用 */
    if ((VOS_TRUE == IMSA_GetNetInfoAddress()->ucImsaRoamingFlg)
        &&(VOS_TRUE == IMSA_GetConfigParaAddress()->ucRoamingImsNotSupportFlag)
        &&(IMSA_IMS_DOMAIN_LTE == IMSA_GetCurrentDomain()))
    {
        IMSA_INFO_LOG("IMSA_SRV_IsNrmSrvConditonSatisfied:Roaming and not support !");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsNrmSrvConditonSatisfied_ENUM, 4);

        /* 漫游网络下不支持发起IMS注册时，增加CHR的主动上报 */
        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_NotInitRegEventReport(IMSA_ERR_LOG_NOT_INIT_IMS_REG_REASON_NOT_SUPPORT_ROAMING_REG);
        #endif

        return IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_ROAM_NOT_SUPPORT;
    }


    /* 卡状态为有效可用，且IMPI、IMPU，DOMAIN参数有效 */
    if (IMSA_TRUE != IMSA_SRV_IsNrmSrvIsimParaSatisfied())
    {
        IMSA_INFO_LOG("IMSA_SRV_IsNrmSrvConditonSatisfied:ISIM PARA not satisfied!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsNrmSrvConditonSatisfied_ENUM, 5);
        return IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_USIM_NOT_SUPPORT;
    }

    /* IMS注册被禁止 */
    if (IMSA_TRUE == IMSA_GetIsImsRegForbidden())
    {
        IMSA_INFO_LOG("IMSA_SRV_IsNrmSrvConditonSatisfied:IMS reg forbidden!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsNrmSrvConditonSatisfied_ENUM, 6);

        /* UE被禁时，增加CHR的主动上报 */
        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_NotInitRegEventReport(IMSA_ERR_LOG_NOT_INIT_IMS_REG_REASON_REG_FORBIDEN);
        #endif

        return IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_REG_FORBIDDEN;
    }

    /* IMS信令承载建立永久被禁使用新标志, 用于注册被禁区分 */
    if (IMSA_TRUE == IMSA_GetIsImsConnForbiddenFlag())
    {
        IMSA_INFO_LOG("IMSA_SRV_IsNrmSrvConditonSatisfied:IMS Conn forbidden!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsNrmSrvConditonSatisfied_ENUM, 7);

        /* UE被禁时，增加CHR的主动上报 */
        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_NotInitRegEventReport(IMSA_ERR_LOG_NOT_INIT_IMS_REG_REASON_CONN_FORBIDEN);
        #endif

        return IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_CONN_FORBIDDEN;
    }

    /* Pnd临时被拒惩罚定时器运行时返回注册被拒 */
    if (VOS_TRUE == IMSA_IsTempForbTimerRunning())
    {
        IMSA_INFO_LOG("IMSA_SRV_IsNrmSrvConditonSatisfied:Forb Timer Running!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsNrmSrvConditonSatisfied_ENUM, 8);

        /* UE被禁时，增加CHR的主动上报 */
        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_NotInitRegEventReport(IMSA_ERR_LOG_NOT_INIT_IMS_REG_REASON_TEMP_FORBIDEN_TIMER_RUNNING);
        #endif

        /* IMS信令承载建立临时被禁使用新标志, 用于注册被禁区分 */
        return IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_CONN_FORBIDDEN;
    }

    return IMSA_NRM_SRV_CON_SAT_STATUE_SUCC;
}


VOS_UINT32 IMSA_SRV_IsMakeEmcCallInNrmSrvSatisfied( VOS_VOID )
{
    IMSA_NETWORK_INFO_STRU             *pstImsaNwInfo       = VOS_NULL_PTR;

    pstImsaNwInfo       = IMSA_GetNetInfoAddress();

    /* 是否在HPLMN上 */
    if (VOS_TRUE == pstImsaNwInfo->ucImsaRoamingFlg)
    {
        IMSA_INFO_LOG("IMSA_SRV_IsMakeEmcCallInNrmSrvSatisfied:NOT HPLMN!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsMakeEmcCallInNrmSrvSatisfied_ENUM, 1);
        return IMSA_FALSE;
    }

    /* 正常服务是否已注册成功 */
    if (IMSA_SRV_STATUS_CONN_REG != IMSA_SRV_GetNormalSrvStatus())
    {
        IMSA_INFO_LOG("IMSA_SRV_IsMakeEmcCallInNrmSrvSatisfied:normal service not reg!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsMakeEmcCallInNrmSrvSatisfied_ENUM, 2);
        return IMSA_FALSE;
    }

    /* 定义了紧急呼承载且网侧支持建紧急承载 */
    if ((IMSA_TRUE == IMSA_CONN_IsEmcPdpDefined())
        && (IMSA_EMS_STAUTS_SUPPORT == pstImsaNwInfo->enImsaEmsStatus))
    {
        IMSA_INFO_LOG("IMSA_SRV_IsMakeEmcCallInNrmSrvSatisfied:defined emc pdp and nw support emc bearer!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsMakeEmcCallInNrmSrvSatisfied_ENUM, 3);

        return IMSA_FALSE;
    }

    return IMSA_TRUE;
}


VOS_UINT32 IMSA_SRV_IsEmcSrvConditonSatisfied( VOS_VOID )
{
    IMSA_NETWORK_INFO_STRU             *pstImsaNwInfo       = VOS_NULL_PTR;
    IMSA_CONFIG_PARA_STRU              *pstImsaConfigPara   = VOS_NULL_PTR;

    pstImsaNwInfo       = IMSA_GetNetInfoAddress();
    pstImsaConfigPara   = IMSA_GetConfigParaAddress();

    /* wifi下不支持紧急呼叫，如果当前在wifi下驻留，则直接回复失败 */
    if (IMSA_IMS_DOMAIN_WIFI == IMSA_GetCurrentDomain())
    {
        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_CallFailErrRecord( IMSA_CALL_TYPE_EMC,
                                IMSA_ERR_LOG_NORM_CALL_FAIL_REASON_NONE,
                                IMSA_ERR_LOG_EMC_CALL_FAIL_REASON_CUR_RAT_NOT_SUPPORT_IMS,
                                VOS_NULL_PTR,
                                VOS_FALSE);
        #endif
        return IMSA_FALSE;
    }

    /* 无需判断当前接入技术是否支持IMS语音，因为即使NV不配置，SPM依旧可以把电话域选到IMS */

    /* 当前接入技术支持EMS特性 */
    if (IMSA_FALSE == IMSA_SRV_IsCurRatSupportEms())
    {
        IMSA_INFO_LOG("IMSA_SRV_IsEmcSrvConditonSatisfied:cur rat not support Ems!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsEmcSrvConditonSatisfied_ENUM, 2);
        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_CallFailErrRecord( IMSA_CALL_TYPE_EMC,
                                IMSA_ERR_LOG_NORM_CALL_FAIL_REASON_NONE,
                                IMSA_ERR_LOG_EMC_CALL_FAIL_REASON_CUR_RAT_NOT_SUPPORT_EMC_IMS,
                                VOS_NULL_PTR,
                                VOS_FALSE);
        #endif
        return IMSA_FALSE;
    }

    /* UE是否支持语音 */
    if (VOS_FALSE == IMSA_GetUeImsVoiceCap())
    {
        IMSA_INFO_LOG("IMSA_SRV_IsEmcSrvConditonSatisfied:ue not support voice!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsEmcSrvConditonSatisfied_ENUM, 3);
        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_CallFailErrRecord( IMSA_CALL_TYPE_EMC,
                                IMSA_ERR_LOG_NORM_CALL_FAIL_REASON_NONE,
                                IMSA_ERR_LOG_EMC_CALL_FAIL_REASON_UE_CAP_NOT_SUPPORT_VOICE_CALL,
                                VOS_NULL_PTR,
                                VOS_FALSE);
        #endif
        return IMSA_FALSE;
    }

    /* 能否在正常服务下打紧急呼 */
    if (IMSA_TRUE == IMSA_SRV_IsMakeEmcCallInNrmSrvSatisfied())
    {
        IMSA_INFO_LOG("IMSA_SRV_IsEmcSrvConditonSatisfied:can make emc call in normal service!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsEmcSrvConditonSatisfied_ENUM, 4);
        return IMSA_TRUE;
    }

    /* 能否打无卡紧急呼 */
    /* 如果网络当前处于非普通服务的状态(limited service或no service)时，
       需要发起无卡紧急呼，如果NV配置不支持，则返回失败 */
    if ((VOS_FALSE == pstImsaConfigPara->ucNoCardEmcCallSupportFlag)
        && (IMSA_PS_SERVICE_STATUS_NORMAL_SERVICE != pstImsaNwInfo->enImsaPsServiceStatus))
    {
        IMSA_INFO_LOG("IMSA_SRV_IsEmcSrvConditonSatisfied:not support anonymous emc call!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsEmcSrvConditonSatisfied_ENUM, 5);
        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_CallFailErrRecord( IMSA_CALL_TYPE_EMC,
                                IMSA_ERR_LOG_NORM_CALL_FAIL_REASON_NONE,
                                IMSA_ERR_LOG_EMC_CALL_FAIL_REASON_CANNOT_MAKE_NOCAR_EMC_CALL,
                                VOS_NULL_PTR,
                                VOS_FALSE);
        #endif
        return IMSA_FALSE;
    }

    /* 有卡，普通服务时，判断网络是否支持IMS紧急承载建立 */
    if ((IMSA_PS_SERVICE_STATUS_NORMAL_SERVICE == pstImsaNwInfo->enImsaPsServiceStatus)
        && (VOS_TRUE == IMSA_SRV_IsNrmSrvIsimParaSatisfied()))
    {
        /* 网侧是否支持建紧急承载 */
        if (IMSA_EMS_STAUTS_NOT_SUPPORT == pstImsaNwInfo->enImsaEmsStatus)
        {
            IMSA_INFO_LOG("IMSA_SRV_IsEmcSrvConditonSatisfied:nw not support emc bearer!");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsEmcSrvConditonSatisfied_ENUM, 6);
            #if (FEATURE_ON == FEATURE_PTM)
            IMSA_CallFailErrRecord( IMSA_CALL_TYPE_EMC,
                                    IMSA_ERR_LOG_NORM_CALL_FAIL_REASON_NONE,
                                    IMSA_ERR_LOG_EMC_CALL_FAIL_REASON_NW_NOT_SUPPORT_EMC_BEAR,
                                    VOS_NULL_PTR,
                                    VOS_FALSE);
            #endif

            return IMSA_FALSE;
        }
    }

    return IMSA_TRUE;
}


IMSA_NORMAL_REG_STATUS_ENUM_UINT32 IMSA_SRV_GetRegStatusFromSrvStatus
(
    IMSA_SRV_STATUS_ENUM_UINT8          enSrvStatus
)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;

    pstControlMagnaer                  = IMSA_GetControlManagerAddress();


    switch (enSrvStatus)
    {
        case IMSA_SRV_STATUS_IDLE_DEREG:
        /* case IMSA_SRV_STATUS_CONNING_DEREG: */
        case IMSA_SRV_STATUS_RELEASING_DEREG:
        case IMSA_SRV_STATUS_CONN_DEREG:

            if (IMSA_IMS_DOMAIN_WIFI == IMSA_GetCurrentDomain())
            {
                return IMSA_NORMAL_REG_STATUS_WIFI_DEREG;
            }

            return IMSA_NORMAL_REG_STATUS_LTE_DEREG;

        case IMSA_SRV_STATUS_CONN_DEREGING:

            if (IMSA_IMS_DOMAIN_WIFI == IMSA_GetCurrentDomain())
            {
                return IMSA_NORMAL_REG_STATUS_WIFI_DEREGING;
            }

            return IMSA_NORMAL_REG_STATUS_LTE_DEREGING;

        case IMSA_SRV_STATUS_CONN_REGING:
        case IMSA_SRV_STATUS_CONNING_DEREG:
        case IMSA_SRV_STATUS_CONNING_REG:

            if (IMSA_IMS_DOMAIN_WIFI == IMSA_GetCurrentDomain())
            {
                return IMSA_NORMAL_REG_STATUS_WIFI_REGING;
            }
            return IMSA_NORMAL_REG_STATUS_LTE_REGING;
        case IMSA_SRV_STATUS_HO_DEREG:
            if( IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH == pstControlMagnaer->enSwitchType)
            {
                /*lint -e655*/
                return (IMSA_NORMAL_REG_STATUS_LTE_DEREG|IMSA_NORMAL_REG_STATUS_LTE2WIFI_HO_START);
                /*lint +e655*/
            }
            else
            {   /*lint -e655*/
                return (IMSA_NORMAL_REG_STATUS_WIFI_DEREG|IMSA_NORMAL_REG_STATUS_WIFI2LTE_HO_START);
                /*lint +e655*/
            }

        case IMSA_SRV_STATUS_HO_REG:

            if( IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH == pstControlMagnaer->enSwitchType)
            {
                /*lint -e655*/
                return (IMSA_NORMAL_REG_STATUS_LTE_REG|IMSA_NORMAL_REG_STATUS_LTE2WIFI_HO_START);
                /*lint +e655*/
            }
            else
            {   /*lint -e655*/
                return (IMSA_NORMAL_REG_STATUS_WIFI_REG|IMSA_NORMAL_REG_STATUS_WIFI2LTE_HO_START);
                /*lint +e655*/
            }

        default:

            if (IMSA_IMS_DOMAIN_WIFI == IMSA_GetCurrentDomain())
            {
                return IMSA_NORMAL_REG_STATUS_WIFI_REG;
            }
            else
            {
                return IMSA_NORMAL_REG_STATUS_LTE_REG;
            }
    }
}


IMSA_NORMAL_AT_REG_STATUS_ENUM_UINT32 IMSA_SRV_GetAtRegStatusFromSrvStatus
(
    IMSA_SRV_STATUS_ENUM_UINT8          enSrvStatus
)
{
    switch (enSrvStatus)
    {
        case IMSA_SRV_STATUS_CONNING_REG:
        case IMSA_SRV_STATUS_CONN_REG:
        case IMSA_SRV_STATUS_HO_REG:

            return IMSA_NORMAL_AT_REG_STATUS_REG;

        default:

            return IMSA_NORMAL_AT_REG_STATUS_DEREG;
    }
}



VOS_UINT32 IMSA_SRV_IsSrvStatusEqual
(
    IMSA_SRV_TYPE_ENUM_UINT8            enSrvType,
    IMSA_SRV_STATUS_ENUM_UINT8          enImsaSrvStatus
)
{
    IMSA_SRV_STATUS_ENUM_UINT8          enImsaSrvStatusTmp = IMSA_SRV_STATUS_BUTT;

    if (IMSA_SRV_TYPE_NORMAL == enSrvType)
    {
        enImsaSrvStatusTmp = IMSA_SRV_GetNormalSrvStatus();
    }
    else
    {
        enImsaSrvStatusTmp = IMSA_SRV_GetEmcSrvStatus();
    }

    if (enImsaSrvStatus == enImsaSrvStatusTmp)
    {
        return IMSA_TRUE;
    }
    else
    {
        return IMSA_FALSE;
    }
}



VOS_VOID IMSA_SRV_SndConnSetupReq
(
    VOS_UINT32                          ulReEstablishFlag,
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_IMS_DOMAIN_ENUM_UINT8           enConnRatType
)
{
    IMSA_CONN_SETUP_REQ_STRU           *pstConnSetupReq     = VOS_NULL_PTR;
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_SRV_SndConnSetupReq is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_SndConnSetupReq_ENUM, LNAS_ENTRY);

    /*分配空间并检验分配是否成功*/
    pstConnSetupReq = (VOS_VOID*)IMSA_GetIntraMsgBuffAddr(sizeof(IMSA_CONN_SETUP_REQ_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstConnSetupReq)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SRV_SndConnSetupReq:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SRV_SndConnSetupReq_ENUM, LNAS_NULL_PTR);
        return ;
    }

    pstControlMagnaer  = IMSA_GetControlManagerAddress();
    /* 如果要建立的是普通连接，则停止周期性尝试IMS普通服务定时器 */
    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        IMSA_StopTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer);
    }

    /* delete PeriodImsEmcSrvTryTimer */


    /*清空*/
    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstConnSetupReq),
                    IMSA_GET_MSG_LENGTH(pstConnSetupReq),
                    0,
                    IMSA_GET_MSG_LENGTH(pstConnSetupReq));

    /*填写消息头*/
    IMSA_WRITE_INTRA_MSG_HEAD(  pstConnSetupReq,
                                ID_IMSA_CONN_SETUP_REQ,
                                sizeof(IMSA_CONN_SETUP_REQ_STRU));

    /*填写消息内容*/
    pstConnSetupReq->ulReEstablishFlag  = ulReEstablishFlag;
    pstConnSetupReq->enConnType         = enConnType;
    pstConnSetupReq->enConnRatType      = enConnRatType;

    /*调用消息发送函数 */
    IMSA_SEND_INTRA_MSG(pstConnSetupReq);
}


VOS_VOID IMSA_SRV_SndConnRelReq
(
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType,
    IMSA_CONN_SIP_PDP_TYPE_ENUM_UINT32      enSipPdpType,
    IMSA_IMS_DOMAIN_ENUM_UINT8               enConnRatType,
    IMSA_CONN_RELEASE_TYPE_ENUM_UINT8        enConnRlsType
)
{
    IMSA_CONN_REL_REQ_STRU             *pstConnRelReq       = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_SRV_SndConnRelReq is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_SndConnRelReq_ENUM, LNAS_ENTRY);

    /*分配空间并检验分配是否成功*/
    pstConnRelReq = (VOS_VOID*)IMSA_GetIntraMsgBuffAddr(sizeof(IMSA_CONN_REL_REQ_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstConnRelReq)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SRV_SndConnSetupReq:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SRV_SndConnRelReq_ENUM, LNAS_NULL_PTR);
        return ;
    }

    /*清空*/
    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstConnRelReq),
                    IMSA_GET_MSG_LENGTH(pstConnRelReq),
                    0,
                    IMSA_GET_MSG_LENGTH(pstConnRelReq));

    /*填写消息头*/
    IMSA_WRITE_INTRA_MSG_HEAD(  pstConnRelReq,
                                ID_IMSA_CONN_REL_REQ,
                                sizeof(IMSA_CONN_REL_REQ_STRU));

    /*填写消息内容*/
    pstConnRelReq->enConnType       = enConnType;
    pstConnRelReq->enSipPdpType     = enSipPdpType;

    pstConnRelReq->enConnRatType    = enConnRatType;
    pstConnRelReq->enConnRlsType    = enConnRlsType;

    /*调用消息发送函数 */
    IMSA_SEND_INTRA_MSG(pstConnRelReq);
}



VOS_VOID IMSA_SRV_SndRegRegReq
(
    IMSA_REG_TYPE_ENUM_UINT8            enRegType,
    IMSA_REG_ADDR_PARAM_ENUM_UINT32     enAddrType
)
{
    IMSA_REG_REG_REQ_STRU              *pstRegRegReq        = VOS_NULL_PTR;
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_SRV_SndRegRegReq is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_SndRegRegReq_ENUM, LNAS_ENTRY);

    /*分配空间并检验分配是否成功*/
    pstRegRegReq = (VOS_VOID*)IMSA_GetIntraMsgBuffAddr(sizeof(IMSA_REG_REG_REQ_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstRegRegReq)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SRV_SndRegRegReq:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SRV_SndRegRegReq_ENUM, LNAS_NULL_PTR);
        return ;
    }

    /* 关闭周期性尝试IMS服务定时器 */
    pstControlMagnaer  = IMSA_GetControlManagerAddress();

    if (IMSA_REG_TYPE_NORMAL == enRegType)
    {
        IMSA_StopTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer);
    }

    /* delete PeriodImsEmcSrvTryTimer */

    /*清空*/
    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstRegRegReq),
                    IMSA_GET_MSG_LENGTH(pstRegRegReq),
                    0,
                    IMSA_GET_MSG_LENGTH(pstRegRegReq));

    /*填写消息头*/
    IMSA_WRITE_INTRA_MSG_HEAD(  pstRegRegReq,
                                ID_IMSA_REG_REG_REQ,
                                sizeof(IMSA_REG_REG_REQ_STRU));

    /*填写消息内容*/
    pstRegRegReq->enRegType     = enRegType;
    pstRegRegReq->enAddrType    = enAddrType;

    /*调用消息发送函数 */
    IMSA_SEND_INTRA_MSG(pstRegRegReq);
}


VOS_VOID IMSA_SRV_SndRegReregReq
(
    IMSA_REG_TYPE_ENUM_UINT8            enRegType,
    IMSA_REG_BEARER_TYPE_ENUM_UINT8     enRegBearerType
)
{
    IMSA_REG_REREG_REQ_STRU            *pstRegReregReq        = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_SRV_SndRegReregReq is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_SndRegRegReq_ENUM, LNAS_ENTRY);

    /*分配空间并检验分配是否成功*/
    pstRegReregReq = (VOS_VOID*)IMSA_GetIntraMsgBuffAddr(sizeof(IMSA_REG_REREG_REQ_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstRegReregReq)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SRV_SndRegReregReq:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SRV_SndRegRegReq_ENUM, LNAS_NULL_PTR);
        return ;
    }

    /*清空*/
    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstRegReregReq),IMSA_GET_MSG_LENGTH(pstRegReregReq), 0, IMSA_GET_MSG_LENGTH(pstRegReregReq));

    /*填写消息头*/
    IMSA_WRITE_INTRA_MSG_HEAD(  pstRegReregReq,
                                ID_IMSA_REG_REREG_REQ,
                                sizeof(IMSA_REG_REREG_REQ_STRU));

    /*填写消息内容*/
    pstRegReregReq->enReregType       = enRegType;
    pstRegReregReq->enRegBearerType   = enRegBearerType;

    /*调用消息发送函数 */
    IMSA_SEND_INTRA_MSG(pstRegReregReq);
}



VOS_VOID IMSA_SRV_SndNrmCallSrvStatusInd
(
    IMSA_CALL_SERVICE_STATUS_ENUM_UINT32       enCallSrvStatus,
    IMSA_CALL_NO_SRV_CAUSE_ENUM_UINT32         enNoSrvCause,
    IMSA_CALL_NORM_SRV_EXTEND_INFO_ENUM_UINT32 enNormSrvExtInfo
)
{
    IMSA_NRM_CALL_SRV_STATUS_IND_STRU    *pstNrmCallSrvStatusInd = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_SRV_SndNrmCallSrvStatusInd is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_SndNrmCallSrvStatusInd_ENUM, LNAS_ENTRY);

    /*分配空间并检验分配是否成功*/
    pstNrmCallSrvStatusInd = (VOS_VOID*)IMSA_GetIntraMsgBuffAddr(sizeof(IMSA_NRM_CALL_SRV_STATUS_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstNrmCallSrvStatusInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SRV_SndNrmCallSrvStatusInd:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SRV_SndNrmCallSrvStatusInd_ENUM, LNAS_NULL_PTR);
        return ;
    }

    /*清空*/
    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstNrmCallSrvStatusInd),
                    IMSA_GET_MSG_LENGTH(pstNrmCallSrvStatusInd),
                    0,
                    IMSA_GET_MSG_LENGTH(pstNrmCallSrvStatusInd));

    /*填写消息头*/
    IMSA_WRITE_INTRA_MSG_HEAD(  pstNrmCallSrvStatusInd,
                                ID_IMSA_NRM_CALL_SRV_STATUS_IND,
                                sizeof(IMSA_NRM_CALL_SRV_STATUS_IND_STRU));

    /*填写消息内容*/
    pstNrmCallSrvStatusInd->enCallSrvStatus     = enCallSrvStatus;
    pstNrmCallSrvStatusInd->enNoSrvCause        = enNoSrvCause;

    /* 新增Norm Srv扩展信息, 用于Call进行相应操作 */
    pstNrmCallSrvStatusInd->enNormSrvExtendInfo = enNormSrvExtInfo;

    /*调用消息发送函数 */
    IMSA_SEND_INTRA_MSG(pstNrmCallSrvStatusInd);
}


VOS_VOID IMSA_SRV_SndEmcCallSrvStatusInd
(
    IMSA_CALL_SERVICE_STATUS_ENUM_UINT32       enCallSrvStatus,
    IMSA_CALL_NO_SRV_CAUSE_ENUM_UINT32         enNoSrvCause
)
{
    IMSA_EMC_CALL_SRV_STATUS_IND_STRU     *pstEmcCallSrvStatusInd = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_SRV_SndEmcCallSrvStatusInd is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_SndEmcCallSrvStatusInd_ENUM, LNAS_ENTRY);

    /*分配空间并检验分配是否成功*/
    pstEmcCallSrvStatusInd = (VOS_VOID*)IMSA_GetIntraMsgBuffAddr(sizeof(IMSA_EMC_CALL_SRV_STATUS_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstEmcCallSrvStatusInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SRV_SndEmcCallSrvStatusInd:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SRV_SndEmcCallSrvStatusInd_ENUM, LNAS_NULL_PTR);
        return ;
    }

    /*清空*/
    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstEmcCallSrvStatusInd),
                    IMSA_GET_MSG_LENGTH(pstEmcCallSrvStatusInd),
                    0,
                    IMSA_GET_MSG_LENGTH(pstEmcCallSrvStatusInd));

    /*填写消息头*/
    IMSA_WRITE_INTRA_MSG_HEAD(  pstEmcCallSrvStatusInd,
                                ID_IMSA_EMC_CALL_SRV_STATUS_IND,
                                sizeof(IMSA_EMC_CALL_SRV_STATUS_IND_STRU));

    /*填写消息内容*/
    pstEmcCallSrvStatusInd->enCallSrvStatus = enCallSrvStatus;
    pstEmcCallSrvStatusInd->enNoSrvCause    = enNoSrvCause;

    /*调用消息发送函数 */
    IMSA_SEND_INTRA_MSG(pstEmcCallSrvStatusInd);
}


VOS_VOID IMSA_SRV_SndRegDeregReq
(
    IMSA_REG_TYPE_ENUM_UINT8            enDeRegType,
    VOS_UINT32                          ulLocalFlag
)
{
    IMSA_REG_DEREG_REQ_STRU            *pstRegDeregReq  = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_SRV_SndRegDeregReq is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_SndRegDeregReq_ENUM, LNAS_ENTRY);

    /*分配空间并检验分配是否成功*/
    pstRegDeregReq = (VOS_VOID*)IMSA_GetIntraMsgBuffAddr(sizeof(IMSA_REG_DEREG_REQ_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstRegDeregReq)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SRV_SndRegRegReq:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SRV_SndRegDeregReq_ENUM, LNAS_NULL_PTR);
        return ;
    }

    /*清空*/
    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstRegDeregReq),
                    IMSA_GET_MSG_LENGTH(pstRegDeregReq),
                    0,
                    IMSA_GET_MSG_LENGTH(pstRegDeregReq));

    /*填写消息头*/
    IMSA_WRITE_INTRA_MSG_HEAD(  pstRegDeregReq,
                                ID_IMSA_REG_DEREG_REQ,
                                sizeof(IMSA_REG_DEREG_REQ_STRU));

    /*填写消息内容*/
    pstRegDeregReq->enDeRegType   = enDeRegType;
    pstRegDeregReq->ulLocalFlag   = ulLocalFlag;

    /*调用消息发送函数 */
    IMSA_SEND_INTRA_MSG(pstRegDeregReq);
}


VOS_VOID IMSA_SRV_SndNrmRegStatusNotify
(
    const IMSA_SUBSCRIPTION_INFO_STRU  *pstSubsciptionInfo,
    IMSA_NORMAL_REG_STATUS_ENUM_UINT32  enNormalRegStatus
)
{
    IMSA_NORMAL_REG_STATUS_NOTIFY_STRU     *pstNrmRegStatusNotify = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_SRV_SndNrmRegStatusNotify is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_SndNrmRegStatusNotify_ENUM, LNAS_ENTRY);

    /*分配空间并检验分配是否成功*/
    pstNrmRegStatusNotify = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_NORMAL_REG_STATUS_NOTIFY_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstNrmRegStatusNotify)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SRV_SndNrmRegStatusNotify:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SRV_SndNrmRegStatusNotify_ENUM, LNAS_NULL_PTR);
        return ;
    }

    /*清空*/
    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstNrmRegStatusNotify),
                    IMSA_GET_MSG_LENGTH(pstNrmRegStatusNotify),
                    0,
                    IMSA_GET_MSG_LENGTH(pstNrmRegStatusNotify));

    /*填写消息头*/
    pstNrmRegStatusNotify->ulMsgId          = ID_IMSA_NORMAL_REG_STATUS_NOTIFY;
    pstNrmRegStatusNotify->ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstNrmRegStatusNotify->ulSenderPid      = PS_PID_IMSA;
    pstNrmRegStatusNotify->ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstNrmRegStatusNotify->ulReceiverPid    = pstSubsciptionInfo->ulPid;

    /* 填写消息内容 */
    pstNrmRegStatusNotify->enNormalRegStatus= enNormalRegStatus;
    pstNrmRegStatusNotify->ulPara           = pstSubsciptionInfo->ulPara;
    pstNrmRegStatusNotify->ulSubscriptionId = pstSubsciptionInfo->ulSubscriptionId;

    /*调用消息发送函数 */
    IMSA_SND_MSG(pstNrmRegStatusNotify);
}


VOS_VOID IMSA_SRV_NrmRegStatusNotify
(
    IMSA_NORMAL_REG_STATUS_ENUM_UINT32  enNrmRegStatus
)
{
    VOS_UINT32                          i                       = IMSA_NULL;
    IMSA_SUBSCRIPTION_INFO_LIST_STRU   *pstSubcriptionInfoList  = VOS_NULL_PTR;
    IMSA_SUBSCRIPTION_INFO_STRU        *pstSubcriptionInfo      = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_SRV_NrmRegStatusNotify is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_NrmRegStatusNotify_ENUM, LNAS_ENTRY);

    pstSubcriptionInfoList  = IMSA_GetSubcriptionInfoListAddr();

    for (i = 0; i < pstSubcriptionInfoList->ulSubscriptionNum; i++)
    {
        pstSubcriptionInfo = &pstSubcriptionInfoList->astSubcriptionInfoArray[i];
        if (IMSA_SUBCRIBE_TYPE_NORMAL_REG == pstSubcriptionInfo->enType)
        {
            IMSA_SRV_SndNrmRegStatusNotify(pstSubcriptionInfo, enNrmRegStatus);
        }
    }
}


/*****************************************************************************
 Function Name  : IMSA_SRV_NrmRegStatusNotify
 Description    : 将最新的普通注册状态通知订阅模块
 Input          : enNrmRegStatus-------------普通注册状态
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.zhangdongfeng       2016-04-11  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_SRV_SrvStatusNotify
(
    IMSA_SRV_STATUS_ENUM_UINT8          enNrmSrvStatus,
    IMSA_SRV_STATUS_ENUM_UINT8          enEmcSrvStatus,
    IMSA_IMS_DOMAIN_ENUM_UINT8          enConnRatType
)
{
    IMSA_AT_IMS_SERVICE_STATUS_ENUM_UINT8   enSmsSrvStatus;
    IMSA_AT_IMS_SERVICE_STATUS_ENUM_UINT8   enVoIpSrvStatus;
    IMSA_AT_IMS_SERVICE_STATUS_ENUM_UINT8   enVtSrvStatus;
    IMSA_AT_IMS_SERVICE_STATUS_ENUM_UINT8   enVsSrvStatus;

    IMSA_AT_IMS_RAT_TYPE_ENUM_UINT8       enSrvRat;
    IMSA_CONTROL_MANAGER_STRU             *pstControlManager   = VOS_NULL_PTR;

    pstControlManager  = IMSA_GetControlManagerAddress();

    /* 设置SMS状态 */
    if((VOS_TRUE == pstControlManager->stImsaConfigPara.ucSmsOnImsSupportFlag)
       && (IMSA_NORMAL_AT_REG_STATUS_REG == IMSA_SRV_GetAtRegStatusFromSrvStatus(enNrmSrvStatus)))
    {
        enSmsSrvStatus = IMSA_AT_IMS_SERVICE_STATUS_FULL_SERVICE;
    }
    else
    {
        enSmsSrvStatus = IMSA_AT_IMS_SERVICE_STATUS_NO_SERVICE;
    }

    /* 设置VT状态 */
    if((VOS_TRUE == pstControlManager->stImsaConfigPara.ucVideoCallOnImsSupportFlag)
       && (IMSA_NORMAL_AT_REG_STATUS_REG == IMSA_SRV_GetAtRegStatusFromSrvStatus(enNrmSrvStatus)))
    {
        enVtSrvStatus = IMSA_AT_IMS_SERVICE_STATUS_FULL_SERVICE;
    }
    else
    {
        enVtSrvStatus = IMSA_AT_IMS_SERVICE_STATUS_NO_SERVICE;
    }

    /* 设置VOIP状态 */
    if((VOS_TRUE == pstControlManager->stImsaConfigPara.ucVoiceCallOnImsSupportFlag)
       && (IMSA_NORMAL_AT_REG_STATUS_REG == IMSA_SRV_GetAtRegStatusFromSrvStatus(enNrmSrvStatus)))
    {
        enVoIpSrvStatus = IMSA_AT_IMS_SERVICE_STATUS_FULL_SERVICE;
    }
    else if(IMSA_NORMAL_AT_REG_STATUS_REG == IMSA_SRV_GetAtRegStatusFromSrvStatus(enEmcSrvStatus))
    {
        enVoIpSrvStatus = IMSA_AT_IMS_SERVICE_STATUS_LIMITED_SERVICE;
    }
    else
    {
        enVoIpSrvStatus = IMSA_AT_IMS_SERVICE_STATUS_NO_SERVICE;
    }

    /* 当前Modem实现不支持VS */
    enVsSrvStatus = IMSA_AT_IMS_SERVICE_STATUS_NO_SERVICE;

    if( IMSA_IMS_DOMAIN_WIFI == enConnRatType)
    {
        enSrvRat = IMSA_AT_IMS_RAT_TYPE_WIFI;
    }
    else
    {
        enSrvRat = IMSA_AT_IMS_RAT_TYPE_LTE;
    }

    /* 上报状态 */
    IMSA_SndMsgAtSrvStatusUpdateInd(enSmsSrvStatus,enVoIpSrvStatus,
                                    enVtSrvStatus,enVsSrvStatus,enSrvRat);

    return;


}






VOS_UINT32 IMSA_SRV_GetImsVoiceCapability(MSCC_IMSA_IMS_VOICE_CAP_ENUM_UINT8 *penImsVoiceCap)
{
    VOS_UINT32                    ulResult           = IMSA_FALSE;
    IMSA_CONTROL_MANAGER_STRU     *pstControlMagnaer = VOS_NULL_PTR;

    pstControlMagnaer  = IMSA_GetControlManagerAddress();

    switch(IMSA_SRV_GetNormalSrvStatus())
    {
        case IMSA_SRV_STATUS_IDLE_DEREG:
            if (VOS_FALSE == IMSA_IsTimerRunning(&pstControlMagnaer->stPeriodImsSrvTryTimer))
            {
                *penImsVoiceCap = MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE;
                ulResult = IMSA_TRUE;
            }
            break;
        case IMSA_SRV_STATUS_CONN_REG:
            if (IMSA_TRUE == IMSA_IsImsVoiceContidionSatisfied())
            {
                *penImsVoiceCap = MSCC_IMSA_IMS_VOICE_CAP_AVAILABLE;
            }
            else
            {
                *penImsVoiceCap = MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE;
            }
            ulResult = IMSA_TRUE;
            break;
        case IMSA_SRV_STATUS_CONN_DEREG:
            *penImsVoiceCap = MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE;
            ulResult = IMSA_TRUE;
            break;

        default:
            break;
    }

    return ulResult;
}



VOS_VOID IMSA_SRV_SetServcieStatus
(
    IMSA_SRV_TYPE_ENUM_UINT8            enSrvType,
    IMSA_SRV_STATUS_ENUM_UINT8          enSrvStatus
)
{
    IMSA_NORMAL_REG_STATUS_ENUM_UINT32  enNrmRegStatus = IMSA_NORMAL_REG_STATUS_BUTT;
    IMSA_NORMAL_AT_REG_STATUS_ENUM_UINT32 enNrmAtRegStatus = IMSA_NORMAL_AT_REG_STATUS_BUTT;
    MSCC_IMSA_IMS_VOICE_CAP_ENUM_UINT8  enImsVoiceCap = MSCC_IMSA_IMS_VOICE_CAP_BUTT;


    /* 获取服务原状态 */

    /* 打印服务类型 */

    /* 打印服务状态变更新信息 */

    if (IMSA_SRV_TYPE_EMC == enSrvType)
    {
        /* 勾出状态的变化 */
        IMSA_SndOmServiceStateChangeInfo(IMSA_SRV_TYPE_EMC, enSrvStatus, IMSA_SRV_GetEmcSrvStatus());

        IMSA_SRV_GetEmcSrvStatus() = enSrvStatus;

        /* 如果取得IMS VOICE能力，则上报IMS VOICE能力 */
        if (IMSA_TRUE == IMSA_SRV_GetImsVoiceCapability(&enImsVoiceCap))
        {
            IMSA_SndMsccMsgImsVoiceCapNotify(enImsVoiceCap, IMSA_CONN_IsExitPersistentBearer());
        }

        IMSA_SRV_SrvStatusNotify(IMSA_SRV_GetNormalSrvStatus(), enSrvStatus, IMSA_IMS_DOMAIN_LTE);
        return ;
    }

    /* 勾出状态的变化 */
    IMSA_SndOmServiceStateChangeInfo(IMSA_SRV_TYPE_NORMAL, enSrvStatus, IMSA_SRV_GetNormalSrvStatus());

    /* 根据服务状态，获取普通注册状态 */
    enNrmRegStatus = IMSA_SRV_GetRegStatusFromSrvStatus(enSrvStatus);

    /* 如果普通注册状态发生变更，则通知签约的模块最新的普通注册状态 */
    if (enNrmRegStatus != IMSA_GetLastNrmRegStatus())
    {
        IMSA_SRV_NrmRegStatusNotify(enNrmRegStatus);

        IMSA_SetLastNrmRegStatus(enNrmRegStatus);
    }

    /* 根据服务状态，获取普通AT注册状态 */
    enNrmAtRegStatus = IMSA_SRV_GetAtRegStatusFromSrvStatus(enSrvStatus);

    IMSA_INFO_LOG2("IMSA_SRV_SetServcieStatus:enNrmAtRegStatus = ,IMSA_GetLastNrmAtRegStatus()=",
                   enNrmAtRegStatus,IMSA_GetLastNrmAtRegStatus());
    TLPS_PRINT2LAYER_INFO2(IMSA_SRV_SetServcieStatus_ENUM, 1, enNrmAtRegStatus, IMSA_GetLastNrmAtRegStatus());

    /* 如果普通AT注册状态发生变更，则通知AT最新的普通AT注册状态 */
    if (enNrmAtRegStatus != IMSA_GetLastNrmAtRegStatus())
    {
        IMSA_SndMsgAtCireguInd(enNrmAtRegStatus);

        IMSA_SetLastNrmAtRegStatus(enNrmAtRegStatus);
    }

    IMSA_SRV_GetNormalSrvStatus() = enSrvStatus;

    if (IMSA_SRV_STATUS_CONN_REG != IMSA_SRV_GetNormalSrvStatus())
    {
        /* SRVCC异常(例如关机，DEREG REQ，状态迁离CONN+REG)，清缓存 */
        IMSA_SrvccAbormalClearBuff(IMSA_SRVCC_ABNORMAL_STATUS_CHANGE);

        IMSA_SMS_ClearResource();

        IMSA_USSD_ClearResource();
    }

    /* 如果取得IMS VOICE能力，则上报IMS VOICE能力 */
    if (IMSA_TRUE == IMSA_SRV_GetImsVoiceCapability(&enImsVoiceCap))
    {
        IMSA_SndMsccMsgImsVoiceCapNotify(enImsVoiceCap, IMSA_CONN_IsExitPersistentBearer());
    }

}

VOS_VOID IMSA_SRV_ProcConnSetupIndSucc
(
    const IMSA_CONN_SETUP_IND_STRU     *pstConnSetupIndMsg
)
{
    IMSA_CONTROL_MANAGER_STRU              *pstImsaControlManager   = VOS_NULL_PTR;
    VOS_CHAR                                acUeAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};
    VOS_CHAR                                acPcscfAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};

    IMSA_INFO_LOG("IMSA_SRV_ProcConnSetupIndSucc is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcConnSetupIndSucc_ENUM, LNAS_ENTRY);

    pstImsaControlManager = IMSA_GetControlManagerAddress();

    /* 给CALL的服务状态发生变更才通知CALL;如果当前接入技术不支持IMS，则是无服务；
       当前接入技术支持IMS时，则增加如下判断:
       CONN REG有服务，CONN DEREG看是否是无卡
    如果是无卡则是有服务，否则是无服务；其他都是无服务,CALL模块收到无服务需要释放
    CALL。*/

    if (IMSA_CONN_SIP_PDP_TYPE_MEDIA == pstConnSetupIndMsg->enSipPdpType)
    {
        /* 通知CALL模块服务已经具备 */
        IMSA_ProcCallResourceIsReady(pstConnSetupIndMsg->enConnType, pstConnSetupIndMsg->enMediaPdpType);
        return ;
    }

    if (IMSA_CONN_TYPE_NORMAL == pstConnSetupIndMsg->enConnType)
    {
        IMSA_UtilStrNCpy(acUeAddr, pstImsaControlManager->stNrmRegParaInfo.acUeAddr, IMSA_IPV6_ADDR_STRING_LEN);
        IMSA_UtilStrNCpy(acPcscfAddr, pstImsaControlManager->stNrmRegParaInfo.acPcscfAddr, IMSA_IPV6_ADDR_STRING_LEN);
    }
    else
    {
        IMSA_UtilStrNCpy(acUeAddr, pstImsaControlManager->stEmcRegParaInfo.acUeAddr, IMSA_IPV6_ADDR_STRING_LEN);
        IMSA_UtilStrNCpy(acPcscfAddr, pstImsaControlManager->stEmcRegParaInfo.acPcscfAddr, IMSA_IPV6_ADDR_STRING_LEN);
    }
    /* 如果当前处于REG+CONNING状态 */
    if (IMSA_TRUE == IMSA_SRV_IsSrvStatusEqual((IMSA_SRV_TYPE_ENUM_UINT8)pstConnSetupIndMsg->enConnType,
                                                    IMSA_SRV_STATUS_CONNING_REG))
    {
        /* CR刷新 Issue A+Issue B+Issue C+Issue D: When the PDP context used for SIP signalling (EPS bearer context
           used for SIP signalling) is deactivated by procedures not triggered by UE and either the network requests
           reactivation or the UE desires or is required to be registered, then the UE performs the same procedures
           as at initially ensuring the PDP context used for SIP signalling (EPS bearer context used for SIP signalling).
           If the operation is successful, then the UE performs a new initial registration. */
        /* 如果备份的注册参数有效，则使用当前地址注册 */
        if (IMSA_TRUE == IMSA_IsRegParaAvailable((IMSA_SRV_TYPE_ENUM_UINT8)pstConnSetupIndMsg->enConnType,
                                                  acUeAddr,
                                                  acPcscfAddr))
        {
            /* Current地址对在去激活信令承载时已经被清除, 所以此时需要重新置上 */
            (VOS_VOID)IMSA_SetCurrentPara(pstConnSetupIndMsg->enConnType, acUeAddr, acPcscfAddr);

            IMSA_SRV_SetServcieStatus((IMSA_SRV_TYPE_ENUM_UINT8)pstConnSetupIndMsg->enConnType, IMSA_SRV_STATUS_CONN_REGING);

            IMSA_SRV_SndRegDeregReq((IMSA_SRV_TYPE_ENUM_UINT8)pstConnSetupIndMsg->enConnType, IMSA_DEREG_LOCAL);

            IMSA_SRV_SndRegRegReq((IMSA_SRV_TYPE_ENUM_UINT8)pstConnSetupIndMsg->enConnType, IMSA_REG_ADDR_PARAM_SAME);
        }
        else
        /* 如果备份的注册参数无效，则将本地去注册，并且通知REG模块重新发起注册 */
        {
            IMSA_SRV_SetServcieStatus((IMSA_SRV_TYPE_ENUM_UINT8)pstConnSetupIndMsg->enConnType, IMSA_SRV_STATUS_CONN_REGING);

            IMSA_SRV_SndRegDeregReq((IMSA_SRV_TYPE_ENUM_UINT8)pstConnSetupIndMsg->enConnType, IMSA_DEREG_LOCAL);

            IMSA_SRV_SndRegRegReq((IMSA_SRV_TYPE_ENUM_UINT8)pstConnSetupIndMsg->enConnType, IMSA_REG_ADDR_PARAM_NEW);


        }

        /* 切离CONNING+REG态时，IMSA需要通知IMS允许发包 */
        if (IMSA_CONN_TYPE_NORMAL == pstConnSetupIndMsg->enConnType)
        {
            IMSA_SndImsMsgServiceSuspendOrResumeSrvInfo(IMSA_IMS_INPUT_SERVICE_REASON_RESUME_NRM_SRV);

            #if (FEATURE_ON == FEATURE_PTM)
            IMSA_SRV_InitDeregEventReport(IMSA_ERR_LOG_INIT_DEREG_REASON_PDN_LOST);

            /* 记录发起注册的时间 */
            ( VOS_VOID )LPS_OM_GET32K_TIMESTAMP(&(IMSA_GetErrlogStopConnTimerAddr()->ul32KTStampHighBit),
                                                &(IMSA_GetErrlogStopConnTimerAddr()->ul32KTStampLowBit));

            /* 记录UE发起注册的时间 */
            ( VOS_VOID )LPS_OM_GET32K_TIMESTAMP(&(IMSA_GetErrlogStartRegTimerAddr()->ul32KTStampHighBit),
                                                &(IMSA_GetErrlogStartRegTimerAddr()->ul32KTStampLowBit));

            /* 承载建立时长超过阈值判断及上报 */
            IMSA_PdnConnTimerExpireEventReport();
            #endif
        }
        else
        {
            IMSA_SndImsMsgServiceSuspendOrResumeSrvInfo(IMSA_IMS_INPUT_SERVICE_REASON_RESUME_EMC_SRV);
        }


        return;
    }

    /* 信令承载激活成功分支 */

    /* 如果不在CONNING&DEREG态，则直接退出 */
    if (IMSA_TRUE != IMSA_SRV_IsSrvStatusEqual( (IMSA_SRV_TYPE_ENUM_UINT8)pstConnSetupIndMsg->enConnType,
                                                IMSA_SRV_STATUS_CONNING_DEREG))
    {
        IMSA_WARN_LOG("IMSA_SRV_ProcConnSetupIndSucc:SRV status is not conningdereg!");
        TLPS_PRINT2LAYER_WARNING(IMSA_SRV_ProcConnSetupIndSucc_ENUM, IMSA_STATE_ERROR);
        return ;
    }

    if (IMSA_CONN_TYPE_EMC == pstConnSetupIndMsg->enConnType)
    {
        /* 如果是无卡紧急呼，则转到CONN&DEREG态，不再发起注册，通知CALL紧急
           服务具备;否则转到CONN&REGING态，请求REG模块发起紧急注册 */
        if (VOS_TRUE == IMSA_SRV_GetNoRegEmcFlag())
        {
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_EMC, IMSA_SRV_STATUS_CONN_DEREG);

            /* 假的注册流程，仅为触发REG模块给IMS配置地址对 */
            IMSA_SRV_SndRegRegReq(IMSA_REG_TYPE_EMC, IMSA_REG_ADDR_PARAM_NO_REG_EMC_CALL);

            /* 无卡紧急呼无需紧急注册，因此通知CALL模块紧急呼叫服务具备 */
            IMSA_SRV_SndEmcCallSrvStatusInd(IMSA_CALL_SERVICE_STATUS_NORMAL_SERVICE,
                                            IMSA_CALL_NO_SRV_CAUSE_BUTT);
        }
        else
        {
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_EMC, IMSA_SRV_STATUS_CONN_REGING);

            IMSA_SRV_SndRegRegReq(IMSA_REG_TYPE_EMC, IMSA_REG_ADDR_PARAM_NEW);
        }
    }
    else
    {
        /* 上报状态给Domain，Domain会发起域选过程 */
        if( VOS_TRUE == pstImsaControlManager->ucDomainRegSrvStatusFlag)
        {
            pstImsaControlManager->ucDomainRegSrvStatusFlag = VOS_FALSE;
            IMSA_SRV_SndDomainSrvReadyInd();
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_CONN_DEREG);
        }
        else
        {
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_CONN_REGING);

            IMSA_SRV_SndRegRegReq(IMSA_REG_TYPE_NORMAL, IMSA_REG_ADDR_PARAM_NEW);

            /* 记录UE建链成功的时间 */
            #if (FEATURE_ON == FEATURE_PTM)
            ( VOS_VOID )LPS_OM_GET32K_TIMESTAMP(&(IMSA_GetErrlogStopConnTimerAddr()->ul32KTStampHighBit),
                                                &(IMSA_GetErrlogStopConnTimerAddr()->ul32KTStampLowBit));

            /* 记录UE发起注册的时间 */
            ( VOS_VOID )LPS_OM_GET32K_TIMESTAMP(&(IMSA_GetErrlogStartRegTimerAddr()->ul32KTStampHighBit),
                                                &(IMSA_GetErrlogStartRegTimerAddr()->ul32KTStampLowBit));

            /* 承载建立时长超过阈值判断及上报 */
            IMSA_PdnConnTimerExpireEventReport();
            #endif
        }
    }

    return ;
}


VOS_VOID IMSA_SRV_ProcWifiConnSetupIndSucc
(
    const IMSA_CONN_SETUP_IND_STRU     *pstConnSetupIndMsg
)
{
    IMSA_CONTROL_MANAGER_STRU              *pstImsaControlManager   = VOS_NULL_PTR;
    VOS_CHAR                                acUeAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};
    VOS_CHAR                                acPcscfAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};

    IMSA_INFO_LOG("IMSA_SRV_ProcConnSetupIndSucc is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcConnSetupIndSucc_ENUM, LNAS_ENTRY);

    pstImsaControlManager = IMSA_GetControlManagerAddress();

    /* 信令承载激活成功分支 */
    if (IMSA_CONN_SIP_PDP_TYPE_MEDIA == pstConnSetupIndMsg->enSipPdpType)
    {
        /* 通知CALL模块服务已经具备 */
        IMSA_ProcCallResourceIsReady(pstConnSetupIndMsg->enConnType, pstConnSetupIndMsg->enMediaPdpType);
        return ;
    }

    if (IMSA_CONN_TYPE_NORMAL == pstConnSetupIndMsg->enConnType)
    {
        IMSA_UtilStrNCpy(acUeAddr, pstImsaControlManager->stNrmRegParaInfo.acUeAddr, IMSA_IPV6_ADDR_STRING_LEN);
        IMSA_UtilStrNCpy(acPcscfAddr, pstImsaControlManager->stNrmRegParaInfo.acPcscfAddr, IMSA_IPV6_ADDR_STRING_LEN);
    }
    else
    {
        IMSA_UtilStrNCpy(acUeAddr, pstImsaControlManager->stEmcRegParaInfo.acUeAddr, IMSA_IPV6_ADDR_STRING_LEN);
        IMSA_UtilStrNCpy(acPcscfAddr, pstImsaControlManager->stEmcRegParaInfo.acPcscfAddr, IMSA_IPV6_ADDR_STRING_LEN);
    }

    /* 如果当前处于REG+CONNING状态 */
    if (IMSA_TRUE == IMSA_SRV_IsSrvStatusEqual((IMSA_SRV_TYPE_ENUM_UINT8)pstConnSetupIndMsg->enConnType,
                                                    IMSA_SRV_STATUS_CONNING_REG))
    {
        /* 如果备份的注册参数有效，将备份的注册参数设置为CURRENT参数，将当前服务状态修改为CONN+REG状态 */
        if (IMSA_TRUE == IMSA_IsRegParaAvailable((IMSA_SRV_TYPE_ENUM_UINT8)pstConnSetupIndMsg->enConnType,
                                                  acUeAddr,
                                                  acPcscfAddr))
        {
            (VOS_VOID)IMSA_SetCurrentPara((IMSA_SRV_TYPE_ENUM_UINT8)pstConnSetupIndMsg->enConnType,
                                          acUeAddr,
                                          acPcscfAddr);
            IMSA_SRV_SetServcieStatus((IMSA_SRV_TYPE_ENUM_UINT8)pstConnSetupIndMsg->enConnType, IMSA_SRV_STATUS_CONN_REG);

            /* 通知domain模块注册成功 */
            /* coverity[unchecked_value] */
            IMSA_SRV_SndDomainSrvStatusInd( IMSA_DOMAIN_SERVICE_STATUS_NORMAL_SERVICE,
                                            IMSA_DOMAIN_NO_SRV_CAUSE_BUTT,
                                            IMSA_GetCurrentDomain());

        }
        else
        /* 如果备份的注册参数无效，则将本地去注册，并且通知REG模块重新发起注册 */
        {
            IMSA_SRV_SetServcieStatus((IMSA_SRV_TYPE_ENUM_UINT8)pstConnSetupIndMsg->enConnType, IMSA_SRV_STATUS_CONN_REGING);

            IMSA_SRV_SndRegDeregReq((IMSA_SRV_TYPE_ENUM_UINT8)pstConnSetupIndMsg->enConnType, IMSA_DEREG_LOCAL);

            IMSA_SRV_SndRegRegReq((IMSA_SRV_TYPE_ENUM_UINT8)pstConnSetupIndMsg->enConnType, IMSA_REG_ADDR_PARAM_NEW);


        }

        /* 切离CONNING+REG态时，IMSA需要通知IMS允许发包 */
        if (IMSA_CONN_TYPE_NORMAL == pstConnSetupIndMsg->enConnType)
        {
            IMSA_SndImsMsgServiceSuspendOrResumeSrvInfo(IMSA_IMS_INPUT_SERVICE_REASON_RESUME_NRM_SRV);
        }
        else
        {
            IMSA_SndImsMsgServiceSuspendOrResumeSrvInfo(IMSA_IMS_INPUT_SERVICE_REASON_RESUME_EMC_SRV);
        }

        return;
    }

    /* 信令承载激活成功分支 */


    /* 如果不在CONNING&DEREG态，则直接退出 */
    if (IMSA_TRUE != IMSA_SRV_IsSrvStatusEqual( (IMSA_SRV_TYPE_ENUM_UINT8)pstConnSetupIndMsg->enConnType,
                                                IMSA_SRV_STATUS_CONNING_DEREG))
    {
        IMSA_WARN_LOG("IMSA_SRV_ProcConnSetupIndSucc:SRV status is not conningdereg!");
        TLPS_PRINT2LAYER_WARNING(IMSA_SRV_ProcConnSetupIndSucc_ENUM, IMSA_STATE_ERROR);
        return ;
    }

    if (IMSA_CONN_TYPE_EMC == pstConnSetupIndMsg->enConnType)
    {
        /* 遗留，WIFI下暂不支持紧急场景 */
    }
    else
    {
        /* 上报状态给Domain，Domain会发起域选过程 */
        if( VOS_TRUE == pstImsaControlManager->ucDomainRegSrvStatusFlag)
        {
            pstImsaControlManager->ucDomainRegSrvStatusFlag = VOS_FALSE;
            IMSA_SRV_SndDomainSrvReadyInd();
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_CONN_DEREG);
        }
        else
        {
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_CONN_REGING);

            IMSA_SRV_SndRegRegReq(IMSA_REG_TYPE_NORMAL, IMSA_REG_ADDR_PARAM_NEW);
        }
    }

    return ;
}

VOS_VOID IMSA_SRV_ProcLteConnSetupIndFail
(
    const IMSA_CONN_SETUP_IND_STRU     *pstConnSetupIndMsg
)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_SRV_ProcConnSetupIndFail is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcConnSetupIndFail_ENUM, LNAS_ENTRY);

    pstControlMagnaer  = IMSA_GetControlManagerAddress();

    if (IMSA_CONN_SIP_PDP_TYPE_MEDIA == pstConnSetupIndMsg->enSipPdpType)
    {
        IMSA_WARN_LOG("IMSA_SRV_ProcConnSetupIndFail:can not happen when nw initiate pdp activation!");
        TLPS_PRINT2LAYER_WARNING(IMSA_SRV_ProcConnSetupIndFail_ENUM, 2);
        return ;
    }

    /* 信令承载激活失败分支 */
    /* 重建立流程处于REG+CONNING状态 */
    if ((IMSA_CONN_TYPE_NORMAL == pstConnSetupIndMsg->enConnType)
        && (IMSA_TRUE == IMSA_SRV_IsSrvStatusEqual(IMSA_SRV_TYPE_NORMAL,
                                                   IMSA_SRV_STATUS_CONNING_REG)))
    {
        /* 本地去注册 */
        IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);
        IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_IDLE_DEREG);

        if (IMSA_CONN_RESULT_FAIL_PERM_FORBIDDEN == pstConnSetupIndMsg->enResult)
        {
            /* 信令承载建立永久被拒标志置上 */
            IMSA_SetIsImsConnForbiddenFlag(IMSA_TRUE);
        }
        else if (IMSA_CONN_RESULT_FAIL_TEMP_FORBIDDEN == pstConnSetupIndMsg->enResult)
        {
            /* 启动临时被拒惩罚定时器 */
            IMSA_StartTempForbTimer();
        }
        else
        {
            /* 启动周期性尝试定时器 */
            IMSA_StartTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer);
        }

        #if (FEATURE_ON == FEATURE_DSDS)
        if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
        {
            IMSA_StopTimer(&pstControlMagnaer->stGetSerProtectTimer);
            IMSA_DsdsProcResourceRelease(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
        }
        #endif

        /* 通知Domain连接异常 */
        IMSA_SRV_SndDomainBearerRelnd(IMSA_IMS_DOMAIN_LTE);
        return;
    }

    if ((IMSA_CONN_TYPE_EMC == pstConnSetupIndMsg->enConnType)
        && (IMSA_TRUE == IMSA_SRV_IsSrvStatusEqual(IMSA_SRV_TYPE_EMC,
                                                   IMSA_SRV_STATUS_CONNING_REG)))
    {
        /* 本地去注册 */
        IMSA_SRV_SndRegDeregReq(IMSA_SRV_TYPE_EMC, IMSA_DEREG_LOCAL);
        IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_EMC, IMSA_SRV_STATUS_IDLE_DEREG);

        return;
    }

    /* 如果不在CONNING&DEREG态，则直接退出 */
    if (IMSA_TRUE != IMSA_SRV_IsSrvStatusEqual( (IMSA_SRV_TYPE_ENUM_UINT8)pstConnSetupIndMsg->enConnType,
                                                IMSA_SRV_STATUS_CONNING_DEREG))
    {
        IMSA_WARN_LOG("IMSA_SRV_ProcConnSetupIndFail:SRV status is not conningdereg!");
        TLPS_PRINT2LAYER_WARNING(IMSA_SRV_ProcConnSetupIndFail_ENUM, IMSA_STATE_ERROR);
        return ;
    }

    if (IMSA_CONN_TYPE_EMC == pstConnSetupIndMsg->enConnType)
    {
        /* 状态转到IDLE&DEREG态，通知CALL紧急服务不具备 */
        IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_EMC, IMSA_SRV_STATUS_IDLE_DEREG);

        /* 通知CALL模块紧急呼叫服务进入无服务状态，促使CALL模块清除缓存的紧急呼 */
        IMSA_SRV_SndEmcCallSrvStatusInd(IMSA_CALL_SERVICE_STATUS_NO_SERVICE,
                                        IMSA_CALL_NO_SRV_CAUSE_SIP_PDP_ERR);
    }
    else
    {
        if ((IMSA_CONN_RESULT_FAIL_TIMER_EXP == pstConnSetupIndMsg->enResult)
            || (IMSA_CONN_RESULT_FAIL_CONN_RELEASING == pstConnSetupIndMsg->enResult)
            || (IMSA_CONN_RESULT_FAIL_SAME_APN_OPERATING == pstConnSetupIndMsg->enResult))
        {
            /* 启动周期性尝试IMS服务定时器 */
            IMSA_INFO_LOG("IMSA_SRV_ProcConnSetupIndFail:start period ims service tyr timer!");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcConnSetupIndFail_ENUM, 4);
            IMSA_StartTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer);

            /* 通知Domain连接异常 */
            IMSA_SRV_SndDomainBearerRelnd(IMSA_IMS_DOMAIN_LTE);
        }

        if (IMSA_CONN_RESULT_FAIL_PERM_FORBIDDEN == pstConnSetupIndMsg->enResult)
        {
            /* 信令承载建立永久被拒标志置上 */
            IMSA_SetIsImsConnForbiddenFlag(IMSA_TRUE);
        }

        if (IMSA_CONN_RESULT_FAIL_TEMP_FORBIDDEN == pstConnSetupIndMsg->enResult)
        {
            /* 启动临时被拒惩罚定时器 */
            IMSA_StartTempForbTimer();
        }

        /* 状态转到IDLE&DEREG态，通知CALL紧急服务不具备，由于IMSA_SRV_SetServcieStatus
           中会用到周期性尝试IMS服务定时器的运行情况，且当前处于注册过程中，
           所以需要先启动定时器后转状态 */
        IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_IDLE_DEREG);

        /* 删除临时以及永久被拒判断, 通知Domain模块进行域选 */
        if((IMSA_CONN_RESULT_FAIL_TIMER_EXP != pstConnSetupIndMsg->enResult)
            && (IMSA_CONN_RESULT_FAIL_CONN_RELEASING != pstConnSetupIndMsg->enResult)
            && (IMSA_CONN_RESULT_FAIL_SAME_APN_OPERATING != pstConnSetupIndMsg->enResult))
        {
            IMSA_SRV_SndDomainSrvStatusInd(IMSA_DOMAIN_SERVICE_STATUS_NO_SERVICE,
                                                IMSA_DOMAIN_NO_SRV_CAUSE_SIP_PDP_ERR,
                                               IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY );

            /* 上报状态给Domain，Domain会发起域选过程 */
            if( VOS_TRUE == pstControlMagnaer->ucDomainRegSrvStatusFlag)
            {
                pstControlMagnaer->ucDomainRegSrvStatusFlag = VOS_FALSE;
                IMSA_SRV_SndDomainSrvReadyInd();
            }

        }

        #if (FEATURE_ON == FEATURE_DSDS)
        if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
        {
            IMSA_StopTimer(&pstControlMagnaer->stGetSerProtectTimer);
            IMSA_DsdsProcResourceRelease(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
        }
        #endif
    }
}



VOS_VOID IMSA_SRV_ProcWifiConnSetupIndFail
(
    const IMSA_CONN_SETUP_IND_STRU     *pstConnSetupIndMsg
)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_SRV_ProcConnSetupIndFail is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcConnSetupIndFail_ENUM, LNAS_ENTRY);

    pstControlMagnaer  = IMSA_GetControlManagerAddress();

    if (IMSA_CONN_SIP_PDP_TYPE_MEDIA == pstConnSetupIndMsg->enSipPdpType)
    {
        IMSA_WARN_LOG("IMSA_SRV_ProcConnSetupIndFail:can not happen when nw initiate pdp activation!");
        TLPS_PRINT2LAYER_WARNING(IMSA_SRV_ProcConnSetupIndFail_ENUM, 2);
        return ;
    }

    /* 如果不在CONNING&DEREG态，则直接退出 */
    if ((IMSA_TRUE != IMSA_SRV_IsSrvStatusEqual( (IMSA_SRV_TYPE_ENUM_UINT8)pstConnSetupIndMsg->enConnType,
                                                IMSA_SRV_STATUS_CONNING_DEREG))
        &&(IMSA_TRUE != IMSA_SRV_IsSrvStatusEqual( (IMSA_SRV_TYPE_ENUM_UINT8)pstConnSetupIndMsg->enConnType,
                                                IMSA_SRV_STATUS_CONNING_REG)))
    {
        IMSA_WARN_LOG("IMSA_SRV_ProcConnSetupIndFail:SRV status is not conningdereg!");
        TLPS_PRINT2LAYER_WARNING(IMSA_SRV_ProcConnSetupIndFail_ENUM, IMSA_STATE_ERROR);
        return ;
    }

    /* 重建立流程处于REG+CONNING状态 */
    if ((IMSA_CONN_TYPE_NORMAL == pstConnSetupIndMsg->enConnType)
        && (IMSA_TRUE == IMSA_SRV_IsSrvStatusEqual(IMSA_SRV_TYPE_NORMAL,
                                                   IMSA_SRV_STATUS_CONNING_REG)))
    {
        /* 本地去注册 */
        IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);
        IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_IDLE_DEREG);

        /* 启动周期性尝试定时器 */
        IMSA_SRV_SndDomainSrvStatusInd(IMSA_DOMAIN_SERVICE_STATUS_NO_SERVICE,
                                            IMSA_DOMAIN_NO_SRV_CAUSE_SIP_PDP_ERR,
                                            IMSA_IMS_DOMAIN_MODE_WIFI_ONLY);

        return;
    }



    /* 如果不在CONNING&DEREG态，则直接退出 */
    if (IMSA_TRUE != IMSA_SRV_IsSrvStatusEqual( (IMSA_SRV_TYPE_ENUM_UINT8)pstConnSetupIndMsg->enConnType,
                                                IMSA_SRV_STATUS_CONNING_DEREG))
    {
        IMSA_WARN_LOG("IMSA_SRV_ProcConnSetupIndFail:SRV status is not conningdereg!");
        TLPS_PRINT2LAYER_WARNING(IMSA_SRV_ProcConnSetupIndFail_ENUM, IMSA_STATE_ERROR);
        return ;
    }


    if (IMSA_CONN_TYPE_EMC == pstConnSetupIndMsg->enConnType)
    {
        return;
    }
    else
    {
        if (IMSA_CONN_RESULT_FAIL_WIFI_READY_IND_TIMEOUT == pstConnSetupIndMsg->enResult)
        {
            /* 启动周期性尝试IMS服务定时器 */
            IMSA_INFO_LOG("IMSA_SRV_ProcConnSetupIndFail:start period ims service tyr timer!");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcConnSetupIndFail_ENUM, 4);
            IMSA_StartTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer);
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_IDLE_DEREG);
            /* 通知Domain连接异常 */
            IMSA_SRV_SndDomainBearerRelnd(IMSA_IMS_DOMAIN_WIFI);
        }
        else
        {

            /* 状态转到IDLE&DEREG态，通知CALL紧急服务不具备，由于IMSA_SRV_SetServcieStatus
                    中会用到周期性尝试IMS服务定时器的运行情况，且当前处于注册过程中，
                    所以需要先启动定时器后转状态 */
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_IDLE_DEREG);

            IMSA_SRV_SndDomainSrvStatusInd(IMSA_DOMAIN_SERVICE_STATUS_NO_SERVICE,
                                                IMSA_DOMAIN_NO_SRV_CAUSE_SIP_PDP_ERR,
                                                IMSA_IMS_DOMAIN_MODE_WIFI_ONLY);

            /* 上报状态给Domain，Domain会发起域选过程 */
            if( VOS_TRUE == pstControlMagnaer->ucDomainRegSrvStatusFlag)
            {
                pstControlMagnaer->ucDomainRegSrvStatusFlag = VOS_FALSE;
                IMSA_SRV_SndDomainSrvReadyInd();
            }
        }
    }
}



VOS_VOID IMSA_SRV_ProcConnSetupIndFail
(
    const IMSA_CONN_SETUP_IND_STRU     *pstConnSetupIndMsg
)
{
    if (IMSA_IMS_DOMAIN_LTE == pstConnSetupIndMsg->enConnRatType)
    {
        IMSA_SRV_ProcLteConnSetupIndFail(pstConnSetupIndMsg);
    }
    else
    {
        IMSA_SRV_ProcWifiConnSetupIndFail(pstConnSetupIndMsg);
    }
}


VOS_VOID IMSA_SRV_ProcRegRegIndSucc
(
    const IMSA_REG_REG_IND_STRU        *pstRegRegIndMsg
)
{
    IMSA_CONTROL_MANAGER_STRU              *pstImsaControlManager   = IMSA_GetControlManagerAddress();

    IMSA_INFO_LOG("IMSA_SRV_ProcRegRegIndSucc is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcRegRegIndSucc_ENUM, LNAS_ENTRY);

    /* 如果是正常服务，则状态转到CONN&REG态，通知CALL模块正常服务具备 */
    if (IMSA_REG_TYPE_NORMAL == pstRegRegIndMsg->enRegType)
    {
        IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_CONN_REG);

        /* 通知CALL模块普通呼叫服务进入有服务状态 */
        /* 新增入参 */
        IMSA_SRV_SndNrmCallSrvStatusInd(IMSA_CALL_SERVICE_STATUS_NORMAL_SERVICE,
                                        IMSA_CALL_NO_SRV_CAUSE_BUTT,
                                        IMSA_CALL_NORM_SRV_EXTEND_INFO_NONE);

        #if (FEATURE_ON == FEATURE_DSDS)
        if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
        {
            IMSA_StopTimer(&pstImsaControlManager->stGetSerProtectTimer);
            IMSA_DsdsProcResourceRelease(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
        }
        #endif
        /* 通知domain模块注册成功 */
        /* coverity[unchecked_value] */
        IMSA_SRV_SndDomainSrvStatusInd( IMSA_DOMAIN_SERVICE_STATUS_NORMAL_SERVICE,
                                        IMSA_DOMAIN_NO_SRV_CAUSE_BUTT,
                                        IMSA_GetCurrentDomain());

        /* 上报状态给Domain，Domain会发起域选过程 */
        if( VOS_TRUE == pstImsaControlManager->ucDomainRegSrvStatusFlag)
        {
            pstImsaControlManager->ucDomainRegSrvStatusFlag = VOS_FALSE;
            IMSA_SRV_SndDomainSrvReadyInd();
        }

        /* 记录UE注册成功的时间 */
        #if (FEATURE_ON == FEATURE_PTM)
        ( VOS_VOID )LPS_OM_GET32K_TIMESTAMP(&(IMSA_GetErrlogStopRegTimerAddr()->ul32KTStampHighBit),
                                            &(IMSA_GetErrlogStopRegTimerAddr()->ul32KTStampLowBit));

        /* 注册时长超过阈值判断及上报 */
        IMSA_RegTimerExpireEventReport();

        /* 注册成功事件上报 */
        IMSA_SRV_RegSuccEventReport();
        #endif
    }
    else
    {
        IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_EMC, IMSA_SRV_STATUS_CONN_REG);

        /* 通知CALL模块紧急呼叫服务进入有服务状态 */
        IMSA_SRV_SndEmcCallSrvStatusInd(IMSA_CALL_SERVICE_STATUS_NORMAL_SERVICE,
                                        IMSA_CALL_NO_SRV_CAUSE_BUTT);
    }

    return ;
}


VOS_VOID IMSA_SRV_SetImsRegForbidden
(
    IMSA_REG_TYPE_ENUM_UINT8       enRegType,
    IMSA_REG_RESULT_ENUM_UINT32    enResult
)
{
    if (IMSA_REG_TYPE_NORMAL == enRegType)
    {
        if (IMSA_REG_RESULT_FAIL_FORBIDDEN == enResult)
        {
            IMSA_SetIsImsRegForbidden(IMSA_TRUE);
        }
    }
}


VOS_VOID IMSA_SRV_ProcRegRegIndFail
(
    const IMSA_REG_REG_IND_STRU        *pstRegRegIndMsg
)
{
    IMSA_CONTROL_MANAGER_STRU              *pstImsaControlManager   = IMSA_GetControlManagerAddress();
    IMSA_CONN_RELEASE_TYPE_ENUM_UINT8       enConnRlsType;

    IMSA_INFO_LOG("IMSA_SRV_ProcRegRegIndFail is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcRegRegIndFail_ENUM, LNAS_ENTRY);

    /* 设置PDN释放类型 */
    if( VOS_TRUE == pstImsaControlManager->ucOrgPdnRlsFlag)
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL;
    }
    else
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_LOCAL;
    }

    switch (pstRegRegIndMsg->enResultAction)
    {
        case IMSA_RESULT_ACTION_NULL:

            IMSA_SRV_SetImsRegForbidden(pstRegRegIndMsg->enRegType,pstRegRegIndMsg->enResult);

            if (IMSA_REG_TYPE_EMC == pstRegRegIndMsg->enRegType)
            {
                if (IMSA_REG_RESULT_FAIL_FORBIDDEN == pstRegRegIndMsg->enResult)
                {
                    /* 通知CALL模块紧急呼叫服务继续*/
                    IMSA_SRV_SndEmcCallSrvStatusInd(IMSA_CALL_SERVICE_STATUS_NORMAL_SERVICE,
                                                    IMSA_CALL_NO_SRV_CAUSE_REG_ERR);
                    IMSA_SRV_SetServcieStatus(  (IMSA_SRV_TYPE_ENUM_UINT8)pstRegRegIndMsg->enRegType,
                                                IMSA_SRV_STATUS_CONN_DEREG);
                    IMSA_SRV_GetNoRegEmcFlag() = VOS_TRUE;
                    return;
                }
                else
                {
                    /* 通知CALL模块紧急呼叫服务进入无服务状态，促使CALL模块清除缓存的紧急呼 */
                    IMSA_SRV_SndEmcCallSrvStatusInd(IMSA_CALL_SERVICE_STATUS_NO_SERVICE,
                                                    IMSA_CALL_NO_SRV_CAUSE_REG_ERR);
                }
            }
            else
            {
                /* 只有普通类型的需要申请资源去注册，紧急的不需要，也不需要修改资源类型 */
                /* 使用注册的资源 */
                #if (FEATURE_ON == FEATURE_DSDS)
                if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
                {
                    IMSA_DsdsModifyTaskSrcByTaskType(IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG, RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);

                    /* 修改资源的触发目的*/
                    IMSA_StopTimer(&pstImsaControlManager->stGetSerProtectTimer);
                    IMSA_StartTimer(&pstImsaControlManager->stDeregSerProtectTimer);
                }
                #endif
            }
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnRelReq((IMSA_CONN_TYPE_ENUM_UINT32)pstRegRegIndMsg->enRegType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_GetCurrentDomain(),
                                    enConnRlsType);

            /* 后续不再注册，则释放连接，状态转到RELEASING&DEREG态 */
            IMSA_SRV_SetServcieStatus(  (IMSA_SRV_TYPE_ENUM_UINT8)pstRegRegIndMsg->enRegType,
                                        IMSA_SRV_STATUS_RELEASING_DEREG);
            break;
        case IMSA_RESULT_ACTION_REG_WITH_FIRST_ADDR_PAIR:

            IMSA_SRV_SndRegRegReq(pstRegRegIndMsg->enRegType, IMSA_REG_ADDR_PARAM_NEW);
            break;
        case IMSA_RESULT_ACTION_REG_WITH_CURRENT_ADDR_PAIR:

            IMSA_SRV_SndRegRegReq(pstRegRegIndMsg->enRegType, IMSA_REG_ADDR_PARAM_SAME);
            break;
        case IMSA_RESULT_ACTION_REG_WITH_NEXT_ADDR_PAIR:

            IMSA_SRV_SndRegRegReq(pstRegRegIndMsg->enRegType, IMSA_REG_ADDR_PARAM_NEXT);
            break;
        case IMSA_RESULT_ACTION_REG_LOCAL_DEREG:
            IMSA_SRV_SetServcieStatus((IMSA_SRV_TYPE_ENUM_UINT8)pstRegRegIndMsg->enRegType,
                                       IMSA_SRV_STATUS_CONN_DEREG);

            /* 切离CONN&REG态，因此通知CALL呼叫服务进入无服务状态 */
            /* 新增入参 */
            IMSA_SRV_SndCallSrvStatusInd((IMSA_SRV_TYPE_ENUM_UINT8)pstRegRegIndMsg->enRegType,
                                          IMSA_CALL_SERVICE_STATUS_NO_SERVICE,
                                          IMSA_CALL_NO_SRV_CAUSE_SIP_PDP_ERR,
                                          IMSA_CALL_NORM_SRV_EXTEND_INFO_NONE);

            /*非IDLE+DEREG状态不能给domain模块发No service*/
#if 0
            /* coverity[unchecked_value] */
            IMSA_SRV_SndDomainSrvStatusInd( IMSA_DOMAIN_SERVICE_STATUS_NO_SERVICE,
                                            IMSA_DOMAIN_NO_SRV_CAUSE_SIP_PDP_ERR,
                                            IMSA_GetCurrentDomain());
#endif

            /* 上报状态给Domain，Domain会发起域选过程 */
            if( VOS_TRUE == pstImsaControlManager->ucDomainRegSrvStatusFlag)
            {
                pstImsaControlManager->ucDomainRegSrvStatusFlag = VOS_FALSE;
                IMSA_SRV_SndDomainSrvReadyInd();
            }
            if (IMSA_REG_TYPE_NORMAL == pstRegRegIndMsg->enRegType)
            {
                #if (FEATURE_ON == FEATURE_DSDS)
                if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
                {
                    IMSA_StopTimer(&pstImsaControlManager->stGetSerProtectTimer);
                    IMSA_DsdsProcResourceRelease(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
                }
                #endif
            }

            break;
        /* 只有普通类型的注册失败，才会进入该分支；注册失败，retry after超时，申请资源失败*/
        #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESULT_ACTION_REG_NORF:
            IMSA_SRV_SetServcieStatus((IMSA_SRV_TYPE_ENUM_UINT8)pstRegRegIndMsg->enRegType,
                                       IMSA_SRV_STATUS_CONN_DEREG);

            /* 切离CONN&REG态，因此通知CALL呼叫服务进入无服务状态 */
            /* 新增入参 */
            IMSA_SRV_SndCallSrvStatusInd((IMSA_SRV_TYPE_ENUM_UINT8)pstRegRegIndMsg->enRegType,
                                          IMSA_CALL_SERVICE_STATUS_NO_SERVICE,
                                          IMSA_CALL_NO_SRV_CAUSE_SIP_PDP_ERR,
                                          IMSA_CALL_NORM_SRV_EXTEND_INFO_NONE);

            IMSA_StopTimer(&pstImsaControlManager->stGetSerProtectTimer);

            /* 通知RRM模块，将注册任务挂到RRM队列中 */
            IMSA_DsdsProcResourceRegisterInd(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                             IMSA_RF_RES_APPLY_TRIGER_SRC_REG_RETRY_EXP,
                                             IMSA_REG_ADDR_PARAM_SAME);
            break;
        #endif
        default:

            IMSA_INFO_LOG("IMSA_SRV_ProcRegRegIndFail:illegal result action!");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcRegRegIndFail_ENUM, LNAS_PARAM_INVALID);

            if (IMSA_REG_TYPE_EMC == pstRegRegIndMsg->enRegType)
            {
                /* 通知CALL模块紧急呼叫服务进入无服务状态，促使CALL模块清除缓存的紧急呼 */
                IMSA_SRV_SndEmcCallSrvStatusInd(IMSA_CALL_SERVICE_STATUS_NO_SERVICE,
                                                IMSA_CALL_NO_SRV_CAUSE_REG_ERR);
            }
            else
            {
                #if (FEATURE_ON == FEATURE_DSDS)
                if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
                {
                    IMSA_DsdsModifyTaskSrcByTaskType(IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG, RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);

                    /* 修改资源的触发目的*/
                    IMSA_StopTimer(&pstImsaControlManager->stGetSerProtectTimer);
                    IMSA_StartTimer(&pstImsaControlManager->stDeregSerProtectTimer);
                }
                #endif
            }

            /* 后续不再注册，则释放连接，状态转到RELEASING&DEREG态 */
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnRelReq((IMSA_CONN_TYPE_ENUM_UINT32)pstRegRegIndMsg->enRegType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_GetCurrentDomain(),
                                    enConnRlsType);
            IMSA_SRV_SetServcieStatus(  (IMSA_SRV_TYPE_ENUM_UINT8)pstRegRegIndMsg->enRegType,
                                        IMSA_SRV_STATUS_RELEASING_DEREG);
            break;
    }
}



VOS_VOID IMSA_SRV_ProcRegRegIndMsg
(
    const IMSA_REG_REG_IND_STRU        *pstRegRegIndMsg
)
{
    IMSA_INFO_LOG("IMSA_SRV_ProcRegRegIndMsg is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcRegRegIndMsg_ENUM, LNAS_ENTRY);

    /* 如果不在CONN&REGING态，则直接退出 */
    if (IMSA_TRUE != IMSA_SRV_IsSrvStatusEqual(     (IMSA_SRV_TYPE_ENUM_UINT8)pstRegRegIndMsg->enRegType,
                                                    IMSA_SRV_STATUS_CONN_REGING))
    {
        IMSA_WARN_LOG("IMSA_SRV_ProcRegRegIndMsg:Not conn&reging state!");
        TLPS_PRINT2LAYER_WARNING(IMSA_SRV_ProcRegRegIndMsg_ENUM, IMSA_STATE_ERROR);

        return ;
    }

    if (IMSA_REG_RESULT_SUCCESS == pstRegRegIndMsg->enResult)
    {
        IMSA_SRV_ProcRegRegIndSucc(pstRegRegIndMsg);
    }
    else
    {
        IMSA_SRV_ProcRegRegIndFail(pstRegRegIndMsg);
    }
}

VOS_VOID IMSA_SRV_RegConningStateProcRegDeregIndMsg
(
    IMSA_REG_TYPE_ENUM_UINT8            enRegType
)
{
    IMSA_CONTROL_MANAGER_STRU          *pstImsaControlManager   = VOS_NULL_PTR;

    pstImsaControlManager   = IMSA_GetControlManagerAddress();

    if (IMSA_REG_TYPE_NORMAL == enRegType)
    {
        /* 等周期性尝试定时器超时 */
        if (VOS_TRUE == IMSA_IsTimerRunning(&pstImsaControlManager->stPeriodImsSrvTryTimer))
        {
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_IDLE_DEREG);
        }
        else
        {
            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);

            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_CONNING_DEREG);
        }
        IMSA_SndImsMsgServiceSuspendOrResumeSrvInfo(IMSA_IMS_INPUT_SERVICE_REASON_RESUME_NRM_SRV);

        return;
    }
    /* 紧急注册 */
    if (IMSA_REG_TYPE_EMC == enRegType)
    {
        /* delete PeriodImsEmcSrvTryTimer */
        IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_EMC, IMSA_DEREG_LOCAL);

        IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_EMC,
                                    IMSA_SRV_STATUS_CONNING_DEREG);

        IMSA_SndImsMsgServiceSuspendOrResumeSrvInfo(IMSA_IMS_INPUT_SERVICE_REASON_RESUME_EMC_SRV);

        return;
    }
}


VOS_VOID IMSA_SRV_ProcRegDeregIndWithAddr
(
    IMSA_SRV_TYPE_ENUM_UINT8            enSrvType,
    VOS_UINT32                          ulPara
)
{
    #if (FEATURE_ON == FEATURE_DSDS)
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = IMSA_GetControlManagerAddress();
    #endif

    IMSA_SRV_SetServcieStatus(  enSrvType,
                                IMSA_SRV_STATUS_CONN_REGING);

    /* 切离CONN&REG态，因此通知CALL呼叫服务进入无服务状态 */
    /* 新增入参 */
    IMSA_SRV_SndCallSrvStatusInd( enSrvType,
                                  IMSA_CALL_SERVICE_STATUS_NO_SERVICE,
                                  IMSA_CALL_NO_SRV_CAUSE_REG_ERR,
                                  IMSA_CALL_NORM_SRV_EXTEND_INFO_NONE);

    switch (IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        case IMSA_RESOURCE_STATUS_ACTIVE:
            /* 进行初始注册，使用第一个地址对，状态转到CONN&REGING态 */
            IMSA_SRV_SndRegRegReq(      enSrvType,
                                        ulPara);

            #if (FEATURE_ON == FEATURE_DSDS)
            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
            }
            #endif
            break;
        #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESOURCE_STATUS_WAIT_STATUS_IND:
            /* 上报voice不可用 */
            IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE,IMSA_CONN_IsExitPersistentBearer());
            break;

        case IMSA_RESOURCE_STATUS_NULL:
            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_REREGISTER_FAIL,
                                          ulPara,
                                          IMSA_FALSE);
            break;

        case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:
            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_REREGISTER_FAIL,
                                          ulPara,
                                          IMSA_FALSE);

            IMSA_SRV_SndRegRegReq(  enSrvType,
                                    ulPara);

            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
            }

            break;

        case IMSA_RESOURCE_STATUS_APPLYING:
            break;
        #endif
        default:
            /* 进行初始注册，使用第一个地址对，状态转到CONN&REGING态 */
            IMSA_SRV_SndRegRegReq(      enSrvType,
                                        ulPara);

            #if (FEATURE_ON == FEATURE_DSDS)
            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
            }
            #endif
            break;
    }
    #if (FEATURE_ON == FEATURE_PTM)
    /* 记录UE发起注册的时间 */
    ( VOS_VOID )LPS_OM_GET32K_TIMESTAMP(&(IMSA_GetErrlogStartRegTimerAddr()->ul32KTStampHighBit),
                                        &(IMSA_GetErrlogStartRegTimerAddr()->ul32KTStampLowBit));

    #endif
}


VOS_VOID IMSA_SRV_ProcRegDeregIndActionNull
(
    IMSA_SRV_TYPE_ENUM_UINT8            enSrvType
)
{

    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = IMSA_GetControlManagerAddress();


    IMSA_CONN_RELEASE_TYPE_ENUM_UINT8       enConnRlsType;

    /* 设置PDN释放类型 */
    if( VOS_TRUE == pstControlMagnaer->ucOrgPdnRlsFlag)
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL;
    }
    else
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_LOCAL;
    }


    IMSA_SRV_SetServcieStatus(  enSrvType,
                                IMSA_SRV_STATUS_RELEASING_DEREG);

    /* 切离CONN&REG态，因此通知CALL呼叫服务进入无服务状态 */
    /* 新增入参 */
    IMSA_SRV_SndCallSrvStatusInd( enSrvType,
                                  IMSA_CALL_SERVICE_STATUS_NO_SERVICE,
                                  IMSA_CALL_NO_SRV_CAUSE_REG_ERR,
                                  IMSA_CALL_NORM_SRV_EXTEND_INFO_NONE);

    if (IMSA_SRV_TYPE_EMC == enSrvType)
    {
        /* GCF 19.5.10用例，紧急呼挂断，IMS紧急注册超时后，上报紧急用户注销，
           IMSA收到后不再主动释放紧急PDN连接，状态迁移至CONN+DEREG */
        /* IMSA_SRV_SndConnRelReq(enSrvType,
                                IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                IMSA_GetCurrentDomain(),
                                enConnRlsType);

        IMSA_SRV_SetServcieStatus(  enSrvType,
                                    IMSA_SRV_STATUS_IDLE_DEREG); */
        IMSA_SRV_SetServcieStatus(  enSrvType,
                                    IMSA_SRV_STATUS_CONN_DEREG);
        return;
    }

    switch (IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        case IMSA_RESOURCE_STATUS_ACTIVE:
            #if (FEATURE_ON == FEATURE_DSDS)
            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_StopTimer(&pstControlMagnaer->stGetSerProtectTimer);
                IMSA_StartTimer(&pstControlMagnaer->stDeregSerProtectTimer);
            }
            #endif
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnRelReq(enSrvType,
                                   IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                   IMSA_GetCurrentDomain(),
                                   enConnRlsType);
            break;
        #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESOURCE_STATUS_NULL:
            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          IMSA_FALSE,
                                          IMSA_FALSE);
            break;

        case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:
            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          IMSA_FALSE,
                                          IMSA_FALSE);
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnRelReq(enSrvType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_GetCurrentDomain(),
                                    enConnRlsType);
            #if (FEATURE_ON == FEATURE_DSDS)
            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_StopTimer(&pstControlMagnaer->stGetSerProtectTimer);
                IMSA_StartTimer(&pstControlMagnaer->stDeregSerProtectTimer);
            }
            #endif
            break;

        case IMSA_RESOURCE_STATUS_APPLYING:
            /* 修改原因值 */
            IMSA_DsdsModifyTaskSrcByTaskType(IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                             RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
            break;
        #endif
        default:
            #if (FEATURE_ON == FEATURE_DSDS)
            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_StopTimer(&pstControlMagnaer->stGetSerProtectTimer);
                IMSA_StartTimer(&pstControlMagnaer->stDeregSerProtectTimer);
            }
            #endif
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnRelReq(enSrvType,
                                   IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                   IMSA_GetCurrentDomain(),
                                   enConnRlsType);

            break;
    }
}

VOS_VOID IMSA_SRV_ProcRegDeregIndConnDereging
(
    IMSA_SRV_TYPE_ENUM_UINT8            enSrvType
)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = IMSA_GetControlManagerAddress();

    IMSA_CONN_RELEASE_TYPE_ENUM_UINT8       enConnRlsType;

    /* 设置PDN释放类型 */
    if( VOS_TRUE == pstControlMagnaer->ucOrgPdnRlsFlag)
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL;
    }
    else
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_LOCAL;
    }


    IMSA_SRV_SetServcieStatus(enSrvType,
                              IMSA_SRV_STATUS_RELEASING_DEREG);

    /* 新增入参 */
    IMSA_SRV_SndCallSrvStatusInd(enSrvType,
                                IMSA_CALL_SERVICE_STATUS_NO_SERVICE,
                                IMSA_CALL_NO_SRV_CAUSE_REG_ERR,
                                IMSA_CALL_NORM_SRV_EXTEND_INFO_NONE);

    /* 如果是紧急类型，则直接通知链接模块释放承载 */
    if (IMSA_SRV_TYPE_EMC == enSrvType)
    {
        /* coverity[unchecked_value] */
        IMSA_SRV_SndConnRelReq(enSrvType,
                                IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                IMSA_GetCurrentDomain(),
                                enConnRlsType);
        return;
    }

    switch (IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
         case IMSA_RESOURCE_STATUS_ACTIVE:
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnRelReq(enSrvType,
                                   IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                   IMSA_GetCurrentDomain(),
                                   enConnRlsType);

            #if (FEATURE_ON == FEATURE_DSDS)
            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_StartTimer(&pstControlMagnaer->stDeregSerProtectTimer);
            }
            #endif
            break;
        #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESOURCE_STATUS_NULL:
            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          IMSA_FALSE,
                                          IMSA_FALSE);
            break;

        case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:
            /* 暂时不会有该原因值 */
            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          IMSA_FALSE,
                                          IMSA_FALSE);
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnRelReq(enSrvType,
                                   IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                   IMSA_GetCurrentDomain(),
                                   enConnRlsType);

            #if (FEATURE_ON == FEATURE_DSDS)
            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_StartTimer(&pstControlMagnaer->stDeregSerProtectTimer);
            }
            #endif
            break;

        case IMSA_RESOURCE_STATUS_APPLYING:
            /* 修改原因值 */
            IMSA_DsdsModifyTaskSrcByTaskType(IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                             RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
            break;
        #endif
        default:
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnRelReq(enSrvType,
                                   IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                   IMSA_GetCurrentDomain(),
                                   enConnRlsType);

            #if (FEATURE_ON == FEATURE_DSDS)
            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_StartTimer(&pstControlMagnaer->stDeregSerProtectTimer);
            }
            #endif
            break;
    }
}


VOS_VOID IMSA_SRV_ProcRegDeregIndMsg
(
    const IMSA_REG_DEREG_IND_STRU      *pstRegDeregIndMsg
)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlManager;
    pstControlManager = IMSA_GetControlManagerAddress();

    IMSA_INFO_LOG("IMSA_SRV_ProcRegDeregIndMsg is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcRegDeregIndMsg_ENUM, LNAS_ENTRY);

    /* 如果是CONN&DEREGING状态，则请求释放连接，状态转到RELEASING&DEREG态 */
    if (IMSA_TRUE == IMSA_SRV_IsSrvStatusEqual((    IMSA_SRV_TYPE_ENUM_UINT8)pstRegDeregIndMsg->enRegType,
                                                    IMSA_SRV_STATUS_CONN_DEREGING))
    {
        IMSA_INFO_LOG("IMSA_SRV_ProcRegDeregIndMsg:CONN+DEREGING!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcRegDeregIndMsg_ENUM, IMSA_STATUS_CONN);
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcRegDeregIndMsg_ENUM, IMSA_STATUS_DEREGISTERING);

        /* 目前只有MMA DEREG REG导致的去注册类型，后续增加了其他类型的去注册，
           此处需要增加考虑 */
        IMSA_SRV_ProcRegDeregIndConnDereging((    IMSA_SRV_TYPE_ENUM_UINT8)pstRegDeregIndMsg->enRegType);
        /* 判断当前是否是IMS动态关闭流程，如果是，则通知MSCC去注册成功 */
        if (IMSA_SRV_DEREG_CAUSE_MSCC_DEREG_REQ == IMSA_GetDeregCause())
        {
            /* 停止去注册等待定时器 */
            IMSA_StopTimer(&pstControlManager->stWaitDeregTimer);
            IMSA_SndMsccMsgDeregCnf();
        }

        return ;
    }

    /* 如果是CONNING&REG状态，则本地去注册，状态修改为CONNING&DEREG状态 */
    if (((IMSA_REG_TYPE_NORMAL == pstRegDeregIndMsg->enRegType)
            && (IMSA_TRUE == IMSA_SRV_IsSrvStatusEqual(IMSA_SRV_TYPE_NORMAL,IMSA_SRV_STATUS_CONNING_REG)))
        ||((IMSA_REG_TYPE_EMC == pstRegDeregIndMsg->enRegType)
            && (IMSA_TRUE == IMSA_SRV_IsSrvStatusEqual(IMSA_SRV_TYPE_EMC,IMSA_SRV_STATUS_CONNING_REG))))
    {
        IMSA_SRV_RegConningStateProcRegDeregIndMsg(pstRegDeregIndMsg->enRegType);

        return;
    }

    if (IMSA_TRUE == IMSA_SRV_IsSrvStatusEqual((    IMSA_SRV_TYPE_ENUM_UINT8)pstRegDeregIndMsg->enRegType,
                                                    IMSA_SRV_STATUS_CONN_REG))
    {
        switch (pstRegDeregIndMsg->enResultAction)
        {
            case IMSA_RESULT_ACTION_NULL:


                IMSA_SRV_SetImsRegForbidden(pstRegDeregIndMsg->enRegType, pstRegDeregIndMsg->enResult);
                IMSA_SRV_ProcRegDeregIndActionNull((IMSA_SRV_TYPE_ENUM_UINT8)pstRegDeregIndMsg->enRegType);
                break;
            case IMSA_RESULT_ACTION_REG_WITH_FIRST_ADDR_PAIR:

                IMSA_SRV_ProcRegDeregIndWithAddr((IMSA_SRV_TYPE_ENUM_UINT8)pstRegDeregIndMsg->enRegType, IMSA_REG_ADDR_PARAM_NEW);

                break;
            case IMSA_RESULT_ACTION_REG_WITH_CURRENT_ADDR_PAIR:

                IMSA_SRV_ProcRegDeregIndWithAddr((IMSA_SRV_TYPE_ENUM_UINT8)pstRegDeregIndMsg->enRegType,
                                                   IMSA_REG_ADDR_PARAM_SAME);

                break;
            case IMSA_RESULT_ACTION_REG_WITH_NEXT_ADDR_PAIR:

                IMSA_SRV_ProcRegDeregIndWithAddr((IMSA_SRV_TYPE_ENUM_UINT8)pstRegDeregIndMsg->enRegType,
                                                   IMSA_REG_ADDR_PARAM_NEXT);

                break;
            case IMSA_RESULT_ACTION_REG_LOCAL_DEREG:
                IMSA_SRV_SetServcieStatus(  (IMSA_SRV_TYPE_ENUM_UINT8)pstRegDeregIndMsg->enRegType,
                                            IMSA_SRV_STATUS_CONN_DEREG);

                /* 切离CONN&REG态，因此通知CALL呼叫服务进入无服务状态 */
                /* 新增入参 */
                IMSA_SRV_SndCallSrvStatusInd((IMSA_SRV_TYPE_ENUM_UINT8)pstRegDeregIndMsg->enRegType,
                                              IMSA_CALL_SERVICE_STATUS_NO_SERVICE,
                                              IMSA_CALL_NO_SRV_CAUSE_SIP_PDP_ERR,
                                              IMSA_CALL_NORM_SRV_EXTEND_INFO_NONE);
                break;
            case IMSA_RESULT_ACTION_WAIT_RETRY_TIME_OUT:

                IMSA_SRV_SetServcieStatus(  (IMSA_SRV_TYPE_ENUM_UINT8)pstRegDeregIndMsg->enRegType,
                                            IMSA_SRV_STATUS_CONN_REGING);

                /* 切离CONN&REG态，因此通知CALL呼叫服务进入无服务状态 */
                /* 新增入参 */
                IMSA_SRV_SndCallSrvStatusInd((IMSA_SRV_TYPE_ENUM_UINT8)pstRegDeregIndMsg->enRegType,
                                              IMSA_CALL_SERVICE_STATUS_NO_SERVICE,
                                              IMSA_CALL_NO_SRV_CAUSE_REG_ERR,
                                              IMSA_CALL_NORM_SRV_EXTEND_INFO_NONE);
                break;
            case IMSA_RESULT_ACTION_REG_RESTORATION:
                IMSA_SRV_ProcRegDeregIndWithAddr((IMSA_SRV_TYPE_ENUM_UINT8)pstRegDeregIndMsg->enRegType,
                                                   IMSA_REG_ADDR_PARAM_RESTORATION);
                break;
            default:

                IMSA_INFO_LOG("IMSA_SRV_ProcRegDeregIndMsg:illegal result action!");
                TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcRegDeregIndMsg_ENUM, LNAS_PARAM_INVALID);

                IMSA_SRV_ProcRegDeregIndActionNull((IMSA_SRV_TYPE_ENUM_UINT8)pstRegDeregIndMsg->enRegType);

                break;
        }
    }

    return ;
}


VOS_VOID IMSA_SRV_ProcConnSetupIndMsg
(
    const IMSA_CONN_SETUP_IND_STRU     *pstConnSetupIndMsg
)
{
    if (IMSA_CONN_RESULT_SUCCESS == pstConnSetupIndMsg->enResult)
    {
        if (IMSA_IMS_DOMAIN_LTE == pstConnSetupIndMsg->enConnRatType)
        {
            IMSA_SRV_ProcConnSetupIndSucc(pstConnSetupIndMsg);
        }
        else if (IMSA_IMS_DOMAIN_WIFI == pstConnSetupIndMsg->enConnRatType)
        {
            IMSA_SRV_ProcWifiConnSetupIndSucc(pstConnSetupIndMsg);
        }
        else
        {
        }
    }
    else
    {
        IMSA_SRV_ProcConnSetupIndFail(pstConnSetupIndMsg);
    }
}


VOS_VOID IMSA_SRV_ProcEmcConnRelIndMsg
(
    IMSA_CONN_SIP_PDP_TYPE_ENUM_UINT32      enSipPdpType
)
{
    IMSA_CONTROL_MANAGER_STRU      *pstControlManager;

    pstControlManager = IMSA_GetControlManagerAddress();

    if (IMSA_CONN_SIP_PDP_TYPE_MEDIA == enSipPdpType)
    {
        IMSA_INFO_LOG("IMSA_SRV_ProcEmcConnRelIndMsg:emc dedicated release!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcEmcConnRelIndMsg_ENUM, 1);

        /* 通知CALL模块媒体承载释放 */
        IMSA_ProcCallResourceIsNotReady(IMSA_CONN_TYPE_EMC);

        /* 按照IR92的需求，网侧释放承载后，应该由网侧去挂断电话，但是可能由于网侧异常，导致UE未
        收到网侧的挂断消息，所以在定时器超时后，由UE主动挂断电话 */
        if ((IMSA_FALSE == pstControlManager->ucMediaLostNotRelCallFlag) &&
            (IMSA_FALSE == IMSA_CONN_HasEmcActiveVoicePdp()))
        {
            /* 如果只是视频承载被释放，则不需要通知CALL模块；如果还有语音承载存在，也不需要通知CALL
            模块 */
            IMSA_SRV_SndEmcCallSrvStatusInd(IMSA_CALL_SERVICE_STATUS_NO_SERVICE,
                                            IMSA_CALL_NO_SRV_CAUSE_MEDIA_PDP_RELEASE);
        }
        return ;
    }

    IMSA_INFO_LOG("IMSA_SRV_ProcEmcConnRelIndMsg:emc default release!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcEmcConnRelIndMsg_ENUM, 2);

    /* 紧急注册状态，收到网侧发送的释放SIP紧急承载后的处理 */
    if (IMSA_SRV_STATUS_CONN_REG == IMSA_SRV_GetEmcSrvStatus())
    {
        /* delete PeriodImsEmcSrvTryTimer */
        IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_EMC, IMSA_SRV_STATUS_IDLE_DEREG);

        /* 通知IMSA本地去注册 */
        IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_EMC, IMSA_DEREG_LOCAL);

        /* 通知CALL模块紧急呼叫服务进入无服务状态，促使CALL模块清除缓存的紧急呼 */
        IMSA_SRV_SndEmcCallSrvStatusInd(IMSA_CALL_SERVICE_STATUS_NO_SERVICE,
                                        IMSA_CALL_NO_SRV_CAUSE_SIP_PDP_ERR);

        return;
    }
    if (IMSA_SRV_STATUS_CONNING_REG == IMSA_SRV_GetEmcSrvStatus())
    {
        /* 切离CONNING+REG态时，IMSA需要通知IMS允许发包 */
        IMSA_SndImsMsgServiceSuspendOrResumeSrvInfo(IMSA_IMS_INPUT_SERVICE_REASON_RESUME_EMC_SRV);

        IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_EMC, IMSA_SRV_STATUS_IDLE_DEREG);

        /* 通知IMSA本地去注册 */
        IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_EMC, IMSA_DEREG_LOCAL);

        return;
    }

    /* 如果是以下三种状态，则通知REG模块本地去注册 */
    if (IMSA_SRV_STATUS_CONN_REGING == IMSA_SRV_GetEmcSrvStatus())
    {
        IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_EMC, IMSA_DEREG_LOCAL);

        /* 需要通知CALL模块紧急呼叫服务进入无服务状态，促使其释放缓存的紧急呼 */
        IMSA_SRV_SndEmcCallSrvStatusInd(IMSA_CALL_SERVICE_STATUS_NO_SERVICE,
                                        IMSA_CALL_NO_SRV_CAUSE_SIP_PDP_ERR);
    }

    if (IMSA_SRV_STATUS_CONN_DEREGING == IMSA_SRV_GetEmcSrvStatus())
    {
        IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_EMC, IMSA_DEREG_LOCAL);

        /* 由于已经进行本地去注册，无需通知CALL模块呼叫服务状态进入无服务，
           否则本地去注册流程和释放CALL流程同时进行，容易出问题 */
    }

    IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_EMC, IMSA_SRV_STATUS_IDLE_DEREG);
}


VOS_VOID IMSA_SRV_ProcNormalConnSigalBearerRelIndMsg( VOS_VOID )
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_SRV_ProcNormalConnSigalBearerRelIndMsg is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcNormalConnSigalBearerRelIndMsg_ENUM, LNAS_ENTRY);

    /* 进入DETACHED态的连接释放(本地DETACH或者MT DETACH导致的)，IMSA不应该重建链路，
       而保持ATTACHED态的连接释放，IMSA应该重建链路，IMSA通过PS域服务状态是否是
       正常服务来区分，但为了避免系统的时序问题(即APS通知连接释放在前，MMA通知
       PS域服务状态在后)，导致了IMSA判定错误，IMSA不马上判断PS域服务状态，先启动
       周期性尝试IMS服务定时器，待定时器超时后再判定建立连接的条件是否具备 */
    pstControlMagnaer  = IMSA_GetControlManagerAddress();
    if (IMSA_SRV_STATUS_CONN_REG == IMSA_SRV_GetNormalSrvStatus())
    {
        IMSA_INFO_LOG("IMSA_SRV_ProcNormalConnSigalBearerRelIndMsg:conn&reg!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcNormalConnSigalBearerRelIndMsg_ENUM, IMSA_STATUS_CONN);
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcNormalConnSigalBearerRelIndMsg_ENUM, IMSA_STATUS_REGISTERED);
#if 0
        if (IMSA_IMS_DOMAIN_LTE == IMSA_GetCurrentDomain())
        {
#endif
        /* SIP信令承载释放后，不再本地去注册，IP地址发生变更或者IMS周期性注册定时器超时后，
        重新发起IMS注册 */
        /* 需要通知IMS不允许发包 */
        IMSA_SndImsMsgServiceSuspendOrResumeSrvInfo(IMSA_IMS_INPUT_SERVICE_REASON_SUSPEND_NRM_SRV);

        /*IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);*/
        IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_CONNING_REG);

        /* 启动周期性尝试IMS服务定时器 */
        IMSA_StartTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer);

        /* 通知Domain连接异常 */
        if( ( IMSA_IMS_DOMAIN_WIFI == IMSA_GetCurrentDomain())
            ||(IMSA_IMS_DOMAIN_LTE == IMSA_GetCurrentDomain()))
        {
            /* coverity[unchecked_value] */
            IMSA_SRV_SndDomainBearerRelnd(IMSA_GetCurrentDomain());
        }

        /* 通知CALL模块普通呼叫服务进入无服务状态 */
        /* 新增入参 */
        IMSA_SRV_SndNrmCallSrvStatusInd(IMSA_CALL_SERVICE_STATUS_NO_SERVICE,
                                      IMSA_CALL_NO_SRV_CAUSE_SIP_PDP_ERR,
                                      IMSA_CALL_NORM_SRV_EXTEND_INFO_NONE);
#if 0
        }
        else if (IMSA_IMS_DOMAIN_WIFI == IMSA_GetCurrentDomain())
        {
            /* vowifi下不进行承载重建，直接去激活 */
            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);

            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_IDLE_DEREG);

            /* 不通知CALL模块普通呼叫服务进入无服务状态，由IMS自己挂断电话 */
            IMSA_SRV_SndDomainSrvStatusInd(IMSA_DOMAIN_SERVICE_STATUS_NO_SERVICE,
                                            IMSA_DOMAIN_NO_SRV_CAUSE_SIP_PDP_ERR);
        }
        else
        {
        }
#endif
        return ;
    }

    if (IMSA_SRV_STATUS_CONNING_REG == IMSA_SRV_GetNormalSrvStatus())
    {
        /* 如果定时器没有再运行，则转到IDLE+DEREG状态，本地去注册 */
        if (IMSA_TRUE != IMSA_IsTimerRunning(&pstControlMagnaer->stPeriodImsSrvTryTimer))
        {
            /* 切离CONNING+REG态时，IMSA需要通知IMS允许发包 */
            IMSA_SndImsMsgServiceSuspendOrResumeSrvInfo(IMSA_IMS_INPUT_SERVICE_REASON_RESUME_NRM_SRV);

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);

            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_IDLE_DEREG);
            /* coverity[unchecked_value] */
            IMSA_SRV_SndDomainSrvStatusInd(IMSA_DOMAIN_SERVICE_STATUS_NO_SERVICE,
                                            IMSA_DOMAIN_NO_SRV_CAUSE_SIP_PDP_ERR,
                                            IMSA_GetCurrentDomain());

            /* 上报状态给Domain，Domain会发起域选过程 */
            if( VOS_TRUE == pstControlMagnaer->ucDomainRegSrvStatusFlag)
            {
                pstControlMagnaer->ucDomainRegSrvStatusFlag = VOS_FALSE;
                IMSA_SRV_SndDomainSrvReadyInd();
            }
            /* 启动周期性尝试IMS服务定时器 */
            IMSA_StartTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer);

            /* 需要释放资源 */
            #if (FEATURE_ON == FEATURE_DSDS)
            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_StopTimer(&pstControlMagnaer->stGetSerProtectTimer);
                IMSA_DsdsProcResourceRelease(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
            }
            #endif
        }
        return;
    }

    if (IMSA_SRV_STATUS_CONN_REGING == IMSA_SRV_GetNormalSrvStatus())
    {
        IMSA_INFO_LOG("IMSA_SRV_ProcNormalConnSigalBearerRelIndMsg:conn&reging!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcNormalConnSigalBearerRelIndMsg_ENUM, IMSA_STATUS_CONN);
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcNormalConnSigalBearerRelIndMsg_ENUM, IMSA_STATUS_REGISTERING);

        IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);

        /* 启动周期性尝试IMS服务定时器 */
        IMSA_StartTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer);

        /* 状态转到IDLE&DEREG态，由于IMSA_SRV_SetServcieStatus中会用到周期性
           尝试IMS服务定时器的运行情况，且当前在注册过程中，所以需要先启动定时器后转状态 */
        IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_IDLE_DEREG);

        /* 通知Domain连接异常 */
        if( ( IMSA_IMS_DOMAIN_WIFI == IMSA_GetCurrentDomain())
            ||(IMSA_IMS_DOMAIN_LTE == IMSA_GetCurrentDomain()))
        {
            /* coverity[unchecked_value] */
            IMSA_SRV_SndDomainBearerRelnd(IMSA_GetCurrentDomain());
        }


        /* 由于已经进行本地去注册，无需通知CALL模块呼叫服务状态进入无服务，
           否则本地去注册流程和释放CALL流程同时进行，容易出问题 */

        #if (FEATURE_ON == FEATURE_DSDS)
        if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
        {
            IMSA_StopTimer(&pstControlMagnaer->stGetSerProtectTimer);
            IMSA_DsdsProcResourceRelease(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
        }
        #endif
        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_RegFailEventReport(IMSA_ERR_LOG_REG_FAIL_REASON_SIP_PDP_RELEASE,
                                IMSA_FALSE,
                                IMSA_FALSE,
                                IMSA_FALSE);
        #endif
        return ;
    }

    if (IMSA_SRV_STATUS_CONN_DEREG == IMSA_SRV_GetNormalSrvStatus())
    {
        IMSA_INFO_LOG("IMSA_SRV_ProcNormalConnSigalBearerRelIndMsg:conn&dereg!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcNormalConnSigalBearerRelIndMsg_ENUM, IMSA_STATUS_CONN);
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcNormalConnSigalBearerRelIndMsg_ENUM, IMSA_STATUS_NOT_REGISTER);

        IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_IDLE_DEREG);

        /* 启动周期性尝试IMS服务定时器 */
        IMSA_StartTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer);


        /* 通知Domain连接异常 */
        if( ( IMSA_IMS_DOMAIN_WIFI == IMSA_GetCurrentDomain())
            ||(IMSA_IMS_DOMAIN_LTE == IMSA_GetCurrentDomain()))
        {
            /* coverity[unchecked_value] */
            IMSA_SRV_SndDomainBearerRelnd(IMSA_GetCurrentDomain());
        }

        #if (FEATURE_ON == FEATURE_DSDS)
        if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
        {
            IMSA_StopTimer(&pstControlMagnaer->stGetSerProtectTimer);
            IMSA_DsdsProcResourceRelease(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
        }
        #endif

        return ;
    }

    if ((IMSA_SRV_STATUS_RELEASING_DEREG == IMSA_SRV_GetNormalSrvStatus())
        || (IMSA_SRV_STATUS_CONNING_DEREG == IMSA_SRV_GetNormalSrvStatus()))
    {
        IMSA_INFO_LOG("IMSA_SRV_ProcNormalConnSigalBearerRelIndMsg:releasing&dereg or conning&dereg!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcNormalConnSigalBearerRelIndMsg_ENUM, IMSA_STATUS_RELEASING);
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcNormalConnSigalBearerRelIndMsg_ENUM, IMSA_STATUS_CONNECTING);
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcNormalConnSigalBearerRelIndMsg_ENUM, IMSA_STATUS_NOT_REGISTER);

        IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_IDLE_DEREG);
        /* coverity[unchecked_value] */
        IMSA_SRV_SndDomainSrvStatusInd(IMSA_DOMAIN_SERVICE_STATUS_NO_SERVICE,
                                            IMSA_DOMAIN_NO_SRV_CAUSE_SIP_PDP_ERR,
                                            IMSA_GetCurrentDomain());

        /* 上报状态给Domain，Domain会发起域选过程 */
        if( VOS_TRUE == pstControlMagnaer->ucDomainRegSrvStatusFlag)
        {
            pstControlMagnaer->ucDomainRegSrvStatusFlag = VOS_FALSE;
            IMSA_SRV_SndDomainSrvReadyInd();
        }

        #if (FEATURE_ON == FEATURE_DSDS)
        if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
        {
            IMSA_StopTimer(&pstControlMagnaer->stGetSerProtectTimer);
            IMSA_DsdsProcResourceRelease(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
        }
        #endif

        return ;
    }

    if (IMSA_SRV_STATUS_CONN_DEREGING == IMSA_SRV_GetNormalSrvStatus())
    {
        IMSA_INFO_LOG("IMSA_SRV_ProcNormalConnSigalBearerRelIndMsg:conn&dereging!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcNormalConnSigalBearerRelIndMsg_ENUM, IMSA_STATUS_CONN);
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcNormalConnSigalBearerRelIndMsg_ENUM, IMSA_STATUS_DEREGISTERING);

        IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_IDLE_DEREG);

        IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);
        /* coverity[unchecked_value] */
        IMSA_SRV_SndDomainSrvStatusInd(IMSA_DOMAIN_SERVICE_STATUS_NO_SERVICE,
                                            IMSA_DOMAIN_NO_SRV_CAUSE_SIP_PDP_ERR,
                                            IMSA_GetCurrentDomain());

        /* 上报状态给Domain，Domain会发起域选过程 */
        if( VOS_TRUE == pstControlMagnaer->ucDomainRegSrvStatusFlag)
        {
            pstControlMagnaer->ucDomainRegSrvStatusFlag = VOS_FALSE;
            IMSA_SRV_SndDomainSrvReadyInd();
        }

        #if (FEATURE_ON == FEATURE_DSDS)
        if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
        {
            IMSA_StopTimer(&pstControlMagnaer->stGetSerProtectTimer);
            IMSA_DsdsProcResourceRelease(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
        }
        #endif

        return ;
    }

    return ;
}



VOS_VOID IMSA_SRV_ProcConnRelIndMsg
(
    const IMSA_CONN_REL_IND_STRU       *pstConnRelIndMsg
)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_SRV_ProcConnRelIndMsg is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcConnRelIndMsg_ENUM, LNAS_ENTRY);

    pstControlMagnaer  = IMSA_GetControlManagerAddress();

    if (IMSA_CONN_TYPE_EMC == pstConnRelIndMsg->enConnType)
    {
        IMSA_SRV_ProcEmcConnRelIndMsg(pstConnRelIndMsg->enSipPdpType);

        return ;
    }

    /* 普通连接分支 */

    /* 清除释放标志 */
    pstControlMagnaer->ucOrgPdnRlsFlag = VOS_TRUE;

    if (IMSA_CONN_SIP_PDP_TYPE_MEDIA == pstConnRelIndMsg->enSipPdpType)
    {
        IMSA_INFO_LOG("IMSA_SRV_ProcConnRelIndMsg:normal media pdp release!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcConnRelIndMsg_ENUM, 2);

        if (IMSA_IMS_DOMAIN_LTE == pstConnRelIndMsg->enConnRatType)
        {

            /* 通知CALL模块媒体承载释放 */
            IMSA_ProcCallResourceIsNotReady(IMSA_CONN_TYPE_NORMAL);

            /* 通知MSCC，当前承载状态 */
            if (IMSA_SRV_STATUS_CONN_REG == IMSA_SRV_GetNormalSrvStatus())
            {
                IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_AVAILABLE,IMSA_CONN_IsExitPersistentBearer());
            }
            /* 按照IR92的需求，网侧释放承载后，应该由网侧去挂断电话，但是可能由于网侧异常，导致UE未
            收到网侧的挂断消息，所以在定时器超时后，由UE主动挂断电话 */
            if ((IMSA_FALSE == pstControlMagnaer->ucMediaLostNotRelCallFlag) &&
                (IMSA_FALSE == IMSA_CONN_HasActiveVoicePdp()))
            {
                /* 如果只是视频承载被释放，则不需要通知CALL模块；如果还有语音承载存在，也不需要通知CALL
                模块 */
                /* 增加入参 */
                IMSA_SRV_SndNrmCallSrvStatusInd(IMSA_CALL_SERVICE_STATUS_NO_SERVICE,
                                                IMSA_CALL_NO_SRV_CAUSE_MEDIA_PDP_RELEASE,
                                                IMSA_CALL_NORM_SRV_EXTEND_INFO_NONE);
            }
            else if((IMSA_TRUE == IMSA_CONN_HasActiveVoicePdp())
                     && (IMSA_CONN_MEDIA_PDP_TYPE_VIDEO == pstConnRelIndMsg->enRelMediaPdpType))
            {
                /* 通知CALL模块, 只是视频承载被释放 */
                IMSA_SRV_SndNrmCallSrvStatusInd(IMSA_CALL_SERVICE_STATUS_NORMAL_SERVICE,
                                                IMSA_CALL_NO_SRV_CAUSE_BUTT,
                                                IMSA_CALL_NORM_SRV_EXTEND_INFO_VIDEO_PDP_LOST);
            }
            else
            {
            }
        }
        else if(IMSA_IMS_DOMAIN_WIFI == pstConnRelIndMsg->enConnRatType)
        {
            /* 如果是WIFI下，没有实际的媒体承载，但是如果端口号绑定失败时，需要释放所有call  */
            /* 通知CALL模块普通呼叫服务进入无服务状态 */
            /* 增加入参 */
            IMSA_SRV_SndNrmCallSrvStatusInd(IMSA_CALL_SERVICE_STATUS_NO_SERVICE,
                                            IMSA_CALL_NO_SRV_CAUSE_SIP_PDP_ERR,
                                            IMSA_CALL_NORM_SRV_EXTEND_INFO_NONE);
        }

    }
    else
    {
        /* 处于切换过程中的REL_IND，不进行处理 */
        if( (IMSA_SRV_STATUS_HO_DEREG == IMSA_SRV_GetNormalSrvStatus())
            ||(IMSA_SRV_STATUS_HO_REG == IMSA_SRV_GetNormalSrvStatus()))
        {
            return;
        }

        /* 释放消息上报的接入技术和当前域的接入技术不同的话，丢弃不处理这个释放消息 */
        if( ( IMSA_IMS_DOMAIN_NULL != IMSA_GetCurrentDomain())
            &&( IMSA_GetCurrentDomain()!= pstConnRelIndMsg->enConnRatType ))
        {
           return;
        }

        IMSA_SRV_ProcNormalConnSigalBearerRelIndMsg();
    }
}


VOS_VOID IMSA_SRV_ProcNormalConnSignalBearerModifyIndConnReging
(
    VOS_VOID
)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = IMSA_GetControlManagerAddress();
    IMSA_CONN_RELEASE_TYPE_ENUM_UINT8       enConnRlsType;

    IMSA_INFO_LOG("IMSA_SRV_ProcNormalConnSignalBearerModifyIndMsg:conn&reging!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcNormalConnSignalBearerModifyIndMsg_ENUM, IMSA_STATUS_CONN);
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcNormalConnSignalBearerModifyIndMsg_ENUM, IMSA_STATUS_REGISTERING);

    /* 设置PDN释放类型 */
    if( VOS_TRUE == pstControlMagnaer->ucOrgPdnRlsFlag)
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL;
    }
    else
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_LOCAL;
    }

    /* 如果是DSDS流程，则需要先申请资源 */
    switch (IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        case IMSA_RESOURCE_STATUS_ACTIVE:

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnRelReq(IMSA_CONN_TYPE_NORMAL,
                                   IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                   IMSA_GetCurrentDomain(),
                                   enConnRlsType);


            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_RELEASING_DEREG);
            #if (FEATURE_ON == FEATURE_DSDS)
            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_StopTimer(&pstControlMagnaer->stGetSerProtectTimer);
                IMSA_StartTimer(&pstControlMagnaer->stDeregSerProtectTimer);
            }
            #endif

            /* 启动周期性尝试IMS服务定时器 */
            IMSA_StartTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer);
            break;
        #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESOURCE_STATUS_APPLYING:

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);

            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_RELEASING_DEREG);

            IMSA_DsdsModifyTaskSrcByTaskType(IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                             RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);

            #if (FEATURE_ON == FEATURE_PTM)
            IMSA_RegFailEventReport(IMSA_ERR_LOG_REG_FAIL_REASON_SIP_PDP_RELEASE,
                                    IMSA_FALSE,
                                    IMSA_FALSE,
                                    IMSA_FALSE);
            #endif
            break;

        /* 以下三个状态应该都不会有 */
        case IMSA_RESOURCE_STATUS_NULL:
            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          IMSA_FALSE,
                                          IMSA_FALSE);
            break;

        case IMSA_RESOURCE_STATUS_WAIT_STATUS_IND:

            break;

        case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:

            break;
        #endif
        default:
            break;
    }

    #if (FEATURE_ON == FEATURE_PTM)
    IMSA_RegFailEventReport(IMSA_ERR_LOG_REG_FAIL_REASON_SIP_PDP_RELEASE,
                            IMSA_FALSE,
                            IMSA_FALSE,
                            IMSA_FALSE);
    #endif
}


VOS_VOID IMSA_SRV_ProcNormalConnSignalBearerModifyIndConnDereg
(
    VOS_VOID
)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = IMSA_GetControlManagerAddress();
    IMSA_CONN_RELEASE_TYPE_ENUM_UINT8       enConnRlsType;

    IMSA_INFO_LOG("IMSA_SRV_ProcNormalConnSignalBearerModifyIndMsg:conn&reging!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcNormalConnSignalBearerModifyIndMsg_ENUM, IMSA_STATUS_CONN);
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcNormalConnSignalBearerModifyIndMsg_ENUM, IMSA_STATUS_REGISTERING);

    /* 设置PDN释放类型 */
    if( VOS_TRUE == pstControlMagnaer->ucOrgPdnRlsFlag)
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL;
    }
    else
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_LOCAL;
    }

    /* 如果是DSDS流程，则需要先申请资源 */
    switch (IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        case IMSA_RESOURCE_STATUS_ACTIVE:
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnRelReq(IMSA_CONN_TYPE_NORMAL,
                                   IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                   IMSA_GetCurrentDomain(),
                                   enConnRlsType);

            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_RELEASING_DEREG);

            /* 启动周期性尝试IMS服务定时器 */
            IMSA_StartTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer);
            #if (FEATURE_ON == FEATURE_DSDS)
            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_StopTimer(&pstControlMagnaer->stGetSerProtectTimer);
                IMSA_StartTimer(&pstControlMagnaer->stDeregSerProtectTimer);
            }
            #endif

            break;
        #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESOURCE_STATUS_APPLYING:

            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_RELEASING_DEREG);

            IMSA_DsdsModifyTaskSrcByTaskType(IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                             RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);

            /* 启动周期性尝试IMS服务定时器 */
            IMSA_StartTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer);

            break;

        case IMSA_RESOURCE_STATUS_NULL:
            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          IMSA_FALSE,
                                          IMSA_FALSE);
            break;

        case IMSA_RESOURCE_STATUS_WAIT_STATUS_IND:
            /* 有可能处于该状态，等待消息，重新发注册 */
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_RELEASING_DEREG);

            IMSA_DsdsSndRrmDeRegisterInd(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          IMSA_FALSE,
                                          IMSA_FALSE);

            /* 启动周期性尝试IMS服务定时器 */
            IMSA_StartTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer);
            break;

        case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:

            break;
        #endif
        default:
            break;
    }
}

VOS_VOID IMSA_SRV_ProcNormalConnSignalBearerModifyIndConnDereging
(
    VOS_VOID
)
{
    IMSA_CONTROL_MANAGER_STRU              *pstControlMagnaer   = VOS_NULL_PTR;
    IMSA_CONN_RELEASE_TYPE_ENUM_UINT8       enConnRlsType;

    IMSA_INFO_LOG("IMSA_SRV_ProcNormalConnSignalBearerModifyIndMsg:conn&reging!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcNormalConnSignalBearerModifyIndMsg_ENUM, IMSA_STATUS_CONN);
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcNormalConnSignalBearerModifyIndMsg_ENUM, IMSA_STATUS_REGISTERING);

    pstControlMagnaer  = IMSA_GetControlManagerAddress();

    /* 设置PDN释放类型 */
    if( VOS_TRUE == pstControlMagnaer->ucOrgPdnRlsFlag)
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL;
    }
    else
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_LOCAL;
    }

    /* 如果是DSDS流程，则需要先申请资源 */
    switch (IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        case IMSA_RESOURCE_STATUS_ACTIVE:

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnRelReq(IMSA_CONN_TYPE_NORMAL,
                                   IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                   IMSA_GetCurrentDomain(),
                                   enConnRlsType);

            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_RELEASING_DEREG);

            break;
        #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESOURCE_STATUS_APPLYING:

            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_RELEASING_DEREG);

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);

            break;
        /* 以下三个状态不应该有 */
        case IMSA_RESOURCE_STATUS_NULL:
            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          IMSA_FALSE,
                                          IMSA_FALSE);
            break;

        case IMSA_RESOURCE_STATUS_WAIT_STATUS_IND:
            break;

        case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:

            break;
        #endif
        default:
            break;
    }
}

VOS_VOID IMSA_SRV_ProcNormalConnSignalBearerModifyPcscfInvalid
(
    VOS_VOID
)
{
    #if (FEATURE_ON == FEATURE_DSDS)
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = IMSA_GetControlManagerAddress();
    #endif

    #if (FEATURE_ON == FEATURE_PTM)
    if (IMSA_SRV_STATUS_CONN_REGING == IMSA_SRV_GetNormalSrvStatus())
    {
        IMSA_RegFailEventReport(IMSA_ERR_LOG_REG_FAIL_REASON_SIP_PDP_RELEASE,
                                IMSA_FALSE,
                                IMSA_FALSE,
                                IMSA_FALSE);
    }
    else
    {
        IMSA_SRV_InitDeregEventReport(IMSA_ERR_LOG_INIT_DEREG_REASON_PDN_LOST);

        /* 记录UE发起注册的时间 */
        ( VOS_VOID )LPS_OM_GET32K_TIMESTAMP(&(IMSA_GetErrlogStartRegTimerAddr()->ul32KTStampHighBit),
                                            &(IMSA_GetErrlogStartRegTimerAddr()->ul32KTStampLowBit));
    }
    #endif

    if ((IMSA_SRV_STATUS_CONN_REGING == IMSA_SRV_GetNormalSrvStatus()) ||
        (IMSA_SRV_STATUS_CONN_REG == IMSA_SRV_GetNormalSrvStatus()))
    {
        switch (IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
        {
            case IMSA_RESOURCE_STATUS_ACTIVE:

                IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_CONN_REGING);
                IMSA_SRV_SndRegDeregReq(IMSA_SRV_TYPE_NORMAL, IMSA_DEREG_LOCAL);
                IMSA_SRV_SndRegRegReq(IMSA_SRV_TYPE_NORMAL, IMSA_REG_ADDR_PARAM_NEW);

                #if (FEATURE_ON == FEATURE_DSDS)
                if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
                {
                    IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
                }
                #endif

                break;
            #if (FEATURE_ON == FEATURE_DSDS)
            case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:

                IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                              IMSA_RF_RES_APPLY_TRIGER_SRC_GET_NOR_SRV,
                                              VOS_FALSE,
                                              VOS_FALSE);

                IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_CONN_REGING);
                IMSA_SRV_SndRegDeregReq(IMSA_SRV_TYPE_NORMAL, IMSA_DEREG_LOCAL);
                IMSA_SRV_SndRegRegReq(IMSA_SRV_TYPE_NORMAL, IMSA_REG_ADDR_PARAM_NEW);

                if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
                {
                    IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
                }
                break;

            case  IMSA_RESOURCE_STATUS_NULL:

                IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                              IMSA_RF_RES_APPLY_TRIGER_SRC_GET_NOR_SRV,
                                              IMSA_REG_ADDR_PARAM_NEW,
                                              VOS_FALSE);

                IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_CONN_REGING);
                IMSA_SRV_SndRegDeregReq(IMSA_SRV_TYPE_NORMAL, IMSA_DEREG_LOCAL);
                break;
            #endif
            default:
                #if (FEATURE_ON == FEATURE_DSDS)
                IMSA_DsdsModifyTaskSrcByTaskType(IMSA_RF_RES_APPLY_TRIGER_SRC_GET_NOR_SRV,
                                                 RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
                #endif
                break;

        }
    }
}


VOS_VOID IMSA_SRV_ProcNormalConnSignalBearerModifyRegParaInvalid
(
    VOS_VOID
)
{

    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = IMSA_GetControlManagerAddress();

    /* 如果在CONN&REGIN或者GCONN&REG态，则请求REG模块本地去注册，
       请求CONN模块释放连接，状态转到RELEASING&DEREG态，启动周期性尝试IMS
       服务定时器 */
    if (IMSA_SRV_STATUS_CONN_REGING == IMSA_SRV_GetNormalSrvStatus())
    {
        IMSA_SRV_ProcNormalConnSignalBearerModifyIndConnReging();
        return ;
    }

    if (IMSA_SRV_STATUS_CONN_REG == IMSA_SRV_GetNormalSrvStatus())
    {
        IMSA_INFO_LOG("IMSA_SRV_ProcNormalConnSignalBearerModifyIndMsg:conn&reg!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcNormalConnSignalBearerModifyIndMsg_ENUM, IMSA_STATUS_CONN);
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcNormalConnSignalBearerModifyIndMsg_ENUM, IMSA_STATUS_REGISTERED);

        /* 需要通知IMS不允许发包 */
        IMSA_SndImsMsgServiceSuspendOrResumeSrvInfo(IMSA_IMS_INPUT_SERVICE_REASON_SUSPEND_NRM_SRV);

        IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_CONNING_REG);

        /* 启动周期性尝试IMS服务定时器 */
        IMSA_StartTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer);

        return;
    }

    /* 如果在CONN&DEREGING态，则请求REG模块本地去注册，请求CONN模块释放连接，
       状态转到RELEASING&DEREG态 */
    if (IMSA_SRV_STATUS_CONN_DEREGING == IMSA_SRV_GetNormalSrvStatus())
    {
        IMSA_INFO_LOG("IMSA_SRV_ProcNormalConnSignalBearerModifyIndMsg:conn&dereging!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcNormalConnSignalBearerModifyIndMsg_ENUM, IMSA_STATUS_CONN);
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcNormalConnSignalBearerModifyIndMsg_ENUM, IMSA_STATUS_DEREGISTERING);

        IMSA_SRV_ProcNormalConnSignalBearerModifyIndConnDereging();
        return ;
    }

    /* 如果在CONN&DEREG态，则请求CONN模块释放连接，状态转到RELEASING&DEREG态,
       启动周期性尝试IMS服务定时器 */
    if (IMSA_SRV_STATUS_CONN_DEREG == IMSA_SRV_GetNormalSrvStatus())
    {
        IMSA_INFO_LOG("IMSA_SRV_ProcNormalConnSignalBearerModifyIndMsg:conn&dereg!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcNormalConnSignalBearerModifyIndMsg_ENUM, IMSA_STATUS_CONN);
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcNormalConnSignalBearerModifyIndMsg_ENUM, IMSA_STATUS_NOT_REGISTER);

        IMSA_SRV_ProcNormalConnSignalBearerModifyIndConnDereg();
        return ;
    }
}


VOS_VOID IMSA_SRV_ProcNormalConnSignalBearerModifyIndMsg
(
    const IMSA_CONN_MODIFY_IND_STRU    *pstConnModifyIndMsg
)
{
    /*IMSA_CONN_RELEASE_TYPE_ENUM_UINT8        enConnRlsType;*/

    IMSA_INFO_LOG("IMSA_SRV_ProcNormalConnSignalBearerModifyIndMsg is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcNormalConnSignalBearerModifyIndMsg_ENUM, LNAS_ENTRY);


    /* 设置PDN释放类型 */
    /*
    if( VOS_TRUE == pstControlMagnaer->ucOrgPdnRlsFlag)
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL;
    }
    else
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_LOCAL;
    }*/
    /* 如果不是注册参数无效，则直接退出 */
    if (IMSA_OP_TRUE != pstConnModifyIndMsg->bitOpModifyType)
    {
        IMSA_INFO_LOG("IMSA_SRV_ProcNormalConnSignalBearerModifyIndMsg:normal,not reg para invalid!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcNormalConnSignalBearerModifyIndMsg_ENUM, 2);

        return ;
    }

    /* 如果紧急连接，且注册P-CSCF地址失效，则重新发起注册；否则直接退出  */
    if (IMSA_CONN_TYPE_EMC == pstConnModifyIndMsg->enConnType)
    {
        if (IMSA_CONN_MODIFY_TYPE_PCSCF_INVALID == pstConnModifyIndMsg->enModifyType)
        {
            /* 需要判断注册状态 */
            if ((IMSA_SRV_STATUS_CONN_REG == IMSA_SRV_GetEmcSrvStatus()) ||
                (IMSA_SRV_STATUS_CONN_REGING == IMSA_SRV_GetEmcSrvStatus()))
            {
                IMSA_SRV_SetServcieStatus(IMSA_REG_TYPE_EMC, IMSA_SRV_STATUS_CONN_REGING);
                IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_EMC, IMSA_DEREG_LOCAL);
                IMSA_SRV_SndRegRegReq(IMSA_REG_TYPE_EMC, IMSA_REG_ADDR_PARAM_NEW);
            }
        }
        return ;
    }

    /* 注册参数失效 */
    if (IMSA_CONN_MODIFY_TYPE_REG_PARA_INVALID == pstConnModifyIndMsg->enModifyType)
    {
        IMSA_SRV_ProcNormalConnSignalBearerModifyRegParaInvalid();

    }
    else /* P-CSCF地址失效 */
    {
        IMSA_SRV_ProcNormalConnSignalBearerModifyPcscfInvalid();
    }
}



VOS_VOID IMSA_SRV_ProcConnModifyIndMsg
(
    const IMSA_CONN_MODIFY_IND_STRU    *pstConnModifyIndMsg
)
{
    IMSA_INFO_LOG("IMSA_SRV_ProcConnModifyIndMsg is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcConnModifyIndMsg_ENUM, LNAS_ENTRY);

    /* 如果是媒体承载修改，则通过CALL模块资源具备 */
    if (IMSA_CONN_SIP_PDP_TYPE_MEDIA == pstConnModifyIndMsg->enSipPdpType)
    {
        IMSA_INFO_LOG("IMSA_SRV_ProcConnModifyIndMsg:no need notify CallModule!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcConnModifyIndMsg_ENUM, LNAS_ENTRY);
    }
    else
    {
        IMSA_SRV_ProcNormalConnSignalBearerModifyIndMsg(pstConnModifyIndMsg);
    }
}

VOS_VOID IMSA_SRV_ProcCallRsltWithFirstAddr
(
    const IMSA_CALL_RESULT_ACTION_IND_STRU *pstCallRsltActionIndMsg
)
{
    if (IMSA_CALL_TYPE_EMC != pstCallRsltActionIndMsg->enCallType)
    {
        IMSA_WARN_LOG("IMSA_SRV_ProcCallRsltActionIndMsg:normal srv,first addr pair can not happen!");
        TLPS_PRINT2LAYER_WARNING1(IMSA_SRV_ProcCallRsltActionIndMsg_ENUM, LNAS_PARAM_INVALID, 1);
        return ;
    }

    if (IMSA_SRV_STATUS_IDLE_DEREG != IMSA_SRV_GetEmcSrvStatus())
    {
        IMSA_WARN_LOG("IMSA_SRV_ProcCallRsltActionIndMsg:first addr pair,emc status is not IDLE&DEREG!");
        TLPS_PRINT2LAYER_WARNING1(IMSA_SRV_ProcCallRsltActionIndMsg_ENUM, IMSA_STATE_ERROR, 1);
        return ;
    }

    if (IMSA_RESOURCE_STATUS_ACTIVE == IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_EMERGENCY_CALL))
    {
        /* coverity[unchecked_value] */
        IMSA_SRV_SndConnSetupReq(   IMSA_FALSE,
                                    IMSA_CONN_TYPE_EMC,
                                    IMSA_GetCurrentDomain());

        IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_EMC,
                                    IMSA_SRV_STATUS_CONNING_DEREG);
    }
    #if (FEATURE_ON == FEATURE_DSDS)
    else if (IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF == IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_EMERGENCY_CALL))
    {
        IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_EMERGENCY_CALL,
                                      IMSA_RF_RES_APPLY_TRIGER_SRC_EMC_CALL,
                                      VOS_FALSE,
                                      VOS_FALSE);

        IMSA_SRV_SndConnSetupReq(   IMSA_FALSE,
                                    IMSA_CONN_TYPE_EMC,
                                    IMSA_IMS_DOMAIN_LTE);

        IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_EMC,
                                    IMSA_SRV_STATUS_CONNING_DEREG);
    }
    else
    {
        IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_EMERGENCY_CALL,
                                      IMSA_RF_RES_APPLY_TRIGER_SRC_EMC_CALL,
                                      VOS_FALSE,
                                      VOS_FALSE);
    }
    #endif
    return;
}


VOS_VOID IMSA_SRV_ProcCallRsltWithNextAddr
(
    const IMSA_CALL_RESULT_ACTION_IND_STRU *pstCallRsltActionIndMsg
)
{
    if (IMSA_CALL_TYPE_EMC != pstCallRsltActionIndMsg->enCallType)
    {
        IMSA_WARN_LOG("IMSA_SRV_ProcCallRsltActionIndMsg:normal srv,next addr pair can not happen!");
        TLPS_PRINT2LAYER_WARNING1(IMSA_SRV_ProcCallRsltActionIndMsg_ENUM, LNAS_PARAM_INVALID, 2);
        return ;
    }

    if (IMSA_SRV_STATUS_CONN_REG != IMSA_SRV_GetEmcSrvStatus())
    {
        IMSA_WARN_LOG("IMSA_SRV_ProcCallRsltActionIndMsg:next addr pair,emc status is not CONN&REG!");
        TLPS_PRINT2LAYER_WARNING1(IMSA_SRV_ProcCallRsltActionIndMsg_ENUM, IMSA_STATE_ERROR, 2);
        return ;
    }

    /* 先本地去注册，在发起注册请求，状态转到CONN&REGING */
    IMSA_SRV_SndRegDeregReq(    IMSA_REG_TYPE_EMC,
                                IMSA_DEREG_LOCAL);

    IMSA_SRV_SndRegRegReq(      IMSA_REG_TYPE_EMC,
                                IMSA_REG_ADDR_PARAM_NEXT);

    IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_EMC,
                                IMSA_SRV_STATUS_CONN_REGING);

    /* 由于CALL模块那还缓存着紧急呼，因此不能通知CALL模块无服务，否则会
       导致CALL模块清除缓存的紧急呼 */
    return;
}


VOS_VOID IMSA_SRV_ProcCallRsltRestoration
(
    const IMSA_CALL_RESULT_ACTION_IND_STRU *pstCallRsltActionIndMsg
)
{
    #if (FEATURE_ON == FEATURE_DSDS)
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = IMSA_GetControlManagerAddress();
    #endif

    if (IMSA_CALL_TYPE_EMC == pstCallRsltActionIndMsg->enCallType)
    {
        IMSA_WARN_LOG("IMSA_SRV_ProcCallRsltActionIndMsg:emc srv,restoration can not happen!");
        TLPS_PRINT2LAYER_WARNING1(IMSA_SRV_ProcCallRsltActionIndMsg_ENUM, LNAS_PARAM_INVALID, 3);
        return ;
    }

    if (IMSA_SRV_STATUS_CONN_REG != IMSA_SRV_GetNormalSrvStatus())
    {
        IMSA_WARN_LOG("IMSA_SRV_ProcCallRsltActionIndMsg:restoration,normal status is not CONN&REG!");
        TLPS_PRINT2LAYER_WARNING1(IMSA_SRV_ProcCallRsltActionIndMsg_ENUM, IMSA_STATE_ERROR, 3);
        return ;
    }

    if (IMSA_RESOURCE_STATUS_ACTIVE == IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        IMSA_SRV_SndRegDeregReq(    IMSA_REG_TYPE_NORMAL,
                                    IMSA_DEREG_LOCAL);

        IMSA_SRV_SndRegRegReq(      IMSA_REG_TYPE_NORMAL,
                                    IMSA_REG_ADDR_PARAM_RESTORATION);

        IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                    IMSA_SRV_STATUS_CONN_REGING);

        #if (FEATURE_ON == FEATURE_DSDS)
        if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
        {
            IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
        }
        #endif
    }
    #if (FEATURE_ON == FEATURE_DSDS)
    else if (IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF == IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                     IMSA_RF_RES_APPLY_TRIGER_SRC_GET_NOR_SRV,
                                     IMSA_REG_ADDR_PARAM_RESTORATION,
                                     IMSA_FALSE);

        IMSA_SRV_SndRegDeregReq(    IMSA_REG_TYPE_NORMAL,
                                IMSA_DEREG_LOCAL);

        IMSA_SRV_SndRegRegReq(      IMSA_REG_TYPE_NORMAL,
                                    IMSA_REG_ADDR_PARAM_RESTORATION);

        IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                    IMSA_SRV_STATUS_CONN_REGING);

        if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
        {
            IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
        }
    }
    else
    {
        IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                      IMSA_RF_RES_APPLY_TRIGER_SRC_GET_NOR_SRV,
                                      IMSA_REG_ADDR_PARAM_RESTORATION,
                                      IMSA_FALSE);

        IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL,
                                IMSA_DEREG_LOCAL);

        IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                    IMSA_SRV_STATUS_CONN_REGING);
    }
    #endif
    /* 由于已经通知注册模块进行本地去注册，因此无需通知CALL模块无服务，
       否则本地去注册流程和释放CALL流程同时进行，容易出问题 */

    /* 记录UE发起去注册事件，及发起注册的时间 */
    #if (FEATURE_ON == FEATURE_PTM)
    /* 记录UE发起注册的时间 */
    ( VOS_VOID )LPS_OM_GET32K_TIMESTAMP(&(IMSA_GetErrlogStartRegTimerAddr()->ul32KTStampHighBit),
                                        &(IMSA_GetErrlogStartRegTimerAddr()->ul32KTStampLowBit));
    #endif

    return;
}


VOS_VOID IMSA_SRV_ProcCallRsltWithCurrAddr
(
    const IMSA_CALL_RESULT_ACTION_IND_STRU *pstCallRsltActionIndMsg
)
{
    #if (FEATURE_ON == FEATURE_DSDS)
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = IMSA_GetControlManagerAddress();
    #endif

    if (IMSA_CALL_TYPE_EMC == pstCallRsltActionIndMsg->enCallType)
    {
        IMSA_WARN_LOG("IMSA_SRV_ProcCallRsltWithCurrAddr:emc srv,restoration can not happen!");
        TLPS_PRINT2LAYER_WARNING1(IMSA_SRV_ProcCallRsltWithCurrAddr_ENUM, LNAS_PARAM_INVALID, 3);
        return ;
    }

    if (IMSA_SRV_STATUS_CONN_REG != IMSA_SRV_GetNormalSrvStatus())
    {
        IMSA_WARN_LOG("IMSA_SRV_ProcCallRsltWithCurrAddr:restoration,normal status is not CONN&REG!");
        TLPS_PRINT2LAYER_WARNING1(IMSA_SRV_ProcCallRsltWithCurrAddr_ENUM, IMSA_STATE_ERROR, 3);
        return ;
    }

    if (IMSA_RESOURCE_STATUS_ACTIVE == IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        IMSA_SRV_SndRegDeregReq(    IMSA_REG_TYPE_NORMAL,
                                    IMSA_DEREG_LOCAL);

        IMSA_SRV_SndRegRegReq(      IMSA_REG_TYPE_NORMAL,
                                    IMSA_REG_ADDR_PARAM_SAME);

        IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                    IMSA_SRV_STATUS_CONN_REGING);

        #if (FEATURE_ON == FEATURE_DSDS)
        if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
        {
            IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
        }
        #endif
    }
    #if (FEATURE_ON == FEATURE_DSDS)
    else if (IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF == IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                     IMSA_RF_RES_APPLY_TRIGER_SRC_GET_NOR_SRV,
                                     IMSA_REG_ADDR_PARAM_SAME,
                                     IMSA_FALSE);

        IMSA_SRV_SndRegDeregReq(    IMSA_REG_TYPE_NORMAL,
                                IMSA_DEREG_LOCAL);

        IMSA_SRV_SndRegRegReq(      IMSA_REG_TYPE_NORMAL,
                                    IMSA_REG_ADDR_PARAM_SAME);

        IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                    IMSA_SRV_STATUS_CONN_REGING);

        if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
        {
            IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
        }
    }
    else
    {
        IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                      IMSA_RF_RES_APPLY_TRIGER_SRC_GET_NOR_SRV,
                                      IMSA_REG_ADDR_PARAM_SAME,
                                      IMSA_FALSE);

        IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL,
                                IMSA_DEREG_LOCAL);

        IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                    IMSA_SRV_STATUS_CONN_REGING);
    }
    #endif
    /* 由于已经通知注册模块进行本地去注册，因此无需通知CALL模块无服务，
       否则本地去注册流程和释放CALL流程同时进行，容易出问题 */
    /* 记录UE发起去注册事件，及发起注册的时间 */
    #if (FEATURE_ON == FEATURE_PTM)
    /* 记录UE发起注册的时间 */
    ( VOS_VOID )LPS_OM_GET32K_TIMESTAMP(&(IMSA_GetErrlogStartRegTimerAddr()->ul32KTStampHighBit),
                                        &(IMSA_GetErrlogStartRegTimerAddr()->ul32KTStampLowBit));
    #endif
    return;
}


VOS_VOID IMSA_SRV_ProcCallRsltActionIndMsg
(
    const IMSA_CALL_RESULT_ACTION_IND_STRU *pstCallRsltActionIndMsg
)
{
    IMSA_INFO_LOG("IMSA_SRV_ProcCallRsltActionIndMsg is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcCallRsltActionIndMsg_ENUM, LNAS_ENTRY);

    switch (pstCallRsltActionIndMsg->enResultAction)
    {
        case IMSA_RESULT_ACTION_REG_WITH_FIRST_ADDR_PAIR:

            IMSA_SRV_ProcCallRsltWithFirstAddr(pstCallRsltActionIndMsg);
            break;
        case IMSA_RESULT_ACTION_REG_WITH_NEXT_ADDR_PAIR:

            IMSA_SRV_ProcCallRsltWithNextAddr(pstCallRsltActionIndMsg);
            break;
        case IMSA_RESULT_ACTION_REG_RESTORATION:

            IMSA_SRV_ProcCallRsltRestoration(pstCallRsltActionIndMsg);
            break;

        case IMSA_RESULT_ACTION_REG_WITH_CURRENT_ADDR_PAIR:
            IMSA_SRV_ProcCallRsltWithCurrAddr(pstCallRsltActionIndMsg);
            break;

        default:

            IMSA_WARN_LOG("IMSA_SRV_ProcCallRsltActionIndMsg:illegal result action!");
            TLPS_PRINT2LAYER_WARNING(IMSA_SRV_ProcCallRsltActionIndMsg_ENUM, LNAS_ERROR);

            break;
    }
}



VOS_VOID IMSA_SRV_ProcCallSrvccSuccIndMsg
(
    const IMSA_CALL_SRVCC_SUCC_IND_STRU *pstCallSrvccSuccIndMsg
)
{
    IMSA_INFO_LOG("IMSA_SRV_ProcCallSrvccSuccIndMsg is entered!");

    (VOS_VOID)pstCallSrvccSuccIndMsg;

    /* 收到CALL模块发来的SRVCC成功信息后，通知TAF当前IMS处于未注册状态,
       TAF就不会再域选到IMS。
       电话挂断后，回到LTE会发TAU，MSCC会通知SERVICE_CHANGE_IND，此时
       IMSA判断之前通知过未注册状态，会再通知TAF当前为已注册状态，
       之后就可以正常域选到IMS了 */
    IMSA_SRV_NrmRegStatusNotify(IMSA_NORMAL_REG_STATUS_LTE_DEREG);

    IMSA_SetLastNrmRegStatus(IMSA_NORMAL_REG_STATUS_LTE_DEREG);
}


VOS_VOID IMSA_SRV_ProcTransToNotSupportEms( VOS_VOID )
{

    IMSA_CONTROL_MANAGER_STRU          *pstImsaControlManager   = VOS_NULL_PTR;
    IMSA_CONN_RELEASE_TYPE_ENUM_UINT8       enConnRlsType;

    IMSA_INFO_LOG("IMSA_SRV_ProcTransToNotSupportEms is entered!");

    pstImsaControlManager   = IMSA_GetControlManagerAddress();
    /* 设置PDN释放类型 */
    if( VOS_TRUE == pstImsaControlManager->ucOrgPdnRlsFlag)
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL;
    }
    else
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_LOCAL;
    }


    switch (IMSA_SRV_GetEmcSrvStatus())
    {
        case IMSA_SRV_STATUS_CONN_REG:

            IMSA_INFO_LOG("IMSA_SRV_ProcTransToNotSupportEms:conn&reg!");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcTransToNotSupportEms_ENUM, IMSA_STATUS_CONN);
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcTransToNotSupportEms_ENUM, IMSA_STATUS_REGISTERED);

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_EMC, IMSA_DEREG_LOCAL);
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnRelReq( IMSA_CONN_TYPE_EMC,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_GetCurrentDomain(),
                                    enConnRlsType);

            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_EMC, IMSA_SRV_STATUS_RELEASING_DEREG);

            break;

        case IMSA_SRV_STATUS_CONNING_DEREG:

            IMSA_INFO_LOG("IMSA_SRV_ProcTransToNotSupportEms:conning&dereg!");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcTransToNotSupportEms_ENUM, IMSA_STATUS_CONNECTING);
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcTransToNotSupportEms_ENUM, IMSA_STATUS_NOT_REGISTER);
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnRelReq( IMSA_CONN_TYPE_EMC,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_GetCurrentDomain(),
                                    enConnRlsType);

            /* 通知CALL模块紧急呼叫服务进入无服务状态，促使CALL模块清除缓存的紧急呼 */
            IMSA_SRV_SndEmcCallSrvStatusInd(IMSA_CALL_SERVICE_STATUS_NO_SERVICE,
                                            IMSA_CALL_NO_SRV_CAUSE_REG_ERR);

            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_EMC, IMSA_SRV_STATUS_RELEASING_DEREG);

            break;

        case IMSA_SRV_STATUS_CONN_DEREG:

            IMSA_INFO_LOG("IMSA_SRV_ProcTransToNotSupportEms:conn&dereg!");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcTransToNotSupportEms_ENUM, IMSA_STATUS_CONN);
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcTransToNotSupportEms_ENUM, IMSA_STATUS_NOT_REGISTER);
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnRelReq( IMSA_CONN_TYPE_EMC,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_GetCurrentDomain(),
                                    enConnRlsType);

            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_EMC, IMSA_SRV_STATUS_RELEASING_DEREG);

            break;

        case IMSA_SRV_STATUS_CONN_REGING:

            IMSA_INFO_LOG("IMSA_SRV_ProcTransToNotSupportEms:conn&reging!");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcTransToNotSupportEms_ENUM, IMSA_STATUS_CONN);
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcTransToNotSupportEms_ENUM, IMSA_STATUS_REGISTERING);

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_EMC, IMSA_DEREG_LOCAL);
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnRelReq( IMSA_CONN_TYPE_EMC,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_GetCurrentDomain(),
                                    enConnRlsType);

            /* 通知CALL模块紧急呼叫服务进入无服务状态，促使CALL模块清除缓存的紧急呼 */
            IMSA_SRV_SndEmcCallSrvStatusInd(IMSA_CALL_SERVICE_STATUS_NO_SERVICE,
                                            IMSA_CALL_NO_SRV_CAUSE_REG_ERR);

            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_EMC, IMSA_SRV_STATUS_RELEASING_DEREG);

            break;

        default:
            break;
    }
}



VOS_VOID IMSA_SRV_UpdateRat
(
    MSCC_IMSA_RAT_TYPE_ENUM_UINT8        enRat,
    VOS_UINT32                         *pulIsNotfiyNrmNoSrv,
    VOS_UINT32                         *pulIsNotfiyEmcNoSrv
)
{
    IMSA_NETWORK_INFO_STRU             *pstNwInfo           = VOS_NULL_PTR;
    IMSA_CAMPED_RAT_TYPE_ENUM_UINT8     enCampedRatTypeTmp  = IMSA_CAMPED_RAT_TYPE_BUTT;
    IMSA_CALL_MANAGER_STRU              *pstImsaCallCtx   = VOS_NULL_PTR;

    pstImsaCallCtx   = IMSA_CallCtxGet();
    pstNwInfo = IMSA_GetNetInfoAddress();

    *pulIsNotfiyNrmNoSrv = IMSA_FALSE;
    *pulIsNotfiyEmcNoSrv = IMSA_FALSE;

    IMSA_SRV_RatFormatTransform(    &enCampedRatTypeTmp,
                                    enRat);

    /* 如果接入未变更，则直接返回 */
    if (pstNwInfo->enImsaCampedRatType == enCampedRatTypeTmp)
    {
        return ;
    }

    /* 从GU变换到L，且当前有呼叫存在，且通知LRRC开启HIFI同步 */
    if ((enCampedRatTypeTmp == IMSA_CAMPED_RAT_TYPE_EUTRAN)
        && (VOS_TRUE == IMSA_IsCallConnExist()))
    {
        IMSA_SndLRrcHifiSyncSwtichInd(IMSA_TRUE);
    }

    pstNwInfo->enImsaCampedRatType = enCampedRatTypeTmp;

    /*异系统变换后需要停止BACK-OFF定时器*/
    IMSA_StopTimer(&pstImsaCallCtx->stBackOffTxTimer);
    IMSA_StopTimer(&pstImsaCallCtx->stBackOffTyTimer);

    /* 如果接入技术发生变更，且目的接入不支持IMS，且SRVCC标识为FALSE，则指示
       IMSA CALL子模块无服务，原因值填为非SRVCC异系统 */
    if ((IMSA_FALSE == IMSA_SRV_IsCurRatSupportIms())
        && (IMSA_FALSE == IMSA_CallGetSrvccFlag()))
    {
        *pulIsNotfiyNrmNoSrv = IMSA_TRUE;

    }

    /* 如果接入技术发生变更，且目的接入不支持EMS，则指示IMSA REG子模块本地紧急
       去注册（如果存在IMS紧急注册）或者通知IMSA CONN子模块释放紧急连接（
       如果只存在IMS紧急连接，不存在IMS紧急注册） */
    if (IMSA_FALSE == IMSA_SRV_IsCurRatSupportEms())
    {
        *pulIsNotfiyEmcNoSrv = IMSA_TRUE;


    }
}


VOS_VOID IMSA_SRV_UpdateImsVoPsStatus
(
    MSCC_IMSA_IMS_VOPS_INDICATOR_ENUM_UINT8      enImsVoPsInd
)
{
    IMSA_NETWORK_INFO_STRU             *pstNwInfo           = VOS_NULL_PTR;

    pstNwInfo = IMSA_GetNetInfoAddress();

    if(IMSA_IMS_DOMAIN_LTE == IMSA_GetCurrentDomain())
    {
       if (pstNwInfo->enImsaImsVoPsStatus != enImsVoPsInd)
       {
           pstNwInfo->enImsaImsVoPsStatus          = enImsVoPsInd;
           IMSA_SndMsgAtCirepiInd(enImsVoPsInd);
       }

       IMSA_ConfigNetCapInfo2Ims();
    }
    else
    {
       if (pstNwInfo->enImsaImsVoPsStatus != enImsVoPsInd)
       {
           pstNwInfo->enImsaImsVoPsStatus          = enImsVoPsInd;
       }
    }


}


VOS_VOID IMSA_SRV_UpdateEmsStatus
(
    MSCC_IMSA_EMS_INDICATOR_ENUM_UINT8           enEmsInd
)
{
    IMSA_NETWORK_INFO_STRU             *pstNwInfo           = VOS_NULL_PTR;

    pstNwInfo = IMSA_GetNetInfoAddress();

    pstNwInfo->enImsaEmsStatus = enEmsInd;

}


VOS_VOID IMSA_SRV_ProcSrvChgIndWhenRoamNotSatisfied(VOS_VOID)
{
    IMSA_SRV_STATUS_ENUM_UINT8          enNrmSrvStatus      = IMSA_SRV_STATUS_BUTT;

    IMSA_CONTROL_MANAGER_STRU          *pstControlManager   = VOS_NULL_PTR;

    IMSA_CONN_RELEASE_TYPE_ENUM_UINT8       enConnRlsType;

    pstControlManager  = IMSA_GetControlManagerAddress();

    enNrmSrvStatus = IMSA_SRV_GetNormalSrvStatus();

    /* 设置PDN释放类型 */
    if( VOS_TRUE == pstControlManager->ucOrgPdnRlsFlag)
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL;
    }
    else
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_LOCAL;
    }

    /* 如果当前有电话存在，则需要等电话结束后，才能发起去注册 */
    if (VOS_TRUE == IMSA_IsImsExist())
    {
        return;
    }

    /* 根据当前服务状态进行不同处理 */
    switch (enNrmSrvStatus)
    {
        case IMSA_SRV_STATUS_IDLE_DEREG:

            /* 通知MMA IMS VOICE  不可用 */
            /* 其他几种场景不满足IMS注册条件时，MMA自己能够识别，直接认为IMS voice不可用， 不需要IMSA
            通知；但是漫游网络下，且不支持IMS注册，MMA是无法识别的，所以需要IMSA主动通知一下 */
            IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE,IMSA_CONN_IsExitPersistentBearer());

            #if (FEATURE_ON == FEATURE_PTM)
            IMSA_NotInitRegErrRecord();
            #endif
            break;

        case IMSA_SRV_STATUS_CONN_REGING:

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_NOT_LOCAL);
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_CONN_DEREGING);
            break;

        case IMSA_SRV_STATUS_CONNING_DEREG:
        case IMSA_SRV_STATUS_CONN_DEREG:

            IMSA_SRV_SndConnRelReq(IMSA_SRV_TYPE_NORMAL, IMSA_CONN_SIP_PDP_TYPE_SIGNAL, IMSA_IMS_DOMAIN_LTE,enConnRlsType);
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_RELEASING_DEREG);
            break;

        case IMSA_SRV_STATUS_CONNING_REG:
            /* 需要通知IMS允许其发包，通知发起本地注册 */
            IMSA_SndImsMsgServiceSuspendOrResumeSrvInfo(IMSA_IMS_INPUT_SERVICE_REASON_RESUME_NRM_SRV);
            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_IDLE_DEREG);
            /* coverity[unchecked_value] */
            IMSA_SRV_SndDomainSrvStatusInd(IMSA_DOMAIN_SERVICE_STATUS_NO_SERVICE,
                                            IMSA_DOMAIN_NO_SRV_CAUSE_NW_NOT_SUPPORT_IMS_VOICE,
                                            IMSA_GetCurrentDomain());

            /* 上报状态给Domain，Domain会发起域选过程 */
            if( VOS_TRUE == pstControlManager->ucDomainRegSrvStatusFlag)
            {
                pstControlManager->ucDomainRegSrvStatusFlag = VOS_FALSE;
                IMSA_SRV_SndDomainSrvReadyInd();
            }
            break;

        case IMSA_SRV_STATUS_CONN_REG:
            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_NOT_LOCAL);
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_CONN_DEREGING);
            break;

        default:
            break;
    }

}



VOS_VOID IMSA_SRV_ProcWhenRoamNotSatisfiedConnReging(VOS_VOID)
{
    IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_CONN_DEREGING);

    switch (IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        case IMSA_RESOURCE_STATUS_ACTIVE:

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_NOT_LOCAL);
            break;
        #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:

            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          VOS_FALSE,
                                          VOS_FALSE);

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_NOT_LOCAL);
            break;

        case IMSA_RESOURCE_STATUS_NULL:

            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          VOS_FALSE,
                                          VOS_FALSE);

            //IMSA_DsdsSaveMoSmsMsg(pstRpDataReq);
            break;
        #endif
        default:
            #if (FEATURE_ON == FEATURE_DSDS)
            IMSA_DsdsModifyTaskSrcByTaskType(IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                             RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
            #endif
            break;

    }
}

VOS_VOID IMSA_SRV_ProcWhenRoamNotSatisfiedConningDeReg(VOS_VOID)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlManager   = VOS_NULL_PTR;
    IMSA_CONN_RELEASE_TYPE_ENUM_UINT8       enConnRlsType;

    pstControlManager  = IMSA_GetControlManagerAddress();

    IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_RELEASING_DEREG);

    /* 设置PDN释放类型 */
    if( VOS_TRUE == pstControlManager->ucOrgPdnRlsFlag)
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL;
    }
    else
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_LOCAL;
    }

    switch (IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        case IMSA_RESOURCE_STATUS_ACTIVE:
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnRelReq(IMSA_SRV_TYPE_NORMAL,
                                   IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                   IMSA_GetCurrentDomain(),
                                   enConnRlsType);
            break;
        #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:

            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          VOS_FALSE,
                                          VOS_FALSE);
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnRelReq(IMSA_SRV_TYPE_NORMAL,
                                   IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                   IMSA_GetCurrentDomain(),
                                   enConnRlsType);
            break;

        case  IMSA_RESOURCE_STATUS_NULL:

            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          VOS_FALSE,
                                          VOS_FALSE);

            //IMSA_DsdsSaveMoSmsMsg(pstRpDataReq);
            break;
        #endif
        default:
            #if (FEATURE_ON == FEATURE_DSDS)
            IMSA_DsdsModifyTaskSrcByTaskType(IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                             RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
            #endif
            break;

    }

}

VOS_VOID IMSA_SRV_ProcWhenRoamNotSatisfiedConnReg(VOS_VOID)
{

    IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_CONN_DEREGING);

    switch (IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        case IMSA_RESOURCE_STATUS_ACTIVE:

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_NOT_LOCAL);
            break;
        #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:

            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          VOS_FALSE,
                                          VOS_FALSE);

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_NOT_LOCAL);
            break;

        case  IMSA_RESOURCE_STATUS_NULL:

            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          VOS_FALSE,
                                          VOS_FALSE);

            //IMSA_DsdsSaveMoSmsMsg(pstRpDataReq);
            break;
        #endif
        default:
            /* */
            break;

    }
}


VOS_VOID IMSA_SRV_ProcWhenRoamNotSatisfied(VOS_VOID)
{
    IMSA_SRV_STATUS_ENUM_UINT8          enNrmSrvStatus      = IMSA_SRV_STATUS_BUTT;
    IMSA_CONTROL_MANAGER_STRU          *pstControlManager   = VOS_NULL_PTR;

    pstControlManager  = IMSA_GetControlManagerAddress();

    enNrmSrvStatus = IMSA_SRV_GetNormalSrvStatus();

    /* 如果当前有IMS业务存在，则需要等业务结束后，才能发起去注册 */
    if (VOS_TRUE == IMSA_IsImsExist())
    {
        return;
    }
    /* 如果当前普通服务支持漫游，则不处理 */
    if (IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_ROAM_NOT_SUPPORT != IMSA_SRV_IsNrmSrvConditonSatisfied())
    {
        return;
    }

    /* 根据当前服务状态进行不同处理 */
    switch (enNrmSrvStatus)
    {
        case IMSA_SRV_STATUS_IDLE_DEREG:
            /* 通知MMA IMS VOICE  不可用 */
            /* 其他几种场景不满足IMS注册条件时，MMA自己能够识别，直接认为IMS voice不可用， 不需要IMSA
            通知；但是漫游网络下，且不支持IMS注册，MMA是无法识别的，所以需要IMSA主动通知一下 */
            IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE,IMSA_CONN_IsExitPersistentBearer());

            /* 增加域选触发机制
               场景:优先域设置为volte prefer，在LTE和WiFi双域都可用的情况,首先会域选到
                    LTE下尝试，由于UE设置为漫游下不支持IMS注册，IDLE+DEREG情况下会走到这里，
                    在这里通知domain模块LTE下的注册状态,并重新域选判决*/
            /* coverity[unchecked_value] */
            IMSA_SRV_SndDomainSrvStatusInd(IMSA_DOMAIN_SERVICE_STATUS_NO_SERVICE,
                                            IMSA_DOMAIN_NO_SRV_CAUSE_SIP_PDP_ERR,
                                            IMSA_GetCurrentDomain());

            #if (FEATURE_ON == FEATURE_PTM)
            IMSA_NotInitRegErrRecord();
            #endif
            break;
        case IMSA_SRV_STATUS_CONN_REGING:
            IMSA_SRV_ProcWhenRoamNotSatisfiedConnReging();
            #if (FEATURE_ON == FEATURE_PTM)
            if (IMSA_SRV_STATUS_CONN_REGING == IMSA_SRV_GetNormalSrvStatus())
            {
                IMSA_RegFailEventReport(IMSA_ERR_LOG_REG_FAIL_REASON_ROAMING_NOT_SUPPORT_REG,
                                        IMSA_FALSE,
                                        IMSA_FALSE,
                                        IMSA_FALSE);
            }
            #endif
            break;

        case IMSA_SRV_STATUS_CONNING_DEREG:
        case IMSA_SRV_STATUS_CONN_DEREG:
            IMSA_SRV_ProcWhenRoamNotSatisfiedConningDeReg();
            #if (FEATURE_ON == FEATURE_PTM)
            IMSA_SRV_InitDeregEventReport(IMSA_ERR_LOG_INIT_DEREG_REASON_ROAMING_NOT_SUPPORT_REG);
            #endif
            break;

        case IMSA_SRV_STATUS_CONNING_REG:
            /* 需要通知IMS允许其发包，通知发起本地注册 */
            IMSA_SndImsMsgServiceSuspendOrResumeSrvInfo(IMSA_IMS_INPUT_SERVICE_REASON_RESUME_NRM_SRV);
            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_IDLE_DEREG);
            /* coverity[unchecked_value] */
            IMSA_SRV_SndDomainSrvStatusInd(IMSA_DOMAIN_SERVICE_STATUS_NO_SERVICE,
                                            IMSA_DOMAIN_NO_SRV_CAUSE_NW_NOT_SUPPORT_IMS_VOICE,
                                            IMSA_GetCurrentDomain());

            /* 上报状态给Domain，Domain会发起域选过程 */
            if( VOS_TRUE == pstControlManager->ucDomainRegSrvStatusFlag)
            {
                pstControlManager->ucDomainRegSrvStatusFlag = VOS_FALSE;
                IMSA_SRV_SndDomainSrvReadyInd();
            }

            /*释放资源 */
            #if (FEATURE_ON == FEATURE_DSDS)
            IMSA_DsdsProcResourceRelease(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
            #endif
            #if (FEATURE_ON == FEATURE_PTM)
            IMSA_SRV_InitDeregEventReport(IMSA_ERR_LOG_INIT_DEREG_REASON_ROAMING_NOT_SUPPORT_REG);
            #endif
            break;

        case IMSA_SRV_STATUS_CONN_REG:
            IMSA_SRV_ProcWhenRoamNotSatisfiedConnReg();

            #if (FEATURE_ON == FEATURE_PTM)
            IMSA_SRV_InitDeregEventReport(IMSA_ERR_LOG_INIT_DEREG_REASON_ROAMING_NOT_SUPPORT_REG);
            #endif
            break;

        default:
            /* 释放资源 */
            #if (FEATURE_ON == FEATURE_DSDS)
            IMSA_DsdsProcResourceRelease(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
            #endif
            break;
    }


}


VOS_VOID IMSA_SRV_ProcWhenRoamIsSatisfiedIdleDereg()
{
    IMSA_IMS_DOMAIN_ENUM_UINT8          ucCurrentDomain;
    #if (FEATURE_ON == FEATURE_DSDS)
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;

    pstControlMagnaer                       = IMSA_GetControlManagerAddress();
    #endif

    ucCurrentDomain = IMSA_GetCurrentDomain();

    if (IMSA_NRM_SRV_CON_SAT_STATUE_SUCC != IMSA_SRV_IsNrmSrvConditonSatisfied())
    {
        return;
    }

    switch (IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        case IMSA_RESOURCE_STATUS_ACTIVE:
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnSetupReq(   IMSA_FALSE,
                                        IMSA_CONN_TYPE_NORMAL,
                                        ucCurrentDomain);
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_CONNING_DEREG);
            #if (FEATURE_ON == FEATURE_DSDS)
            if (PS_RRM_RF_NOT_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
            }
            #endif
            break;
        #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESOURCE_STATUS_NULL:
            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                       IMSA_RF_RES_APPLY_TRIGER_SRC_GET_NOR_SRV,
                                       IMSA_FALSE,
                                       IMSA_FALSE);
            break;

        case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:

           IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                       IMSA_RF_RES_APPLY_TRIGER_SRC_GET_NOR_SRV,
                                       IMSA_FALSE,
                                       IMSA_FALSE);
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnSetupReq(   IMSA_FALSE,
                                    IMSA_CONN_TYPE_NORMAL,
                                    IMSA_GetCurrentDomain());
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_CONNING_DEREG);

            IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
            break;
        #endif
        default:
            break;
    }
}


VOS_VOID IMSA_SRV_ProcWhenRoamIsSatisfiedConnDereg()
{
    #if (FEATURE_ON == FEATURE_DSDS)
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;
    #endif
    IMSA_NRM_SRV_CON_SAT_STATUE_ENMUM_UINT32    enSrvRst    = IMSA_NRM_SRV_CON_SAT_STATUE_BUTT;

    #if (FEATURE_ON == FEATURE_DSDS)
    pstControlMagnaer                       = IMSA_GetControlManagerAddress();
    #endif
    enSrvRst                                = IMSA_SRV_IsNrmSrvConditonSatisfied();

    if (IMSA_NRM_SRV_CON_SAT_STATUE_SUCC != enSrvRst)
    {
        return;
    }

    switch (IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        case IMSA_RESOURCE_STATUS_ACTIVE:
            IMSA_SRV_SndRegRegReq(      IMSA_REG_TYPE_NORMAL,
                                    IMSA_REG_ADDR_PARAM_NEXT);
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_CONN_REGING);
        #if (FEATURE_ON == FEATURE_DSDS)
            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
            }
        #endif
            break;
    #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESOURCE_STATUS_NULL:
            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                       IMSA_RF_RES_APPLY_TRIGER_SRC_GET_NOR_SRV,
                                       IMSA_REG_ADDR_PARAM_NEXT,
                                       IMSA_FALSE);
            break;

        case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:

           IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                       IMSA_RF_RES_APPLY_TRIGER_SRC_GET_NOR_SRV,
                                       IMSA_REG_ADDR_PARAM_NEXT,
                                       IMSA_FALSE);

            IMSA_SRV_SndRegRegReq(      IMSA_REG_TYPE_NORMAL,
                                        IMSA_REG_ADDR_PARAM_NEXT);
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_CONN_REGING);

            IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
            break;
    #endif
        default:
            break;
    }
    #if (FEATURE_ON == FEATURE_PTM)
    /* 记录UE发起注册的时间 */
    ( VOS_VOID )LPS_OM_GET32K_TIMESTAMP(&(IMSA_GetErrlogStartRegTimerAddr()->ul32KTStampHighBit),
                                        &(IMSA_GetErrlogStartRegTimerAddr()->ul32KTStampLowBit));
    #endif
}


VOS_VOID IMSA_SRV_ProcWhenRoamIsSatisfied()
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_SRV_ProcWhenRoamIsSatisfied is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcWhenRoamIsSatisfied_ENUM, LNAS_ENTRY);

    if (IMSA_IMS_DOMAIN_NULL == IMSA_GetCurrentDomain())
    {
        IMSA_SRV_SndDomainSrvReadyInd();
        return;
    }

    pstControlMagnaer  = IMSA_GetControlManagerAddress();

    switch (IMSA_SRV_GetNormalSrvStatus())
    {
        case IMSA_SRV_STATUS_RELEASING_DEREG:
        case IMSA_SRV_STATUS_CONN_DEREGING:

            IMSA_INFO_LOG("IMSA_SRV_ProcWhenRoamIsSatisfied:releasing&dereg!");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcWhenRoamIsSatisfied_ENUM, IMSA_STATUS_RELEASING);
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcWhenRoamIsSatisfied_ENUM, IMSA_STATUS_NOT_REGISTER);

            IMSA_StartTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer);
            break;

        case IMSA_SRV_STATUS_IDLE_DEREG:

            IMSA_INFO_LOG("IMSA_SRV_ProcWhenRoamIsSatisfied:idle&dereg!");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcWhenRoamIsSatisfied_ENUM, IMSA_STATUS_IDLE);
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcWhenRoamIsSatisfied_ENUM, IMSA_STATUS_NOT_REGISTER);

            IMSA_SRV_ProcWhenRoamIsSatisfiedIdleDereg();

            break;
        case IMSA_SRV_STATUS_CONN_DEREG:

            IMSA_INFO_LOG("IMSA_SRV_ProcWhenRoamIsSatisfied:conn&dereg!");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcWhenRoamIsSatisfied_ENUM, IMSA_STATUS_CONN);
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcWhenRoamIsSatisfied_ENUM, IMSA_STATUS_NOT_REGISTER);

            IMSA_SRV_ProcWhenRoamIsSatisfiedConnDereg();

            break;
        default:
            IMSA_INFO_LOG("IMSA_SRV_ProcWhenRoamIsSatisfied:other states!");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcWhenRoamIsSatisfied_ENUM, 1);
            break;
    }
    return;
}


VOS_UINT32 IMSA_SRV_PreProcNrmSrvConSatStatus
(
    IMSA_NRM_SRV_CON_SAT_STATUE_ENMUM_UINT32    enImsaSrvStatue
)
{
    switch(enImsaSrvStatue)
    {
        case IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_RAT_NOT_SUPPORT:
            /* 如果当前接入技术不支持，则给CALL模块上报无服务 */
            if ((MSCC_IMSA_IMS_VOPS_NOT_SUPPORT == IMSA_GetNetInfoAddress()->enImsaImsVoPsStatus)&&
                (IMSA_FALSE == IMSA_GetControlManagerAddress()->ucNonSrvccNotRelCallFlag))
            {
                /* 增加入参 */
                IMSA_SRV_SndNrmCallSrvStatusInd(IMSA_CALL_SERVICE_STATUS_NO_SERVICE,IMSA_CALL_NO_SRV_CAUSE_NON_SRVCC_RAT_CHANGE,IMSA_CALL_NORM_SRV_EXTEND_INFO_NONE);
                IMSA_SRV_SndEmcCallSrvStatusInd(IMSA_CALL_SERVICE_STATUS_NO_SERVICE,IMSA_CALL_NO_SRV_CAUSE_NON_SRVCC_RAT_CHANGE);
            }
            IMSA_CMCCDeregProc();
            break;

        case IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_USIM_NOT_SUPPORT:
            /* 其他几种场景不满足IMS注册条件时，MMA自己能够识别，直接认为IMS voice不可用， 不需要IMSA
            通知；但是卡无效的场景，MMA是无法识别的，所以需要IMSA主动通知一下 */
            IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE,IMSA_CONN_IsExitPersistentBearer());
        #if (FEATURE_ON == FEATURE_PTM)
            IMSA_NotInitRegErrRecord();
        #endif
            break;

        case IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_ROAM_NOT_SUPPORT:
            /* 需要根据当前的服务状态，判断是直接通知VOICE不可用，还是发起去注册 */
            IMSA_CommonDeregProc();
            break;

        case IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_VOICE_NOT_SUPPORT:
            /* 中移动定制需求，如果IMS语音不支持，则发起去注册 */
            IMSA_CMCCDeregProc();
            break;

        case IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_OTHERS:
            /* IMS语音不支持或者PS域服务状态不为NORMAL SERVICE时，无特殊处理 */
            #if (FEATURE_ON == FEATURE_PTM)
            IMSA_NotInitRegErrRecord();
            #endif
            break;
        case IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_REG_FORBIDDEN:
        case IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_CONN_FORBIDDEN:
            IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE,IMSA_CONN_IsExitPersistentBearer());
            #if (FEATURE_ON == FEATURE_PTM)
            IMSA_NotInitRegErrRecord();
            #endif
            break;

        default:
            return IMSA_TRUE;
    }

    return IMSA_FALSE;
}

VOS_VOID IMSA_SRV_ProcServiceChangeIndIdleDereg
(
    VOS_VOID
)
{
    #if (FEATURE_ON == FEATURE_DSDS)
    IMSA_CONTROL_MANAGER_STRU                  *pstControlMagnaer   = VOS_NULL_PTR;

    pstControlMagnaer                       = IMSA_GetControlManagerAddress();
    #endif

    switch(IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESOURCE_STATUS_NULL:
            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_GET_NOR_SRV,
                                          IMSA_FALSE,
                                          IMSA_FALSE);
            break;

        case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:
            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_GET_NOR_SRV,
                                          IMSA_FALSE,
                                          IMSA_FALSE);

            /* 请求连接模块建立连接，将状态转到IMSA_SERVICE_STATUS_CONNING_DEREG */
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnSetupReq(   IMSA_FALSE,
                                        IMSA_CONN_TYPE_NORMAL,
                                        IMSA_GetCurrentDomain());
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_CONNING_DEREG);
            IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
            break;
        #endif
        case IMSA_RESOURCE_STATUS_ACTIVE:

            /* 请求连接模块建立连接，将状态转到IMSA_SERVICE_STATUS_CONNING_DEREG */
            /* coverity[unchecked_value]*/
            IMSA_SRV_SndConnSetupReq(   IMSA_FALSE,
                                        IMSA_CONN_TYPE_NORMAL,
                                        IMSA_GetCurrentDomain());
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_CONNING_DEREG);

            #if (FEATURE_ON == FEATURE_DSDS)
            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
            }
            #endif
            break;
        default:
            break;
    }
    /* 记录UE发起建链的时间 */
    #if (FEATURE_ON == FEATURE_PTM)
    ( VOS_VOID )LPS_OM_GET32K_TIMESTAMP(&(IMSA_GetErrlogStartConnTimerAddr()->ul32KTStampHighBit),
                                        &(IMSA_GetErrlogStartConnTimerAddr()->ul32KTStampLowBit));
    #endif
}


VOS_VOID IMSA_SRV_ProcServiceChangeIndConnDereging
(
    VOS_VOID
)
{
    #if (FEATURE_ON == FEATURE_DSDS)
    IMSA_CONTROL_MANAGER_STRU                  *pstControlMagnaer   = VOS_NULL_PTR;

    pstControlMagnaer                       = IMSA_GetControlManagerAddress();
    #endif

    switch(IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESOURCE_STATUS_NULL:
            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_GET_NOR_SRV,
                                          IMSA_FALSE,
                                          IMSA_FALSE);
            break;

        case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:
            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_GET_NOR_SRV,
                                          IMSA_FALSE,
                                          IMSA_FALSE);

            IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE,IMSA_CONN_IsExitPersistentBearer());

            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_CONN_REGING);

            IMSA_SRV_SndRegDeregReq(    IMSA_REG_TYPE_NORMAL,
                                        IMSA_DEREG_LOCAL);

            IMSA_SRV_SndRegRegReq(      IMSA_REG_TYPE_NORMAL,
                                        IMSA_REG_ADDR_PARAM_NEW);

            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
            }
            break;
        case IMSA_RESOURCE_STATUS_APPLYING:
            /* 修改触发源为获取服务 */
            IMSA_DsdsModifyTaskSrcByTaskType(IMSA_RF_RES_APPLY_TRIGER_SRC_GET_NOR_SRV,
                                             RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
            break;
        #endif
        case IMSA_RESOURCE_STATUS_ACTIVE:

            IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE,IMSA_CONN_IsExitPersistentBearer());

            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_CONN_REGING);

            IMSA_SRV_SndRegDeregReq(    IMSA_REG_TYPE_NORMAL,
                                        IMSA_DEREG_LOCAL);

            IMSA_SRV_SndRegRegReq(      IMSA_REG_TYPE_NORMAL,
                                        IMSA_REG_ADDR_PARAM_NEW);

            #if (FEATURE_ON == FEATURE_DSDS)
            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                /* 修改触发源为获取服务 */
                IMSA_DsdsModifyTaskSrcByTaskType(IMSA_RF_RES_APPLY_TRIGER_SRC_GET_NOR_SRV,
                                                 RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);

                IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
                IMSA_StopTimer(&pstControlMagnaer->stDeregSerProtectTimer);
            }
            #endif
            break;

        default:
            break;
    }
    #if (FEATURE_ON == FEATURE_PTM)

    /* 记录UE发起注册的时间 */
    ( VOS_VOID )LPS_OM_GET32K_TIMESTAMP(&(IMSA_GetErrlogStartRegTimerAddr()->ul32KTStampHighBit),
                                        &(IMSA_GetErrlogStartRegTimerAddr()->ul32KTStampLowBit));
    #endif
}

VOS_VOID IMSA_SRV_ProcServiceChangeIndConnDereg
(
    VOS_VOID
)
{
    #if (FEATURE_ON == FEATURE_DSDS)
    IMSA_CONTROL_MANAGER_STRU                  *pstControlMagnaer   = VOS_NULL_PTR;

    pstControlMagnaer                       = IMSA_GetControlManagerAddress();
    #endif

    switch(IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESOURCE_STATUS_NULL:
            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_GET_NOR_SRV,
                                          IMSA_REG_ADDR_PARAM_NEW,
                                          IMSA_FALSE);
            break;

        case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:
            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_GET_NOR_SRV,
                                          IMSA_REG_ADDR_PARAM_NEW,
                                          IMSA_FALSE);

            /* 请求注册模块发起注册，将状态转到IMSA_SERVICE_STATUS_CONN_REGING */
            IMSA_SRV_SndRegRegReq(      IMSA_REG_TYPE_NORMAL,
                                        IMSA_REG_ADDR_PARAM_NEW);
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_CONN_REGING);

            IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
            break;
        #endif
        case IMSA_RESOURCE_STATUS_ACTIVE:

            /* 请求注册模块发起注册，将状态转到IMSA_SERVICE_STATUS_CONN_REGING */
            IMSA_SRV_SndRegRegReq(      IMSA_REG_TYPE_NORMAL,
                                        IMSA_REG_ADDR_PARAM_NEW);
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_CONN_REGING);
            #if (FEATURE_ON == FEATURE_DSDS)
            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
            }
            #endif
            break;
        default:
            break;
    }
    #if (FEATURE_ON == FEATURE_PTM)
    /* 记录UE发起注册的时间 */
    ( VOS_VOID )LPS_OM_GET32K_TIMESTAMP(&(IMSA_GetErrlogStartRegTimerAddr()->ul32KTStampHighBit),
                                        &(IMSA_GetErrlogStartRegTimerAddr()->ul32KTStampLowBit));
    #endif
}


VOS_VOID IMSA_SRV_ProcServiceChangeInd
(
    const MSCC_IMSA_SERVICE_CHANGE_IND_STRU     *pstServiceChangeInd
)
{
    IMSA_NETWORK_INFO_STRU                     *pstNwInfo           = VOS_NULL_PTR;
    IMSA_CONTROL_MANAGER_STRU                  *pstControlMagnaer   = VOS_NULL_PTR;
    IMSA_SRV_STATUS_ENUM_UINT8                  enNrmSrvStatus      = IMSA_SRV_STATUS_BUTT;
    VOS_UINT32                                  ulIsNotfiyNrmNoSrv  = IMSA_FALSE;
    VOS_UINT32                                  ulIsNotfiyEmcNoSrv  = IMSA_FALSE;
    IMSA_NRM_SRV_CON_SAT_STATUE_ENMUM_UINT32    enImsaSrvStatue     = IMSA_NRM_SRV_CON_SAT_STATUE_BUTT;

    IMSA_INFO_LOG("IMSA_SRV_ProcServiceChangeInd is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcServiceChangeInd_ENUM, LNAS_ENTRY);

    /* 存储SERVICE CHANGE IND消息中携带的参数 */
    pstNwInfo                               = IMSA_GetNetInfoAddress();
    pstControlMagnaer                       = IMSA_GetControlManagerAddress();

    pstNwInfo->enImsaPsServiceStatus        = pstServiceChangeInd->enPsServiceStatus;
    /* 之前的漫游标记是通过CAMP_INFO_CHANGE_IND发过来的，但是并不准确，需要SERVICE_CHANGE_IND来通知给IMSA
       新增一个比特位bitRoamingValid，当该比特位为1时，代表漫游标志有效。此时IMSA更新本地维护的漫游状态。*/
    if (VOS_TRUE == pstServiceChangeInd->bitRoamingValid)
    {
        pstNwInfo->ucImsaRoamingFlg             = pstServiceChangeInd->ucRoamingFlg;
    }

    pstControlMagnaer->ucImsaUsimStatus     = pstServiceChangeInd->ucPsSimValid;

    /* 接入技术更新 */
    IMSA_SRV_UpdateRat(pstServiceChangeInd->enRat, &ulIsNotfiyNrmNoSrv, &ulIsNotfiyEmcNoSrv);

    /* 只有是正常服务时，才更新网络支持IMS和EMS指示 */
    if (MSCC_IMSA_NORMAL_SERVICE == pstServiceChangeInd->enPsServiceStatus)
    {
        IMSA_SRV_UpdateImsVoPsStatus(pstServiceChangeInd->enImsVoPsInd);
        IMSA_SRV_UpdateEmsStatus(pstServiceChangeInd->enEmsInd);
    }

    enNrmSrvStatus = IMSA_SRV_GetNormalSrvStatus();

    /* 进入此分支，说明上次给TAF报的注册状态是未注册，目前只有在SRVCC快速挂断场景中会出现这种情况。 */
    if ((IMSA_SRV_STATUS_CONN_REG == enNrmSrvStatus) && (IMSA_NORMAL_REG_STATUS_LTE_REG != IMSA_GetLastNrmRegStatus()))
    {
        IMSA_SRV_NrmRegStatusNotify(IMSA_NORMAL_REG_STATUS_LTE_REG);

        IMSA_SetLastNrmRegStatus(IMSA_NORMAL_REG_STATUS_LTE_REG);
    }

    enImsaSrvStatue = IMSA_SRV_IsNrmSrvConditonSatisfied();

    if (VOS_FALSE== IMSA_SRV_PreProcNrmSrvConSatStatus(enImsaSrvStatue))
    {
        /* 勾出不发起注册的原因以及当前的状态 */
        IMSA_SndOmNoRegCauseInfo(enNrmSrvStatus, enImsaSrvStatue);
        return;
    }


    /* 根据当前服务状态进行不同处理 */
    switch (enNrmSrvStatus)
    {
        case IMSA_SRV_STATUS_IDLE_DEREG:

            /* 请求连接模块建立连接，将状态转到IMSA_SERVICE_STATUS_CONNING_DEREG */
            IMSA_SRV_ProcServiceChangeIndIdleDereg();
            break;

        case IMSA_SRV_STATUS_RELEASING_DEREG:

            /* 启动周期性尝试IMS服务定时器 */
            IMSA_StartTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer);
            break;

        case IMSA_SRV_STATUS_CONN_DEREGING:

            IMSA_SRV_ProcServiceChangeIndConnDereging();

            break;

        case IMSA_SRV_STATUS_CONN_DEREG:

            IMSA_SRV_ProcServiceChangeIndConnDereg();
            break;
        case IMSA_SRV_STATUS_CONNING_DEREG:
        case IMSA_SRV_STATUS_CONN_REGING:
        case IMSA_SRV_STATUS_CONNING_REG:
            /*这三个状态等注册完成后上报IMS voice能力*/
            break;
        default:
            /*IMSA不发起注册，需上报IMS voice能力*/
            if (IMSA_TRUE == IMSA_IsImsVoiceContidionSatisfied())
            {
                IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_AVAILABLE,IMSA_CONN_IsExitPersistentBearer());
            }
            else
            {
                IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE,IMSA_CONN_IsExitPersistentBearer());
            }

            break;
    }
}


VOS_VOID IMSA_SRV_ProcCampInfoChangeInd
(
    const MSCC_IMSA_CAMP_INFO_CHANGE_IND_STRU     *pstCampInfoChangeInd
)
{
    IMSA_NETWORK_INFO_STRU             *pstNwInfo           = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_SRV_ProcCampInfoChangeInd is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcCampInfoChangeInd_ENUM, LNAS_ENTRY);

    /* 存储SERVICE CHANGE IND消息中携带的参数 */
    pstNwInfo                       = IMSA_GetNetInfoAddress();

    pstNwInfo->enAccessType         = pstCampInfoChangeInd->enAccessType;
    pstNwInfo->usLac                = pstCampInfoChangeInd->usLac;
    pstNwInfo->usTac                = pstCampInfoChangeInd->usTac;
    IMSA_MEM_CPY_S( &pstNwInfo->stPlmnId,
                    sizeof(MSCC_IMSA_PLMN_ID_STRU),
                    &pstCampInfoChangeInd->stPlmnId,
                    sizeof(MSCC_IMSA_PLMN_ID_STRU));
    pstNwInfo->ulCellId             = pstCampInfoChangeInd->ulCellId;

    /* 如果当前在wifi下驻留，则不需要将CGI信息带给IMS */
    if (IMSA_IMS_DOMAIN_WIFI == IMSA_GetCurrentDomain())
    {
        return;
    }

    /* 配置CGI信息给IMS */
    IMSA_ConfigCgi2Ims();
}


VOS_VOID IMSA_SRV_ProcVoiceDomainChangeInd
(
    const MSCC_IMSA_VOICE_DOMAIN_CHANGE_IND_STRU     *pstVoiceDomainChangeInd
)
{
    IMSA_CONTROL_MANAGER_STRU      *pstControlManager;

    IMSA_INFO_LOG("IMSA_SRV_ProcVoiceDomainChangeInd is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcVoiceDomainChangeInd_ENUM, LNAS_ENTRY);

    pstControlManager = IMSA_GetControlManagerAddress();

    /* 存储VOICE DOMAIN CHANGE IND消息中携带的参数 */
    pstControlManager->stImsaConfigPara.enVoiceDomain = (IMSA_VOICE_DOMAIN_ENUM_UINT32)pstVoiceDomainChangeInd->enVoiceDomain;
}


VOS_VOID IMSA_SRV_ProcDeregReqConnReging(VOS_UINT8 ucLocalFlag)
{

    IMSA_CONTROL_MANAGER_STRU          *pstControlManager;

    pstControlManager = IMSA_GetControlManagerAddress();

    /* 如果需要发起本地去注册，则无需申请资源。 */
    if (VOS_TRUE == ucLocalFlag)
    {
        IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_RELEASING_DEREG);
        IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);
        /* coverity[unchecked_value] */
        IMSA_SRV_SndConnRelReq(IMSA_SRV_TYPE_NORMAL,
                               IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                               IMSA_GetCurrentDomain(),
                               IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL);
        IMSA_SndMsccMsgDeregCnf();
        return;
    }

    IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_CONN_DEREGING);

    /* 启动去注册等待定时器 */
    IMSA_StartTimer(&pstControlManager->stWaitDeregTimer);

    switch (IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        case IMSA_RESOURCE_STATUS_ACTIVE:

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_NOT_LOCAL);
            break;
        #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:

            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          VOS_FALSE,
                                          VOS_FALSE);

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_NOT_LOCAL);
            break;

        case IMSA_RESOURCE_STATUS_NULL:

            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          VOS_FALSE,
                                          VOS_FALSE);

            //IMSA_DsdsSaveMoSmsMsg(pstRpDataReq);
            break;
        #endif
        default:
            #if (FEATURE_ON == FEATURE_DSDS)
            IMSA_DsdsModifyTaskSrcByTaskType(IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                             RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
            #endif
            break;

    }
}

VOS_VOID IMSA_SRV_ProcDeregReqConningDeReg(VOS_VOID)
{

    IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_RELEASING_DEREG);

    switch (IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        case IMSA_RESOURCE_STATUS_ACTIVE:
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnRelReq(IMSA_SRV_TYPE_NORMAL,
                                   IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                   IMSA_GetCurrentDomain(),
                                   IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL);
            break;
        #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:

            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          VOS_FALSE,
                                          VOS_FALSE);

            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnRelReq(IMSA_SRV_TYPE_NORMAL,
                                   IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                   IMSA_GetCurrentDomain(),
                                   IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL);
            break;

        case  IMSA_RESOURCE_STATUS_NULL:

            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          VOS_FALSE,
                                          VOS_FALSE);

            //IMSA_DsdsSaveMoSmsMsg(pstRpDataReq);
            break;
        #endif
        default:
            #if (FEATURE_ON == FEATURE_DSDS)
            IMSA_DsdsModifyTaskSrcByTaskType(IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                             RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
            #endif
            break;

    }

}

VOS_VOID IMSA_SRV_ProcDeregReqConnReg(VOS_UINT8 ucLocalFlag)
{

    IMSA_CONTROL_MANAGER_STRU          *pstControlManager;

    pstControlManager = IMSA_GetControlManagerAddress();

    /* 如果需要发起本地去注册，则无需申请资源。 */
    if (VOS_TRUE == ucLocalFlag)
    {
        IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_RELEASING_DEREG);
        IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);
        /* coverity[unchecked_value] */
        IMSA_SRV_SndConnRelReq(IMSA_SRV_TYPE_NORMAL,
            IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
            IMSA_GetCurrentDomain(),
            IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL);
        IMSA_SndMsccMsgDeregCnf();
        return;
    }

    IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_CONN_DEREGING);

    /* 启动去注册等待定时器 */
    IMSA_StartTimer(&pstControlManager->stWaitDeregTimer);

    switch (IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        case IMSA_RESOURCE_STATUS_ACTIVE:

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_NOT_LOCAL);
            break;
        #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:

            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          VOS_FALSE,
                                          VOS_FALSE);

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_NOT_LOCAL);
            break;

        case  IMSA_RESOURCE_STATUS_NULL:

            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          VOS_FALSE,
                                          VOS_FALSE);

            //IMSA_DsdsSaveMoSmsMsg(pstRpDataReq);
            break;
        #endif
        default:
            /* */
            break;

    }
}


VOS_VOID IMSA_SRV_ProcDeregReq(VOS_UINT8 ucLocalFlag)
{
    IMSA_SRV_STATUS_ENUM_UINT8          enNrmSrvStatus      = IMSA_SRV_STATUS_BUTT;
    IMSA_CONTROL_MANAGER_STRU          *pstControlManager;

    enNrmSrvStatus = IMSA_SRV_GetNormalSrvStatus();
    pstControlManager = IMSA_GetControlManagerAddress();

    /* 由于要支持无卡紧急呼，所以不能紧急去注册 */

    /* 根据当前服务状态进行不同处理 */
    switch (enNrmSrvStatus)
    {
        case IMSA_SRV_STATUS_CONN_REGING:
            IMSA_SRV_ProcDeregReqConnReging(ucLocalFlag);
            break;

        /* 已经去注册，直接回复去注册成功，并去激活承载 */
        case IMSA_SRV_STATUS_CONNING_DEREG:
        case IMSA_SRV_STATUS_CONN_DEREG:
        case IMSA_SRV_STATUS_HO_DEREG:
            IMSA_SRV_ProcDeregReqConningDeReg();
            IMSA_SndMsccMsgDeregCnf();
            break;

        /* 需要通知IMS允许其发包，通知发起本地注册 */
        case IMSA_SRV_STATUS_CONNING_REG:
        case IMSA_SRV_STATUS_HO_REG:
            IMSA_SndImsMsgServiceSuspendOrResumeSrvInfo(IMSA_IMS_INPUT_SERVICE_REASON_RESUME_NRM_SRV);
            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnRelReq(IMSA_SRV_TYPE_NORMAL,
                                   IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                   IMSA_GetCurrentDomain(),
                                   IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL);
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_IDLE_DEREG);
            IMSA_SndMsccMsgDeregCnf();

            /*释放资源 */
            #if (FEATURE_ON == FEATURE_DSDS)
            IMSA_DsdsProcResourceRelease(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
            #endif
            break;

        case IMSA_SRV_STATUS_CONN_REG:
            IMSA_SRV_ProcDeregReqConnReg(ucLocalFlag);
            break;

        /* 已经去注册，直接回复去注册成功 */
        case IMSA_SRV_STATUS_RELEASING_DEREG:
        case IMSA_SRV_STATUS_IDLE_DEREG:
            IMSA_SndMsccMsgDeregCnf();
            break;

        /* 去注册过程中，启动等待去注册结果定时器 */
        case IMSA_SRV_STATUS_CONN_DEREGING:
            /* 启动去注册等待定时器 */
            IMSA_StartTimer(&pstControlManager->stWaitDeregTimer);
            break;

        default:
            /* 释放资源 */
            #if (FEATURE_ON == FEATURE_DSDS)
            IMSA_DsdsProcResourceRelease(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
            #endif
            break;
    }
    return ;
}


VOS_VOID IMSA_SRV_DeregProcReceivedUsimReadfileCnfConnReging(VOS_VOID)
{
    IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_CONN_DEREGING);

    switch (IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        case IMSA_RESOURCE_STATUS_ACTIVE:

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_NOT_LOCAL);
            break;
        #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:

            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          VOS_FALSE,
                                          VOS_FALSE);

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_NOT_LOCAL);
            break;

        case IMSA_RESOURCE_STATUS_NULL:

            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          VOS_FALSE,
                                          VOS_FALSE);

            //IMSA_DsdsSaveMoSmsMsg(pstRpDataReq);
            break;
        #endif
        default:
            #if (FEATURE_ON == FEATURE_DSDS)
            IMSA_DsdsModifyTaskSrcByTaskType(IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                             RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
            #endif
            break;

    }
}

VOS_VOID IMSA_SRV_DeregProcReceivedUsimReadfileCnfConningDeReg(VOS_VOID)
{

    IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_RELEASING_DEREG);

    switch (IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        case IMSA_RESOURCE_STATUS_ACTIVE:
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnRelReq(IMSA_SRV_TYPE_NORMAL,
                                   IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                   IMSA_GetCurrentDomain(),
                                   IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL);
            break;
        #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:

            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          VOS_FALSE,
                                          VOS_FALSE);

            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnRelReq(IMSA_SRV_TYPE_NORMAL,
                                   IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                   IMSA_GetCurrentDomain(),
                                   IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL);
            break;

        case  IMSA_RESOURCE_STATUS_NULL:

            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          VOS_FALSE,
                                          VOS_FALSE);

            //IMSA_DsdsSaveMoSmsMsg(pstRpDataReq);
            break;
        #endif
        default:
            #if (FEATURE_ON == FEATURE_DSDS)
            IMSA_DsdsModifyTaskSrcByTaskType(IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                             RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
            #endif
            break;

    }

}

VOS_VOID IMSA_SRV_DeregProcReceivedUsimReadfileCnfConnReg(VOS_VOID)
{
    IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_CONN_DEREGING);

    switch (IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        case IMSA_RESOURCE_STATUS_ACTIVE:

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_NOT_LOCAL);
            break;
        #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:

            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          VOS_FALSE,
                                          VOS_FALSE);

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_NOT_LOCAL);
            break;

        case  IMSA_RESOURCE_STATUS_NULL:

            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          VOS_FALSE,
                                          VOS_FALSE);

            //IMSA_DsdsSaveMoSmsMsg(pstRpDataReq);
            break;
        #endif
        default:
            /* */
            break;

    }
}


VOS_VOID IMSA_SRV_DeregProcReceivedUsimReadfileCnf(VOS_VOID)
{
    IMSA_SRV_STATUS_ENUM_UINT8          enNrmSrvStatus      = IMSA_SRV_STATUS_BUTT;

    enNrmSrvStatus = IMSA_SRV_GetNormalSrvStatus();

    /* 由于要支持无卡紧急呼，所以不能紧急去注册 */

    /* 根据当前服务状态进行不同处理 */
    switch (enNrmSrvStatus)
    {
        case IMSA_SRV_STATUS_CONN_REGING:
            IMSA_SRV_DeregProcReceivedUsimReadfileCnfConnReging();
            break;

        /* 已经去注册，直接回复去注册成功，并去激活承载 */
        case IMSA_SRV_STATUS_CONNING_DEREG:
        case IMSA_SRV_STATUS_CONN_DEREG:
            IMSA_SRV_DeregProcReceivedUsimReadfileCnfConningDeReg();
            break;

        case IMSA_SRV_STATUS_CONNING_REG:
            /* 需要通知IMS允许其发包，通知发起本地注册 */
            IMSA_SndImsMsgServiceSuspendOrResumeSrvInfo(IMSA_IMS_INPUT_SERVICE_REASON_RESUME_NRM_SRV);
            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_IDLE_DEREG);

            /*释放资源 */
            #if (FEATURE_ON == FEATURE_DSDS)
            IMSA_DsdsProcResourceRelease(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
            #endif
            break;

        case IMSA_SRV_STATUS_CONN_REG:
            IMSA_SRV_DeregProcReceivedUsimReadfileCnfConnReg();
            break;

        /* 去注册过程中，启动等待去注册结果定时器 */
        /* 此处流程待确认 */
        case IMSA_SRV_STATUS_CONN_DEREGING:
            break;

        default:
            /* 释放资源 */
            #if (FEATURE_ON == FEATURE_DSDS)
            IMSA_DsdsProcResourceRelease(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
            #endif
            break;
    }
    return ;
}


VOS_VOID IMSA_SRV_RegProcReceivedUsimReadfileCnf(VOS_VOID)
{
    IMSA_SRV_STATUS_ENUM_UINT8          enNrmSrvStatus      = IMSA_SRV_STATUS_BUTT;
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;

    pstControlMagnaer                       = IMSA_GetControlManagerAddress();


    enNrmSrvStatus = IMSA_SRV_GetNormalSrvStatus();

    /* 如果当前不满足发起注册的条件，则返回 */
    if (IMSA_NRM_SRV_CON_SAT_STATUE_SUCC != IMSA_SRV_IsNrmSrvConditonSatisfied())
    {
        return;
    }

    /* 根据当前服务状态进行不同处理 */
    switch (enNrmSrvStatus)
    {
        case IMSA_SRV_STATUS_IDLE_DEREG:

            /* 请求连接模块建立连接，将状态转到IMSA_SERVICE_STATUS_CONNING_DEREG */
            IMSA_SRV_ProcServiceChangeIndIdleDereg();
            break;

        case IMSA_SRV_STATUS_RELEASING_DEREG:

            /* 启动周期性尝试IMS服务定时器 */
            IMSA_StartTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer);
            break;

        case IMSA_SRV_STATUS_CONN_REGING:
        case IMSA_SRV_STATUS_CONN_REG:
            /* 此处调用的是CONN_DEREGING状态下的处理，因为卡文件刷新后需要发起一个去本地注册流程 */
            IMSA_SRV_ProcServiceChangeIndConnDereging();

            break;

        case IMSA_SRV_STATUS_HO_REG:
            /* 切换过程中，无法发起注册，发起本地去注册，等下一次域选 */
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_IDLE_DEREG);
            IMSA_SRV_SndRegDeregReq(    IMSA_REG_TYPE_NORMAL,
                                        IMSA_DEREG_LOCAL);
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnRelReq(     IMSA_SRV_TYPE_NORMAL,
                                        IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                        IMSA_GetCurrentDomain(),
                                        IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL);
            break;

        case IMSA_SRV_STATUS_CONN_DEREG:

            IMSA_SRV_ProcServiceChangeIndConnDereg();
            break;

        default:

            /*这三个状态等注册完成后上报IMS voice能力*/
            break;

    }
    return;
}


VOS_UINT32 IMSA_StartImsNormalService
(
    IMSA_CALL_TYPE_ENUM_UINT8               enCallType
)
{
    /* 如果当前已经有一路电话，说明下面的条件都已经判断过一次了。
       对于服务状态和涉及NV的标志，在有呼叫时是不可能改变的；
       对于网侧的状态，由于当前已经有一路电话，即使网侧能力发生改变，
       IMSA也不会发起去注册流程，故界面显示仍然是VoLTE可用。
       故只需要对呼叫类型进行判断，其他条件不做判断 */
    if ((IMSA_CALL_TYPE_VOICE == enCallType)
        && (VOS_FALSE == IMSA_GetUeImsVoiceCap()))
    {
        IMSA_INFO_LOG("IMSA_StartImsNormalService: 3!");

        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_CallFailErrRecord( enCallType,
                                IMSA_ERR_LOG_NORM_CALL_FAIL_REASON_UE_CAP_NOT_SUPPORT_VOICE_CALL,
                                IMSA_ERR_LOG_EMC_CALL_FAIL_REASON_NONE,
                                VOS_NULL_PTR,
                                VOS_FALSE);
        #endif
        return IMSA_START_SRV_RESULT_CANNOT_MAKE_CALL;
    }

    if ((IMSA_CALL_TYPE_VIDEO == enCallType)
        && (VOS_FALSE == IMSA_GetUeImsVideoCap()))
    {
        IMSA_INFO_LOG("IMSA_StartImsNormalService: 4!");

        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_CallFailErrRecord( enCallType,
                                IMSA_ERR_LOG_NORM_CALL_FAIL_REASON_UE_CAP_NOT_SUPPORT_VIDEO_CALL,
                                IMSA_ERR_LOG_EMC_CALL_FAIL_REASON_NONE,
                                VOS_NULL_PTR,
                                VOS_FALSE);
        #endif
        return IMSA_START_SRV_RESULT_CANNOT_MAKE_CALL;
    }

    if (VOS_TRUE == IMSA_IsCallConnExist())
    {
        IMSA_INFO_LOG("IMSA_StartImsNormalService: 5!");

        return IMSA_START_SRV_RESULT_CAN_MAKE_CALL;
    }

    if (IMSA_SRV_STATUS_CONN_REG != IMSA_SRV_GetNormalSrvStatus())
    {
        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_CallFailErrRecord( enCallType,
                                IMSA_ERR_LOG_NORM_CALL_FAIL_REASON_NORM_SER_NOT_CONN_REG,
                                IMSA_ERR_LOG_EMC_CALL_FAIL_REASON_NONE,
                                VOS_NULL_PTR,
                                VOS_FALSE);
        #endif
        return IMSA_START_SRV_RESULT_CANNOT_MAKE_CALL;
    }

    if (IMSA_VOICE_DOMAIN_CS_ONLY == IMSA_GetVoiceDomain())
    {
        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_CallFailErrRecord( enCallType,
                                IMSA_ERR_LOG_NORM_CALL_FAIL_REASON_VOICE_DOMAIN_CS_ONLY,
                                IMSA_ERR_LOG_EMC_CALL_FAIL_REASON_NONE,
                                VOS_NULL_PTR,
                                VOS_FALSE);
        #endif
        return IMSA_START_SRV_RESULT_CANNOT_MAKE_CALL;
    }

    if ((IMSA_IMS_VOPS_STATUS_NOT_SUPPORT == IMSA_GetNwImsVoiceCap())&& (IMSA_IMS_DOMAIN_LTE == IMSA_GetCurrentDomain()))
    {
        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_CallFailErrRecord( enCallType,
                                IMSA_ERR_LOG_NORM_CALL_FAIL_REASON_IMS_VOPS_STATUS_NOT_SUPPORT,
                                IMSA_ERR_LOG_EMC_CALL_FAIL_REASON_NONE,
                                VOS_NULL_PTR,
                                VOS_FALSE);
        #endif
        return IMSA_START_SRV_RESULT_CANNOT_MAKE_CALL;
    }

    return IMSA_START_SRV_RESULT_CAN_MAKE_CALL;
}


VOS_UINT32 IMSA_StartImsEmergServiceAbilityCheck
(
    IMSA_EMC_CALL_TYPE_ENUM_UINT32     *penEmcCallType
)
{
    IMSA_NETWORK_INFO_STRU             *pstImsaNwInfo       = VOS_NULL_PTR;

    IMSA_SRV_STATUS_ENUM_UINT8          enEmcSrvStatus      = IMSA_SRV_STATUS_BUTT;

    IMSA_INFO_LOG("IMSA_StartImsEmergService is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_StartImsEmergService_ENUM, LNAS_ENTRY);

    /* 清除无卡紧急呼标志 */
    IMSA_SRV_GetNoRegEmcFlag() = VOS_FALSE;


    /* 判断获取紧急IMS服务的条件是否具备，如果不具备，则返回不能打紧急呼 */
    if (IMSA_TRUE != IMSA_SRV_IsEmcSrvConditonSatisfied())
    {
        IMSA_INFO_LOG("IMSA_StartImsEmergService:EMC service condition not satisfied!");
        TLPS_PRINT2LAYER_INFO(IMSA_StartImsEmergService_ENUM, 1);
        return IMSA_START_SRV_RESULT_CANNOT_MAKE_CALL;
    }

    /* 如果可以在正常服务上发起紧急呼，则直接返回能打紧急呼 */
    if (IMSA_TRUE == IMSA_SRV_IsMakeEmcCallInNrmSrvSatisfied())
    {
        IMSA_INFO_LOG("IMSA_StartImsEmergService:can make emc call in normal service!");
        TLPS_PRINT2LAYER_INFO(IMSA_StartImsEmergService_ENUM, 2);
        *penEmcCallType = IMSA_EMC_CALL_TYPE_IN_NORMAL_SRV;
        return IMSA_START_SRV_RESULT_CAN_MAKE_CALL;
    }

    enEmcSrvStatus      = IMSA_SRV_GetEmcSrvStatus();
    pstImsaNwInfo       = IMSA_GetNetInfoAddress();

    /* 根据当前服务状态进行不同处理 */
    switch (enEmcSrvStatus)
    {
        case IMSA_SRV_STATUS_IDLE_DEREG:

            break;

        case IMSA_SRV_STATUS_RELEASING_DEREG:

            return IMSA_START_SRV_RESULT_CANNOT_MAKE_CALL;

        case IMSA_SRV_STATUS_CONN_DEREG:

            /* 如果是无卡紧急呼，则直接返回能打紧急呼 */
            /* 无卡紧急呼包含两种情况，无卡+无服务，和有卡+受限服务/无服务 */
            if (IMSA_PS_SERVICE_STATUS_NORMAL_SERVICE != pstImsaNwInfo->enImsaPsServiceStatus)
            {
                IMSA_INFO_LOG("IMSA_StartImsEmergService:can make anonymous emc call in conn dereg!");
                TLPS_PRINT2LAYER_INFO(IMSA_StartImsEmergService_ENUM, IMSA_STATUS_CONN);
                TLPS_PRINT2LAYER_INFO(IMSA_StartImsEmergService_ENUM, IMSA_STATUS_NOT_REGISTER);
                *penEmcCallType = IMSA_EMC_CALL_TYPE_NO_REG;
                return IMSA_START_SRV_RESULT_CAN_MAKE_CALL;
            }

            break;

        case IMSA_SRV_STATUS_CONN_REG:

            *penEmcCallType = IMSA_EMC_CALL_TYPE_EMC_CONN_EMC_REG;
            return IMSA_START_SRV_RESULT_CAN_MAKE_CALL;

        default:
            break;
    }


    if (IMSA_PS_SERVICE_STATUS_NORMAL_SERVICE != pstImsaNwInfo->enImsaPsServiceStatus)
    {
        /* 需要记录当前的无卡紧急呼流程 */
        IMSA_SRV_GetNoRegEmcFlag() = VOS_TRUE;
        *penEmcCallType = IMSA_EMC_CALL_TYPE_NO_REG;
    }
    else
    {
        *penEmcCallType = IMSA_EMC_CALL_TYPE_EMC_CONN_EMC_REG;
    }


    return IMSA_START_SRV_RESULT_BUFFER_CALL_WAIT_INDICATION;
}

VOS_VOID IMSA_StartImsEmcService
(
    VOS_VOID
)
{
    IMSA_SRV_STATUS_ENUM_UINT8          enEmcSrvStatus      = IMSA_SRV_STATUS_BUTT;

    enEmcSrvStatus      = IMSA_SRV_GetEmcSrvStatus();

    /* 根据当前服务状态进行不同处理 */
    switch (enEmcSrvStatus)
    {
        case IMSA_SRV_STATUS_IDLE_DEREG:

            /* 请求连接模块建立连接，将状态转到IMSA_SERVICE_STATUS_CONNING_DEREG */
            IMSA_SRV_SndConnSetupReq(   IMSA_FALSE,
                                        IMSA_CONN_TYPE_EMC,
                                        IMSA_IMS_DOMAIN_LTE );
            IMSA_SRV_SetServcieStatus(  IMSA_CONN_TYPE_EMC,
                                        IMSA_SRV_STATUS_CONNING_DEREG);
            break;

        case IMSA_SRV_STATUS_CONN_DEREG:

            /* 请求注册模块发起注册，将状态转到IMSA_SERVICE_STATUS_CONN_REGING */
            IMSA_SRV_SndRegRegReq(      IMSA_CONN_TYPE_EMC,
                                        IMSA_REG_ADDR_PARAM_NEW);
            IMSA_SRV_SetServcieStatus(  IMSA_CONN_TYPE_EMC,
                                        IMSA_SRV_STATUS_CONN_REGING);
            break;


        default:
            break;
    }

    return;
}



/* 该函数已经没有被调用的点，故删除 */


VOS_VOID IMSA_ProcTimerMsgPeriodTryImsSrvExpIdleDereg(VOS_VOID)
{
    #if (FEATURE_ON == FEATURE_DSDS)
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;

    pstControlMagnaer                       = IMSA_GetControlManagerAddress();
    #endif

    if (IMSA_NRM_SRV_CON_SAT_STATUE_SUCC != IMSA_SRV_IsNrmSrvConditonSatisfied())
    {
        return;
    }

    switch (IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        case IMSA_RESOURCE_STATUS_ACTIVE:
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnSetupReq(   IMSA_FALSE,
                                        IMSA_CONN_TYPE_NORMAL,
                                        IMSA_GetCurrentDomain());
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_CONNING_DEREG);
            #if (FEATURE_ON == FEATURE_DSDS)
            if (PS_RRM_RF_NOT_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
            }
            #endif
            break;
        #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESOURCE_STATUS_NULL:
            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                       IMSA_RF_RES_APPLY_TRIGER_SRC_GET_NOR_SRV,
                                       IMSA_FALSE,
                                       IMSA_FALSE);
            break;

        case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:

           IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                       IMSA_RF_RES_APPLY_TRIGER_SRC_GET_NOR_SRV,
                                       IMSA_FALSE,
                                       IMSA_FALSE);
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnSetupReq( IMSA_FALSE,
                                      IMSA_CONN_TYPE_NORMAL,
                                      IMSA_GetCurrentDomain());
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_CONNING_DEREG);

            IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
            break;
        #endif
        default:
            break;
    }
    return;
}


VOS_VOID IMSA_ProcTimerMsgPeriodTryImsSrvExpConnDereg(VOS_VOID)
{
    #if (FEATURE_ON == FEATURE_DSDS)
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;
    #endif
    IMSA_NRM_SRV_CON_SAT_STATUE_ENMUM_UINT32    enSrvRst    = IMSA_NRM_SRV_CON_SAT_STATUE_BUTT;

    #if (FEATURE_ON == FEATURE_DSDS)
    pstControlMagnaer                       = IMSA_GetControlManagerAddress();
    #endif
    enSrvRst                                = IMSA_SRV_IsNrmSrvConditonSatisfied();

    if (IMSA_NRM_SRV_CON_SAT_STATUE_SUCC != enSrvRst)
    {
        return;
    }

    switch (IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        case IMSA_RESOURCE_STATUS_ACTIVE:
            IMSA_SRV_SndRegRegReq(      IMSA_REG_TYPE_NORMAL,
                                    IMSA_REG_ADDR_PARAM_NEXT);
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_CONN_REGING);
            #if (FEATURE_ON == FEATURE_DSDS)
            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
            }
            #endif
            break;
        #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESOURCE_STATUS_NULL:
            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                       IMSA_RF_RES_APPLY_TRIGER_SRC_GET_NOR_SRV,
                                       IMSA_REG_ADDR_PARAM_NEXT,
                                       IMSA_FALSE);
            break;

        case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:

           IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                       IMSA_RF_RES_APPLY_TRIGER_SRC_GET_NOR_SRV,
                                       IMSA_REG_ADDR_PARAM_NEXT,
                                       IMSA_FALSE);

            IMSA_SRV_SndRegRegReq(      IMSA_REG_TYPE_NORMAL,
                                        IMSA_REG_ADDR_PARAM_NEXT);
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_CONN_REGING);

            IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
            break;
        #endif
        default:
            break;
    }

}

VOS_VOID IMSA_ProcTimerMsgPeriodTryImsSrvExpConningReg(VOS_VOID)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;
    IMSA_NRM_SRV_CON_SAT_STATUE_ENMUM_UINT32    enSrvStatus = IMSA_NRM_SRV_CON_SAT_STATUE_BUTT;

    pstControlMagnaer                   = IMSA_GetControlManagerAddress();
    enSrvStatus                         = IMSA_SRV_IsNrmSrvConditonSatisfied();


    if (IMSA_NRM_SRV_CON_SAT_STATUE_SUCC != enSrvStatus)
    {
        IMSA_SndImsMsgServiceSuspendOrResumeSrvInfo(IMSA_IMS_INPUT_SERVICE_REASON_RESUME_NRM_SRV);

        IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);
        IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_IDLE_DEREG);

        IMSA_SRV_SndDomainSrvStatusInd(IMSA_DOMAIN_SERVICE_STATUS_NO_SERVICE,
                                       IMSA_DOMAIN_NO_SRV_CAUSE_NW_NOT_SUPPORT_IMS_VOICE,
                                       IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY);

        /* 上报状态给Domain，Domain会发起域选过程 */
        if( VOS_TRUE == pstControlMagnaer->ucDomainRegSrvStatusFlag)
        {
            pstControlMagnaer->ucDomainRegSrvStatusFlag = VOS_FALSE;
            IMSA_SRV_SndDomainSrvReadyInd();
        }

        /* 记录发起去注册的原因值 */
        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_SRV_InitDeregEventReport(IMSA_ERR_LOG_INIT_DEREG_REASON_PDN_LOST);
        #endif
        return;
    }

    switch (IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        case IMSA_RESOURCE_STATUS_ACTIVE:
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnSetupReq(   IMSA_TRUE,
                                        IMSA_CONN_TYPE_NORMAL,
                                        IMSA_GetCurrentDomain());

            /* 启动注册时长保护定时器 */
            #if (FEATURE_ON == FEATURE_DSDS)
            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
            }
            #endif
            break;
        #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESOURCE_STATUS_NULL:
            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                       IMSA_RF_RES_APPLY_TRIGER_SRC_GET_NOR_SRV,
                                       IMSA_FALSE,
                                       IMSA_FALSE);
            break;

        case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:

           IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                       IMSA_RF_RES_APPLY_TRIGER_SRC_GET_NOR_SRV,
                                       IMSA_FALSE,
                                       IMSA_FALSE);
           /* coverity[unchecked_value] */
           IMSA_SRV_SndConnSetupReq(   IMSA_TRUE,
                                        IMSA_CONN_TYPE_NORMAL,
                                        IMSA_GetCurrentDomain());

            /* 启动注册时长保护定时器 */
            #if (FEATURE_ON == FEATURE_DSDS)
            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
            }
            #endif
            break;
        #endif
        default:
            break;
    }
    /* 记录UE发起建链的时间 */
    #if (FEATURE_ON == FEATURE_PTM)
    ( VOS_VOID )LPS_OM_GET32K_TIMESTAMP(&(IMSA_GetErrlogStartConnTimerAddr()->ul32KTStampHighBit),
                                        &(IMSA_GetErrlogStartConnTimerAddr()->ul32KTStampLowBit));
    #endif
}


VOS_VOID IMSA_ProcTimerMsgPeriodTryImsSrvExpLteDoamin(const VOS_VOID *pRcvMsg )
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;
    (void)pRcvMsg;

    IMSA_INFO_LOG("IMSA_ProcTimerMsgPeriodTryImsSrvExp is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_ProcTimerMsgPeriodTryImsSrvExp_ENUM, LNAS_ENTRY);

    pstControlMagnaer  = IMSA_GetControlManagerAddress();

    switch (IMSA_SRV_GetNormalSrvStatus())
    {
        case IMSA_SRV_STATUS_CONNING_DEREG:

            IMSA_INFO_LOG("IMSA_ProcTimerMsgPeriodTryImsSrvExp:conning&dereg or conn&reging!");
            TLPS_PRINT2LAYER_INFO(IMSA_ProcTimerMsgPeriodTryImsSrvExp_ENUM, IMSA_STATUS_CONNECTING);
            TLPS_PRINT2LAYER_INFO(IMSA_ProcTimerMsgPeriodTryImsSrvExp_ENUM, IMSA_STATUS_NOT_REGISTER);

            IMSA_StopTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer);
            break;

        case IMSA_SRV_STATUS_RELEASING_DEREG:

            IMSA_INFO_LOG("IMSA_ProcTimerMsgPeriodTryImsSrvExp:releasing&dereg!");
            TLPS_PRINT2LAYER_INFO(IMSA_ProcTimerMsgPeriodTryImsSrvExp_ENUM, IMSA_STATUS_RELEASING);
            TLPS_PRINT2LAYER_INFO(IMSA_ProcTimerMsgPeriodTryImsSrvExp_ENUM, IMSA_STATUS_NOT_REGISTER);

            IMSA_StartTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer);
            break;

        case IMSA_SRV_STATUS_IDLE_DEREG:

            IMSA_INFO_LOG("IMSA_ProcTimerMsgPeriodTryImsSrvExp:idle&dereg!");
            TLPS_PRINT2LAYER_INFO(IMSA_ProcTimerMsgPeriodTryImsSrvExp_ENUM, IMSA_STATUS_IDLE);
            TLPS_PRINT2LAYER_INFO(IMSA_ProcTimerMsgPeriodTryImsSrvExp_ENUM, IMSA_STATUS_NOT_REGISTER);

            IMSA_ProcTimerMsgPeriodTryImsSrvExpIdleDereg();

            break;
        case IMSA_SRV_STATUS_CONN_DEREG:

            IMSA_INFO_LOG("IMSA_ProcTimerMsgPeriodTryImsSrvExp:conn&dereg!");
            TLPS_PRINT2LAYER_INFO(IMSA_ProcTimerMsgPeriodTryImsSrvExp_ENUM, IMSA_STATUS_CONN);
            TLPS_PRINT2LAYER_INFO(IMSA_ProcTimerMsgPeriodTryImsSrvExp_ENUM, IMSA_STATUS_NOT_REGISTER);

            IMSA_ProcTimerMsgPeriodTryImsSrvExpConnDereg();

            break;
        case IMSA_SRV_STATUS_CONNING_REG:

            IMSA_INFO_LOG("IMSA_ProcTimerMsgPeriodTryImsSrvExp:conning&reg!");
            TLPS_PRINT2LAYER_INFO(IMSA_ProcTimerMsgPeriodTryImsSrvExp_ENUM, IMSA_STATUS_CONNECTING);
            TLPS_PRINT2LAYER_INFO(IMSA_ProcTimerMsgPeriodTryImsSrvExp_ENUM, IMSA_STATUS_REGISTERED);
            IMSA_ProcTimerMsgPeriodTryImsSrvExpConningReg();

            break;
        default:
            IMSA_INFO_LOG("IMSA_ProcTimerMsgPeriodTryImsSrvExp:other states!");
            TLPS_PRINT2LAYER_INFO(IMSA_ProcTimerMsgPeriodTryImsSrvExp_ENUM, 1);
            break;
    }
}

VOS_VOID IMSA_ProcTimerMsgPeriodTryImsSrvExpWifiDoamin(const VOS_VOID *pRcvMsg )
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;
    (void)pRcvMsg;

    IMSA_INFO_LOG("IMSA_ProcTimerMsgPeriodTryImsSrvExp is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_ProcTimerMsgPeriodTryImsSrvExp_ENUM, LNAS_ENTRY);

    pstControlMagnaer  = IMSA_GetControlManagerAddress();

    switch (IMSA_SRV_GetNormalSrvStatus())
    {
        case IMSA_SRV_STATUS_CONNING_DEREG:

            IMSA_INFO_LOG("IMSA_ProcTimerMsgPeriodTryImsSrvExp:conning&dereg or conn&reging!");
            TLPS_PRINT2LAYER_INFO(IMSA_ProcTimerMsgPeriodTryImsSrvExp_ENUM, IMSA_STATUS_CONNECTING);
            TLPS_PRINT2LAYER_INFO(IMSA_ProcTimerMsgPeriodTryImsSrvExp_ENUM, IMSA_STATUS_NOT_REGISTER);

            IMSA_StopTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer);
            break;

        case IMSA_SRV_STATUS_RELEASING_DEREG:

            IMSA_INFO_LOG("IMSA_ProcTimerMsgPeriodTryImsSrvExp:releasing&dereg!");
            TLPS_PRINT2LAYER_INFO(IMSA_ProcTimerMsgPeriodTryImsSrvExp_ENUM, IMSA_STATUS_RELEASING);
            TLPS_PRINT2LAYER_INFO(IMSA_ProcTimerMsgPeriodTryImsSrvExp_ENUM, IMSA_STATUS_NOT_REGISTER);

            IMSA_StartTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer);
            break;

        case IMSA_SRV_STATUS_IDLE_DEREG:

            IMSA_INFO_LOG("IMSA_ProcTimerMsgPeriodTryImsSrvExp:idle&dereg!");
            TLPS_PRINT2LAYER_INFO(IMSA_ProcTimerMsgPeriodTryImsSrvExp_ENUM, IMSA_STATUS_IDLE);
            TLPS_PRINT2LAYER_INFO(IMSA_ProcTimerMsgPeriodTryImsSrvExp_ENUM, IMSA_STATUS_NOT_REGISTER);

            /* 如果重新获取IMS普通服务的条件具备，则发起普通连接建立 */
            if (IMSA_NRM_SRV_CON_SAT_STATUE_SUCC == IMSA_SRV_IsWifiNrmSrvConditonSatisfied())
            {
                /* 请求连接模块建立连接，将状态转到IMSA_SERVICE_STATUS_CONNING_DEREG */
                IMSA_SRV_SndConnSetupReq(   IMSA_FALSE,
                                            IMSA_CONN_TYPE_NORMAL,
                                            IMSA_IMS_DOMAIN_WIFI);
                IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                            IMSA_SRV_STATUS_CONNING_DEREG);
            }

            break;
        case IMSA_SRV_STATUS_CONN_DEREG:

            IMSA_INFO_LOG("IMSA_ProcTimerMsgPeriodTryImsSrvExp:conn&dereg!");
            TLPS_PRINT2LAYER_INFO(IMSA_ProcTimerMsgPeriodTryImsSrvExp_ENUM, IMSA_STATUS_CONN);
            TLPS_PRINT2LAYER_INFO(IMSA_ProcTimerMsgPeriodTryImsSrvExp_ENUM, IMSA_STATUS_NOT_REGISTER);

            /* 如果重新获取IMS普通服务的条件具备，则发起普通注册请求 */
            if (IMSA_NRM_SRV_CON_SAT_STATUE_SUCC == IMSA_SRV_IsWifiNrmSrvConditonSatisfied())
            {
                /* 请求注册模块发起注册，将状态转到IMSA_SERVICE_STATUS_CONN_REGING */
                IMSA_SRV_SndRegRegReq(      IMSA_REG_TYPE_NORMAL,
                                            IMSA_REG_ADDR_PARAM_NEW);
                IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                            IMSA_SRV_STATUS_CONN_REGING);
            }

            break;

        case IMSA_SRV_STATUS_CONNING_REG:

            IMSA_INFO_LOG("IMSA_ProcTimerMsgPeriodTryImsSrvExp:conning&reg!");
            TLPS_PRINT2LAYER_INFO(IMSA_ProcTimerMsgPeriodTryImsSrvExp_ENUM, IMSA_STATUS_CONNECTING);
            TLPS_PRINT2LAYER_INFO(IMSA_ProcTimerMsgPeriodTryImsSrvExp_ENUM, IMSA_STATUS_REGISTERED);
            /* 如果重新获取IMS普通服务的条件具备，则发起普通注册请求 */
            if (IMSA_NRM_SRV_CON_SAT_STATUE_SUCC == IMSA_SRV_IsWifiNrmSrvConditonSatisfied())
            {
                /* 请求连接模块建立连接，将状态转到IMSA_SERVICE_STATUS_CONNING_DEREG */
                IMSA_SRV_SndConnSetupReq(   IMSA_FALSE,
                                            IMSA_CONN_TYPE_NORMAL,
                                            IMSA_IMS_DOMAIN_WIFI);
            }
            else
            {
                IMSA_SndImsMsgServiceSuspendOrResumeSrvInfo(IMSA_IMS_INPUT_SERVICE_REASON_RESUME_NRM_SRV);

                IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);
                IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_IDLE_DEREG);
                IMSA_SRV_SndDomainSrvStatusInd(IMSA_DOMAIN_SERVICE_STATUS_NO_SERVICE,
                                                IMSA_DOMAIN_NO_SRV_CAUSE_NW_NOT_SUPPORT_IMS_VOICE,
                                                IMSA_IMS_DOMAIN_MODE_WIFI_ONLY);

                /* 上报状态给Domain，Domain会发起域选过程 */
                if( VOS_TRUE == pstControlMagnaer->ucDomainRegSrvStatusFlag)
                {
                    pstControlMagnaer->ucDomainRegSrvStatusFlag = VOS_FALSE;
                    IMSA_SRV_SndDomainSrvReadyInd();
                }
            }
            break;

        default:
            IMSA_INFO_LOG("IMSA_ProcTimerMsgPeriodTryImsSrvExp:other states!");
            TLPS_PRINT2LAYER_INFO(IMSA_ProcTimerMsgPeriodTryImsSrvExp_ENUM, 1);
            break;
    }
}



VOS_VOID IMSA_ProcTimerMsgPeriodTryImsSrvExp(const VOS_VOID *pRcvMsg )
{

    /* 保护存在异常的情况 */
    if(IMSA_IMS_DOMAIN_LTE == IMSA_GetCurrentDomain())
    {
        IMSA_ProcTimerMsgPeriodTryImsSrvExpLteDoamin(pRcvMsg);
    }
    else if( IMSA_IMS_DOMAIN_WIFI == IMSA_GetCurrentDomain())
    {
        IMSA_ProcTimerMsgPeriodTryImsSrvExpWifiDoamin(pRcvMsg);
    }
    else if( IMSA_IMS_DOMAIN_NULL == IMSA_GetCurrentDomain())
    {
        IMSA_SRV_SndDomainSrvReadyInd();
    }
    else
    {
        /* Domain没有域选的状态, 则不进行任何处理 */
    }
}
/* delete PeriodImsEmcSrvTryTimer */


VOS_UINT32  IMSA_AddSubscription
(
    VOS_UINT32                          ulPid,
    IMSA_SUBCRIBE_TYPE_ENUM_UINT8       enType,
    VOS_UINT32                          ulPara,
    VOS_UINT32                         *pulSubscriptionId
)
{
    IMSA_SUBSCRIPTION_INFO_STRU        *pstSubcriptionInfo      = VOS_NULL_PTR;
    IMSA_SUBSCRIPTION_INFO_LIST_STRU   *pstSubcriptionInfoList  = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_AddSubscription is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_AddSubscription_ENUM, LNAS_ENTRY);

    pstSubcriptionInfoList  = IMSA_GetSubcriptionInfoListAddr();

    /* 判断是否已经存满，如果是，则返回失败 */
    if (pstSubcriptionInfoList->ulSubscriptionNum >= IMSA_MAX_SUBSCRIPTION_NUM)
    {
        IMSA_WARN_LOG("IMSA_AddSubscription:can not save more!");
        TLPS_PRINT2LAYER_INFO(IMSA_AddSubscription_ENUM, 1);
        return IMSA_FAIL;
    }

    /* 订阅内容是否合法 */
    if (enType >= IMSA_SUBCRIBE_TYPE_BUTT)
    {
        IMSA_WARN_LOG("IMSA_AddSubscription:type illegal!");
        TLPS_PRINT2LAYER_INFO(IMSA_AddSubscription_ENUM, LNAS_PARAM_INVALID);
        return IMSA_FAIL;
    }

    /* 判断输入指针是否合法 */
    if (pulSubscriptionId == VOS_NULL_PTR)
    {
        IMSA_WARN_LOG("IMSA_AddSubscription:null pointer!");
        TLPS_PRINT2LAYER_INFO(IMSA_AddSubscription_ENUM, LNAS_NULL_PTR);
        return IMSA_FAIL;
    }

    /* 存储签约信息 */
    pstSubcriptionInfo = &pstSubcriptionInfoList->astSubcriptionInfoArray[pstSubcriptionInfoList->ulSubscriptionNum];
    pstSubcriptionInfo->ulPid               = ulPid;
    pstSubcriptionInfo->enType              = enType;
    pstSubcriptionInfo->ulPara              = ulPara;
    pstSubcriptionInfo->ulSubscriptionId    = pstSubcriptionInfoList->ulMaxSubscriptionId;
    *pulSubscriptionId                      = pstSubcriptionInfoList->ulMaxSubscriptionId;
    pstSubcriptionInfoList->ulMaxSubscriptionId++;
    pstSubcriptionInfoList->ulSubscriptionNum++;

    return IMSA_SUCC;
}


VOS_UINT32 IMSA_DeleteSubscription
(
    VOS_UINT32                  ulSubscriptionId
)
{
    VOS_UINT32                          i                       = IMSA_NULL;
    IMSA_SUBSCRIPTION_INFO_LIST_STRU   *pstSubcriptionInfoList  = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_DeleteSubscription is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_DeleteSubscription_ENUM, LNAS_ENTRY);

    pstSubcriptionInfoList  = IMSA_GetSubcriptionInfoListAddr();

    for (i = 0; i < pstSubcriptionInfoList->ulSubscriptionNum; i++)
    {
        if (ulSubscriptionId == pstSubcriptionInfoList->astSubcriptionInfoArray[i].ulSubscriptionId)
        {
            IMSA_MEM_CPY_S( &pstSubcriptionInfoList->astSubcriptionInfoArray[i],
                            sizeof(IMSA_SUBSCRIPTION_INFO_STRU)*((pstSubcriptionInfoList->ulSubscriptionNum - 1) - i),
                            &pstSubcriptionInfoList->astSubcriptionInfoArray[i+1],
                            sizeof(IMSA_SUBSCRIPTION_INFO_STRU)*((pstSubcriptionInfoList->ulSubscriptionNum - 1) - i));

            pstSubcriptionInfoList->ulSubscriptionNum--;
            return IMSA_SUCC;
        }
    }

    return IMSA_SUCC;
}


VOS_VOID IMSA_SRV_PrintNrmSrvState( VOS_VOID )
{
    switch (IMSA_SRV_GetNormalSrvStatus())
    {
        case IMSA_SRV_STATUS_IDLE_DEREG:
            IMSA_INFO_LOG("Current Normal Service State: IDLE+DEREG");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintNrmSrvState_ENUM, IMSA_STATUS_IDLE);
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintNrmSrvState_ENUM, IMSA_STATUS_NOT_REGISTER);
            break;
        case IMSA_SRV_STATUS_CONNING_DEREG:
            IMSA_INFO_LOG("Current Normal Service State: CONNING+DEREG");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintNrmSrvState_ENUM, IMSA_STATUS_CONNECTING);
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintNrmSrvState_ENUM, IMSA_STATUS_NOT_REGISTER);
            break;
        case IMSA_SRV_STATUS_CONNING_REG:
            IMSA_INFO_LOG("Current Normal Service State: CONNING+REG");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintNrmSrvState_ENUM, IMSA_STATUS_CONNECTING);
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintNrmSrvState_ENUM, IMSA_STATUS_REGISTERED);
            break;
        case IMSA_SRV_STATUS_RELEASING_DEREG:
            IMSA_INFO_LOG("Current Normal Service State: RELEASING+DEREG");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintNrmSrvState_ENUM, IMSA_STATUS_RELEASING);
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintNrmSrvState_ENUM, IMSA_STATUS_NOT_REGISTER);
            break;
        case IMSA_SRV_STATUS_CONN_DEREG:
            IMSA_INFO_LOG("Current Normal Service State: CONN+DEREG");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintNrmSrvState_ENUM, IMSA_STATUS_CONN);
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintNrmSrvState_ENUM, IMSA_STATUS_NOT_REGISTER);
            break;
        case IMSA_SRV_STATUS_CONN_REGING:
            IMSA_INFO_LOG("Current Normal Service State: CONN+REGING");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintNrmSrvState_ENUM, IMSA_STATUS_CONN);
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintNrmSrvState_ENUM, IMSA_STATUS_REGISTERING);
            break;
        case IMSA_SRV_STATUS_CONN_DEREGING:
            IMSA_INFO_LOG("Current Normal Service State: CONN+DEREGING");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintNrmSrvState_ENUM, IMSA_STATUS_CONN);
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintNrmSrvState_ENUM, IMSA_STATUS_DEREGISTERING);
            break;
        case IMSA_SRV_STATUS_CONN_REG:
            IMSA_INFO_LOG("Current Normal Service State: CONN+REG");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintNrmSrvState_ENUM, IMSA_STATUS_CONN);
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintNrmSrvState_ENUM, IMSA_STATUS_REGISTERED);
            break;
        case IMSA_SRV_STATUS_HO_DEREG:
            IMSA_INFO_LOG("Current Normal Service State: HO+DEREG");
            break;

        case IMSA_SRV_STATUS_HO_REG:
            IMSA_INFO_LOG("Current Normal Service State: HO+REG");
            break;

        default:
            IMSA_INFO_LOG("Current Normal Service State: UNKNOWN");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintNrmSrvState_ENUM, IMSA_STATUS_UNKNOWN);
            break;
    }
}


VOS_VOID IMSA_SRV_PrintEmcSrvState( VOS_VOID )
{
    switch (IMSA_SRV_GetEmcSrvStatus())
    {
        case IMSA_SRV_STATUS_IDLE_DEREG:
            IMSA_INFO_LOG("Current Emc Service State: IDLE+DEREG");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintEmcSrvState_ENUM, IMSA_STATUS_IDLE);
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintEmcSrvState_ENUM, IMSA_STATUS_NOT_REGISTER);
            break;
        case IMSA_SRV_STATUS_CONNING_DEREG:
            IMSA_INFO_LOG("Current Emc Service State: CONNING+DEREG");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintEmcSrvState_ENUM, IMSA_STATUS_CONNECTING);
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintEmcSrvState_ENUM, IMSA_STATUS_NOT_REGISTER);
            break;
        case IMSA_SRV_STATUS_CONNING_REG:
            IMSA_INFO_LOG("Current Emc Service State: CONNING+REG");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintEmcSrvState_ENUM, IMSA_STATUS_CONNECTING);
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintEmcSrvState_ENUM, IMSA_STATUS_REGISTERED);
            break;
        case IMSA_SRV_STATUS_RELEASING_DEREG:
            IMSA_INFO_LOG("Current Emc Service State: RELEASING+DEREG");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintEmcSrvState_ENUM, IMSA_STATUS_RELEASING);
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintEmcSrvState_ENUM, IMSA_STATUS_NOT_REGISTER);
            break;
        case IMSA_SRV_STATUS_CONN_DEREG:
            IMSA_INFO_LOG("Current Emc Service State: CONN+DEREG");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintEmcSrvState_ENUM, IMSA_STATUS_CONN);
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintEmcSrvState_ENUM, IMSA_STATUS_NOT_REGISTER);
            break;
        case IMSA_SRV_STATUS_CONN_REGING:
            IMSA_INFO_LOG("Current Emc Service State: CONN+REGING");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintEmcSrvState_ENUM, IMSA_STATUS_CONN);
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintEmcSrvState_ENUM, IMSA_STATUS_REGISTERING);
            break;
        case IMSA_SRV_STATUS_CONN_DEREGING:
            IMSA_INFO_LOG("Current Emc Service State: CONN+DEREGING");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintEmcSrvState_ENUM, IMSA_STATUS_CONN);
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintEmcSrvState_ENUM, IMSA_STATUS_DEREGISTERING);
            break;
        case IMSA_SRV_STATUS_CONN_REG:
            IMSA_INFO_LOG("Current Emc Service State: CONN+REG");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintEmcSrvState_ENUM, IMSA_STATUS_CONN);
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintEmcSrvState_ENUM, IMSA_STATUS_REGISTERED);
            break;
        default:
            IMSA_INFO_LOG("Current Emc Service State: UNKNOWN");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_PrintEmcSrvState_ENUM, IMSA_STATUS_UNKNOWN);
            break;
    }
}


VOS_UINT32 IMSA_SRV_IsConningRegState(VOS_UINT8 ucIsEmc)
{
    IMSA_SRV_STATUS_ENUM_UINT8          enImsaSrvStatus = IMSA_SRV_STATUS_BUTT;

    if (1 == ucIsEmc)
    {
        enImsaSrvStatus = IMSA_SRV_GetEmcSrvStatus();
    }
    else
    {
        enImsaSrvStatus = IMSA_SRV_GetNormalSrvStatus();
    }

    if (IMSA_SRV_STATUS_CONNING_REG != enImsaSrvStatus)
    {
        return IMSA_FALSE;
    }

    return IMSA_TRUE;
}

VOS_VOID IMSA_CMCCDeregProcConnRegOrReging(VOS_VOID)
{
    #if (FEATURE_ON == FEATURE_DSDS)
    IMSA_CONTROL_MANAGER_STRU                  *pstControlMagnaer   = VOS_NULL_PTR;

    pstControlMagnaer                       = IMSA_GetControlManagerAddress();
    #endif

    IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_CONN_DEREGING);

    /* 如果有定时器在运行，是否需要停止定时器? */
    switch (IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        case IMSA_RESOURCE_STATUS_ACTIVE:
            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_NOT_LOCAL);
            #if (FEATURE_ON == FEATURE_DSDS)
            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_StopTimer(&pstControlMagnaer->stGetSerProtectTimer);
                IMSA_StartTimer(&pstControlMagnaer->stDeregSerProtectTimer);
            }
            #endif
            break;
        #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:

            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          VOS_FALSE,
                                          VOS_FALSE);

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_NOT_LOCAL);

            IMSA_StopTimer(&pstControlMagnaer->stGetSerProtectTimer);
            IMSA_StartTimer(&pstControlMagnaer->stDeregSerProtectTimer);
            break;

        case  IMSA_RESOURCE_STATUS_NULL:

            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          VOS_FALSE,
                                          VOS_FALSE);

            //IMSA_DsdsSaveMoSmsMsg(pstRpDataReq);
            break;
        #endif
        default:
            #if (FEATURE_ON == FEATURE_DSDS)
            IMSA_DsdsModifyTaskSrcByTaskType(IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                             RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
            #endif
            break;

    }


}

VOS_VOID IMSA_CMCCDeregProcConnOrConningDereg(VOS_VOID)
{

    IMSA_CONTROL_MANAGER_STRU                  *pstControlMagnaer   = VOS_NULL_PTR;
    IMSA_CONN_RELEASE_TYPE_ENUM_UINT8       enConnRlsType;

    pstControlMagnaer                       = IMSA_GetControlManagerAddress();

    /* 设置PDN释放类型 */
    if( VOS_TRUE == pstControlMagnaer->ucOrgPdnRlsFlag)
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL;
    }
    else
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_LOCAL;
    }

    IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_RELEASING_DEREG);

    switch (IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        case IMSA_RESOURCE_STATUS_ACTIVE:
            IMSA_SRV_SndConnRelReq(IMSA_SRV_TYPE_NORMAL, IMSA_CONN_SIP_PDP_TYPE_SIGNAL,IMSA_IMS_DOMAIN_LTE,enConnRlsType);
            #if (FEATURE_ON == FEATURE_DSDS)
            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_StopTimer(&pstControlMagnaer->stGetSerProtectTimer);
                IMSA_StartTimer(&pstControlMagnaer->stDeregSerProtectTimer);
            }
            #endif
            break;
        #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:

            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          VOS_FALSE,
                                          VOS_FALSE);

            IMSA_SRV_SndConnRelReq(IMSA_SRV_TYPE_NORMAL, IMSA_CONN_SIP_PDP_TYPE_SIGNAL,IMSA_IMS_DOMAIN_LTE,enConnRlsType);

            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_StopTimer(&pstControlMagnaer->stGetSerProtectTimer);
                IMSA_StartTimer(&pstControlMagnaer->stDeregSerProtectTimer);
            }
            break;

        case  IMSA_RESOURCE_STATUS_NULL:

            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                          VOS_FALSE,
                                          VOS_FALSE);

            //IMSA_DsdsSaveMoSmsMsg(pstRpDataReq);
            break;
        #endif
        default:
            #if (FEATURE_ON == FEATURE_DSDS)
            IMSA_DsdsModifyTaskSrcByTaskType(IMSA_RF_RES_APPLY_TRIGER_SRC_DEREG,
                                             RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
            #endif
            break;

    }

}


VOS_VOID IMSA_CMCCDeregProc(VOS_VOID)
{
    IMSA_CONTROL_MANAGER_STRU                  *pstControlMagnaer   = VOS_NULL_PTR;

    pstControlMagnaer                       = IMSA_GetControlManagerAddress();

    if (VOS_FALSE == IMSA_GetConfigParaAddress()->stCMCCCustomReq.ucCMCCCustomDeregFlag)
    {
        IMSA_INFO_LOG("CMCC Custom Dereg Flag is false.");
        TLPS_PRINT2LAYER_INFO(IMSA_CMCCDeregProc_ENUM, 1);
        return;
    }

    if (MSCC_IMSA_IMS_VOPS_SUPPORT == IMSA_GetNetInfoAddress()->enImsaImsVoPsStatus)
    {
        IMSA_INFO_LOG("VoPS support.");
        TLPS_PRINT2LAYER_INFO(IMSA_CMCCDeregProc_ENUM, 2);
        return;
    }

    /* 根据中移动定制需求，VOLTE通话过程中移动到GU下，RAU成功后，需要立即发起去注册流程;
    但是通话过程中移动到不支持VOLTE的L小区后，需要等电话结束 */
    if (((MSCC_IMSA_ACCESS_TYPE_EUTRAN_TDD == IMSA_GetNetInfoAddress()->enAccessType) ||
          (MSCC_IMSA_ACCESS_TYPE_EUTRAN_FDD == IMSA_GetNetInfoAddress()->enAccessType)) &&
         (VOS_TRUE == IMSA_IsImsExist()))
    {
        IMSA_INFO_LOG("IMS call/sms/ss running. will dereg later.");
        TLPS_PRINT2LAYER_INFO(IMSA_CMCCDeregProc_ENUM, 3);
        return;
    }




    IMSA_INFO_LOG1("IMSA_CMCCDeregProc: Service status: ", IMSA_SRV_GetNormalSrvStatus());
    TLPS_PRINT2LAYER_INFO1(IMSA_CMCCDeregProc_ENUM, 4, IMSA_SRV_GetNormalSrvStatus());
    switch(IMSA_SRV_GetNormalSrvStatus())
    {
        case IMSA_SRV_STATUS_CONN_REGING:
            IMSA_CMCCDeregProcConnRegOrReging();
            #if (FEATURE_ON == FEATURE_PTM)
            IMSA_RegFailEventReport(IMSA_ERR_LOG_REG_FAIL_REASON_MOVE_TO_NOT_SUP_IMS_RAT,
                                    IMSA_FALSE,
                                    IMSA_FALSE,
                                    IMSA_FALSE);
            #endif
            break;

        case IMSA_SRV_STATUS_CONN_REG:
            IMSA_CMCCDeregProcConnRegOrReging();
            #if (FEATURE_ON == FEATURE_PTM)
            IMSA_SRV_InitDeregEventReport(IMSA_ERR_LOG_INIT_DEREG_REASON_RAT_OR_VOICE_NOT_SUPPORT);
            #endif
            break;
        case IMSA_SRV_STATUS_CONNING_DEREG:
        case IMSA_SRV_STATUS_CONN_DEREG:

            IMSA_CMCCDeregProcConnOrConningDereg();
            #if (FEATURE_ON == FEATURE_PTM)
            IMSA_SRV_InitDeregEventReport(IMSA_ERR_LOG_INIT_DEREG_REASON_RAT_OR_VOICE_NOT_SUPPORT);
            #endif
            break;

        case IMSA_SRV_STATUS_CONNING_REG:
            /* 需要通知IMS允许其发包，通知发起本地注册 */
            IMSA_SndImsMsgServiceSuspendOrResumeSrvInfo(IMSA_IMS_INPUT_SERVICE_REASON_RESUME_NRM_SRV);
            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_IDLE_DEREG);

            /* 释放资源 */
            #if (FEATURE_ON == FEATURE_DSDS)
            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_DsdsProcResourceRelease(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
                IMSA_StopTimer(&pstControlMagnaer->stGetSerProtectTimer);
            }
            #endif
            /* coverity[unchecked_value] */
            IMSA_SRV_SndDomainSrvStatusInd(IMSA_DOMAIN_SERVICE_STATUS_NO_SERVICE,
                                            IMSA_DOMAIN_NO_SRV_CAUSE_NW_NOT_SUPPORT_IMS_VOICE,
                                            IMSA_GetCurrentDomain());

            /* 上报状态给Domain，Domain会发起域选过程 */
            if( VOS_TRUE == pstControlMagnaer->ucDomainRegSrvStatusFlag)
            {
                pstControlMagnaer->ucDomainRegSrvStatusFlag = VOS_FALSE;
                IMSA_SRV_SndDomainSrvReadyInd();
            }

            #if (FEATURE_ON == FEATURE_PTM)
            IMSA_SRV_InitDeregEventReport(IMSA_ERR_LOG_INIT_DEREG_REASON_RAT_OR_VOICE_NOT_SUPPORT);
            #endif
            break;
        case IMSA_SRV_STATUS_IDLE_DEREG:

            /* 如果该状态下，有在申请注册资源，且原因值为获取普通服务，则释放资源 */
            #if (FEATURE_ON == FEATURE_DSDS)
            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_DsdsProcResourceRelease(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
            }
            #endif
            #if (FEATURE_ON == FEATURE_PTM)
            IMSA_NotInitRegErrRecord();
            #endif
            break;
        default:
            break;
    }


}

VOS_VOID IMSA_CommonDeregProc(VOS_VOID)
{
    if (IMSA_IMS_DOMAIN_WIFI == IMSA_GetCurrentDomain())
    {
        return;
    }
    IMSA_CMCCDeregProc();
    IMSA_SRV_ProcWhenRoamNotSatisfied();

    return;
}


VOS_UINT32 IMSA_IsImsEmcServiceEstablishSucc(VOS_VOID)
{
    VOS_UINT32    ulResult = IMSA_FALSE;

    IMSA_INFO_LOG("IMSA_IsStopImsEmcService is enter!");

    if ((IMSA_SRV_STATUS_CONNING_DEREG == IMSA_SRV_GetEmcSrvStatus()) ||
        (IMSA_SRV_STATUS_CONN_REGING == IMSA_SRV_GetEmcSrvStatus()))
    {
        ulResult = IMSA_FALSE;
    }
    /* 无卡紧急呼，CONN_DEREG状态下紧急Tcall超时，走CSFB */
    else if ((IMSA_SRV_STATUS_CONN_REG == IMSA_SRV_GetEmcSrvStatus()) ||
             (IMSA_SRV_STATUS_CONN_DEREG == IMSA_SRV_GetEmcSrvStatus()))
    {
        ulResult = IMSA_TRUE;
    }
    else
    {
        IMSA_INFO_LOG("IMSA_IsStopImsEmcService:other status!");
    }


    return ulResult;
}

/*****************************************************************************
 Function Name  : IMSA_GetCellularImsSupportFlag()
 Description    : 获取Cellular是否支持IMS的标识
 Input          :  VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-10-19  Draft Enact
*****************************************************************************/
VOS_UINT8 IMSA_GetCellularImsSupportFlag(VOS_VOID)
{
    IMSA_CONTROL_MANAGER_STRU      *pstControlManager;
    pstControlManager = IMSA_GetControlManagerAddress();


    if((VOS_TRUE == pstControlManager->stImsaConfigPara.ucUtranImsSupportFlag)
       ||(VOS_TRUE == pstControlManager->stImsaConfigPara.ucLteImsSupportFlag))
    {
        return VOS_TRUE;
    }
    else
    {
        return VOS_FALSE;
    }
}





VOS_VOID IMSA_StopImsEmcService(VOS_VOID)
{
    IMSA_INFO_LOG("IMSA_StopImsEmcService is enter!");

    switch(IMSA_SRV_GetEmcSrvStatus())
    {
        case IMSA_SRV_STATUS_CONNING_DEREG:
            IMSA_INFO_LOG("IMSA_SRV_ProcEmcTcallTimerExp:CONNING_DEREG!");

            IMSA_SRV_SndConnRelReq(IMSA_CONN_TYPE_EMC,
                                   IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                   IMSA_IMS_DOMAIN_LTE,
                                   IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL);

            /* 状态转到RELEASE+DEREG态，通知CALL紧急服务不具备 */
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_EMC,
                                      IMSA_SRV_STATUS_RELEASING_DEREG);

            /* 通知CALL模块紧急呼叫服务进入无服务状态，促使CALL模块清除缓存的紧急呼 */
            IMSA_SRV_SndEmcCallSrvStatusInd(IMSA_CALL_SERVICE_STATUS_NO_SERVICE,
                                            IMSA_CALL_NO_SRV_CAUSE_SIP_PDP_ERR);

            break;
        case IMSA_SRV_STATUS_CONN_REGING:
            IMSA_INFO_LOG("IMSA_SRV_ProcEmcTcallTimerExp:CONN_REGING!");

            /* 需要本地去注册 */
            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_EMC, IMSA_DEREG_LOCAL);
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_EMC, IMSA_SRV_STATUS_CONN_DEREGING);
            break;
        default:
            IMSA_INFO_LOG("IMSA_StopImsEmcService:other status!");
            break;
    }

}


/*lint -e18*/
VOS_VOID IMSA_SRV_SndDomainSrvStatusInd
(
    IMSA_DOMAIN_SERVICE_STATUS_ENUM_UINT32       enDoaminSrvStatus,
    IMSA_DOMAIN_NO_SRV_CAUSE_ENUM_UINT32         enNoSrvCause,
    IMSA_DOMAIN_MODE_ENUM_UINT8                  enCurrentDomain
)
{
    IMSA_DOMAIN_SRV_STATUS_IND_STRU    *pstDomainSrvStatusInd = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_SRV_SndDomainSrvStatusInd is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_SndNrmCallSrvStatusInd_ENUM, LNAS_ENTRY);

    /*分配空间并检验分配是否成功*/
    pstDomainSrvStatusInd = (VOS_VOID*)IMSA_GetIntraMsgBuffAddr(sizeof(IMSA_DOMAIN_SRV_STATUS_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstDomainSrvStatusInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SRV_SndDomainSrvStatusInd:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SRV_SndNrmCallSrvStatusInd_ENUM, LNAS_NULL_PTR);
        return ;
    }

    /*清空*/
    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstDomainSrvStatusInd),IMSA_GET_MSG_LENGTH(pstDomainSrvStatusInd), 0, IMSA_GET_MSG_LENGTH(pstDomainSrvStatusInd));

    /*填写消息头*/
    IMSA_WRITE_INTRA_MSG_HEAD(  pstDomainSrvStatusInd,
                                ID_IMSA_DOMAIN_SRV_STATUS_IND,
                                sizeof(IMSA_DOMAIN_SRV_STATUS_IND_STRU));

    /*填写消息内容*/
    pstDomainSrvStatusInd->enDomainSrvStatus    = enDoaminSrvStatus;
    pstDomainSrvStatusInd->enNoSrvCause         = enNoSrvCause;
    pstDomainSrvStatusInd->enCurrentDomain      = enCurrentDomain;

    /*调用消息发送函数 */
    IMSA_SEND_INTRA_MSG(pstDomainSrvStatusInd);
}
/*lint +e18*/


VOS_VOID IMSA_SRV_SndDomainSrvReadyInd()
{
    IMSA_DOMAIN_SRV_READY_IND_STRU    *pstDomainSrvReadyInd = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_SRV_SndDomainSrvReadyInd is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_SndNrmCallSrvStatusInd_ENUM, LNAS_ENTRY);

    /*分配空间并检验分配是否成功*/
    pstDomainSrvReadyInd = (VOS_VOID*)IMSA_GetIntraMsgBuffAddr(sizeof(IMSA_DOMAIN_SRV_STATUS_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstDomainSrvReadyInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SRV_SndDomainSrvStatusInd:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SRV_SndNrmCallSrvStatusInd_ENUM, LNAS_NULL_PTR);
        return;
    }

    /*清空*/
    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstDomainSrvReadyInd), IMSA_GET_MSG_LENGTH(pstDomainSrvReadyInd), 0, IMSA_GET_MSG_LENGTH(pstDomainSrvReadyInd));

    /*填写消息头*/
    IMSA_WRITE_INTRA_MSG_HEAD(  pstDomainSrvReadyInd,
                                ID_IMSA_DOMAIN_SRV_READY_IND,
                                sizeof(IMSA_DOMAIN_SRV_READY_IND_STRU));

    /*调用消息发送函数 */
    IMSA_SEND_INTRA_MSG(pstDomainSrvReadyInd);
}



VOS_VOID IMSA_SRV_SndDomainBearerRelnd(IMSA_IMS_DOMAIN_ENUM_UINT8 enNewPdnConnType)
{
    IMSA_DOMAIN_BEARER_REL_IND_STRU    *pstDomainBearerRelInd = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_SRV_SndDomainBearerRelnd is entered!");

    /*分配空间并检验分配是否成功*/
    pstDomainBearerRelInd = (VOS_VOID*)IMSA_GetIntraMsgBuffAddr(sizeof(IMSA_DOMAIN_BEARER_REL_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstDomainBearerRelInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SRV_SndDomainBearerRelnd:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SRV_SndNrmCallSrvStatusInd_ENUM, LNAS_NULL_PTR);
        return;
    }

    /*清空*/
    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstDomainBearerRelInd),IMSA_GET_MSG_LENGTH(pstDomainBearerRelInd), 0, IMSA_GET_MSG_LENGTH(pstDomainBearerRelInd));

    /*填写消息头*/
    IMSA_WRITE_INTRA_MSG_HEAD(  pstDomainBearerRelInd,
                                ID_IMSA_DOMAIN_BEARER_REL_IND,
                                sizeof(IMSA_DOMAIN_BEARER_REL_IND_STRU));
    pstDomainBearerRelInd->enNewPdnConnType = enNewPdnConnType;

    /*调用消息发送函数 */
    IMSA_SEND_INTRA_MSG(pstDomainBearerRelInd);
}





VOS_VOID IMSA_SRV_SndConnHoReq
(
    IMSA_DOMAIN_SWITCH_TYPE_ENUM_UINT8  enSwitchType,
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_IMS_DOMAIN_ENUM_UINT8           enConnRatType,
    VOS_UINT8                           ucOrgPdnRlFlag
)
{
    IMSA_CONN_HO_REQ_STRU              *pstConnHoReq     = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_SRV_SndConnHoReq is entered!");

    /*分配空间并检验分配是否成功*/
    pstConnHoReq = (VOS_VOID*)IMSA_GetIntraMsgBuffAddr(sizeof(IMSA_CONN_HO_REQ_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstConnHoReq)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SRV_SndConnHoReq:ERROR:Alloc Msg fail!");
        return ;
    }


    /*清空*/
    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstConnHoReq),IMSA_GET_MSG_LENGTH(pstConnHoReq) ,0, IMSA_GET_MSG_LENGTH(pstConnHoReq));

    /*填写消息头*/
    IMSA_WRITE_INTRA_MSG_HEAD(  pstConnHoReq,
                                ID_IMSA_CONN_HO_REQ,
                                sizeof(IMSA_CONN_HO_REQ_STRU));

    /*填写消息内容*/
    pstConnHoReq->enConnHoType       = enSwitchType;
    pstConnHoReq->enConnType         = enConnType;
    pstConnHoReq->enConnRatType      = enConnRatType;
    pstConnHoReq->enOrgPdnRlsFlag    = ucOrgPdnRlFlag;

    /*调用消息发送函数 */
    IMSA_SEND_INTRA_MSG(pstConnHoReq);
}


VOS_VOID IMSA_SRV_SndDomainSwitchResultInd
(
    IMSA_DOMAIN_SWITCH_TYPE_ENUM_UINT8          enSwitchType,
    IMSA_DOMAIN_SWITCH_RESULT_ENUM_UINT8        enSwitchResult
)
{
    IMSA_DOMAIN_SWITCH_RESULT_IND_STRU        *pstDomainSwitchResultInd    = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_SRV_SndDomainSwitchResultInd is entered!");

    /*分配空间并检验分配是否成功*/
    pstDomainSwitchResultInd = (VOS_VOID*)IMSA_GetIntraMsgBuffAddr(sizeof(IMSA_DOMAIN_SWITCH_RESULT_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstDomainSwitchResultInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SRV_SndConnHoReq:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstDomainSwitchResultInd),IMSA_GET_MSG_LENGTH(pstDomainSwitchResultInd), 0, IMSA_GET_MSG_LENGTH(pstDomainSwitchResultInd));

    /*填写消息头*/
    IMSA_WRITE_INTRA_MSG_HEAD(  pstDomainSwitchResultInd,
                                ID_IMSA_DOMAIN_SWITCH_RESULT_IND,
                                sizeof(IMSA_DOMAIN_SWITCH_RESULT_IND_STRU));

    /*填写消息内容*/
    pstDomainSwitchResultInd->enSwitchType       = enSwitchType;
    pstDomainSwitchResultInd->enSwitchResult     = enSwitchResult;

    /*调用消息发送函数 */
    IMSA_SEND_INTRA_MSG(pstDomainSwitchResultInd);
}




IMSA_NRM_SRV_CON_SAT_STATUE_ENMUM_UINT32 IMSA_SRV_IsWifiNrmSrvConditonSatisfied( VOS_VOID )
{
    IMSA_CONFIG_PARA_STRU              *pstImsaConfigPara   = VOS_NULL_PTR;

    pstImsaConfigPara   = IMSA_GetConfigParaAddress();

    /* UE支持VOWIFI */
    /* TODO */
    if (IMSA_TRUE != pstImsaConfigPara->ucWifiImsSupportFlag)
    {
        IMSA_INFO_LOG("IMSA_SRV_IsWifiNrmSrvConditonSatisfied:ISIM PARA not satisfied!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsNrmSrvConditonSatisfied_ENUM, 5);
        return IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_RAT_NOT_SUPPORT;
    }
    /* 如果VOWIFI AP指定LINK LOST，则设置，且当前网络不支持vowifi */
    if (IMSA_PS_SERVICE_STATUS_NORMAL_SERVICE != IMSA_GetWifiPsServiceStatus())
    {
        IMSA_INFO_LOG("IMSA_SRV_IsWifiNrmSrvConditonSatisfied:Ps service not normal!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsNrmSrvConditonSatisfied_ENUM, 3);
        return IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_VOICE_NOT_SUPPORT;
    }

    /* IMS语音不支持，则返回条件不满足 */
    if ((IMSA_VOICE_DOMAIN_CS_ONLY == IMSA_GetVoiceDomain())
            || (VOS_FALSE == IMSA_GetUeImsVoiceCap()))
    {
        IMSA_INFO_LOG("IMSA_SRV_IsNrmSrvConditonSatisfied:voice not support!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsNrmSrvConditonSatisfied_ENUM, 2);
        return IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_VOICE_NOT_SUPPORT;
    }



    /* 卡状态为有效可用，且IMPI、IMPU，DOMAIN参数有效 */
    if (IMSA_TRUE != IMSA_SRV_IsNrmSrvIsimParaSatisfied())
    {
        IMSA_INFO_LOG("IMSA_SRV_IsNrmSrvConditonSatisfied:ISIM PARA not satisfied!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsNrmSrvConditonSatisfied_ENUM, 5);
        return IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_USIM_NOT_SUPPORT;
    }


    return IMSA_NRM_SRV_CON_SAT_STATUE_SUCC;
}

VOS_VOID IMSA_SRV_SaveServiceChangeInd
(
    const MSCC_IMSA_SERVICE_CHANGE_IND_STRU     *pstServiceChangeInd
)
{
    IMSA_NETWORK_INFO_STRU             *pstNwInfo           = VOS_NULL_PTR;
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;
    VOS_UINT32                          ulIsNotfiyNrmNoSrv  = IMSA_FALSE;
    VOS_UINT32                          ulIsNotfiyEmcNoSrv  = IMSA_FALSE;
    IMSA_MSCC_REG_SUCC_PROC_ENUM_UINT32 enRegSuccProcType   = IMSA_MSCC_REG_PROC_BUTT;

    IMSA_INFO_LOG("IMSA_SRV_SaveServiceChangeInd is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcServiceChangeInd_ENUM, LNAS_ENTRY);

    /* 存储SERVICE CHANGE IND消息中携带的参数 */
    pstNwInfo                               = IMSA_GetNetInfoAddress();
    pstControlMagnaer                       = IMSA_GetControlManagerAddress();

    /*新增一个标志记录PS服务状态变更，收到service change ind消息以后，首先要将该标志清为
      FALSE*/
    pstControlMagnaer->ucPsServStatusChangeFlag = VOS_FALSE;

    /*如果当前service change ind消息里面带的服务状态为Normal service,并且前一个service change ind消息里面
     带的服务状态不是Normal service状态，将本地的服务状态变更标志置为True*/
    if(pstNwInfo->enImsaPsServiceStatus != pstServiceChangeInd->enPsServiceStatus)
    {
        if(MSCC_IMSA_NORMAL_SERVICE == pstServiceChangeInd->enPsServiceStatus)
        {
             pstControlMagnaer->ucPsServStatusChangeFlag = VOS_TRUE;
        }
    }

    pstNwInfo->enImsaPsServiceStatus        = pstServiceChangeInd->enPsServiceStatus;

    /* 之前的漫游标记是通过CAMP_INFO_CHANGE_IND发过来的，但是并不准确，需要SERVICE_CHANGE_IND来通知给IMSA
       新增一个比特位bitRoamingValid，当该比特位为1时，代表漫游标志有效。此时IMSA更新本地维护的漫游状态。*/
    if (VOS_TRUE == pstServiceChangeInd->bitRoamingValid)
    {
        pstNwInfo->ucImsaRoamingFlg             = pstServiceChangeInd->ucRoamingFlg;
    }

    pstControlMagnaer->ucImsaUsimStatus     = pstServiceChangeInd->ucPsSimValid;
    pstControlMagnaer->ucRcvServiceChangeIndFlag = VOS_TRUE;

    /* 接入技术更新 */
    IMSA_SRV_UpdateRat(pstServiceChangeInd->enRat,&ulIsNotfiyNrmNoSrv, &ulIsNotfiyEmcNoSrv);


    /* 只有是正常服务时，才更新网络支持IMS和EMS指示 */
    if (MSCC_IMSA_NORMAL_SERVICE == pstServiceChangeInd->enPsServiceStatus)
    {
        IMSA_SRV_UpdateImsVoPsStatus(pstServiceChangeInd->enImsVoPsInd);
        IMSA_SRV_UpdateEmsStatus(pstServiceChangeInd->enEmsInd);

        if(pstServiceChangeInd->bitRegSuccProcTypeValid != pstControlMagnaer->bitRegSuccProcTypeValid)
        {
            pstControlMagnaer->bitRegSuccProcTypeValid = pstServiceChangeInd->bitRegSuccProcTypeValid;
        }
        if(pstServiceChangeInd->enRegSuccProcType != IMSA_GetRegSuccProcType())
        {
            enRegSuccProcType = pstServiceChangeInd->enRegSuccProcType;
            IMSA_SetRegSuccProcType(enRegSuccProcType);
        }


    }

    return;
}

VOS_VOID IMSA_SRV_SaveWifiStateInd
(
    const WIFI_IMSA_STATE_IND_STRU     *pstWifiStateInd
)
{
    IMSA_WIFI_NETWORK_INFO_STRU        *pstWifiNetInfo = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_SRV_SaveWifiStateInd is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcServiceChangeInd_ENUM, LNAS_ENTRY);

    /* 存储SERVICE CHANGE IND消息中携带的参数 */
    pstWifiNetInfo                          = IMSA_GetWifiNetInfoAddress();
    pstWifiNetInfo->enImsaPsServiceStatus = IMSA_PS_SERVICE_STATUS_NORMAL_SERVICE;
    /* 如果MAC地址变更，即切换AP,需要重新初始化惩罚定时器，防止在一个AP上切换失败被惩罚随后换到另一个AP上仍然会惩罚这个场景 */
    if(0 != IMSA_MEM_CMP(&pstWifiNetInfo->stApMac2,&pstWifiStateInd->stApMac,sizeof(IMSA_WIFI_AP_MAC_STRU)))
    {
        IMSA_MEM_CPY_S(&pstWifiNetInfo->stApMac2,
                     sizeof(IMSA_WIFI_AP_MAC_STRU),
                     &pstWifiStateInd->stApMac,
                     sizeof(IMSA_WIFI_AP_MAC_STRU));

        /*初始化WIFI惩罚定时器惩罚时间 */
        IMSA_Domain_StopTimer1WifiDomain();
    }


    return;
}



VOS_VOID IMSA_SRV_SaveSwitchFlag
(
    IMSA_DOMAIN_SWITCH_TYPE_ENUM_UINT8 enSwitchType,
    VOS_UINT8   ucHoFailReturn2OrgPdnFlag,
    VOS_UINT8   ucOrgPdnRlsFlag
)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlManager   = VOS_NULL_PTR;

    pstControlManager                  = IMSA_GetControlManagerAddress();

    pstControlManager->enSwitchType              = enSwitchType;
    pstControlManager->ucHoFailReturn2OrgPdnFlag = ucHoFailReturn2OrgPdnFlag;
    pstControlManager->ucOrgPdnRlsFlag           = ucOrgPdnRlsFlag;

    return;
}


VOS_VOID IMSA_SRV_ProcDomainWifiDereg
(
    VOS_VOID
)
{
    /* 暂不考虑紧急的去注册 */

    /* 如果普通服务已注册成功或者正在注册，则通知注册模块进行去注册 */
    if ((IMSA_SRV_STATUS_CONN_REGING == IMSA_SRV_GetNormalSrvStatus())
        || (IMSA_SRV_STATUS_CONN_REG == IMSA_SRV_GetNormalSrvStatus()))
    {
        IMSA_INFO_LOG("IMSA_SRV_ProcDomainWifiDereg:dereg normal service!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcDeregReq_ENUM, IMSA_NORMAL);
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcDeregReq_ENUM, IMSA_STATUS_CONN);
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcDeregReq_ENUM, IMSA_STATUS_REGISTERING);
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcDeregReq_ENUM, IMSA_STATUS_REGISTERED);

        IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_CONN_DEREGING);

        IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_NOT_LOCAL);

        /* 由于已经进行去注册，无需再通知CALL模块普通呼叫进入无服务状态，
           否则去注册流程和CALL释放流程同时进行，容易出问题 */
        return ;
    }

    if (IMSA_SRV_STATUS_CONN_DEREGING == IMSA_SRV_GetNormalSrvStatus())
    {
        IMSA_INFO_LOG("IMSA_SRV_ProcDomainWifiDereg:wait for dereg result!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcDeregReq_ENUM, 1);
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcDeregReq_ENUM, IMSA_STATUS_CONN);
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcDeregReq_ENUM, IMSA_STATUS_DEREGISTERING);
        return ;
    }

    /* WIFI下面不会有CONNING_REG状态 */
}



VOS_UINT32 IMSA_SRV_CheckSwitchPossible(VOS_VOID)
{
    IMSA_SRV_STATUS_ENUM_UINT8          enNrmSrvStatus      = IMSA_SRV_STATUS_BUTT;
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_SRV_CheckSwitchPossible:enter!");

    pstControlMagnaer                = IMSA_GetControlManagerAddress();

    enNrmSrvStatus = IMSA_SRV_GetNormalSrvStatus();

    /* 只有IDLE&DEREG/CONN&DEREG/CONN&REG状态下，才会处理SWITCH操作 */
    if ((IMSA_SRV_STATUS_IDLE_DEREG == enNrmSrvStatus)
        ||(IMSA_SRV_STATUS_CONN_DEREG == enNrmSrvStatus)
        ||(IMSA_SRV_STATUS_CONN_REG == enNrmSrvStatus))
    {
        return IMSA_TRUE;
    }

    /* PDN异常释放时,SRV会启动周期尝试定时器,同时上报Domain链路异常,
           SRV这个时候处于CONNING&REG, Domain可能会在这个状态下发起域切换 */
    if((IMSA_SRV_STATUS_CONNING_REG == enNrmSrvStatus)
        && ((IMSA_TRUE == IMSA_IsTimerRunning(&pstControlMagnaer->stPeriodImsSrvTryTimer))))
    {
        return IMSA_TRUE;
    }

    return IMSA_FALSE;
}


 VOS_UINT32 IMSA_SRV_IsLteRegConditionSatisfied(VOS_VOID)
 {

    IMSA_CONTROL_MANAGER_STRU                  *pstControlMagnaer   = VOS_NULL_PTR;
    MSCC_IMSA_IMS_VOICE_CAP_ENUM_UINT8          enImsVoiceCap       = MSCC_IMSA_IMS_VOICE_CAP_BUTT;

    IMSA_INFO_LOG("IMSA_SRV_IsLteRegTypeConditionSatisfied is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsLteRegConditionSatisfied_ENUM, LNAS_ENTRY);

    pstControlMagnaer                       = IMSA_GetControlManagerAddress();

    /*若当前处理的是service change ind消息触发的流程，则对L的注册类型和ps服务状态进行判断，
      否则就不再判决L的注册类型和ps服务状态*/
    if(VOS_TRUE == pstControlMagnaer->ucRcvServiceChangeIndFlag)
    {
        if(IMSA_OP_TRUE == pstControlMagnaer->bitRegSuccProcTypeValid)
        {
            /*如果当前处理的是假TAU流程触发的service change ind消息，且ps服务状态没有发生变更
            (这里的变更是指由非normal service 状态变为normal service状态),则上报语音能力并退出；
            否则，继续处理*/
            if((IMSA_MSCC_TAU_UNREAL_PROC == IMSA_GetRegSuccProcType())
               && (VOS_FALSE == pstControlMagnaer->ucPsServStatusChangeFlag))
            {
                if(IMSA_TRUE == IMSA_SRV_GetImsVoiceCapability(&enImsVoiceCap))
                {
                    IMSA_SndMsccMsgImsVoiceCapNotify(enImsVoiceCap, IMSA_CONN_IsExitPersistentBearer());
                }
                return VOS_FALSE;

            }
            else
            {
               /*其他条件，则继续处理*/
            }
        }
    }

    return VOS_TRUE;
 }




VOS_VOID IMSA_SRV_StartCellularImsService
(
    VOS_VOID
)
{
    IMSA_CONTROL_MANAGER_STRU                  *pstControlMagnaer   = VOS_NULL_PTR;
    IMSA_SRV_STATUS_ENUM_UINT8                  enNrmSrvStatus      = IMSA_SRV_STATUS_BUTT;
    IMSA_NRM_SRV_CON_SAT_STATUE_ENMUM_UINT32    enImsaSrvStatue     = IMSA_NRM_SRV_CON_SAT_STATUE_BUTT;

    IMSA_INFO_LOG("IMSA_SRV_ProcServiceChangeInd is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcServiceChangeInd_ENUM, LNAS_ENTRY);

    pstControlMagnaer                       = IMSA_GetControlManagerAddress();

    enNrmSrvStatus = IMSA_SRV_GetNormalSrvStatus();

    /* 进入此分支，说明上次给TAF报的注册状态是未注册，目前只有在SRVCC快速挂断场景中会出现这种情况。 */
    if ((IMSA_SRV_STATUS_CONN_REG == enNrmSrvStatus) && (IMSA_NORMAL_REG_STATUS_LTE_REG != IMSA_GetLastNrmRegStatus()))
    {
        IMSA_SRV_NrmRegStatusNotify(IMSA_NORMAL_REG_STATUS_LTE_REG);

        IMSA_SetLastNrmRegStatus(IMSA_NORMAL_REG_STATUS_LTE_REG);
    }

    enImsaSrvStatue = IMSA_SRV_IsNrmSrvConditonSatisfied();

    if (VOS_FALSE== IMSA_SRV_PreProcNrmSrvConSatStatus(enImsaSrvStatue))
    {
        /* 当IMS信令承载永久被禁或临时被禁时通知Domain模块, 若支持VoWifi时触发去VoWifi上尝试 */
        if (IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_CONN_FORBIDDEN == enImsaSrvStatue)
        {
            /*非IDLE+DEREG情况下，不能给domain模块发no service消息*/
            if(IMSA_SRV_STATUS_IDLE_DEREG == enNrmSrvStatus)
            {
                IMSA_SRV_SndDomainSrvStatusInd( IMSA_DOMAIN_SERVICE_STATUS_NO_SERVICE,
                                                IMSA_DOMAIN_NO_SRV_CAUSE_SIP_PDP_ERR,
                                                IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY );
            }
        }

        /* 勾出不发起注册的原因以及当前的状态 */
        IMSA_SndOmNoRegCauseInfo(enNrmSrvStatus, enImsaSrvStatue);
        return;
    }

    /*根据L的不同的注册类型判决是否需要继续处理,如果不是由service chagne ind消息触发的
     域选到L的函数调用的该流程，就不再对L的注册类型进行判决，直接返回TRUE*/
    if(VOS_FALSE == IMSA_SRV_IsLteRegConditionSatisfied())
    {
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsLteRegConditionSatisfied_ENUM, 0);

        return;
    }

    /* 根据当前服务状态进行不同处理 */
    switch (enNrmSrvStatus)
    {
        case IMSA_SRV_STATUS_IDLE_DEREG:

            /* 配置CGI信息给IMS */
            IMSA_ConfigCgi2Ims();

            /* 给IMS配置 Net Capability信息 */
            IMSA_ConfigNetCapInfo2Ims();

            /* 请求连接模块建立连接，将状态转到IMSA_SERVICE_STATUS_CONNING_DEREG */
            IMSA_SRV_ProcServiceChangeIndIdleDereg();
            break;

        case IMSA_SRV_STATUS_RELEASING_DEREG:

            /* 启动周期性尝试IMS服务定时器 */
            IMSA_StartTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer);
            break;

        case IMSA_SRV_STATUS_CONN_DEREGING:

            IMSA_SRV_ProcServiceChangeIndConnDereging();

            break;

        case IMSA_SRV_STATUS_CONN_DEREG:

            IMSA_SRV_ProcServiceChangeIndConnDereg();
            break;
        case IMSA_SRV_STATUS_CONNING_DEREG:
        case IMSA_SRV_STATUS_CONN_REGING:
        case IMSA_SRV_STATUS_CONNING_REG:
            /*这三个状态等注册完成后上报IMS voice能力*/
            break;
        default:
            /*IMSA不发起注册，需上报IMS voice能力*/
            if (IMSA_TRUE == IMSA_IsImsVoiceContidionSatisfied())
            {
                IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_AVAILABLE,IMSA_CONN_IsExitPersistentBearer());
            }
            else
            {
                IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE,IMSA_CONN_IsExitPersistentBearer());
            }

            break;
    }
}


VOS_VOID IMSA_SRV_StartWifiImsService
(
    VOS_VOID
)
{
    IMSA_SRV_STATUS_ENUM_UINT8          enNrmSrvStatus      = IMSA_SRV_STATUS_BUTT;
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_SRV_StartWifiImsService:enter!");

    pstControlMagnaer                       = IMSA_GetControlManagerAddress();

    switch(IMSA_SRV_IsWifiNrmSrvConditonSatisfied())
    {
        case IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_USIM_NOT_SUPPORT:
        case IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_RAT_NOT_SUPPORT:
        case IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_VOICE_NOT_SUPPORT:
            IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE,IMSA_CONN_IsExitPersistentBearer());
            return;

        case IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_OTHERS:
            /* 上报当前域不支持IMS */
            #if 0
            IMSA_SRV_SndDomainSrvStatusInd(IMSA_DOMAIN_SERVICE_STATUS_NO_SERVICE,
                                          IMSA_DOMAIN_NO_SRV_CAUSE_NW_NOT_SUPPORT_IMS_VOICE,
                                          IMSA_IMS_DOMAIN_MODE_WIFI_ONLY);
            #endif
            return;

        default:
            break;
    }

    enNrmSrvStatus = IMSA_SRV_GetNormalSrvStatus();

    /* 根据当前服务状态进行不同处理 */
    switch (enNrmSrvStatus)
    {
        case IMSA_SRV_STATUS_IDLE_DEREG:

            /* 请求连接模块建立连接，将状态转到IMSA_SERVICE_STATUS_CONNING_DEREG */
            IMSA_SRV_SndConnSetupReq(   IMSA_FALSE,
                                        IMSA_CONN_TYPE_NORMAL,
                                        IMSA_IMS_DOMAIN_WIFI);
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_CONNING_DEREG);
            break;

        case IMSA_SRV_STATUS_RELEASING_DEREG:

            /* 启动周期性尝试IMS服务定时器 */
            IMSA_StartTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer);
            break;

        case IMSA_SRV_STATUS_CONN_DEREGING:

            /* 如果是MMA DEREG REQ类型的去注册，则无操作；
               后续增加了其他类型的去注册，则需要另外考虑，例如IMS特性改为不支持
               导致的去注册过程中，IMS特性重新设为支持，则需要在去注册动作完成后，
               重新再注册，而不释放连接 */
            IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE,IMSA_CONN_IsExitPersistentBearer());

            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_CONN_REGING);

            IMSA_SRV_SndRegDeregReq(    IMSA_REG_TYPE_NORMAL,
                                        IMSA_DEREG_LOCAL);

            IMSA_SRV_SndRegRegReq(      IMSA_REG_TYPE_NORMAL,
                                        IMSA_REG_ADDR_PARAM_NEW);

            /* IMSA_StartTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer); */
            break;

        case IMSA_SRV_STATUS_CONN_DEREG:

            /* 请求注册模块发起注册，将状态转到IMSA_SERVICE_STATUS_CONN_REGING */
            IMSA_SRV_SndRegRegReq(      IMSA_REG_TYPE_NORMAL,
                                        IMSA_REG_ADDR_PARAM_NEW);
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_CONN_REGING);
            break;
        case IMSA_SRV_STATUS_CONNING_DEREG:
        case IMSA_SRV_STATUS_CONN_REGING:
        case IMSA_SRV_STATUS_CONNING_REG:
            /*这三个状态等注册完成后上报IMS voice能力*/
            break;

        case IMSA_SRV_STATUS_CONN_REG:
             /* 上报当前域已经正常服务状态 */
             IMSA_SRV_SndDomainSrvStatusInd(IMSA_DOMAIN_SERVICE_STATUS_NORMAL_SERVICE,
                                            IMSA_DOMAIN_NO_SRV_CAUSE_BUTT,
                                            IMSA_IMS_DOMAIN_MODE_WIFI_ONLY);
             break;

        default:
            /*IMSA不发起注册，需上报IMS voice能力*/
            if (IMSA_TRUE == IMSA_IsImsVoiceContidionSatisfied())
            {
                IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_AVAILABLE,IMSA_CONN_IsExitPersistentBearer());
            }
            else
            {
                IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE,IMSA_CONN_IsExitPersistentBearer());
            }

            break;
    }
}

VOS_VOID IMSA_SRV_StopCellularImsService
(
    VOS_VOID
)
{
    /* 如果紧急服务已注册成功或者正在注册，则通知注册模块进行本地去注册 */
    if (IMSA_SRV_STATUS_CONN_REGING == IMSA_SRV_GetEmcSrvStatus())
    {
        IMSA_INFO_LOG("IMSA_SRV_StopCellularImsService:emc,conn&reging!");

        IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_EMC, IMSA_SRV_STATUS_CONN_DEREG);

        IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_EMC, IMSA_DEREG_LOCAL);

        /* 通知CALL模块紧急呼叫服务进入无服务状态，促使CALL模块清除缓存的紧急呼 */
        IMSA_SRV_SndEmcCallSrvStatusInd(IMSA_CALL_SERVICE_STATUS_NO_SERVICE,
                                        IMSA_CALL_NO_SRV_CAUSE_REG_ERR);
    }

    if (IMSA_SRV_STATUS_CONN_REG == IMSA_SRV_GetEmcSrvStatus())
    {
        IMSA_INFO_LOG("IMSA_SRV_StopCellularImsService:emc,conn&reg!");

        IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_EMC, IMSA_SRV_STATUS_CONN_DEREG);

        IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_EMC, IMSA_DEREG_LOCAL);

        /* 由于已经进行本地去注册，无需再通知CALL模块紧急呼叫进入无服务状态，
           否则本地去注册流程和CALL释放流程同时进行，容易出问题 */
    }

    if (IMSA_SRV_STATUS_CONNING_DEREG == IMSA_SRV_GetEmcSrvStatus())
    {
        IMSA_INFO_LOG("IMSA_SRV_StopCellularImsService:emc,conning&dereg!");

        /* 通知CALL模块紧急呼叫服务进入无服务状态，促使CALL模块清除缓存的紧急呼 */
        IMSA_SRV_SndEmcCallSrvStatusInd(IMSA_CALL_SERVICE_STATUS_NO_SERVICE,
                                        IMSA_CALL_NO_SRV_CAUSE_SIP_PDP_ERR);
    }

    /* 如果普通服务已注册成功或者正在注册，则通知注册模块进行去注册 */
    if ((IMSA_SRV_STATUS_CONN_REGING == IMSA_SRV_GetNormalSrvStatus())
        || (IMSA_SRV_STATUS_CONN_REG == IMSA_SRV_GetNormalSrvStatus()))
    {
        IMSA_INFO_LOG("IMSA_SRV_StopCellularImsService:dereg normal service!");

        IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_CONN_DEREGING);

        IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_NOT_LOCAL);

        /* 由于已经进行去注册，无需再通知CALL模块普通呼叫进入无服务状态，
           否则去注册流程和CALL释放流程同时进行，容易出问题 */
        return ;
    }

    if (IMSA_SRV_STATUS_CONN_DEREGING == IMSA_SRV_GetNormalSrvStatus())
    {
        IMSA_INFO_LOG("IMSA_SRV_StopCellularImsService:wait for dereg result!");
        return ;
    }
}

VOS_VOID IMSA_SRV_StopWifiImsService
(
    VOS_VOID
)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_SRV_StopWifiImsService:enter!");

    pstControlMagnaer                       = IMSA_GetControlManagerAddress();

    /* 如果普通服务已注册成功或者正在注册，则通知注册模块进行去注册 */
    if ((IMSA_SRV_STATUS_CONN_REGING == IMSA_SRV_GetNormalSrvStatus())
        || (IMSA_SRV_STATUS_CONN_REG == IMSA_SRV_GetNormalSrvStatus()))
    {
        IMSA_INFO_LOG("IMSA_SRV_ProcDeregReq:dereg normal service!");

        IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_CONN_DEREGING);

        IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_NOT_LOCAL);

        /* 由于已经进行去注册，无需再通知CALL模块普通呼叫进入无服务状态，
           否则去注册流程和CALL释放流程同时进行，容易出问题 */
        return ;
    }

    if (IMSA_SRV_STATUS_CONN_DEREGING == IMSA_SRV_GetNormalSrvStatus())
    {
        IMSA_INFO_LOG("IMSA_SRV_ProcDeregReq:wait for dereg result!");
        return ;
    }

    if (IMSA_SRV_STATUS_CONNING_DEREG == IMSA_SRV_GetNormalSrvStatus())
    {
        IMSA_INFO_LOG("IMSA_SRV_ProcDeregReq:wait for Conning result!");

        /* 释放连接，状态转到RELEASING&DEREG态 */
        if( VOS_FALSE == pstControlMagnaer->ucOrgPdnRlsFlag)
        {
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnRelReq(     IMSA_CONN_TYPE_NORMAL,
                                        IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                        IMSA_GetCurrentDomain(),
                                        IMSA_CONN_RELEASE_TYPE_ENUM_LOCAL);
        }
        else
        {   /* coverity[unchecked_value] */
            IMSA_SRV_SndConnRelReq(     IMSA_CONN_TYPE_NORMAL,
                                        IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                        IMSA_GetCurrentDomain(),
                                        IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL);
        }

        IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                    IMSA_SRV_STATUS_RELEASING_DEREG);

        return ;
    }
}


VOS_VOID IMSA_SRV_DomainEnable
(
    IMSA_IMS_DOMAIN_ENUM_UINT8          enDomain
)
{
    IMSA_INFO_LOG("IMSA_SRV_DomainEnable:enter!");

    if (IMSA_IMS_DOMAIN_LTE == enDomain)
    {
        IMSA_SRV_StartCellularImsService();
    }
    else if (IMSA_IMS_DOMAIN_WIFI == enDomain)
    {
        IMSA_SRV_StartWifiImsService();
    }
    else
    {
    }
}


VOS_VOID IMSA_SRV_DomainDisable
(
    IMSA_IMS_DOMAIN_ENUM_UINT8          enDomain,
    VOS_UINT8                           ucPdnRlsFlag
)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_SRV_DomainDisable:enter!");

    pstControlMagnaer                       = IMSA_GetControlManagerAddress();

    if( (VOS_TRUE == ucPdnRlsFlag)
        || (VOS_FALSE == ucPdnRlsFlag))
    {
        pstControlMagnaer->ucOrgPdnRlsFlag = ucPdnRlsFlag;
    }
    else
    {
        pstControlMagnaer->ucOrgPdnRlsFlag = VOS_TRUE;
    }

    IMSA_SrvccAbormalClearBuff(IMSA_SRVCC_ABNORMAL_DEREG_REQ);

    if (IMSA_IMS_DOMAIN_LTE == enDomain)
    {
        IMSA_SRV_StopCellularImsService();
    }
    else if (IMSA_IMS_DOMAIN_WIFI == enDomain)
    {
        IMSA_SRV_StopWifiImsService();
    }
    else
    {
    }
}




VOS_VOID IMSA_SRV_DomainContinue
(
    IMSA_IMS_DOMAIN_ENUM_UINT8          enDomain
)
{
    IMSA_INFO_LOG("IMSA_SRV_DomainContinue:enter!");

    if (IMSA_IMS_DOMAIN_LTE == enDomain)
    {
        IMSA_SRV_StartCellularImsService();
    }
    else if (IMSA_IMS_DOMAIN_WIFI == enDomain)
    {
        IMSA_SRV_StartWifiImsService();
    }
    else
    {
       /* 停留在当前状态,不进行处理 */
    }


    return;
}


VOS_VOID  IMSA_SRV_DomainSwitchAbort
(
    IMSA_DOMAIN_SWITCH_TYPE_ENUM_UINT8 enSwitchType,
    VOS_UINT8                          ucNewPdnRlsFlag
)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;
    IMSA_IMS_DOMAIN_ENUM_UINT8           enConnRatType;
    IMSA_SRV_STATUS_ENUM_UINT8          enNrmSrvStatus      = IMSA_SRV_STATUS_BUTT;

    pstControlMagnaer                  = IMSA_GetControlManagerAddress();
    enNrmSrvStatus                     = IMSA_SRV_GetNormalSrvStatus();
    if(enSwitchType !=  pstControlMagnaer->enSwitchType )
    {
        IMSA_INFO_LOG("IMSA_SRV_DomainSwitchAbort:abort switch type error!");
        return;
    }


    /* 设置CONN RAT TYPE*/
    if( IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH == enSwitchType)
    {
        enConnRatType = IMSA_IMS_DOMAIN_WIFI;
    }
    else
    {
        enConnRatType = IMSA_IMS_DOMAIN_LTE;
    }

    switch (enNrmSrvStatus)
    {
        case IMSA_SRV_STATUS_HO_DEREG:
        case IMSA_SRV_STATUS_HO_REG:
            /* 指定切换Abort，是否本地释放切换域的连接 */
            if( VOS_TRUE == ucNewPdnRlsFlag)
            {
                IMSA_SRV_SndConnRelReq(IMSA_CONN_TYPE_NORMAL,
                                       IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                       enConnRatType,
                                       IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL);

            }
            else
            {
                IMSA_SRV_SndConnRelReq(IMSA_CONN_TYPE_NORMAL,
                                       IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                       enConnRatType,
                                       IMSA_CONN_RELEASE_TYPE_ENUM_LOCAL);
            }
            /* 仍旧停留在切换状态 */
            break;
        default:
            /* 直接返回切换失败，保持当前状态 */
            IMSA_SRV_SndDomainSwitchResultInd(enSwitchType,IMSA_DOMAIN_SWITCH_RESULT_FAILURE);

            IMSA_INFO_LOG("IMSA_SRV_DomainSwitchAbort:SRV is not in HO state!");
            break;
    }

    return;
}


VOS_VOID IMSA_SRV_DomainSwitchInReleasingDereg
(
    IMSA_DOMAIN_SWITCH_TYPE_ENUM_UINT8 enSwitchType,
    VOS_UINT8   ucHoFailReturn2OrgPdnFlag,
    VOS_UINT8   ucOrgPdnRlsFlag
)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;

    pstControlMagnaer                  = IMSA_GetControlManagerAddress();

    if( IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH == enSwitchType)
    {
        pstControlMagnaer->enImsaSrvCurrentDomain = IMSA_IMS_DOMAIN_MODE_WIFI_ONLY;
    }
    else
    {
        pstControlMagnaer->enImsaSrvCurrentDomain = IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY;
    }

    IMSA_SRV_SndDomainSwitchResultInd(enSwitchType,IMSA_DOMAIN_SWITCH_RESULT_SUCCESS);

    if( IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH == enSwitchType)
    {
        /* 修改释放类型为加速释放，避免释放处于另外一个CONN建立之后，导致REG误使用失效的地址 */
        IMSA_SRV_SndConnRelReq(IMSA_CONN_TYPE_NORMAL,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_IMS_DOMAIN_LTE,
                                    IMSA_CONN_RELEASE_TYPE_ENUM_SPEEDUP);

        IMSA_SRV_SndConnSetupReq(   IMSA_FALSE,
                                IMSA_CONN_TYPE_NORMAL,
                                IMSA_IMS_DOMAIN_WIFI);

    }
    else
    {
        /* 修改释放类型为加速释放，避免释放处于另外一个CONN建立之后，导致REG误使用失效的地址 */
        IMSA_SRV_SndConnRelReq(IMSA_CONN_TYPE_NORMAL,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_IMS_DOMAIN_WIFI,
                                    IMSA_CONN_RELEASE_TYPE_ENUM_SPEEDUP);


        /* 建立切换域的链接建立 */
        IMSA_SRV_SndConnSetupReq(   IMSA_FALSE,
                                IMSA_CONN_TYPE_NORMAL,
                                IMSA_IMS_DOMAIN_LTE);
    }

    /* 设置状态为CONNING&DEREG */
    IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                IMSA_SRV_STATUS_CONNING_DEREG);
    /* 保存切换参数 */
    IMSA_SRV_SaveSwitchFlag( enSwitchType, ucHoFailReturn2OrgPdnFlag, ucOrgPdnRlsFlag);

}


VOS_VOID IMSA_SRV_DomainSwitchInIdleDereg
(
    IMSA_DOMAIN_SWITCH_TYPE_ENUM_UINT8 enSwitchType,
    VOS_UINT8   ucHoFailReturn2OrgPdnFlag,
    VOS_UINT8   ucOrgPdnRlsFlag,
    IMSA_IMS_DOMAIN_ENUM_UINT8     enConnRatType
)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;

    pstControlMagnaer                  = IMSA_GetControlManagerAddress();

    if( IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH == enSwitchType)
    {
        pstControlMagnaer->enImsaSrvCurrentDomain = IMSA_IMS_DOMAIN_MODE_WIFI_ONLY;
    }
    else
    {
        pstControlMagnaer->enImsaSrvCurrentDomain = IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY;
    }

    IMSA_SRV_SndDomainSwitchResultInd(enSwitchType,IMSA_DOMAIN_SWITCH_RESULT_SUCCESS);

    IMSA_SRV_SndConnSetupReq(   IMSA_FALSE,
                                IMSA_CONN_TYPE_NORMAL,
                                 enConnRatType);

    /* 设置状态为CONNING&DEREG */
    IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_CONNING_DEREG);
    /* 保存切换参数 */
    IMSA_SRV_SaveSwitchFlag( enSwitchType, ucHoFailReturn2OrgPdnFlag, ucOrgPdnRlsFlag);

}


VOS_VOID IMSA_SRV_DomainSwitchInConningDereg
(
    IMSA_DOMAIN_SWITCH_TYPE_ENUM_UINT8 enSwitchType,
    VOS_UINT8   ucHoFailReturn2OrgPdnFlag,
    VOS_UINT8   ucOrgPdnRlsFlag,
    IMSA_IMS_DOMAIN_ENUM_UINT8     enConnRatType
)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;

    pstControlMagnaer                  = IMSA_GetControlManagerAddress();

    /* 非稳态强制切换，释放现有链路 */
    /* 修改释放类型为加速释放，避免释放处于另外一个CONN建立之后，导致REG误使用失效的地址 */

    if( IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH == enSwitchType)
    {
        pstControlMagnaer->enImsaSrvCurrentDomain = IMSA_IMS_DOMAIN_MODE_WIFI_ONLY;

        IMSA_SRV_SndConnRelReq(IMSA_CONN_TYPE_NORMAL,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_IMS_DOMAIN_LTE,
                                    IMSA_CONN_RELEASE_TYPE_ENUM_SPEEDUP);
    }
    else
    {
        pstControlMagnaer->enImsaSrvCurrentDomain = IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY;

        IMSA_SRV_SndConnRelReq(IMSA_CONN_TYPE_NORMAL,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_IMS_DOMAIN_WIFI,
                                    IMSA_CONN_RELEASE_TYPE_ENUM_SPEEDUP);
    }

    IMSA_SRV_SndConnSetupReq(   IMSA_FALSE,
                                IMSA_CONN_TYPE_NORMAL,
                                 enConnRatType);

    /* 回复切换成功 */
    IMSA_SRV_SndDomainSwitchResultInd(enSwitchType,IMSA_DOMAIN_SWITCH_RESULT_SUCCESS);

    /* 切换状态到CONNING&DEREG */
    IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                IMSA_SRV_STATUS_CONNING_DEREG);

    /* 保存切换参数 */
    IMSA_SRV_SaveSwitchFlag( enSwitchType, ucHoFailReturn2OrgPdnFlag, ucOrgPdnRlsFlag);

}



VOS_VOID IMSA_SRV_DomainSwitchInConningReg
(
    IMSA_DOMAIN_SWITCH_TYPE_ENUM_UINT8 enSwitchType,
    VOS_UINT8   ucHoFailReturn2OrgPdnFlag,
    VOS_UINT8   ucOrgPdnRlsFlag,
    IMSA_IMS_DOMAIN_ENUM_UINT8     enConnRatType
)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;

    pstControlMagnaer                  = IMSA_GetControlManagerAddress();

    /* 非稳态强制切换，释放现有链路 */
    if( IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH == enSwitchType)
    {
        pstControlMagnaer->enImsaSrvCurrentDomain = IMSA_IMS_DOMAIN_MODE_WIFI_ONLY;

        IMSA_SRV_SndConnRelReq(IMSA_CONN_TYPE_NORMAL,
                               IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                               IMSA_IMS_DOMAIN_LTE,
                               IMSA_CONN_RELEASE_TYPE_ENUM_SPEEDUP);
    }
    else
    {
        pstControlMagnaer->enImsaSrvCurrentDomain = IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY;

        IMSA_SRV_SndConnRelReq(IMSA_CONN_TYPE_NORMAL,
                               IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                               IMSA_IMS_DOMAIN_WIFI,
                               IMSA_CONN_RELEASE_TYPE_ENUM_SPEEDUP);
    }

    /* 本地去注册*/
    IMSA_SRV_SndRegDeregReq(    IMSA_REG_TYPE_NORMAL,
                                IMSA_DEREG_LOCAL);
    /* 建立目标接入技术链接 */
    IMSA_SRV_SndConnSetupReq(   IMSA_FALSE,
                                IMSA_CONN_TYPE_NORMAL,
                                 enConnRatType);
    /* 回复切换成功 */
    IMSA_SRV_SndDomainSwitchResultInd(enSwitchType,IMSA_DOMAIN_SWITCH_RESULT_SUCCESS);


    /* 切换状态到CONNING&DEREG */
    IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                IMSA_SRV_STATUS_CONNING_DEREG);

    /* 保存切换参数 */
    IMSA_SRV_SaveSwitchFlag( enSwitchType, ucHoFailReturn2OrgPdnFlag, ucOrgPdnRlsFlag);

}



VOS_VOID IMSA_SRV_DomainSwitchInHoReg
(
    IMSA_DOMAIN_SWITCH_TYPE_ENUM_UINT8 enSwitchType,
    VOS_UINT8   ucHoFailReturn2OrgPdnFlag,
    VOS_UINT8   ucOrgPdnRlsFlag,
    IMSA_IMS_DOMAIN_ENUM_UINT8     enConnRatType
)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;

    pstControlMagnaer                  = IMSA_GetControlManagerAddress();

    /* 切换过程中,又发起切换,目的是为了修改切换参数,不做其他用途 */
    if( (ucOrgPdnRlsFlag != pstControlMagnaer->ucOrgPdnRlsFlag)
        &&(enSwitchType == pstControlMagnaer->enSwitchType))
    {
        IMSA_SRV_SndConnHoReq(    enSwitchType,
                                  IMSA_CONN_TYPE_NORMAL,
                                  enConnRatType,
                                  ucOrgPdnRlsFlag);
        /* 保存切换参数 */
        IMSA_SRV_SaveSwitchFlag( enSwitchType, ucHoFailReturn2OrgPdnFlag, ucOrgPdnRlsFlag);
    }
    else if((ucHoFailReturn2OrgPdnFlag != pstControlMagnaer->ucHoFailReturn2OrgPdnFlag)
        &&(enSwitchType == pstControlMagnaer->enSwitchType))
    {
         /* 保存切换参数 */
         IMSA_SRV_SaveSwitchFlag( enSwitchType, ucHoFailReturn2OrgPdnFlag, ucOrgPdnRlsFlag);

    }
    else
    {
        /* 异常操作 */
    }

}


VOS_UINT32 IMSA_SRV_DomainSwitchCheckParmValid
(
    IMSA_DOMAIN_SWITCH_TYPE_ENUM_UINT8 enSwitchType,
    VOS_UINT8   ucHoFailReturn2OrgPdnFlag,
    VOS_UINT8   ucOrgPdnRlsFlag
)
{
    if( ((IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH != enSwitchType)
        &&(IMSA_DOMAIN_SWITCH_TYPE_W2L_SWITCH != enSwitchType))
        || ((VOS_TRUE != ucHoFailReturn2OrgPdnFlag)
           &&(VOS_FALSE != ucHoFailReturn2OrgPdnFlag))
        ||((VOS_TRUE != ucOrgPdnRlsFlag)
           &&(VOS_FALSE != ucOrgPdnRlsFlag)))
    {
        IMSA_INFO_LOG("IMSA_SRV_DomainSwitch: Param Error!");
        IMSA_SRV_SndDomainSwitchResultInd(enSwitchType,IMSA_DOMAIN_SWITCH_RESULT_FAILURE);
        return IMSA_FALSE;
    }

    return IMSA_TRUE;
}



VOS_VOID IMSA_SRV_DomainSwitch
(
    IMSA_DOMAIN_SWITCH_TYPE_ENUM_UINT8 enSwitchType,
    VOS_UINT8   ucHoFailReturn2OrgPdnFlag,
    VOS_UINT8   ucOrgPdnRlsFlag
)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;
    IMSA_SRV_STATUS_ENUM_UINT8          enNrmSrvStatus      = IMSA_SRV_STATUS_BUTT;
    IMSA_IMS_DOMAIN_ENUM_UINT8           enConnRatType;

    pstControlMagnaer     = IMSA_GetControlManagerAddress();
    enNrmSrvStatus        = IMSA_SRV_GetNormalSrvStatus();
    IMSA_INFO_LOG("IMSA_SRV_DomainSwitch:Enter!");

    /* 检查参数合法性 */
    if(IMSA_FALSE ==IMSA_SRV_DomainSwitchCheckParmValid(enSwitchType,ucHoFailReturn2OrgPdnFlag,ucOrgPdnRlsFlag))
    {
        return;
    }

    /* 设置Target CONN RAT TYPE*/
    if( IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH == enSwitchType)
    {
        enConnRatType = IMSA_IMS_DOMAIN_WIFI;
        pstControlMagnaer->enImsaSrvCurrentDomain = IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY;
    }
    else
    {
        enConnRatType = IMSA_IMS_DOMAIN_LTE;
        pstControlMagnaer->enImsaSrvCurrentDomain = IMSA_IMS_DOMAIN_MODE_WIFI_ONLY;
    }

    /* 根据当前服务状态进行不同处理 */
    switch (enNrmSrvStatus)
    {
        case IMSA_SRV_STATUS_CONN_DEREG:
            /* 请求连接模块发起切换过程，将状态转到IMSA_SERVICE_STATUS_HO_DEREG */
            IMSA_SRV_SndConnHoReq(      enSwitchType,
                                        IMSA_CONN_TYPE_NORMAL,
                                        enConnRatType,
                                        ucOrgPdnRlsFlag);
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_HO_DEREG);
            /* 保存切换参数 */
            IMSA_SRV_SaveSwitchFlag( enSwitchType, ucHoFailReturn2OrgPdnFlag, ucOrgPdnRlsFlag);
            break;

        case IMSA_SRV_STATUS_RELEASING_DEREG:

            IMSA_SRV_DomainSwitchInReleasingDereg(enSwitchType,ucHoFailReturn2OrgPdnFlag,ucOrgPdnRlsFlag);

            break;


        case IMSA_SRV_STATUS_IDLE_DEREG:
            IMSA_SRV_DomainSwitchInIdleDereg(enSwitchType,ucHoFailReturn2OrgPdnFlag,ucOrgPdnRlsFlag,enConnRatType);
            break;

        case IMSA_SRV_STATUS_CONNING_DEREG:

            IMSA_SRV_DomainSwitchInConningDereg(enSwitchType,ucHoFailReturn2OrgPdnFlag,ucOrgPdnRlsFlag,enConnRatType);

            break;

        case IMSA_SRV_STATUS_CONNING_REG:
            IMSA_SRV_DomainSwitchInConningReg(enSwitchType,ucHoFailReturn2OrgPdnFlag,ucOrgPdnRlsFlag,enConnRatType);
            break;

        case IMSA_SRV_STATUS_CONN_REGING:
        case IMSA_SRV_STATUS_CONN_DEREGING:
            /* 本地去注册*/
            IMSA_SRV_SndRegDeregReq(    IMSA_REG_TYPE_NORMAL,
                                        IMSA_DEREG_LOCAL);
            /* 发起切换流程 */
            IMSA_SRV_SndConnHoReq(      enSwitchType,
                                        IMSA_CONN_TYPE_NORMAL,
                                        enConnRatType,
                                        ucOrgPdnRlsFlag);
            /* 转移状态到HO&DEREG */
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_HO_DEREG);

            /* 保存切换参数 */
            IMSA_SRV_SaveSwitchFlag( enSwitchType, ucHoFailReturn2OrgPdnFlag, ucOrgPdnRlsFlag);


            break;

        case IMSA_SRV_STATUS_CONN_REG:
            /* 请求连接模块发起切换过程，将状态转到IMSA_SERVICE_STATUS_HO_REG */
            IMSA_SRV_SndConnHoReq(      enSwitchType,
                                      IMSA_CONN_TYPE_NORMAL,
                                      enConnRatType,
                                      ucOrgPdnRlsFlag);
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                      IMSA_SRV_STATUS_HO_REG);

            /* 保存切换参数 */
            IMSA_SRV_SaveSwitchFlag( enSwitchType, ucHoFailReturn2OrgPdnFlag, ucOrgPdnRlsFlag);

            break;

        case IMSA_SRV_STATUS_HO_REG:
        case IMSA_SRV_STATUS_HO_DEREG:
            IMSA_SRV_DomainSwitchInHoReg(enSwitchType,ucHoFailReturn2OrgPdnFlag,ucOrgPdnRlsFlag,enConnRatType);
            break;
        default:

            break;
    }

    return;
}


VOS_VOID IMSA_SRV_SwitchSuccNtf
(
    IMSA_CONN_HO_CNF_STRU* pstConnHoCnf
)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlManager   = VOS_NULL_PTR;
    pstControlManager                  = IMSA_GetControlManagerAddress();

    IMSA_INFO_LOG("IMSA_SRV_SwitchSuccNtf:enter!");
    if( IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH == pstControlManager->enSwitchType)
    {
        pstControlManager->enImsaSrvCurrentDomain = IMSA_IMS_DOMAIN_MODE_WIFI_ONLY;
    }
    else
    {
        pstControlManager->enImsaSrvCurrentDomain = IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY;
    }


    /* 返回切换成功 */
    IMSA_SRV_SndDomainSwitchResultInd(pstControlManager->enSwitchType,IMSA_DOMAIN_SWITCH_RESULT_SUCCESS);

    if( IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH == pstControlManager->enSwitchType)
    {
        IMSA_SRV_SrvStatusNotify(IMSA_SRV_GetNormalSrvStatus(), IMSA_SRV_STATUS_IDLE_DEREG, IMSA_IMS_DOMAIN_WIFI);

        IMSA_SndMsgAtImsRatHandoverInd(IMSA_AT_IMS_RAT_HO_STATUS_SUCCESS,IMSA_AT_IMS_RAT_TYPE_LTE,IMSA_AT_IMS_RAT_TYPE_WIFI, pstConnHoCnf->enHoResult);
    }
    else
    {
        IMSA_SRV_SrvStatusNotify(IMSA_SRV_GetNormalSrvStatus(), IMSA_SRV_STATUS_IDLE_DEREG, IMSA_IMS_DOMAIN_LTE);

        IMSA_SndMsgAtImsRatHandoverInd(IMSA_AT_IMS_RAT_HO_STATUS_SUCCESS,IMSA_AT_IMS_RAT_TYPE_WIFI,IMSA_AT_IMS_RAT_TYPE_LTE, pstConnHoCnf->enHoResult);
    }
}


VOS_VOID IMSA_SRV_SwitchFailNtf
(
    IMSA_CONN_HO_CNF_STRU* pstConnHoCnf
)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlManager   = VOS_NULL_PTR;
    pstControlManager                  = IMSA_GetControlManagerAddress();

    IMSA_INFO_LOG("IMSA_SRV_SwitchFailNtf:enter!");
    if( IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH == pstControlManager->enSwitchType)
    {
        pstControlManager->enImsaSrvCurrentDomain = IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY;
    }
    else
    {
        pstControlManager->enImsaSrvCurrentDomain = IMSA_IMS_DOMAIN_MODE_WIFI_ONLY;
    }


    /* 返回切换失败 */
    IMSA_SRV_SndDomainSwitchResultInd(pstControlManager->enSwitchType,IMSA_DOMAIN_SWITCH_RESULT_FAILURE);

    if( IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH == pstControlManager->enSwitchType)
    {
        IMSA_SndMsgAtImsRatHandoverInd(IMSA_AT_IMS_RAT_HO_STATUS_FAILURE,IMSA_AT_IMS_RAT_TYPE_LTE,IMSA_AT_IMS_RAT_TYPE_WIFI,pstConnHoCnf->enHoResult);
    }
    else
    {
        IMSA_SndMsgAtImsRatHandoverInd(IMSA_AT_IMS_RAT_HO_STATUS_FAILURE,IMSA_AT_IMS_RAT_TYPE_WIFI,IMSA_AT_IMS_RAT_TYPE_LTE,pstConnHoCnf->enHoResult);
    }
}


VOS_VOID IMSA_ProcConnHoCnfInHoDereg
(
    IMSA_CONN_HO_CNF_STRU* pstConnHoCnf
)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlManager   = VOS_NULL_PTR;
    IMSA_IMS_DOMAIN_ENUM_UINT8          enConnRatType;

    pstControlManager                  = IMSA_GetControlManagerAddress();


    /* 设置CONN RAT TYPE*/
    if( IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH == pstControlManager->enSwitchType)
    {
        enConnRatType = IMSA_IMS_DOMAIN_LTE;
    }
    else
    {
        enConnRatType = IMSA_IMS_DOMAIN_WIFI;
    }


    if( IMSA_CONN_RESULT_SUCCESS == pstConnHoCnf->enHoResult)
    {
        IMSA_SRV_SwitchSuccNtf(pstConnHoCnf);

        IMSA_SRV_SndRegRegReq(      IMSA_REG_TYPE_NORMAL,
                                    IMSA_REG_ADDR_PARAM_NEW);

        IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                    IMSA_SRV_STATUS_CONN_REGING);
    }
    else
    {
       /*切换失败不能回到原域的处理*/
       if(VOS_FALSE == pstControlManager->ucHoFailReturn2OrgPdnFlag)
       {
            /*原域PDN没有释放*/
            if(VOS_FALSE == pstConnHoCnf->ucOrgPdnRlsFlag)
            {
                IMSA_SRV_SwitchSuccNtf(pstConnHoCnf);

                /* 指定切换失败无法回到原来域: 本地释放原来域的连接 */
                IMSA_SRV_SndConnRelReq(IMSA_CONN_TYPE_NORMAL,
                                       IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                       enConnRatType,
                                       IMSA_CONN_RELEASE_TYPE_ENUM_LOCAL);

                /* 启动周期性尝试IMS服务定时器 */
                IMSA_StartTimer(&pstControlManager->stPeriodImsSrvTryTimer);

                /* 设置状态为IDLE&DEREG */
                IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                            IMSA_SRV_STATUS_IDLE_DEREG);
            }
            else
            {
                IMSA_SRV_SwitchSuccNtf(pstConnHoCnf);

                IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                            IMSA_SRV_STATUS_IDLE_DEREG);

                /* 启动周期性尝试IMS服务定时器 */
                IMSA_StartTimer(&pstControlManager->stPeriodImsSrvTryTimer);
            }
       }
       else
       {
            IMSA_SRV_SwitchFailNtf(pstConnHoCnf);

            if( VOS_FALSE == pstConnHoCnf->ucOrgPdnRlsFlag)
            {
                /* 请求注册模块发起注册，将状态转到IMSA_SERVICE_STATUS_CONN_REGING */
                IMSA_SRV_SndRegRegReq(      IMSA_REG_TYPE_NORMAL,
                                            IMSA_REG_ADDR_PARAM_NEW);

                IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                            IMSA_SRV_STATUS_CONN_REGING);
            }
            else
            {
                IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                            IMSA_SRV_STATUS_IDLE_DEREG);

                /* 启动周期性尝试IMS服务定时器 */
                IMSA_StartTimer(&pstControlManager->stPeriodImsSrvTryTimer);
            }

       }
    }
}


VOS_VOID IMSA_ProcConnHoCnfFailInHoReg
(
    IMSA_CONN_HO_CNF_STRU* pstConnHoCnf
)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlManager   = VOS_NULL_PTR;

    pstControlManager                  = IMSA_GetControlManagerAddress();


    /*切换失败可以回到原域*/
    if(VOS_TRUE == pstControlManager->ucHoFailReturn2OrgPdnFlag)
    {
        /*判决原域PDN是否被释放*/
        if(VOS_FALSE == pstConnHoCnf->ucOrgPdnRlsFlag)
        {
            IMSA_SRV_SwitchFailNtf(pstConnHoCnf);

            /* 设置状态为CONN&REG */
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                IMSA_SRV_STATUS_CONN_REG);
        }
        else
        {
            /*由于可以回到原域，这里直接回复domain模块切换失败，去原域尝试*/
            IMSA_SRV_SwitchFailNtf(pstConnHoCnf);

            /* 通知CALL模块普通呼叫服务进入无服务状态 */
            IMSA_SRV_SndNrmCallSrvStatusInd(IMSA_CALL_SERVICE_STATUS_NO_SERVICE,
                                    IMSA_CALL_NO_SRV_CAUSE_SIP_PDP_ERR,
                                    IMSA_CALL_NORM_SRV_EXTEND_INFO_NONE);

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);

            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                IMSA_SRV_STATUS_IDLE_DEREG);

            /* 启动周期性尝试IMS服务定时器 */
            IMSA_StartTimer(&pstControlManager->stPeriodImsSrvTryTimer);
        }

    }
    else /*切换失败不可以回到原域*/
    {
        /*判决原域PDN是否被释放*/
        if(VOS_FALSE == pstConnHoCnf->ucOrgPdnRlsFlag)
        {
            IMSA_SRV_SwitchFailNtf(pstConnHoCnf);

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);

            /* 设置状态为CONN&DEREGING */
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                IMSA_SRV_STATUS_CONN_DEREGING);
        }
        else
        {
            IMSA_SRV_SwitchSuccNtf(pstConnHoCnf);

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);

            /* 设置状态为IDLE&DEREG */
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                IMSA_SRV_STATUS_IDLE_DEREG);

            /* 启动周期性尝试IMS服务定时器 */
            IMSA_StartTimer(&pstControlManager->stPeriodImsSrvTryTimer);

            /* 通知CALL模块普通呼叫服务进入无服务状态 */
            IMSA_SRV_SndNrmCallSrvStatusInd(IMSA_CALL_SERVICE_STATUS_NO_SERVICE,
                                    IMSA_CALL_NO_SRV_CAUSE_SIP_PDP_ERR,
                                    IMSA_CALL_NORM_SRV_EXTEND_INFO_NONE);
        }
    }

}


VOS_VOID IMSA_ProcConnHoCnfSuccInHoReg
(
    IMSA_CONN_HO_CNF_STRU* pstConnHoCnf
)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlManager   = VOS_NULL_PTR;

    VOS_CHAR                            acUeAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};
    VOS_CHAR                            acPcscfAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};

    pstControlManager                  = IMSA_GetControlManagerAddress();

    if (IMSA_CONN_TYPE_NORMAL == pstConnHoCnf->enConnType)
    {
        IMSA_UtilStrNCpy(acUeAddr, pstControlManager->stNrmRegParaInfo.acUeAddr, IMSA_IPV6_ADDR_STRING_LEN);
        IMSA_UtilStrNCpy(acPcscfAddr, pstControlManager->stNrmRegParaInfo.acPcscfAddr, IMSA_IPV6_ADDR_STRING_LEN);
    }
    else
    {
        IMSA_UtilStrNCpy(acUeAddr, pstControlManager->stEmcRegParaInfo.acUeAddr, IMSA_IPV6_ADDR_STRING_LEN);
        IMSA_UtilStrNCpy(acPcscfAddr, pstControlManager->stEmcRegParaInfo.acPcscfAddr, IMSA_IPV6_ADDR_STRING_LEN);
    }

    IMSA_SRV_SwitchSuccNtf(pstConnHoCnf);

    /* 如果备份的注册参数有效，将备份的注册参数设置为CURRENT参数，将当前服务状态修改为CONN+REG状态 */
    if (IMSA_TRUE == IMSA_IsRegParaAvailable((IMSA_SRV_TYPE_ENUM_UINT8)pstConnHoCnf->enConnType,
                                              acUeAddr,
                                              acPcscfAddr))
    {
        (VOS_VOID)IMSA_SetCurrentPara((IMSA_SRV_TYPE_ENUM_UINT8)pstConnHoCnf->enConnType,
                                      acUeAddr,
                                      acPcscfAddr);
        IMSA_SRV_SetServcieStatus((IMSA_SRV_TYPE_ENUM_UINT8)pstConnHoCnf->enConnType, IMSA_SRV_STATUS_CONN_REG);

        /* 请求REG在新的接入网络下发起重注册过程 */
        if( IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH == pstControlManager->enSwitchType)
        {
            IMSA_SRV_SndRegReregReq((IMSA_SRV_TYPE_ENUM_UINT8)pstConnHoCnf->enConnType,IMSA_REG_BEARER_TYPE_WIFI);
            /* 发送SRV_STATUS_IND消息 */
            IMSA_SRV_SndDomainSrvStatusInd(IMSA_DOMAIN_SERVICE_STATUS_NORMAL_SERVICE,
                                           IMSA_DOMAIN_NO_SRV_CAUSE_BUTT,
                                           IMSA_IMS_DOMAIN_MODE_WIFI_ONLY);
        }
        else
        {
            IMSA_SRV_SndRegReregReq((IMSA_SRV_TYPE_ENUM_UINT8)pstConnHoCnf->enConnType,IMSA_REG_BEARER_TYPE_LTE);
            /* 发送SRV_STATUS_IND消息 */
            IMSA_SRV_SndDomainSrvStatusInd(IMSA_DOMAIN_SERVICE_STATUS_NORMAL_SERVICE,
                                           IMSA_DOMAIN_NO_SRV_CAUSE_BUTT,
                                           IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY);
        }

        /* 如果存在呼叫,触发呼叫上报给SPM, 高层会根据状态上报触发域查询 */
        (VOS_VOID)IMSA_ImsCallStateNtfInHo();

        /* 上报服务状态，AT根据状态获取注册域状态 */
        IMSA_SndMsgAtCireguInd(IMSA_NORMAL_AT_REG_STATUS_REG);

    }
    else
    /* 如果备份的注册参数无效，则将本地去注册，并且通知REG模块重新发起注册 */
    {
        IMSA_SRV_SetServcieStatus((IMSA_SRV_TYPE_ENUM_UINT8)pstConnHoCnf->enConnType, IMSA_SRV_STATUS_CONN_REGING);

        IMSA_SRV_SndRegDeregReq((IMSA_SRV_TYPE_ENUM_UINT8)pstConnHoCnf->enConnType, IMSA_DEREG_LOCAL);

        IMSA_SRV_SndRegRegReq((IMSA_SRV_TYPE_ENUM_UINT8)pstConnHoCnf->enConnType, IMSA_REG_ADDR_PARAM_NEW);
    }

}



VOS_VOID IMSA_SRV_ProcConnHoCnfMsg
(
    IMSA_CONN_HO_CNF_STRU* pstConnHoCnf
)
{
    IMSA_SRV_STATUS_ENUM_UINT8          enNrmSrvStatus      = IMSA_SRV_STATUS_BUTT;


    enNrmSrvStatus = IMSA_SRV_GetNormalSrvStatus();

    /* 根据当前服务状态进行不同处理 */
    switch (enNrmSrvStatus)
    {
        case IMSA_SRV_STATUS_HO_DEREG:
            IMSA_ProcConnHoCnfInHoDereg(pstConnHoCnf);
            break;

        case IMSA_SRV_STATUS_HO_REG:
            if( IMSA_CONN_RESULT_SUCCESS == pstConnHoCnf->enHoResult)
            {
                IMSA_ProcConnHoCnfSuccInHoReg(pstConnHoCnf);
            }
            else
            {
                IMSA_ProcConnHoCnfFailInHoReg(pstConnHoCnf);
            }
            break;
        default:

            break;
    }

   /* 清除切换变量设置 */
    IMSA_SRV_SaveSwitchFlag( IMSA_DOMAIN_SWITCH_TYPE_NO_SWITCH, VOS_TRUE, VOS_TRUE);

    return;
}


IMSA_IMS_DOMAIN_ENUM_UINT8 IMSA_SRV_GetCurrentDomainInHo(VOS_VOID)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlManager   = VOS_NULL_PTR;
    IMSA_IMS_DOMAIN_ENUM_UINT8          enCurrentDomain;

    pstControlManager                       = IMSA_GetControlManagerAddress();

    enCurrentDomain = pstControlManager->enImsaSrvCurrentDomain;

    /* Domain只能有三种取值: NULL, LTE, WIFI */
    if( (IMSA_IMS_DOMAIN_WIFI != enCurrentDomain)
        &&(IMSA_IMS_DOMAIN_LTE != enCurrentDomain))
    {
        enCurrentDomain = IMSA_IMS_DOMAIN_NULL;
    }

    return enCurrentDomain;
}


VOS_VOID IMSA_SRV_ProcWifiMsgStateInd
(
    WIFI_IMSA_STATE_IND_STRU* pstStateInd
)
{
    IMSA_SRV_STATUS_ENUM_UINT8          enNrmSrvStatus      = IMSA_SRV_STATUS_BUTT;
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;

    pstControlMagnaer                       = IMSA_GetControlManagerAddress();

    switch(IMSA_SRV_IsWifiNrmSrvConditonSatisfied())
    {
        case IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_USIM_NOT_SUPPORT:
        case IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_RAT_NOT_SUPPORT:
        case IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_VOICE_NOT_SUPPORT:
            IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE,IMSA_CONN_IsExitPersistentBearer());
            return;

        case IMSA_NRM_SRV_CON_SAT_STATUE_FAIL_OTHERS:
            /* IMS语音不支持或者PS域服务状态不为NORMAL SERVICE时，无特殊处理 */
            return;

        default:
            break;
    }

    enNrmSrvStatus = IMSA_SRV_GetNormalSrvStatus();

    /* 根据当前服务状态进行不同处理 */
    switch (enNrmSrvStatus)
    {
        case IMSA_SRV_STATUS_IDLE_DEREG:

            /* 请求连接模块建立连接，将状态转到IMSA_SERVICE_STATUS_CONNING_DEREG */
            IMSA_SRV_SndConnSetupReq(   IMSA_FALSE,
                                        IMSA_CONN_TYPE_NORMAL,
                                        IMSA_IMS_DOMAIN_WIFI);
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_CONNING_DEREG);
            break;

        case IMSA_SRV_STATUS_RELEASING_DEREG:

            /* 启动周期性尝试IMS服务定时器 */
            IMSA_StartTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer);
            break;

        case IMSA_SRV_STATUS_CONN_DEREGING:

            /* 如果是MMA DEREG REQ类型的去注册，则无操作；
               后续增加了其他类型的去注册，则需要另外考虑，例如IMS特性改为不支持
               导致的去注册过程中，IMS特性重新设为支持，则需要在去注册动作完成后，
               重新再注册，而不释放连接 */
            IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE,IMSA_CONN_IsExitPersistentBearer());

            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_CONN_REGING);

            IMSA_SRV_SndRegDeregReq(    IMSA_REG_TYPE_NORMAL,
                                        IMSA_DEREG_LOCAL);

            IMSA_SRV_SndRegRegReq(      IMSA_REG_TYPE_NORMAL,
                                        IMSA_REG_ADDR_PARAM_NEW);

            /* IMSA_StartTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer); */
            break;

        case IMSA_SRV_STATUS_CONN_DEREG:

            /* 请求注册模块发起注册，将状态转到IMSA_SERVICE_STATUS_CONN_REGING */
            IMSA_SRV_SndRegRegReq(      IMSA_REG_TYPE_NORMAL,
                                        IMSA_REG_ADDR_PARAM_NEW);
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_CONN_REGING);
            break;
        case IMSA_SRV_STATUS_CONNING_DEREG:
        case IMSA_SRV_STATUS_CONN_REGING:
        case IMSA_SRV_STATUS_CONNING_REG:
            /*这三个状态等注册完成后上报IMS voice能力*/
            break;
        default:
            /*IMSA不发起注册，需上报IMS voice能力*/
            if (IMSA_TRUE == IMSA_IsImsVoiceContidionSatisfied())
            {
                IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_AVAILABLE,IMSA_CONN_IsExitPersistentBearer());
            }
            else
            {
                IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE,IMSA_CONN_IsExitPersistentBearer());
            }

            break;
    }


}






VOS_VOID IMSA_SRV_RegStatusNtfEvent()
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlManager   = VOS_NULL_PTR;

    pstControlManager                       = IMSA_GetControlManagerAddress();

    /* 指定Domain注册了SRV状态事件，SRV进入CONN&REG, CONN&DEREG会上报事件给Domain */
    pstControlManager->ucDomainRegSrvStatusFlag = VOS_TRUE;

    return;
}



#if (FEATURE_ON == FEATURE_DSDS)

VOS_UINT32 IMSA_SRV_IsNotNeedNwDereg
(
    VOS_VOID
)
{
    IMSA_CONTROL_MANAGER_STRU      *pstControlManager;

    IMSA_INFO_LOG("IMSA_SRV_ProcVoiceDomainChangeInd is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcVoiceDomainChangeInd_ENUM, LNAS_ENTRY);

    pstControlManager = IMSA_GetControlManagerAddress();

    if (IMSA_SRV_STATUS_IDLE_DEREG == IMSA_SRV_GetNormalSrvStatus())
    {
        return IMSA_TRUE;
    }

    if (IMSA_SRV_STATUS_CONNING_REG == IMSA_SRV_GetNormalSrvStatus())
    {
        /* 需要通知IMS 允许发包，然后本地去注册 */
        IMSA_SndImsMsgServiceSuspendOrResumeSrvInfo(IMSA_IMS_INPUT_SERVICE_REASON_RESUME_NRM_SRV);

        IMSA_StopTimer(&pstControlManager->stPeriodImsSrvTryTimer);

        IMSA_SRV_SndRegDeregReq(    IMSA_REG_TYPE_NORMAL,
                                    IMSA_DEREG_LOCAL);

        return IMSA_TRUE;
    }

    return IMSA_FALSE;
}

VOS_VOID IMSA_SRV_ProcStopMsgWhenNoRf
(
    VOS_VOID
)
{
    IMSA_INFO_LOG("IMSA_SRV_ProcVoiceDomainChangeInd is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_SRV_ProcVoiceDomainChangeInd_ENUM, LNAS_ENTRY);

    switch(IMSA_SRV_GetNormalSrvStatus())
    {
        /* 仅需要通知IMS本地去注册即可，承载是否需要删除,在收到IMS关机相应后，根据关机
        类型决定是否要通知APS删除 */
        case IMSA_SRV_STATUS_CONN_REGING:
        case IMSA_SRV_STATUS_CONN_DEREGING:
        case IMSA_SRV_STATUS_CONN_REG:

            IMSA_RegNotifyImsLocalDereg();
            break;

        default:
            break;
    }

    return;
}


VOS_UINT32 IMSA_SRV_ProcGetNormSevResourceApplyCnf
(
    IMSA_REG_ADDR_PARAM_ENUM_UINT32     enAddrType
)
{
    IMSA_SRV_STATUS_ENUM_UINT8          enNrmSrvStatus      = IMSA_SRV_STATUS_BUTT;
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;

    enNrmSrvStatus = IMSA_SRV_GetNormalSrvStatus();
    pstControlMagnaer = IMSA_GetControlManagerAddress();

    if (IMSA_NRM_SRV_CON_SAT_STATUE_SUCC != IMSA_SRV_IsNrmSrvConditonSatisfied())
    {
        return IMSA_FALSE;
    }

    /* 根据当前服务状态进行不同处理 */
    switch (enNrmSrvStatus)
    {
        case IMSA_SRV_STATUS_IDLE_DEREG:

            /* 请求连接模块建立连接，将状态转到IMSA_SERVICE_STATUS_CONNING_DEREG */
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnSetupReq(   IMSA_FALSE,
                                        IMSA_CONN_TYPE_NORMAL,
                                        IMSA_GetCurrentDomain());
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_CONNING_DEREG);

            /* 启动注册时长保护定时器 */
            IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);

            break;

        case IMSA_SRV_STATUS_CONN_DEREG:
            /* 如果当前状态为ACTIVE，则直接发起注册 */
            /* 请求注册模块发起注册，将状态转到IMSA_SERVICE_STATUS_CONN_REGING */
            IMSA_SRV_SndRegRegReq(      IMSA_REG_TYPE_NORMAL,
                                        enAddrType);
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_CONN_REGING);

            /* 启动注册时长保护定时器 */
            IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
            break;

        case IMSA_SRV_STATUS_CONNING_REG:
            /* 请求连接模块建立连接 */
            /* coverity[unchecked_value] */
            IMSA_SRV_SndConnSetupReq(   IMSA_TRUE,
                                        IMSA_CONN_TYPE_NORMAL,
                                        IMSA_GetCurrentDomain());

            /* 启动注册时长保护定时器 */
            IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
            break;

        default:
            /* 打印状态 */
            return IMSA_FALSE;
    }
    return IMSA_TRUE;
}

VOS_UINT32 IMSA_SRV_ProcPeriodRegResourceApplyCnf
(
    IMSA_REG_ADDR_PARAM_ENUM_UINT32     enAddrType
)
{
    IMSA_SRV_STATUS_ENUM_UINT8          enNrmSrvStatus      = IMSA_SRV_STATUS_BUTT;
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;

    enNrmSrvStatus = IMSA_SRV_GetNormalSrvStatus();
    pstControlMagnaer = IMSA_GetControlManagerAddress();

    if (IMSA_NRM_SRV_CON_SAT_STATUE_SUCC != IMSA_SRV_IsNrmSrvConditonSatisfied())
    {
        return IMSA_FALSE;
    }

    /* 根据当前服务状态进行不同处理 */
    switch (enNrmSrvStatus)
    {
        case IMSA_SRV_STATUS_CONN_REGING:
            /* 如果当前状态为ACTIVE，则直接发起注册 */
            /* 请求注册模块发起注册，将状态转到IMSA_SERVICE_STATUS_CONN_REGING */
            IMSA_SRV_SndRegRegReq(      IMSA_REG_TYPE_NORMAL,
                                        enAddrType);
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_CONN_REGING);

            /* 启动注册时长保护定时器 */
            IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
            break;

        default:
            /* 打印状态 */
            return IMSA_FALSE;
    }
    return IMSA_TRUE;
}


VOS_VOID  IMSA_SRV_ProcGetNorServiceResourceApplyExp
(
    VOS_VOID
)
{
    switch (IMSA_SRV_GetNormalSrvStatus())
    {
        case IMSA_SRV_STATUS_IDLE_DEREG:
        case IMSA_SRV_STATUS_CONN_DEREG:

            IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE, IMSA_CONN_IsExitPersistentBearer());
            break;
        case IMSA_SRV_STATUS_CONNING_REG:

            IMSA_SndImsMsgServiceSuspendOrResumeSrvInfo(IMSA_IMS_INPUT_SERVICE_REASON_RESUME_NRM_SRV);

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_IDLE_DEREG);
            break;

        case IMSA_SRV_STATUS_CONN_REGING:

            //IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE, IMSA_CONN_IsExitPersistentBearer());
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_CONN_DEREG);
            break;

        default:
            break;
    }
}


VOS_UINT32 IMSA_SRV_ProcPeriodRegResourceApplyExp
(
    VOS_VOID
)
{
    switch (IMSA_SRV_GetNormalSrvStatus())
    {
        case IMSA_SRV_STATUS_CONN_REGING:
            IMSA_SRV_SetServcieStatus(  IMSA_SRV_TYPE_NORMAL,
                                        IMSA_SRV_STATUS_CONN_DEREG);

            break;

        case IMSA_SRV_STATUS_CONNING_REG:

            IMSA_SndImsMsgServiceSuspendOrResumeSrvInfo(IMSA_IMS_INPUT_SERVICE_REASON_RESUME_NRM_SRV);

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_IDLE_DEREG);

            break;

        default:
            /* 打印状态 */
            return IMSA_FALSE;
    }
    return IMSA_TRUE;
}


VOS_UINT32 IMSA_SRV_ProcDeRegResourceApplyCnf
(
    VOS_VOID
)
{
    IMSA_CONTROL_MANAGER_STRU                  *pstControlMagnaer   = VOS_NULL_PTR;
    IMSA_CONN_RELEASE_TYPE_ENUM_UINT8           enConnRlsType;

    pstControlMagnaer                       = IMSA_GetControlManagerAddress();

    /* 设置PDN释放类型 */
    if( VOS_TRUE == pstControlMagnaer->ucOrgPdnRlsFlag)
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL;
    }
    else
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_LOCAL;
    }

    switch(IMSA_SRV_GetNormalSrvStatus())
    {
        case IMSA_SRV_STATUS_CONN_DEREGING:
            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_NOT_LOCAL);
            IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
            //IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_CONN_DEREGING);
            break;

        case IMSA_SRV_STATUS_RELEASING_DEREG:
            IMSA_SRV_SndConnRelReq(IMSA_SRV_TYPE_NORMAL, IMSA_CONN_SIP_PDP_TYPE_SIGNAL,IMSA_IMS_DOMAIN_LTE,enConnRlsType);
            IMSA_StartTimer(&pstControlMagnaer->stGetSerProtectTimer);
            break;

        default:
            return IMSA_FALSE;
    }
    return IMSA_TRUE;
}

VOS_UINT32 IMSA_SRV_ProcDeRegResourceApplyTimerExp
(
    VOS_VOID
)
{
    IMSA_CONTROL_MANAGER_STRU                  *pstControlMagnaer   = VOS_NULL_PTR;
    IMSA_CONN_RELEASE_TYPE_ENUM_UINT8           enConnRlsType;

    pstControlMagnaer                       = IMSA_GetControlManagerAddress();

    /* 设置PDN释放类型 */
    if( VOS_TRUE == pstControlMagnaer->ucOrgPdnRlsFlag)
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL;
    }
    else
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_LOCAL;
    }

    switch(IMSA_SRV_GetNormalSrvStatus())
    {
        case IMSA_SRV_STATUS_CONN_DEREGING:
            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);
            IMSA_SRV_SndConnRelReq(IMSA_SRV_TYPE_NORMAL, IMSA_CONN_SIP_PDP_TYPE_SIGNAL,IMSA_IMS_DOMAIN_LTE,enConnRlsType);
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_IDLE_DEREG);
            IMSA_StopTimer(&pstControlMagnaer->stGetSerProtectTimer);
            return IMSA_TRUE;

        case IMSA_SRV_STATUS_RELEASING_DEREG:


            IMSA_SRV_SndConnRelReq(IMSA_SRV_TYPE_NORMAL, IMSA_CONN_SIP_PDP_TYPE_SIGNAL,IMSA_IMS_DOMAIN_LTE,enConnRlsType);
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_IDLE_DEREG);
            IMSA_StopTimer(&pstControlMagnaer->stGetSerProtectTimer);
            break;

        default:
            break;
    }
    return IMSA_FALSE;
}



VOS_VOID IMSA_SRV_ProcGetSrvTimerExp
(
    const VOS_VOID                     *pMsg
)
{
    IMSA_CONTROL_MANAGER_STRU              *pstControlMagnaer   = VOS_NULL_PTR;
    IMSA_CONN_RELEASE_TYPE_ENUM_UINT8       enConnRlsType;

    IMSA_INFO_LOG("IMSA_SRV_ProcGetSrvTimerExp:enter!");

    pstControlMagnaer  = IMSA_GetControlManagerAddress();

    /* 设置PDN释放类型 */
    if( VOS_TRUE == pstControlMagnaer->ucOrgPdnRlsFlag)
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL;
    }
    else
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_LOCAL;
    }

    switch(IMSA_SRV_GetNormalSrvStatus())
    {
        case IMSA_SRV_STATUS_CONN_REGING:
        /* 获取IMS服务时长定时器超时，如果当前处于conn+dereging，应发起本地去注册*/
        case IMSA_SRV_STATUS_CONN_DEREGING:
            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_CONN_DEREG);

            IMSA_StopTimer(&pstControlMagnaer->stGetSerProtectTimer);

            /* 启动周期尝试定时器 */
            IMSA_StartTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer);

            break;

        case IMSA_SRV_STATUS_CONNING_DEREG:
            IMSA_SRV_SndConnRelReq(IMSA_SRV_TYPE_NORMAL, IMSA_CONN_SIP_PDP_TYPE_SIGNAL,IMSA_IMS_DOMAIN_LTE,enConnRlsType);
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_IDLE_DEREG);

            IMSA_StopTimer(&pstControlMagnaer->stGetSerProtectTimer);

            /* 启动周期尝试定时器 */
            IMSA_StartTimer(&pstControlMagnaer->stPeriodImsSrvTryTimer);

            break;

        default:
            break;
    }
    if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
    {
        IMSA_DsdsProcResourceRelease(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
    }
    return;

}


VOS_VOID IMSA_SRV_ProcDeregSrvTimerExp
(
    const VOS_VOID                     *pMsg
)
{
    IMSA_CONTROL_MANAGER_STRU              *pstControlMagnaer   = VOS_NULL_PTR;
    IMSA_CONN_RELEASE_TYPE_ENUM_UINT8       enConnRlsType;

    IMSA_INFO_LOG("IMSA_SRV_ProcDeregSrvTimerExp:enter!");

    pstControlMagnaer  = IMSA_GetControlManagerAddress();

    /* 设置PDN释放类型 */
    if( VOS_TRUE == pstControlMagnaer->ucOrgPdnRlsFlag)
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL;
    }
    else
    {
        enConnRlsType = IMSA_CONN_RELEASE_TYPE_ENUM_LOCAL;
    }

    switch(IMSA_SRV_GetNormalSrvStatus())
    {
        case IMSA_SRV_STATUS_CONN_DEREGING:

            IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);
            IMSA_SRV_SndConnRelReq(IMSA_SRV_TYPE_NORMAL, IMSA_CONN_SIP_PDP_TYPE_SIGNAL,IMSA_IMS_DOMAIN_LTE,enConnRlsType);
            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_IDLE_DEREG);

            IMSA_StopTimer(&pstControlMagnaer->stGetSerProtectTimer);

            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_DsdsProcResourceRelease(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
            }
            break;

        case IMSA_SRV_STATUS_RELEASING_DEREG:

            IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_IDLE_DEREG);

            IMSA_StopTimer(&pstControlMagnaer->stGetSerProtectTimer);

            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_DsdsProcResourceRelease(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
            }
            break;

        default:
            break;
    }
    return;

}

#endif


VOS_VOID IMSA_StartTempForbTimer(VOS_VOID)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer         = VOS_NULL_PTR;
    VOS_UINT32                          ucTempForbExpDelayTimeLen = 0;

    pstControlMagnaer  = IMSA_GetControlManagerAddress();

    /* 修改定时器时长 */
    /* NV定制增加临时forbidden定时器时长, 解决AT&T用例1844以及1846由于定时器超时发起PDN请求比对比机快0.5
       导致仪器认为我们提起发起PDN连接请求问题 */
    ucTempForbExpDelayTimeLen = pstControlMagnaer->ucTempForbExpDelayTimeLen * 1000;
    pstControlMagnaer->stTempForbiddenTimer.ulTimerLen = LMM_Get3402TimerLen() + ucTempForbExpDelayTimeLen;

    /* 启动定时器 */
    IMSA_StartTimer(&pstControlMagnaer->stTempForbiddenTimer);
}


VOS_UINT32 IMSA_IsTempForbTimerRunning(VOS_VOID)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt              = VOS_FALSE;

    pstControlMagnaer  = IMSA_GetControlManagerAddress();

    ulRslt = IMSA_IsTimerRunning(&pstControlMagnaer->stTempForbiddenTimer);

    return ulRslt;
}


VOS_VOID IMSA_ProcTimerMsgTempForbTimerExp(const VOS_VOID *pRcvMsg )
{
    IMSA_SRV_STATUS_ENUM_UINT8          enNrmSrvStatus;

    enNrmSrvStatus = IMSA_SRV_GetNormalSrvStatus();

    /* 该定时器超时时, 若当前处于IMSA_SRV_STATUS_IDLE_DEREG态, 则发起CONN建立流程 */
    if (IMSA_SRV_STATUS_IDLE_DEREG == enNrmSrvStatus)
    {
        /* 需要通知Domain模块Ready Ind用于Domain模块进行域选 */
        IMSA_SRV_SndDomainSrvReadyInd();
    }

    return;
}

VOS_VOID IMSA_SRV_ProcAreaLostInd
(
    VOS_VOID
)
{
    IMSA_CONTROL_MANAGER_STRU      *pstControlManager;

    IMSA_INFO_LOG("IMSA_SRV_ProcAreaLostInd is entered!");

    pstControlManager = IMSA_GetControlManagerAddress();

    /* 如果丢网后挂断电话的NV打开，则发送呼叫服务不可用消息，原因值为丢网 */
    if (IMSA_FALSE == pstControlManager->ucAreaLostNotRelCallFlag)
    {
        /* 通知CALL模块普通呼叫服务进入有服务状态 */
        /* 增加入参 */
        IMSA_SRV_SndNrmCallSrvStatusInd(IMSA_CALL_SERVICE_STATUS_NO_SERVICE,
                                        IMSA_CALL_NO_SRV_CAUSE_AREA_LOST,
                                        IMSA_CALL_NORM_SRV_EXTEND_INFO_NONE);

        /* 由于受限服务状态需要发起紧急呼，所以不能通知CALL模块进入无服务状态 */
    }
}
#if (FEATURE_ON == FEATURE_PTM)

VOS_UINT32 IMSA_SRV_GetTimeDiffByStampWithMillSecondUnit(const LPS_32K_TIMESTAMP *pstOldTStamp,const LPS_32K_TIMESTAMP *pstNewTStamp)
{
    VOS_UINT32   ulHighDiff = pstNewTStamp->ul32KTStampHighBit - pstOldTStamp->ul32KTStampHighBit;
    VOS_UINT32   ulowDiff;
    VOS_UINT32   ulHighInterval;
    VOS_UINT32   ulLowDiffRight15Bit;
    VOS_UINT32   ulLowDiffLeft17Bit;

    if(pstNewTStamp->ul32KTStampHighBit < pstOldTStamp->ul32KTStampHighBit)
    {
        LPS_OM_INFO_LOG2( "IMSA_SRV_GetTimeDiffByStampWithMillSecondUnit return 0 !oldhi=,newhi=",
                           pstOldTStamp->ul32KTStampHighBit,
                           pstNewTStamp->ul32KTStampHighBit);
        return 0;
    }

    if ((ulHighDiff == 0) && ( pstNewTStamp->ul32KTStampLowBit < pstOldTStamp->ul32KTStampLowBit))
    {
        LPS_OM_INFO_LOG2( "IMSA_SRV_GetTimeDiffByStampWithMillSecondUnit return 0 ! oldlo=,newlow=",
                           pstOldTStamp->ul32KTStampLowBit,pstNewTStamp->ul32KTStampLowBit);
        return 0;
    }

    /*高位差值达到最大，直接返回最大时间差，防止溢出*/
    if (ulHighDiff >= TLPS_32TSTAMP_MAX_HIGH_VAL_MILL_SECOND)
    {
        LPS_OM_INFO_LOG2( "IMSA_SRV_GetTimeDiffByStampWithMillSecondUnit hiout",
                           pstOldTStamp->ul32KTStampHighBit,
                           pstNewTStamp->ul32KTStampHighBit);
        return PS_NULL_UINT32;
    }

    if (pstNewTStamp->ul32KTStampLowBit < pstOldTStamp->ul32KTStampLowBit)
    {
       ulHighInterval = (ulHighDiff-1) << TLPS_32TSTAMP_HIGHBIT_MOVE_BIT_IN_SECOND;
       ulowDiff       = (VOS_UINT32)(0xFFFFFFFFUL  - pstOldTStamp->ul32KTStampLowBit + pstNewTStamp->ul32KTStampLowBit + 1);
    }
    else
    {
       ulHighInterval = ulHighDiff << TLPS_32TSTAMP_HIGHBIT_MOVE_BIT_IN_SECOND;
       ulowDiff       = (VOS_UINT32)(pstNewTStamp->ul32KTStampLowBit  - pstOldTStamp->ul32KTStampLowBit);
    }

    ulLowDiffRight15Bit = ((ulowDiff & 0x00007FFF) * 1000) >> TLPS_32TSTAMP_ACCURACY_MOVE_BIT_IN_SECOND ;
    ulLowDiffLeft17Bit  = (ulowDiff & 0xFFFF8000) >> TLPS_32TSTAMP_ACCURACY_MOVE_BIT_IN_SECOND ;

    return ulHighInterval*1000 + ulLowDiffLeft17Bit*1000 + ulLowDiffRight15Bit;
}


VOS_VOID IMSA_NotInitRegEventReport
(
    IMSA_ERR_LOG_NOT_INIT_IMS_REG_REASON_ENUM_UINT16    enNotInitRegReason
)
{
    IMSA_ERR_LOG_ALM_IMS_REG_FAIL_EVENT_REPORT_STRU stImsNotInitRegEvent;
    VOS_UINT32                          ulIsLogRecord;
    VOS_UINT32                          ulLength;
    VOS_UINT16                          usLevel;
    IMSA_CONTROL_MANAGER_STRU          *pstControlMagnaer   = IMSA_GetControlManagerAddress();

    TLPS_PRINT2LAYER_INFO(IMSA_NotInitRegEventReport_ENUM, LNAS_ENTRY);

    /* SERVICE 状态为IDLE+DEREG时，才进行上报 */
    if (IMSA_SRV_STATUS_IDLE_DEREG != IMSA_SRV_GetNormalSrvStatus())
    {
        TLPS_PRINT2LAYER_WARNING(IMSA_NotInitRegEventReport_ENUM, IMSA_STATE_ERROR);
        return;
    }

    /* 假TAU触发的SERVICE CHANG IND消息，不上报CHR */
    if(IMSA_OP_TRUE == pstControlMagnaer->bitRegSuccProcTypeValid)
    {
        /*这里的处理策略是:将假TAU流程的该消息直接丢弃，不再触发CHR上报*/
        if((IMSA_MSCC_TAU_UNREAL_PROC == IMSA_GetRegSuccProcType())
            ||(IMSA_MSCC_REG_PROC_BUTT == IMSA_GetRegSuccProcType()))
        {
               return;
        }
    }

    /* 查询对应Alarm Id是否需要记录异常信息 */
    usLevel             = IMSA_GetErrLogAlmLevel(IMSA_ERR_LOG_ALM_IMS_REG_FAIL_EVENT_REPORT);
    ulIsLogRecord       = IMSA_IsErrLogNeedRecord(usLevel);

    /* 模块异常不需要记录时，不保存异常信息 */
    if (VOS_FALSE == ulIsLogRecord)
    {
        return;
    }

    ulLength = sizeof(IMSA_ERR_LOG_ALM_IMS_REG_FAIL_EVENT_REPORT_STRU);

    IMSA_MEM_SET_S(&stImsNotInitRegEvent, ulLength, 0x00, ulLength);

    IMSA_WRITE_ERRLOG_HEADER_INFO(&stImsNotInitRegEvent.stHeader,
                                  VOS_GetModemIDFromPid(PS_PID_IMSA),
                                  IMSA_ERR_LOG_ALM_IMS_REG_FAIL_EVENT_REPORT,
                                  usLevel,
                                  VOS_GetSlice(),
                                  (ulLength - sizeof(OM_ERR_LOG_HEADER_STRU)));

    stImsNotInitRegEvent.enImsRegFailReason =  IMSA_OM_IMS_REG_FAIL_REASON_NOT_INIT_IMS_REG;
    stImsNotInitRegEvent.stNotInitRegReason.enNotInitRegReason = enNotInitRegReason;

    IMSA_SndAcpuOmFaultErrLogInd(   &stImsNotInitRegEvent,
                                    ulLength,
                                    IMSA_ERR_LOG_ALM_IMS_REG_FAIL_EVENT_REPORT);

    return;
}

VOS_VOID IMSA_PdnConnTimerExpireEventReport
(
    VOS_VOID
)
{
    IMSA_ERR_LOG_ALM_IMS_REG_TIMER_EXPIRE_EVENT_REPORT_STRU stImsTimerExpireEvent;
    VOS_UINT32                          ulIsLogRecord;
    VOS_UINT32                          ulLength;
    VOS_UINT16                          usLevel;
    IMSA_ERRORLOG_MANAGER_STRU         *pstImsaErrorlogManager   = VOS_NULL_PTR;
    VOS_UINT32                          ulMillSecondDiff;

    TLPS_PRINT2LAYER_INFO(IMSA_PdnConnTimerExpireEventReport_ENUM, LNAS_ENTRY);

    /* 查询对应Alarm Id是否需要记录异常信息 */
    usLevel                 = IMSA_GetErrLogAlmLevel(IMSA_ERR_LOG_ALM_IMS_REG_TIMER_EXPIRE_EVENT_REPORT);
    ulIsLogRecord           = IMSA_IsErrLogNeedRecord(usLevel);
    pstImsaErrorlogManager  = IMSA_GetErrorlogManagerAddress();

    /* 模块异常不需要记录时，不保存异常信息 */
    if (VOS_FALSE == ulIsLogRecord)
    {
        return;
    }

    /* 如果承载建立时长没有超过阈值，则直接退出 */
    ulMillSecondDiff = IMSA_SRV_GetTimeDiffByStampWithMillSecondUnit(&pstImsaErrorlogManager->stStartConnTStamp,
                                                                     &pstImsaErrorlogManager->stStopConnTStamp);

    TLPS_PRINT2LAYER_INFO1(IMSA_PdnConnTimerExpireEventReport_ENUM, IMSA_Conn_Meas_Time_Interval,
                            ulMillSecondDiff);

    if (pstImsaErrorlogManager->ulPdnConnTimeThreshold > ulMillSecondDiff)
    {
        return;
    }
    ulLength = sizeof(IMSA_ERR_LOG_ALM_IMS_REG_TIMER_EXPIRE_EVENT_REPORT_STRU);

    IMSA_MEM_SET_S(&stImsTimerExpireEvent, ulLength, 0x00, ulLength);

    IMSA_WRITE_ERRLOG_HEADER_INFO(&stImsTimerExpireEvent.stHeader,
                                  VOS_GetModemIDFromPid(PS_PID_IMSA),
                                  IMSA_ERR_LOG_ALM_IMS_REG_TIMER_EXPIRE_EVENT_REPORT,
                                  usLevel,
                                  VOS_GetSlice(),
                                  (ulLength - sizeof(OM_ERR_LOG_HEADER_STRU)));

    stImsTimerExpireEvent.enTimeExpireReason    = IMSA_OM_IMS_TIMER_EXPIRE_REASON_PDN_CONN;
    stImsTimerExpireEvent.ulPdnConnTimerLen     = ulMillSecondDiff;

    IMSA_SndAcpuOmFaultErrLogInd(   &stImsTimerExpireEvent,
                                    ulLength,
                                    IMSA_ERR_LOG_ALM_IMS_REG_TIMER_EXPIRE_EVENT_REPORT);

    return;
}

VOS_VOID IMSA_RegTimerExpireEventReport
(
    VOS_VOID
)
{
    IMSA_ERR_LOG_ALM_IMS_REG_TIMER_EXPIRE_EVENT_REPORT_STRU stImsTimerExpireEvent;
    VOS_UINT32                          ulIsLogRecord;
    VOS_UINT32                          ulLength;
    VOS_UINT16                          usLevel;
    IMSA_ERRORLOG_MANAGER_STRU         *pstImsaErrorlogManager   = VOS_NULL_PTR;
    VOS_UINT32                          ulMillSecondDiff;

    TLPS_PRINT2LAYER_INFO(IMSA_RegTimerExpireEventReport_ENUM, LNAS_ENTRY);

    /* 查询对应Alarm Id是否需要记录异常信息 */
    usLevel                 = IMSA_GetErrLogAlmLevel(IMSA_ERR_LOG_ALM_IMS_REG_TIMER_EXPIRE_EVENT_REPORT);
    ulIsLogRecord           = IMSA_IsErrLogNeedRecord(usLevel);
    pstImsaErrorlogManager  = IMSA_GetErrorlogManagerAddress();

    /* 模块异常不需要记录时，不保存异常信息 */
    if (VOS_FALSE == ulIsLogRecord)
    {
        return;
    }

    /* 如果承载建立时长没有超过阈值，则直接退出 */
    ulMillSecondDiff = IMSA_SRV_GetTimeDiffByStampWithMillSecondUnit(&pstImsaErrorlogManager->stStartRegTStamp,
                                                                     &pstImsaErrorlogManager->stStopRegTStamp);

    TLPS_PRINT2LAYER_INFO1(IMSA_RegTimerExpireEventReport_ENUM, IMSA_Reg_Meas_Time_Interval,
                            ulMillSecondDiff);

    if (pstImsaErrorlogManager->ulRegTimeThreshold > ulMillSecondDiff)
    {
        return;
    }
    ulLength = sizeof(IMSA_ERR_LOG_ALM_IMS_REG_TIMER_EXPIRE_EVENT_REPORT_STRU);

    IMSA_MEM_SET_S(&stImsTimerExpireEvent, ulLength, 0x00, ulLength);

    IMSA_WRITE_ERRLOG_HEADER_INFO(&stImsTimerExpireEvent.stHeader,
                                  VOS_GetModemIDFromPid(PS_PID_IMSA),
                                  IMSA_ERR_LOG_ALM_IMS_REG_TIMER_EXPIRE_EVENT_REPORT,
                                  usLevel,
                                  VOS_GetSlice(),
                                  (ulLength - sizeof(OM_ERR_LOG_HEADER_STRU)));

    stImsTimerExpireEvent.enTimeExpireReason    = IMSA_OM_IMS_TIMER_EXPIRE_REASON_REG;
    stImsTimerExpireEvent.ulImsRegTimerLen      = ulMillSecondDiff;

    IMSA_SndAcpuOmFaultErrLogInd(   &stImsTimerExpireEvent,
                                    ulLength,
                                    IMSA_ERR_LOG_ALM_IMS_REG_TIMER_EXPIRE_EVENT_REPORT);

    return;
}

VOS_VOID IMSA_SRV_RegSuccEventReport
(
    VOS_VOID
)
{
    IMSA_ERR_LOG_ALM_IMS_REG_SUCC_EVENT_REPORT_STRU stImsRegSuccEvent;
    VOS_UINT32                          ulIsLogRecord;
    VOS_UINT32                          ulLength;
    VOS_UINT16                          usLevel;
    IMSA_ERRORLOG_MANAGER_STRU         *pstImsaErrorlogManager   = VOS_NULL_PTR;
    VOS_UINT32                          ulConnMillSecondDiff;
    VOS_UINT32                          ulRegMillSecondDiff;

    TLPS_PRINT2LAYER_INFO(IMSA_SRV_RegSuccEventReport_ENUM, LNAS_ENTRY);

    /* 查询对应Alarm Id是否需要记录异常信息 */
    usLevel                 = IMSA_GetErrLogAlmLevel(IMSA_ERR_LOG_ALM_IMS_REG_SUCC_EVENT_REPORT);
    ulIsLogRecord           = IMSA_IsErrLogNeedRecord(usLevel);
    pstImsaErrorlogManager  = IMSA_GetErrorlogManagerAddress();

    /* 模块异常不需要记录时，不保存异常信息 */
    if (VOS_FALSE == ulIsLogRecord)
    {
        return;
    }

    /* 如果承载建立时长没有超过阈值，则直接退出 */
    ulConnMillSecondDiff = IMSA_SRV_GetTimeDiffByStampWithMillSecondUnit(&pstImsaErrorlogManager->stStartConnTStamp,
                                                                         &pstImsaErrorlogManager->stStopConnTStamp);

    ulRegMillSecondDiff = IMSA_SRV_GetTimeDiffByStampWithMillSecondUnit(&pstImsaErrorlogManager->stStartRegTStamp,
                                                                        &pstImsaErrorlogManager->stStopRegTStamp);

    ulLength = sizeof(IMSA_ERR_LOG_ALM_IMS_REG_SUCC_EVENT_REPORT_STRU);

    IMSA_MEM_SET_S(&stImsRegSuccEvent, ulLength, 0x00, ulLength);

    IMSA_WRITE_ERRLOG_HEADER_INFO(&stImsRegSuccEvent.stHeader,
                                  VOS_GetModemIDFromPid(PS_PID_IMSA),
                                  IMSA_ERR_LOG_ALM_IMS_REG_SUCC_EVENT_REPORT,
                                  usLevel,
                                  VOS_GetSlice(),
                                  (ulLength - sizeof(OM_ERR_LOG_HEADER_STRU)));

    stImsRegSuccEvent.ulPdnConnTimerLen = ulConnMillSecondDiff;
    stImsRegSuccEvent.ulImsRegTimerLen  = ulRegMillSecondDiff;

    IMSA_SndAcpuOmFaultErrLogInd(   &stImsRegSuccEvent,
                                    ulLength,
                                    IMSA_ERR_LOG_ALM_IMS_REG_SUCC_EVENT_REPORT);

    return;
}


VOS_VOID IMSA_SRV_InitDeregEventReport
(
    IMSA_ERR_LOG_INIT_DEREG_REASON_ENUM_UINT32  enInitDeregCause
)
{
    IMSA_ERR_LOG_ALM_IMS_INIT_DEREG_EVENT_REPORT_STRU stImsInitDeregEvent;
    VOS_UINT32                          ulIsLogRecord;
    VOS_UINT32                          ulLength;
    VOS_UINT16                          usLevel;

    /* 查询对应Alarm Id是否需要记录异常信息 */
    usLevel                 = IMSA_GetErrLogAlmLevel(IMSA_ERR_LOG_ALM_IMS_INIT_DEREG_EVENT_REPORT);
    ulIsLogRecord           = IMSA_IsErrLogNeedRecord(usLevel);

    TLPS_PRINT2LAYER_INFO(IMSA_SRV_InitDeregEventReport_ENUM, LNAS_ENTRY);

    /* 模块异常不需要记录时，不保存异常信息 */
    if (VOS_FALSE == ulIsLogRecord)
    {
        return;
    }

    /* 如果承载建立时长没有超过阈值，则直接退出 */
    ulLength = sizeof(IMSA_ERR_LOG_ALM_IMS_INIT_DEREG_EVENT_REPORT_STRU);

    IMSA_MEM_SET_S(&stImsInitDeregEvent, ulLength, 0x00, ulLength);

    IMSA_WRITE_ERRLOG_HEADER_INFO(&stImsInitDeregEvent.stHeader,
                                  VOS_GetModemIDFromPid(PS_PID_IMSA),
                                  IMSA_ERR_LOG_ALM_IMS_INIT_DEREG_EVENT_REPORT,
                                  usLevel,
                                  VOS_GetSlice(),
                                  (ulLength - sizeof(OM_ERR_LOG_HEADER_STRU)));

    stImsInitDeregEvent.enInitDeregCause = enInitDeregCause;

    IMSA_SndAcpuOmFaultErrLogInd(   &stImsInitDeregEvent,
                                    ulLength,
                                    IMSA_ERR_LOG_ALM_IMS_INIT_DEREG_EVENT_REPORT);

    return;
}

#endif


VOS_VOID IMSA_ProcTimerMsgWaitDeregTimerExp(const VOS_VOID *pRcvMsg )
{
    IMSA_SRV_STATUS_ENUM_UINT8          enNrmSrvStatus;

    enNrmSrvStatus = IMSA_SRV_GetNormalSrvStatus();

    /* 该定时器超时时, 若当前处于IMSA_SRV_STATUS_CONN_DEREGING态, 则发起本地去注册流程 */
    if (IMSA_SRV_STATUS_CONN_DEREGING == enNrmSrvStatus)
    {
        IMSA_SRV_SndRegDeregReq(IMSA_REG_TYPE_NORMAL, IMSA_DEREG_LOCAL);
        /* coverity[unchecked_value] */
        IMSA_SRV_SndConnRelReq(IMSA_SRV_TYPE_NORMAL,
                               IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                               IMSA_GetCurrentDomain(),
                               IMSA_CONN_RELEASE_TYPE_ENUM_NORMAL);
        IMSA_SRV_SetServcieStatus(IMSA_SRV_TYPE_NORMAL, IMSA_SRV_STATUS_RELEASING_DEREG);

        /* 释放资源 */
        #if (FEATURE_ON == FEATURE_DSDS)
        IMSA_DsdsProcResourceRelease(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
        #endif
    }
    /* 其他状态时不会出现该定时器超时的情况 */
    else
    {
    }
    IMSA_SndMsccMsgDeregCnf();

    return;
}


VOS_VOID IMSA_ProcTimerMsgWaitReadfileCnfTimerExp(const VOS_VOID *pRcvMsg )
{
    /* 等待定时器超时，认为读到了空文件，发起去注册 */
    IMSA_SRV_DeregProcReceivedUsimReadfileCnf();

    return;
}

/*lint +e961*/

#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
/* end of ImsaServiceManagement.c */



