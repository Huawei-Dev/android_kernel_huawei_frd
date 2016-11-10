

#ifndef __IMSALRRCINTERFACE_H__
#define __IMSALRRCINTERFACE_H__

/*****************************************************************************
  1 Include Headfile
*****************************************************************************/

#include    "vos.h"
#include    "LPSCommon.h"
#include    "LPsNvInterface.h"
#include    "LPsOm.h"



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
#define  IMSA_LRRC_MSG_HDR   (PS_MSG_ID_IMSA_TO_ERRC_BASE)


#define  LRRC_IMSA_MSG_HDR   (PS_MSG_ID_ERRC_TO_IMSA_BASE)







/*****************************************************************************
  3 Massage Declare
*****************************************************************************/

enum    IMSA_LRRC_MSG_ID_ENUM
{
    ID_IMSA_LRRC_BEGIN_SESSION_NOTIFY     = (IMSA_LRRC_MSG_HDR + 0x00),     /* _H2ASN_MsgChoice IMSA_LRRC_BEGIN_SESSION_NOTIFY_STRU */
    ID_IMSA_LRRC_END_SESSION_NOTIFY       = (IMSA_LRRC_MSG_HDR + 0x01),     /* _H2ASN_MsgChoice IMSA_LRRC_END_SESSION_NOTIFY_STRU */

    ID_IMSA_LRRC_HIFI_SYNC_SWITCH_IND     = (IMSA_LRRC_MSG_HDR + 0x02),     /* _H2ASN_MsgChoice IMSA_LRRC_HIFI_SYNC_SWITCH_IND_STRU */

    ID_IMSA_LRRC_VOLTE_STATUS_NOTIFY      = (IMSA_LRRC_MSG_HDR + 0x03),    /* _H2ASN_MsgChoice IMSA_LRRC_VOLTE_STATUS_NOTIFY_STRU */
    ID_IMSA_LRRC_QUALITY_CFG_REQ          = (IMSA_LRRC_MSG_HDR + 0x04),     /* _H2ASN_MsgChoice IMSA_LRRC_QUALITY_CFG_REQ_STRU */

    ID_LRRC_IMSA_QUALITY_CFG_CNF          = (LRRC_IMSA_MSG_HDR + 0x00),     /* _H2ASN_MsgChoice LRRC_IMSA_QUALITY_CFG_CNF_STRU */
    ID_LRRC_IMSA_STATE_IND                = (LRRC_IMSA_MSG_HDR + 0x01),     /* _H2ASN_MsgChoice LRRC_IMSA_STATE_IND_STRU */


    ID_IMSA_LRRC_MSG_ID_ENUM_BUTT
};
typedef VOS_UINT32   IMSA_LRRC_MSG_ID_ENUM_UINT32;



/*****************************************************************************
  4 Enum
*****************************************************************************/

enum IMSA_LRRC_SESSION_TYPE_ENUM
{
    IMSA_LRRC_SESSION_TYPE_MO_NORMAL_CALL,
    IMSA_LRRC_SESSION_TYPE_MT_NORMAL_CALL,
    IMSA_LRRC_SESSION_TYPE_EMERGENCY_CALL,
    IMSA_LRRC_SESSION_TYPE_MO_SS,
    IMSA_LRRC_SESSION_TYPE_MT_SS,
    IMSA_LRRC_SESSION_TYPE_MO_SMS,
    IMSA_LRRC_SESSION_TYPE_MT_SMS,
    IMSA_LRRC_SESSION_TYPE_REGISTER,
    IMSA_LRRC_SESSION_TYPE_BUTT
};
typedef VOS_UINT8 IMSA_LRRC_SESSION_TYPE_ENUM_UINT8;

enum IMSA_LRRC_CFG_QUALITY_ENUM
{
    IMSA_LRRC_CFG_QUALITY_START      = 0,
    IMSA_LRRC_CFG_QUALITY_STOP,

    IMSA_LRRC_CFG_QUALITY_BUTT
};

typedef VOS_UINT8 IMSA_LRRC_CFG_QUALITY_ENUM_UINT8;

/*****************************************************************************
    枚举名    : LTE_QUALITY_LEVEL_ENUM
    枚举说明  : LTE质量等级枚举
*****************************************************************************/
enum LTE_IMSA_QUALITY_LEVEL_ENUM
{
    LTE_IMSA_QUALITY_LEVEL_BAD         = 0x00,
    LTE_IMSA_QUALITY_LEVEL_NORMAL      = 0x01,
    LTE_IMSA_QUALITY_LEVEL_GOOD        = 0x02,

    LTE_IMSA_QUALITY_LEVEL_BUTT
};
typedef VOS_INT8 LTE_IMSA_QUALITY_LEVEL_ENUM_UINT8;

/*****************************************************************************
 枚举名    : LRRC_IMSA_RESULT_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : IMSA原语操作结果数据结构
*****************************************************************************/
enum LRRC_IMSA_RESULT_ENUM
{
    LRRC_IMSA_RESULT_SUCC                 = 0,                /* 操作成功 */
    LRRC_IMSA_RESULT_FAIL ,                                   /* 操作失败 */
    LRRC_IMSA_RESULT_BUTT
};
typedef VOS_UINT16    LRRC_IMSA_RESULT_ENUM_UINT16;


enum IMSA_LRRC_VOLTE_STATUS_ENUM
{
    IMSA_LRRC_VOLTE_STATUS_INIT,
    IMSA_LRRC_VOLTE_STATUS_START,
    IMSA_LRRC_VOLTE_STATUS_END,

    IMSA_LRRC_VOLTE_STATUS_BUTT
};
typedef VOS_UINT8  IMSA_LRRC_VOLTE_STATUS_ENUM_UINT8;


enum IMSA_LRRC_VOLTE_CALL_TYPE_ENUM
{
    IMSA_LRRC_VOLTE_CALL_TYPE_MO,
    IMSA_LRRC_VOLTE_CALL_TYPE_MT,

    IMSA_LRRC_VOLTE_CALL_TYPE_BUTT
};
typedef VOS_UINT8  IMSA_LRRC_VOLTE_CALL_TYPE_ENUM_UINT8;

/*****************************************************************************
  5 STRUCT
*****************************************************************************/

typedef struct
{
    MSG_HEADER_STRU                                 stMsgHeader;          /* 消息头    */   /*_H2ASN_Skip*/
    IMSA_LRRC_SESSION_TYPE_ENUM_UINT8               enSessionType;
    VOS_UINT8                                       aucReserved[3];
}IMSA_LRRC_BEGIN_SESSION_NOTIFY_STRU;


typedef struct
{
    MSG_HEADER_STRU                                 stMsgHeader;          /* 消息头    */   /*_H2ASN_Skip*/
    IMSA_LRRC_SESSION_TYPE_ENUM_UINT8               enSessionType;
    VOS_UINT8                                       aucReserved[3];
}IMSA_LRRC_END_SESSION_NOTIFY_STRU;


typedef struct
{
    MSG_HEADER_STRU                                 stMsgHeader;          /* 消息头    */   /*_H2ASN_Skip*/
    VOS_UINT8                                       ucHifiSyncEnabled;    /* 0表示关闭；1表示打开 */
    VOS_UINT8                                       ucPowerState;         /* Modem1开关机状态，0表示关闭;1表示打开 */
    VOS_UINT8                                       aucReserved[2];
}IMSA_LRRC_HIFI_SYNC_SWITCH_IND_STRU;



typedef struct
{
    MSG_HEADER_STRU                                 stMsgHeader;          /* 消息头    */   /*_H2ASN_Skip*/
    IMSA_LRRC_CFG_QUALITY_ENUM_UINT8                enStartOrStop;         /* 配置启动还是停止上报LTE信号质量 */
    VOS_UINT8                                       aucReserved[3];
}IMSA_LRRC_QUALITY_CFG_REQ_STRU;



typedef struct
{
    MSG_HEADER_STRU                                 stMsgHeader;          /* 消息头    */   /*_H2ASN_Skip*/
    LTE_IMSA_QUALITY_LEVEL_ENUM_UINT8               enLteQuality;         /* LTE信号质量 */
    VOS_UINT8                                       aucReserved[3];
}LRRC_IMSA_STATE_IND_STRU;



typedef struct
{
    MSG_HEADER_STRU                                 stMsgHeader;          /* 消息头    */   /*_H2ASN_Skip*/
    LRRC_IMSA_RESULT_ENUM_UINT16                    usResult;
    VOS_UINT8                                       aucReserved[2];
}LRRC_IMSA_QUALITY_CFG_CNF_STRU;





/*****************************************************************************
 结构名    : LRRC_IMSA_VOWIFI_RPT_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : vowifi 能量上报
*****************************************************************************/
typedef struct
{
    IMSA_LRRC_CFG_QUALITY_ENUM_UINT8        enStartRptFlg;      /* 当前是否开始上报 */
    LTE_IMSA_QUALITY_LEVEL_ENUM_UINT8       enLastQuality;      /* 上一次上报的质量 */
    LTE_IMSA_QUALITY_LEVEL_ENUM_UINT8       enUnRptQuality;     /* 当前有变化但未上报的质量 */
    LTE_IMSA_QUALITY_LEVEL_ENUM_UINT8       enCurrQuality;      /* 当前的质量 */
    LPS_32K_TIMESTAMP                       stLatestChangeTime; /* 最近一次状态改变的时间 */
    LRRC_NV_VOWIFI_THRES_STRU               stVoWifiThres;      /* 门限 */
    LPS_32K_TIMESTAMP                       stStableIntervalTime; /*迟滞上报，在稳定区间上触发上报开始计时时刻*/
    VOS_INT16                               sThresWlanDeltaP;   /*服务小区上报质量迟滞值,默认5db*/
    PS_BOOL_ENUM_UINT8                      ucLastBadToNormalEvaFlg;/* 上一次bad转normal评估标记位,true:已评估;否则,未评估 */
    PS_BOOL_ENUM_UINT8                      ucLastNormalToBadEvaFlg;/* 上一次normal转bad评估标记位,true:已评估;否则,未评估 */

}LRRC_IMSA_VOWIFI_RPT_INFO_STRU;

/*****************************************************************************
 函数名称: IMSA_LRRC_GetSsacInfo
 协议表格:
 ASN.1 描述:
 函数说明:
*****************************************************************************/
typedef struct
{
    VOS_UINT8    ucBarFactorForVoice;   /*取值范围:0~15枚举值*/
    VOS_UINT8    ucBarTimeForVoice;     /*取值范围:0~7枚举值,单位:s*/
    VOS_UINT8    ucBarFactorForVideo;   /*取值范围:0~15枚举值*/
    VOS_UINT8    ucBarTimeForVideo;     /*取值范围:0~7枚举值,单位:s*/
}IMSA_LRRC_SSAC_INFO_STRU;

/*****************************************************************************
 结构名    : IMSA_LRRC_VOLTE_STATUS_NOTIFY_STRU
 结构说明  : IMSA通知LRRC VOLTE流程启动以及结束
*****************************************************************************/
typedef struct
{
    MSG_HEADER_STRU                                 stMsgHeader;          /*_H2ASN_Skip*/
    IMSA_LRRC_VOLTE_STATUS_ENUM_UINT8               enVolteStatus;
    IMSA_LRRC_VOLTE_CALL_TYPE_ENUM_UINT8            enVolteCallTye;
    VOS_UINT8                                       aucReserved[2];
}IMSA_LRRC_VOLTE_STATUS_NOTIFY_STRU;


/*****************************************************************************
 函数名称: IMSA_LRRC_GetSsacInfo
 协议表格:
 ASN.1 描述:
 函数说明:LRRC提供给IMSA SSAC相关信息
*****************************************************************************/
extern VOS_UINT32  IMSA_LRRC_GetSSACInfo
(
    IMSA_LRRC_SSAC_INFO_STRU *pstSsacInfo
);



extern VOS_UINT32 IMSA_LRRC_JudgeCurrCellSingalQuality(VOS_VOID);

/*****************************************************************************
 函数名称: IMSA_LRRC_IsAcbEnabled
 协议表格:
 ASN.1 描述:
 函数说明:LRRC提供给IMSA ACB Enable相关信息
*****************************************************************************/
extern VOS_UINT32 IMSA_LRRC_IsAcbEnabled(VOS_VOID);
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

#endif /* end of ImsaLrrcInterface.h */








