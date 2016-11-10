

#include "hw_voltage_dev.h"

#define HW_VOLTAGE_CHANNELS_PERCHIP		8


#ifdef HWLOG_TAG
#undef HWLOG_TAG
#endif
#define HWLOG_TAG hw_voltage
HWLOG_REGIST();

static struct hw_voltage_device *voltage_dev;

static ssize_t hw_voltage_enable_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct hw_voltage_device *vol_dev = dev_get_drvdata(dev);

	return snprintf(buf, 50, "%d\n", vol_dev->voltage_states);
}

static ssize_t hw_voltage_enable_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	int ret = 0;
	int enable = 0;
	long state = 0;
	struct hw_voltage_device *vol_dev = dev_get_drvdata(dev);
	struct hw_voltage_data *temp_data = NULL;

	ret = kstrtol(buf, 10, &state);
	if (ret < 0) {
		hwlog_err("voltage enable input err. ret:%d\n", ret);
		return ret;
	}

	enable = !!state;
	list_for_each_entry(temp_data, &vol_dev->head, list) {
		ret = temp_data->hw_voltage_enable(temp_data->data, enable);
		if (ret < 0) {
			hwlog_err("enable err; id:%d\n", temp_data->id);
			return ret;
		}
		if (enable)
			vol_dev->voltage_states |= (1 << temp_data->id);
		else
			vol_dev->voltage_states &= ~(1 << temp_data->id);
	}
	hwlog_info("status:%d\n", vol_dev->voltage_states);
	return size;
}

static ssize_t hw_voltage_value_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int channel = 0;
	int result = -1;
	int voltage_mv = 0;
	int min_threshold = 0;
	int max_threshold = 0;
	struct hw_voltage_device *vol_dev = dev_get_drvdata(dev);
	struct hw_voltage_info *info = NULL;
	struct hw_voltage_data *temp_data = NULL;

	if (NULL == vol_dev) {
		hwlog_err("dev data null.\n");
		result = -1;
		goto out;
	}

	info = &vol_dev->info;
	info->min_voltage = 0;
	info->max_voltage = 0;
	list_for_each_entry(temp_data, &vol_dev->head, list) {
		if (temp_data->id == info->id) {
			channel = temp_data->hw_voltage_chennel
					(temp_data->data, info);
			if (0 == channel) {
				hwlog_info("hardware not support.\n");
				result = 0;
				break;
			}
			if (channel < 0) {
				hwlog_info("err channel.channel:%d\n",
					info->channel);
				result = 0;
				break;
			}
			voltage_mv = temp_data->hw_voltage_getvoltage
					(temp_data->data, info);
			if (voltage_mv < 0) {
				hwlog_err("voltage:%d\n", voltage_mv);
				result = -1;
				break;
			}
			min_threshold = info->min_voltage;
			max_threshold = info->max_voltage;
			result = 1;
			break;
		}
	}

out:
	if (result != 1) {
		voltage_mv = 0;
		min_threshold = 0;
		max_threshold = 0;
	}

	return snprintf(buf, 50, "%d %d %d %d\n", result, voltage_mv,
				min_threshold, max_threshold);
}

static ssize_t hw_voltage_value_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	int ret = 0;
	long state = 0;
	struct hw_voltage_device *vol_dev = dev_get_drvdata(dev);
	struct hw_voltage_info *info = NULL;

	if (NULL == vol_dev) {
		hwlog_err("dev data null.\n");
		return -1;
	}

	ret = kstrtol(buf, 10, &state);
	if (ret < 0) {
		hwlog_err("voltage value input err. ret:%d\n", ret);
		return ret;
	}

	info = &vol_dev->info;
	/* a chip 8 channel */
	info->id = state/HW_VOLTAGE_CHANNELS_PERCHIP;
	info->channel = state%HW_VOLTAGE_CHANNELS_PERCHIP;
	hwlog_info("id:%d; channel:%d\n", info->id, info->channel);

	return size;
}
DEVICE_ATTR(vol_enable, 0660, hw_voltage_enable_show, hw_voltage_enable_store);
DEVICE_ATTR(vol_value, 0660, hw_voltage_value_show, hw_voltage_value_store);

static struct attribute *hw_voltage_attributes[] = {
	&dev_attr_vol_enable.attr,
	&dev_attr_vol_value.attr,
	NULL,
};

static const struct attribute_group hw_voltage_attr_group = {
	.attrs = hw_voltage_attributes,
};

static int hw_voltage_init(struct hw_voltage_device *vol_dev)
{
	int ret = -1;

	hwlog_info("[%s] +++++\n", __func__);
	vol_dev->vol_class = class_create(THIS_MODULE, "voltage");
	if (IS_ERR(vol_dev->vol_class)) {
		hwlog_err("class create err.\n");
		goto out;
	}
	vol_dev->dev = device_create(vol_dev->vol_class,
				NULL, 0, vol_dev, "voltage");
	if (NULL == vol_dev->dev) {
		hwlog_err("device create err.\n");
		goto out;
	}
	INIT_LIST_HEAD(&vol_dev->head);
	sysfs_create_group(&vol_dev->dev->kobj, &hw_voltage_attr_group);
	vol_dev->voltage_states = 0;
	vol_dev->info.min_voltage = 0;
	vol_dev->info.max_voltage = 0;
	vol_dev->info.id = -1;
	vol_dev->info.channel = -1;
	ret = 0;

out:
	hwlog_info("[%s] -----\n", __func__);
	return ret;
}

static void hw_voltage_exit(struct hw_voltage_device *vol_dev)
{
	sysfs_remove_group(&vol_dev->dev->kobj, &hw_voltage_attr_group);
	device_destroy(vol_dev->vol_class, vol_dev->dev->devt);
	class_destroy(vol_dev->vol_class);
}

struct hw_voltage_data *hw_voltage_register(void *data, int id)
{
	int ret = 0;
	struct hw_voltage_data *vol_data = NULL;

	if (!voltage_dev) {
		voltage_dev = (struct hw_voltage_device *)
			kzalloc(sizeof(struct hw_voltage_device), GFP_KERNEL);
		if (!voltage_dev) {
			hwlog_err("zalloc voltage_dev err.\n");
			goto out;
		}
		ret = hw_voltage_init(voltage_dev);
		if (ret < 0) {
			hwlog_err("hw voltage init err. ret:%d\n", ret);
			goto out;
		}
	}

	vol_data = (struct hw_voltage_data *)
		kzalloc(sizeof(struct hw_voltage_data), GFP_KERNEL);
	if (NULL == vol_data) {
		hwlog_err("hw voltage data kzalloc err.\n");
		goto out;
	}
	vol_data->id = id;
	vol_data->data = data;
	list_add_tail(&vol_data->list, &voltage_dev->head);
	hwlog_info("voltage register success. id:%d\n", id);

out:
	return vol_data;
}
EXPORT_SYMBOL_GPL(hw_voltage_register);

void hw_voltage_unregister(int id)
{
	struct hw_voltage_data *vol_data = NULL;
	struct hw_voltage_data *temp_data = NULL;

	list_for_each_entry_safe(vol_data, temp_data,
					&voltage_dev->head, list) {
		if (temp_data->id == id) {
			list_del(&vol_data->list);
			kfree(vol_data);
		}
	}
	if (list_empty(&voltage_dev->head)) {
		hw_voltage_exit(voltage_dev);
		kfree(voltage_dev);
	}

	hwlog_info("voltage unregister success. id:%d\n", id);
}
EXPORT_SYMBOL_GPL(hw_voltage_unregister);
