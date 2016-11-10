

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include "ImsaDomainManagement.h"
#include "ImsaPublic.h"
#include "ImsaIntraInterface.h"
#include "ImsaServiceManagement.h"
#include "ImsaMsccInterface.h"
#include "ImsaIntraInterface.h"
#include "AtImsaInterface.h"
#include "ImsaWifiInterface.h"
#include "ImsaProcAtMsg.h"
#include "ImsaProcWifiMsg.h"
#include "ImsaProcMsccMsg.h"
#include "ImsaEntity.h"
#include "ImsaProcSpmMsg.h"
#include "ImsaImsAdaption.h"
#include "ImsaImsApi.h"
#include "ImsaProcMsccMsg.h"
#include "SpmImsaInterface.h"
#include "ImsaCallManagement.h"


/*lint -e767*/
#define    THIS_FILE_ID      PS_FILE_ID_IMSADOMAINMANAGEMENT_C
#define    THIS_NAS_FILE_ID  NAS_FILE_ID_IMSADOMAINMANAGEMENT_C
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

extern VOS_UINT32 IMSA_IsImsVoiceContidionSatisfied( VOS_VOID );
/*extern VOS_VOID IMSA_SndMsccMsgImsVoiceCapNotify
(
     MSCC_IMSA_IMS_VOICE_CAP_ENUM_UINT8   enImsVoiceCap,
     VOS_UINT32                           ucIsExistPersistentBearer
);
extern VOS_VOID IMSA_SndMsccMsgStartCnf(VOS_VOID  );*/
extern void IMSA_Domain_SaveDelaySwitchEvent( VOS_UINT32 ulPid, VOS_UINT32 ulMsgId);
extern void IMSA_Domain_StartTimer1LteDomain(VOS_VOID);
extern IMSA_IMS_DOMAIN_ENUM_UINT8 IMSA_JudgeImsDomainC21(VOS_VOID);

extern VOS_UINT32 IMSA_Domain_ProcReadIsimFileResult(VOS_VOID *pRcvMsg);

/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/

/*****************************************************************************
 Function Name  : IMSA_Domain_Init()
 Description    : Domain Manager初始化
 Input          :  VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-10-19  Draft Enact
*****************************************************************************/


VOS_VOID IMSA_Domain_Init(VOS_VOID)
{
    IMSA_DOMAIN_MANAGER_STRU              *pstDomainManager   = VOS_NULL_PTR;

    /* 初始化域管理变量 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();

    IMSA_MEM_SET_S(pstDomainManager, sizeof(IMSA_DOMAIN_MANAGER_STRU), 0x0, sizeof(IMSA_DOMAIN_MANAGER_STRU));
    pstDomainManager->enImsDomainPrefer            = IMSA_IMS_DOMAIN_PREFER_CELLULAR_ONLY;
    pstDomainManager->enImsaAvailDomain            = IMSA_IMS_DOMAIN_NULL;

    pstDomainManager->enImsaTryedDomain            = IMSA_IMS_DOMAIN_NULL;
    pstDomainManager->ucLteQualityCfgFlag          = VOS_FALSE;
    pstDomainManager->enLteQuality                 = WIFI_IMSA_QUALITY_LEVEL_BUTT;
    pstDomainManager->enWifiQuality                = WIFI_IMSA_QUALITY_LEVEL_BUTT;
    pstDomainManager->enDomainFsmL2State           = IMSA_DOMAIN_FSM_L2_STATE_NULL;
    pstDomainManager->enImsaVolteCtrlVowifiStatus  = IMSA_IMS_VOLTECTRLVOWIFI_INIT;

    /* 初始化单次域选过程涉及变量 */
    pstDomainManager->ulCurrentSenderPid         =  0;
    pstDomainManager->ulCurrentMsgId             =  0;
    pstDomainManager->ucMsgDelayProcessFlag      =  VOS_FALSE;
    pstDomainManager->ucHoFailReturn2OrgPdnFlag  =  VOS_FALSE;
    pstDomainManager->ucOrgPdnRlsFlag            =  VOS_TRUE;
    pstDomainManager->ucDomainSelectFlag         =  VOS_FALSE;

    /* 初始化延迟事件处理变量 */
    IMSA_Domain_ClearDelaySwitchEvent();

#if 0
    /* 初始化域选惩罚定时器的时长　*/
    pstDomainManager->ucInitTimer1Len              = 30;
    pstDomainManager->ucMaxTimer1Len               = 60;
#endif

    /* LTE域惩罚定时器初始化 */
    pstDomainManager->stTimer1LteDomain.phTimer    = VOS_NULL_PTR;
    pstDomainManager->stTimer1LteDomain.ucMode     = VOS_RELTIMER_NOLOOP;
    pstDomainManager->stTimer1LteDomain.usName     = TI_IMSA_DOMAIN_TIMER1_LTE;
    pstDomainManager->stTimer1LteDomain.ulTimerLen = 0;
    pstDomainManager->ucPunishTimeInLteDomain      = 1;

   /* WIFI域惩罚定时器初始化 */
    pstDomainManager->stTimer1WifiDomain.phTimer    = VOS_NULL_PTR;
    pstDomainManager->stTimer1WifiDomain.ucMode     = VOS_RELTIMER_NOLOOP;
    pstDomainManager->stTimer1WifiDomain.usName     = TI_IMSA_DOMAIN_TIMER1_WIFI;
    pstDomainManager->stTimer1WifiDomain.ulTimerLen = 0;
    pstDomainManager->ucPunishTimeInWifiDomain      = 1;

    return ;
}


/*****************************************************************************
 Function Name  : IMSA_Domain_ClearResource()
 Description    : Domain Manager初始化
 Input          :  VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-10-19  Draft Enact
*****************************************************************************/


VOS_VOID IMSA_Domain_ClearResource(VOS_VOID)
{
    IMSA_DOMAIN_MANAGER_STRU              *pstDomainManager   = VOS_NULL_PTR;

    /* 初始化紧急连接 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();

    /* 初始化域选状态相关变量 */
    pstDomainManager->enImsaAvailDomain            = IMSA_IMS_DOMAIN_NULL;
    pstDomainManager->enImsaTryedDomain            = IMSA_IMS_DOMAIN_NULL;

    pstDomainManager->ucLteQualityCfgFlag          = VOS_FALSE;
    pstDomainManager->enLteQuality                 = WIFI_IMSA_QUALITY_LEVEL_BUTT;
    pstDomainManager->enWifiQuality                = WIFI_IMSA_QUALITY_LEVEL_BUTT;
    pstDomainManager->enDomainFsmL2State           = IMSA_DOMAIN_FSM_L2_STATE_NULL;
    pstDomainManager->enImsaVolteCtrlVowifiStatus  = IMSA_IMS_VOLTECTRLVOWIFI_INIT;

    /* 初始化单次域选过程涉及变量 */
    pstDomainManager->ulCurrentSenderPid         =  0;
    pstDomainManager->ulCurrentMsgId             =  0;
    pstDomainManager->ucMsgDelayProcessFlag      =  VOS_FALSE;
    pstDomainManager->ucHoFailReturn2OrgPdnFlag  =  VOS_FALSE;
    pstDomainManager->ucOrgPdnRlsFlag            =  VOS_TRUE;
    pstDomainManager->ucDomainSelectFlag         =  VOS_FALSE;

     /* LTE域惩罚定时器初始化 */
     IMSA_StopTimer(&pstDomainManager->stTimer1LteDomain);
     pstDomainManager->stTimer1LteDomain.phTimer    = VOS_NULL_PTR;
     pstDomainManager->stTimer1LteDomain.ucMode     = VOS_RELTIMER_NOLOOP;
     pstDomainManager->stTimer1LteDomain.usName     = TI_IMSA_DOMAIN_TIMER1_LTE;
     pstDomainManager->stTimer1LteDomain.ulTimerLen = 0;
     pstDomainManager->ucPunishTimeInLteDomain      = 1;

    /* WIFI域惩罚定时器初始化 */
     IMSA_StopTimer(&pstDomainManager->stTimer1WifiDomain);
     pstDomainManager->stTimer1WifiDomain.phTimer    = VOS_NULL_PTR;
     pstDomainManager->stTimer1WifiDomain.ucMode     = VOS_RELTIMER_NOLOOP;
     pstDomainManager->stTimer1WifiDomain.usName     = TI_IMSA_DOMAIN_TIMER1_WIFI;
     pstDomainManager->stTimer1WifiDomain.ulTimerLen = 0;
     pstDomainManager->ucPunishTimeInWifiDomain      = 1;


    /* 初始化延迟事件处理变量 */
    IMSA_Domain_ClearDelaySwitchEvent();

    return;
}

/*****************************************************************************
 Function Name  : IMSA_Domain_ClearDomainVar()
 Description    : Domain Manager域选变量初始化
 Input          :  VOS_VOID
                 ucHoFailReturn2OrgPdnFlag: 切换失败以后是否可以回到原来PDN: FALSE 不能回到原域; TRUE 可以回到原域
                 ucOrgPdnRlsFlag: 域切换成功以后是否需要释放原域的PDN: FALSE 不需要释放； TRUE 释放
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-10-19  Draft Enact
*****************************************************************************/

void IMSA_Domain_InitDomainOpVar(
     VOS_UINT32 ulCurrentSenderPid,
     VOS_UINT32 ulCurrentMsgId,
     VOS_UINT8 ucHoFailReturn2OrgPdnFlag,
     VOS_UINT8 ucOrgPdnRlsFlag
)
{
    IMSA_DOMAIN_MANAGER_STRU              *pstDomainManager   = VOS_NULL_PTR;

    /* 初始化域管理变量 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();

    /* 初始化域选相关变量,处理当前域选过程 */
    pstDomainManager->ulCurrentSenderPid         =  ulCurrentSenderPid;
    pstDomainManager->ulCurrentMsgId             =  ulCurrentMsgId;
    pstDomainManager->ucMsgDelayProcessFlag      =  VOS_FALSE;
    pstDomainManager->ucHoFailReturn2OrgPdnFlag  =  ucHoFailReturn2OrgPdnFlag;
    pstDomainManager->ucOrgPdnRlsFlag            =  ucOrgPdnRlsFlag;
    pstDomainManager->ucDomainSelectFlag         =  VOS_FALSE;

    IMSA_Domain_ClearDelaySwitchEvent();

    return;
}

/*****************************************************************************
 Function Name  : IMSA_Domain_InitDomainOpVarByDelayEvent()
 Description    : 根据延迟事件设置域选函数变量
 Input          :  VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-10-19  Draft Enact
*****************************************************************************/

void IMSA_Domain_InitDomainOpVarByDelayEvent()
{

    IMSA_DOMAIN_MANAGER_STRU              *pstDomainManager   = VOS_NULL_PTR;

    /* 初始化域管理变量 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();

    /* 根据记录下来的延迟事件配置域选参数 */
    if(IMSA_OP_TRUE == pstDomainManager->bitDomainCfgEvent)
    {
        pstDomainManager->ulCurrentSenderPid         =  UEPS_PID_MSCC;
        pstDomainManager->ulCurrentMsgId             =  ID_MSCC_IMSA_IMS_DOMAIN_CFG_SET_REQ;
        pstDomainManager->ucHoFailReturn2OrgPdnFlag  =  VOS_TRUE;
        pstDomainManager->ucOrgPdnRlsFlag            =  VOS_TRUE;
    }
    else if(IMSA_OP_TRUE == pstDomainManager->bitLteQualityEvent)
    {
        pstDomainManager->ulCurrentSenderPid         =  PS_PID_ERRC ;
        pstDomainManager->ulCurrentMsgId             =  ID_LRRC_IMSA_STATE_IND;
        pstDomainManager->ucHoFailReturn2OrgPdnFlag  =  VOS_TRUE;
        pstDomainManager->ucOrgPdnRlsFlag            =  VOS_TRUE;
    }
    else if(IMSA_OP_TRUE == pstDomainManager->bitServiceChangeIndEvent)
    {
        pstDomainManager->ulCurrentSenderPid         =  UEPS_PID_MSCC;
        pstDomainManager->ulCurrentMsgId             =  ID_MSCC_IMSA_SERVICE_CHANGE_IND;
        pstDomainManager->ucHoFailReturn2OrgPdnFlag  =  VOS_TRUE;
        pstDomainManager->ucOrgPdnRlsFlag            =  VOS_TRUE;
    }
    else if(IMSA_OP_TRUE == pstDomainManager->bitWifiQualityEvent)
    {
        pstDomainManager->ulCurrentSenderPid         =  WUEPS_PID_MAPCON;
        pstDomainManager->ulCurrentMsgId             =  ID_WIFI_IMSA_STATE_IND  ;
        pstDomainManager->ucHoFailReturn2OrgPdnFlag  =  VOS_TRUE;
        pstDomainManager->ucOrgPdnRlsFlag            =  VOS_TRUE;
    }
    else if(IMSA_OP_TRUE == pstDomainManager->bitPeriodTimerEvent)
    {
        pstDomainManager->ulCurrentSenderPid         =  VOS_PID_TIMER ;
        pstDomainManager->ulCurrentMsgId             =  000  ; /* 后续修改 */
        pstDomainManager->ucHoFailReturn2OrgPdnFlag  =  VOS_TRUE;
        pstDomainManager->ucOrgPdnRlsFlag            =  VOS_TRUE;
    }
    else
    {
    }

    /* 清除记录下来的延迟事件 */
    IMSA_Domain_ClearDelaySwitchEvent();

    /* 配置其他参数 */
    pstDomainManager->ucMsgDelayProcessFlag      =  VOS_FALSE;
    pstDomainManager->ucDomainSelectFlag         =  VOS_FALSE;

    return;
}

/*****************************************************************************
 Function Name  : IMSA_Domain_ClearDelaySwitchEvent()
 Description    : 清除保存的延迟事件变量
 Input          :  VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-10-19  Draft Enact
*****************************************************************************/

void IMSA_Domain_ClearDelaySwitchEvent()
{
    IMSA_DOMAIN_MANAGER_STRU              *pstDomainManager   = VOS_NULL_PTR;

    /* 初始化域管理变量 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();

    /* 清除记录下来的延迟事件 */
    pstDomainManager->bitDomainCfgEvent          =  IMSA_OP_FALSE;
    pstDomainManager->bitLteQualityEvent         =  IMSA_OP_FALSE;
    pstDomainManager->bitPeriodTimerEvent        =  IMSA_OP_FALSE;
    pstDomainManager->bitServiceChangeIndEvent   =  IMSA_OP_FALSE;
    pstDomainManager->bitWifiQualityEvent        =  IMSA_OP_FALSE;
    pstDomainManager->bitRsvTrigDelayEvent       =  0;

    return;
}


/*****************************************************************************
 Function Name  : IMSA_Domain_CheckSignalPrefer()
 Description    : 根据IMS优先关系和信号强度判决选择域
 Input            : 当前可选的域
 Output         : VOS_VOID
 Return Value : 最终判决可用的域
 History        :
      1.Zhangdongfeng 353461        2015-12-10  Draft Enact
*****************************************************************************/
void  IMSA_Domain_CheckSignalPrefer(IMSA_IMS_DOMAIN_ENUM_UINT8 *penAvailDomain)
{

    IMSA_IMS_DOMAIN_ENUM_UINT8     enImsDomainResult = *penAvailDomain;
    IMSA_DOMAIN_MANAGER_STRU      *pstDomainManager   = VOS_NULL_PTR;

    /* 初始化域管理变量 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();

    IMSA_INFO_LOG("IMSA_Domain_CheckSignalPrefer is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_Domain_CheckSignalPrefer_ENUM, LNAS_ENTRY);

    /* 根据信号质量和域优先关系尝试选择域 */
    switch( pstDomainManager->enImsDomainPrefer )
    {
        case IMSA_IMS_DOMAIN_PREFER_CELLULAR_ONLY:
            /* 如果Cellular Only, LTE信号为NORMAL/GOOD：选择LTE域 */
            if( (pstDomainManager->enLteQuality > IMSA_WIRELESS_QUALITY_LEVEL_BAD)
               &&(  ( IMSA_IMS_DOMAIN_LTE_WIFI == enImsDomainResult)
                  ||( IMSA_IMS_DOMAIN_LTE == enImsDomainResult)))
            {
                enImsDomainResult = IMSA_IMS_DOMAIN_LTE;
            }
            else
            {
                enImsDomainResult = IMSA_IMS_DOMAIN_NULL;
            }
            break;

        case IMSA_IMS_DOMAIN_PREFER_WIFI_ONLY:
            /* 如果WIFI only，WIFI信号为NORMAL/GOOD：选择WIFI域 */
            if(   (IMSA_WIRELESS_QUALITY_LEVEL_BAD < pstDomainManager->enWifiQuality )
                &&( ( IMSA_IMS_DOMAIN_LTE_WIFI == enImsDomainResult)
                   ||( IMSA_IMS_DOMAIN_WIFI == enImsDomainResult)))
            {
                enImsDomainResult = IMSA_IMS_DOMAIN_WIFI;
            }
            else
            {
                enImsDomainResult = IMSA_IMS_DOMAIN_NULL;
            }
            break;

        case IMSA_IMS_DOMAIN_PREFER_CELLULAR_PREFER:
            /* 如果Cellular Prefer, LTE信号为NORMAL/GOOD：选择LTE域 */
            if( (pstDomainManager->enLteQuality > IMSA_WIRELESS_QUALITY_LEVEL_BAD)
               &&(  ( IMSA_IMS_DOMAIN_LTE_WIFI == enImsDomainResult)
                  ||( IMSA_IMS_DOMAIN_LTE == enImsDomainResult)))
            {
                enImsDomainResult = IMSA_IMS_DOMAIN_LTE;
            }

            /* 如果Cellular Prefer，LTE信号为BAD，WIFI信号为GOOD/NORMAL：选择WIFI域 */
            if(   (IMSA_WIRELESS_QUALITY_LEVEL_BAD == pstDomainManager->enLteQuality )
                &&(IMSA_WIRELESS_QUALITY_LEVEL_BAD < pstDomainManager->enWifiQuality )
                &&( ( IMSA_IMS_DOMAIN_LTE_WIFI == enImsDomainResult)
                    ||( IMSA_IMS_DOMAIN_WIFI == enImsDomainResult)))
            {
                enImsDomainResult = IMSA_IMS_DOMAIN_WIFI;
            }

            /* WIFI信号为BAD，LTE信号为BAD：维持非注册状态，暂不选择 */
            if( (IMSA_WIRELESS_QUALITY_LEVEL_BAD == pstDomainManager->enLteQuality )
                && (IMSA_WIRELESS_QUALITY_LEVEL_BAD == pstDomainManager->enWifiQuality) )
            {
                enImsDomainResult = IMSA_IMS_DOMAIN_NULL;
            }
#if 0

            if( (pstDomainManager->enLteQuality > IMSA_WIRELESS_QUALITY_LEVEL_NORMAL)
                  &&(  ( IMSA_IMS_DOMAIN_LTE_WIFI == enImsDomainResult)
                     ||( IMSA_IMS_DOMAIN_LTE == enImsDomainResult)))
            {
                enImsDomainResult = IMSA_IMS_DOMAIN_LTE;
                break;
            }
            if((IMSA_WIRELESS_QUALITY_LEVEL_NORMAL == pstDomainManager->enLteQuality)
                &&(IMSA_WIRELESS_QUALITY_LEVEL_NORMAL < pstDomainManager->enWifiQuality)
                &&( ( IMSA_IMS_DOMAIN_LTE_WIFI == enImsDomainResult)
                   ||( IMSA_IMS_DOMAIN_WIFI == enImsDomainResult)))
            {
                enImsDomainResult = IMSA_IMS_DOMAIN_WIFI;
                break;
            }
            if( (pstDomainManager->enLteQuality > IMSA_WIRELESS_QUALITY_LEVEL_BAD)
                  &&(  ( IMSA_IMS_DOMAIN_LTE_WIFI == enImsDomainResult)
                     ||( IMSA_IMS_DOMAIN_LTE == enImsDomainResult)))
            {
                enImsDomainResult = IMSA_IMS_DOMAIN_LTE;
                break;
            }


            /* 如果Cellular Prefer，LTE信号为BAD，WIFI信号为GOOD/NORMAL：选择WIFI域 */
            if(   (IMSA_WIRELESS_QUALITY_LEVEL_BAD == pstDomainManager->enLteQuality )
                 &&(IMSA_WIRELESS_QUALITY_LEVEL_BAD < pstDomainManager->enWifiQuality )
                 &&( ( IMSA_IMS_DOMAIN_LTE_WIFI == enImsDomainResult)
                    ||( IMSA_IMS_DOMAIN_WIFI == enImsDomainResult)))
            {
                enImsDomainResult = IMSA_IMS_DOMAIN_WIFI;
                break;
            }

            /* WIFI信号为BAD，LTE信号为BAD：维持非注册状态，暂不选择 */
            if( (IMSA_WIRELESS_QUALITY_LEVEL_BAD == pstDomainManager->enLteQuality )
            && (IMSA_WIRELESS_QUALITY_LEVEL_BAD == pstDomainManager->enWifiQuality) )
            {
                enImsDomainResult = IMSA_IMS_DOMAIN_NULL;
                break;
            }

#endif
            break;

        case IMSA_IMS_DOMAIN_PREFER_WIFI_PREFER:

           /* 如果WIFI Prefer，WIFI信号为BAD，LTE信号为GOOD/NORMAL：选择LTE域 */
            if(   (IMSA_WIRELESS_QUALITY_LEVEL_BAD == pstDomainManager->enWifiQuality )
                &&(IMSA_WIRELESS_QUALITY_LEVEL_BAD < pstDomainManager->enLteQuality )
                &&(  ( IMSA_IMS_DOMAIN_LTE_WIFI == enImsDomainResult)
                   ||( IMSA_IMS_DOMAIN_LTE == enImsDomainResult)))
            {
                enImsDomainResult = IMSA_IMS_DOMAIN_LTE;
            }

            /* 如果WIFI Prefer，WIFI信号为NORMAL/GOOD：选择WIFI域 */
            if(   (IMSA_WIRELESS_QUALITY_LEVEL_BAD < pstDomainManager->enWifiQuality )
                &&( ( IMSA_IMS_DOMAIN_LTE_WIFI == enImsDomainResult)
                   ||( IMSA_IMS_DOMAIN_WIFI == enImsDomainResult)))
            {
                enImsDomainResult = IMSA_IMS_DOMAIN_WIFI;
            }

            /* WIFI信号为BAD，LTE信号为BAD：维持非注册状态，暂不选择 */
            if((IMSA_WIRELESS_QUALITY_LEVEL_BAD == pstDomainManager->enLteQuality )
                &&(IMSA_WIRELESS_QUALITY_LEVEL_BAD == pstDomainManager->enWifiQuality))
            {
                enImsDomainResult = IMSA_IMS_DOMAIN_NULL;
            }
#if 0
            /* 如果WIFI Prefer，WIFI信号为BAD，LTE信号为GOOD/NORMAL：选择LTE域 */
            if(   (IMSA_WIRELESS_QUALITY_LEVEL_BAD == pstDomainManager->enWifiQuality )
                &&(IMSA_WIRELESS_QUALITY_LEVEL_BAD < pstDomainManager->enLteQuality )
                &&(  ( IMSA_IMS_DOMAIN_LTE_WIFI == enImsDomainResult)
                   ||( IMSA_IMS_DOMAIN_LTE == enImsDomainResult)))
            {
                enImsDomainResult = IMSA_IMS_DOMAIN_LTE;
                break;
            }


            /* 如果WIFI Prefer，WIFI信号为NORMAL,LTE为GOOD：选择LTE域 */
            if(   (IMSA_WIRELESS_QUALITY_LEVEL_NORMAL == pstDomainManager->enWifiQuality )
                &&(IMSA_WIRELESS_QUALITY_LEVEL_NORMAL < pstDomainManager->enLteQuality)
                &&( ( IMSA_IMS_DOMAIN_LTE_WIFI == enImsDomainResult)
                   ||( IMSA_IMS_DOMAIN_LTE == enImsDomainResult)))
            {
                enImsDomainResult = IMSA_IMS_DOMAIN_LTE;
                break;
            }

            /* 如果WIFI Prefer，WIFI信号为NORMAL/GOOD：选择WIFI域 */
            if(   (IMSA_WIRELESS_QUALITY_LEVEL_BAD < pstDomainManager->enWifiQuality )
                &&( ( IMSA_IMS_DOMAIN_LTE_WIFI == enImsDomainResult)
                   ||( IMSA_IMS_DOMAIN_WIFI == enImsDomainResult)))
            {
                enImsDomainResult = IMSA_IMS_DOMAIN_WIFI;
                break;
            }

            /* WIFI信号为BAD，LTE信号为BAD：维持非注册状态，暂不选择 */
            if((IMSA_WIRELESS_QUALITY_LEVEL_BAD == pstDomainManager->enLteQuality )
                &&(IMSA_WIRELESS_QUALITY_LEVEL_BAD == pstDomainManager->enWifiQuality))
            {
                enImsDomainResult = IMSA_IMS_DOMAIN_NULL;
            }
#endif
            break;

        default:
            enImsDomainResult = IMSA_IMS_DOMAIN_NULL;
            IMSA_WARN_LOG("IMSA_Domain_CheckSignalPrefer: Domain Prefer ENUM Wrong!");

            TLPS_PRINT2LAYER_WARNING(IMSA_Domain_CheckSignalPrefer_ENUM, LNAS_PARAM_INVALID);
    }

    *penAvailDomain = enImsDomainResult;
}




VOS_UINT32 IMSA_Domain_IsWifiDomainExist(void)
{
    IMSA_CONTROL_MANAGER_STRU    *pstControlManager;

    IMSA_INFO_LOG("IMSA_Domain_IsWifiDomainExist: ENTER!");

    pstControlManager = IMSA_GetControlManagerAddress();

    if( ((IMSA_STATUS_STARTED == pstControlManager->enImsaWifiStatus)
          ||(IMSA_STATUS_STARTING == pstControlManager->enImsaWifiStatus))
       &&((IMSA_STATUS_STARTED == pstControlManager->enImsaStatus)
          ||(IMSA_STATUS_STARTING == pstControlManager->enImsaStatus))
       && (VOS_TRUE == IMSA_GetWifiImsSupportFlag()))
    {

        return VOS_TRUE;
    }
    else
    {
        return VOS_FALSE;
    }
}




VOS_UINT32 IMSA_Domain_IsCellularDomainExist(void)
{
    IMSA_CONTROL_MANAGER_STRU    *pstControlManager;

    IMSA_INFO_LOG("IMSA_Domain_IsCellularDomainExist: ENTER!");

    pstControlManager = IMSA_GetControlManagerAddress();

    if( ((IMSA_STATUS_STARTED == pstControlManager->enImsaCellularStatus)
          ||(IMSA_STATUS_STARTING == pstControlManager->enImsaCellularStatus))
       &&((IMSA_STATUS_STARTED == pstControlManager->enImsaStatus)
          ||(IMSA_STATUS_STARTING == pstControlManager->enImsaStatus))
       && (VOS_TRUE == IMSA_GetCellularImsSupportFlag()))
    {

        return VOS_TRUE;
    }
    else
    {
        return VOS_FALSE;
    }
}







VOS_UINT32 IMSA_Domain_IsDualDomainExist(void)
{
    IMSA_CONTROL_MANAGER_STRU    *pstControlManager;

    IMSA_INFO_LOG("IMSA_Domain_IsDualDomainExist: ENTER!");

    pstControlManager = IMSA_GetControlManagerAddress();

    if( ((IMSA_STATUS_STARTED == pstControlManager->enImsaWifiStatus)
          ||(IMSA_STATUS_STARTING == pstControlManager->enImsaWifiStatus))
       &&((IMSA_STATUS_STARTED == pstControlManager->enImsaStatus)
          ||(IMSA_STATUS_STARTING == pstControlManager->enImsaStatus))
       && ((IMSA_STATUS_STARTED == pstControlManager->enImsaCellularStatus)
          ||(IMSA_STATUS_STARTING == pstControlManager->enImsaCellularStatus))
       && (VOS_TRUE == IMSA_GetCellularImsSupportFlag())
       && (VOS_TRUE == IMSA_GetWifiImsSupportFlag()))
    {

        return VOS_TRUE;
    }
    else
    {
        return VOS_FALSE;
    }
}


VOS_UINT32 IMSA_Domain_IsNrmSrvConditonSatisfied
(
    MSCC_IMSA_SERVICE_CHANGE_IND_STRU *pRcvMsg
)
{

    IMSA_CONFIG_PARA_STRU              *pstImsaConfigPara   = IMSA_GetConfigParaAddress();


    /* 当前接入技术支持IMS特性 */
    if (MSCC_IMSA_RAT_TYPE_GSM == pRcvMsg->enRat)
    {
        IMSA_INFO_LOG("IMSA_Domain_IsNrmSrvConditonSatisfied:GSM support ims!");
        TLPS_PRINT2LAYER_INFO(IMSA_Domain_IsNrmSrvConditonSatisfied_ENUM, 1);
        return IMSA_FALSE;
    }

    /* 当前接入技术支持IMS特性 */
    if ((MSCC_IMSA_RAT_TYPE_UTRAN == pRcvMsg->enRat)
        && (VOS_TRUE != pstImsaConfigPara->ucUtranImsSupportFlag))
    {
        IMSA_INFO_LOG("IMSA_Domain_IsNrmSrvConditonSatisfied:UNTRAN support ims!");
        TLPS_PRINT2LAYER_INFO(IMSA_Domain_IsNrmSrvConditonSatisfied_ENUM, 2);
        return IMSA_FALSE;
    }

    if ((MSCC_IMSA_RAT_TYPE_LTE == pRcvMsg->enRat)
        && (VOS_TRUE != pstImsaConfigPara->ucLteImsSupportFlag))
    {
        IMSA_INFO_LOG("IMSA_Domain_IsNrmSrvConditonSatisfied:LTE support ims!");
        TLPS_PRINT2LAYER_INFO(IMSA_Domain_IsNrmSrvConditonSatisfied_ENUM, 3);
        return IMSA_FALSE;
    }

    if (IMSA_VOICE_DOMAIN_CS_ONLY == IMSA_GetVoiceDomain())
    {
        IMSA_INFO_LOG("IMSA_Domain_IsNrmSrvConditonSatisfied:voice not support!");
        TLPS_PRINT2LAYER_INFO(IMSA_Domain_IsNrmSrvConditonSatisfied_ENUM, 4);

        return IMSA_FALSE;
    }

    if (VOS_FALSE == IMSA_GetUeImsVoiceCap())
    {
        IMSA_INFO_LOG("IMSA_Domain_IsNrmSrvConditonSatisfied:voice not support!");
        TLPS_PRINT2LAYER_INFO(IMSA_Domain_IsNrmSrvConditonSatisfied_ENUM, 8);

        return IMSA_FALSE;
    }

    if (IMSA_IMS_VOPS_STATUS_NOT_SUPPORT == pRcvMsg->enImsVoPsInd)
    {
        /* 如果网侧不支持IMS语音，但是UE支持IMS短信，且短信触发注册的开关
        打开时，需要继续判断其他条件是否满足 */
        if ((IMSA_FALSE == IMSA_GetUeSmsCapInitRegFlag())
            || (IMSA_FALSE == IMSA_GetUeImsSmsCap()))
        {
            IMSA_INFO_LOG("IMSA_SRV_IsNrmSrvConditonSatisfied:voice not support!");
            TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsNrmSrvConditonSatisfied_ENUM, 9);
            return IMSA_FALSE;
        }
    }
    #if 0
    /* IMS语音不支持，则返回条件不满足 */
    if ((IMSA_VOICE_DOMAIN_CS_ONLY == IMSA_GetVoiceDomain())
            || (VOS_FALSE == IMSA_GetUeImsVoiceCap())
            || (MSCC_IMSA_IMS_VOPS_NOT_SUPPORT == pRcvMsg->enImsVoPsInd))
    {
        IMSA_INFO_LOG("IMSA_Domain_IsNrmSrvConditonSatisfied:voice not support!");
        TLPS_PRINT2LAYER_INFO(IMSA_SRV_IsNrmSrvConditonSatisfied_ENUM, 2);
        return IMSA_FALSE;
    }
    #endif

    /* PS域服务状态为正常服务 */
    if (MSCC_IMSA_NORMAL_SERVICE != pRcvMsg->enPsServiceStatus)
    {
        IMSA_INFO_LOG("IMSA_Domain_IsNrmSrvConditonSatisfied:Ps service not normal!");
        TLPS_PRINT2LAYER_INFO(IMSA_Domain_IsNrmSrvConditonSatisfied_ENUM, 10);
        return IMSA_FALSE;
    }
    /*这里删除漫游网络下，UE设置为不支持漫游网络的IMS注册的判决*/


    return IMSA_TRUE;
}




