/*
 * PCIe host controller driver for Kirin SoCs
 *
 * Copyright (C) 2015 Hilisicon Electronics Co., Ltd.
 *		http://www.huawei.com
 *
 * Author: Xiaowei Song <songxiaowei@huawei.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "pcie-kirin.h"


/*crgperi ctrl registers*/

#define ISO_ENABLE		   0x1
#define ISO_DISABLE        0x0
#define ISO_CTRL_BIT       0x30
#define BUSIDLE_CLEAR_BIT  0x40000
#define BUSIDLE_SET_BIT    0x40004
#define BUSIDLE_CLEAR_FLAG 0x0
#define BUSIDLE_SET_FLAG   0x1
#define MTCMOS_ON          0x1
#define MTCMOS_OFF         0x0
#define MTCMOS_CTRL_BIT    0x10
#define CLK_ON             0x1
#define CLK_OFF            0x0

#define SCTRL_SCPERCLKEN3	0x1A4
#define RESET_ENABLE_FLAG  0x1
#define RESET_DISABLE_FLAG	0x0
#define RESET_CTRL_PHY_BIT	   0x88000000
#define RESET_CTRL_CTRL_BIT 	0x84000000
#define RESET_CTRL_BIT	   (RESET_CTRL_PHY_BIT | RESET_CTRL_CTRL_BIT)

#define PCIE_IRQ_PULSE			0x000
#define PCIE_LINK_RETRAIN		(0x1 < 3)

#define PCIE_APP_LTSSM_ENABLE		0x01c
#define PCIE_ELBI_RDLH_LINKUP		0x400
#define PCIE_LINKUP_ENABLE		(0x8020)
#define PCIE_LTSSM_ENABLE_BIT	  (0x1<<11)


/* PCIe DBI registers */



#define PCIE_VENDOR_ID_RCP (0x19E5)
#define PCIE_DEVICE_ID_RCP (0x3660)

struct pci_saved_state *rc_saved_state;
struct kirin_pcie *g_kirin_pcie;
extern struct pci_bus *g_root_bus;

static int kirin_pcie_link_up(struct pcie_port *pp);

void kirin_elb_writel(struct kirin_pcie *pcie, u32 val, u32 reg)
{
	writel(val, pcie->apb_base + reg);
}

u32 kirin_elb_readl(struct kirin_pcie *pcie, u32 reg)
{
	return readl(pcie->apb_base + reg);
}

/*Registers in PCIePHY*/
void kirin_phy_writel(struct kirin_pcie *pcie, u32 val, u32 reg)
{
	writel(val, pcie->phy_base + reg);
}

u32 kirin_phy_readl(struct kirin_pcie *pcie, u32 reg)
{
	return readl(pcie->phy_base + reg);
}

static void kirin_pcie_oe_ctrl(struct kirin_pcie *pcie)
{
	u32 val = 0;

	val = readl(pcie->sctrl_base + SCTRL_SCPERCLKEN3);

	/* set phy_debounce in&out time: 300ns*/
	val |= 0xF0F000;

	/*select oe_gt_mode */
	val |= 0x400;

	/*IO bypass */
	val &= ~(0x1 << 28);

	writel(val, pcie->sctrl_base + SCTRL_SCPERCLKEN3);

}

static void kirin_pcie_iso_ctrl(struct kirin_pcie *pcie, bool en_flag)
{
	if (en_flag)
		writel(ISO_CTRL_BIT, pcie->sctrl_base + 0x40);
	else
		writel(ISO_CTRL_BIT, pcie->sctrl_base + 0x44);
}

void kirin_pcie_mtcmos_ctrl(struct kirin_pcie *pcie, bool enable)
{
	if (enable)
		writel(MTCMOS_CTRL_BIT, pcie->sctrl_base + 0x60);
	else
		writel(MTCMOS_CTRL_BIT, pcie->sctrl_base + 0x64);
}

static void kirin_pcie_reset_ctrl(struct kirin_pcie *pcie, int reset)
{
	if (reset == RESET_ENABLE_FLAG)
		writel(RESET_CTRL_BIT, pcie->crg_base + 0x84);
	else
		writel(RESET_CTRL_BIT, pcie->crg_base + 0x88);
}

static void kirin_pcie_clk_ctrl(struct kirin_pcie *pcie, bool clk_on)
{
	int ret = 0;
	unsigned long ref_clk_rate = 100000000;
	if (clk_on) {
		ret = clk_set_rate(pcie->phy_ref_clk, ref_clk_rate);
		if (0 != ret) {
			PCIE_PR_ERR("Failed to set ref clk rate 100MHz ");
			return;
		}

		ret = clk_prepare_enable(pcie->phy_ref_clk);
		if (0 != ret) {
			PCIE_PR_ERR("Failed to enable phy_ref_clk ");
			return;
		}

		ret = clk_prepare_enable(pcie->apb_sys_clk);
		if (0 != ret) {
			PCIE_PR_ERR("Failed to enable apb_sys_clk ");
			clk_disable_unprepare(pcie->phy_ref_clk);
			return;
		}

		ret = clk_prepare_enable(pcie->apb_phy_clk);
		if (0 != ret) {
			PCIE_PR_ERR("Failed to enable apb_phy_clk ");
			clk_disable_unprepare(pcie->apb_sys_clk);
			clk_disable_unprepare(pcie->phy_ref_clk);
			return;
		}

		ret = clk_prepare_enable(pcie->pcie_aclk);
		if (0 != ret) {
			PCIE_PR_ERR("Failed to enable pcie_aclk ");
			clk_disable_unprepare(pcie->apb_phy_clk);
			clk_disable_unprepare(pcie->phy_ref_clk);
			clk_disable_unprepare(pcie->apb_sys_clk);
			return;
		}

		ret = clk_prepare_enable(pcie->pcie_aux_clk);
		if (0 != ret) {
			PCIE_PR_ERR("Failed to enable pcie_pclk ");
			clk_disable_unprepare(pcie->pcie_aclk);
			clk_disable_unprepare(pcie->apb_phy_clk);
			clk_disable_unprepare(pcie->phy_ref_clk);
			clk_disable_unprepare(pcie->apb_sys_clk);
			return;
		}
	} else {
		clk_disable_unprepare(pcie->apb_sys_clk);
		clk_disable_unprepare(pcie->apb_phy_clk);
		clk_disable_unprepare(pcie->pcie_aclk);
		clk_disable_unprepare(pcie->phy_ref_clk);
		clk_disable_unprepare(pcie->pcie_aux_clk);
	}
	return;
}

static int32_t kirin_pcie_get_clk(struct kirin_pcie *pcie, struct platform_device *pdev)
{
	pcie->phy_ref_clk = devm_clk_get(&pdev->dev, "pcie_phy_ref");
	if (IS_ERR(pcie->phy_ref_clk)) {
		PCIE_PR_ERR("Failed to get pcie_phy_ref clock");
		return PTR_ERR(pcie->phy_ref_clk);
	}

	pcie->pcie_aux_clk = devm_clk_get(&pdev->dev, "pcie_aux");
	if (IS_ERR(pcie->pcie_aux_clk)) {
		PCIE_PR_ERR("Failed to get pcie_aux clock");
		return PTR_ERR(pcie->pcie_aux_clk);
	}

	pcie->apb_phy_clk = devm_clk_get(&pdev->dev, "pcie_apb_phy");
	if (IS_ERR(pcie->apb_phy_clk)) {
		PCIE_PR_ERR("Failed to get pcie_apb_phy clock");
		return PTR_ERR(pcie->apb_phy_clk);
	}

	pcie->apb_sys_clk = devm_clk_get(&pdev->dev, "pcie_apb_sys");
	if (IS_ERR(pcie->apb_sys_clk)) {
		PCIE_PR_ERR("Failed to get pcie_apb_sys clock");
		return PTR_ERR(pcie->apb_sys_clk);
	}

	pcie->pcie_aclk = devm_clk_get(&pdev->dev, "pcie_aclk");
	if (IS_ERR(pcie->pcie_aclk)) {
		PCIE_PR_ERR("Failed to get pcie_aclk clock");
		return PTR_ERR(pcie->pcie_aclk);
	}
	PCIE_PR_INFO("Successed to get all clock");

	return 0;
}


static int32_t kirin_pcie_get_resource(struct pcie_port *pp, struct platform_device *pdev)
{
	struct resource *apb;
	struct resource *phy;
	struct resource *dbi;
	struct kirin_pcie *pcie = to_kirin_pcie(pp);
	struct device_node *np = NULL;
	unsigned int data[2] = {0};

	apb = platform_get_resource_byname(pdev, IORESOURCE_MEM, "apb");
	pcie->apb_base = devm_ioremap_resource(&pdev->dev, apb);
	if (IS_ERR(pcie->apb_base)) {
		PCIE_PR_ERR("cannot get apb base");
		return PTR_ERR(pcie->apb_base);
	}

	phy = platform_get_resource_byname(pdev, IORESOURCE_MEM, "phy");
	pcie->phy_base = devm_ioremap_resource(&pdev->dev, phy);
	if (IS_ERR(pcie->phy_base)) {
		PCIE_PR_ERR("cannot get PCIePHY base");
		return PTR_ERR(pcie->phy_base);
	}

	dbi = platform_get_resource_byname(pdev, IORESOURCE_MEM, "dbi");
	pp->dbi_base = devm_ioremap_resource(&pdev->dev, dbi);
	if (IS_ERR(pp->dbi_base)) {
		PCIE_PR_ERR("cannot get PCIe dbi base");
		return PTR_ERR(pp->dbi_base);
	}

	np = of_find_compatible_node(NULL, NULL, "hisilicon,crgctrl");
	if (!np) {
		PCIE_PR_ERR("Failed to find crgctrl Node ");
		return -1;
	} else
		pcie->crg_base = of_iomap(np, 0);

	np = of_find_compatible_node(NULL, NULL, "hisilicon,sysctrl");
	if (!np) {
		PCIE_PR_ERR("Failed to find sysctrl Node ");
		return -1;
	} else
		pcie->sctrl_base = of_iomap(np, 0);

	PCIE_PR_INFO("Successed to get all resource");
	return 0;
}

