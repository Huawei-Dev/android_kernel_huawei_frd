/*
 * Copyright (c) 2013-2015, Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/time.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <soc_crgperiph_interface.h>

#include "ufshcd.h"
#include "unipro.h"
#include "ufs-kirin.h"
#include "ufshci.h"

static u64 kirin_ufs_dma_mask = DMA_BIT_MASK(64);

static void ufs_kirin_reg_dump(struct ufs_hba *hba)
{
	dev_err(hba->dev, ": --------------------------------------------------- \n");
	dev_err(hba->dev, ": \t\tHCI STANDARD REGISTER DUMP\n");
	dev_err(hba->dev, ": --------------------------------------------------- \n");
	dev_err(hba->dev, ": CAPABILITIES:                 0x%08x\n", ufshcd_readl(hba, REG_CONTROLLER_CAPABILITIES));
	dev_err(hba->dev, ": UFS VERSION:                  0x%08x\n", ufshcd_readl(hba, REG_UFS_VERSION));
	dev_err(hba->dev, ": PRODUCT ID:                   0x%08x\n", ufshcd_readl(hba, REG_CONTROLLER_DEV_ID));
	dev_err(hba->dev, ": MANUFACTURE ID:               0x%08x\n", ufshcd_readl(hba, REG_CONTROLLER_PROD_ID));
	dev_err(hba->dev, ": INTERRUPT STATUS:             0x%08x\n", ufshcd_readl(hba, REG_INTERRUPT_STATUS));
	dev_err(hba->dev, ": INTERRUPT ENABLE:             0x%08x\n", ufshcd_readl(hba, REG_INTERRUPT_ENABLE));
	dev_err(hba->dev, ": CONTROLLER STATUS:            0x%08x\n", ufshcd_readl(hba, REG_CONTROLLER_STATUS));
	dev_err(hba->dev, ": CONTROLLER ENABLE:            0x%08x\n", ufshcd_readl(hba, REG_CONTROLLER_ENABLE));
	dev_err(hba->dev, ": UIC ERR PHY ADAPTER LAYER:    0x%08x\n", ufshcd_readl(hba, REG_UIC_ERROR_CODE_PHY_ADAPTER_LAYER));
	dev_err(hba->dev, ": UIC ERR DATA LINK LAYER:      0x%08x\n", ufshcd_readl(hba, REG_UIC_ERROR_CODE_DATA_LINK_LAYER));
	dev_err(hba->dev, ": UIC ERR NETWORK LATER:        0x%08x\n", ufshcd_readl(hba, REG_UIC_ERROR_CODE_NETWORK_LAYER));
	dev_err(hba->dev, ": UIC ERR TRANSPORT LAYER:      0x%08x\n", ufshcd_readl(hba, REG_UIC_ERROR_CODE_TRANSPORT_LAYER));
	dev_err(hba->dev, ": UIC ERR DME:                  0x%08x\n", ufshcd_readl(hba, REG_UIC_ERROR_CODE_DME));
	dev_err(hba->dev, ": UTP TRANSF REQ INT AGG CNTRL: 0x%08x\n", ufshcd_readl(hba, REG_UTP_TRANSFER_REQ_INT_AGG_CONTROL));
	dev_err(hba->dev, ": UTP TRANSF REQ LIST BASE L:   0x%08x\n", ufshcd_readl(hba, REG_UTP_TRANSFER_REQ_LIST_BASE_L));
	dev_err(hba->dev, ": UTP TRANSF REQ LIST BASE H:   0x%08x\n", ufshcd_readl(hba, REG_UTP_TRANSFER_REQ_LIST_BASE_H));
	dev_err(hba->dev, ": UTP TRANSF REQ DOOR BELL:     0x%08x\n", ufshcd_readl(hba, REG_UTP_TRANSFER_REQ_DOOR_BELL));
	dev_err(hba->dev, ": UTP TRANSF REQ LIST CLEAR:    0x%08x\n", ufshcd_readl(hba, REG_UTP_TRANSFER_REQ_LIST_CLEAR));
	dev_err(hba->dev, ": UTP TRANSF REQ LIST RUN STOP: 0x%08x\n", ufshcd_readl(hba, REG_UTP_TRANSFER_REQ_LIST_RUN_STOP));
	dev_err(hba->dev, ": UTP TASK REQ LIST BASE L:     0x%08x\n", ufshcd_readl(hba, REG_UTP_TASK_REQ_LIST_BASE_L));
	dev_err(hba->dev, ": UTP TASK REQ LIST BASE H:     0x%08x\n", ufshcd_readl(hba, REG_UTP_TASK_REQ_LIST_BASE_H));
	dev_err(hba->dev, ": UTP TASK REQ DOOR BELL:       0x%08x\n", ufshcd_readl(hba, REG_UTP_TASK_REQ_DOOR_BELL));
	dev_err(hba->dev, ": UTP TASK REQ LIST CLEAR:      0x%08x\n", ufshcd_readl(hba, REG_UTP_TASK_REQ_LIST_CLEAR));
	dev_err(hba->dev, ": UTP TASK REQ LIST RUN STOP:   0x%08x\n", ufshcd_readl(hba, REG_UTP_TASK_REQ_LIST_RUN_STOP));
	dev_err(hba->dev, ": UIC COMMAND:                  0x%08x\n", ufshcd_readl(hba, REG_UIC_COMMAND));
	dev_err(hba->dev, ": UIC COMMAND ARG1:             0x%08x\n", ufshcd_readl(hba, REG_UIC_COMMAND_ARG_1));
	dev_err(hba->dev, ": UIC COMMAND ARG2:             0x%08x\n", ufshcd_readl(hba, REG_UIC_COMMAND_ARG_2));
	dev_err(hba->dev, ": UIC COMMAND ARG3:             0x%08x\n", ufshcd_readl(hba, REG_UIC_COMMAND_ARG_3));
	dev_err(hba->dev, ": DWC HCLKDIV:                  0x%08x\n", ufshcd_readl(hba, UFS_REG_HCLKDIV));
}

static int ufs_kirin_check_hibern8(struct ufs_hba *hba)
{
	int err;
	u32 tx_fsm_val = 0;
	unsigned long timeout = jiffies + msecs_to_jiffies(HBRN8_POLL_TOUT_MS);

	do {
		err = ufshcd_dme_get(hba, UIC_ARG_MIB(MPHY_TX_FSM_STATE),
				     &tx_fsm_val);
		if (err || tx_fsm_val == TX_FSM_HIBERN8)
			break;

		/* sleep for max. 200us */
		usleep_range(100, 200);
	} while (time_before(jiffies, timeout));

	/*
	 * we might have scheduled out for long during polling so
	 * check the state again.
	 */
	if (time_after(jiffies, timeout))
		err = ufshcd_dme_get(hba, UIC_ARG_MIB(MPHY_TX_FSM_STATE),
				     &tx_fsm_val);

	if (err) {
		dev_err(hba->dev, "%s: unable to get TX_FSM_STATE, err %d\n",
			__func__, err);
	} else if (tx_fsm_val != TX_FSM_HIBERN8) {
		err = tx_fsm_val;
		dev_err(hba->dev, "%s: invalid TX_FSM_STATE = %d\n", __func__,
			err);
	}

	return err;
}

/**
 * dwc_ufshcd_program_clk_div()
 * This function programs the clk divider value. this value is needed to
 * provide 1 microsecond tick to unipro layer.
 * @hba: Private Structure pointer
 * @divider_val: clock divider value to be programmed
 *
 * Retruns void
 **/
static void ufs_kirin_ufshcd_clk_div(struct ufs_hba *hba, uint32_t divider_val)
{
	ufshcd_writel(hba, divider_val, UFS_REG_HCLKDIV);

	/* do indeed need 1 microsecs tick to delay? */
	mdelay(1);
}

/**
 * TODO: make clear of every config
 */
static void ufs_kirin_pre_hce_notify(struct ufs_hba *hba)
{
	uint32_t value;
	struct ufs_kirin_host *host = hba->priv;

	BUG_ON(!host->pericrg || !host->ufs_sys_ctrl || !host->pctrl);



	/* Program Clock Divider Value */
	ufs_kirin_ufshcd_clk_div(hba, UFSHCD_CLK_DIV_40);
}

static int ufs_kirin_hce_enable_notify(struct ufs_hba *hba, bool status)
{
	int err = 0;

	switch (status) {
	case PRE_CHANGE:
		ufs_kirin_pre_hce_notify(hba);
		break;
	case POST_CHANGE:
		/* check if UFS PHY moved from DISABLED to HIBERN8 */
		err = ufs_kirin_check_hibern8(hba);
		break;
	default:
		dev_err(hba->dev, "%s: invalid status %d\n", __func__, status);
		err = -EINVAL;
		break;
	}
	return err;
}

