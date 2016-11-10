

#ifndef HISI_ATFDRIVER_H_
#define HISI_ATFDRIVER_H_

#include <linux/init.h>
#include <linux/mutex.h>

/* VivoBus Regs Base & Offset */
#define REG_BASE_VIVOBUS_PHY         (0xE86D0000)
#define ISP_RD_QOS_PRIORITY          (0x8)
#define ISP_RD_QOS_MODE              (0xC)
#define ISP_WR_QOS_PRIORITY          (0x88)
#define ISP_WR_QOS_MODE              (0x8C)
#define A7T0VIVOBUS_RD_QOS_PRIORITY  (0x108)
#define A7T0VIVOBUS_RD_QOS_MODE      (0x10C)
#define A7T0VIVOBUS_WR_QOS_PRIORITY  (0x188)
#define A7T0VIVOBUS_WR_QOS_MODE      (0x18C)
#define ISP1_RD_QOS_PRIORITY          (0x208)
#define ISP1_RD_QOS_MODE              (0x20C)
#define ISP1_WR_QOS_PRIORITY          (0x288)
#define ISP1_WR_QOS_MODE              (0x28C)

#define QOS_FIX_MODE 0x0
#define QOS_BYPASS_MODE 0x2
#define QOS_PRIO_1 0x0
#define QOS_PRIO_2 0x101
#define QOS_PRIO_3 0x202
#define QOS_PRIO_3_PLUS_RD 0x302
#define QOS_PRIO_4 0x303

#define ACCESS_REGISTER_FN_MAIN_ID              (0xc500aa01)
#define ACCESS_REGISTER_FN_SUB_ID_DDR_INTLV     (0x55bbcce0)
#define ACCESS_REGISTER_FN_SUB_ID_DDR_FLUX_W    (0x55bbcce1)
#define ACCESS_REGISTER_FN_SUB_ID_DDR_FLUX_R    (0x55bbcce2)
#define ACCESS_REGISTER_FN_SUB_ID_DDR_DRAM_R    (0x55bbcce3)
#define ACCESS_REGISTER_FN_SUB_ID_DDR_STDID_W   (0x55bbcce4)
#define ACCESS_REGISTER_FN_SUB_ID_DDR_DSS       (0x55bbcce5)
#define ACCESS_REGISTER_FN_SUB_ID_DDR_PASR      (0x55bbcce6)
#define ACCESS_REGISTER_FN_SUB_ID_MASTER_SECURITY_CONFIG    (0x55bbcce7)
#define ACCESS_REGISTER_FN_SUB_ID_DDR_DRM_GRALLOC_SET (0x55bbcce8)
#define ACCESS_REGISTER_FN_SUB_ID_DDR_DRM_VCODEC_SET (0x55bbcce9)
#define ACCESS_REGISTER_FN_SUB_ID_DDR_DRM_CLR (0x55bbccea)

#define IP_REGULATOR_REGISTER_FN_ID             (0xc500fff0)
#define ACCESS_REGISTER_FN_SUB_ID_PCIE_IO_SET	(0xc500eee0)
#define RTC_REGISTER_FN_ID             (0xc500ddd0)
/* pclint coverity--not used */
#define SMMU_BASE (HISI_RESERVED_SMMU_PHYMEM_BASE)

#define DATA_MEM_C_NS_ATTR      0x00000013
#define A7_DYNAMIC_START_ADDR   0xD0000000
#define ISP_FN_ISP_INIT         (0xC500AB11)
#define ISP_FN_ISP_EXIT         (0xC500AB12)
#define ISPMEM_SIZE             (0x8000000)
#define HISP_PARAMS_RDRLIST     (0xCCCE0001)
#define HISP_PARAMS_VQLIST      (0xCCCE0002)
#define HISP_PARAMS_VRLIST      (0xCCCE0003)
#define HISP_PARAMS_DTSLIST     (0xCCCE0004)
#define HISP_PARAMS_RSCLIST     (0xCCCE0005)
#define HISP_PARAMS_RESERVE0    (0xCCCE0080)
#define HISP_PARAMS_RESERVE1    (0xCCCE0081)
#define HISP_PARAMS_RESERVE2    (0xCCCE0082)
#define HISP_PARAMS_RESERVE3    (0xCCCE0083)
#define MEM_PTE_BASE            (0x1AC00000 + 0x00810000)

typedef enum _master_id_type_ {
    MASTER_VDEC_ID = 0,
    MASTER_VENC_ID,
    MASTER_DSS_ID,
    MASTER_ID_MAX
} MASTER_ID_TYPE;


typedef struct tag_atfd_data {
    phys_addr_t  buf_phy_addr;
    unsigned char *buf_virt_addr;
    struct mutex atfd_mutex;
} ATFD_DATA;

noinline int atfd_hisi_service_isp_smc(u64 funcid, u64 arg0, u64 arg1, u64 arg2);
noinline int atfd_hisi_service_efusec_smc(u64 function_id, u64 arg0, u64 arg1, u64 arg2);
noinline int atfd_hisi_service_ip_regulator_smc(u64 function_id, u64 arg0, u64 arg1, u64 arg2);
noinline int atfd_hisi_service_rtc_smc(u64 function_id, u64 arg0, u64 arg1, u64 arg2);
noinline int atfd_hisi_service_access_register_smc(u64 main_fun_id, u64 buff_addr_phy, u64 data_len, u64 sub_fun_id);
noinline int atfd_hisi_rpmb_smc(u64 function_id, u64 arg0, u64 arg1,  u64 arg2);
noinline int atfd_hisee_smc(u64 function_id, u64 arg0, u64 arg1, u64 arg2);
noinline int atfd_hisi_service_pcie_smc(u64 function_id, u64 arg0, u64 arg1, u64 arg2);
void configure_master_security(unsigned int is_security, int master_id);
void configure_dss_register_security(uint32_t addr, uint32_t val, uint8_t bw, uint8_t bs);
#endif /* HISI_ATFDRIVER_H_ */
