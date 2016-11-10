/*
 * PCIe host controller driver for Kirin 960 SoCs
 *
 * Copyright (C) 2015 Huawei Electronics Co., Ltd.
 *		http://www.huawei.com
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */


#include "pcie-kirin.h"
#include <linux/pci-aspm.h>
#include <asm/memory.h>
#include <linux/pci_regs.h>

/*designware register*/
#define PCIE_ATU_VIEWPORT		0x900
#define PCIE_ATU_REGION_INBOUND		(0x1 << 31)
#define PCIE_ATU_REGION_OUTBOUND	(0x0 << 31)
#define PCIE_ATU_REGION_INDEX1		(0x1 << 0)
#define PCIE_ATU_REGION_INDEX0		(0x0 << 0)
#define PCIE_ATU_CR1			0x904
#define PCIE_ATU_TYPE_MEM		(0x0 << 0)
#define PCIE_ATU_TYPE_IO		(0x2 << 0)
#define PCIE_ATU_TYPE_CFG0		(0x4 << 0)
#define PCIE_ATU_TYPE_CFG1		(0x5 << 0)
#define PCIE_ATU_CR2			0x908
#define PCIE_ATU_ENABLE			(0x1 << 31)
#define PCIE_ATU_BAR_MODE_ENABLE	(0x1 << 30)
#define PCIE_ATU_LOWER_BASE		0x90C
#define PCIE_ATU_UPPER_BASE		0x910
#define PCIE_ATU_LIMIT			0x914
#define PCIE_ATU_LOWER_TARGET		0x918
#define PCIE_ATU_BUS(x)			(((x) & 0xff) << 24)
#define PCIE_ATU_DEV(x)			(((x) & 0x1f) << 19)
#define PCIE_ATU_FUNC(x)		(((x) & 0x7) << 16)
#define PCIE_ATU_UPPER_TARGET		0x91C
#define L1SS_CAP_REGISTER 0x164
#define L1SS_CTRL1_REGISTER 0x168
#define L1SS_CTRL2_REGISTER 0x16c

/*SOC dma register*/
#define DMA_BASE_ADDR			0xFDF30000
#define DMA_SRC_ADDR(cn)		(0x814 + 0x40 * cn)
#define DMA_DES_ADDR(cn)		(0x818 + 0x40 * cn)
#define DMA_TRANSMIT_SIZE(cn)	(0x810 + 0x40 * cn)
#define DMA_CHAN_CONFIG(cn)		(0x81c + 0x40 * cn)
#define DMA_SRC_ADDR_INC		(0x1 << 31)
#define DMA_DES_ADDR_INC		(0x1 << 30)
#define DMA_SRC_BURST_LEN(len)		(((len - 1) & 0xF) << 24)
#define DMA_DES_BURST_LEN(len)		(((len - 1) & 0xF) << 20)
#define DMA_SRC_DATA_WIDTH_64BIT		(0x3 << 16)
#define DMA_DES_DATA_WIDTH_64BIT		(0x3 << 12)



#define TEST_LOWMEM_ADDR 0x38000000
#define TEST_HIMEM_ADDR 0x5A4C00000
#define TEST_BUS1_CPU_ADDR 0xF5000000
#define TEST_BUS0_CPU_ADDR 0xF4000000
#define TEST_BUS1_PCI_ADDR 0x1000000
#define TEST_BUS0_PCI_ADDR 0x0

#define TEST_PCIE_BUS_SPACE 0x1000000
#define TEST_MEM_SIZE 0x500000
#define TRANSFER_TIMES 10

#define GEN3_CTRL_REG 0x890
#define PIPE_LOOPBACK_CTRL_REG 0x8B8
#define PORT_LINK_CTRL_REG 0x710

#define LINK_CTRL2_REG 0x30


struct pcie_addr_info{
       u64 rc_phys_addr; /* RC buffer's CPU physical address */
       void __iomem *rc_virt_addr;
       u64 ep_phys_addr; /* EP buffer's CPU physical address */
       void __iomem *ep_virt_addr;

       u64 ep_in_rc_cpu_phys_addr;       /* EP buffer's PCI physical address (CPU domain) */
       void __iomem *ep_in_rc_cpu_virt_addr;
       u64 ep_in_rc_pci_phys_addr;       /* EP buffer's PCI physical address (PCI domain) */
       u64 ep_in_rc_cpu_sc_phys_addr;    /* EP sysctrl's CPU physical address */
       void __iomem *ep_in_rc_cpu_sc_virt_addr;
};

struct pcie_test_st {
	struct pci_dev *rc_dev;
	struct pci_dev *ep_dev;
	u32 is_ep_wifi;
	u64 rc_mem_addr;
	u64 ep_mem_addr;
	struct pcie_addr_info *dma_info;
	u32 wl_power;
};

struct pcie_test_st *g_test_kirin_pcie;
extern struct kirin_pcie *g_kirin_pcie;


void dump_register_init(void);

/**
 * test_enable_dbi - make it possible to access the rc configuration registers in the CDM,
 * or the ep configuration registers.
 * @flag: If flag equals 0, you can access the ep configuration registers in the CDM;
 *  If not, you can access the rc configuration registers in the CDM.
 */
void test_enable_dbi(int flag)
{
	int ret1 = 0;
	int ret2 = 0;
	ret1 = kirin_elb_readl(g_kirin_pcie, 0x0);
	ret2 = kirin_elb_readl(g_kirin_pcie, 0x4);
	if (flag) {
		ret1 = ret1 | (0x1<<21);
		ret2 = ret2 | (0x1<<21);
	} else {
		ret1 = ret1 & (~(0x1<<21));
		ret2 = ret2 & (~(0x1<<21));
	}
	kirin_elb_writel(g_kirin_pcie, ret1, 0x0);
	kirin_elb_writel(g_kirin_pcie, ret2, 0x4);

	udelay(2);
	ret1 = kirin_elb_readl(g_kirin_pcie, 0x0);
	ret2 = kirin_elb_readl(g_kirin_pcie, 0x4);

	PCIE_PR_INFO("apb register 0x0=[0x%x], 0x4=[0x%x]", ret1, ret2);
	return;
}

/**
 * set_bme - enable bus master or not.
 * @flag: If flag equals 0, bus master is disabled. If not, bus master is enabled.
 */
void set_bme(int flag)
{
	int ret = 0;
	test_enable_dbi(1);
	ret = readl(g_kirin_pcie->pp.dbi_base + PCI_COMMAND);
	if (flag) {
		PCIE_PR_INFO("Enable Bus master!!!");
		ret |= PCI_COMMAND_MASTER;
	} else {
		PCIE_PR_INFO("Disable Bus master!!!");
		ret &= ~PCI_COMMAND_MASTER;
	}
	writel(ret, g_kirin_pcie->pp.dbi_base + PCI_COMMAND);
	udelay(5);
	ret = readl(g_kirin_pcie->pp.dbi_base + PCI_COMMAND);
	PCIE_PR_INFO("register[0x4] value is [0x%x]", ret);
	test_enable_dbi(0);
	return;
}

/**
 * set_mse - enable mem space or not.
 * @flag: If flag equals 0, mem space is disabled. If not, mem space is enabled.
 */
void set_mse(int flag)
{
	int ret = 0;
	test_enable_dbi(1);
	ret = readl(g_kirin_pcie->pp.dbi_base + PCI_COMMAND);
	if (flag) {
		PCIE_PR_INFO("Enable MEM space!!!");
		ret |= PCI_COMMAND_MEMORY;
	} else {
		PCIE_PR_INFO("Disable MEM space!!!");
		ret &= ~PCI_COMMAND_MEMORY;
	}
	writel(ret, g_kirin_pcie->pp.dbi_base + PCI_COMMAND);
	udelay(5);
	ret = readl(g_kirin_pcie->pp.dbi_base + 0x4);
	PCIE_PR_INFO("register[0x4] value is [0x%x]", ret);
	test_enable_dbi(0);
	return;
}
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 13, 0))
void test_set_iomux(u32 write_flag, u32 value)
{
	if (write_flag)
		atfd_hisi_service_pcie_smc(ACCESS_REGISTER_FN_SUB_ID_PCIE_IO_SET, WR_FLAG, value, 0x0);
	else
		atfd_hisi_service_pcie_smc(ACCESS_REGISTER_FN_SUB_ID_PCIE_IO_SET, RD_FLAG, 0x0, 0x0);
	return;
}
#endif
/**
 * show_link_state - show the rc link state.
 */
