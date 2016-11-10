
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeclaration-after-statement"

#include <linux/version.h>
#include <linux/cpumask.h>
#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <asm-generic/fcntl.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/mmc/rpmb.h>
#include <linux/mmc/core.h>
#include <linux/mmc/ioctl.h>
#include <linux/mmc/card.h>
#include <linux/mmc/host.h>
#include <linux/mmc/mmc.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/slab.h>
#include <linux/smp.h>
#include <linux/types.h>
#include <linux/bootmem.h>
#include <linux/mm.h>
#include <linux/printk.h>
#include <linux/dma-mapping.h>
#include <linux/device.h>
#include <linux/wakelock.h>
#include <global_ddr_map.h>

#include <asm/byteorder.h>
#include <asm/hardirq.h>

#include "hisi_rpmb.h"
#include <asm/uaccess.h>
#include <linux/syscalls.h>
#include <linux/delay.h>
#include <scsi/sg.h>
#include <linux/bootdevice.h>

#define UFS_RPMB_BLOCK_DEVICE_NAME "/dev/0:0:0:49476"
#define EMMC_RPMB_BLOCK_DEVICE_NAME "/dev/block/mmcblk0rpmb"
/*avoid to invoke mainline code,we can only use this ugly code*/
#include "mmc_hisi_card.h"

/* external function declaration */
#ifdef CONFIG_ATFDRIVER
extern int atfd_hisi_rpmb_smc(u64 function_id, u64 arg0, u64 arg1, u64 arg2);
#else
static int atfd_hisi_rpmb_smc(u64 function_id, u64 arg0, u64 arg1, u64 arg2)
{
	return -1;
}
#endif
int mmc_blk_ioctl_rpmb_cmd(enum func_id id,
			   struct block_device *bdev,
			   struct mmc_blk_ioc_rpmb_data *idata);
extern struct mmc_blk_data *mmc_blk_get(struct gendisk *disk);
extern void mmc_blk_put(struct mmc_blk_data *md);
extern int mmc_blk_part_switch(struct mmc_card *card, struct mmc_blk_data *md);
extern int ioctl_rpmb_card_status_poll(struct mmc_card *card,
				       u32 *status,
				       u32 retries_max);

struct hisi_rpmb {
	struct rpmb_request *rpmb_request;
	struct device *dev;
	struct block_device *blkdev;
	struct workqueue_struct *rpmb_workqueue;
	struct work_struct rpmb_work;
	struct wake_lock wake_lock;
};
extern long
blk_scsi_kern_ioctl(unsigned int fd, unsigned int cmd, unsigned long arg);
static struct hisi_rpmb hisi_rpmb;
static int rpmb_support_device = BOOT_DEVICE_EMMC;
static int rpmb_drivers_init_status = RPMB_DRIVER_IS_NOT_READY;

DEFINE_MUTEX(rpmb_counter_lock);
DEFINE_MUTEX(rpmb_ufs_cmd_lock);

static inline void mmc_rpmb_combine_cmd(struct mmc_blk_ioc_data *data,
					int is_write,
					unsigned short blocks,
					unsigned short blksz,
					bool reliable)
{
	struct mmc_ioc_cmd *ioc;
	ioc = &data->ic;

	ioc->write_flag = is_write | ((is_write & reliable) << 31);
	ioc->opcode =
		is_write ? MMC_WRITE_MULTIPLE_BLOCK : MMC_READ_MULTIPLE_BLOCK;
	ioc->flags = MMC_RSP_SPI_R1 | MMC_RSP_R1 | MMC_CMD_ADTC;
	ioc->arg = 0x0;
	ioc->blksz = blksz;
	ioc->blocks = blocks;
}
static inline void mmc_rpmb_combine_request(struct mmc_blk_ioc_data *data,
					    struct rpmb_frame *frame,
					    int is_write,
					    unsigned short blocks,
					    unsigned short blksz,
					    bool reliable)
{
	data->buf = (void *)frame;
	data->buf_bytes = (u64)blocks * blksz;
	mmc_rpmb_combine_cmd(data, is_write, blocks, blksz, reliable);
}

static inline void
mmc_rpmb_basic_request(uint32_t index,
		       struct rpmb_request *request,
		       struct mmc_blk_ioc_rpmb_data *rpmb_data,
		       uint16_t block_count,
		       uint32_t reliable)
{
	mmc_rpmb_combine_request(&rpmb_data->data[0], &request->frame[0], true,
				 block_count, RPMB_BLK_SZ, reliable);
}

static inline void
mmc_rpmb_status_request(uint32_t index,
			struct rpmb_request *request,
			struct mmc_blk_ioc_rpmb_data *rpmb_data,
			uint16_t block_count,
			uint32_t reliable)
{
	struct rpmb_frame *status_frame = &request->status_frame;
	memset(status_frame, 0, sizeof(*status_frame));
	status_frame->request = cpu_to_be16(RPMB_REQ_STATUS);

	mmc_rpmb_combine_request(&rpmb_data->data[1], status_frame, true, 0x1,
				 RPMB_BLK_SZ, false);
}

static inline void
mmc_rpmb_result_request(uint32_t index,
			struct rpmb_request *request,
			struct mmc_blk_ioc_rpmb_data *rpmb_data,
			uint16_t block_count,
			uint32_t reliable)
{
	mmc_rpmb_combine_request(&rpmb_data->data[index], &request->frame[0],
				 false, block_count, RPMB_BLK_SZ, false);
}

void mmc_rpmb_set_key(struct rpmb_request *request,
		      struct mmc_blk_ioc_rpmb_data *rpmb_data)
{
	mmc_rpmb_basic_request(0, request, rpmb_data, 0x1, true);
	mmc_rpmb_status_request(1, request, rpmb_data, 0x1, false);
	mmc_rpmb_result_request(2, request, rpmb_data, 0x1, false);
}

void mmc_rpmb_get_counter(struct rpmb_request *request,
			  struct mmc_blk_ioc_rpmb_data *rpmb_data)
{
	mmc_rpmb_basic_request(0, request, rpmb_data, 0x1, false);
	mmc_rpmb_result_request(1, request, rpmb_data, 0x1, false);
}

void mmc_rpmb_read_data(struct rpmb_request *request,
			struct mmc_blk_ioc_rpmb_data *rpmb_data)
{
	unsigned short blocks_count = request->info.current_rqst.blks;

	mmc_rpmb_basic_request(0, request, rpmb_data, 0x1, false);
	mmc_rpmb_result_request(1, request, rpmb_data, blocks_count, false);
}

void mmc_rpmb_write_data(struct rpmb_request *request,
			 struct mmc_blk_ioc_rpmb_data *rpmb_data)
{
	unsigned short blocks_count = request->info.current_rqst.blks;

	mmc_rpmb_basic_request(0, request, rpmb_data, blocks_count, true);
	mmc_rpmb_status_request(1, request, rpmb_data, 0x1, false);
	mmc_rpmb_result_request(2, request, rpmb_data, 0x1, false);
}

