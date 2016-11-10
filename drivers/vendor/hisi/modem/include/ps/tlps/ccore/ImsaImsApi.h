

#ifndef __IMSAIMSAPI_H__
#define __IMSAIMSAPI_H__

/*****************************************************************************
  1 Include Headfile
*****************************************************************************/


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
    /* "a1.a2.a3.a4 " */
#define IMSA_IMS_IPV4_ADDR_LEN              (4)

    /* "a1...a16" */
#define IMSA_IMS_IPV6_ADDR_LEN              (16)



/*****************************************************************************
  4 Enum
*****************************************************************************/
enum IMSA_VOICE_CODEC_ENUM
{
    IMSA_VOICE_CODEC_AMR_NB = 0,
    IMSA_VOICE_CODEC_EFR,
    IMSA_VOICE_CODEC_FR,
    IMSA_VOICE_CODEC_HR,
    IMSA_VOICE_CODEC_AMR_NB2,
    IMSA_VOICE_CODEC_AMR_WB,
    IMSA_VOICE_CODEC_G711,
    IMSA_VOICE_CODEC_BUTT
};
typedef unsigned short IMSA_VOICE_CODEC_ENUM_UINT16;

enum IMSA_CODEC_AMR_TYPE_TX_ENUM
{
    IMSA_CODEC_AMR_TYPE_TX_SPEECH_GOOD   = 0,
    IMSA_CODEC_AMR_TYPE_TX_SID_FIRST,
    IMSA_CODEC_AMR_TYPE_TX_SID_UPDATE,
    IMSA_CODEC_AMR_TYPE_TX_NO_DATA,
    IMSA_CODEC_AMR_TYPE_TX_SPEECH_DEGRADED,
    IMSA_CODEC_AMR_TYPE_TX_SPEECH_BAD,
    IMSA_CODEC_AMR_TYPE_TX_SID_BAD,
    IMSA_CODEC_AMR_TYPE_TX_ONSET,
    IMSA_CODEC_AMR_TYPE_TX_BUTT
};
typedef unsigned short IMSA_CODEC_AMR_TYPE_TX_ENUM_UINT16;

/* TIMER */
enum IMSA_TIMER_PRECISION_ENUM
{
    IMSA_TIMER_PRECISION_0   = 0,
    IMSA_TIMER_PRECISION_5   = 5,
    IMSA_TIMER_PRECISION_10   = 10,
    IMSA_TIMER_PRECISION_20   = 20,
    IMSA_TIMER_PRECISION_30   = 30,
    IMSA_TIMER_PRECISION_40   = 40,
    IMSA_TIMER_PRECISION_50   = 50,
    IMSA_TIMER_PRECISION_60   = 60,
    IMSA_TIMER_PRECISION_70   = 70,
    IMSA_TIMER_PRECISION_80   = 80,
    IMSA_TIMER_PRECISION_90   = 90,
    IMSA_TIMER_PRECISION_100   = 100,
    IMSA_TIMER_NO_PRECISION,
    IMSA_TIMER_PRECISION_BUTT
};
typedef unsigned int IMSA_TIMER_PRECISION_ENUM_UINT32;


typedef enum
{
    IMSA_IMS_TIMER_NAME_XXX = 500,
    IMSA_IMS_TIMER_NAME_BUTT
}IMSA_IMS_TIMER_NAME_ENUM;


typedef enum
{
    IMSA_DTMF_TYPE_0 = 0,               /**< @brief Event of "0" */
    IMSA_DTMF_TYPE_1 = 1,               /**< @brief Event of "1" */
    IMSA_DTMF_TYPE_2 = 2,               /**< @brief Event of "2" */
    IMSA_DTMF_TYPE_3 = 3,               /**< @brief Event of "3" */
    IMSA_DTMF_TYPE_4 = 4,               /**< @brief Event of "4" */
    IMSA_DTMF_TYPE_5 = 5,               /**< @brief Event of "5" */
    IMSA_DTMF_TYPE_6 = 6,               /**< @brief Event of "6" */
    IMSA_DTMF_TYPE_7 = 7,               /**< @brief Event of "7" */
    IMSA_DTMF_TYPE_8 = 8,               /**< @brief Event of "8" */
    IMSA_DTMF_TYPE_9 = 9,               /**< @brief Event of "9" */
    IMSA_DTMF_TYPE_STAR  = 10,          /**< @brief Event of "10" */
    IMSA_DTMF_TYPE_POUND = 11,          /**< @brief Event of "11" */
    IMSA_DTMF_TYPE_A     = 12,          /**< @brief Event of "12" */
    IMSA_DTMF_TYPE_B     = 13,          /**< @brief Event of "13" */
    IMSA_DTMF_TYPE_C     = 14,          /**< @brief Event of "14" */
    IMSA_DTMF_TYPE_D     = 15,          /**< @brief Event of "15" */
    IMSA_DTMF_TYPE_BUTT  = 16           /**< @brief Event of "16" */
} IMSA_DTMF_TYPE_ENUM;

