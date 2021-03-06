

#ifndef __CNAS_HSD_PROC_NVIM_H__
#define __CNAS_HSD_PROC_NVIM_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "CnasHsdCtx.h"


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

extern VOS_VOID CNAS_HSD_ReadMruNvim(VOS_VOID);

extern VOS_VOID CNAS_HSD_WriteMruNvim(CNAS_HSD_HRPD_MRU_LIST_STRU *pstMruList);

extern VOS_VOID CNAS_HSD_ReadSearchIntervalNvim(VOS_VOID);

extern VOS_VOID CNAS_HSD_ReadOperLockSysWhiteListInfoNvim(VOS_VOID);

extern VOS_VOID CNAS_HSD_ReadCTCCCustomizeFreqNvim(VOS_VOID);

extern VOS_VOID CNAS_HSD_ReadPrlNvim(VOS_VOID);

extern VOS_VOID CNAS_HSD_ReadHrpdSupportBandClassMaskNvim(VOS_VOID);

extern VOS_VOID CNAS_HSD_ReadHrpdAvoidScheduleInfoNvim(VOS_VOID);

extern VOS_VOID CNAS_HSD_ReadTestConfigNvim(VOS_VOID);

extern VOS_VOID CNAS_HSD_ReadHrpdSysAcqNoRfProtectTimerCfgNvim(VOS_VOID);

extern VOS_VOID CNAS_HSD_WriteCTCCCustomizeFreqNvim(
    VOS_UINT16                          usChannel
);

extern VOS_VOID CNAS_HSD_ReadCbtPrlNvim(VOS_VOID);

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

#endif /* end of CnasHsdProcNvim.h */
