

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "CnasHluSndHrm.h"
#include "CnasHluMain.h"
#include "PsCommonDef.h"

#include "Nas_Mem.h"
#include "CnasMain.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_CNAS_HLU_SND_HRM_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/
/*lint -save -e958*/


VOS_VOID CNAS_HLU_SendLocNotification(
    CNAS_HLU_MSG_TX_OPTION_INFO        *pstMsgTxInfo,
    VOS_BOOL                            bIsHandOff,
    CNAS_HLU_LOC_VALUE_STRU            *pstHandOffLocVal,
    CNAS_HRM_DATA_REQ_TYPE_ENUM_UINT32  enDataReqType
)
{
    HLU_HRM_DATA_REQ_STRU              *pstDataReq;
    VOS_VOID                           *pvDataTmp;
    VOS_UINT32                          ulSize;

    if (CNAS_HLU_IsCurrLocTypeValid() || (VOS_TRUE == bIsHandOff))
    {
        ulSize    = sizeof(HLU_HRM_DATA_REQ_STRU) + 4;
        pvDataTmp = (VOS_VOID *)CNAS_GetIntMsgSendBuf(UEPS_PID_HLU, ulSize);

        if (CNAS_HLU_IS_NULL_PTR(pvDataTmp))
        {
            CNAS_ERROR_LOG(UEPS_PID_HLU, "CNAS_HLU_SendLocNotification:"
                                         "memory allocation failure.");
            return;
        }

        NAS_MEM_SET_S(pvDataTmp, ulSize, 0, ulSize);

        pstDataReq = (HLU_HRM_DATA_REQ_STRU *)pvDataTmp;

        /* 1. Fill usMsgBitLen = [MessageID_bitLen + locationType_bitLen
                                 + locationLength_bitLen + locationValue_bitLen]
           2. Fill LocationNotification : MessageID = 0x04, locationType and
              locationLength.
           3. Fill the current stored LocationValue in netwotk BYTE order.
        */
        pstDataReq->stDataReqCommContent.stMsgData.usMsgBitLen   = CNAS_HLU_LOC_NOTIF_MSG_BIT_LEN;
        pstDataReq->stDataReqCommContent.stMsgData.aucMsgData[0] = CNAS_HLU_HRPD_LOC_NOTIFICATION;
        pstDataReq->stDataReqCommContent.stMsgData.aucMsgData[1] = CNAS_HLU_CDMA2000_HRPD_LOC_TYPE;
        pstDataReq->stDataReqCommContent.stMsgData.aucMsgData[2] = CNAS_HLU_CDMA2000_HRPD_LOC_LEN;

        if (VOS_TRUE == bIsHandOff)
        {
            CNAS_HLU_SetHandOffOpId();
            pstDataReq->usOpId = CNAS_HLU_GetHandOffOpId();
        }

        CNAS_HLU_GetCurrLocValHToN(&(pstDataReq->stDataReqCommContent.stMsgData.aucMsgData[3]),
                                    bIsHandOff,
                                    pstHandOffLocVal);

    }
    else
    {
        /* It means current stored value on AT is not valid. */
        /* Do not include LocationLength and LocationVAlue in message to be sent. */
        ulSize    = sizeof(HLU_HRM_DATA_REQ_STRU);
        pvDataTmp = (VOS_VOID *)CNAS_GetIntMsgSendBuf(UEPS_PID_HLU, ulSize);
        if (VOS_NULL_PTR == pvDataTmp)
        {
            CNAS_ERROR_LOG(UEPS_PID_HLU, "CNAS_HLU_SendLocNotification:" \
                                         "memory allocation failure.");
            return;
        }

        NAS_MEM_SET_S(pvDataTmp, ulSize, 0, ulSize);

        pstDataReq = (HLU_HRM_DATA_REQ_STRU *)pvDataTmp;

        pstDataReq->stDataReqCommContent.stMsgData.usMsgBitLen   = CNAS_HLU_NO_LOC_NOTIF_MSG_BIT_LEN;
        pstDataReq->stDataReqCommContent.stMsgData.aucMsgData[0] = CNAS_HLU_HRPD_LOC_NOTIFICATION;
        pstDataReq->stDataReqCommContent.stMsgData.aucMsgData[1] = CNAS_HLU_CDMA2000_HRPD_LOC_TYPE_INV;

    }

    pstDataReq->ulLength = ulSize - VOS_MSG_HEAD_LENGTH;

    CNAS_HLU_FILL_MSG_HDR(pstDataReq, UEPS_PID_HLU, UEPS_PID_HRM);

    CNAS_HLU_FILL_HRM_REQ_MSG(pstDataReq, pstMsgTxInfo, CNAS_HLU_GetProtocolType(), enDataReqType);

    CNAS_MNTN_LogMsgInfo((MSG_HEADER_STRU*)pstDataReq);

    /* Send message to HRM. */
    (VOS_VOID)CNAS_SndInternalMsg(pstDataReq);

    return;
}


VOS_VOID CNAS_HLU_SendLocComplete(
    CNAS_HLU_MSG_TX_OPTION_INFO        *pstMsgTxInfo
)
{
    HLU_HRM_DATA_REQ_STRU              *pstDataReq;

    pstDataReq = (HLU_HRM_DATA_REQ_STRU *)CNAS_GetIntMsgSendBuf(UEPS_PID_HLU,
                                          sizeof(HLU_HRM_DATA_REQ_STRU));
    if (VOS_NULL_PTR == pstDataReq)
    {
        return;
    }

    NAS_MEM_SET_S(pstDataReq, sizeof(HLU_HRM_DATA_REQ_STRU), 0, sizeof(HLU_HRM_DATA_REQ_STRU));

    /* MsgLen in bits. */
    pstDataReq->stDataReqCommContent.stMsgData.usMsgBitLen   = CNAS_HLU_LOC_COMP_MSG_BIT_LEN;

    pstDataReq->stDataReqCommContent.stMsgData.aucMsgData[0] = CNAS_HLU_HRPD_LOC_COMPLETE;
    pstDataReq->stDataReqCommContent.stMsgData.aucMsgData[1] = CNAS_HLU_GetLocMsgTransId();

    pstDataReq->ulLength = sizeof(HLU_HRM_DATA_REQ_STRU) - VOS_MSG_HEAD_LENGTH;

    CNAS_HLU_FILL_MSG_HDR(pstDataReq, UEPS_PID_HLU, UEPS_PID_HRM);

    CNAS_HLU_FILL_HRM_REQ_MSG(pstDataReq, pstMsgTxInfo, CNAS_HLU_GetProtocolType(), CNAS_HRM_DATA_REQ_LOC_CMPL);

    CNAS_MNTN_LogMsgInfo((MSG_HEADER_STRU*)pstDataReq);

    /* Send message to HRM. */
    (VOS_VOID)CNAS_SndInternalMsg(pstDataReq);

    return;
}


VOS_VOID CNAS_HLU_SendStrgBlobNotification(
    CNAS_HLU_MSG_TX_OPTION_INFO        *pstMsgTxInfo
)
{
    HLU_HRM_DATA_REQ_STRU              *pstDataReq;
    VOS_UINT32                          ulSize;
    VOS_UINT8                           ucBLOBLen;

    if (CNAS_HLU_IsCurrBlobTypeValid())
    {
        ucBLOBLen     = CNAS_HLU_GetCurrBlobLen();

        ulSize        = sizeof(HLU_HRM_DATA_REQ_STRU) + ucBLOBLen;
        pstDataReq    = (HLU_HRM_DATA_REQ_STRU *)CNAS_GetIntMsgSendBuf(UEPS_PID_HLU, ulSize);
        if (VOS_NULL_PTR == pstDataReq)
        {
            return;
        }

        NAS_MEM_SET_S(pstDataReq, ulSize, 0, ulSize);

        /* 1. Fill usMsgBitLen
           2. Fill MessageID.
           3. Fill StorageBLOB in network BYTE order
        */
        pstDataReq->stDataReqCommContent.stMsgData.usMsgBitLen   = CNAS_HLU_BLOB_NOTIF_MSG_BIT_LEN(ucBLOBLen);
        pstDataReq->stDataReqCommContent.stMsgData.aucMsgData[0] = CNAS_HLU_HRPD_BLOB_NOTIFICATION;

        CNAS_HLU_GetCurrStrgBlobHToN(&(pstDataReq->stDataReqCommContent.stMsgData.aucMsgData[1]));

    }
    else
    {
        /* It means current stored value of StorageBlob stored on AT is not
           valid, Do not include aucStorageBLOB in message to be sent.
        */
        ulSize     = sizeof(HLU_HRM_DATA_REQ_STRU);
        pstDataReq = (HLU_HRM_DATA_REQ_STRU *)CNAS_GetIntMsgSendBuf(UEPS_PID_HLU, ulSize);

        if (VOS_NULL_PTR == pstDataReq)
        {
            return;
        }

        NAS_MEM_SET_S(pstDataReq, ulSize, 0, ulSize);

        /* 1. Get the stored value of SID, NID.
           2. Fill usMsgBitLen = [MessageID_bitLen + StorageBLOBType_bitLen
                             + StorageBLOBLength_bitLen + StorageBLOBValue_bitLen]
           3. Fill MessageID, StorageBLOBType and StorageBLOBLength.
        */
        pstDataReq->stDataReqCommContent.stMsgData.usMsgBitLen   = CNAS_HLU_NO_BLOB_NOTIF_MSG_BIT_LEN;
        pstDataReq->stDataReqCommContent.stMsgData.aucMsgData[0] = CNAS_HLU_HRPD_BLOB_NOTIFICATION; /* MessageID */
        pstDataReq->stDataReqCommContent.stMsgData.aucMsgData[1] = CNAS_HLU_BYTE_NULL; /* StorageBLOBType BYTE1 */
        pstDataReq->stDataReqCommContent.stMsgData.aucMsgData[2] = CNAS_HLU_BYTE_NULL; /* StorageBLOBType BYTE2 */
        pstDataReq->stDataReqCommContent.stMsgData.aucMsgData[3] = CNAS_HLU_BYTE_NULL; /* StorageBLOBLength */

    }

    pstDataReq->ulLength             = ulSize - VOS_MSG_HEAD_LENGTH;

    CNAS_HLU_FILL_MSG_HDR(pstDataReq, UEPS_PID_HLU, UEPS_PID_HRM);

    CNAS_HLU_FILL_HRM_REQ_MSG(pstDataReq, pstMsgTxInfo, CNAS_HLU_GetProtocolType(), CNAS_HRM_DATA_REQ_BLOB_NTF);

    CNAS_MNTN_LogMsgInfo((MSG_HEADER_STRU*)pstDataReq);

    /* Send message to HRM. */
    (VOS_VOID)CNAS_SndInternalMsg(pstDataReq);

    return;

}


