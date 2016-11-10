/* Copyright (c) 2011-2013, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "hw_flash.h"
#include "hw_flash_i2c.h"

#include <linux/interrupt.h>
#include <linux/delay.h>
/* LM3642 Registers define */
#define ENABLE_REG			0x0a
#define FLAGS_REG			0x0b
#define FLASH_FEATURES_REG		0x08
#define CURRENT_CONTROL_REG		0x09

#define IVFM_REG			0x01
#define MODE_STANDBY	0x00
#define MODE_FLASH			0x03
#define STROBE_PIN			0x20
#define TX_PIN				0x40

/* Internal data struct define */
struct hw_lm3642_nts_private_data_t {
	unsigned int chipid;
	unsigned int irq_gpio;
	unsigned int stb_irq_no;
};

static struct leds_info
{
    unsigned int enabled;
    unsigned int intensity;
}m_gazeled;

static unsigned int strobe_count = 0;

/* Internal varible define */
static struct hw_lm3642_nts_private_data_t hw_lm3642_nts_pdata;
static struct hw_flash_ctrl_t hw_lm3642_nts_ctrl;
static struct i2c_driver hw_lm3642_nts_i2c_driver;

DEFINE_HISI_FLASH_MUTEX(lm3642_nts);

static int hw_lm3642_nts_init(struct hw_flash_ctrl_t *flash_ctrl)
{
    cam_debug("%s ernter.\n", __func__);
    if (NULL == flash_ctrl) {
    	cam_err("%s flash_ctrl is NULL.", __func__);
    	return -1;
    }
    return 0;
}

static int hw_lm3642_nts_exit(struct hw_flash_ctrl_t *flash_ctrl)
{
    cam_info("%s ernter.\n", __func__);
    if (NULL == flash_ctrl) {
    	cam_err("%s flash_ctrl is NULL.", __func__);
    	return -1;
    }

    flash_ctrl->func_tbl->flash_off(flash_ctrl);
    return 0;
}

static int hw_lm3642_nts_flash_mode(struct hw_flash_ctrl_t *flash_ctrl,
	int data)
{
    struct hw_flash_i2c_client *i2c_client;
    struct hw_flash_i2c_fn_t *i2c_func;
    unsigned char val;

    if (NULL == flash_ctrl) {
    	cam_err("%s flash_ctrl is NULL.", __func__);
    	return -1;
    }

    i2c_client = flash_ctrl->flash_i2c_client;
    i2c_func = flash_ctrl->flash_i2c_client->i2c_func_tbl;

    /* clear error flag,resume chip */
    i2c_func->i2c_read(i2c_client, FLAGS_REG, &val);
    i2c_func->i2c_read(i2c_client, CURRENT_CONTROL_REG, &val);

    /* set LED Flash current value */
    val = (val & 0xf0) | (data & 0x0f);

    if((strobe_count%40) == 0){
        cam_info("%s led flash current val=0x%x, count = %d.\n", __func__, val, strobe_count);
    }
    strobe_count++;

    i2c_func->i2c_write(i2c_client, CURRENT_CONTROL_REG, val);
    i2c_func->i2c_write(i2c_client, ENABLE_REG, MODE_FLASH|STROBE_PIN);
    return 0;
}


static int hw_lm3642_nts_on(struct hw_flash_ctrl_t *flash_ctrl, void *data)
{
    struct hw_flash_cfg_data *cdata = (struct hw_flash_cfg_data *)data;
    int rc=-1;

    if ((NULL == flash_ctrl) || (NULL == cdata)) {
    	cam_err("%s flash_ctrl or cdata is NULL.", __func__);
    	return -1;
    }

    cam_debug("%s mode=%d, level=%d.\n", __func__, cdata->mode, cdata->data);

    mutex_lock(flash_ctrl->hw_flash_mutex);

    rc = hw_lm3642_nts_flash_mode(flash_ctrl, cdata->data);

    flash_ctrl->state.mode = FLASH_MODE;
    flash_ctrl->state.data = cdata->data;
    mutex_unlock(flash_ctrl->hw_flash_mutex);

    return 0;
}

static int hw_lm3642_nts_off(struct hw_flash_ctrl_t *flash_ctrl)
{
    struct hw_flash_i2c_client *i2c_client;
    struct hw_flash_i2c_fn_t *i2c_func;
    unsigned char val;

    cam_info("%s enter.\n", __func__);
    if (NULL == flash_ctrl) {
    	cam_err("%s flash_ctrl is NULL.", __func__);
    	return -1;
    }

    m_gazeled.enabled = STANDBY_MODE;
    m_gazeled.intensity = 0;
    strobe_count = 0;

    mutex_lock(flash_ctrl->hw_flash_mutex);
    flash_ctrl->state.mode = STANDBY_MODE;
    flash_ctrl->state.data = 0;
    i2c_client = flash_ctrl->flash_i2c_client;
    i2c_func = flash_ctrl->flash_i2c_client->i2c_func_tbl;

    i2c_func->i2c_read(i2c_client, FLAGS_REG, &val);
    i2c_func->i2c_write(i2c_client, ENABLE_REG, MODE_STANDBY);
    mutex_unlock(flash_ctrl->hw_flash_mutex);

    return 0;
}

static int hw_lm3642_set_on(struct hw_flash_ctrl_t *flash_ctrl, void *data)
{

    struct hw_flash_cfg_data *cdata = (struct hw_flash_cfg_data *)data;
    m_gazeled.enabled = cdata->mode;
    m_gazeled.intensity = cdata->data;
    cam_debug("%s flash_ctrl enabled =%d, intensity=%d.", __func__, m_gazeled.enabled, m_gazeled.intensity);
    return 0;
}

static irqreturn_t gazeled_irq_thread(int irq, void *handle)
{
    struct hw_flash_cfg_data cdata = {0};
    cam_debug("enter %s",__func__);
    cdata.mode = m_gazeled.enabled;
    cdata.data =   m_gazeled.intensity;
    udelay(2000);
    if (cdata.mode == STANDBY_MODE) {
        hw_lm3642_nts_off(&hw_lm3642_nts_ctrl);
    } else {
        hw_lm3642_nts_on(&hw_lm3642_nts_ctrl, &cdata);
    }

    return IRQ_HANDLED;
}

static int hw_lm3642_nts_match(struct hw_flash_ctrl_t *flash_ctrl)
{
    struct hw_flash_i2c_client *i2c_client;
    struct hw_flash_i2c_fn_t *i2c_func;
    struct hw_lm3642_nts_private_data_t *pdata;
    unsigned char id;
    int rc = 0;

    cam_debug("%s ernter.\n", __func__);

    if (NULL == flash_ctrl) {
        cam_err("%s flash_ctrl is NULL.", __func__);
        return -1;
    }

    i2c_client = flash_ctrl->flash_i2c_client;
    i2c_func = flash_ctrl->flash_i2c_client->i2c_func_tbl;
    pdata = (struct hw_lm3642_nts_private_data_t *)flash_ctrl->pdata;

    i2c_func->i2c_read(i2c_client, FLASH_FEATURES_REG, &id);
    cam_info("%s id=0x%x.\n", __func__, id);
    if (id != pdata->chipid) {
        cam_err("%s match error, id(0x%x) != 0x%x.",
    	    __func__, (id&0x7), pdata->chipid);
        return -1;
    }
    i2c_func->i2c_write(i2c_client, IVFM_REG, 0x00);

    rc = gpio_request(pdata->irq_gpio, "sensor_irq");
    if (rc) {
    	cam_err("gazeled request irq gpio error");
    	return rc;
    }
    if (gpio_direction_input(pdata->irq_gpio)){
    	cam_warn("gazeled - irq gpio direction input error");
    }
    pdata->stb_irq_no = gpio_to_irq(pdata->irq_gpio);
    rc = request_threaded_irq(pdata->stb_irq_no, NULL, gazeled_irq_thread,
    						IRQF_ONESHOT | IRQF_TRIGGER_FALLING,
    						"gazeled_strobe", NULL);
    if (rc) {
    	cam_err("probe - request irq error(%d)", pdata->stb_irq_no);
    	gpio_free(pdata->irq_gpio);
    }
    cam_info("%s request gazeled irq success, gpio = %d", __func__, pdata->irq_gpio);

    return 0;
}

static int hw_lm3642_nts_get_dt_data(struct hw_flash_ctrl_t *flash_ctrl)
{
    struct hw_lm3642_nts_private_data_t *pdata;
    struct device_node *of_node;
    int rc = -1;
    cam_debug("%s enter.\n", __func__);

    if (NULL == flash_ctrl) {
        cam_err("%s flash_ctrl is NULL.", __func__);
        return rc;
    }

    pdata = (struct hw_lm3642_nts_private_data_t *)flash_ctrl->pdata;
    of_node = flash_ctrl->dev->of_node;

    rc = of_property_read_u32(of_node, "huawei,flash-chipid", &pdata->chipid);
    cam_info("%s hisi,chipid 0x%x, rc %d\n", __func__,
        pdata->chipid, rc);
    if (rc < 0) {
        cam_err("%s failed %d\n", __func__, __LINE__);
        return rc;
    }
    rc = of_property_read_u32(of_node, "huawei,irq_gpio", &pdata->irq_gpio);
    cam_info("%s hisi,irq_gpio 0x%x, rc %d\n", __func__,
        pdata->irq_gpio, rc);
    if (rc < 0) {
        cam_err("%s failed %d\n", __func__, __LINE__);
        return rc;
    }
    return rc;
}

static int hw_lm3642_nts_register_attribute(struct hw_flash_ctrl_t *flash_ctrl, struct device *dev)
{
	return 0;
}

static int hw_lm3642_nts_remove(struct i2c_client *client)
{
	cam_debug("%s enter.", __func__);

	hw_lm3642_nts_ctrl.func_tbl->flash_exit(&hw_lm3642_nts_ctrl);

	client->adapter = NULL;
	return 0;
}

static void hw_lm3642_nts_shutdown(struct i2c_client *client)
{
	int rc = -1;
	rc = hw_lm3642_nts_off(&hw_lm3642_nts_ctrl);
	cam_info("%s lm3642_nts shut down at (%d)",__func__,__LINE__);
	if (rc < 0) {
		cam_err("%s lm3642_nts flash off error.", __func__);
	}
}

static const struct i2c_device_id hw_lm3642_nts_id[] = {
	{"lm3642_nts", (unsigned long)&hw_lm3642_nts_ctrl},
	{}
};

static const struct of_device_id hw_lm3642_nts_dt_match[] = {
	{.compatible = "huawei,lm3642_nts"},
	{}
};
MODULE_DEVICE_TABLE(of, lm3642_nts_dt_match);

static struct i2c_driver hw_lm3642_nts_i2c_driver = {
	.probe	= hw_flash_i2c_probe,
	.remove = hw_lm3642_nts_remove,
	.shutdown = hw_lm3642_nts_shutdown,
	.id_table	= hw_lm3642_nts_id,
	.driver = {
		.name = "hw_lm3642_nts",
		.of_match_table = hw_lm3642_nts_dt_match,
	},
};

static struct hw_flash_i2c_client hw_lm3642_nts_i2c_client;

static struct hw_flash_fn_t hw_lm3642_nts_func_tbl = {
	.flash_config = hw_flash_config,
	.flash_init = hw_lm3642_nts_init,
	.flash_exit = hw_lm3642_nts_exit,
	.flash_on = hw_lm3642_set_on,//hw_lm3642_nts_on,
	.flash_off = hw_lm3642_nts_off,
	.flash_match = hw_lm3642_nts_match,
	.flash_get_dt_data = hw_lm3642_nts_get_dt_data,
	.flash_register_attribute = hw_lm3642_nts_register_attribute,
};

static struct hw_flash_ctrl_t hw_lm3642_nts_ctrl = {
	.flash_i2c_client = &hw_lm3642_nts_i2c_client,
	.func_tbl = &hw_lm3642_nts_func_tbl,
	.hw_flash_mutex = &flash_mut_lm3642_nts,
	.pdata = (void*)&hw_lm3642_nts_pdata,
	.flash_mask_enable = false,
	.state = {
		.mode = STANDBY_MODE,
	},
};

static int __init hw_lm3642_nts_module_init(void)
{
	cam_info("%s erter.\n", __func__);
	return i2c_add_driver(&hw_lm3642_nts_i2c_driver);
}

static void __exit hw_lm3642_nts_module_exit(void)
{
	cam_info("%s enter.", __func__);
	i2c_del_driver(&hw_lm3642_nts_i2c_driver);
	return;
}


module_init(hw_lm3642_nts_module_init);
module_exit(hw_lm3642_nts_module_exit);
MODULE_DESCRIPTION("lm3642_nts FLASH");
MODULE_LICENSE("GPL v2");
