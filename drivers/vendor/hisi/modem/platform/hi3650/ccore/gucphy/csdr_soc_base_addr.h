#ifndef __CSDR_SOC_BASEADDR_INTERFACE_H__
#define __CSDR_SOC_BASEADDR_INTERFACE_H__ 
#include "csdr_platform.h"
#define SOC_BBP_CDMA_BASE_ADDR (g_stPlatformContext.stBaseAddr.uwBbpCdmaBaseAddr)
#define SOC_BBP_CDMA_ON_BASE_ADDR (g_stPlatformContext.stBaseAddr.uwBbpCdmaOnBaseAddr)
#define SOC_Modem_DMAC_BASE_ADDR (g_stPlatformContext.stBaseAddr.uwEdmaBaseAddr)
#define SOC_IPC_S_BASE_ADDR (g_stPlatformContext.stBaseAddr.uwIpcmBaseAddr)
#define SOC_SOCP_BASE_ADDR (g_stPlatformContext.stBaseAddr.uwSocpBaseAddr)
#define SOC_AO_SCTRL_BASE_ADDR (g_stPlatformContext.stBaseAddr.uw32kBaseAddr)
#define SOC_MODEM_SCTRL_BASE_ADDR (g_stPlatformContext.stBaseAddr.uwSysctrlBaseAddr)
#define SOC_ABB_CFG_BASE_ADDR (g_stPlatformContext.stBaseAddr.uwAbbCfgAddr)
#define SOC_BBP_COMM_FE_BASE_ADDR (g_stPlatformContext.stBaseAddr.uwBbpcomAddr)
#define SOC_BBP_COMM_ON_BASE_ADDR (g_stPlatformContext.stBaseAddr.uwBbpcomOnAddr)
#define SOC_BBP_GLB_ON_BASE_ADDR (g_stPlatformContext.stBaseAddr.uwBbpGlbOnAddr)
#define SOC_BBP_WCDMA0_BASE_ADDR (g_stPlatformContext.stBaseAddr.uwWbbpBaseAddr)
#define SOC_BBP_COMM0_BASE_ADDR (g_stPlatformContext.stBaseAddr.uwBbpcom2Addr)
#define SOC_PMU_SSI_BASE_ADDR (g_stPlatformContext.stBaseAddr.uwPmuSsiAddr)
#define SOC_PA0_POWER_ON_BASE_ADDR (g_stPlatformContext.stBaseAddr.uwPa0PowerOnBaseAddr)
#define SOC_PA1_POWER_ON_BASE_ADDR (g_stPlatformContext.stBaseAddr.uwPa1PowerOnBaseAddr)
#define SOC_AO_SCTRL_SC_SYSTEST_SLICER_CNT0_ADDR(base) ((base) + (0x890))
#define SOC_BBP_WDG_BASE_ADDR (g_stPlatformContext.stBaseAddr.uwWatchDogAddr)
#endif