void IMSA_Domain_SaveDelaySwitchEvent( VOS_UINT32 ulPid, VOS_UINT32 ulMsgId)
{
    IMSA_DOMAIN_MANAGER_STRU *pstDomainManager = VOS_NULL_PTR;

    pstDomainManager = IMSA_Domain_GetDomainManagerAddr();

    if( ( WUEPS_PID_MAPCON == ulPid )
        && ( ID_WIFI_IMSA_STATE_IND == ulMsgId))
    {
        pstDomainManager->bitWifiQualityEvent = IMSA_OP_TRUE;
    }
    else if( (UEPS_PID_MSCC == ulPid )
        && ( ID_MSCC_IMSA_IMS_DOMAIN_CFG_SET_REQ == ulMsgId))
    {
        pstDomainManager->bitDomainCfgEvent = IMSA_OP_TRUE;
    }
    else if( (UEPS_PID_MSCC == ulPid )
        && ( ID_MSCC_IMSA_SERVICE_CHANGE_IND == ulMsgId))
    {
        pstDomainManager->bitServiceChangeIndEvent = IMSA_OP_TRUE;
    }
    else if( (PS_PID_ERRC == ulPid )
        && ( ID_LRRC_IMSA_STATE_IND == ulMsgId))
    {
        pstDomainManager->bitLteQualityEvent = IMSA_OP_TRUE;
    }
    else if( VOS_PID_TIMER == ulPid )
    {
        pstDomainManager->bitPeriodTimerEvent = IMSA_OP_TRUE;
    }

    return;
}

/*****************************************************************************
 Function Name  : IMSA_Domain_StartTimer1WifiDomain()
 Description    : 启动WIFI域的惩罚定时器
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-11-19  Draft Enact
*****************************************************************************/

VOS_VOID IMSA_Domain_StartTimer1WifiDomain(VOS_VOID)
{
    IMSA_DOMAIN_MANAGER_STRU           *pstDomainManager    = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_Domain_StartTimer1WifiDomain: ENTER!");
    pstDomainManager = IMSA_Domain_GetDomainManagerAddr();

    pstDomainManager->stTimer1WifiDomain.ulTimerLen = (VOS_UINT32)(pstDomainManager->ucInitTimer1Len * pstDomainManager->ucPunishTimeInWifiDomain * 1000);
    if( pstDomainManager->stTimer1WifiDomain.ulTimerLen > (VOS_UINT32)(pstDomainManager->ucMaxTimer1Len * 60 * 1000))
    {
        pstDomainManager->stTimer1WifiDomain.ulTimerLen = (VOS_UINT32)(pstDomainManager->ucMaxTimer1Len * 60 * 1000);
    }

    /* 启动定时器 */
    IMSA_StartTimer(&pstDomainManager->stTimer1WifiDomain);

    return;
}

/*****************************************************************************
 Function Name  : IMSA_Domain_StopTimer1WifiDomain()
 Description    : 停止WIFI域的惩罚定时器
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-11-19  Draft Enact
*****************************************************************************/

void IMSA_Domain_StopTimer1WifiDomain()
{
    IMSA_DOMAIN_MANAGER_STRU           *pstDomainManager    = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_Domain_StopTimer1WifiDomain: ENTER!");

    TLPS_PRINT2LAYER_INFO(IMSA_Domain_StopTimer1WifiDomain_ENUM, LNAS_ENTRY);

    pstDomainManager = IMSA_Domain_GetDomainManagerAddr();

    pstDomainManager->ucPunishTimeInWifiDomain = 1;

    /* 停止定时器 */
    IMSA_StopTimer(&pstDomainManager->stTimer1WifiDomain);

    return;
}

/*****************************************************************************
 Function Name  : IMSA_Domain_StartTimer1LteDomain()
 Description    : 启动LTE域的惩罚定时器
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-11-19  Draft Enact
*****************************************************************************/

void IMSA_Domain_StartTimer1LteDomain(VOS_VOID)
{
    IMSA_DOMAIN_MANAGER_STRU           *pstDomainManager    = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_Domain_StartTimer1LteDomain: ENTER!");

    TLPS_PRINT2LAYER_INFO(IMSA_Domain_StartTimer1LteDomain_ENUM, LNAS_ENTRY);

    pstDomainManager = IMSA_Domain_GetDomainManagerAddr();

    pstDomainManager->stTimer1LteDomain.ulTimerLen = (VOS_UINT32)( pstDomainManager->ucInitTimer1Len * pstDomainManager->ucPunishTimeInLteDomain * 1000 );
    if( pstDomainManager->stTimer1LteDomain.ulTimerLen >(VOS_UINT32) (pstDomainManager->ucMaxTimer1Len * 60* 1000))
    {
        pstDomainManager->stTimer1LteDomain.ulTimerLen =(VOS_UINT32)( pstDomainManager->ucMaxTimer1Len * 60* 1000);
    }

    /* 启动定时器 */
    IMSA_StartTimer(&pstDomainManager->stTimer1LteDomain);

    return;
}

/*****************************************************************************
 Function Name  : IMSA_Domain_StopTimer1LteDomain()
 Description    : 停止LTE域的惩罚定时器
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-11-19  Draft Enact
*****************************************************************************/

void IMSA_Domain_StopTimer1LteDomain(VOS_VOID)
{
    IMSA_DOMAIN_MANAGER_STRU           *pstDomainManager    = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_Domain_StopTimer1LteDomain: ENTER!");

    TLPS_PRINT2LAYER_INFO(IMSA_Domain_StopTimer1LteDomain_ENUM, LNAS_ENTRY);

    pstDomainManager = IMSA_Domain_GetDomainManagerAddr();

    pstDomainManager->ucPunishTimeInLteDomain = 1;

    /* 停止定时器 */
    IMSA_StopTimer(&pstDomainManager->stTimer1LteDomain);

    return;
}

/*****************************************************************************
 Function Name  : IMSA_ImsDomainC1Judge()
 Description    : 判决是否存在多个侯选域
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   :
      返回可以使用的侯选域:

       ID_IMSA_IMS_DOMAIN_NULL: 没有可用域
       IMSA_IMS_DOMAIN_MODE_WIFI_ONLY:

 History        :
      1.Zhangdongfeng 353461        2015-10-19  Draft Enact
*****************************************************************************/
IMSA_IMS_DOMAIN_ENUM_UINT8 IMSA_JudgeImsDomainC1(VOS_VOID)
{

    IMSA_DOMAIN_MANAGER_STRU *pstDomainManager = VOS_NULL_PTR;

    pstDomainManager = IMSA_Domain_GetDomainManagerAddr();
    IMSA_INFO_LOG("IMSA_JudgeImsDomainC1: enter");
    TLPS_PRINT2LAYER_INFO(IMSA_JudgeImsDomainC1_ENUM, LNAS_ENTRY);

    switch(IMSA_GetDomainMode())
    {
    case IMSA_IMS_DOMAIN_MODE_NULL:
        return IMSA_IMS_DOMAIN_NULL;

    case IMSA_IMS_DOMAIN_MODE_WIFI_ONLY:
        /* 满足WIFI驻留条件,则返回WIFI */
        if( ((IMSA_IMS_DOMAIN_WIFI == pstDomainManager->enImsaAvailDomain)
              ||(IMSA_IMS_DOMAIN_LTE_WIFI == pstDomainManager->enImsaAvailDomain))
            &&(IMSA_TRUE == IMSA_SRV_IsNrmSrvIsimParaSatisfied())
            )
        {
            IMSA_INFO_LOG("IMSA_JudgeImsDomainC1: 1_DomainMode is IMSA_IMS_DOMAIN_MODE_WIFI_ONLY!");
            TLPS_PRINT2LAYER_INFO(IMSA_JudgeImsDomainC1_ENUM, LNAS_FUNCTION_LABEL1);
            return IMSA_IMS_DOMAIN_WIFI;
        }

        /* 当前L2处于WIFI状态，则仍旧返回WIFI状态,即使当前不满足驻留条件,仍旧在WIFI下,避免频繁不断选择域  */
        if( (IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING == pstDomainManager->enDomainFsmL2State)
            || (IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN == pstDomainManager->enDomainFsmL2State))
        {
            IMSA_INFO_LOG("IMSA_JudgeImsDomainC1: 2_DomainMode is IMSA_IMS_DOMAIN_MODE_WIFI_ONLY!");
            TLPS_PRINT2LAYER_INFO(IMSA_JudgeImsDomainC1_ENUM, LNAS_FUNCTION_LABEL2);
            return IMSA_IMS_DOMAIN_WIFI;
        }

        break;

    case IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY:
        /*满足LTE驻留条件, 则返回LTE */
        if(((IMSA_IMS_DOMAIN_LTE == pstDomainManager->enImsaAvailDomain)||
            (IMSA_IMS_DOMAIN_LTE_WIFI == pstDomainManager->enImsaAvailDomain))&&
            (IMSA_TRUE == IMSA_SRV_IsNrmSrvIsimParaSatisfied())
            )
        {
            IMSA_INFO_LOG("IMSA_JudgeImsDomainC1: 3_DomainMode is IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY!");
            TLPS_PRINT2LAYER_INFO(IMSA_JudgeImsDomainC1_ENUM, LNAS_FUNCTION_LABEL3);
            return IMSA_IMS_DOMAIN_LTE;
        }

        /* 当前L2处于LTE状态，则仍旧返回LTE状态,即使当前不满足驻留条件,仍旧在LTE下,避免频繁不断选择域 */
        if( (IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING == pstDomainManager->enDomainFsmL2State)
            || (IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN == pstDomainManager->enDomainFsmL2State))
        {
            IMSA_INFO_LOG("IMSA_JudgeImsDomainC1: 4_DomainMode is IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY!");
            TLPS_PRINT2LAYER_INFO(IMSA_JudgeImsDomainC1_ENUM, LNAS_FUNCTION_LABEL4);
            return IMSA_IMS_DOMAIN_LTE;
        }
        break;

    case IMSA_IMS_DOMAIN_MODE_DUAL:
        /* LTE&WIFI都满足驻留条件,则返回双域 */
        if((IMSA_IMS_DOMAIN_LTE_WIFI == pstDomainManager->enImsaAvailDomain)&&
            (IMSA_TRUE == IMSA_SRV_IsNrmSrvIsimParaSatisfied())
            )
        {
            IMSA_INFO_LOG("IMSA_JudgeImsDomainC1: 5_DomainMode is IMSA_IMS_DOMAIN_MODE_DUAL!");
            TLPS_PRINT2LAYER_INFO(IMSA_JudgeImsDomainC1_ENUM, LNAS_FUNCTION_LABEL5);
            return IMSA_IMS_DOMAIN_LTE_WIFI;
        }
        /* 满足LTE驻留条件, 则返回LTE */
        else if((IMSA_IMS_DOMAIN_LTE == pstDomainManager->enImsaAvailDomain)
            &&(IMSA_TRUE == IMSA_SRV_IsNrmSrvIsimParaSatisfied()))
        {
            IMSA_INFO_LOG("IMSA_JudgeImsDomainC1: 6_DomainMode is IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY!");
            TLPS_PRINT2LAYER_INFO(IMSA_JudgeImsDomainC1_ENUM, LNAS_FUNCTION_LABEL6);
            return IMSA_IMS_DOMAIN_LTE;
        }
        /* 满足WIFI驻留条件,则返回WIFI */
        else if((IMSA_IMS_DOMAIN_WIFI == pstDomainManager->enImsaAvailDomain)
            &&(IMSA_TRUE == IMSA_SRV_IsNrmSrvIsimParaSatisfied()))
        {
            IMSA_INFO_LOG("IMSA_JudgeImsDomainC1: 7_DomainMode is IMSA_IMS_DOMAIN_MODE_WIFI_ONLY!");
            TLPS_PRINT2LAYER_INFO(IMSA_JudgeImsDomainC1_ENUM, LNAS_FUNCTION_LABEL7);
            return IMSA_IMS_DOMAIN_WIFI;
        }
        /* 当前L2处于LTE状态，则仍旧返回LTE状态,即使当前不满足驻留条件,仍旧在LTE下,避免频繁不断选择域 */
        else if( (IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING == pstDomainManager->enDomainFsmL2State)
            || (IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN == pstDomainManager->enDomainFsmL2State))
        {
            IMSA_INFO_LOG("IMSA_JudgeImsDomainC1: 8_DomainMode is IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY!");

            TLPS_PRINT2LAYER_INFO(IMSA_JudgeImsDomainC1_ENUM, LNAS_FUNCTION_LABEL8);
            return IMSA_IMS_DOMAIN_LTE;
        }
        /* 当前L2处于WIFI状态，则仍旧返回WIFI状态,即使当前不满足驻留条件,仍旧在WIFI下,避免频繁不断选择域  */
        else if( (IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING == pstDomainManager->enDomainFsmL2State)
            || (IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN == pstDomainManager->enDomainFsmL2State))
        {
            IMSA_INFO_LOG("IMSA_JudgeImsDomainC1: 9_DomainMode is IMSA_IMS_DOMAIN_MODE_WIFI_ONLY!");

            TLPS_PRINT2LAYER_INFO(IMSA_JudgeImsDomainC1_ENUM, LNAS_FUNCTION_LABEL9);
            return IMSA_IMS_DOMAIN_WIFI;
        }
        /*lint -e527 */
        else
        {
            IMSA_ERR_LOG("IMSA_JudgeImsDomainC1: No Meet CampOn Condition!");

            TLPS_PRINT2LAYER_ERROR(IMSA_JudgeImsDomainC1_ENUM, LNAS_FUNCTION_LABEL10);
            return IMSA_IMS_DOMAIN_NULL;
        }
        break;
        /*lint +e527 */

    default:
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_JudgeImsDomainC1:ERROR:Wrong Domain State!");

        TLPS_PRINT2LAYER_ERROR(IMSA_JudgeImsDomainC1_ENUM, LNAS_StateInvalid);
        return IMSA_IMS_DOMAIN_NULL;
    }

    return IMSA_IMS_DOMAIN_NULL;
}


/*****************************************************************************
 Function Name  : IMSA_ImsDomainC21Judge()
 Description    : 初始IMS域选择判决算法 C2_1
 Input            :
 Output         : VOS_VOID
 Return Value : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-07  Draft Enact
*****************************************************************************/

IMSA_IMS_DOMAIN_ENUM_UINT8 IMSA_JudgeImsDomainC21(VOS_VOID)
{

    IMSA_IMS_DOMAIN_ENUM_UINT8     enImsDomainResult = IMSA_IMS_DOMAIN_LTE_WIFI;
    IMSA_DOMAIN_MANAGER_STRU      *pstDomainManager   = VOS_NULL_PTR;

    /* 初始化域管理变量 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();

    TLPS_PRINT2LAYER_INFO(IMSA_JudgeImsDomainC21_ENUM, LNAS_ENTRY);
    /* 初始IMS选择过程,包括如下的策略:
        1: 域惩罚定时器是否启动,如果启动,则暂时不选择这个域;
        2: 根据信号质量和域优先关系尝试选择域
        3: 如果选择的域已经尝试过,并且另外一个域没有被惩罚,则选择新域尝试
            如果两个域都已经尝试过,清除尝试变量.
        */

    /* 检查WIFI/LTE 惩罚定时器是否启动,如果启动,则不选择这个域 */
    if( VOS_TRUE == (IMSA_IsTimerRunning(&pstDomainManager->stTimer1WifiDomain)))
    {
        IMSA_Domain_SetWifiDisable(enImsDomainResult);
    }
    if( VOS_TRUE == (IMSA_IsTimerRunning(&pstDomainManager->stTimer1LteDomain)))
    {
        IMSA_Domain_SetLteDisable(enImsDomainResult);
    }


    /*    2: 根据信号质量和域优先关系尝试选择域 */
    IMSA_Domain_CheckSignalPrefer(&enImsDomainResult);


    /* 两个域都已经尝试过,则初始化尝试变量 */
    if( IMSA_IMS_DOMAIN_LTE_WIFI == pstDomainManager->enImsaTryedDomain )
    {
        pstDomainManager->enImsaTryedDomain = IMSA_IMS_DOMAIN_NULL;
    }

    /* 如果选择的域已经尝试过,且另外一个域满足尝试条件,则选择另外一个域 */
    if( pstDomainManager->enImsaTryedDomain == enImsDomainResult)
    {
        if( ( IMSA_IMS_DOMAIN_WIFI == enImsDomainResult)
            &&( VOS_FALSE == IMSA_IsTimerRunning(&pstDomainManager->stTimer1LteDomain))
            &&( IMSA_WIRELESS_QUALITY_LEVEL_BAD < pstDomainManager->enLteQuality ))
        {
            enImsDomainResult = IMSA_IMS_DOMAIN_LTE;
        }
        else if(( IMSA_IMS_DOMAIN_LTE == enImsDomainResult)
            &&( VOS_FALSE == IMSA_IsTimerRunning(&pstDomainManager->stTimer1WifiDomain))
            &&( IMSA_WIRELESS_QUALITY_LEVEL_BAD < pstDomainManager->enWifiQuality ))
        {
            enImsDomainResult = IMSA_IMS_DOMAIN_WIFI;
        }
    }
    /* else: 如果选择的域没有尝试过,则直接尝试这个域,不需要处理 */

    return enImsDomainResult;
}

/*****************************************************************************
 Function Name  : IMSA_ImsDomainC221W2LJudge()
 Description    : 切换判决算法 C2_2_1(W2L)
 Input            :
 Output         : VOS_VOID
 Return Value : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-07  Draft Enact
*****************************************************************************/

IMSA_IMS_HO_TYPE_ENUM_UINT8 IMSA_JudgeImsDomainC221W2L()
{
    IMSA_IMS_DOMAIN_ENUM_UINT8     enImsDomainResult = IMSA_IMS_DOMAIN_LTE_WIFI;
    IMSA_IMS_HO_TYPE_ENUM_UINT8   enDomainHoType;
    IMSA_DOMAIN_MANAGER_STRU      *pstDomainManager   = VOS_NULL_PTR;

    /* 初始化域管理变量 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();

    IMSA_WARN_LOG("IMSA_JudgeImsDomainC221W2L: Enter!");
    TLPS_PRINT2LAYER_INFO(IMSA_JudgeImsDomainC221W2L_ENUM, LNAS_ENTRY);

    /* LTE域惩罚定时器是否启动,如果启动,则暂时不选择这个域 */
    /*lint -e438 */
    if( VOS_TRUE == (IMSA_IsTimerRunning(&pstDomainManager->stTimer1LteDomain)))
    {
        IMSA_WARN_LOG("IMSA_JudgeImsDomainC221W2L: 1: timer start!");
        enImsDomainResult = IMSA_IMS_DOMAIN_WIFI;
        enDomainHoType = IMSA_IMS_HO_TYPE_NO_HO;
        return enDomainHoType;
    }
    /*lint +e438 */

    /*  根据信号质量和域优先关系尝试选择域 */
    IMSA_Domain_CheckSignalPrefer(&enImsDomainResult);

    if( (IMSA_IMS_DOMAIN_WIFI == enImsDomainResult)
        || (IMSA_IMS_DOMAIN_NULL == enImsDomainResult))
    {
        IMSA_WARN_LOG("IMSA_JudgeImsDomainC221W2L: 2: signal weak!");
        enDomainHoType = IMSA_IMS_HO_TYPE_NO_HO;
        return enDomainHoType;
    }

    /* 是否支持业务态VoWIFI->VoLTE NV切换选项: 电话业务,消息业务,USSD业务存在 */
    /*lint -e438 */
    if( (VOS_FALSE == IMSA_GetW2LHoInActSrvFlag() )
        &&( VOS_TRUE == IMSA_IsImsExist()))
    {
        IMSA_WARN_LOG("IMSA_JudgeImsDomainC221W2L: 3: no support HO in active service!");

        enImsDomainResult = IMSA_IMS_DOMAIN_WIFI;

        enDomainHoType = IMSA_IMS_HO_TYPE_NO_HO;
        return enDomainHoType;
    }
    /*lint +e438 */

    /* 是否支持非业务态的切换:无电话,消息和USSD操作 */
    /*lint -e438 */
    if( (VOS_FALSE == IMSA_GetHoSupportInNonActSrvFlag() )
         &&(VOS_FALSE == IMSA_IsImsExist()))
    {
        IMSA_WARN_LOG("IMSA_JudgeImsDomainC221W2L: 4: no support HO in no-active service!");
        enImsDomainResult = IMSA_IMS_DOMAIN_WIFI;
        enDomainHoType = IMSA_IMS_HO_TYPE_NO_HO;
        return enDomainHoType;
    }
    /*lint +e438 */
    /*非业务稳态是否支持切换选项能力判断:正在电话建立过程中,消息传输中,USSD操作中 */
    /*lint -e438 */
    if( (VOS_FALSE == IMSA_GetHoSupportInUnstableSrvFlag() )
        &&( VOS_TRUE == IMSA_IsImsTransactionExist()))
    {
        IMSA_WARN_LOG("IMSA_JudgeImsDomainC221W2L: 5: no support HO in processing service!");
        enImsDomainResult = IMSA_IMS_DOMAIN_WIFI;
        enDomainHoType = IMSA_IMS_HO_TYPE_NO_HO;
        return enDomainHoType;
    }
    /*lint +e438 */

    /* 由于涉及处理和函数较多，暂时不实现业务结束以后请求发起切换过程 */

    /* 以上条件都满足,则启动W2L的切换 */
    /*lint -e438 */
    enImsDomainResult = IMSA_IMS_DOMAIN_LTE;
    enDomainHoType = IMSA_IMS_HO_TYPE_WIFI2LTE;
    return enDomainHoType;
    /*lint +e438 */

}

/*****************************************************************************
 Function Name  : IMSA_ImsDomainC222L2WJudge()
 Description    : 切换判决算法 C2_2_2(L2W)
 Input            :
 Output         : VOS_VOID
 Return Value : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-07  Draft Enact
*****************************************************************************/

IMSA_IMS_HO_TYPE_ENUM_UINT8 IMSA_JudgeImsDomainC222L2W()
{
    IMSA_IMS_DOMAIN_ENUM_UINT8     enImsDomainResult = IMSA_IMS_DOMAIN_LTE_WIFI;
    IMSA_IMS_HO_TYPE_ENUM_UINT8   enDomainHoType;
    IMSA_DOMAIN_MANAGER_STRU      *pstDomainManager   = VOS_NULL_PTR;

    /* 初始化域管理变量 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_JudgeImsDomainC222L2W_ENUM, LNAS_ENTRY);

    /* WIFI域惩罚定时器是否启动,如果启动,则暂时不选择这个域 */
    /*lint -e438*/
    if( VOS_TRUE == (IMSA_IsTimerRunning(&pstDomainManager->stTimer1WifiDomain)))
    {
        enImsDomainResult = IMSA_IMS_DOMAIN_LTE;
        enDomainHoType = IMSA_IMS_HO_TYPE_NO_HO;
        return enDomainHoType;
    }
    /*lint +e438 */
    /*  根据信号质量和域优先关系尝试选择域 */
    IMSA_Domain_CheckSignalPrefer(&enImsDomainResult);
    /*lint -e438 */
    if( (IMSA_IMS_DOMAIN_LTE == enImsDomainResult)
        || (IMSA_IMS_DOMAIN_NULL == enImsDomainResult))
    {
        enImsDomainResult = IMSA_IMS_DOMAIN_LTE;
        enDomainHoType = IMSA_IMS_HO_TYPE_NO_HO;
        return enDomainHoType;
    }
    /*lint +e438 */
    /* 是否支持业务态VoWIFI->VoLTE NV切换选项: 电话业务,消息业务,USSD业务存在 */
    /*lint -e438 */
    if( (VOS_FALSE == IMSA_GetL2WHoInActSrvFlag() )
        && (VOS_TRUE == IMSA_IsImsExist()))
    {
        enImsDomainResult = IMSA_IMS_DOMAIN_LTE;
        enDomainHoType = IMSA_IMS_HO_TYPE_NO_HO;
        return enDomainHoType;
    }
    /*lint +e438 */

    /* 是否支持非业务态的切换: 电话,消息和USSD操作过程中 */
      /*lint -e438 */
    if( (VOS_FALSE == IMSA_GetHoSupportInNonActSrvFlag() )
        && (VOS_FALSE == IMSA_IsImsExist()))
    {
        enImsDomainResult = IMSA_IMS_DOMAIN_LTE;
        enDomainHoType = IMSA_IMS_HO_TYPE_NO_HO;
        return enDomainHoType;
    }
    /*lint +e438 */

    /*非业务稳态是否支持切换选项能力判断:正在电话建立过程中,消息传输中,USSD操作中 */
    /*lint -e438 */
    if( (VOS_FALSE == IMSA_GetHoSupportInUnstableSrvFlag() )
        && (VOS_TRUE == IMSA_IsImsTransactionExist()))
    {
        enImsDomainResult = IMSA_IMS_DOMAIN_LTE;

        enDomainHoType = IMSA_IMS_HO_TYPE_NO_HO;
        return enDomainHoType;
    }
    /*lint +e438 */
    /* 由于涉及处理和函数较多，暂时不实现业务结束以后请求发起切换过程 */

    /* 判断VoLTE下是否存在紧急PDN */
    /*lint -e438 */
    if(VOS_TRUE == IMSA_IsExistEmcCall() )
    {
        enImsDomainResult = IMSA_IMS_DOMAIN_LTE;

        enDomainHoType = IMSA_IMS_HO_TYPE_NO_HO;
        return enDomainHoType;
    }
    /*lint +e438 */


    /* 以上条件都满足,则启动L2W的切换 */
    /*lint -e438 */
    enImsDomainResult = IMSA_IMS_DOMAIN_WIFI;
    enDomainHoType = IMSA_IMS_HO_TYPE_LTE2WIFI;
    return enDomainHoType;
    /*lint +e438 */


}

/*****************************************************************************
 Function Name  : IMSA_ProcessNoDomainSelect()
 Description    : 没有域可以使用的处理
 Input          :
                  ucOrgPdnRlsFlag: 域切换是否释放原PDN：0 不释放，1 释放
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-07  Draft Enact
*****************************************************************************/

void IMSA_ProcessNoDomainSelect(VOS_UINT8 ucOrignalPdnRlsFlag)
{
    IMSA_DOMAIN_MANAGER_STRU    *pstImsaDomainManager = VOS_NULL_PTR;

    pstImsaDomainManager =  IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_ProcessNoDomainSelect_ENUM, LNAS_ENTRY);

    switch(pstImsaDomainManager->enDomainFsmL2State)
    {
        case IMSA_DOMAIN_FSM_L2_STATE_NULL:
            // no process, return
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN:
            pstImsaDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING;
            IMSA_SRV_DomainDisable(IMSA_IMS_DOMAIN_LTE, ucOrignalPdnRlsFlag);
            pstImsaDomainManager->ucDomainSelectFlag = VOS_TRUE;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN:
            pstImsaDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING;
            IMSA_SRV_DomainDisable(IMSA_IMS_DOMAIN_WIFI, ucOrignalPdnRlsFlag);
            pstImsaDomainManager->ucDomainSelectFlag = VOS_TRUE;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH:
        case IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH:
            IMSA_WARN_LOG("IMSA_ProcessNoDomainSelect: DomainC2 FSM Status Wrong!");

            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessNoDomainSelect_ENUM, LNAS_StateInvalid);
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING:

            //无处理；直接返回
            break;

        default:
            IMSA_WARN_LOG("IMSA_ProcessNoDomainSelect: DomainC2 FSM Status Wrong!");

            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessNoDomainSelect_ENUM, LNAS_StateInvalid);
    }

    return;
}


/*****************************************************************************
 Function Name  : IMSA_ProcessNoDomainSelectInMsccImsDomainCfgSetReq()
 Description    : 没有域可以使用的处理
 Input          :
                  ucOrgPdnRlsFlag: 域切换是否释放原PDN：0 不释放，1 释放
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-07  Draft Enact
*****************************************************************************/

void IMSA_ProcessNoDomainSelectInMsccImsDomainCfgSetReq(VOS_UINT8 ucOrignalPdnRlsFlag)
{
    IMSA_DOMAIN_MANAGER_STRU    *pstImsaDomainManager = VOS_NULL_PTR;

    pstImsaDomainManager =  IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_ProcessNoDomainSelectInMsccImsDomainCfgSetReq_ENUM, LNAS_ENTRY);

    switch(pstImsaDomainManager->enDomainFsmL2State)
    {
        case IMSA_DOMAIN_FSM_L2_STATE_NULL:
            // no process, return
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN:
            pstImsaDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING;
            IMSA_SRV_DomainDisable(IMSA_IMS_DOMAIN_LTE, ucOrignalPdnRlsFlag);
            pstImsaDomainManager->ucDomainSelectFlag = VOS_TRUE;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN:
            pstImsaDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING;
            IMSA_SRV_DomainDisable(IMSA_IMS_DOMAIN_WIFI, ucOrignalPdnRlsFlag);
            pstImsaDomainManager->ucDomainSelectFlag = VOS_TRUE;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH:
        case IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH:
            pstImsaDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_DISABLING;
            IMSA_SRV_DomainDisable(IMSA_IMS_DOMAIN_LTE_WIFI, ucOrignalPdnRlsFlag);
            pstImsaDomainManager->ucDomainSelectFlag = VOS_TRUE;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING:

            //无处理；直接返回
            break;

        default:
            IMSA_WARN_LOG("IMSA_ProcessNoDomainSelectInMsccImsDomainCfgSetReq: DomainC2 FSM Status Wrong!");

            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessNoDomainSelectInMsccImsDomainCfgSetReq_ENUM, LNAS_StateInvalid);
    }

    return;
}





/*****************************************************************************
 Function Name  : IMSA_ProcessNoDomainSelectInWifiPowerOff()
 Description    : 接收到WIFI Power Off后发现没有域可以使用的处理
 Input          :
                  ucOrgPdnRlsFlag: 域切换是否释放原PDN：0 不释放，1 释放
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-07  Draft Enact
*****************************************************************************/

VOS_VOID IMSA_ProcessNoDomainSelectInWifiPowerOff(VOS_VOID)
{
    IMSA_DOMAIN_MANAGER_STRU    *pstImsaDomainManager = VOS_NULL_PTR;

    pstImsaDomainManager =  IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_ProcessNoDomainSelectInWifiPowerOff_ENUM, LNAS_ENTRY);

    switch(pstImsaDomainManager->enDomainFsmL2State)
    {
        case IMSA_DOMAIN_FSM_L2_STATE_NULL:
            // no process, return
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN:
            IMSA_WARN_LOG("IMSA_ProcessNoDomainSelectInWifiPowerOff: DomainC2 FSM Status Wrong1!");

            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessNoDomainSelectInWifiPowerOff_ENUM, LNAS_FUNCTION_LABEL1);
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN:
            pstImsaDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING;
            IMSA_SRV_DomainDisable(IMSA_IMS_DOMAIN_WIFI, VOS_FALSE);
            pstImsaDomainManager->ucDomainSelectFlag = VOS_TRUE;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH:
        case IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH:
            IMSA_WARN_LOG("IMSA_ProcessNoDomainSelectInWifiPowerOff: DomainC2 FSM Status Wrong2!");

            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessNoDomainSelectInWifiPowerOff_ENUM, LNAS_FUNCTION_LABEL2);
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING:
            //无处理；直接返回
            break;

        default:
            IMSA_WARN_LOG("IMSA_ProcessNoDomainSelectInWifiPowerOff: DomainC2 FSM Status Wrong3!");

            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessNoDomainSelectInWifiPowerOff_ENUM, LNAS_FUNCTION_LABEL3);
    }

    return;
}

/*****************************************************************************
 Function Name  : IMSA_ProcessNoDomainSelectInCellularPowerOff()
 Description    : 接收到Cellular Power Off后发现没有域可以使用的处理
 Input          :
                  ucOrgPdnRlsFlag: 域切换是否释放原PDN：0 不释放，1 释放
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-07  Draft Enact
*****************************************************************************/

VOS_VOID IMSA_ProcessNoDomainSelectInCellularPowerOff(VOS_VOID)
{
    IMSA_DOMAIN_MANAGER_STRU    *pstImsaDomainManager = VOS_NULL_PTR;

    pstImsaDomainManager =  IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_ProcessNoDomainSelectInCellularPowerOff_ENUM, LNAS_ENTRY);

    switch(pstImsaDomainManager->enDomainFsmL2State)
    {
        case IMSA_DOMAIN_FSM_L2_STATE_NULL:
            // no process, return
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN:
            pstImsaDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING;
            IMSA_SRV_DomainDisable(IMSA_IMS_DOMAIN_WIFI, VOS_FALSE);
            pstImsaDomainManager->ucDomainSelectFlag = VOS_TRUE;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN:
            IMSA_WARN_LOG("IMSA_ProcessNoDomainSelectInCellularPowerOff: DomainC2 FSM Status Wrong1!");

            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessNoDomainSelectInCellularPowerOff_ENUM, LNAS_FUNCTION_LABEL1);
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH:
        case IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH:
            IMSA_WARN_LOG("IMSA_ProcessNoDomainSelectInCellularPowerOff: DomainC2 FSM Status Wrong2!");

            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessNoDomainSelectInCellularPowerOff_ENUM, LNAS_FUNCTION_LABEL2);
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING:
            //无处理；直接返回
            break;

        default:
            IMSA_WARN_LOG("IMSA_ProcessNoDomainSelectInCellularPowerOff: DomainC2 FSM Status Wrong3!");

            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessNoDomainSelectInCellularPowerOff_ENUM, LNAS_FUNCTION_LABEL3);
    }

    return;
}

/*****************************************************************************
 Function Name  : IMSA_ProcessNoDomainSelectInMsccServiceChangeInd()
 Description    : 收到MSCC_Service_Changed_ind, 判决没有域可以使用的处理
 Input          :
                  ucOrgPdnRlsFlag: 域切换是否释放原PDN：0 不释放，1 释放
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-07  Draft Enact
*****************************************************************************/

void IMSA_ProcessNoDomainSelectInMsccServiceChangeInd(VOS_UINT8 ucOrignalPdnRlsFlag)
{
    IMSA_DOMAIN_MANAGER_STRU    *pstImsaDomainManager = VOS_NULL_PTR;

    pstImsaDomainManager =  IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_ProcessNoDomainSelectInMsccServiceChangeInd_ENUM, LNAS_ENTRY);

    switch(pstImsaDomainManager->enDomainFsmL2State)
    {
        case IMSA_DOMAIN_FSM_L2_STATE_NULL:
            // no process, return
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN:
            
            pstImsaDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING;
            IMSA_SRV_DomainDisable(IMSA_IMS_DOMAIN_LTE, ucOrignalPdnRlsFlag);
            pstImsaDomainManager->ucDomainSelectFlag = VOS_TRUE;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN:
            /* Disable Wifi只有WIFI Power Off 会进行操作,异常不处理 */
            IMSA_WARN_LOG("IMSA_ProcessNoDomainSelectInMsccServiceChangeInd: Abnoral Process to disable WIFI!");

            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessNoDomainSelectInMsccServiceChangeInd_ENUM, LNAS_FUNCTION_LABEL1);
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH:
        case IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH:
            /* 除非整机关机,否则切换过程中不会存在双域关闭的情况,异常不处理 */
            IMSA_WARN_LOG("IMSA_ProcessNoDomainSelectInMsccServiceChangeInd: Abnoral Process to disable LTE/WIFI!");

            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessNoDomainSelectInMsccServiceChangeInd_ENUM, LNAS_FUNCTION_LABEL2);
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING:
            /* 正在Disable 域, 无处理；直接返回 */
            IMSA_WARN_LOG("IMSA_ProcessNoDomainSelectInMsccServiceChangeInd: Disable on-going!");

            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessNoDomainSelectInMsccServiceChangeInd_ENUM, LNAS_FUNCTION_LABEL3);

            break;

        default:
            IMSA_WARN_LOG("IMSA_ProcessNoDomainSelectInMsccServiceChangeInd: DomainC2 FSM Status Wrong!");

            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessNoDomainSelectInMsccServiceChangeInd_ENUM, LNAS_FUNCTION_LABEL4);
            break;
    }

    return;
}


/*****************************************************************************
 Function Name  : IMSA_ProcessWifiOnlyDomainSelect()
 Description    : 只有WIFI域可以使用的处理
 Input          :
                  ucOrgPdnRlsFlag: 域切换是否释放原PDN：0 不释放，1 释放
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-07  Draft Enact
*****************************************************************************/

