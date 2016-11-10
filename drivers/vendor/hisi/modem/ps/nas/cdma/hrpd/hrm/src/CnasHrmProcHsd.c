

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "CnasHrmProcHsd.h"
#include "CnasHrmSndHsd.h"
#include "CnasHrmCtx.h"
#include "hsd_hrm_pif.h"
#include "Nas_Mem.h"
#include "CnasMntn.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#define THIS_FILE_ID                    PS_FILE_ID_CNAS_HRM_PROC_HSD_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/*****************************************************************************
  3 函数实现
*****************************************************************************/
/*lint -save -e958*/

VOS_VOID CNAS_HRM_LogPcReplayFixedContext(VOS_VOID)
{
    CNAS_HRM_CTX_STRU                                      *pstCnasHrmCtx = VOS_NULL_PTR;
    CNAS_HRM_PC_REPLAY_FIXED_CONTEXT_MSG_STRU                         stFixedCtxData;

    pstCnasHrmCtx      = CNAS_HRM_GetCtxAddr();

    NAS_MEM_SET_S(&stFixedCtxData, sizeof(stFixedCtxData), 0, sizeof(stFixedCtxData));

    stFixedCtxData.ucIsNeedWaitResApplied = pstCnasHrmCtx->ucIsNeedWaitResApplied;
    stFixedCtxData.enDsdsType             = pstCnasHrmCtx->enDsdsType;

    CNAS_MNTN_LogPcReplayFixedContext(sizeof(CNAS_HRM_PC_REPLAY_FIXED_CONTEXT_MSG_STRU), UEPS_PID_HRM, (VOS_UINT8 *)&stFixedCtxData);
}

VOS_VOID  CNAS_HRM_RcvHsdStartReq(VOS_VOID)
{
    CNAS_HRM_InitCtx();

    CNAS_HRM_ReadNvimInfo();

#ifndef __PS_WIN32_RECUR__
    CNAS_HRM_LogPcReplayFixedContext();
#endif

    CNAS_HRM_SndHsdStartCnf(HRM_HSD_START_RSLT_SUCC);
}


VOS_VOID  CNAS_HRM_RcvHsdPowerOffReq(VOS_VOID)
{
    CNAS_HRM_InitCtx();

    CNAS_HRM_SndHsdPowerOffCnf(HRM_HSD_POWEROFF_RSLT_SUCC);
}



VOS_VOID  CNAS_HRM_RcvHsdMsg(
    struct MsgCB                       *pstMsg
)
{
    VOS_UINT16                          usMsgName;

    usMsgName = *((VOS_UINT16 *)(pstMsg->aucValue));

    switch (usMsgName)
    {
        case ID_HSD_HRM_START_REQ:
            CNAS_HRM_RcvHsdStartReq();
            break;

        case ID_HSD_HRM_POWEROFF_REQ:
            CNAS_HRM_RcvHsdPowerOffReq();
            break;

        default:
            break;
     }

     return;
}

/*lint -restore*/

#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


