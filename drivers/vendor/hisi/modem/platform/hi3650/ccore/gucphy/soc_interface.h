#ifndef __SOC_INTERFACE_H__
#define __SOC_INTERFACE_H__ 
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#ifndef BIT_X
#define BIT_X(num) (((unsigned long)0x01) << (num))
#endif
#define ZSP_ITCM_BASE_ADDR ((unsigned long)0x00000000)
#define ZSP_ITCM_BASE_ADDR_HIGH ((unsigned long)ZSP_ITCM_BASE_ADDR>>16)
#define ZSP_DTCM_BASE_ADDR ((unsigned long)0x00000000)
#define ZSP_DTCM_BASE_ADDR_HIGH ((unsigned long)ZSP_DTCM_BASE_ADDR>>16)
#define WBBP_BASE_ADDR (SOC_BBP_WCDMA_BASE_ADDR)
#define WBBP_BASE_ADDR_HIGH ((unsigned long)WBBP_BASE_ADDR>>16)
#ifdef INSTANCE_1
#define SOC_BBP_GSM_BASE_ADDR ((unsigned long)SOC_BBP_GSM1_BASE_ADDR)
#define SOC_BBP_GSM_BASE_ADDR_HIGH ((unsigned long)SOC_BBP_GSM1_BASE_ADDR>>16)
#define SOC_BBP_GSM_ON_BASE_ADDR ((unsigned long)SOC_BBP_GSM1_ON_BASE_ADDR)
#define SOC_BBP_GSM_ON_BASE_ADDR_HIGH ((unsigned long)SOC_BBP_GSM1_ON_BASE_ADDR>>16)
#define SOC_BBP_WCDMA_BASE_ADDR ((unsigned long)SOC_BBP_WCDMA0_BASE_ADDR)
#define SOC_BBP_WCDMA_ON_BASE_ADDR ((unsigned long)SOC_BBP_WCDMA0_ON_BASE_ADDR)
#define SOC_BBP_COMM_BASE_ADDR ((unsigned long)SOC_BBP_COMM0_BASE_ADDR)
#define SOC_BBP_GSDR_BASE_ADDR ((unsigned long)SOC_BBP_GSDR1_BASE_ADDR)
#else
#define SOC_BBP_GSM_BASE_ADDR ((unsigned long)SOC_BBP_GSM0_BASE_ADDR)
#define SOC_BBP_GSM_BASE_ADDR_HIGH ((unsigned long)SOC_BBP_GSM0_BASE_ADDR>>16)
#define SOC_BBP_GSM_ON_BASE_ADDR ((unsigned long)SOC_BBP_GSM0_ON_BASE_ADDR)
#define SOC_BBP_GSM_ON_BASE_ADDR_HIGH ((unsigned long)SOC_BBP_GSM0_ON_BASE_ADDR>>16)
#define SOC_BBP_WCDMA_BASE_ADDR ((unsigned long)SOC_BBP_WCDMA0_BASE_ADDR)
#define SOC_BBP_WCDMA_ON_BASE_ADDR ((unsigned long)SOC_BBP_WCDMA0_ON_BASE_ADDR)
#define SOC_BBP_COMM_BASE_ADDR ((unsigned long)SOC_BBP_COMM0_BASE_ADDR)
#define SOC_BBP_GSDR_BASE_ADDR ((unsigned long)SOC_BBP_GSDR0_BASE_ADDR)
#endif
#define GBBP_BASE_ADDR (SOC_BBP_GSM_BASE_ADDR)
#define AHB_BASE_ADDR (uwAhbBaseAddr)
#define AHB_BASE_ADDR_HIGH ((unsigned long)AHB_BASE_ADDR>>16)
#define BBPCOMM_BASE_ADDR (SOC_BBP_COMM_BASE_ADDR)
#define HSUPA_UPACC_BASE_ADDR (SOC_UPACC_BASE_ADDR)
#define HSUPA_UPACC_BASE_ADDR_HIGH ((unsigned long)HSUPA_UPACC_BASE_ADDR>>16)
#define DMAC_BASE_ADDR ((unsigned long)0x20380000)
#define DMAC_BASE_ADDR_HIGH ((unsigned long)DMAC_BASE_ADDR>>16)
#define SOC_BASE_ADDR HI_SYSCTRL_BASE_ADDR
#define SOC_BASE_ADDR_HIGH ((unsigned long)SOC_BASE_ADDR>>16)
#define SYS_CTRL_ADDRESS ( SOC_BASE_ADDR_HIGH )
#define SOC_LP_CFG_ASIC_BASE_ADDR (0xFFF3F000)
#define SOC_LP_CFG_SIZE (0x1000)
#define SOC_PERI_CFG_ASIC_BASE_ADDR (0xFFF35000)
#define SOC_PERI_CFG_SIZE (0x1000)
#define SOC_HARQ_MEM_ASIC_BASE_ADDR (0xEA000000)
#define SOC_HARQ_MEM_SIZE (0x70000)
#if 0
#define HAL_SSI0_BASE_ADDR ((unsigned long)0x5F061000)
#define HAL_SSI0_BASE_ADDR_HIGH ((unsigned long)HAL_SSI0_BASE_ADDR>>16)
#endif
#define IPCRIS_BASE_ADDR ((unsigned long)0x27054000 )
#define IPCRIS_BASE_ADDR_HIGH ((unsigned long)IPCRIS_BASE_ADDR>>16)
#define IPCRIS_BASE_ADDR_LOW (0x4000)
#define APM_BASE_ADDR (0x0)
#define SOC_TIMER_BASE_ADDR ((unsigned long)0x27067000 )
#define SOC_WBBP_CLK_SEL_ADDR ( SOC_BASE_ADDR + 0x148 )
#define DMA_LLI_PARA_HIGH_ADDR (AHB_BASE_ADDR_HIGH)
#define ARM_SOC_CPUVIEW_TIMER_VALUE (0x90003000)
#define ARM_SOC_CPUVIEW_TIMER_LOAD (0x90003000)
#define ARM_SOC_CPUVIEW_TIMER_CTRL (0x90003000)
extern unsigned int uwAhbBaseAddr;
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
#endif
