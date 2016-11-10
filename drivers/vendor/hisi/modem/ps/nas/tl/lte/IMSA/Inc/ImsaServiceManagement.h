

#ifndef __IMSASERVICEMANAGEMENT_H__
#define __IMSASERVICEMANAGEMENT_H__

/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include "ImsaMsccInterface.h"
#include "ImsaIntraInterface.h"
#include "ImsaEntity.h"


/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
#pragma pack(*)    设置字节对齐方式
*****************************************************************************/
#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif

/*****************************************************************************
  2 macro
*****************************************************************************/
#define IMSA_PS_SIMM_INVALID                        (VOS_FALSE)
#define IMSA_DEREG_LOCAL                            (1)
#define IMSA_DEREG_NOT_LOCAL                        (0)

#ifdef PS_ITT_PC_TEST_NAS
#define TI_IMSA_START_OR_STOP_TIMER_LEN             (5*1000)
#define TI_IMSA_PERIOD_IMS_SRV_TRY_TIMER_LEN        (5*1000)
#define TI_IMSA_PERIOD_IMS_EMC_SRV_TRY_TIMER_LEN    (5*1000)
#define TI_IMSA_WAIT_DEREG_TIMER_LEN                (2*1000)
#define TI_IMSA_WAIT_READFILE_CNF_TIMER_LEN         (5*1000)
#define TI_IMSA_GET_SERVICE_PROTECT_TIMER_LEN       (15*1000)
#define TI_IMSA_DEREG_SERVICE_PROTECT_TIMER_LEN     (15*1000)

#else
#define TI_IMSA_START_OR_STOP_TIMER_LEN             (2*1000)
#define TI_IMSA_PERIOD_IMS_SRV_TRY_TIMER_LEN        (2*1000)
#define TI_IMSA_PERIOD_IMS_EMC_SRV_TRY_TIMER_LEN    (2*1000)
#define TI_IMSA_WAIT_DEREG_TIMER_LEN                (2*1000)
#define TI_IMSA_WAIT_READFILE_CNF_TIMER_LEN         (5*1000)

#define TI_IMSA_GET_SERVICE_PROTECT_TIMER_LEN       (15*1000)
#define TI_IMSA_DEREG_SERVICE_PROTECT_TIMER_LEN     (15*1000)

#endif

/*****************************************************************************
  3 Massage Declare
*****************************************************************************/



/*****************************************************************************
  4 Enum
*****************************************************************************/
/*****************************************************************************
    枚举名    : IMSA_NORMAL_REG_STATUS_ENUM
    枚举说明  : 普通注册状态
*****************************************************************************/
enum IMSA_NORMAL_AT_REG_STATUS_ENUM
{
    IMSA_NORMAL_AT_REG_STATUS_DEREG        = 0x00,
    IMSA_NORMAL_AT_REG_STATUS_REG          = 0x01,

    IMSA_NORMAL_AT_REG_STATUS_BUTT
};
typedef VOS_UINT32 IMSA_NORMAL_AT_REG_STATUS_ENUM_UINT32;



/*****************************************************************************
  5 STRUCT
*****************************************************************************/
/*****************************************************************************
 结构名称: IMSA_SUBSCRIPTION_INFO_STRU
 结构说明: 订阅信息结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulPid;
    IMSA_SUBCRIBE_TYPE_ENUM_UINT8       enType;
    VOS_UINT32                          ulPara;
    VOS_UINT32                          ulSubscriptionId;
}IMSA_SUBSCRIPTION_INFO_STRU;

/*****************************************************************************
 结构名称: IMSA_SUBSCRIPTION_INFO_LIST_STRU
 结构说明: 订阅信息列表结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulSubscriptionNum;                      /**< 订阅数 */
    IMSA_SUBSCRIPTION_INFO_STRU         astSubcriptionInfoArray[IMSA_MAX_SUBSCRIPTION_NUM];/**< 订阅信息数组 */

    VOS_UINT32                          ulMaxSubscriptionId;                    /* 分配的最大的签约号 */
    IMSA_NORMAL_REG_STATUS_ENUM_UINT32  enLastNrmRegStatus;                     /* 存储上次普通注册状态 */
    IMSA_NORMAL_AT_REG_STATUS_ENUM_UINT32 enLastNrmAtRegStatus;                 /*存储上次普遍注册上报给AT的状态*/
}IMSA_SUBSCRIPTION_INFO_LIST_STRU;


/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/
extern IMSA_SUBSCRIPTION_INFO_LIST_STRU    g_stSubcriptionInfoList;

#define IMSA_GetSubcriptionInfoListAddr()               (&g_stSubcriptionInfoList)
#define IMSA_GetLastNrmRegStatus()                      (g_stSubcriptionInfoList.enLastNrmRegStatus)
#define IMSA_SetLastNrmRegStatus(enLastNrmRegStatus)    (IMSA_GetLastNrmRegStatus() = (enLastNrmRegStatus))
#define IMSA_GetLastNrmAtRegStatus()                      (g_stSubcriptionInfoList.enLastNrmAtRegStatus)
#define IMSA_SetLastNrmAtRegStatus(enLastNrmAtRegStatus)    (IMSA_GetLastNrmAtRegStatus() = (enLastNrmAtRegStatus))



