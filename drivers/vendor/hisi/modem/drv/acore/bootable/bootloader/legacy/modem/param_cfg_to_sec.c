#include <debug.h>

#include <osl_types.h>
#include <bsp_shared_ddr.h>
#include <bsp_version.h>
#include "param_cfg_to_sec.h"

/* set secure os param cfg*/
void set_param_cfg_to_sec(void)
{
#ifdef SHM_SEC_BASE_ADDR
    u64 cfg_base = 0;
    u64 balong_param_base_addr = 0;

    if (CHIP_V8R5 == bsp_version_get_chip_type())
        balong_param_base_addr = 0x367FF800;      /* 0x367FF800--0x367FF8FF */
    else if(CHIP_K3V6 == bsp_version_get_chip_type())
        balong_param_base_addr = 0x1D7FF800;      /* 0x1D7FF800--0x1D7FF8FF */
    else
        return;

    /*lint -save -e835*/
    cfg_base = SHM_SEC_BASE_ADDR + SHM_OFFSET_PARAM_CFG;
    PRINT_ERROR("shm_sec_addr_h:0x%x\n", (u32)((u64)SHM_SEC_BASE_ADDR >> 32));
    PRINT_ERROR("shm_sec_addr_l:0x%x\n", (u32)SHM_SEC_BASE_ADDR);
    g_param_cfg *param_cfg = ((g_param_cfg * )(cfg_base));

    *(volatile u64 *) (balong_param_base_addr + PARAM_MAGIC_OFFSET) = 0xdeadbeef;
    *(volatile u64 *) (balong_param_base_addr + PARAM_CFG_OFFSET) = (cfg_base);

    param_cfg->magic = 0xdeadbeef;

    param_cfg->param_cfg_size = SHM_SIZE_PARAM_CFG;

    param_cfg->icc_channel_base_addr = SHM_SEC_BASE_ADDR + SHM_OFFSET_SEC_ICC;
    /*lint -restore*/

    param_cfg->icc_channel_max_size = SHM_SIZE_SEC_ICC;
#endif
    cprintf("set param cfg to sec end\n");
}


