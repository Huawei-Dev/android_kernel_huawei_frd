

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include  "CnasHrmSndHsd.h"
#include  "Nas_Mem.h"
#include  "CnasMain.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#define THIS_FILE_ID                    PS_FILE_ID_CNAS_HRM_SND_HSD_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/
/*lint -save -e958*/

VOS_VOID CNAS_HRM_SndHsdStartCnf(
    HRM_HSD_START_RSLT_ENUM_UINT32      enRslt
)
{
    HRM_HSD_START_CNF_STRU             *pstStartCnf = VOS_NULL_PTR;

    pstStartCnf = (HRM_HSD_START_CNF_STRU *)CNAS_GetIntMsgSendBuf(UEPS_PID_HRM,
                                                                sizeof(HRM_HSD_START_CNF_STRU));

    if (VOS_NULL_PTR == pstStartCnf)
    {
        return;
    }

    NAS_MEM_SET_S((VOS_UINT8 *)pstStartCnf + VOS_MSG_HEAD_LENGTH,
                  sizeof(HRM_HSD_START_CNF_STRU) - VOS_MSG_HEAD_LENGTH,
                  0x00,
                  sizeof(HRM_HSD_START_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    pstStartCnf->ulSenderCpuId          = VOS_LOCAL_CPUID;
    pstStartCnf->ulSenderPid            = UEPS_PID_HRM;
    pstStartCnf->ulReceiverCpuId        = VOS_LOCAL_CPUID;
    pstStartCnf->ulReceiverPid          = UEPS_PID_HSD;
    pstStartCnf->ulLength               = sizeof(HRM_HSD_START_CNF_STRU) - VOS_MSG_HEAD_LENGTH;
    pstStartCnf->enMsgId                = ID_HRM_HSD_START_CNF;
    pstStartCnf->usOpId                 = 0;
    pstStartCnf->enRslt                 = enRslt;

    (VOS_VOID)CNAS_SndInternalMsg(pstStartCnf);

    return;

}



VOS_VOID CNAS_HRM_SndHsdPowerOffCnf(
    HRM_HSD_POWEROFF_RSLT_ENUM_UINT32   enRslt
)
{
    HRM_HSD_POWEROFF_CNF_STRU          *pstPowerOffCnf = VOS_NULL_PTR;

    pstPowerOffCnf = (HRM_HSD_POWEROFF_CNF_STRU *)CNAS_GetIntMsgSendBuf(UEPS_PID_HRM,
                                                                sizeof(HRM_HSD_POWEROFF_CNF_STRU));

    if (VOS_NULL_PTR == pstPowerOffCnf)
    {
        return;
    }

    NAS_MEM_SET_S((VOS_UINT8 *)pstPowerOffCnf + VOS_MSG_HEAD_LENGTH,
                  sizeof(HRM_HSD_POWEROFF_CNF_STRU) - VOS_MSG_HEAD_LENGTH,
                  0x00,
                  sizeof(HRM_HSD_POWEROFF_CNF_STRU) - VOS_MSG_HEAD_LENGTH);

    pstPowerOffCnf->ulSenderCpuId       = VOS_LOCAL_CPUID;
    pstPowerOffCnf->ulSenderPid         = UEPS_PID_HRM;
    pstPowerOffCnf->ulReceiverCpuId     = VOS_LOCAL_CPUID;
    pstPowerOffCnf->ulReceiverPid       = UEPS_PID_HSD;
    pstPowerOffCnf->ulLength            = sizeof(HRM_HSD_POWEROFF_CNF_STRU) - VOS_MSG_HEAD_LENGTH;
    pstPowerOffCnf->enMsgId             = ID_HRM_HSD_POWEROFF_CNF;
    pstPowerOffCnf->usOpId              = 0;
    pstPowerOffCnf->enRslt              = enRslt;

    (VOS_VOID)CNAS_SndInternalMsg(pstPowerOffCnf);

    return;

}


/*lint -restore*/

#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


