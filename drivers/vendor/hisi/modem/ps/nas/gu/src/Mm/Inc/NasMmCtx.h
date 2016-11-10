

#ifndef __NASMMCTX_H__
#define __NASMMCTX_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "MM_Global.h"
#include "Nasrrcinterface.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define NAS_MM_CM_TI_INVALID            16

/*****************************************************************************
  3 枚举定义
*****************************************************************************/


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
    VOS_UINT32                          ulTi;
}NAS_MM_LCS_EST_REQ_STRU;


typedef struct
{
    VOS_UINT32                          ulTi;
}NAS_MM_LCS_REL_REQ_STRU;


typedef struct
{
    NAS_MM_LCS_EST_REQ_STRU             stLcsEstReq;                            /* MM模块LCS建链请求记录 */
    NAS_MM_LCS_REL_REQ_STRU             stLcsRelReq;                            /* MM模块LCS释放链路请求 */
}NAS_MM_LCS_ENTITY_CTX_STRU;


typedef struct
{
    NAS_MM_LCS_ENTITY_CTX_STRU          stLcsEntityCtx;                         /* CM层LCS业务实体上下文 */
}NAS_MM_CM_ENTITY_CTX_STRU;

typedef struct
{
    RRMM_LCS_STATE_ENUM_UINT8           enLcsState;                             /* 接入层RRLP或RRC LCS流程状态 */
    VOS_UINT8                           aucReserved[3];
}NAS_MM_RR_INFO_CTX_STRU;


typedef struct
{
    NAS_MM_CM_ENTITY_CTX_STRU           stCmEntityCtx;                          /* MM模块存放的CM层实体上下文 */
    NAS_MM_RR_INFO_CTX_STRU             stRrInfoCtx;                            /* MM模块存放接入层信息上下文 */
}NAS_MM_CONTEXT_STRU;



/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern MM_CONN_CTRL_STRU *NAS_MM_GetConnCtrlInfo(
           NAS_MM_CONN_CTRL_ENUM_UINT8         enMmConnCtrl
       );
extern NAS_MM_CONTEXT_STRU* NAS_MM_GetCtx(VOS_VOID);
extern VOS_UINT32 NAS_MM_GetLcsEstReqTi(VOS_VOID);
extern VOS_UINT32 NAS_MM_GetLcsRelReqTi(VOS_VOID);
extern NAS_MM_STATE_ENUM_UINT8 NAS_MM_GetMmPreState(VOS_VOID);
extern MM_SERVICE_STATUS_ENUM_UINT8 NAS_MM_GetMmServiceState(VOS_VOID);
extern NAS_MM_STATE_ENUM_UINT8 NAS_MM_GetMmState(VOS_VOID);
extern RRMM_LCS_STATE_ENUM_UINT8 NAS_MM_GetRrLcsState(VOS_VOID);
extern NAS_MM_STATE_ENUM_UINT8 NAS_MM_GetStaOfRcvXXEstReq(VOS_VOID);
extern VOS_VOID NAS_MM_InitCtx(VOS_VOID);
extern VOS_VOID NAS_MM_InitLcsEntityCtx(VOS_VOID);
extern VOS_VOID NAS_MM_InitRrInfoCtx(VOS_VOID);
extern VOS_VOID NAS_MM_SetLcsEstReqTi(VOS_UINT32 ulTi);
extern VOS_VOID NAS_MM_SetLcsRelReqTi(VOS_UINT32 ulTi);
extern VOS_VOID NAS_MM_SetMMConnEstingPD(
           VOS_UINT8                           ucMmPD
       );
extern VOS_VOID NAS_MM_SetMmPreState(
           NAS_MM_STATE_ENUM_UINT8             enState
       );
extern VOS_VOID NAS_MM_SetMmServiceState(
           MM_SERVICE_STATUS_ENUM_UINT8        enMmServiceState
       );
extern VOS_VOID NAS_MM_SetRrLcsState(
           RRMM_LCS_STATE_ENUM_UINT8           enLcsState
       );
extern VOS_VOID NAS_MM_SetStaAfterWaitForNwkCmd(
           NAS_MM_STATE_ENUM_UINT8             enState
       );
extern VOS_VOID NAS_MM_SetStaOfRcvXXEstReq(
           NAS_MM_STATE_ENUM_UINT8             enState
       );







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

#endif /* end of NasMmCtx.h */

