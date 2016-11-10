#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/bitops.h>
#include <linux/of.h>
#include <linux/io.h>
#include <linux/semaphore.h>
#include <linux/printk.h>
#include <linux/hisi/hisi_rproc.h>
#include <linux/hisi/ipc_msg.h>
#include "hisi_ddr_autofsgt_proxy_kernel.h"

#if defined (CONFIG_HISI_DDR_AUTO_FSGT)
static unsigned long g_ddr_autofsgt_disable_flag = 0;
static unsigned long g_ddr_autofsgt_bypass_flag = 0;
static unsigned int g_ddr_autofsgt_sw = 1;
static struct semaphore ctrl_sem;


/*s32 ddr_autofsgt_ctrl(DDR_AUTOFSGT_PROXY_CLIENT_ID client, DDR_AUTOFSGT_CMD_ID cmd)*/
int ddr_autofsgt_ctrl(unsigned int client, unsigned int cmd) {
    int ret = 0;
    unsigned int send_cmd = 0;
    union ipc_data msg = {0};
    union ipc_data ack = {0};

    down(&ctrl_sem);
    switch(cmd) {
        case DDR_AUTOFSGT_ENABLE:
            clear_bit(client, &g_ddr_autofsgt_disable_flag);
            if(!g_ddr_autofsgt_disable_flag) {
                /*send ipc enable*/
                send_cmd = DDR_AUTOFSGT_ENABLE;
            }
            break;

        case DDR_AUTOFSGT_DISABLE:
            if(!g_ddr_autofsgt_disable_flag) {
                /*send ipc disable*/
                send_cmd = DDR_AUTOFSGT_DISABLE;
            }
            set_bit(client, &g_ddr_autofsgt_disable_flag);
            break;

        case DDR_AUTOFSGT_LOGIC_EN:
            clear_bit(client, &g_ddr_autofsgt_bypass_flag);
            if(!g_ddr_autofsgt_bypass_flag) {
                /*send ipc no bypass*/
                send_cmd = DDR_AUTOFSGT_LOGIC_EN;
            }
            break;

        case DDR_AUTOFSGT_LOGIC_DIS:
            if(!g_ddr_autofsgt_bypass_flag) {
                /*send ipc bypass*/
                send_cmd = DDR_AUTOFSGT_LOGIC_DIS;
            }
            set_bit(client, &g_ddr_autofsgt_bypass_flag);
            break;
        default:
            up(&ctrl_sem);
            return -EINVAL;
    }

    if(send_cmd && g_ddr_autofsgt_sw) {
        /*send ipc*/
        (msg.cmd_mix).cmd_type = TYPE_CLK;
        (msg.cmd_mix).cmd = CMD_NOTIFY;
        (msg.cmd_mix).cmd_obj = OBJ_DDR;
        (msg.cmd_mix).cmd_src = OBJ_AP;
        (msg.cmd_mix).cmd_para[0] = DDR_AUTOFSGT_CLIENT_AP;
        (msg.cmd_mix).cmd_para[1] = send_cmd;

    	ret = RPROC_SYNC_SEND(HISI_RPROC_LPM3_MBX27,
    			(rproc_msg_t *)&msg,
    			(rproc_msg_len_t)2,
    			(rproc_msg_t *)&ack,
    			(rproc_msg_len_t)2);
    	if (ret) {
    		pr_err("[%s] send data err:[0x%x][0x%x]\n", __func__, msg.data[0], msg.data[1]);
    		up(&ctrl_sem);
    		return -EINVAL;
    	}

        if (ack.data[1]) {
            pr_err("[%s] recv err:[0x%x][0x%x]\n", __func__, ack.data[0], ack.data[1]);
    		up(&ctrl_sem);
    		return -EINVAL;
    	}
    }
    up(&ctrl_sem);
    return 0;
}

static int __init ddr_autofsgt_proxy_init(void) {
	sema_init(&ctrl_sem, 1);
	g_ddr_autofsgt_sw = 1;
	return 0;
}

#else

int ddr_autofsgt_ctrl(unsigned int client, unsigned int cmd) {
    return 0;
}

static int __init ddr_autofsgt_proxy_init(void) {
	return 0;
}

#endif

module_init(ddr_autofsgt_proxy_init);
