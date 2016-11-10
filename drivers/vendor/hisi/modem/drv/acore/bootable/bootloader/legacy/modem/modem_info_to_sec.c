
#include <debug.h>
#include <osl_types.h>
#include <bsp_shared_ddr.h>
#include <bsp_version.h>
#include <bsp_ddr.h>
#include "param_cfg_to_sec.h"

/* set secure os param cfg*/
void set_modem_info_to_sec(void)
{
#ifdef SHM_SEC_BASE_ADDR

    u64 cfg_base;

    /*lint -save -e835*/
    cfg_base = SHM_SEC_BASE_ADDR + SHM_OFFSET_PARAM_CFG;
    g_param_cfg *param_cfg = ((g_param_cfg * )(cfg_base));

    /* sec boot info */

    /* 动态加载状态 */
    /* 没有加载命令 */
    /* 所有镜像均未校验或未校验通过 */

    memset(&param_cfg->sec_boot_modem_info, 0, sizeof(struct sec_boot_modem_info));

    /* 初始化镜像信息 */
    param_cfg->sec_boot_modem_info.image_info[MODEM].ddr_addr = MDDR_FAMA(DDR_MCORE_ADDR);
    param_cfg->sec_boot_modem_info.image_info[MODEM].ddr_size = DDR_MCORE_SIZE;
    param_cfg->sec_boot_modem_info.image_info[MODEM].unreset_dependcore = \
        param_cfg->sec_boot_modem_info.image_info[MODEM].unreset_dependcore | (1 << MODEM);
#ifdef  CONFIG_DSP
    param_cfg->sec_boot_modem_info.image_info[DSP].ddr_addr = MDDR_FAMA(DDR_TLPHY_IMAGE_ADDR);
    param_cfg->sec_boot_modem_info.image_info[DSP].ddr_size = DDR_TLPHY_IMAGE_SIZE;
    param_cfg->sec_boot_modem_info.image_info[MODEM].unreset_dependcore = \
        param_cfg->sec_boot_modem_info.image_info[MODEM].unreset_dependcore | (1 << DSP);
#endif
#if defined(FEATURE_UE_MODE_CDMA) && (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    param_cfg->sec_boot_modem_info.image_info[XDSP].ddr_addr = MDDR_FAMA(DDR_CBBE_IMAGE_ADDR);
    param_cfg->sec_boot_modem_info.image_info[XDSP].ddr_size = DDR_CBBE_IMAGE_SIZE;
    param_cfg->sec_boot_modem_info.image_info[MODEM].unreset_dependcore = \
        param_cfg->sec_boot_modem_info.image_info[MODEM].unreset_dependcore | (1 << XDSP);
#endif

    param_cfg->sec_boot_modem_info.image_info[MODEM_DTB].ddr_addr = MDDR_FAMA(DDR_MCORE_DTS_ADDR);
    param_cfg->sec_boot_modem_info.image_info[MODEM_DTB].ddr_size = DDR_MCORE_DTS_SIZE;
#if 0
    param_cfg->sec_boot_modem_info.image_info[MODEM].unreset_dependcore = \
        param_cfg->sec_boot_modem_info.image_info[MODEM].unreset_dependcore | (1 << MODEM_DTB);
#endif
#endif
    cprintf("set modem info to sec end\n");
}