int ufs_bsg_ioctl_rpmb_cmd(struct ufs_blk_ioc_rpmb_data *rdata)
{
	int32_t fd;
	int32_t i;
	int32_t result = -1;

	mutex_lock(&rpmb_ufs_cmd_lock);
	mm_segment_t oldfs = get_fs();
	set_fs(get_ds());
	fd = sys_open(UFS_RPMB_BLOCK_DEVICE_NAME, O_RDWR,
		      S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0){
		pr_err("rpmb ufs file device open failed!\n");
		goto error;
	}
	for (i = 0; i < UFS_IOC_MAX_RPMB_CMD; i++) {
		if (!rdata->data[i].siv.request_len)
			break;
		int j;
		for (j = 0; j < rdata->data[i].siv.request_len; j++)
			pr_err("%x:%x \n", j,
			       *(uint8_t *)((uint8_t *)rdata->data[i]
						    .siv.request +
					    j));

		/*scsi ioctl */
		pr_err("start blk_dev_op.ioctl:%x", fd);
		result = blk_scsi_kern_ioctl(
			fd, SG_IO, (unsigned long)(&(rdata->data[i].siv)));
		if (result)
			break;
		pr_err("ioctl data %x:%x\n", i, result);
		if (rdata->data[i].siv.din_xfer_len)
			memcpy(rdata->data[i].buf,
			       (void *)rdata->data[i].siv.din_xferp,
			       rdata->data[i].buf_bytes);
		else
			memcpy(rdata->data[i].buf,
			       (void *)rdata->data[i].siv.dout_xferp,
			       rdata->data[i].buf_bytes);
	}
	sys_close(fd);
	set_fs(oldfs);
	mutex_unlock(&rpmb_ufs_cmd_lock);
	return result;
error:
	set_fs(oldfs);
	mutex_unlock(&rpmb_ufs_cmd_lock);
	return -1;
}

void ufs_get_cdb_rpmb_command(uint32_t opcode, uint32_t size, uint8_t *cmd)
{
	switch (opcode) {
	case UFS_OP_SECURITY_PROTOCOL_IN:
		cmd[0] = UFS_OP_SECURITY_PROTOCOL_IN;
		cmd[1] = SECURITY_PROTOCOL; /* Manju updated from 0x00 */
		cmd[2] = 0x00;
		cmd[3] = 0x01;
		cmd[4] = 0x00;
		cmd[5] = 0x00;
		cmd[6] = (uint8_t)(size >> 24);
		cmd[7] = (uint8_t)((size >> 16) & 0xff);
		cmd[8] = (uint8_t)((size >> 8) & 0xff);
		cmd[9] = (uint8_t)(size & 0xff);
		cmd[10] = 0x00;
		cmd[11] = 0x00;
		break;

	case UFS_OP_SECURITY_PROTOCOL_OUT:
		cmd[0] = UFS_OP_SECURITY_PROTOCOL_OUT;
		cmd[1] = SECURITY_PROTOCOL;
		cmd[2] = 0x00;
		cmd[3] = 0x01;
		cmd[4] = 0x00;
		cmd[6] = (uint8_t)((size >> 24));
		cmd[7] = (uint8_t)((size >> 16) & 0xff);
		cmd[8] = (uint8_t)((size >> 8) & 0xff);
		cmd[9] = (uint8_t)(size & 0xff);
		cmd[10] = 0x00;
		cmd[11] = 0x00;
		break;

	default:
		break;
	}
}

static inline void ufs_rpmb_combine_cmd(struct ufs_blk_ioc_data *data,
					bool is_write,
					unsigned short blocks,
					unsigned short blksz,
					u8 *sdb_command,
					u8 *sense_buffer,
					struct rpmb_frame *transfer_frame)
{
	/*the scsi SG_IO header*/
	struct sg_io_v4 *siv;

	siv = &data->siv;

	siv->guard = 'Q';
	siv->protocol = BSG_PROTOCOL_SCSI;
	siv->subprotocol = BSG_SUB_PROTOCOL_SCSI_CMD;
	siv->response = (uint64_t)sense_buffer;
	siv->max_response_len = MAX_SENSE_BUFFER_LENGTH;
	siv->request_len = SCSI_RPMB_COMMAND_LENGTH;
	if (is_write) {
		ufs_get_cdb_rpmb_command(UFS_OP_SECURITY_PROTOCOL_OUT,
					 data->buf_bytes, sdb_command);
		siv->dout_xfer_len = (uint32_t)blocks * blksz;
		siv->dout_xferp = (uint64_t)transfer_frame;
		siv->request = (__u64)sdb_command;
	} else {
		ufs_get_cdb_rpmb_command(UFS_OP_SECURITY_PROTOCOL_IN,
					 data->buf_bytes, sdb_command);
		siv->din_xfer_len = (uint32_t)blocks * blksz;
		siv->din_xferp = (uint64_t)transfer_frame;
		siv->request = (__u64)sdb_command;
	}
}

static inline void ufs_rpmb_combine_request(struct ufs_blk_ioc_data *data,
					    struct rpmb_frame *frame,
					    bool is_write,
					    unsigned short blocks,
					    unsigned short blksz,
					    u8 *sdb_command,
					    u8 *sense_buffer,
					    struct rpmb_frame *transfer_frame)
{
	/*the scsi SG_IO header*/
	data->buf = (void *)frame;
	data->buf_bytes = (u64)blocks * blksz;

	memcpy(transfer_frame, frame, (u64)blocks * blksz);
	ufs_rpmb_combine_cmd(data, is_write, blocks, blksz, sdb_command,
			     sense_buffer, transfer_frame);
}

static inline void
ufs_rpmb_basic_request(uint32_t index,
		       struct rpmb_request *request,
		       struct ufs_blk_ioc_rpmb_data *rpmb_data,
		       uint16_t block_count,
		       uint8_t *sense_buffer,
		       struct rpmb_frame *transfer_frame)
{
	ufs_rpmb_combine_request(&rpmb_data->data[index], &request->frame[0],
				 true, block_count, RPMB_BLK_SZ,
				 (u8 *)&rpmb_data->sdb_command[index], sense_buffer,
				 transfer_frame);
}

static inline void
ufs_rpmb_status_request(uint32_t index,
			struct rpmb_request *request,
			struct ufs_blk_ioc_rpmb_data *rpmb_data,
			uint16_t block_count,
			uint8_t *sense_buffer,
			struct rpmb_frame *transfer_frame)
{
	struct rpmb_frame *status_frame = &request->status_frame;
	memset(status_frame, 0, sizeof(*status_frame));
	status_frame->request = cpu_to_be16(RPMB_REQ_STATUS);
	ufs_rpmb_combine_request(&rpmb_data->data[index], status_frame, true,
				 block_count, RPMB_BLK_SZ,
				 (u8 *)&rpmb_data->sdb_command[index], sense_buffer,
				 transfer_frame);
}

static inline void
ufs_rpmb_result_request(uint32_t index,
			struct rpmb_request *request,
			struct ufs_blk_ioc_rpmb_data *rpmb_data,
			uint16_t block_count,
			uint8_t *sense_buffer,
			struct rpmb_frame *transfer_frame)
{
	ufs_rpmb_combine_request(&rpmb_data->data[index], &request->frame[0],
				 false, block_count, RPMB_BLK_SZ,
				 (u8 *)&rpmb_data->sdb_command[index], sense_buffer,
				 transfer_frame);
}

void ufs_rpmb_set_key(struct rpmb_request *request,
		      struct ufs_blk_ioc_rpmb_data *rpmb_data,
		      uint8_t *sense_buffer[],
		      struct rpmb_frame *transfer_frame[])
{
	ufs_rpmb_basic_request(0, request, rpmb_data, 0x1, sense_buffer[0],
			       transfer_frame[0]);
	ufs_rpmb_status_request(1, request, rpmb_data, 0x1, sense_buffer[1],
				transfer_frame[1]);
	ufs_rpmb_result_request(2, request, rpmb_data, 0x1, sense_buffer[2],
				transfer_frame[2]);
}

void ufs_rpmb_read_data(struct rpmb_request *request,
			struct ufs_blk_ioc_rpmb_data *rpmb_data,
			uint8_t *sense_buffer[],
			struct rpmb_frame *transfer_frame[])
{
	unsigned short blocks_count = request->info.current_rqst.blks;

	ufs_rpmb_basic_request(0, request, rpmb_data, 0x1, sense_buffer[0],
			       transfer_frame[0]);
	ufs_rpmb_result_request(1, request, rpmb_data, blocks_count,
				sense_buffer[1], transfer_frame[1]);
}

void ufs_rpmb_get_counter(struct rpmb_request *request,
			  struct ufs_blk_ioc_rpmb_data *rpmb_data,
			  uint8_t *sense_buffer[],
			  struct rpmb_frame *transfer_frame[])
{
	ufs_rpmb_basic_request(0, request, rpmb_data, 0x1, sense_buffer[0],
			       transfer_frame[0]);
	ufs_rpmb_result_request(1, request, rpmb_data, 0x1, sense_buffer[1],
				transfer_frame[1]);
}

void ufs_rpmb_write_data(struct rpmb_request *request,
			 struct ufs_blk_ioc_rpmb_data *rpmb_data,
			 uint8_t *sense_buffer[],
			 struct rpmb_frame *transfer_frame[])
{
	unsigned short blocks_count = request->info.current_rqst.blks;

	ufs_rpmb_basic_request(0, request, rpmb_data, blocks_count,
			       sense_buffer[0], transfer_frame[0]);
	ufs_rpmb_status_request(1, request, rpmb_data, 0x1, sense_buffer[1],
				transfer_frame[1]);
	ufs_rpmb_result_request(2, request, rpmb_data, 0x1, sense_buffer[2],
				transfer_frame[2]);
}

/* reserved for ufs */
/* static void ufs_rpmb_work(struct rpmb_request *request) */
static int32_t mmc_rpmb_work(struct rpmb_request *request)
{
	int32_t result;

	struct rpmb_frame *frame = &request->frame[0];
	struct mmc_blk_ioc_rpmb_data *rpmb_data =
		kzalloc(sizeof(*rpmb_data), GFP_KERNEL);

	hisi_rpmb.blkdev = blkdev_get_by_path(EMMC_RPMB_BLOCK_DEVICE_NAME,
					      O_RDWR | O_NDELAY, hisi_rpmb.dev);

	switch (request->info.state) {
	case RPMB_STATE_IDLE:
		printk("rpmb maybe issue an error\n");
		break;
	case RPMB_STATE_KEY:
		mmc_rpmb_set_key(request, rpmb_data);
		break;
	case RPMB_STATE_RD:
		mmc_rpmb_read_data(request, rpmb_data);
		break;
	case RPMB_STATE_CNT:
		mmc_rpmb_get_counter(request, rpmb_data);
		break;
	case RPMB_STATE_WR_CNT:
		/* TODO add a lock here for counter before write data */
		mutex_lock(&rpmb_counter_lock);
		mmc_rpmb_get_counter(request, rpmb_data);
		break;
	case RPMB_STATE_WR_DATA:
		mmc_rpmb_write_data(request, rpmb_data);
		/* TODO add a unlock for counter after write data */
		break;
	}

	/*
	 * we must unlock rpmb_counter_lock for some condition
	 * 1. RPMB_STATE_WR_CNT and result failed
	 * 2. RPMB_STATE_WR_CNT and result success but RESPONSE not valid
	 * 3. RPMB_STATE_WR_DATA we always unlock
	 */
	result = mmc_blk_ioctl_rpmb_cmd(request->info.func_id, hisi_rpmb.blkdev,
					rpmb_data);
	if ((request->info.state == RPMB_STATE_WR_CNT &&
	     (result || (be16_to_cpu(frame->result) != RPMB_OK ||
			 be16_to_cpu(frame->request) != RPMB_RESP_WCOUNTER))) ||
	    (request->info.state == RPMB_STATE_WR_DATA)) {
		mutex_unlock(&rpmb_counter_lock);
	}

	blkdev_put(hisi_rpmb.blkdev, O_RDWR | O_NDELAY);
	kfree(rpmb_data);

	return result;
}

static int ufs_rpmb_work(struct rpmb_request *request)
{
	int32_t result;
	struct rpmb_frame *transfer_frame[UFS_IOC_MAX_RPMB_CMD];
	uint8_t *sense_buffer[UFS_IOC_MAX_RPMB_CMD];
	struct rpmb_frame *frame = &request->frame[0];
	/*ufs block ioctl rpmb data, include ufs scsi packet -- sg_io_hdr*/
	struct ufs_blk_ioc_rpmb_data *rpmb_data =
		kzalloc(sizeof(*rpmb_data), GFP_KERNEL);
	int i;
	for (i = 0; i < UFS_IOC_MAX_RPMB_CMD; i++) {
		transfer_frame[i] = kzalloc(
			sizeof(struct rpmb_frame) * MAX_RPMB_FRAME, GFP_KERNEL);
		sense_buffer[i] = kzalloc(
			sizeof(uint8_t) * MAX_SENSE_BUFFER_LENGTH, GFP_KERNEL);
		if (NULL == transfer_frame[i]) {
			printk("rpmb maybe issue an error\n");
			goto failed_free_alloc;
		}
		if (NULL == sense_buffer[i]) {
			printk("rpmb maybe issue an error\n");
			goto failed_free_alloc;
		}
	}

	switch (request->info.state) {
	case RPMB_STATE_IDLE:
		printk("rpmb maybe issue an error\n");
		break;
	case RPMB_STATE_KEY:
		ufs_rpmb_set_key(request, rpmb_data, sense_buffer,
				 transfer_frame);
		break;
	case RPMB_STATE_RD:
		ufs_rpmb_read_data(request, rpmb_data, sense_buffer,
				   transfer_frame);
		break;
	case RPMB_STATE_CNT:
		ufs_rpmb_get_counter(request, rpmb_data, sense_buffer,
				     transfer_frame);
		break;
	case RPMB_STATE_WR_CNT:
		/* TODO add a lock here for counter before write data */
		mutex_lock(&rpmb_counter_lock);
		ufs_rpmb_get_counter(request, rpmb_data, sense_buffer,
				     transfer_frame);
		break;
	case RPMB_STATE_WR_DATA:
		ufs_rpmb_write_data(request, rpmb_data, sense_buffer,
				    transfer_frame);
		/* TODO add a unlock for counter after write data */
		break;
	}
	result = ufs_bsg_ioctl_rpmb_cmd(rpmb_data);

	/*
	 * we must unlock rpmb_counter_lock for some condition
	 * 1. RPMB_STATE_WR_CNT and result failed
	 * 2. RPMB_STATE_WR_CNT and result success but RESPONSE not valid
	 * 3. RPMB_STATE_WR_DATA we always unlock
	 */
	if ((request->info.state == RPMB_STATE_WR_CNT &&
	     (result || (be16_to_cpu(frame->result) != RPMB_OK ||
			 be16_to_cpu(frame->request) != RPMB_RESP_WCOUNTER))) ||
	    (request->info.state == RPMB_STATE_WR_DATA)) {
		mutex_unlock(&rpmb_counter_lock);
	}

	/* blkdev_put(hisi_rpmb.blkdev, O_RDWR | O_NDELAY); */
	for (i = 0; i < UFS_IOC_MAX_RPMB_CMD; i++) {
		kfree(transfer_frame[i]);
		kfree(sense_buffer[i]);
	}
	kfree(rpmb_data);

	return result;
failed_free_alloc:
	for (i = 0; i < UFS_IOC_MAX_RPMB_CMD; i++) {
		if (transfer_frame[i] != NULL)
			kfree(transfer_frame[i]);
		if (sense_buffer[i] != NULL)
			kfree(sense_buffer[i]);
	}
	return -1;
}

static void rpmb_work_routine(struct work_struct *work)
{
	unsigned short result;
	struct rpmb_request *request = hisi_rpmb.rpmb_request;
	wake_lock_timeout(&hisi_rpmb.wake_lock, 2 * HZ);
	if (BOOT_DEVICE_EMMC == rpmb_support_device)
		result = mmc_rpmb_work(request);
	else
		result = ufs_rpmb_work(request);

	atfd_hisi_rpmb_smc(RPMB_SVC_REQUEST_DONE, result, 0, 0);

	/*wake_unlock(&hisi_rpmb.wake_lock);*/
}

/*
 * hisi_rpmb_active - handle rpmb request from ATF
 */
void hisi_rpmb_active(void)
{
	queue_work(hisi_rpmb.rpmb_workqueue, &hisi_rpmb.rpmb_work);
}

EXPORT_SYMBOL(hisi_rpmb_active);

#ifdef CONFIG_DEBUG_FS
/*
 * debugfs defination start here
 */
static int get_counter_fops_get(void *data, u64 *val)
{
	atfd_hisi_rpmb_smc(RPMB_SVC_COUNTER, 0x0, 0x0, 0x0);
	return 0;
}

DEFINE_SIMPLE_ATTRIBUTE(get_counter_fops, get_counter_fops_get, NULL, "%llu\n");

static int read_fops_get(void *data, u64 *val)
{
	atfd_hisi_rpmb_smc(RPMB_SVC_READ, 0x0, 0x0, 0x0);
	return 0;
}

DEFINE_SIMPLE_ATTRIBUTE(read_fops, read_fops_get, NULL, "%llu\n");

static int write_fops_get(void *data, u64 *val)
{
	atfd_hisi_rpmb_smc(RPMB_SVC_WRITE, 0x0, 0x0, 0x0);
	return 0;
}

DEFINE_SIMPLE_ATTRIBUTE(write_fops, write_fops_get, NULL, "%llu\n");
/*
static int key_fops_get(void *data, u64 *val)
{
	atfd_hisi_rpmb_smc(RPMB_SVC_SET_KEY_SE, 0x0, 0x0, 0x0);
	return 0;
}
DEFINE_SIMPLE_ATTRIBUTE(key_fops, key_fops_get, NULL, "%llu\n");
*/

static int hisi_rpmb_debugfs_init(void)
{
	/* debugfs for debug only */
	struct dentry *debugfs_hisi_rpmb = NULL;
	struct rpmb_request *request = hisi_rpmb.rpmb_request;
	debugfs_hisi_rpmb = debugfs_create_dir("hisi_rpmb", NULL);

	debugfs_create_file("get_counter", S_IRUSR | S_IWUSR, debugfs_hisi_rpmb,
			    NULL, &get_counter_fops);
	debugfs_create_file("read", S_IRUSR | S_IWUSR, debugfs_hisi_rpmb, NULL,
			    &read_fops);
	debugfs_create_file("write", S_IRUSR | S_IWUSR, debugfs_hisi_rpmb, NULL,
			    &write_fops);
	/* debugfs_create_file("set_key", S_IRUSR | S_IWUSR, debugfs_hisi_rpmb,
	 * NULL, &key_fops); */

	debugfs_create_u32("key_id", S_IRUSR | S_IWUSR, debugfs_hisi_rpmb,
			   &request->rpmb_debug.key_id);
	debugfs_create_u32("write_value", S_IRUSR | S_IWUSR, debugfs_hisi_rpmb,
			   (u32 *)&request->rpmb_debug.write_value);
	debugfs_create_u32("func_id", S_IRUSR | S_IWUSR, debugfs_hisi_rpmb,
			   &request->rpmb_debug.func_id);
	debugfs_create_u16("start", S_IRUSR | S_IWUSR, debugfs_hisi_rpmb,
			   &request->rpmb_debug.start);
	debugfs_create_u16("block_count", S_IRUSR | S_IWUSR, debugfs_hisi_rpmb,
			   &request->rpmb_debug.block_count);
	debugfs_create_u8("result", S_IRUSR | S_IWUSR, debugfs_hisi_rpmb,
			  &request->rpmb_debug.result);

	return 0;
}
#endif
static ssize_t mmc_rpmb_key_store(struct device *dev,
				  struct device_attribute *attr,
				  const char *buf,
				  size_t count)
{
	int ret = 0;
	int i;
	struct rpmb_request *request = hisi_rpmb.rpmb_request;
	struct rpmb_frame *frame = &request->frame[0];
	struct mmc_blk_ioc_rpmb_data *rpmb_data =
		kzalloc(sizeof(*rpmb_data), GFP_KERNEL);
	if (!rpmb_data) {
		dev_err(dev, "alloc rpmb_data failed\n");
		return -1;
	}

	/* before set key must init the request */
	request->key_frame_status = KEY_NOT_READY;
	memset(frame, 0, sizeof(struct rpmb_frame));

	pr_err("key frame init\n");

#ifdef TEST_AT_SET_KEY_PRINT_DEBUG
	for (i = 0; i < RPMB_SZ_MAC; i++) {
		pr_err("%x ", frame[0].mac[i]);
	}
#endif

	if (strncmp(buf, "set_key", count)) {
		dev_err(dev, "invalid key set command\n");
		kfree(rpmb_data);
		return -1;
	}

	hisi_rpmb.blkdev = blkdev_get_by_path(EMMC_RPMB_BLOCK_DEVICE_NAME,
					      O_RDWR | O_NDELAY, hisi_rpmb.dev);
	if (IS_ERR(hisi_rpmb.blkdev)) {
		dev_err(dev, "blkdev get mmcblk0rpmb failed\n");
		goto alloc_free;
	}

	/* get key from bl31 */
	ret = atfd_hisi_rpmb_smc(RPMB_SVC_SET_KEY, 0x0, 0x0, 0x0);
	if (ret) {
		dev_err(dev, "get rpmb key frame failed\n");
		goto alloc_free;
	}

	for (i = 0; i < WAIT_KEY_FRAME_TIMES; i++) {
		if (request->key_frame_status == KEY_READY)
			break;
		mdelay(5);
	}
	if (i == WAIT_KEY_FRAME_TIMES) {
		dev_err(dev, "wait for key frame ready timeout\n");
		pr_err("wait for key frame ready timeout\n");
		goto alloc_free;
	}

	pr_err("key frame from atf\n");

#ifdef TEST_AT_SET_KEY_PRINT_DEBUG
	for (i = 0; i < RPMB_SZ_MAC; i++) {
		pr_err("%x ", frame[0].mac[i]);
	}
	pr_err("\n");
	pr_err("-------------------rpmb_frame_set_key rpmb "
	       "frame(first)-------------------\n");
	pr_err("write_counter:0x%x  , address:0x%x  \n", frame[0].write_counter,
	       frame[0].address);
	pr_err("block_count:0x%x  , result:0x%x  \n", frame[0].block_count,
	       frame[0].result);
	pr_err("request/response:0x%x  \n", frame[0].request);
#endif

	mmc_rpmb_set_key(request, rpmb_data);
	ret = mmc_blk_ioctl_rpmb_cmd(request->info.func_id, hisi_rpmb.blkdev,
				     rpmb_data);

	blkdev_put(hisi_rpmb.blkdev, O_RDWR | O_NDELAY);
alloc_free:
	kfree(rpmb_data);
	if (ret || (be16_to_cpu(frame->result) != RPMB_OK ||
		    be16_to_cpu(frame->request) != RPMB_RESP_KEY))
		return -1;
	return count;
}

static ssize_t ufs_rpmb_key_store(struct device *dev,
				  struct device_attribute *attr,
				  const char *buf,
				  size_t count)
{
	int i, ret = 0;
	;
	struct rpmb_request *request = hisi_rpmb.rpmb_request;
	struct rpmb_frame *frame = &request->frame[0];

	struct rpmb_frame *transfer_frame[UFS_IOC_MAX_RPMB_CMD];
	uint8_t *sense_buffer[UFS_IOC_MAX_RPMB_CMD];
	struct ufs_blk_ioc_rpmb_data *rpmb_data =
		kzalloc(sizeof(*rpmb_data), GFP_KERNEL);
	for (i = 0; i < UFS_IOC_MAX_RPMB_CMD; i++) {
		transfer_frame[i] = kzalloc(
			sizeof(struct rpmb_frame) * MAX_RPMB_FRAME, GFP_KERNEL);
		sense_buffer[i] = kzalloc(
			sizeof(uint8_t) * MAX_SENSE_BUFFER_LENGTH, GFP_KERNEL);
		if (NULL == transfer_frame[i]) {
			printk("rpmb maybe issue an error\n");
			goto alloc_free;
		}
		if (NULL == sense_buffer[i]) {
			printk("rpmb maybe issue an error\n");
			goto alloc_free;
		}
	}
	if (!rpmb_data) {
		dev_err(dev, "alloc rpmb_data failed\n");
		return -1;
	}

	/* before set key must init the request */
	request->key_frame_status = KEY_NOT_READY;
	memset(frame, 0, sizeof(struct rpmb_frame));

	pr_err("key frame init\n");

#ifdef TEST_AT_SET_KEY_PRINT_DEBUG
	for (i = 0; i < RPMB_SZ_MAC; i++) {
		pr_err("%x ", frame[0].mac[i]);
	}
#endif

	if (strncmp(buf, "set_key", count)) {
		dev_err(dev, "invalid key set command\n");
		kfree(rpmb_data);
		return -1;
	}

	/* get key from bl31 */
	ret = atfd_hisi_rpmb_smc(RPMB_SVC_SET_KEY, 0x0, 0x0, 0x0);
	if (ret) {
		dev_err(dev, "get rpmb key frame failed\n");
		goto alloc_free;
	}

	for (i = 0; i < WAIT_KEY_FRAME_TIMES; i++) {
		if (request->key_frame_status == KEY_READY)
			break;
		mdelay(5);
	}
	if (i == WAIT_KEY_FRAME_TIMES) {
		dev_err(dev, "wait for key frame ready timeout\n");
		pr_err("wait for key frame ready timeout\n");
		goto alloc_free;
	}

	pr_err("key frame from atf\n");

#ifdef TEST_AT_SET_KEY_PRINT_DEBUG
	for (i = 0; i < RPMB_SZ_MAC; i++) {
		pr_err("%x ", frame[0].mac[i]);
	}

	pr_err("\n");
	pr_err("-------------------rpmb_frame_set_key rpmb "
	       "frame(first)-------------------\n");
	pr_err("write_counter:0x%x  , address:0x%x  \n", frame[0].write_counter,
	       frame[0].address);
	pr_err("block_count:0x%x  , result:0x%x  \n", frame[0].block_count,
	       frame[0].result);
	pr_err("request/response:0x%x  \n", frame[0].request);
#endif

	ufs_rpmb_set_key(request, rpmb_data, sense_buffer, transfer_frame);
	ret = ufs_bsg_ioctl_rpmb_cmd(rpmb_data);

alloc_free:
	kfree(rpmb_data);
	for (i = 0; i < UFS_IOC_MAX_RPMB_CMD; i++) {
		if (transfer_frame[i] != NULL)
			kfree(transfer_frame[i]);
		if (sense_buffer[i] != NULL)
			kfree(sense_buffer[i]);
	}
	if (ret || (be16_to_cpu(frame->result) != RPMB_OK ||
		    be16_to_cpu(frame->request) != RPMB_RESP_KEY))
		return -1;
	return count;
}

static ssize_t
mmc_rpmb_key_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int ret = -1;
	struct rpmb_request *request = hisi_rpmb.rpmb_request;
	struct rpmb_frame *frame = &request->frame[0];

	struct mmc_blk_ioc_rpmb_data *rpmb_data =
		kzalloc(sizeof(*rpmb_data), GFP_KERNEL);

	if (!rpmb_data) {
		dev_err(dev, "alloc rpmb_data failed\n");
		return -1;
	}

	hisi_rpmb.blkdev = blkdev_get_by_path(EMMC_RPMB_BLOCK_DEVICE_NAME,
					      O_RDWR | O_NDELAY, hisi_rpmb.dev);
	if (IS_ERR(hisi_rpmb.blkdev)) {
		dev_err(dev, "blkdev get mmcblk0rpmb failed\n");
		goto alloc_free;
	}

	memset(frame, 0, sizeof(struct rpmb_frame));
	frame->request = cpu_to_be16(RPMB_REQ_WCOUNTER);

	mmc_rpmb_get_counter(hisi_rpmb.rpmb_request, rpmb_data);
	ret = mmc_blk_ioctl_rpmb_cmd(request->info.func_id, hisi_rpmb.blkdev,
				     rpmb_data);

	blkdev_put(hisi_rpmb.blkdev, O_RDWR | O_NDELAY);
alloc_free:
	kfree(rpmb_data);
	if (ret) {
		dev_err(dev, "can not get rpmb key status\n");
		goto false_key;
	}

	if (be16_to_cpu(frame->result) != RPMB_OK ||
	    be16_to_cpu(frame->request) != RPMB_RESP_WCOUNTER) {
		goto false_key;
	}

	strncpy(buf, "true", sizeof("true"));
	return strlen(buf);
false_key:
	strncpy(buf, "false", sizeof("false"));
	return strlen(buf);
}

static ssize_t
ufs_rpmb_key_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int ret = -1;
	int i;
	struct rpmb_request *request = hisi_rpmb.rpmb_request;
	struct rpmb_frame *frame = &request->frame[0];

	struct rpmb_frame *transfer_frame[UFS_IOC_MAX_RPMB_CMD];
	uint8_t *sense_buffer[UFS_IOC_MAX_RPMB_CMD];
	struct ufs_blk_ioc_rpmb_data *rpmb_data =
		kzalloc(sizeof(*rpmb_data), GFP_KERNEL);
	for (i = 0; i < UFS_IOC_MAX_RPMB_CMD; i++) {
		transfer_frame[i] = kzalloc(
			sizeof(struct rpmb_frame) * MAX_RPMB_FRAME, GFP_KERNEL);
		sense_buffer[i] = kzalloc(
			sizeof(uint8_t) * MAX_SENSE_BUFFER_LENGTH, GFP_KERNEL);
		if (NULL == transfer_frame[i]) {
			printk("rpmb maybe issue an error\n");
			goto alloc_free;
		}
		if (NULL == sense_buffer[i]) {
			printk("rpmb maybe issue an error\n");
			goto alloc_free;
		}
	}

	if (!rpmb_data) {
		dev_err(dev, "alloc rpmb_data failed\n");
		return -1;
	}

	memset(frame, 0, sizeof(struct rpmb_frame));
	frame->request = cpu_to_be16(RPMB_REQ_WCOUNTER);

	ufs_rpmb_get_counter(hisi_rpmb.rpmb_request, rpmb_data, sense_buffer,
			     transfer_frame);
	ret = ufs_bsg_ioctl_rpmb_cmd(rpmb_data);
alloc_free:
	kfree(rpmb_data);
	for (i = 0; i < UFS_IOC_MAX_RPMB_CMD; i++) {
		if (transfer_frame[i] != NULL)
			kfree(transfer_frame[i]);
		if (sense_buffer[i] != NULL)
			kfree(sense_buffer[i]);
	}
	if (ret) {
		dev_err(dev, "can not get rpmb key status\n");
		goto false_key;
	}

	if (be16_to_cpu(frame->result) != RPMB_OK ||
	    be16_to_cpu(frame->request) != RPMB_RESP_WCOUNTER) {
		goto false_key;
	}

	strncpy(buf, "true", sizeof("true"));
	return strlen(buf);
false_key:
	strncpy(buf, "false", sizeof("false"));
	return strlen(buf);
}

static ssize_t hisi_rpmb_key_store(struct device *dev,
				   struct device_attribute *attr,
				   const char *buf,
				   size_t count)
{
	int ret = count;
	if (BOOT_DEVICE_EMMC == rpmb_support_device)
		ret = mmc_rpmb_key_store(dev, attr, buf, count);
	else
		ret = ufs_rpmb_key_store(dev, attr, buf, count);
	return ret;
}

static ssize_t
hisi_rpmb_key_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int ret = 0;
	if (BOOT_DEVICE_EMMC == rpmb_support_device)
		ret = mmc_rpmb_key_show(dev, attr, buf);
	else
		ret = ufs_rpmb_key_show(dev, attr, buf);
	return ret;
}

static DEVICE_ATTR(rpmb_key,
		   (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP),
		   hisi_rpmb_key_show,
		   hisi_rpmb_key_store);

#if 0


static inline void hisi_ufs_read_frame_recombine(struct storage_blk_ioc_rpmb_data *storage_data)
{
	int i;
	int blocks;
	for(i = 0;i < STORAGE_IOC_MAX_RPMB_CMD; i++){
		blocks = storage_data->data[i].blocks;
		storage_data->data[i].buf[506] = (uint8_t)((blocks & 0xFF00) >>8); /*num of rpmb blks MSB*/
		storage_data->data[i].buf[507] = (uint8_t)(blocks & 0xFF); /*num of rpmb blks MSB*/
	}
	return 0;
}
int hisi_rpmb_ioctl_cmd(enum func_id id, enum rpmb_op_type operation, struct storage_blk_ioc_rpmb_data *storage_data){
	int ret;
	int i;
	for(i = 0;i < WAIT_KEY_FRAME_TIMES;i++){
		if(RPMB_DRIVER_IS_READY == rpmb_drivers_init_status)
			break;
		mdelay(5);
	}
	if(i == WAIT_KEY_FRAME_TIMES){
		pr_err("wait for rpmb init ready timeout\n");
	    goto error;
	}

	if(BOOT_DEVICE_EMMC == rpmb_support_device){
		ret = hisi_mmc_rpmb_ioctl_cmd(id, operation, storage_data);
	}
	else{
		ret = hisi_ufs_rpmb_ioctl_cmd(id, operation, storage_data);
	}
	return ret;
	
	error:
		return -1;
}

int hisi_mmc_rpmb_ioctl_cmd(enum func_id id, enum rpmb_op_type operation, struct storage_blk_ioc_rpmb_data *storage_data){
	struct block_device *bdev;
	int ret;
	int i;
	struct mmc_blk_ioc_rpmb_data *rpmb_data = kzalloc(sizeof(*rpmb_data), GFP_KERNEL);
	bdev = blkdev_get_by_path(EMMC_RPMB_BLOCK_DEVICE_NAME, FMODE_READ | FMODE_WRITE | FMODE_NDELAY, 0);
    if (IS_ERR(bdev)) {
		pr_err("Secure OS open rpmb block failed!\n");
        return -1;
    }

	for(i = 0;i < STORAGE_IOC_MAX_RPMB_CMD; i++){
		rpmb_data->data[i].buf = storage_data->data[i].buf;
		rpmb_data->data[i].buf_bytes = storage_data->data[i].buf_bytes;
	}
	switch (operation) {
		case RPMB_OP_RD:
			mmc_rpmb_combine_cmd(&rpmb_data->data[0], true, storage_data->data[0].blocks, RPMB_BLK_SZ, false);
			mmc_rpmb_combine_cmd(&rpmb_data->data[1], false, storage_data->data[1].blocks, RPMB_BLK_SZ, false);
			break;
		case RPMB_OP_WR_CNT:
			mmc_rpmb_combine_cmd(&rpmb_data->data[0], true, storage_data->data[0].blocks, RPMB_BLK_SZ, false);
			mmc_rpmb_combine_cmd(&rpmb_data->data[1], false, storage_data->data[1].blocks, RPMB_BLK_SZ, false);
		    break;
		case RPMB_OP_WR_DATA:
			mmc_rpmb_combine_cmd(&rpmb_data->data[0], true, storage_data->data[0].blocks, RPMB_BLK_SZ, true);
			mmc_rpmb_combine_cmd(&rpmb_data->data[1], true, storage_data->data[1].blocks, RPMB_BLK_SZ, false);
			mmc_rpmb_combine_cmd(&rpmb_data->data[2], false, storage_data->data[2].blocks, RPMB_BLK_SZ, false);
			break;
	}
	ret = mmc_blk_ioctl_rpmb_cmd(id, bdev, rpmb_data);
	blkdev_put(bdev, FMODE_READ | FMODE_WRITE | FMODE_NDELAY);
	kfree(rpmb_data);
	return ret;
}

int hisi_ufs_rpmb_ioctl_cmd(enum func_id id, enum rpmb_op_type operation, struct storage_blk_ioc_rpmb_data *storage_data){
	int ret;
	int i;
	struct ufs_blk_ioc_rpmb_data *rpmb_data = kzalloc(sizeof(*rpmb_data), GFP_KERNEL);
	uint8_t *sense_buffer[UFS_IOC_MAX_RPMB_CMD];

	for(i=0;i < UFS_IOC_MAX_RPMB_CMD;i++){
		sense_buffer[i] = kzalloc(sizeof(uint8_t) * MAX_SENSE_BUFFER_LENGTH, GFP_KERNEL);
		if(NULL == sense_buffer[i]){
			printk("rpmb maybe issue an error\n");
			goto failed_free_alloc;
		}
	}

	if(RPMB_OP_RD == operation)
		hisi_ufs_read_frame_recombine(storage_data);
	
	for(i = 0;i < STORAGE_IOC_MAX_RPMB_CMD; i++){
		rpmb_data->data[i].buf = storage_data->data[i].buf;
		rpmb_data->data[i].buf_bytes = storage_data->data[i].buf_bytes;
	}
	switch (operation) {
		case RPMB_OP_RD:
			ufs_rpmb_combine_cmd(&rpmb_data->data[0], true, storage_data->data[0].blocks, RPMB_BLK_SZ, &rpmb_data->sdb_command[0], sense_buffer[0], rpmb_data->data[0].buf);
			ufs_rpmb_combine_cmd(&rpmb_data->data[1], false, storage_data->data[1].blocks, RPMB_BLK_SZ, &rpmb_data->sdb_command[1], sense_buffer[1], rpmb_data->data[1].buf);
			break;
		case RPMB_OP_WR_CNT:
			ufs_rpmb_combine_cmd(&rpmb_data->data[0], true, storage_data->data[0].blocks, RPMB_BLK_SZ, &rpmb_data->sdb_command[0], sense_buffer[0], rpmb_data->data[0].buf);
			ufs_rpmb_combine_cmd(&rpmb_data->data[1], false, storage_data->data[1].blocks, RPMB_BLK_SZ, &rpmb_data->sdb_command[1], sense_buffer[1], rpmb_data->data[1].buf);
		    break;
		case RPMB_OP_WR_DATA:
			ufs_rpmb_combine_cmd(&rpmb_data->data[0], true, storage_data->data[0].blocks, RPMB_BLK_SZ, &rpmb_data->sdb_command[0], sense_buffer[0], rpmb_data->data[0].buf);
			ufs_rpmb_combine_cmd(&rpmb_data->data[1], true, storage_data->data[1].blocks, RPMB_BLK_SZ, &rpmb_data->sdb_command[1], sense_buffer[1], rpmb_data->data[1].buf);
			ufs_rpmb_combine_cmd(&rpmb_data->data[2], false, storage_data->data[2].blocks, RPMB_BLK_SZ, &rpmb_data->sdb_command[2], sense_buffer[2], rpmb_data->data[2].buf);
			break;
	}
	ret = ufs_bsg_ioctl_rpmb_cmd(rpmb_data);
	for(i=0;i < UFS_IOC_MAX_RPMB_CMD;i++){
		kfree(sense_buffer[i]);
	}
	kfree(rpmb_data);

	return ret;

	failed_free_alloc:
	for(i=0;i < UFS_IOC_MAX_RPMB_CMD;i++){
		if(sense_buffer[i] != NULL)
			kfree(sense_buffer[i]);
	}
	return -1;
}

#endif

static struct mmc_blk_ioc_rpmb_data *
mmc_blk_ioctl_rpmb_copy_data(struct mmc_blk_ioc_rpmb_data *rdata)
{
	struct mmc_blk_ioc_rpmb_data *idata;
	int err, i;

	idata = kzalloc(sizeof(*idata), GFP_KERNEL);
	if (!idata) {
		err = -ENOMEM;
		goto out;
	}

	for (i = 0; i < MMC_IOC_MAX_RPMB_CMD; i++) {
		idata->data[i].buf_bytes = rdata->data[i].buf_bytes;
		idata->data[i].buf =
			kzalloc(idata->data[i].buf_bytes, GFP_KERNEL);
		if (IS_ERR(idata->data[i].buf)) {
			err = PTR_ERR(idata->data[i].buf);
			goto copy_err;
		}
		memcpy(&idata->data[i].ic, &rdata->data[i].ic,
		       sizeof(struct mmc_ioc_cmd));
		memcpy(idata->data[i].buf, rdata->data[i].buf,
		       idata->data[i].buf_bytes);
	}

	return idata;

copy_err:
	while (--i >= 0) {
		kfree(idata->data[i].buf);
	}
	kfree(idata);
out:
	return ERR_PTR(err);
}

/*output api ++*/
struct mmc_card *get_mmc_card(struct block_device *bdev)
{
	struct mmc_blk_data *md;
	struct mmc_card *card;

	md = mmc_blk_get(bdev->bd_disk);
	if (!md) {
		return NULL;
	}

	card = md->queue.card;
	if (IS_ERR(card)) {
		return NULL;
	}

	return card;
}
EXPORT_SYMBOL(get_mmc_card);

/*This function is responsible for handling RPMB command and is the interface
 *with the eMMC driver.
 *It is used by BL31 and SecureOS.So when modify the fuction please check it
 *with SecureOS.
 *During DMA 64bit development, we modify it using the method of memory copy.
 *idata:the parameter consist of  two command at least and three commd at most,
 *so when copy retuning
 *      data, please confirm copy all the retuning data not include write
 *command.
 */
#if CONFIG_HISI_MMC_SECURE_RPMB
int mmc_blk_ioctl_rpmb_cmd(enum func_id id,
			   struct block_device *bdev,
			   struct mmc_blk_ioc_rpmb_data *rdata)
{
	struct mmc_blk_data *md;
	struct mmc_card *card;
	struct mmc_command cmd = {0};
	struct mmc_data data = {0};
	struct mmc_request mrq = {NULL};
	struct scatterlist sg;
	struct mmc_blk_ioc_rpmb_data *idata = NULL;
	int err = 0, i = 0;
	u32 status = 0;

#if 0
	/* The caller must have CAP_SYS_RAWIO */
	if (!capable(CAP_SYS_RAWIO))
		return -EPERM;
#endif
	md = mmc_blk_get(bdev->bd_disk);
	/* make sure this is a rpmb partition */
	if ((!md) || (!(md->area_type & MMC_BLK_DATA_AREA_RPMB))) {
		err = -EINVAL;
		return err;
	}

	idata = mmc_blk_ioctl_rpmb_copy_data(rdata);
	if (IS_ERR(idata)) {
		err = PTR_ERR(idata);
		goto cmd_done;
	}

	card = md->queue.card;
	if (IS_ERR(card)) {
		err = PTR_ERR(card);
		goto idata_free;
	}

	mmc_get_card(card);
	/*mmc_claim_host(card->host);*/

	err = mmc_blk_part_switch(card, md);
	if (err)
		goto cmd_rel_host;

	for (i = 0; i < MMC_IOC_MAX_RPMB_CMD; i++) {
		struct mmc_blk_ioc_data *curr_data;
		struct mmc_ioc_cmd *curr_cmd;

		curr_data = &idata->data[i];
		curr_cmd = &curr_data->ic;
		if (!curr_cmd->opcode)
			break;

		cmd.opcode = curr_cmd->opcode;
		cmd.arg = curr_cmd->arg;
		cmd.flags = curr_cmd->flags;

		if (curr_data->buf_bytes) {
			data.sg = &sg;
			data.sg_len = 1;
			data.blksz = curr_cmd->blksz;
			data.blocks = curr_cmd->blocks;

			sg_init_one(data.sg, curr_data->buf,
				    curr_data->buf_bytes);

			if (curr_cmd->write_flag)
				data.flags = MMC_DATA_WRITE;
			else
				data.flags = MMC_DATA_READ;

			/* data.flags must already be set before doing this. */
			mmc_set_data_timeout(&data, card);

			/*
			 * Allow overriding the timeout_ns for empirical tuning.
			 */
			if (curr_cmd->data_timeout_ns)
				data.timeout_ns = curr_cmd->data_timeout_ns;

			mrq.data = &data;
		}

		mrq.cmd = &cmd;

		err = mmc_set_blockcount(card, data.blocks,
					 curr_cmd->write_flag & (1 << 31));
		if (err)
			goto cmd_rel_host;

		mmc_wait_for_req(card->host, &mrq);

		if (cmd.error) {
			dev_err(mmc_dev(card->host), "%s: cmd error %d\n",
				__func__, cmd.error);
			err = cmd.error;
			goto cmd_rel_host;
		}
		if (data.error) {
			dev_err(mmc_dev(card->host), "%s: data error %d\n",
				__func__, data.error);
			err = data.error;
			goto cmd_rel_host;
		}

		memcpy(curr_cmd->response, cmd.resp, sizeof(cmd.resp));

		if (!curr_cmd->write_flag) {
			memcpy(rdata->data[i].buf, curr_data->buf,
			       curr_data->buf_bytes);
		}

		/*
		 * Ensure RPMB command has completed by polling CMD13
		 * "Send Status".
		 */
		err = ioctl_rpmb_card_status_poll(card, &status, 5);
		if (err)
			dev_err(mmc_dev(card->host),
				"%s: Card Status=0x%08X, error %d\n", __func__,
				status, err);
	}

cmd_rel_host:
	mmc_put_card(card);
/*mmc_release_host(card->host);*/

idata_free:
	for (i = 0; i < MMC_IOC_MAX_RPMB_CMD; i++)
		kfree(idata->data[i].buf);

	kfree(idata);

cmd_done:
	mmc_blk_put(md);

	return err;
}
#endif

/*output api --*/

/* create a virtual device for dma_alloc */
#define SECURE_STORAGE_NAME "secure_storage"
#define RPMB_DEVICE_NAME "hisi_rpmb"
static int hisi_rpmb_device_init(void)
{
	struct device *pdevice;
	struct class *rpmb_class;
	struct device_node *np = NULL;
	dma_addr_t rpmb_request_phy = 0;
	unsigned long mem_size = 0;
	phys_addr_t bl31_smem_base =
		HISI_SUB_RESERVED_BL31_SHARE_MEM_PHYMEM_BASE;
	u32 data[2] = {0};

	rpmb_class = class_create(THIS_MODULE, SECURE_STORAGE_NAME);
	if (IS_ERR(rpmb_class))
		return PTR_ERR(rpmb_class);

	pdevice = device_create(rpmb_class, NULL, MKDEV(0, 0), NULL,
				RPMB_DEVICE_NAME);
	if (IS_ERR(pdevice))
		goto err_class_destroy;

	hisi_rpmb.dev = pdevice;

	if (device_create_file(pdevice, &dev_attr_rpmb_key)) {
		pr_err("rpmb error: unable to create sysfs attributes\n");
		goto err_device_destroy;
	}

	np = of_find_compatible_node(NULL, NULL, "hisilicon,hisi-rpmb");
	if (!np) {
		pr_err("rpmb err of_find_compatible_node");
		goto err_device_remove_file;
	}

	if (of_property_read_u32_array(np, "hisi,bl31-share-mem", &data[0],
				       2)) {
		pr_err("rpmb get share_mem_address failed\n");
		goto err_node;
	}

	rpmb_request_phy = bl31_smem_base + data[0];
	mem_size = data[1];

	hisi_rpmb.rpmb_request = ioremap_wc(rpmb_request_phy, mem_size);
	if (!hisi_rpmb.rpmb_request)
		goto err_node;

	if (atfd_hisi_rpmb_smc(RPMB_SVC_REQUEST_ADDR, rpmb_request_phy,
			       (u64)rpmb_support_device, 0x0)) {
		printk("rpmb set shared memory address failed\n");
		goto err_ioremap;
	}
	return 0;

err_ioremap:
	iounmap(hisi_rpmb.rpmb_request);
err_node:
	of_node_put(np);
err_device_remove_file:
	device_remove_file(pdevice, &dev_attr_rpmb_key);
err_device_destroy:
	device_destroy(rpmb_class, pdevice->devt);
err_class_destroy:
	class_destroy(rpmb_class);
	return -1;
}

int get_rpmb_init_status(void)
{
	return rpmb_drivers_init_status;
}

static int __init hisi_rpmb_init(void)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 1, 0)
	hisi_rpmb.rpmb_workqueue = alloc_workqueue(
		"hisi-rpmb-queue",
		WQ_MEM_RECLAIM | WQ_NON_REENTRANT | WQ_FREEZABLE, 1);
#else
	hisi_rpmb.rpmb_workqueue =
		alloc_workqueue("hisi-rpmb-queue", WQ_MEM_RECLAIM, 1);
#endif
	if (!hisi_rpmb.rpmb_workqueue)
		return -1;
	rpmb_support_device = get_bootdevice_type();
	INIT_WORK(&hisi_rpmb.rpmb_work, rpmb_work_routine);

	if (hisi_rpmb_device_init())
		goto err_destroy_workqueue;
	rpmb_drivers_init_status = RPMB_DRIVER_IS_READY;

	wake_lock_init(&hisi_rpmb.wake_lock, WAKE_LOCK_SUSPEND,
		       "hisi-rpmb-wakelock");

#ifdef CONFIG_DEBUG_FS
	hisi_rpmb_debugfs_init();
#endif

	return 0;

err_destroy_workqueue:
	destroy_workqueue(hisi_rpmb.rpmb_workqueue);

	return -1;
}

late_initcall(hisi_rpmb_init);

MODULE_AUTHOR("zangleigang@hisilicon.com>");
MODULE_DESCRIPTION("Hisilicon Secure RPMB.");
MODULE_LICENSE("GPL v2");
#pragma GCC diagnostic pop