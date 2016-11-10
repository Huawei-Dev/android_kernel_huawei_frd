

/*****************************************************************************
  1 Header File Include
*****************************************************************************/
#include "CnasEhsmCtx.h"
#include "CnasEhsmMntn.h"
#include "CnasEhsmComFunc.h"
#include "ehsm_hsd_pif.h"
#include "CnasEhsmSndAps.h"
#include "CnasEhsmSndEsm.h"
#include "CnasEhsmSndPpp.h"
#include "CnasEhsmSndHsd.h"
#include "CnasEhsmSndInternalMsg.h"
#include "CnasEhsmFsmDeactivatingTbl.h"
#include "CnasCcb.h"
#include "ehsm_aps_pif.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_CNAS_EHSM_COM_FUNC_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 The Define Of The Gloabal Variable
*****************************************************************************/
CNAS_EHSM_CTTF_ERR_CODE_MAP_STRU         g_astCnasEhsmCttfErrCodeMapTbl[] =
{
    { EHSM_APS_CAUSE_PPP_ERROR_GENERAL_ERROR,                                   CNAS_CTTF_EHRPD_ERROR_CODE_GENERAL_ERROR                                    },
    { EHSM_APS_CAUSE_PPP_ERROR_UNAUTHORIZED_APN,                                CNAS_CTTF_EHRPD_ERROR_CODE_UNAUTHORIZED_APN                                 },
    { EHSM_APS_CAUSE_PPP_ERROR_PDN_LIMIT_EXCEEDED,                              CNAS_CTTF_EHRPD_ERROR_CODE_PDN_LIMIT_EXCEEDED                               },
    { EHSM_APS_CAUSE_PPP_ERROR_NO_PGW_AVAILABLE,                                CNAS_CTTF_EHRPD_ERROR_CODE_NO_PGW_AVAILABLE                                 },
    { EHSM_APS_CAUSE_PPP_ERROR_PGW_UNREACHABLE,                                 CNAS_CTTF_EHRPD_ERROR_CODE_PGW_UNREACHABLE                                  },
    { EHSM_APS_CAUSE_PPP_ERROR_PGW_REJECT,                                      CNAS_CTTF_EHRPD_ERROR_CODE_PGW_REJECT                                       },
    { EHSM_APS_CAUSE_PPP_ERROR_INSUFFICIENT_PARAMETERS,                         CNAS_CTTF_EHRPD_ERROR_CODE_INSUFFICIENT_PARAMETERS                          },
    { EHSM_APS_CAUSE_PPP_ERROR_RESOURCE_UNAVAILABLE,                            CNAS_CTTF_EHRPD_ERROR_CODE_RESOURCE_UNAVAILABLE                             },
    { EHSM_APS_CAUSE_PPP_ERROR_ADMIN_PROHIBITED,                                CNAS_CTTF_EHRPD_ERROR_CODE_ADMIN_PROHIBITED                                 },
    { EHSM_APS_CAUSE_PPP_ERROR_PDNID_ALREADY_IN_USE,                            CNAS_CTTF_EHRPD_ERROR_CODE_PDNID_ALREADY_IN_USE                             },
    { EHSM_APS_CAUSE_PPP_ERROR_SUBSCRIPTION_LIMITATION,                         CNAS_CTTF_EHRPD_ERROR_CODE_SUBSCRIPTION_LIMITATION                          },
    { EHSM_APS_CAUSE_PPP_ERROR_PDN_CONNECTION_ALREADY_EXISTS_FOR_THIS_APN,      CNAS_CTTF_EHRPD_ERROR_CODE_PDN_CONNECTION_ALREADY_EXISTS_FOR_THIS_APN       },
    { EHSM_APS_CAUSE_PPP_ERROR_EMERGENCY_SERVICES_NOT_SUPPORTED,                CNAS_CTTF_EHRPD_ERROR_CODE_EMERGENCY_SERVICES_NOT_SUPPORTED                 },
    { EHSM_APS_CAUSE_PPP_ERROR_RECONNECT_TO_THIS_APN_NOT_ALLOWED,               CNAS_CTTF_EHRPD_ERROR_CODE_RECONNECT_TO_THIS_APN_NOT_ALLOWED                },
    { EHSM_APS_CAUSE_PPP_ERROR_CODE_AN_EAP_FAILURE,                             CNAS_CTTF_EHRPD_ERROR_CODE_AN_EAP_FAILURE                                   },
    { EHSM_APS_CAUSE_PPP_ERROR_CODE_AT_EAP_FAILURE,                             CNAS_CTTF_EHRPD_ERROR_CODE_AT_EAP_FAILURE                                   },
    { EHSM_APS_CAUSE_PPP_ERROR_CODE_EAP_TIMEROUT,                               CNAS_CTTF_EHRPD_ERROR_CODE_EAP_TIMEROUT                                     },
    { EHSM_APS_CAUSE_PPP_ERROR_CODE_LCP_TIMEROUT,                               CNAS_CTTF_EHRPD_ERROR_CODE_LCP_TIMEROUT                                     },
    { EHSM_APS_CAUSE_PPP_ERROR_CODE_LCP_TERMINATE,                              CNAS_CTTF_EHRPD_ERROR_CODE_LCP_TERMINATE                                    },
    { EHSM_APS_CAUSE_PPP_ERROR_CODE_UNSPECIFIC_FAILURE,                         CNAS_CTTF_EHRPD_ERROR_CODE_UNSPECIFIC_FAILURE                               },
    { EHSM_APS_CAUSE_PPP_ERROR_CODE_VSNCP_TIMEROUT,                             CNAS_CTTF_EHRPD_ERROR_CODE_VSNCP_TIMEROUT                                   },
    { EHSM_APS_CAUSE_PPP_ERROR_CODE_PARA_ERROR,                                 CNAS_CTTF_EHRPD_ERROR_CODE_PARA_ERROR                                       },
    { EHSM_APS_CAUSE_PPP_ERROR_CODE_ACCESS_AUTH_FAILURE,                        CNAS_CTTF_EHRPD_ERROR_CODE_ACCESS_AUTH_FAILURE                              },

};
HSM_EHSM_ERR_CODE_MAP_STRU          g_astEhsmHsmErrCodeMapTbl[] =
{
    {EHSM_HSM_RSLT_SUCCESS,                                 EHSM_APS_CAUSE_SUCCESS},
    {EHSM_HSM_RSLT_CONN_FAIL,                               EHSM_APS_CAUSE_HSM_CONN_FAIL},
    {EHSM_HSM_RSLT_MAX_ACCESS_PROBES,                       EHSM_APS_CAUSE_HSM_MAX_ACCESS_PROBES},
    {EHSM_HSM_RSLT_CONN_DENY,                               EHSM_APS_CAUSE_HSM_CONN_DENY},
    {EHSM_HSM_RSLT_RTCACK_FAIL,                             EHSM_APS_CAUSE_HSM_RTCACK_RAILURE},
    {EHSM_HSM_RSLT_SIGNAL_FADE,                             EHSM_APS_CAUSE_HSM_SIGNAL_FADE},
    {EHSM_HSM_RSLT_CAN_NOT_EST_CONN,                        EHSM_APS_CAUSE_HSM_CAN_NOT_EST_CONN},
    {EHSM_HSM_RSLT_TIMEOUT,                                 EHSM_APS_CAUSE_HSM_TIMEOUT},
    {EHSM_HSM_RSLT_HRPD_SLAVE,                              EHSM_APS_CAUSE_HSM_HRPD_SLAVE},
    {EHSM_HSM_RSLT_SESSION_NOT_EXIST,                       EHSM_APS_CAUSE_HSM_SESSION_NOT_EXSIT},
    {EHSM_HSM_RSLT_IN_UATI_PROCEDURE,                       EHSM_APS_CAUSE_HSM_RSLT_IN_UATI_PROCEDURE},
    {EHSM_HSM_RSLT_IN_SCP_PROCEDURE,                        EHSM_APS_CAUSE_HSM_RSLT_IN_SCP_PROCEDURE},
    {EHSM_HSM_RSLT_NO_RF,                                   EHSM_APS_CAUSE_HSM_NO_RF},
    {EHSM_HSM_RSLT_REDIRECT,                                EHSM_APS_CAUSE_HSM_REDIRECT},
    {EHSM_HSM_RSLT_OHM_NOT_CURRENT,                         EHSM_APS_CAUSE_HSM_OHM_NOT_CURRENT},

};

