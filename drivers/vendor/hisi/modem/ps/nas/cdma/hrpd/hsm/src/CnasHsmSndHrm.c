

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "CnasHsmSndHrm.h"
#include "hrm_pif.h"
#include "hsm_hrm_pif.h"
#include "Nas_Mem.h"
#include "CnasMntn.h"
#include "CnasHsmMntn.h"
#include "CnasHsmComFunc.h"
#include "cttf_hrpd_sig_public_pif.h"
#include "CnasHsmFsmUatiRequest.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_CNAS_HSM_SND_HRM_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/
/*lint -save -e958*/

VOS_VOID CNAS_HSM_SndHrmSessionBeginNtf(
    HSM_HRM_SESSION_TYPE_ENUM_UINT16    enSessionType
)
{
    VOS_UINT32                          ulLen;
    HSM_HRM_SESSION_BEGIN_NTF_STRU     *pstMsg = VOS_NULL_PTR;

    ulLen   = sizeof(HSM_HRM_SESSION_BEGIN_NTF_STRU);

    pstMsg = (HSM_HRM_SESSION_BEGIN_NTF_STRU *)CNAS_GetIntMsgSendBuf(UEPS_PID_HSM, ulLen);

    if (VOS_NULL_PTR == pstMsg)
    {
        CNAS_ERROR_LOG(UEPS_PID_HSM, "CNAS_HSM_SndHrmSessionBeginNtf():Alloc memory failed.");

        return;
    }

    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = UEPS_PID_HSM;
    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = UEPS_PID_HRM;

    pstMsg->enMsgId                     = ID_HSM_HRM_SESSION_BEGIN_NTF;
    pstMsg->enSessionType               = enSessionType;
    pstMsg->usOpId                      = 0;
    pstMsg->ulLength                    = sizeof(HSM_HRM_SESSION_BEGIN_NTF_STRU) - VOS_MSG_HEAD_LENGTH;

    CNAS_SndInternalMsg(pstMsg);

    return;
}


VOS_VOID CNAS_HSM_SndHrmSessionEndNtf(
    HSM_HRM_SESSION_TYPE_ENUM_UINT16    enSessionType
)
{
    VOS_UINT32                          ulLen;
    HSM_HRM_SESSION_END_NTF_STRU       *pstMsg = VOS_NULL_PTR;

    ulLen   = sizeof(HSM_HRM_SESSION_END_NTF_STRU);

    pstMsg = (HSM_HRM_SESSION_END_NTF_STRU *)CNAS_GetIntMsgSendBuf(UEPS_PID_HSM, ulLen);

    if (VOS_NULL_PTR == pstMsg)
    {
        CNAS_ERROR_LOG(UEPS_PID_HSM, "CNAS_HSM_SndHrmSessionEndNtf():Alloc memory failed.");

        return;
    }

    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = UEPS_PID_HSM;
    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = UEPS_PID_HRM;

    pstMsg->enMsgId                     = ID_HSM_HRM_SESSION_END_NTF;
    pstMsg->usOpId                      = 0;
    pstMsg->enSessionType               = enSessionType;
    pstMsg->ulLength                    = sizeof(HSM_HRM_SESSION_END_NTF_STRU) - VOS_MSG_HEAD_LENGTH;

    CNAS_SndInternalMsg(pstMsg);

    return;
}