void IMSA_ProcessWifiOnlyDomainSelect(VOS_UINT8 ucPdnRlsFlag)
{
    IMSA_DOMAIN_MANAGER_STRU              *pstDomainManager   = VOS_NULL_PTR;

    /* 初始化紧急连接 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_ProcessWifiOnlyDomainSelect_ENUM, LNAS_ENTRY);

    switch( pstDomainManager->enDomainFsmL2State )
    {
        case IMSA_DOMAIN_FSM_L2_STATE_NULL:
            /* 判断当前VoWIFI是否受VoLTE控制, 返回VoWIFI是否可用 */
            if( VOS_TRUE == IMSA_Domain_CheckWifiPossibleInVolteCtrlVowifi())
            {
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING;
                IMSA_SRV_DomainEnable(IMSA_IMS_DOMAIN_WIFI);
                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            else
            {
                /* VoWIFI不可用, 则停留在NULL状态 */
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN:
            /* 判断当前VoWIFI是否受VoLTE控制, 返回VoWIFI是否可用 */
            if(( VOS_TRUE == IMSA_Domain_CheckWifiPossibleInVolteCtrlVowifi())
               &&(VOS_FALSE == IMSA_IsExistEmcCall()))
            {
                /* VoWIFI可用, 则启动VoLTE->VoWIFI的切换 */
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH;
                IMSA_SRV_DomainSwitch(IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH, VOS_FALSE, ucPdnRlsFlag);

                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            else
            {
                /* VoWIFI不可用, 则Disable LTE域: 因为只有WIFI域可用, 但是WIFI又无法接入 */
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING;
                IMSA_SRV_DomainDisable(IMSA_IMS_DOMAIN_LTE,ucPdnRlsFlag);

                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN:
            /* 判断当前VoWIFI是否受VoLTE控制, 返回VoWIFI是否可用 */
            if( VOS_TRUE == IMSA_Domain_CheckWifiPossibleInVolteCtrlVowifi())
            {
                //无处理，保持现状
            }
            else
            {
                /* VoWIFI不可用, 则Disable WIFI域 */
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING;
                IMSA_SRV_DomainDisable(IMSA_IMS_DOMAIN_WIFI,ucPdnRlsFlag);
                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH:
        case IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH:
            IMSA_WARN_LOG("IMSA_ProcessWifiOnlyDomainSelect: No Switch State In Delay Event!");
            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessWifiOnlyDomainSelect_ENUM, IMSA_STATE_ERROR);
            break;
        case IMSA_DOMAIN_FSM_L2_STATE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING:
            // 记录事件，启动定时器；
            break;

        default:
            IMSA_WARN_LOG("IMSA_ProcessWifiOnlyDomainSelect: DomainC2 FSM Status Wrong!");
            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessWifiOnlyDomainSelect_ENUM, LNAS_StateInvalid);
    }

    return;

}


/*****************************************************************************
 Function Name  : IMSA_ProcessWifiOnlyDomainSelectInMsccImsDomainCfgSetReq()
 Description    : 只有WIFI域可以使用的处理
 Input          :
                  ucOrgPdnRlsFlag: 域切换是否释放原PDN：0 不释放，1 释放
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-07  Draft Enact
*****************************************************************************/

void IMSA_ProcessWifiOnlyDomainSelectInMsccImsDomainCfgSetReq(VOS_UINT8 ucPdnRlsFlag)
{
    IMSA_DOMAIN_MANAGER_STRU              *pstDomainManager   = VOS_NULL_PTR;

    /* 初始化紧急连接 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_ProcessWifiOnlyDomainSelectInMsccImsDomainCfgSetReq_ENUM, LNAS_ENTRY);

    switch( pstDomainManager->enDomainFsmL2State )
    {
        case IMSA_DOMAIN_FSM_L2_STATE_NULL:
            /* 判断当前VoWIFI是否受VoLTE控制, 返回VoWIFI是否可用 */
            if( VOS_TRUE == IMSA_Domain_CheckWifiPossibleInVolteCtrlVowifi())
            {
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING;
                IMSA_SRV_DomainEnable(IMSA_IMS_DOMAIN_WIFI);
                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            else
            {
                /* VoWIFI不可用, 则停留在NULL状态 */
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN:
            /* 判断当前VoWIFI是否受VoLTE控制, 返回VoWIFI是否可用 */
            if( VOS_TRUE == IMSA_Domain_CheckWifiPossibleInVolteCtrlVowifi())
            {
                if(VOS_FALSE == IMSA_IsExistEmcCall())
                {
                    /* VoWIFI可用, 则启动VoLTE->VoWIFI的切换 */
                    pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH;
                    IMSA_SRV_DomainSwitch(IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH, VOS_FALSE, ucPdnRlsFlag);

                    pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
                }
                else
                {
                    /* 停留在LTE下,等待紧急PDN释放 */
                }
            }
            else
            {
                /* VoWIFI不可用, 则Disable LTE域: 因为只有WIFI域可用, 但是WIFI又无法接入 */
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING;
                IMSA_SRV_DomainDisable(IMSA_IMS_DOMAIN_LTE,ucPdnRlsFlag);

                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN:
            /* 判断当前VoWIFI是否受VoLTE控制, 返回VoWIFI是否可用 */
            if( VOS_TRUE == IMSA_Domain_CheckWifiPossibleInVolteCtrlVowifi())
            {
                //无处理，保持现状
            }
            else
            {
                /* VoWIFI不可用, 则Disable WIFI域 */
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING;
                IMSA_SRV_DomainDisable(IMSA_IMS_DOMAIN_WIFI,ucPdnRlsFlag);
                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH:
            /* 判断当前VoWIFI是否受VoLTE控制, 返回VoWIFI是否可用 */
            if( VOS_TRUE == IMSA_Domain_CheckWifiPossibleInVolteCtrlVowifi())
            {
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH;
                IMSA_SRV_DomainSwitch(IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH, VOS_FALSE, ucPdnRlsFlag);
                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            else
            {
                /* VoWIFI不可用, 则Abort LTE2WIFI的切换, disable双域 */
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_DISABLING;
                IMSA_SRV_DomainDisable(IMSA_IMS_DOMAIN_LTE_WIFI,ucPdnRlsFlag);
                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH:
            /* 判断当前VoWIFI是否受VoLTE控制, 返回VoWIFI是否可用 */
            if( VOS_TRUE == IMSA_Domain_CheckWifiPossibleInVolteCtrlVowifi())
            {
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH;
                IMSA_SRV_DomainSwitchAbort(IMSA_DOMAIN_SWITCH_TYPE_W2L_SWITCH,ucPdnRlsFlag);
                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            else
            {
                /* VoWIFI不可用, 则Disable WIFI和LTE域 */
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_DISABLING;
                IMSA_SRV_DomainDisable(IMSA_IMS_DOMAIN_LTE_WIFI,ucPdnRlsFlag);
                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            break;
        case IMSA_DOMAIN_FSM_L2_STATE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING:
            // 记录事件，启动定时器；
            break;

        default:
            IMSA_WARN_LOG("IMSA_ProcessWifiOnlyDomainSelectInMsccImsDomainCfgSetReq: DomainC2 FSM Status Wrong!");

            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessWifiOnlyDomainSelectInMsccImsDomainCfgSetReq_ENUM, LNAS_StateInvalid);
    }

    return;

}

/*****************************************************************************
 Function Name  : IMSA_ProcessWifiOnlyDomainSelectInWifiStateInd()
 Description    : 只有WIFI域可以使用的处理
 Input          :
                  ucOrgPdnRlsFlag: 域切换是否释放原PDN：0 不释放，1 释放
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-07  Draft Enact
*****************************************************************************/

void IMSA_ProcessWifiOnlyDomainSelectInWifiStateInd
(
    VOS_UINT8 ucOrgPdnRlsFlag
)
{
    IMSA_DOMAIN_MANAGER_STRU              *pstDomainManager   = VOS_NULL_PTR;

    /* 初始化紧急连接 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_ProcessWifiOnlyDomainSelectInWifiStateInd_ENUM, LNAS_ENTRY);

    switch( pstDomainManager->enDomainFsmL2State )
    {
        case IMSA_DOMAIN_FSM_L2_STATE_NULL:
            /* 判断当前VoWIFI是否受VoLTE控制, 返回VoWIFI是否可用 */
            if( VOS_TRUE == IMSA_Domain_CheckWifiPossibleInVolteCtrlVowifi())
            {
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING;
                IMSA_SRV_DomainEnable(IMSA_IMS_DOMAIN_WIFI);
                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            else
            {
                /* VoWIFI不可用, 则停留在NULL状态 */
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN:
         /* 当前处于LTE域,一个WIFI信号强度指示不会导致选择到WIFI ONLY去,这是一种异常,不处理
            IMSA_WARN_LOG("IMSA_WifiOnlyProcess: DomainC2 FSM Status Wrong!");*/
            /* 设计时没有考虑到如下场景: 当前处于LTE域, 但是整机在GU下,有可能终端不释放IMS PDN(移动定制要求不在L下释放,但是可以不释放),
                          这时,一个WIFI信号强度指示,就有可能发生L2_FSM在LTE下,但是要求选择到WIFI ONLY去 */
            if(( VOS_TRUE == IMSA_Domain_CheckWifiPossibleInVolteCtrlVowifi())
                &&(VOS_FALSE == IMSA_IsExistEmcCall()))
            {
                /* VoWIFI可用, 则启动VoLTE->VoWIFI的切换 */
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH;
                IMSA_SRV_DomainSwitch(IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH, VOS_FALSE, VOS_TRUE);

                /* 设置启动了域操作行为 */
                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            else
            {
                /* VoWIFI不可用, 则停留于LTE域: 因为只有WIFI域可用, 但是WIFI又无法接入 */
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN:
            /* 判断当前VoWIFI是否受VoLTE控制, 返回VoWIFI是否可用 */
            if( VOS_TRUE == IMSA_Domain_CheckWifiPossibleInVolteCtrlVowifi())
            {
                //无处理，保持现状
                IMSA_SRV_DomainContinue(IMSA_IMS_DOMAIN_WIFI);
            }
            else
            {
                /* VoWIFI不可用, 则Disable WIFI域 */
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING;
                IMSA_SRV_DomainDisable(IMSA_IMS_DOMAIN_WIFI,ucOrgPdnRlsFlag);
                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH:
            /* L2W 切换是前一个域选的结果,因此收到WIFI State Ind消息, 保持当前L2W切换处理不变 */
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH:
            /* 如果前一个状态处于W2L, 但是当前又处于WIFI only,中间肯定插入一个操作导致LTE域disable,
                         可能是LTE转到GU, 这个时候前一个操作应该已经Abort W2L的操作，等待SRV返回结果,
                         因此这种情况下不处理 */

            break;

        case IMSA_DOMAIN_FSM_L2_STATE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING:
            /* 记录事件，等待前一个Disable域操作完成再进行处理 */
            pstDomainManager->ucMsgDelayProcessFlag = VOS_TRUE;
            break;

        default:
            IMSA_WARN_LOG("IMSA_ProcessWifiOnlyDomainSelectInMsccImsDomainCfgSetReq: DomainC2 FSM Status Wrong!");

            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessWifiOnlyDomainSelectInMsccImsDomainCfgSetReq_ENUM, LNAS_StateInvalid);
    }

    return;

}



/*****************************************************************************
 Function Name  : IMSA_ProcessWifiOnlyDomainSelectInMsccServiceChangeInd()
 Description    : MSCC_SERVICE_CHANGE_IND消息下, 只有WIFI域可以使用的处理
 Input          :
                  ucOrgPdnRlsFlag: 域切换是否释放原PDN：0 不释放，1 释放
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-07  Draft Enact
*****************************************************************************/

void IMSA_ProcessWifiOnlyDomainSelectInMsccServiceChangeInd(VOS_UINT8 ucPdnRlsFlag)
{
    IMSA_DOMAIN_MANAGER_STRU              *pstDomainManager   = VOS_NULL_PTR;

    /* 初始化紧急连接 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_ProcessWifiOnlyDomainSelectInMsccServiceChangeInd_ENUM, LNAS_ENTRY);

    switch( pstDomainManager->enDomainFsmL2State )
    {
        case IMSA_DOMAIN_FSM_L2_STATE_NULL:
            /* 判断当前VoWIFI是否受VoLTE控制, 返回VoWIFI是否可用:
                         WIFI下做判决的原因是,很可能由于LTE驻留条件的变更,导致网络IMS能力变化,这时重新尝试接入 */
            if( VOS_TRUE == IMSA_Domain_CheckWifiPossibleInVolteCtrlVowifi())
            {
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING;
                IMSA_SRV_DomainEnable(IMSA_IMS_DOMAIN_WIFI);
                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            else
            {
                /* VoWIFI不可用, 则停留在NULL状态 */
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN:
            /* 当前处于LTE域,一个Cellular服务能力变更(如L->GU), 导致选择到WIFI ONLY */
            /* 判断当前VoWIFI是否受VoLTE控制, 返回VoWIFI是否可用 */
            if(( VOS_TRUE == IMSA_Domain_CheckWifiPossibleInVolteCtrlVowifi())
                &&(VOS_FALSE == IMSA_IsExistEmcCall()))
            {
                /* VoWIFI可用, 则启动VoLTE->VoWIFI的切换 */
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH;
                IMSA_SRV_DomainSwitch(IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH, VOS_FALSE, ucPdnRlsFlag);

                /* 设置启动了域操作行为 */
                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            else
            {
                /* VoWIFI不可用, 则停留于LTE域: 因为只有WIFI域可用, 但是WIFI又无法接入 */
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN:
            /* 判断当前VoWIFI是否受VoLTE控制, 返回VoWIFI是否可用 */
            if( VOS_TRUE == IMSA_Domain_CheckWifiPossibleInVolteCtrlVowifi())
            {
                //无处理，保持现状
            }
            else
            {
                /* Cellular服务状态发生变更,导致VoWIFI变为不可用: 则Disable WIFI域 */
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING;
                IMSA_SRV_DomainDisable(IMSA_IMS_DOMAIN_WIFI,ucPdnRlsFlag);
                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH:
            /* 上次域选结果导致L2W, 而本次接收到Service_change_ind消息导致LTE不可用:保持切换状态 */
            /* 判断当前VoWIFI是否受VoLTE控制, 返回VoWIFI是否可用 */
            if( VOS_TRUE == IMSA_Domain_CheckWifiPossibleInVolteCtrlVowifi())
            {
                //无处理，保持现状
            }
            else
            {
                /* VoWIFI不可用, 则Abort LTE2WIFI的切换, disable双域 */
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_DISABLING;
                IMSA_SRV_DomainDisable(IMSA_IMS_DOMAIN_LTE_WIFI,ucPdnRlsFlag);
                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH:
            /* 判断当前VoWIFI是否受VoLTE控制, 返回VoWIFI是否可用 */
            if( VOS_TRUE == IMSA_Domain_CheckWifiPossibleInVolteCtrlVowifi())
            {
                /* 上次域选结果导致W2L, 本次由于LTE不可用,导致域选结果是停留在WIFI下, abort切换 */
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH;
                IMSA_SRV_DomainSwitchAbort(IMSA_DOMAIN_SWITCH_TYPE_W2L_SWITCH,ucPdnRlsFlag);
                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            else
            {
                /* VoWIFI不可用, 则Disable WIFI和LTE域 */
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_DISABLING;
                IMSA_SRV_DomainDisable(IMSA_IMS_DOMAIN_LTE_WIFI,ucPdnRlsFlag);
                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING:
            break;

        default:
            IMSA_WARN_LOG("IMSA_ProcessWifiOnlyDomainSelectInMsccServiceChangeInd: DomainC2 FSM Status Wrong!");

            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessWifiOnlyDomainSelectInMsccServiceChangeInd_ENUM, LNAS_StateInvalid);
            break;
    }

    return;

}

/*****************************************************************************
 Function Name  : IMSA_ProcessWifiOnlyDomainSelectInUsimFileReady()
 Description    : UsimFile文件收全以后的操作: 只有WIFI域可以使用的处理
 Input          :
                  ucOrgPdnRlsFlag: 域切换是否释放原PDN：0 不释放，1 释放
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-07  Draft Enact
*****************************************************************************/

void IMSA_ProcessWifiOnlyDomainSelectInUsimFileReady(VOS_UINT8 ucPdnRlsFlag)
{
    IMSA_DOMAIN_MANAGER_STRU              *pstDomainManager   = VOS_NULL_PTR;

    /* 初始化紧急连接 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_ProcessWifiOnlyDomainSelectInUsimFileReady_ENUM, LNAS_ENTRY);

    switch( pstDomainManager->enDomainFsmL2State )
    {
        case IMSA_DOMAIN_FSM_L2_STATE_NULL:
            /* 判断当前VoWIFI是否受VoLTE控制, 返回VoWIFI是否可用 */
            if( VOS_TRUE == IMSA_Domain_CheckWifiPossibleInVolteCtrlVowifi())
            {
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING;
                IMSA_SRV_DomainEnable(IMSA_IMS_DOMAIN_WIFI);
                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            else
            {
                /* VoWIFI不可用, 则停留在NULL状态 */
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN:
        case IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH:
        case IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH:
        case IMSA_DOMAIN_FSM_L2_STATE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING:

            IMSA_WARN_LOG("IMSA_ProcessWifiOnlyDomainSelectInUsimFileReady: Error Status in USIM Card File Ready!");
            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessWifiOnlyDomainSelectInUsimFileReady_ENUM, IMSA_STATE_ERROR);
            break;

        default:
            IMSA_WARN_LOG("IMSA_ProcessWifiOnlyDomainSelectInUsimFileReady: DomainC2 FSM Status Wrong!");
            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessWifiOnlyDomainSelectInUsimFileReady_ENUM, LNAS_StateInvalid);
            break;
    }

    return;

}



/*****************************************************************************
 Function Name  : IMSA_ProcessWifiOnlyDomainSelectInCellularPowerOff()
 Description    : CellularPOWER OFF消息处理以后,只剩下WIFI域可以使用的处理
 Input          :
                  ucOrgPdnRlsFlag: 域切换是否释放原PDN：0 不释放，1 释放
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-07  Draft Enact
*****************************************************************************/

VOS_VOID IMSA_ProcessWifiOnlyDomainSelectInCellularPowerOff()
{
    IMSA_DOMAIN_MANAGER_STRU              *pstDomainManager   = VOS_NULL_PTR;

    /* 初始化紧急连接 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();
    IMSA_INFO_LOG("IMSA_ProcessWifiOnlyDomainSelectInCellularPowerOff: Enter!");
    TLPS_PRINT2LAYER_INFO(IMSA_ProcessWifiOnlyDomainSelectInCellularPowerOff_ENUM, LNAS_ENTRY);

    switch( pstDomainManager->enDomainFsmL2State )
    {
        case IMSA_DOMAIN_FSM_L2_STATE_NULL:
            IMSA_INFO_LOG("IMSA_ProcessWifiOnlyDomainSelectInCellularPowerOff:1:DomainEnable!");
            pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING;
            IMSA_SRV_DomainEnable(IMSA_IMS_DOMAIN_WIFI);
            pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN:

            IMSA_INFO_LOG("IMSA_ProcessWifiOnlyDomainSelectInCellularPowerOff:2:Domainswitch L2W!");
            pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH;
            IMSA_SRV_DomainSwitch(IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH, VOS_FALSE, VOS_FALSE);
            pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN:
            //无处理，保持现状
            IMSA_INFO_LOG("IMSA_ProcessWifiOnlyDomainSelectInCellularPowerOff:3: stay on wifi!");
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH:
            IMSA_INFO_LOG("IMSA_ProcessWifiOnlyDomainSelectInCellularPowerOff:4:DomainswitchAbort W2L!");
            IMSA_SRV_DomainSwitchAbort(IMSA_DOMAIN_SWITCH_TYPE_W2L_SWITCH,VOS_TRUE);
            pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH:
            IMSA_INFO_LOG("IMSA_ProcessWifiOnlyDomainSelectInCellularPowerOff:5:Domainswitch L2W change para!");
            IMSA_SRV_DomainSwitch(IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH, VOS_FALSE, VOS_TRUE);
            pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING:

            IMSA_INFO_LOG("IMSA_ProcessWifiOnlyDomainSelectInCellularPowerOff:6:Disabling!");
            // 记录事件，启动定时器；
            break;

        default:
            IMSA_WARN_LOG("IMSA_ProcessWifiOnlyDomainSelectInCellularPowerOff: DomainC2 FSM Status Wrong!");
    }

    return;
}

/*****************************************************************************
 Function Name  : IMSA_ProcessLTEOnlyDomainSelect()
 Description    : 只有WIFI域可以使用的处理
 Input          :
                  ucOrgPdnRlsFlag: 域切换是否释放原PDN：0 不释放，1 释放
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-07  Draft Enact
*****************************************************************************/

void IMSA_ProcessLTEOnlyDomainSelect(VOS_UINT8 ucPdnRlsFlag)
{
    IMSA_DOMAIN_MANAGER_STRU              *pstDomainManager   = VOS_NULL_PTR;

    /* 初始化紧急连接 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_ProcessLTEOnlyDomainSelect_ENUM, LNAS_ENTRY);

    switch( pstDomainManager->enDomainFsmL2State )
    {
        case IMSA_DOMAIN_FSM_L2_STATE_NULL:
            pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING;
            IMSA_SRV_DomainEnable(IMSA_IMS_DOMAIN_LTE);
            pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN:
            //无处理，保持现状
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN:
            pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH;
            IMSA_SRV_DomainSwitch(IMSA_DOMAIN_SWITCH_TYPE_W2L_SWITCH, VOS_FALSE, ucPdnRlsFlag);
            pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH:
        case IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH:
            IMSA_WARN_LOG("IMSA_ProcessLTEOnlyDomainSelect: No Switch State in Delay Event!");
            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessLTEOnlyDomainSelect_ENUM, IMSA_STATE_ERROR);
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING:

            // 记录事件，启动定时器；
            break;

        default:
            IMSA_WARN_LOG("IMSA_ProcessLTEOnlyDomainSelect: DomainC2 FSM Status Wrong!");
            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessLTEOnlyDomainSelect_ENUM, LNAS_StateInvalid);
    }

    return;

}


/*****************************************************************************
 Function Name  : IMSA_ProcessLTEOnlyDomainSelectInMsccImsDomainCfgSetReq()
 Description    : 只有WIFI域可以使用的处理
 Input          :
                  ucOrgPdnRlsFlag: 域切换是否释放原PDN：0 不释放，1 释放
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-07  Draft Enact
*****************************************************************************/

void IMSA_ProcessLTEOnlyDomainSelectInMsccImsDomainCfgSetReq(VOS_UINT8 ucPdnRlsFlag)
{
    IMSA_DOMAIN_MANAGER_STRU              *pstDomainManager   = VOS_NULL_PTR;

    /* 初始化紧急连接 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_ProcessLTEOnlyDomainSelectInMsccImsDomainCfgSetReq_ENUM, LNAS_ENTRY);

    switch( pstDomainManager->enDomainFsmL2State )
    {
        case IMSA_DOMAIN_FSM_L2_STATE_NULL:
            pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING;
            IMSA_SRV_DomainEnable(IMSA_IMS_DOMAIN_LTE);
            pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN:
            //无处理，保持现状
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN:
            pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH;
            IMSA_SRV_DomainSwitch(IMSA_DOMAIN_SWITCH_TYPE_W2L_SWITCH, VOS_FALSE, ucPdnRlsFlag);
            pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH:
            pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH;
            IMSA_SRV_DomainSwitch(IMSA_DOMAIN_SWITCH_TYPE_W2L_SWITCH, VOS_FALSE, ucPdnRlsFlag);
            pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH:
            pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH;
            IMSA_SRV_DomainSwitchAbort(IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH,ucPdnRlsFlag);
            pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING:

            // 记录事件，启动定时器；
            break;

        default:
            IMSA_WARN_LOG("IMSA_ProcessLTEOnlyDomainSelectInMsccImsDomainCfgSetReq: DomainC2 FSM Status Wrong!");
            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessLTEOnlyDomainSelectInMsccImsDomainCfgSetReq_ENUM, LNAS_StateInvalid);
    }

    return;

}

/*****************************************************************************
 Function Name  : IMSA_ProcessLTEOnlyDomainSelectInWifiPowerOff()
 Description    : WIFI POWER OFF消息处理以后,只剩下LTE域可以使用的处理
 Input          :
                  ucOrgPdnRlsFlag: 域切换是否释放原PDN：0 不释放，1 释放
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-07  Draft Enact
*****************************************************************************/

VOS_VOID IMSA_ProcessLTEOnlyDomainSelectInWifiPowerOff()
{
    IMSA_DOMAIN_MANAGER_STRU              *pstDomainManager   = VOS_NULL_PTR;

    /* 初始化紧急连接 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_ProcessLTEOnlyDomainSelectInWifiPowerOff_ENUM, LNAS_ENTRY);

    switch( pstDomainManager->enDomainFsmL2State )
    {
        case IMSA_DOMAIN_FSM_L2_STATE_NULL:
            pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING;
            IMSA_SRV_DomainEnable(IMSA_IMS_DOMAIN_LTE);
            pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN:
            //无处理，保持现状
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN:
            pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH;
            IMSA_SRV_DomainSwitch(IMSA_DOMAIN_SWITCH_TYPE_W2L_SWITCH, VOS_FALSE, VOS_FALSE);
            pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH:
            IMSA_SRV_DomainSwitch(IMSA_DOMAIN_SWITCH_TYPE_W2L_SWITCH, VOS_FALSE, VOS_FALSE);
            pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH:
            IMSA_SRV_DomainSwitchAbort(IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH,VOS_FALSE);
            pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING:

            // 记录事件，启动定时器；
            break;

        default:
            IMSA_WARN_LOG("IMSA_ProcessLTEOnlyDomainSelectInWifiPowerOff: DomainC2 FSM Status Wrong!");
            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessLTEOnlyDomainSelectInWifiPowerOff_ENUM, LNAS_StateInvalid);
    }

    return;
}

/*****************************************************************************
 Function Name  : IMSA_ProcessLTEOnlyDomainSelectInWifiStateInd()
 Description    : 接收到WIFI_STATE_IND消息下, 只有LTE域可以使用的处理
 Input          :
                  ucOrgPdnRlsFlag: 域切换是否释放原PDN：0 不释放，1 释放
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-07  Draft Enact
*****************************************************************************/

void IMSA_ProcessLTEOnlyDomainSelectInWifiStateInd(VOS_UINT8 ucPdnRlsFlag)
{
    IMSA_DOMAIN_MANAGER_STRU              *pstDomainManager   = VOS_NULL_PTR;

    /* 初始化紧急连接 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_ProcessLTEOnlyDomainSelectInWifiStateInd_ENUM, LNAS_ENTRY);

    switch( pstDomainManager->enDomainFsmL2State )
    {
        case IMSA_DOMAIN_FSM_L2_STATE_NULL:
            /* WIFI State_Ind消息接收,触发启动LTE域: 不是正常状态下的处理. */
            IMSA_WARN_LOG("IMSA_ProcessLTEOnlyDomainSelectInWifiStateInd: Go LTE Domain when receiving Wifi State Ind ");
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN:
            /* LTE域驻留或者尝试驻留状态下收到WIFI State ind,无处理,保持当前状态 */

            break;

        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN:
            
#if 0
            IMSA_SRV_DomainSwitch(IMSA_IMS_HO_TYPE_W2L_SWITCH, 0, ucPdnRlsFlag);
            pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH;
#endif
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH:
            /* 单域的W2L切换,大部分是由于WIFI Power Off导致的,因此这种情况下,接收到WIFI State Ind 不影响当前处理,直接返回 */

            break;

        case IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH:
            /* Wifi State Ind消息导致选择LTE ,同时前一个域选导致在L2W切换状态, 异常场景不进行任何处理 */
            IMSA_WARN_LOG("IMSA_ProcessLTEOnlyDomainSelectInWifiStateInd: Abnormal Switch!");
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING:

            /* 记录事件，等待前一个Disable域操作完成再进行处理 */
            pstDomainManager->ucMsgDelayProcessFlag = VOS_TRUE;
            break;

        default:
            IMSA_WARN_LOG("IMSA_ProcessLTEOnlyDomainSelectInWifiStateInd: DomainC2 FSM Status Wrong!");
            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessLTEOnlyDomainSelectInWifiStateInd_ENUM, LNAS_StateInvalid);
    }

    return;

}

/*****************************************************************************
 Function Name  : IMSA_ProcessLTEOnlyDomainSelectInMsccServiceChangeInd()
 Description    : 接收到MSCC_SERVICE_CHANGE_IND消息下, 只有LTE域可以使用的处理
 Input          :
                  ucOrgPdnRlsFlag: 域切换是否释放原PDN：0 不释放，1 释放
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-07  Draft Enact
*****************************************************************************/

void IMSA_ProcessLTEOnlyDomainSelectInMsccServiceChangeInd(VOS_UINT8 ucPdnRlsFlag)
{
    IMSA_DOMAIN_MANAGER_STRU              *pstDomainManager   = VOS_NULL_PTR;

    /* 初始化紧急连接 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_ProcessLTEOnlyDomainSelectInMsccServiceChangeInd_ENUM, LNAS_ENTRY);

    switch( pstDomainManager->enDomainFsmL2State )
    {
        case IMSA_DOMAIN_FSM_L2_STATE_NULL:
            /* NULL状态下收到Service_Change_ind, 且选择了LTE域,则启动LTE操作 */
            pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING;

            IMSA_SRV_DomainEnable(IMSA_IMS_DOMAIN_LTE);
            pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN:
            /* 调用Continue处理 */
            IMSA_SRV_DomainContinue(IMSA_IMS_DOMAIN_LTE);
            pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN:
            /* 收到Service change ind消息,由一个单域转到另外一个单域,这种属于异常场景 */
            IMSA_WARN_LOG("IMSA_ProcessLTEOnlyDomainSelectInMsccServiceChangeInd: Abnormal From WIFI2LTE");
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH:
            /* 如果出现前面域操作发起切换过程, 当前处于单域状态,应该是前一个操作导致的,
                        如WIFI Power off,这时,不再重复发起操作,等待前一个预选结果 */
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH:
            /* 前一个域选结果是L2W的切换, 但是当前域选结果确实LTE单域, 中间存在WIFI disable的情况,这是一种异常 */
            IMSA_WARN_LOG("IMSA_ProcessLTEOnlyDomainSelectInMsccServiceChangeInd: L2W switch in LTE only domain select");
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING:
            break;
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING:
            TLPS_PRINT2LAYER_INFO(IMSA_ProcessLTEOnlyDomainSelectInMsccServiceChangeInd_ENUM, 0);
            pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING;
            IMSA_SRV_DomainContinue(IMSA_IMS_DOMAIN_LTE);
            pstDomainManager->ucDomainSelectFlag = VOS_TRUE;

            break;

        default:
            IMSA_WARN_LOG("IMSA_ProcessLTEOnlyDomainSelectInMsccServiceChangeInd: DomainC2 FSM Status Wrong!");
            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessLTEOnlyDomainSelectInMsccServiceChangeInd_ENUM, LNAS_StateInvalid);
    }

    return;

}


/*****************************************************************************
 Function Name  : IMSA_ProcessLTEOnlyDomainSelectInUsimFileReady()
 Description    : USIM Card File Ready下的域选处理: 只有WIFI域可以使用的处理
 Input          :
                  ucOrgPdnRlsFlag: 域切换是否释放原PDN：0 不释放，1 释放
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-07  Draft Enact
*****************************************************************************/

void IMSA_ProcessLTEOnlyDomainSelectInUsimFileReady(VOS_UINT8 ucPdnRlsFlag)
{
    IMSA_DOMAIN_MANAGER_STRU              *pstDomainManager   = VOS_NULL_PTR;

    /* 初始化紧急连接 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_ProcessLTEOnlyDomainSelectInUsimFileReady_ENUM, LNAS_ENTRY);

    switch( pstDomainManager->enDomainFsmL2State )
    {
        case IMSA_DOMAIN_FSM_L2_STATE_NULL:
            pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING;
            IMSA_SRV_DomainEnable(IMSA_IMS_DOMAIN_LTE);
            pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN:
        case IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH:
        case IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH:
        case IMSA_DOMAIN_FSM_L2_STATE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING:
            IMSA_WARN_LOG("IMSA_ProcessLTEOnlyDomainSelectInUsimFileReady: Error Status in USIM Card File Ready!");
            break;

        default:
            IMSA_WARN_LOG("IMSA_ProcessLTEOnlyDomainSelectInUsimFileReady: DomainC2 FSM Status Wrong!");
    }

    return;

}



/*****************************************************************************
 Function Name  : IMSA_DualDomainProcess()
 Description    : 双候选域的处理
 Input          :
                      ucOrgPdnRlsFlag: 域切换是否释放原PDN：0 不释放，1 释放
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-07  Draft Enact
*****************************************************************************/
void IMSA_ProcessDualDomainSelect(VOS_UINT8 ucHoFailReturn2OrgPdnFlag, VOS_UINT8 ucOrgPdnRlsFlag)
{
    IMSA_DOMAIN_MANAGER_STRU              *pstDomainManager   = VOS_NULL_PTR;
    IMSA_IMS_DOMAIN_ENUM_UINT8             enFinalDomain;
    IMSA_IMS_HO_TYPE_ENUM_UINT8           enHoType;


    IMSA_WARN_LOG("IMSA_ProcessDualDomainSelect: enter!");
    TLPS_PRINT2LAYER_INFO(IMSA_ProcessDualDomainSelect_ENUM, LNAS_ENTRY);

    /* 初始化紧急连接 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();

    switch(pstDomainManager->enDomainFsmL2State)
    {
        case IMSA_DOMAIN_FSM_L2_STATE_NULL:
            /* 初始IMS域选择判决算法 */
            enFinalDomain = IMSA_JudgeImsDomainC21();

            if(IMSA_IMS_DOMAIN_WIFI  == enFinalDomain)
            {
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING;
                IMSA_SRV_DomainEnable(IMSA_IMS_DOMAIN_WIFI);
                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            else if(IMSA_IMS_DOMAIN_LTE  ==  enFinalDomain)
            {
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING;
                IMSA_SRV_DomainEnable(IMSA_IMS_DOMAIN_LTE);
                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            else
            {
                /*没有找到可用的域，这时候，继续保持空闲状态，等待下次判决 */
            }

            IMSA_WARN_LOG("IMSA_ProcessDualDomainSelect: 1: enable one domain!");
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING:
            /* 初始IMS域选择判决算法 */
            enFinalDomain = IMSA_JudgeImsDomainC21();

            if(IMSA_IMS_DOMAIN_WIFI  == enFinalDomain)
            {
                /*记录切换事件，继续保持LTE Domain,等待LTE域完成尝试以后再次判决 */
                pstDomainManager->ucMsgDelayProcessFlag = VOS_TRUE;
            }
            else if(IMSA_IMS_DOMAIN_LTE  ==  enFinalDomain)
            {
                /* 继续保持当前尝试状态 */
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING;
                IMSA_SRV_DomainContinue(IMSA_IMS_DOMAIN_LTE);
                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            else
            {
                /*没有找到可用的域，这个时候报告异常 */
                IMSA_WARN_LOG("IMSA_ProcessDualDomainSelect: IMSA_ImsDomainC21Judge Wrong!");
                TLPS_PRINT2LAYER_WARNING(IMSA_ProcessDualDomainSelect_ENUM, IMSA_DOMAIN_STATUS_NULL);

            }

            IMSA_WARN_LOG("IMSA_ProcessDualDomainSelect: 2: no HO!");
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN:
            /* 切换判决算法 C2_2_2: L2W */
            enHoType = IMSA_JudgeImsDomainC222L2W();

            if( IMSA_IMS_HO_TYPE_LTE2WIFI == enHoType)
            {
                 /* 判断SRV状态是否允许切换 */
                 if(VOS_TRUE == IMSA_SRV_CheckSwitchPossible())
                  {
                     /* 启动L2W切换过程,指定切换失败操作,以及切换成功原域PDN处理方式 */
                     pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH;
                     IMSA_SRV_DomainSwitch(IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH, ucHoFailReturn2OrgPdnFlag, ucOrgPdnRlsFlag);
                     pstDomainManager->ucDomainSelectFlag = VOS_TRUE;

                     IMSA_WARN_LOG("IMSA_ProcessDualDomainSelect: 3: L2W HO!");
                 }
                 else
                 {
                     /*记录切换事件，等待SRV处于稳态再次判决 */
                     pstDomainManager->ucMsgDelayProcessFlag = VOS_TRUE;

                     IMSA_WARN_LOG("IMSA_ProcessDualDomainSelect: 3: wait for L2W HO!");
                 }
            }
            else if(IMSA_IMS_HO_TYPE_WIFI2LTE == enHoType)
            {
                /* 报告异常,不应该有这种返回值 */
                IMSA_WARN_LOG("IMSA_ProcessDualDomainSelect: IMSA_ImsDomainC222Judge Wrong!");
            }
            else
            {
                /* 没有切换,则保留在原有域,继续处理 */

                IMSA_WARN_LOG("IMSA_ProcessDualDomainSelect: 3:No L2W HO!");
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING:
            /* 初始IMS域选择判决算法 */
            enFinalDomain = IMSA_JudgeImsDomainC21();

            if(IMSA_IMS_DOMAIN_LTE  == enFinalDomain)
            {
                 /* 记录切换事件，继续保持WIFI Domain,等待WIFI域完成尝试以后再次判决 */
                 pstDomainManager->ucMsgDelayProcessFlag = VOS_TRUE;

                 IMSA_WARN_LOG("IMSA_ProcessDualDomainSelect: 4: wait L2W HO!");
            }
            else if(IMSA_IMS_DOMAIN_WIFI  ==  enFinalDomain)
            {
                //继续保持当前尝试状态；
                IMSA_WARN_LOG("IMSA_ProcessDualDomainSelect: 4: wait L2W HO!");
            }
            else
            {
               /*没有找到可用的域，这个时候报告异常 */
               IMSA_WARN_LOG("IMSA_ProcessDualDomainSelect: IMSA_ImsDomainC21Judge Wrong!");
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN:
            /* 切换判决算法 C2_2_1:W2L */
            enHoType = IMSA_JudgeImsDomainC221W2L();

            if( IMSA_IMS_HO_TYPE_WIFI2LTE == enHoType)
            {
                /* 判断SRV状态是否允许切换 */
                if(VOS_TRUE == IMSA_SRV_CheckSwitchPossible())
                {
                    /* 启动L2W切换过程,指定切换失败操作,以及切换成功原域PDN处理方式 */
                    pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH;
                    IMSA_SRV_DomainSwitch(IMSA_DOMAIN_SWITCH_TYPE_W2L_SWITCH, ucHoFailReturn2OrgPdnFlag, ucOrgPdnRlsFlag);
                    pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
                    IMSA_WARN_LOG("IMSA_ProcessDualDomainSelect: 5: W2L HO!");

                }
                else
                {
                    /*记录切换事件，等待SRV处于稳态再次判决 */
                    pstDomainManager->ucMsgDelayProcessFlag = VOS_TRUE;

                    IMSA_WARN_LOG("IMSA_ProcessDualDomainSelect: 5: wait for W2L HO!");
                }
            }
            else if(IMSA_IMS_HO_TYPE_LTE2WIFI == enHoType)
            {
                /* 报告异常,不应该有这种返回值 */
                IMSA_WARN_LOG("IMSA_ProcessDualDomainSelect: IMSA_ImsDomainC221Judge Wrong!");
            }
            else
            {
                /* 没有切换,则保留在原有域,继续处理 */
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH:
        case IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH:
            /* 不支持切换过程中的域选择过程: 无处理 */
            IMSA_WARN_LOG("IMSA_ProcessDualDomainSelect: 6:No HO!");
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING:
            /* 如果当前域处于Disable过程中,暂停任何域选操作;
                         记录域选事件，等待SRV处于稳态再次判决 */
            pstDomainManager->ucMsgDelayProcessFlag = 1;
            IMSA_WARN_LOG("IMSA_ProcessDualDomainSelect: 7:No HO!");
            break;

        default:
            /*  报告异常 */
            IMSA_WARN_LOG("IMSA_ProcessDualDomainSelect: DomainC2 FSM Status Wrong!");

            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessDualDomainSelect_ENUM, IMSA_STATE_ERROR);
    }

    return;
}




/*****************************************************************************
 Function Name  : IMSA_ProcessDualDomainSelectInUsimFileReady()
 Description    : USIM Card File Ready下的双候选域的处理
 Input          :
                      ucOrgPdnRlsFlag: 域切换是否释放原PDN：0 不释放，1 释放
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-07  Draft Enact
*****************************************************************************/
void IMSA_ProcessDualDomainSelectInUsimFileReady(VOS_UINT8 ucHoFailReturn2OrgPdnFlag, VOS_UINT8 ucOrgPdnRlsFlag)
{
    IMSA_DOMAIN_MANAGER_STRU              *pstDomainManager   = VOS_NULL_PTR;
    IMSA_IMS_DOMAIN_ENUM_UINT8             enFinalDomain;

    /* 初始化紧急连接 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_ProcessDualDomainSelectInUsimFileReady_ENUM, LNAS_ENTRY);

    switch(pstDomainManager->enDomainFsmL2State)
    {
        case IMSA_DOMAIN_FSM_L2_STATE_NULL:
            /* 初始IMS域选择判决算法 */
            enFinalDomain = IMSA_JudgeImsDomainC21();

            if(IMSA_IMS_DOMAIN_WIFI  == enFinalDomain)
            {
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING;
                IMSA_SRV_DomainEnable(IMSA_IMS_DOMAIN_WIFI);
                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            else if(IMSA_IMS_DOMAIN_LTE  ==  enFinalDomain)
            {
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING;
                IMSA_SRV_DomainEnable(IMSA_IMS_DOMAIN_LTE);
                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            else
            {
                /*没有找到可用的域，这时候，继续保持空闲状态，等待下次判决 */
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN:
        case IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH:
        case IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH:
        case IMSA_DOMAIN_FSM_L2_STATE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING:
            IMSA_WARN_LOG("IMSA_ProcessDualDomainSelectInUsimFileReady: Error Status in USIM Card File Ready!");
            break;

        default:
            /*  报告异常 */
            IMSA_WARN_LOG("IMSA_ProcessDualDomainSelectInUsimFileReady: DomainC2 FSM Status Wrong!");
    }

    return;
}

/*****************************************************************************
 Function Name  : IMSA_ProcessDualDomainSelectInLrrcStateInd()
 Description    : 收到Lrrc state ind消息的双候选域的处理
 Input          :
                      ucOrgPdnRlsFlag: 域切换是否释放原PDN：0 不释放，1 释放
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-07  Draft Enact
*****************************************************************************/
void IMSA_ProcessDualDomainSelectInLrrcStateInd(VOS_UINT8 ucHoFailReturn2OrgPdnFlag, VOS_UINT8 ucOrgPdnRlsFlag)
{
    IMSA_DOMAIN_MANAGER_STRU              *pstDomainManager   = VOS_NULL_PTR;
    /*IMSA_IMS_DOMAIN_ENUM_UINT8             enFinalDomain;*/
    IMSA_IMS_HO_TYPE_ENUM_UINT8            enHoType;

    /* 初始化紧急连接 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_ProcessDualDomainSelectInLrrcStateInd_ENUM, LNAS_ENTRY);

    switch(pstDomainManager->enDomainFsmL2State)
    {
        case IMSA_DOMAIN_FSM_L2_STATE_NULL:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING:
            /* LRRC不影响任何其他判决，仅仅处理双域切换 */
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN:
            /* 切换判决算法 C2_2_2: L2W */
            enHoType = IMSA_JudgeImsDomainC222L2W();

            if( IMSA_IMS_HO_TYPE_LTE2WIFI == enHoType)
            {
                 /* 判断SRV状态是否允许切换 */
                 if(VOS_TRUE == IMSA_SRV_CheckSwitchPossible())
                  {
                     /* 启动L2W切换过程,指定切换失败操作,以及切换成功原域PDN处理方式 */
                     pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH;
                     IMSA_SRV_DomainSwitch(IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH, ucHoFailReturn2OrgPdnFlag, ucOrgPdnRlsFlag);
                     pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
                 }
                 else
                 {
                     /*记录切换事件，等待SRV处于稳态再次判决 */
                     pstDomainManager->ucMsgDelayProcessFlag = 1;
                 }
            }
            else if(IMSA_IMS_HO_TYPE_WIFI2LTE == enHoType)
            {
                /* 报告异常,不应该有这种返回值 */
                IMSA_WARN_LOG("IMSA_ProcessDualDomainSelectInLrrcStateInd: IMSA_ImsDomainC222Judge Wrong!");
            }
            else
            {
                /* 没有切换,则保留在原有域,继续处理 */
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING:
            /* LRRC不影响任何其他判决，仅仅处理双域切换 */
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN:
            /* 切换判决算法 C2_2_1:W2L */
            enHoType = IMSA_JudgeImsDomainC221W2L();

            if( IMSA_IMS_HO_TYPE_WIFI2LTE == enHoType)
            {
                /* 判断SRV状态是否允许切换 */
                if(VOS_TRUE == IMSA_SRV_CheckSwitchPossible())
                {
                    /* 启动L2W切换过程,指定切换失败操作,以及切换成功原域PDN处理方式 */
                    pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH;
                    IMSA_SRV_DomainSwitch(IMSA_DOMAIN_SWITCH_TYPE_W2L_SWITCH, ucHoFailReturn2OrgPdnFlag, ucOrgPdnRlsFlag);
                    pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
                }
                else
                {
                    /*记录切换事件，等待SRV处于稳态再次判决 */
                    pstDomainManager->ucMsgDelayProcessFlag = 1;
                }
            }
            else if(IMSA_IMS_HO_TYPE_LTE2WIFI == enHoType)
            {
                /* 报告异常,不应该有这种返回值 */
                IMSA_WARN_LOG("IMSA_ProcessDualDomainSelectInLrrcStateInd: IMSA_ImsDomainC221Judge Wrong!");
            }
            else
            {
                /* 没有切换,则保留在原有域,继续处理 */
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH:
        case IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH:
            /* 不支持切换过程中的域选择过程: 无处理 */
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING:
            /* 如果当前域处于Disable过程中,暂停任何域选操作;*/
            break;

        default:
            /*  报告异常 */
            IMSA_WARN_LOG("IMSA_ProcessDualDomainSelectInLrrcStateInd: DomainC2 FSM Status Wrong!");
    }

    return;
}

/*****************************************************************************
 Function Name  : IMSA_ProcessDualDomainSelectInWifiErrorInd()
 Description    : 收到Wifi_Error_Ind消息的双候选域的处理
 Input          :
                      ucOrgPdnRlsFlag: 域切换是否释放原PDN：0 不释放，1 释放
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-07  Draft Enact
*****************************************************************************/
void IMSA_ProcessDualDomainSelectInWifiErrorInd(VOS_UINT8 ucHoFailReturn2OrgPdnFlag, VOS_UINT8 ucOrgPdnRlsFlag)
{
    IMSA_DOMAIN_MANAGER_STRU              *pstDomainManager   = VOS_NULL_PTR;
    /*IMSA_IMS_DOMAIN_ENUM_UINT8             enFinalDomain;*/
    IMSA_IMS_HO_TYPE_ENUM_UINT8           enHoType;

    /* 初始化紧急连接 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_ProcessDualDomainSelectInWifiErrorInd_ENUM, LNAS_ENTRY);


    switch(pstDomainManager->enDomainFsmL2State)
    {
        case IMSA_DOMAIN_FSM_L2_STATE_NULL:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING:
            /* Wifi_Error_Ind不影响任何其他判决，仅仅处理双域切换 */
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN:
            /* 切换判决算法 C2_2_1:W2L */
            enHoType = IMSA_JudgeImsDomainC221W2L();

            if( IMSA_IMS_HO_TYPE_WIFI2LTE == enHoType)
            {
                /* 启动L2W切换过程,指定切换失败操作,以及切换成功原域PDN处理方式 */
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH;
                IMSA_SRV_DomainSwitch(IMSA_DOMAIN_SWITCH_TYPE_W2L_SWITCH, ucHoFailReturn2OrgPdnFlag, ucOrgPdnRlsFlag);
                pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
            }
            else if(IMSA_IMS_HO_TYPE_LTE2WIFI == enHoType)
            {
                /* 报告异常,不应该有这种返回值 */
                IMSA_WARN_LOG("IMSA_ProcessDualDomainSelectInWifiErrorInd: IMSA_ImsDomainC221Judge Wrong!");

                TLPS_PRINT2LAYER_WARNING(IMSA_ProcessDualDomainSelectInWifiErrorInd_ENUM, IMSA_STATE_ERROR);
            }
            else
            {
                /* 没有切换,则保留在原有域,继续处理 */
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH:
        case IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH:
            /* 不支持切换过程中的域选择过程: 无处理 */
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING:
            /* 如果当前域处于Disable过程中,暂停任何域选操作;*/
            break;

        default:
            /*  报告异常 */
            IMSA_WARN_LOG("IMSA_ProcessDualDomainSelectInWifiErrorInd: DomainC2 FSM Status Wrong!");
            TLPS_PRINT2LAYER_WARNING(IMSA_ProcessDualDomainSelectInWifiErrorInd_ENUM, LNAS_StateInvalid);
    }

    return;
}

/*****************************************************************************
 Function Name  : IMSA_ProcessDualDomainSelectInBearerRelInd()
 Description    : 收到Bearer_rel_ind消息的双候选域的处理
 Input          :
                      ucOrgPdnRlsFlag: 域切换是否释放原PDN：0 不释放，1 释放
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-25  Draft Enact
*****************************************************************************/
void IMSA_ProcessDualDomainSelectInBearerRelInd(VOS_UINT8 ucHoFailReturn2OrgPdnFlag, VOS_UINT8 ucOrgPdnRlsFlag)
{
    IMSA_DOMAIN_MANAGER_STRU              *pstDomainManager   = VOS_NULL_PTR;
    /*IMSA_IMS_DOMAIN_ENUM_UINT8             enFinalDomain;*/
    IMSA_IMS_HO_TYPE_ENUM_UINT8           enHoType;

    /* 初始化紧急连接 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_ProcessDualDomainSelectInBearerRelInd_ENUM, LNAS_ENTRY);

    switch(pstDomainManager->enDomainFsmL2State)
    {
        case IMSA_DOMAIN_FSM_L2_STATE_NULL:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING:
            /* 承载异常不影响任何其他判决，仅仅处理双域切换 */
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN:
            /* 切换判决算法 C2_2_2: L2W */
            enHoType = IMSA_JudgeImsDomainC222L2W();

            if( IMSA_IMS_HO_TYPE_LTE2WIFI == enHoType)
            {
                 /* 判断SRV状态是否允许切换 */
                 if(VOS_TRUE == IMSA_SRV_CheckSwitchPossible())
                 {
                    /* LTE域已经尝试过,但是失败,需要换域尝试 */
                     IMSA_Domain_SetLteEnable(pstDomainManager->enImsaTryedDomain);

                     /* 启动L2W切换过程,指定切换失败操作,以及切换成功原域PDN处理方式 */
                     pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH;
                     IMSA_SRV_DomainSwitch(IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH, ucHoFailReturn2OrgPdnFlag, ucOrgPdnRlsFlag);
                     pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
                 }
            }
            else if(IMSA_IMS_HO_TYPE_WIFI2LTE == enHoType)
            {
                /* 报告异常,不应该有这种返回值 */
                IMSA_WARN_LOG("IMSA_ProcessDualDomainSelectInBearerRelInd: IMSA_ImsDomainC222Judge Wrong!");
            }
            else
            {
                /* 没有切换,则保留在原有域,继续处理 */
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING:
            /* 承载异常不影响任何其他判决，仅仅处理双域切换 */
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN:
            /* 切换判决算法 C2_2_1:W2L */
            enHoType = IMSA_JudgeImsDomainC221W2L();

            if( IMSA_IMS_HO_TYPE_WIFI2LTE == enHoType)
            {
                /* 判断SRV状态是否允许切换 */
                if(VOS_TRUE == IMSA_SRV_CheckSwitchPossible())
                {
                    /* WIFI域已经尝试过,但是失败,需要换域尝试 */
                     IMSA_Domain_SetWifiEnable(pstDomainManager->enImsaTryedDomain);

                    /* 启动L2W切换过程,指定切换失败操作,以及切换成功原域PDN处理方式 */
                    pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH;
                    IMSA_SRV_DomainSwitch(IMSA_DOMAIN_SWITCH_TYPE_W2L_SWITCH, ucHoFailReturn2OrgPdnFlag, ucOrgPdnRlsFlag);
                    pstDomainManager->ucDomainSelectFlag = VOS_TRUE;
                }
            }
            else if(IMSA_IMS_HO_TYPE_LTE2WIFI == enHoType)
            {
                /* 报告异常,不应该有这种返回值 */
                IMSA_WARN_LOG("IMSA_ProcessDualDomainSelectInBearerRelInd: IMSA_ImsDomainC221Judge Wrong!");
            }
            else
            {
                /* 没有切换,则保留在原有域,继续处理 */
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH:
        case IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH:
            /* 不支持切换过程中的域选择过程: 无处理 */
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING:
            /* 如果当前域处于Disable过程中,暂停任何域选操作;*/
            break;

        default:
            /*  报告异常 */
            IMSA_WARN_LOG("IMSA_ProcessDualDomainSelectInBearerRelInd: DomainC2 FSM Status Wrong!");
    }

    return;
}



/*****************************************************************************
 Function Name  : IMSA_DomainSelectResultInNoSrv()
 Description    : 处理NO SERVICE的Status IND的域选处理结果
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-15  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_DomainSelect(VOS_VOID)
{

    TLPS_PRINT2LAYER_INFO(IMSA_DomainSelect_ENUM, LNAS_ENTRY);

    /* 调用域选函数,根据当前状态选择合适的域 */
    switch(IMSA_JudgeImsDomainC1())
    {
        case IMSA_IMS_DOMAIN_NULL:
             IMSA_ProcessNoDomainSelect(VOS_TRUE);
             break;

        case IMSA_IMS_DOMAIN_WIFI:
             IMSA_ProcessWifiOnlyDomainSelect(VOS_TRUE);
             break;

        case IMSA_IMS_DOMAIN_LTE:
             IMSA_ProcessLTEOnlyDomainSelect(VOS_TRUE);
             break;

        case IMSA_IMS_DOMAIN_LTE_WIFI:
             IMSA_ProcessDualDomainSelect(VOS_TRUE,VOS_TRUE);
             break;

        default:
            IMSA_WARN_LOG("IMSA_DomainSelect: DomainC1Judge Wrong!");
    }

}

/*****************************************************************************
 Function Name  : IMSA_Domain_DomainSelectInWifiStateInd()
 Description    : WifiStateInd的域选处理结果
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-15  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_Domain_DomainSelectInWifiStateInd()
{
    IMSA_DOMAIN_MANAGER_STRU            *pstDomainManager = VOS_NULL;

    pstDomainManager = IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_Domain_DomainSelectInWifiStateInd_ENUM, LNAS_ENTRY);


    /* 设置域选初始化函数处理 */
    IMSA_Domain_InitDomainOpVar(WUEPS_PID_MAPCON,ID_WIFI_IMSA_STATE_IND,VOS_TRUE,VOS_TRUE);

    /* 调用域选函数,根据当前状态选择合适的域 */
    switch(IMSA_JudgeImsDomainC1())
    {
        case IMSA_IMS_DOMAIN_NULL:
            IMSA_WARN_LOG("IMSA_Domain_DomainSelectInWifiStateInd: No Process in This State");
            TLPS_PRINT2LAYER_WARNING(IMSA_Domain_DomainSelectInWifiStateInd_ENUM, IMSA_STATE_ERROR);
            break;

        case IMSA_IMS_DOMAIN_WIFI:
            IMSA_ProcessWifiOnlyDomainSelectInWifiStateInd(VOS_TRUE);
            break;

        case IMSA_IMS_DOMAIN_LTE:
            IMSA_ProcessLTEOnlyDomainSelectInWifiStateInd(VOS_TRUE);
            break;

        case IMSA_IMS_DOMAIN_LTE_WIFI:
            IMSA_ProcessDualDomainSelect(VOS_TRUE,VOS_TRUE);
            break;

        default:
            IMSA_WARN_LOG("IMSA_Domain_DomainSelectInWifiStateInd: DomainC1Judge Wrong!");
            TLPS_PRINT2LAYER_WARNING(IMSA_Domain_DomainSelectInWifiStateInd_ENUM, LNAS_StateInvalid);
    }

    /* 如果需要延迟处理消息,记录事件,等待SRV上报状态,然后重新启动域选过程 */
    if(VOS_TRUE == pstDomainManager->ucMsgDelayProcessFlag)
    {
       /* 保存当前需要延迟处理的域选择事件 */
       IMSA_Domain_SaveDelaySwitchEvent(WUEPS_PID_MAPCON,ID_WIFI_IMSA_STATE_IND);

       /* 向SRV模块注册状态上报事件, SRV处于稳态将上报Ready_ind消息,发起新的域选择事件 */
       IMSA_SRV_RegStatusNtfEvent();

       /* 清除Delay事件 */
       pstDomainManager->ucMsgDelayProcessFlag = VOS_FALSE;
    }
    else
    {
    }

    return;
}


/*****************************************************************************
 Function Name  : IMSA_DomainSelectResultInNoSrv()
 Description    : 处理NO SERVICE的Status IND的域选处理结果
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-15  Draft Enact
*****************************************************************************/


VOS_VOID IMSA_DomainSelectResultInNoSrv(VOS_VOID)
{
    IMSA_DOMAIN_MANAGER_STRU            *pstDomainManager = VOS_NULL;

    pstDomainManager = IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_DomainSelectResultInNoSrv_ENUM, LNAS_ENTRY);

    /* 如果Domain FSM L2处于正在尝试一个IMS域的状态，则回复到空闲态 */
    switch( pstDomainManager->enDomainFsmL2State)
    {
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN:
            /*　如果当前网络模式支持双域 */
            if( IMSA_IMS_DOMAIN_MODE_DUAL == IMSA_JudgeImsDomainC1())
            {
                /* LTE域已经尝试过,但是失败,需要换域尝试 */
                IMSA_Domain_SetLteEnable(pstDomainManager->enImsaTryedDomain);
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_NULL;

                /* 启动LTE域惩罚定时器 */
                /* lint -e734*/
                pstDomainManager->ucPunishTimeInLteDomain = (VOS_UINT8)(pstDomainManager->ucPunishTimeInLteDomain * 2);
                IMSA_Domain_StartTimer1LteDomain();
                /* lint +e734*/

                /* 如果两个域都尝试过，则暂停尝试，等待下次触发 */
                if( IMSA_IMS_DOMAIN_LTE_WIFI == pstDomainManager->enImsaTryedDomain )
                {
                    pstDomainManager->enImsaTryedDomain = IMSA_IMS_DOMAIN_NULL;

#if 0
                    /*上报语音能力 */
                    if (IMSA_TRUE == IMSA_IsImsVoiceContidionSatisfied())
                    {
                        IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_AVAILABLE,IMSA_CONN_IsExitPersistentBearer());
                    }
                    else
                    {
                        IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE,IMSA_CONN_IsExitPersistentBearer());
                    }
#endif
                }
                else
                {
                    /* 调用域选函数,根据当前状态再次选择合适的域 */
                    IMSA_DomainSelect();
                }
            }
            else
            {
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_NULL;
#if 0
                /*上报语音能力 */
                if (IMSA_TRUE == IMSA_IsImsVoiceContidionSatisfied())
                {
                    IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_AVAILABLE,IMSA_CONN_IsExitPersistentBearer());
                }
                else
                {
                    IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE,IMSA_CONN_IsExitPersistentBearer());
                }
#endif
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN:
            /*　如果当前网络模式支持双域，惩罚当前域 */
            if( IMSA_IMS_DOMAIN_MODE_DUAL == IMSA_JudgeImsDomainC1())
            {
                /* WIFI域已经尝试过,但是失败,需要换域尝试 */
                IMSA_Domain_SetWifiEnable(pstDomainManager->enImsaTryedDomain);
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_NULL;

                /* 启动WIFI域惩罚定时器 */
                /*lint -e734*/
                pstDomainManager->ucPunishTimeInWifiDomain = pstDomainManager->ucPunishTimeInWifiDomain * 2;
                IMSA_Domain_StartTimer1WifiDomain();
                /*lint +e734*/

                /* 如果两个域都尝试过，则暂停尝试，等待下次触发 */
                if( IMSA_IMS_DOMAIN_LTE_WIFI == pstDomainManager->enImsaTryedDomain )
                {
                    pstDomainManager->enImsaTryedDomain = IMSA_IMS_DOMAIN_NULL;
#if 0
                    /*上报语音能力 */
                    if (IMSA_TRUE == IMSA_IsImsVoiceContidionSatisfied())
                    {
                        IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_AVAILABLE,IMSA_CONN_IsExitPersistentBearer());
                    }
                    else
                    {
                        IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE,IMSA_CONN_IsExitPersistentBearer());
                    }
#endif
                }
                else
                {
                    /* 调用域选函数,根据当前状态再次选择合适的域 */
                    IMSA_DomainSelect();
                }

            }
            else
            {
                pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_NULL;
#if 0
                /*上报语音能力 */
                if (IMSA_TRUE == IMSA_IsImsVoiceContidionSatisfied())
                {
                    IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_AVAILABLE,IMSA_CONN_IsExitPersistentBearer());
                }
                else
                {
                    IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE,IMSA_CONN_IsExitPersistentBearer());
                }
#endif
            }
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH:
            /* 切换失败,无法回到原来域,上报失败,无服务 */
            pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_NULL;
            pstDomainManager->enImsaTryedDomain = IMSA_IMS_DOMAIN_NULL;

            break;

        case IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH:
            /* 切换失败,无法回到原来域,上报失败,无服务 */
            pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_NULL;
            pstDomainManager->enImsaTryedDomain = IMSA_IMS_DOMAIN_NULL;

            break;

        case IMSA_DOMAIN_FSM_L2_STATE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING:
            /* 域选发起Disable域成功 */
            pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_NULL;
#if 0
            /*上报语音能力 */
            if (IMSA_TRUE == IMSA_IsImsVoiceContidionSatisfied())
            {
                IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_AVAILABLE,IMSA_CONN_IsExitPersistentBearer());
            }
            else
            {
                IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE,IMSA_CONN_IsExitPersistentBearer());
            }
#endif
            break;

        default:
            pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_NULL;
            IMSA_WARN_LOG("IMSA_DomainSelectResultInNoSrv: No Srv Status is Wrong!");
            TLPS_PRINT2LAYER_WARNING(IMSA_DomainSelectResultInNoSrv_ENUM, LNAS_StateInvalid);
            break;
    }

    return;
}

/*****************************************************************************
 Function Name  : IMSA_DomainSelectResultInNrmSrv()
 Description    : 处理Normal SERVICE的Status IND的域选处理结果
 Input          : IMSA_DOMAIN_SRV_STATUS_IND_STRU     *pstSrvStatusInd
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-12-15  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_DomainSelectResultInNrmSrv(IMSA_DOMAIN_SRV_STATUS_IND_STRU     *pstSrvStatusInd)
{
    IMSA_DOMAIN_MANAGER_STRU            *pstDomainManager = VOS_NULL;

    pstDomainManager = IMSA_Domain_GetDomainManagerAddr();

    TLPS_PRINT2LAYER_INFO(IMSA_DomainSelectResultInNrmSrv_ENUM, LNAS_ENTRY);

    /* 如果Domain FSM L2处于正在尝试LTE IMS域的状态，则转到LTE Domain态 */
    if( (IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING == pstDomainManager->enDomainFsmL2State)
        && (IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY == pstSrvStatusInd->enCurrentDomain))
    {
        pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN;

        /*初始化LTE惩罚定时器惩罚时间 */
        IMSA_Domain_StopTimer1LteDomain();
    }
    /* 如果Domain FSM L2处于正在尝试WIFI IMS域的状态，则转到WIFI Domain态 */
    else if((IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING == pstDomainManager->enDomainFsmL2State)
        &&(IMSA_IMS_DOMAIN_MODE_WIFI_ONLY == pstSrvStatusInd->enCurrentDomain))
    {
        pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN;

        /*初始化WIFI惩罚定时器惩罚时间 */
        IMSA_Domain_StopTimer1WifiDomain();
    }
    /* 如果 L2W切换成功,则当前处于WIFI域 */
    else if((IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH == pstDomainManager->enDomainFsmL2State)
        &&(IMSA_IMS_DOMAIN_MODE_WIFI_ONLY == pstSrvStatusInd->enCurrentDomain))
    {
        pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN;

        /*初始化WIFI惩罚定时器惩罚时间 */
        IMSA_Domain_StopTimer1WifiDomain();
    }
    /* 如果 L2W切换失败, 但是成功返回原来的LTE域 */
    else if((IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH == pstDomainManager->enDomainFsmL2State)
        &&(IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY == pstSrvStatusInd->enCurrentDomain))
    {
        pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN;
    }
    /* 如果 W2L切换成功,则当前处于LTE域 */
    else if((IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH == pstDomainManager->enDomainFsmL2State)
        &&(IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY == pstSrvStatusInd->enCurrentDomain))
    {
        pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN;

        /*初始化LTE惩罚定时器惩罚时间 */
        IMSA_Domain_StopTimer1LteDomain();
    }
    /* 如果 W2L切换失败, 但是成功返回原来的WIFI域 */
    else if((IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH == pstDomainManager->enDomainFsmL2State)
        &&(IMSA_IMS_DOMAIN_MODE_WIFI_ONLY == pstSrvStatusInd->enCurrentDomain))
    {
        pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN;
    }
    /* L2状态机处于NULL和Disabling状态时,收到状态上报属于异常,转到NULL状态,同时发起域选过程 */
    else if((IMSA_DOMAIN_FSM_L2_STATE_NULL == pstDomainManager->enDomainFsmL2State)
        || (IMSA_DOMAIN_FSM_L2_STATE_DISABLING == pstDomainManager->enDomainFsmL2State)
        || (IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING == pstDomainManager->enDomainFsmL2State)
        || (IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING == pstDomainManager->enDomainFsmL2State))
    {
        pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_NULL;

        /* 调用域选函数,根据当前状态再次选择合适的域 */
        IMSA_DomainSelect();
    }
    else
    {
        IMSA_WARN_LOG("IMSA_DomainSelectResultInNrmSrv: Srv Status is Wrong!");
    }

#if 0
/* 注册状态下的IMS VOPS上报，等待确认清楚再次更新 */
    if( ( IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN == pstDomainManager->enDomainFsmL2State)
        ||(IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN == pstDomainManager->enDomainFsmL2State))
    {
        /*上报语音能力 */
        if (IMSA_TRUE == IMSA_IsImsVoiceContidionSatisfied())
        {
            IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_AVAILABLE,IMSA_CONN_IsExitPersistentBearer());
        }
        else
        {
            IMSA_SndMsccMsgImsVoiceCapNotify(MSCC_IMSA_IMS_VOICE_CAP_UNAVAILABLE,IMSA_CONN_IsExitPersistentBearer());
        }
    }
#endif
    return;
}



VOS_UINT32 IMSA_Domain_CheckWifiPossibleInVolteCtrlVowifi(VOS_VOID)
{
    IMSA_DOMAIN_MANAGER_STRU    *pstImsaDomainManager = VOS_NULL_PTR;

    pstImsaDomainManager =  IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_Domain_CheckWifiPossibleInVolteCtrlVowifi_ENUM, LNAS_ENTRY);

    /* 域选择处理过程:
        1>  如果VoWIFI_SERVICE_STATUS为INITIAL
          域选择尝试VoWIFI；
        1> 如果VoWIFI_SERVICE_STATUS为LTE_NORMAL
           域选择尝试VoWIFI
        1> 如果VoWIFI_SERVICE_STATUS为LTE_LIMITED
           域选择不尝试VoWIFI，已经驻留VoWIFI，进行IMS去注册，释放VoWIFI PDN；
      */
    if( (IMSA_IMS_VOLTECTRLVOWIFI_INIT == pstImsaDomainManager->enImsaVolteCtrlVowifiStatus)
        || (IMSA_IMS_VOLTECTRLVOWIFI_NORMAL == pstImsaDomainManager->enImsaVolteCtrlVowifiStatus))
    {
        return VOS_TRUE;
    }
    else if( IMSA_IMS_VOLTECTRLVOWIFI_LIMITED == pstImsaDomainManager->enImsaVolteCtrlVowifiStatus)
    {
        return VOS_FALSE;
    }
    else
    {
        IMSA_WARN_LOG("IMSA_Domain_CheckWifiPossibleInVolteCtrlVowifi: VoLTECtrlVoWIFI ENUM Wrong!");

        TLPS_PRINT2LAYER_WARNING(IMSA_Domain_CheckWifiPossibleInVolteCtrlVowifi_ENUM, LNAS_StateInvalid);
        return VOS_TRUE;
    }
}


VOS_VOID IMSA_Domain_SetVolteCtrlVowifi
(
    const MSCC_IMSA_SERVICE_CHANGE_IND_STRU     *pstServiceChangeInd
)
{
    IMSA_DOMAIN_MANAGER_STRU    *pstImsaDomainManager = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_Domain_SetVolteCtrlVowifi is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_Domain_SetVolteCtrlVowifi_ENUM, LNAS_ENTRY);

    /* 存储SERVICE CHANGE IND消息中携带的参数 */
    pstImsaDomainManager                    = IMSA_Domain_GetDomainManagerAddr();
    /* 漫游状态下, VoWIFI和VoLTE间不进行处理关联 */
    if ((VOS_TRUE == pstServiceChangeInd->bitRoamingValid)
         && (VOS_TRUE == pstServiceChangeInd->ucRoamingFlg))
    {
        pstImsaDomainManager->enImsaVolteCtrlVowifiStatus = IMSA_IMS_VOLTECTRLVOWIFI_INIT;
        return;
    }

    /* 如果选项"不支持VoLTE的网络不启动VoWifi"为TRUE, 则根据指示网络IMS能力配置参数 */
    if(VOS_TRUE == IMSA_GetVolteCtrlVoWifiFlag())
    {
        if(IMSA_IMS_VOPS_STATUS_NOT_SUPPORT == IMSA_GetNwImsVoiceCap())
        {
            pstImsaDomainManager->enImsaVolteCtrlVowifiStatus = IMSA_IMS_VOLTECTRLVOWIFI_LIMITED;

            IMSA_INFO_LOG("IMSA_Domain_SetVolteCtrlVowifi:Limited!");
        }
        /* 如果驻留LTE小区支持为IMS */
        else
        {
            /* 设置VoWIFI_SERVICE_STATUS为LTE_NORMAL */
            pstImsaDomainManager->enImsaVolteCtrlVowifiStatus = IMSA_IMS_VOLTECTRLVOWIFI_NORMAL;

            IMSA_INFO_LOG("IMSA_Domain_SetVolteCtrlVowifi:Normal!");
        }
    }
    else
    {
        /*  如果选项"不支持VoLTE的网络不启动VoWifi"为FALSE, 设置VoWIFI_SERVICE_STATUS为INITIAL */
        pstImsaDomainManager->enImsaVolteCtrlVowifiStatus = IMSA_IMS_VOLTECTRLVOWIFI_INIT;

        IMSA_INFO_LOG("IMSA_Domain_SetVolteCtrlVowifi: Init!");
    }

    return;
}

/*****************************************************************************
 Function Name  : IMSA_Domain_SetImsDomainPreferFromSetMsg()
 Description    : 从MSCC IMS优先域消息进行优先域设置
 Input          : 指定优先顺序 enImsDomain
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-10-19  Draft Enact
*****************************************************************************/


VOS_VOID IMSA_Domain_SetImsDomainPreferFromSetMsg( MSCC_IMSA_IMS_DOMAIN_ENUM_UINT32    enImsDomain )
{

    IMSA_DOMAIN_MANAGER_STRU    *pstImsaDomainManager = VOS_NULL_PTR;

    pstImsaDomainManager =  IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_Domain_SetImsDomainPreferFromSetMsg_ENUM, LNAS_ENTRY);

    switch(enImsDomain)
    {
        case MSCC_IMSA_IMS_DOMAIN_WIFI_PREFEER:
            pstImsaDomainManager->enImsDomainPrefer = IMSA_IMS_DOMAIN_PREFER_WIFI_PREFER;
            break;

        case MSCC_IMSA_IMS_DOMAIN_CELLULAR_PREFEER:
            pstImsaDomainManager->enImsDomainPrefer = IMSA_IMS_DOMAIN_PREFER_CELLULAR_PREFER;
            break;

        case MSCC_IMSA_IMS_DOMAIN_WIFI_ONLY:
            pstImsaDomainManager->enImsDomainPrefer = IMSA_IMS_DOMAIN_PREFER_WIFI_ONLY;
            break;

        default:
            pstImsaDomainManager->enImsDomainPrefer = IMSA_IMS_DOMAIN_PREFER_CELLULAR_ONLY;
            IMSA_WARN_LOG("IMSA_Domain_SetImsDomainPreferFromSetMsg: Domain Prefer Set Para is Wrong!");
            break;
    }

    return;
}


/*****************************************************************************
 Function Name  : IMSA_Domain_SetDomainModeInWifiPowerOn()
 Description    : WIFI开机处理状态迁移
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_UINT32  VOS_TRUE/VOS_FALSE

 History        :
      1.Zhangdongfeng 353461        2015-11-19  Draft Enact
*****************************************************************************/
VOS_UINT32 IMSA_Domain_SetDomainModeInMsccImsDomainCfgSetReq(VOS_VOID)
{
    IMSA_DOMAIN_MANAGER_STRU           *pstImsaDomainManager    = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_Domain_SetDomainModeInMsccImsDomainCfgSetReq: ENTER!");
    TLPS_PRINT2LAYER_INFO(IMSA_Domain_SetDomainModeInMsccImsDomainCfgSetReq_ENUM, LNAS_ENTRY);


    pstImsaDomainManager = IMSA_Domain_GetDomainManagerAddr();

    /* 根据IMS优先域配置当前的IMS域状态 */
    switch(pstImsaDomainManager->enImsDomainPrefer)
    {
        case IMSA_IMS_DOMAIN_PREFER_WIFI_ONLY:
            if( IMSA_IMS_DOMAIN_MODE_DUAL == IMSA_GetDomainMode())
            {
                IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_WIFI_ONLY);
            }
            else if( VOS_TRUE == IMSA_Domain_IsWifiDomainExist())
            {
                IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_WIFI_ONLY);
            }
            else
            {
                IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_NULL);
            }
            break;

        case IMSA_IMS_DOMAIN_PREFER_CELLULAR_PREFER:
        case IMSA_IMS_DOMAIN_PREFER_WIFI_PREFER:
            if(VOS_TRUE == IMSA_Domain_IsDualDomainExist())
            {
                IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_DUAL);
            }
            else if(VOS_TRUE == IMSA_Domain_IsWifiDomainExist() )
            {
                IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_WIFI_ONLY);
            }
            else if( VOS_TRUE == IMSA_Domain_IsCellularDomainExist())
            {
                IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY);
            }
            else
            {
                IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_NULL);
            }
            break;

        case IMSA_IMS_DOMAIN_PREFER_CELLULAR_ONLY:
        default:
            IMSA_WARN_LOG("IMSA_Domain_SetImsDomainPreferFromSetMsg: Prefer Set Status Wrong!");

            TLPS_PRINT2LAYER_WARNING(IMSA_Domain_SetImsDomainPreferFromSetMsg_ENUM, LNAS_StateInvalid);
    }

    return IMSA_SUCC;
}




/*****************************************************************************
 Function Name  : IMSA_Domain_SetDomainModeInWifiPowerOn()
 Description    : WIFI开机处理状态迁移
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_UINT32  VOS_TRUE/VOS_FALSE

 History        :
      1.Zhangdongfeng 353461        2015-11-19  Draft Enact
*****************************************************************************/
VOS_UINT32 IMSA_Domain_SetDomainModeInWifiPowerOn(VOS_VOID)
{
    IMSA_CONTROL_MANAGER_STRU    *pstControlManager;

    IMSA_INFO_LOG("IMSA_Domain_SetDomainModeInWifiPowerOn: ENTER!");
    TLPS_PRINT2LAYER_INFO(IMSA_Domain_SetDomainModeInWifiPowerOn_ENUM, LNAS_ENTRY);

    pstControlManager = IMSA_GetControlManagerAddress();

    /* 设置WIFI开机状态 */
    if( IMSA_STATUS_STARTING == pstControlManager->enImsaWifiStatus)
    {
        IMSA_WARN_LOG("IMSA_Domain_SetDomainModeInWifiPowerOn: Status is Starting!");
        return VOS_FALSE;
    }


    /* 域管理状态机迁移 */
    switch(IMSA_GetDomainMode())
    {
        case IMSA_IMS_DOMAIN_MODE_NULL:
            /*  设置域管理模块状态机迁移  */
            if( ((IMSA_IMS_DOMAIN_PREFER_CELLULAR_PREFER == IMSA_GetImsDomainPrefer())
                  ||(IMSA_IMS_DOMAIN_PREFER_WIFI_PREFER == IMSA_GetImsDomainPrefer()))
                && (VOS_TRUE == IMSA_Domain_IsDualDomainExist()))
            {
                /* 如果NV和开机状态支持双域,且IMS优先关系支持双域, 设置IMSA域状态为 Dual Domain */
                IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_DUAL);
                IMSA_INFO_LOG("IMSA_Domain_SetDomainModeInWifiPowerOn: 1_DomainMode is IMSA_IMS_DOMAIN_MODE_DUAL!");
            }
            else if( ((IMSA_IMS_DOMAIN_PREFER_WIFI_PREFER == IMSA_GetImsDomainPrefer())
                      || (IMSA_IMS_DOMAIN_PREFER_CELLULAR_PREFER == IMSA_GetImsDomainPrefer())
                      || (IMSA_IMS_DOMAIN_PREFER_WIFI_ONLY == IMSA_GetImsDomainPrefer()))
                     &&(VOS_TRUE == IMSA_Domain_IsWifiDomainExist()))
            {
                /* 如果NV和开机状态支持WIFI域,且IMS优先关系支持WIFI域,设置IMSA域状态为 WIFI ONLY */
                IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_WIFI_ONLY);
                IMSA_INFO_LOG("IMSA_Domain_SetDomainModeInWifiPowerOn: 2_DomainMode is IMSA_IMS_DOMAIN_MODE_WIFI_ONLY!");
            }
            else if( ((IMSA_IMS_DOMAIN_PREFER_CELLULAR_PREFER == IMSA_GetImsDomainPrefer())
                       || (IMSA_IMS_DOMAIN_PREFER_WIFI_PREFER == IMSA_GetImsDomainPrefer())
                       || (IMSA_IMS_DOMAIN_PREFER_CELLULAR_ONLY == IMSA_GetImsDomainPrefer()))
                     &&(VOS_TRUE == IMSA_Domain_IsCellularDomainExist()))
            {
                /* 如果NV和开机状态支持Cellular域,且IMS优先关系支持Cellular域,设置IMSA域状态为 Cellular ONLY */
                IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY);
                IMSA_INFO_LOG("IMSA_Domain_SetDomainModeInWifiPowerOn: 3_DomainMode is IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY!");
            }
            else
            {
                /* 设置IMSA域状态为 NULL */
                IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_NULL);
                IMSA_INFO_LOG("IMSA_Domain_SetDomainModeInWifiPowerOn: 4_DomainMode is IMSA_IMS_DOMAIN_MODE_NULL!");
            }
            break;

        case IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY:
            /*  设置域管理模块状态机迁移  */
            if( ((IMSA_IMS_DOMAIN_PREFER_CELLULAR_PREFER == IMSA_GetImsDomainPrefer())
                  ||(IMSA_IMS_DOMAIN_PREFER_WIFI_PREFER == IMSA_GetImsDomainPrefer()))
                && (VOS_TRUE == IMSA_Domain_IsDualDomainExist()))
            {
                /* 如果NV和开机状态支持双域,且IMS优先关系支持双域, 设置IMSA域状态为 Dual Domain */
                IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_DUAL);
                IMSA_INFO_LOG("IMSA_Domain_SetDomainModeInWifiPowerOn: 5_DomainMode is IMSA_IMS_DOMAIN_MODE_DUAL!");
            }
            break;

        case IMSA_IMS_DOMAIN_MODE_WIFI_ONLY:
            /* Wifi开机状态收到Wifi开机消息，告警 */
            IMSA_WARN_LOG("IMSA_Domain_SetDomainModeInWifiPowerOn: Multi Wifi Power On Message Received!");

            /* 如果双域支持,迁移到双域状态 */
            if( ((IMSA_IMS_DOMAIN_PREFER_CELLULAR_PREFER == IMSA_GetImsDomainPrefer())
                  ||(IMSA_IMS_DOMAIN_PREFER_WIFI_PREFER == IMSA_GetImsDomainPrefer()))
                && (VOS_TRUE == IMSA_Domain_IsDualDomainExist()))
            {
                /* 如果NV和开机状态支持双域,且IMS优先关系支持双域, 设置IMSA域状态为 Dual Domain */
                IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_DUAL);
                IMSA_INFO_LOG("IMSA_Domain_SetDomainModeInWifiPowerOn: 6_DomainMode is IMSA_IMS_DOMAIN_MODE_DUAL!");
            }
            break;

        case IMSA_IMS_DOMAIN_MODE_DUAL:
            /* Wifi开机状态收到Wifi开机消息，告警 */
            IMSA_WARN_LOG("IMSA_Domain_SetDomainModeInWifiPowerOn: Multi Wifi Power On Message Received!");
            break;

        default:
            IMSA_WARN_LOG("IMSA_Domain_SetDomainModeInWifiPowerOn: Status is Starting!");
            return VOS_FALSE;
    }

    return VOS_TRUE;
}

/*****************************************************************************
 Function Name  : IMSA_Domain_SetDomainModeInWifiPowerOff()
 Description    : WIFI开机处理状态迁移
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_UINT32  VOS_TRUE/VOS_FALSE

 History        :
      1.Zhangdongfeng 353461        2015-12-15  Draft Enact
*****************************************************************************/

void IMSA_Domain_SetDomainModeInWifiPowerOff(void)
{

    IMSA_DOMAIN_MANAGER_STRU           *pstDomainManager    = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_Domain_SetDomainModeInWifiPowerOff: ENTER!");
    TLPS_PRINT2LAYER_INFO(IMSA_Domain_SetDomainModeInWifiPowerOff_ENUM, LNAS_ENTRY);

    pstDomainManager = IMSA_Domain_GetDomainManagerAddr();


    /* 域管理状态机迁移 */
    switch(IMSA_GetDomainMode())
    {
        case IMSA_IMS_DOMAIN_MODE_NULL:
            /* NULL状态下,受到WIFI关机命令,不做任何处理 */
            break;

        case IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY:
            /* Cellular状态下,受到WIFI关机命令,不做任何处理 */
            break;

        case IMSA_IMS_DOMAIN_MODE_WIFI_ONLY:
            /* 设置IMSA域状态为NULL */
            IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_NULL);

            /* 清除支持WIFI域 */
            IMSA_Domain_SetWifiDisable(pstDomainManager->enImsaAvailDomain);

            break;

        case IMSA_IMS_DOMAIN_MODE_DUAL:
            /* 设置IMSA域状态为Cellular Only */
            IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY);

            /* 清除支持WIFI域 */
            IMSA_Domain_SetWifiDisable(pstDomainManager->enImsaAvailDomain);
            break;

        default:
            IMSA_WARN_LOG("IMSA_Domain_SetDomainModeInWifiPowerOff: Domain Status is Wrong!");

            TLPS_PRINT2LAYER_WARNING(IMSA_Domain_SetDomainModeInWifiPowerOff_ENUM, LNAS_StateInvalid);
            break;
    }

    return;
}



