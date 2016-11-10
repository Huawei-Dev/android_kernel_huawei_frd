/*
 *  include/linux/usb/tcpci_timer.h
 *  Include header file for TCPCI Interface Timer Function
 *
 *  Copyright (C) 2015 Richtek Technology Corp.
 *  Jeff Chang <jeff_chang@richtek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef TCPC_TIMER_H_INCLUDED
#define TCPC_TIMER_H_INCLUDED

#include <linux/kernel.h>

struct tcpc_device;
enum {
#ifdef CONFIG_USB_POWER_DELIVERY
	PD_TIMER_BIST_CONT_MODE = 0,
	PD_TIMER_DISCOVER_ID,
	PD_TIMER_HARD_RESET_COMPLETE,
	PD_TIMER_NO_RESPONSE,
	PD_TIMER_PS_HARD_RESET,
	PD_TIMER_PS_SOURCE_OFF,
	PD_TIMER_PS_SOURCE_ON,
	PD_TIMER_PS_TRANSITION,
	PD_TIMER_SENDER_RESPONSE,
	PD_TIMER_SINK_ACTIVITY,
	PD_TIMER_SINK_REQUEST,
	PD_TIMER_SINK_WAIT_CAP,
	PD_TIMER_SOURCE_ACTIVITY,
	PD_TIMER_SOURCE_CAPABILITY,
	PD_TIMER_SOURCE_START,
	PD_TIMER_VCONN_ON,
	PD_TIMER_VDM_MODE_ENTRY,
	PD_TIMER_VDM_MODE_EXIT,
	PD_TIMER_VDM_RESPONSE,
	PD_TIMER_SOURCE_TRANSITION,
	PD_TIMER_SRC_RECOVER,
	PD_TIMER_VSAFE0V,
	PD_TIMER_DISCARD,
	/* VDM POST TIMER */
	PD_PE_VDM_POSTPONE,
	PD_PE_TIMER_END_ID,
	/* TYPEC-TRY-TIMER */
	TYPEC_TRY_TIMER_START_ID = PD_PE_TIMER_END_ID,
	TYPEC_TRY_TIMER_DRP_TRY = TYPEC_TRY_TIMER_START_ID,
	TYPEC_TRY_TIMER_DRP_TRYWAIT,
	/* TYPEC-DEBOUNCE-TIMER */
	TYPEC_TIMER_START_ID,
	TYPEC_TIMER_CCDEBOUNCE = TYPEC_TIMER_START_ID,
	TYPEC_TIMER_PDDEBOUNCE,
	TYPEC_TIMER_ERROR_RECOVERY,
	TYPEC_TIMER_SAFE0V,
	TYPEC_TIMER_DRP_SRC_TOGGLE,
	TYPEC_TIMER_PE_IDLE,
#else
	TYPEC_TRY_TIMER_START_ID = 0,
	TYPEC_TRY_TIMER_DRP_TRY= TYPEC_TRY_TIMER_START_ID,
	TYPEC_TRY_TIMER_DRP_TRYWAIT,
	TYPEC_TIMER_START_ID,
	TYPEC_TIMER_CCDEBOUNCE = TYPEC_TIMER_START_ID,
	TYPEC_TIMER_PDDEBOUNCE,
	TYPEC_TIMER_SAFE0V,
	TYPEC_TIMER_DRP_SRC_TOGGLE,
#endif /* CONFIG_USB_POWER_DELIVERY */
	PD_TIMER_NR,
};


extern int tcpci_timer_init(struct tcpc_device *tcpc);
extern int tcpci_timer_deinit(struct tcpc_device *tcpc);
extern void tcpc_restart_timer(struct tcpc_device *tcpc, uint32_t timer_id);
extern  void tcpc_enable_timer(struct tcpc_device *tcpc, uint32_t timer_id);
extern  void tcpc_disable_timer(
		struct tcpc_device *tcpc, uint32_t timer_id);
extern void tcpc_reset_typec_try_timer(struct tcpc_device *tcpc);
extern void tcpc_reset_typec_debounce_timer(struct tcpc_device *tcpc);

extern void tcpc_reset_pe_timer(struct tcpc_device *tcpc);

#endif /* TCPC_TIMER_H_INCLUDED */
