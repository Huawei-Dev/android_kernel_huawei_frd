

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "RnicSndMsg.h"
#include "RnicLog.h"
#include "RnicCdsInterface.h"
#include "RnicDebug.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*lint -e960*/
#define    THIS_FILE_ID        PS_FILE_ID_RNIC_SND_MSG_C
/*lint +e960*/

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/

VOS_UINT32 RNIC_SendDialInfoMsg(
    RNIC_MSG_ID_ENUM_UINT32            enMsgId
)
{
    RNIC_NOTIFY_MSG_STRU               *pstDialInfo;
    RNIC_DIAL_MODE_STRU                *pstDialMode;


    /* 内存分配 */
    pstDialInfo = (RNIC_NOTIFY_MSG_STRU *)PS_ALLOC_MSG(ACPU_PID_RNIC,
                                                      sizeof(RNIC_NOTIFY_MSG_STRU) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == pstDialInfo)
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_SendDialInfoMsg: Malloc failed!");
        return VOS_ERR;
    }

    pstDialMode                         = RNIC_GetDialModeAddr();

    /* 填充消息 */
    pstDialInfo->ulSenderCpuId          = VOS_LOCAL_CPUID;
    pstDialInfo->ulSenderPid            = ACPU_PID_RNIC;
    pstDialInfo->ulReceiverCpuId        = VOS_LOCAL_CPUID;
    pstDialInfo->ulReceiverPid          = ACPU_PID_RNIC;
    pstDialInfo->enMsgId                = enMsgId;

    PS_MEM_CPY(&(pstDialInfo->stDialInfo), pstDialMode, sizeof(RNIC_DIAL_MODE_STRU));

    if (VOS_OK != PS_SEND_MSG(ACPU_PID_RNIC, pstDialInfo))
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_SendDialInfoMsg: Send msg failed!");
        return VOS_ERR;
    }

    return VOS_OK;

}

#if (FEATURE_ON == FEATURE_IMS)

VOS_UINT32 RNIC_SendCdsImsDataReq(
    struct sk_buff                     *pstSkb,
    RNIC_SPEC_CTX_STRU                 *pstNetCntxt
)
{
    RNIC_CDS_IMS_DATA_REQ_STRU         *pstSndMsg = VOS_NULL_PTR;

    /* 内存分配 */
    pstSndMsg = (RNIC_CDS_IMS_DATA_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ACPU_PID_RNIC,
                                    sizeof(RNIC_CDS_IMS_DATA_REQ_STRU) - 4 + pstSkb->len);

    if (VOS_NULL_PTR == pstSndMsg)
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_SendCdsImsDataReq: Malloc failed!");
        RNIC_DEV_ERR_PRINTK("RNIC_SendCdsImsDataReq: Malloc failed!");
        return VOS_ERR;
    }

    /* 填充消息头 */
    pstSndMsg->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid              = ACPU_PID_RNIC;
    pstSndMsg->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid            = UEPS_PID_CDS;
    pstSndMsg->ulMsgId                  = ID_RNIC_CDS_IMS_DATA_REQ;

    /* 填充消息 */
    pstSndMsg->usModemId                = pstNetCntxt->enModemId;
    pstSndMsg->usDataLen                = (VOS_UINT16)pstSkb->len;

    PS_MEM_CPY(pstSndMsg->aucData, pstSkb->data, pstSkb->len);

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(ACPU_PID_RNIC, pstSndMsg))
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_SendCdsImsDataReq: Send msg failed!");
        RNIC_DEV_ERR_PRINTK("RNIC_SendCdsImsDataReq: Send msg failed!");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_VOID RNIC_TrigImsDataProcEvent(RNIC_RMNET_ID_ENUM_UINT8 enRmNetId)
{
    RNIC_IMS_DATA_PROC_IND_STRU         *pstSndMsg = VOS_NULL_PTR;

    /* 内存分配 */
    pstSndMsg = (RNIC_IMS_DATA_PROC_IND_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ACPU_PID_RNIC,
                                    sizeof(RNIC_IMS_DATA_PROC_IND_STRU));

    if (VOS_NULL_PTR == pstSndMsg)
    {
        RNIC_DEV_ERR_PRINTK("RNIC_TrigImsDataProcEvent: Malloc failed!");
        return;
    }

    /* 填充消息头 */
    pstSndMsg->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid              = ACPU_PID_RNIC;
    pstSndMsg->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid            = ACPU_PID_RNIC;
    pstSndMsg->enMsgId                  = ID_RNIC_IMS_DATA_PROC_IND;
    pstSndMsg->enRmNetId                = enRmNetId;

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(ACPU_PID_RNIC, pstSndMsg))
    {
        RNIC_DEV_ERR_PRINTK("RNIC_TrigImsDataProcEvent: Send msg failed!");
    }

    return;
}
#endif