typedef unsigned short IMSA_DTMF_TYPE_ENUM_UINT16;

typedef enum
{
    IMSA_CONTROL_MSG_PARAM_TYPE_DISABLE     = 0,
    IMSA_CONTROL_MSG_PARAM_TYPE_ENABLE      = 1,
    IMSA_CONTROL_MSG_PARAM_TYPE_BUTT
} IMSA_CONTROL_MSG_PARAM_TYPE_ENUM;

typedef unsigned short IMSA_CTRL_TYPE_PARAM_ENUM_UINT16;
 #if 0
typedef enum
{
    IMS_IMSA_CONTROL_MSG_TYPE_SETSEND   = 0,
    IMS_IMSA_CONTROL_MSG_TYPE_SETRECV   = 1,
    IMS_IMSA_CONTROL_MSG_TYPE_SETPLAY   = 2,
    IMS_IMSA_CONTROL_MSG_TYPE_SETRTCP   = 3,
    IMS_IMSA_CONTROL_MSG_TYPE_BUTT
} IMSA_CONTROL_MSG_TYPE_ENUM;

typedef unsigned short IMSA_CTRL_TYPE_ENUM_UINT16;
#endif
typedef enum
{
    IMSA_IMS_NTF_MSG_PARAM_TYPE_OK  = 0,
    IMSA_IMS_NTF_MSG_PARAM_TYPE_ERR = 1,
    IMSA_IMS_NTF_MSG_PARAM_TYPE_BUTT
} IMSA_NTF_MSG_PARAM_TYPE_ENUM;

typedef unsigned short IMSA_NTY_TYPE_PARAM_ENUM_UINT16;

typedef enum
{
    IMSA_NTY_MSG_TYPE_RTP_STATES = 0,
    IMSA_NTY_MSG_TYPE_BUTT
} IMSA_NTY_MSG_TYPE_ENUM;

typedef unsigned short IMSA_NTY_TYPE_ENUM_UINT16;




typedef void (*TIMER_FUNC)(unsigned int ulParam, unsigned int ulTimerName);

typedef struct
{
    unsigned int                     ulLen;          /*定时器时长，单位millisecond*/
    unsigned int                     ulTimerName;    /*定时器名，标识定时器，采用IMSA_IMS_TIMER_NAME_ENUM枚举值*/
    unsigned int                     ulParam;        /*定时器附加参数*/
    unsigned char                    ucMode;         /*定时器工作模式，0单次定时器,1循环定时器，超时自动重启*/
    unsigned char                    ucRever[3];
    IMSA_TIMER_PRECISION_ENUM_UINT32 enPrecision;    /*定时器精度，表示可容忍的定时误差百分比*/
    TIMER_FUNC                       TimeOutRoutine; /*回调函数，定时器超时后调用，回调函数的参数为ulTimerName和ulTimerName*/
}IMSA_TIMER_PARA_STRU;


typedef struct
{
    IMSA_VOICE_CODEC_ENUM_UINT16  enCodecType;   /*当前语音帧的codec类型,0表示AMR,5表示WB AMR,6表示G711*/
    unsigned short                usDtxEnable;   /*下帧的编码DTX是否启用，0表示不启动，1表示启用*/
    unsigned short                usRateMode;    /*下帧的编码编码速率，值为0到8，AMR和AMR_WB时有效*/
    unsigned short                usErrorFlag;   /*本帧是否是lost packet,0表示否，1表示是*/
    unsigned short                usFrameType;   /* 下行本帧的FrameType */
    unsigned short                usQualityIdx;  /* Quality indification */
    unsigned short                usSN;          /* 包序列号 */
    unsigned short                usReserve;
    unsigned long                 ulTS;          /* 包时间戳 */
    unsigned long                 ulSsrc;        /* synchronization source */
}IMSA_RX_VOICE_PARA_STRU;


