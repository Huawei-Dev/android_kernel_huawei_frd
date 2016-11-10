

#ifndef __IMSADOMAINMANAGEMENT_H__
#define __IMSADOMAINMANAGEMENT_H__

/*****************************************************************************
  1 Include Headfile
*****************************************************************************/

#include    "vos.h"
#include    "ImsaPublic.h"
#include    "ImsaEntity.h"
#include    "LPsOm.h"
#include    "ImsaIntraInterface.h"


/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
#pragma pack(*)
*****************************************************************************/
#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif

/*****************************************************************************
  2 macro
*****************************************************************************/

#define IMSA_Domain_SetLteEnable(ImsaDomainFlag)     (ImsaDomainFlag) |= (VOS_UINT8)(IMSA_IMS_DOMAIN_LTE)
#define IMSA_Domain_SetLteDisable(ImsaDomainFlag)    (ImsaDomainFlag) &= (VOS_UINT8)(~IMSA_IMS_DOMAIN_LTE)
#define IMSA_Domain_SetWifiEnable(ImsaDomainFlag)    (ImsaDomainFlag) |= (VOS_UINT8)(IMSA_IMS_DOMAIN_WIFI)
#define IMSA_Domain_SetWifiDisable(ImsaDomainFlag)   (ImsaDomainFlag) &= (VOS_UINT8)(~IMSA_IMS_DOMAIN_WIFI)


/*****************************************************************************
  3 Massage Declare
*****************************************************************************/
extern VOS_VOID IMSA_Domain_Init(VOS_VOID);
extern VOS_VOID IMSA_Domain_ClearResource(VOS_VOID);
extern VOS_UINT32 IMSA_Domain_ProcWifiPowerOnReq( const VOS_VOID                     *pRcvMsg  );
extern VOS_UINT32 IMSA_Domain_ProcWifiPowerOffReq( const VOS_VOID                     *pRcvMsg);
extern VOS_UINT32 IMSA_Domain_ProcWifiStateInd(const VOS_VOID                     *pRcvMsg);
extern VOS_UINT32 IMSA_Domain_ProcMsccStartReq( const VOS_VOID                     *pRcvMsg  );
extern VOS_UINT32 IMSA_Domain_ProcMsccStopReq( const VOS_VOID                     *pRcvMsg);
extern VOS_UINT32 IMSA_Domain_ProcMsccImsDomainCfgSetReq(  const VOS_VOID                     *pRcvMsg);
extern VOS_UINT32 IMSA_Domain_ProcMsccServiceChangeInd(  const VOS_VOID                     *pRcvMsg);
extern VOS_UINT32 IMSA_Domain_ProcUsimReadFileCnf(const VOS_VOID                *pRcvMsg);
extern VOS_VOID IMSA_Domain_ProcIntraMsgSrvStatusInd(const VOS_VOID                *pRcvMsg);
extern VOS_VOID IMSA_Domain_ProcIntraMsgSrvReadyInd(const VOS_VOID *pRcvMsg);
extern VOS_VOID IMSA_Domain_SetImsDomainPreferFromSetMsg( MSCC_IMSA_IMS_DOMAIN_ENUM_UINT32    enImsDomain );
extern VOS_UINT32 IMSA_Domain_IsNrmSrvConditonSatisfied
(
    MSCC_IMSA_SERVICE_CHANGE_IND_STRU *pRcvMsg
);

extern void   IMSA_Domain_ClearDelaySwitchEvent(VOS_VOID);
extern VOS_UINT32 IMSA_Domain_CheckWifiPossibleInVolteCtrlVowifi(VOS_VOID);
extern IMSA_IMS_HO_TYPE_ENUM_UINT8 IMSA_JudgeImsDomainC221W2L();
extern IMSA_IMS_DOMAIN_ENUM_UINT8 IMSA_ImsDomainC1Judge(VOS_VOID);
extern VOS_VOID IMSA_Domain_ProcIntraMsgSwitchResultInd(const VOS_VOID *pRcvMsg);
extern VOS_VOID IMSA_Domain_ProcIntraMsgBearerRelInd(const VOS_VOID *pRcvMsg);
extern VOS_UINT32 IMSA_Domain_ProcLrrcStateInd
(
 const VOS_VOID                     *pRcvMsg
);
extern VOS_UINT32 IMSA_Domain_ProcLrrcQualityConfigCnf
(
    const VOS_VOID                     *pRcvMsg
);
extern VOS_UINT32 IMSA_Domain_ProcWifiErrorInd(const VOS_VOID                *pRcvMsg);
extern VOS_UINT32 IMSA_IsImsDomainCap(IMSA_IMS_DOMAIN_ENUM_UINT8 enImsaDomain);
extern VOS_UINT32 IMSA_Domain_SetDomainModeInWifiPowerOn(VOS_VOID);
extern VOS_VOID IMSA_Domain_PrintDomainFsmState( VOS_VOID );
extern IMSA_IMS_DOMAIN_ENUM_UINT8 IMSA_JudgeImsDomainC1(VOS_VOID);
extern VOS_UINT32 IMSA_Domain_SetDomainModeInCellularPowerOn(VOS_VOID);
extern VOS_VOID   IMSA_SndSpmImsSwitchStateInd( IMSA_SPM_IMS_SWITCH_STATE_ENUM_UINT8 enSwitchState);
extern VOS_VOID IMSA_Domain_StartTimer1WifiDomain(VOS_VOID);

extern VOS_UINT32 IMSA_Domain_ProcAreaaLostInd
(
    const VOS_VOID                     *pRcvMsg
);

extern VOS_UINT32 IMSA_Domain_ProcMsccMsgRegReq
(
    const VOS_VOID                     *pRcvMsg
);
extern VOS_UINT32 IMSA_Domain_ProcMsccMsgDeregReq
(
    const VOS_VOID                     *pRcvMsg
);

extern void IMSA_Domain_StopTimer1WifiDomain(VOS_VOID);



/*****************************************************************************
  4 Enum
*****************************************************************************/




/*****************************************************************************
  5 STRUCT
*****************************************************************************/




/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/

/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/



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

#endif /* end of ImsaProcApsMsg.h */
