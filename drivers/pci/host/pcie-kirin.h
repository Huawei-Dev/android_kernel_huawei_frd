/*
 * PCIe host controller driver for Kirin 960 SoCs
 *
 * Copyright (C) 2015 Huawei Electronics Co., Ltd.
 *		http://www.huawei.com
 *
 * Author: Xiaowei Song <songxiaowei@huawei.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _PCIE_KIRIN_H
#define _PCIE_KIRIN_H

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/irqdomain.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_gpio.h>
#include <linux/pci.h>
#include <linux/of_pci.h>
#include <linux/platform_device.h>
#include <linux/resource.h>
#include <linux/signal.h>
#include <linux/types.h>
#include <linux/irq.h>
#include <linux/msi.h>
#include <linux/of_address.h>
#include <linux/pci_regs.h>
#include <linux/regulator/consumer.h>
#include <linux/version.h>
#include <linux/hisi/hisi_atfdriver.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 1, 0))
#include "pcie-designware.h"
#else
#include "kirin-pcie-designware.h"
#endif
#define to_kirin_pcie(x)	container_of(x, struct kirin_pcie, pp)


/* PCIe ELBI registers */
#define SOC_PCIECTRL_CTRL0_ADDR 0x000
#define SOC_PCIECTRL_CTRL1_ADDR 0x004
#define SOC_PCIECTRL_CTRL7_ADDR 0x01c
#define SOC_PCIECTRL_CTRL8_ADDR 0x020
#define SOC_PCIECTRL_CTRL11_ADDR 0x02c
#define SOC_PCIECTRL_CTRL12_ADDR 0x030
#define SOC_PCIECTRL_STATE1_ADDR 0x404
#define SOC_PCIEPHY_CTRL0_ADDR 0x000
#define SOC_PCIEPHY_CTRL1_ADDR 0x004
#define SOC_PCIEPHY_STATE0_ADDR 0x400
#define SOC_PCIECTRL_STATE0_ADDR 0x400
#define SOC_PCIECTRL_STATE4_ADDR 0x410
#define SOC_PCIECTRL_STATE5_ADDR 0x414
#define ENTRY_L23_BIT (0x1<<2)
#define PCIE_ELBI_SLV_DBI_ENABLE	(0x1 << 21)
#define PME_TURN_OFF_BIT (0x1<<8)
#define APPS_PM_XMT_PME_BIT (0x1<<9)
#define PME_ACK_BIT (0x1<<16)
#define SOC_PCIECTRL_CTRL6_ADDR 0x018
#define PCI_ANY_ID (~0)

#define RD_FLAG 0
#define WR_FLAG 1
#define IOMG_GPIO 0
#define IOMG_CLKREQ 1
#define BOARD_FPGA 0
#define BOARD_EMU  1
#define BOARD_ASIC 2

struct kirin_pcie {
	void __iomem		*apb_base;
	void __iomem		*phy_base;
	void __iomem		*crg_base;
	void __iomem        *sctrl_base;
	struct clk			*apb_sys_clk;
	struct clk			*apb_phy_clk;
	struct clk			*phy_ref_clk;
	struct clk			*pcie_aclk;
	struct clk			*pcie_aux_clk;
	int					board_type;
	int                 ep_flag;
	struct  pcie_port	pp;
	struct  pci_dev		*pci_dev;
	unsigned int		devid;
	unsigned int		venid;
	unsigned int		usr_suspend;
	unsigned int		usr_resume;
	struct mutex		pm_lock;
};

enum link_aspm_state{
	ASPM_CLOSE = 0,		/*disable aspm L0s L1*/
	ASPM_L0S = 1,		/* enable l0s  */
	ASPM_L1 = 2,		/* enable l1 */
	ASPM_L0S_L1 = 3,	/* enable l0s & l1*/
};

enum l1ss_ctrl_state{
	L1SS_CLOSE = 0x0,		/*disable l1ss*/
	L1SS_PM = 0x3,		/* enable pci-pm l1ss  */
	L1SS_ASPM = 0xC,		/* enable aspm l1ss */
	L1SS_PM_ASPM = 0xF,	/* enable aspm l1ss & pci-pm l1ss*/
};
#ifdef __SLT_FEATURE__
enum pcie_test_result {
	RESULT_OK,
	ERR_DATA_TRANS,
	ERR_L0S,
	ERR_L1,
	ERR_L0_L1,
	ERR_L1_1,
	ERR_L1_2,
	ERR_OTHER
};
#endif
#define PCIE_PR_ERR(fmt, args ...)	do {	printk(KERN_ERR "%s(%d):" fmt "\n", __FUNCTION__, __LINE__, ##args); } while (0)
#define PCIE_PR_INFO(fmt, args ...)	do {	printk(KERN_ERR "%s(%d):" fmt "\n", __FUNCTION__, __LINE__, ##args); } while (0)
#define PCIE_PR_DEBUG(fmt, args ...)	do {	printk(KERN_DEBUG "%s(%d):" fmt "\n", __FUNCTION__, __LINE__, ##args); } while (0)

void kirin_elb_writel(struct kirin_pcie *pcie, u32 val, u32 reg);

u32 kirin_elb_readl(struct kirin_pcie *pcie, u32 reg);

/*Registers in PCIePHY*/
void kirin_phy_writel(struct kirin_pcie *pcie, u32 val, u32 reg);

u32 kirin_phy_readl(struct kirin_pcie *pcie, u32 reg);

void kirin_pcie_readl_rc(struct pcie_port *pp,
					void __iomem *dbi_base, u32 *val);
void kirin_pcie_writel_rc(struct pcie_port *pp,
					u32 val, void __iomem *dbi_base);
int kirin_pcie_rd_own_conf(struct pcie_port *pp, int where, int size,
				u32 *val);
int kirin_pcie_wr_own_conf(struct pcie_port *pp, int where, int size,
				u32 val);
int kirin_pcie_power_on(struct pcie_port *pp, int on_flag);
void kirin_pcie_mtcmos_ctrl(struct kirin_pcie *pcie, bool enable);
#endif
