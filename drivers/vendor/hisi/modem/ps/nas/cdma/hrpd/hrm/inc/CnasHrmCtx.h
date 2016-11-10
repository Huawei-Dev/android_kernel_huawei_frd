

#ifndef __CNAS_HRM_CTX_H__
#define __CNAS_HRM_CTX_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "vos.h"
#include  "PsCommonDef.h"
#include  "PsLogdef.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)


/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define CNAS_HRM_DATA_REQ_BUFF_INDEX_UATI_REQ                                   (0)
#define CNAS_HRM_DATA_REQ_BUFF_INDEX_UATI_CMPL                                  (1)
#define CNAS_HRM_DATA_REQ_BUFF_INDEX_HARDWARE_ID                                (2)
#define CNAS_HRM_DATA_REQ_BUFF_INDEX_MO_KEEP_ALIVE                              (3)
#define CNAS_HRM_DATA_REQ_BUFF_INDEX_MT_KEEP_ALVIE                              (4)
#define CNAS_HRM_DATA_REQ_BUFF_INDEX_SESSION_CLOSE                              (5)
#define CNAS_HRM_DATA_REQ_BUFF_INDEX_LOC_NTF                                    (6)
#define CNAS_HRM_DATA_REQ_BUFF_INDEX_HANDOFF_LOC_NTF                            (7)
#define CNAS_HRM_DATA_REQ_BUFF_INDEX_LOC_CMPL                                   (8)
#define CNAS_HRM_DATA_REQ_BUFF_INDEX_BLOB_NTF                                   (9)
#define CNAS_HRM_DATA_REQ_BUFF_INDEX_BLOB_CMPL                                  (10)

#define CNAS_HRM_DATA_REQ_BUFF_INDEX_MAX                                        (15)

#define CNAS_HRM_BYTE_LEN                                                       (8)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/


enum CNAS_HRM_RES_APPLY_STATE_ENUM
{
    CNAS_HRM_RES_APPLY_STATE_NOT_APPLY,                     /* 资源未申请 */
    CNAS_HRM_RES_APPLY_STATE_APPLYING,                      /* 资源正在申请 */
    CNAS_HRM_RES_APPLY_STATE_APPLY_SUCC,                    /* 资源申请成功 */
    CNAS_HRM_RES_APPLY_STATE_APPLY_FAIL,                    /* 资源申请失败 */

    CNAS_HRM_RES_APPLY_STATE_BUTT
};
typedef VOS_UINT8 CNAS_HRM_RES_APPLY_STATE_ENUM_UINT8;


enum CNAS_HRM_RES_OCCUPY_TYPE_ENUM
{
    CNAS_HRM_RES_OCCUPY_TYPE_NON,                           /* 资源没有被抢占 */
    CNAS_HRM_RES_OCCUPY_TYPE_SHORT,                         /* 资源被短抢 */
    CNAS_HRM_RES_OCCUPY_TYPE_LONG,                          /* 资源被长抢 */

    CNAS_HRM_RES_OCCUPY_TYPE_BUTT
};
typedef VOS_UINT8 CNAS_HRM_RES_OCCUPY_TYPE_ENUM_UINT8;


enum CNAS_HRM_DSDS_TYPE_ENUM
{
    CNAS_HRM_DSDS_TYPE_1    = 0x01,                         /* DSDS1.0 单发单收 */
    CNAS_HRM_DSDS_TYPE_2    = 0x02,                         /* DSDS2.0 单发双收 */
    CNAS_HRM_DSDS_TYPE_BUTT
};
typedef VOS_UINT8 CNAS_HRM_DSDS_TYPE_ENUM_UINT8;

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/

typedef struct
{
    VOS_UINT32                                              ulIsReqMsgExisted;  /* 收到HSM或者HLU的消息时置为true */
    VOS_UINT16                                              usReqOpId;          /* HSM或者HLU请求中带的OpId */
    VOS_UINT16                                              usHrmOpId;          /* HRM将HSM或者HLU请求发给SNP时带的OpId */
    VOS_UINT8                                              *pucMsg;             /* 缓存消息动态申请 */
}CNAS_HRM_DATA_REQ_BUFF_MSG_STRU;


