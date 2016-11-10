#ifndef _HW_VOLTAGE_DEV_H_
#define _HW_VOLTAGE_DEV_H_

#include <linux/list.h>
#include <linux/device.h>
#include <huawei_platform/log/hw_log.h>
#include <linux/slab.h>

struct hw_voltage_data {
	int id;
	int state;
	void *data;
	struct list_head list;
	int (*hw_voltage_enable)(void *, int);
	int (*hw_voltage_getstate)(void *, int *);
	int (*hw_voltage_chennel)(void *, struct hw_voltage_info*);
	int (*hw_voltage_getvoltage)(void *, struct hw_voltage_info*);
};

struct hw_voltage_info {
	int id;
	int channel;
	int min_voltage;
	int max_voltage;
};

struct hw_voltage_device {
	int voltage_states;
	struct device *dev;
	struct class *vol_class;
	struct hw_voltage_info info;
	struct list_head head;
	spinlock_t spinlock;
};

struct hw_voltage_data *hw_voltage_register(void *data, int id);
void hw_voltage_unregister(int id);
#endif