/*****************************************************************************
 Function Name  : IMSA_Domain_SetDomainModeInCellularPowerOn()
 Description    : WIFI开机处理状态迁移
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_UINT32  VOS_TRUE/VOS_FALSE

 History        :
      1.Zhangdongfeng 353461        2015-11-19  Draft Enact
*****************************************************************************/
VOS_UINT32 IMSA_Domain_SetDomainModeInCellularPowerOn(VOS_VOID)
{
    IMSA_CONTROL_MANAGER_STRU  *pstControlManager;

    pstControlManager = IMSA_GetControlManagerAddress();


    IMSA_INFO_LOG("IMSA_Domain_SetDomainModeInCellularPowerOn: ENTER!");
    TLPS_PRINT2LAYER_INFO(IMSA_Domain_SetDomainModeInCellularPowerOn_ENUM, LNAS_ENTRY);

    /* 设置WIFI开机状态 */
    if( IMSA_STATUS_STARTING == pstControlManager->enImsaCellularStatus)
    {
        IMSA_WARN_LOG("IMSA_Domain_SetDomainModeInCellularPowerOn: Status is Starting!");
        return VOS_FALSE;
    }

    switch(IMSA_GetDomainMode())
    {
        case IMSA_IMS_DOMAIN_MODE_NULL:
            /*  设置域管理模块状态机迁移  */
            if( ((IMSA_IMS_DOMAIN_PREFER_CELLULAR_PREFER == IMSA_GetImsDomainPrefer())
                  ||(IMSA_IMS_DOMAIN_PREFER_WIFI_PREFER == IMSA_GetImsDomainPrefer()))
                && (VOS_TRUE == IMSA_Domain_IsDualDomainExist()))
            {
                /* 如果NV和开机状态支持双域,且IMS优先关系支持双域, 设置IMSA域状态为 Dual Domain */
                IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_DUAL);
                IMSA_INFO_LOG("IMSA_Domain_SetDomainModeInCellularPowerOn: 1_DomainMode is IMSA_IMS_DOMAIN_MODE_DUAL!");
            }
            else if( ((IMSA_IMS_DOMAIN_PREFER_WIFI_PREFER == IMSA_GetImsDomainPrefer())
                      || (IMSA_IMS_DOMAIN_PREFER_CELLULAR_PREFER == IMSA_GetImsDomainPrefer())
                      || (IMSA_IMS_DOMAIN_PREFER_WIFI_ONLY == IMSA_GetImsDomainPrefer()))
                     &&(VOS_TRUE == IMSA_Domain_IsWifiDomainExist()))
            {
                /* 如果NV和开机状态支持WIFI域,且IMS优先关系支持WIFI域,设置IMSA域状态为 WIFI ONLY */
                IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_WIFI_ONLY);
                IMSA_INFO_LOG("IMSA_Domain_SetDomainModeInCellularPowerOn: 2_DomainMode is IMSA_IMS_DOMAIN_MODE_WIFI_ONLY!");
            }
            else if( ((IMSA_IMS_DOMAIN_PREFER_CELLULAR_PREFER == IMSA_GetImsDomainPrefer())
                       || (IMSA_IMS_DOMAIN_PREFER_WIFI_PREFER == IMSA_GetImsDomainPrefer())
                       || (IMSA_IMS_DOMAIN_PREFER_CELLULAR_ONLY == IMSA_GetImsDomainPrefer()))
                     &&(VOS_TRUE == IMSA_Domain_IsCellularDomainExist()))
            {
                /* 如果NV和开机状态支持Cellular域,且IMS优先关系支持Cellular域,设置IMSA域状态为 Cellular ONLY */
                IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY);
                IMSA_INFO_LOG("IMSA_Domain_SetDomainModeInCellularPowerOn: 3_DomainMode is IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY!");
            }
            else
            {
                /* 设置IMSA域状态为 NULL */
                IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_NULL);
                IMSA_INFO_LOG("IMSA_Domain_SetDomainModeInCellularPowerOn: 4_DomainMode is IMSA_IMS_DOMAIN_MODE_NULL!");
            }
            break;

        case IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY:
            /* 处于Cellular Only时，已经接收到开机命令消息，不处理IMS开机部分 */
            /* 单独关闭Cellular，需要多个步骤，暂时不支持整机开机状态下，接收到IMS动态开关打开，作为Cellular单独关闭的场景*/
            IMSA_WARN_LOG("IMSA_Domain_SetDomainModeInCellularPowerOn: 5_DomainMode is IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY!");
            break;

        case IMSA_IMS_DOMAIN_MODE_WIFI_ONLY:
            /* 接收到整机开机命令，则记录Cellular开机状态，同时根据情况会迁移到DualMode状态 */
            if( ((IMSA_IMS_DOMAIN_PREFER_CELLULAR_PREFER == IMSA_GetImsDomainPrefer())
                  ||(IMSA_IMS_DOMAIN_PREFER_WIFI_PREFER == IMSA_GetImsDomainPrefer()))
                && (VOS_TRUE == IMSA_Domain_IsDualDomainExist()))
            {
                /* 如果NV和开机状态支持双域,且IMS优先关系支持双域, 设置IMSA域状态为 Dual Domain */
                IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_DUAL);
                IMSA_INFO_LOG("IMSA_Domain_SetDomainModeInCellularPowerOn: 6_DomainMode is IMSA_IMS_DOMAIN_MODE_DUAL!");
            }
            else
            {
                IMSA_INFO_LOG("IMSA_Domain_SetDomainModeInCellularPowerOn: 6_DomainMode is IMSA_IMS_DOMAIN_MODE_WIFI_ONLY!");
            }
            break;

        case IMSA_IMS_DOMAIN_MODE_DUAL:
            /* 暂时不处理 */
            IMSA_INFO_LOG("IMSA_Domain_SetDomainModeInCellularPowerOn: 7_DomainMode is IMSA_IMS_DOMAIN_MODE_DUAL!");
            break;

        default:
            IMSA_WARN_LOG("IMSA_Domain_SetDomainModeInCellularPowerOn: Status is Starting!");
            TLPS_PRINT2LAYER_WARNING(IMSA_Domain_SetDomainModeInCellularPowerOn_ENUM, IMSA_PRINT_STATUS_STARTING);
            return VOS_FALSE;
    }

    return VOS_TRUE;
}

/*****************************************************************************
 Function Name  : IMSA_Domain_SetDomainModeInWifiPowerOff()
 Description    : WIFI开机处理状态迁移
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_UINT32  VOS_TRUE/VOS_FALSE

 History        :
      1.Zhangdongfeng 353461        2015-12-15  Draft Enact
*****************************************************************************/

void IMSA_Domain_SetDomainModeInCellularPowerOff(void)
{

    IMSA_DOMAIN_MANAGER_STRU           *pstDomainManager    = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_Domain_SetDomainModeInCellularPowerOff: ENTER!");
    TLPS_PRINT2LAYER_INFO(IMSA_Domain_SetDomainModeInCellularPowerOff_ENUM, LNAS_ENTRY);
    pstDomainManager = IMSA_Domain_GetDomainManagerAddr();


    switch(IMSA_GetDomainMode())
    {
        case IMSA_IMS_DOMAIN_MODE_NULL:
            break;

        case IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY:

            IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_NULL);
            /* 清除支持Cellular域 */
            IMSA_Domain_SetLteDisable(pstDomainManager->enImsaAvailDomain);
            break;

        case IMSA_IMS_DOMAIN_MODE_WIFI_ONLY:
            break;

        case IMSA_IMS_DOMAIN_MODE_DUAL:
            /* 设置IMSA域状态为WIFI Only */
            IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_WIFI_ONLY);

            /* 清除支持Cellular域 */
            IMSA_Domain_SetLteDisable(pstDomainManager->enImsaAvailDomain);
            break;

        default:
            IMSA_WARN_LOG("IMSA_Domain_SetDomainModeInCellularPowerOff: Domain Status is Wrong!");
    }

    return;
}


/*****************************************************************************
 Function Name  : IMSA_Domain_SetDomainModeInDeregReq()
 Description    : Dereg req to change Domain Cap
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_UINT32  VOS_TRUE/VOS_FALSE

 History        :
      1.Zhangdongfeng 353461        2016-03-28  Draft Enact
*****************************************************************************/

void IMSA_Domain_SetDomainModeInDeregReq(void)
{
    IMSA_INFO_LOG("IMSA_Domain_SetDomainModeInDeregReq: ENTER!");

    switch(IMSA_GetDomainMode())
    {
        case IMSA_IMS_DOMAIN_MODE_NULL:
            break;

        case IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY:

            IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_NULL);
            break;

        case IMSA_IMS_DOMAIN_MODE_WIFI_ONLY:
            IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_NULL);

            break;

        case IMSA_IMS_DOMAIN_MODE_DUAL:
            IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_NULL);
            break;

        default:
            IMSA_WARN_LOG("IMSA_Domain_SetDomainModeInDeregReq: Domain Status is Wrong!");
    }

    return;
}

/*****************************************************************************
 Function Name  : IMSA_Domain_InitD2ImsStack()
 Description    : D2 IMS Stack初始化
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_UINT32  VOS_TRUE/VOS_FALSE

 History        :
      1.Zhangdongfeng 353461        2015-11-19  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_Domain_InitD2ImsStack(VOS_VOID)
{
    IMSA_CONTROL_MANAGER_STRU  *pstControlManager;
    static VOS_UINT8            ucD2ImsTaskStart = 0;

    pstControlManager = IMSA_GetControlManagerAddress();
    TLPS_PRINT2LAYER_INFO(IMSA_Domain_InitD2ImsStack_ENUM, LNAS_ENTRY);

    if(ucD2ImsTaskStart == 0)
    {
        /*初始化D2协议栈*/
        (VOS_VOID)IMSA_ImsInit();
        ucD2ImsTaskStart ++;
    }


    /*判断开机状态，如果没有开机，则初始化NV值 */
    if(pstControlManager->enImsaStatus == IMSA_STATUS_NULL)
    {
        /*读取NV*/
        IMSA_ReadImsaNV();
    }

    return;
}

/*****************************************************************************
 Function Name  : IMSA_Domain_StartImsStack()
 Description    : IMS开机处理状态迁移
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_UINT32  VOS_TRUE/VOS_FALSE

 History        :
      1.Zhangdongfeng 353461        2015-11-19  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_Domain_StartImsStack(VOS_VOID)
{
    IMSA_CONTROL_MANAGER_STRU    *pstControlManager;

    IMSA_INFO_LOG("IMSA_Domain_StartImsStack: ENTER!");
    TLPS_PRINT2LAYER_INFO(IMSA_Domain_StartImsStack_ENUM, LNAS_ENTRY);

    pstControlManager = IMSA_GetControlManagerAddress();

    /*已处于开机过程状态，则丢弃*/
    if(IMSA_STATUS_STARTING == pstControlManager->enImsaStatus)
    {
        IMSA_WARN_LOG("IMSA_Domain_StartImsStack: Status is Starting!");
        TLPS_PRINT2LAYER_WARNING(IMSA_ProcMsccMsgStartReq_ENUM, IMSA_PRINT_STATUS_STARTING);
        return ;
    }

    IMSA_StopTimer(&pstControlManager->stProtectTimer);
    /* 初始化 */

    /*通知IMS开机，转状态*/
    pstControlManager->enImsaStatus = IMSA_STATUS_STARTING;

    IMSA_SndImsMsgStartReq();

    IMSA_StartTimer(&pstControlManager->stProtectTimer);

    /* 卡在位，则读取卡信息 */
    if(IMSA_ISIM_STATUS_AVAILABLE == pstControlManager->enImsaIsimStatus)
    {
        IMSA_INFO_LOG("IMSA_Domain_StartImsStack: ISIM is available!");
        TLPS_PRINT2LAYER_INFO(IMSA_Domain_StartImsStack_ENUM, 2);
        IMSA_ReadIsimFile();
    }

    return;

}

/*****************************************************************************
 Function Name  : IMSA_Domain_StopImsStack()
 Description    : 停止IMS SDK
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_UINT32  VOS_TRUE/VOS_FALSE

 History        :
      1.Zhangdongfeng 353461        2015-11-19  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_Domain_StopImsStack(VOS_VOID)
{
    IMSA_CONTROL_MANAGER_STRU *pstControlManager;

    pstControlManager = IMSA_GetControlManagerAddress();
    TLPS_PRINT2LAYER_INFO(IMSA_Domain_StopImsStack_ENUM, LNAS_ENTRY);

    /*停止开关机保护定时器*/
    IMSA_StopTimer(&pstControlManager->stProtectTimer);

    /*通知IMS关机，转状态*/
    pstControlManager->enImsaStatus = IMSA_STATUS_STOPING;

    /*关机后，卡在位时，需要将卡状态置为有效，因为可能有卡无效的场景，需要关机后清除状态*/
    if( IMSA_ISIM_STATUS_ABSENT != pstControlManager->enImsaIsimStatus )
    {
        pstControlManager->enImsaIsimStatus = IMSA_ISIM_STATUS_AVAILABLE;
    }
    /*启动开关机保护定时器*/
    IMSA_StartTimer(&pstControlManager->stProtectTimer);

    /* 如果当前状态时IDLE+DEREG，则直接通知IMS关机，不需要申请资源 */
#if (FEATURE_ON == FEATURE_DSDS)
    if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
    {
        if (IMSA_TRUE == IMSA_SRV_IsNotNeedNwDereg())
        {
            IMSA_SndImsMsgStopReq();
            return;
        }
    }
#endif
    /* 需要根据当前的注册状态，判断是否需要申请资源 */
    switch (IMSA_DsdsGetResourceStatus(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER))
    {
        case IMSA_RESOURCE_STATUS_ACTIVE:

            IMSA_SndImsMsgStopReq();
        #if (FEATURE_ON == FEATURE_DSDS)
            if (PS_RRM_RF_SHARED == IMSA_Dsds_RRM_RfResourceIsShared())
            {
                IMSA_DsdsModifyTaskSrcByTaskType(IMSA_RF_RES_APPLY_TRIGER_SRC_POWER_OFF,
                                                 RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
            }
        #endif
            break;
    #if (FEATURE_ON == FEATURE_DSDS)
        case IMSA_RESOURCE_STATUS_APPLY_NOT_NEED_WAIT_CNF:

            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_POWER_OFF,
                                          VOS_FALSE,
                                          VOS_FALSE);
            IMSA_SndImsMsgStopReq();
            break;

        case  IMSA_RESOURCE_STATUS_NULL:

            IMSA_DsdsProcResourceApplyReq(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER,
                                          IMSA_RF_RES_APPLY_TRIGER_SRC_POWER_OFF,
                                          VOS_FALSE,
                                          VOS_FALSE);

            //IMSA_DsdsSaveMoSmsMsg(pstRpDataReq);
            break;

        case IMSA_RESOURCE_STATUS_APPLYING:
            /* 如果当前状态为pending，则取消任务的注册，然后通知IMS本地去注册，然后直接关机 */
            IMSA_DsdsModifyTaskSrcByTaskType(IMSA_RF_RES_APPLY_TRIGER_SRC_POWER_OFF,
                                             RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);
            break;

        case IMSA_RESOURCE_STATUS_WAIT_STATUS_IND:

            IMSA_DsdsProcResourceRelease(RRM_PS_TASK_TYPE_LTE_IMS_REGISTER);

            IMSA_SRV_ProcStopMsgWhenNoRf();

            IMSA_SndImsMsgStopReq();
            break;
    #endif
        default:
            break;
    }

//    IMSA_SndImsMsgStopReq();


    return;
}


/*****************************************************************************
 Function Name  : IMSA_Domain_ProcWifiPOwerOnReq()
 Description    : WIFI开机消息处理函数
 Input          : VOS_VOID *pRcvMsg
 Output         : VOS_VOID
 Return Value   : VOS_UINT32  VOS_TRUE/VOS_FALSE

 History        :
      1.Zhangdongfeng 353461        2015-10-19  Draft Enact
*****************************************************************************/
VOS_UINT32 IMSA_Domain_ProcWifiPowerOnReq( const VOS_VOID                     *pRcvMsg  )
{
    IMSA_CONTROL_MANAGER_STRU    *pstControlManager;
    IMSA_DOMAIN_MANAGER_STRU     *pstDomainManager;


    IMSA_INFO_LOG("IMSA_Domain_ProcWifiPowerOnReq: ENTER!");
    TLPS_PRINT2LAYER_INFO(IMSA_Domain_ProcWifiPowerOnReq_ENUM, LNAS_ENTRY);

    pstControlManager = IMSA_GetControlManagerAddress();
    pstDomainManager    = IMSA_Domain_GetDomainManagerAddr();

    /* 初始化D2协议栈和IMS NV值 */
    IMSA_Domain_InitD2ImsStack();

    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOnReq begin with state: DomainMode = ",IMSA_GetDomainMode());
    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOnReq begin with state: enDomainPrefer = ",IMSA_GetImsDomainPrefer());
    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOnReq begin with state: cellular support = ",IMSA_GetCellularImsSupportFlag());
    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOnReq begin with state: wifi support = ",IMSA_GetWifiImsSupportFlag());
    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOnReq begin with state: enImsaCellularStatus = ",pstControlManager->enImsaCellularStatus);
    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOnReq begin with state: enImsawifiStatus = ",pstControlManager->enImsaWifiStatus);
    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOnReq begin with state: enImsaStatus = ",pstControlManager->enImsaStatus);
    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOnReq: enImsaStatus = ",pstControlManager->enImsaStatus);

    /*判断开机状态，如果已开机，则回复开机成功*/
    if(IMSA_STATUS_STARTED == pstControlManager->enImsaWifiStatus)
    {
        /* 指示SPM WIFI开机成功 */
        IMSA_SndSpmImsSwitchStateInd(IMSA_SPM_IMS_SWITCH_STATE_ON);

        /* 回复WIFI开机成功 */
        IMSA_SndWifiPowerOnCnf();

        /* 指示处理开机状态迁移 */
        (VOS_VOID)IMSA_Domain_SetDomainModeInWifiPowerOn();

        /* 通知ERRC启动上报LTE信号质量 */
        if( (VOS_FALSE == pstDomainManager->ucLteQualityCfgFlag)
            && ( IMSA_IMS_DOMAIN_MODE_DUAL == IMSA_GetDomainMode()))
        {
            IMSA_SendRrcQualityConfigReq(IMSA_LRRC_CFG_QUALITY_START);
            pstDomainManager->ucLteQualityCfgFlag = VOS_TRUE;
        }

        return VOS_FALSE;
    }


    if((IMSA_STATUS_STARTING == pstControlManager->enImsaStatus)
        || (IMSA_STATUS_NULL == pstControlManager->enImsaStatus))
    {
        /* 设置WIFI开机状态为开机中 */
        pstControlManager->enImsaWifiStatus = IMSA_STATUS_STARTING;

        /* 启动IMS协议栈 */
        IMSA_Domain_StartImsStack();

        /* IMS由于内部行为,自行启动IMS,清除自行启动标志 */
        if( VOS_TRUE == pstControlManager->ucInternalImsStackStartFlag)
        {
            /* IMSA内部开机行为,不需要回复开机成功消息 */
            pstControlManager->ucInternalImsStackStartFlag = VOS_FALSE;
        }
    }
    else if(IMSA_STATUS_STARTED == pstControlManager->enImsaStatus)
    {
        /* 设置WIFI开机状态为已经开机 */
        pstControlManager->enImsaWifiStatus = IMSA_STATUS_STARTED;

        /* 指示SPM WIFI开机成功 */
        IMSA_SndSpmImsSwitchStateInd(IMSA_SPM_IMS_SWITCH_STATE_ON);

        /* 回复WIFI开机成功 */
        IMSA_SndWifiPowerOnCnf();
    }

    /* 指示处理开机状态迁移 */
    (VOS_VOID)IMSA_Domain_SetDomainModeInWifiPowerOn();

    /* 通知ERRC启动上报LTE信号质量 */
    if( (VOS_FALSE == pstDomainManager->ucLteQualityCfgFlag)
        && ( IMSA_IMS_DOMAIN_MODE_DUAL == IMSA_GetDomainMode()))
    {
        IMSA_SendRrcQualityConfigReq(IMSA_LRRC_CFG_QUALITY_START);
        pstDomainManager->ucLteQualityCfgFlag = VOS_TRUE;
    }

    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOnReq end with state: DomainMode = ",IMSA_GetDomainMode());
    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOnReq end with state: enDomainPrefer = ",IMSA_GetImsDomainPrefer());
    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOnReq end with state: cellular support = ",IMSA_GetCellularImsSupportFlag());
    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOnReq end with state: wifi support = ",IMSA_GetWifiImsSupportFlag());
    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOnReq end with state: enImsaCellularStatus = ",pstControlManager->enImsaCellularStatus);
    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOnReq end with state: enImsawifiStatus = ",pstControlManager->enImsaWifiStatus);
    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOnReq end with state: enImsaStatus = ",pstControlManager->enImsaStatus);
    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOnReq: enImsaStatus = ",pstControlManager->enImsaStatus);


    return VOS_FALSE;
}





/*****************************************************************************
 Function Name  : IMSA_Domain_ProcWifiPowerOffReq()
 Description    : WIFI关机消息处理函数
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-10-19  Draft Enact
*****************************************************************************/
VOS_UINT32 IMSA_Domain_ProcWifiPowerOffReq
(
    const VOS_VOID                     *pRcvMsg
)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlManager;
    IMSA_DOMAIN_MANAGER_STRU           *pstDomainManager = VOS_NULL;

    pstControlManager   = IMSA_GetControlManagerAddress();
    pstDomainManager    = IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_Domain_ProcWifiPowerOffReq_ENUM, LNAS_ENTRY);

    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOffReq begin with state: DomainMode = ",IMSA_GetDomainMode());
    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOffReq begin with state: enDomainPrefer = ",IMSA_GetImsDomainPrefer());
    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOffReq begin with state: cellular support = ",IMSA_GetCellularImsSupportFlag());
    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOffReq begin with state: wifi support = ",IMSA_GetWifiImsSupportFlag());
    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOffReq begin with state: enImsaCellularStatus = ",pstControlManager->enImsaCellularStatus);
    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOffReq begin with state: enImsawifiStatus = ",pstControlManager->enImsaWifiStatus);
    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOffReq begin with state: enImsaStatus = ",pstControlManager->enImsaStatus);

    /* 域能力迁移 */
    IMSA_Domain_SetDomainModeInWifiPowerOff();

    /* 如果WIFI没有开机，直接回复Wifi关机成功 */
    if(IMSA_STATUS_NULL == pstControlManager->enImsaWifiStatus)
    {
        /* 设置WIFI关机状态 */
        pstControlManager->enImsaWifiStatus = IMSA_STATUS_NULL;

        /* 回复WIFI关机成功 */
        IMSA_SndWifiPowerOffCnf();
        return VOS_FALSE;
    }

    /* 如果正处于整机关机状态,则直接返回WIFI关机成功 */
    if(IMSA_STATUS_STOPING == pstControlManager->enImsaStatus)
    {
        return VOS_FALSE;
    }

    /* 设置域选初始化函数处理 */
    IMSA_Domain_InitDomainOpVar(WUEPS_PID_MAPCON,ID_WIFI_IMSA_POWEROFF_REQ,VOS_FALSE,VOS_FALSE);

    /* 调用域选函数, 根据当前状态选择合适的侯选域 */
    switch(IMSA_JudgeImsDomainC1())
    {
        case IMSA_IMS_DOMAIN_NULL:

#ifdef VOLTE_VOWIFI_CTRL_ONLY
            if(IMSA_IMS_DOMAIN_NULL == IMSA_GetDomainMode())
            {
                /*只存在WIFI, 关闭以后需要停IMS SDK */
                IMSA_Domain_StopImsStack();
                /* 设置WIFI关机中状态 */
                pstControlManager->enImsaWifiStatus = IMSA_STATUS_STOPING;
                return VOS_FALSE;
            }
#endif

            IMSA_ProcessNoDomainSelectInWifiPowerOff();
            break;

        case IMSA_IMS_DOMAIN_WIFI:
            IMSA_WARN_LOG("IMSA_Domain_ProcWifiPowerOffReq: DomainC1Judge Wrong1!");
            TLPS_PRINT2LAYER_INFO(IMSA_Domain_ProcWifiPowerOffReq_ENUM, LNAS_FUNCTION_LABEL1);
            break;

        case IMSA_IMS_DOMAIN_LTE:
            IMSA_ProcessLTEOnlyDomainSelectInWifiPowerOff();
            break;

        case IMSA_IMS_DOMAIN_LTE_WIFI:
            IMSA_WARN_LOG("IMSA_Domain_ProcWifiPowerOffReq: DomainC1Judge Wrong2!");

            TLPS_PRINT2LAYER_INFO(IMSA_Domain_ProcWifiPowerOffReq_ENUM, LNAS_FUNCTION_LABEL2);
            break;

        default:
            IMSA_WARN_LOG("IMSA_Domain_ProcWifiPowerOffReq: DomainC1Judge Wrong3!");

            TLPS_PRINT2LAYER_INFO(IMSA_Domain_ProcWifiPowerOffReq_ENUM, LNAS_FUNCTION_LABEL3);
            break;
    }


    /* WIFI关机命令不能延迟处理,发现延迟,则告警 */
    if( VOS_TRUE == pstDomainManager->ucMsgDelayProcessFlag)
    {
        // 告警，不做任何处理
        IMSA_WARN_LOG("IMSA_Domain_ProcWifiPowerOffReq: Poweroff Message can't delay by SRV!");
    }
    else
    {
    }

    /* 通知ERRC停止上报LTE信号质量 */
    if( VOS_TRUE == pstDomainManager->ucLteQualityCfgFlag)
    {
        IMSA_SendRrcQualityConfigReq(IMSA_LRRC_CFG_QUALITY_STOP);
        pstDomainManager->ucLteQualityCfgFlag = VOS_FALSE;
    }
    else
    {
    }

    /* WIFI关机导致域选操作,则需要等待域选完成才回复关机确认 */
    if( VOS_TRUE == pstDomainManager->ucDomainSelectFlag)
    {
        /* 设置WIFI关机中状态 */
        pstControlManager->enImsaWifiStatus = IMSA_STATUS_STOPING;
        return VOS_FALSE;
    }
    else
    {
    }

    /* 设置WIFI关机状态 */
    pstControlManager->enImsaWifiStatus = IMSA_STATUS_NULL;

    /* 指示SPM WIFI关机成功 */
//    IMSA_SndSpmImsSwitchStateInd(IMSA_SPM_IMS_SWITCH_STATE_OFF);

    /* 回复WIFI关机成功 */
    IMSA_SndWifiPowerOffCnf();

    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOffReq end with state: DomainMode = ",IMSA_GetDomainMode());
    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOffReq end with state: enDomainPrefer = ",IMSA_GetImsDomainPrefer());
    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOffReq end with state: cellular support = ",IMSA_GetCellularImsSupportFlag());
    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOffReq end with state: wifi support = ",IMSA_GetWifiImsSupportFlag());
    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOffReq end with state: enImsaCellularStatus = ",pstControlManager->enImsaCellularStatus);
    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOffReq end with state: enImsawifiStatus = ",pstControlManager->enImsaWifiStatus);
    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiPowerOffReq end with state: enImsaStatus = ",pstControlManager->enImsaStatus);

    return VOS_FALSE;
}





/*****************************************************************************
 Function Name  : IMSA_Domain_ProcWifiStateInd()
 Description    : Wifi State Ind消息处理函数
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-10-19  Draft Enact
*****************************************************************************/
VOS_UINT32 IMSA_Domain_ProcWifiStateInd
(
    const VOS_VOID                     *pRcvMsg
)
{
    IMSA_DOMAIN_MANAGER_STRU           *pstDomainManager    = VOS_NULL_PTR;
    AT_IMSA_IMS_CTRL_MSG_STRU          *pAtWifiImsaCtrlMsg  = VOS_NULL_PTR;
    WIFI_IMSA_STATE_IND_STRU           *pWifiStateInd       = VOS_NULL_PTR;
    IMSA_CONTROL_MANAGER_STRU          *pstControlManager   = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_Domain_ProcWifiStateInd: ENTER!");
    TLPS_PRINT2LAYER_INFO(IMSA_Domain_ProcWifiStateInd_ENUM, LNAS_ENTRY);

    pAtWifiImsaCtrlMsg = (AT_IMSA_IMS_CTRL_MSG_STRU *)pRcvMsg;
    pWifiStateInd = (WIFI_IMSA_STATE_IND_STRU *) (pAtWifiImsaCtrlMsg->aucWifiMsg);
    pstDomainManager = IMSA_Domain_GetDomainManagerAddr();
    pstControlManager = IMSA_GetControlManagerAddress();

    /* 设置当前WIFI域可用 */
    IMSA_Domain_SetWifiEnable(pstDomainManager->enImsaAvailDomain);

    /* 保存WIFI信号强度 */
    if(WIFI_IMSA_QUALITY_LEVEL_BUTT > pWifiStateInd->enWifiQuality)
    {
        pstDomainManager->enWifiQuality = pWifiStateInd->enWifiQuality;
    }
    else
    {
        pstDomainManager->enWifiQuality = IMSA_WIRELESS_QUALITY_LEVEL_BAD;
    }

    /* 保存AP MAC地址 */
    IMSA_SRV_SaveWifiStateInd(pWifiStateInd);

    /* NULL状态下，收到State_ind消息,当作开机消息进行处理  */
    if( IMSA_IMS_DOMAIN_MODE_NULL == IMSA_GetDomainMode() )
    {
        /* 初始化D2协议栈和IMS NV值 */
        IMSA_Domain_InitD2ImsStack();

        /* 启动IMS协议栈 */
        if(IMSA_STATUS_NULL == pstControlManager->enImsaStatus)
        {
            IMSA_Domain_StartImsStack();

            /* 设置IMS Stack启动是独自启动 */
            pstControlManager->ucInternalImsStackStartFlag = VOS_TRUE;
        }

        if( IMSA_STATUS_NULL == pstControlManager->enImsaWifiStatus)
        {
            /* 设置WIFI已经开启 */
            pstControlManager->enImsaWifiStatus = IMSA_STATUS_STARTED;
        }

        /* IMS域状态转换 */
        (VOS_VOID)IMSA_Domain_SetDomainModeInWifiPowerOn();

        /* 通知ERRC启动上报LTE信号质量 */
        if( (VOS_FALSE == pstDomainManager->ucLteQualityCfgFlag)
            && ( IMSA_IMS_DOMAIN_MODE_DUAL == IMSA_GetDomainMode()))
        {
            IMSA_SendRrcQualityConfigReq(IMSA_LRRC_CFG_QUALITY_START);
            pstDomainManager->ucLteQualityCfgFlag = VOS_TRUE;
        }

        /* 指示SPM WIFI开机成功 */
        IMSA_SndSpmImsSwitchStateInd(IMSA_SPM_IMS_SWITCH_STATE_ON);
    }
    else if( IMSA_STATUS_NULL == pstControlManager->enImsaWifiStatus)
    {
        /* 设置WIFI已经开启 */
        pstControlManager->enImsaWifiStatus = IMSA_STATUS_STARTED;

        /* IMS域状态转换 */
        (VOS_VOID)IMSA_Domain_SetDomainModeInWifiPowerOn();

        /* 通知ERRC启动上报LTE信号质量 */
        if( (VOS_FALSE == pstDomainManager->ucLteQualityCfgFlag)
            && ( IMSA_IMS_DOMAIN_MODE_DUAL == IMSA_GetDomainMode()))
        {
            IMSA_SendRrcQualityConfigReq(IMSA_LRRC_CFG_QUALITY_START);
            pstDomainManager->ucLteQualityCfgFlag = VOS_TRUE;
        }

        /* 指示SPM WIFI开机成功 */
        IMSA_SndSpmImsSwitchStateInd(IMSA_SPM_IMS_SWITCH_STATE_ON);

        /* 启动域选择 */
        IMSA_Domain_DomainSelectInWifiStateInd();
    }
    else
    {
        /* 启动域选择 */
        IMSA_Domain_DomainSelectInWifiStateInd();
    }


    IMSA_INFO_LOG1("IMSA_Domain_ProcWifiStateInd: domain = ",IMSA_GetDomainMode());
    return VOS_FALSE;
}