VOS_VOID CNAS_HLU_SendStrgBlobComplete(
    CNAS_HLU_MSG_TX_OPTION_INFO        *pstMsgTxInfo
)
{
    HLU_HRM_DATA_REQ_STRU              *pstDataReq;

    pstDataReq = (HLU_HRM_DATA_REQ_STRU *)
                 CNAS_GetIntMsgSendBuf(UEPS_PID_HLU, sizeof(HLU_HRM_DATA_REQ_STRU));

    if (VOS_NULL_PTR == pstDataReq)
    {
        return;
    }

    NAS_MEM_SET_S(pstDataReq, sizeof(HLU_HRM_DATA_REQ_STRU), 0, sizeof(HLU_HRM_DATA_REQ_STRU));

    /* 1. Fill MsgLen in bits, MsgId, TransactionId. */
    pstDataReq->stDataReqCommContent.stMsgData.usMsgBitLen   = CNAS_HLU_BLOB_COMP_MSG_BIT_LEN;

    pstDataReq->stDataReqCommContent.stMsgData.aucMsgData[0] = CNAS_HLU_HRPD_BLOB_COMPLETE;
    pstDataReq->stDataReqCommContent.stMsgData.aucMsgData[1] = CNAS_HLU_GetBLobTransId();

    pstDataReq->ulLength = sizeof(HLU_HRM_DATA_REQ_STRU) - VOS_MSG_HEAD_LENGTH;

    CNAS_HLU_FILL_MSG_HDR(pstDataReq, UEPS_PID_HLU, UEPS_PID_HRM);

    CNAS_HLU_FILL_HRM_REQ_MSG(pstDataReq, pstMsgTxInfo, CNAS_HLU_GetProtocolType(), CNAS_HRM_DATA_REQ_BLOB_CMPL);

    CNAS_MNTN_LogMsgInfo((MSG_HEADER_STRU*)pstDataReq);

    /* Send message to HRM. */
    (VOS_VOID)CNAS_SndInternalMsg(pstDataReq);

    return;

}

VOS_VOID CNAS_HLU_SendDataCancelNtf(VOS_VOID)
{
    HLU_HRM_DATA_CANCEL_NTF_STRU              *pstDataCancel;

    pstDataCancel = (HLU_HRM_DATA_CANCEL_NTF_STRU *)CNAS_GetIntMsgSendBuf(UEPS_PID_HLU,
                                          sizeof(HLU_HRM_DATA_CANCEL_NTF_STRU));
    if (VOS_NULL_PTR == pstDataCancel)
    {
        return;
    }

    NAS_MEM_SET_S(pstDataCancel, sizeof(HLU_HRM_DATA_CANCEL_NTF_STRU), 0, sizeof(HLU_HRM_DATA_CANCEL_NTF_STRU));

    pstDataCancel->ulLength = sizeof(HLU_HRM_DATA_CANCEL_NTF_STRU) - VOS_MSG_HEAD_LENGTH;
    pstDataCancel->enMsgId  = ID_HLU_HRM_DATA_CANCEL_NTF;

    CNAS_HLU_FILL_MSG_HDR(pstDataCancel, UEPS_PID_HLU, UEPS_PID_HRM);

    CNAS_MNTN_LogMsgInfo((MSG_HEADER_STRU*)pstDataCancel);

    /* Send message to HRM. */
    (VOS_VOID)CNAS_SndInternalMsg(pstDataCancel);

    return;
}


/*lint -restore*/

#endif







#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif




