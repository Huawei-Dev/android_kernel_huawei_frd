

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "CnasHrmCommFunc.h"
#include "CnasMntn.h"
#include "Nas_Mem.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_CNAS_HRM_COMM_FUNC_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
CNAS_HRM_MSG_TO_OTA_MSG_STRU            g_astHrmMntnHsmSmpMsgToOtaMsgUpTbl[] =
{
    {CNAS_HRM_HSM_SMP_MSG_SESSION_CLOSE,       NAS_OTA_MSG_HSM_SMP_MESSAGE_TYPE_SESSION_CLOSE_UP},
    {CNAS_HRM_HSM_SMP_MSG_KEEP_ALIVE_REQ,      NAS_OTA_MSG_HSM_SMP_MESSAGE_TYPE_KEEP_ALIVE_REQ_UP},
    {CNAS_HRM_HSM_SMP_MSG_KEEP_ALIVE_RESP,     NAS_OTA_MSG_HSM_SMP_MESSAGE_TYPE_KEEP_ALIVE_RESP_UP},
};

CNAS_HRM_MSG_TO_OTA_MSG_STRU            g_astHrmMntnHsmSmpMsgToOtaMsgDownTbl[] =
{
    {CNAS_HRM_HSM_SMP_MSG_SESSION_CLOSE,       NAS_OTA_MSG_HSM_SMP_MESSAGE_TYPE_SESSION_CLOSE_DOWN},
    {CNAS_HRM_HSM_SMP_MSG_KEEP_ALIVE_REQ,      NAS_OTA_MSG_HSM_SMP_MESSAGE_TYPE_KEEP_ALIVE_REQ_DOWN},
    {CNAS_HRM_HSM_SMP_MSG_KEEP_ALIVE_RESP,     NAS_OTA_MSG_HSM_SMP_MESSAGE_TYPE_KEEP_ALIVE_RESP_DOWN},
};

CNAS_HRM_MSG_TO_OTA_MSG_STRU            g_astHrmMntnHsmAmpMsgToOtaMsgTbl[] =
{
    {CNAS_HRM_HSM_AMP_MSG_UATI_REQ,            NAS_OTA_MSG_HSM_AMP_MSG_TYPE_UATI_REQ},
    {CNAS_HRM_HSM_AMP_MSG_UATI_ASSIGN,         NAS_OTA_MSG_HSM_AMP_MSG_TYPE_UATI_ASSIGN},
    {CNAS_HRM_HSM_AMP_MSG_UATI_COMPLETE,       NAS_OTA_MSG_HSM_AMP_MSG_TYPE_UATI_COMPLETE},
    {CNAS_HRM_HSM_AMP_MSG_HARDWAREID_REQ,      NAS_OTA_MSG_HSM_AMP_MSG_TYPE_HARDWAREID_REQ},
    {CNAS_HRM_HSM_AMP_MSG_HARDWAREID_RESP,     NAS_OTA_MSG_HSM_AMP_MSG_TYPE_HARDWAREID_RESP},
};

CNAS_HRM_MSG_TO_OTA_MSG_STRU            g_astHrmMntnHluMsgToOtaMsgTbl[] =
{
    {CNAS_HRM_HLU_HRPD_LOC_REQUEST,         NAS_OTA_MSG_HLU_LOC_REQUEST},
    {CNAS_HRM_HLU_HRPD_LOC_NOTIFICATION,    NAS_OTA_MSG_HLU_LOC_NOTIFICATION},
    {CNAS_HRM_HLU_HRPD_LOC_ASSIGNMENT,      NAS_OTA_MSG_HLU_LOC_ASSIGNMENT},
    {CNAS_HRM_HLU_HRPD_LOC_COMPLETE,        NAS_OTA_MSG_HLU_LOC_COMPLETE_UP},

    {CNAS_HRM_HLU_HRPD_BLOB_REQUEST,        NAS_OTA_MSG_HLU_BLOB_REQUEST},
    {CNAS_HRM_HLU_HRPD_BLOB_NOTIFICATION,   NAS_OTA_MSG_HLU_BLOB_NOTIFICATION},
    {CNAS_HRM_HLU_HRPD_BLOB_ASSIGNMENT,     NAS_OTA_MSG_HLU_BLOB_ASSIGNMENT},
    {CNAS_HRM_HLU_HRPD_BLOB_COMPLETE,       NAS_OTA_MSG_HLU_BLOB_COMPLETE_UP},
};

CNAS_HRM_MSG_INDEX_AND_TYPE_MAP_STRU    g_astHrmMsgIndexAndTypeMapTbl[] =
{
    {CNAS_HRM_DATA_REQ_BUFF_INDEX_UATI_REQ,                 CNAS_HRM_DATA_REQ_UATI_REQUEST},
    {CNAS_HRM_DATA_REQ_BUFF_INDEX_UATI_CMPL,                CNAS_HRM_DATA_REQ_UATI_CMPL},
    {CNAS_HRM_DATA_REQ_BUFF_INDEX_HARDWARE_ID,              CNAS_HRM_DATA_REQ_HARDWARE_ID},
    {CNAS_HRM_DATA_REQ_BUFF_INDEX_MO_KEEP_ALIVE,            CNAS_HRM_DATA_REQ_MO_KEEP_ALIVE},
    {CNAS_HRM_DATA_REQ_BUFF_INDEX_MT_KEEP_ALVIE,            CNAS_HRM_DATA_REQ_MT_KEEP_ALIVE},
    {CNAS_HRM_DATA_REQ_BUFF_INDEX_SESSION_CLOSE,            CNAS_HRM_DATA_REQ_SESSION_CLOSE},
    {CNAS_HRM_DATA_REQ_BUFF_INDEX_LOC_NTF,                  CNAS_HRM_DATA_REQ_LOC_NTF},
    {CNAS_HRM_DATA_REQ_BUFF_INDEX_HANDOFF_LOC_NTF,          CNAS_HRM_DATA_REQ_HANDOFF_LOC_NTF},
    {CNAS_HRM_DATA_REQ_BUFF_INDEX_LOC_CMPL,                 CNAS_HRM_DATA_REQ_LOC_CMPL},
    {CNAS_HRM_DATA_REQ_BUFF_INDEX_BLOB_NTF,                 CNAS_HRM_DATA_REQ_BLOB_NTF},
    {CNAS_HRM_DATA_REQ_BUFF_INDEX_BLOB_CMPL,                CNAS_HRM_DATA_REQ_BLOB_CMPL},
};

/*****************************************************************************
  3 函数定义
*****************************************************************************/

CNAS_HRM_DATA_REQ_TYPE_ENUM_UINT32 CNAS_HRM_GetDataReqTypeByMsgBuffIndex(
    VOS_UINT32                          ulBuffIndex
)
{

    VOS_UINT8                           i;

    for (i = 0; i< (sizeof(g_astHrmMsgIndexAndTypeMapTbl)/sizeof(CNAS_HRM_MSG_INDEX_AND_TYPE_MAP_STRU)); i++)
    {
        if (ulBuffIndex == g_astHrmMsgIndexAndTypeMapTbl[i].ulDataReqIndex)
        {
            return g_astHrmMsgIndexAndTypeMapTbl[i].enDataReqType;
        }
    }

    return CNAS_HRM_DATA_REQ_TYPE_ENUM_BUTT;
}


VOS_UINT8 CNAS_HRM_GetMsgBuffIndexByDataReqType(
    CNAS_HRM_DATA_REQ_TYPE_ENUM_UINT32  enDataReqType
)
{
    VOS_UINT8                           i;

    for (i = 0; i< (sizeof(g_astHrmMsgIndexAndTypeMapTbl)/sizeof(CNAS_HRM_MSG_INDEX_AND_TYPE_MAP_STRU)); i++)
    {
        if (enDataReqType == g_astHrmMsgIndexAndTypeMapTbl[i].enDataReqType)
        {
            return (VOS_UINT8)(g_astHrmMsgIndexAndTypeMapTbl[i].ulDataReqIndex);
        }
    }

    return CNAS_HRM_DATA_REQ_BUFF_INDEX_MAX;
}



VOS_UINT8 CNAS_HRM_GetMsgBuffIndexByHrmOpId(
    VOS_UINT16                          usOpId
)
{
    VOS_UINT8                           ucIndex;
    CNAS_HRM_DATA_REQ_BUFF_STRU        *pstReqMsgBuff = VOS_NULL_PTR;

    for (ucIndex = 0; ucIndex < CNAS_HRM_DATA_REQ_BUFF_INDEX_MAX; ucIndex++)
    {
        pstReqMsgBuff = CNAS_HRM_GetDataReqBuffAddr(ucIndex);

        if ((usOpId   == pstReqMsgBuff->stReqMsg.usHrmOpId)
         && (VOS_TRUE == pstReqMsgBuff->stReqMsg.ulIsReqMsgExisted))
        {
            return ucIndex;
        }
    }

    return CNAS_HRM_DATA_REQ_BUFF_INDEX_MAX;
}


RFUSER_CAS_HRPD_TASK_TYPE_ENUM_UINT16 CNAS_HRM_GetTaskTypeByDataReqType(
    CNAS_HRM_DATA_REQ_TYPE_ENUM_UINT32  enDataReqType
)
{
    RFUSER_CAS_HRPD_TASK_TYPE_ENUM_UINT16                   enTaskType;

    switch (enDataReqType)
    {
        case CNAS_HRM_DATA_REQ_UATI_REQUEST:
        case CNAS_HRM_DATA_REQ_UATI_CMPL:
            enTaskType = RFUSER_CAS_HRPD_TASK_TYPE_UATI;
            break;

        case CNAS_HRM_DATA_REQ_HARDWARE_ID:
            enTaskType = RFUSER_CAS_HRPD_TASK_TYPE_HARD_ID;
            break;

        case CNAS_HRM_DATA_REQ_MO_KEEP_ALIVE:
            enTaskType = RFUSER_CAS_HRPD_TASK_TYPE_MO_KEEP_ALIVE;
            break;

        case CNAS_HRM_DATA_REQ_MT_KEEP_ALIVE:
            enTaskType = RFUSER_CAS_HRPD_TASK_TYPE_MT_KEEP_ALIVE;
            break;

        case CNAS_HRM_DATA_REQ_SESSION_CLOSE:
            enTaskType = RFUSER_CAS_HRPD_TASK_TYPE_SESSION_CLOSE;
            break;

        case CNAS_HRM_DATA_REQ_LOC_NTF:
        case CNAS_HRM_DATA_REQ_HANDOFF_LOC_NTF:
        case CNAS_HRM_DATA_REQ_LOC_CMPL:
        case CNAS_HRM_DATA_REQ_BLOB_NTF:
        case CNAS_HRM_DATA_REQ_BLOB_CMPL:
            enTaskType = RFUSER_CAS_HRPD_TASK_TYPE_LU;
            break;

        default:
            enTaskType = RFUSER_CAS_HRPD_TASK_TYPE_ENUM_BUTT;
            break;
    }

    return enTaskType;
}

VOS_UINT8 CNAS_HRM_GetHsmMsgBuffIndexByTaskTypeExceptUati(
    RFUSER_CAS_HRPD_TASK_TYPE_ENUM_UINT16                   enTaskType
)
{
    VOS_UINT8                           ucBuffIndex;

    /* 因UATI REQ 和 UATI CMPL 都使用 RFUSER_CAS_HRPD_TASK_TYPE_UATI，
    根据 RFUSER_CAS_HRPD_TASK_TYPE_UATI 无法区分出是UATI REQ 还是 UATI CMPL ，
    故此函数中不包含获取UATI REQ 和 UATI CMPL对应的INDEX */
    switch (enTaskType)
    {
        case RFUSER_CAS_HRPD_TASK_TYPE_HARD_ID:
            ucBuffIndex = CNAS_HRM_DATA_REQ_BUFF_INDEX_HARDWARE_ID;
            break;

        case RFUSER_CAS_HRPD_TASK_TYPE_MO_KEEP_ALIVE:
            ucBuffIndex = CNAS_HRM_DATA_REQ_BUFF_INDEX_MO_KEEP_ALIVE;
            break;

        case RFUSER_CAS_HRPD_TASK_TYPE_MT_KEEP_ALIVE:
            ucBuffIndex = CNAS_HRM_DATA_REQ_BUFF_INDEX_MT_KEEP_ALVIE;
            break;

        case RFUSER_CAS_HRPD_TASK_TYPE_SESSION_CLOSE:
            ucBuffIndex = CNAS_HRM_DATA_REQ_BUFF_INDEX_SESSION_CLOSE;
            break;

        default:
            ucBuffIndex = CNAS_HRM_DATA_REQ_BUFF_INDEX_MAX;
            break;
    }

    return ucBuffIndex;
}

VOS_UINT16 CNAS_HRM_ConvertBitLenToByteLen(VOS_UINT16 usMsgBitLen)
{
    VOS_UINT16                          usMsgByteLen;

    if (usMsgBitLen % CNAS_HRM_BYTE_LEN == 0)
    {
        usMsgByteLen = usMsgBitLen / CNAS_HRM_BYTE_LEN;
    }
    else
    {
        usMsgByteLen = usMsgBitLen / CNAS_HRM_BYTE_LEN + 1;
    }

    return usMsgByteLen;
}


VOS_UINT8 CNAS_HRM_IsValidTaskType(
    RFUSER_CAS_HRPD_TASK_TYPE_ENUM_UINT16                   enTaskType
)
{
    switch (enTaskType)
    {
        case RFUSER_CAS_HRPD_TASK_TYPE_UATI:
        case RFUSER_CAS_HRPD_TASK_TYPE_HARD_ID:
        case RFUSER_CAS_HRPD_TASK_TYPE_MO_KEEP_ALIVE:
        case RFUSER_CAS_HRPD_TASK_TYPE_MT_KEEP_ALIVE:
        case RFUSER_CAS_HRPD_TASK_TYPE_SESSION_CLOSE:
        case RFUSER_CAS_HRPD_TASK_TYPE_LU:
            return VOS_TRUE;

        default:
            return VOS_FALSE;
    }
}

VOS_VOID CNAS_HRM_UpdateLuResApplyState(
    CNAS_HRM_RES_APPLY_STATE_ENUM_UINT8                     enLuResApplyState
)
{
    VOS_UINT8                           ucIndex;
    CNAS_HRM_DATA_REQ_BUFF_STRU        *pstDataReqBuff = VOS_NULL_PTR;

    for (ucIndex = CNAS_HRM_DATA_REQ_BUFF_INDEX_LOC_NTF; ucIndex <= CNAS_HRM_DATA_REQ_BUFF_INDEX_BLOB_CMPL; ucIndex++)
    {
        pstDataReqBuff = CNAS_HRM_GetDataReqBuffAddr(ucIndex);
        pstDataReqBuff->stResInfo.enResApplyState = enLuResApplyState;
    }

}


