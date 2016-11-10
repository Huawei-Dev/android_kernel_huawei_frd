

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include  "vos.h"
#include  "CnasXccSndLmm.h"
#include  "CnasMain.h"
#include  "CnasMntn.h"
#include  "CnasCcb.h"
#include  "Nas_Mem.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#define THIS_FILE_ID                    PS_FILE_ID_CNAS_XCC_SND_LMM_C

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/

VOS_VOID CNAS_XCC_SndLmmCallStartNtf(
    XCC_LMM_CALL_TYPE_ENUM_UINT16       enCallType
)
{
    XCC_LMM_CALL_START_NTF_STRU        *pstMsg         = VOS_NULL_PTR;

    pstMsg = (XCC_LMM_CALL_START_NTF_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(UEPS_PID_XCC,
              sizeof(XCC_LMM_CALL_START_NTF_STRU));

    if (VOS_NULL_PTR == pstMsg)
    {
        return;
    }

    NAS_MEM_SET_S((VOS_UINT8*)pstMsg + VOS_MSG_HEAD_LENGTH,
                  sizeof(XCC_LMM_CALL_START_NTF_STRU) - VOS_MSG_HEAD_LENGTH,
                  0,
                  sizeof(XCC_LMM_CALL_START_NTF_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 填充消息 */
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = UEPS_PID_XCC;
    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = PS_PID_MM;
    pstMsg->ulLength                    = sizeof(XCC_LMM_CALL_START_NTF_STRU) - VOS_MSG_HEAD_LENGTH;

    pstMsg->enMsgId                     = ID_XCC_LMM_CALL_START_NTF;
    pstMsg->usOpId                      = 0;
    pstMsg->enCallType                  = enCallType;
    CNAS_MNTN_LogMsgInfo((MSG_HEADER_STRU*)pstMsg);

    /* 发送消息 */
    (VOS_VOID)PS_SEND_MSG(UEPS_PID_XCC, pstMsg);
}

VOS_VOID CNAS_XCC_SndLmmCallEndNtf(VOS_VOID)
{
    XCC_LMM_CALL_END_NTF_STRU        *pstMsg         = VOS_NULL_PTR;

    pstMsg = (XCC_LMM_CALL_END_NTF_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(UEPS_PID_XCC,
              sizeof(XCC_LMM_CALL_END_NTF_STRU));

    if (VOS_NULL_PTR == pstMsg)
    {
        return;
    }

    NAS_MEM_SET_S((VOS_UINT8*)pstMsg + VOS_MSG_HEAD_LENGTH,
                  sizeof(XCC_LMM_CALL_END_NTF_STRU) - VOS_MSG_HEAD_LENGTH,
                  0,
                  sizeof(XCC_LMM_CALL_END_NTF_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 填充消息 */
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = UEPS_PID_XCC;
    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = PS_PID_MM;
    pstMsg->ulLength                    = sizeof(XCC_LMM_CALL_END_NTF_STRU) - VOS_MSG_HEAD_LENGTH;

    pstMsg->enMsgId                     = ID_XCC_LMM_CALL_END_NTF;
    pstMsg->usOpId                      = 0;

    CNAS_MNTN_LogMsgInfo((MSG_HEADER_STRU*)pstMsg);

    /* 发送消息 */
    (VOS_VOID)PS_SEND_MSG(UEPS_PID_XCC, pstMsg);
}
#endif









#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
