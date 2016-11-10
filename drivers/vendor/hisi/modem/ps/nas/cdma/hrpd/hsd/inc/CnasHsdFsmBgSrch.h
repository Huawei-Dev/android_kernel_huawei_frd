

#ifndef __CNAS_HSD_FSM_BG_SRCH_H__
#define __CNAS_HSD_FSM_BG_SRCH_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "vos.h"
#include  "CnasHsdSndInternalMsg.h"
#include  "CnasHsdCtx.h"
#include "hsd_mscc_pif.h"
#include "hsm_hsd_pif.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/


/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

extern VOS_UINT32 CNAS_HSD_RcvBgSearchReq_BgSrch_Init(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

extern VOS_UINT32 CNAS_HSD_RcvMsccStopBgSearchReq_WaitBgSrchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);
extern VOS_UINT32 CNAS_HSD_RcvCasBgSrchCnf_BgSrch_WaitBgSrchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

extern VOS_UINT32 CNAS_HSD_RcvTiWaitCasBgSrchExpired_BrSrch_WaitBgSrchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

extern VOS_UINT32 CNAS_HSD_RcvCasOhmInd_BgSrch_WaitBgSrchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

extern VOS_UINT32 CNAS_HSD_Rcv1xSysChgInd_BgSrch_WaitBgSrchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

extern VOS_UINT32 CNAS_HSD_RcvCasNetworkLostIndProc_BgSrch_WaitBgSrchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

extern VOS_UINT32 CNAS_HSD_RcvMsccDataCallRedialSysAcqReq_BgSrch_WaitBgSrchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

extern VOS_UINT32 CNAS_HSD_RcvCasSuspendInd_BgSrch_WaitBgSrchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

extern VOS_UINT32 CNAS_HSD_RcvHsmSessionNegResultInd_BgSrch_WaitBgSrchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

extern VOS_UINT32 CNAS_HSD_RcvHsmConnOpenInd_BgSrch_WaitBgSrchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

extern VOS_VOID CNAS_HSD_IsNeedReAcqHrpdProc(VOS_VOID);

extern VOS_VOID CNAS_HSD_SndStopBgSrchReqProc(VOS_VOID);

extern VOS_UINT32 CNAS_HSD_RcvCasStopBgSrchCnf_WaitStopBgSrchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

extern VOS_UINT32 CNAS_HSD_RcvCasStopBgSrchCnf_WaitStopBgSrchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

extern VOS_UINT32 CNAS_HSD_RcvTiWaitCasStopBgSrchExpired_BgSrch_WaitStopBgSrchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

extern VOS_UINT32 CNAS_HSD_RcvCasOhmInd_BgSrch_WaitStopBgSrchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

extern VOS_UINT32 CNAS_HSD_Rcv1xSysChgInd_BgSrch_WaitStopBgSrchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

extern VOS_UINT32 CNAS_HSD_RcvCasNetworkLostIndProc_BgSrch_WaitStopBgSrchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

extern VOS_UINT32 CNAS_HSD_RcvMsccDataCallRedialSysAcqReq_BgSrch_WaitStopBgSrchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

extern VOS_UINT32 CNAS_HSD_RcvCasSuspendInd_BgSrch_WaitStopBgSrchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

extern VOS_UINT32 CNAS_HSD_RcvHsmSessionNegResultInd_BgSrch_WaitStopBgSrchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

extern VOS_UINT32 CNAS_HSD_RcvHsmConnOpenInd_BgSrch_WaitStopBgSrchCnf(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

extern CNAS_HSD_REACQ_FALG_ENUM_UINT32 CNAS_HSD_ProcCasOhmInd_BgSrch(
    CAS_CNAS_HRPD_OVERHEAD_MSG_IND_STRU                    *pstOhmMsg,
    CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32                     *penAcqScene
);

extern CNAS_HSD_REACQ_FALG_ENUM_UINT32 CNAS_HSD_Proc1xSysChgInd_BgSrch(
    MSCC_HSD_1X_SYS_CHANGE_IND_STRU    *pst1xSysIndMsg,
    CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32 *penAcqScene
);

extern CNAS_HSD_REACQ_FALG_ENUM_UINT32 CNAS_HSD_ProcHsmSessionNegResultInd_BgSrch(
    HSM_HSD_SESSION_NEG_RESULT_IND_STRU                    *pstSessionNegRlstInd,
    CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32                     *penAcqScene
);

extern CNAS_HSD_REACQ_FALG_ENUM_UINT32 CNAS_HSD_ProcHsmConnOpenInd_BgSrch(
    HSM_HSD_CONN_OPEN_IND_STRU         *pstOpenInd,
    CNAS_HSD_SYS_ACQ_SCENE_ENUM_UINT32 *penAcqScene
);

#endif

#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of CnasHsdFsmBgSrch.h */