int show_link_state(void)
{
	unsigned int val = 0;

	val = kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_STATE4_ADDR);

	PCIE_PR_INFO("Register 0x410 value [0x%x]", val);

	val = val & 0x3f;

	switch (val) {
	case 0x11:
		PCIE_PR_INFO("L-state: L0");
		break;
	case 0x12:
		PCIE_PR_INFO("L-state: L0S");
		break;
	case 0x14:
		PCIE_PR_INFO("L-state: L1");
		break;
	case 0x15:
		PCIE_PR_INFO("L-state: L2");
		break;
	default: {
			val = kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_STATE5_ADDR);
			val = val & 0xC000;
			if (val == 0x4000)
				PCIE_PR_ERR("L-state: L1.1");
			else if (val == 0xC000)
				PCIE_PR_ERR("L-state: L1.2");
			else
				PCIE_PR_ERR("other state");
			break;
		}
	}
	return val;
}
/**
 * show_trans_rate - show data transmission rate.
 * @begin_time: The time before transmission;
 * @end_time: The time after transmission;
 * @size: The size of datas.
 */
void show_trans_rate(u64 begin_time, u64 end_time, int size)
{
	u64 time_count = end_time - begin_time;
	long int rate = ((size * 8 * 100 * TRANSFER_TIMES)/time_count);

	PCIE_PR_INFO("Transferring Rate is :[%ld] Bit/s", rate);
}

void show_pci_dev_state(void)
{
	int pm;
	u32 reg_value = 0;
	struct pci_dev *rc_dev = g_test_kirin_pcie->rc_dev;
	struct pci_dev *ep_dev = g_test_kirin_pcie->ep_dev;
	struct pcie_port *pp = &g_kirin_pcie->pp;

	if (rc_dev == NULL || ep_dev == NULL) {
		PCIE_PR_ERR("Failed to get Device");
		return;
	}

	pm = pci_find_capability(rc_dev, PCI_CAP_ID_PM);
	if (pm) {
		kirin_pcie_rd_own_conf(pp, pm + PCI_PM_CTRL, 4, &reg_value);
		PCIE_PR_INFO("rc: pci_pm_ctrl reg value = [0x%x]", reg_value & 0x3);
	}

	pm = pci_find_capability(ep_dev, PCI_CAP_ID_PM);
	if (pm) {
		pci_read_config_dword(ep_dev, pm + PCI_PM_CTRL, &reg_value);
		PCIE_PR_INFO("EP: pci_pm_ctrl reg value = [0x%x]", reg_value & 0x3);
	}

	return;
}

void config_aspm_l0s_l1(enum link_aspm_state aspm_state)
{
	struct pci_dev *rc_dev = g_test_kirin_pcie->rc_dev;
	struct pci_dev *ep_dev = g_test_kirin_pcie->ep_dev;

	if (ep_dev == NULL || rc_dev == NULL) {
		PCIE_PR_ERR("Failed to get PCI Device");
		return;
	}

	pcie_capability_clear_and_set_word(rc_dev, PCI_EXP_LNKCTL,
				   PCI_EXP_LNKCTL_ASPMC, aspm_state);
	pcie_capability_clear_and_set_word(ep_dev, PCI_EXP_LNKCTL,
				   PCI_EXP_LNKCTL_ASPMC, aspm_state);
	return;
}


void config_enable_ep_l1ss(void)
{
	int ret = 0;

	ret = kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_CTRL7_ADDR);
	ret |= (0x1<<10);
	kirin_elb_writel(g_kirin_pcie, ret, SOC_PCIECTRL_CTRL7_ADDR);


	ret = kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_CTRL8_ADDR);
	ret |= (0x1<<3);
	kirin_elb_writel(g_kirin_pcie, ret, SOC_PCIECTRL_CTRL8_ADDR);

	ret = kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_CTRL1_ADDR);
	ret |= (0x1<<23);
	kirin_elb_writel(g_kirin_pcie, ret, SOC_PCIECTRL_CTRL1_ADDR);

	ret = kirin_phy_readl(g_kirin_pcie, SOC_PCIEPHY_CTRL0_ADDR);
	ret |= 0xE;
	kirin_phy_writel(g_kirin_pcie, ret, SOC_PCIEPHY_CTRL0_ADDR);
}


void config_enable_l1ss(enum l1ss_ctrl_state state)
{
	unsigned int rc_val = 0;
	unsigned int ep_val = 0;
	unsigned int pm = 0;
	int ret = 0;
	struct pci_dev *ep_dev = g_test_kirin_pcie->ep_dev;
	struct pci_dev *rc_dev = g_test_kirin_pcie->rc_dev;
	struct pcie_port *pp = &g_kirin_pcie->pp;


	config_aspm_l0s_l1(ASPM_L1);

	if (rc_dev == NULL || ep_dev == NULL) {
		PCIE_PR_ERR("Failed to get Device");
		return;
	}


	kirin_pcie_rd_own_conf(pp, 0x168, 4, &rc_val);
	if (state == L1SS_CLOSE)
		rc_val &= ~0xF;
	else
		rc_val |= state;
	kirin_pcie_wr_own_conf(pp, 0x168, 4, rc_val);

	pm = pci_find_capability(rc_dev, PCI_CAP_ID_EXP);
	if (pm) {
		kirin_pcie_rd_own_conf(pp, pm + 0x28, 4, &rc_val);
		rc_val |= (0x1 << 10);
		kirin_pcie_wr_own_conf(pp, pm + 0x28, 4, rc_val);
	}

	ret = kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_CTRL7_ADDR);
	ret |= (0x1<<10);
	kirin_elb_writel(g_kirin_pcie, ret, SOC_PCIECTRL_CTRL7_ADDR);

	ret = kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_CTRL8_ADDR);
	ret |= (0x1<<3);
	kirin_elb_writel(g_kirin_pcie, ret, SOC_PCIECTRL_CTRL8_ADDR);

	if (g_test_kirin_pcie->is_ep_wifi) {
		pci_read_config_dword(ep_dev, 0xbc, &ep_val);
		ep_val &= ~0x3;
		ep_val |= 0x142;
		pci_write_config_dword(ep_dev, 0xbc, ep_val);
		/*Read register to check*/
		ep_val = 0;
		pci_read_config_dword(ep_dev, 0xbc, &ep_val);
		PCIE_PR_INFO("WIFI CFG Register[0xbc] value=[0x%x] ", ep_val);

		pci_read_config_dword(ep_dev, 0x248, &ep_val);
		if (state == L1SS_CLOSE)
			ep_val &= ~0xf;
		else
			ep_val |= state;
		pci_write_config_dword(ep_dev, 0x248, ep_val);

		pci_read_config_dword(ep_dev, 0x24c, &ep_val);
		ep_val &= ~0xFF;
		ep_val |= 0x61;
		pci_write_config_dword(ep_dev, 0x24c, ep_val);

		pci_read_config_dword(ep_dev, 0xD4, &ep_val);
		ep_val |= 0x1 << 10;
		pci_write_config_dword(ep_dev, 0xD4, ep_val);
		pci_write_config_dword(ep_dev, 0x1B4, 0x10031003);

		pm = pci_find_capability(ep_dev, PCI_CAP_ID_EXP);
		if (pm) {
			pci_read_config_dword(ep_dev, pm + 0x28, &ep_val);
			ep_val |= (0x1 << 10);
			pci_write_config_dword(ep_dev, pm + 0x28, ep_val);
		}
	} else {
		pci_read_config_dword(ep_dev, 0x164, &ep_val);
		ep_val |= 0x1f;
		pci_write_config_dword(ep_dev, 0x164, ep_val);
		pci_read_config_dword(ep_dev, 0x168, &ep_val);
		if (state == L1SS_CLOSE)
			ep_val &= ~0xF;
		else
			ep_val |= state;
		pci_write_config_dword(ep_dev, 0x168, ep_val);
		pm = pci_find_capability(ep_dev, PCI_CAP_ID_EXP);
		if (pm) {
			pci_read_config_dword(ep_dev, pm + 0x28, &ep_val);
			ep_val |= (0x1 << 10);
			pci_write_config_dword(ep_dev, pm + 0x28, ep_val);
		}
	}


	ret = kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_CTRL1_ADDR);
	ret |= (0x1<<23);
	kirin_elb_writel(g_kirin_pcie, ret, SOC_PCIECTRL_CTRL1_ADDR);

	ret = kirin_phy_readl(g_kirin_pcie, SOC_PCIEPHY_CTRL0_ADDR);
	ret |= 0xE;
	kirin_elb_writel(g_kirin_pcie, ret, SOC_PCIEPHY_CTRL0_ADDR);

	mdelay(500);
	PCIE_PR_INFO("apb register[0x414] = [0x%x]", kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_STATE5_ADDR));

	return;
}

void config_disable_l1ss(void)
{
	unsigned int val = 0;
	int ret = 0;

	ret = kirin_pcie_rd_own_conf(&g_kirin_pcie->pp, L1SS_CTRL1_REGISTER, 4, &val);
	val &= ~0xf;
	kirin_pcie_wr_own_conf(&g_kirin_pcie->pp, L1SS_CTRL1_REGISTER, 4, val);
	udelay(1);

}


/**
* config_rc_l23 - config rc's l2 or l3 link state;
* @l3_flag: 0----rc link state is configured to be l2 ,
*               others----rc link state is configured to be l3.
*/
int config_rc_l23(int l3_flag)
{
	unsigned int ret = 0;
	struct pci_dev *rc_dev = g_test_kirin_pcie->rc_dev;
	struct pci_dev *ep_dev = g_test_kirin_pcie->ep_dev;
	struct pcie_port *pp = &g_kirin_pcie->pp;
	u32 value = 0;
	u32 pm = 0;

	if (ep_dev == NULL || rc_dev == NULL) {
		PCIE_PR_ERR("Failed to get Device");
		return -1;
	}

	/*EP wake signal Select*/
	ret = kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_CTRL12_ADDR);
	ret |= (0x3<<12);
	kirin_elb_writel(g_kirin_pcie, ret, SOC_PCIECTRL_CTRL12_ADDR);

	/*Enable PME*/
	/*RC */
	pm = pci_find_capability(rc_dev, PCI_CAP_ID_EXP);
	if (pm) {
		kirin_pcie_rd_own_conf(pp, pm + PCI_PM_CTRL, 4, &value);
		value |= 0x103;
		kirin_pcie_wr_own_conf(pp, pm + PCI_PM_CTRL, 4, value);
	}
	/*EP devices*/
	pm = pci_find_capability(ep_dev, PCI_CAP_ID_EXP);
	if (pm) {
		ret = pci_read_config_dword(ep_dev, pm + PCI_PM_CTRL, &value);
		value |= (0x1 << 8);
		ret = pci_write_config_dword(ep_dev, pm + PCI_PM_CTRL, value);
	}

	/*Change PHY reset signal to software*/
	ret = kirin_phy_readl(g_kirin_pcie, SOC_PCIEPHY_CTRL1_ADDR);
	ret |= (0x1<<17);
	kirin_phy_writel(g_kirin_pcie, ret, SOC_PCIEPHY_CTRL1_ADDR);

	/*send PME_turn_off*/
	ret = kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_CTRL7_ADDR);
	ret |= (0x5<<8);
	ret &= ~(0x1<<8);
	kirin_elb_writel(g_kirin_pcie, ret, SOC_PCIECTRL_CTRL7_ADDR);


	ret = pci_save_state(rc_dev);
	if (ret) {
		PCIE_PR_ERR("Failed to save state of RC.");
		return ret;
	}

	ret = pci_save_state(ep_dev);
	if (ret) {
		PCIE_PR_ERR("Failed to save state of EP.");
		return ret;
	}

	PCIE_PR_INFO("Waiting for PME_TO_ACK MSG");
	do {
		udelay(1);
		ret = kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_STATE1_ADDR);
	} while ((ret & (0x1<<16)) != (0x1 << 16));

	PCIE_PR_INFO("Getton PME_TO_ACK MSG");

	if (l3_flag)
		kirin_pcie_mtcmos_ctrl(g_kirin_pcie, 0);

	return 0;
}

int config_ep_l23(void)
{
	unsigned int ret = 0;

	/*EP wake signal Select*/
	ret = kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_CTRL12_ADDR);
	ret |= (0x1<<12);
	ret &= ~(0x1<<13);
	kirin_elb_writel(g_kirin_pcie, ret, SOC_PCIECTRL_CTRL12_ADDR);

	test_enable_dbi(1);
	ret = kirin_pcie_rd_own_conf(&g_kirin_pcie->pp, 0x44, 4, &ret);
	ret |= (0x1 << 8);
	kirin_pcie_wr_own_conf(&g_kirin_pcie->pp, 0x44, 4, ret);


	/*Change PHY reset signal to software*/
	ret = kirin_elb_readl(g_kirin_pcie, SOC_PCIEPHY_CTRL1_ADDR);
	ret |= (0x1<<17);
	kirin_elb_writel(g_kirin_pcie, ret, SOC_PCIEPHY_CTRL1_ADDR);

	ret = kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_CTRL7_ADDR);
	ret |= (0x1<<10);
	ret |= (0x1<<2);
	kirin_elb_writel(g_kirin_pcie, ret, SOC_PCIECTRL_CTRL7_ADDR);

	return 0;
}


void config_exist_l23(void)
{
	int ret = 0;
	u32 value = 0;
	u32 pm = 0;

	PCIE_PR_INFO("Force Exist L23");

	ret = kirin_phy_readl(g_kirin_pcie, SOC_PCIEPHY_CTRL1_ADDR);
	ret &= ~(0x1<<17);
	kirin_phy_writel(g_kirin_pcie, ret, SOC_PCIEPHY_CTRL1_ADDR);

	ret = kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_CTRL7_ADDR);
	ret |= (0x1<<9);
	kirin_phy_writel(g_kirin_pcie, ret, SOC_PCIECTRL_CTRL7_ADDR);

	if (g_kirin_pcie->ep_flag) {
		kirin_pcie_power_on(&g_kirin_pcie->pp, 1);
		/*disable prepare to entry L23*/
		ret = kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_CTRL7_ADDR);
		ret &= ~(0x1<<2);
		kirin_phy_writel(g_kirin_pcie, ret, SOC_PCIECTRL_CTRL7_ADDR);
		udelay(2);
		return;
	} else {
		struct pci_dev *rc_dev = g_test_kirin_pcie->rc_dev;
		struct pci_dev *ep_dev = g_test_kirin_pcie->ep_dev;

		if (ep_dev == NULL || rc_dev == NULL) {
			PCIE_PR_ERR("Failed to get Device");
			return;
		}

		/*disable PME*/
		/*RC */
		pm = pci_find_capability(rc_dev, PCI_CAP_ID_EXP);
		if (pm) {
			ret = kirin_pcie_rd_own_conf(&g_kirin_pcie->pp, pm + PCI_PM_CTRL, 4, &value);
			value &= ~0x103;
			ret = kirin_pcie_wr_own_conf(&g_kirin_pcie->pp, pm + PCI_PM_CTRL, 4, value);
		}
		/*EP devices*/
		pm = pci_find_capability(ep_dev, PCI_CAP_ID_EXP);
		if (pm) {
			ret = pci_read_config_dword(ep_dev, pm + PCI_PM_CTRL, &value);
			value &= ~(0x1 << 8);
			ret = pci_write_config_dword(ep_dev, pm + PCI_PM_CTRL, value);
		}

		/*restore pci register*/
		pci_restore_state(rc_dev);
		pci_restore_state(ep_dev);

		ret = kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_STATE1_ADDR);
		PCIE_PR_INFO("PM current state reg value is [0x%x]", ret);
	}
	return;
}