static void ufs_kirin_phy_init(struct ufs_kirin_host *host)
{
	struct ufs_hba *hba = host->hba;

	if (host->avail_ln_rx == 0 || host->avail_ln_tx == 0) {
		ufshcd_dme_get(hba, UIC_ARG_MIB(PA_AVAILRXDATALANES),
			       &host->avail_ln_rx);
		ufshcd_dme_get(hba, UIC_ARG_MIB(PA_AVAILTXDATALANES),
			       &host->avail_ln_tx);
		if (host->avail_ln_rx != host->avail_ln_tx)
			dev_warn(
			    hba->dev,
			    "available data lane is not equal(rx:%d, tx:%d)\n",
			    host->avail_ln_rx, host->avail_ln_tx);
	}
}

/* snps mphy testchip specific configuration */
static int ufs_kirin_dme_setup_snps_mphy(struct ufs_hba *hba)
{
	uint32_t val;
	uint64_t retry = 1000;
	struct ufs_kirin_host *host = hba->priv;

	while (retry--) {
		ufshcd_dme_get(hba, UIC_ARG_MIB_SEL(0xD010, 0x0), &val);
		if (val == 1)
			break;
		if (!retry)
			return -EIO;
	}

	/* PLS put test UIC command here */
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00c0, 0x0), 0x24);
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0xd0a0, 0x0), 0x18);
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0xd09e, 0x0), 0x1);
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00f2, 0x4), 0x3);
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00f2, 0x5), 0x3);
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00bd, 0x4), 0x16);
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00bd, 0x5), 0x16);
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00c7, 0x4), 0x42);
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00c7, 0x5), 0x42);
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x80f8, 0x0), 0x00);
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00f3, 0x4), 0xa4);
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00f3, 0x4), 0xa4);
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00b4, 0x4), 0x1);
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00b4, 0x5), 0x1);
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00f4, 0x4), 0x1);
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00f4, 0x5), 0x1);
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00e9, 0x4), 0x28);
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00e9, 0x5), 0x28);
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00b5, 0x4), 0x1e);
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00b5, 0x5), 0x1e);
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00bf, 0x4), 0x2f);
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00bf, 0x5), 0x2f);
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x80e6, 0x0), 0x4);
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x80cd, 0x0), 0x2);

	/******************************************************/
	/*                  Common Block                      */
	/******************************************************/

	/* Common block Tx Global Hibernate Exit */
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x802b, 0x0), 0x00);
	/* Common block Reference Clokc Mode 26MHzt */
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x80bf, 0x0), 0x01);
	/* Common block DCO Target Frequency MAX PWM G1:9Mpbs */
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x80ea, 0x0), 0x80);
	/* Common block TX and RX Div Factor is 4 7Mbps/20 = 350KHz */
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x80f1, 0x0), 0x04);
	/* Common Block  */
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x80f3, 0x0), 0x64);
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x80fb, 0x0), 0x09);
	/* Common Block Real Time Observe Select; For debugging */
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x80f0, 0x0), 0x00);

	/******************************************************/
	/*                       Lane 0                       */
	/******************************************************/

	/* Tx0 Reference Clock 26MHz */
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00eb, 0x0), 0x0d);
	/* TX0 Configuration Clock Frequency Val; Divider setting */
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00ec, 0x0), 0x12);
	/* TX0 20bit RMMI Interface */
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00f0, 0x0), 0x12);
	/* TX0  */
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00f1, 0x0), 0xd6);
	/* Rx0 Reference Clock 26MHz */
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00eb, 0x4), 0x01);
	/* RX0 Configuration Clock Frequency Val; Divider setting */
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00ec, 0x4), 0x12);
	/* RX0 20bit RMMI Interface */
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00f0, 0x4), 0x02);
	/* RX0 Squelch Detector output is routed to RX hibernate Exit Type1
	 * signal */
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00ba, 0x4), 0x80);

	/******************************************************/
	/*                       Lane 1                       */
	/******************************************************/

	if (host->avail_ln_tx == 2) {
		/* Tx1 Reference Clock 26MHz */
		ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00eb, 0x1), 0x0d);
		/* TX1 Configuration Clock Frequency Val; Divider setting */
		ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00ec, 0x1), 0x12);
		/* TX1 20bit RMMI Interface */
		ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00f0, 0x1), 0x12);
		/* TX1  */
		ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00f1, 0x1), 0xd6);
	}

	if (host->avail_ln_rx == 2) {
		/* Rx1 Reference Clock 26MHz */
		ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00eb, 0x5), 0x01);
		/* RX1 Configuration Clock Frequency Val; Divider setting */
		ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00ec, 0x5), 0x12);
		/* RX1 20bit RMMI Interface */
		ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00f0, 0x5), 0x02);
		/* RX1 Squelch Detector output is routed to RX hibernate Exit
		 * Type1 signal */
		ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x00ba, 0x5), 0x80);
	}

	/* To write Shadow register bank to effective configuration block */
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0xd085, 0x0), 0x01);
	ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0xd000, 0x0), 0x01);

	return 0;
}

static int ufs_kirin_link_startup_pre_change(struct ufs_hba *hba)
{
	int err = 0;
	struct ufs_kirin_host *host = hba->priv;

	ufs_kirin_phy_init(host);

	if (host->caps & USE_SNPS_MPHY_TC) {
		/* SNPS mphy test chip specific configration */
		err = ufs_kirin_dme_setup_snps_mphy(hba);
		if (err)
			return err;
	}

	return err;
}

static inline void ufs_kirin_device_reset(struct ufs_hba *hba)
{
	struct ufs_kirin_host *host = hba->priv;

	/* ufs device hardware reset */
	ufs_pericrg_writel(host, RST_UFSIO, PERRSTEN3_OFFSET);
	udelay(5);
	ufs_pericrg_writel(host, RST_UFSIO, PERRSTDIS3_OFFSET);
}

static inline void ufs_kirin_areset_disreset(struct ufs_hba *hba)
{
	struct ufs_kirin_host *host = hba->priv;

	ufs_pericrg_writel(host, UFS_ARESET, PERRSTDIS3_OFFSET);
	msleep(20);
}

static inline void ufs_kirin_controller_reset(struct ufs_hba *hba)
{
	struct ufs_kirin_host *host = hba->priv;

	ufs_pericrg_writel(host, BIT(30), 0x94);
	/* ufs controller hardware reset */
	ufs_pericrg_writel(host, RST_UFS, PERRSTEN3_OFFSET);
	msleep(1);
	ufs_pericrg_writel(host, RST_UFS, PERRSTDIS3_OFFSET);
}

static inline void ufs_kirin_ufs_sys_ctrl_config(struct ufs_hba *hba)
{
	uint32_t value;
	struct ufs_kirin_host *host = hba->priv;

	/* kirin specific sys config for ufs, must double check */
	ufs_pctrl_writel(host, 0x10001, 0x10);

	ufs_pericrg_writel(host, 0x40000000, 0xec);
	ufs_pericrg_writel(host, 0x40004, 0x404);

	ufs_sys_ctrl_writel(host, 0x1, 0x1c);
	ufs_sys_ctrl_writel(host, 0x1000014, 0x10);
	ufs_sys_ctrl_writel(host, 0x1, 0x04);
	ufs_sys_ctrl_writel(host, 0x0, 0x08);

	value = ufs_sys_ctrl_readl(host, 0x4);
	value |= 0x1;
	ufs_sys_ctrl_writel(host, value, 0x4);

	value = ufs_sys_ctrl_readl(host, 0xc);
	value |= 0x100;
	ufs_sys_ctrl_writel(host, value, 0xc);

	value = ufs_sys_ctrl_readl(host, 0x1c);
	value |= 0x1;
	ufs_sys_ctrl_writel(host, value, 0x1c);

	value = ufs_sys_ctrl_readl(host, 0x14);
	value |= 0x10;
	ufs_sys_ctrl_writel(host, value, 0x14);

	value = ufs_sys_ctrl_readl(host, 0x4);
	value &= 0xfffeffff;
	ufs_sys_ctrl_writel(host, value, 0x4);

	value = ufs_sys_ctrl_readl(host, 0xc);
	value &= 0xfffeffff;
	ufs_sys_ctrl_writel(host, value, 0xc);

	value = ufs_sys_ctrl_readl(host, 0xc);
	value &= 0xfffffeff;
	ufs_sys_ctrl_writel(host, value, 0xc);

	ufs_sys_ctrl_writel(host, 0x1, 0x4);
	ufs_sys_ctrl_writel(host, 0x100, 0xc);
	ufs_sys_ctrl_writel(host, 0x1000000, 0x10);
	ufs_sys_ctrl_writel(host, 0x1, 0x1c);
	ufs_sys_ctrl_writel(host, 0x11, 0x14);

	ufs_kirin_areset_disreset(hba);

	ufs_kirin_controller_reset(hba);
}

