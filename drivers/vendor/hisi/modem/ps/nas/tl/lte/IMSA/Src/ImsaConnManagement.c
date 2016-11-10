/******************************************************************************

   Copyright(C)2013,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : ImsaConnManagement.c
  Description     : 该C文件实现连接管理模块的初始化，内部消息的处理和发送，
                    提供外部调用的API
  History           :
     1.sunbing 49683      2013-06-19  Draft Enact


******************************************************************************/

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
/*lint -esym(766,*)*/
#include "ImsaConnManagement.h"
#include "ImsaPublic.h"
#include "ImsaEntity.h"
#include "ImsNicInterface.h"
#include "ImsaRegManagement.h"

/*#include "ImsaProcApsMsg.h"*/
#include "ImsaProcRnicMsg.h"
#include "ImsaProcAtMsg.h"
#include "ImsaProcApsMsg.h"

#include "ImsaProcWifiMsg.h"
#include "ImsaDomainManagement.h"

#if (VOS_WIN32 != VOS_OS_VER)
#if (VOS_VXWORKS == VOS_OS_VER)
#include <arpa/inet.h>
#else
#include "tcpip/public/tcpip_basetype.h"
#include "tcpip/public/tcpip_sock.h"
#endif
#endif


/*lint -e767*/
#define    THIS_FILE_ID      PS_FILE_ID_IMSACONNMANAGEMENT_C
#define    THIS_NAS_FILE_ID  NAS_FILE_ID_IMSACONNMANAGEMENT_C
/*lint +e767*/

/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if (FEATURE_ON == FEATURE_IMS)
/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/
VOS_CHAR                                g_acIpStringForSt_ipv4[IMSA_IPV4_ADDR_STRING_LEN + 1] = {0};
VOS_CHAR                                g_acIpStringForSt_ipv6[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};
#if (FEATURE_ON == FEATURE_PTM)
IMSA_CN_CAUSE_TRANS_STRU                g_astImsaCnRejCauseMap[] =
{
    /*------------taf casue-----------------------------------imsa cause---------------------------*/
    {TAF_PS_CAUSE_SM_NW_OPERATOR_DETERMINED_BARRING,        IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_OPERATOR_DETERMINED_BARRING},
    {TAF_PS_CAUSE_SM_NW_MBMS_BC_INSUFFICIENT,               IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_MBMS_BC_INSUFFICIENT},
    {TAF_PS_CAUSE_SM_NW_LLC_OR_SNDCP_FAILURE,               IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_LLC_OR_SNDCP_FAILURE},
    {TAF_PS_CAUSE_SM_NW_INSUFFICIENT_RESOURCES,             IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_INSUFFICIENT_RESOURCES},
    {TAF_PS_CAUSE_SM_NW_MISSING_OR_UKNOWN_APN,              IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_MISSING_OR_UKNOWN_APN},
    {TAF_PS_CAUSE_SM_NW_UNKNOWN_PDP_ADDR_OR_TYPE,           IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_UNKNOWN_PDP_ADDR_OR_TYPE},
    {TAF_PS_CAUSE_SM_NW_USER_AUTHENTICATION_FAIL,           IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_USER_AUTHENTICATION_FAIL},
    {TAF_PS_CAUSE_SM_NW_ACTIVATION_REJECTED_BY_GGSN_SGW_OR_PGW, IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_ACTIVATION_REJECTED_BY_GGSN},
    {TAF_PS_CAUSE_SM_NW_ACTIVATION_REJECTED_UNSPECIFIED,    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_ACTIVATION_REJECTED_UNSPECIFIED},
    {TAF_PS_CAUSE_SM_NW_SERVICE_OPTION_NOT_SUPPORTED,       IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_SERVICE_OPTION_NOT_SUPPORTED},
    {TAF_PS_CAUSE_SM_NW_REQUESTED_SERVICE_NOT_SUBSCRIBED,   IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_REQUESTED_SERVICE_NOT_SUBSCRIBED},
    {TAF_PS_CAUSE_SM_NW_SERVICE_OPTION_TEMP_OUT_ORDER,      IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_SERVICE_OPTION_TEMP_OUT_ORDER},
    {TAF_PS_CAUSE_SM_NW_NSAPI_ALREADY_USED,                 IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_NSAPI_ALREADY_USED},
    {TAF_PS_CAUSE_SM_NW_REGULAR_DEACTIVATION,               IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_REGULAR_DEACTIVATION},
    {TAF_PS_CAUSE_SM_NW_QOS_NOT_ACCEPTED,                   IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_QOS_NOT_ACCEPTED},
    {TAF_PS_CAUSE_SM_NW_NETWORK_FAILURE,                    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_NETWORK_FAILURE},
    {TAF_PS_CAUSE_SM_NW_REACTIVATION_REQUESTED,             IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_REACTIVATION_REQUESTED},
    {TAF_PS_CAUSE_SM_NW_FEATURE_NOT_SUPPORT,                IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_FEATURE_NOT_SUPPORT},
    {TAF_PS_CAUSE_SM_NW_SEMANTIC_ERR_IN_TFT,                IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_SEMANTIC_ERR_IN_TFT},
    {TAF_PS_CAUSE_SM_NW_SYNTACTIC_ERR_IN_TFT,               IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_SYNTACTIC_ERR_IN_TFT},
    {TAF_PS_CAUSE_SM_NW_UKNOWN_PDP_CONTEXT,                 IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_UKNOWN_PDP_CONTEXT},
    {TAF_PS_CAUSE_SM_NW_SEMANTIC_ERR_IN_PACKET_FILTER,      IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_SEMANTIC_ERR_IN_PACKET_FILTER},
    {TAF_PS_CAUSE_SM_NW_SYNCTACTIC_ERR_IN_PACKET_FILTER,    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_SYNCTACTIC_ERR_IN_PACKET_FILTER},
    {TAF_PS_CAUSE_SM_NW_PDP_CONTEXT_WITHOUT_TFT_ACTIVATED,  IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_PDP_CONTEXT_WITHOUT_TFT_ACTIVATED},
    {TAF_PS_CAUSE_SM_NW_MULTICAST_GROUP_MEMBERHHSHIP_TIMEOUT,   IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_MULTICAST_GROUP_MEMBERHHSHIP_TIMEOUT},
    {TAF_PS_CAUSE_SM_NW_REQUEST_REJECTED_BCM_VIOLATION,     IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_REQUEST_REJECTED_BCM_VIOLATION},
    {TAF_PS_CAUSE_SM_NW_LAST_PDN_DISCONN_NOT_ALLOWED,       IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_LAST_PDN_DISCONN_NOT_ALLOWED},
    {TAF_PS_CAUSE_SM_NW_PDP_TYPE_IPV4_ONLY_ALLOWED,         IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_PDP_TYPE_IPV4_ONLY_ALLOWED},
    {TAF_PS_CAUSE_SM_NW_PDP_TYPE_IPV6_ONLY_ALLOWED,         IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_PDP_TYPE_IPV6_ONLY_ALLOWED},
    {TAF_PS_CAUSE_SM_NW_SINGLE_ADDR_BEARERS_ONLY_ALLOWED,   IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_SINGLE_ADDR_BEARERS_ONLY_ALLOWED},
    {TAF_PS_CAUSE_SM_NW_INFORMATION_NOT_RECEIVED,           IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_INFORMATION_NOT_RECEIVED},
    {TAF_PS_CAUSE_SM_NW_PDN_CONNECTION_DOES_NOT_EXIST,      IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_PDN_CONNECTION_DOES_NOT_EXIST},
    {TAF_PS_CAUSE_SM_NW_SAME_APN_MULTI_PDN_CONNECTION_NOT_ALLOWED,   IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_SAME_APN_MULTI_PDN_CONNECTION_NOT_ALLOWED},
    {TAF_PS_CAUSE_SM_NW_COLLISION_WITH_NW_INITIATED_REQUEST,     IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_COLLISION_WITH_NW_INITIATED_REQUEST},
    {TAF_PS_CAUSE_SM_NW_UNSUPPORTED_QCI_VALUE,              IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_UNSUPPORTED_QCI_VALUE},
    {TAF_PS_CAUSE_SM_NW_BEARER_HANDLING_NOT_SUPPORTED,      IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_BEARER_HANDLING_NOT_SUPPORTED},
    {TAF_PS_CAUSE_SM_NW_INVALID_TI,                         IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_INVALID_TI},
    {TAF_PS_CAUSE_SM_NW_SEMANTICALLY_INCORRECT_MESSAGE,     IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_SEMANTICALLY_INCORRECT_MESSAGE},
    {TAF_PS_CAUSE_SM_NW_INVALID_MANDATORY_INFO,             IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_INVALID_MANDATORY_INFO},
    {TAF_PS_CAUSE_SM_NW_MSG_TYPE_NON_EXISTENT,              IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_MSG_TYPE_NON_EXISTENT},
    {TAF_PS_CAUSE_SM_NW_MSG_TYPE_NOT_COMPATIBLE,            IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_MSG_TYPE_NOT_COMPATIBLE},
    {TAF_PS_CAUSE_SM_NW_IE_NON_EXISTENT,                    IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_IE_NON_EXISTENT},
    {TAF_PS_CAUSE_SM_NW_CONDITIONAL_IE_ERR,                 IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_CONDITIONAL_IE_ERR},
    {TAF_PS_CAUSE_SM_NW_MSG_NOT_COMPATIBLE,                 IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_MSG_NOT_COMPATIBLE},
    {TAF_PS_CAUSE_SM_NW_PROTOCOL_ERR_UNSPECIFIED,           IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_PROTOCOL_ERR_UNSPECIFIED},
    {TAF_PS_CAUSE_SM_NW_APN_RESTRICTION_INCOMPATIBLE,       IMSA_ERR_LOG_PDNREJ_CAUSE_SM_NW_APN_RESTRICTION_INCOMPATIBLE}

};

static VOS_UINT32   g_astImsaCnRejCauseNum
        = sizeof(g_astImsaCnRejCauseMap)/sizeof(IMSA_CN_CAUSE_TRANS_STRU);
#endif

extern VOS_UINT32 IMSA_CONN_GetTimerLen
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_TIMER_ID_ENUM_UINT16           enTimerId
);
extern VOS_UINT32 IMSA_CONN_IsImsSdf
(
     const TAF_SDF_PARA_STRU            *pstSdfPara
);
extern VOS_UINT32 IMSA_CONN_IsPndTypeValid
(
     const TAF_SDF_PARA_STRU            *pstSdfPara
);
extern VOS_UINT32 IMSA_CONN_IsEmcSdf
(
     const TAF_SDF_PARA_STRU            *pstSdfPara
);
extern VOS_UINT32 IMSA_CONN_FindSipSignalDialPara
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    VOS_UINT32                          ulSdfNum,
    const TAF_SDF_PARA_STRU            *pstSdfPara,
    VOS_UINT32                         *pulIndex
);
extern VOS_VOID IMSA_CONN_SaveSelectedSdfPara
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    const TAF_SDF_PARA_STRU            *pstTafSdfPara
);
extern VOS_VOID IMSA_CONN_DeletePdpInfo2Reg
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    const IMSA_PDP_CNTXT_INFO_STRU     *pstPdpInfo
);
extern VOS_UINT32 IMSA_CONN_IsPdpIpvxInfoIncludeRegPara
(
    IMSA_IP_TYPE_ENUM_UINT8                 enIpType,
    VOS_CHAR                               *pcRegUeAddr,
    VOS_CHAR                               *pcRegPcscfAddr,
    const IMSA_PDP_CNTXT_INFO_STRU         *pstPdpInfo
);

extern IMSA_FSM_PROC IMSA_FsmGetProcItem(VOS_UINT32 ulState, VOS_UINT32 ulEvt);
extern VOS_UINT32 IMSA_FsmRun(IMSA_REG_ENTITY_STRU *pstEntity, VOS_UINT32 ulEvt, VOS_VOID *pData);
extern VOS_VOID IMSA_RegResetRetryTimes
(
    IMSA_REG_TYPE_ENUM_UINT8            enRegType
);
extern VOS_VOID IMSA_RegConverterReg2Ims
(
    IMSA_REG_TYPE_ENUM_UINT8 enRegType,
    const VOS_CHAR *pucIpAddr,
    const VOS_CHAR *pacPcscfAddr,
    IMSA_IMS_INPUT_EVENT_STRU *pstImsaImsInputEvt
);
extern VOS_VOID IMSA_RegConfigIpAddress2Ims
(
    IMSA_REG_TYPE_ENUM_UINT8            enRegType,
    const VOS_CHAR                     *pcUeAddr,
    const VOS_CHAR                     *pacPcscfAddr
);
extern VOS_VOID IMSA_RegSendImsMsgDereg
(
    IMSA_REG_TYPE_ENUM_UINT8            enRegType,
    VOS_UINT32                          ulIsLocal
);
extern VOS_UINT32 IMSA_RegSendImsMsgReg
(
    IMSA_REG_TYPE_ENUM_UINT8 enRegType,
    const VOS_CHAR *pcUeAddr,
    const VOS_CHAR *pcPcscfAddr
);
extern VOS_UINT32 IMSA_RegSendIntraMsgRegInd
(
    IMSA_REG_TYPE_ENUM_UINT8 enRegType,
    IMSA_REG_RESULT_ENUM_UINT32 enResult,
    IMSA_RESULT_ACTION_ENUM_UINT32 enAction
);
extern VOS_UINT32 IMSA_RegSendIntraMsgDeregInd
(
    IMSA_REG_TYPE_ENUM_UINT8 enRegType,
    IMSA_REG_RESULT_ENUM_UINT32 enResult,
    IMSA_RESULT_ACTION_ENUM_UINT32 enAction
);
extern VOS_UINT32 IMSA_RegGetAddrByParamType
(
    IMSA_REG_TYPE_ENUM_UINT8 enRegType,
    IMSA_REG_ADDR_PARAM_ENUM_UINT32 enParamType,
    VOS_CHAR *pucUeAddr,
    VOS_CHAR *pucPcscfAddr
);
extern VOS_UINT32 IMSA_RegParseImsError
(
    const IMSA_IMS_OUTPUT_SERVICE_EVENT_STRU  *pstOutputService,
    IMSA_IMS_OUTPUT_ERROR_STRU          *pstImsErrorInfo
);
extern VOS_UINT32 IMSA_RegCalculateRetryDelayTime
(
    VOS_UINT32                          ulMaxTime,
    VOS_UINT32                          ulBaseTime,
    VOS_UINT32                          ulConsecutiveFailures
);
extern VOS_UINT32 IMSA_RegCalculateRegFailureRetryIntervelTime
(
    VOS_UINT32                          ulRetryTimes,
    VOS_UINT32                          ulRetryAfter,
    VOS_UINT32                          ulIsReRegisteration
);
extern VOS_VOID IMSA_RegFsmProcRegisteringImsRegFailure305
(
    IMSA_REG_ENTITY_STRU               *pstEntity
);
extern VOS_VOID IMSA_RegInitRegWaitForRetryCommonProc
(
    IMSA_REG_ENTITY_STRU               *pstEntity,
    VOS_UINT32                          ulRetryAfter,
    VOS_UINT16                          usSipStatusCode
);
extern VOS_VOID IMSA_RegReRegWaitForRetryCommonProc
(
    IMSA_REG_ENTITY_STRU               *pstEntity,
    VOS_UINT32                          ulRetryAfter,
    VOS_UINT16                          usSipStatusCode,
    IMSA_REG_RESULT_ENUM_UINT32         enResult
);
extern VOS_UINT32 IMSA_RegProcImsMsgState(const IMSA_IMS_OUTPUT_SERVICE_EVENT_STRU  *pstOutputService);
extern IMSA_REG_ADDR_PAIR_STRU* IMSA_RegAddrPairCreate
(
    IMSA_IP_TYPE_ENUM_UINT8 enIpType,
    const VOS_CHAR *pacUeAddr,
    const VOS_CHAR *pacPcscfAddr
);
extern VOS_VOID IMSA_RegAddrPairDestroy(IMSA_REG_ADDR_PAIR_STRU *pstAddrPair);
extern IMSA_REG_ADDR_PAIR_STRU* IMSA_RegAddrPairListAdd
(
    IMSA_REG_ADDR_PAIR_STRU **ppstListHead,
    IMSA_REG_ADDR_PAIR_STRU *pstNode
);
extern VOS_VOID IMSA_RegAddrPairListDestroy(IMSA_REG_ADDR_PAIR_STRU *pstListHead);
extern VOS_UINT32 IMSA_RegAddrPairMgrUpdateWithNewPcscfAddr
(
    IMSA_REG_ADDR_PAIR_MGR_STRU *pstAddrPairMgr,
    IMSA_IP_TYPE_ENUM_UINT8 enIpType,
    const VOS_CHAR *pacPcscfAddr
);
extern VOS_UINT32 IMSA_RegAddrPairMgrUpdateWithNewUeAddr
(
    IMSA_REG_ADDR_PAIR_MGR_STRU *pstAddrPairMgr,
    IMSA_IP_TYPE_ENUM_UINT8 enIpType,
    const VOS_CHAR *pacUeAddr
);
extern VOS_VOID IMSA_RegGetAddrPairListHead
(
    IMSA_REG_TYPE_ENUM_UINT8            ulRegType,
    IMSA_IP_TYPE_ENUM_UINT8             enIpType,
    IMSA_REG_ADDR_PAIR_STRU          ***pppstListHead
);
extern VOS_VOID IMSA_RegAddrPairListClearAllUsedFlag(IMSA_REG_ADDR_PAIR_STRU *pstList);

extern VOS_UINT32 IMSA_CONN_PdnInfoNicConfig (IMS_NIC_PDN_INFO_CONFIG_STRU *pstConfigInfo);

extern VOS_VOID IMSA_CONN_DeleteEmcPdpInfo
(
    VOS_UINT8                               ucCid,
    VOS_UINT8                               ucRabId
);



extern VOS_VOID IMSA_CONN_PdnInfoNicDelete
(
    VOS_UINT8                           ucPdpId
);


extern VOS_VOID IMSA_CONN_ProcReEstablishConnSetupReq
(
    const IMSA_CONN_SETUP_REQ_STRU     *pstConnSetupReqMsg
);

extern VOS_UINT32 IMSA_CONN_FilterRegedPcscfInfo
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_PDP_CNTXT_INFO_STRU           *pstPdpContext
);

extern VOS_VOID IMSA_CONN_DeletePcscfInfo2Reg
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    const IMSA_PDP_CNTXT_INFO_STRU     *pstPdpInfo
);
extern VOS_VOID IMSA_CONN_ConfigPcscfInfo2Reg
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    const IMSA_PDP_CNTXT_INFO_STRU     *pstPdpInfo
);
extern VOS_VOID IMSA_CONN_DeleteOldPcscfInfoAndConfigNewPcscfInfo
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_PDP_CNTXT_INFO_STRU           *pstPdpContext,
    IMSA_PDP_CNTXT_INFO_STRU           *pstPdpContextOld
);

extern VOS_UINT32 IMSA_CONN_ProcIpAddrPcscfInW2LHO
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType
);

extern VOS_UINT32 IMSA_CONN_ProcIpAddrPcscfInL2WHO
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType
);



extern VOS_VOID IMSA_CONN_ProcLteConnRelReqInHoing
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType
);

extern VOS_VOID IMSA_CONN_ProcLteConnRelReqInAfterHoFail
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType
);

extern VOS_VOID IMSA_CONN_ProcLteConnRelReqInRelSpeedUp
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType
);

extern VOS_VOID IMSA_CONN_ProcWifiConnRelReqInHoing(VOS_VOID);

extern VOS_VOID IMSA_CONN_ProcWifiConnRelReqInAfterHoFail(VOS_VOID);



/*lint -e961*/
/*****************************************************************************
  3 Function
*****************************************************************************/


VOS_VOID IMSA_CONN_ClearNicPdpInfo(VOS_VOID)
{
    IMSA_CONN_MANAGER_STRU  *pstConnManager  = IMSA_CONN_GetConnManagerAddr();

    IMSA_INFO_LOG("IMSA_CONN_ClearNicPdpInfo is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_ClearNicPdpInfo_ENUM, LNAS_ENTRY);

    IMSA_MEM_SET_S( pstConnManager->astNicPdpInfoArray,
                    sizeof(pstConnManager->astNicPdpInfoArray),
                    0,
                    sizeof(IMSA_CONN_NIC_PDP_INFO_STRU)*IMSA_CONN_MAX_NIC_PDP_NUM);
}


VOS_VOID IMSA_CONN_DeletePdpId2Nic(VOS_VOID)
{
    IMSA_CONN_MANAGER_STRU  *pstConnManager  = IMSA_CONN_GetConnManagerAddr();
    VOS_UINT32              i                = 0;

    for (i = 0; i < IMSA_CONN_MAX_NIC_PDP_NUM; i++)
    {
        if (0 == pstConnManager->astNicPdpInfoArray[i].ucIsUsed)
        {
            continue;
        }

        if (IMSA_IMS_DOMAIN_WIFI == pstConnManager->astNicPdpInfoArray[i].enPdnConnType)
        {
            IMSA_CONN_PdnInfoNicDelete(pstConnManager->astNicPdpInfoArray[i].ucRnicId);

             /* 同上海范晶确认，在删除C核虚拟网卡时，通知RNIC模块，删除虚拟网卡，
                同时给AT上报承载状态 */
            IMSA_SndMsgRnicPdnDeactInd(pstConnManager->astNicPdpInfoArray[i].ucWifiPdnId, IMSA_RNIC_IMS_RAT_TYPE_WIFI);
            IMSA_SndMsgAtPdpDeactInd(pstConnManager->astNicPdpInfoArray[i].stPdpAddr.enIpType, IMSA_AT_IMS_RAT_TYPE_WIFI);

         }
        else
        {
            IMSA_CONN_PdnInfoNicDelete(pstConnManager->astNicPdpInfoArray[i].ucRnicId);

            if (IMSA_FALSE == pstConnManager->astNicPdpInfoArray[i].ucIsEmc)
            {
                /* 同上海范晶确认，在删除C核虚拟网卡时，通知RNIC模块，删除虚拟网卡，
                同时给AT上报承载状态 */
                IMSA_SndMsgRnicPdnDeactInd(pstConnManager->astNicPdpInfoArray[i].ucPdpId, IMSA_RNIC_IMS_RAT_TYPE_LTE);
                IMSA_SndMsgAtPdpDeactInd(pstConnManager->astNicPdpInfoArray[i].stPdpAddr.enIpType, IMSA_AT_IMS_RAT_TYPE_LTE);
            }


        }

    }
}



VOS_VOID IMSA_CONN_Init( VOS_VOID )
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn   = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn      = VOS_NULL_PTR;

    /* 初始化紧急连接 */
    pstEmcConn      = IMSA_CONN_GetEmcConnAddr();

    IMSA_MEM_SET_S(pstEmcConn, sizeof(IMSA_EMC_CONN_STRU), 0x0, sizeof(IMSA_EMC_CONN_STRU));

    pstEmcConn->enImsaConnStatus            = IMSA_CONN_STATUS_IDLE;
    pstEmcConn->enFirstReqPdnType           = IMSA_IP_TYPE_BUTT;
    pstEmcConn->enCurReqPdnType             = IMSA_IP_TYPE_BUTT;
    pstEmcConn->stProtectTimer.phTimer      = VOS_NULL_PTR;
    pstEmcConn->ucCurMaxOpid                = IMSA_CONN_EMC_CONN_MIN_OPID_VALUE;

    /* 初始化普通连接 */
    pstNormalConn   = IMSA_CONN_GetNormalConnAddr();

    IMSA_MEM_SET_S( pstNormalConn,
                    sizeof(IMSA_NORMAL_CONN_STRU),
                    0x0,
                    sizeof(IMSA_NORMAL_CONN_STRU));

    pstNormalConn->enImsaConnStatus         = IMSA_CONN_STATUS_IDLE;
    pstNormalConn->enFirstReqPdnType        = IMSA_IP_TYPE_BUTT;
    pstNormalConn->enCurReqPdnType          = IMSA_IP_TYPE_BUTT;
    pstNormalConn->stProtectTimer.phTimer   = VOS_NULL_PTR;
    pstNormalConn->ucCurMaxOpid             = IMSA_CONN_NORMAL_CONN_MIN_OPID_VALUE;

    /* 初始化存储配置给NIC的PDP信息 */
    IMSA_CONN_ClearNicPdpInfo();

    IMSA_CONN_ClearHoEvent();

    return ;
}



VOS_UINT8 IMSA_CONN_FindAvailRnicId(VOS_VOID)
{
    IMSA_CONN_MANAGER_STRU  *pstConnManager    =  IMSA_CONN_GetConnManagerAddr();
    VOS_UINT8                aucRnicId[IMSA_CONN_RNIC_ID_MAX_NUM]     = {0};
    VOS_UINT8                i;

    IMSA_INFO_LOG("IMSA_CONN_FindAvailRnicId: Enter");

    for (i = 0; i < IMSA_CONN_MAX_NIC_PDP_NUM; i++)
    {
        if (1 == pstConnManager->astNicPdpInfoArray[i].ucIsUsed)
        {
            if( pstConnManager->astNicPdpInfoArray[i].ucRnicId < IMSA_CONN_RNIC_ID_MAX_NUM)
            {
                aucRnicId[pstConnManager->astNicPdpInfoArray[i].ucRnicId] = 1;
            }
            else
            {
                IMSA_INFO_LOG("IMSA_CONN_FindAvailRnicId: RnicId error!");
            }
        }
    }
    for( i = 0; i<IMSA_CONN_RNIC_ID_MAX_NUM; i++)
    {
        if( 0 == aucRnicId[i])
        {
            break;
        }
    }

    return (i);
}





VOS_VOID IMSA_CONN_PdnInfoNicDelete
(
    VOS_UINT8                           ucRnicId
)
{
    IMSA_INFO_LOG("before IMS_NIC_PdnInfoDel");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_PdnInfoNicDelete_ENUM, LNAS_ENTRY);

    (VOS_VOID)IMS_NIC_PdnInfoDel(ucRnicId, MODEM_ID_0);

    IMSA_INFO_LOG("after IMS_NIC_PdnInfoDel");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_PdnInfoNicDelete_ENUM, LNAS_LEAVE);
}


VOS_VOID IMSA_CONN_ClearResource( VOS_VOID )
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn   = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn      = VOS_NULL_PTR;
    IMSA_WIFI_NORMAL_CONN_STRU         *pstWifiNormalConn   = VOS_NULL_PTR;
    /* VOS_UINT32                          i               = 0; */

    /* 初始化紧急连接 */
    pstEmcConn      = IMSA_CONN_GetEmcConnAddr();

    /* 停止保护定时器 */
    IMSA_StopTimer(&pstEmcConn->stProtectTimer);

    pstEmcConn->enImsaConnStatus            = IMSA_CONN_STATUS_IDLE;
    pstEmcConn->enFirstReqPdnType           = IMSA_IP_TYPE_BUTT;
    pstEmcConn->enCurReqPdnType             = IMSA_IP_TYPE_BUTT;
    pstEmcConn->stProtectTimer.phTimer      = VOS_NULL_PTR;
    pstEmcConn->ucCurMaxOpid                = IMSA_CONN_EMC_CONN_MIN_OPID_VALUE;
    pstEmcConn->ucOpid                      = IMSA_NULL;

    IMSA_MEM_SET_S( &pstEmcConn->stSelSdfPara,
                    sizeof(IMSA_SEL_SDF_PARA_STRU),
                    0x0,
                    sizeof(IMSA_SEL_SDF_PARA_STRU));

    IMSA_MEM_SET_S( &pstEmcConn->stSipSignalPdp,
                    sizeof(IMSA_PDP_CNTXT_INFO_STRU),
                    0x0,
                    sizeof(IMSA_PDP_CNTXT_INFO_STRU));

    pstEmcConn->ulSipMediaPdpNum        = IMSA_NULL;
    IMSA_MEM_SET_S( pstEmcConn->astSipMediaPdpArray,
                    sizeof(pstEmcConn->astSipMediaPdpArray),
                    0x0,
                    sizeof(IMSA_PDP_CNTXT_INFO_STRU)*IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM);

    /* 初始化普通连接 */
    pstNormalConn   = IMSA_CONN_GetNormalConnAddr();
    IMSA_StopTimer(&pstNormalConn->stProtectTimer);



    pstNormalConn->enImsaConnStatus         = IMSA_CONN_STATUS_IDLE;
    pstNormalConn->enFirstReqPdnType        = IMSA_IP_TYPE_BUTT;
    pstNormalConn->enCurReqPdnType          = IMSA_IP_TYPE_BUTT;
    pstNormalConn->stProtectTimer.phTimer   = VOS_NULL_PTR;
    pstNormalConn->ucCurMaxOpid             = IMSA_CONN_NORMAL_CONN_MIN_OPID_VALUE;
    pstNormalConn->ucOpid                   = IMSA_NULL;
    pstNormalConn->ulSipSignalPdpNum        = IMSA_NULL;

    IMSA_MEM_SET_S( pstNormalConn->astSipSignalPdpArray,
                    sizeof(pstNormalConn->astSipSignalPdpArray),
                    0x0,
                    sizeof(IMSA_PDP_CNTXT_INFO_STRU)*IMSA_CONN_MAX_NORMAL_SIP_SIGNAL_PDP_NUM);

    pstNormalConn->ulSipMediaPdpNum        = IMSA_NULL;
    IMSA_MEM_SET_S( pstNormalConn->astSipMediaPdpArray,
                    sizeof(pstNormalConn->astSipMediaPdpArray),
                    0x0,
                    sizeof(IMSA_PDP_CNTXT_INFO_STRU)*IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM);

    /* 将NIC PDP INFO中存储的承载号通知TTF删除 */
    IMSA_CONN_DeletePdpId2Nic();

    /* 清除存储配置给NIC的PDP信息*/
    IMSA_CONN_ClearNicPdpInfo();

    pstWifiNormalConn = IMSA_CONN_GetWifiNormalConnAddr();

    pstWifiNormalConn->enImsaConnStatus         = IMSA_CONN_STATUS_IDLE;

    IMSA_StopTimer(&pstWifiNormalConn->stWaitActiveCnf);
    IMSA_StopTimer(&pstWifiNormalConn->stWaitDeactiveCnf);
    IMSA_StopTimer(&pstWifiNormalConn->stWaitReadyInd);

    IMSA_CONN_ClearHoEvent();


    /* 关机清连接模块资源后,需要通知CDS清除IMS承载信息, 解决问题如:IMS注册成功, 然后Modem关机并关闭VoLTE, 再次开机
       手动建立承载, 由于在关机时IMSA未通知CDS清除IMS承载信息, 从而导致再次开机建立的承载ID与关机前IMS 信令承载ID
       相同时误以为当前承载为IMS承载, 从而把数据把发给IMS, 从而导致数传不通*/
    IMSA_CONN_SndCdsSetImsBearerReq(IMSA_CDS_IMS_BEARER_RAT_TYPE_3GPP_EUTRAN);

    return ;
}


VOS_VOID IMSA_CONN_ClearRatResource
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_IMS_DOMAIN_ENUM_UINT8           enConnRatType
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn     = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn        = VOS_NULL_PTR;
    IMSA_WIFI_NORMAL_CONN_STRU         *pstWifiNormalConn = VOS_NULL_PTR;

    if (IMSA_IMS_DOMAIN_LTE == enConnRatType)
    {
        if (IMSA_CONN_TYPE_EMC == enConnType)
        {
            pstEmcConn                      = IMSA_CONN_GetEmcConnAddr();

            IMSA_StopTimer(&pstEmcConn->stProtectTimer);
            pstEmcConn->enCurReqPdnType     = IMSA_IP_TYPE_BUTT;
            pstEmcConn->enFirstReqPdnType   = IMSA_IP_TYPE_BUTT;
            pstEmcConn->enImsaConnStatus    = IMSA_CONN_STATUS_IDLE;
            pstEmcConn->ucOpid              = IMSA_CONN_ILLEGAL_OPID;
            IMSA_MEM_SET_S(                   &pstEmcConn->stSelSdfPara,
                                           sizeof(IMSA_SEL_SDF_PARA_STRU),
                                            0x0,
                                            sizeof(IMSA_SEL_SDF_PARA_STRU));

            IMSA_MEM_SET_S(                   &pstEmcConn->stSipSignalPdp,
                                            sizeof(IMSA_PDP_CNTXT_INFO_STRU),
                                            0x0,
                                            sizeof(IMSA_PDP_CNTXT_INFO_STRU));

            pstEmcConn->ulSipMediaPdpNum    = 0;
            IMSA_MEM_SET_S(                   pstEmcConn->astSipMediaPdpArray,
                                            sizeof(IMSA_PDP_CNTXT_INFO_STRU)*IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM,
                                            0x0,

                                            sizeof(IMSA_PDP_CNTXT_INFO_STRU)*IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM);
        }
        else
        {
            pstNormalConn = IMSA_CONN_GetNormalConnAddr();
            IMSA_StopTimer(&pstNormalConn->stProtectTimer);
            pstNormalConn->enCurReqPdnType  = IMSA_IP_TYPE_BUTT;
            pstNormalConn->enFirstReqPdnType= IMSA_IP_TYPE_BUTT;
            pstNormalConn->enImsaConnStatus = IMSA_CONN_STATUS_IDLE;
            pstNormalConn->ucOpid           = IMSA_CONN_ILLEGAL_OPID;
            IMSA_MEM_SET_S(                   &pstNormalConn->stSelSdfPara,
                                              sizeof(IMSA_SEL_SDF_PARA_STRU),
                                            0x0,
                                            sizeof(IMSA_SEL_SDF_PARA_STRU));
            pstNormalConn->ulSipSignalPdpNum= 0;
            IMSA_MEM_SET_S(                   pstNormalConn->astSipSignalPdpArray,
                                            sizeof(IMSA_PDP_CNTXT_INFO_STRU)*IMSA_CONN_MAX_NORMAL_SIP_SIGNAL_PDP_NUM,
                                            0x0,
                                            sizeof(IMSA_PDP_CNTXT_INFO_STRU)*IMSA_CONN_MAX_NORMAL_SIP_SIGNAL_PDP_NUM);
            pstNormalConn->ulSipMediaPdpNum = 0;
            IMSA_MEM_SET_S(                   pstNormalConn->astSipMediaPdpArray,
                                          sizeof(IMSA_PDP_CNTXT_INFO_STRU)*IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM,
                                            0x0,
                                            sizeof(IMSA_PDP_CNTXT_INFO_STRU)*IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM);
        }
    }
    else if(IMSA_IMS_DOMAIN_WIFI == enConnRatType)
    {
        if (IMSA_CONN_TYPE_NORMAL == enConnType)
        {
            pstWifiNormalConn = IMSA_CONN_GetWifiNormalConnAddr();

            IMSA_CONN_StopTimer(IMSA_CONN_TYPE_NORMAL,TI_IMSA_WAIT_WIFI_PDP_ACTIVE_CNF);
            IMSA_CONN_StopTimer(IMSA_CONN_TYPE_NORMAL,TI_IMSA_WAIT_WIFI_PDP_READY_IND);
            IMSA_CONN_StopTimer(IMSA_CONN_TYPE_NORMAL,TI_IMSA_WAIT_WIFI_PDP_DEACTIVE_CNF);

            pstWifiNormalConn->enImsaConnStatus = IMSA_CONN_STATUS_IDLE;

            IMSA_MEM_SET_S(&pstWifiNormalConn->stSipSignalPdp,
                        sizeof(IMSA_WIFI_PDP_CNTXT_INFO_STRU),
                         0x0,
                         sizeof(IMSA_WIFI_PDP_CNTXT_INFO_STRU));
        }
    }
    else
    {

    }
}



VOS_VOID IMSA_CONN_AssignOpid
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    VOS_UINT8                          *pucOpid
)
{
    VOS_UINT8                           ucOpid = IMSA_NULL;

    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        /*获取当前最大OPID*/
        ucOpid = IMSA_CONN_GetNormalConnCurMaxOpid();

        /*如果还未到最大值，直接加1*/
        if( ucOpid < IMSA_CONN_NORMAL_CONN_MAX_OPID_VALUE)
        {
            *pucOpid = ucOpid + 1;
        }
        else/*如果已经是最大值，从初值开始分配*/
        {
            (*pucOpid) = IMSA_CONN_NORMAL_CONN_MIN_OPID_VALUE;
        }

        IMSA_CONN_SetNormalConnCurMaxOpid(*pucOpid);

        IMSA_INFO_LOG1("IMSA_CONN_AssignOpid,normal,assin opid:", (*pucOpid));
        TLPS_PRINT2LAYER_INFO1(IMSA_CONN_AssignOpid_ENUM, IMSA_NORMAL_OPID, (*pucOpid));
    }
    else
    {
        /*获取当前最大OPID*/
        ucOpid = IMSA_CONN_GetEmcConnCurMaxOpid();

        /*如果还未到最大值，直接加1*/
        if( ucOpid < IMSA_CONN_EMC_CONN_MAX_OPID_VALUE)
        {
            *pucOpid = ucOpid + 1;
        }
        else/*如果已经是最大值，从初值开始分配*/
        {
            (*pucOpid) = IMSA_CONN_EMC_CONN_MIN_OPID_VALUE;
        }

        IMSA_CONN_SetEmcConnCurMaxOpid(*pucOpid);

        IMSA_INFO_LOG1("IMSA_CONN_AssignOpid,emc,assin opid:", (*pucOpid));
        TLPS_PRINT2LAYER_INFO1(IMSA_CONN_AssignOpid_ENUM, IMSA_EMERGENCY_OPID, (*pucOpid));
    }
}



VOS_UINT32 IMSA_CONN_GetConnTypeByOpid
(
    VOS_UINT8                           ucOpid,
    IMSA_CONN_TYPE_ENUM_UINT32         *penConnType
)
{
    if (ucOpid == IMSA_CONN_GetNormalConnOpid())
    {
        *penConnType = IMSA_CONN_TYPE_NORMAL;
        return IMSA_SUCC;
    }

    if (ucOpid == IMSA_CONN_GetEmcConnOpid())
    {
        *penConnType = IMSA_CONN_TYPE_EMC;
        return IMSA_SUCC;
    }

    return IMSA_FAIL;
}


IMSA_PDP_STATE_ENUM_UINT8 IMSA_CONN_GetSipSignalPdpState
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_IP_TYPE_ENUM_UINT8             enIpType
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn   = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn      = VOS_NULL_PTR;
    VOS_UINT32                          i               = IMSA_NULL;

    if (IMSA_CONN_TYPE_EMC == enConnType)
    {
        pstEmcConn  = IMSA_CONN_GetEmcConnAddr();

        if ((IMSA_PDP_STATE_ACTIVE == pstEmcConn->stSipSignalPdp.enPdpState)
            && (enIpType == pstEmcConn->stSipSignalPdp.stPdpAddr.enIpType))
        {
            return IMSA_PDP_STATE_ACTIVE;
        }

        return IMSA_PDP_STATE_INACTIVE;
    }

    pstNormalConn   = IMSA_CONN_GetNormalConnAddr();
    for (i = 0; i < pstNormalConn->ulSipSignalPdpNum; i++)
    {
        if ((IMSA_PDP_STATE_ACTIVE == pstNormalConn->astSipSignalPdpArray[i].enPdpState)
            && (enIpType == pstNormalConn->astSipSignalPdpArray[i].stPdpAddr.enIpType))
        {
            return IMSA_PDP_STATE_ACTIVE;
        }
    }

    return IMSA_PDP_STATE_INACTIVE;
}


VOS_UINT32 IMSA_CONN_IsConnStatusEqual
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_CONN_STATUS_ENUM_UINT8         enImsaConnStatus,
    IMSA_IMS_DOMAIN_ENUM_UINT8           enRatType
)
{
    IMSA_CONN_STATUS_ENUM_UINT8         enImsaConnStatusTmp = IMSA_CONN_STATUS_BUTT;

    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        if (IMSA_IMS_DOMAIN_LTE == enRatType)
        {
            enImsaConnStatusTmp = IMSA_CONN_GetNormalConnStatus();
        }
        else
        {
            enImsaConnStatusTmp = IMSA_CONN_GetWifiNormalConnStatus();
        }
    }
    else
    {
        if (IMSA_IMS_DOMAIN_LTE == enRatType)
        {
            enImsaConnStatusTmp = IMSA_CONN_GetEmcConnStatus();
        }
        else
        {
        }
    }

    if (enImsaConnStatus == enImsaConnStatusTmp)
    {
        return IMSA_TRUE;
    }
    else
    {
        return IMSA_FALSE;
    }
}


VOS_VOID IMSA_CONN_ClearConnResource
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn   = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn      = VOS_NULL_PTR;

    if (IMSA_CONN_TYPE_EMC == enConnType)
    {
        pstEmcConn                      = IMSA_CONN_GetEmcConnAddr();

        IMSA_StopTimer(&pstEmcConn->stProtectTimer);
        pstEmcConn->enCurReqPdnType     = IMSA_IP_TYPE_BUTT;
        pstEmcConn->enFirstReqPdnType   = IMSA_IP_TYPE_BUTT;
        pstEmcConn->enImsaConnStatus    = IMSA_CONN_STATUS_IDLE;
        pstEmcConn->ucOpid              = IMSA_CONN_ILLEGAL_OPID;

        IMSA_MEM_SET_S(                 &pstEmcConn->stSelSdfPara,
                                        sizeof(IMSA_SEL_SDF_PARA_STRU),
                                        0x0,
                                        sizeof(IMSA_SEL_SDF_PARA_STRU));

        IMSA_MEM_SET_S(                 &pstEmcConn->stSipSignalPdp,
                                        sizeof(IMSA_PDP_CNTXT_INFO_STRU),
                                        0x0,
                                        sizeof(IMSA_PDP_CNTXT_INFO_STRU));

        pstEmcConn->ulSipMediaPdpNum    = 0;
        IMSA_MEM_SET_S(                 pstEmcConn->astSipMediaPdpArray,
                                        sizeof(pstEmcConn->astSipMediaPdpArray),
                                        0x0,
                                        sizeof(IMSA_PDP_CNTXT_INFO_STRU)*IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM);

    }
    else
    {
        pstNormalConn = IMSA_CONN_GetNormalConnAddr();
        IMSA_StopTimer(&pstNormalConn->stProtectTimer);
        pstNormalConn->enCurReqPdnType  = IMSA_IP_TYPE_BUTT;
        pstNormalConn->enFirstReqPdnType= IMSA_IP_TYPE_BUTT;
        pstNormalConn->enImsaConnStatus = IMSA_CONN_STATUS_IDLE;
        pstNormalConn->ucOpid           = IMSA_CONN_ILLEGAL_OPID;

        IMSA_MEM_SET_S(                 &pstNormalConn->stSelSdfPara,
                                        sizeof(IMSA_SEL_SDF_PARA_STRU),
                                        0x0,
                                        sizeof(IMSA_SEL_SDF_PARA_STRU));
        pstNormalConn->ulSipSignalPdpNum= 0;
        IMSA_MEM_SET_S(                 pstNormalConn->astSipSignalPdpArray,
                                        sizeof(pstNormalConn->astSipSignalPdpArray),
                                        0x0,
                                        sizeof(IMSA_PDP_CNTXT_INFO_STRU)*IMSA_CONN_MAX_NORMAL_SIP_SIGNAL_PDP_NUM);
        pstNormalConn->ulSipMediaPdpNum = 0;
        IMSA_MEM_SET_S(                 pstNormalConn->astSipMediaPdpArray,
                                        sizeof(pstNormalConn->astSipMediaPdpArray),
                                        0x0,
                                        sizeof(IMSA_PDP_CNTXT_INFO_STRU)*IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM);
    }
}


VOS_VOID IMSA_CONN_ClearConnResourceExeptMedia
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn   = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn      = VOS_NULL_PTR;

    if (IMSA_CONN_TYPE_EMC == enConnType)
    {
        pstEmcConn                      = IMSA_CONN_GetEmcConnAddr();

        IMSA_StopTimer(&pstEmcConn->stProtectTimer);
        pstEmcConn->enCurReqPdnType     = IMSA_IP_TYPE_BUTT;
        pstEmcConn->enFirstReqPdnType   = IMSA_IP_TYPE_BUTT;
        pstEmcConn->enImsaConnStatus    = IMSA_CONN_STATUS_IDLE;
        pstEmcConn->ucOpid              = IMSA_CONN_ILLEGAL_OPID;

        IMSA_MEM_SET_S(                 &pstEmcConn->stSelSdfPara,
                                        sizeof(IMSA_SEL_SDF_PARA_STRU),
                                        0x0,
                                        sizeof(IMSA_SEL_SDF_PARA_STRU));

        IMSA_MEM_SET_S(                 &pstEmcConn->stSipSignalPdp,
                                        sizeof(IMSA_PDP_CNTXT_INFO_STRU),
                                        0x0,
                                        sizeof(IMSA_PDP_CNTXT_INFO_STRU));
    }
    else
    {
        pstNormalConn = IMSA_CONN_GetNormalConnAddr();
        IMSA_StopTimer(&pstNormalConn->stProtectTimer);
        pstNormalConn->enCurReqPdnType  = IMSA_IP_TYPE_BUTT;
        pstNormalConn->enFirstReqPdnType= IMSA_IP_TYPE_BUTT;
        pstNormalConn->enImsaConnStatus = IMSA_CONN_STATUS_IDLE;
        pstNormalConn->ucOpid           = IMSA_CONN_ILLEGAL_OPID;

        IMSA_MEM_SET_S(                 &pstNormalConn->stSelSdfPara,
                                        sizeof(IMSA_SEL_SDF_PARA_STRU),
                                        0x0,
                                        sizeof(IMSA_SEL_SDF_PARA_STRU));
        pstNormalConn->ulSipSignalPdpNum= 0;
        IMSA_MEM_SET_S(                 pstNormalConn->astSipSignalPdpArray,
                                        sizeof(pstNormalConn->astSipSignalPdpArray),
                                        0x0,
                                        sizeof(IMSA_PDP_CNTXT_INFO_STRU)*IMSA_CONN_MAX_NORMAL_SIP_SIGNAL_PDP_NUM);
    }
}



VOS_VOID IMSA_CONN_ClearHoEvent(VOS_VOID)
{
    IMSA_CONN_HO_EVENT_STRU *pstImsaHoEvent = VOS_NULL;

    pstImsaHoEvent = IMSA_CONN_GetHoEventAddr();


    IMSA_MEM_SET_S(                   pstImsaHoEvent,
                                  sizeof(IMSA_CONN_HO_EVENT_STRU),
                                    0x0,
                                    sizeof(IMSA_CONN_HO_EVENT_STRU));

    pstImsaHoEvent->enPdnHoState               = IMSA_CONN_PDN_HO_STATE_IDLE;
    pstImsaHoEvent->enPdnHoType                = IMSA_IMS_HO_TYPE_NO_HO;
    pstImsaHoEvent->enNewPdnConnType           = IMSA_IMS_DOMAIN_NULL;
    pstImsaHoEvent->ucOriginalDomainPdnRlsFlag = VOS_TRUE;
    pstImsaHoEvent->ucNewSignalPdnSetupEvent   = VOS_FALSE;
    pstImsaHoEvent->ucOldPdnRelIndEvent        = VOS_FALSE;
    pstImsaHoEvent->ucNewVideoPdnSetupEvent    = VOS_FALSE;
    pstImsaHoEvent->ucNewVoicePdnSetupEvent    = VOS_FALSE;
    pstImsaHoEvent->ulSipSignalPdpNum          = 0;


}


VOS_VOID IMSA_CONN_HoConnFail
(
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType,
    IMSA_CONN_RESULT_ENUM_UINT32            enResult,
    IMSA_DOMAIN_SWITCH_TYPE_ENUM_UINT8      enHoType,
    IMSA_IMS_DOMAIN_ENUM_UINT8               enConnRatType
)
{
    IMSA_CONN_HO_EVENT_STRU   *pstImsaHoEvent = IMSA_CONN_GetHoEventAddr();
    VOS_UINT8                  i               = IMSA_NULL;
    VOS_UINT8                  ucOrgPdnRlsFlag = VOS_FALSE;  /* 指示原域的PDN是否被释放: VOS_TRUE, 释放; VOS_FALSE: 没有被释放 */

    /* 切换失败,原域不存在,直接释放CDS */
    if( IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH == enHoType)
    {
        /* 给CDS配置下行过滤承载 */
        IMSA_CONN_SndCdsSetImsBearerReq(IMSA_CDS_IMS_BEARER_RAT_TYPE_3GPP_EUTRAN);

        if( IMSA_CONN_STATUS_IDLE == IMSA_CONN_GetNormalConnStatus())
        {
            if( pstImsaHoEvent->ulSipSignalPdpNum > IMSA_CONN_MAX_NORMAL_SIP_SIGNAL_PDP_NUM)
            {
                pstImsaHoEvent->ulSipSignalPdpNum = IMSA_CONN_MAX_NORMAL_SIP_SIGNAL_PDP_NUM;
            }

            for( i=0; i<pstImsaHoEvent->ulSipSignalPdpNum; i++)
            {
                IMSA_CONN_DeletePdpInfo2Reg(enConnType, &pstImsaHoEvent->u.astSipSignalPdpArray[i]);
            }
            ucOrgPdnRlsFlag = VOS_TRUE;
        }
    }
    else
    {
        /* 给CDS配置下行过滤承载 */
        IMSA_CONN_SndCdsSetImsBearerReq(IMSA_CDS_IMS_BEARER_RAT_TYPE_IEEE_802_11);
        if( IMSA_CONN_STATUS_IDLE == IMSA_CONN_GetWifiNormalConnStatus())
        {
            IMSA_CONN_DeleteWifiPdpInfo2Reg(enConnType,&pstImsaHoEvent->u.stSipSignalPdp);
            ucOrgPdnRlsFlag = VOS_TRUE;
        }
    }


    /* 更新连接状态为IDLE */
    IMSA_CONN_SetConnStatus(enConnType, IMSA_CONN_STATUS_IDLE, enConnRatType);

    /* 清除切换事件结构信息 */
    IMSA_CONN_ClearHoEvent();

    /* 清除连接实体信息 */
    IMSA_CONN_ClearRatResource(enConnType,enConnRatType);

    /* 发送切换失败消息 */
    IMSA_CONN_SndConnHoCnf(enResult,
                           enConnType,
                           enHoType,
                           ucOrgPdnRlsFlag);
}




VOS_VOID IMSA_CONN_HoConnSucc(
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType,
    IMSA_DOMAIN_SWITCH_TYPE_ENUM_UINT8      enHoType
    )
{
    if( IMSA_CONN_TYPE_NORMAL != enConnType)
    {
        // TODO
        return;
    }

    if(  IMSA_DOMAIN_SWITCH_TYPE_W2L_SWITCH == enHoType )
    {

        /* 给REG模块更新配置地址对 */
        (VOS_VOID)IMSA_CONN_ProcIpAddrPcscfInW2LHO(enConnType);

        /* 发送切换成功消息 */
        /*just for add the fourth parameter*/
        IMSA_CONN_SndConnHoCnf(IMSA_CONN_RESULT_SUCCESS, enConnType, enHoType, VOS_FALSE);

        /* 清除切换资源 */
        IMSA_CONN_ClearHoEvent();
    }
    else if( IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH == enHoType  )
    {
        /* 给REG模块更新配置地址对 */
        (VOS_VOID)IMSA_CONN_ProcIpAddrPcscfInL2WHO(enConnType);

        /* 给REG模块配置AP MAC地址 */
        IMSA_CONN_ConfigApMacInfo2Reg(&IMSA_CONN_GetWifiNormalConnAddr()->stSipSignalPdp);

        /* 给CDS配置下行过滤承载 */
        IMSA_CONN_SndCdsSetImsBearerReq(IMSA_CDS_IMS_BEARER_RAT_TYPE_IEEE_802_11);

        /*上报给service模块切换成功  */
        /*just for add the fourth parameter*/
        IMSA_CONN_SndConnHoCnf(IMSA_CONN_RESULT_SUCCESS, enConnType,enHoType, VOS_FALSE);
        /* 清除切换事件结构信息 */
        IMSA_CONN_ClearHoEvent();
    }
    else
    {
        // Abnormal
    }

    return;
}


VOS_VOID IMSA_CONN_ClearWifiConnResource
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType
)
{
    IMSA_WIFI_NORMAL_CONN_STRU              *pstNormalConn   = VOS_NULL_PTR;

    if (IMSA_CONN_TYPE_EMC == enConnType)
    {
        /* TODO */
    }
    else
    {
        pstNormalConn = IMSA_CONN_GetWifiNormalConnAddr();

        pstNormalConn->enImsaConnStatus = IMSA_CONN_STATUS_IDLE;

        IMSA_StopTimer(&pstNormalConn->stWaitActiveCnf);
        IMSA_StopTimer(&pstNormalConn->stWaitDeactiveCnf);
        IMSA_StopTimer(&pstNormalConn->stWaitReadyInd);

        IMSA_MEM_SET_S( &pstNormalConn->stSipSignalPdp,
                      sizeof(IMSA_WIFI_PDP_CNTXT_INFO_STRU),
                      0x0,
                      sizeof(IMSA_WIFI_PDP_CNTXT_INFO_STRU));
    }
}



VOS_UINT32 IMSA_CONN_HasActiveSipSignalPdp
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn   = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn      = VOS_NULL_PTR;

    if (IMSA_CONN_TYPE_EMC == enConnType)
    {
        pstEmcConn  = IMSA_CONN_GetEmcConnAddr();

        if (IMSA_PDP_STATE_ACTIVE == pstEmcConn->stSipSignalPdp.enPdpState)
        {
            return IMSA_TRUE;
        }
        else
        {
            return IMSA_FALSE;
        }
    }
    else
    {
        pstNormalConn = IMSA_CONN_GetNormalConnAddr();

        if (0 != pstNormalConn->ulSipSignalPdpNum)
        {
            return IMSA_TRUE;
        }
        else
        {
            return IMSA_FALSE;
        }
    }
}

VOS_UINT32 IMSA_CONN_HasActiveVoicePdp
(
    VOS_VOID
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn   = VOS_NULL_PTR;
    VOS_UINT32                          ulSipMediaPdpNum = IMSA_NULL;
    IMSA_ENTITY_STRU                   *pstImsaEntity = IMSA_CtxGet();
    VOS_UINT32                          ulPfNum = IMSA_NULL;
    VOS_UINT16                          usImsMinPort;
    VOS_UINT16                          usImsMaxPort;

    pstNormalConn = IMSA_CONN_GetNormalConnAddr();

    usImsMinPort = pstImsaEntity->stImsaControlManager.stImsaConfigPara.stImsPortConfig.usImsMinPort;
    usImsMaxPort = pstImsaEntity->stImsaControlManager.stImsaConfigPara.stImsPortConfig.usImsMaxPort;

    for (ulSipMediaPdpNum = 0; ulSipMediaPdpNum < pstNormalConn->ulSipMediaPdpNum; ulSipMediaPdpNum ++)
    {
        for (ulPfNum = 0; ulPfNum < pstNormalConn->astSipMediaPdpArray[ulSipMediaPdpNum].stTft.ulPfNum; ulPfNum++)
        {
            /* 如果存在多个下行过滤器，只需要判断一个过滤器的本地端口 */
            if ((IMSA_OP_TRUE == pstNormalConn->astSipMediaPdpArray[ulSipMediaPdpNum].stTft.astTftInfo[ulPfNum].bitOpSingleLocalPort) &&
                (pstNormalConn->astSipMediaPdpArray[ulSipMediaPdpNum].stTft.astTftInfo[ulPfNum].usSingleLcPort >= usImsMinPort)&&
                (pstNormalConn->astSipMediaPdpArray[ulSipMediaPdpNum].stTft.astTftInfo[ulPfNum].usSingleLcPort <= usImsMaxPort))
            {
                return IMSA_TRUE;
            }

            if ((IMSA_OP_TRUE == pstNormalConn->astSipMediaPdpArray[ulSipMediaPdpNum].stTft.astTftInfo[ulPfNum].bitOpLocalPortRange) &&
                (pstNormalConn->astSipMediaPdpArray[ulSipMediaPdpNum].stTft.astTftInfo[ulPfNum].usLcPortLowLimit >= usImsMinPort) &&
                (pstNormalConn->astSipMediaPdpArray[ulSipMediaPdpNum].stTft.astTftInfo[ulPfNum].usLcPortHighLimit <= usImsMaxPort))
            {
                return IMSA_TRUE;
            }


        }
        if (IMSA_VOICE_QCI == pstNormalConn->astSipMediaPdpArray[ulSipMediaPdpNum].stEpsQos.ucQCI)
        {
            return IMSA_TRUE;
        }
    }
    return IMSA_FALSE;
}

VOS_UINT32 IMSA_CONN_HasActiveVideoPdp
(
    VOS_VOID
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn   = VOS_NULL_PTR;
    VOS_UINT32                          ulSipMediaPdpNum = IMSA_NULL;
    IMSA_ENTITY_STRU                   *pstImsaEntity = IMSA_CtxGet();
    VOS_UINT32                          ulPfNum = IMSA_NULL;
    VOS_UINT16                          usImsMinPort;
    VOS_UINT16                          usImsMaxPort;

    pstNormalConn = IMSA_CONN_GetNormalConnAddr();

    usImsMinPort = pstImsaEntity->stImsaControlManager.stImsaConfigPara.stImsPortConfig.usImsMinPort;
    usImsMaxPort = pstImsaEntity->stImsaControlManager.stImsaConfigPara.stImsPortConfig.usImsMaxPort;

    for (ulSipMediaPdpNum = 0; ulSipMediaPdpNum < pstNormalConn->ulSipMediaPdpNum; ulSipMediaPdpNum ++)
    {
        for (ulPfNum = 0; ulPfNum < pstNormalConn->astSipMediaPdpArray[ulSipMediaPdpNum].stTft.ulPfNum; ulPfNum++)
        {
            if ((IMSA_OP_TRUE == pstNormalConn->astSipMediaPdpArray[ulSipMediaPdpNum].stTft.astTftInfo[ulPfNum].bitOpSingleLocalPort) &&
                ((pstNormalConn->astSipMediaPdpArray[ulSipMediaPdpNum].stTft.astTftInfo[ulPfNum].usSingleLcPort < usImsMinPort)  ||
                 (pstNormalConn->astSipMediaPdpArray[ulSipMediaPdpNum].stTft.astTftInfo[ulPfNum].usSingleLcPort > usImsMaxPort)))
            {
                return IMSA_TRUE;
            }

            if ((IMSA_OP_TRUE == pstNormalConn->astSipMediaPdpArray[ulSipMediaPdpNum].stTft.astTftInfo[ulPfNum].bitOpLocalPortRange) &&
                (pstNormalConn->astSipMediaPdpArray[ulSipMediaPdpNum].stTft.astTftInfo[ulPfNum].usLcPortLowLimit < usImsMinPort) &&
                (pstNormalConn->astSipMediaPdpArray[ulSipMediaPdpNum].stTft.astTftInfo[ulPfNum].usLcPortHighLimit > usImsMaxPort))
            {
                return IMSA_TRUE;
            }


        }
        if (IMSA_VIDEO_QCI == pstNormalConn->astSipMediaPdpArray[ulSipMediaPdpNum].stEpsQos.ucQCI)
        {
            return IMSA_TRUE;
        }
    }
    return IMSA_FALSE;
}


VOS_UINT32 IMSA_CONN_HasEmcActiveVoicePdp
(
    VOS_VOID
)
{
    IMSA_EMC_CONN_STRU                 *pstEmcConn          = VOS_NULL_PTR;
    VOS_UINT32                          ulSipMediaPdpNum    = IMSA_NULL;
    IMSA_ENTITY_STRU                   *pstImsaEntity       = IMSA_CtxGet();
    VOS_UINT32                          ulPfNum             = IMSA_NULL;
    VOS_UINT16                          usImsMinPort;
    VOS_UINT16                          usImsMaxPort;

    pstEmcConn = IMSA_CONN_GetEmcConnAddr();

    usImsMinPort = pstImsaEntity->stImsaControlManager.stImsaConfigPara.stImsPortConfig.usImsMinPort;
    usImsMaxPort = pstImsaEntity->stImsaControlManager.stImsaConfigPara.stImsPortConfig.usImsMaxPort;

    for (ulSipMediaPdpNum = 0; ulSipMediaPdpNum < pstEmcConn->ulSipMediaPdpNum; ulSipMediaPdpNum ++)
    {
        for (ulPfNum = 0; ulPfNum < pstEmcConn->astSipMediaPdpArray[ulSipMediaPdpNum].stTft.ulPfNum; ulPfNum++)
        {
            /* 如果存在多个下行过滤器，只需要判断一个过滤器的本地端口 */
            if ((IMSA_OP_TRUE == pstEmcConn->astSipMediaPdpArray[ulSipMediaPdpNum].stTft.astTftInfo[ulPfNum].bitOpSingleLocalPort) &&
                (pstEmcConn->astSipMediaPdpArray[ulSipMediaPdpNum].stTft.astTftInfo[ulPfNum].usSingleLcPort >= usImsMinPort)&&
                (pstEmcConn->astSipMediaPdpArray[ulSipMediaPdpNum].stTft.astTftInfo[ulPfNum].usSingleLcPort <= usImsMaxPort))
            {
                return IMSA_TRUE;
            }

            if ((IMSA_OP_TRUE == pstEmcConn->astSipMediaPdpArray[ulSipMediaPdpNum].stTft.astTftInfo[ulPfNum].bitOpLocalPortRange) &&
                (pstEmcConn->astSipMediaPdpArray[ulSipMediaPdpNum].stTft.astTftInfo[ulPfNum].usLcPortLowLimit >= usImsMinPort) &&
                (pstEmcConn->astSipMediaPdpArray[ulSipMediaPdpNum].stTft.astTftInfo[ulPfNum].usLcPortHighLimit <= usImsMaxPort))
            {
                return IMSA_TRUE;
            }

            if (IMSA_VOICE_QCI == pstEmcConn->astSipMediaPdpArray[ulSipMediaPdpNum].stEpsQos.ucQCI)
            {
                return IMSA_TRUE;
            }
        }
    }
    return IMSA_FALSE;
}


VOS_UINT32 IMSA_CONN_GetPdpContextByPdpId
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    VOS_UINT8                           ucPdpId,
    IMSA_CONN_SIP_PDP_TYPE_ENUM_UINT32 *penSipPdpType,
    IMSA_PDP_CNTXT_INFO_STRU          **ppstPdpContext
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn   = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn      = VOS_NULL_PTR;
    VOS_UINT32                          i               = IMSA_NULL;

    if (IMSA_CONN_TYPE_EMC == enConnType)
    {
        pstEmcConn          = IMSA_CONN_GetEmcConnAddr();

        if ((IMSA_PDP_STATE_ACTIVE == pstEmcConn->stSipSignalPdp.enPdpState)
            && ((ucPdpId == pstEmcConn->stSipSignalPdp.ucPdpId)))
        {
            *ppstPdpContext = &pstEmcConn->stSipSignalPdp;
            *penSipPdpType  = IMSA_CONN_SIP_PDP_TYPE_SIGNAL;

            return IMSA_SUCC;
        }

        for (i = 0; i < pstEmcConn->ulSipMediaPdpNum; i++)
        {
            if ((ucPdpId == pstEmcConn->astSipMediaPdpArray[i].ucPdpId)
                && (IMSA_PDP_STATE_ACTIVE == pstEmcConn->astSipMediaPdpArray[i].enPdpState))
            {
                *ppstPdpContext = &pstEmcConn->astSipMediaPdpArray[i];
                *penSipPdpType  = IMSA_CONN_SIP_PDP_TYPE_MEDIA;
                return IMSA_SUCC;
            }
        }

        return IMSA_FAIL;
    }

    pstNormalConn = IMSA_CONN_GetNormalConnAddr();

    for (i = 0; i < pstNormalConn->ulSipSignalPdpNum; i++)
    {
        if ((ucPdpId == pstNormalConn->astSipSignalPdpArray[i].ucPdpId)
            && (IMSA_PDP_STATE_ACTIVE == pstNormalConn->astSipSignalPdpArray[i].enPdpState))
        {
            *ppstPdpContext = &pstNormalConn->astSipSignalPdpArray[i];
            *penSipPdpType  = IMSA_CONN_SIP_PDP_TYPE_SIGNAL;
            return IMSA_SUCC;
        }
    }

    for (i = 0; i < pstNormalConn->ulSipMediaPdpNum; i++)
    {
        if ((ucPdpId == pstNormalConn->astSipMediaPdpArray[i].ucPdpId)
            && (IMSA_PDP_STATE_ACTIVE == pstNormalConn->astSipMediaPdpArray[i].enPdpState))
        {
            *ppstPdpContext = &pstNormalConn->astSipMediaPdpArray[i];
            *penSipPdpType  = IMSA_CONN_SIP_PDP_TYPE_MEDIA;
            return IMSA_SUCC;
        }
    }

    return IMSA_FAIL;
}


VOS_UINT32 IMSA_CONN_GetPdpContextByCid
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    VOS_UINT8                           ucCid,
    IMSA_CONN_SIP_PDP_TYPE_ENUM_UINT32 *penSipPdpType,
    IMSA_PDP_CNTXT_INFO_STRU          **ppstPdpContext
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn   = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn      = VOS_NULL_PTR;
    VOS_UINT32                          i               = IMSA_NULL;

    if (IMSA_CONN_TYPE_EMC == enConnType)
    {
        pstEmcConn          = IMSA_CONN_GetEmcConnAddr();

        if ((IMSA_PDP_STATE_ACTIVE == pstEmcConn->stSipSignalPdp.enPdpState)
            && ((ucCid == pstEmcConn->stSipSignalPdp.ucCid)))
        {
            *ppstPdpContext = &pstEmcConn->stSipSignalPdp;
            *penSipPdpType  = IMSA_CONN_SIP_PDP_TYPE_SIGNAL;

            return IMSA_SUCC;
        }

        for (i = 0; i < pstEmcConn->ulSipMediaPdpNum; i++)
        {
            if ((ucCid == pstEmcConn->astSipMediaPdpArray[i].ucCid)
                && (IMSA_PDP_STATE_ACTIVE == pstEmcConn->astSipMediaPdpArray[i].enPdpState))
            {
                *ppstPdpContext = &pstEmcConn->astSipMediaPdpArray[i];
                *penSipPdpType  = IMSA_CONN_SIP_PDP_TYPE_MEDIA;
                return IMSA_SUCC;
            }
        }

        return IMSA_FAIL;
    }

    pstNormalConn = IMSA_CONN_GetNormalConnAddr();

    for (i = 0; i < pstNormalConn->ulSipSignalPdpNum; i++)
    {
        if ((ucCid == pstNormalConn->astSipSignalPdpArray[i].ucCid)
            && (IMSA_PDP_STATE_ACTIVE == pstNormalConn->astSipSignalPdpArray[i].enPdpState))
        {
            *ppstPdpContext = &pstNormalConn->astSipSignalPdpArray[i];
            *penSipPdpType  = IMSA_CONN_SIP_PDP_TYPE_SIGNAL;
            return IMSA_SUCC;
        }
    }

    for (i = 0; i < pstNormalConn->ulSipMediaPdpNum; i++)
    {
        if ((ucCid == pstNormalConn->astSipMediaPdpArray[i].ucCid)
            && (IMSA_PDP_STATE_ACTIVE == pstNormalConn->astSipMediaPdpArray[i].enPdpState))
        {
            *ppstPdpContext = &pstNormalConn->astSipMediaPdpArray[i];
            *penSipPdpType  = IMSA_CONN_SIP_PDP_TYPE_MEDIA;
            return IMSA_SUCC;
        }
    }

    return IMSA_FAIL;
}


VOS_UINT32 IMSA_CONN_GetPdpContextByCidOrPdpId
(
    VOS_UINT8                                   ucCid,
    VOS_UINT8                                   ucPdpId,
    IMSA_CONN_SIP_PDP_TYPE_ENUM_UINT32          *penSipPdpType,
    IMSA_PDP_CNTXT_INFO_STRU                    **ppstPdpContext,
    IMSA_CONN_TYPE_ENUM_UINT32                  *penConnType
)
{

    IMSA_INFO_LOG("IMSA_CONN_GetPdpContextByCidOrPdpId is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_GetPdpContextByCidOrPdpId_ENUM, LNAS_ENTRY);
    TLPS_PRINT2LAYER_INFO1(IMSA_CONN_ProcTafPsEvtPdpDeactivateInd_ENUM, IMSA_CID, ucCid);
    TLPS_PRINT2LAYER_INFO1(IMSA_CONN_ProcTafPsEvtPdpDeactivateInd_ENUM, IMSA_PDPID, ucPdpId);

    /* 通过CID获取普通承载上下文 */
    if (IMSA_SUCC == IMSA_CONN_GetPdpContextByCid(IMSA_CONN_TYPE_NORMAL,
                                                  ucCid,
                                                  penSipPdpType,
                                                  ppstPdpContext))
    {
        IMSA_INFO_LOG("IMSA_CONN_GetPdpContextByCidOrPdpId:get normal pdp context by cid!");
        TLPS_PRINT2LAYER_INFO(IMSA_CONN_GetPdpContextByCidOrPdpId_ENUM, 1);
        *penConnType = IMSA_CONN_TYPE_NORMAL;
        return IMSA_SUCC;
    }

    /* 通过承载ID获取普通承载上下文 */
    if (IMSA_SUCC == IMSA_CONN_GetPdpContextByPdpId(IMSA_CONN_TYPE_NORMAL,
                                                    ucPdpId,
                                                    penSipPdpType,
                                                    ppstPdpContext))
    {
        IMSA_INFO_LOG("IMSA_CONN_GetPdpContextByCidOrPdpId:get normal pdp context by pdpid!");
        TLPS_PRINT2LAYER_INFO(IMSA_CONN_GetPdpContextByCidOrPdpId_ENUM, 2);
        *penConnType = IMSA_CONN_TYPE_NORMAL;
        return IMSA_SUCC;
    }

    /* 通过CID获取紧急承载上下文 */
    if (IMSA_SUCC == IMSA_CONN_GetPdpContextByCid(IMSA_CONN_TYPE_EMC,
                                                  ucCid,
                                                  penSipPdpType,
                                                  ppstPdpContext))
    {
        IMSA_INFO_LOG("IMSA_CONN_GetPdpContextByCidOrPdpId:get emc pdp context by cid!");
        TLPS_PRINT2LAYER_INFO(IMSA_CONN_GetPdpContextByCidOrPdpId_ENUM, 3);
        *penConnType = IMSA_CONN_TYPE_EMC;
        return IMSA_SUCC;
    }

    /* 通过承载ID获取紧急承载上下文 */
    if (IMSA_SUCC == IMSA_CONN_GetPdpContextByPdpId(IMSA_CONN_TYPE_EMC,
                                                    ucPdpId,
                                                    penSipPdpType,
                                                    ppstPdpContext))
    {
        IMSA_INFO_LOG("IMSA_CONN_GetPdpContextByCidOrPdpId:get emc pdp context by pdpid!");
        TLPS_PRINT2LAYER_INFO(IMSA_CONN_GetPdpContextByCidOrPdpId_ENUM, 4);
        *penConnType = IMSA_CONN_TYPE_EMC;
        return IMSA_SUCC;
    }

    return IMSA_FAIL;
}



VOS_UINT32 IMSA_CONN_GetTimerLen
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_TIMER_ID_ENUM_UINT16           enTimerId
)
{
    if (TI_IMSA_SIP_SIGAL_PDP_ORIG == enTimerId)
    {
        if (IMSA_CONN_TYPE_NORMAL == enConnType)
        {
            return TI_IMSA_NRM_SIP_SIGAL_PDP_ORIG_TIMER_LEN;
        }
        else
        {
            return TI_IMSA_EMC_SIP_SIGAL_PDP_ORIG_TIMER_LEN;
        }
    }

    if (TI_IMSA_SIP_SIGAL_PDP_END == enTimerId)
    {
        return TI_IMSA_SIP_SIGAL_PDP_END_TIMER_LEN;
    }

    if (TI_IMSA_WAIT_IPV6_INFO == enTimerId)
    {
        return TI_IMSA_WAIT_IPV6_INFO_TIMER_LEN;
    }

    if (TI_IMSA_WAIT_WIFI_PDP_ACTIVE_CNF == enTimerId)
    {
        return TI_IMSA_WAIT_WIFI_PDP_ACTIVE_CNF_LEN;
    }

    if (TI_IMSA_WAIT_WIFI_PDP_READY_IND == enTimerId)
    {
        return TI_IMSA_WAIT_WIFI_PDP_READY_IND_LEN;
    }

    if (TI_IMSA_WAIT_WIFI_PDP_DEACTIVE_CNF == enTimerId)
    {
        return TI_IMSA_WAIT_WIFI_PDP_DEACTIVE_CNF_LEN;
    }

    return 0;
}


VOS_VOID IMSA_CONN_StartTimer
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_TIMER_ID_ENUM_UINT16           enTimerId
)
{
    VOS_UINT32                          ulTimerLen      = IMSA_NULL;
    IMSA_TIMER_STRU                    *pstProtectImer  = VOS_NULL_PTR;

    /* 获取定时器时长 */
    ulTimerLen                  = IMSA_CONN_GetTimerLen(enConnType, enTimerId);

    IMSA_INFO_LOG2("IMSA_CONN_StartTimer:enConnType:Timer Len:", enConnType, ulTimerLen);
    TLPS_PRINT2LAYER_INFO2(IMSA_CONN_StartTimer_ENUM, 1, enConnType, ulTimerLen);

    if (IMSA_CONN_TYPE_NORMAL   == enConnType)
    {
        pstProtectImer          = IMSA_CONN_GetNormalConnProtectTimerAddr();

    }
    else
    {
        pstProtectImer          = IMSA_CONN_GetEmcConnProtectTimerAddr();
    }

    /* 赋值定时器参数 */
    pstProtectImer->ulTimerLen  = ulTimerLen;
    pstProtectImer->usName      = enTimerId;
    pstProtectImer->usPara      = (VOS_UINT16)enConnType;
    pstProtectImer->ucMode      = VOS_RELTIMER_NOLOOP;

    IMSA_StartTimer(pstProtectImer);
}


VOS_VOID IMSA_CONN_StopTimer
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_TIMER_ID_ENUM_UINT16           enTimerId
)
{
    IMSA_TIMER_STRU                    *pstProtectImer  = VOS_NULL_PTR;

    if (IMSA_CONN_TYPE_NORMAL   == enConnType)
    {
        pstProtectImer          = IMSA_CONN_GetNormalConnProtectTimerAddr();

    }
    else
    {
        pstProtectImer          = IMSA_CONN_GetEmcConnProtectTimerAddr();
    }

    if (pstProtectImer->usName == enTimerId)
    {
        IMSA_StopTimer(pstProtectImer);
    }
}

VOS_VOID IMSA_CONN_SetWifiConnStatus
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_CONN_STATUS_ENUM_UINT8         enConnStatus
)
{
    /* 获取连接原状态 */

    /* 打印连接类型 */

    /* 打印连接状态变更新信息 */

    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        IMSA_CONN_GetWifiNormalConnStatus() = enConnStatus;
    }
    else
    {
        /* todo */
    }
}


VOS_VOID IMSA_CONN_SetConnStatus
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_CONN_STATUS_ENUM_UINT8         enConnStatus,
    IMSA_IMS_DOMAIN_ENUM_UINT8           enConnRatType
)
{
    /* 获取连接原状态 */

    /* 打印连接类型 */

    /* 打印连接状态变更新信息 */
    if (IMSA_IMS_DOMAIN_WIFI == enConnRatType)
    {
        if (IMSA_CONN_TYPE_NORMAL == enConnType)
        {
            IMSA_CONN_GetWifiNormalConnStatus() = enConnStatus;
        }
        else
        {
        }
    }
    else if (IMSA_IMS_DOMAIN_LTE == enConnRatType)
    {
        if (IMSA_CONN_TYPE_NORMAL == enConnType)
        {
            IMSA_CONN_GetNormalConnStatus() = enConnStatus;
        }
        else
        {
            IMSA_CONN_GetEmcConnStatus()    = enConnStatus;
        }
    }
    else
    {
    }


}


VOS_VOID IMSA_CONN_SaveFirstReqPdnType
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_IP_TYPE_ENUM_UINT8             enIpType
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn       = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn          = VOS_NULL_PTR;

    /* 获取PRIM PDP参数的格式 */
    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        pstNormalConn                   = IMSA_CONN_GetNormalConnAddr();
        pstNormalConn->enFirstReqPdnType= enIpType;
    }
    else
    {
        pstEmcConn                      = IMSA_CONN_GetEmcConnAddr();
        pstEmcConn->enFirstReqPdnType   = enIpType;
    }
}



VOS_VOID IMSA_CONN_SaveCurReqPdnType
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_IP_TYPE_ENUM_UINT8             enIpType
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn       = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn          = VOS_NULL_PTR;

    /* 获取PRIM PDP参数的格式 */
    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        pstNormalConn                   = IMSA_CONN_GetNormalConnAddr();
        pstNormalConn->enCurReqPdnType  = enIpType;
    }
    else
    {
        pstEmcConn                      = IMSA_CONN_GetEmcConnAddr();
        pstEmcConn->enCurReqPdnType     = enIpType;
    }
}


VOS_UINT32 IMSA_CONN_IsImsSdf
(
     const TAF_SDF_PARA_STRU            *pstSdfPara
)
{
    if (((IMSA_OP_TRUE == pstSdfPara->bitOpImCnSignalFlg)
       && (TAF_PDP_FOR_IM_CN_SIG_ONLY == pstSdfPara->enImCnSignalFlg))
       ||((IMSA_OP_TRUE == pstSdfPara->bitOpImsSuppFlg)
       && (IMSA_TRUE == pstSdfPara->ucImsSuppFlg)))
    {
        return IMSA_TRUE;
    }

    return IMSA_FALSE;
}


VOS_UINT32 IMSA_CONN_IsPndTypeValid
(
     const TAF_SDF_PARA_STRU            *pstSdfPara
)
{
    if ((IMSA_OP_TRUE == pstSdfPara->bitOpPdnType)
       && (TAF_PDP_IPV4 <= pstSdfPara->enPdnType)
       && (TAF_PDP_IPV4V6 >= pstSdfPara->enPdnType))
    {
        return IMSA_TRUE;
    }

    return IMSA_FALSE;
}


VOS_UINT32 IMSA_CONN_IsEmcSdf
(
     const TAF_SDF_PARA_STRU            *pstSdfPara
)
{
    if ((IMSA_OP_TRUE == pstSdfPara->bitOpEmergencyInd)
       && (TAF_PDP_FOR_EMC == pstSdfPara->enEmergencyInd))
    {
        return IMSA_TRUE;
    }

    return IMSA_FALSE;
}


VOS_UINT32 IMSA_CONN_FindSipSignalReStablishDialPara
(
    VOS_UINT8                           ucCid,
    VOS_UINT32                          ulSdfNum,
    const TAF_SDF_PARA_STRU            *pstSdfPara,
    VOS_UINT32                         *pulIndex
)
{
    VOS_UINT32                          i = IMSA_NULL;

    if (ulSdfNum == 0)
    {
        IMSA_ERR_LOG("IMSA_CONN_FindSipSignalReStablishDialPara:No Sdf!");
        TLPS_PRINT2LAYER_ERROR(IMSA_CONN_FindSipSignalReStablishDialPara_ENUM, 1);
        return IMSA_FAIL;
    }

    for (i = 0; i < ulSdfNum; i++)
    {
        if (ucCid == pstSdfPara[i].ucCid)
        {
            *pulIndex = i;

            return IMSA_SUCC;
        }
    }
    return IMSA_FAIL;
}



VOS_UINT32 IMSA_CONN_FindSipSignalDialPara
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    VOS_UINT32                          ulSdfNum,
    const TAF_SDF_PARA_STRU            *pstSdfPara,
    VOS_UINT32                         *pulIndex
)
{
    VOS_UINT32                          i = IMSA_NULL;

    if (ulSdfNum == 0)
    {
        IMSA_ERR_LOG("IMSA_CONN_FindSipSignalDialPara:No Sdf!");
        TLPS_PRINT2LAYER_ERROR(IMSA_CONN_FindSipSignalDialPara_ENUM, 1);
        return IMSA_FAIL;
    }

    /* 普通连接选择具有TAF_PDP_FOR_IMS_CN_SIG_ONLY标识，PDN类型和APN参数，
       且参数合法有效的CID；紧急连接选择具有TAF_PDP_FOR_IMS_CN_SIG_ONLY标识
       和TAF_PDP_FOR_EMC，PDN类型和APN参数，且参数合法有效的CID */
    for (i = 0; i < ulSdfNum; i++)
    {
        if (IMSA_OP_TRUE == pstSdfPara[i].bitOpLinkdCid)
        {
            continue;
        }

        if (IMSA_TRUE != IMSA_CONN_IsImsSdf(&pstSdfPara[i]))
        {
            continue;
        }

        if (IMSA_TRUE != IMSA_CONN_IsPndTypeValid(&pstSdfPara[i]))
        {
            continue;
        }

        if (IMSA_CONN_TYPE_EMC == enConnType)
        {
            if (IMSA_TRUE != IMSA_CONN_IsEmcSdf(&pstSdfPara[i]))
            {
                continue;
            }
        }
        else/* 如果是普通类型，则不选择有紧急标识的CID */
        {
            if (IMSA_OP_TRUE != pstSdfPara[i].bitOpApn)
            {
                continue;
            }

            if (IMSA_TRUE == IMSA_CONN_IsEmcSdf(&pstSdfPara[i]))
            {
                continue;
            }
        }

        *pulIndex = i;

        return IMSA_SUCC;
    }

    return IMSA_FAIL;
}

/*****************************************************************************
 Function Name  : IMSA_CONN_SaveSelectedWifiSdfPara
 Description    : 存储选中的SDF参数
 Input          : enConnType--------------------连接类型
 Output         : pstTafSdfPara-----------------TAF SDF参数指针
 Return Value   : VOS_UINT32

 History        :
      1.zhangdongfeng      2015-12-27  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_CONN_SaveSelectedWifiSdfPara
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    const TAF_SDF_PARA_STRU            *pstTafSdfPara
)
{
    IMSA_SEL_SDF_PARA_STRU             *pstSelSdfPara = VOS_NULL_PTR;

    if (IMSA_CONN_TYPE_EMC == enConnType)
    {
        //To Do
        return;
    }
    else
    {
        pstSelSdfPara = IMSA_CONN_GetWifiNormalConnSelSdfParaAddr();
    }

    IMSA_MEM_SET_S(pstSelSdfPara, sizeof(IMSA_SEL_SDF_PARA_STRU),0x0, sizeof(IMSA_SEL_SDF_PARA_STRU));

    pstSelSdfPara->ucCid                    = pstTafSdfPara->ucCid;

    pstSelSdfPara->bitOpLinkdCid            = pstTafSdfPara->bitOpLinkdCid;
    pstSelSdfPara->ucLinkdCid               = pstTafSdfPara->ucLinkdCid;

    pstSelSdfPara->bitOpPdnType             = pstTafSdfPara->bitOpPdnType;
    pstSelSdfPara->enPdnType                = pstTafSdfPara->enPdnType;

    pstSelSdfPara->bitOpApn = pstTafSdfPara->bitOpApn;
    IMSA_MEM_CPY_S(   &pstSelSdfPara->stApnInfo,
                   sizeof(TAF_PDP_APN_STRU),
                    &pstTafSdfPara->stApnInfo,
                    sizeof(TAF_PDP_APN_STRU));

    pstSelSdfPara->bitOpGwAuthInfo = pstTafSdfPara->bitOpGwAuthInfo;
    IMSA_MEM_CPY_S(   &pstSelSdfPara->stGwAuthInfo,
                    sizeof(TAF_GW_AUTH_STRU),
                    &pstTafSdfPara->stGwAuthInfo,
                    sizeof(TAF_GW_AUTH_STRU));

    pstSelSdfPara->bitOpEmergencyInd        = pstTafSdfPara->bitOpEmergencyInd;
    pstSelSdfPara->enEmergencyInd           = pstTafSdfPara->enEmergencyInd;

    pstSelSdfPara->bitOpIpv4AddrAllocType   = pstTafSdfPara->bitOpIpv4AddrAllocType;
    pstSelSdfPara->enIpv4AddrAllocType      = pstTafSdfPara->enIpv4AddrAllocType;

    pstSelSdfPara->bitOpPcscfDiscovery      = pstTafSdfPara->bitOpPcscfDiscovery;
    pstSelSdfPara->enPcscfDiscovery         = pstTafSdfPara->enPcscfDiscovery;

    pstSelSdfPara->bitOpImCnSignalFlg       = pstTafSdfPara->bitOpImCnSignalFlg;
    pstSelSdfPara->enImCnSignalFlg          = pstTafSdfPara->enImCnSignalFlg;

    pstSelSdfPara->bitOpImsSuppFlg          = pstTafSdfPara->bitOpImsSuppFlg;
    pstSelSdfPara->ucImsSuppFlg             = pstTafSdfPara->ucImsSuppFlg;
}




VOS_VOID IMSA_CONN_SaveSelectedSdfPara
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    const TAF_SDF_PARA_STRU            *pstTafSdfPara
)
{
    IMSA_SEL_SDF_PARA_STRU             *pstSelSdfPara = VOS_NULL_PTR;

    if (IMSA_CONN_TYPE_EMC == enConnType)
    {
        pstSelSdfPara = IMSA_CONN_GetEmcConnSelSdfParaAddr();
    }
    else
    {
        pstSelSdfPara = IMSA_CONN_GetNormalConnSelSdfParaAddr();
    }

    IMSA_MEM_SET_S( pstSelSdfPara,
                    sizeof(IMSA_SEL_SDF_PARA_STRU),
                    0x0,
                    sizeof(IMSA_SEL_SDF_PARA_STRU));

    pstSelSdfPara->ucCid                    = pstTafSdfPara->ucCid;

    pstSelSdfPara->bitOpLinkdCid            = pstTafSdfPara->bitOpLinkdCid;
    pstSelSdfPara->ucLinkdCid               = pstTafSdfPara->ucLinkdCid;

    pstSelSdfPara->bitOpPdnType             = pstTafSdfPara->bitOpPdnType;
    pstSelSdfPara->enPdnType                = pstTafSdfPara->enPdnType;

    pstSelSdfPara->bitOpApn = pstTafSdfPara->bitOpApn;
    IMSA_MEM_CPY_S( &pstSelSdfPara->stApnInfo,
                    sizeof(TAF_PDP_APN_STRU),
                    &pstTafSdfPara->stApnInfo,
                    sizeof(TAF_PDP_APN_STRU));

    pstSelSdfPara->bitOpGwAuthInfo = pstTafSdfPara->bitOpGwAuthInfo;
    IMSA_MEM_CPY_S( &pstSelSdfPara->stGwAuthInfo,
                    sizeof(TAF_GW_AUTH_STRU),
                    &pstTafSdfPara->stGwAuthInfo,
                    sizeof(TAF_GW_AUTH_STRU));

    pstSelSdfPara->bitOpEmergencyInd        = pstTafSdfPara->bitOpEmergencyInd;
    pstSelSdfPara->enEmergencyInd           = pstTafSdfPara->enEmergencyInd;

    pstSelSdfPara->bitOpIpv4AddrAllocType   = pstTafSdfPara->bitOpIpv4AddrAllocType;
    pstSelSdfPara->enIpv4AddrAllocType      = pstTafSdfPara->enIpv4AddrAllocType;

    pstSelSdfPara->bitOpPcscfDiscovery      = pstTafSdfPara->bitOpPcscfDiscovery;
    pstSelSdfPara->enPcscfDiscovery         = pstTafSdfPara->enPcscfDiscovery;

    pstSelSdfPara->bitOpImCnSignalFlg       = pstTafSdfPara->bitOpImCnSignalFlg;
    pstSelSdfPara->enImCnSignalFlg          = pstTafSdfPara->enImCnSignalFlg;

    pstSelSdfPara->bitOpImsSuppFlg          = pstTafSdfPara->bitOpImsSuppFlg;
    pstSelSdfPara->ucImsSuppFlg             = pstTafSdfPara->ucImsSuppFlg;
}


VOS_VOID IMSA_CONN_GetPrimPdpCntFromSelSdfPara
(
    const IMSA_SEL_SDF_PARA_STRU       *pstSelectedSdfPara,
    TAF_PDP_PRIM_CONTEXT_EXT_STRU      *pstPdpPrimContextExt
)
{
    IMSA_MEM_SET_S(                     (VOS_VOID *)pstPdpPrimContextExt,
                                        sizeof(TAF_PDP_PRIM_CONTEXT_EXT_STRU),
                                        0x00,
                                        sizeof(TAF_PDP_PRIM_CONTEXT_EXT_STRU));

    pstPdpPrimContextExt->ucCid         = pstSelectedSdfPara->ucCid;
    pstPdpPrimContextExt->ucDefined     = 1;

    /* 赋值PDN TYPE */
    pstPdpPrimContextExt->bitOpPdpType  = pstSelectedSdfPara->bitOpPdnType;
    pstPdpPrimContextExt->enPdpType     = pstSelectedSdfPara->enPdnType;

    /* 赋值APN */
    pstPdpPrimContextExt->bitOpApn      = pstSelectedSdfPara->bitOpApn;
    IMSA_MEM_CPY_S(                     (VOS_CHAR*)pstPdpPrimContextExt->aucApn,
                                        sizeof(pstPdpPrimContextExt->aucApn),
                                        (VOS_CHAR*)pstSelectedSdfPara->stApnInfo.aucValue,
                                        pstSelectedSdfPara->stApnInfo.ucLength);

    /* 赋值Ipv4AddrAlloc */
    pstPdpPrimContextExt->bitOpIpv4AddrAlloc    = pstSelectedSdfPara->bitOpIpv4AddrAllocType;
    pstPdpPrimContextExt->enIpv4AddrAlloc       = pstSelectedSdfPara->enIpv4AddrAllocType;

    /* 赋值紧急标识 */
    pstPdpPrimContextExt->bitOpEmergencyInd     = pstSelectedSdfPara->bitOpEmergencyInd;
    pstPdpPrimContextExt->enEmergencyFlg        = pstSelectedSdfPara->enEmergencyInd;

    /* 赋值P-CSCF发现方式 */
    pstPdpPrimContextExt->bitOpPcscfDiscovery   = pstSelectedSdfPara->bitOpPcscfDiscovery;
    pstPdpPrimContextExt->enPcscfDiscovery      = pstSelectedSdfPara->enPcscfDiscovery;

    /* 赋值IMS标识 */
    pstPdpPrimContextExt->bitOpImCnSignalFlg    = pstSelectedSdfPara->bitOpImCnSignalFlg;
    pstPdpPrimContextExt->enImCnSignalFlg       = pstSelectedSdfPara->enImCnSignalFlg;
}



VOS_VOID IMSA_CONN_GetImsDailParaFromSelSdfPara
(
    const IMSA_SEL_SDF_PARA_STRU       *pstSelectedSdfPara,
    TAF_PS_DIAL_PARA_STRU              *pstDialParaInfo
)
{
    IMSA_MEM_SET_S( pstDialParaInfo,
                    sizeof(TAF_PS_DIAL_PARA_STRU),
                    0,
                    sizeof(TAF_PS_DIAL_PARA_STRU));

    pstDialParaInfo->ucCid                  = pstSelectedSdfPara->ucCid;
    pstDialParaInfo->enPdpType              = pstSelectedSdfPara->enPdnType;

    /* 赋值APN信息 */
    if (IMSA_OP_TRUE == pstSelectedSdfPara->bitOpApn)
    {
        pstDialParaInfo->bitOpApn           = IMSA_OP_TRUE;

        /* 拨号APN参数没有长度字段，是因为是字符串格式，已在结尾加上'\0' */
        IMSA_MEM_CPY_S(                     pstDialParaInfo->aucApn,
                                            TAF_MAX_APN_LEN,
                                            pstSelectedSdfPara->stApnInfo.aucValue,
                                            pstSelectedSdfPara->stApnInfo.ucLength);
    }

    /* 赋值鉴权参数 */
    if (IMSA_OP_TRUE == pstSelectedSdfPara->bitOpGwAuthInfo)
    {
        pstDialParaInfo->bitOpAuthType      = IMSA_OP_TRUE;
        pstDialParaInfo->enAuthType         = pstSelectedSdfPara->stGwAuthInfo.enAuthType;

        if (pstSelectedSdfPara->stGwAuthInfo.ucUserNameLen != 0)
        {
            pstDialParaInfo->bitOpUserName  = IMSA_OP_TRUE;

            /* 拨号用户名参数没有长度字段，是因为是字符串格式，已在结尾加上'\0' */
            IMSA_MEM_CPY_S(                 pstDialParaInfo->aucUserName,
                                            TAF_MAX_GW_AUTH_USERNAME_LEN,
                                            pstSelectedSdfPara->stGwAuthInfo.aucUserName,
                                            pstSelectedSdfPara->stGwAuthInfo.ucUserNameLen);
        }

        if (pstSelectedSdfPara->stGwAuthInfo.ucPwdLen       != 0)
        {
            pstDialParaInfo->bitOpPassWord  = IMSA_OP_TRUE;

            /* 拨号密码参数没有长度字段，是因为是字符串格式，已在结尾加上'\0' */
            IMSA_MEM_CPY_S(                 pstDialParaInfo->aucPassWord,
                                            TAF_MAX_GW_AUTH_PASSWORD_LEN,
                                            pstSelectedSdfPara->stGwAuthInfo.aucPwd,
                                            pstSelectedSdfPara->stGwAuthInfo.ucPwdLen);
        }
    }

    if (IMSA_OP_TRUE == pstSelectedSdfPara->bitOpIpv4AddrAllocType)
    {
        pstDialParaInfo->bitOpIpv4AddrAlloc = IMSA_OP_TRUE;
        pstDialParaInfo->enIpv4AddrAlloc = pstSelectedSdfPara->enIpv4AddrAllocType;
    }

    if (IMSA_OP_TRUE == pstSelectedSdfPara->bitOpEmergencyInd)
    {
        pstDialParaInfo->bitOpEmergencyInd = IMSA_OP_TRUE;
        pstDialParaInfo->enEmergencyInd = pstSelectedSdfPara->enEmergencyInd;
    }

    if (IMSA_OP_TRUE == pstSelectedSdfPara->bitOpPcscfDiscovery)
    {
        pstDialParaInfo->bitOpPcscfDiscovery = IMSA_OP_TRUE;
        pstDialParaInfo->enPcscfDiscovery = pstSelectedSdfPara->enPcscfDiscovery;
    }

    if (IMSA_OP_TRUE == pstSelectedSdfPara->bitOpImCnSignalFlg)
    {
        pstDialParaInfo->bitOpImCnSignalFlg = IMSA_OP_TRUE;
        pstDialParaInfo->enImCnSignalFlg = pstSelectedSdfPara->enImCnSignalFlg;
    }

    pstDialParaInfo->bitOpReqType = IMSA_OP_FALSE;
    pstDialParaInfo->bitOpPdpDcomp = IMSA_OP_FALSE;
    pstDialParaInfo->bitOpPdpHcomp = IMSA_OP_FALSE;

}


VOS_VOID IMSA_CONN_ConvertIpAddress2String
(
    IMSA_IP_TYPE_ENUM_UINT8             enIpType,
    const VOS_UINT8                    *pucSrc,
    VOS_CHAR                           *pcDst
)
{
#if (VOS_OS_VER != VOS_WIN32)
#if (VOS_OS_VER == VOS_VXWORKS)
/*lint -e718 -e746 -e40 */
    if (IMSA_IP_TYPE_IPV4 == enIpType)
    {
        inet_ntop(AF_INET, pucSrc , pcDst, 16);
    }
    else
    {
        inet_ntop(AF_INET6, pucSrc , pcDst, 46);
    }
/*lint +e718 +e746 +e40 */
#else
/*lint -e64*/
    if (IMSA_IP_TYPE_IPV4 == enIpType)
    {
        (VOS_VOID)vrp_inet_ntop(VRP_AF_INET, (const CHAR *)pucSrc , pcDst, 16);
    }
    else
    {
        (VOS_VOID)vrp_inet_ntop(VRP_AF_INET6, (const CHAR *)pucSrc , pcDst, 46);
    }
/*lint +e64*/
#endif
#else
    VOS_UINT8                           i = 0;

    if (IMSA_IP_TYPE_IPV4 == enIpType)
    {
        /* ST中的IPV6地址和IPV6类型的P-CSCF地址的最后一个字节必须是[0,9] */
        IMSA_MEM_CPY_S( pcDst,
                        (IMSA_IPV4_ADDR_STRING_LEN + 1),
                        g_acIpStringForSt_ipv4,
                        (IMSA_IPV4_ADDR_STRING_LEN + 1));
        i = strlen(g_acIpStringForSt_ipv4) - 1;
        pcDst[i] = pucSrc[3] + 0x30;
        /*g_acIpStringForSt_ipv4[i] ++ ;*/
    }
    else
    {
        /* ST中的IPV6地址和IPV6类型的P-CSCF地址的最后一个字节必须是大于等于16 */
        IMSA_MEM_CPY_S( pcDst,
                        (IMSA_IPV6_ADDR_STRING_LEN + 1),
                        g_acIpStringForSt_ipv6,
                        (IMSA_IPV6_ADDR_STRING_LEN + 1));

        i = strlen(g_acIpStringForSt_ipv6) - 1;
        pcDst[i] = (pucSrc[15] - 16) + 0x30;
        /*g_acIpStringForSt_ipv6[i] ++ ;*/
    }

#endif
}



VOS_UINT32 IMSA_CONN_FilterRegedPcscfInfo
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_PDP_CNTXT_INFO_STRU           *pstPdpContext
)
{
    VOS_CHAR                            acUeAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};
    VOS_CHAR                            acRegUeAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};
    VOS_CHAR                            acRegPcscfAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};
    VOS_UINT32                          ulRslt          = IMSA_FAIL;

    IMSA_IP_TYPE_ENUM_UINT8             enIpType = IMSA_IP_TYPE_BUTT;

    ulRslt = IMSA_RegGetRegedPara(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                    acRegUeAddr,
                                    acRegPcscfAddr,
                                    &enIpType);

    /* 如果未获取到注册上的参数，则直接返回不包含 */
    if (IMSA_FAIL == ulRslt)
    {
        return IMSA_FALSE;
    }

    if ((IMSA_IP_TYPE_IPV4 == enIpType) || (IMSA_IP_TYPE_IPV4V6 == enIpType))
    {
        if (IMSA_OP_TRUE == pstPdpContext->stPdpIpv4Pcscf.bitOpPrimPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpContext->stPdpIpv4Pcscf.aucPrimPcscfAddr,
                                                acUeAddr);
            if (0 == VOS_StrCmp(acRegPcscfAddr, acUeAddr))
            {
                pstPdpContext->stPdpIpv4Pcscf.bitOpPrimPcscfAddr = IMSA_OP_FALSE;
                return IMSA_TRUE;
            }
        }

        if (IMSA_OP_TRUE == pstPdpContext->stPdpIpv4Pcscf.bitOpSecPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpContext->stPdpIpv4Pcscf.aucSecPcscfAddr,
                                                acUeAddr);
            if (0 == VOS_StrCmp(acRegPcscfAddr, acUeAddr))
            {
                pstPdpContext->stPdpIpv4Pcscf.bitOpSecPcscfAddr = IMSA_OP_FALSE;
                return IMSA_TRUE;
            }
        }

        if (IMSA_OP_TRUE == pstPdpContext->stPdpIpv4Pcscf.bitOpThiPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpContext->stPdpIpv4Pcscf.aucThiPcscfAddr,
                                                acUeAddr);
            if (0 == VOS_StrCmp(acRegPcscfAddr, acUeAddr))
            {
                pstPdpContext->stPdpIpv4Pcscf.bitOpThiPcscfAddr = IMSA_OP_FALSE;
                return IMSA_TRUE;
            }
        }
    }

    if ((IMSA_IP_TYPE_IPV6 == enIpType) || (IMSA_IP_TYPE_IPV4V6 == enIpType))
    {
        if (IMSA_OP_TRUE == pstPdpContext->stPdpIpv6Pcscf.bitOpPrimPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpContext->stPdpIpv6Pcscf.aucPrimPcscfAddr,
                                                acUeAddr);
            if (0 == VOS_StrCmp(acRegPcscfAddr, acUeAddr))
            {
                pstPdpContext->stPdpIpv6Pcscf.bitOpPrimPcscfAddr = IMSA_OP_FALSE;
                return IMSA_TRUE;
            }
        }

        if (IMSA_OP_TRUE == pstPdpContext->stPdpIpv6Pcscf.bitOpSecPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpContext->stPdpIpv6Pcscf.aucSecPcscfAddr,
                                                acUeAddr);
            if (0 == VOS_StrCmp(acRegPcscfAddr, acUeAddr))
            {
                pstPdpContext->stPdpIpv6Pcscf.bitOpSecPcscfAddr = IMSA_OP_FALSE;
                return IMSA_TRUE;
            }
        }

        if (IMSA_OP_TRUE == pstPdpContext->stPdpIpv6Pcscf.bitOpThiPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpContext->stPdpIpv6Pcscf.aucThiPcscfAddr,
                                                acUeAddr);
            if (0 == VOS_StrCmp(acRegPcscfAddr, acUeAddr))
            {
                pstPdpContext->stPdpIpv6Pcscf.bitOpThiPcscfAddr = IMSA_OP_FALSE;
                return IMSA_TRUE;
            }
        }
    }

    return IMSA_FALSE;
}

VOS_VOID IMSA_CONN_DeletePcscfInfo2Reg
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    const IMSA_PDP_CNTXT_INFO_STRU     *pstPdpInfo
)
{
    VOS_CHAR                    acUeAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};

    if ((IMSA_IP_TYPE_IPV4 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {

        /* 删除IPV4 P-CSCF地址 */
        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.bitOpPrimPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpInfo->stPdpIpv4Pcscf.aucPrimPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrRmvPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV4,
                                                        acUeAddr);
        }

        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.bitOpSecPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpInfo->stPdpIpv4Pcscf.aucSecPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrRmvPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV4,
                                                        acUeAddr);
        }

        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.bitOpThiPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpInfo->stPdpIpv4Pcscf.aucThiPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrRmvPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV4,
                                                        acUeAddr);
        }
    }

    if ((IMSA_IP_TYPE_IPV6 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        /* 删除IPV6 P-CSCF地址 */
        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.bitOpPrimPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                pstPdpInfo->stPdpIpv6Pcscf.aucPrimPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrRmvPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV6,
                                                        acUeAddr);
        }

        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.bitOpSecPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                pstPdpInfo->stPdpIpv6Pcscf.aucSecPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrRmvPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV6,
                                                        acUeAddr);
        }

        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.bitOpThiPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                pstPdpInfo->stPdpIpv6Pcscf.aucThiPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrRmvPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV6,
                                                        acUeAddr);
        }
    }
}

VOS_VOID IMSA_CONN_ConfigPcscfInfo2Reg
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    const IMSA_PDP_CNTXT_INFO_STRU     *pstPdpInfo
)
{
    VOS_CHAR                    acUeAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};

    if ((IMSA_IP_TYPE_IPV4 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        /* 配置IPV4 P-CSCF地址 */
        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.bitOpPrimPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpInfo->stPdpIpv4Pcscf.aucPrimPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrAddPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV4,
                                                        IMSA_PCSCF_SRC_TYPE_PDN,
                                                        acUeAddr);
        }

        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.bitOpSecPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpInfo->stPdpIpv4Pcscf.aucSecPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrAddPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV4,
                                                        IMSA_PCSCF_SRC_TYPE_PDN,
                                                        acUeAddr);
        }

        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.bitOpThiPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpInfo->stPdpIpv4Pcscf.aucThiPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrAddPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV4,
                                                        IMSA_PCSCF_SRC_TYPE_PDN,
                                                        acUeAddr);
        }

    }

    if ((IMSA_IP_TYPE_IPV6 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        /* 配置IPV6 P-CSCF地址 */
        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.bitOpPrimPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                pstPdpInfo->stPdpIpv6Pcscf.aucPrimPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrAddPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV6,
                                                        IMSA_PCSCF_SRC_TYPE_PDN,
                                                        acUeAddr);
        }

        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.bitOpSecPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                pstPdpInfo->stPdpIpv6Pcscf.aucSecPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrAddPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV6,
                                                        IMSA_PCSCF_SRC_TYPE_PDN,
                                                        acUeAddr);
        }

        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.bitOpThiPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                pstPdpInfo->stPdpIpv6Pcscf.aucThiPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrAddPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV6,
                                                        IMSA_PCSCF_SRC_TYPE_PDN,
                                                        acUeAddr);
        }

    }
}


VOS_VOID IMSA_CONN_DeleteOldPcscfInfoAndConfigNewPcscfInfo
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_PDP_CNTXT_INFO_STRU           *pstPdpContext,
    IMSA_PDP_CNTXT_INFO_STRU           *pstPdpContextOld
)
{
    IMSA_PDP_CNTXT_INFO_STRU            *pstPdpContextOldTemp = VOS_NULL_PTR;
    IMSA_PDP_CNTXT_INFO_STRU            *pstPdpContextNewTemp = VOS_NULL_PTR;

    pstPdpContextOldTemp = IMSA_MEM_ALLOC(sizeof(IMSA_PDP_CNTXT_INFO_STRU));
    if (VOS_NULL_PTR == pstPdpContextOldTemp)
    {
        return;


    }

    pstPdpContextNewTemp = IMSA_MEM_ALLOC(sizeof(IMSA_PDP_CNTXT_INFO_STRU));
    if (VOS_NULL_PTR == pstPdpContextNewTemp)
    {
        IMSA_MEM_FREE(pstPdpContextOldTemp);
        return;
    }

    IMSA_MEM_SET_S( pstPdpContextOldTemp,
                    sizeof(IMSA_PDP_CNTXT_INFO_STRU),
                    0x0,
                    sizeof(IMSA_PDP_CNTXT_INFO_STRU));
    IMSA_MEM_SET_S( pstPdpContextNewTemp,
                    sizeof(IMSA_PDP_CNTXT_INFO_STRU),
                    0x0,
                    sizeof(IMSA_PDP_CNTXT_INFO_STRU));

    IMSA_MEM_CPY_S( pstPdpContextOldTemp,
                    sizeof(IMSA_PDP_CNTXT_INFO_STRU),
                    pstPdpContextOld,
                    sizeof(IMSA_PDP_CNTXT_INFO_STRU));
    IMSA_MEM_CPY_S( pstPdpContextNewTemp,
                    sizeof(IMSA_PDP_CNTXT_INFO_STRU),
                    pstPdpContext,
                    sizeof(IMSA_PDP_CNTXT_INFO_STRU));

    /* 从原有备份的承载信息中过滤出已注册的PCSCF地址，
       并将未注册的PCSCF地址删除 */
    if (IMSA_TRUE == IMSA_CONN_FilterRegedPcscfInfo(enConnType, pstPdpContextOldTemp))
    {
        IMSA_CONN_DeletePcscfInfo2Reg(enConnType, pstPdpContextOldTemp);
    }

    /* 从修改承载信息中过滤出和已注册的PCSCF地址相同的地址信息，
       并将其他PCSCF地址配成地址对 */
    if (IMSA_TRUE == IMSA_CONN_FilterRegedPcscfInfo(enConnType, pstPdpContextNewTemp))
    {
        IMSA_CONN_ConfigPcscfInfo2Reg(enConnType, pstPdpContextNewTemp);
    }

    IMSA_MEM_FREE(pstPdpContextOldTemp);
    IMSA_MEM_FREE(pstPdpContextNewTemp);

    return;
}


VOS_VOID IMSA_CONN_ProcPcscfInvalid
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_PDP_CNTXT_INFO_STRU           *pstPdpContext,
    IMSA_PDP_CNTXT_INFO_STRU           *pstPdpContextOld
)
{
    VOS_CHAR                            acRegUeAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};
    VOS_CHAR                            acRegPcscfAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};
    VOS_UINT32                          ulRslt   = IMSA_FAIL;
    IMSA_IP_TYPE_ENUM_UINT8             enIpType = IMSA_IP_TYPE_BUTT;

    /* 获取注册参数IP类型 */
    ulRslt = IMSA_RegGetRegedPara(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                    acRegUeAddr,
                                    acRegPcscfAddr,
                                    &enIpType);
    if (IMSA_FAIL == ulRslt)
    {
        return ;
    }

    if ((IMSA_IP_TYPE_IPV4V6 != pstPdpContext->stPdpAddr.enIpType) &&
        (enIpType != pstPdpContext->stPdpAddr.enIpType))
    {
        IMSA_INFO_LOG("IMSA_CONN_ProcPcscfInvalid:IP TYPE NOT MATCH!");

        /* 注册参数IP类型与新的承载信息IP类型不一致，只生成新的地址对
           不需要在识别注册参数是否失效 */

        /* 调用REG模块删除修改承载的IP地址和P-CSCF地址 */
        IMSA_CONN_DeletePdpInfo2Reg(enConnType, pstPdpContextOld);

        /* 给REG模块配置地址对 */
        IMSA_CONN_ConfigPdpInfo2Reg(enConnType, pstPdpContext);
        return ;
    }

    /* 如果这注册参数IP类型与新的承载信息IP类型一致，需要识别注册参数是否失效 */
    if (IMSA_FALSE == IMSA_CONN_IsPdpInfoIncludeRegPara(enConnType, pstPdpContext))
    {
        /* 调用REG模块删除修改承载的IP地址和P-CSCF地址 */
        IMSA_CONN_DeletePdpInfo2Reg(enConnType, pstPdpContextOld);

        /* 给REG模块配置地址对 */
        IMSA_CONN_ConfigPdpInfo2Reg(enConnType, pstPdpContext);

        /* 给CONN模块发送MODIFY消息，类型为注册地址失效 */
        IMSA_CONN_SndConnRegPcscfInvalid(enConnType);
    }
    else
    {
        /* 注册参数没有失效，将备份承载信息中非注册参数的PCSCF删除，
           将新的承载信息中非注册参数的PCSCF配置成地址对 */
        IMSA_CONN_DeleteOldPcscfInfoAndConfigNewPcscfInfo(enConnType, pstPdpContext, pstPdpContextOld);
    }
}



VOS_UINT32 IMSA_CONN_ProcIpAddrPcscfInL2WHO
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType
)
{
    IMSA_PDP_CNTXT_INFO_STRU            *pstPdpContextOld = VOS_NULL_PTR;
    IMSA_CONN_HO_EVENT_STRU             *pstImsaHoEvent   = VOS_NULL_PTR;

    IMSA_WIFI_PDP_CNTXT_INFO_STRU       *pstPdpContext = VOS_NULL_PTR;
    VOS_UINT8                           i;

    pstImsaHoEvent = IMSA_CONN_GetHoEventAddr();
    pstPdpContext = &(IMSA_CONN_GetWifiNormalConnAddr()->stSipSignalPdp);

    if( pstImsaHoEvent->ulSipSignalPdpNum >= IMSA_CONN_MAX_NORMAL_SIP_SIGNAL_PDP_NUM)
    {
        pstImsaHoEvent->ulSipSignalPdpNum = IMSA_CONN_MAX_NORMAL_SIP_SIGNAL_PDP_NUM;
    }

    /* 调用REG模块删除修改承载的IP地址和P-CSCF地址 */
    for( i=0; i<pstImsaHoEvent->ulSipSignalPdpNum; i++)
    {

        if (IMSA_PDP_STATE_ACTIVE == pstImsaHoEvent->u.astSipSignalPdpArray[i].enPdpState)
        {
            pstPdpContextOld = &(pstImsaHoEvent->u.astSipSignalPdpArray[i]);
            IMSA_CONN_DeletePdpInfo2Reg(enConnType, pstPdpContextOld);
        }
    }

    /* 给REG模块配置地址对 */
    IMSA_CONN_ConfigWifiPdpInfo2Reg(enConnType, pstPdpContext);
    return VOS_TRUE;

}


VOS_UINT32 IMSA_CONN_ProcIpAddrPcscfInW2LHO
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType
)
{
    IMSA_WIFI_PDP_CNTXT_INFO_STRU       *pstPdpContextOld  = VOS_NULL_PTR;
    IMSA_CONN_HO_EVENT_STRU             *pstImsaHoEvent   = VOS_NULL_PTR;

    IMSA_NORMAL_CONN_STRU              *pstNormalConn   = VOS_NULL_PTR;
    IMSA_PDP_CNTXT_INFO_STRU            *pstPdpContext  = VOS_NULL_PTR;
    VOS_UINT8                            i;

    pstNormalConn = IMSA_CONN_GetNormalConnAddr();
    pstImsaHoEvent = IMSA_CONN_GetHoEventAddr();

    pstPdpContextOld = &(pstImsaHoEvent->u.stSipSignalPdp);

    /* 最大的信令承载个数为2 */
    if( pstNormalConn->ulSipSignalPdpNum > IMSA_CONN_MAX_NORMAL_SIP_SIGNAL_PDP_NUM)
    {
        pstNormalConn->ulSipSignalPdpNum = IMSA_CONN_MAX_NORMAL_SIP_SIGNAL_PDP_NUM;
    }

    /* 调用REG模块删除修改承载的IP地址和P-CSCF地址 */
    IMSA_CONN_DeleteWifiPdpInfo2Reg(enConnType, pstPdpContextOld);

    for (i = 0; i < pstNormalConn->ulSipSignalPdpNum; i++)
    {
        if (IMSA_PDP_STATE_ACTIVE == pstNormalConn->astSipSignalPdpArray[i].enPdpState)
        {
            pstPdpContext = &pstNormalConn->astSipSignalPdpArray[i];

            /* 给REG模块配置地址对 */
            IMSA_CONN_ConfigPdpInfo2Reg(enConnType, pstPdpContext);
        }
    }
    return VOS_TRUE;

}



VOS_VOID IMSA_CONN_ConfigPdpInfo2Reg
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    const IMSA_PDP_CNTXT_INFO_STRU     *pstPdpInfo
)
{
    VOS_CHAR                    acUeAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};
    VOS_UINT8                   aucTemp[IMSA_IPV6_PREFIX_LEN]           = {0};

    if ((IMSA_IP_TYPE_IPV4 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        /* 配置IPV4地址 */
        IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                            pstPdpInfo->stPdpAddr.aucIpV4Addr,
                                            acUeAddr);

        (VOS_VOID)IMSA_RegAddrPairMgrAddUeAddr( (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                IMSA_IP_TYPE_IPV4,
                                                acUeAddr);

        /* 配置IPV4 P-CSCF地址 */
        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.bitOpPrimPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpInfo->stPdpIpv4Pcscf.aucPrimPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrAddPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV4,
                                                        IMSA_PCSCF_SRC_TYPE_PDN,
                                                        acUeAddr);
        }

        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.bitOpSecPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpInfo->stPdpIpv4Pcscf.aucSecPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrAddPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV4,
                                                        IMSA_PCSCF_SRC_TYPE_PDN,
                                                        acUeAddr);
        }
        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.bitOpThiPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpInfo->stPdpIpv4Pcscf.aucThiPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrAddPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV4,
                                                        IMSA_PCSCF_SRC_TYPE_PDN,
                                                        acUeAddr);
        }
    }

    if ((IMSA_IP_TYPE_IPV6 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        /* 配置IPV6地址 */
        if (0 != IMSA_MEM_CMP(aucTemp, pstPdpInfo->stPdpAddr.aucIpV6Addr, IMSA_IPV6_PREFIX_LEN))
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                pstPdpInfo->stPdpAddr.aucIpV6Addr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrAddUeAddr( (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                    IMSA_IP_TYPE_IPV6,
                                                    acUeAddr);
        }

        /* 配置IPV6 P-CSCF地址 */
        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.bitOpPrimPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                pstPdpInfo->stPdpIpv6Pcscf.aucPrimPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrAddPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV6,
                                                        IMSA_PCSCF_SRC_TYPE_PDN,
                                                        acUeAddr);
        }

        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.bitOpSecPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                pstPdpInfo->stPdpIpv6Pcscf.aucSecPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrAddPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV6,
                                                        IMSA_PCSCF_SRC_TYPE_PDN,
                                                        acUeAddr);
        }
        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.bitOpThiPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                pstPdpInfo->stPdpIpv6Pcscf.aucThiPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrAddPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV6,
                                                        IMSA_PCSCF_SRC_TYPE_PDN,
                                                        acUeAddr);
        }
    }
}


VOS_VOID IMSA_CONN_ConfigWifiPdpInfo2Reg
(
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType,
    const IMSA_WIFI_PDP_CNTXT_INFO_STRU     *pstPdpInfo
)
{
    VOS_CHAR                    acUeAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};
    VOS_UINT8                   aucTemp[IMSA_IPV6_PREFIX_LEN]           = {0};

    if ((IMSA_IP_TYPE_IPV4 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        /* 配置IPV4地址 */
        IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                            (VOS_UINT8 *)pstPdpInfo->stPdpAddr.aucIpV4Addr,
                                            acUeAddr);

        (VOS_VOID)IMSA_RegAddrPairMgrAddUeAddr( (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                IMSA_IP_TYPE_IPV4,
                                                acUeAddr);

        /* 配置IPV4 P-CSCF地址 */
        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.cOpPrimPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpInfo->stPdpIpv4Pcscf.aucPrimPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrAddPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV4,
                                                        IMSA_PCSCF_SRC_TYPE_PDN,
                                                        acUeAddr);
        }

        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.cOpSecPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpInfo->stPdpIpv4Pcscf.aucSecPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrAddPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV4,
                                                        IMSA_PCSCF_SRC_TYPE_PDN,
                                                        acUeAddr);
        }
        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.cOpThiPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpInfo->stPdpIpv4Pcscf.aucThiPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrAddPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV4,
                                                        IMSA_PCSCF_SRC_TYPE_PDN,
                                                        acUeAddr);
        }
    }

    if ((IMSA_IP_TYPE_IPV6 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        /* 配置IPV6地址 */
        if (0 != IMSA_MEM_CMP(aucTemp, pstPdpInfo->stPdpAddr.aucIpV6Addr, IMSA_IPV6_PREFIX_LEN))
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                (VOS_UINT8 *)pstPdpInfo->stPdpAddr.aucIpV6Addr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrAddUeAddr( (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                    IMSA_IP_TYPE_IPV6,
                                                    acUeAddr);
        }

        /* 配置IPV6 P-CSCF地址 */
        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.cOpPrimPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                pstPdpInfo->stPdpIpv6Pcscf.aucPrimPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrAddPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV6,
                                                        IMSA_PCSCF_SRC_TYPE_PDN,
                                                        acUeAddr);
        }

        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.cOpSecPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                pstPdpInfo->stPdpIpv6Pcscf.aucSecPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrAddPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV6,
                                                        IMSA_PCSCF_SRC_TYPE_PDN,
                                                        acUeAddr);
        }
        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.cOpThiPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                pstPdpInfo->stPdpIpv6Pcscf.aucThiPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrAddPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV6,
                                                        IMSA_PCSCF_SRC_TYPE_PDN,
                                                        acUeAddr);
        }
    }
}


VOS_VOID IMSA_CONN_ConfigApMacInfo2Reg
(
    const IMSA_WIFI_PDP_CNTXT_INFO_STRU     *pstPdpInfo
)
{
    IMSA_IMS_NW_ACCESS_TYPE_ENUM_UINT8  enNwAccessType = IMSA_IMS_NW_ACCESS_TYPE_IEEE_802_11;

    /* 将WIFI的接入类型转换为IMS的类型 */
    switch(pstPdpInfo->stApMac.enAccessType)
    {
        case WIFI_IMSA_ACCESS_TYPE_802_11:
            enNwAccessType = IMSA_IMS_NW_ACCESS_TYPE_IEEE_802_11;
            break;

        case WIFI_IMSA_ACCESS_TYPE_802_11_a:
            enNwAccessType = IMSA_IMS_NW_ACCESS_TYPE_IEEE_802_11A;
            break;

        case WIFI_IMSA_ACCESS_TYPE_802_11_b:
            enNwAccessType = IMSA_IMS_NW_ACCESS_TYPE_IEEE_802_11B;
            break;

        case WIFI_IMSA_ACCESS_TYPE_802_11_g:
            enNwAccessType = IMSA_IMS_NW_ACCESS_TYPE_IEEE_802_11G;
            break;

        case WIFI_IMSA_ACCESS_TYPE_802_11_n:
            enNwAccessType = IMSA_IMS_NW_ACCESS_TYPE_IEEE_802_11N;
            break;

        case WIFI_IMSA_ACCESS_TYPE_802_11_ac:
            enNwAccessType = IMSA_IMS_NW_ACCESS_TYPE_IEEE_802_11AC;
            break;

        case WIFI_IMSA_ACCESS_TYPE_802_11_ad:
            enNwAccessType = IMSA_IMS_NW_ACCESS_TYPE_IEEE_802_11AD;
            break;

        case WIFI_IMSA_ACCESS_TYPE_802_11_ah:
            enNwAccessType = IMSA_IMS_NW_ACCESS_TYPE_IEEE_802_11AH;
            break;
        default:
            break;
    }
    /*lint -e534*/
    IMSA_RegAddrApMacAddr(IMSA_REG_TYPE_NORMAL, enNwAccessType, &pstPdpInfo->stApMac.acApMac[0]);
    /*lint +e534*/
}


VOS_VOID IMSA_CONN_DeletePdpInfo2Reg
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    const IMSA_PDP_CNTXT_INFO_STRU     *pstPdpInfo
)
{
    VOS_CHAR                    acUeAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};

    if ((IMSA_IP_TYPE_IPV4 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        /* 删除IPV4地址 */
        IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                            pstPdpInfo->stPdpAddr.aucIpV4Addr,
                                            acUeAddr);

        (VOS_VOID)IMSA_RegAddrPairMgrRmvUeAddr( (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                IMSA_IP_TYPE_IPV4,
                                                acUeAddr);

        /* 删除IPV4 P-CSCF地址 */
        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.bitOpPrimPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpInfo->stPdpIpv4Pcscf.aucPrimPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrRmvPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV4,
                                                        acUeAddr);
        }

        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.bitOpSecPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpInfo->stPdpIpv4Pcscf.aucSecPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrRmvPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV4,
                                                        acUeAddr);
        }

        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.bitOpThiPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpInfo->stPdpIpv4Pcscf.aucThiPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrRmvPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV4,
                                                        acUeAddr);
        }



    }

    if ((IMSA_IP_TYPE_IPV6 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        /* 删除IPV6地址 */
        IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                            pstPdpInfo->stPdpAddr.aucIpV6Addr,
                                            acUeAddr);

        (VOS_VOID)IMSA_RegAddrPairMgrRmvUeAddr( (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                IMSA_IP_TYPE_IPV6,
                                                acUeAddr);

        /* 删除IPV6 P-CSCF地址 */
        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.bitOpPrimPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                pstPdpInfo->stPdpIpv6Pcscf.aucPrimPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrRmvPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV6,
                                                        acUeAddr);
        }

        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.bitOpSecPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                pstPdpInfo->stPdpIpv6Pcscf.aucSecPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrRmvPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV6,
                                                        acUeAddr);
        }

        if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.bitOpThiPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                pstPdpInfo->stPdpIpv6Pcscf.aucThiPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrRmvPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV6,
                                                        acUeAddr);
        }
    }
}


VOS_VOID IMSA_CONN_DeleteWifiPdpInfo2Reg
(
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType,
    const IMSA_WIFI_PDP_CNTXT_INFO_STRU     *pstPdpInfo
)
{
    VOS_CHAR                    acUeAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};

    if ((IMSA_IP_TYPE_IPV4 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        /* 删除IPV4地址 */
        IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                            (VOS_UINT8 *)pstPdpInfo->stPdpAddr.aucIpV4Addr,
                                            acUeAddr);

        (VOS_VOID)IMSA_RegAddrPairMgrRmvUeAddr( (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                IMSA_IP_TYPE_IPV4,
                                                acUeAddr);

        /* 删除IPV4 P-CSCF地址 */
        if (IMSA_TRUE == pstPdpInfo->stPdpIpv4Pcscf.cOpPrimPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpInfo->stPdpIpv4Pcscf.aucPrimPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrRmvPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV4,
                                                        acUeAddr);
        }

        if (IMSA_TRUE == pstPdpInfo->stPdpIpv4Pcscf.cOpSecPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpInfo->stPdpIpv4Pcscf.aucSecPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrRmvPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV4,
                                                        acUeAddr);
        }

        if (IMSA_TRUE == pstPdpInfo->stPdpIpv4Pcscf.cOpThiPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                                pstPdpInfo->stPdpIpv4Pcscf.aucThiPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrRmvPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV4,
                                                        acUeAddr);
        }



    }

    if ((IMSA_IP_TYPE_IPV6 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        /* 删除IPV6地址 */
        IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                            (VOS_UINT8* )pstPdpInfo->stPdpAddr.aucIpV6Addr,
                                            acUeAddr);

        (VOS_VOID)IMSA_RegAddrPairMgrRmvUeAddr( (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                IMSA_IP_TYPE_IPV6,
                                                acUeAddr);

        /* 删除IPV6 P-CSCF地址 */
        if (IMSA_TRUE == pstPdpInfo->stPdpIpv6Pcscf.cOpPrimPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                pstPdpInfo->stPdpIpv6Pcscf.aucPrimPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrRmvPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV6,
                                                        acUeAddr);
        }

        if (IMSA_TRUE == pstPdpInfo->stPdpIpv6Pcscf.cOpSecPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                pstPdpInfo->stPdpIpv6Pcscf.aucSecPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrRmvPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV6,
                                                        acUeAddr);
        }

        if (IMSA_TRUE == pstPdpInfo->stPdpIpv6Pcscf.cOpThiPcscfAddr)
        {
            IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                                pstPdpInfo->stPdpIpv6Pcscf.aucThiPcscfAddr,
                                                acUeAddr);

            (VOS_VOID)IMSA_RegAddrPairMgrRmvPcscfAddr(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                                        IMSA_IP_TYPE_IPV6,
                                                        acUeAddr);
        }
    }
}




VOS_UINT32 IMSA_CONN_PdnInfoNicConfig (IMS_NIC_PDN_INFO_CONFIG_STRU *pstConfigInfo)
{
    VOS_UINT32                          ulRslt                          = IMSA_FAIL;

    IMSA_INFO_LOG("IMSA_CONN_PdnInfoNicConfig enter");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_PdnInfoNicConfig_ENUM, LNAS_ENTRY);
    IMSA_INFO_LOG1("IMSA_CONN_PdnInfoNicConfig ,usSockMaxPort=:", pstConfigInfo->stSockPortInfo.usSockMaxPort);
    TLPS_PRINT2LAYER_INFO1(IMSA_CONN_PdnInfoNicConfig_ENUM, 1, pstConfigInfo->stSockPortInfo.usSockMaxPort);
    IMSA_INFO_LOG1("IMSA_CONN_PdnInfoNicConfig ,usSockMinPort=:", pstConfigInfo->stSockPortInfo.usSockMinPort);
    TLPS_PRINT2LAYER_INFO1(IMSA_CONN_PdnInfoNicConfig_ENUM, 2, pstConfigInfo->stSockPortInfo.usSockMinPort);

    ulRslt = IMS_NIC_PdnInfoConfig(pstConfigInfo);

    IMSA_INFO_LOG("IMSA_CONN_PdnInfoNicConfig leave");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_PdnInfoNicConfig_ENUM, LNAS_LEAVE);

    return ulRslt;
}


VOS_VOID IMSA_CONN_ConfigPdpIPv6Info2Bsp
(
    const IMSA_PDP_CNTXT_INFO_STRU     *pstPdpInfo
)
{
    IMS_NIC_PDN_INFO_CONFIG_STRU        stConfigInfo                    = {0};
    VOS_UINT32                          ulRslt                          = IMSA_FAIL;
    VOS_UINT8                           aucTemp[IMSA_IPV6_PREFIX_LEN]   = {0};
    IMSA_CONTROL_MANAGER_STRU          *pstControlManager               = VOS_NULL_PTR;

    if ((IMSA_IP_TYPE_IPV6 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        if (0 != IMSA_MEM_CMP(aucTemp, pstPdpInfo->stPdpAddr.aucIpV6Addr, IMSA_IPV6_PREFIX_LEN))
        {
            IMSA_MEM_SET_S( &stConfigInfo,
                            sizeof(IMS_NIC_PDN_INFO_CONFIG_STRU),
                            0,
                            sizeof(IMS_NIC_PDN_INFO_CONFIG_STRU));

            stConfigInfo.ucRnicId    = pstPdpInfo->ucRnicId;
            stConfigInfo.enModemId   = MODEM_ID_0;

            /* 配置IPV6地址 */
            stConfigInfo.bitOpIpv6PdnInfo    = IMSA_OP_TRUE;
            stConfigInfo.stIpv6PdnInfo.ulBitPrefixLen = IMSA_IPV6_PREFIX_BIT_LEN;
            IMSA_MEM_CPY_S( stConfigInfo.stIpv6PdnInfo.aucIpV6Addr,
                            IMSA_IPV6_ADDR_LEN,
                            pstPdpInfo->stPdpAddr.aucIpV6Addr,
                            IMSA_IPV6_ADDR_LEN);

            /* 配置IPV6 DNS地址 */
            stConfigInfo.stIpv6PdnInfo.bitOpDnsPrim = pstPdpInfo->stPdpIpv6Dns.bitOpPriDns;
            IMSA_MEM_CPY_S( stConfigInfo.stIpv6PdnInfo.aucDnsPrimAddr,
                            IMSA_IPV6_ADDR_LEN,
                            pstPdpInfo->stPdpIpv6Dns.aucPriDns,
                            IMSA_IPV6_ADDR_LEN);

            stConfigInfo.stIpv6PdnInfo.bitOpDnsSec = pstPdpInfo->stPdpIpv6Dns.bitOpSecDns;
            IMSA_MEM_CPY_S( stConfigInfo.stIpv6PdnInfo.aucDnsSecAddr,
                            IMSA_IPV6_ADDR_LEN,
                            pstPdpInfo->stPdpIpv6Dns.aucSecDns,
                            IMSA_IPV6_ADDR_LEN);

            /* 配置IP栈端口号范围 */
            pstControlManager = IMSA_GetControlManagerAddress();
            stConfigInfo.bitOpSockPortInfo = IMSA_OP_TRUE;
            stConfigInfo.stSockPortInfo.usSockMinPort
                    = pstControlManager->stImsaConfigPara.stImsPortConfig.usImsMinPort;
            stConfigInfo.stSockPortInfo.usSockMaxPort
                    = pstControlManager->stImsaConfigPara.stImsPortConfig.usImsMaxPort;

            ulRslt = IMSA_CONN_PdnInfoNicConfig(&stConfigInfo);
            if (IMS_NIC_SUCC != ulRslt)
            {
                IMSA_ERR_LOG1("IMS_NIC_PdnInfoConfig failed,result:", ulRslt);
                TLPS_PRINT2LAYER_ERROR1(IMSA_CONN_ConfigPdpIPv6Info2Bsp_ENUM, IMSA_CALL_API_ERROR, ulRslt);
            }
        }
    }
}


VOS_VOID IMSA_CONN_ConfigPdpInfo2Bsp
(
    const IMSA_PDP_CNTXT_INFO_STRU     *pstPdpInfo
)
{
    IMS_NIC_PDN_INFO_CONFIG_STRU        stConigInfo                     = {0};
    VOS_UINT32                          ulRslt                          = IMSA_FAIL;
    VOS_UINT8                           aucTemp[IMSA_IPV6_PREFIX_LEN]   = {0};
    IMSA_CONTROL_MANAGER_STRU          *pstControlManager               = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_ConfigPdpInfo2Bsp is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_ConfigPdpInfo2Bsp_ENUM, LNAS_ENTRY);

    IMSA_MEM_SET_S( &stConigInfo,
                    sizeof(IMS_NIC_PDN_INFO_CONFIG_STRU),
                    0,
                    sizeof(IMS_NIC_PDN_INFO_CONFIG_STRU));

//    stConigInfo.ucRabId     = pstPdpInfo->ucPdpId;
    stConigInfo.ucRnicId    = pstPdpInfo->ucRnicId;
//    stConigInfo.ucWifiPdnId = 0;

    stConigInfo.enModemId   = MODEM_ID_0;

    if ((IMSA_IP_TYPE_IPV4 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        /* 配置IPV4地址 */
        stConigInfo.bitOpIpv4PdnInfo    = IMSA_OP_TRUE;
        IMSA_MEM_CPY_S(   stConigInfo.stIpv4PdnInfo.aucIpV4Addr,
                        IMSA_IPV4_ADDR_LEN,
                        pstPdpInfo->stPdpAddr.aucIpV4Addr,
                        IMSA_IPV4_ADDR_LEN);

        /* 配置IPV4 DNS地址 */
        stConigInfo.stIpv4PdnInfo.bitOpDnsPrim = pstPdpInfo->stPdpIpv4Dns.bitOpPriDns;
        IMSA_MEM_CPY_S(   stConigInfo.stIpv4PdnInfo.aucDnsPrimAddr,
                        IMSA_IPV4_ADDR_LEN,
                        pstPdpInfo->stPdpIpv4Dns.aucPriDns,
                        IMSA_IPV4_ADDR_LEN);

        stConigInfo.stIpv4PdnInfo.bitOpDnsSec = pstPdpInfo->stPdpIpv4Dns.bitOpSecDns;
        IMSA_MEM_CPY_S(   stConigInfo.stIpv4PdnInfo.aucDnsSecAddr,
                        IMSA_IPV4_ADDR_LEN,
                        pstPdpInfo->stPdpIpv4Dns.aucSecDns,
                        IMSA_IPV4_ADDR_LEN);
    }

    if ((IMSA_IP_TYPE_IPV6 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        if (0 != IMSA_MEM_CMP(aucTemp, pstPdpInfo->stPdpAddr.aucIpV6Addr, IMSA_IPV6_PREFIX_LEN))
        {
            /* 配置IPV6地址 */
            stConigInfo.bitOpIpv6PdnInfo    = IMSA_OP_TRUE;
            stConigInfo.stIpv6PdnInfo.ulBitPrefixLen = IMSA_IPV6_PREFIX_BIT_LEN;
            IMSA_MEM_CPY_S( stConigInfo.stIpv6PdnInfo.aucIpV6Addr,
                            IMSA_IPV6_ADDR_LEN,
                            pstPdpInfo->stPdpAddr.aucIpV6Addr,
                            IMSA_IPV6_ADDR_LEN);

            /* 配置IPV6 DNS地址 */
            stConigInfo.stIpv6PdnInfo.bitOpDnsPrim = pstPdpInfo->stPdpIpv6Dns.bitOpPriDns;
            IMSA_MEM_CPY_S(   stConigInfo.stIpv6PdnInfo.aucDnsPrimAddr,
                            IMSA_IPV6_ADDR_LEN,
                            pstPdpInfo->stPdpIpv6Dns.aucPriDns,
                            IMSA_IPV6_ADDR_LEN);

            stConigInfo.stIpv6PdnInfo.bitOpDnsSec = pstPdpInfo->stPdpIpv6Dns.bitOpSecDns;
            IMSA_MEM_CPY_S(   stConigInfo.stIpv6PdnInfo.aucDnsSecAddr,
                            IMSA_IPV6_ADDR_LEN,
                            pstPdpInfo->stPdpIpv6Dns.aucSecDns,
                            IMSA_IPV6_ADDR_LEN);
        }
    }

    if ((IMSA_OP_TRUE == stConigInfo.bitOpIpv4PdnInfo)
        || (IMSA_OP_TRUE == stConigInfo.bitOpIpv6PdnInfo))
    {
        pstControlManager = IMSA_GetControlManagerAddress();
        stConigInfo.bitOpSockPortInfo = IMSA_OP_TRUE;
        stConigInfo.stSockPortInfo.usSockMinPort
                = pstControlManager->stImsaConfigPara.stImsPortConfig.usImsMinPort;
        stConigInfo.stSockPortInfo.usSockMaxPort
                = pstControlManager->stImsaConfigPara.stImsPortConfig.usImsMaxPort;

        ulRslt = IMSA_CONN_PdnInfoNicConfig(&stConigInfo);
        if (IMS_NIC_SUCC != ulRslt)
        {
            IMSA_ERR_LOG1("IMS_NIC_PdnInfoConfig failed,result:", ulRslt);
            TLPS_PRINT2LAYER_ERROR1(IMSA_CONN_ConfigPdpInfo2Bsp_ENUM, IMSA_CALL_API_ERROR, ulRslt);
        }
    }
}


VOS_VOID IMSA_CONN_ConfigWifiPdpInfo2Bsp
(
    const IMSA_WIFI_PDP_CNTXT_INFO_STRU     *pstPdpInfo
)
{
    IMS_NIC_PDN_INFO_CONFIG_STRU        stConigInfo                     = {0};
    VOS_UINT32                          ulRslt                          = IMSA_FAIL;
    VOS_UINT8                           aucTemp[IMSA_IPV6_PREFIX_LEN]   = {0};
    IMSA_CONTROL_MANAGER_STRU          *pstControlManager               = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_ConfigPdpInfo2Bsp is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_ConfigPdpInfo2Bsp_ENUM, LNAS_ENTRY);

    IMSA_MEM_SET_S(&stConigInfo,sizeof(IMS_NIC_PDN_INFO_CONFIG_STRU), 0, sizeof(IMS_NIC_PDN_INFO_CONFIG_STRU));

    stConigInfo.ucRnicId    = pstPdpInfo->ucRnicId;
    stConigInfo.enModemId   = MODEM_ID_0;

    if ((IMSA_IP_TYPE_IPV4 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        /* 配置IPV4地址 */
        stConigInfo.bitOpIpv4PdnInfo    = IMSA_OP_TRUE;
        IMSA_MEM_CPY_S(   stConigInfo.stIpv4PdnInfo.aucIpV4Addr,
                        IMSA_IPV4_ADDR_LEN,
                        pstPdpInfo->stPdpAddr.aucIpV4Addr,
                        IMSA_IPV4_ADDR_LEN);

        /* 配置IPV4 DNS地址 */
        if (VOS_TRUE == pstPdpInfo->stPdpIpv4Dns.cOpPriDns)
        {
            stConigInfo.stIpv4PdnInfo.bitOpDnsPrim = IMSA_OP_TRUE;
        }
        IMSA_MEM_CPY_S(   stConigInfo.stIpv4PdnInfo.aucDnsPrimAddr,
                        IMSA_IPV4_ADDR_LEN,
                        pstPdpInfo->stPdpIpv4Dns.aucPriDns,
                        IMSA_IPV4_ADDR_LEN);

        if (VOS_TRUE == pstPdpInfo->stPdpIpv4Dns.cOpSecDns)
        {
            stConigInfo.stIpv4PdnInfo.bitOpDnsSec = IMSA_OP_TRUE;
        }
        IMSA_MEM_CPY_S(   stConigInfo.stIpv4PdnInfo.aucDnsSecAddr,
                        IMSA_IPV4_ADDR_LEN,
                        pstPdpInfo->stPdpIpv4Dns.aucSecDns,
                        IMSA_IPV4_ADDR_LEN);
    }

    if ((IMSA_IP_TYPE_IPV6 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        if (0 != IMSA_MEM_CMP(aucTemp, pstPdpInfo->stPdpAddr.aucIpV6Addr, IMSA_IPV6_PREFIX_LEN))
        {
            /* 配置IPV6地址 */
            stConigInfo.bitOpIpv6PdnInfo    = IMSA_OP_TRUE;
            stConigInfo.stIpv6PdnInfo.ulBitPrefixLen = IMSA_IPV6_PREFIX_BIT_LEN;
            IMSA_MEM_CPY_S(   stConigInfo.stIpv6PdnInfo.aucIpV6Addr,
                            IMSA_IPV6_ADDR_LEN,
                            pstPdpInfo->stPdpAddr.aucIpV6Addr,
                            IMSA_IPV6_ADDR_LEN);

            /* 配置IPV6 DNS地址 */
            if (VOS_TRUE == pstPdpInfo->stPdpIpv4Dns.cOpPriDns)
            {
                stConigInfo.stIpv6PdnInfo.bitOpDnsPrim = IMSA_OP_TRUE;
            }
            IMSA_MEM_CPY_S(   stConigInfo.stIpv6PdnInfo.aucDnsPrimAddr,
                            IMSA_IPV6_ADDR_LEN,
                            pstPdpInfo->stPdpIpv6Dns.aucPriDns,
                            IMSA_IPV6_ADDR_LEN);

            if (VOS_TRUE == pstPdpInfo->stPdpIpv4Dns.cOpSecDns)
            {
                stConigInfo.stIpv6PdnInfo.bitOpDnsSec = IMSA_OP_TRUE;
            }
            IMSA_MEM_CPY_S(   stConigInfo.stIpv6PdnInfo.aucDnsSecAddr,
                            IMSA_IPV6_ADDR_LEN,
                            pstPdpInfo->stPdpIpv6Dns.aucSecDns,
                            IMSA_IPV6_ADDR_LEN);
        }
    }

    if ((IMSA_OP_TRUE == stConigInfo.bitOpIpv4PdnInfo)
        || (IMSA_OP_TRUE == stConigInfo.bitOpIpv6PdnInfo))
    {
        pstControlManager = IMSA_GetControlManagerAddress();
        stConigInfo.bitOpSockPortInfo = IMSA_OP_TRUE;
        stConigInfo.stSockPortInfo.usSockMinPort
                = pstControlManager->stImsaConfigPara.stImsPortConfig.usImsMinPort;
        stConigInfo.stSockPortInfo.usSockMaxPort
                = pstControlManager->stImsaConfigPara.stImsPortConfig.usImsMaxPort;

        ulRslt = IMSA_CONN_PdnInfoNicConfig(&stConigInfo);
        if (IMS_NIC_SUCC != ulRslt)
        {
            IMSA_ERR_LOG1("IMS_NIC_PdnInfoConfig failed,result:", ulRslt);
            TLPS_PRINT2LAYER_ERROR1(IMSA_CONN_ConfigPdpInfo2Bsp_ENUM, IMSA_CALL_API_ERROR, ulRslt);
        }
    }
}


VOS_VOID IMSA_CONN_SaveSipSignalPdpInfo
(
    IMSA_PDP_CNTXT_INFO_STRU                   *pstPdpInfo,
    const TAF_PS_CALL_PDP_ACTIVATE_CNF_STRU    *pstPdpActivateCnf
)
{
    IMSA_CONTROL_MANAGER_STRU      *pstControlManager;
    pstControlManager               = IMSA_GetControlManagerAddress();

    pstPdpInfo->ucPdpId             = pstPdpActivateCnf->ucRabId;

    pstPdpInfo->ucRnicId            = IMSA_CONN_FindAvailRnicId();

    pstPdpInfo->enPdpState          = IMSA_PDP_STATE_ACTIVE;
    if (IMSA_OP_TRUE == pstPdpActivateCnf->bitOpLinkdRabId)
    {
        pstPdpInfo->enPdpType = IMSA_PDP_TYPE_DEDICATED;
    }
    else if ((IMSA_OP_TRUE == pstPdpActivateCnf->bitOpEmergencyInd) && (TAF_PDP_FOR_EMC == pstPdpActivateCnf->enEmergencyInd))
    {
        pstPdpInfo->enPdpType = IMSA_PDP_TYPE_EMERGENCY;
    }
    else
    {
        pstPdpInfo->enPdpType = IMSA_PDP_TYPE_DEFAULT;
    }
    pstPdpInfo->ucCid               = pstPdpActivateCnf->ucCid;

    /* 存储IP地址 */
    pstPdpInfo->bitOpPdpAddr        = pstPdpActivateCnf->bitOpPdpAddr;
    IMSA_MEM_CPY_S(                 &pstPdpInfo->stPdpAddr,
                                    sizeof(IMSA_IP_ADDRESS_STRU),
                                    &pstPdpActivateCnf->stPdpAddr,
                                    sizeof(IMSA_IP_ADDRESS_STRU));

    switch(pstControlManager->stPcscfDiscoveryPolicyInfo.enPcscfDiscoverPolicy)
    {
    case IMSA_PCSCF_DISCOVERY_POLICY_PCO:

        /* 通过PCO获取IPV4 P-CSCF */
        pstPdpInfo->stPdpIpv4Pcscf.bitOpPrimPcscfAddr = pstPdpActivateCnf->stPcscf.bitOpPrimPcscfAddr;
        pstPdpInfo->stPdpIpv4Pcscf.bitOpSecPcscfAddr = pstPdpActivateCnf->stPcscf.bitOpSecPcscfAddr;
        pstPdpInfo->stPdpIpv4Pcscf.bitOpThiPcscfAddr = pstPdpActivateCnf->stPcscf.bitOpThiPcscfAddr;

        IMSA_MEM_CPY_S(                 &pstPdpInfo->stPdpIpv4Pcscf.aucPrimPcscfAddr[0],
                                        IMSA_IPV4_ADDR_LEN,
                                        &pstPdpActivateCnf->stPcscf.aucPrimPcscfAddr[0],
                                        TAF_IPV4_ADDR_LEN);

        IMSA_MEM_CPY_S(                 &pstPdpInfo->stPdpIpv4Pcscf.aucSecPcscfAddr[0],
                                        IMSA_IPV4_ADDR_LEN,
                                        &pstPdpActivateCnf->stPcscf.aucSecPcscfAddr[0],
                                        TAF_IPV4_ADDR_LEN);

        IMSA_MEM_CPY_S(                 &pstPdpInfo->stPdpIpv4Pcscf.aucThiPcscfAddr[0],
                                        IMSA_IPV4_ADDR_LEN,
                                        &pstPdpActivateCnf->stPcscf.aucThiPcscfAddr[0],
                                        TAF_IPV4_ADDR_LEN);

        /* 通过PCO获取IPV6 P-CSCF */
        pstPdpInfo->stPdpIpv6Pcscf.bitOpPrimPcscfAddr = pstPdpActivateCnf->stIpv6Pcscf.bitOpPrimPcscfAddr;
        pstPdpInfo->stPdpIpv6Pcscf.bitOpSecPcscfAddr = pstPdpActivateCnf->stIpv6Pcscf.bitOpSecPcscfAddr;
        pstPdpInfo->stPdpIpv6Pcscf.bitOpThiPcscfAddr = pstPdpActivateCnf->stIpv6Pcscf.bitOpThiPcscfAddr;

        IMSA_MEM_CPY_S(                 &pstPdpInfo->stPdpIpv6Pcscf.aucPrimPcscfAddr[0],
                                        IMSA_IPV6_ADDR_LEN,
                                        &pstPdpActivateCnf->stIpv6Pcscf.aucPrimPcscfAddr[0],
                                        TAF_IPV6_ADDR_LEN);

        IMSA_MEM_CPY_S(                 &pstPdpInfo->stPdpIpv6Pcscf.aucSecPcscfAddr[0],
                                        IMSA_IPV6_ADDR_LEN,
                                        &pstPdpActivateCnf->stIpv6Pcscf.aucSecPcscfAddr[0],
                                        TAF_IPV6_ADDR_LEN);

        IMSA_MEM_CPY_S(                 &pstPdpInfo->stPdpIpv6Pcscf.aucThiPcscfAddr[0],
                                        IMSA_IPV6_ADDR_LEN,
                                        &pstPdpActivateCnf->stIpv6Pcscf.aucThiPcscfAddr[0],
                                        TAF_IPV6_ADDR_LEN);

        IMSA_INFO_LOG("IMSA_CONN_SaveSipSignalPdpInfo: IMSA_PCSCF_DISCOVERY_POLICY_PCO!");
        TLPS_PRINT2LAYER_INFO(IMSA_CONN_SaveSipSignalPdpInfo_ENUM, 1);
        break;
    case IMSA_PCSCF_DISCOVERY_POLICY_NV:

        /* 通过NV配置获取IPV4 P-CSCF */
        pstPdpInfo->stPdpIpv4Pcscf.bitOpPrimPcscfAddr = pstControlManager->stPcscfDiscoveryPolicyInfo.stIpv4Pcscf.bitOpPrimPcscfAddr;
        pstPdpInfo->stPdpIpv4Pcscf.bitOpSecPcscfAddr = pstControlManager->stPcscfDiscoveryPolicyInfo.stIpv4Pcscf.bitOpSecPcscfAddr;


        IMSA_MEM_CPY_S(                 &pstPdpInfo->stPdpIpv4Pcscf.aucPrimPcscfAddr[0],
                                        IMSA_IPV4_ADDR_LEN,
                                        &pstControlManager->stPcscfDiscoveryPolicyInfo.stIpv4Pcscf.aucPrimPcscfAddr[0],
                                        TAF_IPV4_ADDR_LEN);

        IMSA_MEM_CPY_S(                 &pstPdpInfo->stPdpIpv4Pcscf.aucSecPcscfAddr[0],
                                        IMSA_IPV4_ADDR_LEN,
                                        &pstControlManager->stPcscfDiscoveryPolicyInfo.stIpv4Pcscf.aucSecPcscfAddr[0],
                                        TAF_IPV4_ADDR_LEN);

        /* 通过NV配置获取IPV6 P-CSCF */
        pstPdpInfo->stPdpIpv6Pcscf.bitOpPrimPcscfAddr = pstControlManager->stPcscfDiscoveryPolicyInfo.stIpv6Pcscf.bitOpPrimPcscfAddr;
        pstPdpInfo->stPdpIpv6Pcscf.bitOpSecPcscfAddr = pstControlManager->stPcscfDiscoveryPolicyInfo.stIpv6Pcscf.bitOpSecPcscfAddr;

        IMSA_MEM_CPY_S(                 &pstPdpInfo->stPdpIpv6Pcscf.aucPrimPcscfAddr[0],
                                        IMSA_IPV6_ADDR_LEN,
                                        &pstControlManager->stPcscfDiscoveryPolicyInfo.stIpv6Pcscf.aucPrimPcscfAddr[0],
                                        TAF_IPV6_ADDR_LEN);

        IMSA_MEM_CPY_S(                 &pstPdpInfo->stPdpIpv6Pcscf.aucSecPcscfAddr[0],
                                        IMSA_IPV6_ADDR_LEN,
                                        &pstControlManager->stPcscfDiscoveryPolicyInfo.stIpv6Pcscf.aucSecPcscfAddr[0],
                                        TAF_IPV6_ADDR_LEN);

        IMSA_INFO_LOG("IMSA_CONN_SaveSipSignalPdpInfo: IMSA_PCSCF_DISCOVERY_POLICY_NV!");
        TLPS_PRINT2LAYER_INFO(IMSA_CONN_SaveSipSignalPdpInfo_ENUM, 2);
        break;
    default:
        IMSA_INFO_LOG("IMSA_CONN_SaveSipSignalPdpInfo:P-CSCF policy error!");
        TLPS_PRINT2LAYER_INFO(IMSA_CONN_SaveSipSignalPdpInfo_ENUM, LNAS_ERROR);
        break;
    }

    /* 存储IPV4 DNS */
    IMSA_MEM_CPY_S(                 &pstPdpInfo->stPdpIpv4Dns,
                                    sizeof(IMSA_PDP_IPV4_DNS_STRU),
                                    &pstPdpActivateCnf->stDns,
                                    sizeof(IMSA_PDP_IPV4_DNS_STRU));

    /* 存储IPV6 DNS */
    IMSA_MEM_CPY_S(                 &pstPdpInfo->stPdpIpv6Dns,
                                    sizeof(IMSA_PDP_IPV6_DNS_STRU),
                                    &pstPdpActivateCnf->stIpv6Dns,
                                    sizeof(IMSA_PDP_IPV6_DNS_STRU));

    /* 存储EPS QOS */
    IMSA_MEM_CPY_S(                 &pstPdpInfo->stEpsQos,
                                    sizeof(IMSA_PDP_EPS_QOS_STRU),
                                    &pstPdpActivateCnf->stEpsQos,
                                    sizeof(IMSA_PDP_EPS_QOS_STRU));

}



VOS_VOID IMSA_CONN_SaveWifiPdpInfo
(
    IMSA_WIFI_PDP_CNTXT_INFO_STRU              *pstPdpInfo,
    const WIFI_IMSA_IMS_PDN_ACTIVATE_CNF_STRU  *pstPdpActivateCnf
)
{
    pstPdpInfo->enPdpState          = IMSA_PDP_STATE_ACTIVE;

    pstPdpInfo->enPdnType           = pstPdpActivateCnf->enPdnType;

    pstPdpInfo->ucIsEmergency       = (VOS_UINT8)pstPdpActivateCnf->cIsEmergency;

    pstPdpInfo->ucRnicId            = IMSA_CONN_FindAvailRnicId();

    /* 存储APN */
    IMSA_MEM_CPY_S(                   &pstPdpInfo->stApn,
                                    sizeof(IMSA_WIFI_PDP_APN_STRU),
                                    &pstPdpActivateCnf->stApn,
                                    sizeof(IMSA_WIFI_PDP_APN_STRU));

    /* 存储AP MAC地址 */
    IMSA_MEM_CPY_S(                   &pstPdpInfo->stApMac,
                                    sizeof(IMSA_WIFI_AP_MAC_STRU),
                                    &pstPdpActivateCnf->stApMac,
                                    sizeof(IMSA_WIFI_AP_MAC_STRU));

    /* 存储IP地址 */
    IMSA_MEM_CPY_S(                   &pstPdpInfo->stPdpAddr,
                                    sizeof(IMSA_WIFI_IP_ADDRESS_STRU),
                                    &pstPdpActivateCnf->stPdpAddr,
                                    sizeof(IMSA_WIFI_IP_ADDRESS_STRU));

    /* 存储IPV4 DNS */
    IMSA_MEM_CPY_S(                   &pstPdpInfo->stPdpIpv4Dns,
                                    sizeof(IMSA_WIFI_PDP_IPV4_DNS_STRU),
                                    &pstPdpActivateCnf->stDns,
                                    sizeof(IMSA_WIFI_PDP_IPV4_DNS_STRU));

    /* 存储IPV6 DNS */
    IMSA_MEM_CPY_S(                   &pstPdpInfo->stPdpIpv6Dns,
                                    sizeof(IMSA_WIFI_PDP_IPV6_DNS_STRU),
                                    &pstPdpActivateCnf->stIPv6Dns,
                                    sizeof(IMSA_WIFI_PDP_IPV6_DNS_STRU));

    /* 存储IPV4 P-CSCF */
    IMSA_MEM_CPY_S(                   &pstPdpInfo->stPdpIpv4Pcscf,
                                    sizeof(IMSA_WIFI_PDP_IPV4_PCSCF_STRU),
                                    &pstPdpActivateCnf->stPcscf,
                                    sizeof(IMSA_WIFI_PDP_IPV4_PCSCF_STRU));

    /* 存储IPV6 P-CSCF */
    IMSA_MEM_CPY_S(                   &pstPdpInfo->stPdpIpv6Pcscf,
                                    sizeof(IMSA_WIFI_PDP_IPV6_PCSCF_STRU),
                                    &pstPdpActivateCnf->stIPv6Pcscf,
                                    sizeof(IMSA_WIFI_PDP_IPV6_PCSCF_STRU));

}




VOS_UINT32 IMSA_CONN_SaveMediaPdpInfoWithSamePdpId
(
    IMSA_CONN_TYPE_ENUM_UINT32                  enConnType,
    const TAF_PS_CALL_PDP_ACTIVATE_IND_STRU    *pstPdpActivateInd
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn       = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn          = VOS_NULL_PTR;
    IMSA_PDP_CNTXT_INFO_STRU           *pstPdpInfo          = VOS_NULL_PTR;
    VOS_UINT32                         i                    = IMSA_NULL;

    IMSA_INFO_LOG("IMSA_CONN_SaveMediaPdpInfoWithSamePdpId is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_SaveMediaPdpInfoWithSamePdpId_ENUM, LNAS_ENTRY);

    pstNormalConn                   = IMSA_CONN_GetNormalConnAddr();
    pstEmcConn                      = IMSA_CONN_GetEmcConnAddr();

    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        for (i = 0; i < pstNormalConn->ulSipMediaPdpNum; i++)
        {
            if ((IMSA_PDP_STATE_ACTIVE == pstNormalConn->astSipMediaPdpArray[i].enPdpState)
                && (pstNormalConn->astSipMediaPdpArray[i].ucPdpId == pstPdpActivateInd->ucRabId))
            {
                if (i < (IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM - 1))
                {
                    IMSA_MEM_CPY_S( &pstNormalConn->astSipMediaPdpArray[i],
                        sizeof(IMSA_PDP_CNTXT_INFO_STRU)*((IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM-1)-i),
                        &pstNormalConn->astSipMediaPdpArray[i+1],
                        sizeof(IMSA_PDP_CNTXT_INFO_STRU)*((IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM-1)-i));
                }

                IMSA_MEM_SET_S( &pstNormalConn->astSipMediaPdpArray[IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM-1],
                    sizeof(IMSA_PDP_CNTXT_INFO_STRU),
                    0x0,
                    sizeof(IMSA_PDP_CNTXT_INFO_STRU));

                pstPdpInfo                      = &pstNormalConn->astSipMediaPdpArray[pstNormalConn->ulSipMediaPdpNum - 1];

                pstPdpInfo->ucPdpId             = pstPdpActivateInd->ucRabId;
                pstPdpInfo->enPdpState          = IMSA_PDP_STATE_ACTIVE;
                pstPdpInfo->enPdpType           = IMSA_PDP_TYPE_DEDICATED;
                pstPdpInfo->ucCid               = pstPdpActivateInd->ucCid;

                /* 存储关联的信令承载号 */
                pstPdpInfo->bitOpLinkedPdpId    = pstPdpActivateInd->bitOpLinkdRabId;
                pstPdpInfo->ucLinkedPdpId       = pstPdpActivateInd->ucLinkdRabId;

                pstPdpInfo->bitOpTft            = pstPdpActivateInd->bitOpTft;
                IMSA_MEM_CPY_S(                 &pstPdpInfo->stTft,
                                                sizeof(IMSA_PDP_TFT_INFO_STRU),
                                                &pstPdpActivateInd->stTft,
                                                sizeof(IMSA_PDP_TFT_INFO_STRU));

                if (IMSA_MAX_PF_NUM < pstPdpActivateInd->stTft.ulPfNum)
                {
                    pstPdpInfo->stTft.ulPfNum = IMSA_MAX_PF_NUM;
                }

                /* 存储EPS QOS */
                IMSA_MEM_CPY_S(                 &pstPdpInfo->stEpsQos,
                                                sizeof(IMSA_PDP_EPS_QOS_STRU),
                                                &pstPdpActivateInd->stEpsQos,
                                                sizeof(IMSA_PDP_EPS_QOS_STRU));

                IMSA_INFO_LOG("IMSA_CONN_SaveMediaPdpInfoWithSamePdpId:get NormalConn MediaInfo by same pdpid!");
                TLPS_PRINT2LAYER_INFO(IMSA_CONN_SaveMediaPdpInfoWithSamePdpId_ENUM, 1);

                return IMSA_SUCC;
            }
        }

        return IMSA_FAIL;
    }

    for (i = 0; i < pstEmcConn->ulSipMediaPdpNum; i++)
    {
        if ((IMSA_PDP_STATE_ACTIVE == pstEmcConn->astSipMediaPdpArray[i].enPdpState)
            && (pstEmcConn->astSipMediaPdpArray[i].ucPdpId == pstPdpActivateInd->ucRabId))
        {
            if (i < (IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM - 1))
            {
                IMSA_MEM_CPY_S( &pstEmcConn->astSipMediaPdpArray[i],
                    sizeof(IMSA_PDP_CNTXT_INFO_STRU)*((IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM-1)-i),
                    &pstEmcConn->astSipMediaPdpArray[i+1],
                    sizeof(IMSA_PDP_CNTXT_INFO_STRU)*((IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM-1)-i));
            }

            IMSA_MEM_SET_S( &pstEmcConn->astSipMediaPdpArray[IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM-1],
                sizeof(IMSA_PDP_CNTXT_INFO_STRU),
                0x0,
                sizeof(IMSA_PDP_CNTXT_INFO_STRU));

            pstPdpInfo                      = &pstEmcConn->astSipMediaPdpArray[pstEmcConn->ulSipMediaPdpNum - 1];

            pstPdpInfo->ucPdpId             = pstPdpActivateInd->ucRabId;
            pstPdpInfo->enPdpState          = IMSA_PDP_STATE_ACTIVE;
            pstPdpInfo->enPdpType           = IMSA_PDP_TYPE_DEDICATED;
            pstPdpInfo->ucCid               = pstPdpActivateInd->ucCid;

            /* 存储关联的信令承载号 */
            pstPdpInfo->bitOpLinkedPdpId    = pstPdpActivateInd->bitOpLinkdRabId;
            pstPdpInfo->ucLinkedPdpId       = pstPdpActivateInd->ucLinkdRabId;

            pstPdpInfo->bitOpTft            = pstPdpActivateInd->bitOpTft;
            IMSA_MEM_CPY_S(                 &pstPdpInfo->stTft,
                                            sizeof(IMSA_PDP_TFT_INFO_STRU),
                                            &pstPdpActivateInd->stTft,
                                            sizeof(IMSA_PDP_TFT_INFO_STRU));

            if (IMSA_MAX_PF_NUM < pstPdpActivateInd->stTft.ulPfNum)
            {
                pstPdpInfo->stTft.ulPfNum = IMSA_MAX_PF_NUM;
            }

            /* 存储EPS QOS */
            IMSA_MEM_CPY_S(                 &pstPdpInfo->stEpsQos,
                                            sizeof(IMSA_PDP_EPS_QOS_STRU),
                                            &pstPdpActivateInd->stEpsQos,
                                            sizeof(IMSA_PDP_EPS_QOS_STRU));

            IMSA_INFO_LOG("IMSA_CONN_SaveMediaPdpInfoWithSamePdpId:get EmcConn MediaInfo by same pdpid!");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_SaveMediaPdpInfoWithSamePdpId_ENUM, 2);

            return IMSA_SUCC;
        }
    }

    return IMSA_FAIL;
}



VOS_UINT32 IMSA_CONN_SaveMediaPdpInfoWithSameQci
(
    IMSA_CONN_TYPE_ENUM_UINT32                  enConnType,
    const TAF_PS_CALL_PDP_ACTIVATE_IND_STRU    *pstPdpActivateInd
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn       = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn          = VOS_NULL_PTR;
    IMSA_PDP_CNTXT_INFO_STRU           *pstPdpInfo          = VOS_NULL_PTR;
    VOS_UINT32                         i                    = IMSA_NULL;

    IMSA_INFO_LOG("IMSA_CONN_SaveMediaPdpInfoWithSameQci is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_SaveMediaPdpInfoWithSameQci_ENUM, LNAS_ENTRY);

    pstNormalConn                   = IMSA_CONN_GetNormalConnAddr();
    pstEmcConn                      = IMSA_CONN_GetEmcConnAddr();

    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        for (i = 0; i < pstNormalConn->ulSipMediaPdpNum; i++)
        {
            if ((IMSA_PDP_STATE_ACTIVE == pstNormalConn->astSipMediaPdpArray[i].enPdpState)
                && (pstNormalConn->astSipMediaPdpArray[i].stEpsQos.ucQCI == pstPdpActivateInd->stEpsQos.ucQCI))
            {
                if (i < (IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM - 1))
                {
                    IMSA_MEM_CPY_S( &pstNormalConn->astSipMediaPdpArray[i],
                        sizeof(IMSA_PDP_CNTXT_INFO_STRU)*((IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM-1)-i),
                        &pstNormalConn->astSipMediaPdpArray[i+1],
                        sizeof(IMSA_PDP_CNTXT_INFO_STRU)*((IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM-1)-i));
                }

                IMSA_MEM_SET_S( &pstNormalConn->astSipMediaPdpArray[IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM-1],
                    sizeof(IMSA_PDP_CNTXT_INFO_STRU),
                    0x0,
                    sizeof(IMSA_PDP_CNTXT_INFO_STRU));

                pstPdpInfo                      = &pstNormalConn->astSipMediaPdpArray[pstNormalConn->ulSipMediaPdpNum - 1];

                pstPdpInfo->ucPdpId             = pstPdpActivateInd->ucRabId;
                pstPdpInfo->enPdpState          = IMSA_PDP_STATE_ACTIVE;
                pstPdpInfo->enPdpType           = IMSA_PDP_TYPE_DEDICATED;
                pstPdpInfo->ucCid               = pstPdpActivateInd->ucCid;

                /* 存储关联的信令承载号 */
                pstPdpInfo->bitOpLinkedPdpId    = pstPdpActivateInd->bitOpLinkdRabId;
                pstPdpInfo->ucLinkedPdpId       = pstPdpActivateInd->ucLinkdRabId;

                pstPdpInfo->bitOpTft            = pstPdpActivateInd->bitOpTft;
                IMSA_MEM_CPY_S(                 &pstPdpInfo->stTft,
                                                sizeof(IMSA_PDP_TFT_INFO_STRU),
                                                &pstPdpActivateInd->stTft,
                                                sizeof(IMSA_PDP_TFT_INFO_STRU));

                if (IMSA_MAX_PF_NUM < pstPdpActivateInd->stTft.ulPfNum)
                {
                    pstPdpInfo->stTft.ulPfNum = IMSA_MAX_PF_NUM;
                }

                /* 存储EPS QOS */
                IMSA_MEM_CPY_S(                 &pstPdpInfo->stEpsQos,
                                                sizeof(IMSA_PDP_EPS_QOS_STRU),
                                                &pstPdpActivateInd->stEpsQos,
                                                sizeof(IMSA_PDP_EPS_QOS_STRU));

                IMSA_INFO_LOG("IMSA_CONN_SaveMediaPdpInfoWithSameQci:get NormalConn MediaInfo by same Qci!");
                TLPS_PRINT2LAYER_INFO(IMSA_CONN_SaveMediaPdpInfoWithSameQci_ENUM, 1);

                return IMSA_SUCC;
            }
        }

        return IMSA_FAIL;
    }

    for (i = 0; i < pstEmcConn->ulSipMediaPdpNum; i++)
    {
        if ((IMSA_PDP_STATE_ACTIVE == pstEmcConn->astSipMediaPdpArray[i].enPdpState)
            && (pstEmcConn->astSipMediaPdpArray[i].stEpsQos.ucQCI == pstPdpActivateInd->stEpsQos.ucQCI))
        {
            if (i < (IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM - 1))
            {
                IMSA_MEM_CPY_S( &pstEmcConn->astSipMediaPdpArray[i],
                    sizeof(IMSA_PDP_CNTXT_INFO_STRU)*((IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM-1)-i),
                    &pstEmcConn->astSipMediaPdpArray[i+1],
                    sizeof(IMSA_PDP_CNTXT_INFO_STRU)*((IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM-1)-i));
            }

            IMSA_MEM_SET_S( &pstEmcConn->astSipMediaPdpArray[IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM-1],
                sizeof(IMSA_PDP_CNTXT_INFO_STRU),
                0x0,
                sizeof(IMSA_PDP_CNTXT_INFO_STRU));

            pstPdpInfo                      = &pstEmcConn->astSipMediaPdpArray[pstEmcConn->ulSipMediaPdpNum - 1];

            pstPdpInfo->ucPdpId             = pstPdpActivateInd->ucRabId;
            pstPdpInfo->enPdpState          = IMSA_PDP_STATE_ACTIVE;
            pstPdpInfo->enPdpType           = IMSA_PDP_TYPE_DEDICATED;
            pstPdpInfo->ucCid               = pstPdpActivateInd->ucCid;

            /* 存储关联的信令承载号 */
            pstPdpInfo->bitOpLinkedPdpId    = pstPdpActivateInd->bitOpLinkdRabId;
            pstPdpInfo->ucLinkedPdpId       = pstPdpActivateInd->ucLinkdRabId;

            pstPdpInfo->bitOpTft            = pstPdpActivateInd->bitOpTft;
            IMSA_MEM_CPY_S(                 &pstPdpInfo->stTft,
                                            sizeof(IMSA_PDP_TFT_INFO_STRU),
                                            &pstPdpActivateInd->stTft,
                                            sizeof(IMSA_PDP_TFT_INFO_STRU));


            if (IMSA_MAX_PF_NUM < pstPdpActivateInd->stTft.ulPfNum)
            {
                pstPdpInfo->stTft.ulPfNum = IMSA_MAX_PF_NUM;
            }

            /* 存储EPS QOS */
            IMSA_MEM_CPY_S(                 &pstPdpInfo->stEpsQos,
                                            sizeof(IMSA_PDP_EPS_QOS_STRU),
                                            &pstPdpActivateInd->stEpsQos,
                                            sizeof(IMSA_PDP_EPS_QOS_STRU));

            IMSA_INFO_LOG("IMSA_CONN_SaveMediaPdpInfoWithSameQci:get EmcConn MediaInfo by same Qci!");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_SaveMediaPdpInfoWithSameQci_ENUM, 2);

            return IMSA_SUCC;
        }
    }

    return IMSA_FAIL;
}




VOS_VOID IMSA_CONN_SaveSipMediaPdpInfo
(
    IMSA_CONN_TYPE_ENUM_UINT32                  enConnType,
    const TAF_PS_CALL_PDP_ACTIVATE_IND_STRU    *pstPdpActivateInd
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn       = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn          = VOS_NULL_PTR;
    IMSA_PDP_CNTXT_INFO_STRU           *pstPdpInfo          = VOS_NULL_PTR;

    pstNormalConn                   = IMSA_CONN_GetNormalConnAddr();
    pstEmcConn                      = IMSA_CONN_GetEmcConnAddr();

    /* 判断承载ID是否相同，如果相同，直接替换 */
    if (IMSA_SUCC == IMSA_CONN_SaveMediaPdpInfoWithSamePdpId(enConnType, pstPdpActivateInd))
    {
        TLPS_PRINT2LAYER_INFO(IMSA_CONN_SaveSipMediaPdpInfo_ENUM, 1);
        return;
    }

    /* 如果普通媒体承载个数达到最大，则替换相同QCI的媒体承载 */
    if((IMSA_CONN_TYPE_NORMAL == enConnType)
       &&(IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM <= pstNormalConn->ulSipMediaPdpNum))
    {
        /*lint -e534*/
        IMSA_CONN_SaveMediaPdpInfoWithSameQci(enConnType, pstPdpActivateInd);
        /*lint +e534*/
        TLPS_PRINT2LAYER_INFO(IMSA_CONN_SaveSipMediaPdpInfo_ENUM, 2);

        return;
    }

    /* 如果紧急媒体承载个数达到最大，则替换相同QCI的媒体承载 */
    if((IMSA_CONN_TYPE_EMC == enConnType)
       &&(IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM <= pstEmcConn->ulSipMediaPdpNum))
    {
        /*lint -e534*/
        IMSA_CONN_SaveMediaPdpInfoWithSameQci(enConnType, pstPdpActivateInd);
        /*lint +e534*/
        TLPS_PRINT2LAYER_INFO(IMSA_CONN_SaveSipMediaPdpInfo_ENUM, 3);

        return;
    }

    IMSA_INFO_LOG("IMSA_CONN_SaveSipMediaPdpInfo_ENUM is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_SaveSipMediaPdpInfo_ENUM, LNAS_ENTRY);


    if (IMSA_CONN_TYPE_NORMAL       == enConnType)
    {
        pstPdpInfo                  = &pstNormalConn->astSipMediaPdpArray[pstNormalConn->ulSipMediaPdpNum];
        pstNormalConn->ulSipMediaPdpNum++;
    }
    else
    {
        pstPdpInfo                  = &pstEmcConn->astSipMediaPdpArray[pstEmcConn->ulSipMediaPdpNum];
        pstEmcConn->ulSipMediaPdpNum++;
    }

    pstPdpInfo->ucPdpId             = pstPdpActivateInd->ucRabId;
    pstPdpInfo->enPdpState          = IMSA_PDP_STATE_ACTIVE;
    pstPdpInfo->enPdpType           = IMSA_PDP_TYPE_DEDICATED;
    pstPdpInfo->ucCid               = pstPdpActivateInd->ucCid;

    /* 存储关联的信令承载号 */
    pstPdpInfo->bitOpLinkedPdpId    = pstPdpActivateInd->bitOpLinkdRabId;
    pstPdpInfo->ucLinkedPdpId       = pstPdpActivateInd->ucLinkdRabId;

    pstPdpInfo->bitOpTft            = pstPdpActivateInd->bitOpTft;
    IMSA_MEM_CPY_S(                 &pstPdpInfo->stTft,
                                    sizeof(IMSA_PDP_TFT_INFO_STRU),
                                    &pstPdpActivateInd->stTft,
                                    sizeof(IMSA_PDP_TFT_INFO_STRU));

    if (IMSA_MAX_PF_NUM < pstPdpActivateInd->stTft.ulPfNum)
    {
        pstPdpInfo->stTft.ulPfNum = IMSA_MAX_PF_NUM;
    }

    /* 存储EPS QOS */
    IMSA_MEM_CPY_S(                 &pstPdpInfo->stEpsQos,
                                    sizeof(IMSA_PDP_EPS_QOS_STRU),
                                    &pstPdpActivateInd->stEpsQos,
                                    sizeof(IMSA_PDP_EPS_QOS_STRU));

}


VOS_VOID IMSA_CONN_ModifySipPdpInfo
(
    IMSA_CONN_SIP_PDP_TYPE_ENUM_UINT32          enSipPdpType,
    IMSA_PDP_CNTXT_INFO_STRU                   *pstPdpInfo,
    const TAF_PS_CALL_PDP_MODIFY_IND_STRU      *pstPdpModifyInd
)
{

    /* 存储EPS QOS */
    IMSA_MEM_CPY_S(     &pstPdpInfo->stEpsQos,
                        sizeof(IMSA_PDP_EPS_QOS_STRU),
                        &pstPdpModifyInd->stEpsQos,
                        sizeof(IMSA_PDP_EPS_QOS_STRU));
    if (IMSA_CONN_SIP_PDP_TYPE_SIGNAL == enSipPdpType)
    {
        /* 存储IPV4 P-CSCF */
        IMSA_MEM_CPY_S( &pstPdpInfo->stPdpIpv4Pcscf,
                        sizeof(IMSA_PDP_IPV4_PCSCF_STRU),
                        &pstPdpModifyInd->stPcscf,
                        sizeof(IMSA_PDP_IPV4_PCSCF_STRU));

        /* 存储IPV4 DNS */
        IMSA_MEM_CPY_S( &pstPdpInfo->stPdpIpv4Dns,
                        sizeof(IMSA_PDP_IPV4_DNS_STRU),
                        &pstPdpModifyInd->stDns,
                        sizeof(IMSA_PDP_IPV4_DNS_STRU));

        /* 存储IPV6 P-CSCF */
        IMSA_MEM_CPY_S( &pstPdpInfo->stPdpIpv6Pcscf,
                        sizeof(IMSA_PDP_IPV6_PCSCF_STRU),
                        &pstPdpModifyInd->stIpv6Pcscf,
                        sizeof(IMSA_PDP_IPV6_PCSCF_STRU));

        /* 存储IPV6 DNS */
        IMSA_MEM_CPY_S( &pstPdpInfo->stPdpIpv6Dns,
                        sizeof(IMSA_PDP_IPV6_DNS_STRU),
                        &pstPdpModifyInd->stIpv6Dns,
                        sizeof(IMSA_PDP_IPV6_DNS_STRU));
    }
    else if(IMSA_CONN_SIP_PDP_TYPE_MEDIA == enSipPdpType)/* 媒体承载的情况下，更新TFT信息 */
    {
        pstPdpInfo->bitOpTft            = pstPdpModifyInd->bitOpTft;
        IMSA_MEM_CPY_S(                 &pstPdpInfo->stTft,
                                        sizeof(IMSA_PDP_TFT_INFO_STRU),
                                        &pstPdpModifyInd->stTft,
                                        sizeof(IMSA_PDP_TFT_INFO_STRU));

        if (IMSA_MAX_PF_NUM < pstPdpModifyInd->stTft.ulPfNum)
        {
            pstPdpInfo->stTft.ulPfNum = IMSA_MAX_PF_NUM;
        }
    }
    else
    {
    }
}

VOS_UINT32 IMSA_CONN_IsVoiceMediaPdpType
(
    IMSA_PDP_TFT_INFO_STRU              *pstTft,
    VOS_UINT8                           ucQCI
)
{
    VOS_UINT32                          ulPfNum             = IMSA_NULL;
    VOS_UINT16                          usImsMinPort        = IMSA_NULL;
    VOS_UINT16                          usImsMaxPort        = IMSA_NULL;
    IMSA_CONTROL_MANAGER_STRU          *pstControlManager   = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_IsVoiceMediaPdpType is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_IsVoiceMediaPdpType_ENUM, LNAS_ENTRY);

    if (VOS_NULL_PTR == pstTft)
    {
        return IMSA_TRUE;
    }

    /*分配空间并检验分配是否成功*/
    pstControlManager = IMSA_GetControlManagerAddress();
    usImsMinPort = pstControlManager->stImsaConfigPara.stImsPortConfig.usImsMinPort;
    usImsMaxPort = pstControlManager->stImsaConfigPara.stImsPortConfig.usImsMaxPort;

    /* 视频包需要走A核，所以不需要配置给CDS；通过TFT的端口信息来区分承载是否是视频承载 */
    /* 如果承载有多个过滤器，仅需要判断一个本地端口就可以，网侧不会给一个过滤器分配C核端口，
    另一个分配A核端口 */
    if (0 == pstTft->ulPfNum)
    {
        return IMSA_TRUE;
    }
    for (ulPfNum = 0; ulPfNum < pstTft->ulPfNum; ulPfNum ++ )
    {
        if ((IMSA_OP_TRUE == pstTft->astTftInfo[ulPfNum].bitOpSingleLocalPort) &&
            (pstTft->astTftInfo[ulPfNum].usSingleLcPort >= usImsMinPort) &&
            (pstTft->astTftInfo[ulPfNum].usSingleLcPort <= usImsMaxPort))
        {
            return IMSA_TRUE;
        }

        if ((IMSA_OP_TRUE == pstTft->astTftInfo[ulPfNum].bitOpLocalPortRange) &&
            (pstTft->astTftInfo[ulPfNum].usLcPortLowLimit >= usImsMinPort) &&
            (pstTft->astTftInfo[ulPfNum].usLcPortHighLimit <= usImsMaxPort))
        {
            return IMSA_TRUE;
        }

        /* 如果不携带本地端口号，也不携带本地端口号范围，则通过QCI判断当前是不是语音承载 */
        if (IMSA_VOICE_QCI == ucQCI)
        {
            return IMSA_TRUE;
        }
    }

    return IMSA_FALSE;
}



IMSA_CONN_MEDIA_PDP_TYPE_ENUM_UINT32 IMSA_CONN_GetCurrMediaPdpType(VOS_UINT8 ucCid, VOS_UINT8 ucRabId)
{
    IMSA_NORMAL_CONN_STRU                  *pstNormalConn       = VOS_NULL_PTR;
    VOS_UINT32                              i;
    VOS_UINT32                              ulRslt              = IMSA_TRUE;
    IMSA_CONN_MEDIA_PDP_TYPE_ENUM_UINT32    enRelMediaPdpType   = IMSA_CONN_MEDIA_PDP_TYPE_VOICE;

    pstNormalConn = IMSA_CONN_GetNormalConnAddr();

    for (i = 0; i < pstNormalConn->ulSipMediaPdpNum; i++)
    {
        /* 根据CID和RAB ID在媒体承载列表中查找对应的承载信息 */
        if ((IMSA_PDP_STATE_ACTIVE == pstNormalConn->astSipMediaPdpArray[i].enPdpState)
            &&((ucCid == pstNormalConn->astSipMediaPdpArray[i].ucCid)
                || (ucRabId == pstNormalConn->astSipMediaPdpArray[i].ucPdpId)))
        {
            /* 判断查找到的承载是否是语音媒体承载 */
            ulRslt = IMSA_CONN_IsVoiceMediaPdpType( &pstNormalConn->astSipMediaPdpArray[i].stTft,
                                                    pstNormalConn->astSipMediaPdpArray[i].stEpsQos.ucQCI);
            if (IMSA_TRUE == ulRslt)
            {
                enRelMediaPdpType = IMSA_CONN_MEDIA_PDP_TYPE_VOICE;
            }
            else
            {
                enRelMediaPdpType = IMSA_CONN_MEDIA_PDP_TYPE_VIDEO;
            }

            return enRelMediaPdpType;
        }
    }

    /* 若没查找到, 则当前CID和RAB ID非媒体承载 */
    return IMSA_CONN_MEDIA_PDP_TYPE_BUTT;
}


VOS_VOID IMSA_CONN_SaveIpv6Info
(
    IMSA_PDP_CNTXT_INFO_STRU                   *pstPdpInfo,
    const TAF_PS_IPV6_INFO_IND_STRU            *pstIpv6InfoInd
)
{
    /* 23.401要求3GPP的IPV6前缀必须是64位
       IPv6 Stateless Address autoconfiguration specified in RFC 4862 [18] is
       the basic mechanism to allocate /64 IPv6 prefix to the UE. */

    /* 29.061要求网侧分配的IPV6前缀是无限长的，因此无需考虑IPV6前缀的有效期
       AdvValidLifetime
           Shall have a value giving Prefixes infinite lifetime, i.e. 0xFFFFFFFF.
           The assigned prefix remains Preferred until PDP Context/Bearer Deactivation.
       AdvPreferredLifetime
           Shall have a value giving Prefixes infinite lifetime, i.e. 0xFFFFFFFF.
           The assigned prefix remains Preferred until PDP Context/Bearer Deactivation.
       */
    IMSA_MEM_CPY_S( pstPdpInfo->stPdpAddr.aucIpV6Addr,
                    IMSA_IPV6_PREFIX_LEN,
                    pstIpv6InfoInd->stIpv6RaInfo.astPrefixList[0].aucPrefix,
                    IMSA_IPV6_PREFIX_LEN);

}


/*****************************************************************************
 Function Name  : IMSA_CONN_SaveIpv6InfoInHo
 Description    : IMSA CONN模块存储APS上报的IPV6信息
 Input          : pstIpv6InfoInd-----------------IPV6 INFO IND事件指针
 Output         : pstPdpInfo---------------------PDP上下文指针
 Return Value   : VOS_VOID

 History        :
      1.zhangdongfeng       2016-03-01  Draft Enact
*****************************************************************************/
VOS_VOID IMSA_CONN_SaveIpv6InfoInHo
(
    IMSA_PDP_CNTXT_INFO_STRU                   *pstPdpInfo,
    const TAF_PS_IPV6_INFO_IND_STRU            *pstIpv6InfoInd
)
{
    IMSA_CONN_HO_EVENT_STRU            *pstImsaHoEvent       = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_SaveIpv6InfoInHo is entered!");

    /* 初始化 */
    pstImsaHoEvent       = IMSA_CONN_GetHoEventAddr();

    /* 如果分配的IPv6前缀和切换前的IPV6前缀一致,则直接使用切换前分配的IPV6地址 */
    if (0 == IMSA_MEM_CMP(  pstImsaHoEvent->u.stSipSignalPdp.stPdpAddr.aucIpV6Addr,
                pstIpv6InfoInd->stIpv6RaInfo.astPrefixList[0].aucPrefix,
                IMSA_IPV6_PREFIX_LEN))
    {
        IMSA_MEM_CPY_S(   pstPdpInfo->stPdpAddr.aucIpV6Addr,
                          IMSA_WIFI_IPV6_ADDR_LEN,
                        pstImsaHoEvent->u.stSipSignalPdp.stPdpAddr.aucIpV6Addr,
                        IMSA_WIFI_IPV6_ADDR_LEN);
        return;
    }

    /* 23.401要求3GPP的IPV6前缀必须是64位
       IPv6 Stateless Address autoconfiguration specified in RFC 4862 [18] is
       the basic mechanism to allocate /64 IPv6 prefix to the UE. */

    /* 29.061要求网侧分配的IPV6前缀是无限长的，因此无需考虑IPV6前缀的有效期
       AdvValidLifetime
           Shall have a value giving Prefixes infinite lifetime, i.e. 0xFFFFFFFF.
           The assigned prefix remains Preferred until PDP Context/Bearer Deactivation.
       AdvPreferredLifetime
           Shall have a value giving Prefixes infinite lifetime, i.e. 0xFFFFFFFF.
           The assigned prefix remains Preferred until PDP Context/Bearer Deactivation.
       */
    IMSA_MEM_CPY_S(   pstPdpInfo->stPdpAddr.aucIpV6Addr,
                     IMSA_IPV6_PREFIX_LEN,
                    pstIpv6InfoInd->stIpv6RaInfo.astPrefixList[0].aucPrefix,
                    IMSA_IPV6_PREFIX_LEN);
    return;
}


VOS_UINT32 IMSA_CONN_IsPdpIpv4InfoIncludeRegPara
(
    VOS_CHAR                               *pcRegUeAddr,
    VOS_CHAR                               *pcRegPcscfAddr,
    const IMSA_PDP_CNTXT_INFO_STRU         *pstPdpInfo
)
{
    VOS_CHAR                            acPdpUeAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};
    VOS_CHAR                            acPdpPcscfAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};

    IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                        pstPdpInfo->stPdpAddr.aucIpV4Addr,
                                        acPdpUeAddr);

    if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.bitOpPrimPcscfAddr)
    {
        IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                            pstPdpInfo->stPdpIpv4Pcscf.aucPrimPcscfAddr,
                                            acPdpPcscfAddr);

        /* 比较地址对 */
        if ((0 == VOS_StrCmp(pcRegUeAddr, acPdpUeAddr))
            && (0 == VOS_StrCmp(pcRegPcscfAddr, acPdpPcscfAddr)))
        {
            return IMSA_TRUE;
        }
    }

    if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.bitOpSecPcscfAddr)
    {
        IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                            pstPdpInfo->stPdpIpv4Pcscf.aucSecPcscfAddr,
                                            acPdpPcscfAddr);

        /* 比较地址对 */
        if ((0 == VOS_StrCmp(pcRegUeAddr, acPdpUeAddr))
            && (0 == VOS_StrCmp(pcRegPcscfAddr, acPdpPcscfAddr)))
        {
            return IMSA_TRUE;
        }
    }

    if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.bitOpThiPcscfAddr)
    {
        IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                            pstPdpInfo->stPdpIpv4Pcscf.aucThiPcscfAddr,
                                            acPdpPcscfAddr);

        /* 比较地址对 */
        if ((0 == VOS_StrCmp(pcRegUeAddr, acPdpUeAddr))
            && (0 == VOS_StrCmp(pcRegPcscfAddr, acPdpPcscfAddr)))
        {
            return IMSA_TRUE;
        }
    }

    return IMSA_FALSE;
}


VOS_UINT32 IMSA_CONN_IsWifiPdpIpv4InfoIncludeRegPara
(
    VOS_CHAR                               *pcRegUeAddr,
    VOS_CHAR                               *pcRegPcscfAddr,
    const IMSA_WIFI_PDP_CNTXT_INFO_STRU         *pstPdpInfo
)
{
    VOS_CHAR                            acPdpUeAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};
    VOS_CHAR                            acPdpPcscfAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};

    IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                        (const VOS_UINT8 *) pstPdpInfo->stPdpAddr.aucIpV4Addr,
                                        acPdpUeAddr);

    if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.cOpPrimPcscfAddr)
    {
        IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                            pstPdpInfo->stPdpIpv4Pcscf.aucPrimPcscfAddr,
                                            acPdpPcscfAddr);

        /* 比较地址对 */
        if ((0 == VOS_StrCmp(pcRegUeAddr, acPdpUeAddr))
            && (0 == VOS_StrCmp(pcRegPcscfAddr, acPdpPcscfAddr)))
        {
            return IMSA_TRUE;
        }
    }

    if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.cOpSecPcscfAddr)
    {
        IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                            pstPdpInfo->stPdpIpv4Pcscf.aucSecPcscfAddr,
                                            acPdpPcscfAddr);

        /* 比较地址对 */
        if ((0 == VOS_StrCmp(pcRegUeAddr, acPdpUeAddr))
            && (0 == VOS_StrCmp(pcRegPcscfAddr, acPdpPcscfAddr)))
        {
            return IMSA_TRUE;
        }
    }

    if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv4Pcscf.cOpThiPcscfAddr)
    {
        IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV4,
                                            pstPdpInfo->stPdpIpv4Pcscf.aucThiPcscfAddr,
                                            acPdpPcscfAddr);

        /* 比较地址对 */
        if ((0 == VOS_StrCmp(pcRegUeAddr, acPdpUeAddr))
            && (0 == VOS_StrCmp(pcRegPcscfAddr, acPdpPcscfAddr)))
        {
            return IMSA_TRUE;
        }
    }

    return IMSA_FALSE;
}



VOS_UINT32 IMSA_CONN_IsPdpIpv6InfoIncludeRegPara
(
    VOS_CHAR                               *pcRegUeAddr,
    VOS_CHAR                               *pcRegPcscfAddr,
    const IMSA_PDP_CNTXT_INFO_STRU         *pstPdpInfo
)
{
    VOS_CHAR                            acPdpUeAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};
    VOS_CHAR                            acPdpPcscfAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};

    IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                            pstPdpInfo->stPdpAddr.aucIpV6Addr,
                                            acPdpUeAddr);

    if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.bitOpPrimPcscfAddr)
    {
        IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                            pstPdpInfo->stPdpIpv6Pcscf.aucPrimPcscfAddr,
                                            acPdpPcscfAddr);

        /* 比较地址对 */
        if ((0 == VOS_StrCmp(pcRegUeAddr, acPdpUeAddr))
            && (0 == VOS_StrCmp(pcRegPcscfAddr, acPdpPcscfAddr)))
        {
            return IMSA_TRUE;
        }
    }

    if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.bitOpSecPcscfAddr)
    {
        IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                            pstPdpInfo->stPdpIpv6Pcscf.aucSecPcscfAddr,
                                            acPdpPcscfAddr);

        /* 比较地址对 */
        if ((0 == VOS_StrCmp(pcRegUeAddr, acPdpUeAddr))
            && (0 == VOS_StrCmp(pcRegPcscfAddr, acPdpPcscfAddr)))
        {
            return IMSA_TRUE;
        }
    }
    if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.bitOpThiPcscfAddr)
    {
        IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                            pstPdpInfo->stPdpIpv6Pcscf.aucThiPcscfAddr,
                                            acPdpPcscfAddr);

        /* 比较地址对 */
        if ((0 == VOS_StrCmp(pcRegUeAddr, acPdpUeAddr))
            && (0 == VOS_StrCmp(pcRegPcscfAddr, acPdpPcscfAddr)))
        {
            return IMSA_TRUE;
        }
    }

    return IMSA_FALSE;
}

VOS_UINT32 IMSA_CONN_IsWifiPdpIpv6InfoIncludeRegPara
(
    VOS_CHAR                               *pcRegUeAddr,
    VOS_CHAR                               *pcRegPcscfAddr,
    const IMSA_WIFI_PDP_CNTXT_INFO_STRU         *pstPdpInfo
)
{
    VOS_CHAR                            acPdpUeAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};
    VOS_CHAR                            acPdpPcscfAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};

    /*lint -e64*/
    IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                        (VOS_UINT8 *)pstPdpInfo->stPdpAddr.aucIpV6Addr,
                                        acPdpUeAddr);
    /*lint +e64*/

    if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.cOpPrimPcscfAddr)
    {
        /*lint -e64*/
        IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                            (VOS_UINT8 *)(pstPdpInfo->stPdpIpv6Pcscf.aucPrimPcscfAddr),
                                            acPdpPcscfAddr);
        /*lint +e64*/
        /* 比较地址对 */
        if ((0 == VOS_StrCmp(pcRegUeAddr, acPdpUeAddr))
            && (0 == VOS_StrCmp(pcRegPcscfAddr, acPdpPcscfAddr)))
        {
            return IMSA_TRUE;
        }
    }

    if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.cOpSecPcscfAddr)
    {
        IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                            pstPdpInfo->stPdpIpv6Pcscf.aucSecPcscfAddr,
                                            acPdpPcscfAddr);

        /* 比较地址对 */
        if ((0 == VOS_StrCmp(pcRegUeAddr, acPdpUeAddr))
            && (0 == VOS_StrCmp(pcRegPcscfAddr, acPdpPcscfAddr)))
        {
            return IMSA_TRUE;
        }
    }
    if (IMSA_OP_TRUE == pstPdpInfo->stPdpIpv6Pcscf.cOpThiPcscfAddr)
    {
        IMSA_CONN_ConvertIpAddress2String(  IMSA_IP_TYPE_IPV6,
                                            pstPdpInfo->stPdpIpv6Pcscf.aucThiPcscfAddr,
                                            acPdpPcscfAddr);

        /* 比较地址对 */
        if ((0 == VOS_StrCmp(pcRegUeAddr, acPdpUeAddr))
            && (0 == VOS_StrCmp(pcRegPcscfAddr, acPdpPcscfAddr)))
        {
            return IMSA_TRUE;
        }
    }

    return IMSA_FALSE;
}



VOS_UINT32 IMSA_CONN_IsPdpIpvxInfoIncludeRegPara
(
    IMSA_IP_TYPE_ENUM_UINT8                 enIpType,
    VOS_CHAR                               *pcRegUeAddr,
    VOS_CHAR                               *pcRegPcscfAddr,
    const IMSA_PDP_CNTXT_INFO_STRU         *pstPdpInfo
)
{
    VOS_UINT32                          ulResult = IMSA_FALSE;

    if (IMSA_IP_TYPE_IPV4 == enIpType)
    {
        ulResult = IMSA_CONN_IsPdpIpv4InfoIncludeRegPara(pcRegUeAddr, pcRegPcscfAddr, pstPdpInfo);
        return ulResult;
    }
    else
    {
        ulResult = IMSA_CONN_IsPdpIpv6InfoIncludeRegPara(pcRegUeAddr, pcRegPcscfAddr, pstPdpInfo);
        return ulResult;
    }

}


VOS_UINT32 IMSA_CONN_IsWifiPdpIpvxInfoIncludeRegPara
(
    IMSA_IP_TYPE_ENUM_UINT8                 enIpType,
    VOS_CHAR                               *pcRegUeAddr,
    VOS_CHAR                               *pcRegPcscfAddr,
    const IMSA_WIFI_PDP_CNTXT_INFO_STRU         *pstPdpInfo
)
{
    VOS_UINT32                          ulResult = IMSA_FALSE;

    if (IMSA_IP_TYPE_IPV4 == enIpType)
    {
        ulResult = IMSA_CONN_IsWifiPdpIpv4InfoIncludeRegPara(pcRegUeAddr, pcRegPcscfAddr, pstPdpInfo);
        return ulResult;
    }
    else
    {
        ulResult = IMSA_CONN_IsWifiPdpIpv6InfoIncludeRegPara(pcRegUeAddr, pcRegPcscfAddr, pstPdpInfo);
        return ulResult;
    }

}


VOS_UINT32 IMSA_CONN_IsPdpInfoIncludeRegPara
(
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType,
    const IMSA_PDP_CNTXT_INFO_STRU         *pstPdpInfo
)
{
    VOS_CHAR                            acRegUeAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};
    VOS_CHAR                            acRegPcscfAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};
    VOS_UINT32                          ulRslt          = IMSA_FAIL;

    IMSA_IP_TYPE_ENUM_UINT8             enIpType = IMSA_IP_TYPE_BUTT;

    ulRslt = IMSA_RegGetRegedPara(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                    acRegUeAddr,
                                    acRegPcscfAddr,
                                    &enIpType);

    /* 如果未获取到注册上的参数，则直接返回不包含 */
    if (IMSA_FAIL == ulRslt)
    {
        return IMSA_FALSE;
    }

    if (IMSA_IP_TYPE_BUTT != enIpType)
    {
        if ((IMSA_IP_TYPE_IPV4V6 != pstPdpInfo->stPdpAddr.enIpType) && (enIpType != pstPdpInfo->stPdpAddr.enIpType))
        {
            return IMSA_FALSE;
        }
    }

    if ((IMSA_IP_TYPE_IPV4 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        if (IMSA_TRUE == IMSA_CONN_IsPdpIpvxInfoIncludeRegPara( IMSA_IP_TYPE_IPV4,
                                                                acRegUeAddr,
                                                                acRegPcscfAddr,
                                                                pstPdpInfo))
        {
            return IMSA_TRUE;
        }
    }

    if ((IMSA_IP_TYPE_IPV6 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        if (IMSA_TRUE == IMSA_CONN_IsPdpIpvxInfoIncludeRegPara( IMSA_IP_TYPE_IPV6,
                                                                acRegUeAddr,
                                                                acRegPcscfAddr,
                                                                pstPdpInfo))
        {
            return IMSA_TRUE;
        }
    }

    return IMSA_FALSE;
}


VOS_UINT32 IMSA_CONN_IsWifiPdpInfoIncludeRegPara
(
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType,
    const IMSA_WIFI_PDP_CNTXT_INFO_STRU     *pstPdpInfo
)
{
    VOS_CHAR                            acRegUeAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};
    VOS_CHAR                            acRegPcscfAddr[IMSA_IPV6_ADDR_STRING_LEN + 1] = {0};
    VOS_UINT32                          ulRslt          = IMSA_FAIL;

    IMSA_IP_TYPE_ENUM_UINT8             enIpType = IMSA_IP_TYPE_BUTT;

    ulRslt = IMSA_RegGetRegedPara(  (IMSA_REG_TYPE_ENUM_UINT8)enConnType,
                                    acRegUeAddr,
                                    acRegPcscfAddr,
                                    &enIpType);

    /* 如果未获取到注册上的参数，则直接返回不包含 */
    if (IMSA_FAIL == ulRslt)
    {
        return IMSA_FALSE;
    }

    if (IMSA_IP_TYPE_BUTT != enIpType)
    {
        if ((IMSA_IP_TYPE_IPV4V6 != pstPdpInfo->stPdpAddr.enIpType) && (enIpType != pstPdpInfo->stPdpAddr.enIpType))
        {
            return IMSA_FALSE;
        }
    }

    if ((IMSA_IP_TYPE_IPV4 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        if (IMSA_TRUE == IMSA_CONN_IsWifiPdpIpvxInfoIncludeRegPara( IMSA_IP_TYPE_IPV4,
                                                                acRegUeAddr,
                                                                acRegPcscfAddr,
                                                                pstPdpInfo))
        {
            return IMSA_TRUE;
        }
    }

    if ((IMSA_IP_TYPE_IPV6 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        if (IMSA_TRUE == IMSA_CONN_IsWifiPdpIpvxInfoIncludeRegPara( IMSA_IP_TYPE_IPV6,
                                                                acRegUeAddr,
                                                                acRegPcscfAddr,
                                                                pstPdpInfo))
        {
            return IMSA_TRUE;
        }
    }

    return IMSA_FALSE;
}


VOS_VOID IMSA_CONN_MaintainMediaPdpInfoWithSignalPdpDeleted
(
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType,
    VOS_UINT8                               ucLinkedPdpId
)
{
    IMSA_EMC_CONN_STRU                 *pstEmcConn       = VOS_NULL_PTR;
    IMSA_NORMAL_CONN_STRU              *pstNormalConn    = VOS_NULL_PTR;
    VOS_UINT32                          ulDeleteMediaNum = IMSA_NULL;
    VOS_UINT32                          ulLoop           = IMSA_NULL;
    VOS_UINT32                          ulIndex          = IMSA_NULL;

    /* 删除紧急信令承载时需要删除相关联的媒体承载 */
    if (IMSA_CONN_TYPE_EMC == enConnType)
    {
        pstEmcConn  = IMSA_CONN_GetEmcConnAddr();
        for (ulLoop = 0; ulLoop < pstEmcConn->ulSipMediaPdpNum; ulLoop++)
        {
            /* 从后往前遍历, 查找LinkedPdpId相关联的媒体承载 */
            ulIndex = pstEmcConn->ulSipMediaPdpNum - ulLoop - 1;

            /* 查找到相关联媒体承载 */
            if (ucLinkedPdpId == pstEmcConn->astSipMediaPdpArray[ulIndex].ucLinkedPdpId)
            {
                /* 删除对应媒体承载 */
                if (ulIndex < (IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM - 1))
                {
                    IMSA_MEM_CPY_S( &pstEmcConn->astSipMediaPdpArray[ulIndex],
                                    sizeof(IMSA_PDP_CNTXT_INFO_STRU)*((IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM - 1) - ulIndex),
                                    &pstEmcConn->astSipMediaPdpArray[ulIndex + 1],
                                    sizeof(IMSA_PDP_CNTXT_INFO_STRU)*((IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM - 1) - ulIndex));
                }

                /* 删除的媒体承载计数 */
                ulDeleteMediaNum++;
            }
        }

        /* 判断是否有关联的媒体承载被删除 */
        if (IMSA_NULL < ulDeleteMediaNum)
        {
            /* 维护媒体承载计数 */
            pstEmcConn->ulSipMediaPdpNum -= ulDeleteMediaNum;

            /* 清除被删除承载信息 */
            IMSA_MEM_SET_S(     &pstEmcConn->astSipMediaPdpArray[pstEmcConn->ulSipMediaPdpNum],
                                sizeof(IMSA_PDP_CNTXT_INFO_STRU) * (IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM - pstEmcConn->ulSipMediaPdpNum),
                                0x0,
                                sizeof(IMSA_PDP_CNTXT_INFO_STRU) * (IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM - pstEmcConn->ulSipMediaPdpNum));

            /* 通知Service模块媒体承载释放 */
            IMSA_CONN_SndConnRelInd(enConnType, IMSA_CONN_SIP_PDP_TYPE_MEDIA, IMSA_CONN_MEDIA_PDP_TYPE_OTHERS, IMSA_IMS_DOMAIN_LTE);
        }

        return;
    }

    /* 删除普通信令承载时需要删除相关联的媒体承载 */
    pstNormalConn = IMSA_CONN_GetNormalConnAddr();
    for (ulLoop = 0; ulLoop < pstNormalConn->ulSipMediaPdpNum; ulLoop++)
    {
        /* 从后往前遍历, 查找LinkedPdpId相关联的媒体承载 */
        ulIndex = pstNormalConn->ulSipMediaPdpNum - ulLoop - 1;

        /* 查找到相关联媒体承载 */
        if (ucLinkedPdpId == pstNormalConn->astSipMediaPdpArray[ulIndex].ucLinkedPdpId)
        {
            /* 删除对应媒体承载 */
            if (ulIndex < (IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM - 1))
            {
                IMSA_MEM_CPY_S( &pstNormalConn->astSipMediaPdpArray[ulIndex],
                                sizeof(IMSA_PDP_CNTXT_INFO_STRU)*((IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM - 1)- ulIndex),
                                &pstNormalConn->astSipMediaPdpArray[ulIndex + 1],
                                sizeof(IMSA_PDP_CNTXT_INFO_STRU)*((IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM - 1)- ulIndex));
            }

            /* 删除的媒体承载计数 */
            ulDeleteMediaNum++;
        }
    }

    /* 判断是否有关联的媒体承载被删除 */
    if (IMSA_NULL < ulDeleteMediaNum)
    {
        /* 维护媒体承载计数 */
        pstNormalConn->ulSipMediaPdpNum -= ulDeleteMediaNum;

        /* 清除被删除承载信息 */
        IMSA_MEM_SET_S( &pstNormalConn->astSipMediaPdpArray[pstNormalConn->ulSipMediaPdpNum],
                        sizeof(IMSA_PDP_CNTXT_INFO_STRU) * (IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM - pstNormalConn->ulSipMediaPdpNum),
                        0x0,
                        sizeof(IMSA_PDP_CNTXT_INFO_STRU) * (IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM - pstNormalConn->ulSipMediaPdpNum));

        /* 通知Service模块媒体承载释放 */
        IMSA_CONN_SndConnRelInd(enConnType, IMSA_CONN_SIP_PDP_TYPE_MEDIA, IMSA_CONN_MEDIA_PDP_TYPE_OTHERS, IMSA_IMS_DOMAIN_LTE);
    }

    return;
}


VOS_VOID IMSA_CONN_DeleteEmcPdpInfo
(
    VOS_UINT8                               ucCid,
    VOS_UINT8                               ucRabId
)
{
    IMSA_EMC_CONN_STRU                 *pstEmcConn      = VOS_NULL_PTR;
    VOS_UINT32                          i               = IMSA_NULL;

    pstEmcConn  = IMSA_CONN_GetEmcConnAddr();

    if ((IMSA_PDP_STATE_ACTIVE == pstEmcConn->stSipSignalPdp.enPdpState)
        &&((ucCid == pstEmcConn->stSipSignalPdp.ucCid)
        || (ucRabId == pstEmcConn->stSipSignalPdp.ucPdpId)))
    {
        /* 删除信令承载时需要把相关联的媒体承载删除, 防止出现某些流程异常时出现IMSA
           维护异常, 导致本地媒体承载维护未清除*/
        IMSA_CONN_MaintainMediaPdpInfoWithSignalPdpDeleted( IMSA_CONN_TYPE_EMC,
                                                            pstEmcConn->stSipSignalPdp.ucPdpId);

        if ((IMSA_IP_TYPE_IPV6 == pstEmcConn->stSipSignalPdp.stPdpAddr.enIpType) ||
            (IMSA_IP_TYPE_IPV4V6 == pstEmcConn->stSipSignalPdp.stPdpAddr.enIpType))
        {
            /* 停止等待IPV6参数定时器 */
            IMSA_CONN_StopTimer(IMSA_CONN_TYPE_EMC, TI_IMSA_WAIT_IPV6_INFO);
        }

        /* 调用REG模块删除IP地址和P-CSCF地址 */
        IMSA_CONN_DeletePdpInfo2Reg(IMSA_CONN_TYPE_EMC, &pstEmcConn->stSipSignalPdp);

        IMSA_MEM_SET_S(     &pstEmcConn->stSipSignalPdp,
                            sizeof(IMSA_PDP_CNTXT_INFO_STRU),
                            0x0,
                            sizeof(IMSA_PDP_CNTXT_INFO_STRU));

        /* 通知CDS下行过滤承载 */
        IMSA_CONN_SndCdsSetImsBearerReq(IMSA_CDS_IMS_BEARER_RAT_TYPE_3GPP_EUTRAN);

        return ;
    }

    /* 查找普通媒体承载，并删除 */
    for (i = 0; i < pstEmcConn->ulSipMediaPdpNum; i++)
    {
        if ((IMSA_PDP_STATE_ACTIVE == pstEmcConn->astSipMediaPdpArray[i].enPdpState)
            &&((ucCid == pstEmcConn->astSipMediaPdpArray[i].ucCid)
            || (ucRabId == pstEmcConn->astSipMediaPdpArray[i].ucPdpId)))
        {
            if (i < (IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM - 1))
            {
                IMSA_MEM_CPY_S( &pstEmcConn->astSipMediaPdpArray[i],
                                sizeof(IMSA_PDP_CNTXT_INFO_STRU)*((IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM-1)-i),
                                &pstEmcConn->astSipMediaPdpArray[i+1],
                                sizeof(IMSA_PDP_CNTXT_INFO_STRU)*((IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM-1)-i));
            }

            IMSA_MEM_SET_S(     &pstEmcConn->astSipMediaPdpArray[IMSA_CONN_MAX_EMC_SIP_MEDIA_PDP_NUM-1],
                                sizeof(IMSA_PDP_CNTXT_INFO_STRU),
                                0x0,
                                sizeof(IMSA_PDP_CNTXT_INFO_STRU));

            pstEmcConn->ulSipMediaPdpNum--;

            /* 通知CDS下行过滤承载 */
            IMSA_CONN_SndCdsSetImsBearerReq(IMSA_CDS_IMS_BEARER_RAT_TYPE_3GPP_EUTRAN);

            return ;
        }
    }

    return ;
}


VOS_VOID IMSA_CONN_DeletePdpInfo
(
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType,
    VOS_UINT8                               ucCid,
    VOS_UINT8                               ucRabId,
    VOS_UINT32                             *pulRegParaValid
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn   = VOS_NULL_PTR;
    /* IMSA_EMC_CONN_STRU                 *pstEmcConn      = VOS_NULL_PTR; */
    VOS_UINT32                          i               = IMSA_NULL;
    IMSA_CONN_HO_EVENT_STRU             *pstImsaHoEvent = VOS_NULL_PTR;


    pstImsaHoEvent = IMSA_CONN_GetHoEventAddr();

    *pulRegParaValid = IMSA_CONN_REG_PARA_VALID;

    if (IMSA_CONN_TYPE_EMC == enConnType)
    {
        IMSA_INFO_LOG("IMSA_CONN_DeletePdpInfo:delete emc pdp!");
        TLPS_PRINT2LAYER_INFO(IMSA_CONN_DeletePdpInfo_ENUM, IMSA_EMERGENCY);

        IMSA_CONN_DeleteEmcPdpInfo(ucCid, ucRabId);

        return ;
    }

    pstNormalConn = IMSA_CONN_GetNormalConnAddr();

    /* 查找普通信令承载，并删除 */
    for (i = 0; i < pstNormalConn->ulSipSignalPdpNum; i++)
    {
        if ((IMSA_PDP_STATE_ACTIVE == pstNormalConn->astSipSignalPdpArray[i].enPdpState)
            &&((ucCid == pstNormalConn->astSipSignalPdpArray[i].ucCid)
            || (ucRabId == pstNormalConn->astSipSignalPdpArray[i].ucPdpId)))
        {
            /* 删除信令承载时需要把相关联的媒体承载删除, 防止出现某些流程异常时出现IMSA
               维护异常, 导致本地媒体承载维护未清除*/
            if( IMSA_CONN_PDN_HO_STATE_IDLE == pstImsaHoEvent->enPdnHoState)
            {
                /* 只有非切换场景才需要释放媒体承载 */
                IMSA_CONN_MaintainMediaPdpInfoWithSignalPdpDeleted( IMSA_CONN_TYPE_NORMAL,
                                                                pstNormalConn->astSipSignalPdpArray[i].ucPdpId);
            }

            if ((IMSA_IP_TYPE_IPV6 == pstNormalConn->astSipSignalPdpArray[i].stPdpAddr.enIpType) ||
                (IMSA_IP_TYPE_IPV4V6 == pstNormalConn->astSipSignalPdpArray[i].stPdpAddr.enIpType))
            {
                /* 停止等待IPV6参数定时器 */
                IMSA_CONN_StopTimer(IMSA_CONN_TYPE_NORMAL, TI_IMSA_WAIT_IPV6_INFO);
            }

            /* 识别注册参数是否失效 */
            if (IMSA_TRUE == IMSA_CONN_IsPdpInfoIncludeRegPara( IMSA_CONN_TYPE_NORMAL,
                                                                &pstNormalConn->astSipSignalPdpArray[i]))
            {
                *pulRegParaValid = IMSA_CONN_REG_PARA_INVALID;
            }
            if( IMSA_CONN_PDN_HO_STATE_IDLE == pstImsaHoEvent->enPdnHoState)
            {
                /* 调用REG模块删除IP地址和P-CSCF地址 */
                IMSA_CONN_DeletePdpInfo2Reg(IMSA_CONN_TYPE_NORMAL, &pstNormalConn->astSipSignalPdpArray[i]);
            }

            if (i < (IMSA_CONN_MAX_NORMAL_SIP_SIGNAL_PDP_NUM - 1))
            {
                IMSA_MEM_CPY_S( &pstNormalConn->astSipSignalPdpArray[i],
                                sizeof(IMSA_PDP_CNTXT_INFO_STRU)*((IMSA_CONN_MAX_NORMAL_SIP_SIGNAL_PDP_NUM-1)-i),
                                &pstNormalConn->astSipSignalPdpArray[i+1],
                                sizeof(IMSA_PDP_CNTXT_INFO_STRU)*((IMSA_CONN_MAX_NORMAL_SIP_SIGNAL_PDP_NUM-1)-i));
            }

            IMSA_MEM_SET_S( &pstNormalConn->astSipSignalPdpArray[IMSA_CONN_MAX_NORMAL_SIP_SIGNAL_PDP_NUM-1],
                            sizeof(IMSA_PDP_CNTXT_INFO_STRU),
                            0x0,
                            sizeof(IMSA_PDP_CNTXT_INFO_STRU));

            pstNormalConn->ulSipSignalPdpNum--;

            /* zhangdongfeng -00353461:切换过程中的承载释放,需要和承载建立一起处理,避免释放顺序问题导致的误删除 */
            if( IMSA_CONN_PDN_HO_STATE_IDLE == pstImsaHoEvent->enPdnHoState)
            {
                /* 通知CDS下行过滤承载 */
                IMSA_CONN_SndCdsSetImsBearerReq(IMSA_CDS_IMS_BEARER_RAT_TYPE_3GPP_EUTRAN);
            }

            return ;
        }
    }

    /* 查找普通媒体承载，并删除 */
    for (i = 0; i < pstNormalConn->ulSipMediaPdpNum; i++)
    {
        if ((IMSA_PDP_STATE_ACTIVE == pstNormalConn->astSipMediaPdpArray[i].enPdpState)
            &&((ucCid == pstNormalConn->astSipMediaPdpArray[i].ucCid)
            || (ucRabId == pstNormalConn->astSipMediaPdpArray[i].ucPdpId)))
        {
            if (i < (IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM - 1))
            {
                IMSA_MEM_CPY_S( &pstNormalConn->astSipMediaPdpArray[i],
                                sizeof(IMSA_PDP_CNTXT_INFO_STRU)*((IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM-1)-i),
                                &pstNormalConn->astSipMediaPdpArray[i+1],
                                sizeof(IMSA_PDP_CNTXT_INFO_STRU)*((IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM-1)-i));
            }

            IMSA_MEM_SET_S(     &pstNormalConn->astSipMediaPdpArray[IMSA_CONN_MAX_NORMAL_SIP_MEDIA_PDP_NUM-1],
                                sizeof(IMSA_PDP_CNTXT_INFO_STRU),
                                0x0,
                                sizeof(IMSA_PDP_CNTXT_INFO_STRU));

            pstNormalConn->ulSipMediaPdpNum--;

            /* zhangdongfeng -00353461:切换过程中的承载释放,需要和承载建立一起处理,避免释放顺序问题导致的误删除 */
            if( IMSA_CONN_PDN_HO_STATE_IDLE == pstImsaHoEvent->enPdnHoState)
            {
                /* 通知CDS下行过滤承载 */
                IMSA_CONN_SndCdsSetImsBearerReq(IMSA_CDS_IMS_BEARER_RAT_TYPE_3GPP_EUTRAN);
            }

            return ;
        }
    }

    return ;
}

VOS_UINT32 IMSA_CONN_IsExitPersistentBearer
(
    VOS_VOID
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn   = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                  *pstEmclConn   = VOS_NULL_PTR;
    VOS_UINT32                          i               = IMSA_NULL;

    IMSA_INFO_LOG("IMSA_CONN_IsExitPersistentBearer:enter!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_IsExitPersistentBearer_ENUM, LNAS_ENTRY);


    pstNormalConn = IMSA_CONN_GetNormalConnAddr();
    pstEmclConn = IMSA_CONN_GetEmcConnAddr();

    /* 查找普通媒体承载，并删除 */
    for (i = 0; i < pstNormalConn->ulSipMediaPdpNum; i++)
    {
        if (IMSA_PDP_STATE_ACTIVE == pstNormalConn->astSipMediaPdpArray[i].enPdpState)
        {
            return IMSA_TRUE;
        }
    }

    if (IMSA_PDP_STATE_ACTIVE == pstEmclConn->stSipSignalPdp.enPdpType)
    {
        return IMSA_TRUE;
    }

    return IMSA_FALSE;
}




VOS_UINT32 IMSA_CONN_IsEmcConnExist
(
    VOS_VOID
)
{
    IMSA_EMC_CONN_STRU                  *pstEmclConn   = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_IsExitEmcConn:enter!");

    pstEmclConn = IMSA_CONN_GetEmcConnAddr();

    /* 查找是否存在紧急连接承载　*/
    if(IMSA_CONN_STATUS_IDLE != pstEmclConn->enImsaConnStatus)
    {
        return IMSA_TRUE;
    }

    return IMSA_FALSE;
}


VOS_VOID IMSA_CONN_SndConnSetupInd
(
    IMSA_CONN_RESULT_ENUM_UINT32            enResult,
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType,
    IMSA_CONN_SIP_PDP_TYPE_ENUM_UINT32      enSipPdpType,
    IMSA_IMS_DOMAIN_ENUM_UINT8               enConnRatType
)
{
    IMSA_CONN_SETUP_IND_STRU           *pstConnSetupInd  = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_SndConnSetupInd is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_SndConnSetupInd_ENUM, LNAS_ENTRY);

    /*分配空间并检验分配是否成功*/
    pstConnSetupInd = (VOS_VOID*)IMSA_GetIntraMsgBuffAddr(sizeof(IMSA_CONN_SETUP_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstConnSetupInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_CONN_SndConnSetupInd:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_CONN_SndConnSetupInd_ENUM, LNAS_NULL_PTR);
        return ;
    }

    /*清空*/
    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstConnSetupInd),
                    IMSA_GET_MSG_LENGTH(pstConnSetupInd),
                    0,
                    IMSA_GET_MSG_LENGTH(pstConnSetupInd));

    /*填写消息头*/
    IMSA_WRITE_INTRA_MSG_HEAD(  pstConnSetupInd,
                                ID_IMSA_CONN_SETUP_IND,
                                sizeof(IMSA_CONN_SETUP_IND_STRU));

    /*填写响应结果*/
    pstConnSetupInd->enResult       = enResult;
    pstConnSetupInd->enConnType     = enConnType;
    pstConnSetupInd->enSipPdpType   = enSipPdpType;

    pstConnSetupInd->enConnRatType  = enConnRatType;

    /*调用消息发送函数 */
    IMSA_SEND_INTRA_MSG(pstConnSetupInd);
}

VOS_VOID IMSA_CONN_SndConnMediaSetupInd
(
    IMSA_CONN_RESULT_ENUM_UINT32            enResult,
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType,
    IMSA_CONN_MEDIA_PDP_TYPE_ENUM_UINT32    enMediaPdpType
)
{
    IMSA_CONN_SETUP_IND_STRU           *pstConnSetupInd  = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_SndConnMediaSetupInd is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_SndConnMediaSetupInd_ENUM, LNAS_ENTRY);

    /*分配空间并检验分配是否成功*/
    pstConnSetupInd = (VOS_VOID*)IMSA_GetIntraMsgBuffAddr(sizeof(IMSA_CONN_SETUP_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstConnSetupInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_CONN_SndConnSetupInd:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_CONN_SndConnMediaSetupInd_ENUM, LNAS_NULL_PTR);
        return ;
    }

    /*清空*/
    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstConnSetupInd),
                    IMSA_GET_MSG_LENGTH(pstConnSetupInd),
                    0,
                    IMSA_GET_MSG_LENGTH(pstConnSetupInd));

    /*填写消息头*/
    IMSA_WRITE_INTRA_MSG_HEAD(  pstConnSetupInd,
                                ID_IMSA_CONN_SETUP_IND,
                                sizeof(IMSA_CONN_SETUP_IND_STRU));

    /*填写响应结果*/
    pstConnSetupInd->enResult       = enResult;
    pstConnSetupInd->enConnType     = enConnType;
    pstConnSetupInd->enSipPdpType   = IMSA_CONN_SIP_PDP_TYPE_MEDIA;
    pstConnSetupInd->enMediaPdpType = enMediaPdpType;

    pstConnSetupInd->enConnRatType  = IMSA_IMS_DOMAIN_LTE;

    /*调用消息发送函数 */
    IMSA_SEND_INTRA_MSG(pstConnSetupInd);
}


VOS_VOID IMSA_CONN_SndConnMediaPdpModifyInd
(
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType,
    IMSA_CONN_MEDIA_PDP_TYPE_ENUM_UINT32    enMeiadModifyType
)
{
    IMSA_CONN_MODIFY_IND_STRU           *pstConnModifyInd  = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_SndConnMediaPdpModifyInd is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_SndConnMediaPdpModifyInd_ENUM, LNAS_ENTRY);

    /*分配空间并检验分配是否成功*/
    pstConnModifyInd = (VOS_VOID*)IMSA_GetIntraMsgBuffAddr(sizeof(IMSA_CONN_MODIFY_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstConnModifyInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_CONN_SndConnMediaPdpModifyInd:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_CONN_SndConnMediaPdpModifyInd_ENUM, LNAS_NULL_PTR);
        return ;
    }

    /*清空*/
    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstConnModifyInd),
                    IMSA_GET_MSG_LENGTH(pstConnModifyInd),
                    0,
                    IMSA_GET_MSG_LENGTH(pstConnModifyInd));

    /*填写消息头*/
    IMSA_WRITE_INTRA_MSG_HEAD(  pstConnModifyInd,
                                ID_IMSA_CONN_MODIFY_IND,
                                sizeof(IMSA_CONN_MODIFY_IND_STRU));

    /*填写响应结果*/
    pstConnModifyInd->enConnType    = enConnType;
    pstConnModifyInd->enSipPdpType  = IMSA_CONN_SIP_PDP_TYPE_MEDIA;

    pstConnModifyInd->bitOpMediaModifyType = IMSA_OP_TRUE;
    pstConnModifyInd->enMediaPdpType  = enMeiadModifyType;

    /*调用消息发送函数 */
    IMSA_SEND_INTRA_MSG(pstConnModifyInd);
}


VOS_VOID IMSA_CONN_SndConnRegParaInvalid( VOS_VOID )
{
    IMSA_CONN_MODIFY_IND_STRU           *pstConnModifyInd  = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_SndConnRegParaInvalid is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_SndConnRegParaInvalid_ENUM, LNAS_ENTRY);

    /*分配空间并检验分配是否成功*/
    pstConnModifyInd = (VOS_VOID*)IMSA_GetIntraMsgBuffAddr(sizeof(IMSA_CONN_MODIFY_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstConnModifyInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_CONN_SndConnMediaPdpModifyInd:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_CONN_SndConnRegParaInvalid_ENUM, LNAS_NULL_PTR);
        return ;
    }

    /*清空*/
    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstConnModifyInd),
                    IMSA_GET_MSG_LENGTH(pstConnModifyInd),
                    0,
                    IMSA_GET_MSG_LENGTH(pstConnModifyInd));

    /*填写消息头*/
    IMSA_WRITE_INTRA_MSG_HEAD(  pstConnModifyInd,
                                ID_IMSA_CONN_MODIFY_IND,
                                sizeof(IMSA_CONN_MODIFY_IND_STRU));

    /*填写响应结果*/
    pstConnModifyInd->enConnType            = IMSA_CONN_TYPE_NORMAL;
    pstConnModifyInd->enSipPdpType          = IMSA_CONN_SIP_PDP_TYPE_SIGNAL;
    pstConnModifyInd->bitOpModifyType       = IMSA_OP_TRUE;
    pstConnModifyInd->enModifyType          = IMSA_CONN_MODIFY_TYPE_REG_PARA_INVALID;

    /*调用消息发送函数 */
    IMSA_SEND_INTRA_MSG(pstConnModifyInd);
}


VOS_VOID IMSA_CONN_SndConnRegPcscfInvalid( IMSA_CONN_TYPE_ENUM_UINT32 enConnType )
{
    IMSA_CONN_MODIFY_IND_STRU           *pstConnModifyInd  = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_SndConnRegParaInvalid is entered!");

    /*分配空间并检验分配是否成功*/
    pstConnModifyInd = (VOS_VOID*)IMSA_GetIntraMsgBuffAddr(sizeof(IMSA_CONN_MODIFY_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstConnModifyInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_CONN_SndConnMediaPdpModifyInd:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstConnModifyInd),
                    IMSA_GET_MSG_LENGTH(pstConnModifyInd),
                    0,
                    IMSA_GET_MSG_LENGTH(pstConnModifyInd));

    /*填写消息头*/
    IMSA_WRITE_INTRA_MSG_HEAD(  pstConnModifyInd, ID_IMSA_CONN_MODIFY_IND, sizeof(IMSA_CONN_MODIFY_IND_STRU));

    /*填写响应结果*/
    pstConnModifyInd->enConnType            = enConnType;
    pstConnModifyInd->enSipPdpType          = IMSA_CONN_SIP_PDP_TYPE_SIGNAL;
    pstConnModifyInd->bitOpModifyType       = IMSA_OP_TRUE;
    pstConnModifyInd->enModifyType          = IMSA_CONN_MODIFY_TYPE_PCSCF_INVALID;

    /* 调用消息发送函数 */
    IMSA_SEND_INTRA_MSG(pstConnModifyInd);

}



/*lint -e18*/
VOS_VOID IMSA_CONN_SndConnRelInd
(
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType,
    IMSA_CONN_SIP_PDP_TYPE_ENUM_UINT32      enSipPdpType,
    IMSA_CONN_MEDIA_PDP_TYPE_ENUM_UINT32    enRelMediaPdpType,
    IMSA_IMS_DOMAIN_ENUM_UINT8              enConnRatType
)
{
    IMSA_CONN_REL_IND_STRU             *pstConnRelInd  = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_SndConnRelInd is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_SndConnRelInd_ENUM, LNAS_ENTRY);

    /*分配空间并检验分配是否成功*/
    pstConnRelInd = (VOS_VOID*)IMSA_GetIntraMsgBuffAddr(sizeof(IMSA_CONN_REL_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstConnRelInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_CONN_SndConnRelInd:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_CONN_SndConnRelInd_ENUM, LNAS_NULL_PTR);
        return ;
    }

    /*清空*/
    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstConnRelInd),
                    IMSA_GET_MSG_LENGTH(pstConnRelInd),
                    0,
                    IMSA_GET_MSG_LENGTH(pstConnRelInd));

    /*填写消息头*/
    IMSA_WRITE_INTRA_MSG_HEAD(  pstConnRelInd,
                                ID_IMSA_CONN_REL_IND,
                                sizeof(IMSA_CONN_REL_IND_STRU));

    /*填写响应结果*/
    pstConnRelInd->enConnType     = enConnType;
    pstConnRelInd->enSipPdpType   = enSipPdpType;

    pstConnRelInd->enRelMediaPdpType   = enRelMediaPdpType;

    pstConnRelInd->enConnRatType  = enConnRatType;

    /*调用消息发送函数 */
    IMSA_SEND_INTRA_MSG(pstConnRelInd);
}




VOS_UINT8 IMSA_CONN_IsIpv4v6InLteConn
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType
)
{
    VOS_UINT8     i;
    IMSA_NORMAL_CONN_STRU              *pstNormalConn   = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn      = VOS_NULL_PTR;
    VOS_UINT8                           ucIpv4Flag =0;
    VOS_UINT8                           ucIpv6Flag =0;

    pstNormalConn = IMSA_CONN_GetNormalConnAddr();
    pstEmcConn    = IMSA_CONN_GetEmcConnAddr();

    /* 如果是紧急连接 */
    /*lint -e539*/
    if( IMSA_CONN_TYPE_EMC == enConnType)
    {
        if( (IMSA_CONN_STATUS_CONN == pstEmcConn->enImsaConnStatus)
            && (IMSA_IP_TYPE_IPV4V6 == pstEmcConn->stSipSignalPdp.stPdpAddr.enIpType))
        {
            return IMSA_TRUE;
        }

        return IMSA_FALSE;
    }
    /*lint +e539*/
    /*lint -e539*/
    /* 如果是普通连接 */
    if( pstNormalConn->ulSipSignalPdpNum >IMSA_CONN_MAX_NORMAL_SIP_SIGNAL_PDP_NUM)
    {
        pstNormalConn->ulSipSignalPdpNum = IMSA_CONN_MAX_NORMAL_SIP_SIGNAL_PDP_NUM;
    }

    for(i=0; i<pstNormalConn->ulSipSignalPdpNum; i++)
    {
        if( IMSA_PDP_STATE_ACTIVE == pstNormalConn->astSipSignalPdpArray[i].enPdpState)
        {
            if( IMSA_IP_TYPE_IPV4 == pstNormalConn->astSipSignalPdpArray[i].stPdpAddr.enIpType)
            {
                ucIpv4Flag = 1;
            }
            else if( IMSA_IP_TYPE_IPV6 == pstNormalConn->astSipSignalPdpArray[i].stPdpAddr.enIpType)
            {
                ucIpv6Flag = 1;
            }
            else if( IMSA_IP_TYPE_IPV4V6 == pstNormalConn->astSipSignalPdpArray[i].stPdpAddr.enIpType)
            {
                ucIpv6Flag = 1;
                ucIpv4Flag = 1;
            }
        }
    }
    /*lint +e539*/
    /*lint -e539*/
    if( (1 == ucIpv6Flag)
        &&(1 == ucIpv4Flag))
    {
        return IMSA_TRUE;
    }
    /*lint +e539*/
    return IMSA_FALSE;
}



VOS_VOID IMSA_CONN_ProcReEstablishConnSetupReq
(
    const IMSA_CONN_SETUP_REQ_STRU     *pstConnSetupReqMsg
)
{
    IMSA_CONN_STATUS_ENUM_UINT8         enConnStatus        = IMSA_CONN_STATUS_BUTT;
    TAF_SDF_PARA_QUERY_INFO_STRU       *pstSdfQueryInfo     = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt              = IMSA_FAIL;
    /* TAF_PS_DIAL_PARA_STRU              *pstDialParaInfo     = VOS_NULL_PTR; */
    VOS_UINT8                           ucOpid              = IMSA_NULL;
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType          = pstConnSetupReqMsg->enConnType;
    VOS_UINT32                          ulFoundIndex        = IMSA_NULL;
    /* IMSA_SEL_SDF_PARA_STRU             *pstSelectedSdfPara  = VOS_NULL_PTR; */
    IMSA_CONTROL_MANAGER_STRU          *pstImsaControlManager   = VOS_NULL_PTR;
    VOS_UINT8                           ucCid               = IMSA_FAIL;
    IMSA_IP_TYPE_ENUM_UINT8             enIpType            = IMSA_FAIL;

    IMSA_INFO_LOG("IMSA_CONN_ProcReEstablishConnSetupReq is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_ProcReEstablishConnSetupReq_ENUM, LNAS_ENTRY);

    pstImsaControlManager   = IMSA_GetControlManagerAddress();

    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        enConnStatus = IMSA_CONN_GetNormalConnStatus();

        ucCid= pstImsaControlManager->stNrmRegParaInfo.ucCid;

        enIpType = pstImsaControlManager->stNrmRegParaInfo.enIpType;
    }
    else
    {
        enConnStatus = IMSA_CONN_GetEmcConnStatus();

        ucCid= pstImsaControlManager->stEmcRegParaInfo.ucCid;

        enIpType = pstImsaControlManager->stEmcRegParaInfo.enIpType;
    }

    /* 如果在CONNING态，则丢弃 */
    /* coverity[cond_cannot_single]*/
    if (IMSA_CONN_STATUS_CONNING == enConnStatus)
    {
        return ;
    }

    /* 如果在CONN态，则直接返回连接建立成功 */
    /* coverity[cond_cannot_set]*/
    if (IMSA_CONN_STATUS_CONN == enConnStatus)
    {
        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_SUCCESS,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_IMS_DOMAIN_LTE);

        return ;
    }

    /* 如果在RELEASING态，则直接回复连接建立失败，原因值为IMSA_CONN_RESULT_FAIL_CONN_RELEASING */
    /* coverity[cond_cannot_set]*/
    if (IMSA_CONN_STATUS_RELEASING == enConnStatus)
    {
        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_CONN_RELEASING,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_IMS_DOMAIN_LTE);

        return ;
    }

    pstSdfQueryInfo = IMSA_MEM_ALLOC(sizeof(TAF_SDF_PARA_QUERY_INFO_STRU));

    if (VOS_NULL_PTR == pstSdfQueryInfo)
    {
        /*打印不合法信息*/
        IMSA_ERR_LOG("IMSA_CONN_ProcReEstablishConnSetupReq:ERROR: Mem alloc fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_CONN_ProcReEstablishConnSetupReq_ENUM, LNAS_NULL_PTR);

        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_OHTERS,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_IMS_DOMAIN_LTE);

        return;
    }

    /* 获取所有NV项中的SDF配置信息 */
    IMSA_CONN_AssignOpid(enConnType, &ucOpid);
    ulRslt = TAF_PS_GetCidSdfParaInfo(PS_PID_IMSA, IMSA_CLIENT_ID, ucOpid, pstSdfQueryInfo);
    if (VOS_OK != ulRslt)
    {
        /*打印不合法信息*/
        IMSA_ERR_LOG("IMSA_CONN_ProcReEstablishConnSetupReq:TAF_PS_GetCidSdfParaInfo fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_CONN_ProcReEstablishConnSetupReq_ENUM, IMSA_CALL_API_ERROR);

        IMSA_MEM_FREE(pstSdfQueryInfo);

        /* 如果获取SDF信息失败，且存在其他PDN的情况下，直接返回成功，SERVICE模块可以判断
        出IP地址发生变化，会本地去注册，然后再重新发起注册 */
        if (IMSA_TRUE == IMSA_CONN_HasActiveSipSignalPdp(enConnType))
        {
            IMSA_CONN_SetupConnSucc(enConnType,IMSA_IMS_DOMAIN_LTE);

        }
        else
        {
            IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_OHTERS,
                                        enConnType,
                                        IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                        IMSA_IMS_DOMAIN_LTE);
        }
        return;
    }
    ulRslt = IMSA_CONN_FindSipSignalDialPara(   enConnType,
                                                pstSdfQueryInfo->ulSdfNum,
                                                pstSdfQueryInfo->astSdfPara,
                                                &ulFoundIndex);

    if (IMSA_FAIL == ulRslt)
    {
        /*打印不合法信息*/
        IMSA_ERR_LOG("IMSA_CONN_ProcReEstablishConnSetupReq:ERROR: Err Para!");
        TLPS_PRINT2LAYER_ERROR(IMSA_CONN_ProcReEstablishConnSetupReq_ENUM, 1);

        IMSA_MEM_FREE(pstSdfQueryInfo);

        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_PARA_ERR,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_IMS_DOMAIN_LTE);


        return;
    }
    /* lint -e429*/
    /* coverity[cannot_set]*/
    /* coverity[dead_error_condition]*/
    if (((TAF_PDP_IPV4 == pstSdfQueryInfo->astSdfPara[ulFoundIndex].enPdnType)
        || (TAF_PDP_IPV6 == pstSdfQueryInfo->astSdfPara[ulFoundIndex].enPdnType))
        && (IMSA_CONN_STATUS_CONN == enConnStatus))
    {
        /* coverity[dead_error_begin]*/
        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_SUCCESS,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_IMS_DOMAIN_LTE);
        IMSA_MEM_FREE(pstSdfQueryInfo);
        return ;
    }
    /* lint +e429*/
    /* lint -e429*/
    if ((TAF_PDP_IPV4V6 == pstSdfQueryInfo->astSdfPara[ulFoundIndex].enPdnType)
        && IMSA_TRUE == IMSA_CONN_IsIpv4v6InLteConn(enConnType))
    {
        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_SUCCESS,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_IMS_DOMAIN_LTE);
        IMSA_MEM_FREE(pstSdfQueryInfo);
        return;
    }
    /* lint +e429*/

    /* 查找拨号参数，如果是IPV4V6类型，且当前无激活承载，则直接发起链接请求 */
    if (TAF_PDP_IPV4V6 == pstSdfQueryInfo->astSdfPara[ulFoundIndex].enPdnType)
    {
        if (IMSA_FALSE == IMSA_CONN_HasActiveSipSignalPdp(enConnType))
        {
            enIpType = IMSA_IP_TYPE_IPV4V6;
        }
        /* 如果是V4V6类型，且当前有其他激活的承载，则需要判断CID和备份的CID是否相同，如果相同，则
       直接用当前的CID发起链接请求；否则，需要重新获取未使用的CID，重新发起拨号请求 */
        else if (ucCid != pstSdfQueryInfo->astSdfPara[ulFoundIndex].ucCid)
        {
            /* 查找一个未激活的CID进行激活 */
            IMSA_CONN_AssignOpid(enConnType, &ucOpid);

            if (VOS_OK != TAF_PS_GetUnusedCid(PS_PID_IMSA, IMSA_CLIENT_ID, ucOpid, &ucCid))
            {
                IMSA_WARN_LOG("IMSA_CONN_ProcReEstablishConnSetupReq:get cid failed!");
                IMSA_MEM_FREE(pstSdfQueryInfo);

                TLPS_PRINT2LAYER_ERROR(IMSA_CONN_ProcReEstablishConnSetupReq_ENUM, LNAS_FUNCTION_LABEL2);

                IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_PARA_ERR,
                                            enConnType,
                                            IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                            IMSA_IMS_DOMAIN_LTE);
                return ;
            }

        }
    }


    /* 将PDN类型修改为备份的PDN类型 */
    pstSdfQueryInfo->astSdfPara[ulFoundIndex].enPdnType = enIpType;

    /* 存储选择的SDF参数 */
    IMSA_CONN_SaveSelectedSdfPara(  enConnType,
                                    &pstSdfQueryInfo->astSdfPara[ulFoundIndex]);

    IMSA_MEM_FREE(pstSdfQueryInfo);

    /* pstSelectedSdfPara = IMSA_CONN_GetNormalConnSelSdfParaAddr(); */


    ulRslt = IMSA_CONN_SipSignalPdpActOrig(enConnType,
                                           ucCid,
                                           enIpType);

    if (IMSA_FAIL== ulRslt)
    {
        /* 如果拨号失败，且存在其他PDN的情况下，直接返回成功，SERVICE模块可以判断
        出IP地址发生变化，会本地去注册，然后再重新发起注册 */
        if (IMSA_TRUE == IMSA_CONN_HasActiveSipSignalPdp(enConnType))
        {
            IMSA_CONN_SetupConnSucc(enConnType,IMSA_IMS_DOMAIN_LTE);

        }
        else
        {
            IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_OHTERS,
                                        enConnType,
                                        IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                        IMSA_IMS_DOMAIN_LTE);
        }
        return;
    }

    /* 连接状态转到CONNING状态 */
    IMSA_CONN_SetConnStatus(enConnType, IMSA_CONN_STATUS_CONNING, IMSA_IMS_DOMAIN_LTE);

    /* 存储最初请求的PDN类型为备份PDN类型 */
    IMSA_CONN_SaveFirstReqPdnType(enConnType, enIpType);

}


VOS_VOID IMSA_CONN_ProcWifiConnSetupReq
(
    const IMSA_CONN_SETUP_REQ_STRU     *pstConnSetupReqMsg
)
{
    IMSA_CONN_STATUS_ENUM_UINT8         enConnStatus        = IMSA_CONN_STATUS_BUTT;
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType          = pstConnSetupReqMsg->enConnType;
    TAF_SDF_PARA_QUERY_INFO_STRU       *pstSdfQueryInfo     = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt              = IMSA_FAIL;
    /*TAF_PS_DIAL_PARA_STRU              *pstDialParaInfo     = VOS_NULL_PTR; */
    VOS_UINT8                           ucOpid              = IMSA_NULL;
    /*IMSA_CONN_TYPE_ENUM_UINT32          enConnType          = pstConnSetupReqMsg->enConnType;*/
    VOS_UINT32                          ulFoundIndex        = IMSA_NULL;
    IMSA_SEL_SDF_PARA_STRU             *pstSelectedSdfPara  = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_ProcWifiConnSetupReq is entered!");

    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        enConnStatus = IMSA_CONN_GetWifiNormalConnStatus();
        IMSA_INFO_LOG1("IMSA_CONN_ProcWifiConnSetupReq enConnStatus = !",enConnStatus);
    }
    else
    {
        /* todo */

        return;

    }

    /* 如果在CONN态，则直接返回连接建立成功 */
    if (IMSA_CONN_STATUS_CONN == enConnStatus)
    {
        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_SUCCESS,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_IMS_DOMAIN_WIFI);

        return ;
    }

    /* 如果在CONNING态，则丢弃 */
    if (IMSA_CONN_STATUS_CONNING == enConnStatus)
    {
        return ;
    }

    /* 如果在RELEASING态，则直接回复连接建立失败，原因值为IMSA_CONN_RESULT_FAIL_CONN_RELEASING */
    if (IMSA_CONN_STATUS_RELEASING == enConnStatus)
    {
        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_CONN_RELEASING,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_IMS_DOMAIN_WIFI);

        return ;
    }

    /* 如果在IDLE态，则发起连接建立流程 */

    pstSdfQueryInfo = IMSA_MEM_ALLOC(sizeof(TAF_SDF_PARA_QUERY_INFO_STRU));

    if (VOS_NULL_PTR == pstSdfQueryInfo)
    {
        /*打印不合法信息*/
        IMSA_ERR_LOG("IMSA_CONN_ProcNormalConnSetupReq:ERROR: Mem alloc fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_CONN_ProcConnSetupReq_ENUM, LNAS_NULL_PTR);

        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_OHTERS,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_IMS_DOMAIN_WIFI);

        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_PdnRejErrRecord(IMSA_ERR_LOG_PDNREJ_CAUSE_IMSA_OTHERS);
        #endif
        return;
    }

    /* 获取所有NV项中的SDF配置信息 */
    IMSA_CONN_AssignOpid(enConnType, &ucOpid);
    ulRslt = TAF_PS_GetCidSdfParaInfo(PS_PID_IMSA, IMSA_CLIENT_ID, ucOpid, pstSdfQueryInfo);
    if (VOS_OK != ulRslt)
    {
        /*打印不合法信息*/
        IMSA_ERR_LOG("IMSA_CONN_ProcNormalConnSetupReq:TAF_PS_GetCidSdfParaInfo fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_CONN_ProcConnSetupReq_ENUM, IMSA_CALL_API_ERROR);

        IMSA_MEM_FREE(pstSdfQueryInfo);

        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_OHTERS,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_IMS_DOMAIN_WIFI);

        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_PdnRejErrRecord(IMSA_ERR_LOG_PDNREJ_CAUSE_IMSA_OTHERS);
        #endif

        return;
    }

    ulRslt = IMSA_CONN_FindSipSignalDialPara(   enConnType,
                                                pstSdfQueryInfo->ulSdfNum,
                                                pstSdfQueryInfo->astSdfPara,
                                                &ulFoundIndex);

    if (IMSA_FAIL == ulRslt)
    {
        /*打印不合法信息*/
        IMSA_ERR_LOG("IMSA_CONN_ProcNormalConnSetupReq:ERROR: Err Para!");
        TLPS_PRINT2LAYER_ERROR(IMSA_CONN_ProcConnSetupReq_ENUM, LNAS_PARAM_INVALID);

        IMSA_MEM_FREE(pstSdfQueryInfo);

        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_PARA_ERR,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_IMS_DOMAIN_LTE);

        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_PdnRejErrRecord(IMSA_ERR_LOG_PDNREJ_CAUSE_IMSA_PARA_ERR);
        /* 暂时先不删除旧的CHR，因为产品线对新增的CHR还未增加解析，等产品线新增后，
        再将原来的删除 */
        IMSA_PdnConnFailErrLogRecord(   enConnType,
                                        IMSA_ERR_LOG_NORM_PDNREJ_CAUSE_NO_IMS_PDP_CONTXT,
                                        IMSA_ERR_LOG_PDN_CONN_NW_REJ_CAUSE_NONE,
                                        IMSA_ERR_LOG_EMC_REG_FAIL_REASON_NO_EMC_PDP_CONTEXT);
        #endif

        return;
    }

    /* 存储选择的SDF参数 */
    IMSA_CONN_SaveSelectedWifiSdfPara(  enConnType,
                                    &pstSdfQueryInfo->astSdfPara[ulFoundIndex]);



    pstSelectedSdfPara = IMSA_CONN_GetWifiNormalConnSelSdfParaAddr();


    /* 如果在IDLE状态，则通知WIFI，发起建链请求  */
    IMSA_CONN_SndWifiMsgPdnActivateReq( pstSelectedSdfPara->bitOpApn,
                                        (IMSA_WIFI_PDP_APN_STRU *)&(pstSelectedSdfPara->stApnInfo),
                                        (IMSA_WIFI_IP_TYPE_ENUM_INT8)(pstSelectedSdfPara->enPdnType),
                                         VOS_FALSE);

    /* 启动IMS拨号定时器 */
    IMSA_CONN_StartTimer(enConnType, TI_IMSA_WAIT_WIFI_PDP_ACTIVE_CNF);

    /* 连接状态转到CONNING状态 */
    IMSA_CONN_SetWifiConnStatus(enConnType, IMSA_CONN_STATUS_CONNING);

    IMSA_MEM_FREE(pstSdfQueryInfo);

    return;

}

VOS_VOID IMSA_CONN_ProcLteConnSetupReq
(
    const IMSA_CONN_SETUP_REQ_STRU     *pstConnSetupReqMsg
)
{
    IMSA_CONN_STATUS_ENUM_UINT8         enConnStatus        = IMSA_CONN_STATUS_BUTT;
    TAF_SDF_PARA_QUERY_INFO_STRU       *pstSdfQueryInfo     = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt              = IMSA_FAIL;
    TAF_PS_DIAL_PARA_STRU              *pstDialParaInfo     = VOS_NULL_PTR;
    VOS_UINT8                           ucOpid              = IMSA_NULL;
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType          = pstConnSetupReqMsg->enConnType;
    VOS_UINT32                          ulFoundIndex        = IMSA_NULL;
    IMSA_SEL_SDF_PARA_STRU             *pstSelectedSdfPara  = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_ProcLteConnSetupReq is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_ProcConnSetupReq_ENUM, LNAS_ENTRY);

    if (IMSA_TRUE == pstConnSetupReqMsg->ulReEstablishFlag)
    {
        IMSA_CONN_ProcReEstablishConnSetupReq(pstConnSetupReqMsg);
        return;
    }

    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        enConnStatus = IMSA_CONN_GetNormalConnStatus();
    }
    else
    {
        enConnStatus = IMSA_CONN_GetEmcConnStatus();
    }

    /* 如果在CONN态，则直接返回连接建立成功 */
    if (IMSA_CONN_STATUS_CONN == enConnStatus)
    {
        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_SUCCESS,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_IMS_DOMAIN_LTE);

        return ;
    }

    /* 如果在CONNING态，则丢弃 */
    if (IMSA_CONN_STATUS_CONNING == enConnStatus)
    {
        return ;
    }

    /* 如果在RELEASING态，则直接回复连接建立失败，原因值为IMSA_CONN_RESULT_FAIL_CONN_RELEASING */
    if (IMSA_CONN_STATUS_RELEASING == enConnStatus)
    {
        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_CONN_RELEASING,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_IMS_DOMAIN_LTE);

        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_PdnRejErrRecord(IMSA_ERR_LOG_PDNREJ_CAUSE_IMSA_CONN_RELEASING);

        /* 暂时先不删除旧的CHR，因为产品线对新增的CHR还未增加解析，等产品线新增后，
        再将原来的删除 */
        IMSA_PdnConnFailErrLogRecord(   enConnType,
                                        IMSA_ERR_LOG_NORM_PDNREJ_CAUSE_IMSA_CONN_RELEASING,
                                        IMSA_ERR_LOG_PDN_CONN_NW_REJ_CAUSE_NONE,
                                        IMSA_ERR_LOG_EMC_REG_FAIL_REASON_SERVICE_STATUS_ERROR);
        #endif
        return ;
    }

    /* 如果在IDLE态，则发起连接建立流程 */

    pstSdfQueryInfo = IMSA_MEM_ALLOC(sizeof(TAF_SDF_PARA_QUERY_INFO_STRU));

    if (VOS_NULL_PTR == pstSdfQueryInfo)
    {
        /*打印不合法信息*/
        IMSA_ERR_LOG("IMSA_CONN_ProcNormalConnSetupReq:ERROR: Mem alloc fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_CONN_ProcConnSetupReq_ENUM, LNAS_NULL_PTR);

        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_OHTERS,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_IMS_DOMAIN_LTE);

        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_PdnRejErrRecord(IMSA_ERR_LOG_PDNREJ_CAUSE_IMSA_OTHERS);
        #endif
        return;
    }

    /* 获取所有NV项中的SDF配置信息 */
    IMSA_CONN_AssignOpid(enConnType, &ucOpid);
    ulRslt = TAF_PS_GetCidSdfParaInfo(PS_PID_IMSA, IMSA_CLIENT_ID, ucOpid, pstSdfQueryInfo);
    if (VOS_OK != ulRslt)
    {
        /*打印不合法信息*/
        IMSA_ERR_LOG("IMSA_CONN_ProcNormalConnSetupReq:TAF_PS_GetCidSdfParaInfo fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_CONN_ProcConnSetupReq_ENUM, IMSA_CALL_API_ERROR);

        IMSA_MEM_FREE(pstSdfQueryInfo);

        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_OHTERS,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_IMS_DOMAIN_LTE);

        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_PdnRejErrRecord(IMSA_ERR_LOG_PDNREJ_CAUSE_IMSA_OTHERS);

        if (IMSA_CONN_TYPE_NORMAL == enConnType)
        {
            IMSA_CONN_PdnConnFailEventReport(IMSA_ERR_LOG_NORM_PDNREJ_CAUSE_IMSA_PARA_ERR,
                                             IMSA_ERR_LOG_PDN_CONN_NW_REJ_CAUSE_BUTT);
        }
        #endif

        return;
    }

    ulRslt = IMSA_CONN_FindSipSignalDialPara(   enConnType,
                                                pstSdfQueryInfo->ulSdfNum,
                                                pstSdfQueryInfo->astSdfPara,
                                                &ulFoundIndex);

    if (IMSA_FAIL == ulRslt)
    {
        /*打印不合法信息*/
        IMSA_ERR_LOG("IMSA_CONN_ProcNormalConnSetupReq:ERROR: Err Para!");
        TLPS_PRINT2LAYER_ERROR(IMSA_CONN_ProcConnSetupReq_ENUM, LNAS_PARAM_INVALID);

        IMSA_MEM_FREE(pstSdfQueryInfo);

        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_PARA_ERR,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_IMS_DOMAIN_LTE);

        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_PdnRejErrRecord(IMSA_ERR_LOG_PDNREJ_CAUSE_IMSA_PARA_ERR);
        /* 暂时先不删除旧的CHR，因为产品线对新增的CHR还未增加解析，等产品线新增后，
        再将原来的删除 */
        IMSA_PdnConnFailErrLogRecord(   enConnType,
                                        IMSA_ERR_LOG_NORM_PDNREJ_CAUSE_NO_IMS_PDP_CONTXT,
                                        IMSA_ERR_LOG_PDN_CONN_NW_REJ_CAUSE_NONE,
                                        IMSA_ERR_LOG_EMC_REG_FAIL_REASON_NO_EMC_PDP_CONTEXT);

        if (IMSA_CONN_TYPE_NORMAL == enConnType)
        {
            IMSA_CONN_PdnConnFailEventReport(IMSA_ERR_LOG_NORM_PDNREJ_CAUSE_NO_IMS_PDP_CONTXT,
                                             IMSA_ERR_LOG_PDN_CONN_NW_REJ_CAUSE_BUTT);
        }
        #endif

        return;
    }

    /* 存储选择的SDF参数 */
    IMSA_CONN_SaveSelectedSdfPara(  enConnType,
                                    &pstSdfQueryInfo->astSdfPara[ulFoundIndex]);

    IMSA_MEM_FREE(pstSdfQueryInfo);

    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        pstSelectedSdfPara = IMSA_CONN_GetNormalConnSelSdfParaAddr();
    }
    else
    {
        pstSelectedSdfPara = IMSA_CONN_GetEmcConnSelSdfParaAddr();
    }

    pstDialParaInfo = IMSA_MEM_ALLOC(sizeof(TAF_PS_DIAL_PARA_STRU));

    if (VOS_NULL_PTR == pstDialParaInfo)
    {
        /*打印不合法信息*/
        IMSA_ERR_LOG("IMSA_CONN_ProcNormalConnSetupReq:TAF_PS_DIAL_PARA_STRU ERROR: Mem alloc fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_CONN_ProcConnSetupReq_ENUM, LNAS_NULL_PTR);

        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_OHTERS,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_IMS_DOMAIN_LTE);

        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_PdnRejErrRecord(IMSA_ERR_LOG_PDNREJ_CAUSE_IMSA_OTHERS);

        if (IMSA_CONN_TYPE_NORMAL == enConnType)
        {
            IMSA_CONN_PdnConnFailEventReport(IMSA_ERR_LOG_NORM_PDNREJ_CAUSE_IMSA_OTHERS,
                                             IMSA_ERR_LOG_PDN_CONN_NW_REJ_CAUSE_BUTT);
        }
        #endif

        return;
    }

    /* 将选择的SDF参数转化成拨号参数格式 */
    IMSA_CONN_GetImsDailParaFromSelSdfPara( pstSelectedSdfPara,
                                            pstDialParaInfo);


    /* 产生OPID并存储 */
    IMSA_CONN_AssignOpid(enConnType, &ucOpid);
    IMSA_CONN_SetOpid(enConnType, ucOpid);

    /* 调用API接口TAF_PS_CallOrig请求APS建立IMS信令承载 */
    if (VOS_OK == TAF_PS_CallOrig(PS_PID_IMSA,IMSA_CLIENT_ID, ucOpid, pstDialParaInfo))
    {
        /* 启动IMS拨号定时器 */
        IMSA_CONN_StartTimer(enConnType, TI_IMSA_SIP_SIGAL_PDP_ORIG);

        /* 连接状态转到CONNING状态 */
        IMSA_CONN_SetConnStatus(enConnType, IMSA_CONN_STATUS_CONNING, IMSA_IMS_DOMAIN_LTE);

        /* 存储最初拨号请求的PDN类型 */
        IMSA_CONN_SaveFirstReqPdnType(enConnType, pstDialParaInfo->enPdpType);

        /* 存储当前拨号请求的PDN类型 */
        IMSA_CONN_SaveCurReqPdnType(enConnType, pstDialParaInfo->enPdpType);

        IMSA_INFO_LOG1("global enFirstReqPdnType:", IMSA_CONN_GetNormalConnFirstReqPdnType());
        TLPS_PRINT2LAYER_INFO1(IMSA_CONN_ProcConnSetupReq_ENUM, 1, IMSA_CONN_GetNormalConnFirstReqPdnType());
    }
    else
    {
        /*打印不合法信息*/
        IMSA_ERR_LOG("IMSA_CONN_ProcConnSetupReq:TAF_PS_CallOrig fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_CONN_ProcConnSetupReq_ENUM, IMSA_CALL_API_ERROR);

        IMSA_CONN_SndConnSetupInd(  IMSA_CONN_RESULT_FAIL_OHTERS,
                                    enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_IMS_DOMAIN_LTE);

        #if (FEATURE_ON == FEATURE_PTM)
        IMSA_PdnRejErrRecord(IMSA_ERR_LOG_PDNREJ_CAUSE_IMSA_OTHERS);

        if (IMSA_CONN_TYPE_NORMAL == enConnType)
        {
            IMSA_CONN_PdnConnFailEventReport(IMSA_ERR_LOG_NORM_PDNREJ_CAUSE_IMSA_OTHERS,
                                             IMSA_ERR_LOG_PDN_CONN_NW_REJ_CAUSE_BUTT);
        }
        #endif

    }

    IMSA_MEM_FREE(pstDialParaInfo);
}




VOS_UINT8 IMSA_CONN_IsCurHoParaSame2Prev
(
   const IMSA_CONN_HO_EVENT_STRU *pstConnHoEvent,
   const IMSA_CONN_HO_REQ_STRU  *pstConnHoReqMsg
)
{
   /*当前为正在切换状态，比较切换的参数，如果一致，丢弃当前切换命令，返回true */
   if(IMSA_CONN_PDN_HO_STATE_HOING == pstConnHoEvent->enPdnHoState )
   {

       if((pstConnHoEvent->enPdnHoType == pstConnHoReqMsg->enConnHoType)
           &&( pstConnHoEvent->ucOriginalDomainPdnRlsFlag == pstConnHoReqMsg->enOrgPdnRlsFlag))
       {
          return VOS_TRUE;
       }


   }
   return VOS_FALSE;

}


VOS_VOID IMSA_CONN_HO_ProcLteConnRel
(
    const IMSA_CONN_HO_REQ_STRU     *pstConnHoReqMsg
)
{
    IMSA_CONN_STATUS_ENUM_UINT8  enConnStatus   = IMSA_CONN_STATUS_BUTT;
    IMSA_CONN_TYPE_ENUM_UINT32   enConnType     = pstConnHoReqMsg->enConnType;

    IMSA_INFO_LOG("IMSA_CONN_HO_ProcLteConnRelReq is entered!");

    if(IMSA_CONN_TYPE_NORMAL == enConnType)
    {
       enConnStatus = IMSA_CONN_GetNormalConnStatus();
    }
    else
    {
        enConnStatus = IMSA_CONN_GetEmcConnStatus();
    }

    /* 如果在IDLE态，则直接返回连接释放成功 */
    if (IMSA_CONN_STATUS_IDLE == enConnStatus)
    {
        IMSA_INFO_LOG("IMSA_CONN_ProcConnRelReq: idle!");
        TLPS_PRINT2LAYER_INFO(IMSA_CONN_ProcConnRelReq_ENUM, IMSA_STATUS_IDLE);

        /* 新增入参 */
        IMSA_CONN_SndConnRelInd(  enConnType,
                                  IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                  IMSA_CONN_MEDIA_PDP_TYPE_BUTT,
                                  IMSA_IMS_DOMAIN_LTE);
        return ;
    }

        /* 如果在RELEASING态，则丢弃 */
    if (IMSA_CONN_STATUS_RELEASING == enConnStatus)
    {
        IMSA_INFO_LOG("IMSA_CONN_ProcConnRelReq: releasing!");
        TLPS_PRINT2LAYER_INFO(IMSA_CONN_ProcConnRelReq_ENUM, IMSA_STATUS_RELEASING);
        return ;
    }

    /* 停止IMS拨号定时器 */
    IMSA_CONN_StopTimer(enConnType, TI_IMSA_SIP_SIGAL_PDP_ORIG);

    /* 停止等待IPV6参数定时器 */
    IMSA_CONN_StopTimer(enConnType, TI_IMSA_WAIT_IPV6_INFO);

    /* 启动IMS去拨号定时器 */
    IMSA_CONN_StartTimer(enConnType, TI_IMSA_SIP_SIGAL_PDP_END);

    /* 请求APS释放连接 */
    IMSA_CONN_RequestApsRelConn(enConnType);
}


VOS_VOID IMSA_CONN_HO_ProcWifiConnRel
(
    IMSA_CONN_TYPE_ENUM_UINT32  enConnType
)

{
    IMSA_WIFI_NORMAL_CONN_STRU         *pstWifiNormalConn       = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_HO_ProcWifiConnRelReq is entered!");

    pstWifiNormalConn                   = IMSA_CONN_GetWifiNormalConnAddr();

    if (IMSA_IMS_DOMAIN_MODE_NULL == IMSA_GetDomainMode())
    {
        /* 关机过程中，不需要给WIFI发送去激活消息，直接本地去激活*/
        /* 停止IMS拨号定时器 */
        IMSA_CONN_StopTimer(enConnType, TI_IMSA_WAIT_WIFI_PDP_ACTIVE_CNF);

        IMSA_CONN_StopTimer(enConnType, TI_IMSA_WAIT_WIFI_PDP_READY_IND);

#if 0
        /* 通知A核删除虚拟网卡 */
        /* 由于现在wifi下承载没有承载ID，所以和范晶约定，填为0 */
        IMSA_SndMsgRnicPdnDeactInd((VOS_UINT8)0, IMSA_RNIC_IMS_RAT_TYPE_WIFI);
        IMSA_SndMsgAtPdpDeactInd((TAF_PDP_TYPE_ENUM_UINT8)pstWifiNormalConn->stSipSignalPdp.enPdnType, IMSA_AT_IMS_RAT_TYPE_WIFI);
#endif
        /* 删除信令承载信息 */
        IMSA_CONN_DeleteWifiPdpInfo2Reg(IMSA_CONN_TYPE_NORMAL, &pstWifiNormalConn->stSipSignalPdp);

        IMSA_CONN_ClearWifiConnResource(IMSA_CONN_TYPE_NORMAL);

        /* 删除CDS的承载 */
        IMSA_CONN_SndCdsSetImsBearerReq(IMSA_CDS_IMS_BEARER_RAT_TYPE_IEEE_802_11);

        /* 连接模块状态迁至IDLE */
        IMSA_CONN_SetWifiConnStatus(IMSA_CONN_TYPE_NORMAL, IMSA_CONN_STATUS_IDLE);

        /* 通知SERVICE模块连接释放 */
        /* 新增入参 */
        IMSA_CONN_SndConnRelInd(IMSA_CONN_TYPE_NORMAL, IMSA_CONN_SIP_PDP_TYPE_SIGNAL, IMSA_CONN_MEDIA_PDP_TYPE_BUTT, IMSA_IMS_DOMAIN_WIFI);
    }
    else
    {
        /* 停止IMS拨号定时器 */
        IMSA_CONN_StopTimer(enConnType, TI_IMSA_WAIT_WIFI_PDP_ACTIVE_CNF);

        IMSA_CONN_StopTimer(enConnType, TI_IMSA_WAIT_WIFI_PDP_READY_IND);

        /* 启动IMS去拨号定时器 */
        IMSA_CONN_StartTimer(enConnType, TI_IMSA_WAIT_WIFI_PDP_DEACTIVE_CNF);

        /* 请求WIFI释放连接 */
        IMSA_CONN_SndWifiMsgPdnDeactiveReq(IMSA_WIFI_PDN_RELEASE_TYPE_LOCAL);

        IMSA_CONN_SetWifiConnStatus(IMSA_CONN_TYPE_NORMAL, IMSA_CONN_STATUS_RELEASING);
    }

}



VOS_VOID IMSA_CONN_SndConnHoCnf
(
    IMSA_CONN_RESULT_ENUM_UINT32            enResult,
    IMSA_CONN_TYPE_ENUM_UINT32              enConnType,
    IMSA_DOMAIN_SWITCH_TYPE_ENUM_UINT8      enConnHoType,
    VOS_UINT8                               ucOrgPdnRlsFlag
)
{
    IMSA_CONN_HO_CNF_STRU  *pstConnHoCnf = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_SndConnHoCnf:entered");

    /*分配空间并检验分配是否成功 */
    pstConnHoCnf = (VOS_VOID *)(IMSA_GetIntraMsgBuffAddr(sizeof(IMSA_CONN_HO_CNF_STRU)));

    /* 检验是否分配成功 */
    if(VOS_NULL_PTR == pstConnHoCnf)
    {
      /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_CONN_SndConnHoCnf:ERROR:Alloc Msg fail!");
        return ;
    }

        /*清空*/
    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstConnHoCnf), IMSA_GET_MSG_LENGTH(pstConnHoCnf) ,0, IMSA_GET_MSG_LENGTH(pstConnHoCnf));

        /*填写消息头*/
    IMSA_WRITE_INTRA_MSG_HEAD(  pstConnHoCnf,
                                    ID_IMSA_CONN_HO_CNF,
                                    sizeof(IMSA_CONN_HO_CNF_STRU));

    /*填写响应结果*/
    pstConnHoCnf->enHoResult        = enResult;
    pstConnHoCnf->enConnType        = enConnType;
    pstConnHoCnf->enConnHoType      = enConnHoType;
    pstConnHoCnf->enSipPdpType      = IMSA_CONN_SIP_PDP_TYPE_SIGNAL;
    pstConnHoCnf->ucOrgPdnRlsFlag   = ucOrgPdnRlsFlag;

    /*调用消息发送函数 */
    IMSA_SEND_INTRA_MSG(pstConnHoCnf);

}


VOS_VOID IMSA_CONN_HO_ProcLteConnSetUpReq
(
    const IMSA_CONN_HO_REQ_STRU     *pstConnHoReqMsg
)
{

    IMSA_CONN_STATUS_ENUM_UINT8         enConnStatus          = IMSA_CONN_STATUS_BUTT;
    VOS_UINT32                          ulRslt                = IMSA_FAIL;
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType            = pstConnHoReqMsg->enConnType;
    VOS_UINT8                           ucCid                 = IMSA_FAIL;
    IMSA_IP_TYPE_ENUM_UINT8             enIpType              = IMSA_FAIL;

    IMSA_INFO_LOG("IMSA_CONN_HO_ProcLteConnSetUpReq is entered!");


    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        enConnStatus = IMSA_CONN_GetNormalConnStatus();

        ucCid= IMSA_CONN_GetWifiNormalConnSelSdfParaAddr()->ucCid;

        enIpType = IMSA_CONN_GetWifiNormalConnSelSdfParaAddr()->enPdnType;
    }
    else
    {
        /* 目前不支持切换的L为紧急连接的情形 */
        IMSA_WARN_LOG("IMSA_CONN_HO_ProcLteConnSetUpReq:enConnType is invalid!");
        /*just add the Fourth parameter*/
        IMSA_CONN_SndConnHoCnf(IMSA_CONN_RESULT_FAIL_OHTERS,
                               enConnType,
                               IMSA_DOMAIN_SWITCH_TYPE_W2L_SWITCH,
                               VOS_FALSE
                               );
        /* 清除切换事件结构信息 */
        IMSA_CONN_ClearHoEvent();

        return;
    }

    /* 如果在CONNING态，则丢弃 */
    if (IMSA_CONN_STATUS_CONNING == enConnStatus)
    {
        return ;
    }

    /* 如果在RELEASING态，则直接回复切换失败*/
    if (IMSA_CONN_STATUS_RELEASING == enConnStatus)
    {
        /*just add the Fourth parameter*/
        IMSA_CONN_SndConnHoCnf(IMSA_CONN_RESULT_FAIL_CONN_RELEASING,
                               enConnType,
                               IMSA_DOMAIN_SWITCH_TYPE_W2L_SWITCH,
                               VOS_FALSE);

        /* 清除切换事件结构信息 */
        IMSA_CONN_ClearHoEvent();

        return;
    }

   /* 如果在CONN，则直接调用HOEventInActCnf */
    if (IMSA_CONN_STATUS_CONN == enConnStatus)
    {
        /*调用切换事件信息处理*/
        IMSA_CONN_ProcHoEventInActCnf();
        return;
    }

    /* 存储选择的SDF参数 */
    IMSA_MEM_CPY_S(   IMSA_CONN_GetNormalConnSelSdfParaAddr(),
                    sizeof(IMSA_SEL_SDF_PARA_STRU),
                    IMSA_CONN_GetWifiNormalConnSelSdfParaAddr(),
                    sizeof(IMSA_SEL_SDF_PARA_STRU));

    ulRslt = IMSA_CONN_SipSignalPdpActOrig(enConnType,
                                           ucCid,
                                           enIpType);

    if (IMSA_FAIL == ulRslt)
    {
        /* 如果拨号失败，且存在其他PDN的情况下，直接返回成功，SERVICE模块可以判断
                  出IP地址发生变化，会本地去注册，然后再重新发起注册 */
        if (IMSA_TRUE == IMSA_CONN_HasActiveSipSignalPdp(enConnType))
        {
            /* 请求APS释放连接 */
            IMSA_CONN_RequestApsRelConn(enConnType);
            IMSA_CONN_GetHoEventAddr()->enPdnHoState = IMSA_CONN_PDN_HO_STATE_AFTER_HOFAIL;
        }
        else
        {

            /*just add the Fourth parameter*/
            IMSA_CONN_SndConnHoCnf(IMSA_CONN_RESULT_FAIL_OHTERS,
                                   enConnType,
                                   IMSA_DOMAIN_SWITCH_TYPE_W2L_SWITCH,
                                   VOS_FALSE);

            /* 清除切换事件结构信息 */
            IMSA_CONN_ClearHoEvent();
        }

        return;
    }

    /* 连接状态转到CONNING状态 */
    IMSA_CONN_SetConnStatus(enConnType, IMSA_CONN_STATUS_CONNING, IMSA_IMS_DOMAIN_LTE);

    /* 存储最初请求的PDN类型为备份PDN类型 */
    IMSA_CONN_SaveFirstReqPdnType(enConnType, enIpType);

}


VOS_VOID IMSA_CONN_HO_ProcWifiConnSetUpReq
(
    const IMSA_CONN_HO_REQ_STRU     *pstConnHoReqMsg
)
{
    IMSA_CONN_STATUS_ENUM_UINT8         enConnStatus        = IMSA_CONN_STATUS_BUTT;
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType          = pstConnHoReqMsg->enConnType;
    IMSA_SEL_SDF_PARA_STRU             *pstSelectedSdfPara  = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_HO_ProcWifiConnSetUpReq entered");

    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        enConnStatus = IMSA_CONN_GetWifiNormalConnStatus();
        IMSA_INFO_LOG1("IMSA_CONN_ProcWifiConnSetupReq enConnStatus = !",enConnStatus);
    }
    else
    {
        /*just add the Fourth parameter*/
        IMSA_CONN_SndConnHoCnf(IMSA_CONN_RESULT_FAIL_OHTERS,
                               enConnType,
                               IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH,
                               VOS_FALSE);
        /* 清除切换事件结构信息 */
        IMSA_CONN_ClearHoEvent();
        return;
    }

    /* 如果在CONN态，则直接回复service模块切换成功*/
    if (IMSA_CONN_STATUS_CONN == enConnStatus)
    {
        /*just add the Fourth parameter*/
        IMSA_CONN_SndConnHoCnf(IMSA_CONN_RESULT_SUCCESS,
                               enConnType,
                               IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH,
                               VOS_FALSE);

        /* 清除切换事件结构信息 */
        IMSA_CONN_ClearHoEvent();

        return ;
    }

    /* 如果在CONNING态，则丢弃 */
    if (IMSA_CONN_STATUS_CONNING == enConnStatus)
    {
        return ;
    }

    /* 如果在RELEASING态，则直接回复连接建立失败，原因值为IMSA_CONN_RESULT_FAIL_CONN_RELEASING */
    if (IMSA_CONN_STATUS_RELEASING == enConnStatus)
    {
        /*打印不合法信息*/
        IMSA_WARN_LOG("IMSA_CONN_HO_ProcWifiConnSetUpReq:ERROR: conn releasing!");

        /*just add the Fourth parameter*/
        IMSA_CONN_SndConnHoCnf(IMSA_CONN_RESULT_FAIL_CONN_RELEASING,
                               enConnType,
                               IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH,
                               VOS_FALSE);
        /* 清除切换事件结构信息 */
        IMSA_CONN_ClearHoEvent();
        return ;
    }

    /* 存储选择的SDF参数 */
    IMSA_MEM_CPY_S(   IMSA_CONN_GetWifiNormalConnSelSdfParaAddr(),
                      sizeof(IMSA_SEL_SDF_PARA_STRU),
                    IMSA_CONN_GetNormalConnSelSdfParaAddr(),
                    sizeof(IMSA_SEL_SDF_PARA_STRU));


    pstSelectedSdfPara = IMSA_CONN_GetWifiNormalConnSelSdfParaAddr();

    /* 如果在IDLE状态，则通知WIFI，发起建链请求  */
    IMSA_CONN_SndWifiMsgPdnActivateReq( pstSelectedSdfPara->bitOpApn,
                                        (IMSA_WIFI_PDP_APN_STRU *)&(pstSelectedSdfPara->stApnInfo),
                                        (IMSA_WIFI_IP_TYPE_ENUM_INT8)(pstSelectedSdfPara->enPdnType),
                                         VOS_TRUE);
    /* 再次发送一次Audio Info,同步端口信息 */
    if( VOS_TRUE == IMSA_IsImsAudioChannelExist())
    {
        /* 同步Audio列表到MAPCON */
        (VOS_VOID)IMSA_ImsEngineSyncPortInfo();
    }

    /* 启动IMS拨号定时器 */
    IMSA_CONN_StartTimer(enConnType, TI_IMSA_WAIT_WIFI_PDP_ACTIVE_CNF);

    /* 连接状态转到CONNING状态 */
    IMSA_CONN_SetWifiConnStatus(enConnType, IMSA_CONN_STATUS_CONNING);
    return;

}



VOS_VOID IMSA_CONN_HO_ProcConnSetUpReq
(
    const IMSA_CONN_HO_REQ_STRU     *pstConnHoReqMsg
)
{
    IMSA_INFO_LOG("IMSA_CONN_HO_ProcConnSetUpReq is entered!");

    if(IMSA_DOMAIN_SWITCH_TYPE_W2L_SWITCH == pstConnHoReqMsg->enConnHoType)
    {

        IMSA_CONN_HO_ProcLteConnSetUpReq(pstConnHoReqMsg);
    }
    else
    {
        IMSA_CONN_HO_ProcWifiConnSetUpReq(pstConnHoReqMsg);
    }
}



VOS_VOID IMSA_CONN_ProcConnHoReq
(

   const IMSA_CONN_HO_REQ_STRU     *pstConnHoReqMsg
)
{
    IMSA_CONN_HO_EVENT_STRU *pstImsaHoEvent = VOS_NULL;

    IMSA_INFO_LOG("IMSA_CONN_ProcConnHoReq:entered");

    pstImsaHoEvent = IMSA_CONN_GetHoEventAddr();

    /* 验证参数的合法性 */
    if( (IMSA_DOMAIN_SWITCH_TYPE_NO_SWITCH == pstConnHoReqMsg->enConnHoType)
        ||(IMSA_IMS_DOMAIN_NULL == pstConnHoReqMsg->enConnRatType)
        ||(IMSA_IMS_DOMAIN_LTE_WIFI == pstConnHoReqMsg->enConnRatType)
        ||(IMSA_CONN_TYPE_EMC== pstConnHoReqMsg->enConnType)
        ||((VOS_TRUE != pstConnHoReqMsg->enOrgPdnRlsFlag)
           &&(VOS_FALSE != pstConnHoReqMsg->enOrgPdnRlsFlag)))
    {
        /* 返回切换失败 */
        IMSA_INFO_LOG("IMSA_CONN_ProcConnHoReq: 1: switch para error");
        /*just add the Fourth parameter*/
        IMSA_CONN_SndConnHoCnf(IMSA_CONN_RESULT_FAIL_PARA_ERR,pstConnHoReqMsg->enConnType,pstConnHoReqMsg->enConnHoType,VOS_FALSE);
        return;
    }

    /* 验证切换类型 */
    if( ((IMSA_IMS_HO_TYPE_LTE2WIFI == pstConnHoReqMsg->enConnHoType)
          &&(IMSA_IMS_DOMAIN_WIFI != pstConnHoReqMsg->enConnRatType))
        ||((IMSA_IMS_HO_TYPE_WIFI2LTE == pstConnHoReqMsg->enConnHoType)
          &&(IMSA_IMS_DOMAIN_LTE != pstConnHoReqMsg->enConnRatType)))
    {
        /* 返回切换失败 */
        IMSA_INFO_LOG("IMSA_CONN_ProcConnHoReq: 2: switch type error");
        /*just add the Fourth parameter*/
        IMSA_CONN_SndConnHoCnf(IMSA_CONN_RESULT_FAIL_PARA_ERR,pstConnHoReqMsg->enConnType,pstConnHoReqMsg->enConnHoType,VOS_FALSE);
        return;
    }

    /* L2W: 原来LTE PDN不存在,无法启动切换 */
    if( (IMSA_IMS_HO_TYPE_LTE2WIFI == pstImsaHoEvent->enPdnHoType )
        &&(IMSA_CONN_STATUS_IDLE == IMSA_CONN_GetNormalConnStatus())
        &&(IMSA_CONN_PDN_HO_STATE_IDLE == pstImsaHoEvent->enPdnHoState))
    {
        /* 返回切换失败 */
        IMSA_INFO_LOG("IMSA_CONN_ProcConnHoReq: 3: original LTE PDN non-exist");
        /*just add the Fourth parameter*/
        IMSA_CONN_SndConnHoCnf(IMSA_CONN_RESULT_FAIL_PARA_ERR,pstConnHoReqMsg->enConnType,pstConnHoReqMsg->enConnHoType,VOS_FALSE);
        return;
    }

    /* W2L: 原来WIFI PDN不存在,无法启动切换 */
    if((IMSA_IMS_HO_TYPE_WIFI2LTE == pstImsaHoEvent->enPdnHoType )
        &&(IMSA_CONN_STATUS_IDLE == IMSA_CONN_GetWifiNormalConnStatus())
        &&(IMSA_CONN_PDN_HO_STATE_IDLE == pstImsaHoEvent->enPdnHoState))
    {
        /* 返回切换失败 */
        IMSA_INFO_LOG("IMSA_CONN_ProcConnHoReq: 3: original WIFI PDN non-exist");
        /*just add the Fourth parameter*/
        IMSA_CONN_SndConnHoCnf(IMSA_CONN_RESULT_FAIL_PARA_ERR,pstConnHoReqMsg->enConnType,pstConnHoReqMsg->enConnHoType,VOS_FALSE);
        return;
    }


     /* 切换承载建立前，备份释放承载的类型和CID */
    if ( (IMSA_SRV_STATUS_HO_DEREG == IMSA_SRV_GetNormalSrvStatus())
        || (IMSA_SRV_STATUS_HO_REG == IMSA_SRV_GetNormalSrvStatus()))
    {
        if( IMSA_IMS_HO_TYPE_WIFI2LTE == pstImsaHoEvent->enPdnHoType)
        {
            IMSA_RegSaveRegedPara(IMSA_REG_TYPE_NORMAL,
                                  IMSA_CONN_GetWifiNormalConnSelSdfParaAddr()->ucCid,
                                  IMSA_CONN_GetWifiNormalConnSelSdfParaAddr()->enPdnType);
        }
        else
        {
            IMSA_RegSaveRegedPara(IMSA_REG_TYPE_NORMAL,
                                  IMSA_CONN_GetNormalConnSelSdfParaAddr()->ucCid,
                                  IMSA_CONN_GetNormalConnSelSdfParaAddr()->enPdnType);
        }
    }

    /* 如果当前正处于切换当中 */
    if(IMSA_CONN_PDN_HO_STATE_HOING == pstImsaHoEvent->enPdnHoState)
    {
        /* 正在切换过程中，如果请求消息中带的切换参数与上次的切换参数一致，直接丢弃该消息，不进行处理 */
        if(VOS_TRUE == IMSA_CONN_IsCurHoParaSame2Prev(pstImsaHoEvent,pstConnHoReqMsg))
        {

            IMSA_WARN_LOG("Para of the msg is same as the previous");
            return;
        }
        /* 原来支持本地释放,现在要求改为网络交互释放,这是一种异常 */
        if((VOS_FALSE == pstImsaHoEvent->ucOriginalDomainPdnRlsFlag)
            && (VOS_TRUE == pstConnHoReqMsg->enOrgPdnRlsFlag))
        {
            IMSA_WARN_LOG("para:ucOriginalDomainPdnRlsFlag is anbormal");
            return ;
        }

        /* */
        if((VOS_TRUE == pstImsaHoEvent->ucOriginalDomainPdnRlsFlag)
            && (VOS_FALSE == pstConnHoReqMsg->enOrgPdnRlsFlag))
        {
            /* 没有收到切换域信令 PDN建立指示 */
            if( VOS_FALSE == pstImsaHoEvent->ucNewSignalPdnSetupEvent)
            {
               pstImsaHoEvent->ucOriginalDomainPdnRlsFlag = VOS_FALSE;
               return;
            }
            else
            {
               if(IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH == pstConnHoReqMsg->enConnHoType)
               {
                   IMSA_CONN_HoConnSucc(IMSA_CONN_TYPE_NORMAL,IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH);

                   /* 清除连接实体信息 */
                   IMSA_CONN_ClearRatResource(IMSA_CONN_TYPE_NORMAL,IMSA_IMS_DOMAIN_LTE);
               }
               else
               {

                   IMSA_CONN_HoConnSucc(IMSA_CONN_TYPE_NORMAL,IMSA_DOMAIN_SWITCH_TYPE_W2L_SWITCH);

                   /* 清除连接实体信息 */
                   IMSA_CONN_ClearRatResource(IMSA_CONN_TYPE_NORMAL,IMSA_IMS_DOMAIN_WIFI);
               }
            }
        }
    }
    /* 没有在切换中 */
    else if(IMSA_CONN_PDN_HO_STATE_IDLE == pstImsaHoEvent->enPdnHoState)
    {
        /* 更新连接模块切换记录*/
        pstImsaHoEvent->enPdnHoState = IMSA_CONN_PDN_HO_STATE_HOING;
        pstImsaHoEvent->enPdnHoType = pstConnHoReqMsg->enConnHoType;

        if(IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH == pstConnHoReqMsg->enConnHoType)
        {
            pstImsaHoEvent->enNewPdnConnType = IMSA_IMS_DOMAIN_WIFI;

            /* 保存原来PDN context */
            pstImsaHoEvent->ulSipSignalPdpNum = IMSA_CONN_GetNormalConnAddr()->ulSipSignalPdpNum;
            IMSA_MEM_CPY_S(pstImsaHoEvent->u.astSipSignalPdpArray,
                            IMSA_CONN_MAX_NORMAL_SIP_SIGNAL_PDP_NUM*sizeof(IMSA_PDP_CNTXT_INFO_STRU),
                            IMSA_CONN_GetNormalConnAddr()->astSipSignalPdpArray,
                            IMSA_CONN_MAX_NORMAL_SIP_SIGNAL_PDP_NUM*sizeof(IMSA_PDP_CNTXT_INFO_STRU));
        }
        else
        {
            pstImsaHoEvent->enNewPdnConnType = IMSA_IMS_DOMAIN_LTE;

            /* 保存原来PDN的CNXT */
            pstImsaHoEvent->ulSipSignalPdpNum = 1;
            IMSA_MEM_CPY_S(&(pstImsaHoEvent->u.stSipSignalPdp),sizeof(IMSA_WIFI_PDP_CNTXT_INFO_STRU) ,&(IMSA_CONN_GetWifiNormalConnAddr()->stSipSignalPdp), sizeof(IMSA_WIFI_PDP_CNTXT_INFO_STRU));
        }
        pstImsaHoEvent->ucOriginalDomainPdnRlsFlag = pstConnHoReqMsg->enOrgPdnRlsFlag;

        /*设置需要等待的事件的状态: VOS_FALSE:需要等待指示，VOS_TRUE；已经收到指示 */
        pstImsaHoEvent->ucNewSignalPdnSetupEvent = VOS_FALSE;
        pstImsaHoEvent->ucOldPdnRelIndEvent      = VOS_FALSE;
        pstImsaHoEvent->ucNewVideoPdnSetupEvent  = VOS_FALSE;
        pstImsaHoEvent->ucNewVoicePdnSetupEvent  = VOS_FALSE;

        /*发起切换域的PDN连接建立请求 */
        IMSA_CONN_HO_ProcConnSetUpReq(pstConnHoReqMsg);
    }
    /* 处于切换成功后处理 */
    else if( IMSA_CONN_PDN_HO_STATE_AFTER_HOSUCC == pstImsaHoEvent->enPdnHoState)
    {
        /* 如果请求消息中带的切换参数与上次的切换参数一致，直接丢弃该消息，不进行处理 */
        if(VOS_TRUE == IMSA_CONN_IsCurHoParaSame2Prev(pstImsaHoEvent,pstConnHoReqMsg))
        {

            IMSA_WARN_LOG("Para of the msg is same as the previous");
            return;
        }

        /* 原来支持本地释放,现在要求改为网络交互释放,这是一种异常 */
        if((VOS_FALSE == pstImsaHoEvent->ucOriginalDomainPdnRlsFlag)
            && (VOS_TRUE == pstConnHoReqMsg->enOrgPdnRlsFlag))
        {
            IMSA_WARN_LOG("para:ucOriginalDomainPdnRlsFlag is anbormal");
            return ;
        }

        if(IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH == pstConnHoReqMsg->enConnHoType)
        {
            IMSA_CONN_SndConnHoCnf(IMSA_CONN_RESULT_SUCCESS,pstConnHoReqMsg->enConnType,IMSA_DOMAIN_SWITCH_TYPE_L2W_SWITCH,VOS_FALSE);
        }
        else
        {
            IMSA_CONN_SndConnHoCnf(IMSA_CONN_RESULT_SUCCESS,pstConnHoReqMsg->enConnType,IMSA_DOMAIN_SWITCH_TYPE_W2L_SWITCH,VOS_FALSE);
        }
        /* 清除切换资源 */
        IMSA_CONN_ClearHoEvent();
    }
    /* 切换失败后处理 */
    else
    {
        /* 不需要处理 */
    }
}



VOS_VOID IMSA_CONN_ProcConnSetupReq
(
    const IMSA_CONN_SETUP_REQ_STRU     *pstConnSetupReqMsg
)
{
    if (IMSA_IMS_DOMAIN_LTE == pstConnSetupReqMsg->enConnRatType)
    {
        IMSA_CONN_ProcLteConnSetupReq(pstConnSetupReqMsg);
    }
    else
    {
        /* 如果域选的结果为wifi，则调用wifi的处理函数 */
        IMSA_CONN_ProcWifiConnSetupReq(pstConnSetupReqMsg);
    }

    return;
}


VOS_VOID IMSA_CONN_RequestApsRelConn
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn   = VOS_NULL_PTR;
    IMSA_EMC_CONN_STRU                 *pstEmcConn      = VOS_NULL_PTR;
    VOS_UINT8                           ucOpid          = IMSA_NULL;
    IMSA_CONN_HO_EVENT_STRU            *pstImsaHoEvent  = VOS_NULL_PTR;

    pstImsaHoEvent  = IMSA_CONN_GetHoEventAddr();

    if (IMSA_CONN_TYPE_EMC == enConnType)
    {
        pstEmcConn      = IMSA_CONN_GetEmcConnAddr();

        /* 如果在CONNING态，则请求释放正在建立的信令承载和已经激活的信令承载 */
        if (IMSA_CONN_STATUS_CONNING == pstEmcConn->enImsaConnStatus)
        {
            /* 产生OPID并存储 */
            IMSA_CONN_AssignOpid(enConnType, &ucOpid);

            if (VOS_OK != TAF_PS_CallEnd(   PS_PID_IMSA, IMSA_CLIENT_ID, ucOpid,
                                            pstEmcConn->stSelSdfPara.ucCid))
            {
                IMSA_WARN_LOG("IMSA_CONN_RequestApsRelConn:EMC,conninig,CallEnd failed!");
                TLPS_PRINT2LAYER_WARNING(IMSA_CONN_RequestApsRelConn_ENUM, LNAS_FAIL);
            }

            IMSA_CONN_ClearConnResource(IMSA_CONN_TYPE_EMC);

            /* 通知SERVICE模块连接释放 */
            /* 新增入参 */
            IMSA_CONN_SndConnRelInd(    IMSA_CONN_TYPE_EMC,
                                        IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                        IMSA_CONN_MEDIA_PDP_TYPE_BUTT,
                                        IMSA_IMS_DOMAIN_LTE);
            return ;
        }

        /* 产生OPID并存储 */
        IMSA_CONN_AssignOpid(enConnType, &ucOpid);
        /*lint -e774*/
        IMSA_CONN_SetOpid(enConnType, ucOpid);
        /*lint +e774*/



        if (VOS_OK != TAF_PS_CallEnd(   PS_PID_IMSA, IMSA_CLIENT_ID, ucOpid,
                                        (VOS_UINT8)pstEmcConn->stSipSignalPdp.ucCid))
        {
            IMSA_WARN_LOG("IMSA_CONN_RequestApsRelConn:EMC,conn,CallEnd failed!");
            TLPS_PRINT2LAYER_WARNING(IMSA_CONN_RequestApsRelConn_ENUM, LNAS_FAIL);

            IMSA_CONN_ClearConnResource(IMSA_CONN_TYPE_EMC);

            IMSA_CONN_SndCdsSetImsBearerReq(IMSA_CDS_IMS_BEARER_RAT_TYPE_3GPP_EUTRAN);

            /* 通知SERVICE模块连接释放 */
            /* 新增入参 */
            IMSA_CONN_SndConnRelInd(    IMSA_CONN_TYPE_EMC,
                                        IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                        IMSA_CONN_MEDIA_PDP_TYPE_BUTT,
                                        IMSA_IMS_DOMAIN_LTE);
            return ;
        }

        /* 启动IMSA去拨号定时器 */
        IMSA_CONN_StartTimer(IMSA_CONN_TYPE_EMC, TI_IMSA_SIP_SIGAL_PDP_END);

        /* 设置连接状态为去连接中 */
        IMSA_CONN_SetConnStatus(IMSA_CONN_TYPE_EMC, IMSA_CONN_STATUS_RELEASING, IMSA_IMS_DOMAIN_LTE);
        return ;
    }

    pstNormalConn   = IMSA_CONN_GetNormalConnAddr();

    /* 如果在CONNING态，则请求释放正在建立的信令承载和已经激活的信令承载 */
    if (IMSA_CONN_STATUS_CONNING == pstNormalConn->enImsaConnStatus)
    {
        /* 产生OPID并存储 */
        IMSA_CONN_AssignOpid(enConnType, &ucOpid);

        if (VOS_OK != TAF_PS_CallEnd(   PS_PID_IMSA, IMSA_CLIENT_ID, ucOpid,
                                        pstNormalConn->stSelSdfPara.ucCid))
        {
            IMSA_WARN_LOG("IMSA_CONN_RequestApsRelConn:normal,conninig,CallEnd failed!");
            TLPS_PRINT2LAYER_WARNING(IMSA_CONN_RequestApsRelConn_ENUM, LNAS_FAIL);
        }
    }

    if (0 == pstNormalConn->ulSipSignalPdpNum)
    {
        IMSA_WARN_LOG("IMSA_CONN_RequestApsRelConn:normal,conning,no active pdp!");
        TLPS_PRINT2LAYER_WARNING(IMSA_CONN_RequestApsRelConn_ENUM, IMSA_NO_ACTIVE_PDP);

        IMSA_CONN_ClearConnResource(IMSA_CONN_TYPE_NORMAL);
        if(IMSA_CONN_PDN_HO_STATE_IDLE == pstImsaHoEvent->enPdnHoState)
        {
            /* 通知SERVICE模块连接释放 */
            /* 新增入参 */
            IMSA_CONN_SndConnRelInd(enConnType, IMSA_CONN_SIP_PDP_TYPE_SIGNAL, IMSA_CONN_MEDIA_PDP_TYPE_BUTT, IMSA_IMS_DOMAIN_LTE);
        }
        else
        {
            /*lint -e746*/
            IMSA_CONN_ProcHoEventInDeactInd();
            /*lint +e746*/
        }
        return ;
    }

    /* 产生OPID并存储 */
    IMSA_CONN_AssignOpid(enConnType, &ucOpid);
    IMSA_CONN_SetNormalConnOpid(ucOpid);

    if (VOS_OK != TAF_PS_CallEnd(   PS_PID_IMSA, IMSA_CLIENT_ID, ucOpid,
                                    pstNormalConn->astSipSignalPdpArray[0].ucCid))
    {
        IMSA_WARN_LOG("IMSA_CONN_RequestApsRelConn:normal,CallEnd failed!");
        TLPS_PRINT2LAYER_WARNING(IMSA_CONN_RequestApsRelConn_ENUM, IMSA_CALL_API_ERROR);

        IMSA_CONN_ClearConnResource(IMSA_CONN_TYPE_NORMAL);

        IMSA_CONN_SndCdsSetImsBearerReq(IMSA_CDS_IMS_BEARER_RAT_TYPE_3GPP_EUTRAN);

        if(IMSA_CONN_PDN_HO_STATE_IDLE == pstImsaHoEvent->enPdnHoState)
        {
            /* 通知SERVICE模块连接释放 */
            /* 新增入参 */
            IMSA_CONN_SndConnRelInd(enConnType, IMSA_CONN_SIP_PDP_TYPE_SIGNAL, IMSA_CONN_MEDIA_PDP_TYPE_BUTT, IMSA_IMS_DOMAIN_LTE);
        }
        else
        {
            IMSA_CONN_ProcHoEventInDeactInd();
        }

        return ;
    }

    /* 启动IMSA去拨号定时器 */
    IMSA_CONN_StartTimer(IMSA_CONN_TYPE_NORMAL, TI_IMSA_SIP_SIGAL_PDP_END);

    /* 设置连接状态为去连接中 */
    IMSA_CONN_SetConnStatus(IMSA_CONN_TYPE_NORMAL, IMSA_CONN_STATUS_RELEASING, IMSA_IMS_DOMAIN_LTE);

    return ;
}


VOS_VOID IMSA_CONN_ProcLteConnRelReqInHoing
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType
)
{
    IMSA_CONN_HO_EVENT_STRU            *pstImsaHoEvent      = VOS_NULL_PTR;

    pstImsaHoEvent = IMSA_CONN_GetHoEventAddr();

    if( IMSA_IMS_HO_TYPE_WIFI2LTE == pstImsaHoEvent->enPdnHoType)
    {
        /* HO abort operation */

        /* 停止IMS拨号定时器 */
        IMSA_CONN_StopTimer(enConnType, TI_IMSA_SIP_SIGAL_PDP_ORIG);

        /* 停止等待IPV6参数定时器 */
        IMSA_CONN_StopTimer(enConnType, TI_IMSA_WAIT_IPV6_INFO);

        /* 启动IMS去拨号定时器 */
        IMSA_CONN_StartTimer(enConnType, TI_IMSA_SIP_SIGAL_PDP_END);
        pstImsaHoEvent->enPdnHoState = IMSA_CONN_PDN_HO_STATE_AFTER_HOFAIL;

        /* 请求APS释放连接 */
        IMSA_CONN_RequestApsRelConn(enConnType);

    }
    else
    {
        /* If it is HO_LTE2WIFI, if L PDN exist,release */
        if( (IMSA_CONN_STATUS_IDLE == IMSA_CONN_GetNormalConnStatus())
            ||(IMSA_CONN_STATUS_RELEASING == IMSA_CONN_GetNormalConnStatus()))
        {
            return;
        }

        /* 停止IMS拨号定时器 */
        IMSA_CONN_StopTimer(enConnType, TI_IMSA_SIP_SIGAL_PDP_ORIG);

        /* 停止等待IPV6参数定时器 */
        IMSA_CONN_StopTimer(enConnType, TI_IMSA_WAIT_IPV6_INFO);

        /* 启动IMS去拨号定时器 */
        IMSA_CONN_StartTimer(enConnType, TI_IMSA_SIP_SIGAL_PDP_END);

        /* 请求APS释放连接 */
        IMSA_CONN_RequestApsRelConn(enConnType);
    }

}



VOS_VOID IMSA_CONN_ProcLteConnRelReqInAfterHoFail
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType
)
{
    IMSA_CONN_HO_EVENT_STRU            *pstImsaHoEvent      = VOS_NULL_PTR;

    pstImsaHoEvent = IMSA_CONN_GetHoEventAddr();

    if( IMSA_IMS_HO_TYPE_WIFI2LTE == pstImsaHoEvent->enPdnHoType)
    {
        /* HO abort operation: wait for Ho process to release lte procedure */

        /* 停止IMS拨号定时器 */
        IMSA_CONN_StopTimer(enConnType, TI_IMSA_SIP_SIGAL_PDP_ORIG);

        /* 停止等待IPV6参数定时器 */
        IMSA_CONN_StopTimer(enConnType, TI_IMSA_WAIT_IPV6_INFO);

        /* 启动IMS去拨号定时器 */
        IMSA_CONN_StartTimer(enConnType, TI_IMSA_SIP_SIGAL_PDP_END);

        /* 请求APS释放连接 */
        IMSA_CONN_RequestApsRelConn(enConnType);
    }
    else
    {
        /* If it is HO_LTE2WIFI, if L pdn exist,release */
        if( (IMSA_CONN_STATUS_IDLE == IMSA_CONN_GetNormalConnStatus())
            ||(IMSA_CONN_STATUS_RELEASING == IMSA_CONN_GetNormalConnStatus()))
        {
            return;
        }

        /* 停止IMS拨号定时器 */
        IMSA_CONN_StopTimer(enConnType, TI_IMSA_SIP_SIGAL_PDP_ORIG);

        /* 停止等待IPV6参数定时器 */
        IMSA_CONN_StopTimer(enConnType, TI_IMSA_WAIT_IPV6_INFO);

        /* 启动IMS去拨号定时器 */
        IMSA_CONN_StartTimer(enConnType, TI_IMSA_SIP_SIGAL_PDP_END);

        /* 请求APS释放连接 */
        IMSA_CONN_RequestApsRelConn(enConnType);
    }
}


VOS_VOID IMSA_CONN_ProcLteConnRelReqInRelSpeedUp
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType
)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn           = VOS_NULL_PTR;
    VOS_UINT32                          ulRegParaInvalidFlag    = IMSA_NULL;

    pstNormalConn   = IMSA_CONN_GetNormalConnAddr();

    /* 如果存在两个连接，需要删除信令承载信息两次 */
    if( 0 != pstNormalConn->ulSipSignalPdpNum)
    {
        IMSA_CONN_DeletePdpInfo(    enConnType,
                                pstNormalConn->astSipSignalPdpArray[0].ucCid,
                                pstNormalConn->astSipSignalPdpArray[0].ucPdpId,
                                &ulRegParaInvalidFlag);

        IMSA_CONN_RequestApsRelConn(enConnType);
    }

    if( 0 != pstNormalConn->ulSipSignalPdpNum)
    {
        IMSA_CONN_DeletePdpInfo(    enConnType,
                                            pstNormalConn->astSipSignalPdpArray[0].ucCid,
                                            pstNormalConn->astSipSignalPdpArray[0].ucPdpId,
                                            &ulRegParaInvalidFlag);

        /* 通知SERVICE模块连接释放 */
        IMSA_CONN_SndConnRelInd(enConnType, IMSA_CONN_SIP_PDP_TYPE_SIGNAL,IMSA_CONN_MEDIA_PDP_TYPE_BUTT, IMSA_IMS_DOMAIN_LTE);
    }

    /* 切换过程中，加速释放，避免释放流程对于另外一个域的建立流程造成冲击 */
    IMSA_CONN_ClearConnResource(enConnType);

}



VOS_VOID IMSA_CONN_ProcLteConnRelReq
(
    const IMSA_CONN_REL_REQ_STRU       *pstConnRelReqMsg
)
{
    IMSA_CONN_STATUS_ENUM_UINT8         enConnStatus        = IMSA_CONN_STATUS_BUTT;
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType          = pstConnRelReqMsg->enConnType;
    IMSA_CONN_HO_EVENT_STRU            *pstImsaHoEvent      = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_ProcConnRelReq is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_ProcConnRelReq_ENUM, LNAS_ENTRY);

    if (IMSA_CONN_SIP_PDP_TYPE_MEDIA == pstConnRelReqMsg->enSipPdpType)
    {
        IMSA_ERR_LOG("IMSA_CONN_ProcConnRelReq:Not support release media type pdp!");
        TLPS_PRINT2LAYER_ERROR(IMSA_CONN_ProcConnRelReq_ENUM, IMSA_NOT_SUPPORT);
        return ;
    }

    pstImsaHoEvent = IMSA_CONN_GetHoEventAddr();

    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        enConnStatus = IMSA_CONN_GetNormalConnStatus();
    }
    else
    {
        enConnStatus = IMSA_CONN_GetEmcConnStatus();
    }

    if( (IMSA_CONN_TYPE_NORMAL == enConnType)
        &&((IMSA_CONN_PDN_HO_STATE_HOING == pstImsaHoEvent->enPdnHoState )
        ||( IMSA_CONN_PDN_HO_STATE_AFTER_HOSUCC == pstImsaHoEvent->enPdnHoState )))
    {
        IMSA_CONN_ProcLteConnRelReqInHoing(enConnType);

        return;
    }
    if((IMSA_CONN_TYPE_NORMAL == enConnType)
            &&( IMSA_CONN_PDN_HO_STATE_AFTER_HOFAIL == pstImsaHoEvent->enPdnHoState ))
    {
        IMSA_CONN_ProcLteConnRelReqInAfterHoFail(enConnType);
        return;
    }

    /* 如果在IDLE态，则直接返回连接释放成功 */
    if (IMSA_CONN_STATUS_IDLE == enConnStatus)
    {
        IMSA_INFO_LOG("IMSA_CONN_ProcConnRelReq: idle!");
        TLPS_PRINT2LAYER_INFO(IMSA_CONN_ProcConnRelReq_ENUM, IMSA_STATUS_IDLE);

        /* 新增入参 */
        IMSA_CONN_SndConnRelInd(  enConnType,
                                  IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                  IMSA_CONN_MEDIA_PDP_TYPE_BUTT,
                                  IMSA_IMS_DOMAIN_LTE);
        return ;
    }

    /* 如果在RELEASING态，则丢弃 */
    if (IMSA_CONN_STATUS_RELEASING == enConnStatus)
    {
        IMSA_INFO_LOG("IMSA_CONN_ProcConnRelReq: releasing!");
        TLPS_PRINT2LAYER_INFO(IMSA_CONN_ProcConnRelReq_ENUM, IMSA_STATUS_RELEASING);

        /* 加速释放 ,只在涉及VoWiFi/VoLTE切换过程中才有这种加速释放处理*/
        if(IMSA_CONN_RELEASE_TYPE_ENUM_SPEEDUP == pstConnRelReqMsg->enConnRlsType)
        {
            IMSA_CONN_ProcLteConnRelReqInRelSpeedUp(enConnType);
        }
        return ;
    }

    /* 停止IMS拨号定时器 */
    IMSA_CONN_StopTimer(enConnType, TI_IMSA_SIP_SIGAL_PDP_ORIG);

    /* 停止等待IPV6参数定时器 */
    IMSA_CONN_StopTimer(enConnType, TI_IMSA_WAIT_IPV6_INFO);

    /* 启动IMS去拨号定时器 */
    IMSA_CONN_StartTimer(enConnType, TI_IMSA_SIP_SIGAL_PDP_END);

    /* 请求APS释放连接 */
    IMSA_CONN_RequestApsRelConn(enConnType);

    /* 加速释放 ,只在涉及VoWiFi/VoLTE切换过程中才有这种加速释放处理*/
    if(IMSA_CONN_RELEASE_TYPE_ENUM_SPEEDUP == pstConnRelReqMsg->enConnRlsType)
    {
        IMSA_CONN_ProcLteConnRelReqInRelSpeedUp(enConnType);
    }
    return ;
}


VOS_VOID IMSA_CONN_ProcWifiConnRelReqInHoing(VOS_VOID)
{
    IMSA_CONN_HO_EVENT_STRU            *pstImsaHoEvent      = VOS_NULL_PTR;

    pstImsaHoEvent = IMSA_CONN_GetHoEventAddr();

    if( IMSA_IMS_HO_TYPE_LTE2WIFI == pstImsaHoEvent->enPdnHoType)
    {
        /* HO abort operation */
        /* CONN||CONNING, 启动释放过程 */
        IMSA_CONN_StartTimer(IMSA_CONN_TYPE_NORMAL, TI_IMSA_WAIT_WIFI_PDP_DEACTIVE_CNF);

        /* 请求WIFI释放连接 */
        IMSA_CONN_SndWifiMsgPdnDeactiveReq(IMSA_WIFI_PDN_RELEASE_TYPE_NW_CMD);

        IMSA_CONN_SetWifiConnStatus(IMSA_CONN_TYPE_NORMAL, IMSA_CONN_STATUS_RELEASING);

        pstImsaHoEvent->enPdnHoState = IMSA_CONN_PDN_HO_STATE_AFTER_HOFAIL;
    }
    else
    {
        /* If it is HO_WIFI2LTE, stay in here and wait for ho operation to release wifi PDN */
        if( (IMSA_CONN_STATUS_IDLE == IMSA_CONN_GetWifiNormalConnStatus())
            ||(IMSA_CONN_STATUS_RELEASING == IMSA_CONN_GetWifiNormalConnStatus()))
        {
            return;
        }
        /* CONN||CONNING, 启动释放过程 */
        IMSA_CONN_StartTimer(IMSA_CONN_TYPE_NORMAL, TI_IMSA_WAIT_WIFI_PDP_DEACTIVE_CNF);

        /* 请求WIFI释放连接 */
        IMSA_CONN_SndWifiMsgPdnDeactiveReq(IMSA_WIFI_PDN_RELEASE_TYPE_NW_CMD);

        IMSA_CONN_SetWifiConnStatus(IMSA_CONN_TYPE_NORMAL, IMSA_CONN_STATUS_RELEASING);
    }

}


VOS_VOID IMSA_CONN_ProcWifiConnRelReqInAfterHoFail(VOS_VOID)
{
    IMSA_CONN_HO_EVENT_STRU            *pstImsaHoEvent      = VOS_NULL_PTR;

    pstImsaHoEvent = IMSA_CONN_GetHoEventAddr();

    if( IMSA_IMS_HO_TYPE_LTE2WIFI == pstImsaHoEvent->enPdnHoType)
    {
        /* HO abort operation: wait for Ho process to release wifi procedure */
    }
    else
    {
        /* If it is HO_WIFI2LTE, stay in here and no operation */
        if( (IMSA_CONN_STATUS_IDLE == IMSA_CONN_GetWifiNormalConnStatus())
            ||(IMSA_CONN_STATUS_RELEASING == IMSA_CONN_GetWifiNormalConnStatus()))
        {
            return;
        }
        /* CONN||CONNING, 启动释放过程 */
        IMSA_CONN_StartTimer(IMSA_CONN_TYPE_NORMAL, TI_IMSA_WAIT_WIFI_PDP_DEACTIVE_CNF);

        /* 请求WIFI释放连接 */
        IMSA_CONN_SndWifiMsgPdnDeactiveReq(IMSA_WIFI_PDN_RELEASE_TYPE_NW_CMD);

        IMSA_CONN_SetWifiConnStatus(IMSA_CONN_TYPE_NORMAL, IMSA_CONN_STATUS_RELEASING);
    }
}




VOS_VOID IMSA_CONN_ProcWifiConnRelReq
(
    const IMSA_CONN_REL_REQ_STRU       *pstConnRelReqMsg
)
{
    IMSA_CONN_STATUS_ENUM_UINT8         enConnStatus        = IMSA_CONN_STATUS_BUTT;
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType          = pstConnRelReqMsg->enConnType;
    IMSA_WIFI_NORMAL_CONN_STRU         *pstNormalConn       = VOS_NULL_PTR;
    IMSA_CONN_HO_EVENT_STRU            *pstImsaHoEvent      = VOS_NULL_PTR;


    IMSA_INFO_LOG("IMSA_CONN_ProcConnRelReq is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_ProcConnRelReq_ENUM, LNAS_ENTRY);

    pstImsaHoEvent = IMSA_CONN_GetHoEventAddr();

    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        enConnStatus = IMSA_CONN_GetWifiNormalConnStatus();
    }
    else
    {
        /* todo */
        return;
    }

    pstNormalConn                   = IMSA_CONN_GetWifiNormalConnAddr();

    if( (IMSA_CONN_PDN_HO_STATE_HOING == pstImsaHoEvent->enPdnHoState )
        ||( IMSA_CONN_PDN_HO_STATE_AFTER_HOSUCC == pstImsaHoEvent->enPdnHoState ))
    {
        IMSA_CONN_ProcWifiConnRelReqInHoing();
        return;
    }
    if( IMSA_CONN_PDN_HO_STATE_AFTER_HOFAIL == pstImsaHoEvent->enPdnHoState )
    {
        IMSA_CONN_ProcWifiConnRelReqInAfterHoFail();
        return;
    }


    /* 如果在IDLE态，则直接返回连接释放成功 */
    if (IMSA_CONN_STATUS_IDLE == enConnStatus)
    {
        IMSA_INFO_LOG("IMSA_CONN_ProcConnRelReq: idle!");
        TLPS_PRINT2LAYER_INFO(IMSA_CONN_ProcConnRelReq_ENUM, IMSA_STATUS_IDLE);

        /* 新增入参 */
        IMSA_CONN_SndConnRelInd(  enConnType,
                                  IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                  IMSA_CONN_MEDIA_PDP_TYPE_BUTT,
                                  IMSA_IMS_DOMAIN_WIFI);
        return ;
    }

    /* 如果在RELEASING态，则丢弃 */
    if (IMSA_CONN_STATUS_RELEASING == enConnStatus)
    {
        IMSA_INFO_LOG("IMSA_CONN_ProcConnRelReq: releasing!");
        TLPS_PRINT2LAYER_INFO(IMSA_CONN_ProcConnRelReq_ENUM, IMSA_STATUS_RELEASING);
        /* 加速释放 */
        if(IMSA_CONN_RELEASE_TYPE_ENUM_SPEEDUP == pstConnRelReqMsg->enConnRlsType)
        {
            /* 删除信令承载信息 */
            IMSA_CONN_DeleteWifiPdpInfo2Reg(IMSA_CONN_TYPE_NORMAL, &pstNormalConn->stSipSignalPdp);

            IMSA_CONN_ClearWifiConnResource(IMSA_CONN_TYPE_NORMAL);

            /* 删除CDS的承载 */
            IMSA_CONN_SndCdsSetImsBearerReq(IMSA_CDS_IMS_BEARER_RAT_TYPE_IEEE_802_11);

            /* 连接模块状态迁至IDLE */
            IMSA_CONN_SetWifiConnStatus(IMSA_CONN_TYPE_NORMAL, IMSA_CONN_STATUS_IDLE);

            /* 通知SERVICE模块连接释放 */
            IMSA_CONN_SndConnRelInd(IMSA_CONN_TYPE_NORMAL, IMSA_CONN_SIP_PDP_TYPE_SIGNAL, IMSA_CONN_MEDIA_PDP_TYPE_BUTT,IMSA_IMS_DOMAIN_WIFI);

        }
        return ;
    }

    if (IMSA_CONN_RELEASE_TYPE_ENUM_LOCAL == pstConnRelReqMsg->enConnRlsType)
    {
        /* 关机过程中，不需要给WIFI发送去激活消息，直接本地去激活*/
        /* 停止IMS拨号定时器 */
        IMSA_CONN_StopTimer(enConnType, TI_IMSA_WAIT_WIFI_PDP_ACTIVE_CNF);

        IMSA_CONN_StopTimer(enConnType, TI_IMSA_WAIT_WIFI_PDP_READY_IND);

        /* 删除信令承载信息 */
        IMSA_CONN_DeleteWifiPdpInfo2Reg(IMSA_CONN_TYPE_NORMAL, &pstNormalConn->stSipSignalPdp);

        IMSA_CONN_ClearWifiConnResource(IMSA_CONN_TYPE_NORMAL);

        /* 删除CDS的承载 */
        IMSA_CONN_SndCdsSetImsBearerReq(IMSA_CDS_IMS_BEARER_RAT_TYPE_IEEE_802_11);

        /* 连接模块状态迁至IDLE */
        IMSA_CONN_SetWifiConnStatus(IMSA_CONN_TYPE_NORMAL, IMSA_CONN_STATUS_IDLE);

        /* 通知SERVICE模块连接释放 */
        /* 新增入参 */
        IMSA_CONN_SndConnRelInd(IMSA_CONN_TYPE_NORMAL, IMSA_CONN_SIP_PDP_TYPE_SIGNAL, IMSA_CONN_MEDIA_PDP_TYPE_BUTT, IMSA_IMS_DOMAIN_WIFI);
    }
    else
    {
        /* 停止IMS拨号定时器 */
        IMSA_CONN_StopTimer(enConnType, TI_IMSA_WAIT_WIFI_PDP_ACTIVE_CNF);

        IMSA_CONN_StopTimer(enConnType, TI_IMSA_WAIT_WIFI_PDP_READY_IND);

        /* 启动IMS去拨号定时器 */
        IMSA_CONN_StartTimer(enConnType, TI_IMSA_WAIT_WIFI_PDP_DEACTIVE_CNF);

        /* 请求WIFI释放连接 */
        IMSA_CONN_SndWifiMsgPdnDeactiveReq(IMSA_WIFI_PDN_RELEASE_TYPE_NW_CMD);

        IMSA_CONN_SetWifiConnStatus(IMSA_CONN_TYPE_NORMAL, IMSA_CONN_STATUS_RELEASING);
        if(IMSA_CONN_RELEASE_TYPE_ENUM_SPEEDUP == pstConnRelReqMsg->enConnRlsType)
        {
            /* 删除信令承载信息 */
            IMSA_CONN_DeleteWifiPdpInfo2Reg(IMSA_CONN_TYPE_NORMAL, &pstNormalConn->stSipSignalPdp);

            IMSA_CONN_ClearWifiConnResource(IMSA_CONN_TYPE_NORMAL);

            /* 删除CDS的承载 */
            IMSA_CONN_SndCdsSetImsBearerReq(IMSA_CDS_IMS_BEARER_RAT_TYPE_IEEE_802_11);

            /* 连接模块状态迁至IDLE */
            IMSA_CONN_SetWifiConnStatus(IMSA_CONN_TYPE_NORMAL, IMSA_CONN_STATUS_IDLE);

            /* 通知SERVICE模块连接释放 */
            IMSA_CONN_SndConnRelInd(IMSA_CONN_TYPE_NORMAL, IMSA_CONN_SIP_PDP_TYPE_SIGNAL,IMSA_CONN_MEDIA_PDP_TYPE_BUTT, IMSA_IMS_DOMAIN_WIFI);

        }
    }
}


VOS_VOID IMSA_CONN_ProcConnRelReq
(
    const IMSA_CONN_REL_REQ_STRU       *pstConnRelReqMsg
)
{
    IMSA_CONN_HO_EVENT_STRU *pstImsaHoEvent = VOS_NULL;
    IMSA_INFO_LOG("IMSA_CONN_ProcConnRelReq:entered");
    pstImsaHoEvent = IMSA_CONN_GetHoEventAddr();

    if (IMSA_IMS_DOMAIN_LTE == pstConnRelReqMsg->enConnRatType)
    {
        IMSA_CONN_ProcLteConnRelReq(pstConnRelReqMsg);
    }
    else if(IMSA_IMS_DOMAIN_WIFI == pstConnRelReqMsg->enConnRatType)
    {
        /* 如果域选的结果为wifi，则调用wifi的处理函数 */
        IMSA_CONN_ProcWifiConnRelReq(pstConnRelReqMsg);
    }
    /*lint -e539*/
    else if(IMSA_IMS_DOMAIN_LTE_WIFI == pstConnRelReqMsg->enConnRatType)
    {
        /* 根据CONN状态确定释放过程 */
        if ((IMSA_CONN_TYPE_NORMAL == pstConnRelReqMsg->enConnType)
            &&(IMSA_CONN_STATUS_IDLE != IMSA_CONN_GetNormalConnStatus()))
        {
            IMSA_CONN_ProcLteConnRelReq(pstConnRelReqMsg);
            return;
        }
        if((IMSA_CONN_TYPE_NORMAL == pstConnRelReqMsg->enConnType)
            &&(IMSA_CONN_STATUS_IDLE != IMSA_CONN_GetWifiNormalConnStatus()))
        {
            IMSA_CONN_ProcWifiConnRelReq(pstConnRelReqMsg);
            return;
        }
        if( (IMSA_CONN_STATUS_IDLE == IMSA_CONN_GetNormalConnStatus())
            &&(IMSA_CONN_STATUS_IDLE == IMSA_CONN_GetWifiNormalConnStatus()))
        {
            /* 新增入参 */
            IMSA_CONN_SndConnRelInd(  pstConnRelReqMsg->enConnType,
                                      IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                      IMSA_CONN_MEDIA_PDP_TYPE_BUTT,
                                      pstConnRelReqMsg->enConnRatType);
        }
        if(IMSA_CONN_PDN_HO_STATE_IDLE != pstImsaHoEvent->enPdnHoState)
        {
            IMSA_CONN_ClearHoEvent();
        }
    }
    else
    {
        /* 新增入参 */
        IMSA_CONN_SndConnRelInd(    pstConnRelReqMsg->enConnType,
                                    IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                    IMSA_CONN_MEDIA_PDP_TYPE_BUTT,
                                    pstConnRelReqMsg->enConnRatType);
    }
    /*lint +e539*/

    return ;
}


VOS_UINT32 IMSA_CONN_IsEmcPdpDefined( VOS_VOID )
{
    VOS_UINT32                          ulRslt              = IMSA_FAIL;
    TAF_SDF_PARA_QUERY_INFO_STRU       *pstSdfQueryInfo     = VOS_NULL_PTR;
    VOS_UINT8                           ucOpid              = IMSA_NULL;
    VOS_UINT32                          ulFoundIndex        = IMSA_NULL;

    pstSdfQueryInfo = IMSA_MEM_ALLOC(sizeof(TAF_SDF_PARA_QUERY_INFO_STRU));

    if (VOS_NULL_PTR == pstSdfQueryInfo)
    {
        /*打印不合法信息*/
        IMSA_ERR_LOG("IMSA_CONN_IsEmcPdpDefined:ERROR: Mem alloc fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_CONN_IsEmcPdpDefined_ENUM, LNAS_NULL_PTR);

        return IMSA_FALSE;
    }

    /* 获取所有NV项中的SDF配置信息 */
    IMSA_CONN_AssignOpid(IMSA_CONN_TYPE_EMC, &ucOpid);
    ulRslt = TAF_PS_GetCidSdfParaInfo(PS_PID_IMSA, IMSA_CLIENT_ID, ucOpid, pstSdfQueryInfo);
    if (VOS_OK != ulRslt)
    {
        IMSA_INFO_LOG("IMSA_CONN_IsEmcPdpDefined:TAF_PS_GetCidSdfParaInfo failed!");
        TLPS_PRINT2LAYER_INFO(IMSA_CONN_IsEmcPdpDefined_ENUM, IMSA_CALL_API_ERROR);

        IMSA_MEM_FREE(pstSdfQueryInfo);

        return IMSA_FALSE;
    }

    ulRslt = IMSA_CONN_FindSipSignalDialPara(   IMSA_CONN_TYPE_EMC,
                                                pstSdfQueryInfo->ulSdfNum,
                                                pstSdfQueryInfo->astSdfPara,
                                                &ulFoundIndex);

    IMSA_MEM_FREE(pstSdfQueryInfo);

    if (IMSA_FAIL == ulRslt)
    {
        IMSA_INFO_LOG("IMSA_CONN_IsEmcPdpDefined:not define emc pdp!");
        TLPS_PRINT2LAYER_INFO(IMSA_CONN_IsEmcPdpDefined_ENUM, 1);

        return IMSA_FALSE;
    }

    return IMSA_TRUE;
}


VOS_UINT32 IMSA_CONN_IsEqualToSelectedCid
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    VOS_UINT8                           ucCid
)
{
    IMSA_SEL_SDF_PARA_STRU             *pstSelSdfPara = VOS_NULL_PTR;

    if (IMSA_CONN_TYPE_NORMAL == enConnType)
    {
        pstSelSdfPara = IMSA_CONN_GetNormalConnSelSdfParaAddr();
    }
    else
    {
        pstSelSdfPara = IMSA_CONN_GetEmcConnSelSdfParaAddr();
    }

    if (ucCid == pstSelSdfPara->ucCid)
    {
        return IMSA_TRUE;
    }
    else
    {
        return IMSA_FALSE;
    }
}


VOS_VOID IMSA_CONN_PrintNrmConnState( VOS_VOID )
{
    switch (IMSA_CONN_GetNormalConnStatus())
    {
        case IMSA_CONN_STATUS_IDLE:
            IMSA_INFO_LOG("Current Normal Conn State: IDLE");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_PrintNrmConnState_ENUM, IMSA_STATUS_IDLE);
            break;
        case IMSA_CONN_STATUS_CONNING:
            IMSA_INFO_LOG("Current Normal Conn State: CONNING");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_PrintNrmConnState_ENUM, IMSA_STATUS_CONNECTING);
            break;
        case IMSA_CONN_STATUS_RELEASING:
            IMSA_INFO_LOG("Current Normal Conn State: RELEASING");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_PrintNrmConnState_ENUM, IMSA_STATUS_RELEASING);
            break;
        case IMSA_CONN_STATUS_CONN:
            IMSA_INFO_LOG("Current Normal Conn State: CONN");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_PrintNrmConnState_ENUM, IMSA_STATUS_CONN);
            break;
        default:
            IMSA_INFO_LOG("Current Normal Conn State: UNKNOWN");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_PrintNrmConnState_ENUM, IMSA_STATUS_UNKNOWN);
            break;
    }
}


VOS_VOID IMSA_CONN_PrintWifiNrmConnState( VOS_VOID )
{
    switch (IMSA_CONN_GetWifiNormalConnStatus())
    {
        case IMSA_CONN_STATUS_IDLE:
            IMSA_INFO_LOG("Current WIFI Normal Conn State: IDLE");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_PrintNrmConnState_ENUM, IMSA_STATUS_IDLE);
            break;
        case IMSA_CONN_STATUS_CONNING:
            IMSA_INFO_LOG("Current WIFI Normal Conn State: CONNING");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_PrintNrmConnState_ENUM, IMSA_STATUS_CONNECTING);
            break;
        case IMSA_CONN_STATUS_RELEASING:
            IMSA_INFO_LOG("Current WIFI Normal Conn State: RELEASING");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_PrintNrmConnState_ENUM, IMSA_STATUS_RELEASING);
            break;
        case IMSA_CONN_STATUS_CONN:
            IMSA_INFO_LOG("Current WIFI Normal Conn State: CONN");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_PrintNrmConnState_ENUM, IMSA_STATUS_CONN);
            break;
        default:
            IMSA_INFO_LOG("Current WIFI Normal Conn State: UNKNOWN");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_PrintNrmConnState_ENUM, IMSA_STATUS_UNKNOWN);
            break;
    }
}



VOS_VOID IMSA_CONN_PrintEmcConnState( VOS_VOID )
{
    switch (IMSA_CONN_GetEmcConnStatus())
    {
        case IMSA_CONN_STATUS_IDLE:
            IMSA_INFO_LOG("Current Emc Conn State: IDLE");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_PrintEmcConnState_ENUM, IMSA_STATUS_IDLE);
            break;
        case IMSA_CONN_STATUS_CONNING:
            IMSA_INFO_LOG("Current Emc Conn State: CONNING");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_PrintEmcConnState_ENUM, IMSA_STATUS_CONNECTING);
            break;
        case IMSA_CONN_STATUS_RELEASING:
            IMSA_INFO_LOG("Current Emc Conn State: RELEASING");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_PrintEmcConnState_ENUM, IMSA_STATUS_RELEASING);
            break;
        case IMSA_CONN_STATUS_CONN:
            IMSA_INFO_LOG("Current Emc Conn State: CONN");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_PrintEmcConnState_ENUM, IMSA_STATUS_CONN);
            break;
        default:
            IMSA_INFO_LOG("Current Emc Conn State: UNKNOWN");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_PrintEmcConnState_ENUM, IMSA_STATUS_UNKNOWN);
            break;
    }
}


VOS_VOID IMSA_CONN_PrintNicPdpInfo(VOS_VOID)
{
    IMSA_CONN_MANAGER_STRU  *pstConnManager  = IMSA_CONN_GetConnManagerAddr();
    IMSA_PRINT_NIC_PDP_INFO_STRU     *pstMsgNicInfo;

    pstMsgNicInfo = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_PRINT_NIC_PDP_INFO_STRU));
    if (VOS_NULL_PTR == pstMsgNicInfo)
    {
        IMSA_ERR_LOG("IMSA_CONN_PrintNicPdpInfo: mem alloc fail!.");
        TLPS_PRINT2LAYER_ERROR(IMSA_CONN_PrintNicPdpInfo_ENUM, LNAS_NULL_PTR);
        return;
    }

    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstMsgNicInfo),
                    IMSA_GET_MSG_LENGTH(pstMsgNicInfo),
                    0,
                    IMSA_GET_MSG_LENGTH(pstMsgNicInfo));

    IMSA_WRITE_INTRA_MSG_HEAD(  pstMsgNicInfo,
                                ID_IMSA_NIC_PDP_INFO_IND,
                                sizeof(IMSA_PRINT_NIC_PDP_INFO_STRU));

    IMSA_MEM_CPY_S( pstMsgNicInfo->astNicPdpInfoArray,
                    sizeof(IMSA_CONN_NIC_PDP_INFO_STRU)*IMSA_CONN_MAX_NIC_PDP_NUM,
                    pstConnManager->astNicPdpInfoArray,
                    sizeof(IMSA_CONN_NIC_PDP_INFO_STRU)*IMSA_CONN_MAX_NIC_PDP_NUM);

    IMSA_INFO_LOG("IMSA_CONN_PrintNicPdpInfo: show nic pdp info.");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_PrintNicPdpInfo_ENUM, 1);

    (VOS_VOID)LTE_MsgHook(pstMsgNicInfo);

    IMSA_FREE_MSG(pstMsgNicInfo);
}



VOS_UINT32 IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo(IMSA_CONN_NIC_PDP_INFO_STRU *pstNicPdpInfo)
{
    IMSA_EMC_CONN_STRU       *pstEmcConn      = VOS_NULL_PTR;
    IMSA_NORMAL_CONN_STRU    *pstNormalConn   = VOS_NULL_PTR;
    VOS_UINT32               i                = 0;

    IMSA_INFO_LOG("IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo_ENUM, LNAS_ENTRY);

    pstEmcConn = IMSA_CONN_GetEmcConnAddr();

    if (0 == IMSA_MEM_CMP(pstNicPdpInfo->stPdpAddr.aucIpV4Addr, pstEmcConn->stSipSignalPdp.stPdpAddr.aucIpV4Addr, IMSA_IPV4_ADDR_LEN))
    {
        IMSA_INFO_LOG("IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo: IPV4 exist in EmcPdpInfo!");
        TLPS_PRINT2LAYER_INFO(IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo_ENUM, 1);

        return IMSA_TRUE;
    }

    IMSA_INFO_LOG("IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo: IPV4 not exist in EmcPdpInfo!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo_ENUM, 2);

    pstNormalConn = IMSA_CONN_GetNormalConnAddr();

    for (i = 0; i < pstNormalConn->ulSipSignalPdpNum; i++)
    {
        if ( 0 == IMSA_MEM_CMP(pstNicPdpInfo->stPdpAddr.aucIpV4Addr, pstNormalConn->astSipSignalPdpArray[i].stPdpAddr.aucIpV4Addr, IMSA_IPV4_ADDR_LEN))
        {
            IMSA_INFO_LOG("IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo: IPV4 exist in NormalPdpInfo!");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo_ENUM, 3);
            /* WIFI->LTE HO或者重建IP地址不变, 需要更新接入技术类型和PDPID */
            pstNicPdpInfo->enPdnConnType = IMSA_IMS_DOMAIN_LTE;
            pstNicPdpInfo->ucPdpId = pstNormalConn->astSipSignalPdpArray[i].ucPdpId;
            /* 切换时,RnicId不变 */
            pstNormalConn->astSipSignalPdpArray[i].ucRnicId = pstNicPdpInfo->ucRnicId;
            IMSA_SndMsgRnicPdnModifyInd(&pstNormalConn->astSipSignalPdpArray[i]);
            return IMSA_TRUE;
        }
    }

    IMSA_INFO_LOG("IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo: IPV4 not exist in NormalPdpInfo!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo_ENUM, 4);

    return IMSA_FALSE;
}


VOS_UINT32 IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo(IMSA_CONN_NIC_PDP_INFO_STRU *pstNicPdpInfo)
{
    IMSA_EMC_CONN_STRU       *pstEmcConn      = VOS_NULL_PTR;
    IMSA_NORMAL_CONN_STRU    *pstNormalConn   = VOS_NULL_PTR;
    VOS_UINT32               i                = 0;

    IMSA_INFO_LOG("IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo_ENUM, LNAS_ENTRY);

    pstEmcConn = IMSA_CONN_GetEmcConnAddr();

    if (0 == IMSA_MEM_CMP(pstNicPdpInfo->stPdpAddr.aucIpV6Addr, pstEmcConn->stSipSignalPdp.stPdpAddr.aucIpV6Addr, IMSA_IPV6_ADDR_LEN))
    {
        IMSA_INFO_LOG("IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo: IPV6 exist in EmcPdpInfo!");
        TLPS_PRINT2LAYER_INFO(IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo_ENUM, 1);

        return IMSA_TRUE;
    }

    IMSA_INFO_LOG("IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo: IPV6 not exist in EmcPdpInfo!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo_ENUM, 2);

    pstNormalConn = IMSA_CONN_GetNormalConnAddr();

    for (i = 0; i < pstNormalConn->ulSipSignalPdpNum; i++)
    {
        if ( 0 == IMSA_MEM_CMP(pstNicPdpInfo->stPdpAddr.aucIpV6Addr, pstNormalConn->astSipSignalPdpArray[i].stPdpAddr.aucIpV6Addr, IMSA_IPV6_ADDR_LEN))
        {
            IMSA_INFO_LOG("IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo: IPV6 exist in NormalPdpInfo!");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo_ENUM, 3);

            /* WIFI->LTE HO或者重建IP地址不变, 需要更新接入技术类型和PDPID */
            pstNicPdpInfo->enPdnConnType = IMSA_IMS_DOMAIN_LTE;
            pstNicPdpInfo->ucPdpId = pstNormalConn->astSipSignalPdpArray[i].ucPdpId;
            /* 切换时,RnicId不变 */
            pstNormalConn->astSipSignalPdpArray[i].ucRnicId = pstNicPdpInfo->ucRnicId;

            IMSA_SndMsgRnicPdnModifyInd(&pstNormalConn->astSipSignalPdpArray[i]);

            return IMSA_TRUE;
        }
    }

    IMSA_INFO_LOG("IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo: IPV6 not exist in NormalPdpInfo!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo_ENUM, 4);

    return IMSA_FALSE;
}

VOS_UINT32 IMSA_CONN_Ipv4NicPdpInfoIsExistInWifiPdpInfo(IMSA_CONN_NIC_PDP_INFO_STRU *pstNicPdpInfo)
{
    IMSA_WIFI_NORMAL_CONN_STRU    *pstNormalConn   = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo_ENUM, LNAS_ENTRY);


    pstNormalConn = IMSA_CONN_GetWifiNormalConnAddr();

    if ( 0 == IMSA_MEM_CMP(pstNicPdpInfo->stPdpAddr.aucIpV4Addr, pstNormalConn->stSipSignalPdp.stPdpAddr.aucIpV4Addr, IMSA_IPV4_ADDR_LEN))
    {
        IMSA_INFO_LOG("IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo: IPV4 exist in NormalPdpInfo!");
        TLPS_PRINT2LAYER_INFO(IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo_ENUM, 3);
        /* LTE->WIFI HO或者重建IP地址不变,, 需要更新接入技术类型 */
        pstNicPdpInfo->enPdnConnType = IMSA_IMS_DOMAIN_WIFI;

        /* 切换时,RnicId不变 */
        pstNormalConn->stSipSignalPdp.ucRnicId = pstNicPdpInfo->ucRnicId;
        IMSA_SndMsgRnicWifiPdnModifyInd(&pstNormalConn->stSipSignalPdp);

        return IMSA_TRUE;
    }

    IMSA_INFO_LOG("IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo: IPV4 not exist in NormalPdpInfo!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo_ENUM, 4);

    return IMSA_FALSE;
}


VOS_UINT32 IMSA_CONN_Ipv6NicPdpInfoIsExistInWifiPdpInfo(IMSA_CONN_NIC_PDP_INFO_STRU *pstNicPdpInfo)
{
    IMSA_WIFI_NORMAL_CONN_STRU    *pstNormalConn   = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo_ENUM, LNAS_ENTRY);


    pstNormalConn = IMSA_CONN_GetWifiNormalConnAddr();

    if ( 0 == IMSA_MEM_CMP(pstNicPdpInfo->stPdpAddr.aucIpV6Addr, pstNormalConn->stSipSignalPdp.stPdpAddr.aucIpV6Addr, IMSA_IPV6_ADDR_LEN))
    {
        IMSA_INFO_LOG("IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo: IPV6 exist in NormalPdpInfo!");
        TLPS_PRINT2LAYER_INFO(IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo_ENUM, 3);

        /* LTE->WIFI HO, 需要更新接入技术类型 */
        pstNicPdpInfo->enPdnConnType = IMSA_IMS_DOMAIN_WIFI;
        /* 切换时,RnicId不变 */
        pstNormalConn->stSipSignalPdp.ucRnicId = pstNicPdpInfo->ucRnicId;

        IMSA_SndMsgRnicWifiPdnModifyInd(&pstNormalConn->stSipSignalPdp);

        return IMSA_TRUE;
    }

    IMSA_INFO_LOG("IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo: IPV6 not exist in NormalPdpInfo!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo_ENUM, 4);

    return IMSA_FALSE;
}

VOS_VOID IMSA_CONN_DeleteNicPdpInfoNotExistInLtePdpInfo(VOS_VOID)
{
    IMSA_CONN_MANAGER_STRU  *pstConnManager  = IMSA_CONN_GetConnManagerAddr();
    VOS_UINT32               ulRst           = IMSA_TRUE;
    VOS_UINT32               i               = 0;

    IMSA_INFO_LOG("IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo_ENUM, LNAS_ENTRY);

    for (i = 0; i < IMSA_CONN_MAX_NIC_PDP_NUM; i++)
    {
        if (0 == pstConnManager->astNicPdpInfoArray[i].ucIsUsed)
        {
            continue;
        }

        switch(pstConnManager->astNicPdpInfoArray[i].stPdpAddr.enIpType)
        {
        case IMSA_IP_TYPE_IPV4:
            if (IMSA_FALSE == IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo(&pstConnManager->astNicPdpInfoArray[i]))
            {
                IMSA_INFO_LOG("IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo: IPV4 not exist in PdpInfo!");
                TLPS_PRINT2LAYER_INFO(IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo_ENUM, IMSA_IPV4);

                ulRst = IMSA_FALSE;
            }

            break;
        case IMSA_IP_TYPE_IPV6:
            if (IMSA_FALSE == IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo(&pstConnManager->astNicPdpInfoArray[i]))
            {
                IMSA_INFO_LOG("IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo: IPV6 not exist in PdpInfo!");
                TLPS_PRINT2LAYER_INFO(IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo_ENUM, IMSA_IPV6);

                ulRst = IMSA_FALSE;
            }

            break;
        case IMSA_IP_TYPE_IPV4V6:
            if (IMSA_FALSE == IMSA_CONN_Ipv4NicPdpInfoIsExistInPdpInfo(&pstConnManager->astNicPdpInfoArray[i]) ||
                IMSA_FALSE == IMSA_CONN_Ipv6NicPdpInfoIsExistInPdpInfo(&pstConnManager->astNicPdpInfoArray[i]))
            {
                IMSA_INFO_LOG("IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo: IPV4V6 not exist in PdpInfo!");
                TLPS_PRINT2LAYER_INFO(IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo_ENUM, IMSA_IPV4V6);

                ulRst = IMSA_FALSE;
            }

            break;
        default:
            IMSA_INFO_LOG("IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo: IP tpye error!");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo_ENUM, IMSA_IP_TYPE_ERROR);
            break;
        }

        if (IMSA_FALSE == ulRst)
        {
            /* 防止紧急注册时，普通连接正在重建的场景或者普通注册时，
               紧急连接正在重建的场景，因此在CONNING+REG状态不能删*/
            if (IMSA_FALSE == IMSA_SRV_IsConningRegState(pstConnManager->astNicPdpInfoArray[i].ucIsEmc))
            {
                IMSA_INFO_LOG("IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo: NIC delete pdp info!");
                TLPS_PRINT2LAYER_INFO(IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo_ENUM, 1);
                IMSA_CONN_PdnInfoNicDelete(pstConnManager->astNicPdpInfoArray[i].ucRnicId);

                /* 同上海范晶确认，在删除C核虚拟网卡时，通知RNIC模块，删除虚拟网卡，
                同时给AT上报承载状态 */
                if (IMSA_FALSE == pstConnManager->astNicPdpInfoArray[i].ucIsEmc)
                {
                    IMSA_SndMsgRnicPdnDeactInd(pstConnManager->astNicPdpInfoArray[i].ucPdpId, IMSA_RNIC_IMS_RAT_TYPE_LTE);
                    IMSA_SndMsgAtPdpDeactInd(pstConnManager->astNicPdpInfoArray[i].stPdpAddr.enIpType, IMSA_AT_IMS_RAT_TYPE_LTE);
                }

                IMSA_MEM_SET_S( &pstConnManager->astNicPdpInfoArray[i],
                                sizeof(IMSA_CONN_NIC_PDP_INFO_STRU ),
                                0,
                                sizeof(IMSA_CONN_NIC_PDP_INFO_STRU ));

                pstConnManager->astNicPdpInfoArray[i].ucIsUsed = 0;
            }

            ulRst = IMSA_TRUE;
        }
    }

}



VOS_VOID IMSA_CONN_DeleteNicPdpInfoNotExistInWifiPdpInfo(VOS_VOID)
{
    IMSA_CONN_MANAGER_STRU  *pstConnManager  = IMSA_CONN_GetConnManagerAddr();
    VOS_UINT32               ulRst           = IMSA_TRUE;
    VOS_UINT32               i               = 0;

    IMSA_INFO_LOG("IMSA_CONN_DeleteNicPdpInfoNotExistInWifiPdpInfo is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo_ENUM, LNAS_ENTRY);

    for (i = 0; i < IMSA_CONN_MAX_NIC_PDP_NUM; i++)
    {
        if (0 == pstConnManager->astNicPdpInfoArray[i].ucIsUsed)
        {
            continue;
        }

        switch(pstConnManager->astNicPdpInfoArray[i].stPdpAddr.enIpType)
        {
        case IMSA_IP_TYPE_IPV4:
            if (IMSA_FALSE == IMSA_CONN_Ipv4NicPdpInfoIsExistInWifiPdpInfo(&pstConnManager->astNicPdpInfoArray[i]))
            {
                IMSA_INFO_LOG("IMSA_CONN_DeleteNicPdpInfoNotExistInWifiPdpInfo: IPV4 not exist in PdpInfo!");
                TLPS_PRINT2LAYER_INFO(IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo_ENUM, IMSA_IPV4);

                ulRst = IMSA_FALSE;
            }

            break;
        case IMSA_IP_TYPE_IPV6:
            if (IMSA_FALSE == IMSA_CONN_Ipv6NicPdpInfoIsExistInWifiPdpInfo(&pstConnManager->astNicPdpInfoArray[i]))
            {
                IMSA_INFO_LOG("IMSA_CONN_DeleteNicPdpInfoNotExistInWifiPdpInfo: IPV6 not exist in PdpInfo!");
                TLPS_PRINT2LAYER_INFO(IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo_ENUM, IMSA_IPV6);

                ulRst = IMSA_FALSE;
            }

            break;
        case IMSA_IP_TYPE_IPV4V6:
            if (IMSA_FALSE == IMSA_CONN_Ipv4NicPdpInfoIsExistInWifiPdpInfo(&pstConnManager->astNicPdpInfoArray[i]) ||
                IMSA_FALSE == IMSA_CONN_Ipv6NicPdpInfoIsExistInWifiPdpInfo(&pstConnManager->astNicPdpInfoArray[i]))
            {
                IMSA_INFO_LOG("IMSA_CONN_DeleteNicPdpInfoNotExistInWifiPdpInfo: IPV4V6 not exist in PdpInfo!");
                TLPS_PRINT2LAYER_INFO(IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo_ENUM, IMSA_IPV4V6);

                ulRst = IMSA_FALSE;
            }

            break;
        default:
            IMSA_INFO_LOG("IMSA_CONN_DeleteNicPdpInfoNotExistInWifiPdpInfo: IP tpye error!");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo_ENUM, IMSA_IP_TYPE_ERROR);
            break;
        }

        if (IMSA_FALSE == ulRst)
        {
            /* 防止紧急注册时，普通连接正在重建的场景或者普通注册时，
               紧急连接正在重建的场景，因此在CONNING+REG状态不能删*/
            if (IMSA_FALSE == IMSA_SRV_IsConningRegState(pstConnManager->astNicPdpInfoArray[i].ucIsEmc))
            {
                IMSA_INFO_LOG("IMSA_CONN_DeleteNicPdpInfoNotExistInWifiPdpInfo: NIC delete pdp info!");
                TLPS_PRINT2LAYER_INFO(IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo_ENUM, 1);

                IMSA_CONN_PdnInfoNicDelete(pstConnManager->astNicPdpInfoArray[i].ucRnicId);


                /* 同上海范晶确认，在删除C核虚拟网卡时，通知RNIC模块，删除虚拟网卡，
                同时给AT上报承载状态 */
                if (IMSA_FALSE == pstConnManager->astNicPdpInfoArray[i].ucIsEmc)
                {
                    IMSA_SndMsgRnicPdnDeactInd(pstConnManager->astNicPdpInfoArray[i].ucWifiPdnId, IMSA_RNIC_IMS_RAT_TYPE_WIFI);
                    IMSA_SndMsgAtPdpDeactInd(pstConnManager->astNicPdpInfoArray[i].stPdpAddr.enIpType, IMSA_AT_IMS_RAT_TYPE_WIFI);
                }
                IMSA_MEM_SET_S(&pstConnManager->astNicPdpInfoArray[i], sizeof(IMSA_CONN_NIC_PDP_INFO_STRU ),0, sizeof(IMSA_CONN_NIC_PDP_INFO_STRU ));
                pstConnManager->astNicPdpInfoArray[i].ucIsUsed = 0;
            }

            ulRst = IMSA_TRUE;
        }
    }
}



VOS_VOID IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo(VOS_VOID)
{

    IMSA_INFO_LOG("IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo_ENUM, LNAS_ENTRY);

    if (IMSA_IMS_DOMAIN_LTE == IMSA_GetCurrentDomain())
    {
        IMSA_CONN_DeleteNicPdpInfoNotExistInLtePdpInfo();
    }
    else
    {
        IMSA_CONN_DeleteNicPdpInfoNotExistInWifiPdpInfo();
    }

}


VOS_UINT32 IMSA_CONN_Ipv4PdpAddrIsExistInNicPdpInfo(const IMSA_IP_ADDRESS_STRU *pstPdpAddr)
{
    IMSA_CONN_MANAGER_STRU  *pstConnManager  = IMSA_CONN_GetConnManagerAddr();
    VOS_UINT32               i               = 0;

    for (i = 0; i < IMSA_CONN_MAX_NIC_PDP_NUM; i++)
    {
        if (0 == pstConnManager->astNicPdpInfoArray[i].ucIsUsed)
        {
            continue;
        }

        if ( 0 == IMSA_MEM_CMP(pstPdpAddr->aucIpV4Addr, pstConnManager->astNicPdpInfoArray[i].stPdpAddr.aucIpV4Addr, IMSA_IPV4_ADDR_LEN))
        {
            IMSA_INFO_LOG("IMSA_CONN_Ipv4PdpAddrIsExistInNicPdpInfo: IPV4 exist in NicPdpInfo!");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_Ipv4PdpAddrIsExistInNicPdpInfo_ENUM, IMSA_EXISTED);
            return IMSA_TRUE;
        }
    }

    IMSA_INFO_LOG("IMSA_CONN_Ipv4PdpAddrIsExistInNicPdpInfo: IPV4 not exist in NicPdpInfo!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_Ipv4PdpAddrIsExistInNicPdpInfo_ENUM, 2);

    return IMSA_FALSE;
}


VOS_UINT32 IMSA_CONN_Ipv6PdpAddrIsExistInNicPdpInfo(const IMSA_IP_ADDRESS_STRU *pstPdpAddr)
{
    IMSA_CONN_MANAGER_STRU  *pstConnManager    = IMSA_CONN_GetConnManagerAddr();
    VOS_UINT32               i                 = 0;

    for (i = 0; i < IMSA_CONN_MAX_NIC_PDP_NUM; i++)
    {
        if (0 == pstConnManager->astNicPdpInfoArray[i].ucIsUsed)
        {
            continue;
        }

        if ( 0 == IMSA_MEM_CMP(pstPdpAddr->aucIpV6Addr, pstConnManager->astNicPdpInfoArray[i].stPdpAddr.aucIpV6Addr, IMSA_IPV6_ADDR_LEN))
        {
            IMSA_INFO_LOG("IMSA_CONN_Ipv6PdpAddrIsExistInNicPdpInfo: IPV6 exist in NicPdpInfo!");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_Ipv6PdpAddrIsExistInNicPdpInfo_ENUM, IMSA_EXISTED);
            return IMSA_TRUE;
        }
    }

    IMSA_INFO_LOG("IMSA_CONN_Ipv6PdpAddrIsExistInNicPdpInfo: IPV6 not exist in NicPdpInfo!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_Ipv6PdpAddrIsExistInNicPdpInfo_ENUM, 2);

    return IMSA_FALSE;
}


VOS_UINT32 IMSA_CONN_RnicIdIsExistInNicPdpInfo(VOS_UINT8 ucRnicId, VOS_UINT32 *pulArrayIndex)
{
    IMSA_CONN_MANAGER_STRU  *pstConnManager    = IMSA_CONN_GetConnManagerAddr();
    VOS_UINT32               i                 = 0;

    for (i = 0; i < IMSA_CONN_MAX_NIC_PDP_NUM; i++)
    {
        if (0 == pstConnManager->astNicPdpInfoArray[i].ucIsUsed)
        {
            continue;
        }

        if (ucRnicId == pstConnManager->astNicPdpInfoArray[i].ucRnicId)
        {
            IMSA_INFO_LOG("IMSA_CONN_RnicIdIsExistInNicPdpInfo: PdpId exist in NicPdpInfo!");
            *pulArrayIndex = i;
            return IMSA_TRUE;
        }
    }

    IMSA_INFO_LOG("IMSA_CONN_RnicIdIsExistInNicPdpInfo: PdpId not exist in NicPdpInfo!");
    /* TLPS_PRINT2LAYER_INFO(IMSA_CONN_RnicIdIsExistInNicPdpInfo_ENUM, 2);*/

    return IMSA_FALSE;
}



VOS_UINT32 IMSA_CONN_InquireNotUsedInNicPdpInfo(VOS_UINT32 *pulArrayIndex)
{
    IMSA_CONN_MANAGER_STRU  *pstConnManager  = IMSA_CONN_GetConnManagerAddr();
    VOS_UINT32               i               = 0;

    for (i = 0; i < IMSA_CONN_MAX_NIC_PDP_NUM; i++)
    {
        if (0 == pstConnManager->astNicPdpInfoArray[i].ucIsUsed)
        {
            IMSA_INFO_LOG("IMSA_CONN_InquireNotUsedInNicPdpInfo: inquire succ!");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_InquireNotUsedInNicPdpInfo_ENUM, LNAS_SUCC);
            *pulArrayIndex = i;
            return  IMSA_TRUE;
        }
    }
    IMSA_INFO_LOG("IMSA_CONN_InquireNotUsedInNicPdpInfo: inquire fail!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_InquireNotUsedInNicPdpInfo_ENUM, LNAS_FAIL);

    return IMSA_FALSE;
}


VOS_VOID IMSA_CONN_SaveNicPdpInfo(VOS_UINT8 ucRnicId,  VOS_UINT8  ucPdpId,      VOS_UINT8 ucIsEmc,
                                            VOS_UINT32 ulArrayIndex, const IMSA_IP_ADDRESS_STRU *pstPdpAddr)
{
    IMSA_CONN_MANAGER_STRU  *pstConnManager  = IMSA_CONN_GetConnManagerAddr();

    IMSA_INFO_LOG("IMSA_CONN_SaveNicPdpInfo is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_SaveNicPdpInfo_ENUM, LNAS_ENTRY);

    IMSA_MEM_CPY_S(&pstConnManager->astNicPdpInfoArray[ulArrayIndex].stPdpAddr,
                    sizeof(IMSA_IP_ADDRESS_STRU),
                    pstPdpAddr,
                    sizeof(IMSA_IP_ADDRESS_STRU));
    pstConnManager->astNicPdpInfoArray[ulArrayIndex].ucIsUsed = 1;
    pstConnManager->astNicPdpInfoArray[ulArrayIndex].ucIsEmc = ucIsEmc;
    pstConnManager->astNicPdpInfoArray[ulArrayIndex].ucRnicId = ucRnicId;
    pstConnManager->astNicPdpInfoArray[ulArrayIndex].ucPdpId  = ucPdpId;
    pstConnManager->astNicPdpInfoArray[ulArrayIndex].ucWifiPdnId = 0;
    pstConnManager->astNicPdpInfoArray[ulArrayIndex].enPdnConnType  = IMSA_IMS_DOMAIN_LTE;
}


VOS_VOID IMSA_CONN_SaveWifiNicPdpInfo(VOS_UINT8 ucRnicId,  VOS_UINT8  ucWifiPdnId,      VOS_UINT8 ucIsEmc,
                                            VOS_UINT32 ulArrayIndex, const IMSA_IP_ADDRESS_STRU *pstPdpAddr)
{
    IMSA_CONN_MANAGER_STRU  *pstConnManager  = IMSA_CONN_GetConnManagerAddr();

    IMSA_INFO_LOG("IMSA_CONN_SaveNicPdpInfo is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_SaveNicPdpInfo_ENUM, LNAS_ENTRY);

    IMSA_MEM_CPY_S(&pstConnManager->astNicPdpInfoArray[ulArrayIndex].stPdpAddr,  sizeof(IMSA_IP_ADDRESS_STRU),pstPdpAddr, sizeof(IMSA_IP_ADDRESS_STRU));
    pstConnManager->astNicPdpInfoArray[ulArrayIndex].ucIsUsed = 1;
    pstConnManager->astNicPdpInfoArray[ulArrayIndex].ucIsEmc = ucIsEmc;
    pstConnManager->astNicPdpInfoArray[ulArrayIndex].ucRnicId = ucRnicId;
    pstConnManager->astNicPdpInfoArray[ulArrayIndex].ucPdpId  = 0;
    pstConnManager->astNicPdpInfoArray[ulArrayIndex].ucWifiPdnId = ucWifiPdnId;
    pstConnManager->astNicPdpInfoArray[ulArrayIndex].enPdnConnType  = IMSA_IMS_DOMAIN_WIFI;
}



VOS_VOID IMSA_CONN_ProcIPV4PdpInfoForNic(VOS_UINT8 ucIsEmc, IMSA_PDP_CNTXT_INFO_STRU *pstPdpCntxtInfo)
{
    IMSA_IP_ADDRESS_STRU     *pstImsaIpAddr   = VOS_NULL_PTR;
    VOS_UINT32               ulArrayIndex     = 0;
    VOS_UINT8                ucPdpId          = 0;
    VOS_UINT8                ucRnicId         = 0;

    IMSA_INFO_LOG("IMSA_CONN_ProcIPV4PdpInfoForNic is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_ProcIPV4PdpInfoForNic_ENUM, LNAS_ENTRY);

    pstImsaIpAddr = &pstPdpCntxtInfo->stPdpAddr;
    ucPdpId       = pstPdpCntxtInfo->ucPdpId;
    ucRnicId      = pstPdpCntxtInfo->ucRnicId;

    if (IMSA_FALSE == IMSA_CONN_Ipv4PdpAddrIsExistInNicPdpInfo(pstImsaIpAddr))
    {
        if (IMSA_TRUE == IMSA_CONN_InquireNotUsedInNicPdpInfo(&ulArrayIndex))
        {
            IMSA_CONN_SaveNicPdpInfo(ucRnicId,ucPdpId, ucIsEmc, ulArrayIndex, pstImsaIpAddr);

            IMSA_CONN_ConfigPdpInfo2Bsp(pstPdpCntxtInfo);
            /* 同上海范晶确认，在绑定C核虚拟网卡时，通知RNIC模块，绑定虚拟网卡，
            同时给AT上报承载状态 */
            if (VOS_FALSE == ucIsEmc)
            {
                IMSA_SndMsgRnicPdnActInd(pstPdpCntxtInfo);
                IMSA_SndMsgAtPdpActInd(pstPdpCntxtInfo);
            }
        }
        else
        {
            IMSA_INFO_LOG("IMSA_CONN_ProcIPV4PdpInfoForNic: Inquire notUsed for ipv4 fail!");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_ProcIPV4PdpInfoForNic_ENUM, LNAS_FAIL);
        }
    }
}


VOS_VOID IMSA_CONN_ProcIPV6PdpInfoForNic(VOS_UINT8 ucIsEmc, IMSA_PDP_CNTXT_INFO_STRU *pstPdpCntxtInfo)
{
    IMSA_IP_ADDRESS_STRU     *pstImsaIpAddr   = VOS_NULL_PTR;
    VOS_UINT32               ulArrayIndex     = 0;
    VOS_UINT8                ucPdpId          = 0;
    VOS_UINT8                ucRnicId         = 0;
    VOS_UINT8                aucTemp[IMSA_IPV6_PREFIX_LEN]   = {0};

    IMSA_INFO_LOG("IMSA_CONN_ProcIPV6PdpInfoForNic is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_ProcIPV6PdpInfoForNic_ENUM, LNAS_ENTRY);

    pstImsaIpAddr = &pstPdpCntxtInfo->stPdpAddr;
    ucPdpId       = pstPdpCntxtInfo->ucPdpId;
    ucRnicId      = pstPdpCntxtInfo->ucRnicId;

    if (0 == IMSA_MEM_CMP(aucTemp, pstImsaIpAddr->aucIpV6Addr, IMSA_IPV6_PREFIX_LEN))
    {
        IMSA_INFO_LOG("IMSA_CONN_ProcIPV6PdpInfoForNic: IPV6 prefix is 0!");
        TLPS_PRINT2LAYER_INFO(IMSA_CONN_ProcIPV6PdpInfoForNic_ENUM, LNAS_PARAM_INVALID);
    }
    else
    {
        if (IMSA_FALSE == IMSA_CONN_Ipv6PdpAddrIsExistInNicPdpInfo(pstImsaIpAddr))
        {
            if (IMSA_TRUE == IMSA_CONN_InquireNotUsedInNicPdpInfo(&ulArrayIndex))
            {
                IMSA_CONN_SaveNicPdpInfo(ucRnicId,ucPdpId, ucIsEmc, ulArrayIndex, pstImsaIpAddr);

                IMSA_CONN_ConfigPdpInfo2Bsp(pstPdpCntxtInfo);
                /* 同上海范晶确认，在绑定C核虚拟网卡时，通知RNIC模块，绑定虚拟网卡，
                同时给AT上报承载状态 */
                if (VOS_FALSE == ucIsEmc)
                {
                    IMSA_SndMsgRnicPdnActInd(pstPdpCntxtInfo);
                    IMSA_SndMsgAtPdpActInd(pstPdpCntxtInfo);
                }
            }
            else
            {
                IMSA_INFO_LOG("IMSA_CONN_ProcIPV6PdpInfoForNic: Inquire notUsed for ipv6 fail!");
                TLPS_PRINT2LAYER_INFO(IMSA_CONN_ProcIPV6PdpInfoForNic_ENUM, LNAS_FAIL);
            }
        }
    }
}


VOS_VOID IMSA_CONN_ProcIPV4V6PdpInfoForNic(VOS_UINT8 ucIsEmc, IMSA_PDP_CNTXT_INFO_STRU *pstPdpCntxtInfo)
{
    IMSA_CONN_MANAGER_STRU   *pstConnManager  = IMSA_CONN_GetConnManagerAddr();
    IMSA_IP_ADDRESS_STRU     *pstImsaIpAddr   = VOS_NULL_PTR;
    VOS_UINT32               ulArrayIndex     = 0;
    VOS_UINT8                ucPdpId          = 0;
    VOS_UINT8                ucRnicId         = 0;
    VOS_UINT8                aucTemp[IMSA_IPV6_PREFIX_LEN]   = {0};

    IMSA_INFO_LOG("IMSA_CONN_ProcIPV4V6PdpInfoForNic is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_ProcIPV4V6PdpInfoForNic_ENUM, LNAS_ENTRY);

    pstImsaIpAddr = &pstPdpCntxtInfo->stPdpAddr;
    ucPdpId       = pstPdpCntxtInfo->ucPdpId;
    ucRnicId      = pstPdpCntxtInfo->ucRnicId;

    /* if (0 == IMSA_MEM_CMP(aucTemp, pstImsaIpAddr->aucIpV6Addr, IMSA_IPV6_PREFIX_LEN))
    {
        IMSA_INFO_LOG("IMSA_CONN_ProcIPV4V6PdpInfoForNic: IPV6 prefix is 0, !");
        return;
    } */

    if ((IMSA_TRUE == IMSA_CONN_Ipv4PdpAddrIsExistInNicPdpInfo(pstImsaIpAddr)) &&
        (IMSA_TRUE == IMSA_CONN_Ipv6PdpAddrIsExistInNicPdpInfo(pstImsaIpAddr)))
    {
        IMSA_INFO_LOG("IMSA_CONN_ProcIPV4V6PdpInfoForNic: IPV4V6 exist in PdpInfo!");
        return;
    }

    if (IMSA_TRUE == IMSA_CONN_RnicIdIsExistInNicPdpInfo(ucRnicId, &ulArrayIndex))
    {

        IMSA_CONN_PdnInfoNicDelete(pstPdpCntxtInfo->ucRnicId);

        IMSA_CONN_SaveNicPdpInfo(ucRnicId,ucPdpId, ucIsEmc, ulArrayIndex, pstImsaIpAddr);

        if (0 == IMSA_MEM_CMP(aucTemp, pstConnManager->astNicPdpInfoArray[ulArrayIndex].stPdpAddr.aucIpV6Addr, IMSA_IPV6_PREFIX_LEN))
        {
            pstConnManager->astNicPdpInfoArray[ulArrayIndex].stPdpAddr.enIpType = IMSA_IP_TYPE_IPV4;
        }

        IMSA_CONN_ConfigPdpInfo2Bsp(pstPdpCntxtInfo);
        /* 同上海范晶确认，在绑定C核虚拟网卡时，通知RNIC模块，绑定虚拟网卡，
        同时给AT上报承载状态 */
        if (VOS_FALSE == ucIsEmc)
        {
            IMSA_SndMsgRnicPdnActInd(pstPdpCntxtInfo);
            IMSA_SndMsgAtPdpActInd(pstPdpCntxtInfo);
        }
    }
    else
    {
        if (IMSA_TRUE == IMSA_CONN_InquireNotUsedInNicPdpInfo(&ulArrayIndex))
        {
            IMSA_CONN_SaveNicPdpInfo(ucRnicId,ucPdpId, ucIsEmc, ulArrayIndex, pstImsaIpAddr);

            if (0 == IMSA_MEM_CMP(aucTemp, pstConnManager->astNicPdpInfoArray[ulArrayIndex].stPdpAddr.aucIpV6Addr, IMSA_IPV6_PREFIX_LEN))
            {
                pstConnManager->astNicPdpInfoArray[ulArrayIndex].stPdpAddr.enIpType = IMSA_IP_TYPE_IPV4;
            }

            IMSA_CONN_ConfigPdpInfo2Bsp(pstPdpCntxtInfo);
            /* 同上海范晶确认，在绑定C核虚拟网卡时，通知RNIC模块，绑定虚拟网卡，
            同时给AT上报承载状态 */
            if (VOS_FALSE == ucIsEmc)
            {
                IMSA_SndMsgRnicPdnActInd(pstPdpCntxtInfo);
                IMSA_SndMsgAtPdpActInd(pstPdpCntxtInfo);
            }
        }
        else
        {
            IMSA_INFO_LOG("IMSA_CONN_ProcIPV4V6PdpInfoForNic: Inquire notUsed for ipv6 fail!");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_ProcIPV4V6PdpInfoForNic_ENUM, LNAS_FAIL);
        }
    }

    return;
}


VOS_VOID IMSA_CONN_ProcWifiIPV4PdpInfoForNic(VOS_UINT8 ucIsEmc, IMSA_WIFI_PDP_CNTXT_INFO_STRU *pstPdpCntxtInfo)
{
    IMSA_WIFI_IP_ADDRESS_STRU      *pstImsaIpAddr   = VOS_NULL_PTR;
    VOS_UINT32                      ulArrayIndex     = 0;
    VOS_UINT8                       ucWifiPdnId          = 0;
    VOS_UINT8                       ucRnicId          = 0;

    IMSA_INFO_LOG("IMSA_CONN_ProcWifiIPV4PdpInfoForNic is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_ProcIPV4PdpInfoForNic_ENUM, LNAS_ENTRY);

    pstImsaIpAddr = &pstPdpCntxtInfo->stPdpAddr;
    ucRnicId   = pstPdpCntxtInfo->ucRnicId;
    ucWifiPdnId  = 0; /* 目前IMS仅支持1个PDN, 缺省添0 */

    if (IMSA_FALSE == IMSA_CONN_Ipv4PdpAddrIsExistInNicPdpInfo((IMSA_IP_ADDRESS_STRU *)pstImsaIpAddr))
    {
        if (IMSA_TRUE == IMSA_CONN_InquireNotUsedInNicPdpInfo(&ulArrayIndex))
        {
            IMSA_CONN_SaveWifiNicPdpInfo(ucRnicId,ucWifiPdnId, ucIsEmc, ulArrayIndex, (IMSA_IP_ADDRESS_STRU *)pstImsaIpAddr);

            IMSA_CONN_ConfigWifiPdpInfo2Bsp(pstPdpCntxtInfo);

            /*给A核虚拟网卡配置承载信息*/
            IMSA_SndMsgRnicWifiPdnActInd(pstPdpCntxtInfo);
            IMSA_SndMsgAtWifiPdpActInd(pstPdpCntxtInfo);
        }
        else
        {
            IMSA_INFO_LOG("IMSA_CONN_ProcIPV4PdpInfoForNic: Inquire notUsed for ipv4 fail!");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_ProcIPV4PdpInfoForNic_ENUM, LNAS_FAIL);
        }
    }
}


VOS_VOID IMSA_CONN_ProcWifiIPV6PdpInfoForNic(VOS_UINT8 ucIsEmc, IMSA_WIFI_PDP_CNTXT_INFO_STRU *pstPdpCntxtInfo)
{
    IMSA_WIFI_IP_ADDRESS_STRU      *pstImsaIpAddr   = VOS_NULL_PTR;
    VOS_UINT32                      ulArrayIndex     = 0;
    VOS_UINT8                       ucWifiPdnId          = 0;
    VOS_UINT8                       ucRnicId          = 0;
    VOS_UINT8                       aucTemp[IMSA_IPV6_PREFIX_LEN]   = {0};

    IMSA_INFO_LOG("IMSA_CONN_ProcIPV6PdpInfoForNic is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_ProcIPV6PdpInfoForNic_ENUM, LNAS_ENTRY);

    pstImsaIpAddr = &pstPdpCntxtInfo->stPdpAddr;
    ucRnicId   = pstPdpCntxtInfo->ucRnicId;
    ucWifiPdnId  = 0; /* 目前IMS仅支持1个PDN, 缺省添0 */

    if (0 == IMSA_MEM_CMP(aucTemp, pstImsaIpAddr->aucIpV6Addr, IMSA_IPV6_PREFIX_LEN))
    {
        IMSA_INFO_LOG("IMSA_CONN_ProcIPV6PdpInfoForNic: IPV6 prefix is 0!");
        TLPS_PRINT2LAYER_INFO(IMSA_CONN_ProcIPV6PdpInfoForNic_ENUM, LNAS_PARAM_INVALID);
    }
    else
    {
        if (IMSA_FALSE == IMSA_CONN_Ipv6PdpAddrIsExistInNicPdpInfo((IMSA_IP_ADDRESS_STRU *)pstImsaIpAddr))
        {
            if (IMSA_TRUE == IMSA_CONN_InquireNotUsedInNicPdpInfo(&ulArrayIndex))
            {
                IMSA_CONN_SaveWifiNicPdpInfo(ucRnicId,ucWifiPdnId, ucIsEmc, ulArrayIndex, (IMSA_IP_ADDRESS_STRU *)pstImsaIpAddr);

                IMSA_CONN_ConfigWifiPdpInfo2Bsp(pstPdpCntxtInfo);

                /*给A核虚拟网卡配置承载信息*/
                IMSA_SndMsgRnicWifiPdnActInd(pstPdpCntxtInfo);
                IMSA_SndMsgAtWifiPdpActInd(pstPdpCntxtInfo);

            }
            else
            {
                IMSA_INFO_LOG("IMSA_CONN_ProcIPV6PdpInfoForNic: Inquire notUsed for ipv6 fail!");
                TLPS_PRINT2LAYER_INFO(IMSA_CONN_ProcIPV6PdpInfoForNic_ENUM, LNAS_FAIL);
            }
        }
    }
}


VOS_VOID IMSA_CONN_ProcWifiIPV4V6PdpInfoForNic(VOS_UINT8 ucIsEmc, IMSA_WIFI_PDP_CNTXT_INFO_STRU *pstPdpCntxtInfo)
{
    IMSA_CONN_MANAGER_STRU         *pstConnManager  = IMSA_CONN_GetConnManagerAddr();
    IMSA_WIFI_IP_ADDRESS_STRU      *pstImsaIpAddr   = VOS_NULL_PTR;
    VOS_UINT32                      ulArrayIndex     = 0;
    VOS_UINT8                       ucWifiPdnId          = 0;
    VOS_UINT8                       ucRnicId          = 0;
    VOS_UINT8                       aucTemp[IMSA_IPV6_PREFIX_LEN]   = {0};

    IMSA_INFO_LOG("IMSA_CONN_ProcIPV4V6PdpInfoForNic is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_ProcIPV4V6PdpInfoForNic_ENUM, LNAS_ENTRY);

    pstImsaIpAddr = &pstPdpCntxtInfo->stPdpAddr;
    ucRnicId   = pstPdpCntxtInfo->ucRnicId;
    ucWifiPdnId  = 0; /* 目前IMS仅支持1个PDN, 缺省添0 */

    if ((IMSA_TRUE == IMSA_CONN_Ipv4PdpAddrIsExistInNicPdpInfo((IMSA_IP_ADDRESS_STRU *)pstImsaIpAddr)) &&
        (IMSA_TRUE == IMSA_CONN_Ipv6PdpAddrIsExistInNicPdpInfo((IMSA_IP_ADDRESS_STRU *)pstImsaIpAddr)))
    {
        IMSA_INFO_LOG("IMSA_CONN_ProcIPV4V6PdpInfoForNic: IPV4V6 exist in PdpInfo!");
        return;
    }

    if (IMSA_TRUE == IMSA_CONN_RnicIdIsExistInNicPdpInfo(ucRnicId, &ulArrayIndex))
    {
        IMSA_CONN_PdnInfoNicDelete(ucRnicId);

        IMSA_CONN_SaveWifiNicPdpInfo(ucRnicId,ucWifiPdnId, ucIsEmc, ulArrayIndex, (IMSA_IP_ADDRESS_STRU *)pstImsaIpAddr);

        if (0 == IMSA_MEM_CMP(aucTemp, pstConnManager->astNicPdpInfoArray[ulArrayIndex].stPdpAddr.aucIpV6Addr, IMSA_IPV6_PREFIX_LEN))
        {
            pstConnManager->astNicPdpInfoArray[ulArrayIndex].stPdpAddr.enIpType = IMSA_IP_TYPE_IPV4;
        }

        IMSA_CONN_ConfigWifiPdpInfo2Bsp(pstPdpCntxtInfo);

        /*给A核虚拟网卡配置承载信息*/
        IMSA_SndMsgRnicWifiPdnActInd(pstPdpCntxtInfo);
        IMSA_SndMsgAtWifiPdpActInd(pstPdpCntxtInfo);

    }
    else
    {
        if (IMSA_TRUE == IMSA_CONN_InquireNotUsedInNicPdpInfo(&ulArrayIndex))
        {
            IMSA_CONN_SaveWifiNicPdpInfo(ucRnicId,ucWifiPdnId, ucIsEmc, ulArrayIndex, (IMSA_IP_ADDRESS_STRU *)pstImsaIpAddr);

            if (0 == IMSA_MEM_CMP(aucTemp, pstConnManager->astNicPdpInfoArray[ulArrayIndex].stPdpAddr.aucIpV6Addr, IMSA_IPV6_PREFIX_LEN))
            {
                pstConnManager->astNicPdpInfoArray[ulArrayIndex].stPdpAddr.enIpType = IMSA_IP_TYPE_IPV4;
            }

            IMSA_CONN_ConfigWifiPdpInfo2Bsp(pstPdpCntxtInfo);

            /*给A核虚拟网卡配置承载信息*/
            IMSA_SndMsgRnicWifiPdnActInd(pstPdpCntxtInfo);
            IMSA_SndMsgAtWifiPdpActInd(pstPdpCntxtInfo);

        }
        else
        {
            IMSA_INFO_LOG("IMSA_CONN_ProcIPV4V6PdpInfoForNic: Inquire notUsed for ipv6 fail!");
            TLPS_PRINT2LAYER_INFO(IMSA_CONN_ProcIPV4V6PdpInfoForNic_ENUM, LNAS_FAIL);
        }
    }

    return;
}


VOS_VOID IMSA_CONN_AddNormalPdpInfo2NicPdpInfo(VOS_VOID)
{
    IMSA_NORMAL_CONN_STRU              *pstNormalConn       = VOS_NULL_PTR;
    IMSA_WIFI_NORMAL_CONN_STRU         *pstWifiNormalConn   = VOS_NULL_PTR;
    VOS_UINT32               i                = 0;

    IMSA_INFO_LOG("IMSA_CONN_AddNormalPdpInfo2NicPdpInfo is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_AddNormalPdpInfo2NicPdpInfo_ENUM, LNAS_ENTRY);

    if (IMSA_IMS_DOMAIN_LTE == IMSA_GetCurrentDomain())
    {

    pstNormalConn = IMSA_CONN_GetNormalConnAddr();

        for (i = 0; i < pstNormalConn->ulSipSignalPdpNum; i ++ )
        {
            switch(pstNormalConn->astSipSignalPdpArray[i].stPdpAddr.enIpType)
            {
            case IMSA_IP_TYPE_IPV4:
                IMSA_CONN_ProcIPV4PdpInfoForNic(0, &pstNormalConn->astSipSignalPdpArray[i]);
                break;
            case IMSA_IP_TYPE_IPV6:
                IMSA_CONN_ProcIPV6PdpInfoForNic(0, &pstNormalConn->astSipSignalPdpArray[i]);
                break;
            case IMSA_IP_TYPE_IPV4V6:
                IMSA_CONN_ProcIPV4V6PdpInfoForNic(0, &pstNormalConn->astSipSignalPdpArray[i]);
                break;
            default:
                IMSA_INFO_LOG("IMSA_CONN_AddNormalPdpInfo2NicPdpInfo: enIpType error!");
                TLPS_PRINT2LAYER_INFO(IMSA_CONN_AddNormalPdpInfo2NicPdpInfo_ENUM, LNAS_PARAM_INVALID);
                break;
            }
        }
    }
    else if (IMSA_IMS_DOMAIN_WIFI == IMSA_GetCurrentDomain())
    {
        pstWifiNormalConn = IMSA_CONN_GetWifiNormalConnAddr();

        switch(pstWifiNormalConn->stSipSignalPdp.enPdnType)
        {
            case IMSA_IP_TYPE_IPV4:
                IMSA_CONN_ProcWifiIPV4PdpInfoForNic(0, &pstWifiNormalConn->stSipSignalPdp);
                break;
            case IMSA_IP_TYPE_IPV6:
                IMSA_CONN_ProcWifiIPV6PdpInfoForNic(0, &pstWifiNormalConn->stSipSignalPdp);
                break;
            case IMSA_IP_TYPE_IPV4V6:
                IMSA_CONN_ProcWifiIPV4V6PdpInfoForNic(0, &pstWifiNormalConn->stSipSignalPdp);
                break;
            default:
                IMSA_INFO_LOG("IMSA_CONN_AddNormalPdpInfo2NicPdpInfo: enIpType error!");
                TLPS_PRINT2LAYER_INFO(IMSA_CONN_AddNormalPdpInfo2NicPdpInfo_ENUM, LNAS_PARAM_INVALID);
                break;
        }
    }
    else
    {
    }
}


VOS_VOID IMSA_CONN_AddEmcPdpInfo2NicPdpInfo(VOS_VOID)
{
    IMSA_EMC_CONN_STRU       *pstEmcConn      = VOS_NULL_PTR;

    IMSA_INFO_LOG("IMSA_CONN_AddEmcPdpInfo2NicPdpInfo is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_AddEmcPdpInfo2NicPdpInfo_ENUM, LNAS_ENTRY);

    pstEmcConn = IMSA_CONN_GetEmcConnAddr();

    switch(pstEmcConn->stSipSignalPdp.stPdpAddr.enIpType)
    {
    case IMSA_IP_TYPE_IPV4:
        IMSA_CONN_ProcIPV4PdpInfoForNic(1, &pstEmcConn->stSipSignalPdp);
        break;
    case IMSA_IP_TYPE_IPV6:
        IMSA_CONN_ProcIPV6PdpInfoForNic(1, &pstEmcConn->stSipSignalPdp);
        break;
    case IMSA_IP_TYPE_IPV4V6:
        IMSA_CONN_ProcIPV4V6PdpInfoForNic(1, &pstEmcConn->stSipSignalPdp);
        break;
    default:
        IMSA_INFO_LOG("IMSA_CONN_AddEmcPdpInfo2NicPdpInfo: enIpType error!");
        TLPS_PRINT2LAYER_INFO(IMSA_CONN_AddEmcPdpInfo2NicPdpInfo_ENUM, LNAS_PARAM_INVALID);
        break;
    }
}


VOS_VOID IMSA_CONN_AddPdpInfo2NicPdpInfo(VOS_VOID)
{
    IMSA_INFO_LOG("IMSA_CONN_AddPdpInfo2NicPdpInfo is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_AddPdpInfo2NicPdpInfo_ENUM, LNAS_ENTRY);

    /* 识别存在于最新普通信令承载信息中，但不存在于NIC PDP INFO中的IP地址，
       添加到NIC PDP INFO中，并配置给TTF */
    if (IMSA_SRV_STATUS_CONNING_REG != IMSA_SRV_GetNormalSrvStatus())
    {
        IMSA_CONN_AddNormalPdpInfo2NicPdpInfo();
    }

    /* 识别存在于最新紧急信令承载信息中，但不存在于NIC PDP INFO中的IP地址，
       添加到NIC PDP INFO中，并配置给TTF */
    if (IMSA_SRV_STATUS_CONNING_REG != IMSA_SRV_GetEmcSrvStatus())
    {
        IMSA_CONN_AddEmcPdpInfo2NicPdpInfo();
    }
}



VOS_VOID IMSA_CONN_UpdateNicPdpInfo( VOS_VOID )
{

    IMSA_INFO_LOG("IMSA_CONN_UpdateNicPdpInfo is entered!");
    TLPS_PRINT2LAYER_INFO(IMSA_CONN_UpdateNicPdpInfo_ENUM, LNAS_ENTRY);
    /* 将更新前的NIC PDP信息上传到HIDS上显示 */
    IMSA_CONN_PrintNicPdpInfo();

    /* 识别存在于NIC PDP INFO中，但不存在于最新承载信息中的IP地址，
       将其从NIC PDP INFO中删除，并通知TTF删除 */
    IMSA_CONN_DeleteNicPdpInfoNotExistInPdpInfo();

    /* 识别存在于最新承载信息中，但不存在于NIC PDP INFO中的IP地址，
       添加到NIC PDP INFO中，并配置给TTF */
    IMSA_CONN_AddPdpInfo2NicPdpInfo ();

    /* 将更新后的NIC PDP信息上传到HIDS上显示 */
    IMSA_CONN_PrintNicPdpInfo();
}


VOS_VOID IMSA_CONN_SetupConnFail
(
    IMSA_CONN_TYPE_ENUM_UINT32          enConnType,
    IMSA_CONN_RESULT_ENUM_UINT32        enResult,
    TAF_PS_CAUSE_ENUM_UINT32            enCause,
    IMSA_IMS_DOMAIN_ENUM_UINT8           enConnRatType
)
{
    #if (FEATURE_ON == FEATURE_PTM)
    IMSA_ERR_LOG_PDNREJ_CAUSE_ENUM_UINT32   enPdnConnRejCause = IMSA_ERR_LOG_PDNREJ_CAUSE_BUTT;
    #endif

    /* 更新连接状态为IDLE */
    IMSA_CONN_SetConnStatus(enConnType, IMSA_CONN_STATUS_IDLE, enConnRatType);

    /* 清除连接实体信息 */
    IMSA_CONN_ClearConnResource(enConnType);

    IMSA_CONN_SndConnSetupInd(  enResult,
                                enConnType,
                                IMSA_CONN_SIP_PDP_TYPE_SIGNAL,
                                enConnRatType);

    #if (FEATURE_ON == FEATURE_PTM)
    enPdnConnRejCause = IMSA_PDN_InterRejCauseProc(enResult, enCause);
    IMSA_PdnRejErrRecord(enPdnConnRejCause);
    #endif
}
#if (FEATURE_ON == FEATURE_PTM)
/*****************************************************************************
 Function Name   :  IMSA_PDN_CnRejCauseProc
 Description     :  将TAF上报的拒绝原因转换成对应的网侧拒绝原因值
 Input           :  enCause 表示TAF上报的拒绝原因值

 Output          :  输出一个网侧拒绝原因值
 Return          :

 History         :
    1.wangchen 　00209181 2015-01-08 新增文件

*****************************************************************************/
IMSA_ERR_LOG_PDNREJ_CAUSE_ENUM_UINT32  IMSA_PDN_CnRejCauseProc
(
    TAF_PS_CAUSE_ENUM_UINT32 enCause
)
{
    IMSA_ERR_LOG_PDNREJ_CAUSE_ENUM_UINT32   enImsaCnCause = IMSA_ERR_LOG_PDNREJ_CAUSE_BUTT;
    VOS_UINT32 i;

    for(i = 0; i < g_astImsaCnRejCauseNum; i++)
    {
        if(enCause == g_astImsaCnRejCauseMap[i].enCause)
        {
            enImsaCnCause = g_astImsaCnRejCauseMap[i].enImsaCnRejCause;
            break;
        }
    }
    if(g_astImsaCnRejCauseNum == i)
    {
        enImsaCnCause = IMSA_ERR_LOG_PDNREJ_CAUSE_BUTT;
    }

    return enImsaCnCause;
}
/*****************************************************************************
 Function Name   :  IMSA_PDN_CnRejCauseProc
 Description     :  将TAF上报的拒绝原因转换成对应的网侧拒绝原因值
 Input           :  enCause 表示TAF上报的拒绝原因值

 Output          :  输出一个网侧拒绝原因值
 Return          :

 History         :
    1.wangchen 　00209181 2015-01-08 新增文件

*****************************************************************************/
IMSA_ERR_LOG_PDNREJ_CAUSE_ENUM_UINT32  IMSA_PDN_InterRejCauseProc
(
    IMSA_CONN_RESULT_ENUM_UINT32    enResult,
    TAF_PS_CAUSE_ENUM_UINT32        enCause
)
{
    IMSA_ERR_LOG_PDNREJ_CAUSE_ENUM_UINT32   enImsaCnCause = IMSA_ERR_LOG_PDNREJ_CAUSE_BUTT;

    switch(enResult)
    {
        case IMSA_CONN_RESULT_FAIL_PARA_ERR:
            enImsaCnCause = IMSA_ERR_LOG_PDNREJ_CAUSE_IMSA_PARA_ERR;
            break;

        case IMSA_CONN_RESULT_FAIL_TIMER_EXP:
            enImsaCnCause = IMSA_ERR_LOG_PDNREJ_CAUSE_IMSA_TIMER_EXP;
            break;

        case IMSA_CONN_RESULT_FAIL_CN_REJ:
            enImsaCnCause = IMSA_PDN_CnRejCauseProc(enCause);
            break;

        case IMSA_CONN_RESULT_FAIL_CONN_RELEASING:
            enImsaCnCause = IMSA_ERR_LOG_PDNREJ_CAUSE_IMSA_CONN_RELEASING;
            break;

        case IMSA_CONN_RESULT_FAIL_PDP_ACTIVATE_LIMIT:
            enImsaCnCause = IMSA_ERR_LOG_PDNREJ_CAUSE_PDP_ACTIVATE_LIMIT;
            break;

        case IMSA_CONN_RESULT_FAIL_SAME_APN_OPERATING:
            enImsaCnCause = IMSA_ERR_LOG_PDNREJ_CAUSE_SAME_APN_OPERATING;
            break;

        case IMSA_CONN_RESULT_FAIL_OHTERS:
            enImsaCnCause = IMSA_ERR_LOG_PDNREJ_CAUSE_IMSA_OTHERS;
            break;

        default:
            break;
    }
    return enImsaCnCause;
}

/*****************************************************************************
 Function Name   :  IMSA_PDN_CnRejCauseProc
 Description     :  将TAF上报的拒绝原因转换成对应的网侧拒绝原因值
 Input           :  enCause 表示TAF上报的拒绝原因值

 Output          :  输出一个网侧拒绝原因值
 Return          :

 History         :
    1.wangchen 　00209181 2015-01-08 新增文件

*****************************************************************************/
IMSA_ERR_LOG_NORM_PDNREJ_CAUSE_ENUM_UINT32  IMSA_Norm_PDN_RejCauseProc
(
    IMSA_CONN_RESULT_ENUM_UINT32    enResult
)
{
    IMSA_ERR_LOG_NORM_PDNREJ_CAUSE_ENUM_UINT32   enImsaCnCause = IMSA_ERR_LOG_NORM_PDNREJ_CAUSE_BUTT;

    switch(enResult)
    {
        case IMSA_CONN_RESULT_FAIL_PARA_ERR:
            enImsaCnCause = IMSA_ERR_LOG_NORM_PDNREJ_CAUSE_IMSA_PARA_ERR;
            break;

        case IMSA_CONN_RESULT_FAIL_TIMER_EXP:
            enImsaCnCause = IMSA_ERR_LOG_NORM_PDNREJ_CAUSE_IMSA_TIMER_EXP;
            break;

        case IMSA_CONN_RESULT_FAIL_CN_REJ:
            enImsaCnCause = IMSA_ERR_LOG_NORM_PDNREJ_CAUSE_NW_REJ;
            break;

        case IMSA_CONN_RESULT_FAIL_CONN_RELEASING:
            enImsaCnCause = IMSA_ERR_LOG_NORM_PDNREJ_CAUSE_IMSA_CONN_RELEASING;
            break;

        case IMSA_CONN_RESULT_FAIL_PDP_ACTIVATE_LIMIT:
            enImsaCnCause = IMSA_ERR_LOG_NORM_PDNREJ_CAUSE_PDP_ACTIVATE_LIMIT;
            break;

        case IMSA_CONN_RESULT_FAIL_SAME_APN_OPERATING:
            enImsaCnCause = IMSA_ERR_LOG_NORM_PDNREJ_CAUSE_SAME_APN_OPERATING;
            break;

        case IMSA_CONN_RESULT_FAIL_OHTERS:
            enImsaCnCause = IMSA_ERR_LOG_NORM_PDNREJ_CAUSE_IMSA_OTHERS;
            break;

        default:
            break;
    }
    return enImsaCnCause;
}


VOS_VOID IMSA_CONN_PdnConnFailEventReport
(
    IMSA_ERR_LOG_NORM_PDNREJ_CAUSE_ENUM_UINT32      enPdnConnRejCause,
    IMSA_ERR_LOG_PDN_CONN_NW_REJ_CAUSE_ENUM_UINT16  enNwRejCause
)
{
    IMSA_ERR_LOG_ALM_IMS_REG_FAIL_EVENT_REPORT_STRU stImsNotInitRegEvent;
    VOS_UINT32                          ulIsLogRecord;
    VOS_UINT32                          ulLength;
    VOS_UINT16                          usLevel;
    IMSA_SEL_SDF_PARA_STRU             *pstSelSdfPara = VOS_NULL_PTR;

    pstSelSdfPara = IMSA_CONN_GetNormalConnSelSdfParaAddr();


    /* 查询对应Alarm Id是否需要记录异常信息 */
    usLevel             = IMSA_GetErrLogAlmLevel(IMSA_ERR_LOG_ALM_IMS_REG_FAIL_EVENT_REPORT);
    ulIsLogRecord       = IMSA_IsErrLogNeedRecord(usLevel);

    /* 模块异常不需要记录时，不保存异常信息 */
    if (VOS_FALSE == ulIsLogRecord)
    {
        return;
    }

    ulLength = sizeof(IMSA_ERR_LOG_ALM_IMS_REG_FAIL_EVENT_REPORT_STRU);

    IMSA_MEM_SET_S(&stImsNotInitRegEvent, ulLength, 0x00, ulLength);

    IMSA_WRITE_ERRLOG_HEADER_INFO(&stImsNotInitRegEvent.stHeader,
                                  VOS_GetModemIDFromPid(PS_PID_IMSA),
                                  IMSA_ERR_LOG_ALM_IMS_REG_FAIL_EVENT_REPORT,
                                  usLevel,
                                  VOS_GetSlice(),
                                  (ulLength - sizeof(OM_ERR_LOG_HEADER_STRU)));

    stImsNotInitRegEvent.enImsRegFailReason =  IMSA_OM_IMS_REG_FAIL_REASON_PDN_CONN_FAIL;
    stImsNotInitRegEvent.stPdnConnFailReason.enPdnConnRejCause = enPdnConnRejCause;
    stImsNotInitRegEvent.stPdnConnFailReason.enNwRejCause = enNwRejCause;

    IMSA_MEM_CPY_S( &stImsNotInitRegEvent.stPdnConnFailReason.stImsaApn,
                    sizeof(TAF_PDP_APN_STRU),
                    &pstSelSdfPara->stApnInfo,
                    sizeof(TAF_PDP_APN_STRU));

    IMSA_SndAcpuOmFaultErrLogInd(   &stImsNotInitRegEvent,
                                    ulLength,
                                    IMSA_ERR_LOG_ALM_IMS_REG_FAIL_EVENT_REPORT);

    return;
}

#endif

/*lint +e961*/

#endif



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
/*lint +esym(766,*)*/
/* end of ImsaConnManagement.c */