typedef struct
{
    IMSA_CODEC_AMR_TYPE_TX_ENUM_UINT16  usStatus;
    unsigned short                      usFrameType;  /* 上行本帧的FrameType */
}IMSA_TX_VOICE_PARA_STRU;


typedef struct
{
   unsigned long                         ulChannelId;
   unsigned short                        usPortType; /* Port type: 0 RTP; 1 RTCP */
   unsigned short                        usLength;    /* 指定数据长度，单位是16Bit */
 } IMSA_IMS_RTP_PARA_STRU;

typedef struct
{
    IMSA_VOICE_CODEC_ENUM_UINT16 ulCodecType;
    unsigned long ulRecivePayload;          /**< @brief codec recive payload type */
    unsigned long ulSendPayload;            /**< @brief codec send payload type */
    unsigned long ulOctetAlign;             /**< @brief Octet Align */
    unsigned long ulBitrate;                /**< @brief bitrate */
    unsigned long ulSplRate;                /**< @brief sample rate (Hz) */
    unsigned long ulTsIncrease;             /**< @brief Ts increase value 样点数 */
}IMSA_ENGINE_PAYLOAD_PARA_STRU;

typedef struct
{
    unsigned long ulDTMFRecivePayload;      /**< @brief dtmf recive payload type */
    unsigned long ulDTMFSendPayload;        /**< @brief dtmf send payload type */
}IMSA_ENGINE_DTMF_PAYLOAD_PARA_STRU;

typedef struct
{
    unsigned short  usLocalRtpPort;        /* RTP Port，没有分配填0 */
    unsigned short  usLocalRtcpPort;       /* RTCP Port，没有分配填0 */
    unsigned short  usRemoteRtpPort;       /* RTP Port，没有分配填0 */
    unsigned short  usRemoteRtcpPort;      /* RTCP Port，没有分配填0 */
    unsigned char   ucIpType;              /* IPV4: 0x01; IPV6: 0x02 */
    unsigned char   aucReserved[3];
    unsigned char   aucIpV4Addr[IMSA_IMS_IPV4_ADDR_LEN];
    unsigned char   aucIpV6Addr[IMSA_IMS_IPV6_ADDR_LEN];
}IMSA_ENGINE_PORT_INFO_PARA_STRU;



/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/

extern unsigned int IMSA_StartVoiceDsp
(
    IMSA_VOICE_CODEC_ENUM_UINT16    enCodecType,
    unsigned long                   ulPacketTime,
    unsigned long                   ulSendBitrate,
    unsigned long                   ulVoiceEncrypt
);

/**
 * \brief 海思提供的API，用于IMS协议栈在挂断所有电话后调用
 *        在该API中，IMSA会通知HIFI,停止语音编解码
 * @retval 0成功，1失败
 */
extern unsigned int IMSA_StopVoiceDsp(void);


/**
 * \brief 海思提供的API，用于IMS协议栈收到下行RTP数据后调用，
 *        将语音数据发送给HIFI，HIFI收到后对RTP数据进行解码处理
 * @param[in]   pstRtpPara    参数信息存放地址
 * @param[in]   pusRevRtpData    RTP数据存放地址，空间大小为160byte
 * @retval 0成功，1失败
 */

extern unsigned int IMSA_Send2HifiRxRtpData
(
   const IMSA_IMS_RTP_PARA_STRU *pstRtpPara,
   const unsigned char         *pucRevRtpData
);

/**
 * \brief IMS协议栈提供给海思的API，
 *        HIFI完成上行RTP编码后，发送码流给IMSA，IMSA收到后调用该API，将数据发给IMS协议栈
 * @param[in]   pstRtpPara       参数信息存放地址
 * @param[in]   pusRevRtpData    RTP数据存放地址，空间大小为160byte
 * @retval 0成功，1失败
 * @retval 无
 */

extern unsigned int IMSA_Send2ImsTxRtpData
(
   const IMSA_IMS_RTP_PARA_STRU *pstRtpPara,
   const unsigned char         *pucRevRtpData
);

/* 对Media Control模块控制的函数 */


/**
 * \brief 海思提供的API，用于IMS协议栈需要打开Media control Engine时进行调用
 * @param[in]   ulChannelId     呼叫通道标识
 * @param[in]    ulType          指定IP类型 0: IPV4, 1: IPV6
 * @retval 0成功，1失败
 */
extern unsigned int IMSA_ImsEngineOpen
(
  unsigned long ulChannelId,
  unsigned long ulType
);

