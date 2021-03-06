

/*****************************************************************************
  1 头文件包含
*****************************************************************************/

#include "msp_errno.h"
#include <dms.h>
#include "dms_core.h"
#include "vos.h"


#ifdef __cplusplus
    #if __cplusplus
    extern "C" {
    #endif
#endif


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/

/*lint -e767 -e960*/
#define THIS_FILE_ID                    PS_FILE_ID_DMS_NDIS_C
/*lint +e767 +e960*/


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

VOS_UINT32              g_ulNdisCfgFlag     = 0xffffffffU;
USBNdisStusChgFunc      g_atConnectBrk      = NULL;
USBNdisAtRecvFunc       g_atCmdRcv          = NULL;


/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_UINT32 DMS_NcmOpen(VOS_VOID)
{
    VOS_UINT32                          ulRet = ERR_MSP_SUCCESS;
    UDI_HANDLE                          lHandle = UDI_INVALID_HANDLE;
    DMS_PHY_BEAR_PROPERTY_STRU         *pstPhyBearProp =NULL;
    UDI_OPEN_PARAM_S                    stParam ;

    pstPhyBearProp = DMS_GetPhyBearProperty(DMS_PHY_BEAR_USB_NCM);

    if (UDI_INVALID_HANDLE != pstPhyBearProp->lPortHandle)
    {
        return ERR_MSP_SUCCESS;
    }

    stParam.devid = UDI_NCM_CTRL_ID;

    lHandle = mdrv_udi_open(&stParam);
    if (UDI_INVALID_HANDLE == lHandle)
    {
        ulRet =  ERR_MSP_FAILURE;
    }
    else
    {
        pstPhyBearProp->lPortHandle = lHandle;

        if (VOS_OK != mdrv_udi_ioctl(lHandle , NCM_IOCTL_REG_NDIS_RESP_STATUS_FUNC, DMS_NcmWrtCB))
        {
            DMS_LOG_WARNING("DMS_NcmOpen: NCM_IOCTL_REG_NDIS_RESP_STATUS_FUNC fail.\n");
        }
    }

    /* 配置 NDIS CTRL 通道 */
    DMS_NcmProcCbReg(g_atConnectBrk,(USB_NAS_AT_CMD_RECV)g_atCmdRcv);

    return ulRet;
}


VOS_UINT32 DMS_NcmClose(VOS_VOID)
{
    VOS_INT32                           lRet = ERR_MSP_SUCCESS;
    DMS_PHY_BEAR_PROPERTY_STRU         *pstPhyBearProp = NULL;

    pstPhyBearProp = DMS_GetPhyBearProperty(DMS_PHY_BEAR_USB_NCM);

    if (UDI_INVALID_HANDLE == pstPhyBearProp->lPortHandle)
    {
        DMS_LOG_WARNING("DMS_UsbPortClose[%d]: Already close.\n", DMS_PHY_BEAR_USB_NCM);
        return ERR_MSP_SUCCESS;
    }

    lRet = mdrv_udi_close(pstPhyBearProp->lPortHandle);

    pstPhyBearProp->lPortHandle = UDI_INVALID_HANDLE;

    return (VOS_UINT32)lRet;
}


VOS_UINT32 DMS_NcmSendData(VOS_UINT8 *pData, VOS_UINT32 ulLen)
{
    VOS_INT32                           lRet = ERR_MSP_SUCCESS;
    NCM_AT_RSP_S                        stATResponse = {0};
    DMS_PHY_BEAR_PROPERTY_STRU         *pstPhyBearProp = DMS_GetPhyBearProperty(DMS_PHY_BEAR_USB_NCM);
    VOS_UINT_PTR                        ptrAddr;

    ptrAddr = (VOS_UINT_PTR)pData;

    /*检查NDIS通道状态*/
    if (ACM_EVT_DEV_SUSPEND == pstPhyBearProp->ucChanStat)
    {
        return ERR_MSP_FAILURE;
    }

    if ((pData == NULL) || (ulLen == 0) || (ulLen > 2048))
    {
        return ERR_MSP_INVALID_PARAMETER;
    }

    stATResponse.pu8AtAnswer = pData;
    stATResponse.u32Length = ulLen;

    if (UDI_INVALID_HANDLE ==pstPhyBearProp->lPortHandle)
    {
        return ERR_MSP_FAILURE;
    }

    DMS_DBG_SDM_FUN(DMS_SDM_VCOM_WRT_NCM, (VOS_UINT32)(ptrAddr & (~0U)), ulLen, 0);

    lRet = mdrv_udi_ioctl(pstPhyBearProp->lPortHandle, NCM_IOCTL_AT_RESPONSE, &stATResponse);
    if (ERR_MSP_SUCCESS != lRet)
    {
        return ERR_MSP_FAILURE;
    }

    DMS_DBG_SDM_FUN(DMS_SDM_VCOM_WRT_SUSS_NCM, 0, 0, 0);

    return ERR_MSP_SUCCESS;
}


VOS_INT32 DMS_NcmStatusChangeReg(NAS_PRO_STRU * pPdpStru)
{
    DMS_PHY_BEAR_PROPERTY_STRU         *pstPhyBearProp = NULL;
    VOS_INT32                           lRet = -1;

    pstPhyBearProp = DMS_GetPhyBearProperty(DMS_PHY_BEAR_USB_NCM);

    if (NULL == pPdpStru)
    {
        return ERR_MSP_INVALID_PARAMETER;
    }

    if (UDI_INVALID_HANDLE == pstPhyBearProp->lPortHandle)
    {
        return lRet;
    }

    lRet = mdrv_udi_ioctl(pstPhyBearProp->lPortHandle, NCM_IOCTL_NETWORK_CONNECTION_NOTIF, (VOS_VOID*)(&pPdpStru->enActiveSatus));
    if (ERR_MSP_SUCCESS != lRet)
    {
        return lRet;
    }

    return lRet;
}


int DMS_NcmExtFuncReg(USBNdisStusChgFunc connectBrk,USB_NAS_AT_CMD_RECV atCmdRcv)
{

    g_atConnectBrk = (USBNdisStusChgFunc )connectBrk;
    g_atCmdRcv     = (USBNdisAtRecvFunc )atCmdRcv;

    DMS_NcmProcCbReg((USBNdisStusChgFunc )connectBrk,(USB_NAS_AT_CMD_RECV )atCmdRcv);

    return ERR_MSP_SUCCESS;
}



VOS_VOID DMS_NcmProcCbReg(USBNdisStusChgFunc connectBrk,USB_NAS_AT_CMD_RECV atCmdRcv)
{
    DMS_PHY_BEAR_PROPERTY_STRU         *pstPhyBearProp = NULL;
    VOS_INT32                           lRet = -1;

    if ((connectBrk == NULL) || (atCmdRcv == NULL))
    {
        return ;
    }

    /*NDIS 通道没有打开或者 通道已经被配置，则直接返回*/
    if ((g_ulNdisCfgFlag == 0xffffffffU)||(g_ulNdisCfgFlag == 0))
    {
        return ;
    }

    pstPhyBearProp = DMS_GetPhyBearProperty(DMS_PHY_BEAR_USB_NCM);

    if (UDI_INVALID_HANDLE == pstPhyBearProp->lPortHandle)
    {
        return ;
    }

    lRet = mdrv_udi_ioctl(pstPhyBearProp->lPortHandle, NCM_IOCTL_REG_CONNECT_STUS_CHG_FUNC, (USBNdisStusChgFunc)(connectBrk));
    if (ERR_MSP_SUCCESS != lRet)
    {
        return ;
    }

    lRet = mdrv_udi_ioctl (pstPhyBearProp->lPortHandle , NCM_IOCTL_REG_AT_PROCESS_FUNC, (USBNdisAtRecvFunc)(atCmdRcv));
    if (ERR_MSP_SUCCESS != lRet)
    {
        return ;
    }

    g_ulNdisCfgFlag = 0;
    return ;
}


VOS_VOID DMS_SetNdisChanStatus(ACM_EVT_E enStatus)
{
    DMS_PHY_BEAR_PROPERTY_STRU         *pstPhyBearProp = DMS_GetPhyBearProperty(DMS_PHY_BEAR_USB_NCM);

    pstPhyBearProp->ucChanStat = (VOS_UINT8)enStatus;
    return;
}

VOS_VOID DMS_NcmWrtCB (char* pDoneBuff, int status)
{
    if (Dms_IsStaticBuf ((VOS_UINT8*)pDoneBuff))
    {
        Dms_FreeStaticBuf((VOS_UINT8*)pDoneBuff);
    }
    else
    {
        if(pDoneBuff != NULL)
        {
#if (VOS_LINUX== VOS_OS_VER)
            kfree(pDoneBuff);
#endif
        }
    }

    return;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