/**param: flag == 0   disable aspm L0sL1
**        flag == 1   enalbe  aspm L0sL1
**/
static void kirin_pcie_config_l0sl1(enum link_aspm_state aspm_state)
{
	struct pci_dev *dev = NULL;
	struct pcie_port *pp = &(g_kirin_pcie->pp);
	u32 dev_id = 0;
	u32 ven_id = 0;

	kirin_pcie_rd_own_conf(pp, PCI_VENDOR_ID, 2, &ven_id);
	kirin_pcie_rd_own_conf(pp, PCI_DEVICE_ID, 2, &dev_id);
	dev = pci_get_device(ven_id, dev_id, NULL);
	if (!dev) {
		PCIE_PR_ERR("Failed to get pci_dev[0x%x:0x%x] ", ven_id, dev_id);
		return;
	}

	pcie_capability_clear_and_set_word(dev, PCI_EXP_LNKCTL,
					   PCI_EXP_LNKCTL_ASPMC, aspm_state);

	return;
}

static int32_t kirin_pcie_get_boardtype(struct kirin_pcie *pcie, struct platform_device *pdev)
{
	int len = 0;
	struct property *prop = NULL;
	int ret = 0;

	ret = of_property_read_u32(pdev->dev.of_node, "board_type", &pcie->board_type);
	if (ret)
		PCIE_PR_ERR("Failed to get BoardType");
	else
		PCIE_PR_INFO("BoardType value [%d] ", pcie->board_type);

	prop = of_find_property(pdev->dev.of_node, "ep_flag", &len);
	if (prop != NULL) {
		pcie->ep_flag = 1;
		PCIE_PR_INFO("EndPoint Device");
	} else {
		pcie->ep_flag = 0;
		PCIE_PR_INFO("RootComplex");
	}

	return 0;
}


static void kirin_pcie_sideband_dbi_w_mode(struct pcie_port *pp, bool on)
{
	u32 val;
	struct kirin_pcie *pcie = to_kirin_pcie(pp);

	val = kirin_elb_readl(pcie, SOC_PCIECTRL_CTRL0_ADDR);
	if (on)
		val = val | PCIE_ELBI_SLV_DBI_ENABLE;
	else
		val = val & ~PCIE_ELBI_SLV_DBI_ENABLE;

	kirin_elb_writel(pcie, val, SOC_PCIECTRL_CTRL0_ADDR);
	return;
}

static void kirin_pcie_sideband_dbi_r_mode(struct pcie_port *pp, bool on)
{
	u32 val;
	struct kirin_pcie *pcie = to_kirin_pcie(pp);
	val = kirin_elb_readl(pcie, SOC_PCIECTRL_CTRL1_ADDR);

	if (on)
		val = val | PCIE_ELBI_SLV_DBI_ENABLE;
	else
		val = val & ~PCIE_ELBI_SLV_DBI_ENABLE;

	kirin_elb_writel(pcie, val, SOC_PCIECTRL_CTRL1_ADDR);
}

int kirin_pcie_power_on(struct pcie_port *pp, int on_flag)
{
	struct kirin_pcie *pcie = to_kirin_pcie(pp);
	int ret = 0;

	/*power on*/
	if (on_flag) {
		kirin_pcie_mtcmos_ctrl(pcie, MTCMOS_ON);
		udelay(100);
		PCIE_PR_DEBUG("mtcmos on Done ");

		kirin_pcie_oe_ctrl(pcie);
		PCIE_PR_DEBUG("OE CFG Done ");

		/*clk enable*/
		kirin_pcie_clk_ctrl(pcie, CLK_ON);
		udelay(1);
		PCIE_PR_DEBUG("clock on Done ");
		/*clk disable*/
		kirin_pcie_clk_ctrl(pcie, CLK_OFF);
		udelay(1);
		PCIE_PR_DEBUG("clk off Done ");

		/*ISO disable*/
		kirin_pcie_iso_ctrl(pcie, ISO_DISABLE);
		PCIE_PR_DEBUG("iso disable Done ");

		/*unset module*/
		kirin_pcie_reset_ctrl(pcie, RESET_DISABLE_FLAG);
		PCIE_PR_DEBUG("module unreset Done ");

		/*clk on*/
		kirin_pcie_clk_ctrl(pcie, CLK_ON);
		PCIE_PR_DEBUG("clk on Done ");
	} else {
		kirin_pcie_reset_ctrl(pcie, RESET_ENABLE_FLAG);
		kirin_pcie_clk_ctrl(pcie, CLK_OFF);
		kirin_pcie_iso_ctrl(pcie, ISO_ENABLE);
		kirin_pcie_mtcmos_ctrl(pcie, MTCMOS_OFF);
	}
	return 0;
}


static int kirin_pcie_establish_link(struct pcie_port *pp)
{
	int count = 0;
	struct kirin_pcie *pcie = to_kirin_pcie(pp);

	PCIE_PR_INFO("++");

	if (kirin_pcie_link_up(pp)) {
		PCIE_PR_ERR("Link already up");
		return 0;
	}

	/* setup root complex */
	dw_pcie_setup_rc(pp);
	PCIE_PR_DEBUG("setup rc Done ");

	/* assert LTSSM enable */
	kirin_elb_writel(pcie, PCIE_LTSSM_ENABLE_BIT,
			  PCIE_APP_LTSSM_ENABLE);

	/* check if the link is up or not */
	while (!kirin_pcie_link_up(pp)) {
		mdelay(1);
		count++;
		if (count == 1000) {
			PCIE_PR_ERR("PCIe Link Fail");
			return -EINVAL;
		}
	}

	PCIE_PR_INFO("PCIe Link Success ");
	return 0;
}

static irqreturn_t kirin_pcie_irq_handler(int irq, void *arg)
{
	PCIE_PR_INFO("intx irq triggled");

	return IRQ_HANDLED;
}

static irqreturn_t kirin_pcie_linkdown_irq_handler(int irq, void *arg)
{
	PCIE_PR_INFO("linkdown irq triggled\n");

	return IRQ_HANDLED;
}
static irqreturn_t kirin_pcie_msi_irq_handler(int irq, void *arg)
{
	struct pcie_port *pp = arg;

	return dw_handle_msi_irq(pp);
}

static void kirin_pcie_msi_init(struct pcie_port *pp)
{
	dw_pcie_msi_init(pp);

}

static void kirin_pcie_enable_interrupts(struct pcie_port *pp)
{
	if (IS_ENABLED(CONFIG_PCI_MSI))
		kirin_pcie_msi_init(pp);
}

void kirin_pcie_readl_rc(struct pcie_port *pp,
					void __iomem *dbi_base, u32 *val)
{
	kirin_pcie_sideband_dbi_r_mode(pp, 1);
	*val = readl(dbi_base);
	kirin_pcie_sideband_dbi_r_mode(pp, 0);
}

void kirin_pcie_writel_rc(struct pcie_port *pp,
					u32 val, void __iomem *dbi_base)
{
	kirin_pcie_sideband_dbi_w_mode(pp, 1);
	writel(val, dbi_base);
	kirin_pcie_sideband_dbi_w_mode(pp, 0);
}

int kirin_pcie_rd_own_conf(struct pcie_port *pp, int where, int size,
				u32 *val)
{
	int ret;
	kirin_pcie_sideband_dbi_r_mode(pp, true);
	ret = dw_pcie_cfg_read(pp->dbi_base + (where & ~0x3), where, size, val);
	kirin_pcie_sideband_dbi_r_mode(pp, false);
	return ret;
}

int kirin_pcie_wr_own_conf(struct pcie_port *pp, int where, int size,
				u32 val)
{
	int ret;
	kirin_pcie_sideband_dbi_w_mode(pp, true);
	ret = dw_pcie_cfg_write(pp->dbi_base + (where & ~0x3),
			where, size, val);
	kirin_pcie_sideband_dbi_w_mode(pp, false);
	return ret;
}

static int kirin_pcie_link_up(struct pcie_port *pp)
{
	struct kirin_pcie *pcie = to_kirin_pcie(pp);
	u32 val = kirin_elb_readl(pcie, PCIE_ELBI_RDLH_LINKUP);

	if ((val & PCIE_LINKUP_ENABLE) == PCIE_LINKUP_ENABLE)
		return 1;

	return 0;
}


static int kirin_pcie_phy_init(struct kirin_pcie *pcie)
{
	u32 reg_val = 0;
	u32 pipe_clk_stable = 0x1 << 19;
	u32 time = 10;

	/*choose 100MHz clk src: Bit[8]==1 pll, Bit[8]==0 Board */
	reg_val = kirin_phy_readl(pcie, SOC_PCIEPHY_CTRL1_ADDR);
	reg_val &= ~(0x1 << 8);
	kirin_phy_writel(pcie, reg_val, SOC_PCIEPHY_CTRL1_ADDR);

	/*pull down phy_test_powerdown signal */
	reg_val = kirin_phy_readl(pcie, SOC_PCIEPHY_CTRL0_ADDR);
	reg_val &= ~(0x1 << 22);
	kirin_phy_writel(pcie, reg_val, SOC_PCIEPHY_CTRL0_ADDR);
	udelay(10);

	/*derst PHY */
	reg_val = kirin_phy_readl(pcie, SOC_PCIEPHY_CTRL1_ADDR);
	reg_val &= ~(0x1 << 16);
	kirin_phy_writel(pcie, reg_val, SOC_PCIEPHY_CTRL1_ADDR);

	reg_val = kirin_phy_readl(pcie, SOC_PCIEPHY_STATE0_ADDR);
	while (reg_val & pipe_clk_stable) {
		udelay(100);
		if (time == 0) {
			PCIE_PR_INFO("PIPE clk is not stable");
			return -1;
		}
		time--;
		reg_val = kirin_phy_readl(pcie, SOC_PCIEPHY_STATE0_ADDR);
	}
	return 0;
}

static void kirin_pcie_host_init(struct pcie_port *pp)
{
	kirin_pcie_establish_link(pp);
	kirin_pcie_enable_interrupts(pp);
}

static struct pcie_host_ops kirin_pcie_host_ops = {
	.readl_rc = kirin_pcie_readl_rc,
	.writel_rc = kirin_pcie_writel_rc,
	.rd_own_conf = kirin_pcie_rd_own_conf,
	.wr_own_conf = kirin_pcie_wr_own_conf,
	.link_up = kirin_pcie_link_up,
	.host_init = kirin_pcie_host_init,
};

static int __init kirin_add_pcie_port(struct pcie_port *pp,
					   struct platform_device *pdev)
{
	int ret;
	PCIE_PR_INFO("++");

	pp->irq = platform_get_irq(pdev, 2);
	if (!pp->irq) {
		PCIE_PR_ERR("failed to get irq");
		return -ENODEV;
	}

	ret = devm_request_irq(&pdev->dev, pp->irq, kirin_pcie_irq_handler,
				IRQF_SHARED | IRQF_TRIGGER_RISING, "kirin-pcie-intc", pp);
	if (ret) {
		PCIE_PR_ERR("failed to request irq");
		return ret;
	}
	pp->irq = platform_get_irq(pdev, 3);
	if (!pp->irq) {
		PCIE_PR_ERR("failed to get irq");
		return -ENODEV;
	}

	ret = devm_request_irq(&pdev->dev, pp->irq, kirin_pcie_irq_handler,
				IRQF_SHARED | IRQF_TRIGGER_RISING, "kirin-pcie-intd", pp);
	if (ret) {
		PCIE_PR_ERR("failed to request irq");
		return ret;
	}
	pp->irq = platform_get_irq(pdev, 4);
	if (!pp->irq) {
		PCIE_PR_ERR("failed to get irq");
		return -ENODEV;
	}

	ret = devm_request_irq(&pdev->dev, pp->irq, kirin_pcie_linkdown_irq_handler,
				IRQF_SHARED | IRQF_TRIGGER_RISING, "kirin-pcie-linkdown-irq", pp);
	if (ret) {
		PCIE_PR_ERR("failed to request irq");
		return ret;
	}

	if (IS_ENABLED(CONFIG_PCI_MSI)) {
		pp->msi_irq = platform_get_irq(pdev, 1);
		if (!pp->msi_irq) {
			PCIE_PR_ERR("failed to get msi irq");
			return -ENODEV;
		}
		ret = devm_request_irq(&pdev->dev, pp->msi_irq,
					kirin_pcie_msi_irq_handler,
					IRQF_SHARED | IRQF_TRIGGER_RISING, "kirin-pcie-msi", pp);
		if (ret) {
			PCIE_PR_ERR("failed to request msi irq");
			return ret;
		}
	}

	pp->root_bus_nr = -1;
	pp->ops = &kirin_pcie_host_ops;

	ret = dw_pcie_host_init(pp);
	if (ret) {
		PCIE_PR_ERR("failed to initialize host");
		return ret;
	}

	PCIE_PR_INFO("Add pcie port sucessed ");
	return 0;
}

static int kirin_pcie_ep_init(struct pci_dev *pdev,
							const struct pci_device_id *ent)
{
	u16 val = 0;
	PCIE_PR_INFO("++");

	PCIE_PR_INFO("pci dev on bus[0x%d] has dev_id[0x%x] and ven_id[0x%x]",
			pdev->bus->number, pdev->device, pdev->vendor);
	if (g_kirin_pcie)
		g_kirin_pcie->pci_dev = pdev;

	if (pci_is_root_bus(pdev->bus))
		PCIE_PR_INFO("root bus");
	do {
		if (pci_enable_device(pdev)) {
			PCIE_PR_ERR("Cannot enable PCI device");
			break;
		}

		pci_set_master(pdev);

		PCIE_PR_INFO("--");
		return 0;
	} while (0);
	pci_read_config_word(pdev, PCI_COMMAND, &val);
	PCIE_PR_INFO("EP CMD register val[0x%x]", val);

	PCIE_PR_INFO("--");
	return 0;
}

static int kirin_pcie_probe(struct platform_device *pdev)
{
	struct kirin_pcie *kirin_pcie;
	struct pcie_port *pp;
	int ret = 0;

	PCIE_PR_INFO("++");

	kirin_pcie = devm_kzalloc(&pdev->dev, sizeof(struct kirin_pcie),
				GFP_KERNEL);
	if (!kirin_pcie) {
		PCIE_PR_ERR("devm_kzalloc fail");
		return -ENOMEM;
	}

	g_kirin_pcie = kirin_pcie;
	g_kirin_pcie->usr_suspend = 0;
	if (pdev->dev.of_node == NULL)
		PCIE_PR_ERR("of node is NULL ");

	pp = &kirin_pcie->pp;

	pp->dev = &(pdev->dev);

	kirin_pcie_get_boardtype(kirin_pcie, pdev);

	ret = kirin_pcie_get_clk(kirin_pcie, pdev);
	if (ret != 0)
		return -ENODEV;

	ret = kirin_pcie_get_resource(pp, pdev);
	if (ret != 0)
		return -ENODEV;

	kirin_pcie_power_on(pp, 1);

	if (kirin_pcie->board_type == BOARD_ASIC)
		kirin_pcie_phy_init(kirin_pcie);


	if (kirin_pcie->ep_flag) {
		PCIE_PR_INFO("@@@EP Mode");
		kirin_elb_writel(kirin_pcie, 0x200000, 0x0);
		writel(0x733c11, pp->dbi_base + 0x7c);
		writel(0x10001, pp->dbi_base + 0xa0);
		writel(0x10023, pp->dbi_base + 0x710);
		kirin_elb_writel(kirin_pcie, 0x800, 0x1c);
		kirin_pcie_sideband_dbi_r_mode(pp, 1);
		kirin_pcie_sideband_dbi_w_mode(pp, 1);
		writel(0x369019e5, pp->dbi_base);
		ret = readl(kirin_pcie->pp.dbi_base);
		PCIE_PR_INFO("DevID&VendorID is [0x%x]", ret);
		/*Modify EP device class from 0x0(unsupport) to net devices*/
		ret = readl(kirin_pcie->pp.dbi_base + 0x8);
		ret |= 0x02800000;
		writel(ret, pp->dbi_base + 0x8);
		ret = readl(kirin_pcie->pp.dbi_base + 0x8);
		PCIE_PR_INFO("Device class is [0x%x]", ret);

		pp->ops = &kirin_pcie_host_ops;
	} else {
		PCIE_PR_INFO("###RC Mode");

		ret = kirin_add_pcie_port(pp, pdev);
		if (ret < 0) {
			PCIE_PR_ERR("Failed to add pcie port, ret=[%d]", ret);
			return ret;
		}

	}

	platform_set_drvdata(pdev, kirin_pcie);

	/*IOMG for clkreq*/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 13, 0))
	ret = atfd_hisi_service_pcie_smc(ACCESS_REGISTER_FN_SUB_ID_PCIE_IO_SET,
				WR_FLAG, IOMG_CLKREQ, 0x0);
	if (ret)
		PCIE_PR_ERR("Failed to set clk_req to GPIO ");
#endif


	PCIE_PR_INFO("--");
	return 0;

}


static struct pci_device_id kirin_pci_devid[] = {
	{ .vendor = 0x19e5,
	.device = 0x3690,
	.subvendor = PCI_ANY_ID,
	.subdevice = PCI_ANY_ID,
	.class = 0x028000,
	.class_mask = 0xffff00,
	.driver_data = 0,
	},
	{ 0, }
};
MODULE_DEVICE_TABLE(pci, kirin_pci_devid);

static struct pci_driver kirin_pcie_ep_driver = {
	node:		{},
	name :		"kirin-pcie-ep",
	id_table :	kirin_pci_devid,
	probe :		kirin_pcie_ep_init,
};


static int kirin_pcie_remove(struct platform_device *pdev)
{
	struct kirin_pcie *pcie = platform_get_drvdata(pdev);

	/*reset module*/
	kirin_pcie_reset_ctrl(pcie, RESET_ENABLE_FLAG);

	/*clk disable*/
	kirin_pcie_clk_ctrl(pcie, CLK_OFF);

	/*ISO enable*/
	kirin_pcie_iso_ctrl(pcie, ISO_ENABLE);

	/*mtcmos off(ctrl)*/
	kirin_pcie_mtcmos_ctrl(pcie, MTCMOS_OFF);

	/*power off phy*/

	return 0;
}

static int kirin_pcie_resume(struct device *dev)
{
	struct pci_dev *rc_dev = NULL;
	u32 dev_id = 0;
	u32 vendor_id = 0;
	u32 pm = 0;
	u32 reg_val = 0;
	struct pcie_port *pp = &g_kirin_pcie->pp;

	PCIE_PR_INFO("++");
	/*link exists from L1SS or L1SSA*/
	kirin_pcie_rd_own_conf(pp, PCI_VENDOR_ID, 2, &vendor_id);
	kirin_pcie_rd_own_conf(pp, PCI_DEVICE_ID, 2, &dev_id);

	rc_dev = pci_get_device(vendor_id, dev_id, NULL);
	pm = pci_find_capability(rc_dev, PCI_CAP_ID_EXP);

	kirin_pcie_rd_own_conf(pp, 0x168, 4, &reg_val);
	reg_val &= ~0xF;
	kirin_pcie_wr_own_conf(pp, 0x168, 4, reg_val);

	kirin_pcie_rd_own_conf(pp, pm + 0x28, 4, &reg_val);
	reg_val &= ~(0x1 << 10);
	kirin_pcie_wr_own_conf(pp, pm + 0x28, 4, reg_val);
	/*IOMG for clkreq*/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 13, 0))
	reg_val = atfd_hisi_service_pcie_smc(ACCESS_REGISTER_FN_SUB_ID_PCIE_IO_SET,
			WR_FLAG, IOMG_CLKREQ, 0x0);
	if (reg_val)
		PCIE_PR_ERR("Failed to set clk_req to GPIO ");
#endif
	PCIE_PR_INFO("--");

	return 0;
}

static int kirin_pcie_suspend(struct device *dev)
{
	/*Set link-state to L1SS or L1SSA*/
	struct pci_dev *rc_dev = NULL;
	u32 dev_id = 0;
	u32 vendor_id = 0;
	u32 pm = 0;
	u32 reg_val = 0;
	struct pcie_port *pp = &g_kirin_pcie->pp;
	int ret = 0;

	PCIE_PR_INFO("++");
	/*link exists from L1SS or L1SSA*/
	kirin_pcie_rd_own_conf(pp, PCI_VENDOR_ID, 2, &vendor_id);
	kirin_pcie_rd_own_conf(pp, PCI_DEVICE_ID, 2, &dev_id);

	rc_dev = pci_get_device(vendor_id, dev_id, NULL);
	pm = pci_find_capability(rc_dev, PCI_CAP_ID_EXP);

	/*Enable ASPM&PCI-PM L1SS*/
	kirin_pcie_rd_own_conf(pp, 0x168, 4, &reg_val);
	reg_val |= 0xF;
	kirin_pcie_wr_own_conf(pp, 0x168, 4, reg_val);

	/*Enable LTR*/
	kirin_pcie_rd_own_conf(pp, pm + 0x28, 4, &reg_val);
	reg_val |= (0x1 << 10);
	kirin_pcie_wr_own_conf(pp, pm + 0x28, 4, reg_val);

	/*IOMG for GPIO as wake source*/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 13, 0))
	ret = atfd_hisi_service_pcie_smc(ACCESS_REGISTER_FN_SUB_ID_PCIE_IO_SET,
			WR_FLAG, IOMG_GPIO, 0x0);
	if (ret)
		PCIE_PR_ERR("Failed to set clk_req to GPIO ");
#endif
	PCIE_PR_INFO("--");

	return 0;
}


#ifdef CONFIG_PM
static const struct dev_pm_ops kirin_pcie_dev_pm_ops = {
	.suspend	= kirin_pcie_suspend,
	.resume	= kirin_pcie_resume,
};
#else
static const struct dev_pm_ops kirin_pcie_dev_pm_ops = {
	.suspend	= NULL,
	.resume	= NULL,
};
#endif


static const struct of_device_id kirin_pcie_match_table[] = {
	{
		.compatible = "hisilicon,kirin-pcie",
		.data = NULL,
	},
};

MODULE_DEVICE_TABLE(of, kirin_pcie_match_table);

struct platform_driver kirin_pcie_driver = {
	.probe			= kirin_pcie_probe,
	.remove			= kirin_pcie_remove,
	.driver			= {
		.name			= "Kirin-pcie",
		.owner			= THIS_MODULE,
		.pm				= &kirin_pcie_dev_pm_ops,
		.of_match_table = of_match_ptr(kirin_pcie_match_table),
	},
};

int kirin_pcie_usr_suspend(u32 id)
{
	struct pci_dev *rc_dev = NULL;
	int ret = 0;
	unsigned int vendor_id = 0;
	unsigned int dev_id = 0;
	struct pcie_port *pp = &(g_kirin_pcie->pp);
	int index = 0;
	u32 value = 0;
	u32 pm = 0;

	PCIE_PR_INFO("++");

	kirin_pcie_rd_own_conf(pp, PCI_VENDOR_ID, 2, &vendor_id);
	kirin_pcie_rd_own_conf(pp, PCI_DEVICE_ID, 2, &dev_id);
	PCIE_PR_INFO("+[0x%x:0x%x]+ ", vendor_id, dev_id);
	rc_dev = pci_get_device(vendor_id, dev_id, NULL);
	pm = pci_find_capability(rc_dev, PCI_CAP_ID_EXP);

	/*EP wake signal Select*/
	ret = readl(g_kirin_pcie->apb_base + SOC_PCIECTRL_CTRL12_ADDR);
	ret |= (0x3<<12);
	writel(ret, g_kirin_pcie->apb_base + SOC_PCIECTRL_CTRL12_ADDR);

	/*Enable PME*/
	kirin_pcie_rd_own_conf(pp, pm + PCI_PM_CTRL, 4, &value);
	value |= 0x103;
	kirin_pcie_wr_own_conf(pp, pm + PCI_PM_CTRL, 4, value);

	/*Broadcast PME_turn_off MSG*/
	ret = kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_CTRL7_ADDR);
	ret |= PME_TURN_OFF_BIT;
	kirin_elb_writel(g_kirin_pcie, ret, SOC_PCIECTRL_CTRL7_ADDR);
	ret = kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_STATE1_ADDR);
	do {
		ret = ret & PME_ACK_BIT;
		if (index >= 500) {
			PCIE_PR_ERR("Failed to get PME_TO_ACK");
			break;
		}
		index++;
		udelay(10);
	} while (ret != PME_ACK_BIT);

	ret = pci_save_state(rc_dev);
	rc_saved_state = pci_store_saved_state(rc_dev);
	if (ret) {
		PCIE_PR_ERR("Failed to save state of RC.");
		return ret;
	}
	/*Entry L23*/
	kirin_elb_writel(g_kirin_pcie, ENTRY_L23_BIT, SOC_PCIECTRL_CTRL7_ADDR);

	/*pull download LTSSM */
	ret = kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_CTRL7_ADDR);
	ret &= ~(0x1<<11);
	kirin_elb_writel(g_kirin_pcie, ret, SOC_PCIECTRL_CTRL7_ADDR);

	/*choose 100MHz clk src: Bit[8]==1 pll, Bit[8]==0 Board */
	ret = kirin_phy_readl(g_kirin_pcie, SOC_PCIEPHY_CTRL1_ADDR);
	ret |= 0x10100;
	kirin_phy_writel(g_kirin_pcie, ret, SOC_PCIEPHY_CTRL1_ADDR);

	/*关闭所有时钟*/
	kirin_pcie_power_on(&g_kirin_pcie->pp, 0);

	PCIE_PR_INFO("--");
	return 0;
}

