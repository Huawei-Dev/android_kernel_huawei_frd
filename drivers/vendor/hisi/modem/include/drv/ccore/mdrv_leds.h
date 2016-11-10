#ifndef __MDRV_CCORE_LEDS_H__
#define __MDRV_CCORE_LEDS_H__

#include "mdrv_leds_comm.h"

/****************************************************************************
 函 数 名  : drv_led_state_func_reg
 功能描述  : 本接口为提供给协议栈的钩子函数，用于获取协议栈的点灯状态。
 输入参数  : 协议栈获取点灯状态函数的指针。
 输出参数  : 无。
 返 回 值  : 无。
 注意事项  : 无。

******************************************************************************/
void drv_led_state_func_reg (pFUNCPTR p);

/*****************************************************************************
 函 数 名  : drv_led_flash
 功能描述  : 三色灯设置。
 输入参数  : status：三色灯的状态
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
int drv_led_flash(unsigned long state);

#define DRV_LED_GET_ADDRESS()   BSP_MNTN_LedControlAddressGet()

#define DRV_LED_FLUSH(state)  drv_led_flash(state)  

#define DRV_LED_STATE_FUNREG(p)  drv_led_state_func_reg(p)
#endif /* #ifndef __MDRV_CCORE_LEDS_H__ */
