

/*****************************************************************************
  1 头文件包含
*****************************************************************************/


#include "NasComm.h"

#include "NasUtranCtrlCtx.h"
#include "NasUtranCtrlCommFunc.h"
#include "NasUtranCtrlFsmMain.h"
#include "Nasrrcinterface.h"
#include "NasUtranCtrlProcNvim.h"
#include "NasMmcTimerMgmt.h"
#include "NasMmlLib.h"
#include "MsccMmcInterface.h"
#include "NasMmcComFunc.h"
#include "NVIM_Interface.h"
#include "NasMmcSndLmm.h"
#include "siappstk.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 常量定义
*****************************************************************************/

#define    THIS_FILE_ID        PS_FILE_ID_NAS_UTRANCTRL_FSM_MAIN_C

/*****************************************************************************
  3 类型定义
*****************************************************************************/

/*lint -save -e958 */

/*****************************************************************************
  4 函数声明
*****************************************************************************/

/*****************************************************************************
  5 变量定义
*****************************************************************************/

/*****************************************************************************
  3 函数实现
*****************************************************************************/
#if (FEATURE_ON == FEATURE_UE_MODE_TDS)

#if defined (__PS_WIN32_RECUR__)


VOS_VOID NAS_UTRANCTRL_ReloadFsmDesc(
    NAS_UTRANCTRL_FSM_CTX_STRU         *pstFsmCtx,
    VOS_UINT32                          ulPcRecurFsmId
)
{
    switch ( ulPcRecurFsmId )
    {
        case NAS_UTRANCTRL_FSM_MAIN:
            pstFsmCtx->pstFsmDesc       = NAS_UTRANCTRL_GetMainFsmDescAddr();
            break;

        case NAS_UTRANCTRL_FSM_SWITCH_ON:
            pstFsmCtx->pstFsmDesc       = NAS_UTRANCTRL_GetSwitchOnFsmDescAddr();
            break;

        case NAS_UTRANCTRL_FSM_POWER_OFF:
            pstFsmCtx->pstFsmDesc       = NAS_UTRANCTRL_GetPowerOffFsmDescAddr();
            break;

        case NAS_UTRANCTRL_FSM_PLMN_SELECTION:
            pstFsmCtx->pstFsmDesc       = NAS_UTRANCTRL_GetPlmnSelectionFsmDescAddr();
            break;

        case NAS_UTRANCTRL_FSM_SYSCFG:
            pstFsmCtx->pstFsmDesc       = NAS_UTRANCTRL_GetSysCfgFsmDescAddr();
            break;

        case NAS_UTRANCTRL_FSM_MODE_CHANGE:
            pstFsmCtx->pstFsmDesc       = NAS_UTRANCTRL_GetModeChangeFsmDescAddr();
            break;

        default:
            break;
    }

}



VOS_UINT32 NAS_UTRANCTRL_RestoreContextData_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_UTRANCTRL_CTX_STRU                                  *pstUtranCtrlCtx;
    NAS_MML_PC_REPLAY_COMPRESS_CONTEXT_STRU                 *pstRcMsg;
    VOS_UINT32                                               ulExpectCount;
    VOS_UINT32                                               ulCount;
    VOS_UINT32                                               ulItemCount;
    VOS_UINT8                                               *pucDest;
    VOS_UINT8                                               *pucSrc;
    VOS_UINT32                                               ulDestLen;

    pstUtranCtrlCtx     = NAS_UTRANCTRL_GetUtranCtrlCtx();

    ulExpectCount = 0;
    ulCount       = 0;
    ulItemCount   = 0;
    ulExpectCount = sizeof(NAS_UTRANCTRL_CTX_STRU);

    /*注册状态机*/
    NAS_UTRANCTRL_RegFsm();

    pstRcMsg = (NAS_MML_PC_REPLAY_COMPRESS_CONTEXT_STRU *)pstMsg;
    pucSrc   = pstRcMsg->aucData;

    /* 解析总长度 */
    PS_MEM_CPY(&ulCount, pucSrc, sizeof(VOS_UINT32));
    pucSrc += sizeof(VOS_UINT32);

    if( ulExpectCount != ulCount )
    {
        return VOS_FALSE;
    }

    /* g_stNasUtranCtrlCtx目标地址 */
    pucDest     = (VOS_UINT8 *)pstUtranCtrlCtx;
    ulDestLen   = sizeof(NAS_UTRANCTRL_CTX_STRU);

    /* 解压缩 */
    if (VOS_FALSE == NAS_MML_UnCompressData(pucDest, &ulDestLen, pucSrc, &ulItemCount) )
    {
        return VOS_FALSE;
    }

    pucSrc += (ulItemCount);

    NAS_UTRANCTRL_ReloadFsmDesc(NAS_UTRANCTRL_GetCurFsmAddr(), pstUtranCtrlCtx->stCurFsm.enFsmId);


    NAS_INFO_LOG(WUEPS_PID_MMC, "UTRANCTRL: NAS_UTRANCTRL_RestoreContextData_Main - data is restored.");

    return VOS_TRUE;
}

#endif  /* END OF #if defined (__PS_WIN32_RECUR__) */




VOS_UINT32 NAS_UTRANCTRL_RcvWasSysInfo_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    RRMM_SYS_INFO_IND_STRU             *pstRrMmSysInfoInd;
    NAS_UTRANCTRL_UTRAN_MODE_ENUM_UINT8 enPreUtranMode;

    enPreUtranMode     = NAS_UTRANCTRL_GetPreUtranMode();
    pstRrMmSysInfoInd  = (RRMM_SYS_INFO_IND_STRU *)pstMsg;

    /* 自动模式收到WAS消息,统一切换成FDD模式,写入NV中 */
    if (NAS_UTRANCTRL_UTRAN_SWITCH_MODE_AUTO == NAS_UTRANCTRL_GetCurUtranSwitchMode())
    {
        /* 如果当前的MCC不在TD的PLMN LIST列表中,UTRAN MODE为FDD */
        if (VOS_FALSE == NAS_MML_IsMccInDestMccList(pstRrMmSysInfoInd->PlmnId.ulMcc,
                                                    NAS_UTRANCTRL_GetSpecTdMccListNum(),
                                                    NAS_UTRANCTRL_GetSpecTdMccList()))
        {
            NAS_UTRANCTRL_SetRoamingZone(NAS_UTRANCTRL_ROAM_ZONE_OUTSIDE_CHINA_MAINLAND);
        }
        else
        {
            NAS_UTRANCTRL_SetSearchedSpecTdMccFlg(VOS_TRUE);
            NAS_UTRANCTRL_SetRoamingZone(NAS_UTRANCTRL_ROAM_ZONE_CHINA_MAINLAND);
        }
        /* 更新当前的工作模式 */
        NAS_UTRANCTRL_SetCurrUtranMode(NAS_UTRANCTRL_UTRAN_MODE_FDD);
    }

    /* 模式不变不通知RRC */
    if (enPreUtranMode == NAS_UTRANCTRL_GetCurrUtranMode())
    {
        /* 消息未被处理完成需继续进入MMC状态机处理 */
        return VOS_FALSE;
    }

    /* 把当前UtranMode写到NV中 */
    NAS_UTRANCTRL_WriteUtranModeNvim(NAS_UTRANCTRL_UTRAN_MODE_FDD);

    /* 更新上次发给接入层的UtranMode */
    NAS_UTRANCTRL_SetPreUtranMode(NAS_UTRANCTRL_GetCurrUtranMode());

    /* 进入ModeChange状态机 */
    NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_MODE_CHANGE);

    return VOS_TRUE;
}


VOS_UINT32 NAS_UTRANCTRL_RcvGasSysInfo_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    GRRMM_SYS_INFO_IND_ST              *pstGrrSysInfo;
    NAS_UTRANCTRL_UTRAN_MODE_ENUM_UINT8 enPreUtranMode;
    NAS_UTRANCTRL_UTRAN_MODE_ENUM_UINT8 enCurrUtranMode;

    pstGrrSysInfo      = (GRRMM_SYS_INFO_IND_ST *)pstMsg;
    enPreUtranMode     = NAS_UTRANCTRL_GetPreUtranMode();

    /* 自动模式收到GAS消息，解析系统消息中的MCC，更新当前模式并写NV */
    if (NAS_UTRANCTRL_UTRAN_SWITCH_MODE_AUTO == NAS_UTRANCTRL_GetCurUtranSwitchMode())
    {
        /* 如果当前的MCC不在TD的PLMN LIST列表中，UTRAN MODE为FDD */
        if (VOS_FALSE == NAS_MML_IsMccInDestMccList(pstGrrSysInfo->PlmnId.ulMcc,
                                                NAS_UTRANCTRL_GetSpecTdMccListNum(),
                                                NAS_UTRANCTRL_GetSpecTdMccList()))
        {
            enCurrUtranMode = NAS_UTRANCTRL_UTRAN_MODE_FDD;
            NAS_UTRANCTRL_SetRoamingZone(NAS_UTRANCTRL_ROAM_ZONE_OUTSIDE_CHINA_MAINLAND);
        }
        else
        {
            enCurrUtranMode = NAS_UTRANCTRL_UTRAN_MODE_TDD;

            NAS_UTRANCTRL_SetRoamingZone(NAS_UTRANCTRL_ROAM_ZONE_CHINA_MAINLAND);
            NAS_UTRANCTRL_SetSearchedSpecTdMccFlg(VOS_TRUE);
        }
        NAS_UTRANCTRL_SetCurrUtranMode(enCurrUtranMode);
    }

    /* 模式不变不通知RRC */
    if (enPreUtranMode == NAS_UTRANCTRL_GetCurrUtranMode())
    {
        /* 消息未被处理完成需继续进入MMC状态机处理 */
        return VOS_FALSE;
    }

    /* 把当前UtranMode写到NV中 */
    NAS_UTRANCTRL_WriteUtranModeNvim(NAS_UTRANCTRL_GetCurrUtranMode());

    /* 更新上次发给接入层的UtranMode */
    NAS_UTRANCTRL_SetPreUtranMode(NAS_UTRANCTRL_GetCurrUtranMode());

    /* 进入ModeChange状态机 */
    NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_MODE_CHANGE);

    return VOS_TRUE;
}



VOS_UINT32 NAS_UTRANCTRL_RcvTdSysInfo_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_UTRANCTRL_UTRAN_MODE_ENUM_UINT8 enPreUtranMode;

    enPreUtranMode   = NAS_UTRANCTRL_GetPreUtranMode();

    /* 自动模式收到TDS消息，可能是FDD  模式切换成TDD模式,需要更新当前模式 */
    if (NAS_UTRANCTRL_UTRAN_SWITCH_MODE_AUTO == NAS_UTRANCTRL_GetCurUtranSwitchMode())
    {
        NAS_UTRANCTRL_SetCurrUtranMode(NAS_UTRANCTRL_UTRAN_MODE_TDD);

        NAS_UTRANCTRL_SetSearchedSpecTdMccFlg(VOS_TRUE);
        NAS_UTRANCTRL_SetRoamingZone(NAS_UTRANCTRL_ROAM_ZONE_CHINA_MAINLAND);
    }

    /* 模式不变不通知RRC */
    if (enPreUtranMode == NAS_UTRANCTRL_GetCurrUtranMode())
    {
        NAS_NORMAL_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTdSysInfo_Main(): UTRANMODE not change");
        /* 消息未被处理完成需继续处理 */
        return VOS_FALSE;
    }

    NAS_NORMAL_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTdSysInfo_Main(): Write NV as TDD and enter NAS_UTRANCTRL_FSM_MODE_CHANGE");

    /* 把当前UtranMode写到NV中 */
    NAS_UTRANCTRL_WriteUtranModeNvim(NAS_UTRANCTRL_UTRAN_MODE_TDD);

    /* 更新上次发给接入层的UtranMode */
    NAS_UTRANCTRL_SetPreUtranMode(NAS_UTRANCTRL_GetCurrUtranMode());

    /* 进入ModeChange状态机 */
    NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_MODE_CHANGE);

    return VOS_TRUE;
}


#if (FEATURE_ON == FEATURE_LTE)

VOS_UINT32 NAS_UTRANCTRL_RcvLmmSysInfo_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    LMM_MMC_SYS_INFO_IND_STRU          *pstLmmSysInfoMsg;
    NAS_UTRANCTRL_UTRAN_MODE_ENUM_UINT8 enPreUtranMode;
    NAS_UTRANCTRL_UTRAN_MODE_ENUM_UINT8 enCurrUtranMode;
    MMC_LMM_PLMN_ID_STRU                stLmmPlmn;
    NAS_MML_PLMN_ID_STRU                stGUNasPlmn;

    pstLmmSysInfoMsg = (LMM_MMC_SYS_INFO_IND_STRU*)pstMsg;
    enPreUtranMode   = NAS_UTRANCTRL_GetPreUtranMode();
    stLmmPlmn        = (pstLmmSysInfoMsg->stLteSysInfo.stSpecPlmnIdList.astSuitPlmnList[0]);

    if (VOS_FALSE == NAS_MML_IsGULModeConfigured())
    {
        return VOS_FALSE;
    }

    /* 把LMM的PLMN转换成GU NAS的PLMN格式 */
    NAS_MMC_ConvertLmmPlmnToGUNasFormat(&stLmmPlmn,
                                        &stGUNasPlmn);

    /* 自动模式收到GAS消息，需要更新当前模式 */
    if (NAS_UTRANCTRL_UTRAN_SWITCH_MODE_AUTO == NAS_UTRANCTRL_GetCurUtranSwitchMode())
    {
        /* 如果当前的MCC不在TD的PLMN LIST列表中,UTRAN MODE为FDD */
        if (VOS_FALSE == NAS_MML_IsMccInDestMccList(stGUNasPlmn.ulMcc,
                                                    NAS_UTRANCTRL_GetSpecTdMccListNum(),
                                                    NAS_UTRANCTRL_GetSpecTdMccList()))
        {
            enCurrUtranMode = NAS_UTRANCTRL_UTRAN_MODE_FDD;
            NAS_UTRANCTRL_SetRoamingZone(NAS_UTRANCTRL_ROAM_ZONE_OUTSIDE_CHINA_MAINLAND);
        }
        else
        {
            enCurrUtranMode = NAS_UTRANCTRL_UTRAN_MODE_TDD;

            NAS_UTRANCTRL_SetSearchedSpecTdMccFlg(VOS_TRUE);
            NAS_UTRANCTRL_SetRoamingZone(NAS_UTRANCTRL_ROAM_ZONE_CHINA_MAINLAND);
        }

        NAS_UTRANCTRL_SetCurrUtranMode(enCurrUtranMode);
    }

    /* 模式不变不通知RRC */
    if (enPreUtranMode == NAS_UTRANCTRL_GetCurrUtranMode())
    {
        /* 消息未被处理完成需继续处理 */
        return VOS_FALSE;
    }

    /* 把当前UtranMode写到NV中 */
    NAS_UTRANCTRL_WriteUtranModeNvim(NAS_UTRANCTRL_GetCurrUtranMode());

    /* 更新上次发给接入层的UtranMode */
    NAS_UTRANCTRL_SetPreUtranMode(NAS_UTRANCTRL_GetCurrUtranMode());

    /* 进入ModeChange状态机 */
    NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_MODE_CHANGE);

    return VOS_TRUE;
}
#endif


VOS_UINT32 NAS_UTRANCTRL_RcvMmcGmmStartCnf_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MML_SIM_MS_IDENTITY_STRU       *pstMsIdentity = VOS_NULL_PTR;
    VOS_UINT8                           ucSimPresentStatus;

    pstMsIdentity                       = NAS_MML_GetSimMsIdentity();
    ucSimPresentStatus                  = NAS_MML_GetSimPresentStatus();


    /* 开机读取 en_NV_Item_SMC_Ctrl_Flg NV项 */
    NAS_UTRANCTRL_ReadSmcCtrlFlgNvim();

    /* 开机读取 en_NV_Item_Utran_Mode_Auto_Switch NV项 */
    NAS_UTRANCTRL_ReadUtranModeAutoSwitchNvim();

    /* 开机读取 en_NV_Item_Current_Utran_Mode NV项 */
    NAS_UTRANCTRL_ReadUtranModeNvim();

    /* 根据IMSI及en_NV_Item_Utran_Mode_Auto_Switch NV项来更新当前的UTRAN切换模式 */
    NAS_UTRANCTRL_UpdateCurUtranSwitchMode( pstMsIdentity->aucImsi,
                                            ucSimPresentStatus,
                                            NAS_UTRANCTRL_GetUtranModeAutoSwtich() );

    /* 开机时会通知RRC当前的UTRAN MODE,此时记录下来,后续收到系统消息不再通知 */
    if (VOS_TRUE == NAS_MML_IsGULModeConfigured())
    {
        NAS_UTRANCTRL_SetPreUtranMode(NAS_UTRANCTRL_GetCurrUtranMode());
    }
    else
    {
        NAS_UTRANCTRL_SetPreUtranMode(NAS_UTRANCTRL_UTRAN_MODE_NULL);
    }

    /* 消息未被处理完成需继续处理 */
    return VOS_FALSE;
}


VOS_UINT32 NAS_UTRANCTRL_RcvPlmnSearchReq_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MSCC_MMC_PLMN_SEARCH_REQ_STRU      *pstMsccPlmnSrchReq;
    VOS_UINT32                         *pulTdMccList;
    VOS_UINT32                          ulTdMccNum;

    pstMsccPlmnSrchReq = (MSCC_MMC_PLMN_SEARCH_REQ_STRU *)pstMsg;
    ulTdMccNum         = NAS_UTRANCTRL_GetSpecTdMccListNum();
    pulTdMccList       = NAS_UTRANCTRL_GetSpecTdMccList();

    if (NAS_MSCC_PIF_ACQ_REASON_POWER_UP == pstMsccPlmnSrchReq->enAcqReason)
    {
        NAS_UTRANCTRL_SetSwithOnPlmnSearchFlag(VOS_TRUE);
    }

    if (VOS_TRUE == NAS_MML_IsPlmnIdValid(&(pstMsccPlmnSrchReq->stInitAcqPlmnInfo.stInitAcqPlmnId)))
    {
        if (VOS_TRUE == NAS_MML_IsMccInDestMccList(pstMsccPlmnSrchReq->stInitAcqPlmnInfo.stInitAcqPlmnId.ulMcc, ulTdMccNum, pulTdMccList))
        {
             /* 更新中国网络标记 */
            NAS_UTRANCTRL_SetSearchedSpecTdMccFlg(VOS_TRUE);
            NAS_UTRANCTRL_SetRoamingZone(NAS_UTRANCTRL_ROAM_ZONE_CHINA_MAINLAND);
        }
        else
        {
            NAS_UTRANCTRL_SetSearchedSpecTdMccFlg(VOS_FALSE);
            NAS_UTRANCTRL_SetRoamingZone(NAS_UTRANCTRL_ROAM_ZONE_OUTSIDE_CHINA_MAINLAND);
        }
    }

    /* 消息未被处理完成需继续处理 */
    return VOS_FALSE;
}


VOS_UINT32 NAS_UTRANCTRL_RcvMsccAcqReq_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_UTRANCTRL_SetSwithOnPlmnSearchFlag(VOS_FALSE);

    /* 消息未被处理完成需继续处理 */
    return VOS_FALSE;
}




VOS_UINT32 NAS_UTRANCTRL_RcvPlmnSpecialReq_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    if (NAS_UTRANCTRL_UTRAN_SWITCH_MODE_AUTO == NAS_UTRANCTRL_GetCurUtranSwitchMode())
    {
        NAS_UTRANCTRL_SetSearchedSpecTdMccFlg(VOS_FALSE);

        NAS_UTRANCTRL_SetRoamingZone(NAS_UTRANCTRL_ROAM_ZONE_UNKNOWN);
    }

    NAS_UTRANCTRL_SetSwithOnPlmnSearchFlag(VOS_FALSE);

    /* 消息未被处理完成需继续处理 */
    return VOS_FALSE;
}
#if (FEATURE_ON == FEATURE_CSG)

VOS_UINT32 NAS_UTRANCTRL_RcvCsgSpecSearchReq_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    if (NAS_UTRANCTRL_UTRAN_SWITCH_MODE_AUTO == NAS_UTRANCTRL_GetCurUtranSwitchMode())
    {
        NAS_UTRANCTRL_SetSearchedSpecTdMccFlg(VOS_FALSE);
        NAS_UTRANCTRL_SetRoamingZone(NAS_UTRANCTRL_ROAM_ZONE_UNKNOWN);
    }

    NAS_UTRANCTRL_SetSwithOnPlmnSearchFlag(VOS_FALSE);

    /* 消息未被处理完成需继续处理 */
    return VOS_FALSE;
}
#endif


VOS_UINT32 NAS_UTRANCTRL_RcvWasStartCnf_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    RRMM_START_CNF_STRU                *pstRrStartCnfMsg;

    pstRrStartCnfMsg = (RRMM_START_CNF_STRU *)pstMsg;

    /* 如果当前UTRANCTRL模的等开机回复时状态定时器不运行认为异常 */
    if ( NAS_MMC_TIMER_STATUS_RUNING != NAS_UTRANCTRL_GetTimerStatus(TI_NAS_UTRANCTRL_WAIT_WAS_START_CNF))
    {
        /* 异常打印 */
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvWasStartCnf_Main: Unexpected Plmnsrch msg!");

        /* 丢弃该异常消息 */
        return VOS_TRUE;
    }

    /* 停止MMC通过UTRANCTRL模块给UTRAN模发送开机消息时候起的定时器 */
    NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_WAS_START_CNF);

    if (MMC_START_FAILURE == pstRrStartCnfMsg->ulResult)
    {
        /* 若UTRAN模块开机失败，则直接进MMC状态机进行处理 */

        NAS_MML_SoftReBoot(NAS_MML_REBOOT_SCENE_MMC_WAIT_WAS_START_CNF_FAIL);

        return VOS_FALSE;
    }

    /* 切换到适配模块的开机状态机进行处理 */
    NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_SWITCH_ON);

    /* WAS的开机回复消息在适配模块的开机状态机进行处理  */
    return VOS_TRUE;
}



VOS_UINT32 NAS_UTRANCTRL_RcvTiWaitWasStartCnfExpired_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 构建开机失败消息，进原MMC状态机进行处理 */
    RRMM_START_CNF_STRU                *pstTempMsg      = VOS_NULL_PTR;
    VOS_UINT32                          ulTempMsgEventType;

    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitWasStartCnfExpired_Main: ENTERED");

    /* 申请内存 */
    pstTempMsg = (RRMM_START_CNF_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMC,
                                               sizeof(RRMM_START_CNF_STRU));
    if( VOS_NULL_PTR == pstTempMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitWasStartCnfExpired_Main: Alloc memeory Error");
        return VOS_FALSE;
    }

    /* 构造开机回复失败消息 */
    NAS_UTRANCTRL_BuildRrMmStartCnfMsg(WUEPS_PID_WRR,
                                       WUEPS_PID_MMC,
                                       MMC_START_FAILURE,
                                       pstTempMsg);

    /* 取得当前构建消息的类型 */
    ulTempMsgEventType = NAS_UTRANCTRL_GetMsgEventType( (struct MsgCB *)pstTempMsg );

    /* 将更改过的超时消息放到输出队列中 */
    NAS_UTRANCTRL_SaveBufferUtranSndMmcMsg(ulTempMsgEventType,
                                           (struct MsgCB *)pstTempMsg);

    /* 设置需要替换入口消息标记 */
    NAS_UTRANCTRL_SetReplaceMmcMsgFlg(VOS_TRUE);

    /* 释放申请的消息 */
    PS_MEM_FREE(WUEPS_PID_MMC, pstTempMsg);

    /* 当前消息进入后续MMC状态机处理 */

    NAS_MML_SoftReBoot(NAS_MML_REBOOT_SCENE_MMC_WAIT_WAS_START_CNF_EXPIRED);

    return VOS_FALSE;
}


VOS_UINT32 NAS_UTRANCTRL_RcvTdStartCnf_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    RRMM_START_CNF_STRU                *pstRrStartCnfMsg;

    pstRrStartCnfMsg = (RRMM_START_CNF_STRU *)pstMsg;

    /* 如果当前UTRANCTRL模的等开机回复时状态定时器不运行认为异常 */
    if ( NAS_MMC_TIMER_STATUS_RUNING != NAS_UTRANCTRL_GetTimerStatus(TI_NAS_UTRANCTRL_WAIT_TD_START_CNF))
    {
        /* 异常打印 */
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTdStartCnf_Main: Unexpected Plmnsrch msg!");

        /* 丢弃该异常消息 */
        return VOS_TRUE;
    }

    /* 停止MMC通过UTRANCTRL模块给UTRAN模发送开机消息时候起的定时器 */
    NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_TD_START_CNF);

    if (MMC_START_FAILURE == pstRrStartCnfMsg->ulResult)
    {
        /* 若UTRAN模块开机失败，则直接进MMC状态机进行处理 */

        NAS_MML_SoftReBoot(NAS_MML_REBOOT_SCENE_MMC_WAIT_TDS_START_CNF_FAIL);

        return VOS_FALSE;
    }

    /* 切换到适配模块的开机状态机进行处理 */
    NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_SWITCH_ON);

    /* TD的开机回复消息在适配模块的开机状态机进行处理  */
    return VOS_TRUE;
}



VOS_UINT32 NAS_UTRANCTRL_RcvTiWaitTdStartCnfExpired_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 构建开机失败消息，进原MMC状态机进行处理 */
    RRMM_START_CNF_STRU                *pstTempMsg      = VOS_NULL_PTR;
    VOS_UINT32                          ulTempMsgEventType;

    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitTdStartCnfExpired_Main: ENTERED");

    /* 申请内存  */
    pstTempMsg = (RRMM_START_CNF_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMC,
                                               sizeof(RRMM_START_CNF_STRU));
    if( VOS_NULL_PTR == pstTempMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitTdStartCnfExpired_Main: Alloc memeory Error");
        return VOS_FALSE;

    }

    /* 构造开机回复失败消息 */
    NAS_UTRANCTRL_BuildRrMmStartCnfMsg(WUEPS_PID_WRR,
                                       WUEPS_PID_MMC,
                                       MMC_START_FAILURE,
                                       pstTempMsg);

    /* 取得当前构建消息的类型 */
    ulTempMsgEventType = NAS_UTRANCTRL_GetMsgEventType( (struct MsgCB *)pstTempMsg );

    /* 将更改过的超时消息放到输出队列中 */
    NAS_UTRANCTRL_SaveBufferUtranSndMmcMsg(ulTempMsgEventType,
                                           (struct MsgCB *)pstTempMsg);

    /* 设置需要替换入口消息标记 */
    NAS_UTRANCTRL_SetReplaceMmcMsgFlg(VOS_TRUE);

    /* 释放申请的消息 */
    PS_MEM_FREE(WUEPS_PID_MMC, pstTempMsg);

    /* 当前消息进入后续MMC状态机处理 */

    NAS_MML_SoftReBoot(NAS_MML_REBOOT_SCENE_MMC_WAIT_TDS_START_CNF_EXPIRED);

    return VOS_FALSE;
}


VOS_UINT32 NAS_UTRANCTRL_RcvWasPowerOffCnf_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 如果当前UTRANCTRL模的等开机回复时状态定时器不运行认为异常 */
    if ( NAS_MMC_TIMER_STATUS_RUNING != NAS_UTRANCTRL_GetTimerStatus(TI_NAS_UTRANCTRL_WAIT_WAS_POWER_OFF_CNF))
    {
        /* 异常打印 */
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvWasPowerOffCnf_Main: Unexpected Plmnsrch msg!");

        /* 丢弃该异常消息 */
        return VOS_TRUE;
    }

    /* 停止MMC通过UTRANCTRL模块给UTRAN模发送关机消息时候起的定时器 */
    NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_WAS_POWER_OFF_CNF);

    /* 切换到适配模块的关机状态机进行处理 */
    NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_POWER_OFF);

    /* Was的关机回复消息在适配模块的关机状态机进行处理  */
    return VOS_TRUE;
}


VOS_UINT32 NAS_UTRANCTRL_RcvTiWaitWasPowerOffCnfExpired_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitWasPowerOffCnfExpired_Main: ENTERED");

    /* 如果当前为UTRAN MODE AUTO SWITCH模式，则需要启动UTRANCTRL模块的关机状态机 */
    /* 切换到适配模块的关机状态机进行处理 */
    NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_POWER_OFF);

    /* Was的关机回复消息在适配模块的关机状态机进行处理  */
    return VOS_TRUE;
}


VOS_UINT32 NAS_UTRANCTRL_RcvTdPowerOffCnf_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 如果当前UTRANCTRL模的等开机回复时状态定时器不运行认为异常 */
    if ( NAS_MMC_TIMER_STATUS_RUNING != NAS_UTRANCTRL_GetTimerStatus(TI_NAS_UTRANCTRL_WAIT_TD_POWER_OFF_CNF))
    {
        /* 异常打印 */
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTdPowerOffCnf_Main: Unexpected Plmnsrch msg!");

        /* 丢弃该异常消息 */
        return VOS_TRUE;
    }

    /* 停止MMC通过UTRANCTRL模块给UTRAN模发送关机消息时候起的定时器 */
    NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_TD_POWER_OFF_CNF);

    /* 切换到适配模块的关机状态机进行处理 */
    NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_POWER_OFF);

    /* td的关机回复消息在适配模块的关机状态机进行处理  */
    return VOS_TRUE;
}


VOS_UINT32 NAS_UTRANCTRL_RcvTiWaitTdPowerOffCnfExpired_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitTdPowerOffCnfExpired_Main: ENTERED");

    /* 切换到适配模块的关机状态机进行处理 */
    NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_POWER_OFF);

    /* Td的关机回复消息在适配模块的关机状态机进行处理  */
    return VOS_TRUE;
}



VOS_UINT32 NAS_UTRANCTRL_RcvWasSysCfgCnf_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    RRMM_SYS_CFG_CNF_STRU              *pstSysCfgCnf;

    pstSysCfgCnf = (RRMM_SYS_CFG_CNF_STRU *)pstMsg;

    /* 如果当前UTRANCTRL模的等开机回复时状态定时器不运行认为异常 */
    if ( NAS_MMC_TIMER_STATUS_RUNING != NAS_UTRANCTRL_GetTimerStatus(TI_NAS_UTRANCTRL_WAIT_WAS_SYSCFG_CNF))
    {
        /* 异常打印 */
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvWasSysCfgCnf_Main: Unexpected Plmnsrch msg!");

        /* 丢弃该异常消息 */
        return VOS_TRUE;
    }

    /* 停止MMC通过UTRANCTRL模块给UTRAN模发送SYSCFG设置消息时候起的定时器 */
    NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_WAS_SYSCFG_CNF);

    if ( RRMM_SYS_CFG_RESULT_FAILURE == pstSysCfgCnf->ulResult)
    {
        /* 若syscfg设置失败，则直接进MMC状态机进行处理 */
        return VOS_FALSE;
    }

    /* 若syscfg设置回复成功，切换到适配模块的SYSCFG状态机进行处理 */
    NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_SYSCFG);

    /* Was的SYSCFG回复消息在适配模块的SYSCFG状态机进行处理  */
    return VOS_TRUE;
}


VOS_UINT32 NAS_UTRANCTRL_RcvTiWaitWasSysCfgCnfExpired_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    RRMM_SYS_CFG_CNF_STRU              *pstTempMsg      = VOS_NULL_PTR;
    VOS_UINT32                          ulTempMsgEventType;

    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitWasSysCfgCnfExpired_Main: ENTERED");

    /* 构建SYSCFG设置失败消息，进原MMC状态机进行处理 */
    pstTempMsg = (RRMM_SYS_CFG_CNF_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMC,
                                               sizeof(RRMM_SYS_CFG_CNF_STRU));
    if( VOS_NULL_PTR == pstTempMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitWasSysCfgCnfExpired_Main: Alloc memeory Error");
        return VOS_TRUE ;
    }

    /* 构造SYSCFG设置回复消息 */
    NAS_UTRANCTRL_BuildRrMmSysCfgCnfMsg(WUEPS_PID_WRR,
                                        WUEPS_PID_MMC,
                                        RRMM_SYS_CFG_RESULT_FAILURE,
                                        pstTempMsg);

    ulTempMsgEventType = NAS_UTRANCTRL_GetMsgEventType( (struct MsgCB *)pstTempMsg );

    /* 将更改过的超时消息放到输出队列中 */
    NAS_UTRANCTRL_SaveBufferUtranSndMmcMsg(ulTempMsgEventType,
                                           (struct MsgCB *)pstTempMsg);

    /* 设置需要替换入口消息标记 */
    NAS_UTRANCTRL_SetReplaceMmcMsgFlg(VOS_TRUE);

    /* 释放申请的消息 */
    PS_MEM_FREE(WUEPS_PID_MMC, pstTempMsg);

    /* 当前消息进入后续MMC状态机处理 */
    return VOS_FALSE;
}



VOS_UINT32 NAS_UTRANCTRL_RcvTdSysCfgCnf_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    RRMM_SYS_CFG_CNF_STRU              *pstSysCfgCnf;

    pstSysCfgCnf = (RRMM_SYS_CFG_CNF_STRU *)pstMsg;

    /* 如果当前UTRANCTRL模的等开机回复时状态定时器不运行认为异常 */
    if ( NAS_MMC_TIMER_STATUS_RUNING != NAS_UTRANCTRL_GetTimerStatus(TI_NAS_UTRANCTRL_WAIT_TD_SYSCFG_CNF))
    {
        /* 异常打印 */
        NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTdSysCfgCnf_Main: Unexpected Plmnsrch msg!");

        /* 丢弃该异常消息 */
        return VOS_TRUE;
    }

    /* 停止MMC通过UTRANCTRL模块给UTRAN模发送SYSCFG设置消息时候起的定时器 */
    NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_TD_SYSCFG_CNF);

    if ( RRMM_SYS_CFG_RESULT_FAILURE == pstSysCfgCnf->ulResult)
    {
        /* 若syscfg设置失败，则直接进MMC状态机进行处理 */
        return VOS_FALSE;
    }

    /* 切换到适配模块的SYSCFG状态机进行处理 */
    NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_SYSCFG);

    /* Td的SYSCFG回复消息在适配模块的SYSCFG状态机进行处理  */
    return VOS_TRUE;
}


VOS_UINT32 NAS_UTRANCTRL_RcvTiWaitTdSysCfgCnfExpired_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    RRMM_SYS_CFG_CNF_STRU              *pstTempMsg      = VOS_NULL_PTR;
    VOS_UINT32                          ulTempMsgEventType;

    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitTdSysCfgCnfExpired_Main: ENTERED");

    /* 构建SYSCFG设置失败消息，进原MMC状态机进行处理 */
    pstTempMsg = (RRMM_SYS_CFG_CNF_STRU *)PS_MEM_ALLOC(WUEPS_PID_MMC,
                                               sizeof(RRMM_SYS_CFG_CNF_STRU));
    if( VOS_NULL_PTR == pstTempMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitTdSysCfgCnfExpired_Main: Alloc memeory Error");
        return VOS_TRUE ;
    }

    /* 构造SYSCFG设置回复消息 */
    NAS_UTRANCTRL_BuildRrMmSysCfgCnfMsg(WUEPS_PID_WRR,
                                        WUEPS_PID_MMC,
                                        RRMM_SYS_CFG_RESULT_FAILURE,
                                        pstTempMsg);

    ulTempMsgEventType = NAS_UTRANCTRL_GetMsgEventType( (struct MsgCB *)pstTempMsg );

    /* 将更改过的超时消息放到输出队列中 */
    NAS_UTRANCTRL_SaveBufferUtranSndMmcMsg(ulTempMsgEventType,
                                           (struct MsgCB *)pstTempMsg);

    /* 设置需要替换入口消息标记 */
    NAS_UTRANCTRL_SetReplaceMmcMsgFlg(VOS_TRUE);

    /* 释放申请的消息 */
    PS_MEM_FREE(WUEPS_PID_MMC, pstTempMsg);

    /* 当前消息进入后续MMC状态机处理 */
    return VOS_FALSE;
}



VOS_UINT32 NAS_UTRANCTRL_RcvWasPlmnSrchCnf_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    RRMM_PLMN_SEARCH_CNF_STRU          *pstSrchCnfMsg;
    NAS_UTRANCTRL_UTRAN_MODE_ENUM_UINT8 enUtranMode;


    enUtranMode                 = NAS_UTRANCTRL_GetCurrUtranMode();

    pstSrchCnfMsg   = (RRMM_PLMN_SEARCH_CNF_STRU *)pstMsg;

    /* 如果当前UTRANCTRL模的等指定搜网回复时状态定时器运行则停止定时器 */
    if (NAS_MMC_TIMER_STATUS_RUNING != NAS_UTRANCTRL_GetTimerStatus(TI_NAS_UTRANCTRL_WAIT_WAS_PLMN_SEARCH_CNF))
    {
        NAS_NORMAL_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvWasPlmnSrchCnf_Main:TI_NAS_UTRANCTRL_WAIT_WAS_PLMN_SEARCH_CNF not running");
        return VOS_FALSE;
    }

    NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_WAS_PLMN_SEARCH_CNF);

    /* 降圈复杂度 */
    if (VOS_FALSE == NAS_UTRANCTRL_IsNeedProcGuPlmnSearchCnf(NAS_MML_NET_RAT_TYPE_WCDMA, pstSrchCnfMsg))
    {
        NAS_NORMAL_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvWasPlmnSrchCnf_Main: No need process in UTRANCTRL, Clear Buffered Msg and enter MMC FSM");
        /* 列表搜或者PrefBand搜索需要更新 enRoamingZone 标志 */
        if ((RRC_PLMN_SEARCH_RLT_USER_LIST== pstSrchCnfMsg->ulSearchRlt)
         || (RRC_PLMN_SEARCH_RLT_PREF_BAND_FAIL == pstSrchCnfMsg->ulSearchRlt))
        {
            /* 指定搜网失败根据搜网列表更新中国网络标记 */
            NAS_UTRANCTRL_UpdateRoamZoneRelatedFlags(NAS_UTRANCTRL_GetSpecTdMccListNum(),
                                                     NAS_UTRANCTRL_GetSpecTdMccList(),
                                                     &(pstSrchCnfMsg->PlmnIdList));
        }

        /* 清除缓存 */
        NAS_UTRANCTRL_ClearBufferedSndUtranReqMsg();

        /* 进入MMC状态机处理 */
        return VOS_FALSE;
    }

    /* 自动切换模式下，CSFB标志存在时，并且当前是FDD模式，不再搜TD */
    if ((VOS_TRUE                       == NAS_MML_IsCsfbServiceStatusExist())
     && (NAS_UTRANCTRL_UTRAN_MODE_FDD   == enUtranMode))
    {
        NAS_NORMAL_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvWasPlmnSrchCnf_Main: CSFB and FDD mode, Clear Buffered Msg and enter MMC FSM");
        /* 清除缓存的搜网请求消息 */
        NAS_UTRANCTRL_ClearBufferedSndUtranReqMsg();

        /* 进入MMC状态机处理 */
        return VOS_FALSE;
    }

    /* 指定搜网失败根据搜网列表更新中国网络标记 */
    NAS_UTRANCTRL_UpdateRoamZoneRelatedFlags(NAS_UTRANCTRL_GetSpecTdMccListNum(),
                                             NAS_UTRANCTRL_GetSpecTdMccList(),
                                             &(pstSrchCnfMsg->PlmnIdList));

    if (NAS_UTRANCTRL_ROAM_ZONE_OUTSIDE_CHINA_MAINLAND == NAS_UTRANCTRL_GetRoamingZone())
    {
        NAS_NORMAL_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvWasPlmnSrchCnf_Main: MCC not in TDS MCC LIST and not in Hongkong, Clear buffer and enter MMC FSM");

        /* 开机搜网场景，如果was搜网结果中没有一个网络mcc在tds mcc列表中且不包括香港网络，且高低质量网络个数不全为0，
           则跳过tds搜网，清除缓存*/
        NAS_UTRANCTRL_ClearBufferedSndUtranReqMsg();

        /* 进入MMC状态机处理 */
        return VOS_FALSE;
    }

    /* 为了降低功耗，history搜或pref band搜fail时W和TD不交替搜 */
    if ((RRC_PLMN_SEARCH_RLT_HISTORY_FAIL   == pstSrchCnfMsg->ulSearchRlt)
     || (RRC_PLMN_SEARCH_RLT_PREF_BAND_FAIL == pstSrchCnfMsg->ulSearchRlt))
    {
        NAS_UTRANCTRL_ClearBufferedSndUtranReqMsg();

        /* 进入MMC状态机处理 */
        return VOS_FALSE;
    }

    /* 切换到适配模块的选网状态机进行处理 */
    NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_PLMN_SELECTION);

    NAS_NORMAL_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvWasPlmnSrchCnf_Main: enter NAS_UTRANCTRL_FSM_PLMN_SELECTION");

    /* was的搜网回复消息在适配模块的选网状态机进行处理  */
    return VOS_TRUE;
}


VOS_UINT32  NAS_UTRANCTRL_RcvWasSearchedPlmnInfoInd_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    RRMM_SEARCHED_PLMN_INFO_IND_STRU   *pstSearchedPlmnInfoMsg = VOS_NULL_PTR;
    VOS_UINT32                         *pulTdMccList  = VOS_NULL_PTR;
    VOS_UINT32                          i;
    VOS_UINT32                          ulIsNonTdMccFound;
    VOS_UINT8                           ucTdMccListNum;
    NAS_UTRANCTRL_ROAM_ZONE_ENUM_UINT8  enRoamingZone;

    ulIsNonTdMccFound                   = VOS_FALSE;
    pulTdMccList                        = NAS_UTRANCTRL_GetSpecTdMccList();
    ucTdMccListNum                      = NAS_UTRANCTRL_GetSpecTdMccListNum();
    enRoamingZone                       = NAS_UTRANCTRL_GetRoamingZone();
    pstSearchedPlmnInfoMsg              = (RRMM_SEARCHED_PLMN_INFO_IND_STRU*)pstMsg;

    if (NAS_UTRANCTRL_UTRAN_SWITCH_MODE_AUTO == NAS_UTRANCTRL_GetCurUtranSwitchMode())
    {
        for (i = 0; i < pstSearchedPlmnInfoMsg->ulAvailPlmnNum; i++)
        {
            /* 判断MCC是否在TD MCC列表中 */
            if (VOS_TRUE == NAS_MML_IsMccInDestMccList(pstSearchedPlmnInfoMsg->astPlmnWithLacList[i].stPlmnId.ulMcc, ucTdMccListNum, pulTdMccList))
            {
                NAS_UTRANCTRL_SetSearchedSpecTdMccFlg(VOS_TRUE);
            }
            else
            {
                ulIsNonTdMccFound = VOS_TRUE;
            }
        }

        /* 如果搜索到TdMcc，则根据当前enRoamingZone的值来设置为CHINA_MAINLAND或者CHINA_BORDER
         * 否则，如果搜到了其他Mcc，则设置为OUTSIDE_CHINA_MAINLAND
         */
        if (VOS_TRUE == NAS_UTRANCTRL_GetSearchedSpecTdMccFlg())
        {
            if ((NAS_UTRANCTRL_ROAM_ZONE_OUTSIDE_CHINA_MAINLAND == enRoamingZone)
             || (VOS_TRUE == ulIsNonTdMccFound))
            {
                NAS_UTRANCTRL_SetRoamingZone(NAS_UTRANCTRL_ROAM_ZONE_CHINA_BORDER);
            }
            else
            {
                NAS_UTRANCTRL_SetRoamingZone(NAS_UTRANCTRL_ROAM_ZONE_CHINA_MAINLAND);
            }
        }
        else if (VOS_TRUE == ulIsNonTdMccFound)
        {
            NAS_UTRANCTRL_SetRoamingZone(NAS_UTRANCTRL_ROAM_ZONE_OUTSIDE_CHINA_MAINLAND);
        }

        NAS_NORMAL_LOG1(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvWasSearchedPlmnInfoInd_Main:", NAS_UTRANCTRL_GetRoamingZone());
    }

    /* 进入MMC状态机处理 */
    return VOS_FALSE;
}


VOS_UINT32  NAS_UTRANCTRL_RcvGasSearchedPlmnInfoInd_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    RRMM_SEARCHED_PLMN_INFO_IND_STRU   *pstSearchedPlmnInfoMsg = VOS_NULL_PTR;
    VOS_UINT32                         *pulTdMccList  = VOS_NULL_PTR;
    VOS_UINT32                          i;
    VOS_UINT32                          ulIsNonTdMccFound;
    VOS_UINT8                           ucTdMccListNum;
    NAS_UTRANCTRL_ROAM_ZONE_ENUM_UINT8  enRoamingZone;

    ulIsNonTdMccFound                   = VOS_FALSE;
    pulTdMccList                        = NAS_UTRANCTRL_GetSpecTdMccList();
    ucTdMccListNum                      = NAS_UTRANCTRL_GetSpecTdMccListNum();
    enRoamingZone                       = NAS_UTRANCTRL_GetRoamingZone();
    pstSearchedPlmnInfoMsg              = (RRMM_SEARCHED_PLMN_INFO_IND_STRU*)pstMsg;

    if (NAS_UTRANCTRL_UTRAN_SWITCH_MODE_AUTO == NAS_UTRANCTRL_GetCurUtranSwitchMode())
    {
        for (i = 0; i < pstSearchedPlmnInfoMsg->ulAvailPlmnNum; i++)
        {
            /* 判断MCC是否在TD MCC列表中 */
            if (VOS_TRUE == NAS_MML_IsMccInDestMccList(pstSearchedPlmnInfoMsg->astPlmnWithLacList[i].stPlmnId.ulMcc, ucTdMccListNum, pulTdMccList))
            {
                NAS_UTRANCTRL_SetSearchedSpecTdMccFlg(VOS_TRUE);
            }
            else
            {
                ulIsNonTdMccFound = VOS_TRUE;
            }
        }

        /* 如果搜索到TdMcc，则根据当前enRoamingZone的值来设置为CHINA_MAINLAND或者CHINA_BORDER
         * 否则，如果搜到了其他Mcc，则设置为OUTSIDE_CHINA_MAINLAND
         */
        if (VOS_TRUE == NAS_UTRANCTRL_GetSearchedSpecTdMccFlg())
        {
            if ((NAS_UTRANCTRL_ROAM_ZONE_OUTSIDE_CHINA_MAINLAND == enRoamingZone)
             || (VOS_TRUE == ulIsNonTdMccFound))
            {
                NAS_UTRANCTRL_SetRoamingZone(NAS_UTRANCTRL_ROAM_ZONE_CHINA_BORDER);
            }
            else
            {
                NAS_UTRANCTRL_SetRoamingZone(NAS_UTRANCTRL_ROAM_ZONE_CHINA_MAINLAND);
                NAS_UTRANCTRL_SetCurrUtranMode(NAS_UTRANCTRL_UTRAN_MODE_TDD);
            }
        }
        else if (VOS_TRUE == ulIsNonTdMccFound)
        {
            NAS_UTRANCTRL_SetRoamingZone(NAS_UTRANCTRL_ROAM_ZONE_OUTSIDE_CHINA_MAINLAND);
            NAS_UTRANCTRL_SetCurrUtranMode(NAS_UTRANCTRL_UTRAN_MODE_FDD);
        }

        NAS_NORMAL_LOG1(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvGasSearchedPlmnInfoInd_Main:", NAS_UTRANCTRL_GetRoamingZone());
    }

    /* 进入MMC状态机处理 */
    return VOS_FALSE;
}


VOS_UINT32  NAS_UTRANCTRL_RcvLmmSearchedPlmnInfoInd_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    LMM_MMC_SEARCHED_PLMN_INFO_IND_STRU                    *pstSearchedPlmnInfoMsg = VOS_NULL_PTR;
    VOS_UINT32                                             *pulTdMccList  = VOS_NULL_PTR;
    VOS_UINT32                                              i;
    VOS_UINT32                                              ulIsNonTdMccFound;
    VOS_UINT8                                               ucTdMccListNum;
    NAS_UTRANCTRL_ROAM_ZONE_ENUM_UINT8                      enRoamingZone;
    NAS_MML_PLMN_ID_STRU                                    stGuNasPlmn;

    ulIsNonTdMccFound                   = VOS_FALSE;
    pulTdMccList                        = NAS_UTRANCTRL_GetSpecTdMccList();
    ucTdMccListNum                      = NAS_UTRANCTRL_GetSpecTdMccListNum();
    enRoamingZone                       = NAS_UTRANCTRL_GetRoamingZone();
    pstSearchedPlmnInfoMsg              = (LMM_MMC_SEARCHED_PLMN_INFO_IND_STRU*)pstMsg;

    if (NAS_UTRANCTRL_UTRAN_SWITCH_MODE_AUTO == NAS_UTRANCTRL_GetCurUtranSwitchMode())
    {
        for (i = 0; i < pstSearchedPlmnInfoMsg->ulTaiNum; i++)
        {
            /* 把LMM的PLMN转换成GU NAS的PLMN格式 */
            NAS_MMC_ConvertLmmPlmnToGUNasFormat(&(pstSearchedPlmnInfoMsg->stTaiList[i].stPlmnId),
                                                &stGuNasPlmn);

            /* 判断MCC是否在TD MCC列表中 */
            if (VOS_TRUE == NAS_MML_IsMccInDestMccList(stGuNasPlmn.ulMcc, ucTdMccListNum, pulTdMccList))
            {
                NAS_UTRANCTRL_SetSearchedSpecTdMccFlg(VOS_TRUE);
            }
            else
            {
                ulIsNonTdMccFound = VOS_TRUE;
            }
        }

        /* 如果搜索到TdMcc，则根据当前enRoamingZone的值来设置为CHINA_MAINLAND或者CHINA_BORDER
         * 否则，如果搜到了其他Mcc，则设置为OUTSIDE_CHINA_MAINLAND
         */
        if (VOS_TRUE == NAS_UTRANCTRL_GetSearchedSpecTdMccFlg())
        {
            if ((NAS_UTRANCTRL_ROAM_ZONE_OUTSIDE_CHINA_MAINLAND == enRoamingZone)
             || (VOS_TRUE == ulIsNonTdMccFound))
            {
                NAS_UTRANCTRL_SetRoamingZone(NAS_UTRANCTRL_ROAM_ZONE_CHINA_BORDER);
            }
            else
            {
                NAS_UTRANCTRL_SetRoamingZone(NAS_UTRANCTRL_ROAM_ZONE_CHINA_MAINLAND);
                NAS_UTRANCTRL_SetCurrUtranMode(NAS_UTRANCTRL_UTRAN_MODE_TDD);
            }
        }
        else if (VOS_TRUE == ulIsNonTdMccFound)
        {
            NAS_UTRANCTRL_SetRoamingZone(NAS_UTRANCTRL_ROAM_ZONE_OUTSIDE_CHINA_MAINLAND);
            NAS_UTRANCTRL_SetCurrUtranMode(NAS_UTRANCTRL_UTRAN_MODE_FDD);
        }

        NAS_NORMAL_LOG1(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvLmmSearchedPlmnInfoInd_Main:", NAS_UTRANCTRL_GetRoamingZone());
    }

    /* 进入MMC状态机处理 */
    return VOS_FALSE;
}


VOS_UINT32 NAS_UTRANCTRL_RcvInterAbortUtranCtrlPlmnSearchReq_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    /* 如果当前UTRANCTRL模的等指定搜网回复时状态定时器运行则停止定时器 */
    if (NAS_MMC_TIMER_STATUS_RUNING == NAS_UTRANCTRL_GetTimerStatus(TI_NAS_UTRANCTRL_WAIT_WAS_PLMN_SEARCH_CNF))
    {
        NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_WAS_PLMN_SEARCH_CNF);
    }

    /* 更新utran mode为fdd，utranctrl 搜网状态机退出，消息进mmc继续处理，通知接入层打断由mmc处理 */
    NAS_UTRANCTRL_SetCurrUtranMode(NAS_UTRANCTRL_UTRAN_MODE_FDD);

    /* 通知mmc MMCMMC_INTER_ABORT_UTRAN_CTRL_PLMN_SEARCH_CNF*/
    NAS_MMC_SndInterAbortUtranCtrlPlmnSearchCnfMsg();

    return VOS_TRUE;

}









VOS_UINT32 NAS_UTRANCTRL_RcvMmcInterSkipSearchWasIndMsg_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 如果正在搜TD，就不需要进状态机处理，在搜到TD search cnf的时候会跳过W */
    if (NAS_MMC_TIMER_STATUS_RUNING == NAS_MMC_GetTimerStatus(TI_NAS_UTRANCTRL_WAIT_TD_PLMN_SEARCH_CNF))
    {
        NAS_NORMAL_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvMmcInterSkipSearchWasIndMsg_Main: TI_NAS_UTRANCTRL_WAIT_TD_PLMN_SEARCH_CNF runing, return");
        return VOS_TRUE;
    }

    NAS_NORMAL_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvMmcInterSkipSearchWasIndMsg_Main: Set to NAS_UTRANCTRL_UTRAN_MODE_TDD and enter NAS_UTRANCTRL_FSM_PLMN_SELECTION");

    /* SKIP W时候，则设置当前为TDD模式 */
    NAS_UTRANCTRL_SetCurrUtranMode(NAS_UTRANCTRL_UTRAN_MODE_TDD);

    NAS_UTRANCTRL_SetSearchedSpecTdMccFlg(VOS_TRUE);
    NAS_UTRANCTRL_SetRoamingZone(NAS_UTRANCTRL_ROAM_ZONE_CHINA_MAINLAND);

    /* 切换到适配模块的选网状态机进行处理 */
    NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_PLMN_SELECTION);

    /* was的搜网回复消息在适配模块的选网状态机进行处理  */
    return VOS_TRUE;
}



VOS_UINT32 NAS_UTRANCTRL_RcvMmcInterSkipSearchTdsIndMsg_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* SKIP TDS时候，则设置当前为FDD模式 */
    NAS_UTRANCTRL_SetCurrUtranMode(NAS_UTRANCTRL_UTRAN_MODE_FDD);

    /* 切换到适配模块的选网状态机进行处理 */
    NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_PLMN_SELECTION);

    /* tds的搜网回复消息在适配模块的选网状态机进行处理  */
    return VOS_TRUE;
}



VOS_UINT32 NAS_UTRANCTRL_RcvTiWaitWasPlmnSearchCnfExpired_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_UTRANCTRL_ENTRY_MSG_STRU       *pstBufferEntryMsg = VOS_NULL_PTR;

    NAS_UTRANCTRL_ROAM_ZONE_ENUM_UINT8  enRoamingZone;

    NAS_UTRANCTRL_UTRAN_MODE_ENUM_UINT8 enUtranMode;
    RRC_PLMN_SEARCH_TYPE_ENUM_UINT32    enPlmnSearchType;

    enPlmnSearchType    = RRC_PLMN_SEARCH_TYPE_BUTT;

    enUtranMode     = NAS_UTRANCTRL_GetCurrUtranMode();
    enRoamingZone   = NAS_UTRANCTRL_GetRoamingZone();

    /* 获得当前缓存的缓冲区地址 */
    pstBufferEntryMsg = NAS_UTRANCTRL_GetBufferUtranSndMmcMsgAddr();

    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitWasPlmnSearchCnfExpired_Main: ENTERED");

    /* CSFB标志存在时，并且当前是FDD模式，不再搜TD */
    if ((VOS_TRUE                       == NAS_MML_IsCsfbServiceStatusExist())
     && (NAS_UTRANCTRL_UTRAN_MODE_FDD   == enUtranMode))
    {
        /* 构造指定搜网回复失败消息到内存的缓存区域 */
        NAS_UTRANCTRL_BuildRrMmPlmnSearchCnfFailMsg(WUEPS_PID_WRR,
                                                        WUEPS_PID_MMC,
                                                        NAS_UTRANCTRL_GetBufferedPlmnSearchReqType(),
                                                       (RRMM_PLMN_SEARCH_CNF_STRU *)pstBufferEntryMsg->aucEntryMsgBuffer);

        /* 保存构造指定搜网回复失败消息事件类型 */
        pstBufferEntryMsg->ulEventType = NAS_UTRANCTRL_GetMsgEventType( (struct MsgCB *)pstBufferEntryMsg->aucEntryMsgBuffer );

        /* 设置需要替换入口消息标记 */
        NAS_UTRANCTRL_SetReplaceMmcMsgFlg(VOS_TRUE);

        /* 当前消息进入后续MMC状态机处理 */
        return VOS_FALSE;
    }

    /* 为了省功耗，history和pref band搜时W与TD不交替搜 */
    enPlmnSearchType    = NAS_UTRANCTRL_GetBufferedPlmnSearchReqType();
    if ((RRC_PLMN_SEARCH_HISTORY == enPlmnSearchType)
     || (RRC_PLMN_SEARCH_PREF_BAND == enPlmnSearchType))
    {
        NAS_NORMAL_LOG1(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitWasPlmnSearchCnfExpired_Main: enPlmnSearchType is ", enPlmnSearchType);

        /* 构造W搜网回复定时器超时的消息到MMC处理 */
        NAS_UTRANCTRL_BuildTimerExpireMsg(WUEPS_PID_MMC,
                                          TI_NAS_MMC_WAIT_WAS_PLMN_SEARCH_CNF,
                                          (REL_TIMER_MSG *)pstBufferEntryMsg->aucEntryMsgBuffer);

        /* 保存构造指定搜网回复失败消息事件类型 */
        pstBufferEntryMsg->ulEventType = NAS_UTRANCTRL_GetMsgEventType( (struct MsgCB *)pstBufferEntryMsg->aucEntryMsgBuffer );

        /* 设置需要替换入口消息标记 */
        NAS_UTRANCTRL_SetReplaceMmcMsgFlg(VOS_TRUE);

        /* 当前消息进入后续MMC状态机处理 */
        return VOS_FALSE;
    }

    /* 如果当前为UTRAN MODE AUTO SWITCH模式，则需要启动UTRANCTRL模块的搜网状态机 */
    if ((NAS_UTRANCTRL_UTRAN_SWITCH_MODE_AUTO == NAS_UTRANCTRL_GetCurUtranSwitchMode())
     && (NAS_UTRANCTRL_ROAM_ZONE_OUTSIDE_CHINA_MAINLAND != enRoamingZone))
    {
        /* 切换到适配模块的选网状态机进行处理 */
        NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_PLMN_SELECTION);

        /* 进入适配层选网状态机处理  */
        return VOS_TRUE;
    }

    /* 构造指定搜网回复失败消息到内存的缓存区域 */
    NAS_UTRANCTRL_BuildRrMmPlmnSearchCnfFailMsg(WUEPS_PID_WRR,
                                                WUEPS_PID_MMC,
                                                enPlmnSearchType,
                                                (RRMM_PLMN_SEARCH_CNF_STRU *)pstBufferEntryMsg->aucEntryMsgBuffer);

    /* 保存构造指定搜网回复失败消息事件类型 */
    pstBufferEntryMsg->ulEventType = NAS_UTRANCTRL_GetMsgEventType( (struct MsgCB *)pstBufferEntryMsg->aucEntryMsgBuffer );

    /* 设置需要替换入口消息标记 */
    NAS_UTRANCTRL_SetReplaceMmcMsgFlg(VOS_TRUE);

    /* 当前消息进入后续MMC状态机处理 */
    return VOS_FALSE;
}



VOS_UINT32 NAS_UTRANCTRL_RcvWasRrMmRelInd_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    RRMM_REL_IND_STRU                  *pstRrmmRelIndMsg    = VOS_NULL_PTR;
    NAS_UTRANCTRL_ROAM_ZONE_ENUM_UINT8  enRoamingZone;

    pstRrmmRelIndMsg = (RRMM_REL_IND_STRU *)pstMsg;

    /* 如果当前UTRANCTRL模的等搜网回复时状态定时器运行，则停止定时器 */
    if (NAS_MMC_TIMER_STATUS_RUNING != NAS_UTRANCTRL_GetTimerStatus(TI_NAS_UTRANCTRL_WAIT_WAS_PLMN_SEARCH_CNF))
    {
        /* 当前消息进入后续MMC状态机处理 */
        return VOS_FALSE;
    }

    /* WAS搜网时接入层复位，搜TD模 */
    NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_WAS_PLMN_SEARCH_CNF);


    /* 如果原因为NO RF，则进入MMC状态机进行处理 */
    if (RRC_REL_CAUSE_NO_RF == pstRrmmRelIndMsg->ulRelCause)
    {
        return VOS_FALSE;
    }

    enRoamingZone  = NAS_UTRANCTRL_GetRoamingZone();

    if ((NAS_UTRANCTRL_UTRAN_SWITCH_MODE_AUTO == NAS_UTRANCTRL_GetCurUtranSwitchMode())
     && (NAS_UTRANCTRL_ROAM_ZONE_OUTSIDE_CHINA_MAINLAND != enRoamingZone))
    {
        /* 切换到适配模块的选网状态机进行处理 */
        NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_PLMN_SELECTION);

        /* 当前消息进入适配层选网状态机处理 */
        return VOS_TRUE;
    }

    /* 当前消息进入后续MMC状态机处理 */
    return VOS_FALSE;
}



VOS_UINT32 NAS_UTRANCTRL_RcvGasPlmnSrchCnf_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    RRMM_PLMN_SEARCH_CNF_STRU          *pstSrchCnfMsg = VOS_NULL_PTR;

    pstSrchCnfMsg   = (RRMM_PLMN_SEARCH_CNF_STRU *)pstMsg;

    /* 降圈复杂度 */
    if (VOS_FALSE == NAS_UTRANCTRL_IsNeedProcGuPlmnSearchCnf(NAS_MML_NET_RAT_TYPE_GSM, pstSrchCnfMsg))
    {
        NAS_NORMAL_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvGasPlmnSrchCnf_Main: No need process in UTRANCTRL, Clear Buffered Msg and enter MMC FSM");

        /* 列表搜或者PrefBand搜索需要更新 enRoamingZone 标志 */
        if (RRC_PLMN_SEARCH_RLT_USER_LIST == pstSrchCnfMsg->ulSearchRlt)
        {
            /* 指定搜网失败根据搜网列表更新中国网络标记 */
            NAS_UTRANCTRL_UpdateRoamZoneRelatedFlags(NAS_UTRANCTRL_GetSpecTdMccListNum(),
                                                     NAS_UTRANCTRL_GetSpecTdMccList(),
                                                     &(pstSrchCnfMsg->PlmnIdList));
        }
        /* 进入MMC状态机处理 */
        return VOS_FALSE;
    }

    /* 根据搜网列表更新中国网络标记 */
    NAS_UTRANCTRL_UpdateRoamZoneRelatedFlags(NAS_UTRANCTRL_GetSpecTdMccListNum(),
                                             NAS_UTRANCTRL_GetSpecTdMccList(),
                                             &(pstSrchCnfMsg->PlmnIdList));

    /* 进入状态机处理 */
    return VOS_FALSE;
}

#if (FEATURE_ON == FEATURE_LTE)

VOS_UINT32 NAS_UTRANCTRL_RcvLmmPlmnSrchCnf_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    LMM_MMC_PLMN_SRCH_CNF_STRU                             *pstLmmSrchCnfMsg;
    RRC_PLMN_ID_LIST_STRU                                  *pstGuPlmnIdList;

    pstLmmSrchCnfMsg = (LMM_MMC_PLMN_SRCH_CNF_STRU*)pstMsg;

    pstGuPlmnIdList   = (RRC_PLMN_ID_LIST_STRU*)PS_MEM_ALLOC(WUEPS_PID_MMC,
                                                                        sizeof(RRC_PLMN_ID_LIST_STRU));
    if (VOS_NULL_PTR == pstGuPlmnIdList)
    {
        NAS_ERROR_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvLmmPlmnSrchCnf_Main:Invalid pstGuPlmnIdList");
        return VOS_FALSE;
    }

    /* 提取函数，utranctrl不需要处理的消息或场景都放在函数里 */
    if (VOS_FALSE == NAS_UTRANCTRL_IsNeedProcLmmPlmnSearchCnf(NAS_MML_NET_RAT_TYPE_LTE, pstLmmSrchCnfMsg))
    {
        /* 列表搜或者PrefBand搜索需要更新 enRoamingZone */
        if (MMC_LMM_PLMN_SRCH_RLT_USER_LIST_SUCC == pstLmmSrchCnfMsg->enRlst)
        {
            /*转换L格式的搜索列表成为GU格式的*/
            NAS_MMC_ConvertLmmPlmnIdListToGUFormat(&(pstLmmSrchCnfMsg->stPlmnIdList), pstGuPlmnIdList);

            /* 指定搜网失败根据搜网列表更新中国网络标记 */
            NAS_UTRANCTRL_UpdateRoamZoneRelatedFlags(NAS_UTRANCTRL_GetSpecTdMccListNum(),
                                                     NAS_UTRANCTRL_GetSpecTdMccList(),
                                                     pstGuPlmnIdList);
        }

        PS_MEM_FREE(WUEPS_PID_MMC, pstGuPlmnIdList);

        /* 进入MMC状态机处理 */
        return VOS_FALSE;
    }

    /*转换L格式的搜索列表成为GU格式的*/
    NAS_MMC_ConvertLmmPlmnIdListToGUFormat(&(pstLmmSrchCnfMsg->stPlmnIdList), pstGuPlmnIdList);

    /* 根据搜网列表更新中国网络标记 */
    NAS_UTRANCTRL_UpdateRoamZoneRelatedFlags(NAS_UTRANCTRL_GetSpecTdMccListNum(),
                                             NAS_UTRANCTRL_GetSpecTdMccList(),
                                             pstGuPlmnIdList);

    PS_MEM_FREE(WUEPS_PID_MMC, pstGuPlmnIdList);

    /* 进入MMC状态机处理 */
    return VOS_FALSE;
}
#endif


VOS_UINT32 NAS_UTRANCTRL_RcvTdPlmnSrchCnf_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    RRMM_PLMN_SEARCH_CNF_STRU          *pstSrchCnfMsg = VOS_NULL_PTR;

    VOS_UINT32                          ulIsOtherModemPlmnInTdMccList;
    NAS_MML_PLMN_ID_STRU               *pstOtherModemPlmnId = VOS_NULL_PTR;

    NAS_UTRANCTRL_UTRAN_MODE_ENUM_UINT8 enUtranMode;

    enUtranMode     = NAS_UTRANCTRL_GetCurrUtranMode();

    pstSrchCnfMsg   = (RRMM_PLMN_SEARCH_CNF_STRU *)pstMsg;

    /* 如果当前UTRANCTRL模的等指定搜网回复时状态定时器运行则停止定时器 */
    if (NAS_MMC_TIMER_STATUS_RUNING != NAS_UTRANCTRL_GetTimerStatus(TI_NAS_UTRANCTRL_WAIT_TD_PLMN_SEARCH_CNF))
    {
        NAS_NORMAL_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTdPlmnSrchCnf_Main TI_NAS_UTRANCTRL_WAIT_TD_PLMN_SEARCH_CNF NOT RUNNING");
        return VOS_FALSE;
    }

    NAS_UTRANCTRL_StopTimer(TI_NAS_UTRANCTRL_WAIT_TD_PLMN_SEARCH_CNF);

    /* 降圈复杂度 */
    if (VOS_FALSE == NAS_UTRANCTRL_IsNeedProcGuPlmnSearchCnf(NAS_MML_NET_RAT_TYPE_WCDMA, pstSrchCnfMsg))
    {
        NAS_NORMAL_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTdPlmnSrchCnf_Main: No need process in UTRANCTRL, Clear Buffered Msg and enter MMC FSM");

        /* 列表搜或者PrefBand搜索需要更新 enRoamingZone 标志 */
        if (RRC_PLMN_SEARCH_RLT_USER_LIST == pstSrchCnfMsg->ulSearchRlt)
        {
            /* 指定搜网失败根据搜网列表更新中国网络标记 */
            NAS_UTRANCTRL_UpdateRoamZoneRelatedFlags(NAS_UTRANCTRL_GetSpecTdMccListNum(),
                                                     NAS_UTRANCTRL_GetSpecTdMccList(),
                                                     &(pstSrchCnfMsg->PlmnIdList));
        }
        /* 清除缓存的搜网请求消息 */
        NAS_UTRANCTRL_ClearBufferedSndUtranReqMsg();

        /* 进入MMC状态机处理 */
        return VOS_FALSE;
    }

    /* 自动切换模式下，CSFB标志存在时，并且当前是TDD模式，不再搜W */
    if ((VOS_TRUE                       == NAS_MML_IsCsfbServiceStatusExist())
     && (NAS_UTRANCTRL_UTRAN_MODE_TDD   == enUtranMode))
    {
        NAS_NORMAL_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTdPlmnSrchCnf_Main: CSFB and FDD mode, Clear Buffered Msg and enter MMC FSM");
        /* 清除缓存的搜网请求消息 */
        NAS_UTRANCTRL_ClearBufferedSndUtranReqMsg();

        /* 进入MMC状态机处理 */
        return VOS_FALSE;
    }

    /* 根据搜网列表更新中国网络标记 */
    NAS_UTRANCTRL_UpdateRoamZoneRelatedFlags(NAS_UTRANCTRL_GetSpecTdMccListNum(),
                                             NAS_UTRANCTRL_GetSpecTdMccList(),
                                             &(pstSrchCnfMsg->PlmnIdList));

    /* 根据用户漫游的区域设置utran工作模式 */
    if (NAS_UTRANCTRL_ROAM_ZONE_CHINA_MAINLAND == NAS_UTRANCTRL_GetRoamingZone())
    {
        NAS_NORMAL_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTdPlmnSrchCnf_Main: Set to NAS_UTRANCTRL_UTRAN_MODE_TDD");
        NAS_UTRANCTRL_SetCurrUtranMode(NAS_UTRANCTRL_UTRAN_MODE_TDD);

        /* 进入MMC状态机处理 */
        return VOS_FALSE;
    }

    pstOtherModemPlmnId = NAS_MMC_GetOtherModemPlmnId();
    ulIsOtherModemPlmnInTdMccList = NAS_MML_IsMccInDestMccList(pstOtherModemPlmnId->ulMcc,
                                                NAS_UTRANCTRL_GetSpecTdMccListNum(),
                                                NAS_UTRANCTRL_GetSpecTdMccList());

    if ((VOS_TRUE == NAS_MMC_GetUtranSkipWPlmnSearchFlag())
     && (VOS_TRUE == ulIsOtherModemPlmnInTdMccList))
    {
        NAS_NORMAL_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTdPlmnSrchCnf_Main: Other modem registered on TDD NW, clear buffer and enter MMC FSM");
        /* 如果副卡已驻留中国网络跳过搜w网络，清除缓存的搜网请求消息 */
        NAS_UTRANCTRL_ClearBufferedSndUtranReqMsg();

        /* 进入MMC状态机处理 */
        return VOS_FALSE;
    }

    /* 为了降低功耗，history搜和pref band搜fail时只更新标志，但W和TD不交替搜 */
    if ((RRC_PLMN_SEARCH_RLT_HISTORY_FAIL   == pstSrchCnfMsg->ulSearchRlt)
     || (RRC_PLMN_SEARCH_RLT_PREF_BAND_FAIL == pstSrchCnfMsg->ulSearchRlt))
    {
        NAS_UTRANCTRL_ClearBufferedSndUtranReqMsg();

        /* 进入MMC状态机处理 */
        return VOS_FALSE;
    }

    /* 切换到适配模块的选网状态机进行处理 */
    NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_PLMN_SELECTION);

    NAS_NORMAL_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTdPlmnSrchCnf_Main: enter NAS_UTRANCTRL_FSM_PLMN_SELECTION");

    /* tdrrc的搜网回复消息在适配模块的选网状态机进行处理 */
    return VOS_TRUE;
}



VOS_UINT32 NAS_UTRANCTRL_RcvTiWaitTdPlmnSearchCnfExpired_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_UTRANCTRL_ENTRY_MSG_STRU                           *pstBufferEntryMsg = VOS_NULL_PTR;
    NAS_UTRANCTRL_UTRAN_SWITCH_MODE_ENUM_UINT8              enCurUtranSwitchMode;
    NAS_UTRANCTRL_UTRAN_MODE_ENUM_UINT8                     enUtranMode;
    RRC_PLMN_SEARCH_TYPE_ENUM_UINT32                        enPlmnSearchType;

    enPlmnSearchType    = RRC_PLMN_SEARCH_TYPE_BUTT;

    enUtranMode       = NAS_UTRANCTRL_GetCurrUtranMode();

    /* 获得当前缓存的缓冲区地址 */
    pstBufferEntryMsg = NAS_UTRANCTRL_GetBufferUtranSndMmcMsgAddr();

    enCurUtranSwitchMode = NAS_UTRANCTRL_GetCurUtranSwitchMode();

    /* 异常打印 */
    NAS_WARNING_LOG(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitTdPlmnSearchCnfExpired_Main: ENTERED");

    /* 自动切换模式下，CSFB标志存在时，并且当前是TDD模式，不再搜W */
    if ((VOS_TRUE                       == NAS_MML_IsCsfbServiceStatusExist())
     && (NAS_UTRANCTRL_UTRAN_MODE_TDD   == enUtranMode))
    {
        /* 构造指定搜网回复失败消息到内存的缓存区域 */
        NAS_UTRANCTRL_BuildRrMmPlmnSearchCnfFailMsg(WUEPS_PID_WRR,
                                                    WUEPS_PID_MMC,
                                                    NAS_UTRANCTRL_GetBufferedPlmnSearchReqType(),
                                                   (RRMM_PLMN_SEARCH_CNF_STRU *)pstBufferEntryMsg->aucEntryMsgBuffer);

        /* 保存构造指定搜网回复失败消息事件类型 */
        pstBufferEntryMsg->ulEventType = NAS_UTRANCTRL_GetMsgEventType( (struct MsgCB *)pstBufferEntryMsg->aucEntryMsgBuffer );

        /* 设置需要替换入口消息标记 */
        NAS_UTRANCTRL_SetReplaceMmcMsgFlg(VOS_TRUE);

        /* 当前消息进入后续MMC状态机处理 */
        return VOS_FALSE;
    }


    /* 为了省功耗，history和pref band搜时W与TD不交替搜 */
    enPlmnSearchType    = NAS_UTRANCTRL_GetBufferedPlmnSearchReqType();
    if ((RRC_PLMN_SEARCH_HISTORY == enPlmnSearchType)
     || (RRC_PLMN_SEARCH_PREF_BAND == enPlmnSearchType))
    {
        NAS_NORMAL_LOG1(WUEPS_PID_MMC, "NAS_UTRANCTRL_RcvTiWaitTdPlmnSearchCnfExpired_Main: enPlmnSearchType is ", enPlmnSearchType);

        /* 构造W搜网回复定时器超时的消息到MMC处理 */
        NAS_UTRANCTRL_BuildTimerExpireMsg(WUEPS_PID_MMC,
                                          TI_NAS_MMC_WAIT_WAS_PLMN_SEARCH_CNF,
                                          (REL_TIMER_MSG *)pstBufferEntryMsg->aucEntryMsgBuffer);

        /* 保存构造指定搜网回复失败消息事件类型 */
        pstBufferEntryMsg->ulEventType = NAS_UTRANCTRL_GetMsgEventType( (struct MsgCB *)pstBufferEntryMsg->aucEntryMsgBuffer );

        /* 设置需要替换入口消息标记 */
        NAS_UTRANCTRL_SetReplaceMmcMsgFlg(VOS_TRUE);

        /* 当前消息进入后续MMC状态机处理 */
        return VOS_FALSE;
    }


    /* 如果当前为UTRAN MODE AUTO SWITCH模式，则需要启动UTRANCTRL模块的搜网状态机 */
    if ((NAS_UTRANCTRL_ROAM_ZONE_CHINA_MAINLAND != NAS_UTRANCTRL_GetRoamingZone())
     && (NAS_UTRANCTRL_UTRAN_SWITCH_MODE_AUTO == enCurUtranSwitchMode))
    {
        /* 切换到适配模块的选网状态机进行处理 */
        NAS_UTRANCTRL_SwitchCurrFsmCtx(NAS_UTRANCTRL_FSM_PLMN_SELECTION);

        /* 进入适配层选网状态机处理 */
        return VOS_TRUE;
    }

    /* 构造指定搜网回复失败消息到内存的缓存区域 */
    NAS_UTRANCTRL_BuildRrMmPlmnSearchCnfFailMsg(WUEPS_PID_WRR,
                                                WUEPS_PID_MMC,
                                                enPlmnSearchType,
                                               (RRMM_PLMN_SEARCH_CNF_STRU *)pstBufferEntryMsg->aucEntryMsgBuffer);

    /* 保存构造指定搜网回复失败消息事件类型 */
    pstBufferEntryMsg->ulEventType = NAS_UTRANCTRL_GetMsgEventType( (struct MsgCB *)pstBufferEntryMsg->aucEntryMsgBuffer );

    /* 设置需要替换入口消息标记 */
    NAS_UTRANCTRL_SetReplaceMmcMsgFlg(VOS_TRUE);

    /* 当前消息进入后续MMC状态机处理 */
    return VOS_FALSE;
}


VOS_UINT32 NAS_UTRANCTRL_RcvMmcInterPlmnSearchReq_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MMC_INTER_PLMN_SEARCH_REQ_STRU                     *pstInterPlmnSearchReq;
    NAS_MMC_PLMN_SEARCH_SCENE_ENUM_UINT32                   enPlmnSearchScene;

    pstInterPlmnSearchReq = (NAS_MMC_INTER_PLMN_SEARCH_REQ_STRU*)pstMsg;

    enPlmnSearchScene     = pstInterPlmnSearchReq->enPlmnSearchScene;

    if (NAS_UTRANCTRL_UTRAN_SWITCH_MODE_AUTO == NAS_UTRANCTRL_GetCurUtranSwitchMode())
    {
        NAS_UTRANCTRL_SetSearchedSpecTdMccFlg(VOS_FALSE);

        /* 仅考虑 Available Timer Expired (Except Ncell) scenes 触发的选网才重置 Roaming zone，RF enable scene */
        /* 因为在之前已经修改，所以不需要修改，其他场景保持值不变，保证接下来不会去搜 TD 或者 Wcdma */
        if ((VOS_FALSE == NAS_MML_IsUsimStausValid())
         || (NAS_MMC_PLMN_SEARCH_SCENE_AVAILABLE_TIMER_EXPIRED == enPlmnSearchScene)
         || (NAS_MMC_PLMN_SEARCH_SCENE_AVAILABLE_TIMER_EXPIRED_HISTORY_PLMN_SEARCH == enPlmnSearchScene)
         || (NAS_MMC_PLMN_SEARCH_SCENE_AVAILABLE_TIMER_EXPIRED_PREFBAND_PLMN_SEARCH == enPlmnSearchScene))
        {
            NAS_UTRANCTRL_SetRoamingZone(NAS_UTRANCTRL_ROAM_ZONE_UNKNOWN);
        }
    }

    NAS_UTRANCTRL_SetSwithOnPlmnSearchFlag(VOS_FALSE);

    /* 当前消息进入后续MMC状态机处理 */
    return VOS_FALSE;
}


VOS_UINT32 NAS_UTRANCTRL_RcvMmcInterAnycellSearchReq_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    if (NAS_UTRANCTRL_UTRAN_SWITCH_MODE_AUTO == NAS_UTRANCTRL_GetCurUtranSwitchMode())
    {
        NAS_UTRANCTRL_SetSearchedSpecTdMccFlg(VOS_FALSE);
        NAS_UTRANCTRL_SetRoamingZone(NAS_UTRANCTRL_ROAM_ZONE_UNKNOWN);
    }

    NAS_UTRANCTRL_SetSwithOnPlmnSearchFlag(VOS_FALSE);

    /* 当前消息进入后续MMC状态机处理 */
    return VOS_FALSE;
}


VOS_UINT32 NAS_UTRANCTRL_RcvMmcInterPlmnListReq_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    if (NAS_UTRANCTRL_UTRAN_SWITCH_MODE_AUTO == NAS_UTRANCTRL_GetCurUtranSwitchMode())
    {
        NAS_UTRANCTRL_SetSearchedSpecTdMccFlg(VOS_FALSE);
        NAS_UTRANCTRL_SetRoamingZone(NAS_UTRANCTRL_ROAM_ZONE_UNKNOWN);
    }

    NAS_UTRANCTRL_SetSwithOnPlmnSearchFlag(VOS_FALSE);

    /* 当前消息进入后续MMC状态机处理 */
    return VOS_FALSE;
}




VOS_UINT32 NAS_UTRANCTRL_RcvTiHplmnTimerExpired_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    /* 消息未被处理完成需继续处理 */
    return VOS_FALSE;
}


VOS_UINT32 NAS_UTRANCTRL_RcvTiTryingHighPrioPlmnSearchExpired_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    /* 消息未被处理完成需继续处理 */
    return VOS_FALSE;
}


/*lint -restore */


#endif  /* END OF #if (FEATURE_ON == FEATURE_UE_MODE_TDS) */

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
