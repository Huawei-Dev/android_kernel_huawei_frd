/*
 *  include/linux/usb/tcpci.h
 *  Include header file for TCPC Interface
 *
 *  Copyright (C) 2015 Richtek Technology Corp.
 *  Jeff Chang <jeff_chang@richtek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef __LINUX_RT_TCPC_H
#define __LINUX_RT_TCPC_H

#include <linux/device.h>
#include <linux/hrtimer.h>
#include <linux/workqueue.h>
#include <linux/wakelock.h>
#include <linux/err.h>
#include <linux/cpu.h>
#include <linux/delay.h>

#ifdef CONFIG_DUAL_ROLE_USB_INTF
#include <linux/usb/class-dual-role.h>
#endif /* CONFIG_DUAL_ROLE_USB_INTF */

#include <huawei_platform/usb/pd/richtek/tcpci_event.h>
#include <huawei_platform/usb/pd/richtek/tcpci_timer.h>
#include <huawei_platform/usb/pd/richtek/tcpci_core.h>
#include <huawei_platform/usb/pd/richtek/tcpm.h>

#ifdef CONFIG_PD_DBG_INFO
#include <huawei_platform/usb/pd/richtek/pd_dbg_info.h>
#endif /* CONFIG_PD_DBG_INFO */

#ifdef CONFIG_USB_POWER_DELIVERY
#include <huawei_platform/usb/pd/richtek/pd_core.h>
#endif /* CONFIG_USB_POWER_DELIVERY */

#define PE_STATE_FULL_NAME	0

/* provide to TCPC interface */
extern int tcpci_report_usb_port_changed(struct tcpc_device *tcpc);
extern int tcpc_typec_init(struct tcpc_device *tcpc, uint8_t typec_role);
extern void tcpc_typec_deinit(struct tcpc_device *tcpc);
extern int tcpc_dual_role_phy_init(struct tcpc_device *tcpc);

extern struct tcpc_device *tcpc_device_register(
		struct device *parent, struct tcpc_desc *tcpc_desc,
		struct tcpc_ops *ops, void *drv_data);
extern void tcpc_device_unregister(
			struct device *dev, struct tcpc_device *tcpc);

extern int tcpc_schedule_init_work(struct tcpc_device *tcpc);

extern void *tcpc_get_dev_data(struct tcpc_device *tcpc);
extern void tcpci_lock_typec(struct tcpc_device *tcpc);
extern void tcpci_unlock_typec(struct tcpc_device *tcpc);
extern int tcpci_alert(struct tcpc_device *tcpc);

extern void tcpci_vbus_level_init(struct tcpc_device *tcpc, uint16_t power_status);

static inline int tcpci_check_vbus_valid(struct tcpc_device *tcpc)
{
	return tcpc->vbus_level >= TCPC_VBUS_VALID;
}

static inline int tcpci_check_vsafe0v(struct tcpc_device *tcpc, bool detect_en)
{
	int ret = 0;

#ifdef CONFIG_TCPC_VSAFE0V_DETECT_IC
	ret = (tcpc->vbus_level == TCPC_VBUS_SAFE0V);
#else
	ret = (tcpc->vbus_level == TCPC_VBUS_INVALID);
#endif

	return ret;
}

static inline int tcpci_alert_status_clear(
    struct tcpc_device *tcpc, uint32_t mask)
{
    return tcpc->ops->alert_status_clear(tcpc, mask);
}

static inline int tcpci_fault_status_clear(
	struct tcpc_device *tcpc, uint8_t status)
{
	if (tcpc->ops->fault_status_clear)
		return tcpc->ops->fault_status_clear(tcpc, status);
	else
		return 0;
}

static inline int tcpci_get_alert_status(
    struct tcpc_device *tcpc, uint32_t *alert)
{
    return tcpc->ops->get_alert_status(tcpc, alert);
}

static inline int tcpci_get_fault_status(
    struct tcpc_device *tcpc, uint8_t *fault)
{
	if (tcpc->ops->get_fault_status)
	return tcpc->ops->get_fault_status(tcpc, fault);
	else {
		*fault = 0;
		return 0;
	}
}

static inline int tcpci_get_power_status(
    struct tcpc_device *tcpc, uint16_t *pw_status)
{
    return tcpc->ops->get_power_status(tcpc, pw_status);
}

static inline int tcpci_init(struct tcpc_device *tcpc, bool sw_reset)
{
	int ret;
	uint16_t power_status;

	ret = tcpc->ops->init(tcpc, sw_reset);
	if (ret)
		return ret;

	ret = tcpci_get_power_status(tcpc, &power_status);
	if (ret)
		return ret;
	pr_info("%s sndebug power_status = %d  %d\n", __func__, power_status, __LINE__);

	tcpci_vbus_level_init(tcpc, power_status);
    return 0;
}

static inline int tcpci_get_cc(struct tcpc_device *tcpc, int *cc1, int *cc2)
{
	return tcpc->ops->get_cc(tcpc, cc1, cc2);
}

static inline int tcpci_set_cc(struct tcpc_device *tcpc, int pull)
{
#ifdef CONFIG_USB_PD_DBG_ALWAYS_LOCAL_RP
	if (pull == TYPEC_CC_RP)
		pull = tcpc->typec_local_rp_level;
#endif

    return tcpc->ops->set_cc(tcpc, pull);
}

static inline int tcpci_set_polarity(struct tcpc_device *tcpc, int polarity)
{
    return tcpc->ops->set_polarity(tcpc, polarity);
}

static inline int tcpci_set_vconn(struct tcpc_device *tcpc, int enable)
{
	struct tcp_notify tcp_noti;
	tcp_noti.en_state.en = enable != 0;

	srcu_notifier_call_chain(&tcpc->evt_nh,
				TCP_NOTIFY_SOURCE_VCONN, &tcp_noti);

	return tcpc->ops->set_vconn(tcpc, enable);
}

static inline int tcpci_set_low_power_mode(
	struct tcpc_device *tcpc, bool en, int pull)
{
	int rv = 0;

#ifdef CONFIG_TCPC_LOW_POWER_MODE
	rv = tcpc->ops->set_low_power_mode(tcpc, en, pull);
#endif

	return rv;
}

#ifdef CONFIG_USB_POWER_DELIVERY

static inline int tcpci_set_msg_header(
	struct tcpc_device *tcpc, int power_role, int data_role)
{
    return tcpc->ops->set_msg_header(tcpc, power_role, data_role);
}

static inline int tcpci_set_rx_enable(struct tcpc_device *tcpc, uint8_t enable)
{
    return tcpc->ops->set_rx_enable(tcpc, enable);
}

static inline int tcpci_get_message(struct tcpc_device *tcpc,
    uint32_t *payload, uint16_t *head, enum tcpm_transmit_type *type)
{
    return tcpc->ops->get_message(tcpc, payload, head, type);
}

static inline int tcpci_transmit(struct tcpc_device *tcpc,
    enum tcpm_transmit_type type, uint16_t header, const uint32_t *data)
{
    return tcpc->ops->transmit(tcpc, type, header, data);
}

static inline int tcpci_set_bist_test_mode(
	struct tcpc_device *tcpc, bool en)
{
	return tcpc->ops->set_bist_test_mode(tcpc, en);
}

static inline int tcpci_set_bist_carrier_mode(
	struct tcpc_device *tcpc, uint8_t pattern)
{
	if (pattern)	/* wait for GoodCRC */
		udelay(240);

	return tcpc->ops->set_bist_carrier_mode(tcpc, pattern);
}

#ifdef CONFIG_USB_PD_RETRY_CRC_DISCARD
static inline int tcpci_retransmit(struct tcpc_device *tcpc)
{
	return tcpc->ops->retransmit(tcpc);
}
#endif	/* CONFIG_USB_PD_RETRY_CRC_DISCARD */
#endif	/* CONFIG_USB_POWER_DELIVERY */

static inline int tcpci_notify_typec_state(
	struct tcpc_device *tcpc)
{
	struct tcp_notify tcp_noti;

	tcp_noti.typec_state.polarity = tcpc->typec_polarity;
	tcp_noti.typec_state.old_state = tcpc->typec_attach_old;
	tcp_noti.typec_state.new_state = tcpc->typec_attach_new;

	return srcu_notifier_call_chain(&tcpc->evt_nh,
				TCP_NOTIFY_TYPEC_STATE, &tcp_noti);
}

static inline int tcpci_notify_role_swap(
	struct tcpc_device *tcpc, uint8_t event, uint8_t role)
{
	struct tcp_notify tcp_noti;
	tcp_noti.swap_state.new_role = role;

	return srcu_notifier_call_chain(
		&tcpc->evt_nh, event, &tcp_noti);
}

static inline int tcpci_notify_pd_state(
	struct tcpc_device *tcpc, uint8_t connect)
{
	struct tcp_notify tcp_noti;
	tcp_noti.pd_state.connected = connect;

	return srcu_notifier_call_chain(
		&tcpc->evt_nh, TCP_NOTIFY_PD_STATE, &tcp_noti);
}

static inline int tcpci_source_vbus(struct tcpc_device *tcpc, int mv, int ma)
{
	struct tcp_notify tcp_noti;

	pr_info("%s : sndebug %d  \n", __func__,__LINE__);

	if ((mv != 0) && (ma == 0)) {
		switch (tcpc->typec_local_rp_level) {
		case TYPEC_CC_RP_1_5:
			ma = 1500;
			break;
		case TYPEC_CC_RP_3_0:
			ma = 3000;
			break;
		default:
		case TYPEC_CC_RP_DFT:
			ma = 500;
			break;
		}
	}

	pr_info("%s : sndebug %d  ma = %d\n", __func__,__LINE__,ma);

	if (ma >= 500)
        	tcp_noti.vbus_state.ma = 500;
	else
		tcp_noti.vbus_state.ma = ma;
	tcp_noti.vbus_state.mv = mv;
	return srcu_notifier_call_chain(&tcpc->evt_nh,
				TCP_NOTIFY_SOURCE_VBUS, &tcp_noti);
}

static inline int tcpci_sink_vbus(struct tcpc_device *tcpc, int mv, int ma)
{
	struct tcp_notify tcp_noti;

	return 0;

	if ((mv != 0) && (ma == 0)) {
		switch (tcpc->typec_remote_rp_level) {
		case TYPEC_CC_VOLT_SNK_1_5:
			ma = 1500;
			break;
		case TYPEC_CC_VOLT_SNK_3_0:
			ma = 1500;
			break;
		default:
		case TYPEC_CC_VOLT_SNK_DFT:
			ma = 500;
			break;
		}
	}

	tcp_noti.vbus_state.ma = ma;
	tcp_noti.vbus_state.mv = mv;
	return srcu_notifier_call_chain(&tcpc->evt_nh,
				TCP_NOTIFY_SINK_VBUS, &tcp_noti);
}


#ifdef CONFIG_USB_POWER_DELIVERY

static inline int tcpci_enter_mode(struct tcpc_device *tcpc,
	uint16_t svid, uint8_t ops, uint32_t mode)
{
	/* DFP_U : DisplayPort Mode, USB Configuration */

	return 0;
}

static inline int tcpci_exit_mode(
	struct tcpc_device *tcpc, uint16_t svid)
{
	return 0;
}

#ifdef CONFIG_USB_PD_ALT_MODE

static inline int tcpci_dp_status_update(
	struct tcpc_device *tcpc, uint32_t dp_status)
{
	TCPC_DBG("DP_StatusUpdate: 0x%x\r\n", dp_status);
	return 0;
}

static inline int tcpci_dp_configure(
	struct tcpc_device *tcpc, uint32_t dp_config)
{
	struct tcp_notify tcp_noti;
	switch (dp_config & 0x03) {
		case 0:
			tcp_noti.ama_dp_state.sel_config =SW_USB;
			break;
		case MODE_DP_SNK:
			tcp_noti.ama_dp_state.sel_config =SW_UFP_D;
			tcp_noti.ama_dp_state.pin_assignment = (dp_config >> 16) & 0xff;
			break;
		case MODE_DP_SRC:
			tcp_noti.ama_dp_state.sel_config =SW_DFP_D;
			tcp_noti.ama_dp_state.pin_assignment = (dp_config >> 8) & 0xff;
			break;
	}

	tcp_noti.ama_dp_state.signal = (dp_config >>2 ) & 0x0f;
	tcp_noti.ama_dp_state.polarity = tcpc->typec_polarity;
	tcp_noti.ama_dp_state.active = 1;
	return srcu_notifier_call_chain(&tcpc->evt_nh,
				TCP_NOTIFY_AMA_DP_STATE, &tcp_noti);
}

static inline int tcpci_dp_attention(
	struct tcpc_device *tcpc, uint32_t dp_status)
{
	/* DFP_U : Not call this function during internal flow */

	TCPC_DBG("DP_Attention: 0x%x\r\n", dp_status);
	return 0;
}

static inline int tcpci_dp_notify_status_update_done(
	struct tcpc_device *tcpc, uint32_t dp_status, bool ack)
{
	/* DFP_U : Not call this function during internal flow */

	TCPC_DBG("DP_StatusUpdate_Done: 0x%x, ack=%d\r\n", dp_status, ack);
	return 0;
}

static inline int tcpci_dp_notify_config_done(
	struct tcpc_device *tcpc, uint32_t dp_config, bool ack)
{
	TCPC_DBG("DP_Config_Done: 0x%x, ack=%d\r\n", dp_config, ack);
	return 0;
}
#endif	/* CONFIG_USB_PD_ALT_MODE */
#endif	/* CONFIG_USB_POWER_DELIVERY */

#endif /* #ifndef __LINUX_RT_TCPC_H */
