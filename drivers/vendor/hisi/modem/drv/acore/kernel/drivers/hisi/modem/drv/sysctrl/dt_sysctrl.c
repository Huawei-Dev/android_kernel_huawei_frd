/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2012-2015. All rights reserved.
 * foss@huawei.com
 *
 * If distributed as part of the Linux kernel, the following license terms
 * apply:
 *
 * * This program is free software; you can redistribute it and/or modify
 * * it under the terms of the GNU General Public License version 2 and 
 * * only version 2 as published by the Free Software Foundation.
 * *
 * * This program is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * * GNU General Public License for more details.
 * *
 * * You should have received a copy of the GNU General Public License
 * * along with this program; if not, write to the Free Software
 * * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
 *
 * Otherwise, the following license terms apply:
 *
 * * Redistribution and use in source and binary forms, with or without
 * * modification, are permitted provided that the following conditions
 * * are met:
 * * 1) Redistributions of source code must retain the above copyright
 * *    notice, this list of conditions and the following disclaimer.
 * * 2) Redistributions in binary form must reproduce the above copyright
 * *    notice, this list of conditions and the following disclaimer in the
 * *    documentation and/or other materials provided with the distribution.
 * * 3) Neither the name of Huawei nor the names of its contributors may 
 * *    be used to endorse or promote products derived from this software 
 * *    without specific prior written permission.
 * 
 * * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <linux/of.h>
#include <linux/of_address.h>
#include <osl_module.h>
#include <bsp_sysctrl.h>

struct sysctrl_tag{
	void* addr_virt[sysctrl_max];
	unsigned long addr_phy[sysctrl_max];
	unsigned int reg_sum;
};
struct sysctrl_tag sysctrl;

static unsigned long reg_node_info_get(struct device_node *np, unsigned int sc_index)
{
	const __be32 *spec = NULL;
	int len = 0;
	int na = 0, ns = 0;



	na = of_n_addr_cells(np);
	ns = of_n_size_cells(np);

	spec = of_get_property(np, "reg", &len);

	if (!spec || len < (na + ns) * sizeof(*spec))
	{
		sc_pr_err("index = %d is out of range\n", sc_index);
		return 0;
	}

	return (unsigned long)of_read_number(spec + sc_index *(na + ns), na);
}

static void* virt_addr_lookup(unsigned long phy_addr)
{
	u32 i = 0;
	unsigned long addr = phy_addr & ~0xfff;

	for(i = 0; i < sysctrl.reg_sum; i++)
	{
		if(addr == sysctrl.addr_phy[i])
		{
			return (void*)((unsigned long)sysctrl.addr_virt[i] + phy_addr - sysctrl.addr_phy[i]);
		}
	}

	return 0;
}

void* bsp_sysctrl_addr_get(void* phy_addr){
	return (void*)virt_addr_lookup((unsigned long)phy_addr);
}
EXPORT_SYMBOL_GPL(bsp_sysctrl_addr_get);

void* bsp_sysctrl_addr_byindex(BSP_SYSCTRL_INDEX sc_index){
	return (void*)sysctrl.addr_virt[sc_index];
}
EXPORT_SYMBOL_GPL(bsp_sysctrl_addr_byindex);

void* bsp_sysctrl_addr_phy_byindex(BSP_SYSCTRL_INDEX sc_index){
	return (void*)sysctrl.addr_phy[sc_index];
}
EXPORT_SYMBOL_GPL(bsp_sysctrl_addr_phy_byindex);

int __init dt_sysctrl_init(void)
{
	u32 i;
	struct device_node *node = NULL;
	int ret = 0;
	
	node = of_find_compatible_node(NULL, NULL, "hisilicon,sysctrl_app");
	if (!node)
	{
		sc_pr_err("dts node not found!\n");
		return -1;
	}

	ret = of_property_read_u32(node, "reg_sum", &sysctrl.reg_sum);
	if(ret)
	{
		sc_pr_err("read reg_sum from dts is failed,ret = %d!\n", ret);
		return -1;
	}

	sysctrl.reg_sum = (sysctrl.reg_sum <= sysctrl_max) ? sysctrl.reg_sum : sysctrl_max;
	for(i = 0; i < sysctrl.reg_sum; i++)
	{
		sysctrl.addr_virt[i] = of_iomap(node, (int)i);
		if (0 == sysctrl.addr_virt[i])
		{
			sc_pr_err("of iomap fail!, index=%d\n", i);
			return -1;
		}
		sysctrl.addr_phy[i] = reg_node_info_get(node, i);
		if(sysctrl.addr_phy[i] == 0)
		{
			sc_pr_err("cann't find reg property!, index=%d\n", i);
			return -1;
		}
	}
	for(i = sysctrl.reg_sum; i < sysctrl_max; i++)
	{
		sysctrl.addr_virt[i] = sysctrl.addr_virt[sysctrl_ao];     
		sysctrl.addr_phy[i] = sysctrl.addr_phy[sysctrl_ao];
	}

	system_status_init();

	sc_pr_err("ok\n");

	return 0;
}

#ifdef CONFIG_GET_SYSTEM_STATUS
struct system_status{
	unsigned int reset_ctrl_reg1;
	unsigned int reset_ctrl_reg2;
	unsigned int reset_magic;
	unsigned int power_on_magic;
};
static struct system_status g_system_status;

void system_status_init(void)
{
	struct device_node *node = NULL;
	int ret = 0;

	node = of_find_compatible_node(NULL, NULL, "hisilicon,get_system_status");
	if (!node)
	{
		sc_pr_err("dts node not found!\n");
		return;
	}

	ret = of_property_read_u32_array(node, "system_status_ctrl", (unsigned int*)(&g_system_status), (unsigned int)(sizeof(struct system_status) / sizeof(unsigned int)));
	if (ret)
	{
		sc_pr_err("get system_status_ctrl fail\n");
		return;
	}
	sc_pr_err("ok\n");
}

int get_system_status(void)
{
	void *sc_ao_base;

	sc_ao_base = bsp_sysctrl_addr_byindex(sysctrl_ao);
	/* 非掉电重启 */
	if ((g_system_status.reset_magic==readl(sc_ao_base + g_system_status.reset_ctrl_reg1))
		&& (g_system_status.reset_magic==readl(sc_ao_base + g_system_status.reset_ctrl_reg2)))
		return 1;

	/* 掉电重启 */
	if (((g_system_status.power_on_magic==readl(sc_ao_base+g_system_status.reset_ctrl_reg1))&&(g_system_status.power_on_magic==readl(sc_ao_base+g_system_status.reset_ctrl_reg2)))
		||((g_system_status.reset_magic==readl(sc_ao_base+g_system_status.reset_ctrl_reg1))&&(g_system_status.power_on_magic==readl(sc_ao_base+g_system_status.reset_ctrl_reg2)))
		||((g_system_status.power_on_magic==readl(sc_ao_base+g_system_status.reset_ctrl_reg1))&&(g_system_status.reset_magic==readl(sc_ao_base+g_system_status.reset_ctrl_reg2))))
		return 0;

	/* 没有初始化、或芯片不支持该功能、或魔数被其他组件误改 */
	return -1;
}

#else
/* 芯片不存在"不受上电复位控制的寄存器"，打桩处理 */
void system_status_init(void)
{
	return;
}

int get_system_status(void)
{
	return -1;
}
#endif

core_initcall(dt_sysctrl_init);