int kirin_pcie_usr_resume(u32 id)
{
	struct pci_dev *rc_dev = NULL;
	unsigned int val = 0;
	int ret = 0;
	unsigned int vendor_id = 0;
	unsigned int dev_id = 0;
	struct pcie_port *pp = &(g_kirin_pcie->pp);

	PCIE_PR_INFO("++");

	if (g_kirin_pcie->usr_suspend) {
		PCIE_PR_INFO("Poweroff By Endpoint, don't resume");
		return 0;
	}

	kirin_pcie_power_on(&g_kirin_pcie->pp, 1);

	/*Stop Broadcast PME_TURN_OFF*/
	val = kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_CTRL7_ADDR);
	val &= ~PME_TURN_OFF_BIT;
	kirin_elb_writel(g_kirin_pcie, val, SOC_PCIECTRL_CTRL7_ADDR);

	kirin_pcie_rd_own_conf(pp, PCI_VENDOR_ID, 2, &vendor_id);
	kirin_pcie_rd_own_conf(pp, PCI_DEVICE_ID, 2, &dev_id);
	rc_dev = pci_get_device(vendor_id, dev_id, NULL);
	if (!rc_dev) {
		PCIE_PR_ERR("Failed to get rc pci_dev");
		return -EINVAL;
	}

	/*choose 100MHz clk src: Bit[8]==1 pll, Bit[8]==0 Board */
	val = kirin_phy_readl(g_kirin_pcie, SOC_PCIEPHY_CTRL1_ADDR);
	val &= ~0x10100;
	kirin_phy_writel(g_kirin_pcie, val, SOC_PCIEPHY_CTRL1_ADDR);

	/*pull up LTSSM */
	val = kirin_elb_readl(g_kirin_pcie, SOC_PCIECTRL_CTRL7_ADDR);
	val |= (0x1<<11);
	kirin_elb_writel(g_kirin_pcie, val, SOC_PCIECTRL_CTRL7_ADDR);

	ret = kirin_pcie_link_up(pp);
	if (ret != 1)
		PCIE_PR_ERR("Establish linkup Again Failed");

	pci_load_and_free_saved_state(rc_dev, &rc_saved_state);
	pci_restore_state(rc_dev);

	PCIE_PR_INFO("--");

	return 0;
}

void kirin_pcie_pm_control(int resume_flag)
{
	if (resume_flag) {
		PCIE_PR_INFO("EP Notify to PowerON");
		kirin_pcie_usr_resume(1);
	} else {
		PCIE_PR_INFO("EP Notify to Poweroff");
		kirin_pcie_usr_suspend(1);
	}
}

EXPORT_SYMBOL_GPL(kirin_pcie_pm_control);


extern struct pci_bus *g_root_bus;
int kirin_pcie_enumerate(void)
{
	int ret = 0;
	/*clk on*/
	PCIE_PR_INFO("++");
	kirin_pcie_config_l0sl1(ASPM_CLOSE);

	ret = kirin_pcie_establish_link(&g_kirin_pcie->pp);
	if (ret) {
		PCIE_PR_ERR("Failed to establish linkup ");
		return -1;
	}

	pci_scan_child_bus(g_root_bus);
	pci_assign_unassigned_bus_resources(g_root_bus);
	pci_bus_add_devices(g_root_bus);

	kirin_pcie_config_l0sl1(ASPM_L0S_L1);

	PCIE_PR_INFO("--");

	return 0;
}
EXPORT_SYMBOL(kirin_pcie_enumerate);

int __init kirin_pcie_init(void)
{
	int ret = 0;

	ret = pci_register_driver(&kirin_pcie_ep_driver);
	if (ret) {
		PCIE_PR_ERR("Failed to register kirin ep driver");
		pci_unregister_driver(&kirin_pcie_ep_driver);
	}

	ret |= platform_driver_probe(&kirin_pcie_driver, kirin_pcie_probe);
	return ret;
}


subsys_initcall(kirin_pcie_init);

MODULE_AUTHOR("Xiaowei Song<songxiaowei@huawei.com>");
MODULE_DESCRIPTION("Hisilicon Kirin pcie driver");
MODULE_LICENSE("GPL");


