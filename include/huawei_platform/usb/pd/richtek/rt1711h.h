/*
 *  include/linux/usb/rt1711h.h
 *  Include header file for Richtek RT1711H TypeC Port Driver
 *
 *  Copyright (C) 2015 Richtek Technology Corp.
 *  Tsunghan_Tsai <tsunghan_tsai@richtek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef __LINUX_RT1711H_H
#define __LINUX_RT1711H_H

#include <huawei_platform/usb/pd/richtek/std_tcpci_v10.h>
#include <huawei_platform/usb/pd/richtek/pd_dbg_info.h>

/*show debug message or not */
#define ENABLE_RT1711_DBG	0

/* RT1711H Private RegMap */

#define RT1711H_REG_PHY_CTRL1				(0x80)
#define RT1711H_REG_PHY_CTRL2				(0x81)
#define RT1711H_REG_PHY_CTRL3				(0x82)
#define RT1711H_REG_PHY_CTRL6				(0x85)

#define RT1711H_REG_RX_TX_DBG				(0x8b)
#define RT1711H_REG_BMC_CTRL				(0x90)
#define RT1711H_REG_BMCIO_RXDZSEL			(0x93)

#define RT1711H_REG_RT_STATUS				(0x97)
#define RT1711H_REG_RT_INT					(0x98)
#define RT1711H_REG_RT_MASK					(0x99)

#define RT1711H_REG_SOFTRESET				(0xA0)
#define RT1711H_REG_TTCPC_FILTER			(0xA1)
#define RT1711H_REG_DRP_TOGGLE_CYCLE		(0xA2)
#define RT1711H_REG_DRP_DUTY_CTRL			(0xA3)

#define RT1711H_REG_UNLOCK_PW2				(0xF0)
#define RT1711H_REG_UNLOCK_PW1				(0xF1)

/*
 * Device ID
 */

#define RT1711H_DID_A		0x2170
#define RT1711H_DID_B		0x2171

/*
 * RT1711H_REG_RX_TX_DBG			(0x8b)
 */

#define RT1711H_REG_RX_TX_DBG_RX_BUSY		(1<<7)
#define RT1711H_REG_RX_TX_DBG_TX_BUSY		(1<<6)

/*
 * RT1711H_REG_BMC_CTRL				(0x90)
 */

#define RT1711H_REG_DISCHARGE_EN			(1<<5)
#define RT1711H_REG_BMCIO_LPRPRD			(1<<4)
#define RT1711H_REG_BMCIO_LPEN				(1<<3)
#define RT1711H_REG_BMCIO_BG_EN				(1<<2)
#define RT1711H_REG_VBUS_DET_EN				(1<<1)
#define RT1711H_REG_BMCIO_OSC_EN			(1<<0)

/*
 * RT1711H_REG_RT_STATUS				(0x97)
 */

#define RT1711H_REG_VBUS_80					(1<<1)

/*
 * RT1711H_REG_RT_INT				(0x98)
 */

#define RT1711H_REG_INT_VBUS_80				(1<<1)

/*
 * RT1711H_REG_RT_MASK				(0x99)
 */

#define RT1711H_REG_M_VBUS_80				(1<<1)

#if ENABLE_RT1711_DBG
#define RT1711H_INFO(format, args...) \
	pd_dbg_info(KERN_INFO "%s() line-%d: " format,\
	__func__, __LINE__, ##args)
#else
#define RT1711_INFO(foramt, args...)
#endif

#endif /* #ifndef __LINUX_RT1711H_H */
