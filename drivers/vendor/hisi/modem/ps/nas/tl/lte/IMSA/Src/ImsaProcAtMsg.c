

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include    "ImsaProcAtMsg.h"
#include    "ImsaEntity.h"
#include    "ImsaPublic.h"
#include    "ImsaServiceManagement.h"
#include    "ImsaConnManagement.h"
#include    "ImsaImsAdaption.h"
#include    "ImsaProcWifiMsg.h"
/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_IMSAPROCATMSG_C
#define    THIS_NAS_FILE_ID    NAS_FILE_ID_IMSAPROCATMSG_C
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
extern VOS_VOID IMSA_SndMsgAtCiregSetCnf (VOS_UINT8 ucOpId, VOS_UINT16 usClientId,VOS_UINT32 ulRslt);
extern VOS_VOID IMSA_SndMsgAtCiregQryCnf
(
    VOS_UINT8 ucOpId,
    VOS_UINT16 usClientId,
    VOS_UINT32 ulRegInfo
);
extern VOS_VOID IMSA_SndMsgAtCirepSetCnf (VOS_UINT8 ucOpId, VOS_UINT16 usClientId, VOS_UINT32 ulRslt);
extern VOS_VOID IMSA_SndMsgAtCirepQryCnf
(
    VOS_UINT8 ucOpId,
    VOS_UINT16 usClientId,
    AT_IMSA_IMSVOPS_CAPABILITY_ENUM_UINT32      enImsvops
);


/*****************************************************************************
  3 Function
*****************************************************************************/

/*lint -e961*/

VOS_VOID IMSA_ProcMsgCiregSetReq (const AT_IMSA_CIREG_SET_REQ_STRU *pstCiregSetReq)
{
    IMSA_AT_CONTROL_STRU                *pstImsaAtControl;
    VOS_UINT32                          ulRslt;

    IMSA_INFO_LOG("IMSA_ProcMsgCiregSetReq is entered! ");
    TLPS_PRINT2LAYER_INFO(IMSA_ProcMsgCiregSetReq_ENUM, LNAS_ENTRY);

    if (VOS_NULL_PTR == pstCiregSetReq)
    {
        IMSA_WARN_LOG("IMSA_ProcMsgCiregSetReq: Input is null! ");
        TLPS_PRINT2LAYER_WARNING(IMSA_ProcMsgCiregSetReq_ENUM, LNAS_NULL_PTR);
        return;
    }

    pstImsaAtControl = IMSA_GetAtControlAddress();

    if (AT_IMSA_IMS_REG_STATE_REPROT_BUTT <= pstCiregSetReq->enCireg )
    {
        ulRslt = VOS_ERR;
    }
    else
    {
        /*保存是否主动上报变量*/
        pstImsaAtControl->enCireg = pstCiregSetReq->enCireg;

        ulRslt = VOS_OK;
    }

    /* 给AT发送回复消息*/
    IMSA_SndMsgAtCiregSetCnf(pstCiregSetReq->ucOpId,pstCiregSetReq->usClientId,ulRslt);

    return;
}


VOS_VOID IMSA_SndMsgAtCiregSetCnf (VOS_UINT8 ucOpId, VOS_UINT16 usClientId,VOS_UINT32 ulRslt)
{

    IMSA_AT_CIREG_SET_CNF_STRU          *pstCiregSetCnf;

    /*分配消息空间*/
    pstCiregSetCnf = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_AT_CIREG_SET_CNF_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstCiregSetCnf)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndMsgAtCiregSetCnf:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SndMsgAtCiregSetCnf_ENUM, LNAS_NULL_PTR);
        return ;
    }

    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstCiregSetCnf),
                    IMSA_GET_MSG_LENGTH(pstCiregSetCnf),
                    0,
                    IMSA_GET_MSG_LENGTH(pstCiregSetCnf));

    IMSA_WRITE_AT_MSG_MSG_HEAD(pstCiregSetCnf,ID_IMSA_AT_CIREG_SET_CNF);

    /*填写消息内容*/
    pstCiregSetCnf->ucOpId = ucOpId;
    pstCiregSetCnf->usClientId = usClientId;
    pstCiregSetCnf->ulResult = ulRslt;

    /*调用消息发送函数 */
    IMSA_SND_MSG(pstCiregSetCnf);

    return;
}


VOS_VOID IMSA_SndMsgAtCireguInd (VOS_UINT32 ulRegInfo)
{
    IMSA_AT_CIREGU_IND_STRU             *pstCireguInd;
    IMSA_AT_CONTROL_STRU                *pstImsaAtControl;

    pstImsaAtControl = IMSA_GetAtControlAddress();

    if (AT_IMSA_IMS_REG_STATE_DISABLE_REPORT == pstImsaAtControl->enCireg)
    {
        IMSA_INFO_LOG("IMSA_SndMsgAtCireguInd: NOT SEND CIREGU IND MSG!");
        TLPS_PRINT2LAYER_INFO(IMSA_SndMsgAtCireguInd_ENUM, 1);
        return;
    }

    /*分配消息空间*/
    pstCireguInd = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_AT_CIREGU_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstCireguInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndMsgAtCireguInd:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_INFO(IMSA_SndMsgAtCireguInd_ENUM, LNAS_NULL_PTR);
        return ;
    }

    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstCireguInd),
                    IMSA_GET_MSG_LENGTH(pstCireguInd),
                    0,
                    IMSA_GET_MSG_LENGTH(pstCireguInd));

    IMSA_WRITE_AT_MSG_MSG_HEAD(pstCireguInd,ID_IMSA_AT_CIREGU_IND);

    /*填写消息内容*/
    pstCireguInd->ucOpId = 0;
    pstCireguInd->usClientId = 0x3fff;

    pstCireguInd->ulRegInfo = ulRegInfo;

    if (AT_IMSA_IMS_REG_STATE_ENABLE_EXTENDED_REPROT == pstImsaAtControl->enCireg)
    {
        pstCireguInd->bitOpExtInfo = VOS_TRUE;
        pstCireguInd->ulExtInfo = 0;
    }

    /*调用消息发送函数 */
    IMSA_SND_MSG(pstCireguInd);
    return;
}



VOS_VOID IMSA_SndMsgAtImsRatHandoverInd
(
    IMSA_AT_IMS_RAT_HO_STATUS_ENUM_UINT8    enHoStatus,
    IMSA_AT_IMS_RAT_TYPE_ENUM_UINT8         enSrcRat,
    IMSA_AT_IMS_RAT_TYPE_ENUM_UINT8         enDstRat,
    IMSA_AT_HO_CAUSE_ENUM_UINT32            enCause
)
{
    IMSA_AT_IMS_RAT_HANDOVER_IND_STRU     *pstImsRatHandoverInd = VOS_NULL_PTR;


    IMSA_INFO_LOG("IMSA_SndMsgAtImsRatHandoverInd is entered!");

    /*分配空间并检验分配是否成功*/
    pstImsRatHandoverInd = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_AT_IMS_RAT_HANDOVER_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstImsRatHandoverInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndMsgAtImsRatHandoverInd:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstImsRatHandoverInd),
                    IMSA_GET_MSG_LENGTH(pstImsRatHandoverInd),
                    0,
                    IMSA_GET_MSG_LENGTH(pstImsRatHandoverInd));
    IMSA_WRITE_AT_MSG_MSG_HEAD(pstImsRatHandoverInd,ID_IMSA_AT_IMS_RAT_HANDOVER_IND);

    /*填写消息内容*/
    pstImsRatHandoverInd->ucOpId = 0;
    pstImsRatHandoverInd->usClientId = 0x3fff;

    /* 填写消息内容 */
    pstImsRatHandoverInd->enHoStatus = enHoStatus;
    pstImsRatHandoverInd->enSrcRat   = enSrcRat;
    pstImsRatHandoverInd->enDstRat   = enDstRat;
    pstImsRatHandoverInd->enCause    = enCause;


    /*调用消息发送函数 */
    IMSA_SND_MSG(pstImsRatHandoverInd);
}




VOS_VOID IMSA_SndMsgAtSrvStatusUpdateInd
(
      IMSA_AT_IMS_SERVICE_STATUS_ENUM_UINT8 enSmsSrvStatus,
      IMSA_AT_IMS_SERVICE_STATUS_ENUM_UINT8   enVoIpSrvStatus,
      IMSA_AT_IMS_SERVICE_STATUS_ENUM_UINT8   enVtSrvStatus,
      IMSA_AT_IMS_SERVICE_STATUS_ENUM_UINT8   enVsSrvStatus,
      IMSA_AT_IMS_RAT_TYPE_ENUM_UINT8         enSrvRat
)
{
    IMSA_AT_IMS_SRV_STATUS_UPDATE_IND_STRU     *pstSrvStatusNotify = VOS_NULL_PTR;


    IMSA_INFO_LOG("IMSA_SRV_AtSrvStatusUpdate is entered!");

    /*分配空间并检验分配是否成功*/
    pstSrvStatusNotify = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_AT_IMS_SRV_STATUS_UPDATE_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstSrvStatusNotify)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SRV_AtSrvStatusUpdate:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstSrvStatusNotify),
                    IMSA_GET_MSG_LENGTH(pstSrvStatusNotify),
                    0,
                    IMSA_GET_MSG_LENGTH(pstSrvStatusNotify));
    IMSA_WRITE_AT_MSG_MSG_HEAD(pstSrvStatusNotify,ID_IMSA_AT_IMS_SRV_STATUS_UPDATE_IND);

    /*填写消息内容*/
    pstSrvStatusNotify->ucOpId = 0;
    pstSrvStatusNotify->usClientId = 0x3fff;

    /* 填写消息内容 */
    pstSrvStatusNotify->enSmsSrvStatus   = enSmsSrvStatus;
    pstSrvStatusNotify->enVoIpSrvStatus   = enVoIpSrvStatus;
    pstSrvStatusNotify->enVtSrvStatus   = enVtSrvStatus;
    pstSrvStatusNotify->enVsSrvStatus   = enVsSrvStatus;


    if( (IMSA_AT_IMS_RAT_TYPE_LTE == enSrvRat)
        || (IMSA_AT_IMS_RAT_TYPE_WIFI == enSrvRat))
    {
        pstSrvStatusNotify->enSmsSrvRat    = enSrvRat;
        pstSrvStatusNotify->enVoIpSrvRat   = enSrvRat;
        pstSrvStatusNotify->enVtSrvRat     = enSrvRat;
        pstSrvStatusNotify->enVsSrvRat     = enSrvRat;
    }
    else
    {
        pstSrvStatusNotify->enSmsSrvRat    = IMSA_AT_IMS_RAT_TYPE_LTE;
        pstSrvStatusNotify->enVoIpSrvRat   = IMSA_AT_IMS_RAT_TYPE_LTE;
        pstSrvStatusNotify->enVtSrvRat     = IMSA_AT_IMS_RAT_TYPE_LTE;
        pstSrvStatusNotify->enVsSrvRat     = IMSA_AT_IMS_RAT_TYPE_LTE;
    }

    /*调用消息发送函数 */
    IMSA_SND_MSG(pstSrvStatusNotify);
}






VOS_VOID IMSA_ProcMsgCiregQryReq (const AT_IMSA_CIREG_QRY_REQ_STRU *pstCiregQryReq)
{
    VOS_UINT32                          ulRegInfo;

    IMSA_INFO_LOG("IMSA_ProcMsgCiregQryReq is entered! ");
    TLPS_PRINT2LAYER_INFO(IMSA_ProcMsgCiregQryReq_ENUM, LNAS_ENTRY);

    if (VOS_NULL_PTR == pstCiregQryReq)
    {
        IMSA_WARN_LOG("IMSA_ProcMsgCiregQryReq: Input is null! ");
        TLPS_PRINT2LAYER_WARNING(IMSA_ProcMsgCiregQryReq_ENUM, LNAS_NULL_PTR);
        return;
    }

    ulRegInfo = IMSA_SRV_GetAtRegStatusFromSrvStatus(IMSA_SRV_GetNormalSrvStatus());


    /* 给AT发送回复消息*/
    IMSA_SndMsgAtCiregQryCnf(pstCiregQryReq->ucOpId,pstCiregQryReq->usClientId,ulRegInfo);

    return;
}


VOS_VOID IMSA_SndMsgAtCiregQryCnf
(
    VOS_UINT8 ucOpId,
    VOS_UINT16 usClientId,
    VOS_UINT32 ulRegInfo
)
{
    IMSA_AT_CIREG_QRY_CNF_STRU          *pstCiregQryCnf;
    IMSA_AT_CONTROL_STRU                *pstImsaAtControl;

    pstImsaAtControl = IMSA_GetAtControlAddress();

    /*分配消息空间*/
    pstCiregQryCnf = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_AT_CIREG_QRY_CNF_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstCiregQryCnf)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndMsgAtCiregQryCnf:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SndMsgAtCiregQryCnf_ENUM, LNAS_NULL_PTR);
        return ;
    }

    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstCiregQryCnf),
                    IMSA_GET_MSG_LENGTH(pstCiregQryCnf),
                    0,
                    IMSA_GET_MSG_LENGTH(pstCiregQryCnf));

    IMSA_WRITE_AT_MSG_MSG_HEAD(pstCiregQryCnf,ID_IMSA_AT_CIREG_QRY_CNF);

    /*填写消息内容*/
    pstCiregQryCnf->ucOpId = ucOpId;
    pstCiregQryCnf->usClientId = usClientId;

    pstCiregQryCnf->ulRegInfo = ulRegInfo;

    if (AT_IMSA_IMS_REG_STATE_ENABLE_EXTENDED_REPROT == pstImsaAtControl->enCireg)
    {
        pstCiregQryCnf->bitOpExtInfo = VOS_TRUE;
        pstCiregQryCnf->ulExtInfo = 0;
    }
    pstCiregQryCnf->enCireg = pstImsaAtControl->enCireg;
    pstCiregQryCnf->ulResult = VOS_OK;

    /*调用消息发送函数 */
    IMSA_SND_MSG(pstCiregQryCnf);
    return;
}



VOS_VOID IMSA_ProcMsgCirepSetReq (const AT_IMSA_CIREP_SET_REQ_STRU *pstCirepSetReq)
{
    IMSA_AT_CONTROL_STRU                *pstImsaAtControl;
    VOS_UINT32                          ulRslt;

    IMSA_WARN_LOG("IMSA_ProcMsgCirepSetReq is entered! ");
    TLPS_PRINT2LAYER_INFO(IMSA_ProcMsgCirepSetReq_ENUM, LNAS_ENTRY);

    if (VOS_NULL_PTR == pstCirepSetReq)
    {
        IMSA_WARN_LOG("IMSA_ProcMsgCirepSetReq: Input is null! ");
        TLPS_PRINT2LAYER_WARNING(IMSA_ProcMsgCirepSetReq_ENUM, LNAS_NULL_PTR);
        return;
    }

    pstImsaAtControl = IMSA_GetAtControlAddress();

    if (AT_IMSA_IMSVOPS_CAPABILITY_BUTT <= pstCirepSetReq->enReport)
    {
        ulRslt = VOS_ERR;
    }
    else
    {
        /*保存是否主动上报变量*/
        pstImsaAtControl->enReport = pstCirepSetReq->enReport;
        ulRslt = VOS_OK;
    }

    /* 给AT发送回复消息*/
    IMSA_SndMsgAtCirepSetCnf(pstCirepSetReq->ucOpId,pstCirepSetReq->usClientId,ulRslt);

    return;
}


VOS_VOID IMSA_SndMsgAtCirepSetCnf (VOS_UINT8 ucOpId, VOS_UINT16 usClientId, VOS_UINT32 ulRslt)
{
    IMSA_AT_CIREP_SET_CNF_STRU          *pstCirepSetCnf;

    /*分配消息空间*/
    pstCirepSetCnf = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_AT_CIREP_SET_CNF_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstCirepSetCnf)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndMsgAtCirepSetCnf:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SndMsgAtCirepSetCnf_ENUM, LNAS_NULL_PTR);
        return ;
    }

    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstCirepSetCnf),
                    IMSA_GET_MSG_LENGTH(pstCirepSetCnf),
                    0,
                    IMSA_GET_MSG_LENGTH(pstCirepSetCnf));

    IMSA_WRITE_AT_MSG_MSG_HEAD(pstCirepSetCnf,ID_IMSA_AT_CIREP_SET_CNF);

    /*填写消息内容*/
    pstCirepSetCnf->ucOpId = ucOpId;
    pstCirepSetCnf->usClientId = usClientId;
    pstCirepSetCnf->ulResult = ulRslt;

    /*调用消息发送函数 */
    IMSA_SND_MSG(pstCirepSetCnf);
    return;
}


VOS_VOID IMSA_SndMsgAtCirephInd (AT_IMSA_SRVCC_HANDVOER_ENUM_UINT32   enHandover)
{
    IMSA_AT_CIREPH_IND_STRU             *pstCirephInd;
    IMSA_AT_CONTROL_STRU                *pstImsaAtControl;

    pstImsaAtControl = IMSA_GetAtControlAddress();

    if (AT_IMSA_CIREP_REPORT_DISENABLE == pstImsaAtControl->enReport)
    {
        IMSA_INFO_LOG("IMSA_SndMsgAtCirephInd: NOT SEND CIREPH IND MSG!");
        TLPS_PRINT2LAYER_INFO(IMSA_SndMsgAtCirephInd_ENUM, 1);
        return;
    }

    /*分配消息空间*/
    pstCirephInd = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_AT_CIREPH_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstCirephInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndMsgAtCirephInd:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SndMsgAtCirephInd_ENUM, LNAS_NULL_PTR);
        return ;
    }

    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstCirephInd),
                    IMSA_GET_MSG_LENGTH(pstCirephInd),
                    0,
                    IMSA_GET_MSG_LENGTH(pstCirephInd));

    IMSA_WRITE_AT_MSG_MSG_HEAD(pstCirephInd,ID_IMSA_AT_CIREPH_IND);

    /*填写消息内容*/
    pstCirephInd->ucOpId = 0;
    pstCirephInd->usClientId = 0x3fff;


    pstCirephInd->enHandover = enHandover;


    /*调用消息发送函数 */
    IMSA_SND_MSG(pstCirephInd);
    return;
}


VOS_VOID IMSA_SndMsgAtCirepiInd (AT_IMSA_IMSVOPS_CAPABILITY_ENUM_UINT32      enImsvops)
{
    IMSA_AT_CIREPI_IND_STRU             *pstCirepiInd;
    IMSA_AT_CONTROL_STRU                *pstImsaAtControl;

    pstImsaAtControl = IMSA_GetAtControlAddress();

    if (AT_IMSA_CIREP_REPORT_DISENABLE == pstImsaAtControl->enReport)
    {
        IMSA_INFO_LOG("IMSA_SndMsgAtCirepiInd: NOT SEND CIREPH IND MSG!");
        TLPS_PRINT2LAYER_INFO(IMSA_SndMsgAtCirepiInd_ENUM, 1);
        return;
    }

    /*分配消息空间*/
    pstCirepiInd = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_AT_CIREPI_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstCirepiInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndMsgAtCirephInd:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SndMsgAtCirepiInd_ENUM, LNAS_NULL_PTR);
        return ;
    }

    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstCirepiInd),
                    IMSA_GET_MSG_LENGTH(pstCirepiInd),
                    0,
                    IMSA_GET_MSG_LENGTH(pstCirepiInd));

    IMSA_WRITE_AT_MSG_MSG_HEAD(pstCirepiInd,ID_IMSA_AT_CIREPI_IND);


    /*填写消息内容*/
    pstCirepiInd->ucOpId = 0;
    pstCirepiInd->usClientId = 0x3fff;


    pstCirepiInd->enImsvops = enImsvops;


    /*调用消息发送函数 */
    IMSA_SND_MSG(pstCirepiInd);
    return;
}



VOS_VOID IMSA_ProcMsgCirepQryReq (const AT_IMSA_CIREP_QRY_REQ_STRU *pstCiregQryReq)
{
    AT_IMSA_IMSVOPS_CAPABILITY_ENUM_UINT32      enImsvops;

    IMSA_INFO_LOG("IMSA_ProcMsgCirepQryReq is entered! ");
    TLPS_PRINT2LAYER_INFO(IMSA_ProcMsgCirepQryReq_ENUM, LNAS_ENTRY);

    if (VOS_NULL_PTR == pstCiregQryReq)
    {
        IMSA_WARN_LOG("IMSA_ProcMsgCirepQryReq: Input is null! ");
        TLPS_PRINT2LAYER_WARNING(IMSA_ProcMsgCirepQryReq_ENUM, LNAS_NULL_PTR);
        return;
    }

    enImsvops = IMSA_GetNwImsVoiceCap();

    if (AT_IMSA_IMSVOPS_CAPABILITY_BUTT == enImsvops)
    {
        enImsvops = AT_IMSA_NW_NOT_SUPORT_IMSVOPS;
    }

    /* 给AT发送回复消息*/
    IMSA_SndMsgAtCirepQryCnf(pstCiregQryReq->ucOpId,pstCiregQryReq->usClientId,enImsvops);

    return;
}


VOS_VOID IMSA_ProcMsgVolteImpuQryReq(const AT_IMSA_VOLTEIMPU_QRY_REQ_STRU *pstVolteImpuQryReq)
{
    IMSA_AT_CONTROL_STRU                *pstImsaAtControl;

    pstImsaAtControl = IMSA_GetAtControlAddress();

    IMSA_INFO_LOG("IMSA_ProcMsgVolteImpuQryReq is entered! ");
    TLPS_PRINT2LAYER_INFO(IMSA_ProcMsgVolteImpuQryReq_ENUM, LNAS_ENTRY);

    /* 需要保存CLENT ID和OPID，用于回复时使用 */
    pstImsaAtControl->usClientId = pstVolteImpuQryReq->usClientId;
    pstImsaAtControl->ucOpId = pstVolteImpuQryReq->ucOpId;

    /* 给IMSA发送查询命令 */
    IMSA_SendImsVolteImpuQryReq();
}

#if 0

VOS_VOID IMSA_ProcMsgRoamingImsSetReq(const AT_IMSA_ROAMING_IMS_SET_REQ_STRU *pstRoamingImsSetReq)
{
    IMSA_NV_IMS_RAT_SUPPORT_STRU        stNvImsRatSupport;
    VOS_UINT32                          ulRslt;

    IMSA_INFO_LOG("IMSA_ProcMsgRoamingImsSetReq is entered! ");
    TLPS_PRINT2LAYER_INFO(IMSA_ProcMsgRoamingImsSetReq_ENUM, LNAS_ENTRY);

    /* 如果当前有IMS业务存在，则直接返回失败 */
    if (VOS_TRUE == IMSA_IsImsExist())
    {
        /* 返回AT命令失败 */
        IMSA_SndMsgAtRoamingImsSetCnf(pstRoamingImsSetReq->ucOpId, pstRoamingImsSetReq->usClientId, VOS_ERR);
        return;
    }

    ulRslt = IMSA_NV_Read(EN_NV_ID_IMS_RAT_SUPPORT,&stNvImsRatSupport,\
        sizeof(IMSA_NV_IMS_RAT_SUPPORT_STRU));

    /* 读NV失败，返回失败 */
    if (VOS_OK != ulRslt)
    {
        IMSA_INFO_LOG("IMSA_ProcMsgRoamingImsSetReq: Read NV Err! ");
        TLPS_PRINT2LAYER_INFO1(IMSA_ProcMsgRoamingImsSetReq_ENUM, IMSA_CALL_API_ERROR, 1);

        /* 返回AT命令失败 */
        IMSA_SndMsgAtRoamingImsSetCnf(pstRoamingImsSetReq->ucOpId, pstRoamingImsSetReq->usClientId, VOS_ERR);
        return;
    }

    /* 根据配置的不同写NV结构体 */
    switch (pstRoamingImsSetReq->enRoamingImsSupportFlag)
    {
        case AT_IMSA_ROAMING_IMS_SUPPORT:
            stNvImsRatSupport.ucRoamingImsNotSupportFlag = VOS_FALSE;
            break;

        case AT_IMSA_ROAMING_IMS_NOT_SUPPORT:
            stNvImsRatSupport.ucRoamingImsNotSupportFlag = VOS_TRUE;
            break;

        /* 参数错误分支，直接返回失败 */
        default:
            /* 返回AT命令失败 */
            IMSA_SndMsgAtRoamingImsSetCnf(pstRoamingImsSetReq->ucOpId, pstRoamingImsSetReq->usClientId, VOS_ERR);
            return;
    }

    ulRslt = IMSA_NV_Write(EN_NV_ID_IMS_RAT_SUPPORT,&stNvImsRatSupport,\
            sizeof(IMSA_NV_IMS_RAT_SUPPORT_STRU));

    /* 写NV失败，返回失败 */
    if (VOS_OK != ulRslt)
    {
        IMSA_INFO_LOG("IMSA_ProcMsgRoamingImsSetReq: Write NV Err! ");
        TLPS_PRINT2LAYER_INFO1(IMSA_ProcMsgRoamingImsSetReq_ENUM, IMSA_CALL_API_ERROR, 2);

        /* 返回AT命令失败 */
        IMSA_SndMsgAtRoamingImsSetCnf(pstRoamingImsSetReq->ucOpId, pstRoamingImsSetReq->usClientId, VOS_ERR);
        return;
    }

    /* 读写NV全部成功，则修改本地维护的全局变量 */
    IMSA_GetConfigParaAddress()->ucRoamingImsNotSupportFlag = stNvImsRatSupport.ucRoamingImsNotSupportFlag;

    /* 返回AT命令处理结果 */
    IMSA_SndMsgAtRoamingImsSetCnf(pstRoamingImsSetReq->ucOpId, pstRoamingImsSetReq->usClientId, VOS_OK);

    /* 如果配置漫游不支持，则发起去注册，反之则发起注册 */
    if (VOS_TRUE == IMSA_GetConfigParaAddress()->ucRoamingImsNotSupportFlag)
    {
        /* 调用漫游去注册流程接口 */
        IMSA_SRV_ProcWhenRoamNotSatisfied();
    }
    else
    {
        /* 调用注册流程接口 */
        IMSA_SRV_ProcWhenRoamIsSatisfied();
    }
    return;
}
#endif


VOS_VOID IMSA_ProcMsgRoamingImsQryReq(const AT_IMSA_ROAMING_IMS_QRY_REQ_STRU *pstRoamingImsSetReq)
{
    IMSA_INFO_LOG("IMSA_ProcMsgRoamingImsQryReq is entered! ");
    TLPS_PRINT2LAYER_INFO(IMSA_ProcMsgRoamingImsQryReq_ENUM, LNAS_ENTRY);

    if (VOS_TRUE == IMSA_GetConfigParaAddress()->ucRoamingImsNotSupportFlag)
    {
        IMSA_SndMsgAtRoamingImsQryCnf(pstRoamingImsSetReq->ucOpId,
                                      pstRoamingImsSetReq->usClientId,
                                      AT_IMSA_ROAMING_IMS_NOT_SUPPORT);
    }

    else
    {
        IMSA_SndMsgAtRoamingImsQryCnf(pstRoamingImsSetReq->ucOpId,
                                      pstRoamingImsSetReq->usClientId,
                                      AT_IMSA_ROAMING_IMS_SUPPORT);
    }

    return;
}




VOS_VOID IMSA_ProcMsgEncryptSetReq(const AT_IMSA_CALL_ENCRYPT_SET_REQ_STRU *pstEncryptSetReq)
{
    IMSA_AT_CONTROL_STRU                *pstImsaAtControl;

    pstImsaAtControl = IMSA_GetAtControlAddress();


    IMSA_INFO_LOG("IMSA_ProcMsgCcwaiSetReq is entered! ");
    TLPS_PRINT2LAYER_INFO(IMSA_ProcMsgCcwaiSetReq_ENUM, LNAS_ENTRY);

    /* 需要保存CLENT ID和OPID，用于回复时使用 */
    pstImsaAtControl->usClientId = pstEncryptSetReq->usClientId;
    pstImsaAtControl->ucOpId = pstEncryptSetReq->ucOpId;

    /* 给IMSA发送查询命令 */
    IMSA_SendImsEncryptSetReq(pstEncryptSetReq->ucEncrypt);

}


VOS_VOID IMSA_SndMsgAtEncryptSetCnf
(
    VOS_UINT32 ulResult
)
{
    IMSA_AT_CALL_ENCRYPT_SET_CNF_STRU  *pstAtEncryptSetCnf;

    IMSA_AT_CONTROL_STRU               *pstImsaAtControl;

    pstImsaAtControl = IMSA_GetAtControlAddress();

    /*分配消息空间*/
    pstAtEncryptSetCnf = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_AT_CALL_ENCRYPT_SET_CNF_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstAtEncryptSetCnf)
    {
        return ;
    }

    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstAtEncryptSetCnf),
                    IMSA_GET_MSG_LENGTH(pstAtEncryptSetCnf),
                    0,
                    IMSA_GET_MSG_LENGTH(pstAtEncryptSetCnf));
    IMSA_WRITE_AT_MSG_MSG_HEAD(pstAtEncryptSetCnf,ID_IMSA_AT_CALL_ENCRYPT_SET_CNF);

    /*填写消息内容*/
    pstAtEncryptSetCnf->ucOpId = pstImsaAtControl->ucOpId;
    pstAtEncryptSetCnf->usClientId = pstImsaAtControl->usClientId;

    pstAtEncryptSetCnf->ulResult = ulResult;

    /*调用消息发送函数 */
    IMSA_SND_MSG(pstAtEncryptSetCnf);

    return;
}


/*lint -e818*/
/*lint -e830*/
VOS_VOID IMSA_SndMsgAtQryVolteImpuCnf
(
    VOS_UINT32 ulResult,
    VOS_UINT32 ulImpuLen,
    VOS_CHAR *pucImpu
)
{
    IMSA_AT_VOLTEIMPU_QRY_CNF_STRU          *pstVolteImpuQryCnf;

    IMSA_AT_CONTROL_STRU                    *pstImsaAtControl;

    pstImsaAtControl = IMSA_GetAtControlAddress();

    /*分配消息空间*/
    pstVolteImpuQryCnf = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_AT_VOLTEIMPU_QRY_CNF_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstVolteImpuQryCnf)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndMsgAtQryVolteImpuCnf:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SndMsgAtQryVolteImpuCnf_ENUM, LNAS_NULL_PTR);
        return ;
    }

    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstVolteImpuQryCnf),
                    IMSA_GET_MSG_LENGTH(pstVolteImpuQryCnf),
                    0,
                    IMSA_GET_MSG_LENGTH(pstVolteImpuQryCnf));

    IMSA_WRITE_AT_MSG_MSG_HEAD(pstVolteImpuQryCnf,ID_IMSA_AT_VOLTEIMPU_QRY_CNF);

    /*填写消息内容*/
    pstVolteImpuQryCnf->ucOpId = pstImsaAtControl->ucOpId;
    pstVolteImpuQryCnf->usClientId = pstImsaAtControl->usClientId;

    pstVolteImpuQryCnf->ulResult = ulResult;

    pstVolteImpuQryCnf->ulImpuLen = ulImpuLen;

    if (VOS_NULL_PTR != pucImpu)
    {
        IMSA_UtilStrNCpy(pstVolteImpuQryCnf->aucImpu, pucImpu, AT_IMSA_IMPU_MAX_LENGTH);
    }

    /*调用消息发送函数 */
    IMSA_SND_MSG(pstVolteImpuQryCnf);

    return;
}
/*lint +e830*/
/*lint +e818*/



VOS_VOID IMSA_SndMsgAtCirepQryCnf
(
    VOS_UINT8 ucOpId,
    VOS_UINT16 usClientId,
    AT_IMSA_IMSVOPS_CAPABILITY_ENUM_UINT32      enImsvops
)
{
    IMSA_AT_CIREP_QRY_CNF_STRU          *pstCirepQryCnf;
    IMSA_AT_CONTROL_STRU                *pstImsaAtControl;

    pstImsaAtControl = IMSA_GetAtControlAddress();

    /*分配消息空间*/
    pstCirepQryCnf = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_AT_CIREP_QRY_CNF_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstCirepQryCnf)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndMsgAtCirepQryCnf:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SndMsgAtCirepQryCnf_ENUM, LNAS_NULL_PTR);
        return ;
    }

    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstCirepQryCnf),
                    IMSA_GET_MSG_LENGTH(pstCirepQryCnf),
                    0,
                    IMSA_GET_MSG_LENGTH(pstCirepQryCnf));

    IMSA_WRITE_AT_MSG_MSG_HEAD(pstCirepQryCnf,ID_IMSA_AT_CIREP_QRY_CNF);


    /*填写消息内容*/
    pstCirepQryCnf->ucOpId = ucOpId;
    pstCirepQryCnf->usClientId = usClientId;
    pstCirepQryCnf->enReport = pstImsaAtControl->enReport;
    pstCirepQryCnf->enImsvops = enImsvops;
    pstCirepQryCnf->ulResult = VOS_OK;

    /*调用消息发送函数 */
    IMSA_SND_MSG(pstCirepQryCnf);
    return;
}
/*lint +e961*/

VOS_VOID IMSA_SndMsgAtPdpActInd (const IMSA_PDP_CNTXT_INFO_STRU     *pstPdpInfo)
{
    IMSA_AT_VT_PDP_ACTIVATE_IND_STRU    *pstPdpActInd;
    VOS_UINT8                           aucTemp[IMSA_IPV6_PREFIX_LEN]   = {0};


    /*分配消息空间*/
    pstPdpActInd = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_AT_VT_PDP_ACTIVATE_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstPdpActInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndMsgAtPdpActInd:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SndMsgAtPdpActInd_ENUM, LNAS_NULL_PTR);
        return ;
    }

    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstPdpActInd),
                    IMSA_GET_MSG_LENGTH(pstPdpActInd),
                    0,
                    IMSA_GET_MSG_LENGTH(pstPdpActInd));

    IMSA_WRITE_AT_MSG_MSG_HEAD(pstPdpActInd,ID_IMSA_AT_VT_PDP_ACTIVATE_IND);


    /*填写消息内容*/
    pstPdpActInd->ucOpId = 0;
    pstPdpActInd->usClientId = 0x3fff;

    pstPdpActInd->stPdpAddr.enPdpType = pstPdpInfo->stPdpAddr.enIpType;

    if ((IMSA_IP_TYPE_IPV4 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        /* 配置IPV4地址 */
        IMSA_MEM_CPY_S( pstPdpActInd->stPdpAddr.aucIpv4Addr,
                        IMSA_IPV4_ADDR_LEN,
                        pstPdpInfo->stPdpAddr.aucIpV4Addr,
                        IMSA_IPV4_ADDR_LEN);

        /* 配置IPV4 DNS地址 */
        pstPdpActInd->stIpv4Dns.bitOpPrimDnsAddr = pstPdpInfo->stPdpIpv4Dns.bitOpPriDns;
        IMSA_MEM_CPY_S( pstPdpActInd->stIpv4Dns.aucPrimDnsAddr,
                        TAF_IPV4_ADDR_LEN,
                        pstPdpInfo->stPdpIpv4Dns.aucPriDns,
                        IMSA_IPV4_ADDR_LEN);

        pstPdpActInd->stIpv4Dns.bitOpSecDnsAddr = pstPdpInfo->stPdpIpv4Dns.bitOpSecDns;
        IMSA_MEM_CPY_S( pstPdpActInd->stIpv4Dns.aucSecDnsAddr,
                        TAF_IPV4_ADDR_LEN,
                        pstPdpInfo->stPdpIpv4Dns.aucSecDns,
                        IMSA_IPV4_ADDR_LEN);
    }

    if ((IMSA_IP_TYPE_IPV6 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        if (0 != IMSA_MEM_CMP(aucTemp, pstPdpInfo->stPdpAddr.aucIpV6Addr, IMSA_IPV6_PREFIX_LEN))
        {
            /* 配置IPV6地址 */
            IMSA_MEM_CPY_S( pstPdpActInd->stPdpAddr.aucIpv6Addr,
                            TAF_IPV6_ADDR_LEN,
                            pstPdpInfo->stPdpAddr.aucIpV6Addr,
                            IMSA_IPV6_ADDR_LEN);

            /* 配置IPV6 DNS地址 */
            pstPdpActInd->stIpv6Dns.bitOpPrimDnsAddr = pstPdpInfo->stPdpIpv6Dns.bitOpPriDns;
            IMSA_MEM_CPY_S( pstPdpActInd->stIpv6Dns.aucPrimDnsAddr,
                            TAF_IPV6_ADDR_LEN,
                            pstPdpInfo->stPdpIpv6Dns.aucPriDns,
                            IMSA_IPV6_ADDR_LEN);

            pstPdpActInd->stIpv6Dns.bitOpSecDnsAddr = pstPdpInfo->stPdpIpv6Dns.bitOpSecDns;
            IMSA_MEM_CPY_S( pstPdpActInd->stIpv6Dns.aucSecDnsAddr,
                            TAF_IPV6_ADDR_LEN,
                            pstPdpInfo->stPdpIpv6Dns.aucSecDns,
                            IMSA_IPV6_ADDR_LEN);
        }
    }


    /*调用消息发送函数 */
    IMSA_SND_MSG(pstPdpActInd);
    return;
}


VOS_VOID IMSA_SndMsgAtPdpDeactInd
(
    TAF_PDP_TYPE_ENUM_UINT8             enPdpType,
    IMSA_AT_IMS_RAT_TYPE_ENUM_UINT8     enRatType
)
{
    IMSA_AT_VT_PDP_DEACTIVATE_IND_STRU    *pstPdpDeactInd;
    /*VOS_UINT8                           aucTemp[IMSA_IPV6_PREFIX_LEN]   = {0};*/


    /*分配消息空间*/
    pstPdpDeactInd = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_AT_VT_PDP_DEACTIVATE_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstPdpDeactInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndMsgAtPdpDeactInd:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SndMsgAtPdpDeactInd_ENUM, LNAS_NULL_PTR);
        return ;
    }

    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstPdpDeactInd),
                    IMSA_GET_MSG_LENGTH(pstPdpDeactInd),
                    0,
                    IMSA_GET_MSG_LENGTH(pstPdpDeactInd));

    IMSA_WRITE_AT_MSG_MSG_HEAD(pstPdpDeactInd,ID_IMSA_AT_VT_PDP_DEACTIVATE_IND);


    /*填写消息内容*/
    pstPdpDeactInd->ucOpId = 0;
    pstPdpDeactInd->usClientId = 0x3fff;

    pstPdpDeactInd->enPdpType = enPdpType;

    pstPdpDeactInd->enRatType   = enRatType;

    /*调用消息发送函数 */
    IMSA_SND_MSG(pstPdpDeactInd);
    return;
}



VOS_VOID IMSA_SndMsgAtWifiPdpActInd (const IMSA_WIFI_PDP_CNTXT_INFO_STRU     *pstPdpInfo)
{
    IMSA_AT_VT_PDP_ACTIVATE_IND_STRU    *pstPdpActInd;
    VOS_UINT8                           aucTemp[IMSA_IPV6_PREFIX_LEN]   = {0};


    /*分配消息空间*/
    pstPdpActInd = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_AT_VT_PDP_ACTIVATE_IND_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstPdpActInd)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndMsgAtPdpActInd:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SndMsgAtPdpActInd_ENUM, LNAS_NULL_PTR);
        return ;
    }

    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstPdpActInd), IMSA_GET_MSG_LENGTH(pstPdpActInd),0, IMSA_GET_MSG_LENGTH(pstPdpActInd));

    IMSA_WRITE_AT_MSG_MSG_HEAD(pstPdpActInd,ID_IMSA_AT_VT_PDP_ACTIVATE_IND);


    /*填写消息内容*/
    pstPdpActInd->ucOpId = 0;
    pstPdpActInd->usClientId = 0x3fff;

    pstPdpActInd->stPdpAddr.enPdpType = (TAF_PDP_TYPE_ENUM_UINT8)pstPdpInfo->stPdpAddr.enIpType;
    pstPdpActInd->enRatType           = IMSA_AT_IMS_RAT_TYPE_WIFI;

    if ((IMSA_IP_TYPE_IPV4 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        /* 配置IPV4地址 */
        IMSA_MEM_CPY_S(   pstPdpActInd->stPdpAddr.aucIpv4Addr,
                        IMSA_IPV4_ADDR_LEN,
                        pstPdpInfo->stPdpAddr.aucIpV4Addr,
                        IMSA_IPV4_ADDR_LEN);

        /* 配置IPV4 DNS地址 */
        if (IMSA_TRUE == pstPdpInfo->stPdpIpv4Dns.cOpPriDns)
        {
            pstPdpActInd->stIpv4Dns.bitOpPrimDnsAddr = IMSA_OP_TRUE;
            IMSA_MEM_CPY_S( pstPdpActInd->stIpv4Dns.aucPrimDnsAddr,
                            IMSA_IPV4_ADDR_LEN,
                            pstPdpInfo->stPdpIpv4Dns.aucPriDns,
                            IMSA_IPV4_ADDR_LEN);
        }


        if (IMSA_TRUE == pstPdpInfo->stPdpIpv4Dns.cOpSecDns)
        {
            pstPdpActInd->stIpv4Dns.bitOpSecDnsAddr = IMSA_OP_TRUE;
            IMSA_MEM_CPY_S( pstPdpActInd->stIpv4Dns.aucSecDnsAddr,
                            IMSA_IPV4_ADDR_LEN,
                            pstPdpInfo->stPdpIpv4Dns.aucSecDns,
                            IMSA_IPV4_ADDR_LEN);
        }

    }

    if ((IMSA_IP_TYPE_IPV6 == pstPdpInfo->stPdpAddr.enIpType)
        || (IMSA_IP_TYPE_IPV4V6 == pstPdpInfo->stPdpAddr.enIpType))
    {
        if (0 != IMSA_MEM_CMP(aucTemp, pstPdpInfo->stPdpAddr.aucIpV6Addr, IMSA_IPV6_PREFIX_LEN))
        {
            /* 配置IPV6地址 */
            IMSA_MEM_CPY_S(   pstPdpActInd->stPdpAddr.aucIpv6Addr,
                            IMSA_IPV6_ADDR_LEN,
                            pstPdpInfo->stPdpAddr.aucIpV6Addr,
                            IMSA_IPV6_ADDR_LEN);

            /* 配置IPV6 DNS地址 */
            if (IMSA_TRUE == pstPdpInfo->stPdpIpv6Dns.cOpPriDns)
            {
                pstPdpActInd->stIpv6Dns.bitOpPrimDnsAddr = IMSA_OP_TRUE;
                IMSA_MEM_CPY_S( pstPdpActInd->stIpv6Dns.aucPrimDnsAddr,
                                IMSA_IPV6_ADDR_LEN,
                                pstPdpInfo->stPdpIpv6Dns.aucPriDns,
                                IMSA_IPV6_ADDR_LEN);
            }


            if (IMSA_TRUE == pstPdpInfo->stPdpIpv6Dns.cOpSecDns)
            {
                pstPdpActInd->stIpv6Dns.bitOpSecDnsAddr = IMSA_OP_TRUE;
                IMSA_MEM_CPY_S( pstPdpActInd->stIpv6Dns.aucSecDnsAddr,
                                IMSA_IPV6_ADDR_LEN,
                                pstPdpInfo->stPdpIpv6Dns.aucSecDns,
                                IMSA_IPV6_ADDR_LEN);
            }

        }
    }


    /*调用消息发送函数 */
    IMSA_SND_MSG(pstPdpActInd);
    return;
}



VOS_VOID IMSA_SndMsgAtMtStatusInd(const IMSA_MT_STATUS_REPORT_STRU    *pstMtReport)
{
    IMSA_AT_MT_STATES_IND_STRU        *pstMtStatusInd = VOS_NULL_PTR;

    pstMtStatusInd = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_AT_MT_STATES_IND_STRU));

    if (VOS_NULL_PTR == pstMtStatusInd)
    {
        IMSA_ERR_LOG("IMSA_SndMsgAtMtStatusInd:ERROR:Alloc Msg fail!");
        return ;
    }

    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstMtStatusInd),
                    IMSA_GET_MSG_LENGTH(pstMtStatusInd),
                    0,
                    IMSA_GET_MSG_LENGTH(pstMtStatusInd));

    IMSA_WRITE_AT_MSG_MSG_HEAD(pstMtStatusInd, ID_IMSA_AT_MT_STATES_IND);

    pstMtStatusInd->ucOpId = 0;
    pstMtStatusInd->usClientId = 0x3fff;
    pstMtStatusInd->ulCauseCode = pstMtReport->ulCauseCode;
    pstMtStatusInd->ucMtStatus = pstMtReport->enMtStatus;
    IMSA_UtilStrNCpy((VOS_CHAR*)pstMtStatusInd->aucAsciiCallNum, pstMtReport->acNumber, IMSA_CALL_NUMBER_MAX_NUM);

    /*调用消息发送函数 */
    IMSA_SND_MSG(pstMtStatusInd);
}



VOS_VOID IMSA_SndMsgAtImsCtrlMsg(const VOS_VOID    *pstMsg, VOS_UINT32 ulLength)
{
    IMSA_AT_IMS_CTRL_MSG_STRU        *pstImsCtrlMsg = VOS_NULL_PTR;

    pstImsCtrlMsg = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_AT_IMS_CTRL_MSG_STRU) - 4 + ulLength);

    if (VOS_NULL_PTR == pstImsCtrlMsg)
    {
        IMSA_ERR_LOG("IMSA_SndMsgAtMtStatusInd:ERROR:Alloc Msg fail!");
        return ;
    }

    /*IMSA_MEM_SET( IMSA_GET_MSG_ENTITY(pstImsCtrlMsg), 0, IMSA_GET_MSG_LENGTH(pstImsCtrlMsg));*/
    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstImsCtrlMsg),
                  sizeof(IMSA_AT_IMS_CTRL_MSG_STRU) - 4 + ulLength - VOS_MSG_HEAD_LENGTH,
                  0,
                  sizeof(IMSA_AT_IMS_CTRL_MSG_STRU) - 4 + ulLength - VOS_MSG_HEAD_LENGTH);

    pstImsCtrlMsg->ucOpId       = 0;
    pstImsCtrlMsg->usClientId   = 0x3fff;

    pstImsCtrlMsg->ulWifiMsgLen = ulLength;

    IMSA_WRITE_AT_MSG_MSG_HEAD(pstImsCtrlMsg,
                               ID_IMSA_AT_IMS_CTRL_MSG);

    IMSA_MEM_CPY_S(pstImsCtrlMsg->aucWifiMsg,
                   ulLength,
                   pstMsg,
                   ulLength);

    /*调用消息发送函数 */
    IMSA_SND_MSG(pstImsCtrlMsg);
}



VOS_VOID IMSA_ProcMsgAtCtrlInd(const AT_IMSA_IMS_CTRL_MSG_STRU *pstCtrlInd)
{
    VOS_UINT32                           ulMsgType           = IMSA_FALSE;

    ulMsgType = ((((VOS_UINT32)pstCtrlInd->aucWifiMsg[3]) << 12) |
                 (((VOS_UINT32)pstCtrlInd->aucWifiMsg[2]) << 8) |
                 (((VOS_UINT32)pstCtrlInd->aucWifiMsg[1]) << 4) |
                 ((VOS_UINT32)pstCtrlInd->aucWifiMsg[0]));

    IMSA_INFO_LOG1("IMSA_ProcMsgAtCtrlInd:enter,ulMsgType= ",ulMsgType);

    switch (ulMsgType)
    {
        case ID_WIFI_IMSA_IMS_PDN_ACTIVATE_CNF:
            IMSA_CONN_ProcWifiMsgPdnActivateCnf((WIFI_IMSA_IMS_PDN_ACTIVATE_CNF_STRU*)&pstCtrlInd->aucWifiMsg[0]);
            break;

        case ID_WIFI_IMSA_IMS_PDN_READY_IND:
            IMSA_CONN_ProcWifiMsgPdnReadyInd((WIFI_IMSA_IMS_PDN_READY_IND_STRU*)&pstCtrlInd->aucWifiMsg[0]);
            break;

        case ID_WIFI_IMSA_IMS_PDN_DEACTIVATE_CNF:
            IMSA_CONN_ProcWifiMsgPdnDeactiveCnf((WIFI_IMSA_IMS_PDN_DEACTIVATE_CNF_STRU*)&pstCtrlInd->aucWifiMsg[0]);
            break;

        case ID_WIFI_IMSA_ERROR_IND:
            IMSA_CONN_ProcWifiMsgErrorInd((WIFI_IMSA_ERROR_IND_STRU*)&pstCtrlInd->aucWifiMsg[0]);
            break;

        case ID_WIFI_IMSA_STATE_IND:
            IMSA_SRV_ProcWifiMsgStateInd((WIFI_IMSA_STATE_IND_STRU*)&pstCtrlInd->aucWifiMsg[0]);
            break;

        default:
            break;
    }
}



VOS_VOID IMSA_ProcMsgAtDomainQryReq(const AT_IMSA_IMS_REG_DOMAIN_QRY_REQ_STRU *pstDomainQryReq)
{
    IMSA_AT_IMS_REG_DOMAIN_QRY_CNF_STRU *pstCirepQryCnf;


    /*分配消息空间*/
    pstCirepQryCnf = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_AT_IMS_REG_DOMAIN_QRY_CNF_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstCirepQryCnf)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndMsgAtPdpActInd:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SndMsgAtPdpActInd_ENUM, LNAS_NULL_PTR);
        return ;
    }

    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstCirepQryCnf), IMSA_GET_MSG_LENGTH(pstCirepQryCnf),0, IMSA_GET_MSG_LENGTH(pstCirepQryCnf));

    IMSA_WRITE_AT_MSG_MSG_HEAD(pstCirepQryCnf,ID_IMSA_AT_IMS_REG_DOMAIN_QRY_CNF);

    pstCirepQryCnf->usClientId          = pstDomainQryReq->usClientId;
    pstCirepQryCnf->ucOpId              = pstDomainQryReq->ucOpId;

    if (IMSA_SRV_STATUS_CONN_REG == IMSA_SRV_GetNormalSrvStatus())
    {

        if (IMSA_IMS_DOMAIN_LTE == IMSA_GetCurrentDomain())
        {
            pstCirepQryCnf->enImsRegDomain      = IMSA_AT_IMS_REG_DOMAIN_TYPE_LTE;
        }
        else if (IMSA_IMS_DOMAIN_WIFI == IMSA_GetCurrentDomain())
        {
            pstCirepQryCnf->enImsRegDomain      = IMSA_AT_IMS_REG_DOMAIN_TYPE_WIFI;
        }
        else
        {
            pstCirepQryCnf->enImsRegDomain      = IMSA_AT_IMS_REG_DOMAIN_TYPE_UNKNOWN;
        }
    }
    else
    {
        pstCirepQryCnf->enImsRegDomain          = IMSA_AT_IMS_REG_DOMAIN_TYPE_UNKNOWN;
    }




    /*调用消息发送函数 */
    IMSA_SND_MSG(pstCirepQryCnf);
}


#if 0

VOS_VOID IMSA_SndMsgAtRoamingImsSetCnf
(
    VOS_UINT8                           ucOpId,
    VOS_UINT16                          usClientId,
    VOS_UINT32                          ulResult
)
{
    IMSA_AT_ROAMING_IMS_SET_CNF_STRU          *pstRoamingImsSetCnf;

    /*分配消息空间*/
    pstRoamingImsSetCnf = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_AT_ROAMING_IMS_SET_CNF_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstRoamingImsSetCnf)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndMsgAtRoamingImsSetCnf:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SndMsgAtRoamingImsSetCnf_ENUM, LNAS_NULL_PTR);
        return ;
    }

    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstRoamingImsSetCnf),
                    IMSA_GET_MSG_LENGTH(pstRoamingImsSetCnf),
                    0,
                    IMSA_GET_MSG_LENGTH(pstRoamingImsSetCnf));
    IMSA_WRITE_AT_MSG_MSG_HEAD(pstRoamingImsSetCnf,ID_IMSA_AT_ROAMING_IMS_SET_CNF);


    /*填写消息内容*/
    pstRoamingImsSetCnf->ucOpId = ucOpId;
    pstRoamingImsSetCnf->usClientId = usClientId;
    pstRoamingImsSetCnf->ulResult = ulResult;

    /*调用消息发送函数 */
    IMSA_SND_MSG(pstRoamingImsSetCnf);
    return;
}
#endif


VOS_VOID IMSA_SndMsgAtRoamingImsQryCnf
(
    VOS_UINT8                           ucOpId,
    VOS_UINT16                          usClientId,
    AT_IMSA_ROAMING_IMS_SUPPORT_ENUM_UINT32     enRoamingImsSupportFlag
)
{
    IMSA_AT_ROAMING_IMS_QRY_CNF_STRU          *pstRoamingImsQryCnf;

    /*分配消息空间*/
    pstRoamingImsQryCnf = (VOS_VOID*)IMSA_ALLOC_MSG(sizeof(IMSA_AT_ROAMING_IMS_QRY_CNF_STRU));

    /*检测是否分配成功*/
    if (VOS_NULL_PTR == pstRoamingImsQryCnf)
    {
        /*打印异常信息*/
        IMSA_ERR_LOG("IMSA_SndMsgAtRoamingImsQryCnf:ERROR:Alloc Msg fail!");
        TLPS_PRINT2LAYER_ERROR(IMSA_SndMsgAtRoamingImsQryCnf_ENUM, LNAS_NULL_PTR);
        return ;
    }

    IMSA_MEM_SET_S( IMSA_GET_MSG_ENTITY(pstRoamingImsQryCnf),
                    IMSA_GET_MSG_LENGTH(pstRoamingImsQryCnf),
                    0,
                    IMSA_GET_MSG_LENGTH(pstRoamingImsQryCnf));
    IMSA_WRITE_AT_MSG_MSG_HEAD(pstRoamingImsQryCnf,ID_IMSA_AT_ROAMING_IMS_QRY_CNF);


    /*填写消息内容*/
    pstRoamingImsQryCnf->ucOpId = ucOpId;
    pstRoamingImsQryCnf->usClientId = usClientId;
    pstRoamingImsQryCnf->ulResult = VOS_OK;
    pstRoamingImsQryCnf->enRoamingImsSupportFlag = enRoamingImsSupportFlag;

    /*调用消息发送函数 */
    IMSA_SND_MSG(pstRoamingImsQryCnf);
    return;
}
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
/* end of ImsaProcAtMsg.c */




