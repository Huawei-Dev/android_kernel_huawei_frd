/* drivers/switch/hisi_usb_callback.c
 * HISI USB Callback Driver for Huawei
 *
 * Copyright (C) 2016 Richtek Technology Corp.
 * Author: Patrick Chang <patrick_chang@richtek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/workqueue.h>
#include <linux/gpio.h>
#include <linux/notifier.h>

#include <linux/huawei/usb/hisi_usb.h>
#include <huawei_platform/usb/switch/switch_usb_class.h>

#ifdef CONFIG_TCPC_CLASS
#include <huawei_platform/usb/pd/richtek/tcpm.h>
#endif

#define HISI_USB_ATTACH_DELAY	300
#define HISI_USB_DETACH_DELAY	100
#define HISI_USB_BC12_DELAY		100

typedef struct hisi_usb_callback_data {
	struct notifier_block nb;
	struct tcpc_device *tcpc;
	const char *tcpc_name;
	struct mutex io_lock;
	int pending_usb_event;
	int last_usb_event;
	struct workqueue_struct *wq;
	struct delayed_work usb_state_update_work;
} hisi_usb_callback_data_t;

enum {
	HISI_USB_TYPEC_NONE = 0,
	HISI_USB_TYPEC_DETACHED,
	HISI_USB_TYPEC_DEVICE_ATTACHED,
	HISI_USB_TYPEC_HOST_ATTACHED,
	HISI_USB_TYPEC_BC12_START,	/* enable bc1.2 function */
};

static int hisi_usb_tcp_notifer_call(struct notifier_block *nb,
				unsigned long event, void *data)
{
	hisi_usb_callback_data_t* usb_cb_data = container_of(nb, hisi_usb_callback_data_t, nb);
	struct tcp_notify *tcp_noti = data;

	int new_event = HISI_USB_TYPEC_NONE;

	switch (event) {
	case TCP_NOTIFY_TYPEC_STATE:
		switch (tcp_noti->typec_state.new_state) {
		case TYPEC_ATTACHED_SNK:
			new_event = HISI_USB_TYPEC_DEVICE_ATTACHED;
			break;

		case TYPEC_ATTACHED_SRC:
			new_event = HISI_USB_TYPEC_HOST_ATTACHED;
			break;

		default:
			new_event = HISI_USB_TYPEC_DETACHED;
			break;
		}
		break;

	case TCP_NOTIFY_PD_STATE:
		if (tcp_noti->pd_state.connected == PD_CONNECT_TYPEC_ONLY_SNK_DFT)
			new_event = HISI_USB_TYPEC_BC12_START;
		break;

	case TCP_NOTIFY_DR_SWAP:
		if (tcp_noti->swap_state.new_role == PD_ROLE_DFP)
			new_event = HISI_USB_TYPEC_HOST_ATTACHED;
		else
			new_event = HISI_USB_TYPEC_DEVICE_ATTACHED;
		break;
	};

	if (new_event == HISI_USB_TYPEC_NONE)
		return NOTIFY_OK;

	mutex_lock(&usb_cb_data->io_lock);
	if (usb_cb_data->pending_usb_event != new_event) {
	cancel_delayed_work(&usb_cb_data->usb_state_update_work);
	usb_cb_data->pending_usb_event = new_event;
	queue_delayed_work(usb_cb_data->wq,
			&usb_cb_data->usb_state_update_work,
			msecs_to_jiffies(0));
	} else
		pr_info("Pending event is same --> ignore this event %d\n", new_event);
	mutex_unlock(&usb_cb_data->io_lock);

	return NOTIFY_OK;
}

static inline void hisi_usb_report_device_attach(void)
{
	pr_info("hisi_usb_report_device_attach\r\n");
	//hisi_usb_otg_event(CHARGER_CONNECT_EVENT);
	msleep(HISI_USB_ATTACH_DELAY);
}

static inline void hisi_usb_report_host_attach(void)
{
	pr_info("hisi_usb_report_host_attach\r\n");
	fsa9685_dcd_timeout_enable(true);
	fsa9685_manual_sw(FSA9685_USB1_ID_TO_IDBYPASS);
	hisi_usb_otg_event(ID_FALL_EVENT);
	msleep(HISI_USB_ATTACH_DELAY);
}

static inline void hisi_usb_report_device_detach(void)
{
	pr_info("hisi_usb_report_device_detach\r\n");
	//hisi_usb_otg_event(CHARGER_DISCONNECT_EVENT);
	msleep(HISI_USB_DETACH_DELAY);
}

static inline void hisi_usb_report_host_detach(void)
{
	pr_info("hisi_usb_report_host_detach\r\n");
	fsa9685_dcd_timeout_enable(false);
	hisi_usb_otg_event(ID_RISE_EVENT);
	mdelay(HISI_USB_DETACH_DELAY);
}

static inline void hisi_usb_report_bc12_start(void)
{
	pr_info("hisi_usb_report_bc12_start\r\n");
	msleep(HISI_USB_BC12_DELAY);
}

static void hisi_usb_report_attach(int new_ev) 
{
	switch (new_ev) {
	case HISI_USB_TYPEC_DEVICE_ATTACHED:
		hisi_usb_report_device_attach();
		break;

	case HISI_USB_TYPEC_HOST_ATTACHED:
		hisi_usb_report_host_attach();
		break;
	}
}

static void hisi_usb_report_detach(int last_ev)
{
	switch (last_ev) {
	case HISI_USB_TYPEC_DEVICE_ATTACHED:
		hisi_usb_report_device_detach();
		break;

	case HISI_USB_TYPEC_HOST_ATTACHED:
		hisi_usb_report_host_detach();
		break;
	}
}


static void hisi_usb_update_state(
				struct work_struct *work)
{
	int new_ev, last_ev;
	struct hisi_usb_callback_data *usb_cb_data =
			container_of(to_delayed_work(work),
					struct hisi_usb_callback_data,
					usb_state_update_work);

	mutex_lock(&usb_cb_data->io_lock);
	new_ev = usb_cb_data->pending_usb_event;
	mutex_unlock(&usb_cb_data->io_lock);

	last_ev = usb_cb_data->last_usb_event;

	if (last_ev == new_ev)
		return;

	switch (new_ev) {
	case HISI_USB_TYPEC_DETACHED:
		hisi_usb_report_detach(last_ev);
		break;

	case HISI_USB_TYPEC_DEVICE_ATTACHED:
	case HISI_USB_TYPEC_HOST_ATTACHED:
		if (last_ev != HISI_USB_TYPEC_DETACHED)
			hisi_usb_report_detach(last_ev);
		hisi_usb_report_attach(new_ev);
		break;

	case HISI_USB_TYPEC_BC12_START:
		/* Don't update last_usb_event */
		if (last_ev == HISI_USB_TYPEC_DEVICE_ATTACHED)
			hisi_usb_report_bc12_start();
		return;
	}

	usb_cb_data->last_usb_event = new_ev;
}

static int hisi_usb_callback_parse_dt(struct hisi_usb_callback_data *usb_cb_data,
	struct device *dev)
{
	struct device_node *np = dev->of_node;

	if (!np)
		return -EINVAL;
	// default name
	if (of_property_read_string(np, "tcp_name",
		&usb_cb_data->tcpc_name) < 0)
		usb_cb_data->tcpc_name = "tcp_usb_port";

	return 0;
}


static int hisi_usb_probe(struct platform_device *pdev)
{
	struct hisi_usb_callback_data *usb_cb_data;
	int ret = 0;

	pr_info("hisi_usb_probe\n");

	usb_cb_data = devm_kzalloc(&pdev->dev,
		sizeof(*usb_cb_data), GFP_KERNEL);
	if (!usb_cb_data)
		return -ENOMEM;
	mutex_init(&usb_cb_data->io_lock);
	usb_cb_data->wq = create_workqueue("hisi_usb_cb_workqueue");
	INIT_DELAYED_WORK(&usb_cb_data->usb_state_update_work,
		hisi_usb_update_state);
	platform_set_drvdata(pdev, usb_cb_data);
	ret = hisi_usb_callback_parse_dt(usb_cb_data, &pdev->dev);
	if (ret < 0)
		goto err_parse_dt;

	usb_cb_data->last_usb_event = HISI_USB_TYPEC_NONE;
	usb_cb_data->pending_usb_event = HISI_USB_TYPEC_NONE;

	usb_cb_data->tcpc = tcpc_dev_get_by_name(usb_cb_data->tcpc_name);
	if (usb_cb_data->tcpc) {
		usb_cb_data->nb.notifier_call = hisi_usb_tcp_notifer_call;
		register_tcp_dev_notifier(usb_cb_data->tcpc,
			&usb_cb_data->nb);
	} else
		pr_info("%s : Warning *** can't find tcpc device [%s]\n",
			__FUNCTION__, usb_cb_data->tcpc_name);

	return 0;
err_parse_dt:
	mutex_destroy(&usb_cb_data->io_lock);
	return ret;
}

static int hisi_usb_remove(struct platform_device *pdev)
{
	struct hisi_usb_callback_data *usb_cb_data = platform_get_drvdata(pdev);

	if (usb_cb_data) {
		if (usb_cb_data->tcpc)
			unregister_tcp_dev_notifier(
				usb_cb_data->tcpc, &usb_cb_data->nb);

		destroy_workqueue(usb_cb_data->wq);
	}
	mutex_destroy(&usb_cb_data->io_lock);

	return 0;
}

#ifdef CONFIG_PM
static int hisi_usb_callback_suspend(struct device *dev)
{
	dev_dbg(dev, "hisi_usb_callback system suspending...\n");
	return 0;
}

static int hisi_usb_callback_resume(struct device *dev)
{
	dev_dbg(dev, "hisi_usb_callback system resuming...\n");
	return 0;
}



static int hisi_usb_callback_pm_suspend_runtime(struct device *dev)
{
	dev_dbg(dev, "hisi_usb_callback pm_runtime: suspending...\n");
	return 0;
}

static int hisi_usb_callback_pm_resume_runtime(struct device *dev)
{
	dev_dbg(dev, "hisi_usb_callback pm_runtime: resuming...\n");
	return 0;
}
static void hisi_usb_callback_shutdown(struct platform_device *pdev)
{
	dev_dbg(&pdev->dev, "hisi_usb_callback shutdown...\n");
}

static const struct dev_pm_ops hisi_usb_callback_pm_ops = {
    SET_SYSTEM_SLEEP_PM_OPS(
    hisi_usb_callback_suspend,
    hisi_usb_callback_resume)
	SET_RUNTIME_PM_OPS(
		hisi_usb_callback_pm_suspend_runtime,
		hisi_usb_callback_pm_resume_runtime,
		NULL
	)
};
#define HISH_USB_CALLBACK_PM_OPS	(&hisi_usb_callback_pm_ops)
#else
#define HISH_USB_CALLBACK_PM_OPS	(NULL)
#endif /* CONFIG_PM */


static const struct of_device_id hisi_usb_callback_match_table[] = {
	{.compatible = "hisi,usb_callback",},
	{},
};

static struct platform_driver hisi_usb_callback_driver = {
	.probe		= hisi_usb_probe,
	.remove		= hisi_usb_remove,
	.driver		= {
		.name	= "hisi-usb-callback",
		.owner	= THIS_MODULE,
		.of_match_table = hisi_usb_callback_match_table,
		.pm = HISH_USB_CALLBACK_PM_OPS,
	},
	.shutdown = hisi_usb_callback_shutdown,
};

static int __init hisi_usb_init(void)
{
	pr_info("hisi_usb_init\n");
	return platform_driver_register(&hisi_usb_callback_driver);
}

static void __exit hisi_usb_exit(void)
{
	platform_driver_unregister(&hisi_usb_callback_driver);
}

device_initcall_sync(hisi_usb_init);
module_exit(hisi_usb_exit);

MODULE_AUTHOR("Patrick Chang <patrick_chang@richtek.com>");
MODULE_DESCRIPTION("hisi usb callback driver");
MODULE_LICENSE("GPL");
