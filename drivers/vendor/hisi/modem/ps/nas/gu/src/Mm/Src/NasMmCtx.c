

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "NasMmCtx.h"
#include "MM_Ext.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NAS_MM_CTX_C
/*lint +e767*/


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
NAS_MM_CONTEXT_STRU                     gstNasMmCtx;


/*****************************************************************************
  3 函数实现
*****************************************************************************/

NAS_MM_CONTEXT_STRU* NAS_MM_GetCtx(VOS_VOID)
{
    return &gstNasMmCtx;
}


VOS_UINT32 NAS_MM_GetLcsEstReqTi(VOS_VOID)
{
    return NAS_MM_GetCtx()->stCmEntityCtx.stLcsEntityCtx.stLcsEstReq.ulTi;
}


VOS_VOID NAS_MM_SetLcsEstReqTi(VOS_UINT32 ulTi)
{
    NAS_MM_GetCtx()->stCmEntityCtx.stLcsEntityCtx.stLcsEstReq.ulTi = ulTi;
    return;
}


VOS_UINT32 NAS_MM_GetLcsRelReqTi(VOS_VOID)
{
    return NAS_MM_GetCtx()->stCmEntityCtx.stLcsEntityCtx.stLcsRelReq.ulTi;
}


VOS_VOID NAS_MM_SetLcsRelReqTi(VOS_UINT32 ulTi)
{
    NAS_MM_GetCtx()->stCmEntityCtx.stLcsEntityCtx.stLcsRelReq.ulTi = ulTi;
    return;
}


RRMM_LCS_STATE_ENUM_UINT8 NAS_MM_GetRrLcsState(VOS_VOID)
{
    return NAS_MM_GetCtx()->stRrInfoCtx.enLcsState;
}


VOS_VOID NAS_MM_SetRrLcsState(
    RRMM_LCS_STATE_ENUM_UINT8           enLcsState
)
{
    NAS_MM_GetCtx()->stRrInfoCtx.enLcsState = enLcsState;
    return;
}


MM_CONN_CTRL_STRU *NAS_MM_GetConnCtrlInfo(
    NAS_MM_CONN_CTRL_ENUM_UINT8         enMmConnCtrl
)
{
    return &g_MmGlobalInfo.ConnCtrlInfo[enMmConnCtrl];
}


VOS_VOID NAS_MM_SetStaOfRcvXXEstReq(
    NAS_MM_STATE_ENUM_UINT8             enState
)
{
    g_MmGlobalInfo.ucStaOfRcvXXEstReq = enState;
    return;
}


NAS_MM_STATE_ENUM_UINT8 NAS_MM_GetStaOfRcvXXEstReq(VOS_VOID)
{
    return g_MmGlobalInfo.ucStaOfRcvXXEstReq;
}


VOS_VOID NAS_MM_SetStaAfterWaitForNwkCmd(
    NAS_MM_STATE_ENUM_UINT8             enState
)
{
    g_MmGlobalInfo.ucStaAfterWaitForNwkCmd = enState;
    return;
}


NAS_MM_STATE_ENUM_UINT8 NAS_MM_GetMmState(VOS_VOID)
{
    return g_MmGlobalInfo.ucState;
}


VOS_VOID NAS_MM_SetMmPreState(
    NAS_MM_STATE_ENUM_UINT8             enState
)
{
    g_MmGlobalInfo.ucPreState = enState;
    return;
}


NAS_MM_STATE_ENUM_UINT8 NAS_MM_GetMmPreState(VOS_VOID)
{
    return g_MmGlobalInfo.ucPreState;
}


VOS_VOID NAS_MM_SetMmServiceState(
    MM_SERVICE_STATUS_ENUM_UINT8        enMmServiceState
)
{
    g_MmGlobalInfo.ucMmServiceState = enMmServiceState;
    return;
}


MM_SERVICE_STATUS_ENUM_UINT8 NAS_MM_GetMmServiceState(VOS_VOID)
{
    return g_MmGlobalInfo.ucMmServiceState;
}


VOS_VOID NAS_MM_SetMMConnEstingPD(
    VOS_UINT8                           ucMmPD
)
{
    g_MmGlobalInfo.ucMMConnEstingPD = ucMmPD;
    return;
}


VOS_VOID NAS_MM_InitLcsEntityCtx(VOS_VOID)
{
    NAS_MM_SetLcsEstReqTi(NAS_MM_CM_TI_INVALID);
    NAS_MM_SetLcsRelReqTi(NAS_MM_CM_TI_INVALID);

    return;
}


VOS_VOID NAS_MM_InitRrInfoCtx(VOS_VOID)
{
    NAS_MM_SetRrLcsState(RRMM_LCS_STATE_FINISH);

    return;
}


VOS_VOID NAS_MM_InitCtx(VOS_VOID)
{
    NAS_MM_InitLcsEntityCtx();
    NAS_MM_InitRrInfoCtx();

    return;
}







#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