CTTF_EHSM_PDN_TYPE_MAP_STRU          g_astCttfEhsmPdnTypeMapTbl[] =
{
    {CNAS_CTTF_EHRPD_PDN_TYPE_INVALID,                    CNAS_EHSM_PDN_TYPE_INVALID},
    {CNAS_CTTF_EHRPD_PDN_TYPE_IPV4,                       CNAS_EHSM_PDN_TYPE_IPV4},
    {CNAS_CTTF_EHRPD_PDN_TYPE_IPV6,                       CNAS_EHSM_PDN_TYPE_IPV6},
    {CNAS_CTTF_EHRPD_PDN_TYPE_IPV4V6,                     CNAS_EHSM_PDN_TYPE_IPV4V6},
    {CNAS_CTTF_EHRPD_PDN_TYPE_ENUM_BUTT,                  CNAS_EHSM_PDN_TYPE_BUTT},
};

CNAS_EHSM_APS_EHSM_PDN_TYPE_MAP_STRU          g_astApsEhsmPdnTypeMapTbl[] =
{
    {EHSM_APS_PDN_TYPE_INVALID,                    CNAS_EHSM_PDN_TYPE_INVALID},
    {EHSM_APS_PDN_TYPE_IPV4,                       CNAS_EHSM_PDN_TYPE_IPV4},
    {EHSM_APS_PDN_TYPE_IPV6,                       CNAS_EHSM_PDN_TYPE_IPV6},
    {EHSM_APS_PDN_TYPE_IPV4V6,                     CNAS_EHSM_PDN_TYPE_IPV4V6},
    {EHSM_APS_PDN_TYPE_BUTT,                       CNAS_EHSM_PDN_TYPE_BUTT},
};

