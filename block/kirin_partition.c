/*
 *
 * Copyright (c) 2013-2015, Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/err.h>
#include <linux/bootdevice.h>
#include <asm/uaccess.h>
#include <partition.h>

int get_cunrrent_total_ptn_num(void)
{
	int current_ptn_num = 0;
	enum bootdevice_type boot_device_type = BOOT_DEVICE_EMMC;

	boot_device_type = get_bootdevice_type();

	if (BOOT_DEVICE_EMMC == boot_device_type) {
		current_ptn_num = sizeof(partition_table_emmc) / sizeof(struct partition) - 1;
	}
#ifdef CONFIG_HISI_STORAGE_UFS_PARTITION
	else if (BOOT_DEVICE_UFS  == boot_device_type){
		current_ptn_num = sizeof(partition_table_ufs) / sizeof(struct partition) - 1;
	}
#endif
	return current_ptn_num;
}
EXPORT_SYMBOL(get_cunrrent_total_ptn_num);

/*
 *Function: Get the device path by partition name
 *Input : str, partition name
 *Output: pblkname, boot device path, such as:/dev/block/bootdevice/by-name/xxx
 */
int flash_find_ptn(const char* str, char* pblkname)
{
	unsigned int n;
	char device_path[] = "/dev/block/bootdevice/by-name/";
	int current_ptn_num = 0;
	struct partition *current_partition_table = NULL;
	enum bootdevice_type boot_device_type = BOOT_DEVICE_EMMC;

	if ((NULL == pblkname) || (NULL == str)) {
		printk(KERN_ERR "Input partition name or device path buffer is NULL\n");
		return -1;
	}

	boot_device_type = get_bootdevice_type();

	if (BOOT_DEVICE_EMMC == boot_device_type) {
		current_partition_table  = partition_table_emmc;
	}
#ifdef CONFIG_HISI_STORAGE_UFS_PARTITION
	else if (BOOT_DEVICE_UFS  == boot_device_type){
		current_partition_table  = partition_table_ufs;
	}
#endif

	current_ptn_num = get_cunrrent_total_ptn_num();
	for(n = 0; n < current_ptn_num; n++) {
		if(!strcmp((current_partition_table + n)->name, str)) {/*[false alarm]:current_partition_table!=NULL*/
			strncpy(pblkname, device_path, strlen(device_path));
			strncpy(pblkname + strlen(device_path), str, strlen(str)+1);
			return 0;
		}
	}

	printk(KERN_ERR "partition is not found\n");
	return -1;
}
EXPORT_SYMBOL(flash_find_ptn);

/*
 *Get partition offset in total partitions(all lu), not the current LU
 */
int flash_get_ptn_index(const char* pblkname)
{
	unsigned int n;
	int current_ptn_num = 0;
	struct  partition *current_partition_table = NULL;
	enum bootdevice_type boot_device_type = BOOT_DEVICE_EMMC;

	if (NULL == pblkname) {
		printk(KERN_ERR "Input partition name is NULL\n");
		return -1;
	}

	boot_device_type = get_bootdevice_type();

	if (BOOT_DEVICE_EMMC == boot_device_type) {
		current_partition_table  = partition_table_emmc;
	}
#ifdef CONFIG_HISI_STORAGE_UFS_PARTITION
	else if (BOOT_DEVICE_UFS  == boot_device_type){
		current_partition_table  = partition_table_ufs;
	}
#endif

	current_ptn_num = get_cunrrent_total_ptn_num();

	if(!strcmp(PART_XLOADER, pblkname)) {
		printk(KERN_ERR "This is boot partition\n");
		return -1;
	}

	for(n = 0; n < current_ptn_num; n++) {
		if (!strcmp((current_partition_table + n)->name, pblkname)) {/*[false alarm]:current_partition_table!=NULL*/
			return n;
		}
	}

	printk(KERN_ERR "Input partition is not found\n");
	return -1;
}
EXPORT_SYMBOL(flash_get_ptn_index);