/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/
extern VOS_VOID IMSA_SRV_ProcRegRegIndMsg
(
    const IMSA_REG_REG_IND_STRU        *pstRegRegIndMsg
);
extern VOS_VOID IMSA_SRV_ProcRegDeregIndMsg
(
    const IMSA_REG_DEREG_IND_STRU      *pstRegDeregIndMsg
);
extern VOS_VOID IMSA_SRV_ProcConnSetupIndMsg
(
    const IMSA_CONN_SETUP_IND_STRU     *pstConnSetupIndMsg
);
extern VOS_VOID IMSA_SRV_ProcConnRelIndMsg
(
    const IMSA_CONN_REL_IND_STRU       *pstConnRelIndMsg
);
extern VOS_VOID IMSA_SRV_ProcConnModifyIndMsg
(
    const IMSA_CONN_MODIFY_IND_STRU    *pstConnModifyIndMsg
);
extern VOS_VOID IMSA_SRV_ProcCallRsltActionIndMsg
(
    const IMSA_CALL_RESULT_ACTION_IND_STRU *pstCallRsltActionIndMsg
);
extern VOS_VOID IMSA_SRV_ProcCallSrvccSuccIndMsg
(
    const IMSA_CALL_SRVCC_SUCC_IND_STRU *pstCallSrvccSuccIndMsg
);

extern VOS_VOID IMSA_SRV_ProcServiceChangeInd
(
    const MSCC_IMSA_SERVICE_CHANGE_IND_STRU     *pstServiceChangeInd
);
extern VOS_VOID IMSA_CONTROL_Init( VOS_VOID );

extern VOS_VOID IMSA_ProcTimerMsgPeriodTryImsSrvExp(const VOS_VOID *pRcvMsg );
extern VOS_VOID IMSA_CONTROL_ClearResource( VOS_VOID );
extern VOS_VOID IMSA_SRV_ProcCampInfoChangeInd
(
    const MSCC_IMSA_CAMP_INFO_CHANGE_IND_STRU     *pstCampInfoChangeInd
);
extern VOS_VOID IMSA_SRV_PrintNrmSrvState( VOS_VOID );
extern VOS_VOID IMSA_SRV_PrintEmcSrvState( VOS_VOID );

extern VOS_VOID IMSA_SRV_SndNrmCallSrvStatusInd
(
    IMSA_CALL_SERVICE_STATUS_ENUM_UINT32       enCallSrvStatus,
    IMSA_CALL_NO_SRV_CAUSE_ENUM_UINT32         enNoSrvCause,
    IMSA_CALL_NORM_SRV_EXTEND_INFO_ENUM_UINT32 enNormSrvExtInfo
);

extern VOS_VOID IMSA_SRV_SndEmcCallSrvStatusInd
(
    IMSA_CALL_SERVICE_STATUS_ENUM_UINT32       enCallSrvStatus,
    IMSA_CALL_NO_SRV_CAUSE_ENUM_UINT32         enNoSrvCause
);
extern IMSA_NORMAL_AT_REG_STATUS_ENUM_UINT32 IMSA_SRV_GetAtRegStatusFromSrvStatus
(
    IMSA_SRV_STATUS_ENUM_UINT8          enSrvStatus
);
/* delete PeriodImsEmcSrvTryTimer */

extern VOS_UINT32 IMSA_SRV_IsHasActiveSipSignalPdp
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType
);

extern VOS_VOID IMSA_SRV_ProcVoiceDomainChangeInd
(
    const MSCC_IMSA_VOICE_DOMAIN_CHANGE_IND_STRU     *pstVoiceDomainChangeInd
);

extern VOS_VOID IMSA_SRV_ProcWhenRoamNotSatisfied(VOS_VOID);
extern VOS_UINT32 IMSA_SRV_GetImsVoiceCapability(MSCC_IMSA_IMS_VOICE_CAP_ENUM_UINT8 *penImsVoiceCap);

extern VOS_UINT32 IMSA_SRV_CheckSwitchPossible(VOS_VOID);
extern VOS_VOID IMSA_SRV_SndDomainSrvStatusInd
(
    IMSA_DOMAIN_SERVICE_STATUS_ENUM_UINT32       enDoaminSrvStatus,
    IMSA_DOMAIN_NO_SRV_CAUSE_ENUM_UINT32         enNoSrvCause,
    IMSA_DOMAIN_MODE_ENUM_UINT8                  enCurrentDomain
);
extern VOS_VOID IMSA_SRV_SndDomainSrvReadyInd( VOS_VOID );
extern IMSA_NRM_SRV_CON_SAT_STATUE_ENMUM_UINT32 IMSA_SRV_IsWifiNrmSrvConditonSatisfied( VOS_VOID );
extern VOS_VOID IMSA_SRV_ProcWifiMsgStateInd
(
    WIFI_IMSA_STATE_IND_STRU* pstStateInd
);
extern VOS_UINT32 IMSA_SRV_IsNrmSrvIsimParaSatisfied( VOS_VOID );
extern VOS_VOID IMSA_SRV_SaveWifiStateInd( const WIFI_IMSA_STATE_IND_STRU     *pstWifiStateInd);
extern VOS_VOID IMSA_SRV_SaveServiceChangeInd(    const MSCC_IMSA_SERVICE_CHANGE_IND_STRU     *pstServiceChangeInd);
extern VOS_UINT8 IMSA_GetCellularImsSupportFlag(VOS_VOID);
extern VOS_VOID IMSA_SRV_ProcDomainWifiDereg(VOS_VOID);