/**
 * \brief 海思提供的API，用于IMS协议栈需要关闭Media control Engine时进行调用
 * @param[in]   ulChannelId     呼叫通道标识
 * @retval 0成功，1失败
 */
extern unsigned int  IMSA_ImsEngineClose
(
  unsigned long ulChannelId
);

/**
 * \brief 海思提供的API，用于IMS协议栈需要配置Media control Engine参数时进行调用
 * @param[in]   ulChannelId     呼叫通道标识
 * @param[in]   pstEngineParam     呼叫参数
 * @retval 0成功，1失败
 */
extern unsigned int  IMSA_ImsEngineSetPayload
(
    unsigned long                  ulChannelId,
    IMSA_ENGINE_PAYLOAD_PARA_STRU *pstEngineParam
);

/**
 * \brief 海思提供的API，用于IMS协议栈需要配置Media control Engine DTMF参数时进行调用
 * @param[in]   ulChannelId     呼叫通道标识
 * @param[in]   pstEngineParam     呼叫参数
 * @retval 0成功，1失败
 */
extern unsigned int  IMSA_ImsEngineSetDtmfPayload
(
    unsigned long                       ulChannelId,
    IMSA_ENGINE_DTMF_PAYLOAD_PARA_STRU *pstEngineParam
);

/**
 * \brief 海思提供的API，用于IMS协议栈需要配置WIFI模块端口信息时进行调用
 * @param[in]   ulChannelId     呼叫通道标识
 * @param[in]   pstEngineParam     呼叫参数
 * @retval 0成功，1失败
 */
extern unsigned int  IMSA_ImsEngineSetPortInfo
(
    unsigned long                    ulChannelId,
    IMSA_ENGINE_PORT_INFO_PARA_STRU *pstEngineParam
);

/**
 * \brief 海思提供的API，用于IMS协议栈设置Media control Engine发送控制时进行调用
 * @param[in]   ulChannelId     呼叫通道标识
 * @param[in]   IMSA_CTRL_TYPE_PARAM_ENUM_UINT16 enCtrlParam 控制参数
 * @retval 0成功，1失败
 */
extern unsigned int  IMSA_ImsEngineSendCtrlRecv
(
   unsigned long                    ulChannelId,
   IMSA_CTRL_TYPE_PARAM_ENUM_UINT16 enCtrlParam
);

/**
 * \brief 海思提供的API，用于IMS协议栈设置Media control Engine发送控制时进行调用
 * @param[in]   ulChannelId     呼叫通道标识
 * @param[in]   IMSA_CTRL_TYPE_PARAM_ENUM_UINT16 enCtrlParam 控制参数
 * @retval 0成功，1失败
 */
extern unsigned int  IMSA_ImsEngineSendCtrlSend
(
   unsigned long                    ulChannelId,
   IMSA_CTRL_TYPE_PARAM_ENUM_UINT16 enCtrlParam
);

/**
 * \brief 海思提供的API，用于IMS协议栈设置Media control Engine发送控制时进行调用
 * @param[in]   ulChannelId     呼叫通道标识
 * @param[in]   IMSA_CTRL_TYPE_PARAM_ENUM_UINT16 enCtrlParam 控制参数
 * @retval 0成功，1失败
 */
extern unsigned int  IMSA_ImsEngineSendCtrlPlay
(
   unsigned long                    ulChannelId,
   IMSA_CTRL_TYPE_PARAM_ENUM_UINT16 enCtrlParam
);

/**
 * \brief 海思提供的API，用于IMS协议栈设置Media control Engine发送控制时进行调用
 * @param[in]   ulChannelId     呼叫通道标识
 * @param[in]   IMSA_CTRL_TYPE_PARAM_ENUM_UINT16 enCtrlParam 控制参数
 * @retval 0成功，1失败
 */
extern unsigned int  IMSA_ImsEngineSendCtrlRtcp
(
   unsigned long                    ulChannelId,
   IMSA_CTRL_TYPE_PARAM_ENUM_UINT16 enCtrlParam
);


/**
 * \brief 海思提供的API，用于IMS协议栈通知Media control Engine反馈状态时进行调用
 * @param[in]   ulChannelId     呼叫通道标识
 * @param[in]   iSend     0: stop to send data, 1: start to send data
 * @retval 0成功，1失败
 */
