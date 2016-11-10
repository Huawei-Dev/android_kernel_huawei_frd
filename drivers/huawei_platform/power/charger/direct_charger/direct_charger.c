/*
	*drivers/power/huawei_charger.c
	*
	*huawei	charger	driver
	*
	*Copyright(C)2012-2015 HUAWEI, Inc.
	*Author: HUAWEI, Inc.
	*
	*This package is free software; you can	redistribute it and/or modify
	*it under the terms of the GNU General Public License version 2 as
	*published by the Free Software Foundation.
*/
/**********************************************************
*	Function: scp_retry_pre_operate
*	Discription: pre operate before retry scp enable
*	Parameters: di:charge_device_info,type : enum scp_retry_operate_type
*	return value: 0: scp pre operate success
*	              -1:scp pre operate fail
**********************************************************/
#include <huawei_platform/log/hw_log.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/wakelock.h>
#include <linux/power/hisi/coul/hisi_coul_drv.h>
#include <linux/power/hisi/hisi_bci_battery.h>
#include <huawei_platform/power/direct_charger.h>
#include <huawei_platform/power/huawei_charger.h>
#ifdef CONFIG_HUAWEI_HW_DEV_DCT
#include <huawei_platform/devdetect/hw_dev_dec.h>
#endif
#if defined CONFIG_HUAWEI_DSM
#include <dsm/dsm_pub.h>
#endif
#define HWLOG_TAG direct_charge
HWLOG_REGIST();

static struct wake_lock direct_charge_lock;
static struct direct_charge_device *g_di;
static enum scp_stage_type scp_stage = SCP_STAGE_DEFAULT;
struct smart_charge_ops* g_scp_ops;
struct loadswitch_ops* g_ls_ops;
struct batinfo_ops* g_bi_ops;

int scp_ops_register(struct smart_charge_ops* ops)
{
	int ret = 0;

	if (ops != NULL)
	{
		g_scp_ops = ops;
	}
	else
	{
		hwlog_err("scp ops register fail!\n");
		ret = -EPERM;
	}
	return ret;
}

int loadswitch_ops_register(struct loadswitch_ops* ops)
{
	int ret = 0;

	if (ops != NULL)
	{
		g_ls_ops = ops;
	}
	else
	{
		hwlog_err("ls ops register fail!\n");
		ret = -EPERM;
	}
	return ret;
}

int batinfo_ops_register(struct batinfo_ops* ops)
{
	int ret = 0;

	if (ops != NULL)
	{
		g_bi_ops = ops;
	}
	else {
		hwlog_err("batinfo ops register fail!\n");
		ret = -EPERM;
	}
	return ret;
}

/**********************************************************
*  Function:       direct_charge_wake_lock
*  Description:   apply direct_charge wake_lock
*  Parameters:   NULL
*  return value:  NULL
**********************************************************/
static void direct_charge_wake_lock(void)
{
	if (!wake_lock_active(&direct_charge_lock)) {
		wake_lock(&direct_charge_lock);
		hwlog_info("direct_charge wake lock\n");
	}
}

/**********************************************************
*  Function:       direct_charge_wake_unlock
*  Description:   release direct_charge wake_lock
*  Parameters:   NULL
*  return value:  NULL
**********************************************************/
static void direct_charge_wake_unlock(void)
{
	if (wake_lock_active(&direct_charge_lock)) {
		wake_unlock(&direct_charge_lock);
		hwlog_info("direct_charge wake unlock\n");
	}
}

/**********************************************************
*  Function:       scp_get_stage_status
*  Description:    get the stage of scp charge
*  Parameters:
*  return value:   stage
**********************************************************/
static enum scp_stage_type scp_get_stage_status(void)
{
	return scp_stage;
}

/**********************************************************
*  Function:       scp_set_stage_status
*  Description:    set the stage of scp charge
*  Parameters:     stage type
*  return value:   NULL
**********************************************************/
static void scp_set_stage_status(enum scp_stage_type stage_type)
{
	scp_stage = stage_type;
}

static int is_direct_charge_ops_valid(struct direct_charge_device *di)
{
	if (NULL == di)
	{
		hwlog_err("[%s]:di is NULL!\n", __func__);
		return INVALID;
	}
	if ((NULL == di->scp_ops) || (NULL == di->scp_ops->is_support_scp)
		||(NULL	== di->scp_ops->scp_init) || (NULL == di->scp_ops->scp_adaptor_detect)
		||(NULL	== di->scp_ops->scp_set_adaptor_voltage) || (NULL == di->scp_ops->scp_get_adaptor_voltage)
		||(NULL	== di->scp_ops->scp_get_adaptor_current) || (NULL == di->scp_ops->scp_set_adaptor_current)
		||(NULL	== di->scp_ops->scp_adaptor_reset) || (NULL == di->scp_ops->scp_chip_reset)
		||(NULL	== di->scp_ops->scp_stop_charge_config)	|| (NULL == di->scp_ops->scp_get_adaptor_status)
		||(NULL	== di->scp_ops->scp_get_chip_status) || (NULL == di->scp_ops->scp_exit)
		||(NULL == di->scp_ops->scp_adaptor_support_second_output) || (NULL == di->scp_ops->scp_open_second_output))
	{
		hwlog_err("scp ops is null!\n");
		return	INVALID;
	}

	if ((NULL == di->ls_ops) || (NULL == di->ls_ops->ls_init) || (NULL == di->ls_ops->ls_exit
		|| NULL == di->ls_ops->get_ls_id) || (NULL == di->ls_ops->ls_enable))
	{
		hwlog_err("ls ops is null!\n");
		return	INVALID;
	}

	if ((NULL == di->bi_ops) || (NULL == di->bi_ops->get_bat_current) || (NULL == di->bi_ops->exit)
		|| (NULL == di->bi_ops->get_bat_btb_voltage) || (NULL == di->bi_ops->get_vbus_voltage))
	{
		hwlog_err("bi ops is null!\n");
		return	INVALID;
	}
	return	VALID;
}

static int get_bat_voltage(struct direct_charge_device* di)
{
	int btb_vol = 0;
	int package_vol = 0;

	btb_vol = di->bi_ops->get_bat_btb_voltage();
	package_vol = di->bi_ops->get_bat_package_voltage();
	if (btb_vol < 0 && package_vol < 0)
	{
		hwlog_err("[%s]:error\n", __func__);
		di->scp_stop_charging_flag = 1;
		return 0;
	}

	return btb_vol > package_vol ? btb_vol : package_vol;
}

static int get_bat_current(struct direct_charge_device* di)
{
	int bat_curr = 0;
	int ret;
	ret = di->bi_ops->get_bat_current(&bat_curr);
	if (ret < 0)
	{
		hwlog_err("[%s]:error\n", __func__);
		di->scp_stop_charging_flag = 1;
	}
	return bat_curr;
}

static int get_ls_resistance(struct direct_charge_device *di)
{
	int r = 0;
	int ret;
	ret = di->ls_ops->get_ls_resistance(&r);
	if (ret < 0)
	{
		hwlog_err("[%s]:error\n", __func__);
		di->scp_stop_charging_flag = 1;
	}
	return r;
}

static int get_ls_vbus(struct direct_charge_device *di)
{
	int vbus = 0;
	int ret;
	ret = di->bi_ops->get_vbus_voltage(&vbus);
	if (ret < 0)
	{
		hwlog_err("[%s]:error\n", __func__);
		di->scp_stop_charging_flag = 1;
	}
	return vbus;
}

static int get_ls_ibus(struct direct_charge_device *di)
{
	int ibus = 0;
	int ret;
	ret = di->bi_ops->get_ls_ibus(&ibus);
	if (ret < 0)
	{
		hwlog_err("[%s]:error\n", __func__);
		di->scp_stop_charging_flag = 1;
	}
	return ibus;
}

static int get_adaptor_voltage(struct direct_charge_device* di)
{
	int adaptor_vol = -1;

	if(di->scp_stop_charging_flag)
		return -1;
	if (di->scp_ops->scp_get_adaptor_voltage)
	{
 		adaptor_vol = di->scp_ops->scp_get_adaptor_voltage();
		if (adaptor_vol < 0)
			di->scp_stop_charging_flag = 1;
	}
	return adaptor_vol;
}

static int get_adaptor_current(struct direct_charge_device* di)
{
	int adaptor_cur = -1;

	if(di->scp_stop_charging_flag)
		return -1;
	if (di->scp_ops->scp_get_adaptor_current)
	{
 		adaptor_cur = di->scp_ops->scp_get_adaptor_current();
		if (adaptor_cur < 0)
			di->scp_stop_charging_flag = 1;
	}
	return adaptor_cur;
}

static int get_adaptor_current_set(struct direct_charge_device* di)
{
	int adaptor_cur_set = -1;

	if(di->scp_stop_charging_flag)
		return -1;
	if (di->scp_ops->scp_get_adaptor_current_set)
	{
 		adaptor_cur_set = di->scp_ops->scp_get_adaptor_current_set();
		if (adaptor_cur_set < 0)
			di->scp_stop_charging_flag = 1;
	}
	return adaptor_cur_set;
}

static void set_adaptor_voltage(struct direct_charge_device* di)
{
	int ret = -1;

	if(di->scp_stop_charging_flag)
		return;
	if (di->scp_ops->scp_set_adaptor_voltage)
	{
		hwlog_info("set_adaptor_vol = %d!\n", di->adaptor_vset);
		if (di->adaptor_vset >= di->max_adaptor_vset)
		{
			di->adaptor_vset = di->max_adaptor_vset;
		}
		ret = di->scp_ops->scp_set_adaptor_voltage(di->adaptor_vset);
		if (ret)
		{
			di->scp_stop_charging_flag = 1;
			hwlog_err("set adaptor vol fail, vol = %d\n", di->adaptor_vset);
		}
	}
}

static void set_adaptor_current(struct direct_charge_device* di)
{
	int ret = -1;

	if(di->scp_stop_charging_flag)
		return;
	if (di->scp_ops->scp_set_adaptor_current)
	{
		hwlog_info("set_adaptor_cur = %d!\n", di->adaptor_iset);
		if (di->adaptor_vset >= di->max_adaptor_iset)
		{
			di->adaptor_vset = di->max_adaptor_iset;
		}
		ret = di->scp_ops->scp_set_adaptor_current(di->adaptor_iset);
		if (ret)
		{
			di->scp_stop_charging_flag = 1;
			hwlog_err("set adaptor cur fail, cur = %d\n", di->adaptor_iset);
		}
	}
}

static int can_battery_do_direct_charge(struct direct_charge_device *di)
{
	int bat_vol = hisi_battery_voltage();
	int bat_temp = hisi_battery_temperature();

	if (bat_vol < di->min_dc_bat_vol || bat_vol > di->max_dc_bat_vol)
	{
		hwlog_info("%s : vol = %d, can not do direct charging \n", __func__, bat_vol);
		return 0;
	}
	if (bat_temp < di->min_dc_bat_temp || bat_temp > di->max_dc_bat_temp)
	{
		hwlog_info("%s : temp = %d, can not do direct charging \n", __func__, bat_temp);
		return 0;
	}
	return	1;
}

static int scp_retry_pre_operate(enum scp_retry_operate_type type, struct direct_charge_device *di)
{
	int ret	= -1;

	switch (type) {
	case SCP_RETRY_OPERATE_RESET_ADAPTER:
		if (NULL != di->scp_ops->scp_adaptor_reset)
		{
			hwlog_info("send scp adapter reset cmd \n");
			ret = di->scp_ops->scp_adaptor_reset();
		}
		else
		{
			ret = -1;
		}
		break;
	case SCP_RETRY_OPERATE_RESET_CHIP:
		if (NULL != di->scp_ops->scp_chip_reset)
		{
			hwlog_info("scp_chip_reset \n");
			ret = di->scp_ops->scp_chip_reset();
			msleep(2000);
		}
		else
		{
			ret = -1;
		}
		break;
	default:
		break;
	}
	return ret;
}

static int restore_normal_charge(struct direct_charge_device* di)
{
	int ret;

	/*no need to check the return val, here when ovp_en set fail ,we do note return*/
	ret = gpio_direction_input(di->ovp_en);
	ret |= gpio_direction_output(di->scp_power_en, 0);
	if (ret)
		ret = FAIL;
	else
		ret = SUCC;
	msleep(100); /*need to wait loadswitch discharge*/
	restore_pluggin_pluggout_interrupt();
	return ret;
}
static int cutoff_normal_charge(struct direct_charge_device* di)
{
	int ret;

	ignore_pluggin_and_pluggout_interrupt();
	ret = gpio_direction_output(di->scp_power_en, 1);
	msleep(100);
	/*no need to check the return val, here when ovp_en set fail ,we still return succ*/
	gpio_direction_output(di->ovp_en, 1);
	if (ret)
		ret = FAIL;
	else
		ret = SUCC;
	return ret;
}

static void scp_adaptor_detect(struct direct_charge_device *di)
{
	int ret;
	int i;

	if (NULL == di || NULL == di->scp_ops || NULL == di->scp_ops->scp_adaptor_detect)
	{
		hwlog_err("[%s]bad scp adaptor detect ops!\n", __func__);
		return;
	}
	ret = di->scp_ops->scp_adaptor_detect();
	if (SCP_ADAPTOR_DETECT_FAIL == ret)
	{
		for (i = 0; i < 3 && SCP_ADAPTOR_DETECT_FAIL == ret; ++i)
		{
			if ((scp_retry_pre_operate(SCP_RETRY_OPERATE_RESET_ADAPTER, di)) < 0)
			{
				hwlog_err("reset adapter failed	\n");
				break;
			}
			ret = di->scp_ops->scp_adaptor_detect();
		}
		if (SCP_ADAPTOR_DETECT_FAIL == ret)
		{
			/* reset scp chip and try again	*/
			if ((scp_retry_pre_operate(SCP_RETRY_OPERATE_RESET_CHIP, di)) == 0)
			{
				ret = di->scp_ops->scp_adaptor_detect();
			}
			else
			{
				hwlog_err("%s : scp_retry_pre_operate failed \n", __func__);
			}
		}
	}
	hwlog_info("%s : scp adaptor detect ret = %d \n", __func__, ret);
	if (SCP_ADAPTOR_DETECT_SUCC == ret)
	{
		ret = di->scp_ops->scp_get_adaptor_info(&(di->adp_info));
		if (ret)
		{
			hwlog_err("get adaptor info failed\n");
		}
		hwlog_info("b_adp_type= 0x%x\n", di->adp_info.b_adp_type);
		hwlog_info("vendor_id_h= 0x%x\n", di->adp_info.vendor_id_h);
		hwlog_info("vendor_id_l= 0x%x\n", di->adp_info.vendor_id_l);
		hwlog_info("module_id_h= 0x%x\n", di->adp_info.module_id_h);
		hwlog_info("module_id_l= 0x%x\n", di->adp_info.module_id_l);
		hwlog_info("serrial_no_h= 0x%x\n", di->adp_info.serrial_no_h);
		hwlog_info("serrial_no_l= 0x%x\n", di->adp_info.serrial_no_l);
		hwlog_info("pchip_id= 0x%x\n", di->adp_info.pchip_id);
		hwlog_info("hwver= 0x%x\n", di->adp_info.hwver);
		hwlog_info("fwver_h= 0x%x\n", di->adp_info.fwver_h);
		hwlog_info("fwver_l= 0x%x\n", di->adp_info.fwver_l);
		scp_set_stage_status(SCP_STAGE_SWITCH_DETECT);
		di->need_open_second_output = di->scp_ops->scp_adaptor_support_second_output();
		di->need_open_second_output &= di->support_second_output;
	}
	/*scp charger but ping fail ,try again in the next loop*/
	if (SCP_ADAPTOR_DETECT_FAIL == ret)
	{
		scp_set_stage_status(SCP_STAGE_DEFAULT);
	}
	/*if detect result is other charger, in the next loop, we will not try again*/
}


static int do_adpator_voltage_accuracy_check(struct direct_charge_device *di)
{
	int adp_vol;
	int vol_err;
	int i;
	char buf[1024] = { 0 };

	di->adaptor_vset = 4400;
	set_adaptor_voltage(di);
	msleep(500);
	for (i = 0; i < 3; ++i)
	{
        	adp_vol = get_adaptor_voltage(di);
		if (adp_vol < 0)
		{
			hwlog_err("get adptor voltage fail!\n");
			return -1;
		}
		vol_err = adp_vol - 4400;
		if (vol_err < 0)
			vol_err = -vol_err;
		hwlog_info("vol_err = %d vol_err_th = %d, vol_set = %d, vol_read = %d!\n", vol_err, di->vol_err_th, di->adaptor_vset, adp_vol);
		if (vol_err > di->vol_err_th)
		{
			snprintf(buf, sizeof(buf), "vol_err(%d) > vol_err_th(%d)\n", vol_err, di->vol_err_th);
			dsm_report(DSM_DIRECT_CHARGE_VOL_ACCURACY, buf);
			return -1;
		}
	}
	return 0;
}
static int do_full_path_resistance_check(struct direct_charge_device *di)
{
	int adp_vol;
	int iadapt;
	int vbus_vol;
	int ibus;
	int delta_vol;
	int r;
	int ret;
	int i;
	int sum = 0;
	char buf[1024] = { 0 };

	msleep(500);
	adp_vol = get_adaptor_voltage(di); /*keep communication with the adaptor within 1 second*/
	if (adp_vol < 0)
	{
		hwlog_err("get adptor voltage fail!\n");
		return -1;
	}
	msleep(400);
	for (i = 0; i < 3; ++i)
	{
		ret = di->bi_ops->get_ls_ibus(&ibus);
		if (ret)
		{
			hwlog_err("[%s]: get ibus fail!\n", __func__);
			return -1;
		}
		ret = di->bi_ops->get_vbus_voltage(&vbus_vol);
		if (ret)
		{
			hwlog_err("[%s]: get vbus vol fail!\n", __func__);
			return -1;
		}
        	adp_vol = get_adaptor_voltage(di);
		if (adp_vol < 0)
		{
			hwlog_err("get adptor voltage fail!\n");
			return -1;
		}
		iadapt = get_adaptor_current(di);
		if (iadapt < 0)
		{
			hwlog_err("get adptor current fail!\n");
			return -1;
		}
		delta_vol = adp_vol - vbus_vol;
		r = delta_vol* 1000 / iadapt;
		hwlog_info("full path resistance = %d adp_vol =%d, iadapt = %d, vbus_vol = %d ibus = %d\n", r, adp_vol, iadapt, vbus_vol, ibus);
		sum += r;
	}
	r = sum / 3;
	di->full_path_resistance = r;
	if (r >= -di->full_path_res_max && r <= di->full_path_res_max)
	{
		return 0;
	}
	hwlog_err("full path resistance = %d is out of[%d, %d]\n", r, -di->full_path_res_max, di->full_path_res_max);
	snprintf(buf, sizeof(buf), "full_path_resistance(%d) is out of[%d, %d]\n", r, di->full_path_res_min, di->full_path_res_max);
	dsm_report(DSM_DIRECT_CHARGE_FULL_PATH_RESISTANCE, buf);
	return -1;

}
static int do_loadswitch_resistance_check(struct direct_charge_device *di)
{
	int r;
	int ret;
	char buf[1024] = { 0 };

	ret = di->ls_ops->get_ls_resistance(&r);
	if (ret)
	{
		hwlog_err("get ls resistance fail!\n");
		return -1;
	}
	hwlog_info("loadswitch resistance = %d\n", r);
	di->loadswitch_resistance = r;
	if (r >= di->ls_resistance_min && r <= di->ls_resistance_max)
	{
		return 0;
	}
	hwlog_err("loadswitch resistance = %d is out of[%d, %d]\n", r, di->ls_resistance_min, di->ls_resistance_max);
	snprintf(buf, sizeof(buf), "full_path_resistance(%d) is out of[%d, %d]\n", r, di->ls_resistance_min, di->ls_resistance_max);
	dsm_report(DSM_DIRECT_CHARGE_LOADSWITCH_RESISTANCE, buf);
	return -1;

}
static int do_usb_port_leakage_current_check(struct direct_charge_device *di)
{
	int iadapt = get_adaptor_current(di);
	char buf[1024] = { 0 };

	hwlog_info("[%s]:iadapt = %d\n", __func__, iadapt);
	if (iadapt < 0)
	{
		hwlog_err("get adptor current fail!\n");
		return -1;
	}
	if (iadapt > di->adaptor_leakage_current_th)
	{
		hwlog_err("idapt(%d) > adaptor_leakage_current_th(%d)\n", iadapt, di->adaptor_leakage_current_th);
		snprintf(buf, sizeof(buf), "idapt(%d) > adaptor_leakage_current_th(%d)\n", iadapt, di->adaptor_leakage_current_th);
		dsm_report(DSM_DIRECT_CHARGE_USB_PORT_LEAKAGE_CURRENT, buf);
		return -1;
	}
	return 0;
}
static int open_direct_charge_path(struct direct_charge_device *di)
{
	int bat_vol;
	int ret;

	bat_vol = get_bat_voltage(di);
	di->adaptor_vset = bat_vol + 300;
	di->adaptor_iset = 1000;
	set_adaptor_voltage(di);
	set_adaptor_current(di);
	ret = di->ls_ops->ls_enable(1);
	if (ret)
	{
		hwlog_err("ls enable fail!\n");
		return -1;
	}
	return 0;
}
static int scp_security_check(struct direct_charge_device *di)
{
	int ret;

	ret = do_usb_port_leakage_current_check(di);
	if (ret)
	{
		hwlog_err("usb port leakage current check fail!\n");
		return -1;
	}
	ret = do_adpator_voltage_accuracy_check(di);
	if (ret)
	{
		hwlog_err("adaptor voltage accuracy check fail!\n");
		return -1;
	}
	ret = open_direct_charge_path(di);
	if (ret)
	{
		hwlog_err("open direct charge path fail!\n");
		return -1;
	}
	ret = do_full_path_resistance_check(di);
	if (ret)
	{
		hwlog_err("full path resiststance check fail!\n");
		return -1;
	}
	ret = do_loadswitch_resistance_check(di);
	if (ret)
	{
		hwlog_err("load switch resiststance check fail!\n");
		return -1;
	}
	return	ret;
}


static void scp_support_check(struct direct_charge_device* di)
{
	/*check	whether	support	scp detect*/
	if (di->scp_ops && di->scp_ops->is_support_scp)
	{
		/*return 0 means support scp*/
		if (di->scp_ops->is_support_scp())
		{
			hwlog_err("not support scp!\n");
			return;
		}
		scp_set_stage_status(SCP_STAGE_ADAPTER_DETECT);
	}
}
static int scp_direct_charge_init(struct direct_charge_device* di)
{
	int ret;

	ret = di->scp_ops->scp_init();
	if (ret)
	{
		hwlog_err("scp init fail!\n");
		return FAIL;
	}
	if (di->need_open_second_output)
	{
		ret = di->scp_ops->scp_open_second_output();
		if (ret)
		{
			hwlog_err("scp open second ouput fail!\n");
			return FAIL;
		}
	}
	ret = di->ls_ops->ls_init();
	if (ret)
	{
		hwlog_err("ls fail!\n");
		return FAIL;
	}
	ret = di->bi_ops->init();
	if (ret)
	{
		hwlog_err("bi init fail!\n");
		return FAIL;
	}
	hwlog_info("direct charge init succ!\n");
	return SUCC;
}

static void scp_stop_charging(void)
{
	int ret;
	struct direct_charge_device *di = g_di;
	int vbus_vol = 0;

	ret = di->ls_ops->ls_enable(0);
	if (ret)
	{
		hwlog_err("[%s]: ls enable fail!\n", __func__);
	}
	if (FAIL == restore_normal_charge(di))
	{
		hwlog_info("%s: restore normal charge fail!\n", __func__);
	}
	ret = di->bi_ops->get_vbus_voltage(&vbus_vol);
	if (ret)
	{
		hwlog_err("[%s]: get vbus vol fail!\n", __func__);
	}
	hwlog_info("%s: vbus_vol = %d!\n", __func__, vbus_vol);
	if (vbus_vol < 3000)
	{
		di->sysfs_enable_charger = 1;
		hwlog_info("%s:direct charger disconnect!\n", __func__);
		di->full_path_resistance = ERROR_RESISTANCE;
		direct_charger_disconnect_send_uevent();
	}
	else
	{
		hwlog_info("%s:switch to normal charge!\n", __func__);
		ret = di->scp_ops->scp_exit();
		if (ret)
		{
			hwlog_err("[%s]: scp exit fail!\n", __func__);
		}
	}
	ret = di->ls_ops->ls_exit();
	if (ret)
	{
		hwlog_err("[%s]: ls exit fail!\n", __func__);
	}
	ret = di->bi_ops->exit();
	if (ret)
	{
		hwlog_err("[%s]: bi exit fail!\n", __func__);
	}
	if (di->scp_stop_charging_flag)
	{
		scp_set_stage_status(SCP_STAGE_DEFAULT);
	}
	else
	{
		scp_set_stage_status(SCP_STAGE_CHARGE_DONE);
	}
	hrtimer_cancel(&di->threshold_caculation_timer);
	cancel_work_sync(&di->threshold_caculation_work);
	di->scp_stop_charging_flag = 0;
	di->cur_stage = 0;
	di->pre_stage = 0;
	di->vbat = 0;
	di->ibat = 0;
	di->vadapt = 0;
	di->iadapt = 0;
	di->ls_vbus = 0;
	di->ls_ibus = 0;
	direct_charge_wake_unlock();
}

static int direct_charge_parse_dts(struct device_node* np, struct direct_charge_device* di)
{
	int ret = 0;
	int i = 0;
	int array_len = 0;
	int idata = 0;
	const char *chrg_data_string = NULL;

	di->ovp_en = of_get_named_gpio(np, "ovp_en", 0);
	hwlog_info("ovp_en = %d\n", di->ovp_en);
	if (!gpio_is_valid(di->ovp_en))
	{
		hwlog_err("ovp_en is not valid\n");
		//ret |= -1;
	}
	di->scp_power_en = of_get_named_gpio(np, "scp_power_en", 0);
	hwlog_info("scp_power_en = %d\n", di->scp_power_en);
	if (!gpio_is_valid(di->scp_power_en))
	{
		hwlog_err("%s: get scp_power_en fail\n", __func__);
		ret |= -1;
	}
	ret |= of_property_read_u32(np, "vol_err_th", &(di->vol_err_th));
	if (ret)
	{
		hwlog_err("get vol_err_th failed\n");
	}
	hwlog_info("vol_err_th = %d\n", di->vol_err_th);
	ret |= of_property_read_u32(np, "full_path_res_min", &(di->full_path_res_min));
	if (ret)
	{
		hwlog_err("get full_path_res_min failed\n");
	}
	hwlog_info("full_path_res_min = %d\n", di->full_path_res_min);
	ret |= of_property_read_u32(np, "full_path_res_max", &(di->full_path_res_max));
	if (ret)
	{
		hwlog_err("get full_path_res_max failed\n");
	}
	hwlog_info("full_path_res_max = %d\n", di->full_path_res_max);
	ret |= of_property_read_u32(np, "ls_resistance_min", &(di->ls_resistance_min));
	if (ret)
	{
		hwlog_err("get ls_resistance_min failed\n");
	}
	hwlog_info("ls_resistance_min = %d\n", di->ls_resistance_min);
	ret |= of_property_read_u32(np, "ls_resistance_max", &(di->ls_resistance_max));
	if (ret)
	{
		hwlog_err("get ls_resistance_max failed\n");
	}
	hwlog_info("ls_resistance_max = %d\n", di->ls_resistance_max);
	ret |= of_property_read_u32(np, "adaptor_leakage_current_th", &(di->adaptor_leakage_current_th));
	if (ret)
	{
		hwlog_err("get adaptor_leakage_current_th failed\n");
	}
	hwlog_info("adaptor_leakage_current_th = %d\n", di->adaptor_leakage_current_th);
	ret |= of_property_read_u32(np, "compensate_r", &(di->compensate_r));
	if (ret)
	{
		hwlog_err("get compensate_r failed\n");
	}
	hwlog_info("compensate_r = %d\n", di->compensate_r);
	ret |= of_property_read_u32(np, "support_second_output", &(di->support_second_output));
	if (ret)
	{
		hwlog_err("get support_second_output failed\n");
	}
	hwlog_info("support_second_output = %d\n", di->support_second_output);
	ret |= of_property_read_u32(np, "max_dc_bat_vol", &(di->max_dc_bat_vol));
	if (ret)
	{
		hwlog_err("get max_dc_bat_vol failed\n");
	}
	hwlog_info("max_dc_bat_vol = %d\n", di->max_dc_bat_vol);
	ret |= of_property_read_u32(np, "min_dc_bat_vol", &(di->min_dc_bat_vol));
	if (ret)
	{
		hwlog_err("get min_dc_bat_vol failed\n");
	}
	hwlog_info("min_dc_bat_vol = %d\n", di->min_dc_bat_vol);
	ret |= of_property_read_u32(np, "max_dc_bat_temp", &(di->max_dc_bat_temp));
	if (ret)
	{
		hwlog_err("get max_dc_bat_temp failed\n");
	}
	hwlog_info("max_dc_bat_temp = %d\n", di->max_dc_bat_temp);
	ret |= of_property_read_u32(np, "min_dc_bat_temp", &(di->min_dc_bat_temp));
	if (ret)
	{
		hwlog_err("get min_dc_bat_temp failed\n");
	}
	hwlog_info("min_dc_bat_temp = %d\n", di->min_dc_bat_temp);
	ret |= of_property_read_u32(np, "max_adaptor_vset", &(di->max_adaptor_vset));
	if (ret)
	{
		hwlog_err("get max_adaptor_vset failed\n");
	}
	hwlog_info("max_adaptor_vset = %d\n", di->max_adaptor_vset);
	ret |= of_property_read_u32(np, "max_adaptor_iset", &(di->max_adaptor_iset));
	if (ret)
	{
		hwlog_err("get max_adaptor_iset failed\n");
	}
	hwlog_info("max_adaptor_iset = %d\n", di->max_adaptor_iset);
	ret |= of_property_read_u32(np, "charge_control_interval", &(di->charge_control_interval));
	if (ret)
	{
		hwlog_err("get charge_control_interval failed\n");
	}
	hwlog_info("charge_control_interval = %d\n", di->charge_control_interval);
	ret |= of_property_read_u32(np, "threshold_caculation_interval", &(di->threshold_caculation_interval));
	if (ret)
	{
		hwlog_err("get threshold_caculation_interval failed\n");
	}
	hwlog_info("threshold_caculation_interval = %d\n", di->threshold_caculation_interval);
	ret |= of_property_read_u32(np, "vstep", &(di->vstep));
	if (ret)
	{
		hwlog_err("get vstep failed\n");
	}
	hwlog_info("vstep = %d\n", di->vstep);
	ret |= of_property_read_u32(np, "delta_err", &(di->delta_err));
	if (ret)
	{
		hwlog_err("get delta_err failed\n");
	}
	hwlog_info("delta_err = %d\n", di->delta_err);
	array_len = of_property_count_strings(np, "dc_para");
	di->stage_size = array_len / DC_PARA_TOTAL;
	hwlog_info("stage_size = %d\n", di->stage_size);
	if ((array_len <= 0) || (array_len % DC_PARA_TOTAL != 0))
	{
		hwlog_err("dc_para is invaild,please check dc_para number!!\n");
		ret |= -EINVAL;
	}
	if (array_len > DC_PARA_LEVEL * DC_PARA_TOTAL)
	{
		array_len = DC_PARA_LEVEL * DC_PARA_TOTAL;
		hwlog_err("dc_para is too long,use only front %d paras!!\n", array_len);
		ret |= -EINVAL;
	}
	for (i = 0; i < array_len; i++)
	{
		ret = of_property_read_string_index(np, "dc_para", i, &chrg_data_string);
		if (ret)
		{
			hwlog_err("get dc_para failed\n");
			ret |= -EINVAL;
		}

		idata = simple_strtol(chrg_data_string, NULL, 10);
		switch (i % DC_PARA_TOTAL) {
		case DC_PARA_VOL_TH:
			if (idata < 0)
			{
				hwlog_err ("the dc_para vol_th is out of range!!\n");
				ret |= -EINVAL;
			}
			di->dc_para[i / (DC_PARA_TOTAL)].vol_th = idata;
			break;
		case DC_PARA_CUR_TH_HIGH:
			if (idata < 0)
			{
				hwlog_err ("the dc_para cur_th_high is out of range!!\n");
				ret |= -EINVAL;
			}
			di->dc_para[i / (DC_PARA_TOTAL)].cur_th_high = idata;
			break;
		case DC_PARA_CUR_TH_LOW:
			if (idata < 0)
			{
				hwlog_err ("the dc_para cur_th_low is out of range!!\n");
				ret |= -EINVAL;
			}
			di->dc_para[i / (DC_PARA_TOTAL)].cur_th_low = idata;
			break;
		default:
			hwlog_err("get dc_para failed\n");
			ret |= -EINVAL;
			break;
		}
		hwlog_info("di->dc_para[%d][%d] = %d\n", i / (DC_PARA_TOTAL), i % (DC_PARA_TOTAL), idata);
	}
	return ret;
}
static void select_direct_charge_stage(struct direct_charge_device* di)
{
	int i;
	int stage_size = di->stage_size;
	int vol = get_bat_voltage(di);
	int cur = get_bat_current(di);

	di->pre_stage = di->cur_stage;
	for (i = 0; i < stage_size; ++i)
	{
		if (vol < di->dc_para[i].vol_th)
		{
			break;
		}
	}
	if (0 == i)
	{
		di->cur_stage = 0;
	}
	else
	{
		if (cur < di->dc_para[i - 1].cur_th_low)
		{
			di->cur_stage = 2*i;
		}
		else
		{
			di->cur_stage = 2*i -1;
		}
	}
	/*stage can not go back*/
	if (di->cur_stage < di->pre_stage)
	{
		di->cur_stage = di->pre_stage;
	}
}
static void select_direct_charge_param(struct direct_charge_device* di)
{
	int cur_th_high;
	int bat_temp = hisi_battery_temperature();
	int vbat = get_bat_voltage(di);
	int ibat = get_bat_current(di);

	di->vbat = vbat;
	di->ibat = ibat;
	di->cur_vbat_th = di->dc_para[di->cur_stage/2].vol_th + ibat*di->compensate_r/1000;
	cur_th_high = di->dc_para[di->cur_stage/2].cur_th_high;
	di->cur_ibat_th_high = cur_th_high > di->sysfs_iin_thermal ? di->sysfs_iin_thermal: cur_th_high;
	di->cur_ibat_th_low = di->dc_para[di->cur_stage/2].cur_th_low;
}

static void direct_charge_regulation(struct direct_charge_device* di)
{
	int ibus;
	int vbat = di->vbat;
	int ibat = di->ibat;
	int vbat_sh = di->cur_vbat_th;
	int ibat_sh_high = di->cur_ibat_th_high;
	int ibat_sh_low = di->cur_ibat_th_low;
	int iadapt = get_adaptor_current(di);
	int iadapt_set = get_adaptor_current_set(di);

	di->vadapt = get_adaptor_voltage(di);
	di->iadapt = iadapt;
	di->ls_ibus = get_ls_ibus(di);
	di->ls_vbus = get_ls_vbus(di);

	hwlog_info("cur_stage = %d vbat = %d ibat = %d vbat_sh = %d ibat_sh_high = %d ibat_sh_low = %d vadp = %d iadap = %d ls_vbus = %d ls_ibus = %d!\n",
			di->cur_stage, vbat, ibat, vbat_sh, ibat_sh_high, ibat_sh_low, di->vadapt, iadapt, di->ls_vbus, di->ls_ibus);
	if (di->cur_stage % 2)
	{
		if (vbat > vbat_sh)
		{
			di->adaptor_vset += vbat_sh - vbat;
			set_adaptor_voltage(di);
			return;
		}
		if (iadapt > ibat_sh_high)
		{
			di->adaptor_vset -= di->vstep;
			set_adaptor_voltage(di);
			return;
		}
		if (ibat > ibat_sh_high)
		{
			di->adaptor_vset -= di->vstep;
			set_adaptor_voltage(di);
			return;
		}
		if (ibat > ibat_sh_high - di->delta_err)
		{
			hwlog_info("do nothing!\n");
			return;
		}
		if (di->adaptor_iset < ibat_sh_high - 1000)
		{
			di->adaptor_iset += 1000;
			set_adaptor_current(di);
			return;
		}
		else if (di->adaptor_iset < ibat_sh_high)
		{
			di->adaptor_iset = ibat_sh_high;
			set_adaptor_current(di);
			return;
		}
		else
		{
			if (iadapt < ibat_sh_high - di->delta_err)
			{
				di->adaptor_vset += di->vstep;
				set_adaptor_voltage(di);
				return;
			}
		}
	}
	else
	{
		if (iadapt > ibat_sh_high)
		{
			di->adaptor_vset -= di->vstep;
			set_adaptor_voltage(di);
			return;
		}
		if (ibat > ibat_sh_high)
		{
			di->adaptor_vset -= di->vstep;
			set_adaptor_voltage(di);
			return;
		}
		if (ibat > ibat_sh_high - di->delta_err)
		{
			hwlog_info("do nothing!\n");
			return;
		}
		if (di->adaptor_iset < ibat_sh_high - 1000)
		{
			di->adaptor_iset += 1000;
			set_adaptor_current(di);
			return;
		}
		else if (di->adaptor_iset < ibat_sh_high)
		{
			di->adaptor_iset = ibat_sh_high;
			set_adaptor_current(di);
			return;
		}
		else
		{
			if (iadapt < ibat_sh_high - di->delta_err)
			{
				di->adaptor_vset += di->vstep;
				set_adaptor_voltage(di);
				return;
			}
		}

	}
}

static void charge_control_work(struct work_struct *work)
{
	struct direct_charge_device *di = container_of(work,struct direct_charge_device, charge_control_work);
	int interval = di->charge_control_interval;

	if (0 == can_battery_do_direct_charge(di))
	{
		di->scp_stop_charging_flag = 1;
	}

	if (di->scp_stop_charging_flag || (0 == di->sysfs_enable_charger))
	{
		hwlog_info("direct charge stop!\n");
		scp_stop_charging();
		return;
	}
	if (2*di->stage_size == di->cur_stage)
	{
		hwlog_info("direct charge done!\n");
		scp_stop_charging();
		return;
	}
	direct_charge_regulation(di);

	hrtimer_start(&di->charge_control_timer, ktime_set(interval/MSEC_PER_SEC, (interval % MSEC_PER_SEC) * USEC_PER_SEC), HRTIMER_MODE_REL);
}

static void threshold_caculation_work(struct work_struct *work)
{
	struct direct_charge_device *di = container_of(work,struct direct_charge_device, threshold_caculation_work);
	int interval = di->threshold_caculation_interval;

	if (di->scp_stop_charging_flag || (0 == di->sysfs_enable_charger))
	{
		hwlog_info("direct charge stop, stop threshold_caculation!\n");
		return;
	}
	select_direct_charge_stage(di);
	select_direct_charge_param(di);
	if (2*di->stage_size == di->cur_stage)
	{
		hwlog_info("direct charge done, stop threshold_caculation!\n");
		return;
	}
	hrtimer_start(&di->threshold_caculation_timer, ktime_set(interval/MSEC_PER_SEC, (interval % MSEC_PER_SEC) * USEC_PER_SEC), HRTIMER_MODE_REL);
}

static enum hrtimer_restart threshold_caculation_timer_func(struct hrtimer *timer)
{
	struct direct_charge_device *di;

	di = container_of(timer, struct direct_charge_device, threshold_caculation_timer);
	queue_work(di->direct_charge_wq, &di->threshold_caculation_work);
	return HRTIMER_NORESTART;
}

static enum hrtimer_restart charge_control_timer_func(struct hrtimer *timer)
{
	struct direct_charge_device *di;

	di = container_of(timer, struct direct_charge_device, charge_control_timer);
	queue_work(di->direct_charge_wq, &di->charge_control_work);
	return HRTIMER_NORESTART;
}

static void scp_start_charging(struct direct_charge_device *di)
{
	int interval;

	hwlog_info("%s \n",__func__);
	direct_charge_wake_lock();
	select_direct_charge_stage(di);
	scp_set_stage_status(SCP_STAGE_CHARGING);
	select_direct_charge_param(di);
	interval = di->charge_control_interval;
	hrtimer_start(&di->charge_control_timer, ktime_set(interval/MSEC_PER_SEC, (interval % MSEC_PER_SEC) * USEC_PER_SEC), HRTIMER_MODE_REL);
	interval = di->threshold_caculation_interval;
	hrtimer_start(&di->threshold_caculation_timer, ktime_set(interval/MSEC_PER_SEC, (interval % MSEC_PER_SEC) * USEC_PER_SEC), HRTIMER_MODE_REL);
}

void direct_charge_check(void)
{
	struct direct_charge_device *di = g_di;

	if (INVALID == is_direct_charge_ops_valid(di))
	{
		hwlog_err("%s : NULL pointer \n", __func__);
		return;
	}
	if (0 == di->sysfs_enable_charger)
	{
		hwlog_info("%s direct_charge is disabled\n",__func__);
		return;
	}

	if (SCP_STAGE_DEFAULT == scp_get_stage_status())
	{
		scp_support_check(di);
		if (SCP_STAGE_ADAPTER_DETECT == scp_get_stage_status())
		{
			scp_adaptor_detect(di);
		}
	}
	if(SCP_STAGE_SWITCH_DETECT == scp_get_stage_status())
	{
		if (can_battery_do_direct_charge(di))
		{
			if (SUCC == cutoff_normal_charge(di))
			{
				scp_set_stage_status(SCP_STAGE_CHARGE_INIT);
			}
			else
			{
				hwlog_err("%s : switch to scp failed \n", __func__);
				di->scp_stop_charging_flag =1;
				scp_stop_charging();
			}
		}
		else
		{
			hwlog_err("not ready for switching to scp, try next loop!\n");
		}
	}
	if (SCP_STAGE_CHARGE_INIT == scp_get_stage_status())
	{
		if (SUCC == scp_direct_charge_init(di))
		{
			scp_set_stage_status(SCP_STAGE_SECURITY_CHECK);
		}
		else
		{
			hwlog_err("%s : direct_charge init failed \n", __func__);
			di->scp_stop_charging_flag =1;
			scp_stop_charging();
		}
	}
	if (SCP_STAGE_SECURITY_CHECK == scp_get_stage_status())
	{
		if (0 == scp_security_check(di))
		{
			scp_set_stage_status(SCP_STAGE_SUCCESS);
		}
		else
		{
			hwlog_err("%s : scp security check fail	\n", __func__);
			di->scp_stop_charging_flag =1;
			scp_stop_charging();
		}
	}
	if (SCP_STAGE_SUCCESS == scp_get_stage_status())
	{
		scp_start_charging(di);
	}
	if (SCP_STAGE_CHARGING == scp_get_stage_status())
	{
		hwlog_info("%s : in scp charging process\n", __func__);
	}
	hwlog_info("[%s]scp stage  %s !!! \n", __func__, scp_check_stage[scp_get_stage_status()]);
}

void direct_charge_stop_charging(void)
{
	struct direct_charge_device *di = g_di;
	if(NULL == di)
		return;

	scp_set_stage_status(SCP_STAGE_DEFAULT);
	di->scp_stop_charging_flag = 0;
	di->cur_stage = 0;
	di->pre_stage = 0;
	di->vbat = 0;
	di->ibat = 0;
	di->vadapt = 0;
	di->iadapt = 0;
	di->ls_vbus = 0;
	di->ls_ibus = 0;
	di->sysfs_enable_charger = 1;
}
#if CONFIG_SYSFS
#define DIRECT_CHARGE_SYSFS_FIELD(_name, n, m, store)                \
{                                                   \
    .attr = __ATTR(_name, m, direct_charge_sysfs_show, store),    \
    .name = DIRECT_CHARGE_SYSFS_##n,          \
}

#define DIRECT_CHARGE_SYSFS_FIELD_RW(_name, n)               \
	DIRECT_CHARGE_SYSFS_FIELD(_name, n, S_IWUSR | S_IRUGO, direct_charge_sysfs_store)

#define DIRECT_CHARGE_SYSFS_FIELD_RO(_name, n)               \
	DIRECT_CHARGE_SYSFS_FIELD(_name, n, S_IRUGO, NULL)

static ssize_t direct_charge_sysfs_show(struct device *dev,
				 struct device_attribute *attr, char *buf);
static ssize_t direct_charge_sysfs_store(struct device *dev,
				  struct device_attribute *attr,
				  const char *buf, size_t count);

struct direct_charge_sysfs_field_info {
	struct device_attribute attr;
	u8 name;
};

static struct direct_charge_sysfs_field_info direct_charge_sysfs_field_tbl[] = {
	DIRECT_CHARGE_SYSFS_FIELD_RW(enable_charger, ENABLE_CHARGER),
	DIRECT_CHARGE_SYSFS_FIELD_RW(iin_thermal, IIN_THERMAL),
	DIRECT_CHARGE_SYSFS_FIELD_RO(adaptor_detect, ADAPTOR_DETECT),
	DIRECT_CHARGE_SYSFS_FIELD_RO(loadswitch_id, LOADSWITCH_ID),
	DIRECT_CHARGE_SYSFS_FIELD_RO(loadswitch_name, LOADSWITCH_NAME),
	DIRECT_CHARGE_SYSFS_FIELD_RO(vbat, VBAT),
	DIRECT_CHARGE_SYSFS_FIELD_RO(ibat, IBAT),
	DIRECT_CHARGE_SYSFS_FIELD_RO(vadapt, VADAPT),
	DIRECT_CHARGE_SYSFS_FIELD_RO(iadapt, IADAPT),
	DIRECT_CHARGE_SYSFS_FIELD_RO(ls_vbus, LS_VBUS),
	DIRECT_CHARGE_SYSFS_FIELD_RO(ls_ibus, LS_IBUS),
	DIRECT_CHARGE_SYSFS_FIELD_RO(full_path_resistance, FULL_PATH_RESISTANCE),
	DIRECT_CHARGE_SYSFS_FIELD_RO(loadswitch_resistance, LOADSWITCH_RESISTANCE),
};

static struct attribute *direct_charge_sysfs_attrs[ARRAY_SIZE(direct_charge_sysfs_field_tbl) + 1];

static const struct attribute_group direct_charge_sysfs_attr_group = {
	.attrs = direct_charge_sysfs_attrs,
};

/**********************************************************
*  Function:       direct_charge_sysfs_init_attrs
*  Description:    initialize direct_charge_sysfs_attrs[] for direct_charge attribute
*  Parameters:   NULL
*  return value:  NULL
**********************************************************/
static void direct_charge_sysfs_init_attrs(void)
{
	int i, limit = ARRAY_SIZE(direct_charge_sysfs_field_tbl);

	for (i = 0; i < limit; i++) {
		direct_charge_sysfs_attrs[i] = &direct_charge_sysfs_field_tbl[i].attr.attr;
	}
	direct_charge_sysfs_attrs[limit] = NULL;	/* Has additional entry for this */
}

/**********************************************************
*  Function:       direct_charge_sysfs_field_lookup
*  Description:    get the current device_attribute from direct_charge_sysfs_field_tbl by attr's name
*  Parameters:   name:device attribute name
*  return value:  direct_charge_sysfs_field_tbl[]
**********************************************************/
static struct direct_charge_sysfs_field_info *direct_charge_sysfs_field_lookup(const char *name)
{
	int i, limit = ARRAY_SIZE(direct_charge_sysfs_field_tbl);

	for (i = 0; i < limit; i++) {
		if (!strncmp
		    (name, direct_charge_sysfs_field_tbl[i].attr.attr.name,
		     strlen(name)))
			break;
	}
	if (i >= limit)
		return NULL;

	return &direct_charge_sysfs_field_tbl[i];
}
/**********************************************************
*  Function:       direct_charge_sysfs_show
*  Description:    show the value for all direct charge device's node
*  Parameters:   dev:device
*                      attr:device_attribute
*                      buf:string of node value
*  return value:  0-sucess or others-fail
**********************************************************/
static ssize_t direct_charge_sysfs_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	struct direct_charge_sysfs_field_info *info = NULL;
	struct direct_charge_device *di = dev_get_drvdata(dev);
	enum usb_charger_type type = charge_get_charger_type();
	int ret;

	info = direct_charge_sysfs_field_lookup(attr->attr.name);
	if (!info)
		return -EINVAL;

	switch (info->name) {
	case DIRECT_CHARGE_SYSFS_ENABLE_CHARGER:
		return snprintf(buf, PAGE_SIZE, "%u\n", di->sysfs_enable_charger);
	case DIRECT_CHARGE_SYSFS_IIN_THERMAL:
		return snprintf(buf, PAGE_SIZE, "%u\n", di->sysfs_iin_thermal);
	case DIRECT_CHARGE_SYSFS_ADAPTOR_DETECT:
		if (INVALID == is_direct_charge_ops_valid(di))
		{
			hwlog_err("(%s)invalid ops\n", __func__);
			return snprintf(buf, PAGE_SIZE, "%u\n", 1);
		}
		if (CHARGER_TYPE_STANDARD != type && CHARGER_REMOVED != type)
		{
			hwlog_err("(%s)chargertype = %u\n", __func__, type);
			return snprintf(buf, PAGE_SIZE, "%u\n", 1);
		}
		if(di->scp_ops->is_support_scp())
		{
			hwlog_err("(%s)not support scp\n", __func__);
			return snprintf(buf, PAGE_SIZE, "%u\n", 1);
		}
		ret = di->scp_ops->scp_adaptor_detect();
		hwlog_info("(%s)adaptor_detect = %d\n", __func__, ret);
		return snprintf(buf, PAGE_SIZE, "%d\n", di->scp_ops->scp_adaptor_detect());
	case DIRECT_CHARGE_SYSFS_LOADSWITCH_ID:
		return snprintf(buf, PAGE_SIZE, "%d\n", di->ls_id);
	case DIRECT_CHARGE_SYSFS_LOADSWITCH_NAME:
		return snprintf(buf, PAGE_SIZE, "%s\n", NULL == di->ls_name ? "ERROR" : di->ls_name);
	case DIRECT_CHARGE_SYSFS_VBAT:
		return snprintf(buf, PAGE_SIZE, "%d\n", di->vbat);
	case DIRECT_CHARGE_SYSFS_IBAT:
		return snprintf(buf, PAGE_SIZE, "%d\n", di->ibat);
	case DIRECT_CHARGE_SYSFS_VADAPT:
		return snprintf(buf, PAGE_SIZE, "%d\n", di->vadapt);
	case DIRECT_CHARGE_SYSFS_IADAPT:
		return snprintf(buf, PAGE_SIZE, "%d\n", di->iadapt);
	case DIRECT_CHARGE_SYSFS_LS_VBUS:
		return snprintf(buf, PAGE_SIZE, "%d\n", di->ls_vbus);
	case DIRECT_CHARGE_SYSFS_LS_IBUS:
		return snprintf(buf, PAGE_SIZE, "%d\n", di->ls_ibus);
	case DIRECT_CHARGE_SYSFS_FULL_PATH_RESISTANCE:
		return snprintf(buf, PAGE_SIZE, "%d\n", di->full_path_resistance);
	case DIRECT_CHARGE_SYSFS_LOADSWITCH_RESISTANCE:
		return snprintf(buf, PAGE_SIZE, "%d\n", di->loadswitch_resistance);
	default:
		hwlog_err("(%s)NODE ERR!!HAVE NO THIS NODE:(%d)\n", __func__, info->name);
		break;
	}
	return 0;
}
/**********************************************************
*  Function:       direct_charge_sysfs_store
*  Description:    set the value for charge_data's node which is can be written
*  Parameters:   dev:device
*                      attr:device_attribute
*                      buf:string of node value
*                      count:unused
*  return value:  0-sucess or others-fail
**********************************************************/
static ssize_t direct_charge_sysfs_store(struct device *dev,
				  struct device_attribute *attr,
				  const char *buf, size_t count)
{
	struct direct_charge_sysfs_field_info *info = NULL;
	struct direct_charge_device *di = dev_get_drvdata(dev);
	long val = 0;

	info = direct_charge_sysfs_field_lookup(attr->attr.name);
	if (!info)
		return -EINVAL;

	switch (info->name) {
	case DIRECT_CHARGE_SYSFS_ENABLE_CHARGER:
		if ((strict_strtol(buf, 10, &val) < 0) || (val < 0) || (val > 1))
			return -EINVAL;
		di->sysfs_enable_charger = val;
		hwlog_info("set enable_charger = %d\n", di->sysfs_enable_charger);
		break;
	case DIRECT_CHARGE_SYSFS_IIN_THERMAL:
		if ((strict_strtol(buf, 10, &val) < 0) || (val < 0) || (val > 8000))
			return -EINVAL;
		hwlog_info("set iin_thermal = %d\n", val);
		if (0 == val)
		{
			di->sysfs_iin_thermal = di->dc_para[0].cur_th_high;
		}
		else if (val < di->dc_para[di->stage_size - 1].cur_th_low)
		{
			hwlog_info("iin_thermal = %d < %d, ignored\n", val, di->dc_para[di->stage_size - 1].cur_th_low);
			return -EINVAL;
		}
		else
		{
			di->sysfs_iin_thermal = val;
		}
		break;
	default:
		hwlog_err("(%s)NODE ERR!!HAVE NO THIS NODE:(%d)\n", __func__, info->name);
		break;
	}
	return count;
}

/**********************************************************
*  Function:       direct_charge_sysfs_create_group
*  Description:    create the direct charge device sysfs group
*  Parameters:   di:direct_charge_device
*  return value:  0-sucess or others-fail
**********************************************************/
static int direct_charge_sysfs_create_group(struct direct_charge_device *di)
{
	direct_charge_sysfs_init_attrs();
	return sysfs_create_group(&di->dev->kobj, &direct_charge_sysfs_attr_group);
}

