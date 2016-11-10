

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "CnasHrmSndHsm.h"
#include "CnasMain.h"
#include "CnasHrmCommFunc.h"
#include "Nas_Mem.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#define THIS_FILE_ID                    PS_FILE_ID_CNAS_HRM_SND_HSM_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/
/*lint -save -e958*/


VOS_VOID CNAS_HRM_SndHsmDataCnf(
    VOS_UINT16                                              usOpId,
    HRM_HSM_DATA_CNF_RSLT_ENUM_UINT32                       enRslt,
    CTTF_HRPD_SNP_DATA_FAIL_REASON_ENUM_UINT32              enFailReason
)
{
    HRM_HSM_DATA_CNF_STRU              *pstDataCnf = VOS_NULL_PTR;

    pstDataCnf = (HRM_HSM_DATA_CNF_STRU *)CNAS_GetIntMsgSendBuf(UEPS_PID_HRM, sizeof(HRM_HSM_DATA_CNF_STRU));

    if (VOS_NULL_PTR == pstDataCnf)
    {
        return;
    }

    NAS_MEM_SET_S((VOS_UINT8 *)pstDataCnf + VOS_MSG_HEAD_LENGTH,
                  sizeof(HRM_HSM_DATA_CNF_STRU) - VOS_MSG_HEAD_LENGTH,
                  0x00,
                  sizeof(HRM_HSM_DATA_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    pstDataCnf->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstDataCnf->ulSenderPid              = UEPS_PID_HRM;
    pstDataCnf->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstDataCnf->ulReceiverPid            = UEPS_PID_HSM;
    pstDataCnf->ulLength                 = sizeof(HRM_HSM_DATA_CNF_STRU)- VOS_MSG_HEAD_LENGTH;
    pstDataCnf->enMsgId                  = ID_HRM_HSM_DATA_CNF;
    pstDataCnf->usOpId                   = usOpId;
    pstDataCnf->enRslt                   = enRslt;
    pstDataCnf->enFailReason             = enFailReason;

    (VOS_VOID)CNAS_SndInternalMsg(pstDataCnf);

    return;

}


VOS_VOID CNAS_HRM_SndHsmDataInd(
    CTTF_HRPD_PROTOCOL_TYPE_ENUM_UINT16                     enProtocolType,
    CTTF_HRPD_INSTANCE_TYPE_ENUM_UINT8                      enInstanceType,
    VOS_UINT16                                              usOpId,
    SNPITF_MSG_DATA_STRU                                   *pstMsgData
)
{
    HRM_HSM_DATA_IND_STRU              *pstDataInd = VOS_NULL_PTR;
    VOS_UINT16                          usMsgByteLen;
    VOS_UINT16                          usDataIndMsgLen;

    usMsgByteLen = CNAS_HRM_ConvertBitLenToByteLen(pstMsgData->usMsgBitLen);

    usDataIndMsgLen = (VOS_UINT16)CNAS_MAX((sizeof(HRM_HSM_DATA_IND_STRU) + usMsgByteLen - 4), (sizeof(HRM_HSM_DATA_IND_STRU)));

    pstDataInd = (HRM_HSM_DATA_IND_STRU *)CNAS_GetIntMsgSendBuf(UEPS_PID_HRM, usDataIndMsgLen);

    if (VOS_NULL_PTR == pstDataInd)
    {
        return;
    }

    NAS_MEM_SET_S((VOS_UINT8 *)pstDataInd + VOS_MSG_HEAD_LENGTH,
                  usDataIndMsgLen - VOS_MSG_HEAD_LENGTH,
                  0x00,
                  usDataIndMsgLen - VOS_MSG_HEAD_LENGTH);

    pstDataInd->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstDataInd->ulSenderPid              = UEPS_PID_HRM;
    pstDataInd->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstDataInd->ulReceiverPid            = UEPS_PID_HSM;
    pstDataInd->ulLength                 = usDataIndMsgLen - VOS_MSG_HEAD_LENGTH;
    pstDataInd->enMsgId                  = ID_HRM_HSM_DATA_IND;
    pstDataInd->usOpId                   = usOpId;
    pstDataInd->enProtocolType           = enProtocolType;
    pstDataInd->enInstanceType           = enInstanceType;
    pstDataInd->stMsgData.usMsgBitLen    = pstMsgData->usMsgBitLen;

    NAS_MEM_CPY_S(pstDataInd->stMsgData.aucMsgData, usMsgByteLen, pstMsgData->aucMsgData, usMsgByteLen);

    (VOS_VOID)CNAS_SndInternalMsg(pstDataInd);

    return;
}


VOS_VOID CNAS_HRM_SndHsmUatiCmplSndInd(
    VOS_UINT16                          usOpId
)
{
    HRM_HSM_UATI_CMPL_SND_IND_STRU     *pstUatiCmplSndInd = VOS_NULL_PTR;

    pstUatiCmplSndInd = (HRM_HSM_UATI_CMPL_SND_IND_STRU *)CNAS_GetIntMsgSendBuf(UEPS_PID_HRM, sizeof(HRM_HSM_UATI_CMPL_SND_IND_STRU));

    if (VOS_NULL_PTR == pstUatiCmplSndInd)
    {
        return;
    }

    NAS_MEM_SET_S((VOS_UINT8 *)pstUatiCmplSndInd + VOS_MSG_HEAD_LENGTH,
                  sizeof(HRM_HSM_UATI_CMPL_SND_IND_STRU) - VOS_MSG_HEAD_LENGTH,
                  0x00,
                  sizeof(HRM_HSM_UATI_CMPL_SND_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    pstUatiCmplSndInd->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstUatiCmplSndInd->ulSenderPid              = UEPS_PID_HRM;
    pstUatiCmplSndInd->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstUatiCmplSndInd->ulReceiverPid            = UEPS_PID_HSM;
    pstUatiCmplSndInd->ulLength                 = sizeof(HRM_HSM_UATI_CMPL_SND_IND_STRU)- VOS_MSG_HEAD_LENGTH;
    pstUatiCmplSndInd->enMsgId                  = ID_HRM_HSM_UATI_CMPL_SND_IND;
    pstUatiCmplSndInd->usOpId                   = usOpId;

    (VOS_VOID)CNAS_SndInternalMsg(pstUatiCmplSndInd);

    return;

}

/*lint -restore*/

#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif



