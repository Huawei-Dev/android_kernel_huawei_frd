

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "CnasHsdFsmMain.h"
#include "CnasHsdFsmMainTbl.h"
#include "CnasHsdSndInternalMsg.h"
#include "CnasHsdTimer.h"
#include "hsd_mscc_pif.h"
#include "cas_hrpd_airlinkmgmt_nas_pif.h"
#include "cas_hrpd_overheadmsg_nas_pif.h"
#include "CnasHsdFsmBgSrch.h"
#include "CnasHsdFsmBgSrchTbl.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#define THIS_FILE_ID                    PS_FILE_ID_CNAS_HSD_FSM_BG_SRCH_TBL_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* CNAS HSD 主状态机 */
NAS_FSM_DESC_STRU                       g_stCnasHsdBgSrchFsmDesc;

/* CNAS_HSD_BG_SRCH_STA_INIT状态下事件处理表 */
NAS_ACT_STRU g_astCnasHsdBgSrchInitTbl[] =
{
    NAS_ACT_TBL_ITEM( UEPS_PID_MSCC,
                      ID_MSCC_HSD_BG_SEARCH_REQ,
                      CNAS_HSD_RcvBgSearchReq_BgSrch_Init),
};

/* CNAS_HSD_BG_SRCH_STA_WATI_BG_SRCH_CNF状态下事件处理表 */
NAS_ACT_STRU g_astCnasHsdBgSrchWaitBgSrchCnfActTbl[] =
{
    NAS_ACT_TBL_ITEM(UEPS_PID_MSCC,
                     ID_MSCC_HSD_STOP_BG_SEARCH_REQ,
                     CNAS_HSD_RcvMsccStopBgSearchReq_WaitBgSrchCnf),

    NAS_ACT_TBL_ITEM(UEPS_PID_HALMP,
                     ID_CAS_CNAS_HRPD_BSR_LTE_CNF,
                     CNAS_HSD_RcvCasBgSrchCnf_BgSrch_WaitBgSrchCnf),

    NAS_ACT_TBL_ITEM( VOS_PID_TIMER,
                      TI_CNAS_HSD_WAIT_CAS_BG_SRCH_CNF,
                      CNAS_HSD_RcvTiWaitCasBgSrchExpired_BrSrch_WaitBgSrchCnf),

    NAS_ACT_TBL_ITEM( UEPS_PID_HSP,
                      ID_CAS_CNAS_HRPD_OVERHEAD_MSG_IND,
                      CNAS_HSD_RcvCasOhmInd_BgSrch_WaitBgSrchCnf),

    NAS_ACT_TBL_ITEM(UEPS_PID_MSCC,
                     ID_MSCC_HSD_1X_SYS_CHANGE_IND,
                     CNAS_HSD_Rcv1xSysChgInd_BgSrch_WaitBgSrchCnf),

    NAS_ACT_TBL_ITEM( UEPS_PID_HALMP,
                      ID_CAS_CNAS_HRPD_NETWORK_LOST_IND,
                      CNAS_HSD_RcvCasNetworkLostIndProc_BgSrch_WaitBgSrchCnf),

    NAS_ACT_TBL_ITEM( UEPS_PID_MSCC,
                      ID_MSCC_HSD_DATA_CALL_REDIAL_SYS_ACQ_REQ,
                      CNAS_HSD_RcvMsccDataCallRedialSysAcqReq_BgSrch_WaitBgSrchCnf),

    NAS_ACT_TBL_ITEM(UEPS_PID_HALMP,
                     ID_CAS_CNAS_HRPD_SUSPEND_IND,
                     CNAS_HSD_RcvCasSuspendInd_BgSrch_WaitBgSrchCnf),

    NAS_ACT_TBL_ITEM( UEPS_PID_HSM,
                      ID_HSM_HSD_SESSION_NEG_RESULT_IND,
                      CNAS_HSD_RcvHsmSessionNegResultInd_BgSrch_WaitBgSrchCnf),

    NAS_ACT_TBL_ITEM( UEPS_PID_HSM,
                      ID_HSM_HSD_CONN_OPEN_IND,
                      CNAS_HSD_RcvHsmConnOpenInd_BgSrch_WaitBgSrchCnf),
};

/* CNAS_HSD_BG_SRCH_STA_WATI_STOP_BG_SRCH_CNF状态下事件处理表 */
NAS_ACT_STRU g_astCnasHsdBgSrchWaitStopBgSrchCnfActTbl[] =
{
    NAS_ACT_TBL_ITEM( UEPS_PID_HALMP,
                      ID_CAS_CNAS_HRPD_STOP_BSR_LTE_CNF,
                      CNAS_HSD_RcvCasStopBgSrchCnf_WaitStopBgSrchCnf),

     NAS_ACT_TBL_ITEM( VOS_PID_TIMER,
                       TI_CNAS_HSD_WAIT_CAS_STOP_BG_SRCH_CNF,
                       CNAS_HSD_RcvTiWaitCasStopBgSrchExpired_BgSrch_WaitStopBgSrchCnf),

    NAS_ACT_TBL_ITEM( UEPS_PID_HSP,
                      ID_CAS_CNAS_HRPD_OVERHEAD_MSG_IND,
                      CNAS_HSD_RcvCasOhmInd_BgSrch_WaitStopBgSrchCnf),

    NAS_ACT_TBL_ITEM(UEPS_PID_MSCC,
                     ID_MSCC_HSD_1X_SYS_CHANGE_IND,
                     CNAS_HSD_Rcv1xSysChgInd_BgSrch_WaitStopBgSrchCnf),

    NAS_ACT_TBL_ITEM( UEPS_PID_HALMP,
                      ID_CAS_CNAS_HRPD_NETWORK_LOST_IND,
                      CNAS_HSD_RcvCasNetworkLostIndProc_BgSrch_WaitStopBgSrchCnf),

    NAS_ACT_TBL_ITEM( UEPS_PID_MSCC,
                      ID_MSCC_HSD_DATA_CALL_REDIAL_SYS_ACQ_REQ,
                      CNAS_HSD_RcvMsccDataCallRedialSysAcqReq_BgSrch_WaitStopBgSrchCnf),

    NAS_ACT_TBL_ITEM(UEPS_PID_HALMP,
                     ID_CAS_CNAS_HRPD_SUSPEND_IND,
                     CNAS_HSD_RcvCasSuspendInd_BgSrch_WaitStopBgSrchCnf),

    NAS_ACT_TBL_ITEM( UEPS_PID_HSM,
                      ID_HSM_HSD_SESSION_NEG_RESULT_IND,
                      CNAS_HSD_RcvHsmSessionNegResultInd_BgSrch_WaitStopBgSrchCnf),

    NAS_ACT_TBL_ITEM( UEPS_PID_HSM,
                      ID_HSM_HSD_CONN_OPEN_IND,
                      CNAS_HSD_RcvHsmConnOpenInd_BgSrch_WaitStopBgSrchCnf),
};

/* HSD BG SRCH 状态机状态处理表 */
NAS_STA_STRU g_astCnasHsdBgSrchStaTbl[] =
{
    /*****************定义及关机到开机初始化状态转移表**********************/
    NAS_STA_TBL_ITEM( CNAS_HSD_BG_SRCH_STA_INIT,
                      g_astCnasHsdBgSrchInitTbl ),

    /*****************定义在HRPD休眠时候的状态转移表**********************/
    NAS_STA_TBL_ITEM( CNAS_HSD_BG_SRCH_STA_WAIT_BG_SRCH_CNF,
                      g_astCnasHsdBgSrchWaitBgSrchCnfActTbl ),

    /*****************定义系统选择状态转移表**********************/
    NAS_STA_TBL_ITEM( CNAS_HSD_BG_SRCH_STA_WAIT_STOP_BG_SRCH_CNF,
                      g_astCnasHsdBgSrchWaitStopBgSrchCnfActTbl ),
};

/*****************************************************************************
  3 函数定义
*****************************************************************************/
/*lint -save -e958*/

VOS_UINT32 CNAS_HSD_GetBgSrchStaTblSize(VOS_VOID)
{
    return (sizeof(g_astCnasHsdBgSrchStaTbl)/sizeof(NAS_STA_STRU));
}


NAS_FSM_DESC_STRU* CNAS_HSD_GetBgSrchFsmDescAddr(VOS_VOID)
{
    return (&g_stCnasHsdBgSrchFsmDesc);
}

/*lint -restore*/
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */



