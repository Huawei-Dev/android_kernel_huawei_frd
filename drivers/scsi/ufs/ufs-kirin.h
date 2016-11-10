/* Copyright (c) 2013-2015, The Linux Foundation. All rights reserved.
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

#ifndef UFS_KIRIN_H_
#define UFS_KIRIN_H_

#define HBRN8_POLL_TOUT_MS      100

/* Device ID */
#define DEV_ID        0x00
#define PEER_DEV_ID   0x01
#define PEER_CPORT_ID 0x00
#define TRAFFIC_CLASS 0x00

/* pericrg specific define */
#define PERRSTEN3_OFFSET (0x84)
#define PERRSTDIS3_OFFSET (0x088)
#define UFS_ARESET BIT(7)
#define RST_UFS BIT(12)
#define RST_UFSIO BIT(30)

/* sysctrl specific define */

/* KIRIN UFS HC specific Registers */
enum {
	UFS_REG_OCPTHRTL = 0xC0,
	UFS_REG_OOCPR    = 0xc4,

	UFS_REG_CDACFG   = 0xd0,
	UFS_REG_CDATX1   = 0xd4,
	UFS_REG_CDATX2   = 0xd8,
	UFS_REG_CDARX1   = 0xdc,
	UFS_REG_CDARX2   = 0xe0,
	UFS_REG_CDASTA   = 0xe4,

	UFS_REG_LBMCFG   = 0xf0,
	UFS_REG_LBMSTA   = 0xf4,
	UFS_REG_UFSMODE  = 0xf8,

	UFS_REG_HCLKDIV  = 0xFC,
};

/* Clock Divider Values: Hex equivalent of frequency in MHz */
enum { UFSHCD_CLK_DIV_40 = 0x28,
       UFSHCD_CLK_DIV_62_5 = 0x3e,
       UFSHCD_CLK_DIV_125 = 0x7d,
       UFSHCD_CLK_DIV_200 = 0xc8,
};

/* vendor specific pre-defined parameters */
#define SLOW 1
#define FAST 2

#define UFS_KIRIN_LIMIT_NUM_LANES_RX	2
#define UFS_KIRIN_LIMIT_NUM_LANES_TX	2
#define UFS_KIRIN_LIMIT_HSGEAR_RX	UFS_HS_G1
#define UFS_KIRIN_LIMIT_HSGEAR_TX	UFS_HS_G1
#define UFS_KIRIN_LIMIT_PWMGEAR_RX	UFS_PWM_G1
#define UFS_KIRIN_LIMIT_PWMGEAR_TX	UFS_PWM_G1
#define UFS_KIRIN_LIMIT_RX_PWR_PWM	SLOWAUTO_MODE
#define UFS_KIRIN_LIMIT_TX_PWR_PWM	SLOWAUTO_MODE
#define UFS_KIRIN_LIMIT_RX_PWR_HS	FASTAUTO_MODE
#define UFS_KIRIN_LIMIT_TX_PWR_HS	FASTAUTO_MODE
#define UFS_KIRIN_LIMIT_HS_RATE		PA_HS_MODE_A
#define UFS_KIRIN_LIMIT_DESIRED_MODE	FAST

struct ufs_kirin_host {
	struct ufs_hba *hba;

	void __iomem *ufs_sys_ctrl;
	void __iomem *pctrl;
	void __iomem *pericrg;

	uint64_t caps;
#define KIRIN_CAP_RESERVED 	BIT(0)
#define USE_SNPS_MPHY_TC 	BIT(1)

	int avail_ln_rx;
	int avail_ln_tx;

	struct ufs_pa_layer_attr dev_req_params;
};

#define ufs_kirin_is_link_off(hba) ufshcd_is_link_off(hba)
#define ufs_kirin_is_link_active(hba) ufshcd_is_link_active(hba)
#define ufs_kirin_is_link_hibern8(hba) ufshcd_is_link_hibern8(hba)

#define ufs_sys_ctrl_writel(host, val, reg)                                    \
	writel((val), (host)->ufs_sys_ctrl + (reg))
#define ufs_sys_ctrl_readl(host, reg) readl((host)->ufs_sys_ctrl + (reg))

#define ufs_pctrl_writel(host, val, reg) writel((val), (host)->pctrl + (reg))
#define ufs_pctrl_readl(host, reg) readl((host)->pctrl + (reg))

#define ufs_pericrg_writel(host, val, reg)                                     \
	writel((val), (host)->pericrg + (reg))
#define ufs_pericrg_readl(host, reg) readl((host)->pericrg + (reg))

#endif /* UFS_KIRIN_H_ */
