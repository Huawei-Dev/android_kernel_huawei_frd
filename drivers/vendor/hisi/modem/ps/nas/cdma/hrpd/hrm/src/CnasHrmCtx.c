

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "CnasHrmCtx.h"
#include "CnasMntn.h"
#include "Nas_Mem.h"
#include "NVIM_Interface.h"
#include "TtfNvInterface.h"
#include "CnasCcb.h"


#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_CNAS_HRM_CTX_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
CNAS_HRM_CTX_STRU                       g_stCnasHrmCtx;

#if defined(DMT)
CNAS_HRM_DSDS_TYPE_ENUM_UINT8           g_enDsdsType_Dmt = CNAS_HRM_DSDS_TYPE_1;
VOS_UINT8                               g_ucIsNeedWaitResApplied_Dmt = VOS_FALSE;

#endif

/*****************************************************************************
  3 函数定义
*****************************************************************************/


CNAS_HRM_CTX_STRU* CNAS_HRM_GetCtxAddr(VOS_VOID)
{
    return &(g_stCnasHrmCtx);
}


CNAS_HRM_DATA_REQ_BUFF_STRU* CNAS_HRM_GetDataReqBuffAddr(VOS_UINT8 ucIndex)
{
    if (ucIndex > (CNAS_HRM_DATA_REQ_BUFF_INDEX_MAX - 1))
    {
        CNAS_ERROR_LOG(UEPS_PID_HRM, "CNAS_HRM_GetDataReqBuffAddr: invalid index");

        return &(CNAS_HRM_GetCtxAddr()->astDataReqBuff[CNAS_HRM_DATA_REQ_BUFF_INDEX_MAX -1]);
    }

    return &(CNAS_HRM_GetCtxAddr()->astDataReqBuff[ucIndex]);
}


CNAS_HRM_DATA_REQ_BUFF_MSG_STRU* CNAS_HRM_GetDataReqBuffMsgAddr(VOS_UINT8 ucIndex)
{
    if (ucIndex > (CNAS_HRM_DATA_REQ_BUFF_INDEX_MAX - 1))
    {
        CNAS_ERROR_LOG(UEPS_PID_HRM, "CNAS_HRM_GetDataReqBuffMsgAddr: invalid index");

        return &(CNAS_HRM_GetCtxAddr()->astDataReqBuff[CNAS_HRM_DATA_REQ_BUFF_INDEX_MAX -1].stReqMsg);
    }

    return &(CNAS_HRM_GetCtxAddr()->astDataReqBuff[ucIndex].stReqMsg);
}


CNAS_HRM_DATA_REQ_BUFF_RES_INFO_STRU* CNAS_HRM_GetDataReqBuffResInfoAddr(VOS_UINT8 ucIndex)
{
    if (ucIndex > (CNAS_HRM_DATA_REQ_BUFF_INDEX_MAX - 1))
    {
        CNAS_ERROR_LOG(UEPS_PID_HRM, "CNAS_HRM_GetDataReqBuffResInfoAddr: invalid index");

        return &(CNAS_HRM_GetCtxAddr()->astDataReqBuff[CNAS_HRM_DATA_REQ_BUFF_INDEX_MAX -1].stResInfo);
    }

    return &(CNAS_HRM_GetCtxAddr()->astDataReqBuff[ucIndex].stResInfo);
}


VOS_VOID CNAS_HRM_ClearMsgInSpecifiedDataReqBuff(
    VOS_UINT8                           ucIndex
)
{
    CNAS_HRM_DATA_REQ_BUFF_MSG_STRU    *pstReqMsg = VOS_NULL_PTR;

    pstReqMsg = CNAS_HRM_GetDataReqBuffMsgAddr(ucIndex);

    pstReqMsg->ulIsReqMsgExisted    = VOS_FALSE;
    pstReqMsg->usReqOpId            = 0;
    pstReqMsg->usHrmOpId            = 0;

    if (VOS_NULL_PTR != pstReqMsg->pucMsg)
    {
        PS_MEM_FREE(UEPS_PID_HRM, pstReqMsg->pucMsg);

        pstReqMsg->pucMsg = VOS_NULL_PTR;
    }
}


VOS_VOID CNAS_HRM_ClearAllDataReqBuff(VOS_VOID)
{
    VOS_UINT8                           ucIndex;

    for (ucIndex = 0; ucIndex < CNAS_HRM_DATA_REQ_BUFF_INDEX_MAX; ucIndex++)
    {
        CNAS_HRM_ClearMsgInSpecifiedDataReqBuff(ucIndex);

        CNAS_HRM_ClearResInfoInSpecifiedDataReqBuff(ucIndex);
    }


}


VOS_VOID CNAS_HRM_ClearResInfoInSpecifiedDataReqBuff(
    VOS_UINT8                           ucIndex
)
{
    CNAS_HRM_DATA_REQ_BUFF_RES_INFO_STRU                   *pstResInfo = VOS_NULL_PTR;

    pstResInfo = CNAS_HRM_GetDataReqBuffResInfoAddr(ucIndex);

    pstResInfo->enResApplyState      = CNAS_HRM_RES_APPLY_STATE_NOT_APPLY;
    pstResInfo->enResOccupyType      = CNAS_HRM_RES_OCCUPY_TYPE_NON;
}


VOS_VOID CNAS_HRM_InitCtx(VOS_VOID)
{
    CNAS_HRM_CTX_STRU                  *pstCnasHrmCtx;

    pstCnasHrmCtx = CNAS_HRM_GetCtxAddr();

    pstCnasHrmCtx->ucIsNeedWaitResApplied = VOS_FALSE;
    pstCnasHrmCtx->enDsdsType             = CNAS_HRM_DSDS_TYPE_1;
    pstCnasHrmCtx->usCurOpId              = 0;

    CNAS_HRM_ClearAllDataReqBuff();
}


VOS_UINT16 CNAS_HRM_GetOpId(VOS_VOID)
{
    CNAS_HRM_GetCtxAddr()->usCurOpId++;

    CNAS_HRM_GetCtxAddr()->usCurOpId %= 255;

    if (0 == CNAS_HRM_GetCtxAddr()->usCurOpId)
    {
        CNAS_HRM_GetCtxAddr()->usCurOpId++;
    }

    return (CNAS_HRM_GetCtxAddr()->usCurOpId);
}


CNAS_HRM_DSDS_TYPE_ENUM_UINT8 CNAS_HRM_GetDsdsType(VOS_VOID)
{
    return (CNAS_HRM_GetCtxAddr()->enDsdsType);
}


VOS_UINT8 CNAS_HRM_IsNeedWaitResApplied(VOS_VOID)
{
    return (CNAS_HRM_GetCtxAddr()->ucIsNeedWaitResApplied);
}

#if defined(DMT)
VOS_VOID CNAS_HRM_SetDsdsType_DMT(
    CNAS_HRM_DSDS_TYPE_ENUM_UINT8       enDsdsType
)
{
    g_enDsdsType_Dmt = enDsdsType;
}

VOS_VOID CNAS_HRM_SetIsNeedWaitResApplied_DMT(
    VOS_UINT8                           ucIsNeedWaitResApplied
)
{
    g_ucIsNeedWaitResApplied_Dmt = ucIsNeedWaitResApplied;
}

CNAS_HRM_DSDS_TYPE_ENUM_UINT8 CNAS_HRM_GetDsdsType_DMT(VOS_VOID)
{
    return g_enDsdsType_Dmt;
}

VOS_UINT8 CNAS_HRM_GetIsNeedWaitResApplied_DMT(VOS_VOID)
{
    return g_ucIsNeedWaitResApplied_Dmt;
}


#endif


VOS_VOID CNAS_HRM_ReadNvHrpdRfAllocSwitchMask(VOS_VOID)
{
    /*
    en_NV_Item_HRPD_RfAllocSwitchMask
    bit 0: signaling alloc Rf
    bit 1: ppp in access auth alloc Rf
    其余bit 保留，都填0
    */

    NV_HRPD_RF_ALLOC_SWITCH_MASK_STRU                          stRfAllocSwitchMask = {0};

#if defined(DMT)
    CNAS_HRM_GetCtxAddr()->ucIsNeedWaitResApplied = CNAS_HRM_GetIsNeedWaitResApplied_DMT();

    return;
#endif

    if (NV_OK != NV_Read(en_NV_Item_HRPD_RfAllocSwitchMask,
                         &stRfAllocSwitchMask, sizeof(NV_HRPD_RF_ALLOC_SWITCH_MASK_STRU)))
    {
        CNAS_ERROR_LOG(UEPS_PID_HRM, "CNAS_HRM_ReadNvHrpdRfAllocSwitchMask:Read Nvim Failed");
        return;
    }

    /* 根据 bit 0 来判断是否需要申请资源 */
    if (VOS_TRUE == (stRfAllocSwitchMask.ulHrpdRfAllocSwitchMask & 0x00000001))
    {
        CNAS_HRM_GetCtxAddr()->ucIsNeedWaitResApplied = VOS_TRUE;
    }
}



VOS_VOID CNAS_HRM_ReadNvFemChanProfile(VOS_VOID)
{
    MODEM_ID_ENUM_UINT16                enCdmaModeModemId;
    VOS_UINT32                          ulProfileId;
    NV_MODEM_RF_SHARE_CFG_EX_STRU       stNvimRfShareCfgExInfo = {0};
    CNAS_HRM_CTX_STRU                  *pstCnasHrmCtx;

    pstCnasHrmCtx = CNAS_HRM_GetCtxAddr();

#if defined(DMT)
    pstCnasHrmCtx->enDsdsType = CNAS_HRM_GetDsdsType_DMT();

    return;

#endif

    /* 读NV项en_NV_Item_TRI_MODE_FEM_PROFILE_ID，失败，直接返回 */
    if (NV_OK != NV_Read(en_NV_Item_TRI_MODE_FEM_PROFILE_ID,
                      &ulProfileId, sizeof(VOS_UINT32)))
    {
        CNAS_ERROR_LOG(UEPS_PID_HRM, "CNAS_HRM_ReadNvFemChanProfile(): Read en_NV_Item_TRI_MODE_FEM_PROFILE_ID Fail.");
        return;
    }

    if(ulProfileId > 7)
    {
        CNAS_ERROR_LOG(UEPS_PID_HRM, "CNAS_HRM_ReadNvFemChanProfile(): ProfileId Error");
        return;
    }

    enCdmaModeModemId = CNAS_CCB_GetCdmaModeModemId();

    /* 读NV项en_NV_MODEM_RF_SHARE_EX_CFG，失败，直接返回 */
    if (NV_OK != NV_ReadEx(enCdmaModeModemId,
                           en_NV_MODEM_RF_SHARE_EX_CFG,
                           &stNvimRfShareCfgExInfo,
                           sizeof(NV_MODEM_RF_SHARE_CFG_EX_STRU)))
    {
        CNAS_ERROR_LOG(UEPS_PID_HRM, "CNAS_HRM_ReadNvFemChanProfile(): read en_NV_MODEM_RF_SHARE_EX_CFG Error");
        return;
    }

    /* SupportFlag 取值0, 1, 2 。0 代表不支持DSDS，1代表DSDS1.0, 2代表DSDS2.0。
    所以usSupportFlag=2时，刷新enDsdsType的值，usSupportFlag为其他值时，enDsdsType保持默认值*/
    if (CNAS_HRM_DSDS_TYPE_2 == stNvimRfShareCfgExInfo.astNvModemRfShareCfg[ulProfileId].usSupportFlag)
    {
        pstCnasHrmCtx->enDsdsType = CNAS_HRM_DSDS_TYPE_2;
    }

    return;
}


VOS_VOID CNAS_HRM_ReadNvimInfo(VOS_VOID)
{
    /* 读取是否申请资源的NV项 */
    CNAS_HRM_ReadNvHrpdRfAllocSwitchMask();

    /* 读取DSDS版本的NV项 */
    CNAS_HRM_ReadNvFemChanProfile();
}

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */




