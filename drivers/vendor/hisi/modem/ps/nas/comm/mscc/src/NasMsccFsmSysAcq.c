


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "NasMsccFsmSysAcq.h"
#include "NasComm.h"
#include "NasMsccProcNvim.h"
#include "NasMsccCtx.h"
#include "NasMsccComFunc.h"
#include "NasErrLogCcbCtx.h"
#include "NasMsccSndMma.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_NAS_MSCC_FSM_SYS_ACQ_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/*****************************************************************************
  3 函数定义
*****************************************************************************/

VOS_UINT32 NAS_MSCC_RcvMsccSysAcqReq_SysAcq_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_MSCC_SYS_ACQ_STRU                             *pstSystemAcqReq;
    NAS_MSCC_SYS_ACQ_SYS_LIST_INFO_STRU                    *pstSysListInfo;
    VOS_UINT32                                              ulRslt;
    NAS_MSCC_SYS_ACQ_SYS_INFO_STRU                          stNextAcqSysInfo;
    NAS_MSCC_BUILD_SYS_ACQ_LIST_SCENE_ENUM_UINT32           enBuildSysAcqListScene;

    /* Save the entry message */
    NAS_MSCC_SaveCurEntryMsg(ulEventType, pstMsg);

    pstSystemAcqReq = (NAS_MSCC_MSCC_SYS_ACQ_STRU*)pstMsg;

    pstSysListInfo  = NAS_MSCC_GetCLSysAcqSysList_SysAcq();

    if (NAS_MSCC_SYS_ACQ_SCENE_HRPD_LOST_NO_RF == pstSystemAcqReq->enSysAcqScene)
    {
        NAS_MSCC_SetInterSysSearchHrpdNoRfFlag_SysAcq(VOS_TRUE);
    }

    NAS_MSCC_ConvertSysAcqSceneToBuildSysAcqListScene(pstSystemAcqReq->enSysAcqScene,
                                                     &enBuildSysAcqListScene);

    NAS_MSCC_BuildSysAcqList(enBuildSysAcqListScene,
                             VOS_NULL_PTR,
                             VOS_NULL_PTR,
                             pstSysListInfo);

    NAS_MSCC_UpdateClInterSysSearchFlagWithSysAcqScence(enBuildSysAcqListScene);


    PS_MEM_SET(&stNextAcqSysInfo,
               0x00,
               sizeof(NAS_MSCC_SYS_ACQ_SYS_INFO_STRU));

    /* send next acquire rat mode for normal aquire req */
    ulRslt = NAS_MSCC_GetNextNormalAcquireSysInAcqSysList(pstSysListInfo,
                                                         &stNextAcqSysInfo);

    if (VOS_TRUE == ulRslt)
    {
        /* search network or power save */
        NAS_MSCC_SrchNetwork_SysAcq(stNextAcqSysInfo.ulRatMode);

        /* 使用1x或lte无卡初搜位置信息,发送搜网之后，恢复无效值 */
        NAS_MSCC_ResetNoCardInitSearchInfo();

        return VOS_TRUE;
    }

    /* if get the next Acq sys Info fail, it should process intial search to get loaction */
    ulRslt = NAS_MSCC_GetNextInitialAcquireSysInAcqSysList(pstSysListInfo,
                                                          &stNextAcqSysInfo);

    if (VOS_FALSE == ulRslt)
    {
        /* abnormal case, should quit fsm with fail result */
        NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_FAILURE);

        /* 使用1x或lte无卡初搜位置信息,发送搜网之后，恢复无效值 */
        NAS_MSCC_ResetNoCardInitSearchInfo();

        return VOS_TRUE;
    }

    NAS_MSCC_SrchInitLoc_SysAcq(stNextAcqSysInfo.ulRatMode);

    /* 使用1x或lte无卡初搜位置信息,发送搜网之后，恢复无效值 */
    NAS_MSCC_ResetNoCardInitSearchInfo();

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaCdmaMoCallRedialSysAcqNtf_SysAcq_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_CDMA_MO_CALL_REDIAL_SYS_ACQ_NTF_STRU          *pstCdmaMoCallRedialSysAcqNtf = VOS_NULL_PTR;
    NAS_MSCC_SYS_ACQ_SYS_LIST_INFO_STRU                    *pstSysListInfo;

    pstCdmaMoCallRedialSysAcqNtf = (MMA_MSCC_CDMA_MO_CALL_REDIAL_SYS_ACQ_NTF_STRU*)pstMsg;

    /* Save the entry message */
    NAS_MSCC_SaveCurEntryMsg(ulEventType, pstMsg);

    pstSysListInfo  = NAS_MSCC_GetCLSysAcqSysList_SysAcq();

    NAS_MSCC_BuildSysAcqList(NAS_MSCC_BUILD_SYS_ACQ_LIST_SCENE_QUICK_SEARCH_HRPD_MRUO,
                             VOS_NULL_PTR,
                             VOS_NULL_PTR,
                             pstSysListInfo);

    NAS_MSCC_SetWaitQuickSrchDoCnfFlg_SysAcq(VOS_TRUE);

    NAS_MSCC_SndHsdCdmaMoCallRedialSysAcqNtf(pstCdmaMoCallRedialSysAcqNtf);

    NAS_MSCC_SetHrpdCampOnFlag(VOS_FALSE);

    NAS_MSCC_SetHrpdActiveFlg(VOS_TRUE);

    NAS_MSCC_SetCurrFsmState(NAS_MSCC_SYSACQ_STA_WAIT_HSD_ACQUIRE_CNF);

    NAS_MSCC_StartTimer(TI_NAS_MSCC_WAIT_HSD_ACQUIRE_CNF,
                        TI_NAS_MSCC_WAIT_HSD_ACQUIRE_CNF_LEN);


    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvHsdDataCallRedialAcqCnf_SysAcq_WaitHsdSysAcqCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_SYS_ACQ_SYS_LIST_INFO_STRU                    *pstSysListInfo;
    HSD_MSCC_DATA_CALL_REDIAL_SYS_ACQ_CNF_STRU             *pstDataCallSysAcqCnf;

    pstDataCallSysAcqCnf = (HSD_MSCC_DATA_CALL_REDIAL_SYS_ACQ_CNF_STRU *)pstMsg;

    /* 当前不需要等待HSD的快搜响应,丢弃该消息 */
    if (VOS_FALSE == NAS_MSCC_GetWaitQuickSrchDoCnfFlg_SysAcq())
    {
        NAS_WARNING_LOG(UEPS_PID_MSCC, "NAS_MSCC_RcvHsdDataCallRedialAcqCnf_SysAcq_WaitHsdSysAcqCnf:invalid system acquire scene!");

        return VOS_TRUE;
    }


    NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_HSD_ACQUIRE_CNF);

    NAS_MSCC_SetInterSysSearchHrpdFlag_SysAcq(VOS_TRUE);

    pstSysListInfo      = NAS_MSCC_GetCLSysAcqSysList_SysAcq();

    if (NAS_MSCC_PIF_ACQUIRED_RESULT_SUCC == pstDataCallSysAcqCnf->enRslt)
    {
        /* refresh the system acquire result */
        NAS_MSCC_UpdateAcqRatStatusInNormalAcqSysList(VOS_RATMODE_HRPD,
                                                      NAS_MSCC_SYS_ACQ_STATUS_SEARCHED_REGISTERED,
                                                      pstSysListInfo);

        NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_SUCCESS);

        return VOS_TRUE;
    }

    if (NAS_MSCC_PIF_ACQUIRED_RESULT_ABORTED == pstDataCallSysAcqCnf->enRslt)
    {
        NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_FAILURE);

        return VOS_TRUE;
    }

    /* refresh the system acquire result */
    NAS_MSCC_UpdateAcqRatStatusInNormalAcqSysList(VOS_RATMODE_HRPD,
                                                  NAS_MSCC_SYS_ACQ_STATUS_SEARCHED_NO_EXIST,
                                                  pstSysListInfo);

    /* system acquire failed, so power save hrpd */
    NAS_MSCC_SndHsdPowerSaveReq_SysAcq();

    return VOS_TRUE;
}






VOS_UINT32 NAS_MSCC_RcvHsdSysAcqCnf_SysAcq_WaitHsdSysAcqCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    HSD_MSCC_SYSTEM_ACQUIRE_CNF_STRU                       *pstSysAcqCnf;
    NAS_MSCC_SYS_ACQ_SYS_LIST_INFO_STRU                    *pstSysListInfo;

    pstSysAcqCnf                     = (HSD_MSCC_SYSTEM_ACQUIRE_CNF_STRU *)pstMsg;

    /* 需要上班HRPD无服务*/
    if (NAS_MSCC_PIF_ACQUIRED_RESULT_SUCC != pstSysAcqCnf->enRslt)
    {
        NAS_MSCC_SndMmaHrpdServiceStatusInd(MSCC_MMA_HRPD_SERVICE_STATUS_NO_SERVICE,
                                            VOS_FALSE,
                                            VOS_FALSE,
                                            MSCC_MMA_SESSION_RELEASE_TYPE_BUTT);
    }

    /* 需要等待HSD的快搜响应,收到搜网回复,直接丢弃 */
    if (VOS_TRUE == NAS_MSCC_GetWaitQuickSrchDoCnfFlg_SysAcq())
    {
        NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_RcvHsdSysAcqCnf_SysAcq_WaitHsdSysAcqCnf:discard acq cnf and continue wait data call redial sys acq ntf msg!");

        return VOS_TRUE;
    }

    NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_HSD_ACQUIRE_CNF);


    NAS_MSCC_SetInterSysSearchHrpdFlag_SysAcq(VOS_TRUE);

    pstSysListInfo      = NAS_MSCC_GetCLSysAcqSysList_SysAcq();

    switch (pstSysAcqCnf->enRslt)
    {
        case NAS_MSCC_PIF_ACQUIRED_RESULT_SUCC:
            {
                /* refresh the system acquire result */
                NAS_MSCC_UpdateAcqRatStatusInNormalAcqSysList(VOS_RATMODE_HRPD,
                                                              NAS_MSCC_SYS_ACQ_STATUS_SEARCHED_REGISTERED,
                                                              pstSysListInfo);
                NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_SUCCESS);
            }
            break;

        case NAS_MSCC_PIF_ACQUIRED_RESULT_ABORTED:
            {
                NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_FAILURE);
            }
            break;

        case NAS_MSCC_PIF_ACQUIRED_RESULT_NO_RF:
            {
                NAS_MSCC_SetInterSysSearchHrpdNoRfFlag_SysAcq(VOS_TRUE);

                /* SRLTE版本处理 */
                if (VOS_TRUE == NAS_MSCC_IsSrlte())
                {
                    /***********************************************************************************************
                       如果是同步场景下的NO RF，退出搜网状态后，power save HRPD， 等待RF available资源可用时再触发搜网；
                     **********************************************************************************************/
                    NAS_MSCC_UpdateAcqRatStatusInNormalAcqSysList(VOS_RATMODE_LTE,
                                                                  NAS_MSCC_SYS_ACQ_STATUS_SEARCHED_NO_EXIST,
                                                                  pstSysListInfo);

                    NAS_MSCC_UpdateAcqRatStatusInNormalAcqSysList(VOS_RATMODE_HRPD,
                                                                  NAS_MSCC_SYS_ACQ_STATUS_SEARCHED_NO_EXIST,
                                                                  pstSysListInfo);

                    NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_SRCH_NO_RF);
                }
                /* SVLTE版本处理 */
                else
                {
                    /* refresh the system acquire result */
                    NAS_MSCC_UpdateAcqRatStatusInNormalAcqSysList(VOS_RATMODE_HRPD,
                                                                  NAS_MSCC_SYS_ACQ_STATUS_SEARCHED_NO_EXIST,
                                                                  pstSysListInfo);

                    /* system acquire failed, so power save hrpd */
                    NAS_MSCC_SndHsdPowerSaveReq_SysAcq();
                }
            }
            break;

        case NAS_MSCC_PIF_ACQUIRED_RESULT_REG_NO_RF:
            {
                /***********************************************************************************
                   如果已经驻留，是session协商导致的NO RF，则退出流程，由HRPD自己等待RF可用时，再做协商；
                 **********************************************************************************/
                NAS_MSCC_UpdateAcqRatStatusInNormalAcqSysList(VOS_RATMODE_HRPD,
                                                              NAS_MSCC_SYS_ACQ_STATUS_SEARCHED_UNREGISTERED,
                                                              pstSysListInfo);

                NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_REG_NO_RF);
            }
            break;

        case NAS_MSCC_PIF_ACQUIRED_RESULT_FAIL:
        default:
            {
                /* refresh the system acquire result */
                NAS_MSCC_UpdateAcqRatStatusInNormalAcqSysList(VOS_RATMODE_HRPD,
                                                              NAS_MSCC_SYS_ACQ_STATUS_SEARCHED_NO_EXIST,
                                                              pstSysListInfo);

                /* system acquire failed, so power save hrpd */
                NAS_MSCC_SndHsdPowerSaveReq_SysAcq();
            }
            break;
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmaCdmaMoCallRedialSysAcqNtf_SysAcq_WaitHsdSysAcqCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMA_MSCC_CDMA_MO_CALL_REDIAL_SYS_ACQ_NTF_STRU          *pstCdmaMoCallRedialSysAcqNtf = VOS_NULL_PTR;

    pstCdmaMoCallRedialSysAcqNtf = (MMA_MSCC_CDMA_MO_CALL_REDIAL_SYS_ACQ_NTF_STRU*)pstMsg;

    /* 需要等待HSD的快搜响应,再次收到MMA的快搜请求,直接丢弃该消息 */
    if (VOS_TRUE == NAS_MSCC_GetWaitQuickSrchDoCnfFlg_SysAcq())
    {
        NAS_WARNING_LOG(UEPS_PID_MSCC, "NAS_MSCC_RcvMmaCdmaMoCallRedialSysAcqNtf_SysAcq_WaitHsdSysAcqCnf:discard duplicated msg!");

        return VOS_TRUE;
    }

    NAS_MSCC_SetWaitQuickSrchDoCnfFlg_SysAcq(VOS_TRUE);

    NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_HSD_ACQUIRE_CNF);

    NAS_MSCC_SndHsdCdmaMoCallRedialSysAcqNtf(pstCdmaMoCallRedialSysAcqNtf);

    /* Set the Camp On flag to FALSE */
    NAS_MSCC_SetHrpdCampOnFlag(VOS_FALSE);

    NAS_MSCC_SetHrpdActiveFlg(VOS_TRUE);

    NAS_MSCC_StartTimer(TI_NAS_MSCC_WAIT_HSD_ACQUIRE_CNF,
                        TI_NAS_MSCC_WAIT_HSD_ACQUIRE_CNF_LEN);

    return VOS_TRUE;
}




VOS_UINT32 NAS_MSCC_RcvMsccAbortFsmReq_SysAcq_WaitHsdSysAcqCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_HSD_ACQUIRE_CNF);

    NAS_MSCC_SetAbortFlag_SysAcq(VOS_TRUE);

    NAS_MSCC_SndHsdPowerSaveReq_SysAcq();

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvHsdSysSyncInd_SysAcq_WaitHsdSysAcqCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_FSM_SYS_ACQ_CTX_STRU                          *pstSysAcqFsmCtx;
    NAS_MSCC_SYS_ACQ_SYS_LIST_INFO_STRU                    *pstSysListInfo;

    if (VOS_TRUE == NAS_MSCC_IsNeedReAcqLte_SysAcq())
    {
        NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_RcvHsdSysSyncInd_SysAcq_WaitHsdSysAcqCnf: Need Reacquire Lte!");

        pstSysListInfo      = NAS_MSCC_GetCLSysAcqSysList_SysAcq();
        pstSysAcqFsmCtx     = NAS_MSCC_GetSysAcqFsmCtxAddr();

        pstSysAcqFsmCtx->ucNeedReAcqLteFlg = VOS_FALSE;
        NAS_MSCC_ResetAcqRatStatusInNormalAcqSysList(pstSysListInfo);

        NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_HSD_ACQUIRE_CNF);
        NAS_MSCC_SndHsdPowerSaveReq_SysAcq();
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvTiWaitHsdSysAcqCnfExpired_SysAcq_WaitHsdSysAcqCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_SYS_ACQ_SYS_LIST_INFO_STRU                    *pstSysListInfo;

    NAS_WARNING_LOG(UEPS_PID_MSCC, "NAS_MSCC_RcvTiWaitHsdSysAcqCnfExpired_SysAcq_WaitHsdSysAcqCnf: timer out");

    pstSysListInfo      = NAS_MSCC_GetCLSysAcqSysList_SysAcq();

    /* refresh the system acquire result */
    NAS_MSCC_UpdateAcqRatStatusInNormalAcqSysList(VOS_RATMODE_HRPD,
                                                  NAS_MSCC_SYS_ACQ_STATUS_SEARCHED_NO_EXIST,
                                                  pstSysListInfo);

    NAS_MSCC_SetInterSysSearchHrpdFlag_SysAcq(VOS_TRUE);

    NAS_MSCC_SndHsdPowerSaveReq_SysAcq();

#if (FEATURE_PTM == FEATURE_ON)
    /* HRPD搜网,在mscc被打断  */
    NAS_MSCC_SaveErrLogCcbSysAcqEndInfo(VOS_RATMODE_HRPD, NAS_ERR_LOG_CL_SEARCH_RSLT_HRPD_TIMEOUT);
#endif

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvHsdPowerSaveCnf_SysAcq_WaitHsdPowerSaveCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_SYS_ACQ_SYS_LIST_INFO_STRU                    *pstSysListInfo;
    VOS_UINT32                                              ulRslt;
    NAS_MSCC_SYS_ACQ_SYS_INFO_STRU                          stNextAcqSysInfo;

    NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_HSD_POWER_SAVE_CNF);

    if (VOS_TRUE == NAS_MSCC_GetAbortFlag_SysAcq())
    {
        NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_ABORTED);

        return VOS_TRUE;
    }

    pstSysListInfo      = NAS_MSCC_GetCLSysAcqSysList_SysAcq();

    /* send next acquire rat mode for normal aquire req */
    ulRslt = NAS_MSCC_GetNextNormalAcquireSysInAcqSysList(pstSysListInfo,
                                                         &stNextAcqSysInfo);

    if (VOS_FALSE == ulRslt)
    {
        NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_FAILURE);

        return VOS_TRUE;
    }

    NAS_MSCC_SndMmcPlmnSearchReq_SysAcq();

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMsccAbortFsmReq_SysAcq_WaitHsdPowerSaveCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
     NAS_MSCC_SetAbortFlag_SysAcq(VOS_TRUE);
     return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmaCdmaMoCallRedialSysAcqNtf_SysAcq_WaitMmcPlmnSrchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    /* 全频段搜LTE的时候如果收到DO的快搜请求,直接回复失败;否则进后处理做打断处理 */
    if (NAS_MSCC_PIF_PLMN_SEARCH_SPEC == NAS_MSCC_GetSysAcqLteSrchType_SysAcq())
    {
        NAS_MSCC_SndMmaDataCallRedialSysAcqInd(NAS_MSCC_PIF_ACQUIRED_RESULT_FAIL, VOS_FALSE);

        return VOS_TRUE;
    }

    return VOS_FALSE;
}



VOS_UINT32 NAS_MSCC_RcvTiWaitHsdPowerSaveCnfExpired_SysAcq_WaitHsdPowerSaveCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_WARNING_LOG(UEPS_PID_MSCC, "NAS_MSCC_RcvTiWaitHsdPowerSaveCnfExpired_SysAcq_WaitHsdPowerSaveCnf: timer out");

    if (VOS_TRUE == NAS_MSCC_GetAbortFlag_SysAcq())
    {
        NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_ABORTED);

        return VOS_TRUE;
    }

    NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_FAILURE);

    return VOS_TRUE;
}


VOS_VOID NAS_MSCC_ProcessMmcPlmnSrchCnfWithSrchNoRf_SysAcq(
    MMC_MSCC_PLMN_SELECTION_RLST_IND_STRU                  *pstRsltInd,
    NAS_MSCC_SYS_ACQ_SYS_LIST_INFO_STRU                    *pstSysListInfo
)
{
    NAS_MSCC_FSM_SYS_ACQ_CTX_STRU                          *pstSysAcqFsmCtx = VOS_NULL_PTR;

    NAS_MSCC_SetInterSysSearchLteNoRfFlag_SysAcq(VOS_TRUE);

    /* SRLTE版本处理 */
    if (VOS_TRUE == NAS_MSCC_IsSrlte())
    {
        /*******************************************************************************************
         * 搜网阶段出现的NO RF，则退出状态机后，power save LTE
         ******************************************************************************************/
        /* 刷新LTE为搜索不存在 */
        NAS_MSCC_UpdateAcqRatStatusInNormalAcqSysList(VOS_RATMODE_HRPD,
                                                      NAS_MSCC_SYS_ACQ_STATUS_SEARCHED_NO_EXIST,
                                                      pstSysListInfo);
        /* 刷新LTE为搜索不存在 */
        NAS_MSCC_UpdateAcqRatStatusInNormalAcqSysList(VOS_RATMODE_LTE,
                                                      NAS_MSCC_SYS_ACQ_STATUS_SEARCHED_NO_EXIST,
                                                      pstSysListInfo);

        NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_SRCH_NO_RF);

        return;
    }

    /* SVLTE处理 */
    /* Lte下NO RF时,开机场景下副卡进行LAU流程导致主卡Lte搜索失败,优先驻留到DO,此处根据NV配置的,对于
       开机场景,lte下NO RF场景,根据NV配置时间等待一段时间,待副卡注册流程完成后再尝试LTE，保证优先驻
       留LTE;此处先让LTE进从模 */
    if (VOS_TRUE == NAS_MSCC_IsNeedReAcqLteUponLteNoRf_SysAcq())
    {
        NAS_MSCC_SetPwrSaveLteUponReAcqLteNoRfFlg_SysAcq(VOS_TRUE);

        NAS_MSCC_SndMmcPowerSaveReq_SysAcq();

        return;
    }

    if (VOS_TRUE == pstRsltInd->ucLteCoverageFlg)
    {
        pstSysAcqFsmCtx  = NAS_MSCC_GetSysAcqFsmCtxAddr();

        /* lte is not the normal service and the coverage is ok, so set the ReAcqLteFlg to false */
        pstSysAcqFsmCtx->ucNeedReAcqLteFlg = VOS_FALSE;
    }

    /* refresh the system acquire result */
    NAS_MSCC_UpdateAcqRatStatusInNormalAcqSysList(VOS_RATMODE_LTE,
                                                  NAS_MSCC_SYS_ACQ_STATUS_SEARCHED_NO_EXIST,
                                                  pstSysListInfo);

    /* Plmn search cnf failed, so power save the lte */
    NAS_MSCC_SndMmcPowerSaveReq_SysAcq();

    return;
}


VOS_VOID NAS_MSCC_ProcessMmcPlmnSrchCnfWithRegNoRf_SysAcq(
    MMC_MSCC_PLMN_SELECTION_RLST_IND_STRU                  *pstRsltInd,
    NAS_MSCC_SYS_ACQ_SYS_LIST_INFO_STRU                    *pstSysListInfo
)
{
    NAS_MSCC_FSM_SYS_ACQ_CTX_STRU                          *pstSysAcqFsmCtx = VOS_NULL_PTR;

    /* SRLTE版本处理 */
    if (VOS_TRUE == NAS_MSCC_IsSrlte())
    {
        /*******************************************************************************************
         * 如果注册失败，原因是No RF，可以不需要发起HRPD的搜索，仍然驻留LTE，LTE会自己注册RF资源，待
         * 上行射频发射资源释放后，LTE自行再发起注册;
         ******************************************************************************************/
        NAS_MSCC_UpdateAcqRatStatusInNormalAcqSysList(VOS_RATMODE_LTE,
                                                      NAS_MSCC_SYS_ACQ_STATUS_SEARCHED_UNREGISTERED,
                                                      pstSysListInfo);


        NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_REG_NO_RF);

        return;
    }

    /* SVLTE版本处理 */
    if (VOS_TRUE == pstRsltInd->ucLteCoverageFlg)
    {
        pstSysAcqFsmCtx  = NAS_MSCC_GetSysAcqFsmCtxAddr();

        /* lte is not the normal service and the coverage is ok, so set the ReAcqLteFlg to false */
        pstSysAcqFsmCtx->ucNeedReAcqLteFlg = VOS_FALSE;
    }

    /* refresh the system acquire result */
    NAS_MSCC_UpdateAcqRatStatusInNormalAcqSysList(VOS_RATMODE_LTE,
                                                  NAS_MSCC_SYS_ACQ_STATUS_SEARCHED_NO_EXIST,
                                                  pstSysListInfo);

    /* Plmn search cnf failed, so power save the lte */
    NAS_MSCC_SndMmcPowerSaveReq_SysAcq();

    return;
}



VOS_UINT32 NAS_MSCC_RcvMmcPlmnSrchCnf_SysAcq_WaitMmcPlmnSrchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_PLMN_SELECTION_RLST_IND_STRU                  *pstMmcPlmnSeleRlstInd;
    NAS_MSCC_FSM_SYS_ACQ_CTX_STRU                          *pstSysAcqFsmCtx;
    NAS_MSCC_SYS_ACQ_SYS_LIST_INFO_STRU                    *pstSysListInfo;

    pstMmcPlmnSeleRlstInd                     = (MMC_MSCC_PLMN_SELECTION_RLST_IND_STRU *)pstMsg;

    NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_MMC_PLMN_SEARCH_CNF);

    NAS_MSCC_SetInterSysSearchLteFlag_SysAcq(VOS_TRUE);

    pstSysAcqFsmCtx  = NAS_MSCC_GetSysAcqFsmCtxAddr();

    pstSysListInfo   = NAS_MSCC_GetCLSysAcqSysList_SysAcq();

    switch (pstMmcPlmnSeleRlstInd->enResult)
    {
        case NAS_MSCC_PIF_PLMN_SELECTION_RESULT_NORMAL_SERVICE:
            {
                /* refresh the system acquire result */
                NAS_MSCC_UpdateAcqRatStatusInNormalAcqSysList(VOS_RATMODE_LTE,
                                                              NAS_MSCC_SYS_ACQ_STATUS_SEARCHED_REGISTERED,
                                                              pstSysListInfo);

                NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_SUCCESS);
            }
            break;

        case NAS_MSCC_PIF_PLMN_SELECTION_RESULT_INTERRUPT:
            {
                NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_FAILURE);
            }
            break;

        case NAS_MSCC_PIF_PLMN_SELECTION_RESULT_NO_RF:
            {
                NAS_MSCC_ProcessMmcPlmnSrchCnfWithSrchNoRf_SysAcq(pstMmcPlmnSeleRlstInd, pstSysListInfo);
            }
            break;
        case NAS_MSCC_PIF_PLMN_SELECTION_RESULT_REG_NO_RF:
            {
                NAS_MSCC_ProcessMmcPlmnSrchCnfWithRegNoRf_SysAcq(pstMmcPlmnSeleRlstInd, pstSysListInfo);
            }
            break;

        case NAS_MSCC_PIF_PLMN_SELECTION_RESULT_LIMITED_SERVICE:
        case NAS_MSCC_PIF_PLMN_SELECTION_RESULT_NO_SERVICE:
        case NAS_MSCC_PIF_PLMN_SELECTION_RESULT_L2C:
        default:
            {
                if (VOS_TRUE == pstMmcPlmnSeleRlstInd->ucLteCoverageFlg)
                {
                    /* lte is not the normal service and the coverage is ok, so set the ReAcqLteFlg to false */
                    pstSysAcqFsmCtx->ucNeedReAcqLteFlg = VOS_FALSE;
                }

                /* refresh the system acquire result */
                NAS_MSCC_UpdateAcqRatStatusInNormalAcqSysList(VOS_RATMODE_LTE,
                                                              NAS_MSCC_SYS_ACQ_STATUS_SEARCHED_NO_EXIST,
                                                              pstSysListInfo);

                /* Plmn search cnf failed, so power save the lte */
                NAS_MSCC_SndMmcPowerSaveReq_SysAcq();
            }
            break;
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMsccAbortFsmReq_SysAcq_WaitMmcPlmnSrchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_MMC_PLMN_SEARCH_CNF);

    NAS_MSCC_SetAbortFlag_SysAcq(VOS_TRUE);

    NAS_MSCC_SndMmcPowerSaveReq_SysAcq();

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMmcInterSysStartInd_SysAcq_WaitMmcPlmnSrchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT32                                              ulReaminTimerLen;
    VOS_UINT32                                              ulInterSysLen;
    MMC_MSCC_INTERSYS_START_IND_STRU                       *pstInterSysStartInd;

    pstInterSysStartInd = (MMC_MSCC_INTERSYS_START_IND_STRU *)pstMsg;

    NAS_MSCC_GetRemainTimerLen(TI_NAS_MSCC_WAIT_MMC_PLMN_SEARCH_CNF, &ulReaminTimerLen);

    /* 重选 */
    if (NAS_MSCC_PIF_INTERSYS_CAUSE_RESELECT == pstInterSysStartInd->enInterSysCause)
    {
        ulInterSysLen = TI_NAS_MSCC_WAIT_CL_INTERSYS_END_IND_L2C_RESEL_LEN;
    }
    /* 重定向 */
    else
    {
        ulInterSysLen = TI_NAS_MSCC_WAIT_CL_INTERSYS_END_IND_L2C_REDIR_LEN;
    }

    NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_MMC_PLMN_SEARCH_CNF);

    /* 延长等待MMC的搜网回复定时器 */
    ulInterSysLen += ulReaminTimerLen;

    NAS_MSCC_StartTimer(TI_NAS_MSCC_WAIT_MMC_PLMN_SEARCH_CNF, ulInterSysLen);

    NAS_MSCC_InitFsmL2(NAS_MSCC_FSM_CL_INTERSYS);

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvMsccCLInterSysRsltCnf_SysAcq_WaitMmcPlmnSrchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_MSCC_CL_INTERSYS_RSLT_CNF_STRU                *pstClInterSysRslt;

    pstClInterSysRslt = (NAS_MSCC_MSCC_CL_INTERSYS_RSLT_CNF_STRU *)pstMsg;

    /* 出现L2C乒乓重选的情况时，也可以通知aps切换，所以发生L2C重选，也认为L下已经尝试过搜索，后续可结合搜索LTE和Hrpd标识、搜网结果通知APS */
    NAS_MSCC_SetInterSysSearchLteFlag_SysAcq(VOS_TRUE);

    if (NAS_MSCC_CL_INTERSYS_RESULT_SUCC == pstClInterSysRslt->enRslt)
    {
        /* LTE异系统变换到HRPD退出状态机 */
        if (VOS_TRUE == NAS_MSCC_GetHrpdActiveFlg())
        {
            NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_MMC_PLMN_SEARCH_CNF);

            NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_L2C);
        }
    }

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_RcvTiWaitMmcPlmnSrchCnfExpired_SysAcq_WaitMmcPlmnSrchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_SYS_ACQ_SYS_LIST_INFO_STRU                    *pstSysListInfo;

    NAS_WARNING_LOG(UEPS_PID_MSCC, "NAS_MSCC_RcvTiWaitMmcPlmnSrchCnfExpired_SysAcq_WaitMmcPlmnSrchCnf: timer out");

    pstSysListInfo  = NAS_MSCC_GetCLSysAcqSysList_SysAcq();
    /* refresh the system acquire result */
    NAS_MSCC_UpdateAcqRatStatusInNormalAcqSysList(VOS_RATMODE_LTE,
                                                  NAS_MSCC_SYS_ACQ_STATUS_SEARCHED_NO_EXIST,
                                                  pstSysListInfo);

    NAS_MSCC_SetInterSysSearchLteFlag_SysAcq(VOS_TRUE);

    NAS_MSCC_SndMmcPowerSaveReq_SysAcq();

#if (FEATURE_PTM == FEATURE_ON)
    /* LTE搜网等待MMC响应超时  */
    NAS_MSCC_SaveErrLogCcbSysAcqEndInfo(VOS_RATMODE_LTE, NAS_ERR_LOG_CL_SEARCH_RSLT_LTE_TIMEOUT);
#endif

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmcPowerSaveCnf_SysAcq_WaitMmcPowerSaveCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_SYS_ACQ_SYS_LIST_INFO_STRU                    *pstSysListInfo;
    VOS_UINT32                                              ulRslt;
    NAS_MSCC_SYS_ACQ_SYS_INFO_STRU                          stNextAcqSysInfo;
    VOS_UINT32                                              ulEpsAttachAllowFlg;
    MMC_MSCC_POWER_SAVE_CNF_STRU                           *pstPowerSaveCnf;

    ulEpsAttachAllowFlg = NAS_MSCC_GetEpsAttachAllowFlg();

    pstPowerSaveCnf = (MMC_MSCC_POWER_SAVE_CNF_STRU*)pstMsg;

    if (NAS_MSCC_PIF_POWER_SAVE_FAILURE == pstPowerSaveCnf->enResult)
    {
        NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_MMC_POWER_SAVE_CNF);

        NAS_WARNING_LOG(UEPS_PID_MSCC, "NAS_MSCC_RcvMmcPowerSaveCnf_SysAcq_WaitMmcPowerSaveCnf:power save failure!");

        NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_FAILURE);

        return VOS_TRUE;
    }

    NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_MMC_POWER_SAVE_CNF);

    if (VOS_TRUE == NAS_MSCC_GetAbortFlag_SysAcq())
    {
        NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_ABORTED);

        return VOS_TRUE;
    }

    /* LTE注册被拒卡无效或EPS被detach,退出状态机,状态机外面触发搜HRPD */
    if ((VOS_FALSE == NAS_MSCC_GetSimPsRegStatus())
     || (VOS_FALSE == ulEpsAttachAllowFlg))
    {
        NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_FAILURE);

        return VOS_TRUE;
    }

    /* LTE NO RF时需要重新捕获一次LTE,之前记录过power save标记, 此时迁移状态等待RF可用指示 */
    if (VOS_TRUE == NAS_MSCC_GetPwrSaveLteUponReAcqLteNoRfFlg_SysAcq())
    {
        NAS_MSCC_SetPwrSaveLteUponReAcqLteNoRfFlg_SysAcq(VOS_FALSE);

        NAS_MSCC_SetCurrFsmState(NAS_MSCC_SYSACQ_STA_WAIT_MMC_RF_AVAILABLE_IND);

        NAS_MSCC_StartTimer(TI_NAS_MSCC_WAIT_MMC_RF_AVAILABLE_IND, NAS_MSCC_GetReAcqLteWithLteNoRfDelayTime() * NAS_MSCC_THOUSAND_MILLISECOND_UNIT);

        return VOS_TRUE;
    }

    /* 如果是SRLTE下，TCH下搜LTE失败后，如果此时仍然是TCH态，退出当前搜网流程，否可以尝试搜一下搜DO */
    if ((NAS_MSCC_SYS_ACQ_SCENE_AVAILABLE_TIMER_EXPIRED_SRLTE_1X_TCH == NAS_MSCC_GetCLSysAcqScene_SysAcq())
     && (NAS_MSCC_PIF_1X_CAS_TCH_STATE                               == NAS_MSCC_Get1xUeMainState()))
    {
        /* 强制设置下HRPD为已搜索，确保退出状态后，acquire end ind能上报给MMA */
        NAS_MSCC_SetInterSysSearchHrpdFlag_SysAcq(VOS_TRUE);

        NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_FAILURE);

        return VOS_TRUE;
    }

    pstSysListInfo = NAS_MSCC_GetCLSysAcqSysList_SysAcq();

    /* send next acquire rat mode for normal aquire req */
    ulRslt = NAS_MSCC_GetNextNormalAcquireSysInAcqSysList(pstSysListInfo,
                                                         &stNextAcqSysInfo);

    if (VOS_FALSE == ulRslt)
    {
        NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_FAILURE);

        return VOS_TRUE;
    }

    NAS_MSCC_SndHsdSysAcqReq_SysAcq();

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMsccAbortFsmReq_SysAcq_WaitMmcPowerSaveCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
     NAS_MSCC_SetAbortFlag_SysAcq(VOS_TRUE);
     return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvTiWaitMmcPowerSaveCnfExpired_SysAcq_WaitMmcPowerSaveCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_WARNING_LOG(UEPS_PID_MSCC, "NAS_MSCC_RcvTiWaitMmcPowerSaveCnfExpired_SysAcq_WaitMmcPowerSaveCnf: timer out");

    if (VOS_TRUE == NAS_MSCC_GetAbortFlag_SysAcq())
    {
        NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_ABORTED);

        return VOS_TRUE;
    }

    NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_FAILURE);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMmcAcqCnf_SysAcq_WaitInitlocInfoInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_ACQ_CNF_STRU              *pstAcqCnf;

    pstAcqCnf = (MMC_MSCC_ACQ_CNF_STRU *)pstMsg;

    if (NAS_MSCC_PIF_ACQ_RESULT_SUCCESS == pstAcqCnf->enAcqRslt)
    {
        NAS_MSCC_ProcLteInitSysAcqSucc_SysAcq(pstAcqCnf);
    }
    else
    {
        NAS_MSCC_ProcLteInitSysAcqFail_SysAcq();
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMsccAbortFsmReq_SysAcq_WaitInitlocInfoInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_INIT_LOC_INFO_IND);

    NAS_MSCC_SetAbortFlag_SysAcq(VOS_TRUE);

    NAS_MSCC_SndMmcPowerSaveReq_SysAcq();

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvXsd1xSystemServiceInfoInd_SysAcq_WaitInitlocInfoInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    XSD_MSCC_1X_SYSTEM_SERVICE_INFO_IND_STRU               *pstSysInfoInd;

    pstSysInfoInd = (XSD_MSCC_1X_SYSTEM_SERVICE_INFO_IND_STRU *)pstMsg;

    if ((NAS_MSCC_PIF_SERVICE_STATUS_NORMAL_SERVICE  == pstSysInfoInd->stSysSrvInfo.enServiceStatus)
     || (NAS_MSCC_PIF_SERVICE_STATUS_LIMITED_SERVICE == pstSysInfoInd->stSysSrvInfo.enServiceStatus))
    {
        NAS_MSCC_Proc1xInitSysAcqSucc_SysAcq(pstSysInfoInd);
    }
    else
    {
        NAS_MSCC_Proc1xInitSysAcqFail_SysAcq();
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvTiInitLocInfoIndExpired_SysAcq_WaitInitlocInfoInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT8                           uc1xInitAcqFinishFlg;
    VOS_UINT8                           ucLteInitAcqFinishFlg;

    NAS_WARNING_LOG(UEPS_PID_MSCC, "NAS_MSCC_RcvTiInitLocInfoIndExpired_SysAcq_WaitInitlocInfoInd: timer out");

    uc1xInitAcqFinishFlg  = NAS_MSCC_Get1xInitialAcqFinishFlg_SysAcq();
    ucLteInitAcqFinishFlg = NAS_MSCC_GetLteInitialAcqFinishFlg_SysAcq();

    /* both 1x and lte initial acquire are not finished,we consider lte inital acquire failed
       and search hrpd first */
    if ((VOS_FALSE == uc1xInitAcqFinishFlg)
     && (VOS_FALSE == ucLteInitAcqFinishFlg))
    {
        NAS_MSCC_ProcLteInitSysAcqFail_SysAcq();

        return VOS_TRUE;
    }

    /* if 1x initial acquire not finished,we consider 1x inital acquire failed */
    if (VOS_FALSE == uc1xInitAcqFinishFlg)
    {
        NAS_MSCC_Proc1xInitSysAcqFail_SysAcq();

        return VOS_TRUE;
    }

    /* if lte initial acquire not finished,we consider lte inital acquire failed */
    if (VOS_FALSE == ucLteInitAcqFinishFlg)
    {
        NAS_MSCC_ProcLteInitSysAcqFail_SysAcq();
    }

    return VOS_TRUE;
}


VOS_VOID NAS_MSCC_QuitFsmSysAcq(
    NAS_MSCC_SYSTEM_ACQUIRED_RESULT_ENUM_UINT32             enRslt
)
{
    VOS_UINT8                                               ucSearchLteFlag  = VOS_FALSE;
    VOS_UINT8                                               ucSearchHrpdFlag = VOS_FALSE;

    VOS_UINT8                                               ucLteNoRfFlag    = VOS_FALSE;
    VOS_UINT8                                               ucHrpdNoRfFlag   = VOS_FALSE;

    ucSearchLteFlag  = NAS_MSCC_GetInterSysSearchLteFlag_SysAcq();
    ucSearchHrpdFlag = NAS_MSCC_GetInterSysSearchHrpdFlag_SysAcq();

    ucLteNoRfFlag    = NAS_MSCC_GetInterSysSearchLteNoRfFlag_SysAcq();
    ucHrpdNoRfFlag   = NAS_MSCC_GetInterSysSearchHrpdNoRfFlag_SysAcq();

    /* send the result to Main FSM */
    NAS_MSCC_SndInternalSysAcqRsltCnf(enRslt, ucSearchLteFlag, ucSearchHrpdFlag, ucLteNoRfFlag, ucHrpdNoRfFlag);

    /* Quit the SubFSM */
    NAS_MSCC_QuitFsmL2();
}



NAS_MSCC_SYS_ACQ_SCENE_ENUM_UINT32 NAS_MSCC_GetCLSysAcqScene_SysAcq(VOS_VOID)
{
    NAS_MSCC_SYS_ACQ_SCENE_ENUM_UINT32                      enCurrSysAcqScene;
    NAS_MSCC_MSG_STRU                                      *pstEntryMsg;
    NAS_MSCC_MSCC_SYS_ACQ_STRU                             *pstSysAcq;

    pstEntryMsg = NAS_MSCC_GetCurrFsmEntryMsgAddr();

    if (NAS_BuildEventType(UEPS_PID_MSCC, ID_NAS_MSCC_MSCC_SYS_ACQ_REQ)== pstEntryMsg->ulEventType)
    {
        pstSysAcq         = (NAS_MSCC_MSCC_SYS_ACQ_STRU *)pstEntryMsg->aucMsgBuffer;
        enCurrSysAcqScene = pstSysAcq->enSysAcqScene;
    }
    else if (NAS_BuildEventType(WUEPS_PID_MMA, ID_MMA_MSCC_CDMA_MO_CALL_REDIAL_SYS_ACQ_NTF)== pstEntryMsg->ulEventType)
    {
        enCurrSysAcqScene = NAS_MSCC_SYS_ACQ_SCENE_QUICK_SEARCH_HRPD_MRUO;
    }
    else
    {
        enCurrSysAcqScene = NAS_MSCC_SYS_ACQ_SCENE_BUTT;
    }

    return enCurrSysAcqScene;
}



NAS_MSCC_SYS_ACQ_SYS_LIST_INFO_STRU* NAS_MSCC_GetCLSysAcqSysList_SysAcq(VOS_VOID)
{
    NAS_MSCC_FSM_SYS_ACQ_CTX_STRU                          *pstSysAcqFsmCtx;

    pstSysAcqFsmCtx     = NAS_MSCC_GetSysAcqFsmCtxAddr();

    return &(pstSysAcqFsmCtx->stSysAcqListInfo);
}



VOS_UINT32 NAS_MSCC_IsNeedReAcqLte_SysAcq(VOS_VOID)
{
    NAS_MSCC_FSM_SYS_ACQ_CTX_STRU                          *pstSysAcqFsmCtx;
    VOS_UINT8                                               ucNeedReAcqLteFlg;
    NAS_MSCC_SYS_ACQ_SYS_LIST_INFO_STRU                    *pstSysListInfo;
    NAS_MSCC_SYS_ACQ_SCENE_ENUM_UINT32                      enCurrSysAcqScene;

    enCurrSysAcqScene   = NAS_MSCC_GetCLSysAcqScene_SysAcq();

    /* 定制需求:开机搜网时，当LTE优先于HPRD并且NV2014中的ucReAcqLteOnHrpdSyncIndFlag为TRUE时，
        如果第一次搜索LTE失败，接下来在搜索HRPD时如果HPRD有覆盖，则先power save HRPD，再给LTE搜索一次的机会*/
    if (NAS_MSCC_SYS_ACQ_SCENE_SWITCH_ON == enCurrSysAcqScene)
    {
        pstSysListInfo      = NAS_MSCC_GetCLSysAcqSysList_SysAcq();
        pstSysAcqFsmCtx     = NAS_MSCC_GetSysAcqFsmCtxAddr();
        ucNeedReAcqLteFlg   = pstSysAcqFsmCtx->ucNeedReAcqLteFlg;

        if ((VOS_RATMODE_LTE == pstSysListInfo->astNormalAcqSysList[0].ulRatMode)
         && (VOS_TRUE == ucNeedReAcqLteFlg))
        {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;

}



VOS_VOID NAS_MSCC_SndMmcPowerSaveReq_SysAcq(VOS_VOID)
{
    /*power save LTE*/
    NAS_MSCC_SndMmcPowerSaveReq();

    NAS_MSCC_SetCurrFsmState(NAS_MSCC_SYSACQ_STA_WAIT_MMC_POWER_SAVE_CNF);

    NAS_MSCC_StartTimer(TI_NAS_MSCC_WAIT_MMC_POWER_SAVE_CNF,
                        TI_NAS_MSCC_WAIT_MMC_POWER_SAVE_CNF_LEN);

}



VOS_VOID NAS_MSCC_SndHsdPowerSaveReq_SysAcq(VOS_VOID)
{
    /*power save hrpd*/
    NAS_MSCC_SndHsdPowerSaveReq();

    NAS_MSCC_SetCurrFsmState(NAS_MSCC_SYSACQ_STA_WAIT_HSD_POWER_SAVE_CNF);

    NAS_MSCC_StartTimer(TI_NAS_MSCC_WAIT_HSD_POWER_SAVE_CNF,
                        TI_NAS_MSCC_WAIT_HSD_POWER_SAVE_CNF_LEN);
}



VOS_VOID NAS_MSCC_SndMmcPlmnSearchReq_SysAcq(VOS_VOID)
{
    NAS_MSCC_PIF_ACQ_REASON_ENUM_UINT32                     enAcqReason;

    NAS_MSCC_PIF_INIT_ACQ_PLMN_INFO_STRU                    stInitAcqPlmnInfo;
    NAS_MSCC_PIF_INIT_ACQ_PLMN_INFO_STRU                   *pstInitAcqPlmnInfo         = VOS_NULL_PTR;
    NAS_MSCC_NO_CARD_INIT_SEARCH_LOC_STRU                  *pstNoCardInitSearchLocInfo = VOS_NULL_PTR;

    PS_MEM_SET(&stInitAcqPlmnInfo, 0x00, sizeof(NAS_MSCC_PIF_INIT_ACQ_PLMN_INFO_STRU));

    if (NAS_MSCC_SYS_ACQ_SCENE_SWITCH_ON == NAS_MSCC_GetCLSysAcqScene_SysAcq())
    {
        enAcqReason = NAS_MSCC_PIF_ACQ_REASON_POWER_UP;

        pstNoCardInitSearchLocInfo = NAS_MSCC_GetNoCardInitSearchLocInfoAddr();

        stInitAcqPlmnInfo.enRat                 = NAS_MSCC_PIF_NET_RAT_TYPE_LTE;

        if (NAS_MSCC_INVALID_MCC != pstNoCardInitSearchLocInfo->stInitSearchGulLoc.ulMcc)
        {
            stInitAcqPlmnInfo.stInitAcqPlmnId.ulMcc = pstNoCardInitSearchLocInfo->stInitSearchGulLoc.ulMcc;
            stInitAcqPlmnInfo.stInitAcqPlmnId.ulMnc = pstNoCardInitSearchLocInfo->stInitSearchGulLoc.ulMnc;
        }
        else
        {
            stInitAcqPlmnInfo.stInitAcqPlmnId.ulMcc = NAS_MSCC_INVALID_MCC;
            stInitAcqPlmnInfo.stInitAcqPlmnId.ulMnc = NAS_MSCC_INVALID_MNC;
        }

        pstInitAcqPlmnInfo = &stInitAcqPlmnInfo;
    }
    else
    {
        enAcqReason = NAS_MSCC_PIF_ACQ_REASON_OTHER;
    }

    NAS_MSCC_SndMmcPlmnSearchReq(enAcqReason,
                                 NAS_MSCC_GetSysAcqLteSrchType_SysAcq(),
                                 pstInitAcqPlmnInfo,
                                 VOS_NULL_PTR);

    NAS_MSCC_Set3gppCampOnFlag(VOS_FALSE);

    NAS_MSCC_Set3gppActiveFlg(VOS_TRUE);

    NAS_MSCC_SetCurrFsmState(NAS_MSCC_SYSACQ_STA_WAIT_MMC_PLMN_SEARCH_CNF);

    NAS_MSCC_StartTimer(TI_NAS_MSCC_WAIT_MMC_PLMN_SEARCH_CNF,
                        TI_NAS_MSCC_WAIT_MMC_PLMN_SEARCH_CNF_LEN);

}



VOS_VOID NAS_MSCC_SndHsdSysAcqReq_SysAcq(VOS_VOID)
{
    MSCC_HSD_ACQUIRE_REASON_ENUM_UINT32 enAcqReason;

    if (NAS_MSCC_SYS_ACQ_SCENE_SWITCH_ON == NAS_MSCC_GetCLSysAcqScene_SysAcq())
    {
        enAcqReason = MSCC_HSD_ACQUIRE_REASON_POWER_UP;
    }
    else
    {
        enAcqReason = MSCC_HSD_ACQUIRE_REASON_OTHER;
    }

    /*acquire Hrpd*/
    NAS_MSCC_SndHsdSysAcqReq(enAcqReason,
                             0,
                             VOS_NULL_PTR);

    /* Set the Camp On flag to FALSE */
    NAS_MSCC_SetHrpdCampOnFlag(VOS_FALSE);

    NAS_MSCC_SetHrpdActiveFlg(VOS_TRUE);

    NAS_MSCC_SetCurrFsmState(NAS_MSCC_SYSACQ_STA_WAIT_HSD_ACQUIRE_CNF);

    NAS_MSCC_StartTimer(TI_NAS_MSCC_WAIT_HSD_ACQUIRE_CNF,
                        TI_NAS_MSCC_WAIT_HSD_ACQUIRE_CNF_LEN);

}



VOS_VOID NAS_MSCC_SrchNetwork_SysAcq(
    VOS_RATMODE_ENUM_UINT32             ulRatMode
)
{
    VOS_UINT8                           ucHrpdActFlg;
    VOS_UINT8                           uc3gppActFlg;

    ucHrpdActFlg   = NAS_MSCC_GetHrpdActiveFlg();
    uc3gppActFlg   = NAS_MSCC_Get3gppActiveFlg();

    switch (ulRatMode)
    {
        case VOS_RATMODE_LTE :

            /* LTE如果正常驻留,此时不立即搜索LTE,成功退出状态机即可; */
            if (VOS_TRUE == NAS_MSCC_Is3gppNormalService())
            {
                NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_SUCCESS);

                return;
            }

            if (VOS_TRUE == ucHrpdActFlg)
            {
                NAS_MSCC_SndHsdPowerSaveReq_SysAcq();
            }
            else
            {
                NAS_MSCC_SndMmcPlmnSearchReq_SysAcq();
            }

            break;

        case VOS_RATMODE_HRPD :

            /* HRPD如果正常驻留,此时不立即搜索HRPD,成功退出状态机即可; */
            if (VOS_TRUE == NAS_MSCC_IsHrpdNormalService())
            {
                NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_SUCCESS);

                return;
            }

            if (VOS_TRUE == uc3gppActFlg)
            {
                NAS_MSCC_SndMmcPowerSaveReq_SysAcq();
            }
            else
            {
                NAS_MSCC_SndHsdSysAcqReq_SysAcq();
            }

            break;

        default:

            NAS_WARNING_LOG(UEPS_PID_MSCC, "NAS_MSCC_SrchNetwork_SysAcq: unknown ulRatMode!");
            NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_FAILURE);
            break;
    }

    return;
}



VOS_VOID NAS_MSCC_SrchInitLoc_SysAcq(
    VOS_RATMODE_ENUM_UINT32             ulRatMode
)
{
    MMA_MSCC_ACQ_REQ_STRU                                   stAcqReq;;

    /* LTE如果正常驻留,此时不立即搜索LTE,成功退出状态机即可; */
    if (VOS_TRUE == NAS_MSCC_Is3gppNormalService())
    {
        NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_SUCCESS);

        return;
    }

    if (VOS_RATMODE_LTE == ulRatMode)
    {
        /* 初始搜索LTE */
        stAcqReq.enAcqReason             = NAS_MSCC_PIF_ACQ_REASON_POWER_UP;
        stAcqReq.ucNotRegAfterAcqSucc    = VOS_TRUE;
        NAS_MSCC_SndMmcAcqReq(&stAcqReq);

        NAS_MSCC_Set3gppCampOnFlag(VOS_FALSE);

        NAS_MSCC_Set3gppActiveFlg(VOS_TRUE);

        NAS_MSCC_SetCurrFsmState(NAS_MSCC_SYSACQ_STA_WAIT_INIT_LOC_INFO_IND);

        NAS_MSCC_StartTimer(TI_NAS_MSCC_WAIT_INIT_LOC_INFO_IND,
                            TI_NAS_MSCC_WAIT_INIT_LOC_INFO_IND_LEN);
    }
    else
    {
        NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_FAILURE);
    }
}



VOS_UINT32 NAS_MSCC_SrchNetworkByLteInitLocInfo_SysAcq(
     NAS_MSCC_SYS_ACQ_LTE_INIT_LOC_STA_STRU                *pstLteInitLocSta
)
{
    NAS_MSCC_SYS_ACQ_SYS_LIST_INFO_STRU                    *pstSysListInfo;
    VOS_UINT32                                              ulRslt;
    NAS_MSCC_SYS_ACQ_SYS_INFO_STRU                          stNextAcqSysInfo;

    pstSysListInfo      = NAS_MSCC_GetCLSysAcqSysList_SysAcq();

    NAS_MSCC_BuildSysAcqList(NAS_MSCC_BUILD_SYS_ACQ_LIST_SCENE_ENUM_SWITCH_ON_INIT_ACQ_SUCC,
                             VOS_NULL_PTR,
                             &pstLteInitLocSta->stLteLocInfo,
                             pstSysListInfo);

    /* send next acquire rat mode for normal acquire req */
    ulRslt = NAS_MSCC_GetNextNormalAcquireSysInAcqSysList(pstSysListInfo,
                                                         &stNextAcqSysInfo);

    if (VOS_TRUE ==  ulRslt)
    {
        if (VOS_RATMODE_HRPD == stNextAcqSysInfo.ulRatMode)
        {
            NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_INIT_LOC_INFO_IND);

            NAS_MSCC_SetLteInitialAcqLocInfoStatus_SysAcq(NAS_MSCC_INIT_ACQ_LOC_INFO_STATUS_SEARCH_MLPL_HRPD_PRIO);

            /* Plmn search cnf failed, so power save the lte */
            NAS_MSCC_SndMmcPowerSaveReq_SysAcq();
            return VOS_TRUE;
        }

        if (VOS_RATMODE_LTE == stNextAcqSysInfo.ulRatMode)
        {
            NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_INIT_LOC_INFO_IND);

            NAS_MSCC_SetLteInitialAcqLocInfoStatus_SysAcq(NAS_MSCC_INIT_ACQ_LOC_INFO_STATUS_SEARCH_MLPL_LTE_PRIO);

            NAS_MSCC_SndMmcPlmnSearchReq_SysAcq();

            return VOS_TRUE;
        }
    }

    NAS_MSCC_SetLteInitialAcqLocInfoStatus_SysAcq(NAS_MSCC_INIT_ACQ_LOC_INFO_STATUS_SEARCH_MLPL_FAIL);

    return VOS_FALSE;
}



VOS_UINT32 NAS_MSCC_SrchNetworkBy1xInitLocInfo_SysAcq(
    NAS_MSCC_SYS_ACQ_1X_INIT_LOC_STA_STRU                  *pst1xInitLocSta
)
{
    NAS_MSCC_SYS_ACQ_SYS_LIST_INFO_STRU                    *pstSysListInfo;
    VOS_UINT32                                              ulRslt;
    NAS_MSCC_SYS_ACQ_SYS_INFO_STRU                          stNextAcqSysInfo;

    pstSysListInfo = NAS_MSCC_GetCLSysAcqSysList_SysAcq();

    NAS_MSCC_BuildSysAcqList(NAS_MSCC_BUILD_SYS_ACQ_LIST_SCENE_ENUM_SWITCH_ON_INIT_ACQ_SUCC,
                             &pst1xInitLocSta->st1xLocInfo,
                             VOS_NULL_PTR,
                             pstSysListInfo);

    /* send next acquire rat mode for normal aquire req */
    ulRslt = NAS_MSCC_GetNextNormalAcquireSysInAcqSysList(pstSysListInfo,
                                                         &stNextAcqSysInfo);

    if (VOS_TRUE == ulRslt)
    {
        if (VOS_RATMODE_HRPD == stNextAcqSysInfo.ulRatMode)
        {
            NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_INIT_LOC_INFO_IND);

            NAS_MSCC_Set1xInitialAcqLocInfoStatus_SysAcq(NAS_MSCC_INIT_ACQ_LOC_INFO_STATUS_SEARCH_MLPL_HRPD_PRIO);

            /* Plmn search cnf failed, so power save the lte */
            NAS_MSCC_SndMmcPowerSaveReq_SysAcq();

            return VOS_TRUE;
        }

        if (VOS_RATMODE_LTE == stNextAcqSysInfo.ulRatMode)
        {
            NAS_MSCC_Set1xInitialAcqLocInfoStatus_SysAcq(NAS_MSCC_INIT_ACQ_LOC_INFO_STATUS_SEARCH_MLPL_LTE_PRIO);

            /* use 1x initial acq location info to determine lte is more prefered,so here
               if lte system acqurie is not finsished,we should first wait lte initial acquire finish */
            if (VOS_TRUE == NAS_MSCC_GetLteInitialAcqFinishFlg_SysAcq())
            {
                NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_INIT_LOC_INFO_IND);

                /* lte initial acquire locaiton info is invalid we srch hrpd,else we srch lte */
                if (NAS_MSCC_INIT_ACQ_LOC_INFO_STATUS_INVALID == NAS_MSCC_GetLteInitialAcqLocInfoStatus_SysAcq())
                {
                    /* use default system acquire list */
                    return NAS_MSCC_SrchNetworkByNonInitLocInfo_SysAcq();
                }
                else
                {
                    NAS_MSCC_SndMmcPlmnSearchReq_SysAcq();
                }
            }

            return VOS_TRUE;
        }
    }

    NAS_MSCC_Set1xInitialAcqLocInfoStatus_SysAcq(NAS_MSCC_INIT_ACQ_LOC_INFO_STATUS_SEARCH_MLPL_FAIL);

    return VOS_FALSE;
}


VOS_UINT32 NAS_MSCC_SrchNetworkByNonInitLocInfo_SysAcq(VOS_VOID)
{
    NAS_MSCC_SYS_ACQ_SYS_LIST_INFO_STRU                    *pstSysListInfo;
    VOS_UINT32                                              ulRslt;
    NAS_MSCC_SYS_ACQ_SYS_INFO_STRU                          stNextAcqSysInfo;

    pstSysListInfo      = NAS_MSCC_GetCLSysAcqSysList_SysAcq();

    NAS_MSCC_BuildSysAcqList(NAS_MSCC_BUILD_SYS_ACQ_LIST_SCENE_ENUM_SWITCH_ON_INIT_ACQ_FAIL,
                             VOS_NULL_PTR,
                             VOS_NULL_PTR,
                             pstSysListInfo);

    /* send next acquire rat mode for normal aquire req */
    ulRslt = NAS_MSCC_GetNextNormalAcquireSysInAcqSysList(pstSysListInfo,
                                                         &stNextAcqSysInfo);

    if (VOS_TRUE == ulRslt)
    {
        if (VOS_RATMODE_HRPD == stNextAcqSysInfo.ulRatMode)
        {
            /* Plmn search cnf failed, so power save the lte */
            NAS_MSCC_SndMmcPowerSaveReq_SysAcq();

            return VOS_TRUE;
        }

        if (VOS_RATMODE_LTE == stNextAcqSysInfo.ulRatMode)
        {
            NAS_MSCC_SndMmcPlmnSearchReq_SysAcq();

            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}




VOS_VOID NAS_MSCC_Proc1xInitSysAcqSucc_SysAcq(
    XSD_MSCC_1X_SYSTEM_SERVICE_INFO_IND_STRU               *pst1xSysInfoInd
)
{
    NAS_MSCC_FSM_SYS_ACQ_CTX_STRU      *pstSysAcqFsmCtx;
    VOS_UINT32                          ulRslt;

    pstSysAcqFsmCtx = NAS_MSCC_GetSysAcqFsmCtxAddr();

    pstSysAcqFsmCtx->st1xInitLocSta.ucAcqFinishFlg    = VOS_TRUE;

    NAS_MSCC_Set1xInitialAcqLocInfoStatus_SysAcq(NAS_MSCC_INIT_ACQ_LOC_INFO_STATUS_NO_SEARCH_MLPL);

    /* 判定1x位置有效才进行MCC的判定 */
    if (PS_TRUE == pst1xSysInfoInd->stSysSrvInfo.enServiceInfoIncl)
    {
        pstSysAcqFsmCtx->st1xInitLocSta.st1xLocInfo.ulMcc = pst1xSysInfoInd->stSysSrvInfo.stServiceInfo.ulMcc;
        pstSysAcqFsmCtx->st1xInitLocSta.st1xLocInfo.ulMnc = pst1xSysInfoInd->stSysSrvInfo.stServiceInfo.usMnc;
    }
    else
    {
        NAS_WARNING_LOG(UEPS_PID_MSCC, "NAS_MSCC_Proc1xInitSysAcqSucc_SysAcq:service info is invalid,so use last camped on info!");

        pstSysAcqFsmCtx->st1xInitLocSta.st1xLocInfo.ulMcc = NAS_MSCC_Get1xSysInfoAddr()->ulMcc;
        pstSysAcqFsmCtx->st1xInitLocSta.st1xLocInfo.ulMnc = NAS_MSCC_Get1xSysInfoAddr()->ulMnc;
    }
    pstSysAcqFsmCtx->st1xInitLocSta.st1xLocInfo.usSid = pst1xSysInfoInd->stSysSrvInfo.usSid;
    pstSysAcqFsmCtx->st1xInitLocSta.st1xLocInfo.usNid = pst1xSysInfoInd->stSysSrvInfo.usNid;

    pstSysAcqFsmCtx->st1xInitLocSta.st1xLocInfo.en1xPrioClass =
                    NAS_MSCC_ConvertMmcSysPriClassToMsccFormat(pst1xSysInfoInd->stSysSrvInfo.en1xPriClass);

    pstSysAcqFsmCtx->st1xInitLocSta.st1xLocInfo.enAIPrioClass=
                    NAS_MSCC_ConvertMmcSysPriClassToMsccFormat(pst1xSysInfoInd->stSysSrvInfo.enAIPriClass);
    /* judge if we can use 1x initial loc info,if not,return */
    if (VOS_FALSE == NAS_MSCC_IsNeedUse1xInitAcqLocInfo_SysAcq())
    {
        NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_Proc1xInitSysAcqSucc_SysAcq: could not use 1x init acq loc info and return!");

        return;
    }

    ulRslt = NAS_MSCC_SrchNetworkBy1xInitLocInfo_SysAcq(&pstSysAcqFsmCtx->st1xInitLocSta);

    if (VOS_TRUE == ulRslt)
    {
        NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_Proc1xInitSysAcqSucc_SysAcq: use 1x init acq loc info srch network!");

        return;
    }

    /* if lte initial location info is not ready,wait */
    if (VOS_FALSE == NAS_MSCC_GetLteInitialAcqFinishFlg_SysAcq())
    {
        NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_Proc1xInitSysAcqSucc_SysAcq: lte init acq not finished!");

        return;
    }

    /* judge if we can use lte initial loc info */
    if (VOS_TRUE == NAS_MSCC_IsNeedUseLteInitAcqLocInfo_SysAcq())
    {
        ulRslt = NAS_MSCC_SrchNetworkByLteInitLocInfo_SysAcq(&pstSysAcqFsmCtx->stLteInitLocSta);

        if (VOS_TRUE == ulRslt)
        {
            NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_Proc1xInitSysAcqSucc_SysAcq: use lte init acq loc info srch network!");

            return;
        }
    }

    NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_Proc1xInitSysAcqSucc_SysAcq: use non-init loc info srch network!");

    /* use default search list */
    ulRslt = NAS_MSCC_SrchNetworkByNonInitLocInfo_SysAcq();

    NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_INIT_LOC_INFO_IND);

    if (VOS_FALSE == ulRslt)
    {
       NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_FAILURE);
    }

    return;
}




VOS_VOID NAS_MSCC_Proc1xInitSysAcqFail_SysAcq(VOS_VOID)
{
    NAS_MSCC_FSM_SYS_ACQ_CTX_STRU      *pstSysAcqFsmCtx;
    VOS_UINT32                          ulRslt;

    pstSysAcqFsmCtx     = NAS_MSCC_GetSysAcqFsmCtxAddr();

    pstSysAcqFsmCtx->st1xInitLocSta.ucAcqFinishFlg = VOS_TRUE;

    NAS_MSCC_Set1xInitialAcqLocInfoStatus_SysAcq(NAS_MSCC_INIT_ACQ_LOC_INFO_STATUS_INVALID);

    /* if lte initial location info is not ready,wait */
    if (VOS_FALSE == NAS_MSCC_GetLteInitialAcqFinishFlg_SysAcq())
    {
        NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_Proc1xInitSysAcqFail_SysAcq: lte init acq not finished!");

        return;
    }

    /* judge if we can use lte initial loc info */
    if (VOS_TRUE == NAS_MSCC_IsNeedUseLteInitAcqLocInfo_SysAcq())
    {
        ulRslt = NAS_MSCC_SrchNetworkByLteInitLocInfo_SysAcq(&pstSysAcqFsmCtx->stLteInitLocSta);

        if (VOS_TRUE == ulRslt)
        {
            NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_Proc1xInitSysAcqFail_SysAcq: use lte init acq loc info srch network!");

            return;
        }
    }

    NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_Proc1xInitSysAcqFail_SysAcq: use non-init acq loc info srch network!");

    /* use default search list */
    ulRslt = NAS_MSCC_SrchNetworkByNonInitLocInfo_SysAcq();

    NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_INIT_LOC_INFO_IND);

    if (VOS_FALSE == ulRslt)
    {
        NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_FAILURE);
    }

    return;
}





VOS_VOID NAS_MSCC_ProcLteInitSysAcqSucc_SysAcq(
    MMC_MSCC_ACQ_CNF_STRU              *pstAcqCnf
)
{
    NAS_MSCC_FSM_SYS_ACQ_CTX_STRU      *pstSysAcqFsmCtx;
    VOS_UINT32                          ulRslt;
    NAS_MSCC_3GPP_SYS_INFO_STRU        *pst3gppSysInfo = VOS_NULL_PTR;

    pst3gppSysInfo = NAS_MSCC_Get3gppSysInfoAddr();

    pst3gppSysInfo->enPrioClass = NAS_MSCC_ConvertMmcSysPriClassToMsccFormat(pstAcqCnf->enPrioClass);

    pstSysAcqFsmCtx = NAS_MSCC_GetSysAcqFsmCtxAddr();

    pstSysAcqFsmCtx->stLteInitLocSta.ucAcqFinishFlg = VOS_TRUE;

    NAS_MSCC_SetLteInitialAcqLocInfoStatus_SysAcq(NAS_MSCC_INIT_ACQ_LOC_INFO_STATUS_NO_SEARCH_MLPL);

    pstSysAcqFsmCtx->stLteInitLocSta.stLteLocInfo.ulMcc = pstAcqCnf->stPlmnId.ulMcc;
    pstSysAcqFsmCtx->stLteInitLocSta.stLteLocInfo.ulMnc = pstAcqCnf->stPlmnId.ulMnc;
    pstSysAcqFsmCtx->stLteInitLocSta.stLteLocInfo.enPrioClass = pst3gppSysInfo->enPrioClass;

    /* here first judge 1x location info is used and is already lte prefered,
       then we can directly srch lte */
    if (NAS_MSCC_INIT_ACQ_LOC_INFO_STATUS_SEARCH_MLPL_LTE_PRIO == NAS_MSCC_Get1xInitialAcqLocInfoStatus_SysAcq())
    {
        NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_ProcLteInitSysAcqSucc_SysAcq: 1x init acq loc info status is search mlpl lte prio and search lte!");

        NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_INIT_LOC_INFO_IND);

        NAS_MSCC_SndMmcPlmnSearchReq_SysAcq();

        return;
    }

    /* judge if we can use lte initial acquire location info or not,if not,return */
    if (VOS_FALSE == NAS_MSCC_IsNeedUseLteInitAcqLocInfo_SysAcq())
    {
        NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_ProcLteInitSysAcqSucc_SysAcq: could not use lte init acq loc info and return!");

        return;
    }

    ulRslt = NAS_MSCC_SrchNetworkByLteInitLocInfo_SysAcq(&pstSysAcqFsmCtx->stLteInitLocSta);

    if (VOS_TRUE == ulRslt)
    {
        NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_ProcLteInitSysAcqSucc_SysAcq: use lte init loc info srch network!");

        return;
    }

    /* if 1x initial location info is not ready,wait */
    if (VOS_TRUE == NAS_MSCC_IsSpecRatSupported(NAS_MSCC_PIF_NET_RAT_TYPE_1X))
    {
        if (VOS_FALSE == NAS_MSCC_Get1xInitialAcqFinishFlg_SysAcq())
        {
            NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_ProcLteInitSysAcqSucc_SysAcq: 1x supported and 1x init acq not finished!");

            return;
        }

        /* judge if we can use 1x initial acquire location info or not */
        if (VOS_TRUE == NAS_MSCC_IsNeedUse1xInitAcqLocInfo_SysAcq())
        {
            ulRslt = NAS_MSCC_SrchNetworkBy1xInitLocInfo_SysAcq(&pstSysAcqFsmCtx->st1xInitLocSta);

            if (VOS_TRUE == ulRslt)
            {
                NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_ProcLteInitSysAcqSucc_SysAcq: 1x supported and use 1x init acq loc info srch network!");

                return;
            }
        }
    }

    NAS_NORMAL_LOG(UEPS_PID_MSCC, "NAS_MSCC_ProcLteInitSysAcqSucc_SysAcq: use non-init loc info srch network!");

    /* use default search list */
    ulRslt = NAS_MSCC_SrchNetworkByNonInitLocInfo_SysAcq();

    NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_INIT_LOC_INFO_IND);

    if (VOS_FALSE == ulRslt)
    {
        NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_FAILURE);
    }

    return;
}




VOS_VOID NAS_MSCC_ProcLteInitSysAcqFail_SysAcq(VOS_VOID)
{
    NAS_MSCC_FSM_SYS_ACQ_CTX_STRU      *pstSysAcqFsmCtx;
    VOS_UINT32                          ulRslt;

    pstSysAcqFsmCtx = NAS_MSCC_GetSysAcqFsmCtxAddr();

    pstSysAcqFsmCtx->stLteInitLocSta.ucAcqFinishFlg   = VOS_TRUE;

    NAS_MSCC_SetLteInitialAcqLocInfoStatus_SysAcq(NAS_MSCC_INIT_ACQ_LOC_INFO_STATUS_INVALID);

    /* lte initial acquire failed and we can directly power save lte and search hrpd */
    ulRslt = NAS_MSCC_SrchNetworkByNonInitLocInfo_SysAcq();

    NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_INIT_LOC_INFO_IND);

    if (VOS_FALSE == ulRslt)
    {
        NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_FAILURE);
    }

    return;
}



VOS_UINT32 NAS_MSCC_IsNeedUse1xInitAcqLocInfo_SysAcq(VOS_VOID)
{
    VOS_UINT8                                               ucIs1xLocInfoPrefThanLte;
    NAS_MSCC_INIT_ACQ_LOC_INFO_STATUS_ENUM_UINT8            enLteInitLocInfoStatus;

    ucIs1xLocInfoPrefThanLte = NAS_MSCC_GetMmssNvimCfgInfoAddr()->stMmssSysAcqCfg.ucIs1xLocInfoPrefThanLte;

    /* 1x preferred and 1x location info is not used and valid */
    if ((NAS_MSCC_INIT_ACQ_LOC_INFO_STATUS_NO_SEARCH_MLPL == NAS_MSCC_Get1xInitialAcqLocInfoStatus_SysAcq())
     && (VOS_TRUE                                         == ucIs1xLocInfoPrefThanLte))
    {
        return VOS_TRUE;
    }

    /* lte preferred and lte location info is invalid or valid but search mlpl failed */
    enLteInitLocInfoStatus = NAS_MSCC_GetLteInitialAcqLocInfoStatus_SysAcq();

    if ((VOS_FALSE                                           == ucIs1xLocInfoPrefThanLte)
     && ((NAS_MSCC_INIT_ACQ_LOC_INFO_STATUS_INVALID          == enLteInitLocInfoStatus)
      || (NAS_MSCC_INIT_ACQ_LOC_INFO_STATUS_SEARCH_MLPL_FAIL == enLteInitLocInfoStatus)))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}





VOS_UINT32 NAS_MSCC_IsNeedUseLteInitAcqLocInfo_SysAcq(VOS_VOID)
{
    VOS_UINT8                                               ucIs1xLocInfoPrefThanLte;
    NAS_MSCC_INIT_ACQ_LOC_INFO_STATUS_ENUM_UINT8            en1xInitLocInfoStatus;

    ucIs1xLocInfoPrefThanLte = NAS_MSCC_GetMmssNvimCfgInfoAddr()->stMmssSysAcqCfg.ucIs1xLocInfoPrefThanLte;

    /* first judge 1x support or not, if not support,directly use lte loc info */
    if (VOS_FALSE == NAS_MSCC_IsSpecRatSupported(NAS_MSCC_PIF_NET_RAT_TYPE_1X))
    {
        return VOS_TRUE;
    }

    /* Lte preferred and Lte location info is not used and valid */
    if ((NAS_MSCC_INIT_ACQ_LOC_INFO_STATUS_NO_SEARCH_MLPL == NAS_MSCC_GetLteInitialAcqLocInfoStatus_SysAcq())
     && (VOS_FALSE                                        == ucIs1xLocInfoPrefThanLte))
    {
        return VOS_TRUE;
    }

    /* 1x preferred and 1x location info is invalid or valid but search mlpl failed */
    en1xInitLocInfoStatus = NAS_MSCC_Get1xInitialAcqLocInfoStatus_SysAcq();

    if ((VOS_TRUE                                            == ucIs1xLocInfoPrefThanLte)
     && ((NAS_MSCC_INIT_ACQ_LOC_INFO_STATUS_INVALID          == en1xInitLocInfoStatus)
      || (NAS_MSCC_INIT_ACQ_LOC_INFO_STATUS_SEARCH_MLPL_FAIL == en1xInitLocInfoStatus)))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}




NAS_MSCC_PIF_PLMN_SEARCH_TYPE_ENUM_UINT32 NAS_MSCC_GetSysAcqLteSrchType_SysAcq(VOS_VOID)
{
    NAS_MSCC_MSG_STRU                  *pstEntryMsg  = VOS_NULL_PTR;
    NAS_MSCC_MSCC_SYS_ACQ_STRU         *pstSysAcqReq = VOS_NULL_PTR;
    NAS_MSCC_SYS_ACQ_STATUS_ENUM_UINT8  enAcqStatus;

    /* Get the Current entry message address from the FSM context */
    pstEntryMsg                 = NAS_MSCC_GetCurrFsmEntryMsgAddr();

    if (pstEntryMsg->ulEventType == NAS_BuildEventType(UEPS_PID_MSCC, ID_NAS_MSCC_MSCC_SYS_ACQ_REQ))
    {
        pstSysAcqReq = (NAS_MSCC_MSCC_SYS_ACQ_STRU *)pstEntryMsg->aucMsgBuffer;

        enAcqStatus = NAS_MSCC_GetCLSysAcqSysList_SysAcq()->astNormalAcqSysList[0].enAcqStatus;

        /* 如果是LTE历史搜+LTE掉网搜场景，搜索列表首次按历史搜，后续按全频搜 */
        if ((NAS_MSCC_SYS_ACQ_SCENE_LTE_HISTORY_PLUS_LTE_LOST == pstSysAcqReq->enSysAcqScene)
         && (NAS_MSCC_SYS_ACQ_STATUS_NO_SEARCHED              != enAcqStatus))
        {
            return NAS_MSCC_PIF_PLMN_SEARCH_SPEC;
        }

        return pstSysAcqReq->enLteSrchType;
    }

    return NAS_MSCC_PIF_PLMN_SEARCH_SPEC;
}



VOS_VOID NAS_MSCC_UpdateClInterSysSearchFlagWithSysAcqScence(
    NAS_MSCC_BUILD_SYS_ACQ_LIST_SCENE_ENUM_UINT32           enBuildSysAcqListScene
)
{
    if (NAS_MSCC_BUILD_SYS_ACQ_LIST_SCENE_ENUM_LTE_NOT_AVAILABLE == enBuildSysAcqListScene)
    {
        NAS_MSCC_SetInterSysSearchLteFlag_SysAcq(VOS_TRUE);
    }
    else if (NAS_MSCC_BUILD_SYS_ACQ_LIST_SCENE_ENUM_HRPD_LOST == enBuildSysAcqListScene)
    {
        NAS_MSCC_SetInterSysSearchHrpdFlag_SysAcq(VOS_TRUE);
    }
    else
    {
        ;
    }

    return;
}


VOS_UINT32 NAS_MSCC_RcvMmcRfAvailableInd_SysAcq_WaitMmcRfAvailableInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    MMC_MSCC_RF_AVAILABLE_IND_STRU                         *pstRfAvailInd;
    NAS_MSCC_SYS_ACQ_SYS_LIST_INFO_STRU                    *pstSysListInfo;
    VOS_UINT32                                              ulRslt;
    NAS_MSCC_SYS_ACQ_SYS_INFO_STRU                          stNextAcqSysInfo;

    pstRfAvailInd = (MMC_MSCC_RF_AVAILABLE_IND_STRU *)pstMsg;

    if (VOS_TRUE == pstRfAvailInd->ucRfAvailFlg)
    {
        NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_MMC_RF_AVAILABLE_IND);

        pstSysListInfo      = NAS_MSCC_GetCLSysAcqSysList_SysAcq();

        /* send next acquire rat mode for normal aquire req */
        ulRslt = NAS_MSCC_GetNextNormalAcquireSysInAcqSysList(pstSysListInfo,
                                                             &stNextAcqSysInfo);

        /* 获取不到下个搜索的模式或者获取失败直接退出状态机 */
        if ((VOS_FALSE       == ulRslt)
         || (VOS_RATMODE_LTE != stNextAcqSysInfo.ulRatMode))
        {
            NAS_WARNING_LOG(UEPS_PID_MSCC, "NAS_MSCC_RcvMmcRfAvailableInd_SysAcq_WaitMmcRfAvailableInd: get next normal acquire sys failure!");

            NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_FAILURE);

            return VOS_TRUE;
        }

        NAS_MSCC_SetReAcqLteUpOnNoRfFinishFlg_SysAcq(VOS_TRUE);

        NAS_MSCC_SndMmcPlmnSearchReq_SysAcq();
    }

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvMsccAbortFsmReq_SysAcq_WaitMmcRfAvailableInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_StopTimer(TI_NAS_MSCC_WAIT_MMC_RF_AVAILABLE_IND);

    NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_ABORTED);

    return VOS_TRUE;
}


VOS_UINT32 NAS_MSCC_RcvTiWaitMmcRfAvailableIndExpired_SysAcq_WaitMmcRfAvailableInd(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
)
{
    NAS_MSCC_SYS_ACQ_SYS_LIST_INFO_STRU                    *pstSysListInfo;
    VOS_UINT32                                              ulRslt;
    NAS_MSCC_SYS_ACQ_SYS_INFO_STRU                          stNextAcqSysInfo;

    NAS_WARNING_LOG(UEPS_PID_MSCC, "NAS_MSCC_RcvTiWaitMmcRfAvailableIndExpired_SysAcq_WaitMmcRfAvailableInd: timer out");

    NAS_MSCC_SetReAcqLteUpOnNoRfFinishFlg_SysAcq(VOS_TRUE);

    pstSysListInfo   = NAS_MSCC_GetCLSysAcqSysList_SysAcq();

    /* refresh the system acquire result */
    NAS_MSCC_UpdateAcqRatStatusInNormalAcqSysList(VOS_RATMODE_LTE,
                                                  NAS_MSCC_SYS_ACQ_STATUS_SEARCHED_NO_EXIST,
                                                  pstSysListInfo);

    pstSysListInfo = NAS_MSCC_GetCLSysAcqSysList_SysAcq();

    /* send next acquire rat mode for normal aquire req */
    ulRslt = NAS_MSCC_GetNextNormalAcquireSysInAcqSysList(pstSysListInfo,
                                                         &stNextAcqSysInfo);

    /* 获取不到下个搜索的模式或者获取失败直接退出状态机 */
    if ((VOS_FALSE        == ulRslt)
     || (VOS_RATMODE_HRPD != stNextAcqSysInfo.ulRatMode))
    {
        NAS_WARNING_LOG(UEPS_PID_MSCC, "NAS_MSCC_RcvTiWaitMmcRfAvailableIndExpired_SysAcq_WaitMmcRfAvailableInd: get next normal acquire sys failure!");

        NAS_MSCC_QuitFsmSysAcq(NAS_MSCC_SYSTEM_ACQUIRED_RESULT_FAILURE);

        return VOS_TRUE;
    }

    NAS_MSCC_SndHsdSysAcqReq_SysAcq();

    return VOS_TRUE;
}



VOS_UINT32 NAS_MSCC_IsNeedReAcqLteUponLteNoRf_SysAcq(VOS_VOID)
{
    NAS_MSCC_CL_SYS_ACQ_DSDS_STRATEGY_SCENE_ENUM_UINT8      enClSysAcqDsdsScene;
    NAS_MSCC_SYS_ACQ_SCENE_ENUM_UINT32                      enCurSysAcqScene;


    /* NV控制关闭lte no rf时重新搜LTE */
    if (VOS_FALSE == NAS_MSCC_GetReAcqLteWithLteNoRfEnableFlg())
    {
        return VOS_FALSE;
    }

    /* 已经尝试过lte一次则不再继续尝试 */
    if (VOS_TRUE == NAS_MSCC_GetReAcqLteUpOnNoRfFinishFlg_SysAcq())
    {
       return VOS_FALSE;
    }

    enClSysAcqDsdsScene = NAS_MSCC_GetReAcqLteWithLteNoRfSysAcqScene();
    enCurSysAcqScene    = NAS_MSCC_GetCLSysAcqScene_SysAcq();

    /* 开机场景进行此优化 */
    if (NAS_MSCC_CL_SYS_ACQ_DSDS_STRATEGY_SCENE_SWITCH_ON == enClSysAcqDsdsScene)
    {
        if (NAS_MSCC_SYS_ACQ_SCENE_SWITCH_ON == enCurSysAcqScene)
        {
            return VOS_TRUE;
        }

        return VOS_FALSE;
    }

    if (NAS_MSCC_CL_SYS_ACQ_DSDS_STRATEGY_SCENE_SWITCH_ON_AND_SYSCFG_SET == enClSysAcqDsdsScene)
    {
        if ((NAS_MSCC_SYS_ACQ_SCENE_SWITCH_ON   == enCurSysAcqScene)
         || (NAS_MSCC_SYS_ACQ_SCENE_SYS_CFG_SET == enCurSysAcqScene))
        {
            return VOS_TRUE;
        }

        return VOS_FALSE;
    }

    if (NAS_MSCC_CL_SYS_ACQ_DSDS_STRATEGY_SCENE_ANY == enClSysAcqDsdsScene)
    {
        return VOS_TRUE;
    }

    return VOS_TRUE;
}


#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


