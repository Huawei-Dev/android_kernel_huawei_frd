

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "CnasHrmSndCas.h"
#include "Nas_Mem.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#define THIS_FILE_ID                    PS_FILE_ID_CNAS_HRM_SND_CAS_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/
/*lint -save -e958*/

VOS_VOID CNAS_HRM_SndCasHrpdResApplyReq(
    RFUSER_CAS_HRPD_TASK_TYPE_ENUM_UINT16                   enTaskType
)
{
    RFUSER_CAS_HRPD_RESOURCE_APPLY_REQ_STRU                *pstHrpdResApplyReq = VOS_NULL_PTR;

    pstHrpdResApplyReq = (RFUSER_CAS_HRPD_RESOURCE_APPLY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(UEPS_PID_HRM,
                        sizeof(RFUSER_CAS_HRPD_RESOURCE_APPLY_REQ_STRU));

    if (VOS_NULL_PTR == pstHrpdResApplyReq)
    {
        return;
    }

    NAS_MEM_SET_S((VOS_UINT8 *)pstHrpdResApplyReq + VOS_MSG_HEAD_LENGTH,
                  sizeof(RFUSER_CAS_HRPD_RESOURCE_APPLY_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                  0x00,
                  sizeof(RFUSER_CAS_HRPD_RESOURCE_APPLY_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    pstHrpdResApplyReq->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstHrpdResApplyReq->ulSenderPid              = UEPS_PID_HRM;
    pstHrpdResApplyReq->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstHrpdResApplyReq->ulReceiverPid            = UEPS_PID_HALMP;
    pstHrpdResApplyReq->ulLength                 = sizeof(RFUSER_CAS_HRPD_RESOURCE_APPLY_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstHrpdResApplyReq->enMsgId                  = ID_RFUSER_CAS_HRPD_RESOURCE_APPLY_REQ;
    pstHrpdResApplyReq->usOpId                   = 0x00;
    pstHrpdResApplyReq->enTaskType               = enTaskType;

    (VOS_VOID)PS_SEND_MSG(UEPS_PID_HRM, pstHrpdResApplyReq);

    return;

}


VOS_VOID CNAS_HRM_SndCasHrpdResRelNtf(
    RFUSER_CAS_HRPD_TASK_TYPE_ENUM_UINT16                   enTaskType
)
{
    RFUSER_CAS_HRPD_RESOURCE_REL_NTF_STRU                  *pstHrpdResRelNtf = VOS_NULL_PTR;

    pstHrpdResRelNtf = (RFUSER_CAS_HRPD_RESOURCE_REL_NTF_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(UEPS_PID_HRM,
                        sizeof(RFUSER_CAS_HRPD_RESOURCE_REL_NTF_STRU));

    if (VOS_NULL_PTR == pstHrpdResRelNtf)
    {
        return;
    }

    NAS_MEM_SET_S((VOS_UINT8 *)pstHrpdResRelNtf + VOS_MSG_HEAD_LENGTH,
                  sizeof(RFUSER_CAS_HRPD_RESOURCE_REL_NTF_STRU) - VOS_MSG_HEAD_LENGTH,
                  0x00,
                  sizeof(RFUSER_CAS_HRPD_RESOURCE_REL_NTF_STRU) - VOS_MSG_HEAD_LENGTH);

    pstHrpdResRelNtf->ulSenderCpuId            = VOS_LOCAL_CPUID;
    pstHrpdResRelNtf->ulSenderPid              = UEPS_PID_HRM;
    pstHrpdResRelNtf->ulReceiverCpuId          = VOS_LOCAL_CPUID;
    pstHrpdResRelNtf->ulReceiverPid            = UEPS_PID_HALMP;
    pstHrpdResRelNtf->ulLength                 = sizeof(RFUSER_CAS_HRPD_RESOURCE_REL_NTF_STRU) - VOS_MSG_HEAD_LENGTH;
    pstHrpdResRelNtf->enMsgId                  = ID_RFUSER_CAS_HRPD_RESOURCE_REL_NTF;
    pstHrpdResRelNtf->usOpId                   = 0x00;
    pstHrpdResRelNtf->enTaskType               = enTaskType;

    (VOS_VOID)PS_SEND_MSG(UEPS_PID_HRM, pstHrpdResRelNtf);

    return;

}

/*lint -restore*/

#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


