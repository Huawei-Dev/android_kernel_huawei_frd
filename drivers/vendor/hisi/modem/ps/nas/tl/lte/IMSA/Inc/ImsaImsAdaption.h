

#ifndef __IMSAIMSADAPTION_H__
#define __IMSAIMSADAPTION_H__

/*****************************************************************************
  1 Include Headfile
*****************************************************************************/

#include "vos.h"
#include "ImsaImsInterface.h"

#include "ImsCodecInterface.h"
#include "ImsaEntity.h"
#include "ImsaImsApi.h"
#include "VcImsaInterface.h"

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
/* 保护定时器时长为10ms */
#define IMSA_HIFI_ACK_PROTECT_LENGTH     10

#define IMSA_IMS_AUDIO_LIST_NUM     4
#define IMSA_IMS_VIDEO_LIST_NUM     2


/*****************************************************************************
  3 Massage Declare
*****************************************************************************/

#define IMSA_GetImsAudioInfoListAddr()     (&g_stImsaAudioInfo)


/*****************************************************************************
  4 Enum
*****************************************************************************/

/*****************************************************************************
 枚举名    : IMSA_PORT_STATUS_ENUM
 结构说明  : 端口号单元使用状态
*****************************************************************************/
enum IMSA_PORT_STATUS_ENUM
{
   IMSA_PORT_STATUS_NO_USE                     = 0,    /**< 没有使用 */
   IMSA_PORT_STATUS_IN_USE                     = 1,    /**< 正在使用 */

   IMSA_PORT_STATUS_BUTT
};
typedef VOS_UINT16   IMSA_PORT_STATUS_ENUM_U16;


/*****************************************************************************
  5 STRUCT
*****************************************************************************/

/*****************************************************************************
 结构名    : IMSA_IP_ADDRESS_STRU
 结构说明  : IP地址的结构体
*****************************************************************************/
typedef struct
{
   VOS_UINT16                          usChannelId;   /*保存呼叫通道标识，IMS分配维护 */
   IMSA_PORT_STATUS_ENUM_U16           enUseFlag;     /* 0：没有使用; 1:正在使用 */
   VOS_UINT16                          usLocalRTPPort;   /* 本地RTP端口号 */
   VOS_UINT16                          usLocalRTCPPort;  /* 本地RTCP 端口号 */
   VOS_UINT16                          usRemoteRTPPort;  /* 远端RTP端口号 */
   VOS_UINT16                          usRemoteRTCPPort;  /* 远端RTCP端口号,0表示尚未获得 */
   IMSA_IP_ADDRESS_STRU                stRemoteAddr;      /* 远端IP地址 */
} IMSA_MEDIA_INFO_STRU;

typedef struct
{
   VOS_UINT8                            ucAudioInfoCnt;
   VOS_UINT8                            ucReserve;
   IMSA_VOICE_CODEC_ENUM_UINT16         enCodecType;
   IMSA_MEDIA_INFO_STRU                 stAudioInfo[IMSA_IMS_AUDIO_LIST_NUM];
} IMSA_IMS_AUDIO_INFO_IND_STRU;

typedef struct
{
   VOS_UINT8                            ucVideoInfoCnt;
   VOS_UINT8                            ucReserve[3];
   IMSA_MEDIA_INFO_STRU                 stVideoInfo[IMSA_IMS_VIDEO_LIST_NUM];
} IMSA_IMS_VIDEO_INFO_IND_STRU;


/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/

/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/
extern VOS_VOID IMSA_Snd2ImsaCallMsg(const IMSA_IMS_OUTPUT_CALL_EVENT_STRU *pstOutputCallEvent);
extern VOS_VOID IMSA_Snd2ImsaSmsMsg(const IMSA_IMS_OUTPUT_SMS_EVENT_STRU *pstImsaImsOutputSms);
extern VOS_VOID IMSA_Snd2ImsaServiceMsg(const IMSA_IMS_OUTPUT_SERVICE_EVENT_STRU *pstHicsmOutputService);
extern VOS_VOID IMSA_SndImsMsgStopReq(VOS_VOID  );
extern VOS_VOID IMSA_SndImsMsgStartReq(VOS_VOID  );
extern VOS_VOID IMSA_SndImsMsgServiceEvent(IMSA_IMS_INPUT_EVENT_STRU *pstHiInputEvent);
extern VOS_VOID IMSA_SndImsMsgCallEvent(const IMSA_IMS_INPUT_EVENT_STRU *pstHiInputEvent);
extern VOS_VOID IMSA_SndImsMsgSmsEvent(IMSA_IMS_INPUT_EVENT_STRU *pstImsaImsInputEvent);
extern VOS_VOID IMSA_SndImsMsgSystemEvent(IMSA_IMS_INPUT_EVENT_STRU *pstImsaImsInputEvent);
extern VOS_VOID IMSA_SndImsMsgParaEvent(IMSA_IMS_INPUT_EVENT_STRU *pstImsaImsInputEvent);
extern VOS_VOID IMSA_SndImsMsgUssdEvent(IMSA_IMS_INPUT_EVENT_STRU *pstImsaImsInputEvent);
extern VOS_VOID IMSVA_ProcHifiMsg(const VOS_VOID *pRcvMsg );
extern VOS_VOID IMSVA_ProcHifiRtMsg(const VOS_VOID *pRcvMsg );
extern VOS_VOID IMSVA_TimerMsgDistr(const REL_TIMER_MSG *pRcvMsg );

extern VOS_VOID IMSA_Snd2ImsaParaMsg(const IMSA_IMS_OUTPUT_PARA_EVENT_STRU *pstOutputService);
extern VOS_VOID IMSA_Snd2ImsaSystemMsg(const IMSA_IMS_OUTPUT_SYSTEM_EVENT_STRU *pstOutputService);
extern VOS_VOID IMSA_NtfVcHoEvent(IMSA_VC_MODE_ENUM_UINT8 enMode);
extern VOS_UINT32 IMSA_IsImsAudioChannelExist(VOS_VOID);

extern VOS_VOID IMSA_SendImsVolteImpuQryReq(VOS_VOID);
extern VOS_VOID IMSA_ProcImsMsgQryVolteImpuCnf(const IMSA_IMS_OUTPUT_PARA_EVENT_STRU *pstOutPara);
extern VOS_VOID IMSA_SndImsMsgNvInfoEvent
(
    IMSA_IMS_INPUT_EVENT_STRU   *pstImsaImsInputEvent,
    VOS_UINT32                  ulNvInfoStruLen
);

extern VOS_UINT32 IMSA_ImsInputParaMsgSave(const IMSA_IMS_INPUT_PARA_EVENT_STRU *pstParaEvt);
extern VOS_VOID IMSA_ImsInputParaMsgClear(VOS_VOID);
extern VOS_VOID IMSA_SendImsCcwaiSetReq(VOS_UINT8 ucCallWaiting);
extern VOS_VOID IMSA_ProcImsMsgCcwaiSetCnf(const IMSA_IMS_OUTPUT_PARA_EVENT_STRU *pstOutPara);
extern VOS_VOID IMSA_SndLRrcHifiSyncSwtichInd( VOS_UINT8   ucHifiSyncEnalbed);
extern VOS_VOID IMSA_ImsAudioListInit(VOS_VOID);
extern VOS_VOID IMSA_ProcHifiFailInd(VOS_VOID *pRcvMsg);
extern VOS_VOID IMSA_ProcHIfiRtpTxDataInd(const VOICE_IMSA_RTP_TX_DATA_IND_STRU  *pstTxDataInd);
extern VOS_VOID IMSA_ProcHifiEngineStatusNtf(VOICE_IMSA_ENGINE_STATUS_NTF_STRU *pRcvMsg);
extern unsigned int  IMSA_ImsEngineSyncPortInfo(VOS_VOID);

extern VOS_VOID IMSA_ImsAdaption_Init(VOS_VOID);
extern VOS_VOID IMSA_ClearHifiData(VOS_VOID);
extern VOS_VOID IMSA_ImsAdaption_ClearResource(VOS_VOID);
extern VOS_VOID IMSA_AddDataToBuffer(IMSA_VOICE_RTP_RX_DATA_IND_STRU *pstRxRtpDataInd);
extern VOS_VOID IMSA_ProcTimerMsgHifiAckProtect(const VOS_VOID *pRcvMsg);
extern VOS_VOID IMSA_SendOneBufferData(VOS_UINT8 ulNeedAckFlag);
extern VOS_VOID IMSA_SndHifiDataInfo(VOS_VOID);
extern VOS_VOID IMSVA_StartTimer(IMSA_TIMER_STRU *pstTimer);
extern VOS_VOID IMSVA_StopTimer(IMSA_TIMER_STRU *pstTimer);

extern VOS_VOID IMSA_SendImsEncryptSetReq(VOS_UINT8 ucCallEncrypt);
extern VOS_VOID IMSA_ProcImsMsgEncryptSetCnf(const IMSA_IMS_OUTPUT_PARA_EVENT_STRU *pstOutPara);
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

#endif /* end of ImsaImsAdaption.h */




