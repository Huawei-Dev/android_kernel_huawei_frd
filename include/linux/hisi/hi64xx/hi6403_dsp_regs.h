/* Copyright (c) 2013-2014, Hisilicon Tech. Co., Ltd. All rights reserved.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 and
* only version 2 as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
*/
#ifndef __HI6403_DSP_REGS_H__
#define __HI6403_DSP_REGS_H__
#include "hi64xx_dsp_regs.h"
#define HI6403_DSP_I2S_DSPIF_CLK_EN 	   (HI64xx_DSP_SCTRL_BASE + 0x0F0)

#define HI6403_CFG_REG_CLK_CTRL			   (BASE_ADDR_PAGE_CFG + 0x038)
#define HI6403_DSP_LP_CTRL                 (BASE_ADDR_PAGE_CFG + 0x050)
#define HI6403_DSP_CTRL6_DMAC		       (BASE_ADDR_PAGE_CFG + 0x054)
#define HI6403_DSP_LP_CTRL1				   (BASE_ADDR_PAGE_CFG + 0x082)
#define HI6403_EC_REG_DSP1                 (BASE_ADDR_PAGE_CFG + 0x09D)

#define HI6403_MSG_ADDR					   0x10010000
#define HI6403_DSP_CMD_BASE				   (0x10010000 + 0x40)
#define HI6403_PARA_ADDR				   (0x10010000 + 0x80)

#define HI6403_DSP_CMD0 				   (HI6403_DSP_CMD_BASE + 0x004 * 0)
/* use for notify DSP PLL status */
#define HI6403_DSP_CMD1 				   (HI6403_DSP_CMD_BASE + 0x004 * 1)
#define HI6403_DSP_CMD2 				   (HI6403_DSP_CMD_BASE + 0x004 * 2)
/* use for notify soundtrigger event */
#define HI6403_DSP_CMD3 				   (HI6403_DSP_CMD_BASE + 0x004 * 3)

#define HI6403_DUMP_OCRAM_ADDR             (0x10011080)
#define HI6403_DUMP_OCRAM_SIZE             (0x200)
#define HI6403_DUMP_LOG_ADDR               (0x10058000)
#define HI6403_DUMP_LOG_SIZE               (0x4000)


#define HI6403_DSP_S1_CTRL_L     (HI64xx_AUDIO_SUB_BASE + 0xAE)
#define HI6403_DSP_S1_CTRL_H     (HI64xx_AUDIO_SUB_BASE + 0xAF)

#define HI6403_DSP_S3_CTRL_L     (HI64xx_AUDIO_SUB_BASE + 0xB2)
#define HI6403_DSP_S3_CTRL_H     (HI64xx_AUDIO_SUB_BASE + 0xB3)

#define HI6403_SLIM_CTRL_3     (HI64xx_AUDIO_SUB_BASE + 0xC6)
#define HI6403_SLIM_CTRL_5     (HI64xx_AUDIO_SUB_BASE + 0xC8)

#define HI6403_SLIM_UP_EN1     (HI64xx_AUDIO_SUB_BASE + 0x1FE)
#define HI6403_SLIM_UP_EN2     (HI64xx_AUDIO_SUB_BASE + 0x1FF)

#endif