/**********************************************************
*  Function:       direct_charge_sysfs_remove_group
*  Description:    remove the direct_charge device sysfs group
*  Parameters:   di:direct_charge_device
*  return value:  NULL
**********************************************************/
static inline void direct_charge_sysfs_remove_group(struct direct_charge_device *di)
{
	sysfs_remove_group(&di->dev->kobj, &direct_charge_sysfs_attr_group);
}
#else
static int direct_charge_sysfs_create_group(struct direct_charge_device *di)
{
	return 0;
}

static inline void direct_charge_sysfs_remove_group(struct direct_charge_device *di)
{
}
#endif
static int direct_charge_probe(struct platform_device	*pdev)
{
	int ret = 0;
	struct direct_charge_device *di;
	struct class *power_class =	NULL;
	struct device_node *np = NULL;

	di = kzalloc(sizeof(*di), GFP_KERNEL);
	if (!di)
	{
		hwlog_err("alloc di failed\n");
		return	-ENOMEM;
	}
	di->dev	= &pdev->dev;
	np = di->dev->of_node;
	ret = direct_charge_parse_dts(np, di);
	if (ret)
	{
		hwlog_err("parse dts fail\n");
		goto fail_0;
	}
	di->scp_ops = g_scp_ops;
	di->ls_ops = g_ls_ops;
	di->bi_ops = g_bi_ops;
	di->sysfs_enable_charger = 1;
	di->sysfs_iin_thermal = di->dc_para[0].cur_th_high;
	if ((NULL == di->scp_ops) || (NULL == di->ls_ops) || (NULL == di->bi_ops))
	if (INVALID == is_direct_charge_ops_valid(di))
	{
		hwlog_err("direct charge ops is	NULL!\n");
		ret = -EINVAL;
		goto fail_0;
	}
	ret = gpio_request(di->ovp_en, "ovp_en");
	if (ret)
	{
		hwlog_err("could not request ovp_en\n");
		//ret = -ENOMEM;
		//goto fail_0;
	}
	ret = gpio_request(di->scp_power_en, "scp_power_en");
	if (ret)
	{
		hwlog_err("can not request scp_power_en\n");
		goto free_ovp_en;
	}
	ret = gpio_direction_input(di->ovp_en);/*avoid ovp_en to hiz mode*/
	if (ret)
	{
		hwlog_err("could not set ovp_en input\n");
		//ret = -ENOMEM;
		//goto free_scp_power_en;
	}

	platform_set_drvdata(pdev, di);


	di->direct_charge_wq = create_singlethread_workqueue("direct_charge_wq");

	wake_lock_init(&direct_charge_lock, WAKE_LOCK_SUSPEND, "direct_charge_wakelock");
	INIT_WORK(&di->threshold_caculation_work, threshold_caculation_work);
	INIT_WORK(&di->charge_control_work, charge_control_work);


	hrtimer_init(&di->threshold_caculation_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	di->threshold_caculation_timer.function	= threshold_caculation_timer_func;

	hrtimer_init(&di->charge_control_timer,	CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	di->charge_control_timer.function = charge_control_timer_func;

	ret = direct_charge_sysfs_create_group(di);
	if (ret)
		hwlog_err("can't create	charge sysfs entries\n");
	power_class = hw_power_get_class();
	if (power_class)
	{
		if (charge_dev == NULL)
			charge_dev = device_create(power_class,	NULL, 0, NULL, "charger");
		ret = sysfs_create_link(&charge_dev->kobj, &di->dev->kobj, "direct_charger");
		if (ret)
		{
			hwlog_err("create link to direct_charger fail.\n");
			goto free_scp_power_en;
		}
	}
	g_di = di;

#ifdef CONFIG_HUAWEI_HW_DEV_DCT
	ret = di->ls_ops->ls_init();
	di->ls_id = di->ls_ops->get_ls_id();
	if (di->ls_id < 0 || di->ls_id >= LOADSWITCH_TOTAL)
	{
		hwlog_err("error loadswitch id info\n");
		di->ls_id = LOADSWITCH_TOTAL;
	}
	di->ls_name = loadswitch_name[di->ls_id];
	hwlog_info("loadswitch id = %d(%s)\n", di->ls_id, di->ls_name);
	ret |= di->ls_ops->ls_exit();
	if (ret)
	{
		hwlog_err("dev_check for loadswitch fail.\n");
	}
	else
	{
		set_hw_dev_flag(DEV_I2C_LOADSWITCH);
		hwlog_info("dev_check for loadswitch succ.\n");
	}
#endif
	hwlog_info("direct charger probe ok!\n");
	return	0;

free_scp_power_en:
	gpio_free(di->scp_power_en);
free_ovp_en:
	gpio_free(di->ovp_en);
fail_0:
	kfree(di);
	di = NULL;
	return	ret;
}
/**********************************************************
*	Function: direct_charge_remove
*	Description: direct_charge module remove
*	Parameters: pdev:platform_device
*	return value: NULL
**********************************************************/
static int direct_charge_remove(struct platform_device *pdev)
{
	struct direct_charge_device *di = platform_get_drvdata(pdev);

	hwlog_info("%s ++\n", __func__);
	if (NULL == di)
	{
		hwlog_err("[%s]di is NULL!\n", __func__);
		return -ENODEV;
	}

	hwlog_info("%s	--\n", __func__);

	return 0;
}
/**********************************************************
*	Function: direct_charge_shutdown
*	Description: direct_charge module shutdown
*	Parameters: pdev:platform_device
*	return value: NULL
**********************************************************/
static void direct_charge_shutdown(struct platform_device *pdev)
{
	struct direct_charge_device *di = platform_get_drvdata(pdev);

	hwlog_info("%s ++\n", __func__);
	if (NULL == di)
	{
		hwlog_err("[%s]di is NULL!\n", __func__);
		return;
	}

	hwlog_info("%s	--\n", __func__);

	return;
}

#ifdef	CONFIG_PM
/**********************************************************
*	Function: direct_charge_suspend
*	Description: direct charge module suspend
*	Parameters: pdev:platform_device
*		    state:unused
*	return	value: 0-sucess	or others-fail
**********************************************************/
static int direct_charge_suspend(struct	platform_device	*pdev, pm_message_t state)
{
	hwlog_info("%s ++\n", __func__);

	hwlog_info("%s --\n", __func__);

	return	0;
}

/**********************************************************
*	Function: direct charge_resume
*	Description: direct charge module resume
*	Parameters: pdev:platform_device
*	return	value: 0-sucess	or others-fail
**********************************************************/
static int direct_charge_resume(struct	platform_device	*pdev)
{
	hwlog_info("%s	++\n", __func__);

	hwlog_info("%s --\n", __func__);

	return	0;
}
#endif	/* CONFIG_PM */

static struct of_device_id direct_charge_match_table[] = {
	{
		.compatible = "direct_charger",
		.data = NULL,
	},
	{
	},
};

static struct platform_driver direct_charge_driver = {
	.probe = direct_charge_probe,
	.remove	= direct_charge_remove,
#ifdef	CONFIG_PM
	.suspend = direct_charge_suspend,
	.resume	= direct_charge_resume,
#endif
	.shutdown = direct_charge_shutdown,
	.driver	= {
		.name = "direct_charger",
		.owner = THIS_MODULE,
		.of_match_table	= of_match_ptr(direct_charge_match_table),
	},
};

/**********************************************************
*	Function: direct_charge_init
*	Description: direct charge module initialization
*	Parameters: NULL
*	return value: 0-sucess or others-fail
**********************************************************/
static int __init direct_charge_init(void)
{
	return platform_driver_register(&direct_charge_driver);
}

/**********************************************************
*	Function: direct_charge_exit
*	Description: direct charge module exit
*	Parameters: NULL
*	return	value:	NULL
**********************************************************/
static void __exit direct_charge_exit(void)
{
	platform_driver_unregister(&direct_charge_driver);
}

late_initcall(direct_charge_init);
module_exit(direct_charge_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("direct charger module driver");
MODULE_AUTHOR("HUAWEI Inc");