VOS_UINT32 CNAS_HRM_IsLuDataReqBuffExisted(VOS_VOID)
{
    VOS_UINT8                           ucIndex;
    CNAS_HRM_DATA_REQ_BUFF_STRU        *pstDataReqBuff = VOS_NULL_PTR;

    for (ucIndex = CNAS_HRM_DATA_REQ_BUFF_INDEX_LOC_NTF; ucIndex <= CNAS_HRM_DATA_REQ_BUFF_INDEX_BLOB_CMPL; ucIndex++)
    {
        pstDataReqBuff = CNAS_HRM_GetDataReqBuffAddr(ucIndex);

        if (VOS_TRUE == pstDataReqBuff->stReqMsg.ulIsReqMsgExisted)
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


VOS_UINT32 CNAS_HRM_IsDsds1FullFlowResProtect(
    VOS_UINT8                           ucBuffIndex
)
{
    CNAS_HRM_DSDS_TYPE_ENUM_UINT8       enDsdsType;

    enDsdsType        = CNAS_HRM_GetDsdsType();

    if ((CNAS_HRM_DSDS_TYPE_1 == enDsdsType)
        && ((CNAS_HRM_DATA_REQ_BUFF_INDEX_UATI_REQ == ucBuffIndex)
         || (CNAS_HRM_DATA_REQ_BUFF_INDEX_UATI_CMPL == ucBuffIndex)
         || (CNAS_HRM_DATA_REQ_BUFF_INDEX_MO_KEEP_ALIVE == ucBuffIndex)))
    {
        return VOS_TRUE;
    }
    else
    {
        return VOS_FALSE;
    }

}


NAS_OTA_MSG_ID_ENUM_UINT16 CNAS_HRM_ConvertMsgToOtaMsg(
    VOS_UINT16                          usHrmMsg,
    CNAS_HRM_MSG_TO_OTA_MSG_STRU       *pstHrmMsgToOtaMsgTab,
    VOS_UINT32                          ulHrmMsgToOtaMsgTabSize
)
{
    NAS_OTA_MSG_ID_ENUM_UINT16          enOtaMsgId;
    VOS_UINT32                          i;

    enOtaMsgId  = NAS_OTA_MSG_ID_BUTT;

    for (i = 0; i < ulHrmMsgToOtaMsgTabSize; i++)
    {
        if (usHrmMsg == pstHrmMsgToOtaMsgTab[i].usHrmMsgId)
        {
            enOtaMsgId = pstHrmMsgToOtaMsgTab[i].enOtaMsgId;
            break;
        }
    }

    return enOtaMsgId;
}


VOS_VOID CNAS_HRM_LogOmAirMsgUp(
    CNAS_CTTF_HRPD_SNP_DATA_REQ_STRU                       *pstDataReq
)
{
    NAS_OTA_MSG_ID_ENUM_UINT16                              enOtaMsgId;
    VOS_UINT32                                              ulSendPid;

    if (CTTF_HRPD_SNP_SESSION_MANAGEMENT_PROTOCOL == pstDataReq->enProtocolType)
    {
        enOtaMsgId = CNAS_HRM_ConvertMsgToOtaMsg(pstDataReq->stMsgData.aucMsgData[0],
                                                 g_astHrmMntnHsmSmpMsgToOtaMsgUpTbl,
                                                 sizeof(g_astHrmMntnHsmSmpMsgToOtaMsgUpTbl)/sizeof(CNAS_HRM_MSG_TO_OTA_MSG_STRU));
        ulSendPid  = UEPS_PID_HSM;
    }
    else if (CTTF_HRPD_SNP_ADDRESS_MANAGEMENT_PROTOCOL == pstDataReq->enProtocolType)
    {
        enOtaMsgId = CNAS_HRM_ConvertMsgToOtaMsg(pstDataReq->stMsgData.aucMsgData[0],
                                                 g_astHrmMntnHsmAmpMsgToOtaMsgTbl,
                                                 sizeof(g_astHrmMntnHsmAmpMsgToOtaMsgTbl)/sizeof(CNAS_HRM_MSG_TO_OTA_MSG_STRU));
        ulSendPid  = UEPS_PID_HSM;
    }
    else if (CTTF_HRPD_SNP_STREAM_2_APPLICATION_PROTOCOL == pstDataReq->enProtocolType)
    {

        enOtaMsgId = CNAS_HRM_ConvertMsgToOtaMsg(pstDataReq->stMsgData.aucMsgData[0],
                                                 g_astHrmMntnHluMsgToOtaMsgTbl,
                                                 sizeof(g_astHrmMntnHluMsgToOtaMsgTbl)/sizeof(CNAS_HRM_MSG_TO_OTA_MSG_STRU));
        ulSendPid  = UEPS_PID_HLU;
    }
    else
    {
        return;
    }

    CNAS_MNTN_LogOmAirMsg(CNAS_HRPD_OM_AIR_MSG_UP,
                          enOtaMsgId,
                          ulSendPid,
                         (VOS_UINT32)pstDataReq->stMsgData.usMsgBitLen,
                          pstDataReq->stMsgData.aucMsgData);
}


VOS_VOID CNAS_HRM_LogHrmCtx(VOS_VOID)
{
    CNAS_HRM_HRM_LOG_CTX_STRU          *pstLogCtxMsg = VOS_NULL_PTR;

    pstLogCtxMsg = (CNAS_HRM_HRM_LOG_CTX_STRU*)PS_MEM_ALLOC(UEPS_PID_HRM,
                             sizeof(CNAS_HRM_HRM_LOG_CTX_STRU));

    if ( VOS_NULL_PTR == pstLogCtxMsg )
    {
        CNAS_ERROR_LOG(UEPS_PID_HRM, "CNAS_HRM_LogHrmCtx: Alloc Memory Failed!");
        return;
    }

    NAS_MEM_SET_S(pstLogCtxMsg, sizeof(CNAS_HRM_HRM_LOG_CTX_STRU), 0, sizeof(CNAS_HRM_HRM_LOG_CTX_STRU));

    pstLogCtxMsg->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstLogCtxMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstLogCtxMsg->ulSenderPid     = UEPS_PID_HRM;
    pstLogCtxMsg->ulReceiverPid   = UEPS_PID_HRM;
    pstLogCtxMsg->ulLength        = sizeof(CNAS_HRM_HRM_LOG_CTX_STRU) - VOS_MSG_HEAD_LENGTH;
    pstLogCtxMsg->enMsgId         = ID_CNAS_HRM_HRM_LOG_CTX;
    NAS_MEM_CPY_S(&(pstLogCtxMsg->stHrmCtx), sizeof(pstLogCtxMsg->stHrmCtx), CNAS_HRM_GetCtxAddr(), sizeof(CNAS_HRM_CTX_STRU));

    DIAG_TraceReport(pstLogCtxMsg);

    PS_MEM_FREE(UEPS_PID_HRM, pstLogCtxMsg);

}

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */




