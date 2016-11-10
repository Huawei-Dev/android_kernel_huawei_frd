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
#include <linux/delay.h>
/* MPS3331 Registers define */
#define MODE_SET_REG			0x01
#define FLAGS_REG			0x0b
#define DEVID_REG		0x00
#define CURRENT_REG		0x06
#define TIME_SET_REG      0x03

#define STR_MODE	0xe0
#define MODE_FLASH			0x06
#define MODE_STANDBY	0xc0
#define LED_SD           0x08
#define LED_EN           0x10

/* Internal data struct define */
struct hw_mps3331_nts_private_data_t {
	unsigned int chipid;	
};

/* Internal varible define */
static struct hw_mps3331_nts_private_data_t hw_mps3331_nts_pdata;
static struct hw_flash_ctrl_t hw_mps3331_nts_ctrl;
static struct i2c_driver hw_mps3331_nts_i2c_driver;

DEFINE_HISI_FLASH_MUTEX(mps3331_nts);

static int hw_mps3331_nts_init(struct hw_flash_ctrl_t *flash_ctrl)
{
    cam_debug("%s enter.\n", __func__);
    if (NULL == flash_ctrl) {
    	cam_err("%s flash_ctrl is NULL.", __func__);
    	return -1;
    }
    return 0;
}

static int hw_mps3331_nts_exit(struct hw_flash_ctrl_t *flash_ctrl)
{
    cam_info("%s enter.\n", __func__);
    if (NULL == flash_ctrl) {
    	cam_err("%s flash_ctrl is NULL.", __func__);
    	return -1;
    }

    flash_ctrl->func_tbl->flash_off(flash_ctrl);
    return 0;
}

static int hw_mps3331_nts_flash_mode(struct hw_flash_ctrl_t *flash_ctrl,
	void *data)
{
    struct hw_flash_i2c_client *i2c_client;
    struct hw_flash_i2c_fn_t *i2c_func;
    unsigned char val;
    struct hw_flash_cfg_data *cdata = (struct hw_flash_cfg_data *)data;

    if ((NULL == flash_ctrl) || (NULL == cdata)) {
    	cam_err("%s flash_ctrl or cdata is NULL.", __func__);
    	return -1;
    }

    i2c_client = flash_ctrl->flash_i2c_client;
    i2c_func = flash_ctrl->flash_i2c_client->i2c_func_tbl;

    /* clear error flag,resume chip */
    //i2c_func->i2c_write(i2c_client, MODE_SET_REG, STR_MODE|MODE_STANDBY);
    i2c_func->i2c_read(i2c_client, FLAGS_REG, &val);

    if(STANDBY_MODE == cdata->mode){
        i2c_func->i2c_write(i2c_client, MODE_SET_REG, STR_MODE|MODE_STANDBY);
    }else{
        i2c_func->i2c_write(i2c_client, MODE_SET_REG, STR_MODE|MODE_FLASH); 
        /* set LED Flash current value */
        val = ((cdata->data)+1)*3;
        cam_info("%s led flash current val=0x%x.\n", __func__, val);
        i2c_func->i2c_write(i2c_client, CURRENT_REG, val);
        i2c_func->i2c_write(i2c_client, TIME_SET_REG, LED_SD);
        i2c_func->i2c_write(i2c_client, MODE_SET_REG, STR_MODE|MODE_FLASH|LED_EN);
    }   
    return 0;
}


static int hw_mps3331_nts_on(struct hw_flash_ctrl_t *flash_ctrl, void *data)
{
    struct hw_flash_cfg_data *cdata = (struct hw_flash_cfg_data *)data;
    int rc=-1;

    if ((NULL == flash_ctrl) || (NULL == cdata)) {
    	cam_err("%s flash_ctrl or cdata is NULL.", __func__);
    	return -1;
    }

    cam_debug("%s mode=%d, level=%d.\n", __func__, cdata->mode, cdata->data);

    if((flash_ctrl->state.data == cdata->data) 
           &&(flash_ctrl->state.mode == cdata->mode)){
        return 0;
    }

    mutex_lock(flash_ctrl->hw_flash_mutex);
    rc = hw_mps3331_nts_flash_mode(flash_ctrl, data);

    flash_ctrl->state.mode = cdata->mode;
    flash_ctrl->state.data = cdata->data;
    mutex_unlock(flash_ctrl->hw_flash_mutex);

    return 0;
}

static int hw_mps3331_nts_off(struct hw_flash_ctrl_t *flash_ctrl)
{
    struct hw_flash_i2c_client *i2c_client;
    struct hw_flash_i2c_fn_t *i2c_func;
    unsigned char val;

    cam_info("%s enter.\n", __func__);
    if (NULL == flash_ctrl) {
    	cam_err("%s flash_ctrl is NULL.", __func__);
    	return -1;
    }

    mutex_lock(flash_ctrl->hw_flash_mutex);
    flash_ctrl->state.mode = STANDBY_MODE;
    flash_ctrl->state.data = 0;
    i2c_client = flash_ctrl->flash_i2c_client;
    i2c_func = flash_ctrl->flash_i2c_client->i2c_func_tbl;

    i2c_func->i2c_read(i2c_client, FLAGS_REG, &val);
    i2c_func->i2c_write(i2c_client, MODE_SET_REG, STR_MODE|MODE_STANDBY);
    mutex_unlock(flash_ctrl->hw_flash_mutex);

    return 0;
}


static int hw_mps3331_nts_match(struct hw_flash_ctrl_t *flash_ctrl)
{
	struct hw_flash_i2c_client *i2c_client;
	struct hw_flash_i2c_fn_t *i2c_func;
	struct hw_mps3331_nts_private_data_t *pdata;
	unsigned char id;

	cam_debug("%s ernter.\n", __func__);

	if (NULL == flash_ctrl) {
		cam_err("%s flash_ctrl is NULL.", __func__);
		return -1;
	}

	i2c_client = flash_ctrl->flash_i2c_client;
	i2c_func = flash_ctrl->flash_i2c_client->i2c_func_tbl;
	pdata = (struct hw_mps3331_nts_private_data_t *)flash_ctrl->pdata;

	i2c_func->i2c_read(i2c_client, DEVID_REG, &id);
	cam_info("%s id=0x%x.\n", __func__, id);
	if (id != pdata->chipid) {
		cam_err("%s match error, id(0x%x) != 0x%x.",
			__func__, id, pdata->chipid);
		return -1;
	}

	return 0;
}

static int hw_mps3331_nts_get_dt_data(struct hw_flash_ctrl_t *flash_ctrl)
{
	struct hw_mps3331_nts_private_data_t *pdata;
	struct device_node *of_node;
	int rc = -1;

	cam_debug("%s enter.\n", __func__);

	if (NULL == flash_ctrl) {
		cam_err("%s flash_ctrl is NULL.", __func__);
		return rc;
	}

	pdata = (struct hw_mps3331_nts_private_data_t *)flash_ctrl->pdata;
	of_node = flash_ctrl->dev->of_node;

	rc = of_property_read_u32(of_node, "huawei,flash-chipid",
		&pdata->chipid);
	cam_info("%s hisi,chipid 0x%x, rc %d\n", __func__,
		pdata->chipid, rc);
	if (rc < 0) {
		cam_err("%s failed %d\n", __func__, __LINE__);
		return rc;
	}
	return rc;
}
static int hw_mps3331_nts_register_attribute(struct hw_flash_ctrl_t *flash_ctrl, struct device *dev)
{
	return 0;
}

static int hw_mps3331_nts_remove(struct i2c_client *client)
{
	cam_debug("%s enter.", __func__);

	hw_mps3331_nts_ctrl.func_tbl->flash_exit(&hw_mps3331_nts_ctrl);

	client->adapter = NULL;
	return 0;
}

static void hw_mps3331_nts_shutdown(struct i2c_client *client)
{
	int rc = -1;
	rc = hw_mps3331_nts_off(&hw_mps3331_nts_ctrl);
	cam_info("%s mps3331_nts shut down at (%d)",__func__,__LINE__);
	if (rc < 0) {
		cam_err("%s mps3331_nts flash off error.", __func__);
	}
}

static const struct i2c_device_id hw_mps3331_nts_id[] = {
	{"mps3331_nts", (unsigned long)&hw_mps3331_nts_ctrl},
	{}
};

static const struct of_device_id hw_mps3331_nts_dt_match[] = {
	{.compatible = "huawei,mps3331_nts"},
	{}
};
MODULE_DEVICE_TABLE(of, mps3331_nts_dt_match);

static struct i2c_driver hw_mps3331_nts_i2c_driver = {
	.probe	= hw_flash_i2c_probe,
	.remove = hw_mps3331_nts_remove,
	.shutdown = hw_mps3331_nts_shutdown,
	.id_table	= hw_mps3331_nts_id,
	.driver = {
		.name = "hw_mps3331_nts",
		.of_match_table = hw_mps3331_nts_dt_match,
	},
};

static struct hw_flash_i2c_client hw_mps3331_nts_i2c_client;

static struct hw_flash_fn_t hw_mps3331_nts_func_tbl = {
	.flash_config = hw_flash_config,
	.flash_init = hw_mps3331_nts_init,
	.flash_exit = hw_mps3331_nts_exit,
	.flash_on = hw_mps3331_nts_on,
	.flash_off = hw_mps3331_nts_off,
	.flash_match = hw_mps3331_nts_match,
	.flash_get_dt_data = hw_mps3331_nts_get_dt_data,
	.flash_register_attribute = hw_mps3331_nts_register_attribute,
};

static struct hw_flash_ctrl_t hw_mps3331_nts_ctrl = {
	.flash_i2c_client = &hw_mps3331_nts_i2c_client,
	.func_tbl = &hw_mps3331_nts_func_tbl,
	.hw_flash_mutex = &flash_mut_mps3331_nts,
	.pdata = (void*)&hw_mps3331_nts_pdata,
	.flash_mask_enable = false,
	.state = {
		.mode = STANDBY_MODE,
	},
};

static int __init hw_mps3331_nts_module_init(void)
{
	cam_info("%s enter.\n", __func__);
	return i2c_add_driver(&hw_mps3331_nts_i2c_driver);
}

static void __exit hw_mps3331_nts_module_exit(void)
{
	cam_info("%s enter.", __func__);
	i2c_del_driver(&hw_mps3331_nts_i2c_driver);
	return;
}


module_init(hw_mps3331_nts_module_init);
module_exit(hw_mps3331_nts_module_exit);
MODULE_DESCRIPTION("mps3331_nts FLASH");
MODULE_LICENSE("GPL v2");
