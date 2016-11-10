

#ifndef __TLL2ErrorLog_H__
#define __TLL2ErrorLog_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include "product_config.h"
#include "omnvinterface.h"
#include "omerrorlog.h"
#include "omringbuffer.h"
#include "SysNvId.h"

#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif

/*****************************************************************************
  2 macro
*****************************************************************************/
#define TLL2_LTE_RING_BUFFER_SIZE    (1024 * 4)  /* TLL2_LTE errlog 申请buffer的大小 */
#define TLL2_TDS_RING_BUFFER_SIZE    (1024 * 4)  /* TLL2_TDS errlog 申请buffer的大小 */

#define LTE_ERRLOG_L2_STAT_RPT_MAX_NUM         10   /*统计量最多上报10组*/
/*****************************************************************************
  3 Massage Declare
*****************************************************************************/

/*****************************************************************************
  4 Enum
*****************************************************************************/

/*****************************************************************************
 枚举名    : TLL2_ERRLOG_MODEM_ID_ENUM
 枚举说明  : TLL2 MODEM_ID
*****************************************************************************/
enum TLL2_ERRLOG_MODEM_ID_ENUM
{
    TLL2_ERRLOG_MODEM_ID_0        = 0x00 ,      /* 主卡 */
    TLL2_ERRLOG_MODEM_ID_1        = 0x01 ,      /* 副卡 */
    TLL2_ERRLOG_MODEM_ID_REV1     = 0x02 ,      /* 保留 */
    TLL2_ERRLOG_MODEM_ID_REV2     = 0x03 ,      /* 保留 */

    TLL2_ERRLOG_MODEM_ID_BUTT
};
typedef VOS_UINT16 TLL2_ERRLOG_MODEM_ID_ENUM_UINT16;

/*****************************************************************************
 枚举名    : TLL2_ERRLOG_ALM_LEVEL_ENUM
 枚举说明  : TLL2告警级别
*****************************************************************************/
enum TLL2_ERRLOG_ALM_LEVEL_ENUM
{
    TLL2_ERRLOG_ALM_LEVEL_CRITICAL   = 0x01,        /*紧急*/
    TLL2_ERRLOG_ALM_LEVEL_MAJOR      = 0x02,        /*重要*/
    TLL2_ERRLOG_ALM_LEVEL_MINOR      = 0x03,        /*次要*/
    TLL2_ERRLOG_ALM_LEVEL_WARNING    = 0x04,        /*提示*/

    TLL2_ERRLOG_ALM_LEVEL_BUTT
};
typedef VOS_UINT16  TLL2_ERRLOG_ALM_LEVEL_ENUM_UINT16;

/*****************************************************************************
 枚举名    : TLL2_ERRLOG_ALM_TYPE_ENUM
 枚举说明  : TLL2告警 类型
*****************************************************************************/
enum TLL2_ERRLOG_ALM_TYPE_ENUM
{
    TLL2_ERRLOG_ALM_TYPE_COMMUNICATION          = 0x00,     /* 通信*/
    TLL2_ERRLOG_ALM_TYPE_QUALITY_OF_SERVICE     = 0x01,     /* 业务质量*/
    TLL2_ERRLOG_ALM_TYPE_PROCESS_ERROR          = 0x02,     /* 处理出错*/
    TLL2_ERRLOG_ALM_TYPE_EQUIPMENT_ERROR        = 0x03,     /* 设备故障*/
    TLL2_ERRLOG_ALM_TYPE_ENVIRONMENTAL_ERROR    = 0x04,     /* 环境故障*/

    TLL2_ERRLOG_ALM_TYPE_BUTT
};
typedef VOS_UINT16  TLL2_ERRLOG_ALM_TYPE_ENUM_UINT16;

/*****************************************************************************
 枚举名    : TLL2_LTE_ERROR_ALARM_ID_ENUM
 枚举说明  : TLL2_LTE 告警ID
*****************************************************************************/
enum TLL2_LTE_ERROR_ALARM_ID_ENUM
{
    TLL2_LTE_ALM_INFO = 1,                  /*LTE L2告警*/
    TLL2_LTE_STAT_INFO,                     /*L2统计量*/
    TLL2_LTE_THR_INFO,                      /*L2流量*/
    TLL2_LTE_ERRLOG_ALARM_ID_BUTT
};
typedef VOS_UINT16 TLL2_LTE_ERROR_ALARM_ID_ENUM_UINT16;

/*****************************************************************************
 枚举名    : TLL2_LTE_ERROR_TYPE_ENUM
 枚举说明  : TLL2_LTE 告警类型
*****************************************************************************/
enum TLL2_LTE_ERROR_TYPE_ENUM
{
    TLL2_LMAC_RAR_NOTMATCH = 1,
    TLL2_LMAC_RAR_TIMEOUT,
    TLL2_LMAC_CONTENT_TIMEOUT,
    TLL2_LMAC_CONTENT_NOTMATCH,
    TLL2_LMAC_OVER_PBL_MAX_TXNUM,
    TLL2_LMAC_MSG3_FAIL,
    TLL2_LMAC_TA_TIMEOUT,
    TLL2_LMAC_OVER_MAX_SR_SEND,
    TLL2_LRLC_UL_AM_MAX_RETXNUM,
    TLL2_LRLC_DL_TBMEM_FULL,
    TLL2_LPDCP_ERROR_INTEGRITY_VERIFY,
    TLL2_LMAC_DSDS_NO_RF_RA,
    TLL2_LMAC_DSDS_NO_RF_TIMEOUT,
    TLL2_LPDCP_UL_LOSS,                     /*PDCP上行丢包*/
    TLL2_LPDCP_UL_MAX_BUFFTIME,             /*PDCP上行PDU缓存时间过长*/
    TLL2_LPDCP_DL_LOSS,                     /*PDCP下行丢包*/
    TLL2_LMAC_DL_TB_CHECK_ERR,              /*下行误码*/
    TLL2_LPDCP_UL_SIG_PAS,                  /*上行单通*/
    TLL2_IMS_VOICE_STOP,                    /*VoLTE语音卡顿*/
    TLL2_LTE_ERROR_TYPE_BUTT
};
typedef VOS_UINT16 TLL2_LTE_ERROR_TYPE_ENUM_UINT16;

/*****************************************************************************
 枚举名    : TLL2_TDS_ERROR_ALARM_ID_ENUM
 枚举说明  : TLL2_TDS 告警ID
*****************************************************************************/
enum TLL2_TDS_ERROR_ALARM_ID_ENUM
{
    TLL2_TMAC_OUT_OF_SYNC = 1,
    TLL2_TMAC_QUEUE_FULL,
    TLL2_TMAC_SENDMSG_FAIL,
    TLL2_TRLC_RST,
    TLL2_TRLC_UL_BUFFER_FULL,
    TLL2_TPDCP_FAIL,
    TLL2_TMAC_HSUPA_INFO,
    TLL2_TMAC_HSDPA_INFO,
    TLL2_TRLC_UL_TRANS_INFO,
    TLL2_TRLC_DL_TRANS_INFO,
    TLL2_TRLC_STATICS_INFO,

    TLL2_TDS_BUFFER_OVERFLOW,
    TLL2_TDS_ERRLOG_ALARM_ID_BUTT
};
typedef VOS_UINT16 TLL2_TDS_ERROR_ALARM_ID_ENUM_UINT8;


/*****************************************************************************
   5 STRUCT
*****************************************************************************/

/*****************************************************************************
 结构名    : TLL2_ERRLOG_BUFFER_OVERFLOW_STRU
 结构说明  : 缓存区溢出
*****************************************************************************/
typedef struct
{
    OM_ERR_LOG_HEADER_STRU              stHeader;       /* ERR LOG消息头 */

    VOS_UINT32                          ulCount;        /* 缓存区溢出计数 */
}TLL2_ERRLOG_BUFFER_OVERFLOW_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_CDS_STAT_INFO_STRU
 结构说明  : LTE CDS统计量
*****************************************************************************/
typedef struct
{
    /*IPF统计量*/
    VOS_UINT32                          ulULIpfRxRDNum;
    VOS_UINT32                          ulULIpfFilterErrNum;
    VOS_UINT32                          ulULADQCfgAdNum;
    VOS_UINT32                          ulUlAdqEmptyIntNum;
    VOS_UINT32                          ulDLIpfCfgHaveNoBD;
    VOS_UINT32                          ulDLIpfCfgBDSucc;

    /*上行统计量*/
    VOS_UINT32                          ulULASRxIpPktNum;
    VOS_UINT32                          ulULLTERxPktNum;
    VOS_UINT32                          ulULLTESendToLPdcpNum;
    VOS_UINT32                          ulULLTEFcFreePktNum;
    VOS_UINT32                          ulULBuffSoftFilterFail;
    VOS_UINT32                          ulULBuffFreePktNum;

    /*下行统计量*/
    VOS_UINT32                          ulDLLteRxSduNum;
    VOS_UINT32                          ulDLLteEnQueSucc;
    VOS_UINT32                          ulDLLteRxImsNum;
    VOS_UINT32                          ulDLLteFcFreeSduNum;

    /*IMS统计量*/
    VOS_UINT32                          ulImsULRxNicPktNum;
    VOS_UINT32                          ulImsULSoftFilterFail;
    VOS_UINT32                          ulImsDLSendToNicSucc;
}LTE_ERRLOG_CDS_STAT_INFO_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_PDCP_RB_STAT_INFO_STRU
 结构说明  : LTE L2 PDCP统计量
*****************************************************************************/
typedef struct
{
    /*上行统计量*/
    VOS_UINT32                          ulUlRxSduCnt;
    VOS_UINT32                          ulUlTxPduCnt;
    VOS_UINT32                          ulUlAckedSduCnt;
    VOS_UINT32                          ulUlDisSduCnt;
    VOS_UINT32                          ulUlOtherDisSduCnt;
    VOS_UINT32                          ulCompFailCnt;

    /*下行统计量*/
    VOS_UINT32                          ulDlRxPduCnt;
    VOS_UINT32                          ulDlTxSduCnt;
    VOS_UINT32                          ulDlDiscPduCnt;
    VOS_UINT32                          ulDeCompFailCnt;

    /*Cipher统计量*/
    VOS_UINT32                          ulThrChRxSduCnt;
    VOS_UINT32                          ulThrChEnQueSucc;
    VOS_UINT32                          ulThrChHaveNoBDCnt;
    VOS_UINT32                          ulThrChCfgBdNum;
    VOS_UINT32                          ulNonThrRxSduCnt;
    VOS_UINT32                          ulNonThrEnqueSucc;
    VOS_UINT32                          ulNonThrChHaveNoBDCnt;
    VOS_UINT32                          ulNonThrChCfgBdNum;

}LTE_ERRLOG_PDCP_STAT_INFO_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_RLC_RB_STAT_INFO_STRU
 结构说明  : LTE L2 RLC统计量
*****************************************************************************/
typedef struct
{
    /*上行统计量*/
    VOS_UINT32                          ulRxPdcpSdu;
    VOS_UINT32                          ulTxPduToMac;
    VOS_UINT32                          ulTxDataPdu;
    VOS_UINT32                          ulTxCtrlPdu;
    VOS_UINT32                          ulTotalDropSdu;
    VOS_UINT32                          ulTotalRetxPdu;

    /*下行统计量*/
    VOS_UINT32                          ulRxMacPdu;
    VOS_UINT32                          ulTxSduToPdcpSucc;
    VOS_UINT32                          ulRxDataPdu;
    VOS_UINT32                          ulRxCtrlPdu;

}LTE_ERRLOG_RLC_STAT_INFO_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_MAC_STAT_INFO_STRU
 结构说明  : LTE L2 MAC统计量
*****************************************************************************/

typedef struct
{
    /*上行统计量*/
    VOS_UINT32                          ulMacRecvRlcPduTotal;
    VOS_UINT32                          ulMacSchedMacPduSuccNum;
    VOS_UINT32                          ulMacSchedFailNum;
    VOS_UINT32                          ulRecvUlGrantNum;
    VOS_UINT32                          ulBuffLen;
    VOS_UINT32                          ulTaTimeOutNum;

    /*下行统计量*/
    VOS_UINT32                          ulDlMacGetDlTbNum;
    VOS_UINT32                          ulActSendRlcNum;
    VOS_UINT32                          ulDlTbCheckErr;
    VOS_UINT32                          ulRecvScellActivation;

}LTE_ERRLOG_MAC_STAT_INFO_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_L2_STAT_INFO_STRU
 结构说明  : LTE L2数传统计信息上报结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucCurInd;
    VOS_UINT8                           ucRsv[3];
    LTE_ERRLOG_CDS_STAT_INFO_STRU       astCDSStatInfo[LTE_ERRLOG_L2_STAT_RPT_MAX_NUM];
    LTE_ERRLOG_PDCP_STAT_INFO_STRU      astLPDCPStatInfo[LTE_ERRLOG_L2_STAT_RPT_MAX_NUM];
    LTE_ERRLOG_RLC_STAT_INFO_STRU       astLRLCStatInfo[LTE_ERRLOG_L2_STAT_RPT_MAX_NUM];
    LTE_ERRLOG_MAC_STAT_INFO_STRU       astLMACStatInfo[LTE_ERRLOG_L2_STAT_RPT_MAX_NUM];
}LTE_ERRLOG_L2_STAT_INFO_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_L2_SINGLE_THR_INFO_STRU
 结构说明  : LTE L2流量统计信息存储单元
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulAppUlRate;           /*APP上行流量*/
    VOS_UINT32                          ulAppDlRate;           /*APP下行流量*/
    VOS_UINT32                          ulPdcpUlRate;          /*PDCP上行流量*/
    VOS_UINT32                          ulPdcpDlRate;          /*PDCP下行流量*/
    VOS_UINT32                          ulRlcUlRate;           /*RLC上行流量*/
    VOS_UINT32                          ulRlcDlRate;           /*RLC下行流量*/
    VOS_UINT32                          ulMacUlRate;           /*MAC上行流量*/
    VOS_UINT32                          ulMacDlRate;           /*MAC下行流量*/
}LTE_ERRLOG_L2_SINGLE_THR_INFO_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_L2_THR_INFO_RPT_STRU
 结构说明  : LTE L2流量统计信息存储结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                                   ucCurInd;
    VOS_UINT8                                   ucRsv[3];
    LTE_ERRLOG_L2_SINGLE_THR_INFO_STRU          astL2ThrInfo[LTE_ERRLOG_L2_STAT_RPT_MAX_NUM];
}LTE_ERRLOG_L2_THR_INFO_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_L2_ALM_INFO_RPT_STRU
 结构说明  : LTE L2异常场景上报结构
*****************************************************************************/
typedef struct
{
    OM_ERR_LOG_HEADER_STRU              stHeader;       /*ERR LOG消息头*/

    TLL2_LTE_ERROR_TYPE_ENUM_UINT16     enErrType;      /*异常类型*/
    VOS_UINT8                           ucRbId;         /*接入层承载ID*/
    VOS_UINT8                           ucRabId;        /*非接入层承载ID*/
    VOS_UINT8                           ucIsIms;        /*IMS业务指示，0:非IMS，1:IMS*/
    VOS_UINT8                           ucRsv[3];       /*4字节对齐*/
}LTE_ERRLOG_L2_ALM_INFO_RPT_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_L2_STAT_INFO_RPT_STRU
 结构说明  : LTE L2统计量上报结构
*****************************************************************************/
typedef struct
{
    OM_ERR_LOG_HEADER_STRU              stHeader;       /*ERR LOG消息头*/

    LTE_ERRLOG_L2_STAT_INFO_STRU        stL2StatInfo;   /*统计量*/
}LTE_ERRLOG_L2_STAT_INFO_RPT_STRU;

/*****************************************************************************
 结构名    : LTE_ERRLOG_L2_THR_INFO_RPT_STRU
 结构说明  : LTE L2流量上报结构
*****************************************************************************/
typedef struct
{
    OM_ERR_LOG_HEADER_STRU              stHeader;       /*ERR LOG消息头*/

    LTE_ERRLOG_L2_THR_INFO_STRU         stL2ThrInfo;    /*流量*/
}LTE_ERRLOG_L2_THR_INFO_RPT_STRU;

/*****************************************************************************
 结构名    : TDS_ERRLOG_MAC_OUTOFSYNC_INFO
 结构说明  : MAC失步
*****************************************************************************/
typedef struct
{
    OM_ERR_LOG_HEADER_STRU              stHeader;           /* ERR LOG消息头 */
    VOS_UINT8                           cause;              /*T312 expired：0；T313 expired：1*/
    VOS_UINT8                           inActivationTime;
    VOS_UINT8                           tfcsIdentity;
    VOS_UINT8                           aucRev;             /*4字节对齐*/
}TDS_ERRLOG_MAC_OUTOFSYNC_INFO;

/*****************************************************************************
 结构名    : TDS_ERRLOG_MAC_QUEUEFULL_INFO
 结构说明  : MAC队列满结构体
*****************************************************************************/
typedef struct
{
    OM_ERR_LOG_HEADER_STRU              stHeader;       /* ERR LOG消息头 */
    VOS_UINT32                          LossMsgID;
}TDS_ERRLOG_MAC_QUEUEFULL_INFO;

/*****************************************************************************
 结构名    : TDS_ERRLOG_MAC_SENDMSGFAIL_INFO
 结构说明  : MAC消息发送失败结构体
*****************************************************************************/
typedef struct
{
    OM_ERR_LOG_HEADER_STRU              stHeader;       /* ERR LOG消息头 */
    VOS_UINT32                          FailMsgID;
}TDS_ERRLOG_MAC_SENDMSGFAIL_INFO;

/*****************************************************************************
 结构名    : TDS_ERRLOG_RLC_RST_INFO
 结构说明  : RLC RST信息
*****************************************************************************/
typedef struct
{
    OM_ERR_LOG_HEADER_STRU              stHeader;       /* ERR LOG消息头 */
    VOS_UINT8                           rbIdentity;
    VOS_UINT8                           aucRev[3];      /*4字节对齐*/
}TDS_ERRLOG_RLC_RST_INFO;

/*****************************************************************************
 结构名    : TDS_ERRLOG_RLC_UL_BUFFERFULL_INFO
 结构说明  : RLC UL buffer 满
*****************************************************************************/
typedef struct
{
    OM_ERR_LOG_HEADER_STRU              stHeader;       /* ERR LOG消息头 */
    VOS_UINT8                           rbIdentity;
    VOS_UINT8                           aucRev[3];      /*4字节对齐*/
}TDS_ERRLOG_RLC_UL_BUFFERFULL_INFO;

/*****************************************************************************
 结构名    : TDS_ERRLOG_PDCP_FAIL_INFO
 结构说明  : pdcp错误信息上报内容
*****************************************************************************/
typedef struct
{
    OM_ERR_LOG_HEADER_STRU              stHeader;       /* ERR LOG消息头 */
    VOS_UINT32                          failCause;
}TDS_ERRLOG_PDCP_FAIL_INFO;

/*****************************************************************************
 结构名    : TDS_ERRLOG_MAC_HSUPA_INFO_STRU
 结构说明  TDS MAC HSUPA信息结构体
*****************************************************************************/
typedef struct
{
    OM_ERR_LOG_HEADER_STRU              stHeader;

    VOS_UINT16                          usSysFramNo;    /*系统帧号*/
    VOS_UINT16                          usTbSize;       /*MAC UPA上行授权大小*/

    VOS_UINT8                           ucSnpl;         /*MAC UPA SNPL 邻区路损值*/
    VOS_UINT8                           ucUph;          /*MAC UPA UPH*/
    VOS_UINT8                           ucTebs;         /*MAC UPA TEBS*/
    VOS_UINT8                           ucPrri;         /*MAC UPA PRRI*/

    VOS_UINT8                           ucTrri;         /*MAC UPA TRRI*/
    VOS_UINT8                           ucCrri;         /*MAC UPA CRRI*/
    VOS_UINT8                           ucRscpLevel;    /*MAC 服务小区RSCP值*/
    VOS_UINT8                           ucRsv;          /*保留*/

}TDS_ERRLOG_MAC_HSUPA_INFO_STRU;

/*****************************************************************************
 结构名    : TDS_ERRLOG_MAC_HSDPA_INFO_STRU
 结构说明  TDS MAC HSDPA信息结构体
*****************************************************************************/
typedef struct
{
    OM_ERR_LOG_HEADER_STRU              stHeader;
    VOS_UINT16                          usSysFramNo;    /*系统帧号*/
    VOS_UINT16                          usTsn;          /*MAC HSDPA 发送SN值*/
    VOS_UINT16                          usExpectedSn;   /*MAC HSDPA 期待的下一个SN*/
    VOS_UINT16                          usTbSize;       /*MAC HSDPA 下行授权大小*/

}TDS_ERRLOG_MAC_HSDPA_INFO_STRU;

/*****************************************************************************
 结构名    : TDS_ERRLOG_L2_STATICS_INFO_STRU
 结构说明  TDS L2数传统计信息结构体
*****************************************************************************/
typedef struct
{
    OM_ERR_LOG_HEADER_STRU              stHeader;

    VOS_UINT16                          usSysFramNo;    /*系统帧号*/
    VOS_UINT16                          usRsv;

    VOS_UINT8                           ucMacState;     /*MAC当前状态*/
    VOS_UINT8                           ucHsupaFlag;    /*MAC HSUPA是否生效*/
    VOS_UINT8                           ucHsdpaFlag;    /*MAC HSDPA是否生效*/
    VOS_UINT8                           ucRbId;         /*RB ID值*/

    VOS_UINT32                          ulPdcpUlRxNum;  /*PDCP上行从RABM接收到的SDU数目*/
    VOS_UINT32                          ulPdcpUlTxNum;  /*PDCP层上行发送数据包个数*/
    VOS_UINT32                          ulRlcUlTxNum;   /*RLC层上行发送数据包个数*/
    VOS_UINT32                          ulMacUlTxNum;   /*MAC层上行发送数据包个数*/

    VOS_UINT32                          ulPdcpDlTxNum;  /*PDCP下行发送给RABM的SDU数目*/
    VOS_UINT32                          ulPdcpDlRxNum;  /*PDCP层下行接收数据包个数*/
    VOS_UINT32                          ulRlcDlRxNum;   /*RLC层下行接收数据包个数*/
    VOS_UINT32                          ulMacDlRxNum;   /*MAC层下行接收数据包个数*/

}TDS_ERRLOG_L2_STATICS_INFO_STRU;

/*****************************************************************************
 结构名    : TDS_ERRLOG_RLC_UL_TRANS_INFO_STRU
 结构说明  TDS L2 RLC上行数传信息结构体
*****************************************************************************/
typedef struct
{
    OM_ERR_LOG_HEADER_STRU              stHeader;

    VOS_UINT16                          usSysFramNo;    /*系统帧号*/
    VOS_UINT16                          usUlSendSn;     /*RLC层上行发送的PDU SN值*/

    VOS_UINT8                           ucRbId;         /*RB ID*/
    VOS_UINT8                           ucPduSize;      /*RLC 层PDU 大小，单位byte*/
    VOS_UINT8                           ucPduNum;       /*一次组包RLC PDU个数*/
    VOS_UINT8                           ucRsv;

    VOS_UINT32                          ulRlcBo;        /*RLC 层缓存量统计*/
    VOS_UINT32                          ulPdcpBo;       /*PDCP 层缓存量统计*/

    VOS_UINT16                          usVTs;          /*RLC 层窗口变量VTS*/
    VOS_UINT16                          usVTa;          /*RLC 层窗口变量VTA*/

}TDS_ERRLOG_RLC_UL_TRANS_INFO_STRU;

/*****************************************************************************
 结构名    : TDS_ERRLOG_RLC_DL_TRANS_INFO_STRU
 结构说明  TDS L2 RLC下行数传信息结构体
*****************************************************************************/
typedef struct
{

    OM_ERR_LOG_HEADER_STRU              stHeader;

    VOS_UINT16                          usSysFramNo;    /*系统帧号*/
    VOS_UINT16                          usDlRecvSn;     /*RLC层下行发送的PDU SN值*/


    VOS_UINT8                           ucRbId;         /*RB ID*/
    VOS_UINT8                           ucPduSize;      /*RLC 层PDU 大小，单位byte*/
    VOS_UINT8                           ucPduNum;       /*一次组包RLC PDU个数*/
    VOS_UINT8                           ucRsv;

    VOS_UINT16                          usVRr;          /*RLC 层窗口变量VR(R)*/
    VOS_UINT16                          usVRh;          /*RLC 层窗口变量VR(H)*/

}TDS_ERRLOG_RLC_DL_TRANS_INFO_STRU;

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

#endif /* end of TLL2ErrorLog.h */