VOS_VOID CNAS_HSM_SndHrmDataCancelNtf(
    CNAS_HRM_DATA_REQ_TYPE_ENUM_UINT32  enCancelDataType
)
{
    VOS_UINT32                          ulLen;
    HSM_HRM_DATA_CANCEL_NTF_STRU       *pstMsg = VOS_NULL_PTR;

    ulLen   = sizeof(HSM_HRM_DATA_CANCEL_NTF_STRU);

    pstMsg  = (HSM_HRM_DATA_CANCEL_NTF_STRU *)CNAS_GetIntMsgSendBuf(UEPS_PID_HSM, ulLen);

    if (VOS_NULL_PTR == pstMsg)
    {
        CNAS_ERROR_LOG(UEPS_PID_HSM, "CNAS_HSM_SndHrmDataCancelNtf():Alloc memory failed.");

        return;
    }

    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = UEPS_PID_HSM;
    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = UEPS_PID_HRM;

    pstMsg->enMsgId                     = ID_HSM_HRM_DATA_CANCEL_NTF;
    pstMsg->usOpId                      = 0;
    pstMsg->ulLength                    = sizeof(HSM_HRM_DATA_CANCEL_NTF_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg->enDataReqType               = enCancelDataType;

    CNAS_SndInternalMsg(pstMsg);

    return;
}


VOS_VOID CNAS_HSM_SndHrmDataReq(
    CNAS_HSM_DATA_REQ_HEADER_STRU      *pstDataReqHeader,
    VOS_UINT16                         *pusOpId,
    VOS_UINT16                          usMsgBitLen,
    VOS_UINT8                          *pucMsgBuf
)
{
    HSM_HRM_DATA_REQ_STRU              *pstHsmHrmDataReq = VOS_NULL_PTR;
    VOS_UINT16                          usMsgByteLen;
    VOS_UINT32                          ulMsgDataLen;

    usMsgByteLen = CNAS_HSM_CovertBitLenToByteLen(usMsgBitLen);

    *pusOpId  = CNAS_HSM_GetSnpDataReqOpId();

    CNAS_HSM_IncreaseSnpDataReqOpId();

    /* SNPITF_MSG_DATA_STRU----should reduce the length of aucMsgData array */
    if (usMsgByteLen >= 4)
    {
        ulMsgDataLen = sizeof(HSM_HRM_DATA_REQ_STRU) + usMsgByteLen - VOS_MSG_HEAD_LENGTH - 4;
    }
    else
    {
        ulMsgDataLen = sizeof(HSM_HRM_DATA_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    }

    pstHsmHrmDataReq = (HSM_HRM_DATA_REQ_STRU *)CNAS_GetIntMsgSendBuf(UEPS_PID_HSM, ulMsgDataLen + VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstHsmHrmDataReq)
    {
        CNAS_ERROR_LOG(UEPS_PID_HSM, "CNAS_HSM_SndHrmDataReq():Alloc memory failed.");

        return;
    }

    NAS_MEM_SET_S(((VOS_UINT8 *)pstHsmHrmDataReq) + VOS_MSG_HEAD_LENGTH, ulMsgDataLen, 0, ulMsgDataLen);

    CNAS_CFG_MSG_HDR(pstHsmHrmDataReq,
                     UEPS_PID_HSM,
                     UEPS_PID_HRM,
                     ID_HSM_HRM_DATA_REQ,
                     ulMsgDataLen);

    pstHsmHrmDataReq->usOpId                                        = *pusOpId;
    pstHsmHrmDataReq->enDataReqType                                 = pstDataReqHeader->enDataReqType;

    /* here we always use inuse instance in HSM module,config instance is used in AS layer */
    pstHsmHrmDataReq->stDataReqCommContent.enInstanceType           = pstDataReqHeader->enInstanceType;
    pstHsmHrmDataReq->stDataReqCommContent.enDeliveryType           = pstDataReqHeader->enDeliveryType;
    pstHsmHrmDataReq->stDataReqCommContent.enChannelType            = pstDataReqHeader->enChannelType;
    pstHsmHrmDataReq->stDataReqCommContent.ucPriority               = pstDataReqHeader->ucPriority;
    pstHsmHrmDataReq->stDataReqCommContent.stMsgData.usMsgBitLen    = usMsgBitLen;
    pstHsmHrmDataReq->stDataReqCommContent.enProtocolType           = pstDataReqHeader->enProtocolType;
    pstHsmHrmDataReq->stDataReqCommContent.enAddressType            = pstDataReqHeader->enAddressType;
    pstHsmHrmDataReq->stDataReqCommContent.enNeedRslt               = CTTF_HRPD_BEST_EFFORT_NEED_RSLT;

    NAS_MEM_CPY_S(pstHsmHrmDataReq->stDataReqCommContent.stMsgData.aucMsgData, usMsgByteLen, pucMsgBuf, usMsgByteLen);

    CNAS_MNTN_LogMsgInfo((MSG_HEADER_STRU*)pstHsmHrmDataReq);

    CNAS_SndInternalMsg(pstHsmHrmDataReq);


    return;
}


VOS_VOID CNAS_HSM_SndKeepAliveReqMsg(
    VOS_UINT32                          ulTransIdAddFlag
)
{
    VOS_UINT8                          *pucKeepAliveReqMsgBuf = VOS_NULL_PTR;
    VOS_UINT16                          usMsgBufBitLen;
    VOS_UINT16                          usMsgBufByteLen;
    VOS_UINT8                           ucTransId;
    VOS_UINT16                          usOpId;
    CNAS_HSM_DATA_REQ_HEADER_STRU       stDataReqHeader;

    /* ulTransIdAddFlag为false的场景:收到hrm的Keep Alive Req Snp Data Cnf指示为重试原因值，
       需重发KeepAlive Req，此时KeepAlive Req的TransId不再加1   */
    if (VOS_TRUE == ulTransIdAddFlag)
    {
        /* increase keep alive req TI while send new keep alive req to AN */
        CNAS_HSM_IncreaseKeepAliveReqTransId();
    }

    ucTransId   = CNAS_HSM_GetKeepAliveReqTransId();

    usMsgBufByteLen = CNAS_HSM_KEEP_ALIVE_REQ_MSG_OCTET_LEN;

    /* first alloc UATI request message buffer */
    pucKeepAliveReqMsgBuf = (VOS_UINT8 *)PS_MEM_ALLOC(UEPS_PID_HSM, usMsgBufByteLen);

    if (VOS_NULL_PTR == pucKeepAliveReqMsgBuf)
    {
        return;
    }

    NAS_MEM_SET_S(pucKeepAliveReqMsgBuf, usMsgBufByteLen, 0, usMsgBufByteLen);

    /* Encode keep alive message */
    usMsgBufBitLen   = 0;
    CNAS_HSM_EncodeKeepAliveReq((VOS_VOID *)&ucTransId,
                                &usMsgBufBitLen,
                                pucKeepAliveReqMsgBuf);

    if (0 == usMsgBufBitLen)
    {
        CNAS_HSM_LogCodecResultInd(ID_CNAS_HSM_MNTN_LOG_ENCODE_RESULT_IND, CNAS_HSM_ENCODE_FAILURE);

        CNAS_WARNING_LOG(UEPS_PID_HSM, "CNAS_HSM_SndKeepAliveReqMsg: After encode usMsgBitLen is 0");

        PS_MEM_FREE(UEPS_PID_HSM, pucKeepAliveReqMsgBuf);

        return;
    }

    /* 由hrm再消息发送后打印 */

    NAS_MEM_SET_S(&stDataReqHeader, sizeof(stDataReqHeader), 0, sizeof(stDataReqHeader));

    /* fill hrpd snp date request header */
    CNAS_HSM_CFG_DATA_REQ_HEADER(&stDataReqHeader,
                                 CTTF_HRPD_SNP_SESSION_MANAGEMENT_PROTOCOL,
                                 CTTF_HRPD_ADDRESS_UNICAST,
                                 CTTF_HRPD_CHANNEL_AC_OR_RTC,
                                 CTTF_HRPD_DELIVERY_TYPE_BEST_EFFORT,
                                 CNAS_HSM_MSG_PRIORITY_KEEP_ALVIE_REQ,
                                 CTTF_HRPD_INSTANCE_INUSE,
                                 CNAS_HRM_DATA_REQ_MO_KEEP_ALIVE);

    CNAS_HSM_SndHrmDataReq(&stDataReqHeader,
                           &usOpId,
                           usMsgBufBitLen,
                           pucKeepAliveReqMsgBuf);

    CNAS_HSM_SaveKeepAliveReqSnpDataReqOpId(usOpId);

    PS_MEM_FREE(UEPS_PID_HSM, pucKeepAliveReqMsgBuf);
}


VOS_VOID CNAS_HSM_SndKeepAliveRspMsg(
    VOS_UINT8                           ucKeepAliveReqTransId
)
{
    VOS_UINT8                          *pucKeepAliveRspMsgBuf = VOS_NULL_PTR;
    VOS_UINT16                          usMsgBufBitLen;
    VOS_UINT16                          usMsgBufByteLen;
    VOS_UINT16                          usOpId;
    CNAS_HSM_DATA_REQ_HEADER_STRU       stDataReqHeader;

    usMsgBufByteLen = CNAS_HSM_KEEP_ALIVE_RESP_MSG_OCTET_LEN;

    /* first alloc UATI request message buffer */
    pucKeepAliveRspMsgBuf = (VOS_UINT8 *)PS_MEM_ALLOC(UEPS_PID_HSM, usMsgBufByteLen);

    if (VOS_NULL_PTR == pucKeepAliveRspMsgBuf)
    {
        return;
    }

    NAS_MEM_SET_S(pucKeepAliveRspMsgBuf, usMsgBufByteLen, 0, usMsgBufByteLen);

    /* Encode keep alive message */
    usMsgBufBitLen   = 0;
    CNAS_HSM_EncodeKeepAliveResp((VOS_VOID *)&ucKeepAliveReqTransId,
                                &usMsgBufBitLen,
                                pucKeepAliveRspMsgBuf);

    /* 由hrm再消息发送后打印 */

    if (0 == usMsgBufBitLen)
    {
        CNAS_HSM_LogCodecResultInd(ID_CNAS_HSM_MNTN_LOG_ENCODE_RESULT_IND, CNAS_HSM_ENCODE_FAILURE);

        CNAS_WARNING_LOG(UEPS_PID_HSM, "CNAS_HSM_SndKeepAliveRspMsg: After encode usMsgBitLen is 0");

        PS_MEM_FREE(UEPS_PID_HSM, pucKeepAliveRspMsgBuf);

        return;
    }

    NAS_MEM_SET_S(&stDataReqHeader, sizeof(stDataReqHeader), 0, sizeof(stDataReqHeader));

    /* fill hrpd date request header */
    CNAS_HSM_CFG_DATA_REQ_HEADER(&stDataReqHeader,
                                 CTTF_HRPD_SNP_SESSION_MANAGEMENT_PROTOCOL,
                                 CTTF_HRPD_ADDRESS_UNICAST,
                                 CTTF_HRPD_CHANNEL_AC_OR_RTC,
                                 CTTF_HRPD_DELIVERY_TYPE_BEST_EFFORT,
                                 CNAS_HSM_MSG_PRIORITY_KEEP_ALVIE_RSP,
                                 CTTF_HRPD_INSTANCE_INUSE,
                                 CNAS_HRM_DATA_REQ_MT_KEEP_ALIVE);

    CNAS_HSM_SndHrmDataReq(&stDataReqHeader,
                           &usOpId,
                           usMsgBufBitLen,
                           pucKeepAliveRspMsgBuf);

    CNAS_HSM_SaveKeepAliveRspSnpDataReqOpId(usOpId);

    CNAS_HSM_SaveStoreANKeepAliveReqTransId(ucKeepAliveReqTransId);

    PS_MEM_FREE(UEPS_PID_HSM, pucKeepAliveRspMsgBuf);
}



VOS_VOID CNAS_HSM_SndHardWareIdRspMsg(
    CNAS_HSM_HARDWARE_ID_RESPONSE_MSG_STRU                 *pstHardWareIdRsp
)
{
    CNAS_HSM_DATA_REQ_HEADER_STRU                           stDataReqHeader;
    VOS_UINT8                                               ucHardwareIdLen;
    VOS_UINT16                                              usMsgBitLen;
    VOS_UINT16                                              usMsgByteLen;
    VOS_UINT16                                              usOpId;
    VOS_UINT8                                              *pucHwIdRspMsgBuf      = VOS_NULL_PTR;
    CNAS_HSM_HARDWARE_ID_RESPONSE_MSG_STRU                 *pstStoreHardWareIdRsp = VOS_NULL_PTR;

    /* first alloc message for hardare id response */
    ucHardwareIdLen = CNAS_HSM_GetHardwareIdLength(pstHardWareIdRsp);
    usMsgByteLen    = CNAS_HSM_GET_HARDWARE_ID_RESPONSE_MSG_OCTET_LEN(ucHardwareIdLen);

    pucHwIdRspMsgBuf = (VOS_UINT8 *)PS_MEM_ALLOC(UEPS_PID_HSM, usMsgByteLen);

    if (VOS_NULL_PTR == pucHwIdRspMsgBuf)
    {
        return;
    }

    /* Encode HardWareId Resp message and send to SNP */
    CNAS_HSM_EncodeHardwareIdResponse((VOS_VOID*)pstHardWareIdRsp,
                                      &usMsgBitLen,
                                      pucHwIdRspMsgBuf);

    /* 由hrm再消息发送后打印 */

    if (0 == usMsgBitLen)
    {
        CNAS_HSM_LogCodecResultInd(ID_CNAS_HSM_MNTN_LOG_ENCODE_RESULT_IND, CNAS_HSM_ENCODE_FAILURE);
        PS_MEM_FREE(UEPS_PID_HSM, pucHwIdRspMsgBuf);

        return;
    }

    NAS_MEM_SET_S(&stDataReqHeader, sizeof(stDataReqHeader), 0, sizeof(stDataReqHeader));

    /* fill hrpd snp date request header */
    CNAS_HSM_CFG_DATA_REQ_HEADER(&stDataReqHeader,
                                 CTTF_HRPD_SNP_ADDRESS_MANAGEMENT_PROTOCOL,
                                 CTTF_HRPD_ADDRESS_UNICAST,
                                 CTTF_HRPD_CHANNEL_AC_OR_RTC,
                                 CTTF_HRPD_DELIVERY_TYPE_RELIABLE,
                                 CNAS_HSM_MSG_PRIORITY_HARDWARE_ID_RESP,
                                 CTTF_HRPD_INSTANCE_INUSE,
                                 CNAS_HRM_DATA_REQ_HARDWARE_ID);

    CNAS_HSM_SndHrmDataReq(&stDataReqHeader,
                              &usOpId,
                              usMsgBitLen,
                              pucHwIdRspMsgBuf);

    CNAS_HSM_SaveHardWareIdRspSnpDataReqOpId(usOpId);

    pstStoreHardWareIdRsp = CNAS_HSM_GetStoreHardWareIdRspAddr();

    NAS_MEM_CPY_S(pstStoreHardWareIdRsp,
                  sizeof(CNAS_HSM_HARDWARE_ID_RESPONSE_MSG_STRU),
                  pstHardWareIdRsp,
                  sizeof(CNAS_HSM_HARDWARE_ID_RESPONSE_MSG_STRU));

    PS_MEM_FREE(UEPS_PID_HSM, pucHwIdRspMsgBuf);
}



VOS_VOID CNAS_HSM_SndSessionCloseMsg(
    CNAS_HSM_SESSION_CLOSE_MSG_STRU                        *pstSessionClose
)
{
    VOS_UINT8                          *pucSessionCloseMsgBuf;
    VOS_UINT16                          usMsgBitLen;
    VOS_UINT16                          usMsgByteLen;
    CNAS_HSM_DATA_REQ_HEADER_STRU       stDataReqHeader;

    VOS_UINT16                          usOpId;

    CNAS_HSM_SESSION_CLOSE_MSG_STRU    *pstStoreSessionClose;

    usMsgByteLen  = CNAS_HSM_GetSessionCloseMsgEncodedBufLen(pstSessionClose);

    pucSessionCloseMsgBuf = (VOS_UINT8 *)PS_MEM_ALLOC(UEPS_PID_HSM, usMsgByteLen);

    if (VOS_NULL_PTR == pucSessionCloseMsgBuf)
    {
        return;
    }

    /* Encode Session Close message and send to SNP */
    usMsgBitLen = 0;
    CNAS_HSM_EncodeSessionClose((VOS_VOID*)pstSessionClose,
                                 &usMsgBitLen,
                                 pucSessionCloseMsgBuf);

    /* 由hrm再消息发送后打印 */

    NAS_MEM_SET_S(&stDataReqHeader, sizeof(stDataReqHeader), 0, sizeof(CNAS_HSM_DATA_REQ_HEADER_STRU));

    /* fill hrpd snp date request header */
    CNAS_HSM_CFG_DATA_REQ_HEADER(&stDataReqHeader,
                                 CNAS_HSM_SESSION_MANAGEMENT_PROTOCOL,
                                 CTTF_HRPD_ADDRESS_UNICAST,
                                 CTTF_HRPD_CHANNEL_AC_OR_RTC,
                                 CTTF_HRPD_DELIVERY_TYPE_BEST_EFFORT,
                                 CNAS_HSM_MSG_PRIORITY_SESSION_CLOSE,
                                 CTTF_HRPD_INSTANCE_INUSE,
                                 CNAS_HRM_DATA_REQ_SESSION_CLOSE);

    CNAS_HSM_SndHrmDataReq(&stDataReqHeader,
                           &usOpId,
                           usMsgBitLen,
                           pucSessionCloseMsgBuf);

    /* 无论是AN发起还是AT发起同一时间只能存在一个Session Deactive流程，
       所以可以使用一个全局变量来保存AN和AT触发的两种场景发送的Session Close消息的OpId */
    CNAS_HSM_SaveSessionCloseSnpDataReqOpId(usOpId);

    pstStoreSessionClose = CNAS_HSM_GetStoreSessionCloseAddr();

    NAS_MEM_CPY_S(pstStoreSessionClose,
                  sizeof(CNAS_HSM_SESSION_CLOSE_MSG_STRU),
                  pstSessionClose,
                  sizeof(CNAS_HSM_SESSION_CLOSE_MSG_STRU));

    CNAS_HSM_SetSendSessionCloseFlg(VOS_TRUE);

    PS_MEM_FREE(UEPS_PID_HSM, pucSessionCloseMsgBuf);
}


VOS_VOID CNAS_HSM_SndUatiCmplMsg(VOS_VOID)
{
    VOS_UINT8                          *pucUATICmpMsgBuf    = VOS_NULL_PTR;
    CNAS_HSM_UATI_INFO_STRU            *pstGlobalUait       = VOS_NULL_PTR;
    VOS_UINT16                          usMsgBufBitLen;
    VOS_UINT16                          usMsgBufByteLen;
    CNAS_HSM_UATI_COMP_MSG_STRU         stUaitCmpInput;
    CNAS_HSM_DATA_REQ_HEADER_STRU       stDataReqHeader;

    VOS_UINT16                          usOpId;

    pstGlobalUait = &(CNAS_HSM_GetSessionCtrlInfoAddr()->stUATIInfo);

    /* first alloc uati complete message */
    usMsgBufByteLen = CNAS_HSM_GET_UATI_COMPLETE_MSG_OCTET_LEN(pstGlobalUait->ucOldUATILen);

    pucUATICmpMsgBuf = (VOS_UINT8 *)PS_MEM_ALLOC(UEPS_PID_HSM, usMsgBufByteLen);

    if (VOS_NULL_PTR == pucUATICmpMsgBuf)
    {
        return;
    }

    NAS_MEM_SET_S(pucUATICmpMsgBuf, usMsgBufByteLen, 0, usMsgBufByteLen);

    NAS_MEM_SET_S(&stUaitCmpInput, sizeof(stUaitCmpInput), 0, sizeof(stUaitCmpInput));

    /* Fullfill UATIComplte input paramters, C.S0024 7.3.7.2.3 UAIComplete */
    stUaitCmpInput.ucMsgSeq        = CNAS_HSM_GetCurrUATIAssignMsgSeqNum();
    stUaitCmpInput.ucOldUATILen    = pstGlobalUait->ucOldUATILen;

    NAS_MEM_CPY_S(stUaitCmpInput.aucOldUATI, sizeof(stUaitCmpInput.aucOldUATI), pstGlobalUait->aucOldUATI, CNAS_HSM_UATI_OCTET_LENGTH);

    /* Encode UATIComplete message */
    usMsgBufBitLen = 0;
    CNAS_HSM_EncodeUATIComplete(&stUaitCmpInput,
                                &usMsgBufBitLen,
                                pucUATICmpMsgBuf);

    /* 由hrm在消息发送后打印 */

    if (0 == usMsgBufBitLen)
    {
        CNAS_HSM_LogCodecResultInd(ID_CNAS_HSM_MNTN_LOG_ENCODE_RESULT_IND, CNAS_HSM_ENCODE_FAILURE);
        CNAS_WARNING_LOG(UEPS_PID_HSM, "CNAS_HSM_SndUatiCmplMsg_UatiReq: After encode usMsgBitLen is 0");

        PS_MEM_FREE(UEPS_PID_HSM, pucUATICmpMsgBuf);

        return;
    }

    NAS_MEM_SET_S(&stDataReqHeader, sizeof(stDataReqHeader), 0, sizeof(stDataReqHeader));

    /* fill hrpd snp date request header */
    CNAS_HSM_CFG_DATA_REQ_HEADER(&stDataReqHeader,
                                 CTTF_HRPD_SNP_ADDRESS_MANAGEMENT_PROTOCOL,
                                 CTTF_HRPD_ADDRESS_UNICAST,
                                 CTTF_HRPD_CHANNEL_AC_OR_RTC,
                                 CTTF_HRPD_DELIVERY_TYPE_RELIABLE,
                                 CNAS_HSM_MSG_PRIORITY_UATI_COMP,
                                 CTTF_HRPD_INSTANCE_INUSE,
                                 CNAS_HRM_DATA_REQ_UATI_CMPL);

    CNAS_HSM_SndHrmDataReq(&stDataReqHeader,
                           &usOpId,
                           usMsgBufBitLen,
                           pucUATICmpMsgBuf);

    CNAS_HSM_SaveUatiCmplSnpDataReqOpId(usOpId);

    PS_MEM_FREE(UEPS_PID_HSM, pucUATICmpMsgBuf);
}



#endif
/*lint -restore*/

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