typedef struct
{
    CNAS_HRM_RES_APPLY_STATE_ENUM_UINT8                     enResApplyState;    /* 当前资源申请的状态 */
    CNAS_HRM_RES_OCCUPY_TYPE_ENUM_UINT8                     enResOccupyType;    /* 当前资源被抢占的类型 */
    VOS_UINT8                                               aucRsv[2];
}CNAS_HRM_DATA_REQ_BUFF_RES_INFO_STRU;


typedef struct
{
    CNAS_HRM_DATA_REQ_BUFF_MSG_STRU                         stReqMsg;
    CNAS_HRM_DATA_REQ_BUFF_RES_INFO_STRU                    stResInfo;
}CNAS_HRM_DATA_REQ_BUFF_STRU;


typedef struct
{
    VOS_UINT8                           ucIsNeedWaitResApplied;                 /* 记录是否需要等资源申请结果 */
    CNAS_HRM_DSDS_TYPE_ENUM_UINT8       enDsdsType;                             /* DSDS版本号*/
    VOS_UINT16                          usCurOpId;                              /* 和SNP请求的OP ID */
    CNAS_HRM_DATA_REQ_BUFF_STRU         astDataReqBuff[CNAS_HRM_DATA_REQ_BUFF_INDEX_MAX];
}CNAS_HRM_CTX_STRU;


/*****************************************************************************
 Structure      : CNAS_HRM_PC_REPLAY_FIXED_CONTEXT_MSG_STRU
 Description    : PC回放工程，所有HRM相关的固定全局变量通过消息结构发送
 Message origin :
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucIsNeedWaitResApplied;                 /* 记录是否需要等资源申请结果 */
    CNAS_HRM_DSDS_TYPE_ENUM_UINT8       enDsdsType;                             /* DSDS版本号*/
    VOS_UINT8                           aucRsv[2];
}CNAS_HRM_PC_REPLAY_FIXED_CONTEXT_MSG_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/




/*****************************************************************************
  10 函数声明
*****************************************************************************/
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
CNAS_HRM_CTX_STRU* CNAS_HRM_GetCtxAddr(VOS_VOID);

CNAS_HRM_DATA_REQ_BUFF_MSG_STRU* CNAS_HRM_GetDataReqBuffMsgAddr(VOS_UINT8 ucIndex);

CNAS_HRM_DATA_REQ_BUFF_RES_INFO_STRU* CNAS_HRM_GetDataReqBuffResInfoAddr(VOS_UINT8 ucIndex);

CNAS_HRM_DATA_REQ_BUFF_STRU* CNAS_HRM_GetDataReqBuffAddr(VOS_UINT8 ucIndex);

VOS_VOID CNAS_HRM_ClearMsgInSpecifiedDataReqBuff(
    VOS_UINT8                           ucIndex
);

VOS_VOID CNAS_HRM_ClearResInfoInSpecifiedDataReqBuff(
    VOS_UINT8                           ucIndex
);

VOS_VOID CNAS_HRM_InitCtx(VOS_VOID);

VOS_UINT16 CNAS_HRM_GetOpId(VOS_VOID);

CNAS_HRM_DSDS_TYPE_ENUM_UINT8 CNAS_HRM_GetDsdsType(VOS_VOID);

VOS_UINT8 CNAS_HRM_IsNeedWaitResApplied(VOS_VOID);

VOS_VOID CNAS_HRM_ReadNvHrpdRfAllocSwitchMask(VOS_VOID);

VOS_VOID CNAS_HRM_ReadNvFemChanProfile(VOS_VOID);

VOS_VOID CNAS_HRM_ReadNvimInfo(VOS_VOID);

#if defined(DMT)
VOS_VOID CNAS_HRM_SetDsdsType_DMT(
    CNAS_HRM_DSDS_TYPE_ENUM_UINT8       enDsdsType
);

VOS_VOID CNAS_HRM_SetIsNeedWaitResApplied_DMT(
    VOS_UINT8                           ucIsNeedWaitResApplied
);

#endif


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

#endif /* end of CnasHrmCtx.h */