CNAS_EHSM_APS_CTTF_PDN_TYPE_MAP_STRU          g_astApsCttfPdnTypeMapTbl[] =
{
    {EHSM_APS_PDN_TYPE_INVALID,                           CNAS_CTTF_EHRPD_PDN_TYPE_INVALID},
    {EHSM_APS_PDN_TYPE_IPV4,                              CNAS_CTTF_EHRPD_PDN_TYPE_IPV4},
    {EHSM_APS_PDN_TYPE_IPV6,                              CNAS_CTTF_EHRPD_PDN_TYPE_IPV6},
    {EHSM_APS_PDN_TYPE_IPV4V6,                            CNAS_CTTF_EHRPD_PDN_TYPE_IPV4V6},
    {EHSM_APS_PDN_TYPE_BUTT,                              CNAS_CTTF_EHRPD_PDN_TYPE_ENUM_BUTT},
};

APS_CCB_PS_RAT_TYPE_MAP_STRU          g_astApsCcbPsRatTypeMapTbl[] =
{
    {APS_EHSM_PS_RATTYPE_NULL,                            CNAS_CCB_PS_RATTYPE_NULL},
    {APS_EHSM_PS_RATTYPE_1X,                              CNAS_CCB_PS_RATTYPE_1X},
    {APS_EHSM_PS_RATTYPE_HRPD,                            CNAS_CCB_PS_RATTYPE_HRPD},
    {APS_EHSM_PS_RATTYPE_EHRPD,                           CNAS_CCB_PS_RATTYPE_EHRPD},
    {APS_EHSM_PS_RATTYPE_LTE,                             CNAS_CCB_PS_RATTYPE_LTE},
    {APS_EHSM_PS_RATTYPE_BUTT,                            CNAS_CCB_PS_RATTYPE_BUTT},

};

CNAS_EHSM_APS_EHSM_AUTH_TYPE_MAP_STRU             g_astApsEhsmAuthTypeMapTbl[] =
{
    {EHSM_APS_AUTH_TYPE_NONE,                     CNAS_EHSM_AUTH_TYPE_NONE},
    {EHSM_APS_AUTH_TYPE_PAP,                      CNAS_EHSM_AUTH_TYPE_PAP},
    {EHSM_APS_AUTH_TYPE_CHAP,                     CNAS_EHSM_AUTH_TYPE_CHAP},
    {EHSM_APS_AUTH_TYPE_PAP_OR_CHAP,              CNAS_EHSM_AUTH_TYPE_CHAP},
    {EHSM_APS_AUTH_TYPE_BUTT,                     CNAS_EHSM_AUTH_TYPE_BUTT},
};

CTTF_EHSM_ATTACH_TYPE_MAP_STRU          g_astCttfEhsmAttachTypeMapTbl[] =
{
    {CNAS_CTTF_EHRPD_ATTACH_TYPE_INITIAL,                  CNAS_EHSM_ATTACH_TYPE_INITIAL},
    {CNAS_CTTF_EHRPD_ATTACH_TYPE_HANDOVER,                 CNAS_EHSM_ATTACH_TYPE_HANDOVER},
    {CNAS_CTTF_EHRPD_ATTACH_TYPE_ENUM_BUTT,                CNAS_EHSM_ATTACH_TYPE_ENUM_BUTT},
};
/*****************************************************************************
  3 Function Define
*****************************************************************************/



VOS_UINT8 CNAS_EHSM_GetCidFromLteBitCid(
    VOS_UINT32                          ulBitCid
)
{
    VOS_UINT8                           ucCid;
    VOS_UINT8                           ucBitCidIndex;

    /*
        根据当前与L4A的接口，转换成APS内部定义的Cid，由于是32位的数据长度，
        因此查找32位中的每一个bit
    */
    ucCid = CNAS_EHSM_INVALID_CID;

    for (ucBitCidIndex = 0; ucBitCidIndex < CNAS_EHSM_32BIT_DATA_LEN; ucBitCidIndex++)
    {
        /* 查看当前哪个一个BIT上是有值的，就认为这个是当前的CID定义 */
        if (ulBitCid & ((VOS_UINT32)0x00000001 << ucBitCidIndex))
        {
            /* BIT1代表CID 0，BIT32代表CID 31 */
            ucCid = ucBitCidIndex;
            break;
        }
    }

    return ucCid;
 }



VOS_UINT32 CNAS_EHSM_IsPsRatTypeChanged
(
    CNAS_CCB_PS_RATTYPE_ENUM_UINT32     enCcbPsRatType,
    APS_EHSM_PS_RATTYPE_ENUM_UINT32     enPsRatType
)
{
    if (enCcbPsRatType != CNAS_EHSM_MapApsPsRatTypeToCcb(enPsRatType))
    {
        return VOS_TRUE;
    }
    else
    {
        return VOS_FALSE;
    }
}


EHSM_APS_CAUSE_ENUM_UINT32 CNAS_EHSM_MapCttfErrCode(
    CNAS_CTTF_EHRPD_ERROR_CODE_ENUM_UINT16   enErrorCode
)
{
    CNAS_EHSM_CTTF_ERR_CODE_MAP_STRU   *pstCttfErrMapTblPtr = VOS_NULL_PTR;
    VOS_UINT32                          ulCttfErrMapTblSize;
    VOS_UINT32                          ulCnt;

    pstCttfErrMapTblPtr = CNAS_EHSM_GET_CTTF_ERR_CODE_MAP_TBL_PTR();
    ulCttfErrMapTblSize = CNAS_EHSM_GET_CTTF_ERR_CODE_MAP_TBL_SIZE();

    for (ulCnt = 0; ulCnt < ulCttfErrMapTblSize; ulCnt++)
    {
        if (pstCttfErrMapTblPtr[ulCnt].ulCttfErrCode == enErrorCode)
        {
            return pstCttfErrMapTblPtr[ulCnt].enEsmApsCause;
        }
    }

    return EHSM_APS_CAUSE_UNKNOWN;
}


VOS_UINT32 CNAS_EHSM_MapHsmCauseToAps(
    EHSM_HSM_RSLT_ENUM_UINT32        enCause
)
{
    HSM_EHSM_ERR_CODE_MAP_STRU         *pstHsmErrMapPtr = VOS_NULL_PTR;
    VOS_UINT32                          ulHsmErrMapTblSize;
    VOS_UINT32                          ulCnt;

    pstHsmErrMapPtr = CNAS_EHSM_GET_HSM_ERR_CODE_MAP_TBL_PTR();
    ulHsmErrMapTblSize = CNAS_EHSM_GET_HSM_ERR_CODE_MAP_TBL_SIZE();

    for (ulCnt = 0; ulCnt < ulHsmErrMapTblSize; ulCnt++)
    {
        if (pstHsmErrMapPtr[ulCnt].enHsmCause == enCause)
        {
            return pstHsmErrMapPtr[ulCnt].enApsCause;
        }
    }

    return EHSM_APS_CAUSE_UNKNOWN;
}

CNAS_EHSM_PDN_TYPE_ENUM_UINT8 CNAS_EHSM_MapCttfPdnTypeToEhms(
    CNAS_CTTF_EHRPD_PDN_TYPE_ENUM_UINT8                     enCttfPdnType
)
{
    CTTF_EHSM_PDN_TYPE_MAP_STRU        *pstPdnTypeMapPtr = VOS_NULL_PTR;
    VOS_UINT32                          ulPdnTypeMapTblSize;
    VOS_UINT32                          ulCnt;

    pstPdnTypeMapPtr    = CNAS_EHSM_GET_CTTF_PDN_TYPE_MAP_TBL_PTR();
    ulPdnTypeMapTblSize = CNAS_EHSM_GET_CTTF_PDN_TYPE_MAP_TBL_SIZE();

    for (ulCnt = 0; ulCnt < ulPdnTypeMapTblSize; ulCnt++)
    {
        if (pstPdnTypeMapPtr[ulCnt].enCttfPdnType == enCttfPdnType)
        {
            return pstPdnTypeMapPtr[ulCnt].enEhsmPdnType;
        }
    }

    return CNAS_EHSM_PDN_TYPE_INVALID;
}

CNAS_CTTF_EHRPD_PDN_TYPE_ENUM_UINT8 CNAS_EHSM_MapEhsmPdnTypeToCttf(
    CNAS_EHSM_PDN_TYPE_ENUM_UINT8       enPdnType
)
{
    CTTF_EHSM_PDN_TYPE_MAP_STRU        *pstPdnTypeMapPtr = VOS_NULL_PTR;
    VOS_UINT32                          ulPdnTypeMapTblSize;
    VOS_UINT32                          ulCnt;

    pstPdnTypeMapPtr    = CNAS_EHSM_GET_CTTF_PDN_TYPE_MAP_TBL_PTR();
    ulPdnTypeMapTblSize = CNAS_EHSM_GET_CTTF_PDN_TYPE_MAP_TBL_SIZE();

    for (ulCnt = 0; ulCnt < ulPdnTypeMapTblSize; ulCnt++)
    {
        if (pstPdnTypeMapPtr[ulCnt].enEhsmPdnType == enPdnType)
        {
            return pstPdnTypeMapPtr[ulCnt].enCttfPdnType;
        }
    }

    return CNAS_CTTF_EHRPD_PDN_TYPE_INVALID;
}


CNAS_EHSM_PDN_TYPE_ENUM_UINT8 CNAS_EHSM_MapApsPdnTypeToEhsm(
    EHSM_APS_PDN_TYPE_ENUM_UINT8        enPdnType
)
{
    CNAS_EHSM_APS_EHSM_PDN_TYPE_MAP_STRU                   *pstPdnTypeMapPtr = VOS_NULL_PTR;
    VOS_UINT32                                              ulPdnTypeMapTblSize;
    VOS_UINT32                                              ulCnt;

    pstPdnTypeMapPtr    = CNAS_EHSM_GET_EHSM_PDN_TYPE_MAP_TBL_PTR();
    ulPdnTypeMapTblSize = CNAS_EHSM_GET_EHSM_PDN_TYPE_MAP_TBL_SIZE();

    for (ulCnt = 0; ulCnt < ulPdnTypeMapTblSize; ulCnt++)
    {
        if (pstPdnTypeMapPtr[ulCnt].enApsPdnType== enPdnType)
        {
            return pstPdnTypeMapPtr[ulCnt].enEhsmPdnType;
        }
    }

    return CNAS_EHSM_PDN_TYPE_INVALID;
}


EHSM_APS_PDN_TYPE_ENUM_UINT8 CNAS_EHSM_MapEhsmPdnTypeToAps(
    CNAS_EHSM_PDN_TYPE_ENUM_UINT8       enPdnType
)
{
    CNAS_EHSM_APS_EHSM_PDN_TYPE_MAP_STRU                   *pstPdnTypeMapPtr = VOS_NULL_PTR;
    VOS_UINT32                                              ulPdnTypeMapTblSize;
    VOS_UINT32                                              ulCnt;

    pstPdnTypeMapPtr    = CNAS_EHSM_GET_EHSM_PDN_TYPE_MAP_TBL_PTR();
    ulPdnTypeMapTblSize = CNAS_EHSM_GET_EHSM_PDN_TYPE_MAP_TBL_SIZE();

    for (ulCnt = 0; ulCnt < ulPdnTypeMapTblSize; ulCnt++)
    {
        if (pstPdnTypeMapPtr[ulCnt].enEhsmPdnType == enPdnType)
        {
            return pstPdnTypeMapPtr[ulCnt].enApsPdnType;
        }
    }

    return EHSM_APS_PDN_TYPE_INVALID;
}


CNAS_CCB_PS_RATTYPE_ENUM_UINT32 CNAS_EHSM_MapApsPsRatTypeToCcb(
    APS_EHSM_PS_RATTYPE_ENUM_UINT32     enApsPsRatType
)
{
    APS_CCB_PS_RAT_TYPE_MAP_STRU       *pstPsRatTypeMapPtr = VOS_NULL_PTR;
    VOS_UINT32                          ulCttfPdnTypeMapTblSize;
    VOS_UINT32                          ulCnt;

    pstPsRatTypeMapPtr = CNAS_EHSM_GET_CCB_PS_RAT_TYPE_MAP_TBL_PTR();
    ulCttfPdnTypeMapTblSize = CNAS_EHSM_GET_CCB_PS_RAT_TYPE_MAP_TBL_SIZE();

    for (ulCnt = 0; ulCnt < ulCttfPdnTypeMapTblSize; ulCnt++)
    {
        if (pstPsRatTypeMapPtr[ulCnt].enAspPsRatType == enApsPsRatType)
        {
            return pstPsRatTypeMapPtr[ulCnt].enCcbPsRatType;
        }
    }

    return CNAS_CCB_PS_RATTYPE_BUTT;
}


CNAS_EHSM_ATTACH_TYPE_ENUM_UINT8 CNAS_EHSM_MapCttfAttachTypeToEhsm(
    CNAS_CTTF_EHRPD_ATTACH_TYPE_ENUM_UINT8                  enAttachType
)
{
    CTTF_EHSM_ATTACH_TYPE_MAP_STRU     *pstAttachTypeMapPtr = VOS_NULL_PTR;
    VOS_UINT32                          ulAttachTypeMapTblSize;
    VOS_UINT32                          ulCnt;

    pstAttachTypeMapPtr = CNAS_EHSM_GET_EHSM_ATTACH_TYPE_MAP_TBL_PTR();
    ulAttachTypeMapTblSize = CNAS_EHSM_GET_EHSM_ATTACH_TYPE_MAP_TBL_SIZE();

    for (ulCnt = 0; ulCnt < ulAttachTypeMapTblSize; ulCnt++)
    {
        if (pstAttachTypeMapPtr[ulCnt].enCttfAttachType== enAttachType)
        {
            return pstAttachTypeMapPtr[ulCnt].enEhsmAttachType;
        }
    }

    return CNAS_EHSM_ATTACH_TYPE_ENUM_BUTT;
}




CNAS_EHSM_AUTH_TYPE_ENUM_UINT8 CNAS_EHSM_MapApsAuthTypeToEhsm(
    EHSM_APS_AUTH_TYPE_ENUM_UINT8       enAuthType
)
{
    CNAS_EHSM_APS_EHSM_AUTH_TYPE_MAP_STRU                  *pstAuthTypeMapPtr = VOS_NULL_PTR;
    VOS_UINT32                                              ulAuthTypeMapTblSize;
    VOS_UINT32                                              ulCnt;

    pstAuthTypeMapPtr    = CNAS_EHSM_GET_EHSM_AUTH_TYPE_MAP_TBL_PTR();
    ulAuthTypeMapTblSize = CNAS_EHSM_GET_EHSM_AUTH_TYPE_MAP_TBL_SIZE();

    for (ulCnt = 0; ulCnt < ulAuthTypeMapTblSize; ulCnt++)
    {
        if (pstAuthTypeMapPtr[ulCnt].enAuthType == enAuthType)
        {
            return pstAuthTypeMapPtr[ulCnt].enEhsmAuthType;
        }
    }

    return CNAS_EHSM_AUTH_TYPE_NONE;
}



VOS_UINT32 CNAS_EHSM_IsCommonConnRetryNeeded(
    EHSM_HSM_RSLT_ENUM_UINT32           ulHsmCommConnEstCnfResult
)
{
    switch(ulHsmCommConnEstCnfResult)
    {
        case EHSM_HSM_RSLT_MAX_ACCESS_PROBES:
        case EHSM_HSM_RSLT_CONN_DENY:
        case EHSM_HSM_RSLT_RTCACK_FAIL:
        case EHSM_HSM_RSLT_SIGNAL_FADE:
        case EHSM_HSM_RSLT_CAN_NOT_EST_CONN:
        case EHSM_HSM_RSLT_SESSION_NOT_EXIST:
        case EHSM_HSM_RSLT_HRPD_SLAVE:
        case EHSM_HSM_RSLT_IN_UATI_PROCEDURE:
        case EHSM_HSM_RSLT_IN_SCP_PROCEDURE:
        case EHSM_HSM_RSLT_NO_RF:
        case EHSM_HSM_RSLT_REDIRECT:
        case EHSM_HSM_RSLT_PREFER_CH_NOT_VALID:
        case EHSM_HSM_RSLT_OHM_NOT_CURRENT:
            return VOS_TRUE;

        default:

            return VOS_FALSE;
    }
}



VOS_VOID CNAS_EHSM_GetApsPdnInfoStatusForEhsm(
    EHSM_APS_PDN_INFO_STRU             *pstPndInfo
)
{
    if (MODEM_ID_0 == CNAS_CCB_GetCdmaModeModemId())
    {
        TAF_APS_GetApsPdnInfoStatusForEhsm(pstPndInfo);
    }
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
    else if (MODEM_ID_1 == CNAS_CCB_GetCdmaModeModemId())
    {
        I1_TAF_APS_GetApsPdnInfoStatusForEhsm(pstPndInfo);
    }
#if ( 3 == MULTI_MODEM_NUMBER )/* three modems */
    else if (MODEM_ID_2 == CNAS_CCB_GetCdmaModeModemId())
    {
        I2_TAF_APS_GetApsPdnInfoStatusForEhsm(pstPndInfo);
    }
#endif
#endif
    else
    {
    }

    return;
}



VOS_VOID CNAS_EHSM_PorcEhsmPdnInfoDifferWithAps(VOS_VOID)
{
    EHSM_APS_PDN_INFO_STRU              stPndInfo;
    VOS_UINT32                          i;
    VOS_UINT32                          ulLtePdnNum;

    NAS_MEM_SET_S(&stPndInfo,
                  sizeof(EHSM_APS_PDN_INFO_STRU),
                  0X00,
                  sizeof(EHSM_APS_PDN_INFO_STRU));

    /* 获取APS中当前所有激活状态的PDN信息 */
    CNAS_EHSM_GetApsPdnInfoStatusForEhsm(&stPndInfo);

    ulLtePdnNum     = CNAS_EHSM_GetLteActivePdnNum();

    /* 第一种场景:
       EHSM无本地PDN信息；
       无LTE下的PDN信息；
       UE之前的数据业务在LTE模式下；
       APS当前LTE下的PDN实体个数非0
    */

    /* 第二种场景:
       EHSM有本地的PDN信息；
       无LTE下的PDN信息；
       UE之前的数据业务在LTE模式下；
       APS当前LTE下的PDN实体个数非0
    */

    if ((VOS_TRUE == stPndInfo.ulIsLteSerMode)
     && (0 != stPndInfo.ulPdnNum)
     && (0 == ulLtePdnNum))
    {

        /* EHSM与APS状态不一致的时候，为了恢复数传，需要先把当前的PDN全部去激活 */
        /* 记录ERROR LOG */
        CNAS_EHSM_RecodeErrorLogInfo(0,
                                     0,
                                     0,
                                     0,
                                     CNAS_EHSM_ERROR_CODE_PDN_DIFFER);

        /* 通知APS去激活 */
        for(i = 0; i < stPndInfo.ulPdnNum; i++)
        {
            CNAS_EHSM_SndApsLteHandOverToEhrpdInd(CNAS_EHSM_INVALID_PDN_ID,
                                                  stPndInfo.aucRabId[i],
                                                  EHSM_APS_PDN_OPTION_DEACTIVED);
        }

        CNAS_EHSM_SndCttfEhrpdDetachInd();
        CNAS_EHSM_SndEsmClearAllNtf();
        CNAS_EHSM_ClearAllEhrpdLocalPdnInfo();
        CNAS_EHSM_ClearAllLteLocalPdnInfo();
        CNAS_EHSM_InitApnRetryInfo();

        CNAS_EHSM_SetAirLinkExistFlag(VOS_FALSE);
        CNAS_EHSM_SetCurrFsmState(CNAS_EHSM_L1_STA_INACTIVE);
        CNAS_EHSM_SetEhrpdState(CNAS_EHSM_EHRPD_INACTIVE_STA);
    }

    return;
}

#endif /* FEATURE_UE_MODE_CDMA */

#ifdef __cplusplus
#if __cplusplus
    }
#endif /* __cpluscplus */
#endif /* __cpluscplus */


