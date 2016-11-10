/*
 *  include/linux/usb/tcpm.h
 *  Include header file for Type-C Port Manager
 *
 *  Copyright (C) 2015 Richtek Technology Corp.
 *  Jeff Chang <jeff_chang@richtek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef TCPM_H_
#define TCPM_H_

#include <linux/kernel.h>
#include <linux/notifier.h>

struct tcpc_device;

/*
 * Type-C Port Notify Chain
 */

enum typec_attach_type {
	TYPEC_UNATTACHED = 0,
	TYPEC_ATTACHED_SNK,
	TYPEC_ATTACHED_SRC,
	TYPEC_ATTACHED_AUDIO,
	TYPEC_ATTACHED_DEBUG,
};

enum pd_connect_result {
	PD_CONNECT_NONE = 0,
	PD_CONNECT_TYPEC_ONLY,
	PD_CONNECT_TYPEC_ONLY_SNK_DFT,
	PD_CONNECT_TYPEC_ONLY_SNK,
	PD_CONNECT_TYPEC_ONLY_SRC,
	PD_CONNECT_PE_READY,
	PD_CONNECT_PE_READY_SNK,
	PD_CONNECT_PE_READY_SRC,
};

/* Power role */
#define PD_ROLE_SINK   0
#define PD_ROLE_SOURCE 1

/* Data role */
#define PD_ROLE_UFP    0
#define PD_ROLE_DFP    1

/* Vconn role */
#define PD_ROLE_VCONN_OFF 0
#define PD_ROLE_VCONN_ON  1

enum {
	TCP_NOTIFY_SOURCE_VCONN,
	TCP_NOTIFY_SOURCE_VBUS,
	TCP_NOTIFY_SINK_VBUS,
	TCP_NOTIFY_PR_SWAP,
	TCP_NOTIFY_DR_SWAP,
	TCP_NOTIFY_VCONN_SWAP,
	TCP_NOTIFY_AMA_DP_STATE,

	TCP_NOTIFY_TYPEC_STATE,
	TCP_NOTIFY_PD_STATE,
};

struct tcp_ny_pd_state {
	uint8_t connected;
};

struct tcp_ny_swap_state {
	uint8_t new_role;
};

struct tcp_ny_enable_state {
	bool en;
};

struct tcp_ny_typec_state {
	uint8_t polarity;
	uint8_t old_state;
	uint8_t new_state;
};

struct tcp_ny_vbus_state {
	int mv;
	int ma;
};

enum {
	SW_USB = 0,
	SW_DFP_D,
	SW_UFP_D,
};

struct tcp_ny_ama_dp_state {
	uint8_t sel_config;
	uint8_t signal;
	uint8_t pin_assignment;
	uint8_t polarity;
	uint8_t active;
};

struct tcp_notify {
	union {
		struct tcp_ny_enable_state en_state;
		struct tcp_ny_vbus_state vbus_state;
		struct tcp_ny_typec_state typec_state;
		struct tcp_ny_swap_state swap_state;
		struct tcp_ny_pd_state pd_state;
		struct tcp_ny_ama_dp_state ama_dp_state;
	};
};

extern struct tcpc_device
		*tcpc_dev_get_by_name(const char *name);

extern int register_tcp_dev_notifier(struct tcpc_device *tcp_dev,
				     struct notifier_block *nb);
extern int unregister_tcp_dev_notifier(struct tcpc_device *tcp_dev,
				       struct notifier_block *nb);

/*
 * Type-C Port Control I/F
 */

enum tcpm_error_list {
	TCPM_SUCCESS = 0,
	TCPM_ERROR_UNKNOWN = -1,
	TCPM_ERROR_PUT_EVENT = -2,
};

#define TCPM_PDO_MAX_SIZE	7

struct tcpm_power_cap {
	uint8_t cnt;
	uint32_t pdos[TCPM_PDO_MAX_SIZE];
};

/* Request TCPM to send PD Request */

extern int tcpm_power_role_swap(struct tcpc_device *tcpc_dev);
extern int tcpm_data_role_swap(struct tcpc_device *tcpc_dev);
extern int tcpm_vconn_swap(struct tcpc_device *tcpc_dev);
extern int tcpm_goto_min(struct tcpc_device *tcpc_dev);
extern int tcpm_soft_reset(struct tcpc_device *tcpc_dev);
extern int tcpm_hard_reset(struct tcpc_device *tcpc_dev);
extern int tcpm_get_source_cap(
	struct tcpc_device *tcpc_dev, struct tcpm_power_cap *cap);
extern int tcpm_get_sink_cap(
	struct tcpc_device *tcpc_dev, struct tcpm_power_cap *cap);
extern int tcpm_request(
	struct tcpc_device *tcpc_dev, int mv, int ma);
extern int tcpm_error_recovery(struct tcpc_device *tcpc_dev);

/* Request TCPM to send VDM */

extern int tcpm_vdm_request_id(
	struct tcpc_device *tcpc_dev, uint8_t* cnt, uint8_t* payload);

/* Request TCPM to send PD-DP Request */

#ifdef CONFIG_USB_PD_ALT_MODE

extern int tcpm_dp_attention(
	struct tcpc_device *tcpc_dev, uint32_t dp_status);

#ifdef CONFIG_USB_PD_ALT_MODE_DFP
extern int tcpm_dp_status_update(
	struct tcpc_device *tcpc_dev, uint32_t dp_status);
extern int tcpm_dp_configuration(
	struct tcpc_device *tcpc_dev, uint32_t dp_config);
#endif	/* CONFIG_USB_PD_ALT_MODE_DFP */

#endif	/* CONFIG_USB_PD_ALT_MODE */


/* Notify TCPM */

extern int tcpm_notify_vbus_stable(struct tcpc_device *tcpc_dev);

#endif /* TCPM_H_ */
