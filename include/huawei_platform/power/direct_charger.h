#ifndef __DIRECT_CHARGER_H_
#define __DIRECT_CHARGER_H_

#include <linux/module.h>
#include <linux/device.h>
#include <huawei_platform/power/huawei_charger.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/timer.h>
#include <linux/hrtimer.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#ifndef BIT
#define BIT(x) (1 << x)
#endif
/*scp adaptor register*/
#define SCP_ADP_TYPE 0x80
#define SCP_ADP_TYPE_B_MASK (BIT(5)|BIT(4))
#define SCP_ADP_TYPE_B 0x10

#define SCP_B_ADP_TYPE 0x81
#define SCP_B_DIRECT_ADP 0x10

#define SCP_VENDOR_ID_H 0x82
#define SCP_VENDOR_ID_L 0x83
#define SCP_MODULE_ID_H 0x84
#define SCP_MODULE_ID_L 0x85
#define SCP_SERRIAL_NO_H 0x86
#define SCP_SERRIAL_NO_L 0x87
#define SCP_PCHIP_ID 0x88
#define SCP_HWVER 0x89
#define SCP_FWVER_H 0x8a
#define SCP_FWVER_L 0x8b
#define SCP_SPID 0x8c
#define SCP_NEED_OPEN_OUTPUT2 0x31
#define SCP_MAX_POWER 0x90
#define SCP_CNT_POWER 0x91
#define SCP_MIN_VOLT 0x92
#define SCP_MAX_VOLT 0x93
#define SCP_MIN_IOLT 0x94
#define SCP_MAX_IOLT 0x95
#define SCP_VSTEP 0x96
#define SCP_ISTEP 0x97
#define SCP_MAX_VERR 0x98
#define SCP_MAX_IVRR 0x99
#define SCP_MAX_STTIME 0x9a
#define SCP_MAX_RSPTIME 0x9b
#define SCP_SCTRL 0x9e
#define SCP_OUTPUT2_ENABLE_MASK BIT(7)
#define SCP_OUTPUT2_ENABLE BIT(7)
#define SCP_CTRL_BYTE0 0xa0
#define SCP_OUTPUT_MODE_MASK BIT(6)
#define SCP_OUTPUT_MODE_ENABLE BIT(6)
#define SCP_OUTPUT_MODE_DISABLE 0
#define SCP_CTRL_BYTE1 0xa1
#define SCP_STATUS_BYTR0 0xa2
#define SCP_STATUS_BYTE1 0xa3
#define SCP_INSIDE_TMP 0xa6
#define SCP_PORT_TMP 0xa7
#define SCP_READ_VOLT_L 0xa8
#define SCP_READ_VOLT_H 0xa9
#define SCP_READ_IOLT_L 0xaa
#define SCP_READ_IOLT_H 0xab
#define SCP_SREAD_VOUT 0xc8
#define SCP_SREAD_VOUT_OFFSET 3000
#define SCP_SREAD_VOUT_STEP 10
#define SCP_SREAD_IOUT 0xc9
#define SCP_SREAD_IOUT_STEP 50
#define SCP_DAC_VSET_L 0xac
#define SCP_DAC_VSET_H 0xad
#define SCP_DAC_ISET_L 0xae
#define SCP_DAC_ISET_H 0xaf
#define SCP_VSET_BOUNDARY_L 0xb0
#define SCP_VSET_BOUNDARY_H 0xb1
#define SCP_ISET_BOUNDARY_L 0xb2
#define SCP_ISET_BOUNDARY_H 0xb3
#define SCP_MAX_VSET_OFFSET 0xb4
#define SCP_MAX_ISET_OFFSET 0xb5
#define SCP_VSET_L 0xb8
#define SCP_VSET_H 0xb9
#define SCP_ISET_L 0xba
#define SCP_ISET_H 0xbb
#define SCP_VSSET 0xca
#define VSSET_OFFSET 3000
#define VSSET_STEP 10
#define SCP_ISSET 0xcb
#define ISSET_STEP 50
#define SCP_VSET_OFFSET_L 0xbc
#define SCP_VSET_OFFSET_H 0xbd
#define SCP_ISET_OFFSET_L 0xbe
#define SCP_ISET_OFFSET_H 0xbf
#define SCP_STEP_VSET_OFFSET 0xcc
#define SCP_STEP_ISET_OFFSET 0xcd

#define SCP_ADAPTOR_DETECT_FAIL 1
#define SCP_ADAPTOR_DETECT_SUCC 0
#define SCP_ADAPTOR_DETECT_OTHER -1

#define ERROR_RESISTANCE -99999

#define DC_PARA_LEVEL             (5)

#define FAIL -1
#define SUCC 0

#define INVALID -1
#define VALID 0

enum scp_retry_operate_type {
	SCP_RETRY_OPERATE_DEFAUTL,
	SCP_RETRY_OPERATE_RESET_ADAPTER,
	SCP_RETRY_OPERATE_RESET_CHIP,
	SCP_RETRY_OPERATE_UNVALID,
};

enum scp_stage_type {
	SCP_STAGE_DEFAULT,
	SCP_STAGE_SUPPORT_DETECT,
	SCP_STAGE_ADAPTER_DETECT,
	SCP_STAGE_SWITCH_DETECT,
	SCP_STAGE_CHARGE_INIT,
	SCP_STAGE_SECURITY_CHECK,
	SCP_STAGE_SUCCESS,
	SCP_STAGE_CHARGING,
	SCP_STAGE_CHARGE_DONE,
};

enum dc_para_info {
	DC_PARA_VOL_TH = 0,
	DC_PARA_CUR_TH_HIGH,
	DC_PARA_CUR_TH_LOW,
	DC_PARA_TOTAL,
};

enum direct_charge_sysfs_type {
	DIRECT_CHARGE_SYSFS_ENABLE_CHARGER = 0,
	DIRECT_CHARGE_SYSFS_IIN_THERMAL,
	DIRECT_CHARGE_SYSFS_ADAPTOR_DETECT,
	DIRECT_CHARGE_SYSFS_LOADSWITCH_ID,
	DIRECT_CHARGE_SYSFS_LOADSWITCH_NAME,
	DIRECT_CHARGE_SYSFS_VBAT,
	DIRECT_CHARGE_SYSFS_IBAT,
	DIRECT_CHARGE_SYSFS_VADAPT,
	DIRECT_CHARGE_SYSFS_IADAPT,
	DIRECT_CHARGE_SYSFS_LS_VBUS,
	DIRECT_CHARGE_SYSFS_LS_IBUS,
	DIRECT_CHARGE_SYSFS_FULL_PATH_RESISTANCE,
	DIRECT_CHARGE_SYSFS_LOADSWITCH_RESISTANCE,
};

enum loadswitch_id {
	LOADSWITCH_RICHTEK,
	LOADSWITCH_TI,
	LOADSWITCH_FAIRCHILD,
	LOADSWITCH_NXP,
	LOADSWITCH_TOTAL,
};
static const char *const loadswitch_name[] = {
	[0] = "RICHTEK",
	[1] = "TI",
	[2] = "FAIRCHILD",
	[3] = "NXP",
	[4] = "ERROR",
};
static const char *const scp_check_stage[] = {
	[0] = "SCP_STAGE_DEFAULT",
	[1] = "SCP_STAGE_SUPPORT_DETECT",
	[2] = "SCP_STAGE_ADAPTER_DETECT",
	[3] = "SCP_STAGE_SWITCH_DETECT",
	[4] = "SCP_STAGE_CHARGE_INIT",
	[5] = "SCP_STAGE_SECURITY_CHECK",
	[6] = "SCP_STAGE_SUCCESS",
	[7] = "SCP_STAGE_CHARGING",
	[8] = "SCP_STAGE_CHARGE_DONE",
};
struct smart_charge_ops {
	int (*is_support_scp)(void);
	int (*scp_init)(void);
	int (*scp_exit)(void);
	int (*scp_adaptor_detect)(void);
	int (*scp_set_adaptor_voltage)(int);
	int (*scp_get_adaptor_voltage)(void);
	int (*scp_set_adaptor_current)(int);
	int (*scp_get_adaptor_current)(void);
	int (*scp_get_adaptor_current_set)(void);
	int (*scp_adaptor_reset)(void);
	int (*scp_chip_reset)(void);
	int (*scp_stop_charge_config)(void);
	int (*is_scp_charger_type)(void);
	int (*scp_get_adaptor_status)(void);
	int (*scp_get_chip_status)(void);
	int (*scp_adaptor_support_second_output)(void);
	int (*scp_open_second_output)(void);
	int (*scp_get_adaptor_info)(void*);
};
struct loadswitch_ops {
	int (*ls_init)(void);
	int (*ls_exit)(void);
	int (*ls_enable)(int);
	int (*get_ls_id)(void);
	int (*get_ls_resistance)(int* r);
};
struct batinfo_ops {
	int (*init)(void);
	int (*exit)(void);
	int (*get_bat_btb_voltage)(void);
	int (*get_bat_package_voltage)(void);
	int (*get_vbus_voltage)(int* vol);
	int (*get_bat_current)(int* cur);
	int (*get_ls_ibus)(int *ibus);
};

/*currently we support 5 cc stage*/
struct direct_charge_para {
	int vol_th;
	int cur_th_high;
	int cur_th_low;
};
struct adaptor_info {
	int b_adp_type;
	int vendor_id_h;
	int vendor_id_l;
	int module_id_h;
	int module_id_l;
	int serrial_no_h;
	int serrial_no_l;
	int pchip_id;
	int hwver;
	int fwver_h;
	int fwver_l;
};
struct direct_charge_device {
	struct device *dev;
	struct smart_charge_ops* scp_ops;
	struct loadswitch_ops* ls_ops;
	struct batinfo_ops* bi_ops;
	struct hrtimer threshold_caculation_timer;
	struct hrtimer charge_control_timer;
	struct workqueue_struct *direct_charge_wq;
	struct work_struct threshold_caculation_work;
	struct work_struct charge_control_work;
	struct direct_charge_para dc_para[DC_PARA_LEVEL];
	struct adaptor_info adp_info;
	int sysfs_enable_charger;
	int sysfs_iin_thermal;
	int support_second_output;
	int need_open_second_output;
	int threshold_caculation_interval;
	int charge_control_interval;
	int cur_stage;
	int cur_vbat_th;
	int cur_ibat_th_high;
	int cur_ibat_th_low;
	int vbat;
	int ibat;
	int vadapt;
	int iadapt;
	int ls_vbus;
	int ls_ibus;
	int full_path_resistance;
	int loadswitch_resistance;
	int stage_size;
	int pre_stage;
	int adaptor_vset;
	int max_adaptor_vset;
	int adaptor_iset;
	int max_adaptor_iset;
	int delta_err;
	int vstep;
	int scp_stop_charging_flag;
	int max_dc_bat_vol;
	int min_dc_bat_vol;
	int max_dc_bat_temp;
	int min_dc_bat_temp;
	int ovp_en;
	int scp_power_en;
	int compensate_r;
	int ls_id;
	char* ls_name;
	int vol_err_th;
	int full_path_res_min;
	int full_path_res_max;
	int ls_resistance_min;
	int ls_resistance_max;
	int adaptor_leakage_current_th;
};

int scp_ops_register(struct smart_charge_ops*);
int loadswitch_ops_register(struct loadswitch_ops*);
int batinfo_ops_register(struct batinfo_ops*);
void direct_charge_check(void);
void direct_charge_stop_charging(void);
#ifdef CONFIG_LLT_TEST
#endif
#endif
