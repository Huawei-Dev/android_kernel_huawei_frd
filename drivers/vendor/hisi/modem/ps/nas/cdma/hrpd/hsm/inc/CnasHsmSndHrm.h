

#ifndef __CNAS_HSM_SND_HRM_H__
#define __CNAS_HSM_SND_HRM_H__

/*****************************************************************************
  1 The Include of the header file
*****************************************************************************/
#include "vos.h"
#include "CnasHsmCtx.h"
#include "CnasMain.h"
#include "cttf_hrpd_sig_public_pif.h"
#include "nas_cttf_hrpd_rsnp_pif.h"
#include "CnasHsmCtx.h"
#include "CnasHsmEncode.h"
#include "hrm_pif.h"
#include "hsm_hrm_pif.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 The Macro Define
*****************************************************************************/

/* fill send cttf data request message header info */
#define CNAS_HSM_CFG_DATA_REQ_HEADER(pstDataReqHeader, enProtoType, enAddrType, enChanType, enDeliverType, ucPri, enInsType, enReqType)\
            (pstDataReqHeader)->enProtocolType      = enProtoType;\
            (pstDataReqHeader)->enAddressType       = enAddrType;\
            (pstDataReqHeader)->enChannelType       = enChanType;\
            (pstDataReqHeader)->enDeliveryType      = enDeliverType;\
            (pstDataReqHeader)->ucPriority          = (ucPri);\
            (pstDataReqHeader)->enInstanceType      = (enInsType);\
            (pstDataReqHeader)->enDataReqType       = (enReqType)


/*****************************************************************************
  3 The Enumeration Define
*****************************************************************************/



/*****************************************************************************
  4 The Declaration Of The Gloabal Variable
*****************************************************************************/

/*****************************************************************************
  5 The Define Of the Message Header
*****************************************************************************/


/*****************************************************************************
  6 The Define of the Message Name
*****************************************************************************/


/*****************************************************************************
  7 The Struct Define
*****************************************************************************/

typedef struct
{
    CTTF_HRPD_PROTOCOL_TYPE_ENUM_UINT16                     enProtocolType; /**< protocol type */
    CTTF_HRPD_INSTANCE_TYPE_ENUM_UINT8                      enInstanceType;
    CTTF_HRPD_ADDRESS_TYPE_ENUM_UINT8                       enAddressType;  /**< addressing modes: broadcast, multicast, unicast */
    CTTF_HRPD_CHANNEL_TYPE_ENUM_UINT8                       enChannelType;  /**< channel type */
    CTTF_HRPD_DELLIVERY_TYPE_ENUM_UINT8                     enDeliveryType; /**< SLP delivery type */
    VOS_UINT8                                               ucPriority;     /**< a number between 0 and 255 */
    VOS_UINT8                                               aucRsv[1];
    CNAS_HRM_DATA_REQ_TYPE_ENUM_UINT32                      enDataReqType;
} CNAS_HSM_DATA_REQ_HEADER_STRU;



/*****************************************************************************
  8 The Union Define
*****************************************************************************/


/*****************************************************************************
  9 Other Defines
*****************************************************************************/


/*****************************************************************************
  10 The Declaration Of The Function
*****************************************************************************/
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

VOS_VOID CNAS_HSM_SndHrmSessionBeginNtf(
    HSM_HRM_SESSION_TYPE_ENUM_UINT16    enSessionType
);

VOS_VOID CNAS_HSM_SndHrmSessionEndNtf(
    HSM_HRM_SESSION_TYPE_ENUM_UINT16    enSessionType
);

VOS_VOID CNAS_HSM_SndHrmDataCancelNtf(
    CNAS_HRM_DATA_REQ_TYPE_ENUM_UINT32  enCancelDataType
);

VOS_VOID CNAS_HSM_SndHrmDataReq(
    CNAS_HSM_DATA_REQ_HEADER_STRU      *pstDataReqHeader,
    VOS_UINT16                         *pusOpId,
    VOS_UINT16                          usMsgBitLen,
    VOS_UINT8                          *pucMsgBuf
);

VOS_VOID CNAS_HSM_SndKeepAliveReqMsg(
    VOS_UINT32                          ulTransIdAddFlag
);

VOS_VOID CNAS_HSM_RetransmitKeepAliveReqMsg(VOS_VOID);

VOS_VOID CNAS_HSM_SndKeepAliveRspMsg(
    VOS_UINT8                           ucKeepAliveReqTransId
);

VOS_VOID CNAS_HSM_SndHardWareIdRspMsg(
    CNAS_HSM_HARDWARE_ID_RESPONSE_MSG_STRU                 *pstHardWareIdRsp
);

VOS_VOID CNAS_HSM_SndSessionCloseMsg(
    CNAS_HSM_SESSION_CLOSE_MSG_STRU                        *pstSessionClose
);

VOS_VOID CNAS_HSM_SndUatiCmplMsg(VOS_VOID);


#endif


#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of CnasHsmMain.h */