/*****************************************************************************
 Function Name  : IMSA_Domain_ProcMsccStartReq()
 Description    : MSCC开机消息处理函数
 Input          : VOS_VOID *pRcvMsg
 Output         : VOS_VOID
 Return Value   : VOS_UINT32  VOS_TRUE/VOS_FALSE

 History        :
      1.Zhangdongfeng 353461        2015-10-19  Draft Enact
*****************************************************************************/
VOS_UINT32 IMSA_Domain_ProcMsccStartReq( const VOS_VOID                     *pRcvMsg )
{
    IMSA_CONTROL_MANAGER_STRU   *pstControlManager;
    IMSA_DOMAIN_MANAGER_STRU    *pstImsaDomainManager = VOS_NULL_PTR;

    pstControlManager = IMSA_GetControlManagerAddress();
    pstImsaDomainManager = IMSA_Domain_GetDomainManagerAddr();

    TLPS_PRINT2LAYER_INFO(IMSA_Domain_ProcMsccStartReq_ENUM, LNAS_ENTRY);
    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStartReq start with state: DomainMode = ",IMSA_GetDomainMode());
    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStartReq start with state: enDomainPrefer = ",IMSA_GetImsDomainPrefer());
    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStartReq start with state: cellular support = ",IMSA_GetCellularImsSupportFlag());
    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStartReq start with state: wifi support = ",IMSA_GetWifiImsSupportFlag());
    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStartReq start with state: enImsaCellularStatus = ",pstControlManager->enImsaCellularStatus);
    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStartReq start with state: enImsawifiStatus = ",pstControlManager->enImsaWifiStatus);
    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStartReq start with state: enImsaStatus = ",pstControlManager->enImsaStatus);

    /* 初始化D2协议栈和IMS NV值 */
    IMSA_Domain_InitD2ImsStack();

    /* 判断开机状态，如果已开机，则回复开机成功 */
    if(IMSA_STATUS_STARTED == pstControlManager->enImsaStatus)
    {
        /* 设置Cellular开机状态 */
        pstControlManager->enImsaCellularStatus = IMSA_STATUS_STARTED;

        /* IMS域状态转换 */
        (VOS_VOID)IMSA_Domain_SetDomainModeInCellularPowerOn();

        /* 通知ERRC启动上报LTE信号质量 */
        if( (VOS_FALSE == pstImsaDomainManager->ucLteQualityCfgFlag)
            && ( IMSA_IMS_DOMAIN_MODE_DUAL == IMSA_GetDomainMode()))
        {
            IMSA_SendRrcQualityConfigReq(IMSA_LRRC_CFG_QUALITY_START);
            pstImsaDomainManager->ucLteQualityCfgFlag = VOS_TRUE;
        }

        /* 回复开机成功 */
        IMSA_SndMsccMsgStartCnf();
        return VOS_FALSE;
    }

    /* 设置Cellular开机状态 */
    if(  IMSA_STATUS_NULL == pstControlManager->enImsaCellularStatus)
    {
        pstControlManager->enImsaCellularStatus = IMSA_STATUS_STARTING;
    }

    /* IMS由于内部行为,自行启动IMS,清除自行启动标志 */
    if( VOS_TRUE == pstControlManager->ucInternalImsStackStartFlag)
    {
        /* IMSA内部开机行为,不需要回复开机成功消息 */
        pstControlManager->ucInternalImsStackStartFlag = VOS_FALSE;
    }

    /* 启动IMS协议栈 */
    IMSA_Domain_StartImsStack();

    /* IMS域状态转换 */
    (void)IMSA_Domain_SetDomainModeInCellularPowerOn();

    /* 通知ERRC启动上报LTE信号质量 */
    if( (VOS_FALSE == pstImsaDomainManager->ucLteQualityCfgFlag)
        && ( IMSA_IMS_DOMAIN_MODE_DUAL == IMSA_GetDomainMode()))
    {
        IMSA_SendRrcQualityConfigReq(IMSA_LRRC_CFG_QUALITY_START);
        pstImsaDomainManager->ucLteQualityCfgFlag = VOS_TRUE;
    }

    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStartReq end with state: DomainMode = ",IMSA_GetDomainMode());
    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStartReq end with state: enDomainPrefer = ",IMSA_GetImsDomainPrefer());
    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStartReq end with state: cellular support = ",IMSA_GetCellularImsSupportFlag());
    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStartReq end with state: wifi support = ",IMSA_GetWifiImsSupportFlag());
    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStartReq end with state: enImsaCellularStatus = ",pstControlManager->enImsaCellularStatus);
    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStartReq end with state: enImsawifiStatus = ",pstControlManager->enImsaWifiStatus);
    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStartReq end with state: enImsaStatus = ",pstControlManager->enImsaStatus);

    return VOS_FALSE;
}


VOS_UINT32 IMSA_Domain_ProcMsccStopReq
(
    const VOS_VOID                     *pRcvMsg
)
{
    IMSA_CONTROL_MANAGER_STRU *pstControlManager;

    IMSA_INFO_LOG("IMSA_ProcMsccMsgStopReq: enter!");
    TLPS_PRINT2LAYER_INFO(IMSA_Domain_ProcMsccStopReq_ENUM, LNAS_ENTRY);

    pstControlManager = IMSA_GetControlManagerAddress();

    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStopReq begin with state: DomainMode = ",IMSA_GetDomainMode());
    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStopReq begin with state: enDomainPrefer = ",IMSA_GetImsDomainPrefer());
    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStopReq begin with state: cellular support = ",IMSA_GetCellularImsSupportFlag());
    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStopReq begin with state: wifi support = ",IMSA_GetWifiImsSupportFlag());
    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStopReq begin with state: enImsaCellularStatus = ",pstControlManager->enImsaCellularStatus);
    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStopReq begin with state: enImsawifiStatus = ",pstControlManager->enImsaWifiStatus);
    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStopReq begin with state: enImsaStatus = ",pstControlManager->enImsaStatus);

    /* SRVCC异常(例如关机，DEREG REQ，状态迁离CONN+REG)，清缓存 */
    IMSA_SrvccAbormalClearBuff(IMSA_SRVCC_ABNORMAL_STOP_REQ);

    /* 如果是整机关机，记录Cellular关机状态 */
    /* WIFI正处于关机状态, 不需要清除WIFI标志,同时为避免冲突,设置Cellular为关机中 */
    if( IMSA_STATUS_STOPING == pstControlManager->enImsaWifiStatus)
    {
        pstControlManager->enImsaCellularStatus = IMSA_STATUS_STOPING;
    }
    else
    {
        pstControlManager->enImsaCellularStatus = IMSA_STATUS_NULL;
        pstControlManager->enImsaWifiStatus = IMSA_STATUS_NULL;
    }


    /*判断状态，如果已关机，则回复关机成功*/
    if(pstControlManager->enImsaStatus == IMSA_STATUS_NULL)
    {
        IMSA_SndMsccMsgStopCnf();
        return VOS_FALSE;
    }

    /*已处于关机状态，则丢弃*/
    if(IMSA_STATUS_STOPING == pstControlManager->enImsaStatus)
    {
        IMSA_WARN_LOG("IMSA_Domain_ProcMsccStopReq: Status is Stoping!");
        TLPS_PRINT2LAYER_WARNING(IMSA_Domain_ProcMsccStopReq_ENUM, IMSA_PRINT_STATUS_STOPING);
        return VOS_FALSE;
    }


    /* 如果是整机关机，则直接停止IMS SDK */

    IMSA_Domain_StopImsStack();

    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStopReq end with state: DomainMode = ",IMSA_GetDomainMode());
    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStopReq end with state: enDomainPrefer = ",IMSA_GetImsDomainPrefer());
    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStopReq end with state: cellular support = ",IMSA_GetCellularImsSupportFlag());
    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStopReq end with state: wifi support = ",IMSA_GetWifiImsSupportFlag());
    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStopReq end with state: enImsaCellularStatus = ",pstControlManager->enImsaCellularStatus);
    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStopReq end with state: enImsawifiStatus = ",pstControlManager->enImsaWifiStatus);
    IMSA_INFO_LOG1("IMSA_Domain_ProcMsccStopReq end with state: enImsaStatus = ",pstControlManager->enImsaStatus);
    return VOS_FALSE;

}


/*****************************************************************************
 Function Name  : IMSA_Domain_ProcMsccImsDomainCfgSetReq()
 Description    : MSCC IMS优先域设置消息处理函数
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-10-19  Draft Enact
*****************************************************************************/
VOS_UINT32 IMSA_Domain_ProcMsccImsDomainCfgSetReq
(
    const VOS_VOID                     *pRcvMsg
)
{
    MSCC_IMSA_IMS_DOMAIN_CFG_SET_REQ_STRU   *pstMsccImsaImsDomainCfgSetReq = VOS_NULL_PTR;
    IMSA_DOMAIN_MANAGER_STRU                *pstImsaDomainManager = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_Domain_ProcMsccImsDomainCfgSetReq:enter!");
    TLPS_PRINT2LAYER_INFO(IMSA_Domain_ProcMsccImsDomainCfgSetReq_ENUM, LNAS_ENTRY);

    pstMsccImsaImsDomainCfgSetReq = (MSCC_IMSA_IMS_DOMAIN_CFG_SET_REQ_STRU*) pRcvMsg;
    pstImsaDomainManager =  IMSA_Domain_GetDomainManagerAddr();

    /* 调用Domain函数保存优先域设置 */
    IMSA_Domain_SetImsDomainPreferFromSetMsg(pstMsccImsaImsDomainCfgSetReq->enImsDomain);

    /* 设置Domain域能力变迁 */
    (VOS_VOID)IMSA_Domain_SetDomainModeInMsccImsDomainCfgSetReq();

    /* 通知ERRC启动上报LTE信号质量 */
    if( (VOS_FALSE == pstImsaDomainManager->ucLteQualityCfgFlag)
        && ( IMSA_IMS_DOMAIN_MODE_DUAL == IMSA_GetDomainMode()))
    {
        IMSA_SendRrcQualityConfigReq(IMSA_LRRC_CFG_QUALITY_START);
        pstImsaDomainManager->ucLteQualityCfgFlag = VOS_TRUE;
    }

    /* 回复设置成功消息 */
    IMSA_SndMsccImsDomainCfgSetCnf();

    /* 设置域选初始化函数处理 */
    IMSA_Domain_InitDomainOpVar(UEPS_PID_MSCC,ID_MSCC_IMSA_IMS_DOMAIN_CFG_SET_REQ,VOS_TRUE,VOS_TRUE);

    /* 调用域选函数,根据当前状态选择合适的域 */
    switch(IMSA_JudgeImsDomainC1())
    {
        case IMSA_IMS_DOMAIN_NULL:
            IMSA_ProcessNoDomainSelectInMsccImsDomainCfgSetReq(VOS_TRUE);
            break;

        case IMSA_IMS_DOMAIN_WIFI:
            IMSA_ProcessWifiOnlyDomainSelectInMsccImsDomainCfgSetReq(VOS_TRUE);
            break;

        case IMSA_IMS_DOMAIN_LTE:
            IMSA_ProcessLTEOnlyDomainSelectInMsccImsDomainCfgSetReq(VOS_TRUE);
            break;

        case IMSA_IMS_DOMAIN_LTE_WIFI:
            IMSA_ProcessDualDomainSelect(VOS_TRUE,VOS_TRUE);
            break;

        default:
            IMSA_WARN_LOG("IMSA_Domain_ProcMsccImsDomainCfgSetReq: DomainC1Judge Wrong!");
            TLPS_PRINT2LAYER_WARNING(IMSA_Domain_ProcMsccImsDomainCfgSetReq_ENUM, LNAS_StateInvalid);
    }

    /* 如果需要延迟处理消息,记录事件,等待SRV上报状态,然后重新启动域选过程 */
    if(VOS_TRUE == pstImsaDomainManager->ucMsgDelayProcessFlag)
    {
       /* 保存当前需要延迟处理的域选择事件 */
       IMSA_Domain_SaveDelaySwitchEvent(UEPS_PID_MSCC,ID_MSCC_IMSA_IMS_DOMAIN_CFG_SET_REQ);

       /* 向SRV模块注册状态上报事件, SRV处于稳态将上报Ready_ind消息,发起新的域选择事件 */
       IMSA_SRV_RegStatusNtfEvent();

       /* 清除Delay事件 */
       pstImsaDomainManager->ucMsgDelayProcessFlag = VOS_FALSE;
    }

    return VOS_FALSE;
}



/*****************************************************************************
 Function Name  : IMSA_ProcMsccServiceChangeIndNoRsp()
 Description    : MSCC Service Change Ind消息处理函数
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-10-19  Draft Enact
*****************************************************************************/
VOS_UINT32 IMSA_Domain_ProcMsccServiceChangeInd
(
    const VOS_VOID                     *pRcvMsg
)
{
    MSCC_IMSA_SERVICE_CHANGE_IND_STRU  *pstMsccServiceChangeInd = VOS_NULL_PTR;
    IMSA_DOMAIN_MANAGER_STRU           *pstDomainManager = VOS_NULL_PTR;
    IMSA_CONTROL_MANAGER_STRU          *pstControlManager = VOS_NULL_PTR;
    MSCC_IMSA_IMS_VOICE_CAP_ENUM_UINT8  enImsVoiceCap = MSCC_IMSA_IMS_VOICE_CAP_BUTT;

    pstDomainManager    = IMSA_Domain_GetDomainManagerAddr();
    pstControlManager   = IMSA_GetControlManagerAddress();
    pstMsccServiceChangeInd = (MSCC_IMSA_SERVICE_CHANGE_IND_STRU*)pRcvMsg;
    TLPS_PRINT2LAYER_INFO(IMSA_Domain_ProcMsccServiceChangeInd_ENUM, LNAS_ENTRY);


    /* 当当前网络为LTE，且PS状态正常，且网络支持VOPS 业务， 则确认支持LTE */
    if( VOS_TRUE == IMSA_Domain_IsNrmSrvConditonSatisfied(pstMsccServiceChangeInd))
    {
        /* 设置支持LTE域 */
        IMSA_Domain_SetLteEnable(pstDomainManager->enImsaAvailDomain);
    }
    else
    {
        /* 否则清除支持LTE域 */
        IMSA_Domain_SetLteDisable(pstDomainManager->enImsaAvailDomain);
    }

    /* 调用SRV保存Wifi State Ind中的网络参数 */
    IMSA_SRV_SaveServiceChangeInd(pstMsccServiceChangeInd);

    /* 设置VoLTE控制VoWIFI参数 */
    IMSA_Domain_SetVolteCtrlVowifi(pstMsccServiceChangeInd);

    /*这里删除wifi only状态下，收到service change ind消息，上报语音能力的处理，
       改到在该函数最后统一进行处理*/

    /* 设置域选初始化函数处理 */
    IMSA_Domain_InitDomainOpVar(UEPS_PID_MSCC,ID_MSCC_IMSA_SERVICE_CHANGE_IND,VOS_TRUE,VOS_TRUE);

    /* 调用域选函数,根据当前状态选择合适的域 */
    switch(IMSA_JudgeImsDomainC1())
    {
        case IMSA_IMS_DOMAIN_NULL:
            IMSA_ProcessNoDomainSelectInMsccServiceChangeInd(VOS_TRUE);
            break;

        case IMSA_IMS_DOMAIN_WIFI:
            IMSA_ProcessWifiOnlyDomainSelectInMsccServiceChangeInd(VOS_TRUE);
            break;

        case IMSA_IMS_DOMAIN_LTE:
            IMSA_ProcessLTEOnlyDomainSelectInMsccServiceChangeInd(VOS_TRUE);
            break;

        case IMSA_IMS_DOMAIN_LTE_WIFI:
            IMSA_ProcessDualDomainSelect(VOS_TRUE,VOS_TRUE);
            break;

        default:
            IMSA_WARN_LOG("IMSA_Domain_ProcMsccServiceChangeInd: DomainC1Judge Wrong!");
            TLPS_PRINT2LAYER_WARNING(IMSA_Domain_ProcMsccServiceChangeInd_ENUM, LNAS_StateInvalid);
    }

    /* 如果需要延迟处理消息,记录事件,等待SRV上报状态,然后重新启动域选过程 */
    if(VOS_TRUE == pstDomainManager->ucMsgDelayProcessFlag)
    {
       /* 保存当前需要延迟处理的域选择事件 */
       IMSA_Domain_SaveDelaySwitchEvent(UEPS_PID_MSCC,ID_MSCC_IMSA_SERVICE_CHANGE_IND);

       /* 向SRV模块注册状态上报事件, SRV处于稳态将上报Ready_ind消息,发起新的域选择事件 */
       IMSA_SRV_RegStatusNtfEvent();

       /* 清除Delay事件 */
       pstDomainManager->ucMsgDelayProcessFlag = VOS_FALSE;
    }

    /*IMSA在收到service change ind消息后，如果没有发生域选，需要通知MSCC语音能力*/
    if ((IMSA_TRUE == IMSA_SRV_GetImsVoiceCapability(&enImsVoiceCap))&& (VOS_FALSE == pstDomainManager->ucDomainSelectFlag))
    {
        IMSA_SndMsccMsgImsVoiceCapNotify(enImsVoiceCap, IMSA_CONN_IsExitPersistentBearer());
    }

    /*在函数的最后，清除IMSA收到本次service change ind消息以后本地保存的LTE的注册类型和有效标志位*/
    pstControlManager->bitRegSuccProcTypeValid = IMSA_OP_FALSE;
    pstControlManager->enRegSuccProcType       = IMSA_MSCC_REG_PROC_BUTT;


    return VOS_FALSE;
}

/*****************************************************************************
 Function Name  : IMSA_Domain_ProcLrrcStateInd()
 Description    : Lrrc State Ind消息处理函数
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-10-19  Draft Enact
*****************************************************************************/
VOS_UINT32 IMSA_Domain_ProcLrrcStateInd
(
    const VOS_VOID                     *pRcvMsg
)
{
    IMSA_DOMAIN_MANAGER_STRU           *pstDomainManager    = VOS_NULL_PTR;
    LRRC_IMSA_STATE_IND_STRU           *pLrrcStateInd       = VOS_NULL_PTR;
    PS_MSG_HEADER_STRU                 *pHeader             = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_Domain_ProcLrrcStateInd: ENTER!");
    TLPS_PRINT2LAYER_INFO(IMSA_Domain_ProcLrrcStateInd_ENUM, LNAS_ENTRY);

    pLrrcStateInd = (LRRC_IMSA_STATE_IND_STRU *) pRcvMsg;
    pHeader = (PS_MSG_HEADER_STRU *) pRcvMsg;
    pstDomainManager = IMSA_Domain_GetDomainManagerAddr();

    /* 保存LTE信号强度 */
    if( (pLrrcStateInd->enLteQuality < LTE_IMSA_QUALITY_LEVEL_BUTT)
        &&( VOS_TRUE == pstDomainManager->ucLteQualityCfgFlag))
    {
        pstDomainManager->enLteQuality = pLrrcStateInd->enLteQuality;
    }
    else
    {
        pstDomainManager->enLteQuality = IMSA_WIRELESS_QUALITY_LEVEL_BAD;
    }

    /* 当前能力不支持双域，不处理LRRC信号强度消息，直接返回 */
    if( IMSA_IMS_DOMAIN_MODE_DUAL != IMSA_GetDomainMode())
    {
        IMSA_WARN_LOG("IMSA_Domain_ProcLrrcStateInd: No Dual Domain Enter, no process!");
        return VOS_FALSE;
    }

    /* 设置域选初始化函数处理 */
    IMSA_Domain_InitDomainOpVar(PS_PID_ERRC,ID_LRRC_IMSA_STATE_IND,VOS_TRUE,VOS_TRUE);

    /* 调用域选函数,根据当前状态选择合适的域 */
    switch(IMSA_JudgeImsDomainC1())
    {
        case IMSA_IMS_DOMAIN_NULL:
        case IMSA_IMS_DOMAIN_WIFI:
        case IMSA_IMS_DOMAIN_LTE:
            IMSA_WARN_LOG("IMSA_Domain_ProcLrrcStateInd: No Process in This State");
            return VOS_FALSE;

        case IMSA_IMS_DOMAIN_LTE_WIFI:
            IMSA_ProcessDualDomainSelectInLrrcStateInd(VOS_TRUE,VOS_TRUE);
            break;

        default:
            IMSA_WARN_LOG("IMSA_Domain_ProcLrrcStateInd: DomainC1Judge Wrong!");
            return VOS_FALSE;
    }

    /* 如果需要延迟处理消息,记录事件,等待SRV上报状态,然后重新启动域选过程 */
    if(VOS_TRUE == pstDomainManager->ucMsgDelayProcessFlag)
    {
       /* 保存当前需要延迟处理的域选择事件 */
       IMSA_Domain_SaveDelaySwitchEvent(pHeader->ulSenderPid,pHeader->ulMsgName);

       /* 向SRV模块注册状态上报事件, SRV处于稳态将上报Ready_ind消息,发起新的域选择事件 */
       IMSA_SRV_RegStatusNtfEvent();

       /* 清除Delay事件 */
       pstDomainManager->ucMsgDelayProcessFlag = VOS_FALSE;
    }

    IMSA_INFO_LOG1("IMSA_Domain_ProcLrrcStateInd: domain = ",IMSA_GetDomainMode());
    return VOS_FALSE;
}

/*****************************************************************************
 Function Name  : IMSA_Domain_ProcLrrcQualityConfigCnf()
 Description    : Lrrc QualityConfigCnf处理函数
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2015-10-19  Draft Enact
*****************************************************************************/
VOS_UINT32 IMSA_Domain_ProcLrrcQualityConfigCnf
(
    const VOS_VOID                     *pRcvMsg
)
{
     IMSA_INFO_LOG("IMSA_Domain_ProcLrrcQualityConfigCnf: ENTER!");

     return VOS_FALSE;
}


/*****************************************************************************
 Function Name  : IMSA_Domain_ProcWifiErrorInd()
 Description    : Lrrc Error Ind消息处理函数
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.Zhangdongfeng 353461        2016-01-14  Draft Enact
*****************************************************************************/

VOS_UINT32 IMSA_Domain_ProcWifiErrorInd(const VOS_VOID                *pRcvMsg)
{
    IMSA_DOMAIN_MANAGER_STRU           *pstDomainManager    = VOS_NULL_PTR;
    AT_IMSA_IMS_CTRL_MSG_STRU          *pAtWifiImsaCtrlMsg  = VOS_NULL_PTR;
    WIFI_IMSA_ERROR_IND_STRU           *pWifiErrorInd       = VOS_NULL_PTR;
    IMSA_WIFI_NETWORK_INFO_STRU        *pstWifiNetInfo = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_Domain_ProcWifiErrorInd: ENTER!");
    TLPS_PRINT2LAYER_INFO(IMSA_Domain_ProcWifiErrorInd_ENUM, LNAS_ENTRY);

    pAtWifiImsaCtrlMsg = (AT_IMSA_IMS_CTRL_MSG_STRU *)pRcvMsg;
    pWifiErrorInd = (WIFI_IMSA_ERROR_IND_STRU *) (pAtWifiImsaCtrlMsg->aucWifiMsg);
    pstDomainManager = IMSA_Domain_GetDomainManagerAddr();
    pstWifiNetInfo                          = IMSA_GetWifiNetInfoAddress();

    /* 设置PsService为NO_SERVICE，不能再发起业务 */
    if( WIFI_IMSA_ERROR_CAUSE_LINK_LOST == pWifiErrorInd->enCause)
    {
        pstWifiNetInfo->enImsaPsServiceStatus = IMSA_PS_SERVICE_STATUS_NO_SERVICE;
        pstDomainManager->enWifiQuality = IMSA_WIRELESS_QUALITY_LEVEL_BAD;

    }


    /* 当前能力不支持双域，不处理ERROR_IND消息，直接返回 */
    if( IMSA_IMS_DOMAIN_MODE_DUAL != IMSA_GetDomainMode())
    {
        IMSA_WARN_LOG("IMSA_Domain_ProcWifiErrorInd: No Dual Domain Enter, no process!");
        return VOS_TRUE;
    }

    if( IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN != pstDomainManager->enDomainFsmL2State)
    {
        IMSA_WARN_LOG("IMSA_Domain_ProcWifiErrorInd: It is no WIFI Domain, no process!");
        return VOS_TRUE;
    }

    if(((WIFI_IMSA_ERROR_CAUSE_LINK_LOST == pWifiErrorInd->enCause)
        || (WIFI_IMSA_ERROR_CAUSE_TUNNEL_EXCEPTION == pWifiErrorInd->enCause)
        || (WIFI_IMSA_ERROR_CAUSE_NW_RELEASE_IND == pWifiErrorInd->enCause))
         &&(VOS_FALSE == IMSA_IsImsExist()))
    {
       /*如果原因值是链路丢失、隧道异常和网侧释放，且当前没有IMS业务存在，在Domain模块不进行处理，直接去conn
         模块处理*/
       IMSA_WARN_LOG("IMSA_Domain_ProcWifiErrorInd: no WIFI->LTE HO!");
       return VOS_TRUE;
    }


    /* WIFI LINK/TUNNEL Abnormal, and set WIFI Signal to BAD */
     /*lint -e539*/
     pstDomainManager->enWifiQuality = IMSA_WIRELESS_QUALITY_LEVEL_BAD;

    /* 设置域选初始化函数处理 */
    IMSA_Domain_InitDomainOpVar(WUEPS_PID_MAPCON,ID_WIFI_IMSA_STATE_IND,VOS_TRUE,VOS_TRUE);
    /*lint +e539*/

    /* 调用域选函数,根据当前状态选择合适的域 */
    switch(IMSA_JudgeImsDomainC1())
    {
        case IMSA_IMS_DOMAIN_NULL:
        case IMSA_IMS_DOMAIN_WIFI:
        case IMSA_IMS_DOMAIN_LTE:
            IMSA_WARN_LOG("IMSA_Domain_ProcLrrcStateInd: No Process in This State");
            return VOS_TRUE;

        case IMSA_IMS_DOMAIN_LTE_WIFI:
            IMSA_ProcessDualDomainSelectInWifiErrorInd(VOS_FALSE,VOS_TRUE);
            break;

        default:
            IMSA_WARN_LOG("IMSA_Domain_ProcLrrcStateInd: DomainC1Judge Wrong!");
            return VOS_TRUE;
    }


    /* 如果需要其他模块继续处理, 则返回TRUE */
    if(VOS_TRUE == pstDomainManager->ucDomainSelectFlag)
    {
        /* 不需要CONN继续处理 */
        return VOS_FALSE;
    }
    else
    {
        return VOS_TRUE;
    }
}

/*****************************************************************************
     Function Name  : IMSA_Domain_ProcUsimReadFileCnf()
     Description    : USIM ReadFile Cnf消息处理函数
     Input          : VOS_VOID
     Output         : VOS_VOID
     Return Value   : VOS_VOID

     History        :
          1.Zhangdongfeng 353461        2015-10-19  Draft Enact
*****************************************************************************/

VOS_UINT32 IMSA_Domain_ProcUsimReadFileCnf(const VOS_VOID                *pRcvMsg)
{
    IMSA_CONTROL_MANAGER_STRU       *pstControlManager;

    pstControlManager   = IMSA_GetControlManagerAddress();
    TLPS_PRINT2LAYER_INFO(IMSA_Domain_ProcUsimReadFileCnf_ENUM, LNAS_ENTRY);

    /*调用函数对读卡文件进行处理 */
    (VOS_VOID)IMSA_Domain_ProcReadIsimFileResult((VOS_VOID *)pRcvMsg);

    if((IMSA_OP_TRUE == pstControlManager->stImsaCommonInfo.stImsaUeId.bitOpImpi)
        && (IMSA_OP_TRUE == pstControlManager->stImsaCommonInfo.stImsaUeId.bitOpTImpu)
        && (IMSA_OP_TRUE == pstControlManager->stImsaCommonInfo.bitOpHomeNetDomainName))
    {
        /* 停止读卡文件等待定时器 */
        IMSA_StopTimer(&pstControlManager->stWaitReadfileCnfTimer);

        /* 设置域选初始化函数处理 */
        IMSA_Domain_InitDomainOpVar(WUEPS_PID_USIM,USIMM_READFILE_CNF,VOS_TRUE,VOS_TRUE);

        /* 调用域选函数,根据当前状态选择合适的域 */
        switch(IMSA_JudgeImsDomainC1())
        {
            case IMSA_IMS_DOMAIN_NULL:
                IMSA_WARN_LOG("IMSA_Domain_ProcUsimReadFileCnf: No Process in This State");
                break;

            case IMSA_IMS_DOMAIN_WIFI:
                IMSA_ProcessWifiOnlyDomainSelectInUsimFileReady(VOS_TRUE);
                break;

            case IMSA_IMS_DOMAIN_LTE:
                IMSA_ProcessLTEOnlyDomainSelectInUsimFileReady(VOS_TRUE);
                break;

            case IMSA_IMS_DOMAIN_LTE_WIFI:
                IMSA_ProcessDualDomainSelectInUsimFileReady(VOS_TRUE,VOS_TRUE);
                break;

            default:
                IMSA_WARN_LOG("IMSA_Domain_ProcUsimReadFileCnf: DomainC1Judge Wrong!");
                TLPS_PRINT2LAYER_WARNING(IMSA_Domain_ProcUsimReadFileCnf_ENUM, LNAS_StateInvalid);
        }
    }
    /* 需要将消息给其他模块继续处理 */
    return VOS_TRUE;
}


/*****************************************************************************
     Function Name  : IMSA_Domain_ProcIntraMsgSrvStatusInd()
     Description    : SRV Status Ind消息处理函数
     Input          : VOS_VOID
     Output         : VOS_VOID
     Return Value   : VOS_VOID

     History        :
          1.Zhangdongfeng 353461        2015-10-19  Draft Enact
*****************************************************************************/

VOS_VOID IMSA_Domain_ProcIntraMsgSrvStatusInd(const VOS_VOID                *pRcvMsg)
{
    IMSA_CONTROL_MANAGER_STRU           *pstControlManager = VOS_NULL;
    IMSA_DOMAIN_SRV_STATUS_IND_STRU     *pstSrvStatusInd = VOS_NULL;

    pstSrvStatusInd   = (IMSA_DOMAIN_SRV_STATUS_IND_STRU     *)pRcvMsg;
    pstControlManager = IMSA_GetControlManagerAddress();

    IMSA_INFO_LOG("IMSA_Domain_ProcIntraMsgSrvStatusInd: ENTER!");
    TLPS_PRINT2LAYER_INFO(IMSA_Domain_ProcIntraMsgSrvStatusInd_ENUM, LNAS_ENTRY);

    /* 如果处于WIFI关机中，回复关机成功，转状态到关机状态 */
    if(IMSA_STATUS_STOPING == pstControlManager->enImsaWifiStatus)
    {
        /* 指示SPM WIFI关机成功 */
//        IMSA_SndSpmImsSwitchStateInd(IMSA_SPM_IMS_SWITCH_STATE_OFF);

        /* 回复WIFI关机成功 */
        IMSA_SndWifiPowerOffCnf();
        pstControlManager->enImsaWifiStatus = IMSA_STATUS_NULL;
    }

    /* 如果处于Cellular关机中，回复关机成功，转状态到关机状态 */
    if(IMSA_STATUS_STOPING == pstControlManager->enImsaCellularStatus)
    {
        /* 回复Cellular关机成功 */
        IMSA_SndMsccMsgStopCnf();
        pstControlManager->enImsaCellularStatus = IMSA_STATUS_NULL;
    }

    if(IMSA_DOMAIN_SERVICE_STATUS_NO_SERVICE == pstSrvStatusInd->enDomainSrvStatus)
    {
        /* No Service下控制域选结果的状态控制 */
        IMSA_DomainSelectResultInNoSrv();
    }
    else if(IMSA_DOMAIN_SERVICE_STATUS_NORMAL_SERVICE == pstSrvStatusInd->enDomainSrvStatus)
    {
        /* Normal Service状态下域选结果的状态控制 */
        IMSA_DomainSelectResultInNrmSrv(pstSrvStatusInd);
    }
    else
    {
        IMSA_WARN_LOG("IMSA_Domain_ProcIntraMsgSrvStatusInd: Srv Status is Wrong!");
    }

}

/*****************************************************************************
     Function Name  : IMSA_Domain_ProcIntraMsgSrvReadyInd()
     Description    : SRV Ready Ind消息处理函数
     Input          : VOS_VOID
     Output         : VOS_VOID
     Return Value   : VOS_VOID

     History        :
          1.Zhangdongfeng 353461        2015-10-19  Draft Enact
*****************************************************************************/

VOS_VOID IMSA_Domain_ProcIntraMsgSrvReadyInd(const VOS_VOID *pRcvMsg)
{

    IMSA_DOMAIN_MANAGER_STRU *pstDomainManager = VOS_NULL;

    PS_MSG_HEADER_STRU       *pHeader = VOS_NULL_PTR;

    pHeader = (PS_MSG_HEADER_STRU *) pRcvMsg;
    pstDomainManager = IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_Domain_ProcIntraMsgSrvReadyInd_ENUM, LNAS_ENTRY);


    /* 根据延迟的消息, 设置域选初始化函数处理 */
    IMSA_Domain_InitDomainOpVarByDelayEvent();

    /* 调用域选函数,根据当前状态选择合适的域 */
    IMSA_DomainSelect();

    /* 如果需要延迟处理消息,记录事件,等待SRV上报状态,然后重新启动域选过程 */
    if( VOS_TRUE == pstDomainManager->ucMsgDelayProcessFlag)
    {
       /* 保存当前需要延迟处理的域选择事件 */
       IMSA_Domain_SaveDelaySwitchEvent(pHeader->ulSenderPid,pHeader->ulMsgName);

       /* 向SRV模块注册状态上报事件, SRV处于稳态将上报Ready_ind消息,发起新的域选择事件 */
       IMSA_SRV_RegStatusNtfEvent();
    }
    else
    {
        /* 清除保存需要延迟处理的域选择事件 */
        IMSA_Domain_ClearDelaySwitchEvent();
    }

    /* 如果函数SRV没有进行任何处理,为了驱动SRV当前的处理,调用Continue函数 */
    if( VOS_FALSE == pstDomainManager->ucDomainSelectFlag)
    {
        /* coverity[unchecked_value] */
        IMSA_SRV_DomainContinue(IMSA_GetCurrentDomain());
    }

    return;
}


/*****************************************************************************
     Function Name  : IMSA_Domain_ProcIntraMsgSrvReadyInd()
     Description    : IMSA_DOMAIN_SWITCH_RESULT_IND消息处理函数
     Input          : VOS_VOID
     Output         : VOS_VOID
     Return Value   : VOS_VOID

     History        :
          1.Zhangdongfeng 353461        2015-10-19  Draft Enact
*****************************************************************************/

VOS_VOID IMSA_Domain_ProcIntraMsgSwitchResultInd(const VOS_VOID *pRcvMsg)
{

    IMSA_DOMAIN_MANAGER_STRU *pstDomainManager = VOS_NULL;
    IMSA_DOMAIN_SWITCH_RESULT_IND_STRU *pSwitchResultInd;
    IMSA_CONTROL_MANAGER_STRU           *pstControlManager = VOS_NULL;

    pstDomainManager = IMSA_Domain_GetDomainManagerAddr();
    pSwitchResultInd = (IMSA_DOMAIN_SWITCH_RESULT_IND_STRU *)pRcvMsg;
    pstControlManager = IMSA_GetControlManagerAddress();

    IMSA_INFO_LOG("IMSA_Domain_ProcIntraMsgSwitchResultInd: ENTER!");
    TLPS_PRINT2LAYER_INFO(IMSA_Domain_ProcIntraMsgSwitchResultInd_ENUM, LNAS_ENTRY);

    /* 关机导致的切换, 回关机回复 */

    /* 如果处于WIFI关机中，回复关机成功，转状态到关机状态 */
    if(IMSA_STATUS_STOPING == pstControlManager->enImsaWifiStatus)
    {
        /* 指示SPM WIFI关机成功 */
//        IMSA_SndSpmImsSwitchStateInd(IMSA_SPM_IMS_SWITCH_STATE_OFF);

        /* 回复WIFI关机成功 */
        IMSA_SndWifiPowerOffCnf();
        pstControlManager->enImsaWifiStatus = IMSA_STATUS_NULL;
    }

    /* 如果处于Cellular关机中，回复关机成功，转状态到关机状态 */
    if(IMSA_STATUS_STOPING == pstControlManager->enImsaCellularStatus)
    {
        /* 回复Cellular关机成功 */
        IMSA_SndMsccMsgStopCnf();
        pstControlManager->enImsaCellularStatus = IMSA_STATUS_NULL;
    }

    /*　如果当前处于L2W切换状态，且与上报切换结果消息一致，则跳出切换状态 */
    if( (IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH == pstDomainManager->enDomainFsmL2State)
        && ( IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH == pSwitchResultInd->enSwitchType))
    {
        IMSA_INFO_LOG("IMSA_Domain_ProcIntraMsgSwitchResultInd: 1 L2W");
        if( IMSA_DOMAIN_SWITCH_RESULT_SUCCESS == pSwitchResultInd->enSwitchResult)
        {
            pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING;
        }
        else
        {
            pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING;

            /* 启动WIFI域惩罚定时器 */
            /*lint -e734*/
            pstDomainManager->ucPunishTimeInWifiDomain = pstDomainManager->ucPunishTimeInWifiDomain * 2;
            IMSA_Domain_StartTimer1WifiDomain();
            /*lint +e734*/
        }
    }
    /*　如果当前处于W2L切换状态，且与上报切换结果消息一致，则跳出切换状态 */
    else if( (IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH == pstDomainManager->enDomainFsmL2State)
        && ( IMSA_DOMAIN_SWITCH_TYPE_W2L_SWITCH == pSwitchResultInd->enSwitchType))
    {
        IMSA_INFO_LOG("IMSA_Domain_ProcIntraMsgSwitchResultInd: 2 W2L");
        if( IMSA_DOMAIN_SWITCH_RESULT_SUCCESS == pSwitchResultInd->enSwitchResult)
        {
            pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING;
        }
        else
        {
            pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING;

            /* 启动LTE域惩罚定时器 */
            pstDomainManager->ucPunishTimeInLteDomain = (VOS_UINT8)(pstDomainManager->ucPunishTimeInLteDomain * 2);
            IMSA_Domain_StartTimer1LteDomain();
        }
    }
    else
    {
        IMSA_INFO_LOG("IMSA_Domain_ProcIntraMsgSwitchResultInd: 3 Error");
        /*  L2 FSM状态和消息内容不一致，则返回空闲状态　*/
        pstDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_NULL;
    }

    return;
}

/*****************************************************************************
     Function Name  : IMSA_Domain_ProcIntraMsgBearerRelInd()
     Description    : IMSA_DOMAIN_BEARER_REL_IND消息处理函数
     Input          : VOS_VOID
     Output         : VOS_VOID
     Return Value   : VOS_VOID

     History        :
          1.Zhangdongfeng 353461        2015-12-25  Draft Enact
*****************************************************************************/

VOS_VOID IMSA_Domain_ProcIntraMsgBearerRelInd(const VOS_VOID *pRcvMsg)
{

    TLPS_PRINT2LAYER_INFO(IMSA_Domain_ProcIntraMsgBearerRelInd_ENUM, LNAS_ENTRY);
    /* 当前能力不支持双域，不处理承载异常消息，由SRV启动周期定时器处理，直接返回 */
    if( IMSA_IMS_DOMAIN_MODE_DUAL != IMSA_GetDomainMode())
    {
        IMSA_WARN_LOG("IMSA_Domain_ProcIntraMsgBearerRelInd: No Dual Domain Enter, no process!");
        return ;
    }

    /* 调用域选函数,根据当前状态选择合适的域 */
    switch(IMSA_JudgeImsDomainC1())
    {
        case IMSA_IMS_DOMAIN_NULL:
        case IMSA_IMS_DOMAIN_WIFI:
        case IMSA_IMS_DOMAIN_LTE:
            IMSA_WARN_LOG("IMSA_Domain_ProcIntraMsgBearerRelInd: No Process in This State");
            return ;
        case IMSA_IMS_DOMAIN_LTE_WIFI:
            IMSA_ProcessDualDomainSelectInBearerRelInd(VOS_TRUE,VOS_TRUE);
            break;

        default:
            IMSA_WARN_LOG("IMSA_Domain_ProcIntraMsgBearerRelInd: DomainC1Judge Wrong!");
            return ;
    }

    return ;

}


VOS_UINT32 IMSA_Domain_ProcMsccMsgRegReq
(
    const VOS_VOID                     *pRcvMsg
)
{
    IMSA_CONTROL_MANAGER_STRU          *pstControlManager;

    (VOS_VOID)pRcvMsg;

    /* 清除去注册原因值 */
    IMSA_SetDeregCause(IMSA_SRV_DEREG_CAUSE_BUTT);

    pstControlManager = IMSA_GetControlManagerAddress();

    /* 停止去注册等待定时器 */
    IMSA_StopTimer(&pstControlManager->stWaitDeregTimer);

    /* NV由GUNAS修改，IMSA只需要把NV对应的全局变量修改了即可。 */
    pstControlManager->stImsaConfigPara.ucWifiImsSupportFlag = VOS_TRUE;
    pstControlManager->stImsaConfigPara.ucLteImsSupportFlag  = VOS_TRUE;

    /* 与GUNAS沟通打开IMS动态开关是, 在收到AP侧的IMSSWITCH=1时, 并不会修改Voice Domain,
       而是在收到AP侧修改Voice Domain AT命令时才会修改, 解决:打开IMS开关时, 由于IMSA提前
       修改了Voice Domain, 与此同时MSCC下发Service Change Ind, 从而导致提前触发IMS信令承载
       建立, 进而出现于LMM的TAU流程对冲 */

    /*  设置域管理模块状态机迁移  */
    if( ((IMSA_IMS_DOMAIN_PREFER_CELLULAR_PREFER == IMSA_GetImsDomainPrefer())
          ||(IMSA_IMS_DOMAIN_PREFER_WIFI_PREFER == IMSA_GetImsDomainPrefer()))
        && (VOS_TRUE == IMSA_Domain_IsDualDomainExist()))
    {
        /* 如果NV和开机状态支持双域,且IMS优先关系支持双域, 设置IMSA域状态为 Dual Domain */
        IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_DUAL);
        IMSA_INFO_LOG("IMSA_Domain_ProcMsccMsgRegReq: 1_DomainMode is IMSA_IMS_DOMAIN_MODE_DUAL!");
        TLPS_PRINT2LAYER_INFO(IMSA_Domain_ProcMsccMsgRegReq_ENUM, LNAS_FUNCTION_LABEL1);
    }
    else if( ((IMSA_IMS_DOMAIN_PREFER_WIFI_PREFER == IMSA_GetImsDomainPrefer())
              || (IMSA_IMS_DOMAIN_PREFER_CELLULAR_PREFER == IMSA_GetImsDomainPrefer())
              || (IMSA_IMS_DOMAIN_PREFER_WIFI_ONLY == IMSA_GetImsDomainPrefer()))
             &&(VOS_TRUE == IMSA_Domain_IsWifiDomainExist()))
    {
        /* 如果NV和开机状态支持WIFI域,且IMS优先关系支持WIFI域,设置IMSA域状态为 WIFI ONLY */
        IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_WIFI_ONLY);
        IMSA_INFO_LOG("IMSA_Domain_ProcMsccMsgRegReq: 2_DomainMode is IMSA_IMS_DOMAIN_MODE_WIFI_ONLY!");
        TLPS_PRINT2LAYER_INFO(IMSA_Domain_ProcMsccMsgRegReq_ENUM, LNAS_FUNCTION_LABEL2);
    }
    else if( ((IMSA_IMS_DOMAIN_PREFER_CELLULAR_PREFER == IMSA_GetImsDomainPrefer())
               || (IMSA_IMS_DOMAIN_PREFER_WIFI_PREFER == IMSA_GetImsDomainPrefer())
               || (IMSA_IMS_DOMAIN_PREFER_CELLULAR_ONLY == IMSA_GetImsDomainPrefer()))
             &&(VOS_TRUE == IMSA_Domain_IsCellularDomainExist()))
    {
        /* 如果NV和开机状态支持Cellular域,且IMS优先关系支持Cellular域,设置IMSA域状态为 Cellular ONLY */
        IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY);
        IMSA_INFO_LOG("IMSA_Domain_ProcMsccMsgRegReq: 3_DomainMode is IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY!");
        TLPS_PRINT2LAYER_INFO(IMSA_Domain_ProcMsccMsgRegReq_ENUM, LNAS_FUNCTION_LABEL3);
    }
    else
    {
        /* 设置IMSA域状态为 NULL */
        IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_NULL);
        IMSA_INFO_LOG("IMSA_Domain_ProcMsccMsgRegReq: 4_DomainMode is IMSA_IMS_DOMAIN_MODE_NULL!");
        TLPS_PRINT2LAYER_INFO(IMSA_Domain_ProcMsccMsgRegReq_ENUM, LNAS_FUNCTION_LABEL4);
    }

    /*AT解锁*/
    IMSA_SndMsccMsgRegCnf();
    return VOS_FALSE;
}


VOS_UINT32 IMSA_Domain_ProcMsccMsgDeregReq
(
    const VOS_VOID                     *pRcvMsg
)
{
    IMSA_DOMAIN_MANAGER_STRU           *pstImsaDomainManager = VOS_NULL_PTR;
    IMSA_CONTROL_MANAGER_STRU          *pstControlManager;

    (VOS_VOID)pRcvMsg;

    pstControlManager = IMSA_GetControlManagerAddress();

    /* NV由GUNAS修改，IMSA只需要把NV对应的全局变量修改了即可。 */
    pstControlManager->stImsaConfigPara.ucWifiImsSupportFlag = VOS_FALSE;
    pstControlManager->stImsaConfigPara.ucLteImsSupportFlag  = VOS_FALSE;

    /* 与GUNAS沟通关闭IMS动态开关时, 在收到AP侧的IMSSWITCH=0时, 并不会修改Voice Domain,
       而是在收到AP侧修改Voice Domain AT命令时才会修改, 解决:打开IMS开关时, 由于IMSA提前
       修改了Voice Domain, 与此同时MSCC下发Service Change Ind, 从而导致提前触发IMS信令承载
       建立, 进而出现于LMM的TAU流程对冲 */

    /* SRVCC异常(例如关机，DEREG REQ，状态迁离CONN+REG)，清缓存 */
    IMSA_SrvccAbormalClearBuff(IMSA_SRVCC_ABNORMAL_DEREG_REQ);

    /* 保存去注册原因值 */
    IMSA_SetDeregCause(IMSA_SRV_DEREG_CAUSE_MSCC_DEREG_REQ);

    IMSA_SetDomainMode(IMSA_IMS_DOMAIN_MODE_NULL);

    pstImsaDomainManager =  IMSA_Domain_GetDomainManagerAddr();

    switch(pstImsaDomainManager->enDomainFsmL2State)
    {
        case IMSA_DOMAIN_FSM_L2_STATE_NULL:
            IMSA_SRV_ProcDeregReq(VOS_FALSE);
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN:
            IMSA_SRV_ProcDeregReq(VOS_FALSE);
            pstImsaDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN:
            IMSA_SRV_ProcDeregReq(VOS_FALSE);
            pstImsaDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH:
        case IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH:
            IMSA_SRV_ProcDeregReq(VOS_TRUE);
            pstImsaDomainManager->enDomainFsmL2State = IMSA_DOMAIN_FSM_L2_STATE_DISABLING;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING:
            IMSA_SRV_ProcDeregReq(VOS_FALSE);
            break;

        default:
            IMSA_WARN_LOG("IMSA_Domain_ProcMsccMsgDeregReq: DomainC2 FSM Status Wrong!");
            TLPS_PRINT2LAYER_WARNING(IMSA_Domain_ProcMsccMsgDeregReq_ENUM, LNAS_StateInvalid);
    }

    return VOS_FALSE;
}

/*****************************************************************************
 Function Name  : IMSA_GetCurrentDomain()
 Description    : 获得当前选择的IMS Domain
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   :
      返回当前已经选择的域:
               IMSA_IMS_DOMAIN_NULL: 没有可用域
               IMSA_IMS_DOMAIN_WIFI: 当前选择了WIFI,
               IMSA_IMS_DOMAIN_LTE :当前选择了LTE
 History        :
      1.Zhangdongfeng 353461        2015-10-19  Draft Enact
*****************************************************************************/
IMSA_IMS_DOMAIN_ENUM_UINT8 IMSA_GetCurrentDomain(void)
{
    IMSA_DOMAIN_MANAGER_STRU            *pstDomainManager = VOS_NULL_PTR;
    IMSA_IMS_DOMAIN_ENUM_UINT8           enCurrentDomain = IMSA_IMS_DOMAIN_NULL;

    pstDomainManager = IMSA_Domain_GetDomainManagerAddr();

    TLPS_PRINT2LAYER_INFO(IMSA_GetCurrentDomain_ENUM, LNAS_ENTRY);

    switch(pstDomainManager->enDomainFsmL2State)
    {
        case IMSA_DOMAIN_FSM_L2_STATE_NULL:
            enCurrentDomain = IMSA_IMS_DOMAIN_NULL;
            break;
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN:
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING:
            enCurrentDomain = IMSA_IMS_DOMAIN_LTE;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN:
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING:
            enCurrentDomain = IMSA_IMS_DOMAIN_WIFI;
            break;

        case IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH:
        case IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH:
        case IMSA_DOMAIN_FSM_L2_STATE_DISABLING:
            /* 切换过程中，判决SRV模块状态确认状态 */
            enCurrentDomain = IMSA_SRV_GetCurrentDomainInHo();
            break;

        default:
            break;
    }

    /* 当前只有LTE域支持紧急呼，因此只存在紧急SRV状态时，上报LTE域 */
    if( (IMSA_SRV_STATUS_IDLE_DEREG == IMSA_SRV_GetNormalSrvStatus())
        &&( IMSA_SRV_STATUS_IDLE_DEREG != IMSA_SRV_GetEmcSrvStatus()))
    {
        enCurrentDomain = IMSA_IMS_DOMAIN_LTE;
    }

    return enCurrentDomain;
}

/*****************************************************************************
 Function Name  : IMSA_IsImsDomainCap()
 Description    : 判断当前网络是否支持IMS Domain
 Input          :
                   IMSA_IMS_DOMAIN_WIFI: 当前选择了WIFI,
                   IMSA_IMS_DOMAIN_LTE :当前选择了LTE
 Output         : VOS_VOID
 Return Value   :
      返回选择的域是否存在: VOS_TRUE:存在 VOS_FALSE:不存在
 History        :
      1.Zhangdongfeng 353461        2015-10-19  Draft Enact
*****************************************************************************/

VOS_UINT32 IMSA_IsImsDomainCap(IMSA_IMS_DOMAIN_ENUM_UINT8 enImsaDomain)
{
    if( IMSA_IMS_DOMAIN_NULL == enImsaDomain)
    {
        return VOS_TRUE;
    }
    if(IMSA_GetDomainMode()>=IMSA_IMS_DOMAIN_BUTT)
    {
        return VOS_FALSE;
    }

    if(IMSA_GetDomainMode() == enImsaDomain)
    {
        return VOS_TRUE;
    }
    else if( (IMSA_IMS_DOMAIN_LTE_WIFI == IMSA_GetDomainMode())
             &&(  (IMSA_IMS_DOMAIN_WIFI == enImsaDomain)
                ||(IMSA_IMS_DOMAIN_LTE == enImsaDomain)))
    {
        return VOS_TRUE;
    }
    else
    {
        return VOS_FALSE;
    }
}

/*****************************************************************************
 Function Name  : IMSA_SndSpmImsSwitchStateInd()
 Description    : 发送当前WIFI开机状态给SPM
 Input          :  enSwitchState: IMSA_SPM_IMS_SWITCH_STATE_OFF/ON
 Output         : VOS_VOID
 Return Value   :
               无
 History        :
      1.Zhangdongfeng 353461        2015-11-17  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_SndSpmImsSwitchStateInd( IMSA_SPM_IMS_SWITCH_STATE_ENUM_UINT8 enSwitchState)
{

    IMSA_SPM_IMS_SWITCH_STATE_IND_STRU           *pstImsaSpmImsSwitchStateInd;

    /*分配空间并检验分配是否成功*/
    pstImsaSpmImsSwitchStateInd = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_SPM_IMS_SWITCH_STATE_IND_STRU));
    TLPS_PRINT2LAYER_INFO(IMSA_SndSpmImsSwitchStateInd_ENUM, LNAS_ENTRY);

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstImsaSpmImsSwitchStateInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndSpmImsSwitchStateInd:ERROR:Alloc Msg fail!");
        return;
    }

    /*清空*/
    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstImsaSpmImsSwitchStateInd),IMSA_GET_MSG_LENGTH(pstImsaSpmImsSwitchStateInd), 0, IMSA_GET_MSG_LENGTH(pstImsaSpmImsSwitchStateInd));

    /*填写消息头*/
    IMSA_WRITE_SPM_MSG_HEAD(pstImsaSpmImsSwitchStateInd, ID_IMSA_SPM_IMS_SWITCH_STATE_IND);

    /* 填写消息体 */
    pstImsaSpmImsSwitchStateInd->enImsSwitch = enSwitchState;

    /*调用消息发送函数 */
    IMSA_SND_MSG(pstImsaSpmImsSwitchStateInd);
}


/*****************************************************************************
 Function Name   : IMSA_Domain_PrintDomainFsmState
 Description     :
 Input           : None
 Output          : None
 Return          : VOS_INT32

 History         :
    1.zhangdongfeng    2016-03-02  Draft Enact

*****************************************************************************/
VOS_VOID IMSA_Domain_PrintDomainFsmState( VOS_VOID )
{
    IMSA_DOMAIN_MANAGER_STRU              *pstDomainManager   = VOS_NULL_PTR;
    /* 初始化紧急连接 */
    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();
    TLPS_PRINT2LAYER_INFO(IMSA_Domain_PrintDomainFsmState_ENUM, LNAS_ENTRY);

    switch(IMSA_GetDomainMode())
    {
        case IMSA_IMS_DOMAIN_MODE_NULL:
            IMSA_INFO_LOG("IMSA_Domain_PrintDomainFsmState Current Domain Cap: NULL");
            TLPS_PRINT2LAYER_INFO(IMSA_Domain_PrintDomainFsmState_ENUM, IMSA_DOMAIN_CAP_NULL);
            break;
        case IMSA_IMS_DOMAIN_MODE_WIFI_ONLY:
            IMSA_INFO_LOG("IMSA_Domain_PrintDomainFsmState Current Domain Cap: WIFI_ONLY");
            TLPS_PRINT2LAYER_INFO(IMSA_Domain_PrintDomainFsmState_ENUM, IMSA_DOMAIN_CAP_WIFIONLY);

            break;
        case IMSA_IMS_DOMAIN_MODE_CELLULAR_ONLY:
            IMSA_INFO_LOG("IMSA_Domain_PrintDomainFsmState Current Domain Cap: CELLULAR_ONLY");
            TLPS_PRINT2LAYER_INFO(IMSA_Domain_PrintDomainFsmState_ENUM, IMSA_DOMAIN_CAP_CELLULARONLY);

            break;
        case IMSA_IMS_DOMAIN_MODE_DUAL:
            IMSA_INFO_LOG("IMSA_Domain_PrintDomainFsmState Current Domain Cap: DUAL_DOMAIN");
            TLPS_PRINT2LAYER_INFO(IMSA_Domain_PrintDomainFsmState_ENUM, IMSA_DOMAIN_CAP_DUAL);
            break;
        default:
            IMSA_INFO_LOG("IMSA_Domain_PrintDomainFsmState Current Domain Cap: UNKNOW");
            TLPS_PRINT2LAYER_INFO(IMSA_Domain_PrintDomainFsmState_ENUM, IMSA_DOMAIN_CAP_UNKNOWN);
            break;
    }

    switch(pstDomainManager->enDomainFsmL2State)
    {
        case IMSA_DOMAIN_FSM_L2_STATE_NULL:
            IMSA_INFO_LOG("IMSA_Domain_PrintDomainFsmState Current Domain State: NULL");
            TLPS_PRINT2LAYER_INFO(IMSA_Domain_PrintDomainFsmState_ENUM, IMSA_DOMAIN_STATUS_NULL);
            break;
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING:
            IMSA_INFO_LOG("IMSA_Domain_PrintDomainFsmState Current Domain State: LTE_TRYING");
            TLPS_PRINT2LAYER_INFO(IMSA_Domain_PrintDomainFsmState_ENUM, IMSA_DOMAIN_STATUS_LTE);
            break;
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN:
            IMSA_INFO_LOG("IMSA_Domain_PrintDomainFsmState Current Domain State: LTE");
            TLPS_PRINT2LAYER_INFO(IMSA_Domain_PrintDomainFsmState_ENUM, IMSA_DOMAIN_STATUS_LTE);
            break;
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN_TRYING:
            IMSA_INFO_LOG("IMSA_Domain_PrintDomainFsmState Current Domain State: WIFI_TRYING");
            TLPS_PRINT2LAYER_INFO(IMSA_Domain_PrintDomainFsmState_ENUM, IMSA_DOMAIN_STATUS_WIFI);
            break;
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DOMAIN:
            IMSA_INFO_LOG("IMSA_Domain_PrintDomainFsmState Current Domain State: WIFI");
            TLPS_PRINT2LAYER_INFO(IMSA_Domain_PrintDomainFsmState_ENUM, IMSA_DOMAIN_STATUS_WIFI);
            break;
        case IMSA_DOMAIN_FSM_L2_STATE_L2W_SWITCH:
            IMSA_INFO_LOG("IMSA_Domain_PrintDomainFsmState Current Domain State: L2W_SWITCH");
            TLPS_PRINT2LAYER_INFO(IMSA_Domain_PrintDomainFsmState_ENUM, IMSA_DOMAIN_STATUS_L2W);
            break;
        case IMSA_DOMAIN_FSM_L2_STATE_W2L_SWITCH:
            IMSA_INFO_LOG("IMSA_Domain_PrintDomainFsmState Current Domain State: W2L_SWITCH");
            TLPS_PRINT2LAYER_INFO(IMSA_Domain_PrintDomainFsmState_ENUM, IMSA_DOMAIN_STATUS_W2L);
            break;
        case IMSA_DOMAIN_FSM_L2_STATE_DISABLING:
            IMSA_INFO_LOG("IMSA_Domain_PrintDomainFsmState Current Domain State: DISABLING");
            TLPS_PRINT2LAYER_INFO(IMSA_Domain_PrintDomainFsmState_ENUM, IMSA_DOMAIN_STATUS_DISABLING);
            break;
        case IMSA_DOMAIN_FSM_L2_STATE_LTE_DISABLING:
            IMSA_INFO_LOG("IMSA_Domain_PrintDomainFsmState Current Domain State: LTE_DISABLING");
            TLPS_PRINT2LAYER_INFO(IMSA_Domain_PrintDomainFsmState_ENUM, IMSA_DOMAIN_STATUS_DISABLING);
            break;
        case IMSA_DOMAIN_FSM_L2_STATE_WIFI_DISABLING:
            IMSA_INFO_LOG("IMSA_Domain_PrintDomainFsmState Current Domain State: WIFI_DISABLING");
            TLPS_PRINT2LAYER_INFO(IMSA_Domain_PrintDomainFsmState_ENUM, IMSA_DOMAIN_STATUS_DISABLING);
            break;
        default:
            IMSA_INFO_LOG("IMSA_Domain_PrintDomainFsmState Current Domain State: UNKNOWN");
            TLPS_PRINT2LAYER_INFO(IMSA_Domain_PrintDomainFsmState_ENUM, IMSA_DOMAIN_STATUS_UNKNOWN);
            break;
    }
}


VOS_UINT32 IMSA_Domain_ProcAreaaLostInd
(
    const VOS_VOID                     *pRcvMsg
)
{
    IMSA_DOMAIN_MANAGER_STRU    *pstDomainManager = VOS_NULL_PTR;

    pstDomainManager      = IMSA_Domain_GetDomainManagerAddr();

    /* 如果由WIFI域的时候，丢网前，电话会切换到WIFI下，不需要处理；仅在只有L
    域的时候，才处理该消息 */
    if (IMSA_IMS_DOMAIN_LTE == IMSA_JudgeImsDomainC1())
    {
        return IMSA_TRUE;
    }
    else if (IMSA_IMS_DOMAIN_LTE_WIFI == IMSA_JudgeImsDomainC1())
    {
        if ((IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN == pstDomainManager->enDomainFsmL2State) ||
            (IMSA_DOMAIN_FSM_L2_STATE_LTE_DOMAIN_TRYING == pstDomainManager->enDomainFsmL2State))
        {
            return IMSA_TRUE;
        }
    }
    else
    {
        return IMSA_FALSE;
    }
    return IMSA_FALSE;
}

#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
/* end of ImsaProcApsMsg.c*/

