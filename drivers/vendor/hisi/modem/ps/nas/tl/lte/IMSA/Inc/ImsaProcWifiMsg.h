

#ifndef __IMSAPROCWIFIMSG_H__
#define __IMSAPROCWIFIMSG_H__

/*****************************************************************************
  1 Include Headfile
*****************************************************************************/

#include    "vos.h"
#include    "ImsaWifiInterface.h"
#include    "ImsaEntity.h"
#include    "LPsOm.h"
#include    "ImsaImsAdaption.h"


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


/*****************************************************************************
  3 Massage Declare
*****************************************************************************/



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
extern VOS_VOID IMSA_CONN_SndWifiMsgPdnActivateReq
(
    VOS_UINT32                          bitOpApn,
    IMSA_WIFI_PDP_APN_STRU              *pstApnInfo,
    IMSA_WIFI_IP_TYPE_ENUM_INT8         enIpType,
    VOS_INT8                            enHoFlg
);
extern VOS_VOID IMSA_CONN_ProcTimerOutDeactcnf(    const VOS_VOID *pTimerMsg);
extern VOS_VOID IMSA_CONN_ProcTimerOutReadyInd(    const VOS_VOID *pTimerMsg);
extern VOS_VOID IMSA_CONN_ProcTimerOutActiveCnf(    const VOS_VOID *pTimerMsg);
extern VOS_VOID IMSA_CONN_ProcWifiMsgErrorInd(    WIFI_IMSA_ERROR_IND_STRU* pstErrorInd);
extern VOS_VOID IMSA_SndWifiPowerOffCnf(VOS_VOID );
extern VOS_VOID IMSA_SndWifiPowerOnCnf(VOS_VOID );
extern unsigned int Imsa_SndWifiAudioInfoInd( IMSA_IMS_AUDIO_INFO_IND_STRU  *pstImsAudioInfoList);


extern VOS_VOID IMSA_CONN_SndWifiMsgPdnDeactiveReq(VOS_INT8 ucLocalRlsFlag);
extern VOS_VOID IMSA_CONN_ProcWifiMsgPdnActivateCnf(WIFI_IMSA_IMS_PDN_ACTIVATE_CNF_STRU* pstPdnActCnf);
extern VOS_VOID IMSA_CONN_ProcWifiMsgPdnReadyInd(WIFI_IMSA_IMS_PDN_READY_IND_STRU* pstPdnReadyInd);
extern VOS_VOID IMSA_CONN_ProcWifiMsgPdnDeactiveCnf
(
    WIFI_IMSA_IMS_PDN_DEACTIVATE_CNF_STRU* pstPdnDeactCnf
);
extern VOS_VOID IMSA_CONN_ConfigWifiPdpInfo2Reg
(
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType,
    const IMSA_WIFI_PDP_CNTXT_INFO_STRU     *pstPdpInfo
);
void IMSA_CONN_ProcHoWifiMsgPdnActiveCnf
(
    const WIFI_IMSA_IMS_PDN_ACTIVATE_CNF_STRU* pstPdnActCnf
);
void IMSA_CONN_ProcNoHoWifiMsgPdnActiveCnf
(
    const WIFI_IMSA_IMS_PDN_ACTIVATE_CNF_STRU* pstPdnActCnf
);
VOS_VOID IMSA_CONN_ProcNoHoWifiMsgPdnReadyInd
(
    const  WIFI_IMSA_IMS_PDN_READY_IND_STRU*  pstPdnReadyInd
);

VOS_VOID IMSA_CONN_ProcHoWifiMsgPdnReadyInd
(
    const  WIFI_IMSA_IMS_PDN_READY_IND_STRU*  pstPdnReadyInd
);
VOS_VOID IMSA_CONN_ProcNoHoWifiMsgErrorInd
(
    const WIFI_IMSA_ERROR_IND_STRU* pstErrorInd
);

VOS_VOID IMSA_CONN_ProcHoWifiMsgErrorInd
(
    const WIFI_IMSA_ERROR_IND_STRU* pstErrorInd
);
extern VOS_VOID IMSA_ClearConnHoEventStatus(VOS_VOID);
extern VOS_VOID IMSA_CONN_RlsWifiPdn(VOS_VOID);




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

#endif /* end of ImsaProcAtMsg.h */




