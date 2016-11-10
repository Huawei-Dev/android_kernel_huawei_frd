/* drives/usb/phy/tcpm.c
 * Power Delvery Managert Driver
 *
 * Copyright (C) 2015 Richtek Technology Corp.
 * Author: TH <tsunghan_tasi@richtek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <huawei_platform/usb/pd/richtek/tcpm.h>
#include <huawei_platform/usb/pd/richtek/pd_core.h>
#include <huawei_platform/usb/pd/richtek/tcpci.h>
#include <huawei_platform/usb/pd/richtek/pd_policy_engine.h>
#include <huawei_platform/usb/pd/richtek/pd_dpm_core.h>

#ifdef CONFIG_USB_POWER_DELIVERY

int tcpm_power_role_swap(struct tcpc_device *tcpc_dev)
{
	bool ret;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	ret = pd_put_dpm_pd_request_event(pd_port,
				PD_DPM_PD_REQUEST_PR_SWAP);
	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_power_role_swap);

int tcpm_data_role_swap(struct tcpc_device *tcpc_dev)
{
	bool ret;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	ret = pd_put_dpm_pd_request_event(pd_port,
				PD_DPM_PD_REQUEST_DR_SWAP);
	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_data_role_swap);

int tcpm_vconn_swap(struct tcpc_device *tcpc_dev)
{
	bool ret;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	ret = pd_put_dpm_pd_request_event(pd_port,
				PD_DPM_PD_REQUEST_VCONN_SWAP);
	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_vconn_swap);

int tcpm_goto_min(struct tcpc_device *tcpc_dev)
{
	bool ret;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	ret = pd_put_dpm_pd_request_event(pd_port,
				PD_DPM_PD_REQUEST_GOTOMIN);
	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_goto_min);

int tcpm_soft_reset(struct tcpc_device *tcpc_dev)
{
	bool ret;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	ret = pd_put_dpm_pd_request_event(pd_port,
				PD_DPM_PD_REQUEST_SOFTRESET);
	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_soft_reset);

int tcpm_hard_reset(struct tcpc_device *tcpc_dev)
{
	bool ret;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	ret = pd_put_dpm_pd_request_event(pd_port,
				PD_DPM_PD_REQUEST_HARDRESET);
	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_hard_reset);

int tcpm_get_source_cap(
	struct tcpc_device *tcpc_dev, struct tcpm_power_cap *cap)
{
	bool ret;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	ret = pd_put_dpm_pd_request_event(pd_port,
				PD_DPM_PD_REQUEST_GET_SOURCE_CAP);
	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	/* TODO: Finish it later */

	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_get_source_cap);

int tcpm_get_sink_cap(
	struct tcpc_device *tcpc_dev, struct tcpm_power_cap *cap)
{
	bool ret;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	ret = pd_put_dpm_pd_request_event(pd_port,
				PD_DPM_PD_REQUEST_GET_SINK_CAP);
	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	/* TODO: Finish it later */

	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_get_sink_cap);

int tcpm_request(struct tcpc_device *tcpc_dev, int mv, int ma)
{
	bool ret;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	mutex_lock(&pd_port->pd_lock);
	ret = pd_dpm_send_request(pd_port, mv, ma);
	mutex_unlock(&pd_port->pd_lock);

	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_request);

int tcpm_error_recovery(struct tcpc_device *tcpc_dev)
{
	bool ret;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	ret = pd_put_dpm_event(pd_port, PD_DPM_ERROR_RECOVERY);
	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return TCPM_SUCCESS;
}

int tcpm_vdm_request_id(
	struct tcpc_device *tcpc_dev, uint8_t* cnt, uint8_t* payload)
{
	bool ret;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	mutex_lock(&pd_port->pd_lock);
	ret = vdm_put_dpm_vdm_request_event(
		pd_port, PD_DPM_VDM_REQUEST_DISCOVER_ID);
	mutex_unlock(&pd_port->pd_lock);

	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return TCPM_SUCCESS;
}

#ifdef CONFIG_USB_PD_ALT_MODE

int tcpm_dp_attention(
	struct tcpc_device *tcpc_dev, uint32_t dp_status)
{
	bool ret = false;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	mutex_lock(&pd_port->pd_lock);

	ret = vdm_put_dpm_vdm_request_event(
		pd_port, PD_DPM_VDM_REQUEST_ATTENTION);


	if (ret) {
		pd_port->dp_status = dp_status;
		pd_port->mode_svid = USB_SID_DISPLAYPORT;
	}

	mutex_unlock(&pd_port->pd_lock);

	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_dp_attention);

#ifdef CONFIG_USB_PD_ALT_MODE_DFP

int tcpm_dp_status_update(
	struct tcpc_device *tcpc_dev, uint32_t dp_status)
{
	bool ret = false;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	mutex_lock(&pd_port->pd_lock);

	ret = vdm_put_dpm_vdm_request_event(
		pd_port, PD_DPM_VDM_REQUEST_DP_STATUS_UPDATE);

	if (ret) {
		pd_port->dp_status = dp_status;
		pd_port->mode_svid = USB_SID_DISPLAYPORT;
	}

	mutex_unlock(&pd_port->pd_lock);

	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_dp_status_update);

int tcpm_dp_configuration(
	struct tcpc_device *tcpc_dev, uint32_t dp_config)
{
	bool ret = false;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	mutex_lock(&pd_port->pd_lock);

	ret = vdm_put_dpm_vdm_request_event(
		pd_port, PD_DPM_VDM_REQUEST_DP_CONFIG);

	if (ret) {
		pd_port->local_dp_config = dp_config;
		pd_port->mode_svid = USB_SID_DISPLAYPORT;
	}

	mutex_unlock(&pd_port->pd_lock);

	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_dp_configuration);

#endif	/* CONFIG_USB_PD_ALT_MODE_DFP */
#endif	/* CONFIG_USB_PD_ALT_MODE */


int tcpm_notify_vbus_stable(
	struct tcpc_device *tcpc_dev)
{
	pd_put_vbus_stable_event(tcpc_dev);
	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_notify_vbus_stable);


#endif /* CONFIG_USB_POWER_DELIVERY */