void kirin_pcie_outbound_atu(int index,
		int type, u64 cpu_addr, u64 pci_addr, u32 size)
{
	struct pcie_port *pp = &(g_kirin_pcie->pp);
	char *dbi_base = pp->dbi_base;

	kirin_pcie_writel_rc(pp, PCIE_ATU_REGION_OUTBOUND | index,
			  dbi_base + PCIE_ATU_VIEWPORT);
	kirin_pcie_writel_rc(pp, lower_32_bits(cpu_addr), dbi_base + PCIE_ATU_LOWER_BASE);
	kirin_pcie_writel_rc(pp, upper_32_bits(cpu_addr), dbi_base + PCIE_ATU_UPPER_BASE);
	kirin_pcie_writel_rc(pp, lower_32_bits(cpu_addr + size - 1),
			  dbi_base + PCIE_ATU_LIMIT);
	kirin_pcie_writel_rc(pp, lower_32_bits(pci_addr), dbi_base + PCIE_ATU_LOWER_TARGET);
	kirin_pcie_writel_rc(pp, upper_32_bits(pci_addr), dbi_base + PCIE_ATU_UPPER_TARGET);
	kirin_pcie_writel_rc(pp, type, dbi_base + PCIE_ATU_CR1);
	kirin_pcie_writel_rc(pp, PCIE_ATU_ENABLE, dbi_base + PCIE_ATU_CR2);
}

void kirin_pcie_inbound_atu(int index,
		int type, u64 cpu_addr, u64 pci_addr, u32 size)
{
	void __iomem *dbi_base = g_kirin_pcie->pp.dbi_base;
	struct pcie_port *pp = &g_kirin_pcie->pp;

	kirin_pcie_writel_rc(pp, PCIE_ATU_REGION_INBOUND | index,
			  dbi_base + PCIE_ATU_VIEWPORT);
	kirin_pcie_writel_rc(pp, lower_32_bits(pci_addr), dbi_base + PCIE_ATU_LOWER_BASE);
	kirin_pcie_writel_rc(pp, upper_32_bits(pci_addr), dbi_base + PCIE_ATU_UPPER_BASE);
	kirin_pcie_writel_rc(pp, lower_32_bits(pci_addr + size - 1),
			  dbi_base + PCIE_ATU_LIMIT);
	kirin_pcie_writel_rc(pp, lower_32_bits(cpu_addr), dbi_base + PCIE_ATU_LOWER_TARGET);
	kirin_pcie_writel_rc(pp, upper_32_bits(cpu_addr), dbi_base + PCIE_ATU_UPPER_TARGET);
	kirin_pcie_writel_rc(pp, type, dbi_base + PCIE_ATU_CR1);
	kirin_pcie_writel_rc(pp, PCIE_ATU_ENABLE, dbi_base + PCIE_ATU_CR2);

	return;
}

void rc_read_wifi_cfg(void)
{
	unsigned int index = 0;
	u64 ep_vaddr_in_cpu = 0;
	u32 busdev;
	struct pci_dev *ep_dev = pci_get_device(0x14e4, 0x4355, NULL);

	if (ep_dev == NULL) {
		PCIE_PR_ERR("Failed to get EP Device");
		return;
	}

	busdev = 0x1400000;
	PCIE_PR_INFO("busdev addr is [0x%x] ", busdev);

	ep_vaddr_in_cpu = pci_resource_start(ep_dev, 0);
	PCIE_PR_INFO("ep addr in cpu physical is [0x%llx]", ep_vaddr_in_cpu);

	kirin_pcie_outbound_atu(PCIE_ATU_REGION_INDEX0, PCIE_ATU_TYPE_CFG0, ep_vaddr_in_cpu, busdev, 0x1000);

	ep_vaddr_in_cpu = (__force u64)ioremap(ep_vaddr_in_cpu, TEST_MEM_SIZE);
	PCIE_PR_INFO("ep addr in cpu virtual is [0x%llx]", ep_vaddr_in_cpu);

	for (index = 0; index < 0x1000; index = index + 4)
		PCIE_PR_INFO("cfg register: offset[0x%x] = [0x%x]", index, readl((char *)ep_vaddr_in_cpu+index));

	return;
}

void rc_write_wifi_cfg(unsigned int offset, unsigned int val)
{
	u64 ep_vaddr_in_cpu = 0;
	u32 busdev;
	struct pci_dev *ep_dev = pci_get_device(0x14e4, 0x4355, NULL);

	if (ep_dev == NULL) {
		PCIE_PR_ERR("Failed to get EP Device");
		return;
	}

	busdev = 0x1400000;
	PCIE_PR_INFO("busdev addr is [0x%x] ", busdev);

	ep_vaddr_in_cpu = pci_resource_start(ep_dev, 0);
	PCIE_PR_INFO("ep addr in cpu physical is [0x%llx]", ep_vaddr_in_cpu);

	kirin_pcie_outbound_atu(PCIE_ATU_REGION_INDEX0, PCIE_ATU_TYPE_CFG0, ep_vaddr_in_cpu, busdev, 0x1000);

	ep_vaddr_in_cpu = (__force u64)ioremap(ep_vaddr_in_cpu, TEST_MEM_SIZE);
	PCIE_PR_INFO("ep addr in cpu virtual is [0x%llx]", ep_vaddr_in_cpu);

	writel(val, ep_vaddr_in_cpu + offset);
	PCIE_PR_INFO("cfg register: offset[0x%x] = [0x%x]", offset, readl((char *)ep_vaddr_in_cpu + offset));

	return;
}


void set_ep_cfg_inbound(void)
{
	kirin_pcie_inbound_atu(PCIE_ATU_REGION_INDEX1, PCIE_ATU_TYPE_CFG0, TEST_BUS0_CPU_ADDR, TEST_BUS1_PCI_ADDR, 0x1000);
}

void rc_read_ep_cfg(void)
{
	unsigned int index = 0;
	u64 ep_vaddr_in_cpu = 0;
	u64 busdev;

	struct pci_dev *ep_dev = g_test_kirin_pcie->ep_dev;

	if (ep_dev == NULL) {
		PCIE_PR_ERR("Failed to get EP Device");
		return;
	}

	busdev = TEST_BUS1_PCI_ADDR;
	ep_vaddr_in_cpu = pci_resource_start(ep_dev, 0);

	PCIE_PR_INFO("ep addr in cpu physical is [0x%llx]", busdev);
	kirin_pcie_outbound_atu(PCIE_ATU_REGION_INDEX0, PCIE_ATU_TYPE_CFG0, ep_vaddr_in_cpu, busdev, 0x1000);
	ep_vaddr_in_cpu = (__force u64)ioremap(ep_vaddr_in_cpu, 0x1000);
	PCIE_PR_INFO("ep addr in cpu virtual is [0x%llx]", ep_vaddr_in_cpu);

	test_enable_dbi(0);
	for (index = 0; index < 0x100; index += 4)
		PCIE_PR_INFO("cfg register: offset[%d] = [0x%x]", index, readl(ep_vaddr_in_cpu + index));

	return;
}

void rc_read_ep_mem(void)
{
	unsigned int index = 0;
	u64 cpu_addr = TEST_BUS1_CPU_ADDR;

	PCIE_PR_INFO("ep addr in cpu physical is [0x%llx], size is [0x%x]", cpu_addr, TEST_MEM_SIZE);

	kirin_pcie_outbound_atu(PCIE_ATU_REGION_INDEX0, PCIE_ATU_TYPE_MEM, cpu_addr, TEST_BUS1_PCI_ADDR, TEST_MEM_SIZE);

	cpu_addr = (__force u64)ioremap(cpu_addr, TEST_MEM_SIZE);

	for (index = 0; index < 0x100; index = index + 4)
		PCIE_PR_INFO("ep mem: offset[0x%x] = [0x%x]", index, readl((char *)cpu_addr+index));

	return;
}

/**
** RC springs data-transfer between RC and EP
**/
void set_ep_mem_inbound(unsigned int hi_addr_flag)
{
	u64 busdev, cpu_addr, temp_addr;
	set_bme(1);
	set_mse(1);
	busdev = TEST_BUS1_PCI_ADDR;

	temp_addr = virt_to_phys(g_test_kirin_pcie->ep_mem_addr);
	cpu_addr = g_test_kirin_pcie->ep_mem_addr;

	PCIE_PR_INFO("inbound pci_add [0x%llx] to cpu_addr[0x%llx]", busdev, temp_addr);
	kirin_pcie_inbound_atu(PCIE_ATU_REGION_INDEX1, PCIE_ATU_TYPE_MEM, temp_addr, busdev, TEST_MEM_SIZE);

	memset((void *)cpu_addr, 0xaa, TEST_MEM_SIZE);

}

void read_ep_addr(void)
{
	u32 index = 0;
	u32 val = 0;

	if (g_test_kirin_pcie->ep_mem_addr)
		for (index = 0; index < 0x100; index += 4) {
			val = readl(g_test_kirin_pcie->ep_mem_addr+index);
			PCIE_PR_INFO("index[0x%x], value=[0x%x]", index, val);
		}
	return;
}

void rc_write_ep_mem(unsigned int size)
{
	u64 cpu_addr = 0x0;
	u64 begin_time = 0;
	u64 end_time = 0;
	int index = 0;
	void *data = kzalloc(0x1000*sizeof(int), GFP_KERNEL);

	if (data == NULL)
		return;
	memset(data, 0xef, 0x1000);

	if (size > TEST_MEM_SIZE || size == 0)
		size = TEST_MEM_SIZE;

	cpu_addr = TEST_BUS1_CPU_ADDR;
	PCIE_PR_INFO("EP in RC cpu Addr is [0x%llx]", cpu_addr);

	kirin_pcie_outbound_atu(PCIE_ATU_REGION_INDEX1, PCIE_ATU_TYPE_MEM, cpu_addr, TEST_BUS1_PCI_ADDR, TEST_MEM_SIZE);

	cpu_addr = (__force u64)ioremap_nocache(cpu_addr, TEST_MEM_SIZE);

	PCIE_PR_INFO("Writing 0xef to EP mem");
	begin_time = jiffies;

	for (index = 0; index < TRANSFER_TIMES; index++)
		do {
			if (size < 0x1000) {
				memcpy((void *)cpu_addr, data, size);
				break;
			} else {
				memcpy((void *)cpu_addr, data, 0x1000);
				cpu_addr += 0x1000;
				size -= 0x1000;
			}
		} while (size > 0);

	end_time = jiffies;

	show_trans_rate(begin_time, end_time, size*sizeof(char));

	return;
}

/**
** EP springs data-transfer between RC and EP
**/
void set_rc_mem_inbound(unsigned int hi_addr_flag)
{
	unsigned int index = 0;
	u64 busdev, cpu_addr, temp_addr;
	u32 val = 0;

	set_bme(1);
	set_mse(1);
	busdev = TEST_BUS0_PCI_ADDR;

	temp_addr = virt_to_phys(g_test_kirin_pcie->rc_mem_addr);
	cpu_addr = g_test_kirin_pcie->rc_mem_addr;

	PCIE_PR_INFO("inbound pci_add [0x%llx] to cpu_addr[0x%llx]", busdev, temp_addr);
	kirin_pcie_inbound_atu(PCIE_ATU_REGION_INDEX0, PCIE_ATU_TYPE_MEM, temp_addr, busdev, TEST_MEM_SIZE);

	memset((void *)cpu_addr, 0xbb, TEST_MEM_SIZE);

	for (index = 0; index < 0x100; index += 4) {
		val = readl((char *)cpu_addr+index);
		PCIE_PR_INFO("Crg register[0x%x], value=[0x%x]", index, val);
	}
	return;

}


void read_rc_addr(void)
{
	u32 index = 0;
	u32 val = 0;

	if (g_test_kirin_pcie->rc_mem_addr)
		for (index = 0; index < 0x100; index += 4) {
			val = readl(g_test_kirin_pcie->rc_mem_addr+index);
			PCIE_PR_INFO("offset[0x%x], value=[0x%x]", index, val);
		}
}

void ep_read_rc_mem(void)
{
	u64 cpu_addr = 0x0;
	u32 index = 0;

	cpu_addr = TEST_BUS0_CPU_ADDR;
	kirin_pcie_outbound_atu(PCIE_ATU_REGION_INDEX1, PCIE_ATU_TYPE_MEM, cpu_addr, 0x0, TEST_MEM_SIZE);

	cpu_addr = (__force u64)ioremap_nocache(cpu_addr, TEST_MEM_SIZE);

	for (index = 0; index < 0x100; index += 4)
		PCIE_PR_INFO("index[0x%x], src_data:[0x%x]", index, readl(cpu_addr + index));

	return;
}

void ep_write_rc_mem(unsigned int size)
{
	u64 cpu_addr = 0x0;
	u64 begin_time = 0;
	u64 end_time = 0;
	int index = 0;
	void *data = kzalloc(0x1000*sizeof(int), GFP_KERNEL);

	if (data == NULL)
		return;
	memset(data, 0xbc, 0x1000);

	if (size > TEST_MEM_SIZE || size <= 0)
		size = TEST_MEM_SIZE;

	cpu_addr = TEST_BUS0_CPU_ADDR;
	kirin_pcie_outbound_atu(PCIE_ATU_REGION_INDEX1, PCIE_ATU_TYPE_MEM, cpu_addr, 0x0, TEST_MEM_SIZE);

	cpu_addr = (__force u64)ioremap(cpu_addr, TEST_MEM_SIZE);

	PCIE_PR_INFO("Writing 0xbc to RC mem");

	begin_time = jiffies;

	for (index = 0; index < TRANSFER_TIMES; index++)
		do {
			if (size < 0x1000) {
				memcpy((void *)cpu_addr, data, size);
				break;
			} else {
				memcpy((void *)cpu_addr, data, 0x1000);
				cpu_addr += 0x1000;
				size -= 0x1000;
			}
		} while (size > 0);

	end_time = jiffies;

	show_trans_rate(begin_time, end_time, size*sizeof(char));

}


void test_access_phy_register(void)
{
	unsigned int index = 0;

	PCIE_PR_INFO("List PCIePHY Register:");

	for (index = 0; index < 10; index += 4)
		PCIE_PR_INFO("Register[0x%x]----Value[0x%x]", index, readl(g_kirin_pcie->phy_base+index));

	return;
}

void test_access_core_register(void)
{
	unsigned int index = 0;
	PCIE_PR_INFO("List PCIeCORE Register:");

	for (index = 0; index < 20; index += 4)
		PCIE_PR_INFO("Register[0x%x]----Value[0x%x]", index, readl(g_kirin_pcie->apb_base+index));

	return;
}

void test_access_apb_register(void)
{
	int ret = 0;
	unsigned int index = 0;
	/*PCIePHY apb*/
	PCIE_PR_INFO("PCIePHY apb Read and Write testcase");
	test_enable_dbi(1);
	for (index = 0; index < 100; index += 4) {
		ret = readl(g_kirin_pcie->pp.dbi_base + index);
		PCIE_PR_ERR("config register[0x%x] value is [0x%x]", index, ret);
	}

	test_enable_dbi(0);
	return;
}

void ltssm_enable(int yes)
{
	u32 val = 0;
	if (yes) {
		val = kirin_elb_readl(g_kirin_pcie,  SOC_PCIECTRL_CTRL7_ADDR);
		val |= (0x1 << 11);
		kirin_elb_writel(g_kirin_pcie, val, SOC_PCIECTRL_CTRL7_ADDR);
	} else {
		val = kirin_elb_readl(g_kirin_pcie,  SOC_PCIECTRL_CTRL7_ADDR);
		val &= ~(0x1 << 11);
		kirin_elb_writel(g_kirin_pcie, val, SOC_PCIECTRL_CTRL7_ADDR);
	}

	return;
}
int test_compliance(u32 entry)
{
	struct pci_dev *rc_dev = NULL;
	int pm = 0;
	u32 val = 0;

	rc_dev = g_test_kirin_pcie->rc_dev;
	if (!rc_dev) {
		PCIE_PR_ERR("Failed to get RC Device");
		return -1;
	}

	pm = pci_find_capability(rc_dev, PCI_CAP_ID_EXP);
	if (!pm) {
		PCIE_PR_ERR("Failed to get PCI Express capability !");
		return -1;
	}
	pci_read_config_dword(rc_dev, pm + LINK_CTRL2_REG, &val);
	if (entry)
		val |= (0x1 << 4);
	else
		val &= ~(0x1 << 4);
	pci_write_config_dword(rc_dev, pm + LINK_CTRL2_REG, val);
	if (!entry) {
		ltssm_enable(0);
		ltssm_enable(1);
	}

	show_link_state();
	return 0;
}

void test_wlan_on(int on)
{
	int ret = 0;
	ret = gpio_request(g_test_kirin_pcie->wl_power, "wl_on");
	if (ret < 0) {
		PCIE_PR_ERR("Can't request gpio");
		return;
	}
	if (on) {
		PCIE_PR_INFO("Power on Wlan");
		gpio_direction_output(g_test_kirin_pcie->wl_power, 1);
	} else {
		PCIE_PR_INFO("Power down Wlan");
		gpio_direction_output(g_test_kirin_pcie->wl_power, 0);
	}
	gpio_free(g_test_kirin_pcie->wl_power);

	return;
}
void test_entry_loopback(u32 local)
{
	struct pcie_port *pp = &g_kirin_pcie->pp;
	u32 val = 0;

	kirin_pcie_rd_own_conf(pp, PORT_LINK_CTRL_REG, 4, &val);
	val |= (0x1 << 2);
	kirin_pcie_wr_own_conf(pp, PORT_LINK_CTRL_REG, 4, val);

	if (local) {
		kirin_pcie_rd_own_conf(pp, GEN3_CTRL_REG, 4, &val);
		val |= (0x1 << 16);
		kirin_pcie_wr_own_conf(pp, GEN3_CTRL_REG, 4, val);

		kirin_pcie_rd_own_conf(pp, PIPE_LOOPBACK_CTRL_REG, 4, &val);
		val |= (0x1 << 31);
		kirin_pcie_wr_own_conf(pp, PIPE_LOOPBACK_CTRL_REG, 4, val);
	}

	show_link_state();

	return;
}

void test_exit_loopback(u32 local)
{
	struct pcie_port *pp = &g_kirin_pcie->pp;
	u32 val = 0;

	kirin_pcie_rd_own_conf(pp, PORT_LINK_CTRL_REG, 4, &val);
	val &= ~(0x1 << 2);
	kirin_pcie_wr_own_conf(pp, PORT_LINK_CTRL_REG, 4, val);

	if (local) {
		kirin_pcie_rd_own_conf(pp, GEN3_CTRL_REG, 4, &val);
		val &= ~(0x1 << 16);
		kirin_pcie_wr_own_conf(pp, GEN3_CTRL_REG, 4, val);

		kirin_pcie_rd_own_conf(pp, PIPE_LOOPBACK_CTRL_REG, 4, &val);
		val &= ~(0x1 << 31);
		kirin_pcie_wr_own_conf(pp, PIPE_LOOPBACK_CTRL_REG, 4, val);
	}

	show_link_state();
	return;
}


/***
* RC acess EP through SoC DMA
* test_dma_rx - rc read ep by dma;
* @size: 0-15bit----the size of an array;
*           16-31bit----the number of arrays;
*/
void test_dma_rx(u32 size)
{
	u64 begin_time = 0;
	int i = 0;
	int chan = 1;
	u32 ch_en = 0;
	u32 trans_size = 0;
	void __iomem *dma_base_addr = ioremap_nocache(DMA_BASE_ADDR, 0x1000);

	g_test_kirin_pcie->dma_info->ep_in_rc_cpu_phys_addr = TEST_BUS1_CPU_ADDR;
	g_test_kirin_pcie->dma_info->ep_in_rc_cpu_virt_addr = ioremap_nocache(g_test_kirin_pcie->dma_info->ep_in_rc_cpu_phys_addr, TEST_MEM_SIZE);
	g_test_kirin_pcie->dma_info->rc_phys_addr = 0x39000000;
	g_test_kirin_pcie->dma_info->rc_virt_addr = phys_to_virt(g_test_kirin_pcie->dma_info->rc_phys_addr);

	if (((size & 0xFFFF) == 0) || ((size & 0xFFFF0000) == 0xFFFF0000))	{
		PCIE_PR_ERR("input size error!");
		return;
	}
	trans_size = ((size & 0xFFFF) * (((size & 0xFFFF0000) >> 16) + 1));

	for (i = 0; i < trans_size; i += 4)
		writel(i + 2, g_test_kirin_pcie->dma_info->ep_in_rc_cpu_virt_addr + i);
	PCIE_PR_ERR("####!");
	writel(g_test_kirin_pcie->dma_info->ep_in_rc_cpu_phys_addr, dma_base_addr + DMA_SRC_ADDR(chan));
	writel(g_test_kirin_pcie->dma_info->rc_phys_addr, dma_base_addr + DMA_DES_ADDR(chan));
	writel(size, dma_base_addr + DMA_TRANSMIT_SIZE(chan));

	begin_time = jiffies;
	ch_en = readl(dma_base_addr + DMA_CHAN_CONFIG(chan));
	ch_en |= (DMA_SRC_ADDR_INC | DMA_DES_ADDR_INC);
	ch_en |= (DMA_SRC_BURST_LEN(16) | DMA_DES_BURST_LEN(16));
	ch_en |= (DMA_SRC_DATA_WIDTH_64BIT | DMA_DES_DATA_WIDTH_64BIT);
	ch_en &= ~(0x1 << 2);

	writel(ch_en, dma_base_addr + DMA_CHAN_CONFIG(chan));
	PCIE_PR_INFO("rx begin jiffies = %lld", begin_time);
}
/***
* RC acess EP through SoC DMA
* test_dma_tx - rc write ep by dma;
* @size: 0-15bit----the size of an array;
*           16-31bit----the number of arrays;
*/
void test_dma_tx(u32 size)
{
	u64 begin_time = 0;
	int i = 0;
	int chan = 1;
	u32 ch_en = 0;
	u32 trans_size = 0;
	void __iomem *dma_base_addr = ioremap_nocache(DMA_BASE_ADDR, 0x1000);
	g_test_kirin_pcie->dma_info->ep_in_rc_cpu_phys_addr = TEST_BUS1_CPU_ADDR;
	g_test_kirin_pcie->dma_info->ep_in_rc_cpu_virt_addr = ioremap_nocache(g_test_kirin_pcie->dma_info->ep_in_rc_cpu_phys_addr, TEST_MEM_SIZE);
	g_test_kirin_pcie->dma_info->rc_phys_addr = 0x39000000;
	g_test_kirin_pcie->dma_info->rc_virt_addr = phys_to_virt(g_test_kirin_pcie->dma_info->rc_phys_addr);
	if (((size & 0xFFFF) == 0) || ((size & 0xFFFF0000) == 0xFFFF0000))	{
		PCIE_PR_ERR("input size error!");
		return;
	}
	trans_size = ((size & 0xFFFF) * (((size & 0xFFFF0000) >> 16) + 1));

	for (i = 0; i < trans_size; i += 4)
		writel(i, g_test_kirin_pcie->dma_info->rc_virt_addr + i);

	writel(g_test_kirin_pcie->dma_info->rc_phys_addr, dma_base_addr + DMA_SRC_ADDR(chan));
	writel(g_test_kirin_pcie->dma_info->ep_in_rc_cpu_phys_addr, dma_base_addr + DMA_DES_ADDR(chan));
	writel(size, dma_base_addr + DMA_TRANSMIT_SIZE(chan));
	begin_time = jiffies;
	ch_en = readl(dma_base_addr + DMA_CHAN_CONFIG(chan));
	ch_en |= (DMA_SRC_ADDR_INC | DMA_DES_ADDR_INC);
	ch_en |= (DMA_SRC_BURST_LEN(16) | DMA_DES_BURST_LEN(16));
	ch_en |= (DMA_SRC_DATA_WIDTH_64BIT | DMA_DES_DATA_WIDTH_64BIT);
	ch_en &= ~(0x1 << 2);
	writel(ch_en, dma_base_addr + DMA_CHAN_CONFIG(chan));
	PCIE_PR_INFO("tx begin jiffies = %lld", begin_time);
}
/***
* show_dma_data - show the date transmit by dma;
* @flag: 0----rx,
*           others----tx;
* @size: the size of datas.
*/
void show_dma_data(int flag, u32 size)
{
	u32 i = 0;
	if ((size <= 0) || (size > TEST_MEM_SIZE))
		size = 0x10;

	if (flag) {
		for (i = 0; i < size; i += 4)
			PCIE_PR_ERR("src data = [0x39000000+0x%x] = [0x%x]", i, readl(g_test_kirin_pcie->dma_info->rc_virt_addr+i));
		for (i = 0; i < size; i += 4)
			PCIE_PR_ERR("dest in mem[0xf5000000+0x%x] = [0x%x]", i, readl(g_test_kirin_pcie->dma_info->ep_in_rc_cpu_virt_addr+i));
	} else {
		for (i = 0; i < size; i += 4)
			PCIE_PR_ERR("src in mem[0xf5000000+0x%x] = [0x%x]", i, readl(g_test_kirin_pcie->dma_info->ep_in_rc_cpu_virt_addr+i));
		for (i = 0; i < size; i += 4)
			PCIE_PR_ERR("dest data = [0x39000000+0x%x] = [0x%x]", i, readl(g_test_kirin_pcie->dma_info->rc_virt_addr+i));
	}
}

void ep_triggle_intr(void)
{
	u32 value_temp = 0x0;
	value_temp = kirin_elb_readl(g_kirin_pcie, 0x8);
	value_temp |= (0x1 << 26);
	kirin_elb_writel(g_kirin_pcie, value_temp, 0x8);
	udelay(2);

	value_temp &= ~(0x1 << 26);
	kirin_elb_writel(g_kirin_pcie, value_temp, 0x8);
	PCIE_PR_INFO("read after write, ctl2 is %x", kirin_elb_readl(g_kirin_pcie, 0x8));
	PCIE_PR_INFO("ep_triggle_msi_intr ");
}

void config_ep_msi(void)
{
	u32 value_temp = 0x0;
	struct pcie_port *pp = &(g_kirin_pcie->pp);
	void __iomem *dbi_base = pp->dbi_base;
	kirin_pcie_writel_rc(pp, 0xffff0000, dbi_base + 0x54);
	kirin_pcie_writel_rc(pp, 0xffff0000, dbi_base + 0x58);
	kirin_pcie_writel_rc(pp, 0x0, dbi_base + 0x5c);
	kirin_pcie_readl_rc(pp, dbi_base + 0x50, &value_temp);
	value_temp |= (0x1<<16);
	kirin_pcie_writel_rc(pp, value_temp, dbi_base + 0x50);
	PCIE_PR_INFO("config ep msi ***");
}
void config_rc_msi(void)
{
	struct pcie_port *pp = &(g_kirin_pcie->pp);
	void __iomem *dbi_base = pp->dbi_base;

	kirin_pcie_writel_rc(pp, 0xffff0000, dbi_base + 0x820);
	kirin_pcie_writel_rc(pp, 0xffff0000, dbi_base + 0x824);
	kirin_pcie_writel_rc(pp, 0x1, dbi_base + 0x828);
	PCIE_PR_INFO("config rc msi ***");
}

void ep_triggle_intA_intr(void)
{
	u32 value_temp = 0;
	value_temp = kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_CTRL7_ADDR);
	value_temp |= (0x1 << 5);
	kirin_elb_writel(g_kirin_pcie, value_temp, SOC_PCIECTRL_CTRL7_ADDR);
	PCIE_PR_INFO("read after write, ctl7 is %x",
			kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_CTRL7_ADDR));
	PCIE_PR_INFO("ep_triggle_inta_intr");
}

void ep_clr_intA_intr(void)
{
	u32 value_temp = 0;
	value_temp = kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_CTRL7_ADDR);
	value_temp &= ~(0x1 << 5);
	kirin_elb_writel(g_kirin_pcie, value_temp, SOC_PCIECTRL_CTRL7_ADDR);
	PCIE_PR_INFO("read after write, ctl7 is %x",
			kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_CTRL7_ADDR));
	PCIE_PR_INFO("ep_clr_inta_intr");
}

static void msg_pcie_prog_outbound_atu(struct pcie_port *pp, int index,
				int type, u64 cpu_addr, u64 pci_addr, u32 size, u32 ctl2)
{
	void __iomem *dbi_base = pp->dbi_base;
	kirin_pcie_writel_rc(pp, PCIE_ATU_REGION_OUTBOUND | index,
						  dbi_base + PCIE_ATU_VIEWPORT);
	kirin_pcie_writel_rc(pp, lower_32_bits(cpu_addr), dbi_base + PCIE_ATU_LOWER_BASE);
	kirin_pcie_writel_rc(pp, upper_32_bits(cpu_addr), dbi_base + PCIE_ATU_UPPER_BASE);
	kirin_pcie_writel_rc(pp, lower_32_bits(cpu_addr + size - 1),
					  dbi_base + PCIE_ATU_LIMIT);
	kirin_pcie_writel_rc(pp, lower_32_bits(pci_addr), dbi_base + PCIE_ATU_LOWER_TARGET);
	kirin_pcie_writel_rc(pp, upper_32_bits(pci_addr), dbi_base + PCIE_ATU_UPPER_TARGET);
	kirin_pcie_writel_rc(pp, type, dbi_base + PCIE_ATU_CR1);
	kirin_pcie_writel_rc(pp, ctl2, dbi_base + PCIE_ATU_CR2);
}

void ep_send_msg(void)
{
	u64 cpu_addr = TEST_BUS1_CPU_ADDR;
	cpu_addr = (__force u64)ioremap(cpu_addr, 0x1000);
	writel(0x0, cpu_addr);
}

void ep_triggle_legacy_intx(int intx)
{
	struct pcie_port *pp = &g_kirin_pcie->pp;

	intx += 0x80400020;

	msg_pcie_prog_outbound_atu(pp, PCIE_ATU_REGION_INDEX0, 0x14, TEST_BUS1_CPU_ADDR, 0x0, TEST_PCIE_BUS_SPACE, intx);
	ep_send_msg();
}

void ep_clr_legacy_intx(int intx)
{
	struct pcie_port *pp = &g_kirin_pcie->pp;

	intx += 0x80400024;
	msg_pcie_prog_outbound_atu(pp, PCIE_ATU_REGION_INDEX0, 0x14, TEST_BUS1_CPU_ADDR, 0x0, TEST_PCIE_BUS_SPACE, intx);
	ep_send_msg();
}


/*msg test */

void rc_send_unlock_msg(void)
{
	u32 value_temp = 0;
	value_temp = kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_CTRL7_ADDR);
	value_temp |= (0x1 << 7);
	kirin_elb_writel(g_kirin_pcie, value_temp, SOC_PCIECTRL_CTRL7_ADDR);
}

void ep_unlock_msg_stat(void)
{
	u32 value_temp = 0;
	value_temp = kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_STATE1_ADDR);
	if (value_temp & (0x1 << 18))
		PCIE_PR_INFO("ep received a unlock message");
	else
		PCIE_PR_INFO("ep have not receive any unlock message");
}

void ep_clr_unlock_msg(void)
{
		u32 value_temp = 0;
		/*clear unlock message intr,signal rising active */
		value_temp = kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_CTRL11_ADDR);
		value_temp |= (0x1 << 9);
		kirin_elb_writel(g_kirin_pcie, value_temp, SOC_PCIECTRL_CTRL11_ADDR);
}