static int ufs_kirin_link_startup_notify(struct ufs_hba *hba, bool status)
{
	int err = 0;
	switch (status) {
	case PRE_CHANGE:
		err = ufs_kirin_link_startup_pre_change(hba);
		ufs_kirin_device_reset(hba);
		break;
	case POST_CHANGE:
		break;
	default:
		break;
	}

	return err;
}

/* TODO: get limit information from dts */
struct ufs_kirin_dev_params {
	u32 pwm_rx_gear; /* pwm rx gear to work in */
	u32 pwm_tx_gear; /* pwm tx gear to work in */
	u32 hs_rx_gear;  /* hs rx gear to work in */
	u32 hs_tx_gear;  /* hs tx gear to work in */
	u32 rx_lanes;    /* number of rx lanes */
	u32 tx_lanes;    /* number of tx lanes */
	u32 rx_pwr_pwm;  /* rx pwm working pwr */
	u32 tx_pwr_pwm;  /* tx pwm working pwr */
	u32 rx_pwr_hs;   /* rx hs working pwr */
	u32 tx_pwr_hs;   /* tx hs working pwr */
	u32 hs_rate;     /* rate A/B to work in HS */
	u32 desired_working_mode;
};

static int ufs_kirin_get_pwr_dev_param(struct ufs_kirin_dev_params *kirin_param,
				       struct ufs_pa_layer_attr *dev_max,
				       struct ufs_pa_layer_attr *agreed_pwr)
{
	int min_kirin_gear;
	int min_dev_gear;
	bool is_dev_sup_hs = false;
	bool is_kirin_max_hs = false;

	if (dev_max->pwr_rx == FASTAUTO_MODE || dev_max->pwr_rx == FAST_MODE)
		is_dev_sup_hs = true;

	if (kirin_param->desired_working_mode == FAST) {
		is_kirin_max_hs = true;
		min_kirin_gear = min_t(u32, kirin_param->hs_rx_gear,
				       kirin_param->hs_tx_gear);
	} else {
		min_kirin_gear = min_t(u32, kirin_param->pwm_rx_gear,
				       kirin_param->pwm_tx_gear);
	}

	/*
	 * device doesn't support HS but kirin_param->desired_working_mode is
	 * HS, thus device and kirin_param don't agree
	 */
	if (!is_dev_sup_hs && is_kirin_max_hs) {
		pr_err("%s: failed to agree on power mode (device doesn't "
		       "support HS but requested power is HS)\n",
		       __func__);
		return -ENOTSUPP;
	} else if (is_dev_sup_hs && is_kirin_max_hs) {
		/*
		 * since device supports HS, it supports FAST_MODE.
		 * since kirin_param->desired_working_mode is also HS
		 * then final decision (FAST/FASTAUTO) is done according
		 * to kirin_params as it is the restricting factor
		 */
		agreed_pwr->pwr_rx = agreed_pwr->pwr_tx =
		    kirin_param->rx_pwr_hs;
	} else {
		/*
		 * here kirin_param->desired_working_mode is PWM.
		 * it doesn't matter whether device supports HS or PWM,
		 * in both cases kirin_param->desired_working_mode will
		 * determine the mode
		 */
		agreed_pwr->pwr_rx = agreed_pwr->pwr_tx =
		    kirin_param->rx_pwr_pwm;
	}

	/*
	 * we would like tx to work in the minimum number of lanes
	 * between device capability and vendor preferences.
	 * the same decision will be made for rx
	 */
	agreed_pwr->lane_tx =
	    min_t(u32, dev_max->lane_tx, kirin_param->tx_lanes);
	agreed_pwr->lane_rx =
	    min_t(u32, dev_max->lane_rx, kirin_param->rx_lanes);

	/* device maximum gear is the minimum between device rx and tx gears */
	min_dev_gear = min_t(u32, dev_max->gear_rx, dev_max->gear_tx);

	/*
	 * if both device capabilities and vendor pre-defined preferences are
	 * both HS or both PWM then set the minimum gear to be the chosen
	 * working gear.
	 * if one is PWM and one is HS then the one that is PWM get to decide
	 * what is the gear, as it is the one that also decided previously what
	 * pwr the device will be configured to.
	 */
	if ((is_dev_sup_hs && is_kirin_max_hs) ||
	    (!is_dev_sup_hs && !is_kirin_max_hs))
		agreed_pwr->gear_rx = agreed_pwr->gear_tx =
		    min_t(u32, min_dev_gear, min_kirin_gear);
	else if (!is_dev_sup_hs)
		agreed_pwr->gear_rx = agreed_pwr->gear_tx = min_dev_gear;
	else
		agreed_pwr->gear_rx = agreed_pwr->gear_tx = min_kirin_gear;

	agreed_pwr->hs_rate = kirin_param->hs_rate;

	pr_info("ufs final power mode: gear = %d, lane = %d, pwr = %d, "
		"rate = %d\n",
		agreed_pwr->gear_rx, agreed_pwr->lane_rx, agreed_pwr->pwr_rx,
		agreed_pwr->hs_rate);

	return 0;
}

static int ufs_kirin_pwr_change_notify(struct ufs_hba *hba,
				       bool status,
				       struct ufs_pa_layer_attr *dev_max_params,
				       struct ufs_pa_layer_attr *dev_req_params)
{
	struct ufs_kirin_dev_params ufs_kirin_cap;
	int ret = 0;

	if (!dev_req_params) {
		pr_err("%s: incoming dev_req_params is NULL\n", __func__);
		ret = -EINVAL;
		goto out;
	}

	switch (status) {
	case PRE_CHANGE:
		ufs_kirin_cap.tx_lanes = UFS_KIRIN_LIMIT_NUM_LANES_TX;
		ufs_kirin_cap.rx_lanes = UFS_KIRIN_LIMIT_NUM_LANES_RX;
		ufs_kirin_cap.hs_rx_gear = UFS_KIRIN_LIMIT_HSGEAR_RX;
		ufs_kirin_cap.hs_tx_gear = UFS_KIRIN_LIMIT_HSGEAR_TX;
		ufs_kirin_cap.pwm_rx_gear = UFS_KIRIN_LIMIT_PWMGEAR_RX;
		ufs_kirin_cap.pwm_tx_gear = UFS_KIRIN_LIMIT_PWMGEAR_TX;
		ufs_kirin_cap.rx_pwr_pwm = UFS_KIRIN_LIMIT_RX_PWR_PWM;
		ufs_kirin_cap.tx_pwr_pwm = UFS_KIRIN_LIMIT_TX_PWR_PWM;
		ufs_kirin_cap.rx_pwr_hs = UFS_KIRIN_LIMIT_RX_PWR_HS;
		ufs_kirin_cap.tx_pwr_hs = UFS_KIRIN_LIMIT_TX_PWR_HS;
		ufs_kirin_cap.hs_rate = UFS_KIRIN_LIMIT_HS_RATE;
		ufs_kirin_cap.desired_working_mode =
		    UFS_KIRIN_LIMIT_DESIRED_MODE;

		ret = ufs_kirin_get_pwr_dev_param(
		    &ufs_kirin_cap, dev_max_params, dev_req_params);
		if (ret) {
			pr_err("%s: failed to determine capabilities\n",
			       __func__);
			goto out;
		}

		break;
	case POST_CHANGE:
		break;
	default:
		ret = -EINVAL;
		break;
	}
out:
	return ret;
}

/* platform_get_resource will require resource exclusively, ufs_sys_ctrl used
 * for ufs only, but pctrl and pericrg are common resource */
static int ufs_kirin_get_resource(struct ufs_kirin_host *host)
{
	struct resource *mem_res;
	struct device_node *np = NULL;
	struct device *dev = host->hba->dev;
	struct platform_device *pdev = to_platform_device(dev);

	/* get resource of ufs sys ctrl */
	mem_res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	host->ufs_sys_ctrl = devm_ioremap_resource(dev, mem_res);
	if (!host->ufs_sys_ctrl){
		dev_err(dev, "cannot ioremap for ufs sys ctrl register\n");
		return -ENOMEM;
	}

	np = of_find_compatible_node(NULL, NULL, "hisilicon,crgctrl");
	if (!np) {
		dev_err(host->hba->dev,
			"can't find device node \"hisilicon,crgctrl\"\n");
		return -ENXIO;
	}

	host->pericrg = of_iomap(np, 0);
	if (!host->pericrg) {
		dev_err(host->hba->dev, "crgctrl iomap error!\n");
		return -ENOMEM;
	}

	np = of_find_compatible_node(NULL, NULL, "hisilicon,pctrl");
	if (!np) {
		dev_err(host->hba->dev,
			"can't find device node \"hisilicon,pctrl\"\n");
		return -ENXIO;
	}

	host->pctrl = of_iomap(np, 0);
	if (!host->pctrl) {
		dev_err(host->hba->dev, "pctrl iomap error!\n");
		return -ENOMEM;
	}

	/* we only use 64 bit dma */
	dev->dma_mask = &kirin_ufs_dma_mask;

	return 0;
}

/**
 * ufs_kirin_advertise_quirks - advertise the known KIRIN UFS controller quirks
 * @hba: host controller instance
 *
 * KIRIN UFS host controller might have some non standard behaviours (quirks)
 * than what is specified by UFSHCI specification. Advertise all such
 * quirks to standard UFS host controller driver so standard takes them into
 * account.
 */
static void ufs_kirin_advertise_quirks(struct ufs_hba *hba)
{
	/* put all our quirks here */
	hba->quirks |= 0;
}

static void ufs_kirin_populate_dt(struct device *dev, struct ufs_kirin_host *host){

	struct device_node *np = dev->of_node;

	if(!np){
		dev_err(dev, "can not find device node\n");
		return;
	}

	if (of_find_property(np, "ufs-kirin-use-snps-mphy-tc", NULL))
		host->caps |= USE_SNPS_MPHY_TC;
}

/**
 * ufs_kirin_init
 * @hba: host controller instance
 */
static int ufs_kirin_init(struct ufs_hba *hba)
{
	int err = 0;
	struct device *dev = hba->dev;
	struct ufs_kirin_host *host;

	host = devm_kzalloc(dev, sizeof(*host), GFP_KERNEL);
	if (!host) {
		err = -ENOMEM;
		dev_err(dev, "%s: no memory for kirin ufs host\n", __func__);
		goto out;
	}

	host->hba = hba;
	hba->priv = (void *)host;

	ufs_kirin_advertise_quirks(hba);

	ufs_kirin_populate_dt(dev, host);

	err = ufs_kirin_get_resource(host);
	if(err)
		goto host_free;

	ufs_kirin_ufs_sys_ctrl_config(hba);

	goto out;

host_free:
	devm_kfree(dev, host);
	hba->priv = NULL;
out:
	return err;
}

static void ufs_kirin_exit(struct ufs_hba *hba)
{
	kfree(hba->priv);
	return;
}

/**
 * struct ufs_hba_kirin_vops - UFS KIRIN specific variant operations
 *
 * The variant operations configure the necessary controller and PHY
 * handshake during initialization.
 */
const struct ufs_hba_variant_ops ufs_hba_kirin_vops = {
    .name                = "kirin",
    .init                = ufs_kirin_init,
    .exit                = ufs_kirin_exit,
    .setup_clocks        = NULL,
    .hce_enable_notify   = ufs_kirin_hce_enable_notify,
    .link_startup_notify = ufs_kirin_link_startup_notify,
    .pwr_change_notify   = ufs_kirin_pwr_change_notify,
    .dump_reg            = ufs_kirin_reg_dump,
};
EXPORT_SYMBOL(ufs_hba_kirin_vops);
