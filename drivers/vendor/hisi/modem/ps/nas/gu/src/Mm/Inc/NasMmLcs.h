

#ifndef __NASMMLCS_H__
#define __NASMMLCS_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"


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
extern VOS_VOID NAS_MM_DealBufferedLcsEstReq(VOS_VOID);
extern VOS_VOID NAS_MM_RcvLcsDataReq_MmConnectionActive(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsDataReq_MmInterRatChange(VOS_VOID* pMsg);
extern VOS_UINT32 NAS_MM_RcvLcsDataReq_PreProc(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsDataReq_WaitForAdditionalMmConn(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_CSFB(VOS_VOID);
extern VOS_VOID NAS_MM_RcvLcsEstReq_ImsiDetachInitiated(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_ImsiDetachPending(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_LauInitiated(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_LauPending(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_LauRejected(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_MmConnectionActive(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_MmIdleAttempToUpdate(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_MmIdleECallInactive(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_MmIdleLauNeeded(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_MmIdleLimitedService(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_MmIdleNoCellAvaliable(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_MmIdleNoImsi(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_MmIdleNormalService(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_MmIdlePlmnSearch(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_MmInterRatChange(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_Null(VOS_VOID* pMsg);
extern VOS_UINT32 NAS_MM_RcvLcsEstReq_PreProc(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_ProcCmServicePrompt(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_RrConnRelNotAllow(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_TestContrlActive(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_WaitForAdditionalMmConn(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_WaitForAttach(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_WaitForNetworkCmd(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_WaitForOutgoingMmConn(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_WaitForReestCnf(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_WaitForReestReq(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_WaitForRrConn(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_WaitForRrConnImsiDetach(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsEstReq_WaitForRrConnLau(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsRelReq_ImsiDetachInitiated(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsRelReq_ImsiDetachPending(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsRelReq_LauInitiated(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsRelReq_LauPending(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsRelReq_LauRejected(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsRelReq_MmConnectionActive(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsRelReq_MmIdleAttempToUpdate(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsRelReq_MmIdleLauNeeded(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsRelReq_MmIdleLimitedService(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsRelReq_MmIdleNoCellAvaliable(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsRelReq_MmIdleNoImsi(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsRelReq_MmIdleNormalService(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsRelReq_MmIdlePlmnSearch(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsRelReq_MmInterRatChange(VOS_VOID* pMsg);
extern VOS_UINT32 NAS_MM_RcvLcsRelReq_PreProc(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsRelReq_ProcCmServicePrompt(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsRelReq_RrConnRelNotAllow(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsRelReq_WaitForAdditionalMmConn(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsRelReq_WaitForAttach(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsRelReq_WaitForNetworkCmd(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsRelReq_WaitForOutgoingMmConn(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsRelReq_WaitForReestCnf(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsRelReq_WaitForReestReq(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsRelReq_WaitForRrConn(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsRelReq_WaitForRrConnImsiDetach(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsRelReq_WaitForRrConnLau(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvLcsStatusNty(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrDataIndLcs_MmConnectionActive(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrDataIndLcs_RrConnRelNotAllow(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrDataIndLcs_WaitForAdditionalMmConn(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrDataIndLcs_WaitForNetworkCmd(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrDataIndLcs_WaitForOutgoingMmConn(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_ImsiDetachInitiated(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_ImsiDetachPending(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_LauInitiated(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_LauPending(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_LauRejected(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_MmConnectionActive(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_MmIdleAttempToUpdate(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_MmIdleECallInactive(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_MmIdleLauNeeded(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_MmIdleLimitedService(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_MmIdleNoCellAvaliable(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_MmIdleNoImsi(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_MmIdleNormalService(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_MmIdlePlmnSearch(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_MmInterRatChange(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_Null(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_ProcCmServicePrompt(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_RrConnRelNotAllow(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_TestContrlActive(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_WaitForAdditionalMmConn(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_WaitForAttach(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_WaitForNetworkCmd(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_WaitForOutgoingMmConn(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_WaitForReestCnf(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_WaitForReestReq(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_WaitForRrConn(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_WaitForRrConnImsiDetach(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvRrLcsStateInd_WaitForRrConnLau(VOS_VOID* pMsg);
extern VOS_VOID NAS_MM_RcvT3241Expired_PreProc(VOS_VOID);
extern VOS_VOID NAS_MM_RcvT3241Expired_RrConnRelNotAllow(VOS_VOID* pMsg);








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

#endif /* end of NasMmLcs.h */

