

#ifndef __IMSAPROCATMSG_H__
#define __IMSAPROCATMSG_H__

/*****************************************************************************
  1 Include Headfile
*****************************************************************************/

#include    "vos.h"
#include    "AtImsaInterface.h"
#include    "ImsaEntity.h"
#include    "ImsaProcImsCallMsg.h"

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
 
typedef struct
{
    VOS_UINT32                          ulMsgId;
    VOS_UINT16                          usClientId;
    VOS_UINT8                           ucOpId;
    VOS_UINT8                           aucReserved[1];
} IMSA_AT_MGMT_DATA_STRU;


/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/

/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/
extern VOS_VOID IMSA_ProcMsgCiregSetReq (const AT_IMSA_CIREG_SET_REQ_STRU *pstCiregSetReq);
extern VOS_VOID IMSA_SndMsgAtCireguInd (VOS_UINT32 ulRegInfo);
extern VOS_VOID IMSA_ProcMsgCiregQryReq (const AT_IMSA_CIREG_QRY_REQ_STRU *pstCiregQryReq);
extern VOS_VOID IMSA_ProcMsgCirepSetReq (const AT_IMSA_CIREP_SET_REQ_STRU *pstCirepSetReq);
extern VOS_VOID IMSA_SndMsgAtCirephInd (AT_IMSA_SRVCC_HANDVOER_ENUM_UINT32   enHandover);
extern VOS_VOID IMSA_SndMsgAtCirepiInd (AT_IMSA_IMSVOPS_CAPABILITY_ENUM_UINT32      enImsvops);

extern VOS_VOID IMSA_SndMsgAtSrvStatusUpdateInd
(
      IMSA_AT_IMS_SERVICE_STATUS_ENUM_UINT8 enSmsSrvStatus,
      IMSA_AT_IMS_SERVICE_STATUS_ENUM_UINT8   enVoIpSrvStatus,
      IMSA_AT_IMS_SERVICE_STATUS_ENUM_UINT8   enVtSrvStatus,
      IMSA_AT_IMS_SERVICE_STATUS_ENUM_UINT8   enVsSrvStatus,
      IMSA_AT_IMS_RAT_TYPE_ENUM_UINT8         enSrvRat
);
extern VOS_VOID IMSA_SndMsgAtImsRatHandoverInd
(
    IMSA_AT_IMS_RAT_HO_STATUS_ENUM_UINT8    enHoStatus,
    IMSA_AT_IMS_RAT_TYPE_ENUM_UINT8         enSrcRat,
    IMSA_AT_IMS_RAT_TYPE_ENUM_UINT8         enDstRat,
    IMSA_AT_HO_CAUSE_ENUM_UINT32            enCause
);


extern VOS_VOID IMSA_ProcMsgCirepQryReq (const AT_IMSA_CIREP_QRY_REQ_STRU *pstCiregQryReq);

extern VOS_VOID IMSA_ProcMsgVolteImpuQryReq(const AT_IMSA_VOLTEIMPU_QRY_REQ_STRU *pstVolteImpuQryReq);
extern VOS_VOID IMSA_SndMsgAtQryVolteImpuCnf
(
    VOS_UINT32 ulResult,
    VOS_UINT32 ulImpuLen,
    VOS_CHAR *pucImpu
);
extern VOS_VOID IMSA_SndMsgAtPdpActInd (const IMSA_PDP_CNTXT_INFO_STRU     *pstPdpInfo);
extern VOS_VOID IMSA_SndMsgAtPdpDeactInd
(
    TAF_PDP_TYPE_ENUM_UINT8             enPdpType,
    IMSA_AT_IMS_RAT_TYPE_ENUM_UINT8     enRatType
);


extern VOS_VOID IMSA_SndMsgAtMtStatusInd(const IMSA_MT_STATUS_REPORT_STRU    *pstMtReport);

extern VOS_VOID IMSA_SndMsgAtImsCtrlMsg(const VOS_VOID    *pstMsg, VOS_UINT32 ulLength);
extern VOS_VOID IMSA_ProcMsgAtCtrlInd(const AT_IMSA_IMS_CTRL_MSG_STRU *pstCtrlInd);
extern VOS_VOID IMSA_ProcMsgAtDomainQryReq(const AT_IMSA_IMS_REG_DOMAIN_QRY_REQ_STRU *pstDomainQryReq);
extern VOS_VOID IMSA_SndMsgAtWifiPowerOffCnf( const IMSA_WIFI_POWEROFF_CNF_STRU    *pstPowerOffCnf);
extern VOS_VOID IMSA_SndMsgAtWifiPowerOnCnf( const IMSA_WIFI_POWERON_CNF_STRU    *pstPowerOnCnf);
extern VOS_VOID IMSA_SndMsgAtWifiAudioInfoInd(  const IMSA_WIFI_IMS_AUDIO_INFO_IND_STRU    *pstAudioInfoInd);
extern VOS_VOID IMSA_SndMsgAtWifiPdnDeactReq
(
    const IMSA_WIFI_IMS_PDN_DEACTIVATE_REQ_STRU    *pstPdnDeActReq
);
extern VOS_VOID IMSA_SndMsgAtWifiPdpActInd (const IMSA_WIFI_PDP_CNTXT_INFO_STRU     *pstPdpInfo);
extern VOS_VOID IMSA_ProcMsgRoamingImsQryReq
(
    const AT_IMSA_ROAMING_IMS_QRY_REQ_STRU *pstRoamingImsSetReq
);
extern VOS_VOID IMSA_SndMsgAtRoamingImsQryCnf
(
    VOS_UINT8                           ucOpId,
    VOS_UINT16                          usClientId,
    AT_IMSA_ROAMING_IMS_SUPPORT_ENUM_UINT32     enRoamingImsSupportFlag
);

extern VOS_VOID IMSA_ProcMsgEncryptSetReq(const AT_IMSA_CALL_ENCRYPT_SET_REQ_STRU *pstEncryptSetReq);
extern VOS_VOID IMSA_SndMsgAtEncryptSetCnf(VOS_UINT32 ulResult);

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