VOS_UINT32 RNIC_SndRnicRmnetConfigReq(
    RNIC_RMNET_CONFIG_STRU             *pstConfigInfo
)
{
    RNIC_RMNET_CONFIG_REQ_STRU         *pstSndMsg = VOS_NULL_PTR;

    /* 内存分配 */
    pstSndMsg = (RNIC_RMNET_CONFIG_REQ_STRU *)PS_ALLOC_MSG(ACPU_PID_RNIC,
                        sizeof(RNIC_RMNET_CONFIG_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstSndMsg)
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_SndRnicRmnetConfigMsg: Malloc failed!");
        return VOS_ERR;
    }

    /* 填充消息头 */
    pstSndMsg->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid              = ACPU_PID_RNIC;
    pstSndMsg->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid            = ACPU_PID_RNIC;
    pstSndMsg->enMsgId                  = ID_RNIC_RMNET_CONFIG_REQ;

    /* 填充消息 */
    pstSndMsg->enModemType              = pstConfigInfo->enModemType;
    pstSndMsg->enRmnetStatus            = pstConfigInfo->enRmnetStatus;
    pstSndMsg->enIpType                 = pstConfigInfo->enIpType;
    pstSndMsg->ucPdnId                  = pstConfigInfo->ucPdnId;
    pstSndMsg->ucRabId                  = pstConfigInfo->ucRabId;
    pstSndMsg->ucRmNetId                = pstConfigInfo->ucRmNetId;
    pstSndMsg->usModemId                = pstConfigInfo->usModemId;

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(ACPU_PID_RNIC, pstSndMsg))
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_SndRnicRmnetConfigMsg: Send msg failed!");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_VOID RNIC_MNTN_SndRmnetConfigInfoMsg(
    RNIC_RMNET_CONFIG_STRU             *pstConfigInfo
)
{
    RNIC_RMNET_CONFIG_REQ_STRU          *pstSndMsg = VOS_NULL_PTR;

    /* 内存分配 */
    pstSndMsg = (RNIC_RMNET_CONFIG_REQ_STRU *)PS_ALLOC_MSG(ACPU_PID_RNIC,
                        sizeof(RNIC_RMNET_CONFIG_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstSndMsg)
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_SndRnicRmnetConfigMsg: Malloc failed!");
        return;
    }

    /* 填充消息 */
    pstSndMsg->enModemType              = pstConfigInfo->enModemType;
    pstSndMsg->enRmnetStatus            = pstConfigInfo->enRmnetStatus;
    pstSndMsg->enIpType                 = pstConfigInfo->enIpType;
    pstSndMsg->ucPdnId                  = pstConfigInfo->ucPdnId;
    pstSndMsg->ucRabId                  = pstConfigInfo->ucRabId;
    pstSndMsg->ucRmNetId                = pstConfigInfo->ucRmNetId;
    pstSndMsg->usModemId                = pstConfigInfo->usModemId;

    pstSndMsg->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstSndMsg->ulSenderPid              = ACPU_PID_RNIC;
    pstSndMsg->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstSndMsg->ulReceiverPid            = ACPU_PID_RNIC;
    pstSndMsg->enMsgId                  = ID_RNIC_MNTN_RMNET_CONFIG_INFO;

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(ACPU_PID_RNIC, pstSndMsg))
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_SndRnicRmnetConfigMsg: Send msg failed!");
        return;
    }

    return;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif