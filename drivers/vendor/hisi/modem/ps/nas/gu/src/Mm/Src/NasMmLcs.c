

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "MM_Ext.h"
#include "NasMmlLib.h"
#include "MmSuspend.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NAS_MM_LCS_C
/*lint +e767*/


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_UINT32 NAS_MM_RcvLcsEstReq_PreProc(VOS_VOID* pMsg)
{
    LCS_MM_EST_REQ_STRU                *pstEstReqMsg  = VOS_NULL_PTR;

    pstEstReqMsg    = (LCS_MM_EST_REQ_STRU *)pMsg;

    /* 消息合法性检查 */
    /* Ti Value不能为'111' */
    if (ID_LCS_MM_EST_REQ != pstEstReqMsg->ulMsgId)
    {
        NAS_WARNING_LOG(WUEPS_PID_MM, "NAS_MM_RcvLcsEstReq_PreProc: Message legitimacy check failed!");
        return VOS_FALSE;
    }

    /* Ti非法处理 */
    if ( (pstEstReqMsg->ulTi < MM_CONST_NUM_8)
      || (pstEstReqMsg->ulTi > MM_CONST_NUM_14) )
    {
        /* 通知LCS建链失败 */
        NAS_MM_SndLcsRelInd(pstEstReqMsg->ulTi, NAS_MMCM_REL_CAUSE_MM_INTER_ERR_TI_INVALID);
        return VOS_FALSE;
    }

    /* 检查成功，记录Ti */
    NAS_MM_SetLcsEstReqTi(pstEstReqMsg->ulTi);

    /* 更新LCS业务建链状态 */
    NAS_MML_SetMoLcsStatus(NAS_MML_MO_LCS_SETUP_START);

    return VOS_TRUE;
}


VOS_VOID NAS_MM_RcvLcsEstReq_CSFB(VOS_VOID)
{
    /* 已经存在CSFB流程，则回复LCS建链失败 */
    if (VOS_TRUE == NAS_MML_IsCsfbServiceStatusExist())
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_CSFB_ALREADY_EXISTS);
        return;
    }

    /* 缓存建立请求 */
    NAS_MM_UpdateConnCtrlInfo(MM_CONN_CTRL_LCS);

    /* 更新CSFB状态 */
    NAS_MM_ProcCsfbServiceStatusChange(NAS_MML_GetCsfbServiceStatus(), NAS_MML_CSFB_SERVICE_STATUS_MO_NORMAL_LCS_EXIST);

    /* 记录CSFB业务建立开始 */
    NAS_MML_SetMoLcsStatus(NAS_MML_CSFB_MO_LCS_SETUP_START);

    /* 向L发送CSFB流程开始请求，CSFB是否可以发起的判断由LMM负责 */
    /* 如果不允许发起,L会回复LMM_MM_CSFB_SERVICE_END_IND */
    NAS_MM_SndLmmCsfbServiceStartNotify();

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_Null(VOS_VOID* pMsg)
{
    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsEstReq_PreProc(pMsg))
    {
        return;
    }

    /* CS域服务处于DETACH状态，回复LCS建链失败 */
    NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_CS_DETACH);

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_MmIdleNoCellAvaliable(VOS_VOID* pMsg)
{
    VOS_UINT8                           ucSimCsRegStatus;

    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsEstReq_PreProc(pMsg))
    {
        return;
    }

#if (FEATURE_ON == FEATURE_LTE)
    /* 当前接入技术为LTE only，则给LCS回复建链失败 */
    if (VOS_TRUE == NAS_MML_IsLteOnlyMode(NAS_MML_GetMsPrioRatList()))
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_NOT_SUPPORT_CS_CALL_S1_MODE_ONLY);
        return;
    }

    /* 如果当前驻留LTE，则触发CSFB流程 */
    if (NAS_MML_NET_RAT_TYPE_LTE == NAS_MML_GetCurrNetRatType())
    {
        NAS_MM_RcvLcsEstReq_CSFB();
        return;
    }
#endif

    /* 如果当前NO RF，需要缓存业务请求，通知MMC触发搜网 */
    ucSimCsRegStatus = NAS_MML_GetSimCsRegStatus();
    if ( (VOS_FALSE == NAS_MML_GetRfAvailFlg())
      && (VOS_TRUE == ucSimCsRegStatus) )
    {
        NAS_MM_UpdateConnCtrlInfo(MM_CONN_CTRL_LCS);
        Mm_SndMmcCmSvcInd(MM_CS_SERV_EXIST);
        return;
    }

    /* 其它情况通知LCS建链失败 */
    NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_OUT_OF_COVERAGE);

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_MmIdlePlmnSearch(VOS_VOID* pMsg)
{
    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsEstReq_PreProc(pMsg))
    {
        return;
    }

#if (FEATURE_ON == FEATURE_LTE)
    /* 当前接入技术为LTE only，则给LCS回复建链失败 */
    if (VOS_TRUE == NAS_MML_IsLteOnlyMode(NAS_MML_GetMsPrioRatList()))
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_NOT_SUPPORT_CS_CALL_S1_MODE_ONLY);
        return;
    }
#endif

    /* 其它情况通知LCS建链失败 */
    NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_SEARCHING_NETWORK);

    return;

}


VOS_VOID NAS_MM_RcvLcsEstReq_MmIdleNormalService(VOS_VOID* pMsg)
{
    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsEstReq_PreProc(pMsg))
    {
        return;
    }

#if (FEATURE_ON == FEATURE_LTE)
    /* 当前接入技术为LTE only，则给LCS回复建链失败 */
    if (VOS_TRUE == NAS_MML_IsLteOnlyMode(NAS_MML_GetMsPrioRatList()))
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_NOT_SUPPORT_CS_CALL_S1_MODE_ONLY);
        return;
    }

    /* 如果当前驻留LTE，则触发LCS CSFB流程 */
    if (NAS_MML_NET_RAT_TYPE_LTE == NAS_MML_GetCurrNetRatType())
    {
        NAS_MM_RcvLcsEstReq_CSFB();
        return;
    }
#endif

    /* 当前小区CS域正常业务受限，则回复LCS建链失败 */
    if (VOS_TRUE == NAS_MML_GetCsRestrictNormalServiceFlg())
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_CS_ACCESS_BAR);
        return;
    }

    /* 记录收到建链请求时的状态，触发MM建链流程 */
    NAS_MM_SetStaOfRcvXXEstReq(NAS_MM_GetMmState());
    NAS_MM_EstMmConnection(MM_CONN_CTRL_LCS);

    /* 更新CS域服务连接状态、缓存状态 */
    NAS_MM_UpdateCsServiceConnStatusFlg();
    NAS_MM_UpdateCsServiceBufferStatusFlg();

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_MmIdleLimitedService(VOS_VOID* pMsg)
{
    NAS_MML_MISCELLANEOUS_CFG_INFO_STRU    *pstMiscellaneousCfgInfo = VOS_NULL_PTR;

    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsEstReq_PreProc(pMsg))
    {
        return;
    }

    /* CS域卡无效，则回复LCS建链失败 */
    if (VOS_FALSE  == NAS_MML_GetSimCsRegStatus())
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_CS_SIM_INVALID);
        return;
    }

    /* 如果当前PS ONLY时支持CS业务NV未激活，或非PS ONLY则回复LCS建链失败 */
    pstMiscellaneousCfgInfo = NAS_MML_GetMiscellaneousCfgInfo();
    if ( (NAS_MML_MS_MODE_PS_ONLY != NAS_MML_GetMsMode())
      || (NAS_MMC_NV_ITEM_DEACTIVE == pstMiscellaneousCfgInfo->ucPsOnlyCsServiceSupportFlg) )
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_CS_DETACH);
        return;
    }

#if (FEATURE_ON == FEATURE_LTE)
    /* 当前接入技术为LTE only，则给LCS回复建链失败 */
    if (VOS_TRUE == NAS_MML_IsLteOnlyMode(NAS_MML_GetMsPrioRatList()))
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_NOT_SUPPORT_CS_CALL_S1_MODE_ONLY);
        return;
    }

    /* 如果当前驻留LTE，则触发LCS的CSFB流程 */
    if (NAS_MML_NET_RAT_TYPE_LTE == NAS_MML_GetCurrNetRatType())
    {
        NAS_MM_RcvLcsEstReq_CSFB();
        return;
    }
#endif

    /* 当前小区CS域正常业务受限，则回复LCS建链失败 */
    if (VOS_TRUE == NAS_MML_GetCsRestrictNormalServiceFlg())
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_CS_ACCESS_BAR);
        return;
    }

    /* 缓存LCS业务建链请求，触发CS注册流程 */
    g_MmGlobalInfo.ucStaOfRcvXXEstReq = g_MmGlobalInfo.ucState;
    NAS_MM_UpdateConnCtrlInfo(MM_CONN_CTRL_LCS);
    NAS_MML_SetCsAttachAllowFlg(VOS_TRUE);

    if (VOS_TRUE == NAS_MM_IsClassAAndNmoIMode(g_MmGlobalInfo.ucNtMod))
    {
        /* 网络模式I，通知GMM发起ATTACH流程 */
        NAS_MM_SndGmmRegisterInit();
    }
    else
    {
        NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_RcvLcsEstReq_MmIdleLimitedService: NOT A+I trigger lau");

        /* 设置LU类型，触发LAU流程 */
        g_MmGlobalInfo.LuInfo.ucLuType      = MM_IE_LUT_NORMAL_LU;
        g_MmGlobalInfo.LuInfo.ucLuAttmptCnt = 0;
        Mm_ComLuOnly();
    }

    /* 更新CS域服务连接状态、缓存状态 */
    NAS_MM_UpdateCsServiceConnStatusFlg();
    NAS_MM_UpdateCsServiceBufferStatusFlg();

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_MmIdleAttempToUpdate(VOS_VOID* pMsg)
{
    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsEstReq_PreProc(pMsg))
    {
        return;
    }

#if (FEATURE_ON == FEATURE_LTE)
    /* 如果当前驻留LTE，则触发LCS的CSFB流程 */
    if (NAS_MML_NET_RAT_TYPE_LTE == NAS_MML_GetCurrNetRatType())
    {
        NAS_MM_RcvLcsEstReq_CSFB();
        return;
    }
#endif

    /* 当前小区CS域正常业务受限，则回复LCS建链失败 */
    if (VOS_TRUE == NAS_MML_GetCsRestrictNormalServiceFlg())
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_CS_ACCESS_BAR);
        return;
    }

    if (VOS_TRUE == NAS_MML_IsPlmnSupportDam(NAS_MML_GetCurrCampPlmnId()))
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_UE_INVALID_STATE);
        return;
    }

    /* 缓存LCS业务建链请求，触发CS注册流程 */
    NAS_MM_UpdateConnCtrlInfo(MM_CONN_CTRL_LCS);

    if (VOS_TRUE == NAS_MM_IsClassAAndNmoIMode(g_MmGlobalInfo.ucNtMod))
    {
        /* 网络模式I，通知GMM发起ATTACH流程 */
        NAS_MM_SndGmmRegisterInit();
    }
    else
    {
        NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_RcvLcsEstReq_MmIdleAttempToUpdate: NOT A+I trigger lau");

        /* 设置LU类型，触发LAU流程 */
        g_MmGlobalInfo.LuInfo.ucLuType      = MM_IE_LUT_NORMAL_LU;
        g_MmGlobalInfo.LuInfo.ucLuAttmptCnt = 0;
        Mm_ComLuOnly();
    }

    /* 更新CS域服务连接状态、缓存状态 */
    NAS_MM_UpdateCsServiceConnStatusFlg();
    NAS_MM_UpdateCsServiceBufferStatusFlg();

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_MmIdleLauNeeded(VOS_VOID* pMsg)
{
    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsEstReq_PreProc(pMsg))
    {
        return;
    }

#if (FEATURE_ON == FEATURE_LTE)
    /* 当前接入技术为LTE only，则给LCS回复建链失败 */
    if (VOS_TRUE == NAS_MML_IsLteOnlyMode(NAS_MML_GetMsPrioRatList()))
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_NOT_SUPPORT_CS_CALL_S1_MODE_ONLY);
        return;
    }

    /* 如果当前驻留LTE，则触发LCS CSFB流程 */
    if (NAS_MML_NET_RAT_TYPE_LTE == NAS_MML_GetCurrNetRatType())
    {
        NAS_MM_RcvLcsEstReq_CSFB();
        return;
    }
#endif

    /* 当前小区CS域正常业务受限，则回复LCS建链失败 */
    if (VOS_TRUE == NAS_MML_GetCsRestrictNormalServiceFlg())
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_CS_ACCESS_BAR);
        return;
    }

    /* 当前CS域服务状态不正常，则回复LCS建链失败 */
    if (MM_COM_SRVST_NORMAL_SERVICE != NAS_MM_GetLauUptNeededCsSrvStatus())
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_CS_ACCESS_BAR);
        return;
    }

    /* 记录收到建链请求时的状态，触发MM建链流程 */
    g_MmGlobalInfo.ucStaOfRcvXXEstReq = g_MmGlobalInfo.ucState;
    NAS_MM_EstMmConnection(MM_CONN_CTRL_LCS);

    /* 更新CS域服务连接状态、缓存状态 */
    NAS_MM_UpdateCsServiceConnStatusFlg();
    NAS_MM_UpdateCsServiceBufferStatusFlg();

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_MmIdleNoImsi(VOS_VOID* pMsg)
{
    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsEstReq_PreProc(pMsg))
    {
        return;
    }

    /* SIM卡无效，回复LCS建链失败 */
    NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_CS_SIM_INVALID);

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_WaitForOutgoingMmConn(VOS_VOID* pMsg)
{
    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsEstReq_PreProc(pMsg))
    {
        return;
    }

    /* 当前小区CS域正常业务受限，则回复LCS建链失败 */
    if (VOS_TRUE == NAS_MML_GetCsRestrictNormalServiceFlg())
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_CS_ACCESS_BAR);
        return;
    }

    /* 检查多业务冲突是否允许 */
    if (VOS_FALSE == Mm_IsMultiSrvCollisionAllow(MM_CONN_CTRL_LCS))
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_CS_SERVICE_CONGESTION);
        return;
    }

    /* 有缓存的LCS建链请求且与新请求ti值不同,则回复LCS建链失败 */
    if ( (MM_TRUE == g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_LCS].RcvXXEstReq.ucFlg)
      && (NAS_MM_GetLcsEstReqTi() != g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_LCS].RcvXXEstReq.ucFlg) )
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(),
                       NAS_MMCM_REL_CAUSE_MM_INTER_ERR_CC_CONN_REQ_EXIST);
        return;
    }

    /* 缓存LCS业务建链请求 */
    NAS_MM_UpdateConnCtrlInfo(MM_CONN_CTRL_LCS);

    /* 更新CS域服务连接状态、缓存状态 */
    NAS_MM_UpdateCsServiceConnStatusFlg();
    NAS_MM_UpdateCsServiceBufferStatusFlg();

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_MmConnectionActive(VOS_VOID* pMsg)
{
    MM_MSG_CM_SVC_REQ_STRU              stCmSvcReq;

    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsEstReq_PreProc(pMsg))
    {
        return;
    }

    /* 存在其他业务链接时一定处理LCS请求 */
    /* 请求Ti的链路已经建立，则直接给LCS回复建链成功 */
    if (VOS_TRUE == NAS_MM_IsMmConnExist(MM_CONN_CTRL_LCS, NAS_MM_GetLcsEstReqTi()))
    {
        NAS_MM_SndLcsEstCnf(NAS_MM_GetLcsEstReqTi());
    }
    else
    {
        /* 链接没有建立则触发MM建链 */
        NAS_MM_SetStaOfRcvXXEstReq(NAS_MM_GetMmState());

        /* 缓存消息 */
        NAS_MM_UpdateConnCtrlInfo(MM_CONN_CTRL_LCS);

        /* 填充CM SERVICE REQUEST结构体，并发送到接入层 */
        /* 后续LCS支持LMU定位流程时需要判断修改CMSvcType为MM_IE_CM_SVC_TYPE_LCS */
        Mm_ComFillInCMSncReq(MM_IE_CM_SVC_TYPE_SS_ACT, MM_FALSE, &stCmSvcReq);
        Mm_ComMsgCmSvcReqSnd(&stCmSvcReq);

        /* 记录正在建立的MM连接的PD */
        NAS_MM_SetMMConnEstingPD(MM_IE_PD_LOCATION_SRVC);

        /* 迁移到WAIT FOR ADDITIONAL OUTGING MM CONNECTION状态，启动T3230定时器 */
        Mm_ComSetMmState(WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION);
        NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_RcvLcsEstReq_MmConnectionActive: MM STATUS is WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION");
        Mm_TimerStart(MM_TIMER_T3230);
    }

    /* 更新CS域服务连接状态、缓存状态 */
    NAS_MM_UpdateCsServiceConnStatusFlg();
    NAS_MM_UpdateCsServiceBufferStatusFlg();

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_WaitForNetworkCmd(VOS_VOID* pMsg)
{
    /* 此状态下处理同WAIT FOR OUTGOING MM CONNECTION状态 */
    NAS_MM_RcvLcsEstReq_WaitForOutgoingMmConn(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_WaitForRrConn(VOS_VOID* pMsg)
{
    /* 此状态下处理同WAIT FOR OUTGOING MM CONNECTION状态 */
    NAS_MM_RcvLcsEstReq_WaitForOutgoingMmConn(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_WaitForReestReq(VOS_VOID* pMsg)
{
    /* 此状态下处理同WAIT FOR OUTGOING MM CONNECTION状态 */
    NAS_MM_RcvLcsEstReq_WaitForOutgoingMmConn(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_WaitForReestCnf(VOS_VOID* pMsg)
{
    /* 此状态下处理同WAIT FOR OUTGOING MM CONNECTION状态 */
    NAS_MM_RcvLcsEstReq_WaitForOutgoingMmConn(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_WaitForAdditionalMmConn(VOS_VOID* pMsg)
{
    /* 此状态下处理同WAIT FOR OUTGOING MM CONNECTION状态 */
    NAS_MM_RcvLcsEstReq_WaitForOutgoingMmConn(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_LauPending(VOS_VOID* pMsg)
{
    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsEstReq_PreProc(pMsg))
    {
        return;
    }

#if (FEATURE_ON == FEATURE_LTE)
    /* 当前接入技术为LTE only，则给LCS回复建链失败 */
    if (VOS_TRUE == NAS_MML_IsLteOnlyMode(NAS_MML_GetMsPrioRatList()))
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_NOT_SUPPORT_CS_CALL_S1_MODE_ONLY);
        return;
    }
#endif

    /* 当前小区CS域正常业务受限，则回复LCS建链失败 */
    if (VOS_TRUE == NAS_MML_GetCsRestrictNormalServiceFlg())
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_CS_ACCESS_BAR);
        return;
    }

    /* 有缓存的LCS建链请求且与新请求ti值不同,则回复LCS建链失败 */
    if ( (MM_TRUE == g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_LCS].RcvXXEstReq.ucFlg)
      && (NAS_MM_GetLcsEstReqTi() != g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_LCS].RcvXXEstReq.ucFlg) )
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(),
                       NAS_MMCM_REL_CAUSE_MM_INTER_ERR_CC_CONN_REQ_EXIST);
        return;
    }

    /* 缓存LCS业务建链请求 */
    NAS_MM_UpdateConnCtrlInfo(MM_CONN_CTRL_LCS);

    /* 更新CS域服务连接状态、缓存状态 */
    NAS_MM_UpdateCsServiceConnStatusFlg();
    NAS_MM_UpdateCsServiceBufferStatusFlg();

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_ImsiDetachPending(VOS_VOID* pMsg)
{
    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsEstReq_PreProc(pMsg))
    {
        return;
    }

#if (FEATURE_ON == FEATURE_LTE)
    /* 当前接入技术为LTE only，则给LCS回复建链失败 */
    if (VOS_TRUE == NAS_MML_IsLteOnlyMode(NAS_MML_GetMsPrioRatList()))
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_NOT_SUPPORT_CS_CALL_S1_MODE_ONLY);
        return;
    }

    /* 如果当前驻留LTE，则触发LCS的CSFB流程 */
    if (NAS_MML_NET_RAT_TYPE_LTE == NAS_MML_GetCurrNetRatType())
    {
        NAS_MM_RcvLcsEstReq_CSFB();
        return;
    }
#endif

    /* 当前小区CS域正常业务受限，则回复LCS建链失败 */
    if (VOS_TRUE == NAS_MML_GetCsRestrictNormalServiceFlg())
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_CS_ACCESS_BAR);
        return;
    }

    /* 有缓存的LCS建链请求且与新请求ti值不同,则回复LCS建链失败 */
    if ( (MM_TRUE == g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_LCS].RcvXXEstReq.ucFlg)
      && (NAS_MM_GetLcsEstReqTi() != g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_LCS].RcvXXEstReq.ucFlg) )
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(),
                       NAS_MMCM_REL_CAUSE_MM_INTER_ERR_CC_CONN_REQ_EXIST);
        return;
    }

    /* 缓存LCS业务建链请求 */
    NAS_MM_UpdateConnCtrlInfo(MM_CONN_CTRL_LCS);

    /* 更新CS域服务连接状态、缓存状态 */
    NAS_MM_UpdateCsServiceConnStatusFlg();
    NAS_MM_UpdateCsServiceBufferStatusFlg();

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_WaitForAttach(VOS_VOID* pMsg)
{
    NAS_MML_MISCELLANEOUS_CFG_INFO_STRU    *pstMiscellaneousCfgInfo = VOS_NULL_PTR;

    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsEstReq_PreProc(pMsg))
    {
        return;
    }

    /* 当前小区CS域正常业务受限，则回复LCS建链失败 */
    if (VOS_TRUE == NAS_MML_GetCsRestrictNormalServiceFlg())
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_CS_ACCESS_BAR);
        return;
    }

    /* 如果当前PS ONLY时支持CS业务NV未激活，则回复LCS建链失败 */
    pstMiscellaneousCfgInfo = NAS_MML_GetMiscellaneousCfgInfo();
    if ( (NAS_MML_MS_MODE_PS_ONLY == NAS_MML_GetMsMode())
      && (NAS_MMC_NV_ITEM_DEACTIVE == pstMiscellaneousCfgInfo->ucPsOnlyCsServiceSupportFlg) )
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_CS_DETACH);
        return;
    }

    /* 缓存LCS业务建链请求，触发CS注册流程 */
    g_MmGlobalInfo.ucStaOfRcvXXEstReq = g_MmGlobalInfo.ucState;
    NAS_MM_UpdateConnCtrlInfo(MM_CONN_CTRL_LCS);
    NAS_MML_SetCsAttachAllowFlg(VOS_TRUE);

    if (VOS_TRUE == NAS_MM_IsClassAAndNmoIMode(g_MmGlobalInfo.ucNtMod))
    {
        /* 网络模式I，通知GMM发起ATTACH流程 */
        NAS_MM_SndGmmRegisterInit();
    }
    else
    {
        NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_RcvLcsEstReq_WaitForAttach: NOT A+I trigger lau");

        /* 设置LU类型，触发LAU流程 */
        g_MmGlobalInfo.LuInfo.ucLuType      = MM_IE_LUT_NORMAL_LU;
        g_MmGlobalInfo.LuInfo.ucLuAttmptCnt = 0;
        Mm_ComLuOnly();
    }

    /* 更新CS域服务连接状态、缓存状态 */
    NAS_MM_UpdateCsServiceConnStatusFlg();
    NAS_MM_UpdateCsServiceBufferStatusFlg();

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_WaitForRrConnLau(VOS_VOID* pMsg)
{
    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsEstReq_PreProc(pMsg))
    {
        return;
    }

    /* 当前小区CS域正常业务受限，则回复LCS建链失败 */
    if (VOS_TRUE == NAS_MML_GetCsRestrictNormalServiceFlg())
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_CS_ACCESS_BAR);
        return;
    }

    /* 缓存LCS业务建链请求 */
    g_MmGlobalInfo.ucStaOfRcvXXEstReq = g_MmGlobalInfo.ucState;
    NAS_MM_UpdateConnCtrlInfo(MM_CONN_CTRL_LCS);

    /* 更新CS域服务连接状态、缓存状态 */
    NAS_MM_UpdateCsServiceConnStatusFlg();
    NAS_MM_UpdateCsServiceBufferStatusFlg();

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_LauInitiated(VOS_VOID* pMsg)
{
    /* 此状态下处理同WAIT FOR RR CONNECTION (LOCATION UPDATING)状态 */
    NAS_MM_RcvLcsEstReq_WaitForRrConnLau(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_LauRejected(VOS_VOID* pMsg)
{
    /* 此状态下处理同WAIT FOR RR CONNECTION (LOCATION UPDATING)状态 */
    NAS_MM_RcvLcsEstReq_WaitForRrConnLau(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_WaitForRrConnImsiDetach(VOS_VOID* pMsg)
{
    /* 此状态下处理同NULL状态 */
    NAS_MM_RcvLcsEstReq_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_ImsiDetachInitiated(VOS_VOID* pMsg)
{
    /* 此状态下处理同NULL状态 */
    NAS_MM_RcvLcsEstReq_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_ProcCmServicePrompt(VOS_VOID* pMsg)
{
    /* 此状态下处理同WAIT FOR RR CONNECTION (LOCATION UPDATING)状态 */
    NAS_MM_RcvLcsEstReq_WaitForRrConnLau(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_RrConnRelNotAllow(VOS_VOID* pMsg)
{
    MM_MSG_CM_SVC_REQ_STRU              stCmSvcReq;

    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsEstReq_PreProc(pMsg))
    {
        return;
    }

    /* 当前小区CS域正常业务受限，则回复LCS建链失败 */
    if (VOS_TRUE == NAS_MML_GetCsRestrictNormalServiceFlg())
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_CS_ACCESS_BAR);
        return;
    }

    /* 当前处于受限服务状态，则回复LCS建链失败 */
    if (MM_TRUE == Mm_ComJudgeLimitSvc())
    {
        NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_UE_INVALID_STATE);
        return;
    }

    /* 停止T3241定时器 */
    Mm_TimerStop(MM_TIMER_T3241);

    /* 触发MM建链 */

    /* 缓存消息 */
    NAS_MM_UpdateConnCtrlInfo(MM_CONN_CTRL_LCS);

    /* 填充CM SERVICE REQUEST结构体，并发送到接入层 */
    /* 后续LCS支持LMU定位流程时需要判断修改CMSvcType为MM_IE_CM_SVC_TYPE_LCS */
    Mm_ComFillInCMSncReq(MM_IE_CM_SVC_TYPE_SS_ACT, MM_FALSE, &stCmSvcReq);
    Mm_ComMsgCmSvcReqSnd(&stCmSvcReq);

    /* 记录正在建立的MM连接的PD */
    NAS_MM_SetMMConnEstingPD(MM_IE_PD_LOCATION_SRVC);

    /* 迁移到WAIT FOR OUTGING MM CONNECTION状态，启动T3230定时器 */
    Mm_ComSetMmState(WAIT_FOR_OUTGOING_MM_CONNECTION);
    NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_RcvLcsEstReq_MmConnectionActive: MM STATUS is WAIT_FOR_OUTGOING_MM_CONNECTION");
    Mm_TimerStart(MM_TIMER_T3230);

    /* 更新CS域服务连接状态、缓存状态 */
    NAS_MM_UpdateCsServiceConnStatusFlg();
    NAS_MM_UpdateCsServiceBufferStatusFlg();

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_TestContrlActive(VOS_VOID* pMsg)
{
    /* 此状态下处理同NULL状态 */
    NAS_MM_RcvLcsEstReq_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_MmInterRatChange(VOS_VOID* pMsg)
{
    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsEstReq_PreProc(pMsg))
    {
        return;
    }

    /* 内部接入技术切换中，回复LCS建链失败 */
    NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_INTER_RAT_SYSTEM_CHANGE);

    return;
}


VOS_VOID NAS_MM_RcvLcsEstReq_MmIdleECallInactive(VOS_VOID* pMsg)
{
    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsEstReq_PreProc(pMsg))
    {
        return;
    }

    /* eCall Inactive时拒绝LCS MO-LR业务，回复LCS建链失败 */
    NAS_MM_SndLcsRelInd(NAS_MM_GetLcsEstReqTi(), NAS_MMCM_REL_CAUSE_MM_INTER_ERR_ECALL_INACTIVE);

    return;
}


VOS_UINT32 NAS_MM_RcvLcsDataReq_PreProc(VOS_VOID* pMsg)
{
    LCS_MM_DATA_REQ_STRU               *pstDataReqMsg   = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;

    pstDataReqMsg   = (LCS_MM_DATA_REQ_STRU *)pMsg;
    ulResult        = VOS_TRUE;

    /* 消息合法性检查 */
    if ( (ID_LCS_MM_DATA_REQ != pstDataReqMsg->ulMsgId)
      || (MM_CONST_NUM_15 < pstDataReqMsg->ulTi) )
    {
        ulResult = VOS_FALSE;
        NAS_WARNING_LOG(WUEPS_PID_MM, "NAS_MM_RcvLcsDataReq_PreProc: Message legitimacy check failed!");
    }

    /* 判断Ti对应的MM链接是否存在，不存在则通知LCS释放链接 */
    if (VOS_TRUE != NAS_MM_IsMmConnExist(MM_CONN_CTRL_LCS, pstDataReqMsg->ulTi))
    {
        ulResult = VOS_FALSE;
        NAS_MM_SndLcsRelInd(pstDataReqMsg->ulTi, NAS_MMCM_REL_CAUSE_MM_INTER_ERR_TI_INVALID);
    }

    NAS_SS_ReportM2NOtaMsg((SS_MSG_STRU *)&pstDataReqMsg->stLcsMsg);

    return ulResult;
}


VOS_VOID NAS_MM_RcvLcsDataReq_MmConnectionActive(VOS_VOID* pMsg)
{
    LCS_MM_DATA_REQ_STRU               *pstLcsDataReq = VOS_NULL_PTR;

    pstLcsDataReq = (LCS_MM_DATA_REQ_STRU *)pMsg;

    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsDataReq_PreProc(pMsg))
    {
        return;
    }

    /* 通过RRMM_DATA_REQ消息将LCS空口信令发给接入层 */
    Mm_SndRrDataReq(pstLcsDataReq->stLcsMsg.ulLcsMsgSize,
        &pstLcsDataReq->stLcsMsg.aucLcsMsg[0],
        RRC_NAS_MSG_PRIORTY_HIGH);

    return;
}


VOS_VOID NAS_MM_RcvLcsDataReq_WaitForAdditionalMmConn(VOS_VOID* pMsg)
{
    LCS_MM_DATA_REQ_STRU               *pstLcsDataReq = VOS_NULL_PTR;

    pstLcsDataReq = (LCS_MM_DATA_REQ_STRU *)pMsg;

    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsDataReq_PreProc(pMsg))
    {
        return;
    }

    /* 通过RRMM_DATA_REQ消息将LCS空口信令发给接入层 */
    Mm_SndRrDataReq(pstLcsDataReq->stLcsMsg.ulLcsMsgSize,
        &pstLcsDataReq->stLcsMsg.aucLcsMsg[0],
        RRC_NAS_MSG_PRIORTY_HIGH);

    return;
}


VOS_VOID NAS_MM_RcvLcsDataReq_MmInterRatChange(VOS_VOID* pMsg)
{
    /* 消息头结构，用于判断消息类型 */
    MSG_HEADER_STRU                    *pstMsgHeader;

    /* 提取消息头 */
    pstMsgHeader = (MSG_HEADER_STRU *)pMsg;

    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsDataReq_PreProc(pMsg))
    {
        return;
    }

    /* 调用函数 MM_SaveMsg 进行缓存处理 */
    MM_SaveMsg(pMsg, g_MmSubLyrShare.MmShare.MmSuspendEvent, pstMsgHeader->ulLength + VOS_MSG_HEAD_LENGTH);

    return;
}


VOS_UINT32 NAS_MM_RcvLcsRelReq_PreProc(VOS_VOID* pMsg)
{
    LCS_MM_REL_REQ_STRU                *pstRelReqMsg   = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;

    pstRelReqMsg    = (LCS_MM_REL_REQ_STRU *)pMsg;
    ulResult        = VOS_FALSE;

    /* 消息合法性检查 */
    /* Ti Value不能为'111' */
    if ( (ID_LCS_MM_REL_REQ != pstRelReqMsg->ulMsgId)
      || (MM_CONST_NUM_14 < pstRelReqMsg->ulTi)
      || (MM_CONST_NUM_7 == pstRelReqMsg->ulTi) )
    {
        ulResult = VOS_FALSE;
        NAS_WARNING_LOG(WUEPS_PID_MM, "NAS_MM_RcvLcsRelReq_PreProc: Message legitimacy check failed!");
    }
    else
    {
        /* 记录LCS释放链路请求的Ti */
        ulResult = VOS_TRUE;
        NAS_MM_SetLcsRelReqTi(pstRelReqMsg->ulTi);
    }

    return ulResult;
}


VOS_VOID NAS_MM_RcvLcsRelReq_MmIdleNoCellAvaliable(VOS_VOID* pMsg)
{
    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsRelReq_PreProc(pMsg))
    {
        return;
    }

    /* 当前并没有缓存该Ti的建链请求，则直接返回 */
    if (VOS_TRUE != NAS_MM_IsCsServiceBuffered(MM_CONN_CTRL_LCS, NAS_MM_GetLcsRelReqTi()))
    {
        NAS_WARNING_LOG1(WUEPS_PID_MM, "NAS_MM_RcvLcsRelReq_MmIdleNoCellAvaliable: Establish request not buffered! Ti:", NAS_MM_GetLcsRelReqTi());
        return;
    }

    /* 清除LCS建链请求缓存标志及MM建链标志 */
    NAS_MM_ClearConnCtrlInfoWithBuffer(MM_CONN_CTRL_LCS);

#if (FEATURE_ON == FEATURE_LTE)
    /* 当前存在LCS业务的CSFB流程，则清空CSFB存在标志 */
    if (NAS_MML_CSFB_SERVICE_STATUS_MO_NORMAL_LCS_EXIST == NAS_MML_GetCsfbServiceStatus())
    {
        NAS_MM_ProcCsfbServiceStatusChange(NAS_MML_GetCsfbServiceStatus(), NAS_MML_CSFB_SERVICE_STATUS_NOT_EXIST);

        if (NAS_MML_NET_RAT_TYPE_LTE == NAS_MML_GetCurrNetRatType())
        {
            /* 当前接入技术为LTE，通知LMM终止CSFB流程 */
            NAS_MM_SndLmmCsfbServiceAbortNotify();
        }
    }
#endif

    /* 更新CS域服务连接状态、缓存状态 */
    NAS_MM_UpdateCsServiceConnStatusFlg();
    NAS_MM_UpdateCsServiceBufferStatusFlg();

    return;
}


VOS_VOID NAS_MM_RcvLcsRelReq_MmIdlePlmnSearch(VOS_VOID* pMsg)
{
    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsRelReq_PreProc(pMsg))
    {
        return;
    }

    /* 当前并没有缓存该Ti的建链请求，则直接返回 */
    if (VOS_TRUE != NAS_MM_IsCsServiceBuffered(MM_CONN_CTRL_LCS, NAS_MM_GetLcsRelReqTi()))
    {
        NAS_WARNING_LOG1(WUEPS_PID_MM, "NAS_MM_RcvLcsRelReq_MmIdleNoCellAvaliable: Establish request not buffered! Ti:", NAS_MM_GetLcsRelReqTi());
        return;
    }

    /* 清除LCS建链请求缓存标志及MM建链标志 */
    NAS_MM_ClearConnCtrlInfoWithBuffer(MM_CONN_CTRL_LCS);

#if (FEATURE_ON == FEATURE_LTE)
    /* 当前存在LCS业务的CSFB流程，则清空CSFB存在标志 */
    if (NAS_MML_CSFB_SERVICE_STATUS_MO_NORMAL_LCS_EXIST == NAS_MML_GetCsfbServiceStatus())
    {
        NAS_MM_ProcCsfbServiceStatusChange(NAS_MML_GetCsfbServiceStatus(), NAS_MML_CSFB_SERVICE_STATUS_NOT_EXIST);

        if (NAS_MML_NET_RAT_TYPE_LTE == NAS_MML_GetCurrNetRatType())
        {
            /* 当前接入技术为LTE，通知LMM终止CSFB流程 */
            NAS_MM_SndLmmCsfbServiceAbortNotify();
        }

        /* 通知MMC终止CSFB流程 */
        NAS_MM_SndMmcPlmnSearchInd(MM_MMC_PLMN_SEARCH_TYPE_CSFB_END);
    }
#endif

    /* 更新CS域服务连接状态、缓存状态 */
    NAS_MM_UpdateCsServiceConnStatusFlg();
    NAS_MM_UpdateCsServiceBufferStatusFlg();

    return;
}


VOS_VOID NAS_MM_RcvLcsRelReq_MmIdleNormalService(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM IDLE NO CELL AVAILABLE状态 */
    NAS_MM_RcvLcsRelReq_MmIdleNoCellAvaliable(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsRelReq_MmIdleLimitedService(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM IDLE NO CELL AVAILABLE状态 */
    NAS_MM_RcvLcsRelReq_MmIdleNoCellAvaliable(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsRelReq_MmIdleAttempToUpdate(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM IDLE NO CELL AVAILABLE状态 */
    NAS_MM_RcvLcsRelReq_MmIdleNoCellAvaliable(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsRelReq_MmIdleLauNeeded(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM IDLE NO CELL AVAILABLE状态 */
    NAS_MM_RcvLcsRelReq_MmIdleNoCellAvaliable(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsRelReq_MmIdleNoImsi(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM IDLE NO CELL AVAILABLE状态 */
    NAS_MM_RcvLcsRelReq_MmIdleNoCellAvaliable(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsRelReq_WaitForOutgoingMmConn(VOS_VOID* pMsg)
{
    VOS_UINT8                           ucMmConnFlg;

    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsRelReq_PreProc(pMsg))
    {
        return;
    }

    if (g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_LCS].ucMMConnEstingTI == NAS_MM_GetLcsEstReqTi())
    {
        /* 请求的Ti正在进行MM建链流程 */
        /* 清除MM建链标记和清除缓存 */
        NAS_MM_ClearConnCtrlInfoWithBuffer(MM_CONN_CTRL_LCS);

        /* 停T3230定时器 */
        Mm_TimerStop(MM_TIMER_T3230);

        /* 向网络发送CM_SRV_ABORT请求 */
        Mm_ComMsgCmSvcAbortSnd();

        /* 记录返回到接收到建立MM连接请求时的状态 */
        NAS_MM_SetStaAfterWaitForNwkCmd(NAS_MM_GetStaOfRcvXXEstReq());

        /* 存储建链流程及原因值 */
        Mm_ComSaveProcAndCauseVal( MM_MM_CONN_PROC, NAS_MML_REG_FAIL_CAUSE_NULL );

        /* 记录迁移之前的状态 */
        NAS_MM_SetMmPreState(NAS_MM_GetMmState());

        ucMmConnFlg = Mm_ComMmConnExist();
        if ( (RRMM_LCS_STATE_START == NAS_MM_GetRrLcsState())
          && (MM_TRUE != ucMmConnFlg) )
        {
            /* 迁移到RR CONNECTION RELEASE NOT ALLOWED状态，启动T3241定时器 */
            Mm_ComSetMmState(MM_STATE_RR_CONNECTION_RELEASE_NOT_ALLOWED);
            NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_RcvLcsRelReq_WaitForOutgoingMmConn: MM STATUS is MM_STATE_RR_CONNECTION_RELEASE_NOT_ALLOWED.");
            Mm_TimerStart(MM_TIMER_T3241);
        }
        else
        {
            /* 迁移到WAIT FOR NETWORK COMMAND状态，启动T3240定时器 */
            Mm_ComSetMmState(WAIT_FOR_NETWORK_COMMAND);
            NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_RcvLcsRelReq_WaitForOutgoingMmConn: MM STATUS is WAIT_FOR_NETWORK_COMMAND.");
            Mm_TimerStart(MM_TIMER_T3240);
        }
    }
    else if (VOS_TRUE == NAS_MM_IsCsServiceBuffered(MM_CONN_CTRL_LCS, NAS_MM_GetLcsRelReqTi()))
    {
        /* 请求的Ti为缓存的建链请求的Ti，则清除缓存 */
        NAS_MM_ClearConnCtrlInfoWithBuffer(MM_CONN_CTRL_LCS);
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MM, "NAS_MM_RcvLcsRelReq_WaitForOutgoingMmConn: Invalid Transaction Id!");
        return;
    }

#if (FEATURE_ON == FEATURE_LTE)
    /* 当前存在LCS业务的CSFB流程，则清空CSFB存在标志 */
    if (NAS_MML_CSFB_SERVICE_STATUS_MO_NORMAL_LCS_EXIST == NAS_MML_GetCsfbServiceStatus())
    {
        NAS_MM_ProcCsfbServiceStatusChange(NAS_MML_GetCsfbServiceStatus(), NAS_MML_CSFB_SERVICE_STATUS_NOT_EXIST);
    }
#endif

    /* 更新CS域服务连接状态、缓存状态 */
    NAS_MM_UpdateCsServiceConnStatusFlg();
    NAS_MM_UpdateCsServiceBufferStatusFlg();

    return;
}


VOS_VOID NAS_MM_RcvLcsRelReq_MmConnectionActive(VOS_VOID* pMsg)
{
    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsRelReq_PreProc(pMsg))
    {
        return;
    }

    /* 请求Ti的MM链接是否存在，存在则清除该Ti的MM链接标志 */
    if (VOS_TRUE == NAS_MM_IsMmConnExist(MM_CONN_CTRL_LCS, NAS_MM_GetLcsRelReqTi()))
    {
        NAS_MM_UpdateMmConnByTi(MM_CONN_CTRL_LCS, NAS_MM_GetLcsRelReqTi(), NAS_MM_CONN_NOT_EXIST);
    }

    /* 当前MM链接都已经释放，则迁移MM状态为WAIT FOR NETWORK COMMAND */
    if (MM_TRUE != Mm_ComMmConnExist())
    {
        /* 记录返回到接收到建立MM连接请求时的状态 */
        NAS_MM_SetStaAfterWaitForNwkCmd(NAS_MM_GetStaOfRcvXXEstReq());

        /* 存储建链流程及原因值 */
        Mm_ComSaveProcAndCauseVal(MM_MM_CONN_PROC, NAS_MML_REG_FAIL_CAUSE_NULL);

        /* 记录迁移之前的状态 */
        NAS_MM_SetMmPreState(NAS_MM_GetMmState());

        if (RRMM_LCS_STATE_START == NAS_MM_GetRrLcsState())
        {
            /* 迁移到RR CONNECTION RELEASE NOT ALLOWED状态，启动T3241定时器 */
            Mm_ComSetMmState(MM_STATE_RR_CONNECTION_RELEASE_NOT_ALLOWED);
            NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_RcvLcsRelReq_MmConnectionActive: MM STATUS is MM_STATE_RR_CONNECTION_RELEASE_NOT_ALLOWED.");
            Mm_TimerStart(MM_TIMER_T3241);
        }
        else
        {
            /* 迁移到WAIT FOR NETWORK COMMAND状态，启动T3240定时器 */
            Mm_ComSetMmState(WAIT_FOR_NETWORK_COMMAND);
            NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_RcvLcsRelReq_MmConnectionActive: MM STATUS is WAIT_FOR_NETWORK_COMMAND.");
            Mm_TimerStart(MM_TIMER_T3240);
        }
    }

    /* 更新CS域服务连接状态、缓存状态 */
    NAS_MM_UpdateCsServiceConnStatusFlg();
    NAS_MM_UpdateCsServiceBufferStatusFlg();

    return;
}


VOS_VOID NAS_MM_RcvLcsRelReq_WaitForNetworkCmd(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM IDLE NO CELL AVAILABLE状态 */
    NAS_MM_RcvLcsRelReq_MmIdleNoCellAvaliable(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsRelReq_WaitForRrConn(VOS_VOID* pMsg)
{
    VOS_UINT8                           ucMmConnFlg;

    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsRelReq_PreProc(pMsg))
    {
        return;
    }

    if (g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_LCS].ucMMConnEstingTI == NAS_MM_GetLcsEstReqTi())
    {
        /* 停信令保护定时器 */
        Mm_TimerStop( MM_TIMER_PROTECT_SIGNALLING );

        /* 记录迁移之前的状态 */
        NAS_MM_SetMmPreState(NAS_MM_GetMmState());

        if (g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_LCS].ucRat == NAS_MML_GetCurrNetRatType())
        {
            /* 缓存请求的接入技术未发生变化 */
            /* 通知接入层释放链路 */
            Mm_SndRrRelReq(RRC_CELL_UNBARRED);

            /* 记录返回到接收到建立MM连接请求时的状态 */
            NAS_MM_SetStaAfterWaitForNwkCmd(NAS_MM_GetStaOfRcvXXEstReq());

            /* 存储建链流程及原因值 */
            Mm_ComSaveProcAndCauseVal(MM_MM_CONN_PROC, NAS_MML_REG_FAIL_CAUSE_NULL);

            ucMmConnFlg = Mm_ComMmConnExist();
            if ( (RRMM_LCS_STATE_START == NAS_MM_GetRrLcsState())
              && (MM_TRUE != ucMmConnFlg) )
            {
                /* 迁移到RR CONNECTION RELEASE NOT ALLOWED状态，启动T3241定时器 */
                Mm_ComSetMmState(MM_STATE_RR_CONNECTION_RELEASE_NOT_ALLOWED);
                NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_RcvLcsRelReq_WaitForRrConn: MM STATUS is MM_STATE_RR_CONNECTION_RELEASE_NOT_ALLOWED.");
                Mm_TimerStart(MM_TIMER_T3241);
            }
            else
            {
                /* 迁移到WAIT FOR NETWORK COMMAND状态，启动T3240定时器 */
                Mm_ComSetMmState(WAIT_FOR_NETWORK_COMMAND);
                NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_RcvLcsRelReq_WaitForRrConn: MM STATUS is WAIT_FOR_NETWORK_COMMAND.");
                Mm_TimerStart(MM_TIMER_T3240);
            }
        }
        else
        {
            /* 缓存请求的接入技术发生了变化，则直接迁移到收到建链请求前的状态 */
            Mm_ComSetMmState(NAS_MM_GetStaOfRcvXXEstReq());
        }

        /* 请求的Ti正在进行MM建链流程 */
        /* 清除MM建链标记及LCS建链请求缓存标志 */
        NAS_MM_ClearConnCtrlInfoWithBuffer(MM_CONN_CTRL_LCS);
    }
    else if (VOS_TRUE == NAS_MM_IsCsServiceBuffered(MM_CONN_CTRL_LCS, NAS_MM_GetLcsEstReqTi()))
    {
        /* 请求的Ti还在缓存中，则仅清除LCS建链请求缓存标志 */
        NAS_MM_ClearConnCtrlInfoWithBuffer(MM_CONN_CTRL_LCS);
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MM, "NAS_MM_RcvLcsRelReq_WaitForRrConn: Invalid Transaction Id!");
        return;
    }

#if (FEATURE_ON == FEATURE_LTE)
    /* 当前存在LCS业务的CSFB流程，则清空CSFB存在标志 */
    if (NAS_MML_CSFB_SERVICE_STATUS_MO_NORMAL_LCS_EXIST == NAS_MML_GetCsfbServiceStatus())
    {
        NAS_MM_ProcCsfbServiceStatusChange(NAS_MML_GetCsfbServiceStatus(), NAS_MML_CSFB_SERVICE_STATUS_NOT_EXIST);
    }
#endif

    /* 更新CS域服务连接状态、缓存状态 */
    NAS_MM_UpdateCsServiceConnStatusFlg();
    NAS_MM_UpdateCsServiceBufferStatusFlg();

    return;
}


VOS_VOID NAS_MM_RcvLcsRelReq_WaitForReestReq(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM IDLE NO CELL AVAILABLE状态 */
    NAS_MM_RcvLcsRelReq_MmIdleNoCellAvaliable(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsRelReq_WaitForReestCnf(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM IDLE NO CELL AVAILABLE状态 */
    NAS_MM_RcvLcsRelReq_MmIdleNoCellAvaliable(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsRelReq_WaitForAdditionalMmConn(VOS_VOID* pMsg)
{
    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsRelReq_PreProc(pMsg))
    {
        return;
    }

    /* 请求Ti的MM链接是否存在，存在则清除该Ti的MM链接标志 */
    if (VOS_TRUE == NAS_MM_IsMmConnExist(MM_CONN_CTRL_LCS, NAS_MM_GetLcsRelReqTi()))
    {
        NAS_MM_UpdateMmConnByTi(MM_CONN_CTRL_LCS, NAS_MM_GetLcsRelReqTi(), NAS_MM_CONN_NOT_EXIST);
    }

    if (MM_TRUE != Mm_ComMmConnExist())
    {
        /* 当前没有其他MM链接 */
        /* 记录迁移之前的状态，并迁移到WAIT FOR OUTGOING MM CONNECTION状态 */
        NAS_MM_SetMmPreState(NAS_MM_GetMmState());
        Mm_ComSetMmState(WAIT_FOR_OUTGOING_MM_CONNECT);
        NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_RcvLcsRelReq_WaitForAdditionalMmConn: MM STATUS is WAIT_FOR_OUTGOING_MM_CONNECT.");
    }
    else
    {
        /* 当前还有其他MM链接 */
        if (NAS_MM_GetConnCtrlInfo(MM_CONN_CTRL_LCS)->ucMMConnEstingTI == NAS_MM_GetLcsEstReqTi())
        {
            /* 请求的Ti正在进行MM建链流程 */
            /* 清除MM建链标记和清除缓存 */
            NAS_MM_ClearConnCtrlInfoWithBuffer(MM_CONN_CTRL_LCS);

            /* 停T3230定时器 */
            Mm_TimerStop(MM_TIMER_T3230);

            /* 记录迁移之前的状态，并迁移到MM CONNECTION ACTIVE状态 */
            NAS_MM_SetMmPreState(NAS_MM_GetMmState());
            Mm_ComSetMmState(MM_CONNECTION_ACTIVE);
            NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_RcvLcsRelReq_WaitForAdditionalMmConn: MM STATUS is MM_CONNECTION_ACTIVE.");
        }
        else if (VOS_TRUE == NAS_MM_IsCsServiceBuffered(MM_CONN_CTRL_LCS, NAS_MM_GetLcsRelReqTi()))
        {
            /* 请求的Ti为缓存的建链请求的Ti，则清除缓存 */
            NAS_MM_ClearConnCtrlInfoWithBuffer(MM_CONN_CTRL_LCS);
        }
        else
        {
            NAS_WARNING_LOG(WUEPS_PID_MM, "NAS_MM_RcvLcsRelReq_WaitForAdditionalMmConn: Invalid Transaction Id!");
            return;
        }
    }

    /* 更新CS域服务连接状态、缓存状态 */
    NAS_MM_UpdateCsServiceConnStatusFlg();
    NAS_MM_UpdateCsServiceBufferStatusFlg();

    return;
}


VOS_VOID NAS_MM_RcvLcsRelReq_LauPending(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM IDLE NO CELL AVAILABLE状态 */
    NAS_MM_RcvLcsRelReq_MmIdleNoCellAvaliable(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsRelReq_ImsiDetachPending(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM IDLE NO CELL AVAILABLE状态 */
    NAS_MM_RcvLcsRelReq_MmIdleNoCellAvaliable(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsRelReq_WaitForAttach(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM IDLE NO CELL AVAILABLE状态 */
    NAS_MM_RcvLcsRelReq_MmIdleNoCellAvaliable(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsRelReq_WaitForRrConnLau(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM IDLE NO CELL AVAILABLE状态 */
    NAS_MM_RcvLcsRelReq_MmIdleNoCellAvaliable(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsRelReq_LauInitiated(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM IDLE NO CELL AVAILABLE状态 */
    NAS_MM_RcvLcsRelReq_MmIdleNoCellAvaliable(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsRelReq_LauRejected(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM IDLE NO CELL AVAILABLE状态 */
    NAS_MM_RcvLcsRelReq_MmIdleNoCellAvaliable(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsRelReq_WaitForRrConnImsiDetach(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM IDLE NO CELL AVAILABLE状态 */
    NAS_MM_RcvLcsRelReq_MmIdleNoCellAvaliable(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsRelReq_ImsiDetachInitiated(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM IDLE NO CELL AVAILABLE状态 */
    NAS_MM_RcvLcsRelReq_MmIdleNoCellAvaliable(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsRelReq_ProcCmServicePrompt(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM IDLE NO CELL AVAILABLE状态 */
    NAS_MM_RcvLcsRelReq_MmIdleNoCellAvaliable(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsRelReq_RrConnRelNotAllow(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM IDLE NO CELL AVAILABLE状态 */
    NAS_MM_RcvLcsRelReq_MmIdleNoCellAvaliable(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvLcsRelReq_MmInterRatChange(VOS_VOID* pMsg)
{
    /* 消息头结构，用于判断消息类型 */
    MSG_HEADER_STRU                    *pstMsgHeader    = VOS_NULL_PTR;

    /* 提取消息头 */
    pstMsgHeader = (MSG_HEADER_STRU* )pMsg;

    /* 消息检查失败，直接返回 */
    if (VOS_TRUE != NAS_MM_RcvLcsRelReq_PreProc(pMsg))
    {
        return;
    }

    if (WAIT_FOR_OUTGOING_MM_CONNECTION == g_MmGlobalInfo.ucSuspendPreState)
    {
        /* 调用函数 MM_SaveMsg 进行处理 */
        MM_SaveMsg(pMsg, g_MmSubLyrShare.MmShare.MmSuspendEvent, pstMsgHeader->ulLength + VOS_MSG_HEAD_LENGTH);
        return;
    }

    if (VOS_TRUE == NAS_MM_IsCsServiceBuffered(MM_CONN_CTRL_LCS, NAS_MM_GetLcsRelReqTi()))
    {
        /* 请求的Ti为缓存的建链请求的Ti，则清除缓存 */
        NAS_MM_ClearConnCtrlInfoWithBuffer(MM_CONN_CTRL_LCS);


        NAS_MM_SndRrMmServiceAbortNotify(WUEPS_PID_WRR);
        NAS_MM_SndRrMmServiceAbortNotify(UEPS_PID_GAS);

#if (FEATURE_ON == FEATURE_LTE)
        /* 当前存在LCS业务的CSFB流程，则清空CSFB存在标志 */
        if (NAS_MML_CSFB_SERVICE_STATUS_MO_NORMAL_LCS_EXIST == NAS_MML_GetCsfbServiceStatus())
        {
            NAS_MM_ProcCsfbServiceStatusChange(NAS_MML_GetCsfbServiceStatus(), NAS_MML_CSFB_SERVICE_STATUS_NOT_EXIST);

            if (NAS_MML_NET_RAT_TYPE_LTE == NAS_MML_GetCurrNetRatType())
            {
                /* 当前接入技术为LTE，通知LMM终止CSFB流程 */
                NAS_MM_SndLmmCsfbServiceAbortNotify();
            }
        }
#endif
    }

    /* 更新CS域服务连接状态、缓存状态 */
    NAS_MM_UpdateCsServiceConnStatusFlg();
    NAS_MM_UpdateCsServiceBufferStatusFlg();

    return;
}


VOS_VOID NAS_MM_DealBufferedLcsEstReq(VOS_VOID)
{
    LCS_MM_EST_REQ_STRU                 stLcsEstReq;

    PS_MEM_SET(&stLcsEstReq, 0, sizeof(stLcsEstReq));

    NAS_MM_BuildLcsEstReqFromBuffer(&stLcsEstReq);
    NAS_MM_ClearConnCtrlInfoWithBuffer(MM_CONN_CTRL_LCS);

    NAS_INFO_LOG1(WUEPS_PID_MM, "NAS_MM_DealBufferedLcsEstReq: Buffered Lcs Est Request Ti:", stLcsEstReq.ulTi);

    /*lint -e961*/
    gaMmStateTable[MM_EVENT_ID_LCS_EST_REQ - 1][g_MmGlobalInfo.ucState](&stLcsEstReq);
    /*lint +e961*/

    return;
}


VOS_VOID NAS_MM_RcvRrDataIndLcs_WaitForOutgoingMmConn(VOS_VOID* pMsg)
{
    RRMM_DATA_IND_STRU      *pRrDataInd     = VOS_NULL_PTR;
    VOS_UINT8               *pucCmMsg       = VOS_NULL_PTR;
    VOS_UINT8                ucTi;

    pRrDataInd  = (RRMM_DATA_IND_STRU *)pMsg;
    pucCmMsg    = pRrDataInd->RcvNasMsg.aucNasMsg;

    /* 停T3212定时器 */
    Mm_TimerStop( MM_TIMER_T3212 );

    /* 从NAS空口消息中获取LCS业务TI */
    ucTi = (VOS_UINT8)((pucCmMsg[0] & 0xf0) >> 4);
    if ( MM_CONST_NUM_7 > ucTi )
    {
        /* 给LCS发送消息ID_MM_LCS_DATA_IND */
        NAS_MM_SndLcsDataInd(ucTi, pucCmMsg, pRrDataInd->RcvNasMsg.ulNasMsgSize);

        /* 网侧建立的MM连接 */
        if (VOS_TRUE != NAS_MM_IsMmConnExist(MM_CONN_CTRL_LCS, ucTi))
        {
            /* 网侧通知的第一条LCS空口消息，更新MM链接标识 */
            NAS_MM_UpdateMmConnByTi(MM_CONN_CTRL_LCS, ucTi, NAS_MM_CONN_EXIST);

            /* 记录迁移之前的状态 */
            NAS_MM_SetMmPreState(NAS_MM_GetMmState());

            /* 迁移状态到WAIT FOR ADDITIONAL OUTGOING MM CONNECTION */
            Mm_ComSetMmState(WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION);
            NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_RcvRrDataIndLcs_WaitForOutgoingMmConn: MM STATUS is WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION");
        }
    }
    else if ( ( MM_CONST_NUM_7 < ucTi ) && ( MM_CONST_NUM_15 > ucTi ) )
    {
        /* 给LCS发送消息ID_MM_LCS_DATA_IND */
        NAS_MM_SndLcsDataInd(ucTi, pucCmMsg, pRrDataInd->RcvNasMsg.ulNasMsgSize);

        /* LCS建立的MM连接 */
        if (VOS_TRUE != NAS_MM_IsMmConnExist(MM_CONN_CTRL_LCS, ucTi))
        {
            /* 网侧回复LCS的第一条消息，更新MM链接标识 */
            NAS_MM_UpdateMmConnByTi(MM_CONN_CTRL_LCS, ucTi, NAS_MM_CONN_EXIST);

            /* 停T3230 */
            Mm_TimerStop(MM_TIMER_T3230);

            /* 记录迁移之前的状态 */
            NAS_MM_SetMmPreState(NAS_MM_GetMmState());

            /* 迁移状态到MM CONNECTION ACTIVE */
            Mm_ComSetMmState(MM_CONNECTION_ACTIVE);
            NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_RcvRrDataIndLcs_WaitForOutgoingMmConn: MM STATUS is MM_CONNECTION_ACTIVE");

            /* 处理缓存的建链请求 */
            Mm_ComCheckDelayMmConn(MM_FALSE);
        }
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MM, "NAS_MM_RcvRrDataIndLcs_WaitForOutgoingMmConn: Invalid Network TransactionId!");
    }

    return;
}


VOS_VOID NAS_MM_RcvRrDataIndLcs_MmConnectionActive(VOS_VOID* pMsg)
{
    RRMM_DATA_IND_STRU      *pRrDataInd     = VOS_NULL_PTR;
    VOS_UINT8               *pucCmMsg       = VOS_NULL_PTR;
    VOS_UINT8                ucTi;

    pRrDataInd  = (RRMM_DATA_IND_STRU *)pMsg;
    pucCmMsg    = pRrDataInd->RcvNasMsg.aucNasMsg;

    /* 从NAS空口消息中获取LCS业务TI */
    ucTi = (VOS_UINT8)((pucCmMsg[0] & 0xf0) >> 4);

    if ( MM_CONST_NUM_7 > ucTi )
    {
        /* 给LCS发送消息ID_MM_LCS_DATA_IND */
        NAS_MM_SndLcsDataInd(ucTi, pucCmMsg, pRrDataInd->RcvNasMsg.ulNasMsgSize);

        /* 网侧建立的MM连接 */
        if (VOS_TRUE != NAS_MM_IsMmConnExist(MM_CONN_CTRL_LCS, ucTi))
        {
            /* 网侧通知的第一条LCS空口消息，更新MM链接标识 */
            NAS_MM_UpdateMmConnByTi(MM_CONN_CTRL_LCS, ucTi, NAS_MM_CONN_EXIST);
        }
    }
    else if ( ( MM_CONST_NUM_7 < ucTi ) && ( MM_CONST_NUM_15 > ucTi ) )
    {
        /* 给LCS发送消息ID_MM_LCS_DATA_IND */
        NAS_MM_SndLcsDataInd(ucTi, pucCmMsg, pRrDataInd->RcvNasMsg.ulNasMsgSize);

        /* LCS建立的MM连接 */
        if (VOS_TRUE != NAS_MM_IsMmConnExist(MM_CONN_CTRL_LCS, ucTi))
        {
            /* 网侧回复LCS的第一条消息，更新MM链接标识 */
            NAS_MM_UpdateMmConnByTi(MM_CONN_CTRL_LCS, ucTi, NAS_MM_CONN_EXIST);

            /* 处理缓存的建链请求 */
            Mm_ComCheckDelayMmConn(MM_FALSE);
        }
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MM, "NAS_MM_RcvRrDataIndLcs_MmConnectionActive: Invalid Network TransactionId!");
    }

    /* 更新CS域服务连接状态 */
    NAS_MM_UpdateCsServiceConnStatusFlg();

    return;
}


VOS_VOID NAS_MM_RcvRrDataIndLcs_WaitForNetworkCmd(VOS_VOID* pMsg)
{
    RRMM_DATA_IND_STRU      *pRrDataInd     = VOS_NULL_PTR;
    VOS_UINT8               *pucCmMsg       = VOS_NULL_PTR;
    VOS_UINT8                ucTi;

    pRrDataInd  = (RRMM_DATA_IND_STRU *)pMsg;
    pucCmMsg    = pRrDataInd->RcvNasMsg.aucNasMsg;

    /* 从NAS空口消息中获取LCS业务TI */
    ucTi = (VOS_UINT8)((pucCmMsg[0] & 0xf0) >> 4);

    if ( MM_CONST_NUM_7 > ucTi )
    {
        /* 给LCS发送消息ID_MM_LCS_DATA_IND */
        NAS_MM_SndLcsDataInd(ucTi, pucCmMsg, pRrDataInd->RcvNasMsg.ulNasMsgSize);

        /* 网侧通知的第一条LCS空口消息，更新MM链接标识 */
        NAS_MM_UpdateMmConnByTi(MM_CONN_CTRL_LCS, ucTi, NAS_MM_CONN_EXIST);

        /* 停T3240定时器 */
        Mm_TimerStop(MM_TIMER_T3240);
        
        NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_RcvRrDataIndLcs_WaitForNetworkCmd: Call NAS_MM_ResetCsPagingFlgAndT3240Len()");
        NAS_MM_ResetCsPagingFlgAndT3240Len();

        /* 记录迁移之前的状态 */
        NAS_MM_SetMmPreState(NAS_MM_GetMmState());

        /* 迁移状态到MM CONNECTION ACTIVE */
        Mm_ComSetMmState(MM_CONNECTION_ACTIVE);
        NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_RcvRrDataIndLcs_WaitForNetworkCmd: MM STATUS is MM_CONNECTION_ACTIVE");

        /* 处理缓存的建链请求 */
        Mm_ComCheckDelayMmConn(MM_FALSE);
    }

    else if ( ( MM_CONST_NUM_7 < ucTi ) && ( MM_CONST_NUM_15 > ucTi ) )
    {
        /* 给LCS发送消息ID_MM_LCS_DATA_IND */
        NAS_MM_SndLcsDataInd(ucTi, pucCmMsg, pRrDataInd->RcvNasMsg.ulNasMsgSize);

        /* 网侧回复LCS的第一条消息，更新MM链接标识 */
        NAS_MM_UpdateMmConnByTi(MM_CONN_CTRL_LCS, ucTi, NAS_MM_CONN_EXIST);

        /* 停T3240定时器 */
        Mm_TimerStop(MM_TIMER_T3240);

        NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_RcvRrDataIndLcs_WaitForNetworkCmd: Call NAS_MM_ResetCsPagingFlgAndT3240Len()");
        NAS_MM_ResetCsPagingFlgAndT3240Len();

        /* 记录迁移之前的状态 */
        NAS_MM_SetMmPreState(NAS_MM_GetMmState());

        /* 迁移状态到MM CONNECTION ACTIVE */
        Mm_ComSetMmState(MM_CONNECTION_ACTIVE);
        NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_RcvRrDataIndLcs_WaitForNetworkCmd: MM STATUS is MM_CONNECTION_ACTIVE");

        /* 处理缓存的建链请求 */
        Mm_ComCheckDelayMmConn(MM_FALSE);
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MM, "NAS_MM_RcvRrDataIndLcs_WaitForNetworkCmd: Invalid Network TransactionId!");
    }

    /* 更新CS域服务连接状态 */
    NAS_MM_UpdateCsServiceConnStatusFlg();

    return;
}


VOS_VOID NAS_MM_RcvRrDataIndLcs_WaitForAdditionalMmConn(VOS_VOID* pMsg)
{
    RRMM_DATA_IND_STRU      *pRrDataInd     = VOS_NULL_PTR;
    VOS_UINT8               *pucCmMsg       = VOS_NULL_PTR;
    VOS_UINT8                ucTi;

    pRrDataInd  = (RRMM_DATA_IND_STRU *)pMsg;
    pucCmMsg    = pRrDataInd->RcvNasMsg.aucNasMsg;

    /* 从NAS空口消息中获取LCS业务TI */
    ucTi = (VOS_UINT8)((pucCmMsg[0] & 0xf0) >> 4);
    if ( MM_CONST_NUM_7 > ucTi )
    {
        /* 给LCS发送消息ID_MM_LCS_DATA_IND */
        NAS_MM_SndLcsDataInd(ucTi, pucCmMsg, pRrDataInd->RcvNasMsg.ulNasMsgSize);

        /* 网侧建立的MM连接 */
        if (VOS_TRUE != NAS_MM_IsMmConnExist(MM_CONN_CTRL_LCS, ucTi))
        {
            /* 网侧通知的第一条LCS空口消息，更新MM链接标识 */
            NAS_MM_UpdateMmConnByTi(MM_CONN_CTRL_LCS, ucTi, NAS_MM_CONN_EXIST);
        }
    }
    else if ( ( MM_CONST_NUM_7 < ucTi ) && ( MM_CONST_NUM_15 > ucTi ) )
    {
        /* 给LCS发送消息ID_MM_LCS_DATA_IND */
        NAS_MM_SndLcsDataInd(ucTi, pucCmMsg, pRrDataInd->RcvNasMsg.ulNasMsgSize);

        /* LCS建立的MM连接 */
        if (VOS_TRUE != NAS_MM_IsMmConnExist(MM_CONN_CTRL_LCS, ucTi))
        {
            /* 网侧回复LCS的第一条消息，更新MM链接标识 */
            NAS_MM_UpdateMmConnByTi(MM_CONN_CTRL_LCS, ucTi, NAS_MM_CONN_EXIST);

            /* 停T3230 */
            Mm_TimerStop(MM_TIMER_T3230);

            /* 记录迁移之前的状态 */
            NAS_MM_SetMmPreState(NAS_MM_GetMmState());

            /* 迁移状态到MM CONNECTION ACTIVE */
            Mm_ComSetMmState(MM_CONNECTION_ACTIVE);
            NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_RcvRrDataIndLcs_WaitForAdditionalMmConn: MM STATUS is MM_CONNECTION_ACTIVE");

            /* 处理缓存的建链请求 */
            Mm_ComCheckDelayMmConn(MM_FALSE);
        }
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MM, "NAS_MM_RcvRrDataIndLcs_WaitForAdditionalMmConn: Invalid Network TransactionId!");
    }

    return;
}


VOS_VOID NAS_MM_RcvRrDataIndLcs_RrConnRelNotAllow(VOS_VOID* pMsg)
{
    RRMM_DATA_IND_STRU      *pstRrDataInd   = VOS_NULL_PTR;
    VOS_UINT8               *pucCmMsg       = VOS_NULL_PTR;
    VOS_UINT8                ucTi;

    pstRrDataInd    = (RRMM_DATA_IND_STRU *)pMsg;
    pucCmMsg        = pstRrDataInd->RcvNasMsg.aucNasMsg;

    /* 从NAS空口消息中获取LCS业务TI */
    ucTi = (VOS_UINT8)((pucCmMsg[0] & 0xf0) >> 4);

    if ( MM_CONST_NUM_7 > ucTi )
    {
        /* 给LCS发送消息ID_MM_LCS_DATA_IND */
        NAS_MM_SndLcsDataInd(ucTi, pucCmMsg, pstRrDataInd->RcvNasMsg.ulNasMsgSize);

        /* 网侧通知的第一条LCS空口消息，更新MM链接标识 */
        NAS_MM_UpdateMmConnByTi(MM_CONN_CTRL_LCS, ucTi, NAS_MM_CONN_EXIST);

        /* 停T3240定时器 */
        Mm_TimerStop(MM_TIMER_T3241);

        /* 记录迁移之前的状态 */
        NAS_MM_SetMmPreState(NAS_MM_GetMmState());

        /* 迁移状态到MM CONNECTION ACTIVE */
        Mm_ComSetMmState(MM_CONNECTION_ACTIVE);
        NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_RcvRrDataIndLcs_RrConnRelNotAllow: MM STATUS is MM_CONNECTION_ACTIVE");

        /* 处理缓存的建链请求 */
        Mm_ComCheckDelayMmConn(MM_FALSE);
    }

    else if ( ( MM_CONST_NUM_7 < ucTi ) && ( MM_CONST_NUM_15 > ucTi ) )
    {
        /* 给LCS发送消息ID_MM_LCS_DATA_IND */
        NAS_MM_SndLcsDataInd(ucTi, pucCmMsg, pstRrDataInd->RcvNasMsg.ulNasMsgSize);

        /* 网侧回复LCS的第一条消息，更新MM链接标识 */
        NAS_MM_UpdateMmConnByTi(MM_CONN_CTRL_LCS, ucTi, NAS_MM_CONN_EXIST);

        /* 停T3240定时器 */
        Mm_TimerStop(MM_TIMER_T3241);

        /* 记录迁移之前的状态 */
        NAS_MM_SetMmPreState(NAS_MM_GetMmState());

        /* 迁移状态到MM CONNECTION ACTIVE */
        Mm_ComSetMmState(MM_CONNECTION_ACTIVE);
        NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_RcvRrDataIndLcs_RrConnRelNotAllow: MM STATUS is MM_CONNECTION_ACTIVE");

        /* 处理缓存的建链请求 */
        Mm_ComCheckDelayMmConn(MM_FALSE);
    }
    else
    {
        NAS_WARNING_LOG(WUEPS_PID_MM, "NAS_MM_RcvRrDataIndLcs_RrConnRelNotAllow: Invalid Network TransactionId!");
    }

    /* 更新CS域服务连接状态 */
    NAS_MM_UpdateCsServiceConnStatusFlg();

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_Null(VOS_VOID* pMsg)
{
    RRMM_LCS_STATE_IND_STRU            *pstLcsStateInd = VOS_NULL_PTR;
    RRMM_LCS_STATE_ENUM_UINT8           enLcsNewState;

    pstLcsStateInd = (RRMM_LCS_STATE_IND_STRU *)pMsg;
    enLcsNewState  = pstLcsStateInd->enState;

    if (enLcsNewState >= RRMM_LCS_STATE_BUTT)
    {
        enLcsNewState = RRMM_LCS_STATE_FINISH;
    }

    if  (RRMM_LCS_STATE_FINISH != enLcsNewState)
    {
        NAS_WARNING_LOG1(WUEPS_PID_MM, "NAS_MM_RcvRrLcsStateInd_Null: RR Lcs State:", NAS_MM_GetRrLcsState());
        enLcsNewState = RRMM_LCS_STATE_FINISH;
    }

    NAS_MM_SetRrLcsState(enLcsNewState);
    NAS_NORMAL_LOG1(WUEPS_PID_MM, "NAS_MM_RcvRrLcsStateInd_Null: RR Lcs State:", enLcsNewState);

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_MmIdleNoCellAvaliable(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM NULL状态 */
    NAS_MM_RcvRrLcsStateInd_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_MmIdlePlmnSearch(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM NULL状态 */
    NAS_MM_RcvRrLcsStateInd_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_MmIdleNormalService(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM NULL状态 */
    NAS_MM_RcvRrLcsStateInd_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_MmIdleLimitedService(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM NULL状态 */
    NAS_MM_RcvRrLcsStateInd_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_MmIdleAttempToUpdate(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM NULL状态 */
    NAS_MM_RcvRrLcsStateInd_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_MmIdleLauNeeded(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM NULL状态 */
    NAS_MM_RcvRrLcsStateInd_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_MmIdleNoImsi(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM NULL状态 */
    NAS_MM_RcvRrLcsStateInd_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_WaitForOutgoingMmConn(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM NULL状态 */
    NAS_MM_RcvRrLcsStateInd_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_MmConnectionActive(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM NULL状态 */
    NAS_MM_RcvRrLcsStateInd_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_WaitForNetworkCmd(VOS_VOID* pMsg)
{
    RRMM_LCS_STATE_IND_STRU            *pstLcsStateInd = VOS_NULL_PTR;
    RRMM_LCS_STATE_ENUM_UINT8           enLcsNewState;

    pstLcsStateInd  = (RRMM_LCS_STATE_IND_STRU *)pMsg;
    enLcsNewState   = pstLcsStateInd->enState;

    /* 异常保护 */
    if (enLcsNewState >= RRMM_LCS_STATE_BUTT)
    {
        /* 接入层LCS状态异常 */
        NAS_WARNING_LOG1(WUEPS_PID_MM, "NAS_MM_RcvRrLcsStateInd_WaitForNetworkCmd: RR LCS State is Abnormal.", enLcsNewState);

        enLcsNewState = RRMM_LCS_STATE_FINISH;
    }

    /* 更新全局变量 */
    NAS_MM_SetRrLcsState(enLcsNewState);

    /* LCS流程状态为FINISH，则直接返回 */
    if (RRMM_LCS_STATE_FINISH == enLcsNewState)
    {
        return;
    }

    /* 存在MM链接，则直接返回 */
    if (MM_TRUE == Mm_ComMmConnExist())
    {
        return;
    }

    /* 接入层LCS状态变为START，则切换MM状态为RR CONNECTION RELEASE NOT ALLOWED状态 */
    NAS_NORMAL_LOG1(WUEPS_PID_MM, "NAS_MM_RcvRrLcsStateInd_WaitForNetworkCmd: RR Lcs State:", RRMM_LCS_STATE_START);

    /* 停止T3240定时器 */
    Mm_TimerStop(MM_TIMER_T3240);

    /* 记录迁移之前的状态 */
    NAS_MM_SetMmPreState(NAS_MM_GetMmState());

    /* 迁移状态到RR CONNECTION RELEASE NOT ALLOWED */
    Mm_ComSetMmState(MM_STATE_RR_CONNECTION_RELEASE_NOT_ALLOWED);
    NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_RcvRrLcsStateInd_WaitForNetworkCmd: MM STATUS is MM_STATE_RR_CONNECTION_RELEASE_NOT_ALLOWED");

    /* 启动T3241定时器 */
    Mm_TimerStart(MM_TIMER_T3241);

    /* 处理缓存的建链请求 */
    Mm_DealWithBuffMsg();

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_WaitForRrConn(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM NULL状态 */
    NAS_MM_RcvRrLcsStateInd_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_WaitForReestReq(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM NULL状态 */
    NAS_MM_RcvRrLcsStateInd_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_WaitForReestCnf(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM NULL状态 */
    NAS_MM_RcvRrLcsStateInd_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_WaitForAdditionalMmConn(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM NULL状态 */
    NAS_MM_RcvRrLcsStateInd_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_LauPending(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM NULL状态 */
    NAS_MM_RcvRrLcsStateInd_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_ImsiDetachPending(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM NULL状态 */
    NAS_MM_RcvRrLcsStateInd_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_WaitForAttach(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM NULL状态 */
    NAS_MM_RcvRrLcsStateInd_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_WaitForRrConnLau(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM NULL状态 */
    NAS_MM_RcvRrLcsStateInd_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_LauInitiated(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM NULL状态 */
    NAS_MM_RcvRrLcsStateInd_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_LauRejected(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM NULL状态 */
    NAS_MM_RcvRrLcsStateInd_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_WaitForRrConnImsiDetach(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM NULL状态 */
    NAS_MM_RcvRrLcsStateInd_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_ImsiDetachInitiated(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM NULL状态 */
    NAS_MM_RcvRrLcsStateInd_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_ProcCmServicePrompt(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM NULL状态 */
    NAS_MM_RcvRrLcsStateInd_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_RrConnRelNotAllow(VOS_VOID* pMsg)
{
    RRMM_LCS_STATE_IND_STRU            *pstLcsStateInd = VOS_NULL_PTR;
    RRMM_LCS_STATE_ENUM_UINT8           enLcsNewState;

    pstLcsStateInd  = (RRMM_LCS_STATE_IND_STRU *)pMsg;
    enLcsNewState   = pstLcsStateInd->enState;

    /* 异常保护 */
    if (enLcsNewState >= RRMM_LCS_STATE_BUTT)
    {
        /* 接入层LCS状态异常 */
        NAS_WARNING_LOG1(WUEPS_PID_MM, "NAS_MM_RcvRrLcsStateInd_RrConnRelNotAllow: RR LCS State is Abnormal.", enLcsNewState);

        enLcsNewState = RRMM_LCS_STATE_FINISH;
    }

    /* LCS流程状态为START，则直接返回 */
    if (RRMM_LCS_STATE_START == enLcsNewState)
    {
        return;
    }

    /* 接入层LCS状态变为为START，则切换MM状态为RR CONNECTION RELEASE NOT ALLOWED状态 */
    NAS_MM_SetRrLcsState(RRMM_LCS_STATE_FINISH);
    NAS_NORMAL_LOG1(WUEPS_PID_MM, "NAS_MM_RcvRrLcsStateInd_RrConnRelNotAllow: RR Lcs State:", RRMM_LCS_STATE_FINISH);

    /* 停止T3241定时器 */
    Mm_TimerStop(MM_TIMER_T3241);

    /* 记录迁移之前的状态 */
    NAS_MM_SetMmPreState(NAS_MM_GetMmState());

    /* 迁移状态到WAIT FOR NETWORK COMMAND */
    Mm_ComSetMmState(WAIT_FOR_NETWORK_COMMAND);
    NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_RcvRrLcsStateInd_WaitForNetworkCmd: MM STATUS is WAIT_FOR_NETWORK_COMMAND");

    /* 启动T3240定时器 */
    Mm_TimerStart(MM_TIMER_T3240);

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_TestContrlActive(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM NULL状态 */
    NAS_MM_RcvRrLcsStateInd_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_MmInterRatChange(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM NULL状态 */
    NAS_MM_RcvRrLcsStateInd_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvRrLcsStateInd_MmIdleECallInactive(VOS_VOID* pMsg)
{
    /* 此状态下处理同MM NULL状态 */
    NAS_MM_RcvRrLcsStateInd_Null(pMsg);

    return;
}


VOS_VOID NAS_MM_RcvT3241Expired_PreProc(VOS_VOID)
{
    /* 停止T3241定时器 */
    Mm_TimerStop(MM_TIMER_T3241);
    return;
}


VOS_VOID NAS_MM_RcvT3241Expired_RrConnRelNotAllow(VOS_VOID* pMsg)
{
    /* 更新接入层LCS流程状态为FINISH */
    NAS_MM_SetRrLcsState(RRMM_LCS_STATE_FINISH);

    /* 通知接入层释放RR链接 */
    Mm_SndRrRelReq(RRC_CELL_UNBARRED);

    /* 记录迁移之前的状态 */
    NAS_MM_SetMmPreState(NAS_MM_GetMmState());

    /* 迁移状态到WAIT FOR NETWORK COMMAND */
    Mm_ComSetMmState(WAIT_FOR_NETWORK_COMMAND);
    NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_RcvT3241Expired_RrConnRelNotAllow: MM STATUS is WAIT_FOR_NETWORK_COMMAND");

    /* 启动T3240定时器 */
    Mm_TimerStart(MM_TIMER_T3240);

    return;
}


VOS_VOID NAS_MM_RcvLcsStatusNty(VOS_VOID* pMsg)
{
    LCS_MM_STATUS_NTY_STRU             *pstLcsStatusNty = VOS_NULL_PTR;
    RRMM_TRANSACTION_STATUS_ENUM_UINT8  enTransStatus;
    NAS_MML_MO_LCS_STATUS_ENUM_UINT8    enOldLcsMoStatus;

    pstLcsStatusNty     = (LCS_MM_STATUS_NTY_STRU*)pMsg;
    enOldLcsMoStatus    = NAS_MML_GetMoLcsStatus();
    enTransStatus       = RRMM_TRANSACTION_FAIL;

    /* 消息有效性检查 */
    if ( (NAS_LCS_STATUS_SETUP_SUCC != pstLcsStatusNty->enLcsStatus)
      && (NAS_LCS_STATUS_SETUP_FAIL != pstLcsStatusNty->enLcsStatus) )
    {
        return;
    }

    /* 有业务建立标志时，通知AS，链接释放时可以FR */
    if (VOS_TRUE == NAS_MML_GetMoLcsSetupFlg())
    {
        enTransStatus = NAS_MM_ConvertLcsStatusToTransactionStatus(pstLcsStatusNty->enLcsStatus);
        NAS_MM_SndTransactionStatusNotify(enTransStatus);
    }

    /* 如果MMC正在搜网时业务结束，通知MMC停止搜GU，重新从LTE开始搜网;如果MMC处于ON PLMN启高优先级接入技术BG搜定时器 */
    if ((NAS_MML_CSFB_MO_LCS_SETUP_START == enOldLcsMoStatus)
     && (NAS_LCS_STATUS_SETUP_FAIL == pstLcsStatusNty->enLcsStatus))
    {
        NAS_MM_SndMmcPlmnSearchInd(MM_MMC_PLMN_SEARCH_TYPE_CSFB_END);
    }

    /* 更新标志 */
    NAS_MM_UpdateLcsStatus(pstLcsStatusNty->enLcsStatus);

    return;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

