

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "CnasHrmSndTtf.h"
#include "CnasHrmCtx.h"
#include "CnasHrmCommFunc.h"
#include "Nas_Mem.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#define THIS_FILE_ID                    PS_FILE_ID_CNAS_HRM_SND_TTF_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/
/*lint -save -e958*/


VOS_VOID CNAS_HRM_SndSnpDataReq(
    VOS_UINT16                                              usOpId,
    VOS_UINT32                                              ulMsgLen,
    CNAS_HRM_DATA_REQ_COMM_CONTENT_STRU                    *pstDataReqContent
)
{
    CNAS_CTTF_HRPD_SNP_DATA_REQ_STRU   *pstSnpDataReq = VOS_NULL_PTR;
    VOS_UINT16                          usMsgDataByteLen;

    usMsgDataByteLen = CNAS_HRM_ConvertBitLenToByteLen(pstDataReqContent->stMsgData.usMsgBitLen);

    pstSnpDataReq = (CNAS_CTTF_HRPD_SNP_DATA_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(UEPS_PID_HRM, ulMsgLen + VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstSnpDataReq)
    {
        return;
    }

    NAS_MEM_SET_S((VOS_UINT8 *)pstSnpDataReq + VOS_MSG_HEAD_LENGTH,
                  ulMsgLen,
                  0x00,
                  ulMsgLen);

    pstSnpDataReq->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstSnpDataReq->ulSenderPid              = UEPS_PID_HRM;
    pstSnpDataReq->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstSnpDataReq->ulReceiverPid            = CTTF_PID_HRPD_SIG;
    pstSnpDataReq->ulLength                 = ulMsgLen;
    pstSnpDataReq->enMsgId                  = ID_CNAS_CTTF_HRPD_SNP_DATA_REQ;
    pstSnpDataReq->usOpId                   = usOpId;
    pstSnpDataReq->enProtocolType           = pstDataReqContent->enProtocolType;
    pstSnpDataReq->enInstanceType           = pstDataReqContent->enInstanceType;
    pstSnpDataReq->enAddressType            = pstDataReqContent->enAddressType;
    pstSnpDataReq->enChannelType            = pstDataReqContent->enChannelType;
    pstSnpDataReq->enDeliveryType           = pstDataReqContent->enDeliveryType;
    pstSnpDataReq->ucPriority               = pstDataReqContent->ucPriority;
    pstSnpDataReq->enNeedRslt               = pstDataReqContent->enNeedRslt;
    pstSnpDataReq->stMsgData.usMsgBitLen    = pstDataReqContent->stMsgData.usMsgBitLen;

    NAS_MEM_CPY_S(pstSnpDataReq->stMsgData.aucMsgData, usMsgDataByteLen, pstDataReqContent->stMsgData.aucMsgData, usMsgDataByteLen);

    CNAS_HRM_LogOmAirMsgUp(pstSnpDataReq);

    (VOS_VOID)PS_SEND_MSG(UEPS_PID_HRM, pstSnpDataReq);

    return;
}


VOS_VOID CNAS_HRM_SndSnpDataCancelReq(
    VOS_UINT32                                ulSigClientOpId
)
{
    NAS_CTTF_HRPD_SNP_DATA_CANCEL_REQ_STRU   *pstSnpCancelDataReq = VOS_NULL_PTR;


    pstSnpCancelDataReq = (NAS_CTTF_HRPD_SNP_DATA_CANCEL_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(UEPS_PID_HRM,
                                                                sizeof(NAS_CTTF_HRPD_SNP_DATA_CANCEL_REQ_STRU));

    if (VOS_NULL_PTR == pstSnpCancelDataReq)
    {
        return;
    }

    NAS_MEM_SET_S((VOS_UINT8 *)pstSnpCancelDataReq + VOS_MSG_HEAD_LENGTH,
                  sizeof(NAS_CTTF_HRPD_SNP_DATA_CANCEL_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                  0x00,
                  sizeof(NAS_CTTF_HRPD_SNP_DATA_CANCEL_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstSnpCancelDataReq->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstSnpCancelDataReq->ulSenderPid              = UEPS_PID_HRM;
    pstSnpCancelDataReq->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstSnpCancelDataReq->ulReceiverPid            = CTTF_PID_HRPD_SIG;
    pstSnpCancelDataReq->ulLength                 = sizeof(NAS_CTTF_HRPD_SNP_DATA_CANCEL_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstSnpCancelDataReq->enMsgId                  = ID_NAS_CTTF_HRPD_SNP_DATA_CANCEL_REQ;
    pstSnpCancelDataReq->usOpId                   = 0x00;
    pstSnpCancelDataReq->ulSigClientPid           = UEPS_PID_HRM;
    pstSnpCancelDataReq->ulSigClientOpId          = ulSigClientOpId;

    (VOS_VOID)PS_SEND_MSG(UEPS_PID_HRM, pstSnpCancelDataReq);

    return;
}

/*lint -restore*/

#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