extern unsigned int  IMSA_ImsEngineRecvStateMsg
(
   unsigned long                    ulChannelId,
   IMSA_NTY_TYPE_ENUM_UINT16        enCtrlType,
   IMSA_NTY_TYPE_PARAM_ENUM_UINT16  enCtrlParam
);



/**
 * \brief 海思提供的API，用于IMS协议栈通过Media control Engine发送DTMF进行调用
 * @param[in]   ulChannelId     呼叫通道标识
 * @param[in]   enDtmfType      标识DTMF类型
 * @param[in]   dwLen           长度
 * @retval 0成功，1失败
 */

extern unsigned int IMSA_ImsEngineSendDtmf
(
    unsigned long              ulChannelId,
    IMSA_DTMF_TYPE_ENUM_UINT16 enDtmfType,
    unsigned long              dwLen
);


/**
 * \brief 海思提供的API，用于IMS协议栈收到下行数据后调用，
 *        将语音数据发送给HIFI，HIFI收到后进行译码
 * @param[in]   pstVoicePara   参数信息存放地址，该数据空间由调用者释放
 * @param[in]   pusData        语音数据存放地址，空间大小为160byte，该数据空间由调用者释放
 * @retval 0成功，1失败
 */
extern unsigned int IMSA_Send2HifiRxVoiceData(const IMSA_RX_VOICE_PARA_STRU *pstVoicePara,
                                                        const unsigned short *pusData);
#if 0
/**
 * \brief IMS协议栈提供给海思的API，
 *        HIFI完成上行语音编码后，发送码流给IMSA，IMSA收到后调用该API，将数据发给IMS协议栈
 * @param[in]   pstVoicePara   参数信息存放地址，该数据空间由调用者(海思)释放
 * @param[in]   pusData        语音数据存放地址，空间大小为160byte，该数据空间由调用者(海思)释放
 * @retval 无
 */
extern void IMSA_Send2ImsTxVoiceData(const IMSA_TX_VOICE_PARA_STRU *pstVoicePara,
                                              const unsigned short *pusData);

#endif
/**
 * \brief 海思提供给API，用于IMS协议栈给IMSA发送消息时调用
 *
 * @param[in]   pstOutputEvent    事件存放地址，该数据空间由调用者释放，类型为IMSA_IMS_OUTPUT_EVENT_STRU
 * @retval 0成功，1失败
 */
extern unsigned int IMSA_Send2ImsaMsg(void *pstOutputEvent);

/**
 * \brief 海思提供给API，用于IMS协议栈给HIFI发送启动/停止RTP检查指示消息时调用
 *
 * @param[in]   ulChannelId     呼叫通道标识
 * @param[in]   enChrlParam     1 启动/0 停止 RTP检查
 * @retval 0成功，1失败
 */
extern unsigned int IMSA_ImsEngineSendRTPChkEnable
(
    unsigned long                       ulChannelId,
    unsigned int                        enChrlParam
);

/**
 * \brief 海思提供给的API，用于IMS协议栈启动定时器
 *
 * @param[in]    pstTimerPara： 定时器参数存放地址，该数据空间由调用者释放
 * @retval 0成功，1失败
 */
extern unsigned int IMSA_ImsStartTimer(const IMSA_TIMER_PARA_STRU  *pstTimerPara);


/**
 * \brief 海思提供的API，用于IMS协议栈停止定时器
 *
 * @param[in]    ulTimerName： 定时器名称
 * @retval 0成功，1失败
 */
extern unsigned int IMSA_ImsStopTimer(unsigned int  ulTimerName);

/**
 * \brief IMS协议栈提供给海思的API，用于IMS协议栈的初始化
 * @retval 0失败，1成功
 */
extern unsigned int IMSA_ImsInit(void);


/**
 * \brief IMS协议栈提供给海思的API，用于给IMS协议栈发送事件
 *
 * @param[in]    pstInputEvent： INPUT事件指针 类型为IMSA_IMS_INPUT_EVENT_STRU
 * @retval 0失败，1成功
 */
extern unsigned int IMSA_WriteImsEvent(void *pstInputEvent);

/**
 * \brief IMS协议栈提供给海思的API，用于IMS协议栈提供Srvcc过程中转入CS域的呼叫列表
 *
 * @param[in]    pstSrvccCallList： 类型为IMSA_IMS_SRVCC_CALL_LIST_STRU，内存由海思分配
 * @retval 0失败，1成功
 */
extern unsigned int IMSA_ImsWriteSrvccCallList(void *pstSrvccCallList);

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

#endif /* end of ImsaImsApi.h */