void pcie_enable_msg_num(int num)
{
	void __iomem *dbi_base = g_kirin_pcie->pp.dbi_base;
	struct pcie_port *pp = &g_kirin_pcie->pp;
	u32 value_temp = 0;
	kirin_pcie_readl_rc(pp, dbi_base + 0x98, &value_temp);
	switch (num) {
	case 13:
		value_temp |= (0x1 << 13);
		PCIE_PR_INFO("enable obff message");
		kirin_pcie_writel_rc(pp, value_temp, dbi_base + 0x98);
		break;
	case 10:
		value_temp |= (0x1 << 10);
		PCIE_PR_INFO("enable LTR message");
		kirin_pcie_writel_rc(pp, value_temp, dbi_base + 0x98);
		break;
	default:
		PCIE_PR_INFO("unsupport function");
		break;
	}

}

void rc_send_obff_msg(void)
{
	u32 value_temp = 0;
	value_temp = kirin_elb_readl(g_kirin_pcie, 0x34);
	value_temp &= ~(0x7 << 7);
	kirin_elb_writel(g_kirin_pcie, value_temp, 0x34);
	udelay(10);
	value_temp = kirin_elb_readl(g_kirin_pcie, 0x34);
	value_temp |= (0x7 << 7);
	kirin_elb_writel(g_kirin_pcie, value_temp, 0x34);
}

void ep_show_obff_msg_stat(void)
{
	u32 value_temp = 0;
	value_temp = kirin_elb_readl(g_kirin_pcie, 0x414);
	PCIE_PR_INFO("obff_msg is bit4~bit6 of 0x%x", value_temp);
}

void ep_clr_obff_msg(void)
{
	u32 value_temp = 0;
	value_temp = kirin_elb_readl(g_kirin_pcie, 0x3c);
	value_temp |= (0x7 << 20);
	kirin_elb_writel(g_kirin_pcie, value_temp, 0x3c);
	udelay(10);
	/*sigal rising active */
	value_temp = kirin_elb_readl(g_kirin_pcie, 0x3c);
	value_temp &= ~(0x7 << 20);
	kirin_elb_writel(g_kirin_pcie, value_temp, 0x3c);
}
void ep_send_ltr_msg(void)
{

	u32 value_temp = 0;
	value_temp = kirin_elb_readl(g_kirin_pcie, 0x44);
	value_temp |= (0x30 | 0x3 << 10 | 0x5 << 16 | 0x2 << 26);
	kirin_elb_writel(g_kirin_pcie, value_temp, 0x44);
	udelay(10);
	value_temp = kirin_elb_readl(g_kirin_pcie, 0x34);
	value_temp &= ~(0x1 << 10);
	kirin_elb_writel(g_kirin_pcie, value_temp, 0x34);
	udelay(10);
	value_temp = kirin_elb_readl(g_kirin_pcie, 0x34);
	value_temp |= (0x1 << 10);
	kirin_elb_writel(g_kirin_pcie, value_temp, 0x34);
}

void rc_show_ltr_msg_stat(void)
{
	u32 value_temp = 0;
	value_temp = kirin_elb_readl(g_kirin_pcie, 0x414);
	if (value_temp & (0x1 << 7))
		PCIE_PR_INFO("we have receive a ltr msg");
	else
		PCIE_PR_INFO("we have not receive any ltr msg");

	PCIE_PR_INFO("app_ltr_latency is %x", kirin_elb_readl(g_kirin_pcie, 0x424));
}

void rc_clr_ltr_msg(void)
{
	u32 value_temp = 0;
	value_temp = kirin_elb_readl(g_kirin_pcie, 0x3c);
	value_temp &= ~(0x1 << 23);
	kirin_elb_writel(g_kirin_pcie, value_temp, 0x3c);
	udelay(10);
	value_temp = kirin_elb_readl(g_kirin_pcie, 0x3c);
	value_temp |= (0x1 << 23);
	kirin_elb_writel(g_kirin_pcie, value_temp, 0x3c);
	PCIE_PR_INFO("clr ltr msg");
}


/**
*dump register functions*
**/
void *apb_data_fir;
void *apb_data_sec;
void dump_register_init(void)
{
	apb_data_fir = kzalloc(0x428 * sizeof(int), GFP_KERNEL);
	apb_data_sec = kzalloc(0x428 * sizeof(int), GFP_KERNEL);

	if (!apb_data_fir || !apb_data_sec)
		return;
	else
		PCIE_PR_INFO("malloc addr: rc_data:[0x%p]--[0x%p] ",
			apb_data_fir, apb_data_sec);
}

static void cmp_data(void *fir_data, void *sec_data, u32 size)
{
	u32 index = 0;
	for (index = 0; index < size; index++)
		if (*((int *)fir_data + index) != *((int *)sec_data + index))
			PCIE_PR_INFO("register offset[0x%x] changes from [0x%x] to [0x%x] \n",
				4*index, *((int *)fir_data + index), *((int *)sec_data + index));
}
/*
*dump host cfg or apb or phy register
*param:
*    first_time: 0 dump register and cmp with result which dumps at first
*/
void dump_register(u32 first_time)
{
	PCIE_PR_INFO("Dumping apb register");
	if (first_time)
		memcpy(apb_data_fir, g_kirin_pcie->apb_base, 0x428);
	else {

		PCIE_PR_INFO("Dumping &comparing apb register");
		memcpy(apb_data_sec, g_kirin_pcie->apb_base, 0x428);
		cmp_data(apb_data_fir, apb_data_sec, 0x428);
	}

	return;
}

void test_get_ep_dev(u32 ep_venid, u32 ep_devid, u32 wifi_flag)
{
	if (wifi_flag)
		g_test_kirin_pcie->is_ep_wifi = 1;
	else
		g_test_kirin_pcie->is_ep_wifi = 0;

	g_test_kirin_pcie->ep_dev = pci_get_device(ep_venid, ep_devid, NULL);
	if (!g_test_kirin_pcie->ep_dev) {
		PCIE_PR_ERR("Failed to get EP Device");
		return;
	}
}

void kirin_pcie_test_init(void)
{
	struct device_node *np = NULL;
	u32 ven_id = 0;
	u32 dev_id = 0;

	g_test_kirin_pcie = kzalloc(sizeof(struct pcie_test_st), GFP_KERNEL);
	if (!g_test_kirin_pcie) {
		PCIE_PR_ERR("Failed to kzalloc test struct ");
		return;
	}

	g_test_kirin_pcie->rc_mem_addr = __get_free_pages(GFP_KERNEL, 10);
	g_test_kirin_pcie->ep_mem_addr = __get_free_pages(GFP_KERNEL, 10);
	if (!g_test_kirin_pcie->rc_mem_addr || !g_test_kirin_pcie->ep_mem_addr) {
		PCIE_PR_ERR("Failed to alloc ep or rc mem_space ");
		return;
	}

	kirin_pcie_rd_own_conf(&g_kirin_pcie->pp, PCI_VENDOR_ID, 2, &ven_id);
	kirin_pcie_rd_own_conf(&g_kirin_pcie->pp, PCI_DEVICE_ID, 2, &dev_id);
	g_test_kirin_pcie->rc_dev = pci_get_device(ven_id, dev_id, NULL);
	if (!g_test_kirin_pcie->rc_dev) {
		PCIE_PR_ERR("Failed to get RC Device");
		return;
	}

	g_test_kirin_pcie->dma_info = kzalloc(sizeof(struct pcie_addr_info), GFP_KERNEL);
	if (!g_test_kirin_pcie->dma_info) {
		PCIE_PR_ERR("dma_info devm_kzalloc fail");
		return;
	}

	np = of_find_compatible_node(NULL, NULL, "hisilicon,kirin-pcie");
	if (np) {
		of_property_read_u32(np, "wl_power", &g_test_kirin_pcie->wl_power);
		PCIE_PR_INFO("WL Power On Number is [%d] ", g_test_kirin_pcie->wl_power);
	}
	dump_register_init();
}



MODULE_DESCRIPTION("Hisilicon Kirin pcie driver");
MODULE_LICENSE("GPL");


