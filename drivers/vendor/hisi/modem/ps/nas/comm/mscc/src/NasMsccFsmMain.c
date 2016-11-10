


/*****************************************************************************
  1 头文件包含
*****************************************************************************/

#include "NasMsccFsmMain.h"
#include "NasMsccFsmMainTbl.h"
#include "NasMsccCtx.h"
#include "NasMsccSndInternalMsg.h"
#include "NasMsccTimerMgmt.h"
#include "NasMsccSndMma.h"
#include "PsLogdef.h"
#include "NasMsccSndXsd.h"
#include "NasMsccSndHsd.h"
#include "NasMsccSndMmc.h"
#include "NasComm.h"
#include "NasMsccComFunc.h"
#include "NasMsccMlplMsplApi.h"
#include "NasMsccProcNvim.h"
#include "NasMsccFsmSysCfg.h"

#include "NasMsccSndRrm.h"
#include "NasUtranCtrlInterface.h"
#if (FEATURE_ON == FEATURE_TDS_WCDMA_DYNAMIC_LOAD)
#include "NasDynLoadApi.h"
#include "NasDynLoad.h"
#endif

#include "NasErrorLogCdma.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_NAS_MSCC_FSM_MAIN_C


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
#ifdef DMT
    VOS_UINT32 g_ulDisableAutoSysAcq;
#endif
/*****************************************************************************
  3 函数定义
*****************************************************************************/






VOS_UINT32 NAS_MSCC_RcvMmaStartReq_Main_Null(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_ReadNvim();

    /* Start the Switch ON FSM */
    NAS_MSCC_InitFsmL2(NAS_MSCC_FSM_SWITCH_ON);

    return VOS_TRUE;
}




VOS_UINT32 NAS_MSCC_RcvMsccStartCnf_Main_Null(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_MSCC_SWITCH_ON_RSLT_CNF_STRU                  *pstSwitchOnRslt;

    pstSwitchOnRslt                     = (NAS_MSCC_MSCC_SWITCH_ON_RSLT_CNF_STRU *)pstMsg;

    if (NAS_MSCC_SWITCH_ON_RESULT_SUCCESS == pstSwitchOnRslt->enRslt)
    {
        /* Send Switch On Cnf with Success result to MMA */
        NAS_MSCC_SndMmaSwitchOnRsltCnf(NAS_MSCC_PIF_START_RESULT_SUCC);

        /* Set next state to Deactive */
        NAS_MSCC_SetCurrFsmState(NAS_MSCC_L1_STA_DEACTIVE);

#if ((FEATURE_ON == FEATURE_UE_MODE_CDMA) && (FEATURE_ON == FEATURE_DSDS))
        /* 主动上报当前的是CL mode还是GUL mode */
        NAS_MSCC_SndRrmRatCombModeInd();
#endif

#if (FEATURE_ON == FEATURE_TDS_WCDMA_DYNAMIC_LOAD)
        /* 动态加载只有在TDD/FDD同时存在时才有意义，如果都不支持的话，动态加载就没有意义 */
        if (VOS_TRUE == NAS_DYNLOAD_GetEnableDynloadTWFlg())
        {
            NAS_MSCC_ProcDynamicLoadAfterStartCnf_Main();
        }
#endif

    }
    else
    {
        /* 开机失败后mma不会给mscc发送关机请求, mscc需要在此清除mspl/mlpl解析申请的内存 */
        NAS_MSCC_InitCtx(NAS_MSCC_INIT_CTX_POWEROFF);

        /* Send Switch On Cnf with Fail result to MMA */
        NAS_MSCC_SndMmaSwitchOnRsltCnf(NAS_MSCC_PIF_START_RESULT_FAIL);

        NAS_TRACE_HIGH("NAS_MSCC_RcvMsccStartCnf_Main_Null: Switch ON result Failed!");
    }

    return VOS_TRUE;

}



VOS_UINT32 NAS_MSCC_RcvMmaStartReq_Main_Deactive(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_RcvMmaStartReq_Main_Deactive: Directly respond with START CNF msg");
    /* Send Switch On Cnf with Success result to MMA */
    NAS_MSCC_SndMmaSwitchOnRsltCnf(NAS_MSCC_PIF_START_RESULT_SUCC);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmaPowerOffReq_Main_Deactive(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* Start the Power Off FSM */
    NAS_MSCC_InitFsmL2(NAS_MSCC_FSM_POWER_OFF);

    return VOS_TRUE;
}




VOS_UINT32 NAS_MSCC_RcvMmaSysAcquireReq_Main_Deactive(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
#ifdef DMT
    if (VOS_TRUE == g_ulDisableAutoSysAcq)
    {
        /* For ST testing purpose: Test sys cfg handling in DEACTIVE state */
        return VOS_TRUE;
    }
#endif

    NAS_MSCC_PerformSystemAcquireUponPowerOn_Main(pstMsg);

    return VOS_TRUE;
}




VOS_UINT32 NAS_MSCC_RcvMsccPowerOffCnf_Main_Deactive(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_MSCC_POWER_OFF_RSLT_CNF_STRU                      *pstPowerOffRslt;

    pstPowerOffRslt = (NAS_MSCC_MSCC_POWER_OFF_RSLT_CNF_STRU *)pstMsg;

    NAS_MSCC_ProcPowerOffResult_Main(pstPowerOffRslt->enRslt);

    return VOS_TRUE;

}



VOS_UINT32 NAS_MSCC_RcvMmaPowerOffReq_Main_Active(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    /* Start the Power OFF FSM  */
    NAS_MSCC_InitFsmL2(NAS_MSCC_FSM_POWER_OFF);

    return VOS_TRUE;

}

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)


VOS_VOID NAS_MSCC_ProcSrlteHsdSysAcqIndWithSrchNoRf_Main_Active(
    HSD_MSCC_SYSTEM_ACQUIRE_END_IND_STRU                   *pstHsdSysAcqInd
)
{
    VOS_UINT16                                              usRatNum;
    NAS_MSCC_PIF_NET_RAT_TYPE_ENUM_UINT8                    aenRat[NAS_MSCC_PIF_MAX_RAT_NUM];

    /***********************************************************************************************
     * 如果是搜网失败，则power save HRPD, 后续RF资源可用后，由MSCC触发搜网；
     **********************************************************************************************/
    NAS_MSCC_SndMmaHrpdServiceStatusInd(MSCC_MMA_HRPD_SERVICE_STATUS_NO_SERVICE,
                                        VOS_FALSE,
                                        VOS_FALSE,
                                        MSCC_MMA_SESSION_RELEASE_TYPE_BUTT);

#if (FEATURE_PTM == FEATURE_ON)
    NAS_ERRLOG_CCB_SetHrpdOosCause(NAS_ERR_LOG_HSM_SESSION_NEG_RSLT_ENUM_BUTT + pstHsdSysAcqInd->enRslt);
#endif

    /* 向MMA上报无服务，和MMA保持服务状态一致，此处也设置无服务 */
    NAS_MSCC_SetCurrHrpdServiceStatus(NAS_MSCC_SERVICE_STATUS_NO_SERVICE);

    if (VOS_TRUE  == NAS_MSCC_IsNeedPerformCLSysAcq())
    {
        NAS_MSCC_SetPrioSrchLastRatFlag(VOS_TRUE);

        NAS_MEM_SET_S(&aenRat[0],
                      sizeof(NAS_MSCC_PIF_NET_RAT_TYPE_ENUM_UINT8) * NAS_MSCC_PIF_MAX_RAT_NUM,
                      0x00,
                      sizeof(NAS_MSCC_PIF_NET_RAT_TYPE_ENUM_UINT8) * NAS_MSCC_PIF_MAX_RAT_NUM);

        usRatNum = 1;
        aenRat[0] = NAS_MSCC_PIF_NET_RAT_TYPE_HRPD;

        NAS_MSCC_SndInternalPowerSaveReq(usRatNum, &aenRat[0]);

        /* 1X数据业务造成的NO RF，需要启动available定时器，确保后续可以尝试搜LTE */
        if (VOS_TRUE == NAS_MSCC_IsUe1xDataServiceExist())
        {
            if (VOS_TRUE == NAS_MSCC_StartTimer(TI_NAS_MSCC_AVAILABLE_TIMER, NAS_MSCC_GetNextAvailableTimerLength_L1Main()))
            {
                NAS_MSCC_Start1xServiceClSysAcqPhaseOneTotalTimer_L1Main();
            }
        }
    }
}


VOS_VOID NAS_MSCC_ProcHsdSysAcqIndWithOtheFailure_Main_Active(
    HSD_MSCC_SYSTEM_ACQUIRE_END_IND_STRU                   *pstHsdSysAcqInd
)
{
    NAS_MSCC_SndMmaHrpdServiceStatusInd(MSCC_MMA_HRPD_SERVICE_STATUS_NO_SERVICE,
                                        VOS_FALSE,
                                        VOS_FALSE,
                                        MSCC_MMA_SESSION_RELEASE_TYPE_BUTT);

#if (FEATURE_PTM == FEATURE_ON)
    NAS_ERRLOG_CCB_SetHrpdOosCause(NAS_ERR_LOG_HSM_SESSION_NEG_RSLT_ENUM_BUTT + pstHsdSysAcqInd->enRslt);
#endif

    /* 向MMA上报无服务，和MMA保持服务状态一致，此处也设置无服务 */
    NAS_MSCC_SetCurrHrpdServiceStatus(NAS_MSCC_SERVICE_STATUS_NO_SERVICE);

    if (VOS_TRUE == NAS_MSCC_IsNeedPerformCLSysAcq())
    {
        if (NAS_MSCC_PIF_ACQUIRED_RESULT_NO_RF == pstHsdSysAcqInd->enRslt)
        {
            NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_HRPD_LOST_NO_RF, NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_HRPD_LOST_NO_RF));
        }
        else
        {
            NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_HRPD_LOST, NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_HRPD_LOST));
        }

        return;
    }

    if (VOS_TRUE == NAS_MSCC_IsSpecRatSupported(NAS_MSCC_PIF_NET_RAT_TYPE_LTE))
    {
        NAS_MSCC_SndMmaSystemAcquireEndInd(MSCC_MMA_SYSTEM_ACQUIRE_RESULT_NO_SERVICE,
                                           MSCC_MMA_ACQ_SYS_TYPE_DO_LTE,
                                           MSCC_MMA_CL_SYSTEM_ACQUIRE_RESULT_TYPE_BUTT);
    }
    else
    {
        NAS_MSCC_SndMmaSystemAcquireEndInd(MSCC_MMA_SYSTEM_ACQUIRE_RESULT_NO_SERVICE,
                                           MSCC_MMA_ACQ_SYS_TYPE_DO,
                                           MSCC_MMA_CL_SYSTEM_ACQUIRE_RESULT_TYPE_BUTT);
    }
}


VOS_VOID NAS_MSCC_ProcHsdSysAcqIndWithSrchNoRf_Main_Active(
    HSD_MSCC_SYSTEM_ACQUIRE_END_IND_STRU                   *pstHsdSysAcqInd
)
{
    /* 如果SRLTE */
    if (VOS_TRUE == NAS_MSCC_IsSrlte())
    {
        NAS_MSCC_ProcSrlteHsdSysAcqIndWithSrchNoRf_Main_Active(pstHsdSysAcqInd);

        return;
    }

    NAS_MSCC_ProcHsdSysAcqIndWithOtheFailure_Main_Active(pstHsdSysAcqInd);
}



VOS_UINT32 NAS_MSCC_RcvHsdSysAcqInd_Main_Active(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    HSD_MSCC_SYSTEM_ACQUIRE_END_IND_STRU                   *pstHsdSysAcqInd = VOS_NULL_PTR;

    pstHsdSysAcqInd = (HSD_MSCC_SYSTEM_ACQUIRE_END_IND_STRU *)pstMsg;

    switch (pstHsdSysAcqInd->enRslt)
    {
        case NAS_MSCC_PIF_ACQUIRED_RESULT_SUCC:
            {
                if (VOS_TRUE == NAS_MSCC_IsSpecRatSupported(NAS_MSCC_PIF_NET_RAT_TYPE_LTE))
                {
                    NAS_MSCC_SndMmaSystemAcquireEndInd(MSCC_MMA_SYSTEM_ACQUIRE_RESULT_NORMAL_SERVICE,
                                                       MSCC_MMA_ACQ_SYS_TYPE_DO_LTE,
                                                       MSCC_MMA_CL_SYSTEM_ACQUIRE_RESULT_TYPE_BUTT);
                }
                else
                {
                    NAS_MSCC_SndMmaSystemAcquireEndInd(MSCC_MMA_SYSTEM_ACQUIRE_RESULT_NORMAL_SERVICE,
                                                       MSCC_MMA_ACQ_SYS_TYPE_DO,
                                                       MSCC_MMA_CL_SYSTEM_ACQUIRE_RESULT_TYPE_BUTT);
                }
            }
            break;

        case NAS_MSCC_PIF_ACQUIRED_RESULT_REG_NO_RF:
            {
#if (FEATURE_PTM == FEATURE_ON)
                NAS_ERRLOG_CCB_SetHrpdOosCause(NAS_ERR_LOG_HSM_SESSION_NEG_RSLT_ENUM_BUTT + pstHsdSysAcqInd->enRslt);
#endif
            }
            break;

        case NAS_MSCC_PIF_ACQUIRED_RESULT_NO_RF:
            NAS_MSCC_ProcHsdSysAcqIndWithSrchNoRf_Main_Active(pstHsdSysAcqInd);
            break;

        default:
            NAS_MSCC_ProcHsdSysAcqIndWithOtheFailure_Main_Active(pstHsdSysAcqInd);
            break;
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvInternalSysAcqReq_Main_Active(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_SndMmaSystemAcquireStartInd(MSCC_MMA_ACQ_SYS_TYPE_DO_LTE);

    /* 启动L2 System Acquire状态机 */
    NAS_MSCC_InitFsmL2(NAS_MSCC_FSM_SYS_ACQ);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmcMmssLteUnavailableInd_Main_Active(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_MMSS_LTE_UNAVAILABLE_IND_STRU                 *pstLteUnavailInd;
    NAS_MSCC_SYS_ACQ_SCENE_ENUM_UINT32                      enSysAcqScene;
    VOS_UINT16                                              usRatNum;
    NAS_MSCC_PIF_NET_RAT_TYPE_ENUM_UINT8                    aenRat[NAS_MSCC_PIF_MAX_RAT_NUM];

    pstLteUnavailInd = (MMC_MSCC_MMSS_LTE_UNAVAILABLE_IND_STRU *)pstMsg;

    /* LTE处于从模,依赖于available定时器超时搜L,丢弃该消息 */
    if (VOS_FALSE == NAS_MSCC_Get3gppActiveFlg())
    {
        return VOS_TRUE;
    }

    /* SYSCFG触发的detach不需要立即搜DO，SYSCFG流程完成后会触发搜索 */
    if ((MSCC_MMC_MMSS_LTE_UNAVIALBLE_REASON_BUTT               == pstLteUnavailInd->enReason)
     || (MSCC_MMC_MMSS_LTE_UNAVIALBLE_REASON_RAT_OR_BAND_DETACH == pstLteUnavailInd->enReason))
    {
        return VOS_TRUE;
    }

    /* 不满足搜网条件直接退出 */
    if (VOS_FALSE == NAS_MSCC_IsNeedPerformCLSysAcq())
    {
        return VOS_TRUE;
    }

    /***********************************************************************************************
     * 如果当前版本为SRLTE, 1X连接状态为连接态，如果当前驻留在LTE, 则挂起L，
     * 同时记录当前RAT驻留标志为LTE. 等待后续1X连接态释放后，再搜索LTE.
     **********************************************************************************************/
    if (VOS_TRUE == NAS_MSCC_IsSrlte())
    {
        if (MSCC_MMC_MMSS_LTE_UNAVIALBLE_REASON_REG_NO_RF == pstLteUnavailInd->enReason)
        {
            return VOS_TRUE;
        }

        if (MSCC_MMC_MMSS_LTE_UNAVIALBLE_REASON_NO_RF == pstLteUnavailInd->enReason)
        {
            /* 记录上次RAT驻留标志 */
            NAS_MSCC_SetPrioSrchLastRatFlag(VOS_TRUE);

            NAS_MEM_SET_S(&aenRat[0],
                          sizeof(NAS_MSCC_PIF_NET_RAT_TYPE_ENUM_UINT8) * NAS_MSCC_PIF_MAX_RAT_NUM,
                          0x00,
                          sizeof(NAS_MSCC_PIF_NET_RAT_TYPE_ENUM_UINT8) * NAS_MSCC_PIF_MAX_RAT_NUM);

            usRatNum = 1;
            aenRat[0] = NAS_MSCC_PIF_NET_RAT_TYPE_LTE;

            NAS_MSCC_SndInternalPowerSaveReq(usRatNum, &aenRat[0]);

            return VOS_TRUE;
        }
    }

    /* 注册被拒卡无效或detach PS域或NO RF场景,此轮搜索不尝试LTE */
    if ((VOS_FALSE                                             == NAS_MSCC_GetSimPsRegStatus())
     || (MSCC_MMC_MMSS_LTE_UNAVIALBLE_REASON_USER_DETACH       == pstLteUnavailInd->enReason)
     || (MSCC_MMC_MMSS_LTE_UNAVIALBLE_REASON_SRV_DOMAIN_DETACH == pstLteUnavailInd->enReason)
     || (MSCC_MMC_MMSS_LTE_UNAVIALBLE_REASON_NO_RF             == pstLteUnavailInd->enReason))
    {
        enSysAcqScene = NAS_MSCC_SYS_ACQ_SCENE_LTE_NOT_AVAILABLE;
    }
    /* 注册被拒或丢网场景,此轮搜索按照LTE单模掉网场景处理,先搜索一轮DO后再尝试LTE */
    else
    {
        enSysAcqScene = NAS_MSCC_SYS_ACQ_SCENE_LTE_LOST;
    }

    /* 用户detach PS域的场景,考虑APP切换APN会使用该命令,先detach PS域,然后切换完成后会attach
       PS 域,为避免切换过程中立即去3G,此处等待几秒让APP attach PS域 */
    if (MSCC_MMC_MMSS_LTE_UNAVIALBLE_REASON_USER_DETACH == pstLteUnavailInd->enReason)
    {
        NAS_MSCC_StartTimer(TI_NAS_MSCC_AVAILABLE_TIMER,  TI_NAS_MSCC_AVAILABL_TIMER_WAIT_APP_ATTACH_PS_LEN);

        return VOS_TRUE;
    }

    NAS_MSCC_SndInternalSysAcqReq(enSysAcqScene, NAS_MSCC_GetSysAcqLteSrchType_L1Main(enSysAcqScene));

    return VOS_TRUE;
}


VOS_VOID NAS_MSCC_ProcBsrFailureRslt_Main_Active(
    NAS_MSCC_MSCC_BSR_RSLT_CNF_STRU    *pstBsrSearchRsltCnf
)
{
    VOS_UINT8                           uc3gppActiveFlg;

    if (NAS_MSCC_BSR_NEED_ACQ_SYS == pstBsrSearchRsltCnf->enIsNeedAcqSys)
    {
        NAS_MSCC_ResetBsrRelatedCtrlInfo();

        uc3gppActiveFlg = NAS_MSCC_Get3gppActiveFlg();

        if (VOS_TRUE == NAS_MSCC_IsNeedPerformCLSysAcq())
        {
            /* BG搜网中回LTE时如果注册被拒卡无效,需要通过搜网流程切到HRPD下去 */
            if (VOS_TRUE == uc3gppActiveFlg)
            {
                /* 卡无效或者用户detach ps域，只搜hrpd */
                if ((VOS_TRUE == NAS_MSCC_GetSimPsRegStatus())
                 && (VOS_TRUE == NAS_MSCC_GetEpsAttachAllowFlg()))
                {
                    NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_LTE_LOST, NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_LTE_LOST));
                }
                else
                {
                    NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_LTE_NOT_AVAILABLE, NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_LTE_NOT_AVAILABLE));
                }

                return;
            }

            /* LTE下PS注册被拒卡无效但回HRPD后,后续靠HSD的选网定时器周期性搜网;这里不再搜网 */
            if ((VOS_TRUE  == NAS_MSCC_GetSimPsRegStatus())
             && (VOS_FALSE == uc3gppActiveFlg))
            {
                if (VOS_TRUE == pstBsrSearchRsltCnf->ucHrpdNoRfFlg)
                {
                    NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_HRPD_LOST_NO_RF, NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_HRPD_LOST_NO_RF));
                }
                else
                {
                    NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_HRPD_LOST, NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_HRPD_LOST));
                }

                return;
            }
        }
    }
    else
    {
        if (VOS_TRUE == NAS_MSCC_IsNeedStartBsrTimer())
        {
            /* 虽然目前BSR失败场景不会设置启动Retry Timer标识, 仍然先保留此if分支 */
            if (NAS_MSCC_BSR_NEED_START_RETRY_TIMER == pstBsrSearchRsltCnf->enIsNeedStartRetryTimer)
            {
                (VOS_VOID)NAS_MSCC_StartTimer(TI_NAS_MSCC_PERIOD_TRYING_HIGH_PRI_SYSTEM_SEARCH,
                                             TI_NAS_MSCC_PERIOD_TRYING_HIGH_PRI_SYSTEM_SEARCH_LEN);
            }
            else
            {
                NAS_MSCC_StartBsrTimer();
            }
        }
        else
        {
            NAS_MSCC_ResetBsrRelatedCtrlInfo();
        }
    }

    return;
}


VOS_VOID NAS_MSCC_ProcBsrNoRfRslt_Main_Active(
    NAS_MSCC_MSCC_BSR_RSLT_CNF_STRU    *pstBsrSearchRsltCnf
)
{
    VOS_UINT16                                              usRatNum;
    NAS_MSCC_PIF_NET_RAT_TYPE_ENUM_UINT8                    aenRat[NAS_MSCC_PIF_MAX_RAT_NUM];

    if ((VOS_TRUE                       == NAS_MSCC_IsSrlte())
     && (NAS_MSCC_BSR_RESULT_SRCH_NO_RF == pstBsrSearchRsltCnf->enRslt))
    {
        NAS_MSCC_SetPrioSrchLastRatFlag(VOS_TRUE);

        /***********************************************************************************
         * 如果是搜网NO RF，则去激活当前激活的RAT，等待资源回复后，再发起搜网
         **********************************************************************************/
        NAS_MEM_SET_S(&aenRat[0],
                      sizeof(NAS_MSCC_PIF_NET_RAT_TYPE_ENUM_UINT8) * NAS_MSCC_PIF_MAX_RAT_NUM,
                      0x00,
                      sizeof(NAS_MSCC_PIF_NET_RAT_TYPE_ENUM_UINT8) * NAS_MSCC_PIF_MAX_RAT_NUM);

        if (VOS_TRUE == NAS_MSCC_GetHrpdActiveFlg())
        {
            usRatNum = 1;
            aenRat[0] = NAS_MSCC_PIF_NET_RAT_TYPE_HRPD;

            NAS_MSCC_SndInternalPowerSaveReq(usRatNum, &aenRat[0]);
        }

        if (VOS_TRUE == NAS_MSCC_Get3gppActiveFlg())
        {
            usRatNum = 1;
            aenRat[0] = NAS_MSCC_PIF_NET_RAT_TYPE_LTE;

            NAS_MSCC_SndInternalPowerSaveReq(usRatNum, &aenRat[0]);
        }

        /* 数据业务造成的NO RF，需要启动available定时器，确保后续可以尝试搜LTE */
        if (VOS_TRUE == NAS_MSCC_IsUe1xDataServiceExist())
        {
            if (VOS_TRUE == NAS_MSCC_StartTimer(TI_NAS_MSCC_AVAILABLE_TIMER, NAS_MSCC_GetNextAvailableTimerLength_L1Main()))
            {
                NAS_MSCC_Start1xServiceClSysAcqPhaseOneTotalTimer_L1Main();
            }
        }

        /* if both 1x and hrpd ,lte no service,start scan timer */
        if (NAS_MSCC_SERVICE_STATUS_NO_SERVICE == NAS_MSCC_GetCurr1xServiceStatus())
        {
            /* we start the timer only in CL mode */
            if (VOS_FALSE == NAS_MSCC_IsGULModeConfigured())
            {
                (VOS_VOID)NAS_MSCC_StartTimer(TI_NAS_MSCC_SCAN_TIMER, NAS_MSCC_GetScanTimerLength());
            }
        }

        return;
    }

    /* 按失败流程处理 */
    NAS_MSCC_ProcBsrFailureRslt_Main_Active(pstBsrSearchRsltCnf);
}



VOS_UINT32 NAS_MSCC_RcvInternalBsrRsltCnf_Main_Active(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_MSCC_BSR_RSLT_CNF_STRU    *pstBsrSearchRsltCnf = VOS_NULL_PTR;

    pstBsrSearchRsltCnf = (NAS_MSCC_MSCC_BSR_RSLT_CNF_STRU *)pstMsg;

    if (NAS_MSCC_BSR_RESULT_ABORT != pstBsrSearchRsltCnf->enRslt)
    {
        NAS_MSCC_UpdateBsrSrchCtrlInfoUponBsrSrchFinished();
    }

    switch (pstBsrSearchRsltCnf->enRslt)
    {
        /* BSR驻留到高优先级制式成功 */
        case NAS_MSCC_BSR_RESULT_SUCCESS:
        case NAS_MSCC_BSR_RESULT_L2C:

            if (VOS_TRUE == NAS_MSCC_IsNeedStartBsrTimer())
            {
                NAS_MSCC_StartBsrTimer();
            }
            else
            {
                NAS_MSCC_ResetBsrRelatedCtrlInfo();
            }

            break;

        case NAS_MSCC_BSR_RESULT_FAILURE:

            NAS_MSCC_ProcBsrFailureRslt_Main_Active(pstBsrSearchRsltCnf);

            break;

        case NAS_MSCC_BSR_RESULT_ABORT:

            /* 虽然设置了需要启动BSR Retry Timer标记，但是如果不再满足启动BSR定时器条件，不启动Retry定时器 */
            if (VOS_TRUE == NAS_MSCC_IsNeedStartBsrTimer())
            {
                if (NAS_MSCC_BSR_NEED_START_RETRY_TIMER == pstBsrSearchRsltCnf->enIsNeedStartRetryTimer)
                {
                    (VOS_VOID)NAS_MSCC_StartTimer(TI_NAS_MSCC_PERIOD_TRYING_HIGH_PRI_SYSTEM_SEARCH,
                                                  TI_NAS_MSCC_PERIOD_TRYING_HIGH_PRI_SYSTEM_SEARCH_LEN);
                }
                else
                {
                    NAS_MSCC_StartBsrTimer();
                }
            }
            else
            {
                NAS_MSCC_ResetBsrRelatedCtrlInfo();
            }

            break;

        case NAS_MSCC_BSR_RESULT_REG_NO_RF:
        case NAS_MSCC_BSR_RESULT_SRCH_NO_RF:
            {
                NAS_MSCC_ProcBsrNoRfRslt_Main_Active(pstBsrSearchRsltCnf);
            }
            break;

        default:

            NAS_WARNING_LOG(UEPS_PID_MSCC, "BSR FSM return result is except");

            break;
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvTiBsrTimerExpired_Main_Active(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_ProcBsrStartProcedure_Main();

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvTiPeriodTryingHighPrioNetworkSearchExpired_Main_Active(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_ProcBsrStartProcedure_Main();

    return VOS_TRUE;

}


VOS_VOID NAS_MSCC_ProcInterSysAcqSucc_Main(VOS_VOID)
{


    /* 判断是否需要启动或者重启BSR定时器 */
    if (VOS_TRUE == NAS_MSCC_IsNeedStartBsrTimer())
    {
        if (VOS_TRUE == NAS_MSCC_IsNeedRestartBsrTimer())
        {
            NAS_MSCC_StopTimer(TI_NAS_MSCC_BSR_TIMER);
            NAS_MSCC_StartBsrTimer();
        }
        else
        {
            if (NAS_MSCC_TIMER_STATUS_RUNING != NAS_MSCC_GetTimerStatus(TI_NAS_MSCC_BSR_TIMER))
            {
                NAS_MSCC_StartBsrTimer();
            }
        }
    }


    NAS_MSCC_StopTimer(TI_NAS_MSCC_SCAN_TIMER);

    NAS_MSCC_Reset1xServiceClSysAcqRelatedInfo_L1Main();

    NAS_MSCC_SndMmaSystemAcquireEndInd(MSCC_MMA_SYSTEM_ACQUIRE_RESULT_NORMAL_SERVICE,
                                       MSCC_MMA_ACQ_SYS_TYPE_DO_LTE,
                                       MSCC_MMA_CL_SYSTEM_ACQUIRE_RESULT_TYPE_BUTT);

    return;
}


VOS_VOID NAS_MSCC_ProcInterSysAcqFail_Main(
    NAS_MSCC_MSCC_SYS_ACQ_RSLT_CNF_STRU                    *pstSysAcqRslt
)
{
    VOS_UINT8                           ucSearchLteFLag  = VOS_FALSE;
    VOS_UINT8                           ucSearchHrpdFLag = VOS_FALSE;

    VOS_UINT8                           ucLteNoRfFLag    = VOS_FALSE;
    VOS_UINT8                           ucHrpdNoRfFLag   = VOS_FALSE;

    VOS_UINT32                          ulEpsAttachAllowFlg;

    ulEpsAttachAllowFlg = NAS_MSCC_GetEpsAttachAllowFlg();

    ucSearchLteFLag  = pstSysAcqRslt->ucSearchLteFlag;
    ucSearchHrpdFLag = pstSysAcqRslt->ucSearchHrpdFlag;

    ucLteNoRfFLag    = pstSysAcqRslt->ucLteNoRfFlag;
    ucHrpdNoRfFLag   = pstSysAcqRslt->ucHrpdNoRfFlag;

    /* Lte和Hrpd都尝试搜索后，且是失败的以后，才通知MMA sys acq end */
    if ((VOS_TRUE == ucSearchLteFLag)
     && (VOS_TRUE == ucSearchHrpdFLag))
    {
        if ((VOS_TRUE == ucLteNoRfFLag) && (VOS_TRUE == ucHrpdNoRfFLag))
        {
            NAS_MSCC_SndMmaSystemAcquireEndInd(MSCC_MMA_SYSTEM_ACQUIRE_RESULT_NO_SERVICE,
                                               MSCC_MMA_ACQ_SYS_TYPE_DO_LTE,
                                               MSCC_MMA_CL_SYSTEM_ACQUIRE_RESULT_TYPE_DO_NO_RF_LTE_NO_RF);
        }
        else if (VOS_TRUE == ucLteNoRfFLag)
        {
            NAS_MSCC_SndMmaSystemAcquireEndInd(MSCC_MMA_SYSTEM_ACQUIRE_RESULT_NO_SERVICE,
                                               MSCC_MMA_ACQ_SYS_TYPE_DO_LTE,
                                               MSCC_MMA_CL_SYSTEM_ACQUIRE_RESULT_TYPE_DO_NO_SERVICE_LTE_NO_RF);
        }
        else if (VOS_TRUE == ucHrpdNoRfFLag)
        {
            NAS_MSCC_SndMmaSystemAcquireEndInd(MSCC_MMA_SYSTEM_ACQUIRE_RESULT_NO_SERVICE,
                                               MSCC_MMA_ACQ_SYS_TYPE_DO_LTE,
                                               MSCC_MMA_CL_SYSTEM_ACQUIRE_RESULT_TYPE_DO_NO_RF_LTE_NO_SERVICE);
        }
        else
        {
            NAS_MSCC_SndMmaSystemAcquireEndInd(MSCC_MMA_SYSTEM_ACQUIRE_RESULT_NO_SERVICE,
                                               MSCC_MMA_ACQ_SYS_TYPE_DO_LTE,
                                               MSCC_MMA_CL_SYSTEM_ACQUIRE_RESULT_TYPE_DO_NO_SERVICE_LTE_NO_SERVICE);
        }
    }

    /* LTE注册被拒卡无效或EPS被detach,不再启动available timer,直接搜索HRPD */
    if ((VOS_FALSE == NAS_MSCC_GetSimPsRegStatus())
     || (VOS_FALSE == ulEpsAttachAllowFlg))
    {
        NAS_MSCC_StopTimer(TI_NAS_MSCC_1X_SERVICE_CL_SYSTEM_ACQUIRE_PHASE_ONE_TOTAL_TIMER);

        NAS_MSCC_SndHsdSysAcqReq(MSCC_HSD_ACQUIRE_REASON_OTHER,
                                 0,
                                 VOS_NULL_PTR);

        /* Set the Camp On flag to FALSE */
        NAS_MSCC_SetHrpdCampOnFlag(VOS_FALSE);

        NAS_MSCC_SetHrpdActiveFlg(VOS_TRUE);

        return;
    }

    /* 是否需要启动1x下BSR LTE的定时器 */
    if (VOS_TRUE == NAS_MSCC_IsNeedStart1xBsrLteTimer())
    {
        NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_ProcInterSysAcqFail_Main: start 1x bsr lte timer.");

        NAS_MSCC_StartTimer(TI_NAS_MSCC_1X_BSR_LTE_TIMER, NAS_MSCC_Get1xBsrLteTimerLen());
    }

    if (VOS_TRUE == NAS_MSCC_StartTimer(TI_NAS_MSCC_AVAILABLE_TIMER, NAS_MSCC_GetNextAvailableTimerLength_L1Main()))
    {
        if (VOS_TRUE == NAS_MSCC_Is1xNormalService())
        {
            NAS_MSCC_Start1xServiceClSysAcqPhaseOneTotalTimer_L1Main();
        }
        else
        {
            NAS_MSCC_AddCurOosAvailableTimerStartCount_L1Main();
        }
    }


    /* if both 1x and hrpd ,lte no service,start scan timer */
    if (NAS_MSCC_SERVICE_STATUS_NO_SERVICE == NAS_MSCC_GetCurr1xServiceStatus())
    {
        /* we start the timer only in CL mode */
        if (VOS_FALSE == NAS_MSCC_IsGULModeConfigured())
        {
            (VOS_VOID)NAS_MSCC_StartTimer(TI_NAS_MSCC_SCAN_TIMER, NAS_MSCC_GetScanTimerLength());
        }
    }

    return;
}


VOS_VOID NAS_MSCC_ProcInterSysAcqRegNoRf_Main(
    NAS_MSCC_MSCC_SYS_ACQ_RSLT_CNF_STRU                    *pstSysAcqRslt
)
{
    if (VOS_TRUE == NAS_MSCC_IsNeedStartBsrTimer())
    {
        if (VOS_TRUE == NAS_MSCC_IsNeedRestartBsrTimer())
        {
            NAS_MSCC_StopTimer(TI_NAS_MSCC_BSR_TIMER);
            NAS_MSCC_StartBsrTimer();
        }
        else
        {
            if (NAS_MSCC_TIMER_STATUS_RUNING != NAS_MSCC_GetTimerStatus(TI_NAS_MSCC_BSR_TIMER))
            {
                NAS_MSCC_StartBsrTimer();
            }
        }
    }

    NAS_MSCC_StopTimer(TI_NAS_MSCC_SCAN_TIMER);

    NAS_MSCC_Reset1xServiceClSysAcqRelatedInfo_L1Main();

    /* 如果是注册或session协商造成的NO RF，上报为no service */
    NAS_MSCC_SndMmaSystemAcquireEndInd(MSCC_MMA_SYSTEM_ACQUIRE_RESULT_NO_SERVICE,
                                       MSCC_MMA_ACQ_SYS_TYPE_DO_LTE,
                                       MSCC_MMA_CL_SYSTEM_ACQUIRE_RESULT_TYPE_DO_NO_SERVICE_LTE_NO_SERVICE);
}



VOS_VOID NAS_MSCC_ProcInterSysAcqSrchNoRf_Main(
    NAS_MSCC_MSCC_SYS_ACQ_RSLT_CNF_STRU                    *pstSysAcqRslt
)
{
    VOS_UINT16                                              usRatNum;
    NAS_MSCC_PIF_NET_RAT_TYPE_ENUM_UINT8                    aenRat[NAS_MSCC_PIF_MAX_RAT_NUM];

    /* SRLTE处理 */
    if (VOS_TRUE == NAS_MSCC_IsSrlte())
    {
        if ((VOS_TRUE == pstSysAcqRslt->ucSearchLteFlag)
         && (VOS_TRUE == pstSysAcqRslt->ucSearchHrpdFlag))
        {
            if ((VOS_TRUE == pstSysAcqRslt->ucLteNoRfFlag) && (VOS_TRUE == pstSysAcqRslt->ucHrpdNoRfFlag))
            {
                NAS_MSCC_SndMmaSystemAcquireEndInd(MSCC_MMA_SYSTEM_ACQUIRE_RESULT_NO_SERVICE,
                                                   MSCC_MMA_ACQ_SYS_TYPE_DO_LTE,
                                                   MSCC_MMA_CL_SYSTEM_ACQUIRE_RESULT_TYPE_DO_NO_RF_LTE_NO_RF);
            }
            else if (VOS_TRUE == pstSysAcqRslt->ucLteNoRfFlag)
            {
                NAS_MSCC_SndMmaSystemAcquireEndInd(MSCC_MMA_SYSTEM_ACQUIRE_RESULT_NO_SERVICE,
                                                   MSCC_MMA_ACQ_SYS_TYPE_DO_LTE,
                                                   MSCC_MMA_CL_SYSTEM_ACQUIRE_RESULT_TYPE_DO_NO_SERVICE_LTE_NO_RF);
            }
            else if (VOS_TRUE == pstSysAcqRslt->ucHrpdNoRfFlag)
            {
                NAS_MSCC_SndMmaSystemAcquireEndInd(MSCC_MMA_SYSTEM_ACQUIRE_RESULT_NO_SERVICE,
                                                   MSCC_MMA_ACQ_SYS_TYPE_DO_LTE,
                                                   MSCC_MMA_CL_SYSTEM_ACQUIRE_RESULT_TYPE_DO_NO_RF_LTE_NO_SERVICE);
            }
            else
            {
                NAS_MSCC_SndMmaSystemAcquireEndInd(MSCC_MMA_SYSTEM_ACQUIRE_RESULT_NO_SERVICE,
                                                   MSCC_MMA_ACQ_SYS_TYPE_DO_LTE,
                                                   MSCC_MMA_CL_SYSTEM_ACQUIRE_RESULT_TYPE_DO_NO_SERVICE_LTE_NO_SERVICE);
            }
        }

        /* 搜网造成的NO RF，需要power当前激活的模 */
        NAS_MEM_SET_S(&aenRat[0],
                      sizeof(NAS_MSCC_PIF_NET_RAT_TYPE_ENUM_UINT8) * NAS_MSCC_PIF_MAX_RAT_NUM,
                      0x00,
                      sizeof(NAS_MSCC_PIF_NET_RAT_TYPE_ENUM_UINT8) * NAS_MSCC_PIF_MAX_RAT_NUM);

        if (VOS_TRUE == NAS_MSCC_GetHrpdActiveFlg())
        {
            usRatNum = 1;
            aenRat[0] = NAS_MSCC_PIF_NET_RAT_TYPE_HRPD;

            NAS_MSCC_SndInternalPowerSaveReq(usRatNum, &aenRat[0]);
        }

        if (VOS_TRUE == NAS_MSCC_Get3gppActiveFlg())
        {
            usRatNum = 1;
            aenRat[0] = NAS_MSCC_PIF_NET_RAT_TYPE_LTE;

            NAS_MSCC_SndInternalPowerSaveReq(usRatNum, &aenRat[0]);
        }

        /* 数据业务造成的NO RF，需要启动available定时器，确保后续可以尝试搜LTE */
        if (VOS_TRUE == NAS_MSCC_IsUe1xDataServiceExist())
        {
            if (VOS_TRUE == NAS_MSCC_StartTimer(TI_NAS_MSCC_AVAILABLE_TIMER, NAS_MSCC_GetNextAvailableTimerLength_L1Main()))
            {
                NAS_MSCC_Start1xServiceClSysAcqPhaseOneTotalTimer_L1Main();
            }
        }

        /* if both 1x and hrpd ,lte no service,start scan timer */
        if (NAS_MSCC_SERVICE_STATUS_NO_SERVICE == NAS_MSCC_GetCurr1xServiceStatus())
        {
            /* we start the timer only in CL mode */
            if (VOS_FALSE == NAS_MSCC_IsGULModeConfigured())
            {
                (VOS_VOID)NAS_MSCC_StartTimer(TI_NAS_MSCC_SCAN_TIMER, NAS_MSCC_GetScanTimerLength());
            }
        }

        return;
    }

    /* SVLTE处理 */
    NAS_MSCC_ProcInterSysAcqFail_Main(pstSysAcqRslt);

    return;
}


VOS_UINT32 NAS_MSCC_RcvInterSysAcqRsltCnf_Main_Active(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_MSCC_SYS_ACQ_RSLT_CNF_STRU                    *pstSysAcqRslt = VOS_NULL_PTR;
    VOS_UINT8                                               ucSearchLteFLag  = VOS_FALSE;
    VOS_UINT8                                               ucSearchHrpdFLag = VOS_FALSE;

    pstSysAcqRslt = (NAS_MSCC_MSCC_SYS_ACQ_RSLT_CNF_STRU *)pstMsg;

    ucSearchLteFLag  = pstSysAcqRslt->ucSearchLteFlag;
    ucSearchHrpdFLag = pstSysAcqRslt->ucSearchHrpdFlag;


    if (NAS_MSCC_SYSTEM_ACQUIRED_RESULT_SRCH_NO_RF != pstSysAcqRslt->enRslt)
    {
        NAS_MSCC_SetPrioSrchLastRatFlag(VOS_FALSE);
    }

    switch (pstSysAcqRslt->enRslt)
    {
        case NAS_MSCC_SYSTEM_ACQUIRED_RESULT_SUCCESS:
            {
                NAS_MSCC_ProcInterSysAcqSucc_Main();
            }
            break;

        case NAS_MSCC_SYSTEM_ACQUIRED_RESULT_FAILURE:
            {
                NAS_MSCC_ProcInterSysAcqFail_Main(pstSysAcqRslt);
            }
            break;

        case NAS_MSCC_SYSTEM_ACQUIRED_RESULT_REG_NO_RF:
            {
                NAS_MSCC_ProcInterSysAcqRegNoRf_Main(pstSysAcqRslt);
            }
            break;

        case NAS_MSCC_SYSTEM_ACQUIRED_RESULT_SRCH_NO_RF:
            {
                NAS_MSCC_ProcInterSysAcqSrchNoRf_Main(pstSysAcqRslt);
            }
            break;

        case NAS_MSCC_SYSTEM_ACQUIRED_RESULT_ABORTED:
            {
                NAS_MSCC_SndMmaSystemAcquireEndInd(MSCC_MMA_SYSTEM_ACQUIRE_RESULT_INTERRUPT,
                                                   MSCC_MMA_ACQ_SYS_TYPE_DO_LTE,
                                                   MSCC_MMA_CL_SYSTEM_ACQUIRE_RESULT_TYPE_BUTT);
            }
            break;

        case NAS_MSCC_SYSTEM_ACQUIRED_RESULT_L2C:
            {
                if ((VOS_TRUE == ucSearchLteFLag)
                 && (VOS_TRUE == ucSearchHrpdFLag))
                {
                    NAS_MSCC_SndMmaSystemAcquireEndInd(MSCC_MMA_SYSTEM_ACQUIRE_RESULT_NO_SERVICE,
                                                       MSCC_MMA_ACQ_SYS_TYPE_DO_LTE,
                                                       MSCC_MMA_CL_SYSTEM_ACQUIRE_RESULT_TYPE_DO_NO_SERVICE_LTE_NO_SERVICE);
                }
            }
            break;

        default:
            break;
    }


    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvTiSleepTimerExpired_Main_Deactive(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* here must be CL mode */
    NAS_MSCC_PerformSystemAcquireUponSleepTimerExpired_Main();

    return VOS_TRUE;
}


VOS_VOID NAS_MSCC_ProcSrlteRfAvailableInd_Main_Deactive(VOS_VOID)
{
    switch (NAS_MSCC_GetLastCampOnLTEOrDoRat())
    {
        case NAS_MSCC_PIF_NET_RAT_TYPE_LTE:
            {
                /* 如果上次驻留的是LTE，RF available ind后，按LTE历史搜，再加HRPD+LTE全频搜 */
                NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_LTE_HISTORY_PLUS_LTE_LOST,
                                              NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_LTE_HISTORY_PLUS_LTE_LOST));
            }
            break;

        case NAS_MSCC_PIF_NET_RAT_TYPE_HRPD:
            {
                /* 如果上次驻留的是HRPD，RF available ind后，按HRPD掉网搜 */
                NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_HRPD_RF_AVAILABLE,
                                              NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_HRPD_RF_AVAILABLE));
            }
            break;

        default:
            {
                NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_SLEEP_TIMER_EXPIRED,
                                              NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_SLEEP_TIMER_EXPIRED));
            }
            break;
    }

    return;
}


VOS_VOID NAS_MSCC_ProcRfAvailableInd_Main_Deactive()
{


    /* 关机进入NULL状态，只有进SLEEP模式时，才会进DEACTIVE状态 */
    NAS_MSCC_StopTimer(TI_NAS_MSCC_SLEEP_TIMER);

    NAS_MSCC_SetCurrFsmState(NAS_MSCC_L1_STA_ACTIVE);

    (VOS_VOID)NAS_MSCC_StartTimer(TI_NAS_MSCC_SCAN_TIMER, NAS_MSCC_GetScanTimerLength());

    if (VOS_TRUE == NAS_MSCC_IsSpecRatSupported(NAS_MSCC_PIF_NET_RAT_TYPE_1X))
    {
        NAS_MSCC_SndXsdSysAcqReq(MSCC_XSD_SYS_ACQ_TYPE_NORMAL);

        NAS_MSCC_Set1xCampOnFlag(VOS_FALSE);

        NAS_MSCC_Set1xActiveFlg(VOS_TRUE);
    }

    if (VOS_TRUE == NAS_MSCC_IsNeedPerformCLSysAcq())
    {
        if (VOS_TRUE == NAS_MSCC_IsSrlte())
        {
            NAS_MSCC_ProcSrlteRfAvailableInd_Main_Deactive();
        }
        else
        {
            NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_SLEEP_TIMER_EXPIRED,
                                          NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_SLEEP_TIMER_EXPIRED));
        }

        return;
    }

    /* current support lte, usim card status is avialable and sim ps reg status is true, start lte acquir */
    /* 用接口NAS_MSCC_IsNeedAcqLte代替散开的多个条件判断 */
    if (VOS_TRUE == NAS_MSCC_IsNeedAcqLte())
    {
        NAS_MSCC_SndMmcPlmnSearchReq(NAS_MSCC_PIF_ACQ_REASON_OTHER,
                                     NAS_MSCC_PIF_PLMN_SEARCH_SPEC,
                                     VOS_NULL_PTR,
                                     VOS_NULL_PTR);

        NAS_MSCC_Set3gppCampOnFlag(VOS_FALSE);

        NAS_MSCC_Set3gppActiveFlg(VOS_TRUE);

        return;
    }

    /* Send HSD system acquire request if CDMA EVDO is supported */
    if (VOS_TRUE == NAS_MSCC_IsNeedAcqHrpd())
    {
        NAS_MSCC_SndHsdSysAcqReq(MSCC_HSD_ACQUIRE_REASON_OTHER,
                                 0,
                                 VOS_NULL_PTR);

        NAS_MSCC_SetHrpdCampOnFlag(VOS_FALSE);

        NAS_MSCC_SetHrpdActiveFlg(VOS_TRUE);
    }

    return;
}


VOS_UINT32 NAS_MSCC_RcvMmcMsccRfAvailableInd_Main_Deactive(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_ProcRfAvailableInd_Main_Deactive();

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvHsdMsccRfAvailableInd_Main_Deactive(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    if (VOS_FALSE == NAS_MSCC_IsSpecRatSupported(NAS_MSCC_PIF_NET_RAT_TYPE_1X))
    {
        NAS_MSCC_ProcRfAvailableInd_Main_Deactive();
    }
    else
    {
        NAS_WARNING_LOG(UEPS_PID_MSCC, "NAS_MSCC_RcvHsdMsccRfAvailableInd_Main_Deactive discard hsd rf available ind msg!");
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvXsdMsccRfAvailableInd_Main_Deactive(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_ProcRfAvailableInd_Main_Deactive();

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaSrvAcqReq_Main_Deactive(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_SRV_ACQ_REQ_STRU          *pstSrvAcqReq = VOS_NULL_PTR;

    pstSrvAcqReq = (MMA_MSCC_SRV_ACQ_REQ_STRU *)pstMsg;

    /* 关机进入NULL状态，只有进SLEEP模式时，才会进DEACTIVE状态 */
    NAS_MSCC_StopTimer(TI_NAS_MSCC_SLEEP_TIMER);

    NAS_MSCC_SetCurrFsmState(NAS_MSCC_L1_STA_ACTIVE);

    (VOS_VOID)NAS_MSCC_StartTimer(TI_NAS_MSCC_SCAN_TIMER, NAS_MSCC_GetScanTimerLength());

    if (VOS_TRUE == NAS_MSCC_IsSpecRatSupported(NAS_MSCC_PIF_NET_RAT_TYPE_1X))
    {
        NAS_MSCC_Set1xCampOnFlag(VOS_FALSE);

        NAS_MSCC_Set1xActiveFlg(VOS_TRUE);

        NAS_MSCC_SndXsdSrvAcqReq(pstSrvAcqReq);
    }

    if (VOS_TRUE == NAS_MSCC_IsNeedPerformCLSysAcq())
    {
        NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_SLEEP_TIMER_EXPIRED, NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_SLEEP_TIMER_EXPIRED));

        return VOS_TRUE;
    }

    /* current support lte, usim card status is avialable and sim ps reg status is true, start lte acquir */
    /* 用接口NAS_MSCC_IsNeedAcqLte代替散开的多个条件判断 */
    if (VOS_TRUE == NAS_MSCC_IsNeedAcqLte())
    {
        NAS_MSCC_SndMmcPlmnSearchReq(NAS_MSCC_PIF_ACQ_REASON_OTHER,
                                     NAS_MSCC_PIF_PLMN_SEARCH_SPEC,
                                     VOS_NULL_PTR,
                                     VOS_NULL_PTR);

        NAS_MSCC_Set3gppCampOnFlag(VOS_FALSE);

        NAS_MSCC_Set3gppActiveFlg(VOS_TRUE);

        return VOS_TRUE;
    }

    /* Send HSD system acquire request if CDMA EVDO is supported */
    if (VOS_TRUE == NAS_MSCC_IsNeedAcqHrpd())
    {
        NAS_MSCC_SndHsdSysAcqReq(MSCC_HSD_ACQUIRE_REASON_OTHER,
                                 0,
                                 VOS_NULL_PTR);

        NAS_MSCC_SetHrpdCampOnFlag(VOS_FALSE);

        NAS_MSCC_SetHrpdActiveFlg(VOS_TRUE);
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaCdmaMoCallStartNtf_Main_Deactive(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{


    NAS_MSCC_StopTimer(TI_NAS_MSCC_SLEEP_TIMER);

    (VOS_VOID)NAS_MSCC_StartTimer(TI_NAS_MSCC_SCAN_TIMER, NAS_MSCC_GetScanTimerLength());

    NAS_MSCC_SetCurrFsmState(NAS_MSCC_L1_STA_ACTIVE);

    if (VOS_TRUE == NAS_MSCC_IsNeedPerformCLSysAcq())
    {
        NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_MO_TRIGGER, NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_MO_TRIGGER));

        return VOS_TRUE;
    }

    /* judge if need acqurie lte first, start lte acquire */
    if (VOS_TRUE == NAS_MSCC_IsNeedAcqLte())
    {
        NAS_MSCC_SndMmcPlmnSearchReq(NAS_MSCC_PIF_ACQ_REASON_OTHER,
                                     NAS_MSCC_PIF_PLMN_SEARCH_SPEC,
                                     VOS_NULL_PTR,
                                     VOS_NULL_PTR);

        NAS_MSCC_Set3gppCampOnFlag(VOS_FALSE);

        NAS_MSCC_Set3gppActiveFlg(VOS_TRUE);

        return VOS_TRUE;
    }

    /* Send HSD system acquire request if CDMA EVDO is supported */
    if (VOS_TRUE == NAS_MSCC_IsNeedAcqHrpd())
    {
        NAS_MSCC_SndHsdSysAcqReq(MSCC_HSD_ACQUIRE_REASON_OTHER,
                                 0,
                                 VOS_NULL_PTR);

        NAS_MSCC_SetHrpdCampOnFlag(VOS_FALSE);

        NAS_MSCC_SetHrpdActiveFlg(VOS_TRUE);
    }

    return VOS_TRUE;
}




VOS_UINT32 NAS_MSCC_RcvTiAvailableTimerExpired_Main_Active(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_MNTN_LogFsmL1MainCtxRelatedInfo();
    NAS_MSCC_MNTN_LogClSystemAcquireStrategyRelatedInfo();

    if (VOS_TRUE == NAS_MSCC_ProcTiAvailableTimerExpiredOosSrch_L1Main())
    {
        NAS_MSCC_MNTN_LogFsmL1MainCtxRelatedInfo();
        NAS_MSCC_MNTN_LogClSystemAcquireStrategyRelatedInfo();

        return VOS_TRUE;
    }

    if (VOS_TRUE == NAS_MSCC_ProcTiAvailableTimerExpiredLteHistorySrchWith1xSrvExist_L1Main())
    {
        NAS_MSCC_MNTN_LogFsmL1MainCtxRelatedInfo();
        NAS_MSCC_MNTN_LogClSystemAcquireStrategyRelatedInfo();

        return VOS_TRUE;
    }

    if (VOS_TRUE == NAS_MSCC_ProcTiAvailableTimerExpiredLteFullBandSrchWith1xSrvExist_L1Main())
    {
        NAS_MSCC_MNTN_LogFsmL1MainCtxRelatedInfo();
        NAS_MSCC_MNTN_LogClSystemAcquireStrategyRelatedInfo();

        return VOS_TRUE;
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvTi1xBsrLteTimerExpired_Main_Active(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                          uc3gppNormalSrvFlg;
    VOS_UINT32                          ucHrpdNormalSrvFlg;

    uc3gppNormalSrvFlg  = NAS_MSCC_Is3gppNormalService();
    ucHrpdNormalSrvFlg  = NAS_MSCC_IsHrpdNormalService();

    if ((VOS_TRUE == uc3gppNormalSrvFlg)
     || (VOS_TRUE == ucHrpdNormalSrvFlg))
    {
        NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_RcvTi1xBsrLteTimerExpired_Main_Active:could not perform 1x bsr lte system acquire flow due to lte or hrpd normal service.");

        return VOS_TRUE;
    }

    NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_RcvTi1xBsrLteTimerExpired_Main_Active:start perform 1x bsr lte system acquire flow.");

    /* 对于缓存了超时消息的情况，退出sys acq流程，可能会继续启动这个定时器，因此显示停止一次，保证BSR过程中该定时器不在启动状态 */
    NAS_MSCC_StopTimer(TI_NAS_MSCC_1X_BSR_LTE_TIMER);

    NAS_MSCC_StopTimer(TI_NAS_MSCC_AVAILABLE_TIMER);

    NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_HRPD_LOST, NAS_MSCC_PIF_PLMN_SEARCH_SPEC);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvTiScanTimerExpired_Main_Active(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_SERVICE_STATUS_ENUM_UINT8  enHrpdSrvst;

    /* maybe here is the buffered scan timer expied message,should judge the service
       status, if one of the system is normal service, hould ignore this message */

    enHrpdSrvst = NAS_MSCC_GetCurrHrpdServiceStatus();

    if ((NAS_MSCC_SERVICE_STATUS_NORMAL_SERVICE == NAS_MSCC_GetCurr3gppPsServiceStatus())
     || (NAS_MSCC_SERVICE_STATUS_NORMAL_SERVICE == enHrpdSrvst))
    {
        /* discard this message */
        return VOS_TRUE;
    }

    if (VOS_TRUE == NAS_MSCC_Get1xMoCallConnectingFlag())
    {
        (VOS_VOID)NAS_MSCC_StartTimer(TI_NAS_MSCC_SCAN_TIMER, NAS_MSCC_GetScanTimerLength());
        return VOS_TRUE;
    }

    /* Start the Power Off FSM */
    NAS_MSCC_InitFsmL2(NAS_MSCC_FSM_POWER_OFF);

    return VOS_TRUE;
}


VOS_VOID NAS_MSCC_PerformSystemAcquireUponSleepTimerExpired_Main(VOS_VOID)
{


    NAS_MSCC_SetCurrFsmState(NAS_MSCC_L1_STA_ACTIVE);

    (VOS_VOID)NAS_MSCC_StartTimer(TI_NAS_MSCC_SCAN_TIMER, NAS_MSCC_GetScanTimerLength());

    if (VOS_TRUE == NAS_MSCC_IsSpecRatSupported(NAS_MSCC_PIF_NET_RAT_TYPE_1X))
    {
        NAS_MSCC_SndXsdSysAcqReq(MSCC_XSD_SYS_ACQ_TYPE_NORMAL);

        NAS_MSCC_Set1xCampOnFlag(VOS_FALSE);

        NAS_MSCC_Set1xActiveFlg(VOS_TRUE);
    }

    if (VOS_TRUE == NAS_MSCC_IsNeedPerformCLSysAcq())
    {
        NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_SLEEP_TIMER_EXPIRED, NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_SLEEP_TIMER_EXPIRED));

        return;
    }

    /* judge if need acqurie lte first, start lte acquire */
    if (VOS_TRUE == NAS_MSCC_IsNeedAcqLte())
    {
        NAS_MSCC_SndMmcPlmnSearchReq(NAS_MSCC_PIF_ACQ_REASON_OTHER,
                                     NAS_MSCC_PIF_PLMN_SEARCH_SPEC,
                                     VOS_NULL_PTR,
                                     VOS_NULL_PTR);

        NAS_MSCC_Set3gppCampOnFlag(VOS_FALSE);

        NAS_MSCC_Set3gppActiveFlg(VOS_TRUE);

        return;
    }

    /* Send HSD system acquire request if CDMA EVDO is supported */
    if (VOS_TRUE == NAS_MSCC_IsNeedAcqHrpd())
    {
        NAS_MSCC_SndHsdSysAcqReq(MSCC_HSD_ACQUIRE_REASON_OTHER,
                                 0,
                                 VOS_NULL_PTR);

        NAS_MSCC_SetHrpdCampOnFlag(VOS_FALSE);

        NAS_MSCC_SetHrpdActiveFlg(VOS_TRUE);
    }

    return;
}


VOS_UINT32 NAS_MSCC_RcvHsdInterSysStartInd_Main_Active(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_InitFsmL2(NAS_MSCC_FSM_CL_INTERSYS);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmcInterSysStartInd_Main_Active(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_InitFsmL2(NAS_MSCC_FSM_CL_INTERSYS);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMsccCLInterSysRsltCnf_Main_Active(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaCdmaMoCallSusccessNtf_Main_Active(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{

    /* Start the Power OFF FSM  */
    NAS_MSCC_InitFsmL2(NAS_MSCC_FSM_POWER_OFF);

    return VOS_TRUE;

}


VOS_UINT32 NAS_MSCC_RcvXsdEmcCallBackInd_Main_Active(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{


    NAS_MSCC_SetEmcCallExistFlag(VOS_FALSE);

    if (VOS_TRUE == NAS_MSCC_IsNeedPerformCLSysAcq())
    {
        NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_SLEEP_TIMER_EXPIRED, NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_SLEEP_TIMER_EXPIRED));

        return VOS_TRUE;
    }

    /* judge if need acqurie lte first, start lte acquire */
    if (VOS_TRUE == NAS_MSCC_IsNeedAcqLte())
    {
        NAS_MSCC_SndMmcPlmnSearchReq(NAS_MSCC_PIF_ACQ_REASON_OTHER,
                                     NAS_MSCC_PIF_PLMN_SEARCH_SPEC,
                                     VOS_NULL_PTR,
                                     VOS_NULL_PTR);

        NAS_MSCC_Set3gppCampOnFlag(VOS_FALSE);

        NAS_MSCC_Set3gppActiveFlg(VOS_TRUE);

        return VOS_TRUE;
    }

    /* Send HSD system acquire request if CDMA EVDO is supported */
    if (VOS_TRUE == NAS_MSCC_IsNeedAcqHrpd())
    {
        NAS_MSCC_SndHsdSysAcqReq(MSCC_HSD_ACQUIRE_REASON_OTHER,
                                 0,
                                 VOS_NULL_PTR);

        NAS_MSCC_SetHrpdCampOnFlag(VOS_FALSE);

        NAS_MSCC_SetHrpdActiveFlg(VOS_TRUE);
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_ProcTiAvailableTimerExpiredOosSrch_L1Main(VOS_VOID)
{
    NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_ProcTiAvailableTimerExpiredOosSrch_L1Main entered.");

    if (VOS_FALSE == NAS_MSCC_Is1xNormalService())
    {
        /* 1x可能从有服务变到无服务,需要清除1x有服务时相关信息 */
        NAS_MSCC_StopTimer(TI_NAS_MSCC_1X_SERVICE_CL_SYSTEM_ACQUIRE_PHASE_ONE_TOTAL_TIMER);

        NAS_MSCC_ResetCur1xServiceLteHistorySrchCount_L1Main();

        NAS_MSCC_ResetCur1xServiceLteFullBandSrchCount_L1Main();

        NAS_MSCC_ResetCur1xServiceSysAcqPhaseNum();

        NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_AVAILABLE_TIMER_EXPIRED, NAS_MSCC_PIF_PLMN_SEARCH_SPEC);

        return VOS_TRUE;
    }

    /* 非OOS场景,重置定时器启动次数 */
    NAS_MSCC_ResetCurOosAvailableTimerStartCount_L1Main();

    return VOS_FALSE;
}


VOS_UINT32 NAS_MSCC_ProcTiAvailableTimerExpiredLteFullBandSrchWith1xSrvExist_L1Main(VOS_VOID)
{
    VOS_UINT8                           ucFullBandSrchCount;
    VOS_UINT8                           ucFullBandSrchNum;

    NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_ProcTiAvailableTimerExpiredLteFullBandSrchWith1xSrvExist_L1Main entered.");

    if (VOS_TRUE == NAS_MSCC_IsNeedPerfromLteFullBandSrchWith1xServiceExist_L1Main())
    {
        NAS_MSCC_AddCur1xServiceLteFullBandSrchCount_L1Main();

        if (NAS_MSCC_1X_SERVICE_CL_SYSTEM_ACQUIRE_PHASE_ONE == NAS_MSCC_GetCur1xServiceSysAcqPhaseNum())
        {
            ucFullBandSrchNum = NAS_MSCC_Get1xServiceClSysAcqPatternOneCfgAddr()->ucLteFullBandSrchNum;
        }
        else
        {
            ucFullBandSrchNum = NAS_MSCC_Get1xServiceClSysAcqPatternTwoCfgAddr()->ucLteFullBandSrchNum;
        }

        ucFullBandSrchCount = NAS_MSCC_GetCur1xServiceLteFullBandSrchCount_L1Main();

        /* 当前处于最大阶段,不需要进入下个阶段了 */
        if ((NAS_MSCC_1X_SERVICE_CL_SYSTEM_ACQUIRE_PHASE_MAX != NAS_MSCC_GetCur1xServiceSysAcqPhaseNum())
         && (ucFullBandSrchCount == ucFullBandSrchNum))
        {
            NAS_MSCC_AddCur1xServiceSysAcqPhaseNum();

            NAS_MSCC_ResetCur1xServiceLteFullBandSrchCount_L1Main();

            NAS_MSCC_ResetCur1xServiceLteHistorySrchCount_L1Main();

            NAS_MSCC_StopTimer(TI_NAS_MSCC_1X_SERVICE_CL_SYSTEM_ACQUIRE_PHASE_ONE_TOTAL_TIMER);
        }

        /* 是否需要重置当前阶段 */
        if ((NAS_MSCC_1X_SERVICE_CL_SYSTEM_ACQUIRE_PHASE_MAX == NAS_MSCC_GetCur1xServiceSysAcqPhaseNum())
         && (ucFullBandSrchCount == ucFullBandSrchNum))
        {
            NAS_MSCC_ResetCur1xServiceLteFullBandSrchCount_L1Main();

            NAS_MSCC_ResetCur1xServiceLteHistorySrchCount_L1Main();
        }

        /* 需要进行全频搜的时候,需要清除历史搜的次数,保证进行NV限定次数的历史搜后再进行一次全频搜 */
        NAS_MSCC_ResetCur1xServiceLteHistorySrchCount_L1Main();

        NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_AVAILABLE_TIMER_EXPIRED, NAS_MSCC_PIF_PLMN_SEARCH_SPEC);

        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_VOID NAS_MSCC_ProcSrlteTiAvailableTimerExpiredLteHistorySrchWith1xDataExist_L1Main(
    VOS_UINT8                           ucMaxHistorySrchNum,
    VOS_UINT8                           ucMaxFullBandSrchNum
)
{
    NAS_MSCC_1X_SERVICE_CL_SYSTEM_ACQUIRE_CTRL_STRU        *pst1xSrvClSysAcqCtrl;

    pst1xSrvClSysAcqCtrl = NAS_MSCC_Get1xServiceClSysAcqCtrlAddr();

    NAS_NORMAL_LOG1(UEPS_PID_MSCC, "NAS_MSCC_ProcSrlteTiAvailableTimerExpiredLteHistorySrchWith1xDataExist_L1Main: ulSrlte1xBsrLteEnableFlag",
                                    pst1xSrvClSysAcqCtrl->ucSrlte1xBsrLteEnableFlag);

    /* SRLTE下，判断是否允许1x数据连接态BSR LTE，如果不支持，直接返回 */
    if (VOS_FALSE == pst1xSrvClSysAcqCtrl->ucSrlte1xBsrLteEnableFlag)
    {
        return;
    }

    if (NAS_MSCC_TIMER_STATUS_RUNING == NAS_MSCC_GetTimerStatus(TI_NAS_MSCC_1X_BSR_LTE_TIMER))
    {
        return;
    }


    if (ucMaxHistorySrchNum != NAS_MSCC_GetCur1xServiceLteHistorySrchCount_L1Main())
    {
        /* 当前历史搜索次数没有达到NV中配置的最大历史次数，增加历史搜索次数 */
        NAS_MSCC_AddCur1xServiceLteHistorySrchCount_L1Main();
    }

    /***************************************************************************************
     * 如果当前搜索LTE历史次数已经达到了NV配置的历史搜索次数，处理如下:
     * 1. 如果搜索阶段不是最大阶段，则跳到下一阶段
     * 2. 如果当前已经时最大阶段，如果全频搜次数为0，则需要reset全频搜与历史搜次数，否则不再
     *    刷新历史搜索次数；
     **************************************************************************************/
    if (ucMaxHistorySrchNum == NAS_MSCC_GetCur1xServiceLteHistorySrchCount_L1Main())
    {
        if (NAS_MSCC_1X_SERVICE_CL_SYSTEM_ACQUIRE_PHASE_MAX != NAS_MSCC_GetCur1xServiceSysAcqPhaseNum())
        {
            NAS_MSCC_AddCur1xServiceSysAcqPhaseNum();

            NAS_MSCC_ResetCur1xServiceLteFullBandSrchCount_L1Main();

            NAS_MSCC_ResetCur1xServiceLteHistorySrchCount_L1Main();

            NAS_MSCC_StopTimer(TI_NAS_MSCC_1X_SERVICE_CL_SYSTEM_ACQUIRE_PHASE_ONE_TOTAL_TIMER);
        }
        else
        {
            if (0 == ucMaxFullBandSrchNum)
            {
                NAS_MSCC_ResetCur1xServiceLteFullBandSrchCount_L1Main();

                NAS_MSCC_ResetCur1xServiceLteHistorySrchCount_L1Main();
            }
        }
    }

    /* 1X TCH态available定时器超时场景，该场景只做LTE历史, 原因是1X数据业务下，搜DO底层不支持 */
    NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_AVAILABLE_TIMER_EXPIRED_SRLTE_1X_TCH,
                                  NAS_MSCC_PIF_PLMN_SEARCH_HISTORY);
}


VOS_UINT32 NAS_MSCC_ProcTiAvailableTimerExpiredLteHistorySrchWith1xSrvExist_L1Main(VOS_VOID)
{
    VOS_UINT8                           ucHistorySrchCount;
    VOS_UINT8                           ucHistorySrchNum;
    VOS_UINT8                           ucFullBandSrchNum;

    NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_ProcTiAvailableTimerExpiredLteHistorySrchWith1xSrvExist_L1Main entered.");

    if (NAS_MSCC_1X_SERVICE_CL_SYSTEM_ACQUIRE_PHASE_ONE == NAS_MSCC_GetCur1xServiceSysAcqPhaseNum())
    {
        ucHistorySrchNum  = NAS_MSCC_Get1xServiceClSysAcqPatternOneCfgAddr()->ucLteHistorySrchNum;

        ucFullBandSrchNum = NAS_MSCC_Get1xServiceClSysAcqPatternOneCfgAddr()->ucLteFullBandSrchNum;
    }
    else
    {
        ucHistorySrchNum  = NAS_MSCC_Get1xServiceClSysAcqPatternTwoCfgAddr()->ucLteHistorySrchNum;

        ucFullBandSrchNum = NAS_MSCC_Get1xServiceClSysAcqPatternTwoCfgAddr()->ucLteFullBandSrchNum;
    }

    if (VOS_TRUE == NAS_MSCC_IsSrlte1xDataServiceState())
    {
        NAS_MSCC_ProcSrlteTiAvailableTimerExpiredLteHistorySrchWith1xDataExist_L1Main(ucHistorySrchNum, ucFullBandSrchNum);

        return VOS_TRUE;
    }

    if (VOS_TRUE == NAS_MSCC_IsNeedPerfromLteHistorySrchWith1xServiceExist_L1Main())
    {
        NAS_MSCC_AddCur1xServiceLteHistorySrchCount_L1Main();

        ucHistorySrchCount  = NAS_MSCC_GetCur1xServiceLteHistorySrchCount_L1Main();

        /* 当前处于最大阶段,不需要进入下个阶段了 */
        if ((NAS_MSCC_1X_SERVICE_CL_SYSTEM_ACQUIRE_PHASE_MAX != NAS_MSCC_GetCur1xServiceSysAcqPhaseNum())
         && (ucHistorySrchCount                              == ucHistorySrchNum)
         && (0                                               == ucFullBandSrchNum))
        {
            NAS_MSCC_AddCur1xServiceSysAcqPhaseNum();

            NAS_MSCC_ResetCur1xServiceLteFullBandSrchCount_L1Main();

            NAS_MSCC_ResetCur1xServiceLteHistorySrchCount_L1Main();

            NAS_MSCC_StopTimer(TI_NAS_MSCC_1X_SERVICE_CL_SYSTEM_ACQUIRE_PHASE_ONE_TOTAL_TIMER);
        }

        /* 是否需要重置当前阶段 */
        if ((NAS_MSCC_1X_SERVICE_CL_SYSTEM_ACQUIRE_PHASE_MAX == NAS_MSCC_GetCur1xServiceSysAcqPhaseNum())
         && (ucHistorySrchCount                              == ucHistorySrchNum)
         && (0                                               == ucFullBandSrchNum))
        {
            NAS_MSCC_ResetCur1xServiceLteFullBandSrchCount_L1Main();

            NAS_MSCC_ResetCur1xServiceLteHistorySrchCount_L1Main();
        }

        NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_AVAILABLE_TIMER_EXPIRED, NAS_MSCC_PIF_PLMN_SEARCH_HISTORY);

        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 NAS_MSCC_RcvTi1xSrvClSysAcqPhaseOneTotalTimerExpired_Main_Active(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_RcvTi1xSrvClSysAcqPhaseOneTotalTimerExpired_Main_Active entered.");

    /* 注意:打断流程都在后处理进行,此处是打断后回到L1 MAIN的操作 */

    /* 显式停止下当前总阶段时长定时器,因为可能时先超时后被缓存,
       在处理缓存定时器超时之前启动的 */
    NAS_MSCC_StopTimer(TI_NAS_MSCC_1X_SERVICE_CL_SYSTEM_ACQUIRE_PHASE_ONE_TOTAL_TIMER);

    if (NAS_MSCC_1X_SERVICE_CL_SYSTEM_ACQUIRE_PHASE_ONE != NAS_MSCC_GetCur1xServiceSysAcqPhaseNum())
    {
        NAS_ERROR_LOG(UEPS_PID_MSCC, "NAS_MSCC_RcvTi1xSrvClSysAcqPhaseOneTotalTimerExpired_Main_Active:Not In Phase One.");

        return VOS_TRUE;
    }

    /* 重置当前阶段的一些参数,进入下个阶段搜索 */
    NAS_MSCC_ResetCur1xServiceLteFullBandSrchCount_L1Main();
    NAS_MSCC_ResetCur1xServiceLteHistorySrchCount_L1Main();

    if (NAS_MSCC_1X_SERVICE_CL_SYSTEM_ACQUIRE_PHASE_MAX != NAS_MSCC_GetCur1xServiceSysAcqPhaseNum())
    {
        NAS_MSCC_AddCur1xServiceSysAcqPhaseNum();
    }

    /* 如果当前available搜网在运行，则重启定时器等待其超时触发后续动作 */
    if (NAS_MSCC_TIMER_STATUS_RUNING == NAS_MSCC_GetTimerStatus(TI_NAS_MSCC_AVAILABLE_TIMER))
    {
        NAS_MSCC_StopTimer(TI_NAS_MSCC_AVAILABLE_TIMER);

        /* 启动一个短时定时器,超时后切换到下个阶段 */
        (VOS_VOID)NAS_MSCC_StartTimer(TI_NAS_MSCC_AVAILABLE_TIMER, TI_NAS_MSCC_DEFAULT_AVAILABLE_TIMER_SKIP_LEN);
    }

    return VOS_TRUE;
}


/* 系统捕获lte的搜索类型映射表 */
NAS_MSCC_SYS_ACQ_SCENE_MAP_LTE_SRCH_TYPE_STRU g_astSysAcqSceneMapLteSrchTypeTbl[]=
{
    {NAS_MSCC_SYS_ACQ_SCENE_HRPD_LOST, NAS_MSCC_PIF_PLMN_SEARCH_SPEC},
    {NAS_MSCC_SYS_ACQ_SCENE_LTE_LOST, NAS_MSCC_PIF_PLMN_SEARCH_SPEC},
    {NAS_MSCC_SYS_ACQ_SCENE_HRPD_LOST_NO_RF, NAS_MSCC_PIF_PLMN_SEARCH_SPEC},
    {NAS_MSCC_SYS_ACQ_SCENE_LTE_NOT_AVAILABLE, NAS_MSCC_PIF_PLMN_SEARCH_SPEC},
    {NAS_MSCC_SYS_ACQ_SCENE_SWITCH_ON, NAS_MSCC_PIF_PLMN_SEARCH_SPEC},
    {NAS_MSCC_SYS_ACQ_SCENE_SLEEP_TIMER_EXPIRED, NAS_MSCC_PIF_PLMN_SEARCH_SPEC},
    {NAS_MSCC_SYS_ACQ_SCENE_SYS_CFG_SET, NAS_MSCC_PIF_PLMN_SEARCH_SPEC},
    {NAS_MSCC_SYS_ACQ_SCENE_LTE_HISTORY_PLUS_LTE_LOST, NAS_MSCC_PIF_PLMN_SEARCH_HISTORY},
};




NAS_MSCC_PIF_PLMN_SEARCH_TYPE_ENUM_UINT32 NAS_MSCC_GetSysAcqLteSrchType_L1Main(
    NAS_MSCC_SYS_ACQ_SCENE_ENUM_UINT32  enSysAcqScene
)
{
    VOS_UINT32                          i;

    /* 先在表里查找是否有映射关系,如果没有根据当前LTE的搜索次数以及1x是否有服务进行判断 */
    for (i = 0; i < (sizeof(g_astSysAcqSceneMapLteSrchTypeTbl)/sizeof(NAS_MSCC_SYS_ACQ_SCENE_MAP_LTE_SRCH_TYPE_STRU)); i++)
    {
        if (enSysAcqScene == g_astSysAcqSceneMapLteSrchTypeTbl[i].enSysAcqScene)
        {
            return g_astSysAcqSceneMapLteSrchTypeTbl[i].enLteSrchType;
        }
    }

    if (VOS_TRUE == NAS_MSCC_Is1xNormalService())
    {
        if (VOS_TRUE == NAS_MSCC_IsNeedPerfromLteHistorySrchWith1xServiceExist_L1Main())
        {
            return NAS_MSCC_PIF_PLMN_SEARCH_HISTORY;
        }
    }

    return NAS_MSCC_PIF_PLMN_SEARCH_SPEC;
}


VOS_UINT32 NAS_MSCC_RcvMmaCdmaMoCallRedialSysAcqNtf_Main_Active(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 如果available定时器在允许,停止该定时器 */
    NAS_MSCC_StopTimer(TI_NAS_MSCC_AVAILABLE_TIMER);

    NAS_MSCC_SndMmaSystemAcquireStartInd(MSCC_MMA_ACQ_SYS_TYPE_DO_LTE);

    NAS_MSCC_InitFsmL2(NAS_MSCC_FSM_SYS_ACQ);

    return VOS_TRUE;
}



VOS_VOID NAS_MSCC_ProcBsrStartProcedure_Main(VOS_VOID)
{
    if (VOS_TRUE == NAS_MSCC_IsNeedPerfromBsrHistorySrch())
    {
        NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_ProcBsrStartProcedure_Main start bsr history srch.");

        NAS_MSCC_SetCurBsrSrchType(NAS_MSCC_BSR_SRCH_TYPE_HISTORY);

        NAS_MSCC_InitFsmL2(NAS_MSCC_FSM_BETTER_SYSTEM_RESELECTION);

        return;
    }

    NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_ProcBsrStartProcedure_Main start pref band history srch.");

    NAS_MSCC_SetCurBsrSrchType(NAS_MSCC_BSR_SRCH_TYPE_PREF_BAND);

    NAS_MSCC_InitFsmL2(NAS_MSCC_FSM_BETTER_SYSTEM_RESELECTION);

    return;
}


#endif


VOS_VOID NAS_MSCC_PerformSystemAcquireUponPowerOn_Main(
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_SYSTEM_ACQUIRE_REQ_STRU                       *pstMmaSysAcqReq = VOS_NULL_PTR;

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    NAS_MSCC_DISABLE_LTE_REASON_ENUM_UINT32                 enDisableLReason;
#endif

    pstMmaSysAcqReq = (MMA_MSCC_SYSTEM_ACQUIRE_REQ_STRU *)pstMsg;


#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    enDisableLReason = NAS_MSCC_DISABLE_LTE_REASON_BUTT;
#endif

    /* 更新初搜位置信息 */
    NAS_MSCC_UpdateGulNoCardInitSearchLocInfo(pstMmaSysAcqReq->stInitAcqPlmnInfo.stInitAcqPlmnId.ulMcc,
                                              pstMmaSysAcqReq->stInitAcqPlmnInfo.stInitAcqPlmnId.ulMnc);

    NAS_MSCC_Update1xNoCardInitSearchLocInfo(pstMmaSysAcqReq->stCdmaInitLocInfo.lSid,
                                             pstMmaSysAcqReq->stCdmaInitLocInfo.lNid);

    NAS_MSCC_Update1xNoCardInitSearchLocMcc(pstMmaSysAcqReq->stCdmaInitLocInfo.ulMcc);


    if (VOS_TRUE == NAS_MSCC_IsGULModeConfigured())
    {
        NAS_MSCC_SndMmcPlmnSearchReq(NAS_MSCC_PIF_ACQ_REASON_POWER_UP,
                                     NAS_MSCC_PIF_PLMN_SEARCH_SPEC,
                                     &(pstMmaSysAcqReq->stInitAcqPlmnInfo),
                                     VOS_NULL_PTR);

        NAS_MSCC_Set3gppCampOnFlag(VOS_FALSE);

        NAS_MSCC_Set3gppActiveFlg(VOS_TRUE);

        /* Set the Current FSM state to Active */
        NAS_MSCC_SetCurrFsmState(NAS_MSCC_L1_STA_ACTIVE);

        return;
    }

    /* 下面处理CL模式下的系统捕获流程 */
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    NAS_MSCC_SetCurrFsmState(NAS_MSCC_L1_STA_ACTIVE);

    (VOS_VOID)NAS_MSCC_StartTimer(TI_NAS_MSCC_SCAN_TIMER, NAS_MSCC_GetScanTimerLength());

    if (VOS_TRUE == NAS_MSCC_IsSpecRatSupported(NAS_MSCC_PIF_NET_RAT_TYPE_1X))
    {
        NAS_MSCC_SndXsdSysAcqReq(MSCC_XSD_SYS_ACQ_TYPE_NORMAL);

        NAS_MSCC_Set1xCampOnFlag(VOS_FALSE);

        NAS_MSCC_Set1xActiveFlg(VOS_TRUE);
    }

    if (VOS_TRUE == NAS_MSCC_IsNeedPerformCLSysAcq())
    {
        NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_SWITCH_ON, NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_SWITCH_ON));

        return;
    }

    /* judge if need acqurie lte first, start lte acquire */
    if (VOS_TRUE == NAS_MSCC_IsNeedAcqLte())
    {
        NAS_MSCC_SndMmcPlmnSearchReq(NAS_MSCC_PIF_ACQ_REASON_POWER_UP,
                                     NAS_MSCC_PIF_PLMN_SEARCH_SPEC,
                                     &(pstMmaSysAcqReq->stInitAcqPlmnInfo),
                                     VOS_NULL_PTR);

        NAS_MSCC_Set3gppCampOnFlag(VOS_FALSE);

        NAS_MSCC_Set3gppActiveFlg(VOS_TRUE);

        /* hrpd and lte could not both perform system acquire,here return */
        return;
    }

    if (VOS_TRUE == NAS_MSCC_IsNeedAcqHrpd())
    {
        /* Send HSD system acquire request if CDMA EVDO is supported */
        NAS_MSCC_SndHsdSysAcqReq(MSCC_HSD_ACQUIRE_REASON_POWER_UP,
                                 0,
                                 VOS_NULL_PTR);

        /* Set the Camp On flag to FALSE */
        NAS_MSCC_SetHrpdCampOnFlag(VOS_FALSE);

        NAS_MSCC_SetHrpdActiveFlg(VOS_TRUE);

        /* USIM卡应用为absent时要disable lte */
        if (VOS_TRUE == NAS_MSCC_IsNeedDisableLte(&enDisableLReason))
        {
            NAS_MSCC_SndHsdDisableLteNtf();

            NAS_MSCC_SetDisableLteReason(enDisableLReason);

            NAS_MSCC_SetLteCapbilityStatus(NAS_MSCC_LTE_CAPABILITY_STATUS_DISABLE_NOTIFIED_AS);
        }

        return;
    }

    /* lte和hrpd均不搜 */
    NAS_WARNING_LOG2(UEPS_PID_MSCC, "NAS_MSCC_PerformSystemAcquireUponPowerOn_Main:do not srch lte and hrpd, usim card status and csim card status are",
                                                NAS_MSCC_GetUsimCardStatus(), NAS_MSCC_GetCsimCardStatus());

    /* 1X也未激活,停止SCAN定时器,进入deactive态; */
    if (VOS_FALSE == NAS_MSCC_Get1xActiveFlg())
    {
        NAS_MSCC_StopTimer(TI_NAS_MSCC_SCAN_TIMER);
    }

#endif

    return;
}


VOS_VOID NAS_MSCC_PerformSystemAcquireUponSysCfg_Main(VOS_VOID)
{
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    VOS_UINT32                          uc3gppNormalSrvFlg;
    VOS_UINT32                          ucHrpdNormalSrvFlg;

#endif

    if (VOS_TRUE == NAS_MSCC_IsGULModeConfigured())
    {
         if (VOS_FALSE == NAS_MSCC_Get3gppActiveFlg())
         {
             NAS_MSCC_SndMmcPlmnSearchReq(NAS_MSCC_PIF_ACQ_REASON_OTHER,
                                          NAS_MSCC_PIF_PLMN_SEARCH_SPEC,
                                          VOS_NULL_PTR,
                                          VOS_NULL_PTR);

             NAS_MSCC_Set3gppCampOnFlag(VOS_FALSE);

             NAS_MSCC_Set3gppActiveFlg(VOS_TRUE);
         }

         return;
    }

    /* 下面处理CL模式下的系统捕获流程 */
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

    if (VOS_TRUE == NAS_MSCC_IsNeedStartScanTimerUponSyscfg())
    {
        (VOS_VOID)NAS_MSCC_StartTimer(TI_NAS_MSCC_SCAN_TIMER, NAS_MSCC_GetScanTimerLength());
    }

    if (VOS_TRUE == NAS_MSCC_IsSpecRatSupported(NAS_MSCC_PIF_NET_RAT_TYPE_1X))
    {
        if (VOS_FALSE == NAS_MSCC_Get1xActiveFlg())
        {
            NAS_MSCC_SndXsdSysAcqReq(MSCC_XSD_SYS_ACQ_TYPE_NORMAL);

            NAS_MSCC_Set1xCampOnFlag(VOS_FALSE);

            NAS_MSCC_Set1xActiveFlg(VOS_TRUE);
        }
    }



    uc3gppNormalSrvFlg  = NAS_MSCC_Is3gppNormalService();
    ucHrpdNormalSrvFlg  = NAS_MSCC_IsHrpdNormalService();

    /* 如果3GPP或HRPD已经是normal service且驻留到最高优先级,不需要搜网;直接返回 */
    if ((VOS_TRUE  == NAS_MSCC_IsCampOnHighestPrioritySystem())
     && ((VOS_TRUE == uc3gppNormalSrvFlg)
      || (VOS_TRUE == ucHrpdNormalSrvFlg)))
    {
        return;
    }

    /* 满足CL搜网条件且PS卡有效,进CL搜网流程 */
    if (VOS_TRUE == NAS_MSCC_IsNeedPerformCLSysAcq())
    {
        NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_SYS_CFG_SET, NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_SYS_CFG_SET));

        return;
    }

    /* judge if need acqurie lte first, start lte acquire */
    if (VOS_TRUE == NAS_MSCC_IsNeedAcqLte())
    {
        if (VOS_FALSE == NAS_MSCC_Get3gppActiveFlg())
        {
            NAS_MSCC_SndMmcPlmnSearchReq(NAS_MSCC_PIF_ACQ_REASON_OTHER,
                                         NAS_MSCC_PIF_PLMN_SEARCH_SPEC,
                                         VOS_NULL_PTR,
                                         VOS_NULL_PTR);

            NAS_MSCC_Set3gppCampOnFlag(VOS_FALSE);

            NAS_MSCC_Set3gppActiveFlg(VOS_TRUE);
        }

        return;
    }

    /* Send HSD system acquire request if CDMA EVDO is supported */
    if (VOS_TRUE == NAS_MSCC_IsNeedAcqHrpd())
    {
        if (VOS_FALSE == NAS_MSCC_GetHrpdActiveFlg())
        {
            NAS_MSCC_SndHsdSysAcqReq(MSCC_HSD_ACQUIRE_REASON_OTHER,
                                     0,
                                     VOS_NULL_PTR);

            NAS_MSCC_SetHrpdCampOnFlag(VOS_FALSE);

            NAS_MSCC_SetHrpdActiveFlg(VOS_TRUE);
        }
    }
#endif

    return;
}

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

VOS_UINT32 NAS_MSCC_RcvHsdRfAvailableInd_Main_Active(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_PIF_NET_RAT_TYPE_ENUM_UINT8     enRat;

    /* HSD在DEACTIVE状态收到RF AVAILABLE IND，说明之前HSD丢过RF资源
       但在预处理中判断，如果当前模不被支持，则不进状态机处理，理论上进该函数
       一定是CL模式。
       不满足CL模式搜网的条件:(1)SYSCFG设置不支持CL;(2)SYSCFG支持CL，但不满足CL搜条件 */
    if (VOS_FALSE == NAS_MSCC_IsNeedPerformCLSysAcq())
    {
        /* (1)HRPD不支持后，SYSCFG会进入该分支;
           (2)不支持CL模式，搜网1X和HRPD失败，TI_NAS_MSCC_SCAN_TIMER定时器超时，HSD进入DEACTVE状态
           (3)支持CL模式，不满足HRPD+LTE混合搜网模式 */
        if (VOS_TRUE == NAS_MSCC_IsNeedAcqHrpd())
        {
            /* Send HSD system acquire request if CDMA EVDO is supported */
            NAS_MSCC_SndHsdSysAcqReq(MSCC_HSD_ACQUIRE_REASON_OTHER,
                                     0,
                                     VOS_NULL_PTR);

            /* Set the Camp On flag to FALSE */
            NAS_MSCC_SetHrpdCampOnFlag(VOS_FALSE);

            NAS_MSCC_SetHrpdActiveFlg(VOS_TRUE);
        }

        return VOS_TRUE;
    }

    /*******************************************************************************************
     * 如果当前是SRLTE，且上次驻留的是HRPD，则进行DO搜与LTE全频搜
     ******************************************************************************************/
    if (VOS_TRUE == NAS_MSCC_IsSrlte())
    {
        if (VOS_TRUE == NAS_MSCC_GetPrioSrchLastRatFlag())
        {
            enRat = NAS_MSCC_GetLastCampOnLTEOrDoRat();

            if (NAS_MSCC_PIF_NET_RAT_TYPE_HRPD == enRat)
            {
                NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_HRPD_RF_AVAILABLE,
                                              NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_HRPD_RF_AVAILABLE));
                return VOS_TRUE;
            }
            if (NAS_MSCC_PIF_NET_RAT_TYPE_LTE == enRat)
            {
                NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_LTE_HISTORY_PLUS_LTE_LOST,
                                              NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_LTE_HISTORY_PLUS_LTE_LOST));
                return VOS_TRUE;
            }
        }
    }

    if (VOS_FALSE == NAS_MSCC_Get3gppActiveFlg())
    {
        /* 停AVAILABLE定时器 */
        NAS_MSCC_StopTimer(TI_NAS_MSCC_AVAILABLE_TIMER);

        /* 以HRPD资源可用的场景搜网 */
        NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_HRPD_RF_AVAILABLE, NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_HRPD_RF_AVAILABLE));
    }



    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmcRfAvailableInd_Main_Active(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           ucHrpdActiveFlg;
    NAS_MSCC_PIF_NET_RAT_TYPE_ENUM_UINT8     enRat;



    ucHrpdActiveFlg = NAS_MSCC_GetHrpdActiveFlg();

    if (VOS_TRUE == NAS_MSCC_IsNeedPeformBsrUpOn3gppRfAvailable())
    {
        NAS_MSCC_StopTimer(TI_NAS_MSCC_BSR_TIMER);

        /* DO下存在数据业务为保证尽快发起BSR,此处使用10S定时器短暂尝试 */
        if (VOS_TRUE == NAS_MSCC_GetHrpdConnExistFlg())
        {
            (VOS_VOID)NAS_MSCC_StartTimer(TI_NAS_MSCC_PERIOD_TRYING_HIGH_PRI_SYSTEM_SEARCH, TI_NAS_MSCC_PERIOD_TRYING_HIGH_PRI_SYSTEM_SEARCH_LEN);
        }
        else
        {
            NAS_MSCC_ProcBsrStartProcedure_Main();
        }

        return VOS_TRUE;
    }

    /* MMC在DEACTIVE状态收到RF AVAILABLE IND，说明之前MMC丢过RF资源
       但在预处理中判断，如果当前模不被支持，则不进状态机处理，理论上进该函数
       一定是CL模式 */
    if (VOS_FALSE == NAS_MSCC_IsNeedPerformCLSysAcq())
    {
        /* (1)LTE不支持后，SYSCFG会进入该分支;
           (2)不支持CL模式，搜网1X和LTE失败，TI_NAS_MSCC_SCAN_TIMER定时器超时，MMC进入DEACTVE状态
           (3)支持CL模式，不满足HRPD+LTE混合搜网模式 */
        if ((VOS_TRUE == NAS_MSCC_IsNeedAcqLte())
         && (VOS_FALSE == ucHrpdActiveFlg))
        {
            NAS_MSCC_SndMmcPlmnSearchReq(NAS_MSCC_PIF_ACQ_REASON_OTHER,
                                         NAS_MSCC_PIF_PLMN_SEARCH_SPEC,
                                         VOS_NULL_PTR,
                                         VOS_NULL_PTR);

            NAS_MSCC_Set3gppCampOnFlag(VOS_FALSE);

            NAS_MSCC_Set3gppActiveFlg(VOS_TRUE);
        }

        return VOS_TRUE;
    }

    /***********************************************************************************************
     * 如果当前是SRLTE，且上次驻留的是LTE，则先进行LTE历史搜, 如果后续LTE搜索失败，再按LTE丢网搜
     * (DO搜与LTE全频搜)
     **********************************************************************************************/
    if (VOS_TRUE == NAS_MSCC_IsSrlte())
    {
        enRat = NAS_MSCC_GetLastCampOnLTEOrDoRat();

        if (VOS_TRUE == NAS_MSCC_GetPrioSrchLastRatFlag())
        {
            if (NAS_MSCC_PIF_NET_RAT_TYPE_HRPD == enRat)
            {
                NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_HRPD_RF_AVAILABLE,
                                              NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_HRPD_RF_AVAILABLE));
                return VOS_TRUE;
            }
            if (NAS_MSCC_PIF_NET_RAT_TYPE_LTE == enRat)
            {
                NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_LTE_HISTORY_PLUS_LTE_LOST,
                                              NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_LTE_HISTORY_PLUS_LTE_LOST));
                return VOS_TRUE;
            }
        }
    }

    /* 如果HRPD当前HRPD未被激活的话，则发起HRPD混合模式搜网 */
    if (VOS_FALSE == NAS_MSCC_GetHrpdActiveFlg())
    {
        /* 停AVAILABLE定时器 */
        NAS_MSCC_StopTimer(TI_NAS_MSCC_AVAILABLE_TIMER);

        /* 以lte资源可用超时搜网场景搜索CL  */
        NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_LTE_RF_AVAILABLE, NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_LTE_RF_AVAILABLE));
    }

    return VOS_TRUE;
}


#endif


VOS_UINT32 NAS_MSCC_RcvMmcPlmnSelectionRsltInd_Main_Active(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_PLMN_SELECTION_RLST_IND_STRU                  *pstMmcSysAcqCnf = VOS_NULL_PTR;
    MSCC_MMA_SYSTEM_ACQUIRE_RESULT_ENUM_UINT32              enMmaSysAcqRlst;
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    VOS_UINT16                                              usRatNum;
    NAS_MSCC_PIF_NET_RAT_TYPE_ENUM_UINT8                    aenRat[NAS_MSCC_PIF_MAX_RAT_NUM];
#endif

    pstMmcSysAcqCnf                     = (MMC_MSCC_PLMN_SELECTION_RLST_IND_STRU *)pstMsg;

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    if (NAS_MSCC_PIF_PLMN_SELECTION_RESULT_NO_RF == pstMmcSysAcqCnf->enResult)
    {
        if (VOS_FALSE == NAS_MSCC_IsGULModeConfigured())
        {
            /***************************************************************************************
             * 如果当前版本为SRLTE, 1X连接状态为非idle态，则先挂起LTE，等待后续LTE资源可用时，再根据
             * 上次驻留的RAT触发搜网
             **************************************************************************************/
            if (VOS_TRUE == NAS_MSCC_IsSrlte())
            {
                NAS_MEM_SET_S(&aenRat[0],
                           sizeof(NAS_MSCC_PIF_NET_RAT_TYPE_ENUM_UINT8) * NAS_MSCC_PIF_MAX_RAT_NUM,
                           0x00,
                           sizeof(NAS_MSCC_PIF_NET_RAT_TYPE_ENUM_UINT8) * NAS_MSCC_PIF_MAX_RAT_NUM);

                usRatNum = 1;
                aenRat[0] = NAS_MSCC_PIF_NET_RAT_TYPE_LTE;

                NAS_MSCC_SndInternalPowerSaveReq(usRatNum, &aenRat[0]);

                return VOS_TRUE;
            }

            /* check the current mode(only CL mode) and find the prior system in mspl*/
            if (VOS_TRUE == NAS_MSCC_IsNeedPerformCLSysAcq())
            {
                NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_LTE_NOT_AVAILABLE, NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_LTE_NOT_AVAILABLE));
            }

            return VOS_TRUE;
        }
    }
#endif

    enMmaSysAcqRlst = NAS_MSCC_ConvertMmcPlmnSelRsltToMmaSysAcqRlst(pstMmcSysAcqCnf->enResult);

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    if ((NAS_MSCC_PIF_PLMN_SELECTION_RESULT_LIMITED_SERVICE == pstMmcSysAcqCnf->enResult)
     || (NAS_MSCC_PIF_PLMN_SELECTION_RESULT_NO_SERVICE      == pstMmcSysAcqCnf->enResult))
    {
        /* check the current mode(only CL mode) and find the prior system in mspl*/
        if (VOS_TRUE == NAS_MSCC_IsNeedPerformCLSysAcq())
        {
            /* 卡无效或者用户detach ps域，只搜hrpd */
            if ((VOS_TRUE == NAS_MSCC_GetSimPsRegStatus())
             && (VOS_TRUE == NAS_MSCC_GetEpsAttachAllowFlg()))
            {
                NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_LTE_LOST, NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_LTE_LOST));
            }
            else
            {
                NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_LTE_NOT_AVAILABLE, NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_LTE_NOT_AVAILABLE));
            }

            return VOS_TRUE;
        }
    }
#endif

    /* GUL模式按照原有逻辑通知MMA搜网结束,模式为3GPP */
    if (VOS_TRUE == NAS_MSCC_IsGULModeConfigured())
    {
        NAS_MSCC_SndMmaSystemAcquireEndInd(enMmaSysAcqRlst,
                                           MSCC_MMA_ACQ_SYS_TYPE_3GPP,
                                           MSCC_MMA_CL_SYSTEM_ACQUIRE_RESULT_TYPE_BUTT);

        return VOS_TRUE;
    }

    /* 打断、NO_RF、L2C且不需要搜网的情况下不上报，其他情况上报mma_sys_end_ind结果 */
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    if ((NAS_MSCC_PIF_PLMN_SELECTION_RESULT_NORMAL_SERVICE == enMmaSysAcqRlst)
     || (NAS_MSCC_PIF_PLMN_SELECTION_RESULT_LIMITED_SERVICE == enMmaSysAcqRlst)
     || (NAS_MSCC_PIF_PLMN_SELECTION_RESULT_NO_SERVICE == enMmaSysAcqRlst))
    {
        NAS_MSCC_SndMmaSystemAcquireEndInd(enMmaSysAcqRlst,
                                          MSCC_MMA_ACQ_SYS_TYPE_DO_LTE,
                                          MSCC_MMA_CL_SYSTEM_ACQUIRE_RESULT_TYPE_BUTT);
    }
#endif

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMsccPowerOffCnf_Main_Active(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_MSCC_POWER_OFF_RSLT_CNF_STRU                      *pstPowerOffRslt;

    pstPowerOffRslt = (NAS_MSCC_MSCC_POWER_OFF_RSLT_CNF_STRU *)pstMsg;

    NAS_MSCC_ProcPowerOffResult_Main(pstPowerOffRslt->enRslt);

    return VOS_TRUE;
}


MSCC_MMA_SYSTEM_ACQUIRE_RESULT_ENUM_UINT32 NAS_MSCC_GetSystemAcquireFinalResult(
    NAS_MSCC_PIF_ACQUIRED_RESULT_ENUM_UINT32                enAcqRslt
)
{

    if(NAS_MSCC_PIF_ACQUIRED_RESULT_SUCC == enAcqRslt)
    {
        return MSCC_MMA_SYSTEM_ACQUIRE_RESULT_NORMAL_SERVICE;
    }
    else
    {
        return MSCC_MMA_SYSTEM_ACQUIRE_RESULT_NO_SERVICE;
    }

}




VOS_UINT32 NAS_MSCC_RcvMmaSysCfgReq_Main_L1Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* Save the entry message */
    NAS_MSCC_SaveCurEntryMsg(ulEventType, pstMsg);

    /* Start the System config FSM */
    NAS_MSCC_InitFsmL2(NAS_MSCC_FSM_SYSTEM_CONFIG);

    return VOS_TRUE;
}

/* 删除此函数 */


VOS_UINT32 NAS_MSCC_RcvMsccSysCfgCnf_Main_Deactive(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_SYS_CFG_SET_REQ_STRU                          *pstSysCfgReq = VOS_NULL_PTR;
    NAS_MSCC_MSCC_SYS_CFG_RSLT_CNF_STRU                    *pstSysCfgCnf = VOS_NULL_PTR;
    #if ((FEATURE_ON == FEATURE_UE_MODE_CDMA) && (FEATURE_ON == FEATURE_DSDS))
    VOS_UINT8                                               ucIsCLmodeLast;
    VOS_UINT8                                               ucIsCLmodeCurr;
    #endif

    pstSysCfgCnf = (NAS_MSCC_MSCC_SYS_CFG_RSLT_CNF_STRU *)pstMsg;

    pstSysCfgReq  = (MMA_MSCC_SYS_CFG_SET_REQ_STRU *)NAS_MSCC_GetCurrFsmEntryMsgBufferAddr();

    if (NAS_MSCC_SYS_CFG_RESULT_SUCCESS == pstSysCfgCnf->enRslt)
    {
        #if ((FEATURE_ON == FEATURE_UE_MODE_CDMA) && (FEATURE_ON == FEATURE_DSDS))
        ucIsCLmodeLast = NAS_MSCC_IsCLCombinedModeWithRatPrio(NAS_MSCC_GetRatPrioList());
        ucIsCLmodeCurr = NAS_MSCC_IsCLCombinedModeWithRatPrio(&(pstSysCfgReq->stRatPrioList));
        #endif

        /* set the user rat priority */
        NAS_MSCC_SavePrioRatList(&pstSysCfgReq->stRatPrioList);

        /*send the system config success result to MMA */
        NAS_MSCC_SndMmaSysCfgCnf(NAS_MSCC_PIF_SYS_CFG_SET_SUCCESS);

        /* Send the system acquire if Sleep timer is running */
        if (NAS_MSCC_TIMER_STATUS_RUNING == NAS_MSCC_GetTimerStatus(TI_NAS_MSCC_SLEEP_TIMER))
        {
            /* 在发送系统捕获后，停止sleep timer定时器 */
            NAS_MSCC_StopTimer(TI_NAS_MSCC_SLEEP_TIMER);

            NAS_MSCC_PerformSystemAcquireUponSysCfg_Main();

            /* Set the Current FSM state to Active */
            NAS_MSCC_SetCurrFsmState(NAS_MSCC_L1_STA_ACTIVE);
        }

        #if ((FEATURE_ON == FEATURE_UE_MODE_CDMA) && (FEATURE_ON == FEATURE_DSDS))
        /* 前后CL和GUL之间有变更时，上报 */
        if (ucIsCLmodeLast != ucIsCLmodeCurr)
        {
            NAS_MSCC_SndRrmRatCombModeInd();
        }
        #endif
        return VOS_TRUE;
    }

    NAS_MSCC_SndMmaSysCfgCnf(pstSysCfgCnf->enRslt);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMsccSysCfgCnf_Main_Active(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_SYS_CFG_SET_REQ_STRU                          *pstSysCfgReq  = VOS_NULL_PTR;
    NAS_MSCC_MSCC_SYS_CFG_RSLT_CNF_STRU                    *pstSysCfgCnf = VOS_NULL_PTR;
    #if ((FEATURE_ON == FEATURE_UE_MODE_CDMA) && (FEATURE_ON == FEATURE_DSDS))
    VOS_UINT8                                               ucIsCLmodeLast;
    VOS_UINT8                                               ucIsCLmodeCurr;
    #endif

    pstSysCfgCnf = (NAS_MSCC_MSCC_SYS_CFG_RSLT_CNF_STRU *)pstMsg;

    pstSysCfgReq  = (MMA_MSCC_SYS_CFG_SET_REQ_STRU *)NAS_MSCC_GetCurrFsmEntryMsgBufferAddr();


    if (NAS_MSCC_SYS_CFG_RESULT_SUCCESS == pstSysCfgCnf->enRslt)
    {
        /* 停止相关的定时器 */
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
        if (VOS_TRUE == NAS_MSCC_IsHrpdAndLteBothSupported())
        {
            NAS_MSCC_StopTimer(TI_NAS_MSCC_BSR_TIMER);
            NAS_MSCC_Reset1xServiceClSysAcqRelatedInfo_L1Main();

            NAS_MSCC_StopTimer(TI_NAS_MSCC_PERIOD_TRYING_HIGH_PRI_SYSTEM_SEARCH);

            NAS_MSCC_StopTimer(TI_NAS_MSCC_AVAILABLE_TIMER);

            NAS_MSCC_StopTimer(TI_NAS_MSCC_1X_BSR_LTE_TIMER);

            NAS_MSCC_StopTimer(TI_NAS_MSCC_1X_SERVICE_CL_SYSTEM_ACQUIRE_PHASE_ONE_TOTAL_TIMER);
            NAS_MSCC_ResetBsrRelatedCtrlInfo();
        }

        NAS_MSCC_StopTimer(TI_NAS_MSCC_SCAN_TIMER);
#endif
        #if ((FEATURE_ON == FEATURE_UE_MODE_CDMA) && (FEATURE_ON == FEATURE_DSDS))
        ucIsCLmodeLast = NAS_MSCC_IsCLCombinedModeWithRatPrio(NAS_MSCC_GetRatPrioList());
        ucIsCLmodeCurr = NAS_MSCC_IsCLCombinedModeWithRatPrio(&(pstSysCfgReq->stRatPrioList));
        #endif

        /* Note: firstly, set user rat priority. And then process system acquire. */
        NAS_MSCC_SavePrioRatList(&pstSysCfgReq->stRatPrioList);

        /*send the system config success result to MMA */
        NAS_MSCC_SndMmaSysCfgCnf(NAS_MSCC_PIF_SYS_CFG_SET_SUCCESS);

        NAS_MSCC_PerformSystemAcquireUponSysCfg_Main();

        #if ((FEATURE_ON == FEATURE_UE_MODE_CDMA) && (FEATURE_ON == FEATURE_DSDS))
        /* 前后CL和GUL之间有变更时，上报 */
        if (ucIsCLmodeLast != ucIsCLmodeCurr)
        {
            NAS_MSCC_SndRrmRatCombModeInd();
        }
        #endif
        return VOS_TRUE;
    }

    NAS_MSCC_SndMmaSysCfgCnf(pstSysCfgCnf->enRslt);

    return VOS_TRUE;
}


VOS_VOID NAS_MSCC_ProcPowerOffResult_Main(
    NAS_MSCC_POWEROFF_RSLT_ENUM_UINT32  enRslt
)
{
#if (FEATURE_ON == FEATURE_TDS_WCDMA_DYNAMIC_LOAD)
    VOS_RATMODE_ENUM_UINT32             enRatType;
#endif

    if (NAS_MSCC_POWEROFF_RSLT_POWER_SAVE_HRPD_LTE == enRslt)
    {
        NAS_MSCC_StopAllTimer();

        NAS_MSCC_Reset1xServiceClSysAcqRelatedInfo_L1Main();

        NAS_MSCC_ResetBsrRelatedCtrlInfo();

        return;
    }

    if (NAS_MSCC_POWEROFF_RSLT_POWER_OFF == enRslt)
    {
        NAS_MSCC_StopAllTimer();

        /* Send the Power off result cnf to MMA */
        NAS_MSCC_SndMmaPowerOffRsltCnf();

#if (FEATURE_ON == FEATURE_TDS_WCDMA_DYNAMIC_LOAD)
        if (VOS_TRUE == NAS_DYNLOAD_GetEnableDynloadTWFlg())
        {
            enRatType = NAS_DYNLOAD_GetCurRatInCached(NAS_DYNLOAD_LOAD_RATCOMB_TYPE_T_W);

            if (VOS_RATMODE_BUTT != enRatType)
            {
                NAS_NORMAL_LOG1(UEPS_PID_MSCC, "NAS_MSCC_ProcPowerOffResult_Main: RatType is ", (VOS_UINT32)enRatType);
            }

            (VOS_VOID)NAS_DYNLOAD_UnloadAS(NAS_DYNLOAD_LOAD_RATCOMB_TYPE_T_W, enRatType);
        }
#endif

        /* Reset the Context For MSCC */
        NAS_MSCC_InitCtx(NAS_MSCC_INIT_CTX_POWEROFF);

        /* set next state to NULL */
        NAS_MSCC_SetCurrFsmState(NAS_MSCC_L1_STA_NULL);

        return;
    }

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    /* stop all timer */
    NAS_MSCC_StopAllTimer();

    NAS_MSCC_ResetBsrRelatedCtrlInfo();

    NAS_MSCC_ResetCurOosAvailableTimerStartCount_L1Main();

    if (VOS_FALSE == NAS_MSCC_Get1xActiveFlg())
    {
        NAS_MSCC_SetCurrFsmState(NAS_MSCC_L1_STA_DEACTIVE);

        (VOS_VOID)NAS_MSCC_StartTimer(TI_NAS_MSCC_SLEEP_TIMER, NAS_MSCC_GetSleepTimerLength());
    }
#endif

    return;
}

#if (FEATURE_ON == FEATURE_TDS_WCDMA_DYNAMIC_LOAD)

VOS_VOID NAS_MSCC_ProcDynamicLoadAfterStartCnf_Main(VOS_VOID)
{
    VOS_RATMODE_ENUM_UINT32                                 enRatMode;
    NAS_UTRANCTRL_UTRAN_MODE_ENUM_UINT8                     enUtranMode;

    enRatMode = VOS_RATMODE_BUTT;

    if (VOS_TRUE == NAS_MSCC_IsGULModeConfigured())
    {
        enUtranMode = NAS_UTRANCTRL_GetCurrUtranMode();
        enRatMode   = NAS_UTRANCTRL_GetRatModeBasedOnUtranMode(enUtranMode);

        (VOS_VOID)NAS_DYNLOAD_LoadAS(NAS_DYNLOAD_LOAD_RATCOMB_TYPE_T_W, enRatMode);
    }

    return;
}
#endif

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

VOS_UINT32 NAS_MSCC_RcvMmaPlmnUserReselReq_Main_Deactive(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{


    NAS_MSCC_StopTimer(TI_NAS_MSCC_SLEEP_TIMER);

    NAS_MSCC_StartTimer(TI_NAS_MSCC_SCAN_TIMER, NAS_MSCC_GetScanTimerLength());

    NAS_MSCC_SetCurrFsmState(NAS_MSCC_L1_STA_ACTIVE);

    if (VOS_TRUE == NAS_MSCC_IsNeedPerformCLSysAcq())
    {
        NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_SLEEP_TIMER_EXPIRED, NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_SLEEP_TIMER_EXPIRED));

        return VOS_TRUE;
    }

    /* judge if need acqurie lte first, start lte acquire */
    if (VOS_TRUE == NAS_MSCC_IsNeedAcqLte())
    {
        NAS_MSCC_SndMmcPlmnSearchReq(NAS_MSCC_PIF_ACQ_REASON_OTHER,
                                     NAS_MSCC_PIF_PLMN_SEARCH_SPEC,
                                     VOS_NULL_PTR,
                                     VOS_NULL_PTR);

        NAS_MSCC_Set3gppCampOnFlag(VOS_FALSE);

        NAS_MSCC_Set3gppActiveFlg(VOS_TRUE);

        return VOS_TRUE;
    }

    /* Send HSD system acquire request if CDMA EVDO is supported */
    if (VOS_TRUE == NAS_MSCC_IsNeedAcqHrpd())
    {
        NAS_MSCC_SndHsdSysAcqReq(MSCC_HSD_ACQUIRE_REASON_OTHER,
                                 0,
                                 VOS_NULL_PTR);

        NAS_MSCC_SetHrpdCampOnFlag(VOS_FALSE);

        NAS_MSCC_SetHrpdActiveFlg(VOS_TRUE);
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaPlmnUserReselReq_Main_Active(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 如果available定时器在运行，表明DO&L都无服务，需要触发搜网 */
    if (NAS_MSCC_TIMER_STATUS_RUNING == NAS_MSCC_GetTimerStatus(TI_NAS_MSCC_AVAILABLE_TIMER))
    {
        NAS_MSCC_StopTimer(TI_NAS_MSCC_AVAILABLE_TIMER);

        NAS_MSCC_SndInternalSysAcqReq(NAS_MSCC_SYS_ACQ_SCENE_SLEEP_TIMER_EXPIRED, NAS_MSCC_GetSysAcqLteSrchType_L1Main(NAS_MSCC_SYS_ACQ_SCENE_SLEEP_TIMER_EXPIRED));
    }

    return VOS_TRUE;
}
#endif


VOS_UINT32 NAS_MSCC_RcvMsccPowerSaveReq_Main_Active(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 启动L2 power off状态机 */
    NAS_MSCC_InitFsmL2(NAS_MSCC_FSM_POWER_OFF);

    return VOS_TRUE;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