extern VOS_VOID IMSA_SRV_ProcWhenRoamIsSatisfied(VOS_VOID);

extern VOS_VOID IMSA_SRV_ProcGetSrvTimerExp
(
    const VOS_VOID                     *pMsg
);

extern VOS_VOID IMSA_SRV_ProcDeregSrvTimerExp
(
    const VOS_VOID                     *pMsg
);
extern VOS_VOID IMSA_SRV_ProcConnHoCnfMsg
(
    IMSA_CONN_HO_CNF_STRU* pstConnHoCnf
);

extern VOS_VOID IMSA_SRV_DomainSwitch
(
    IMSA_DOMAIN_SWITCH_TYPE_ENUM_UINT8 enSwitchType,
    VOS_UINT8   ucHoFailReturn2OrgPdnFlag,
    VOS_UINT8   ucOrgPdnRlsFlag
);
extern VOS_VOID IMSA_SRV_SetServcieStatus
(
    IMSA_SRV_TYPE_ENUM_UINT8            enSrvType,
    IMSA_SRV_STATUS_ENUM_UINT8          enSrvStatus
);

extern VOS_VOID IMSA_SRV_SndConnRelReq
(
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType,
    IMSA_CONN_SIP_PDP_TYPE_ENUM_UINT32      enSipPdpType,
    IMSA_IMS_DOMAIN_ENUM_UINT8               enConnRatType,
    IMSA_CONN_RELEASE_TYPE_ENUM_UINT8        enConnRlsType
);

extern VOS_VOID IMSA_SRV_SndConnHoReq
(
    IMSA_DOMAIN_SWITCH_TYPE_ENUM_UINT8  enSwitchType,
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_IMS_DOMAIN_ENUM_UINT8           enConnRatType,
    VOS_UINT8                           ucOrgPdnRlFlag
);
extern VOS_VOID IMSA_SRV_SndRegDeregReq
(
    IMSA_REG_TYPE_ENUM_UINT8            enDeRegType,
    VOS_UINT32                          ulLocalFlag
);
extern VOS_VOID IMSA_SRV_ProcGetSrvTimerExp
(
    const VOS_VOID                     *pMsg
);
extern VOS_VOID IMSA_SRV_ProcDeregSrvTimerExp
(
    const VOS_VOID                     *pMsg
);


extern VOS_VOID IMSA_ProcTimerMsgTempForbTimerExp(const VOS_VOID *pRcvMsg );

extern VOS_VOID IMSA_SRV_RegStatusNtfEvent(VOS_VOID);
extern VOS_VOID IMSA_SRV_DomainContinue
(
    IMSA_IMS_DOMAIN_ENUM_UINT8          enDomain
);
extern VOS_VOID IMSA_SRV_SndDomainSrvReadyInd(VOS_VOID);
extern VOS_VOID IMSA_SRV_ProcAreaLostInd
(
    VOS_VOID
);
#if (FEATURE_ON == FEATURE_PTM)
extern VOS_VOID IMSA_NotInitRegEventReport
(
    IMSA_ERR_LOG_NOT_INIT_IMS_REG_REASON_ENUM_UINT16    enNotInitRegReason
);
extern VOS_VOID IMSA_PdnConnTimerExpireEventReport
(
    VOS_VOID
);
extern VOS_VOID IMSA_RegTimerExpireEventReport
(
    VOS_VOID
);
extern VOS_VOID IMSA_SRV_RegSuccEventReport
(
    VOS_VOID
);
extern VOS_VOID IMSA_SRV_InitDeregEventReport
(
    IMSA_ERR_LOG_INIT_DEREG_REASON_ENUM_UINT32  enInitDeregCause
);
#endif

extern VOS_VOID IMSA_ProcTimerMsgWaitDeregTimerExp(const VOS_VOID *pRcvMsg );
extern VOS_VOID IMSA_ProcTimerMsgWaitReadfileCnfTimerExp(const VOS_VOID *pRcvMsg );
extern IMSA_IMS_DOMAIN_ENUM_UINT8 IMSA_SRV_GetCurrentDomainInHo(VOS_VOID);

/*****************************************************************************
  9 OTHERS
*****************************************************************************/


#if (VOS_OS_VER != VOS_WIN32)
#pragma pack()
#else
#pragma pack(pop)
#endif




#ifdef __cplusplus
    #if __cplusplus
            }
    #endif
#endif

#endif /* end of ImsaServiceManagement.h */




