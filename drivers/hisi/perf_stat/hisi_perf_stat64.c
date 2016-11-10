/*
  * hisilicon perf_stat driver, hisi_perf_stat.c
  *
  * Copyright (c) 2013 Hisilicon Technologies CO., Ltd.
  *
  */

#include <asm/memory.h>
#include <asm/cacheflush.h>
#include <asm/irq_regs.h>
#include <asm/irqflags.h>
/*lint -e451*/
#include <linux/bug.h>
/*lint +e451*/
#include <linux/uaccess.h>

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/mm.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/device.h>
#include <linux/stddef.h>
#include <linux/types.h>
#include <linux/timer.h>
#include <linux/platform_device.h>
#include <linux/perf_event.h>
#include <linux/bitmap.h>
#include <linux/vmalloc.h>
#include <linux/gfp.h>
#include <linux/dma-direction.h>
#include <linux/ptrace.h>
#include <linux/mutex.h>
#include <linux/printk.h>
#include <linux/wakelock.h>
#include <linux/regulator/consumer.h>
#include <linux/syscalls.h>
#include <linux/workqueue.h>
#include <soc_pmctrl_interface.h>
#include <soc_acpu_baseaddr_interface.h>
#include <soc_crgperiph_interface.h>
#include <soc_perfstat_interface.h>
#include "hisi_perf_stat64.h"
#include <linux/atomic.h>

static char *master_name_hi3660[PERF_PORT_NUM] = { "perf-dsssubsys0",	/*0:dss0 */
	"perf-dsssubsys1",	/*1:dss1 */
	"perf-ccisubsys0",			/*2:cci0 */
	"perf-ccisubsys1",			/*3:cci1 */
	"perf-g3d",		/*4:gpu */
	"perf-modemsubsys0",			/*5:modem0 */
	"perf-modemsubsys1",			/*6:modem1 */
	"perf-ispsubsys0",	/*7:isp0 */
	"perf-ispsubsys1",	/*8:isp1 */
	"perf-vdec",		/*8:vdec */
	"perf-venc",		/*10:venc */
	"perf-ivp",		/*11:ivp */
	"perf-system noc",			/*12:system noc */
	"perf-asp",		/*13:asp */
	"perf-emmc",			/*14:EMMC */
	"perf-usbotg",			/*15:usbotg */
	"perf-usf",
	"perf-pcie",
	"perf-ispsrtdrm",
	"perf-ccisubsys2",			/*19:cci2 */
	"perf-ccisubsys3",			/*20:cci3 */
	"perf-dmsssubsys0",			/*21:dmss0 */
	"perf-dmsssubsys1",			/*22:dmss1 */
	"perf-gpu1",
	"perf-ipf",
	"perf-dmca",			/*25:dmc a */
	"perf-dmcb",			/*26:dmc b */
	"perf-dmcc",			/*27:dmc c */
	"perf-dmcd",			/*28:dmc d */
	"reserved",			/*29:reserve */
	"reserved",			/*30:reserve */
	"reserved"			/*31:reserve */
};

static char **master_name;

static struct perfstat_dev_info *perfstat_info;
static void __iomem *perfstat_base;
static void __iomem *pericrg_base;
static unsigned long sample_time;
static bool perf_file_saved;	/* default value is false */
struct completion perf_comp;
static bool is_probe_comp;
static struct mutex perfstat_transaction_mutex;
static atomic_t is_recording = ATOMIC_INIT(0);
static unsigned int suggest_clk;
static unsigned int suggest_aclk;
static unsigned int fpga_flag;

static struct of_device_id of_perfstat_match_tbl[] = {
	{
	 .compatible = "hisilicon,hi3660-perfstat-driver",
	 .data = master_name_hi3660,
	 },
	{ /* end */ }
};

static void perfstat_reset_en(void)
{
	writel(1 << SOC_CRGPERIPH_PERRSTEN0_ip_rst_perf_stat_START,
	       SOC_CRGPERIPH_PERRSTEN0_ADDR(pericrg_base));
	pr_info("[%s]\n", __func__);
}

static void perfstat_reset_dis(void)
{
	writel(1 << SOC_CRGPERIPH_PERRSTDIS0_ip_rst_perf_stat_START,
	       SOC_CRGPERIPH_PERRSTDIS0_ADDR(pericrg_base));
	pr_info("[%s]\n", __func__);
}

static void perfstat_set_sample_cnt(int cnt)
{
	writel(cnt, SOC_PERFSTAT_SAMPLE_CNT_REG_ADDR(perfstat_base));
}

static void perfstat_set_sample_period(int usec)
{
	int value;
	struct perfstat_dev_info *dev_info = perfstat_info;

	value = usec * (int)(dev_info->clk_perf_stat_rate / PERF_MILLION);
	pr_err("[%s] clk_perf_stat_rate is 0x%lx\n", __func__,
	       dev_info->clk_perf_stat_rate);
	perfstat_set_sample_cnt(value);
}

static void perfstat_set_sample_num(int times)
{
	writel(times, SOC_PERFSTAT_SAMPLE_NUM_REG_ADDR(perfstat_base));
}

static void perfstat_set_sample_port(int port)
{
	writel(port, SOC_PERFSTAT_REAL_PORT_NUM_ADDR(perfstat_base));
}

static void perfstat_set_addr_mode(int mode)
{
	writel(mode, SOC_PERFSTAT_AXI_ADDR_MODE_ADDR(perfstat_base));
}

static int perfstat_set_seq_mode_addr(u64 addr)
{
	if (!PERF_IS_ALIGNED(addr, 32)) {
		pr_err("[%s] 64 bytes align needed\n", __func__);
		return PERF_ERROR;
	}

	writel((u32) addr, SOC_PERFSTAT_SEQ_ADDR_LOW_ADDR(perfstat_base));
	writel((u32) addr, SOC_PERFSTAT_SEQ_ADDR_HIGH_ADDR(perfstat_base));

	return PERF_OK;
}

static int perfstat_set_seq_mode_len(int len)
{
	if (!PERF_IS_ALIGNED(len, 32)) {
		pr_err("[%s] 32 bytes align needed\n", __func__);
		return PERF_ERROR;
	}

	writel(len, SOC_PERFSTAT_SEQ_ADDR_LEN_ADDR(perfstat_base));

	return PERF_OK;
}

static int perfstat_set_list_mode_addr(u64 addr)
{
	if (!PERF_IS_ALIGNED(addr, 16)) {
		pr_err("[%s] 8 bytes align needed\n", __func__);
		return PERF_ERROR;
	}

	writel((u32) addr, SOC_PERFSTAT_DESCRIPTOR_ADDR_LOW_ADDR(perfstat_base));
	writel((u32) (addr >> 32), SOC_PERFSTAT_DESCRIPTOR_ADDR_HIGH_ADDR(perfstat_base));

	return PERF_OK;
}

static void perfstat_set_timeout_cnt(int cnt)
{
	writel(cnt, SOC_PERFSTAT_OVERTIME_CFG_CNT_ADDR(perfstat_base));
}

static void perfstat_set_timeout_duration(int msec)
{
	int value;
	struct perfstat_dev_info *dev_info = perfstat_info;

	value = msec * (int)(dev_info->clk_perf_stat_rate / 1000);
	perfstat_set_timeout_cnt(value);
}

static void perfstat_set_monitor_reset(void)
{
	writel(0x1, SOC_PERFSTAT_MONITOR_RESET_ADDR(perfstat_base));
}

static void perfstat_trigger_sample_start(void)
{
	writel(0x1, SOC_PERFSTAT_CMD_START_ADDR(perfstat_base));
}

static void perfstat_trigger_sample_stop(void)
{
	writel(0x1, SOC_PERFSTAT_SAMPLE_STOP_ADDR(perfstat_base));
}

static void perfstat_enable_interrupt(int type)
{
	int value;

	value = readl(SOC_PERFSTAT_INT_EN_ADDR(perfstat_base));
	value |= type;
	writel(value, SOC_PERFSTAT_INT_EN_ADDR(perfstat_base));
}

static void perfstat_disable_interrupt(int type)
{
	int value;

	value = readl(SOC_PERFSTAT_INT_EN_ADDR(perfstat_base));
	value &= ~type;
	writel(value, SOC_PERFSTAT_INT_EN_ADDR(perfstat_base));
}

static void perfstat_clear_interrupt(int type)
{
	writel(type, SOC_PERFSTAT_INT_CLR_ADDR(perfstat_base));
}

static int perfstat_get_masked_int_stat(void)
{
	return
	    readl(SOC_PERFSTAT_MASK_INT_STAT_ADDR(perfstat_base));
}

static int perfstat_get_cur_sample_cnt(void)
{
	return
	    readl(SOC_PERFSTAT_ALL_SAMPLE_NUM_ADDR(perfstat_base));
}

static u64 perfstat_get_cur_data_addr(void)
{
	u64 val_l, val_h, val;
	val_l =
	    readl(SOC_PERFSTAT_LAST_SAMPLE_AXI_ADDR_LOW_ADDR
			      (perfstat_base));
	val_h =
	    readl(SOC_PERFSTAT_LAST_SAMPLE_AXI_ADDR_HIGH_ADDR
			      (perfstat_base));
	val = (val_h << 32) | (val_l & 0xffffffff);
	return val;
}

static u64 perfstat_get_cur_list_addr(void)
{
	u64 val_l, val_h, val;
	val_l =
	    readl(SOC_PERFSTAT_LAST_DESC_ADDR_LOW_ADDR
			      (perfstat_base));
	val_h =
	    readl(SOC_PERFSTAT_LAST_DESC_ADDR_HIGH_ADDR
			      (perfstat_base));
	val = (val_h << 32) | (val_l & 0xffffffff);
	return val;
}

static int perfstat_get_fifo_status(void)
{
	return
	    readl(SOC_PERFSTAT_DEBUG_FIFO_FULL_ADDR(perfstat_base));
}

static int perfstat_get_block_port(void)
{
	return
	    readl(SOC_PERFSTAT_DEBUG_MONITOR_ADDR(perfstat_base));
}

static u64 perfstat_get_list_mode_addr(void)
{
	u64 val;

	val =
	    readl(SOC_PERFSTAT_DESCRIPTOR_ADDR_HIGH_ADDR
			      (perfstat_base));
	val |=
	    (val << 32) +
	    readl(SOC_PERFSTAT_LAST_DESC_ADDR_LOW_ADDR
			      (perfstat_base));
	return val;
}

static int perfstat_get_reset_cnt(void)
{
	return readl(SOC_PERFSTAT_STAT_RST_CNT_ADDR(perfstat_base));
}

static int perfstat_get_is_sampling(void)
{
	return readl(SOC_PERFSTAT_CMD_START_ADDR(perfstat_base));
}

static void perfstat_pwr_enable(void)
{
	perfstat_reset_dis();
	clk_prepare_enable(perfstat_info->clk_perf_stat);
	clk_prepare_enable(perfstat_info->aclk_perf_stat);
	clk_prepare_enable(perfstat_info->pclk_perf_stat);
}

static void perfstat_pwr_disable(void)
{
	perfstat_reset_en();
	clk_disable_unprepare(perfstat_info->clk_perf_stat);
	clk_disable_unprepare(perfstat_info->aclk_perf_stat);
	clk_disable_unprepare(perfstat_info->pclk_perf_stat);
}

static void perfstat_debug_info_show(void)
{
	int cur_samp_cnt, dbg_fifo_stat, dbg_block_port;
	u64 cur_data_addr, cur_list_addr;

	cur_samp_cnt = perfstat_get_cur_sample_cnt();
	pr_err("[%s]CurSampCnt   : 0x%x\n", __func__, cur_samp_cnt);
	cur_data_addr = perfstat_get_cur_data_addr();
	pr_err("[%s]CurDataAddr  : 0x%llx\n", __func__, cur_data_addr);
	cur_list_addr = perfstat_get_cur_list_addr();
	pr_err("[%s]CurListAddr  : 0x%llx\n", __func__, cur_list_addr);
	dbg_fifo_stat = perfstat_get_fifo_status();
	pr_err("[%s]DbgFIFOStat  : 0x%x\n", __func__, dbg_fifo_stat);
	dbg_block_port = perfstat_get_block_port();
	pr_err("[%s]DbgBlockPort : 0x%x\n", __func__, dbg_block_port);

	return;
}

static void perfstat_list_done_handler(void)
{
	struct perfstat_dev_info *dev_info = perfstat_info;
	pr_err("[%s]perfstat list done.\n", __func__);
	perfstat_debug_info_show();

	if (PERF_SAMPLE_HIGHSPEED == dev_info->samp_type) {
		if (!perf_file_saved && !atomic_read(&is_recording)) {
			pr_err("%s: save perf data...\n", __func__);
			schedule_delayed_work(&dev_info->hs_record_data_work,
					      0);
		}
	}

	return;
}

static void perfstat_sample_done_handler(void)
{
	struct perfstat_dev_info *dev_info = perfstat_info;

	pr_err("[%s]perf sample done\n", __func__);
	perfstat_debug_info_show();
	if (!dev_info->list) {
		pr_err("[%s]no list exits\n", __func__);
		return;
	}

	if (PERF_SAMPLE_HSCYCLE == dev_info->samp_type &&
	    !perf_file_saved && !atomic_read(&is_recording)) {
		pr_err("%s: save perf data...\n", __func__);
		schedule_delayed_work(&dev_info->hs_record_data_work, 0);
	}
	return;
}

static void perfstat_list_error_handler(void)
{
	struct perfstat_dev_info *dev_info = perfstat_info;
	u64 val;
	perf_list_descriptor *list_cfg;

	pr_err("[%s]perf list error\n", __func__);
	perfstat_debug_info_show();

	val = perfstat_get_list_mode_addr();
	pr_err("[%s]list addr inReg is 0x%llx.\n", __func__, val);
	list_cfg = dev_info->list;
	if (!list_cfg)
		pr_err("[%s]list addr inSof is null.\n", __func__);
	else
		pr_err("[%s]list addr inSof is 0x%llx.\n", __func__,
		       (u64) __virt_to_phys(list_cfg));

	return;
}

static void perfstat_port_overtime_handler(void)
{
	pr_err("[%s]perf port overtime\n", __func__);
	perfstat_debug_info_show();
}

static irqreturn_t perfstat_interrupt_handler(int irq, void *data)
{
	int val;
	struct perfstat_dev_info *dev_info = (struct perfstat_dev_info *)data;

	spin_lock(&dev_info->synclock);
	if (PERF_TASK_DONE == dev_info->status) {
		pr_err("[%s]perfstat is in TASK_DONE mode!\n", __func__);
		perfstat_clear_interrupt(PERF_INT_ALL);
		goto out;
	}

	val = perfstat_get_masked_int_stat();
	if (val & PERF_INT_SAMP_DONE) {
		perfstat_clear_interrupt(PERF_INT_ALL);
		perfstat_sample_done_handler();
	}

	if (val & PERF_INT_LIST_DONE) {
		perfstat_clear_interrupt(PERF_INT_LIST_DONE);
		perfstat_list_done_handler();
	}

	if (val & PERF_INT_LIST_ERR) {
		perfstat_clear_interrupt(PERF_INT_LIST_ERR);
		perfstat_list_error_handler();
	}

	if (val & PERF_INT_OVER_TIME) {
		perfstat_clear_interrupt(PERF_INT_OVER_TIME);
		perfstat_port_overtime_handler();
	}

out:
	spin_unlock(&dev_info->synclock);
	return IRQ_HANDLED;
}

static int perfstat_check_ready(void)
{
	volatile int val;
	volatile int count;

	count = PERF_CHECK_READY_TIMEOUT;
	do {
		val = perfstat_get_reset_cnt();
		count--;
	} while (val & count);

	if (!count)
		return PERF_ERROR;

	perfstat_set_monitor_reset();
	return PERF_OK;
}

static int perfstat_master_is_valid(unsigned long *sprt)
{
	int i;
	struct perfstat_dev_info *dev_info = perfstat_info;

	pr_err("[%s]port is 0x%x\n", __func__, (int)*sprt);

	for (i = 0; i < PERF_PORT_NUM; i++) {
		if (*sprt & PERF_PORT_BIT(i)) {
			if (!(dev_info->vldmsk_of_sprt & PERF_PORT_BIT(i))) {
				pr_err("[%s] (%d)%s this port is not valid!\n",
				       __func__, i, master_name[i]);
				*sprt &= (~PERF_PORT_BIT(i));	/* clear this port */
			}
		}
	}

	/* If no master is powered up, not need to start perfstat. */
	if (0 == *sprt) {
		pr_err("[%s]no master is valid\n", __func__);
		return PERF_ERROR;
	}

	pr_err("[%s]port is 0x%x\n", __func__, (int)*sprt);
	return PERF_OK;
}

static void perfstat_record_sample_data(struct work_struct *work)
{
	struct perfstat_dev_info *dev_info = perfstat_info;
	unsigned long fs;
	long fd = 0;
	int ret = 0;
	u64 cur_list_addr, list_addr, cur_data_addr;
	void *start;
	int size, offset;
	int size_l = 0, size_h = 0;
	int page_idx, i;
	int sample_total_size, addr_mod;

	if (!atomic_add_unless(&is_recording, 1, 1)) {
		pr_err("[%s] is recording, failed to record.\n", __func__);
		return;
	}

	pr_err("[%s]perf sample done\n", __func__);
	perfstat_debug_info_show();
	if (!dev_info->list) {
		pr_err("[%s]no list exits\n", __func__);
		goto record_err;
	}

	start = dev_info->virt_addr;
	size = PAGE_SIZE * dev_info->pages_n;
	/*invalid data buffer to be copied */
	__dma_map_area((const void *)start, size, DMA_FROM_DEVICE);

	sample_total_size =
	    dev_info->sample_per_sz * perfstat_get_cur_sample_cnt() +
	    sizeof(perf_sample_data_head);
	pr_err("[%s]sample_total_size is 0x%x.\n", __func__, sample_total_size);
	pr_err("[%s]sample_per_sz is 0x%x,perfstat_get_cur_sample_cnt() is 0x%x.\n",
	     __func__, dev_info->sample_per_sz, perfstat_get_cur_sample_cnt());
	pr_err("[%s]size is 0x%x, pages_n is 0x%x.\n", __func__, size,
	       dev_info->pages_n);
	if (PERF_SAMPLE_HSCYCLE == dev_info->samp_type
	    && sample_total_size > PAGE_SIZE * dev_info->pages_n) {
		/*find out the current page_idx in pages */
		cur_list_addr = perfstat_get_cur_list_addr();
		cur_data_addr = perfstat_get_cur_data_addr();
		list_addr = __virt_to_phys(dev_info->list);

		pr_err("[%s] cur_list_addr is 0x%llx.\n", __func__,
		       cur_list_addr);
		pr_err("[%s] cur_data_addr is 0x%llx.\n", __func__,
		       cur_data_addr);
		pr_err("[%s] list_addr is 0x%llx.\n", __func__, list_addr);
		for (i = 0; i < dev_info->pages_n; i++) {
			if (list_addr == cur_list_addr)
				break;
			list_addr = list_addr + sizeof(perf_list_descriptor);
		}
		if (unlikely(dev_info->pages_n == i)) {
			pr_err("[%s]no list addr matched,dev_info->pagns_n is 0x%x \n",
			     __func__, dev_info->pages_n);
			goto record_err;
		} else {
			page_idx = i;
		}
		/*count the data start_addr & size */
		addr_mod =
		    (dev_info->pages_n * PAGE_SIZE) % dev_info->sample_per_sz;
		if (0 == addr_mod) {
			addr_mod = dev_info->sample_per_sz;
		}

		pr_info("[%s] addr_mode=%d", __func__, addr_mod);
		offset =
		    cur_data_addr - dev_info->list[page_idx].bit_config.address;
		pr_info("[%s] offset=%d", __func__, offset);
		start =
		    (void *)((char *)(dev_info->virt_addr) +
			     PAGE_SIZE * page_idx + offset + addr_mod);
		size_l =
		    PAGE_SIZE * (dev_info->pages_n - page_idx) - offset -
		    addr_mod;
		size_h = PAGE_SIZE * page_idx + offset + addr_mod;

		pr_err("[%s] end\n", __func__);
	}

	fs = get_fs();
	set_fs(KERNEL_DS);
	fd = sys_open(PERF_HIGHSPEED_DATA_PATH, O_CREAT | O_WRONLY | O_TRUNC,
		      PERF_FILE_MASK);
	if (fd < 0) {
		pr_err("[%s]open file failed, ret is %lu.\n", __func__, fd);
		set_fs(fs);
		goto record_err;
	}

	/*write time info */
	ret = sys_write(fd, (const char *)&sample_time, sizeof(sample_time));
	if (ret < 0) {
		pr_err("[%s]write file failed, ret is %d.\n", __func__, ret);
		set_fs(fs);
		goto writing_err;
	}

	if (PERF_SAMPLE_HSCYCLE == dev_info->samp_type
	    && sample_total_size > PAGE_SIZE * dev_info->pages_n) {
		/*write perfstat raw data */
		ret = sys_write(fd, start, size_l);
		if (ret < 0) {
			pr_err("[%s]write file failed, ret is %d.\n", __func__,
			       ret);
			set_fs(fs);
			goto writing_err;
		}
		/*write perfstat raw data */
		ret = sys_write(fd, dev_info->virt_addr, size_h);
		if (ret < 0) {
			pr_err("[%s]write file failed, ret is %d.\n", __func__,
			       ret);
			set_fs(fs);
			goto writing_err;
		}
	} else {
		/*write perfstat raw data */
		ret = sys_write(fd, start, size);
		if (ret < 0) {
			pr_err("[%s]write file failed, ret is %d.\n", __func__,
			       ret);
			set_fs(fs);
			goto writing_err;
		}
	}

	ret = sys_fsync(fd);
	if (ret < 0)
		pr_err("[%s]sys_fsync failed, ret is %d\n", __func__, ret);
	perf_file_saved = true;
writing_err:
	ret = sys_close(fd);
	if (ret < 0)
		pr_err("[%s]sys_close failed, ret is %d\n", __func__, ret);

	set_fs(fs);
	pr_err("[%s]perf data saved complete!\n", __func__);

record_err:
	atomic_dec(&is_recording);
	complete(&perf_comp);
}

static void perfstat_event_destroy(struct perf_event *event)
{
	struct perfstat_dev_info *dev_info = perfstat_info;
	struct page **pages;
	int i;
	mutex_lock(&perfstat_transaction_mutex);
	if (PERF_DEL_DONE != dev_info->status) {
		pr_err("[%s] perfstat is not running.\n", __func__);
		goto err_end;
	}
	pr_err("[%s] start running.\n", __func__);
	if (perfstat_get_is_sampling()) {
		perfstat_trigger_sample_stop();
	}

	wait_for_completion(&perf_comp);



	pages = dev_info->pages;
	vunmap((const void *)dev_info->virt_addr);
	for (i = 0; i < dev_info->pages_n; i++) {
		__free_pages(pages[i], 0);
	}

	kfree(dev_info->list);
	kfree(dev_info->pages);
	dev_info->list = NULL;
	dev_info->pages = NULL;

	perf_file_saved = false;
	dev_info->event_bk = NULL;
	dev_info->virt_addr = NULL;

	perfstat_pwr_disable();

	spin_lock(&dev_info->synclock);
	dev_info->status = PERF_TASK_DONE;
	spin_unlock(&dev_info->synclock);
err_end:
	mutex_unlock(&perfstat_transaction_mutex);
	return;
}

static int perfstat_check_perm(struct perf_event *event,
			       struct perfstat_dev_info *dev_info)
{
	int sample_period;

	if (event->attr.type != PERF_TYPE_RAW) {
		goto check_err;
	}

	/*get the period from attr.sample_period. */
	sample_period = event->attr.sample_period & SAMPLE_PERIOD_MASK;
	if (sample_period <= 0) {
		pr_err("[%s]sample_period(%d)<=0\n", __func__, sample_period);
		goto check_err;
	}

	return 0;
check_err:
	return -1;
}

static int perfstat_calculate_pages(struct perfstat_dev_info *dev_info, int *page_n,
				    int sample_period, unsigned int allocate_size)
{
	int sz;
	if (NULL == dev_info) {
		pr_err("[%s] dev_info is NULL.", __func__);
		return -1;
	}

	if (0 == allocate_size) {
		pr_err("%s: timing not specified, use default time.\n",
		       __func__);
		*page_n = PERF_MALLOC_PAGES_100M;
	} else {
		if (PERF_SAMPLE_HSCYCLE == dev_info->samp_type) {
			/*when in this mode, sptm means sample space size. */
			*page_n = allocate_size * PERF_SIZE_1M / PAGE_SIZE;
		} else {
			sz = dev_info->sample_per_sz *
			    ((allocate_size * PERF_TIME_1US / sample_period) +
			     1);
			*page_n = Count_xINy_Num(PAGE_SIZE, sz);
		}
	}
	pr_info("[%s] page_n=%d, sample_period=%d, allocate_size=%x\n", __func__, *page_n, sample_period, allocate_size);
	return 0;
}

static int perfstat_initialize_list_cfg(struct perfstat_dev_info *dev_info,
					perf_list_descriptor *list_cfg,
					u64 *list_head_phyaddr)
{
	signed int i = 0;
	u64 phy_addr;
	if (NULL == list_cfg) {
		pr_err("[%s]list_cfg is not intialized\n", __func__);
		return -1;
	}

	if (NULL == dev_info) {
		pr_err("[%s]dev_info is not intialized\n", __func__);
		return -1;
	}
	*list_head_phyaddr = (u64) __virt_to_phys(list_cfg);
	pr_info("[%s] list_head_phyaddr is 0x%llx\n", __func__,
		*list_head_phyaddr);

	for (i = 0; i < dev_info->pages_n; i++) {
		dev_info->pages[i] = alloc_page(GFP_KERNEL);
		if (!dev_info->pages[i]) {
			pr_err("[%s] alloc page error\n", __func__);
			return -1;
		}

		phy_addr =
		    (u64) __virt_to_phys(page_address(dev_info->pages[i]));
		if (PERF_SAMPLE_HIGHSPEED == dev_info->samp_type
		    && i == dev_info->pages_n - 1) {
			list_cfg[i].value.value0 = PERF_LIST_CFG_INT;
		} else {
			list_cfg[i].value.value0 = PERF_LIST_CFG_NORMAL;
		}
		list_cfg[i].bit_config.address = phy_addr;
		list_cfg[i].bit_config.length = PAGE_SIZE;
	}
	/*last descriptor is a link to the head */
	if (PERF_SAMPLE_HSCYCLE == dev_info->samp_type) {
		list_cfg[dev_info->pages_n].value.value0 = PERF_LIST_CFG_LINK;
		list_cfg[dev_info->pages_n].bit_config.address =
		    *list_head_phyaddr;
	}

	return 0;
}

static int perfstat_clk_init(struct platform_device *pdev, unsigned int fpga_flag)
{
	struct perfstat_dev_info *dev_info = perfstat_info;
	int ret;

	dev_info->clk_perf_stat = devm_clk_get(&pdev->dev, "clk_perf_stat");
	if (IS_ERR_OR_NULL(dev_info->clk_perf_stat)) {
		pr_err("[%s] get clk_perf_stat error\n", __func__);
		goto clk_init_fail;
	}
	dev_info->aclk_perf_stat = devm_clk_get(&pdev->dev, "aclk_perf_stat");
	if (IS_ERR_OR_NULL(dev_info->aclk_perf_stat)) {
		pr_err("[%s] get aclk_perf_stat error\n", __func__);
		goto clk_init_fail;
	}
	dev_info->pclk_perf_stat = devm_clk_get(&pdev->dev, "pclk_perf_stat");
	if (IS_ERR_OR_NULL(dev_info->pclk_perf_stat)) {
		pr_err("[%s] get pclk_perf_stat error\n", __func__);
		goto clk_init_fail;
	}

	if (!fpga_flag) {	/*When fpga mode, no need to set clk_perf_stat to 120MHz */
		ret = clk_set_rate(dev_info->clk_perf_stat, suggest_clk);
		if (ret) {
			pr_err("[%s] set clk_perf_stat error\n", __func__);
			goto clk_init_fail;
		}
	}

	dev_info->clk_perf_stat_rate = clk_get_rate(dev_info->clk_perf_stat);
	pr_err("[%s] clk_perf_stat is %lu\n", __func__, dev_info->clk_perf_stat_rate);

	dev_info->aclk_perf_stat_rate = clk_get_rate(dev_info->aclk_perf_stat);
	pr_err("[%s] aclk_perf_stat is %lu\n", __func__, dev_info->aclk_perf_stat_rate);
	return PERF_OK;
clk_init_fail:
	return -ENODEV;
}

static int perfstat_get_edata(struct perf_event *event, int *sample_period,
	    unsigned long *sample_port, unsigned int *allocate_size)
{
	int ret;
	int event_id, event_base;

	if (NULL == event) {
		pr_err("[%s]perf_event is not intialized\n", __func__);
		goto intialize_err;
	}

	if (NULL == sample_port) {
		pr_err("[%s]samplr_port is not intialized\n", __func__);
		goto intialize_err;
	}

	if (NULL == sample_period) {
		pr_err("[%s]sample_period is not intialized\n", __func__);
		goto intialize_err;
	}

	if (NULL == allocate_size) {
		pr_err("[%s]sample_time is not intialized\n", __func__);
		goto intialize_err;
	}

	event_id = (int)((event->attr.config >> 48) & 0xffff);
	event_base = (perfstat_info->event_id >> 16) & 0x0000ffff;
	pr_err("[%s] event_id=%x, event_base=%x", __func__, event_id, event_base);
	if (event_id == event_base + PERF_SAMPLE_HSCYCLE) {
		perfstat_info->samp_type = PERF_SAMPLE_HSCYCLE;
		pr_err("[%s]perfstat use high speed cycle sample type.\n",
		       __func__);
	} else if (event_id == event_base + PERF_SAMPLE_HIGHSPEED) {
		perfstat_info->samp_type = PERF_SAMPLE_HIGHSPEED;
		pr_err("[%s]perfstat use high speed sample type.\n", __func__);
	} else {
		pr_err("[%s] use mode error.\n", __func__);
		goto intialize_err;
	}
	/*count buffer size to store data,then figure out
	 * the pages size needed, and also the page size
	 * reported once a interrupt */
	/*get the period from attr.sample_period. */
	*sample_period = event->attr.sample_period & 0xffffffff;
	/*get the low 32 bit from attr.config as sample port. */
	*sample_port = event->attr.config & PERF_PORT_ALL;
	/*get the third low 16 bit from attr.config as sample time or perf data space size. */
	*allocate_size = (int)((event->attr.config >> 32) & 0x0000ffff);

	ret = perfstat_master_is_valid(sample_port);
	if (PERF_OK != ret) {
		pr_err("[%s]no master is valid!\n", __func__);
		goto intialize_err;
	}

	event->attr.sample_type = PERF_SAMPLE_RAW;
	event->attr.mmap = 0;
	event->attr.comm = 0;
	event->attr.task = 0;
	event->destroy = perfstat_event_destroy;
	event->attr.disabled = 0;
	event->state = PERF_EVENT_STATE_INACTIVE;

	return 0;
intialize_err:
	return -ENOENT;
}


static void perfstat_fill_devinfo(struct perfstat_dev_info *dev_info, struct page **pages,
			int page_n, unsigned long sample_port, perf_list_descriptor *list_cfg)
{
	pr_err("[%s] sample_port is 0x%lx,dev_info->per_data_sz is 0x%d \n", __func__,
	       sample_port, dev_info->per_data_sz);
	pr_err("[%s] perf_sample_data_head size is 0x%d,perf_sample_data_end sz is 0x%d \n",
	     __func__, (int) sizeof(perf_sample_data_head),
	     (int) sizeof(perf_sample_data_end));

	dev_info->pages = pages;
	dev_info->pages_n = page_n;
	dev_info->sprt = (int)sample_port;

	dev_info->list = list_cfg;
}

static int perfstat_event_init(struct perf_event *event)
{
	struct perfstat_dev_info *dev_info = perfstat_info;
	int sample_period;
	unsigned long sample_port;
	unsigned int allocate_size;
	int page_n;
	signed int i = 0;
	int ret;
	struct page **pages = NULL;
	int sample_port_num;
	perf_list_descriptor *list_cfg = NULL;
	u64 list_head_phyaddr;
	mutex_lock(&perfstat_transaction_mutex);

	if (NULL == event) {
		pr_err("[%s] event is null!\n", __func__);
		goto err_end;
	}

	if (dev_info->status != PERF_TASK_DONE) {
		pr_err("[%s] Perf sampling, please wait for it finished!\n",
		       __func__);
		goto err_end;
	}

	if (!fpga_flag) {	/*When fpga mode, no need to set aclk_perf_stat to 228MHz */
		ret = clk_set_rate(dev_info->aclk_perf_stat, suggest_aclk);
		if (ret)  {
			pr_err("[%s] set aclk_perf_stat error\n", __func__);
			goto err_end;
		}
	}

	perfstat_pwr_enable();

	ret = perfstat_check_ready();
	if (ret < 0) {
		pr_err("[%s]perfstat is not ready\n", __func__);
		goto perf_supply_err;
	}


	ret = perfstat_check_perm(event, dev_info);
	if (ret < 0) {
		pr_err("[%s] initialize data fail.\n", __func__);
		goto perf_supply_err;
	}

	ret = perfstat_get_edata(event, &sample_period,  &sample_port, &allocate_size);
	if (ret < 0) {
		pr_err("[%s] initialize data fail.\n", __func__);
		goto perf_supply_err;
	}
	pr_info("[%s] perfstat allocate_size:%d\n", __func__, allocate_size);
	pr_info("[%s] perfstat sample_period:%d\n", __func__, sample_period);
	pr_info("[%s] perfstat sample_port:%ld\n", __func__, sample_port);
	pr_err("==============[hisi perf prepare +]================\n");


	sample_port_num = bitmap_weight((const unsigned long *)&sample_port, 4 * sizeof(sample_port));
	dev_info->sample_per_sz =
	    (sample_port_num * dev_info->per_data_sz) + sizeof(perf_sample_data_end);
	pr_err("[%s] sample_port_num is 0x%x,dev_info->sample_per_sz is 0x%x \n", __func__,
	       sample_port_num, dev_info->sample_per_sz);

	ret = perfstat_calculate_pages(dev_info, &page_n, sample_period, allocate_size); /* false alarm]:fortify */
	if (ret < 0) {
		pr_err("[%s] calculate pages fail.\n", __func__);
		goto err;
	}
	pr_info("[%s] page_n : %d\n", __func__, page_n);
	pages = kzalloc((page_n * sizeof(struct page *)), GFP_KERNEL | GFP_DMA);
	if (!pages) {
		pr_err("[%s] cannot allocate pages ptr buffer\n", __func__);
		goto err;
	}

	list_cfg = kzalloc(((page_n + 1) * sizeof(perf_list_descriptor)),
			   GFP_KERNEL | GFP_DMA);
	pr_info("[%s]  kzalloc list_cfg is 0x%llx\n", __func__, (u64) list_cfg);
	if (!list_cfg) {
		pr_err("[%s] cannot allocate list_cfg buffer\n", __func__);
		goto err;
	}

	perfstat_fill_devinfo(dev_info, pages, page_n, sample_port, list_cfg);

	pr_err("[%s] port   : 0x%x\n", __func__, (int)sample_port);
	pr_err("[%s] period : %dus\n", __func__, sample_period);
	pr_err("[%s] page_n  : %d\n", __func__, page_n);

	ret = perfstat_initialize_list_cfg(dev_info, list_cfg, &list_head_phyaddr);
	if (ret < 0) {
		pr_err("[%s] init list config error.\n", __func__);
		goto err;
	}
	pr_info("[%s] initialize list_cfg done.\n", __func__);
	/*flush list_cfg into ddr */
	__dma_map_area((const void *)list_cfg,
		       ((page_n + 1) * sizeof(perf_list_descriptor)),
		       DMA_TO_DEVICE);

	/*map pages to a continuous virtual address */
	dev_info->virt_addr = vmap(pages, page_n, VM_MAP, PAGE_KERNEL);

	if (!dev_info->virt_addr) {
		pr_err("[%s] vmap pages error\n", __func__);
		goto err;
	}

	memset(dev_info->virt_addr, 0, page_n * PAGE_SIZE);
	__dma_map_area((const void *)dev_info->virt_addr, (page_n * PAGE_SIZE),
		       DMA_FROM_DEVICE);

	pr_info("[%s] vmap is %p\n", __func__, dev_info->virt_addr);

	perfstat_set_sample_period(sample_period);
	perfstat_set_sample_port((int)sample_port);
	perfstat_set_sample_num(PERF_SAMPLE_ALWAYS);
	perfstat_set_addr_mode(PERF_LIST_ADDR_MODE);
	ret = perfstat_set_list_mode_addr(list_head_phyaddr);
	if (PERF_OK != ret) {
		pr_err("[%s] set list addr error\n", __func__);
		goto err;
	}

	perfstat_clear_interrupt(PERF_INT_ALL);
	perfstat_enable_interrupt(PERF_INT_ALL);
	perfstat_set_timeout_duration(10);
	dev_info->status = PERF_INIT_DONE;
	mutex_unlock(&perfstat_transaction_mutex);
	pr_err("==============[hisi perf prepare -]================\n");

	dev_info->event_bk = event;

	return 0;

err:
	if (dev_info->virt_addr) {
		vunmap(dev_info->virt_addr);
		dev_info->virt_addr = NULL;
	}
	if (dev_info->list) {
		kfree(dev_info->list);
		dev_info->list = NULL;
	}
	if (dev_info->pages) {
		for (i = i - 1; i >= 0; i--)
			__free_pages(dev_info->pages[i], 0);
		kfree(dev_info->pages);
		dev_info->pages = NULL;
	}
	dev_info->status = PERF_TASK_DONE;
perf_supply_err:
	perfstat_pwr_disable();
err_end:
	mutex_unlock(&perfstat_transaction_mutex);
	return -ENOENT;
}

static int perfstat_add(struct perf_event *bp, int flags)
{
	struct perfstat_dev_info *dev_info = perfstat_info;
	mutex_lock(&perfstat_transaction_mutex);
	if (PERF_INIT_DONE != dev_info->status) {
		pr_err("[%s] perf is sampling cannot add event\n", __func__);
		goto err_end;
	}

	wake_lock(&dev_info->wakelock);

	sample_time = hisi_getcurtime();
	pr_err("[%s] sample time is %lu\n", __func__, sample_time);
	pr_info("[%s][hisi perf start sample...]\n", __func__);
	perfstat_trigger_sample_start();
	dev_info->status = PERF_ADD_DONE;
	mutex_unlock(&perfstat_transaction_mutex);
	return 0;
err_end:

	mutex_unlock(&perfstat_transaction_mutex);
	return -ENOENT;
}

static void perfstat_del(struct perf_event *bp, int flags)
{
	struct perfstat_dev_info *dev_info = perfstat_info;
	mutex_lock(&perfstat_transaction_mutex);
	if (PERF_ADD_DONE != dev_info->status) {
		pr_err("[%s] perf is sampling cannot del event", __func__);
		goto err_end;
	}

	if ((PERF_SAMPLE_HIGHSPEED == dev_info->samp_type) && (!perf_file_saved)
	    && !atomic_read(&is_recording)) {
		pr_err("%s: save perf data...\n", __func__);
		schedule_delayed_work(&dev_info->hs_record_data_work, 0);
	}

	pr_info("[%s][hisi perf stop sample]\n", __func__);
	perfstat_trigger_sample_stop();

	pr_err("==============[hisi perf finish +]================\n");
	perfstat_debug_info_show();
	pr_err("==============[hisi perf finish -]================\n");

	wake_unlock(&dev_info->wakelock);
	dev_info->status = PERF_DEL_DONE;
err_end:
	mutex_unlock(&perfstat_transaction_mutex);
	return;
}

static void perfstat_start(struct perf_event *bp, int flags)
{
	bp->hw.state = 0;
}

static void perfstat_stop(struct perf_event *bp, int flags)
{
	bp->hw.state = PERF_HES_STOPPED;
}

static void perfstat_read(struct perf_event *bp)
{
	return;
}

static int perfstat_event_idx(struct perf_event *bp)
{
	return 0;
}

static struct pmu perf_stat_pmu = {
	.event_init = perfstat_event_init,
	.add = perfstat_add,
	.del = perfstat_del,
	.start = perfstat_start,
	.stop = perfstat_stop,
	.read = perfstat_read,
	.event_idx = perfstat_event_idx,
};

static int fake_of_get_perfstat_attribute(const struct device_node *np,
					  struct perfstat_dev_info *dev_info)
{
	int ret;

	ret = of_property_read_u32(np, "hisilicon,perf-event-id",
				   &dev_info->event_id);
	if (ret < 0) {
		pr_err("[%s] cannot get perf-event-id\n", __func__);
		return -ENOENT;
	}

	ret = of_property_read_u32(np, "hisilicon,per-data-size",
				   &dev_info->per_data_sz);
	if (ret < 0) {
		pr_err("[%s] cannot get per-data-size\n", __func__);
		return -ENOENT;
	}

	ret = of_property_read_u32(np, "hisilicon,vldmsk-of-sprt",
				   &dev_info->vldmsk_of_sprt);
	if (ret < 0) {
		pr_err("[%s] cannot get vldmsk-of-sprt\n", __func__);
		return -ENOENT;
	}

	ret = of_property_read_u32(np, "suggest_clk", &suggest_clk);
	if (ret < 0) {
		pr_err("[%s] cannot get suggest_clk\n", __func__);
	}

	ret = of_property_read_u32(np, "suggest_aclk", &suggest_aclk);
	if (ret < 0) {
		pr_err("[%s] cannot get suggest_aclk\n", __func__);
	}
	return 0;
}

static int perfstat_get_fpga_flag(unsigned int *pFpga)
{
	struct device_node *np = NULL;
	unsigned int fpga_flag = 0;
	int ret = 0;

	np = of_find_compatible_node(NULL, NULL, "hisilicon,hisifb");
	if (!np) {
		pr_err("NOT FOUND device node 'hisilicon,hisifb'!\n");
		return -ENXIO;
	}
	ret = of_property_read_u32(np, "fpga_flag", &fpga_flag);
	if (ret) {
		pr_err("failed to get fpga_flag resource.\n");
		return -ENXIO;
	}
	*pFpga = fpga_flag;
	return 0;
}

/*******************************************************************************
 * Function:		perfstat_probe
 * Description:		initialize perfstat driver, it's called when this platform_device
 * 			and this platform_driver match.
 * Input:		platform_device: an abstraction of perfstat device
 * Output:		NA
 * Return:		0:	perfstat_probe successfully
 * 			<0:	perfstat_probe failed
 * Other:		NA
 * ********************************************************************************/
static int perfstat_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	int ret;
	struct perfstat_dev_info *dev_info;
	const struct of_device_id *match;

	pr_err("================[hisi perf probe s]================\n");

	/* Get fpga flag first */
	perfstat_get_fpga_flag(&fpga_flag);

	/* to check which type of regulator this is */
	match = of_match_device(of_perfstat_match_tbl, dev);
	if (NULL == match) {
		pr_err("[%s]: mismatch of perf_stat driver \n\r", __func__);
		return -ENODEV;
	}

	master_name = (char **)match->data;

	dev_info =
	    devm_kzalloc(dev, sizeof(struct perfstat_dev_info), GFP_KERNEL);
	if (!dev_info) {
		pr_err("[%s] cannot alloc perfstat desc\n", __func__);
		return -ENOMEM;
	}
	perfstat_info = dev_info;

	dev_info->res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!dev_info->res) {
		pr_err("[%s] platform_get_resource err\n", __func__);
		return -ENOENT;
	}

	if (!devm_request_mem_region(dev, dev_info->res->start,
				     resource_size(dev_info->res),
				     pdev->name)) {
		pr_err("[%s] cannot claim register memory\n", __func__);
		return -ENOMEM;
	}

	pericrg_base = ioremap(SOC_ACPU_PERI_CRG_BASE_ADDR, PERICRG_SIZE);

	perfstat_base =
	    ioremap(dev_info->res->start, resource_size(dev_info->res));
	if (!perfstat_base) {
		pr_err("[%s] base address ioremap fail\n", __func__);
		return -ENOMEM;
	}

	dev_info->irq = platform_get_irq(pdev, 0);
	if (dev_info->irq < 0) {
		pr_err("[%s] platform_get_irq err\n", __func__);
		return -ENXIO;
	}

	ret =
	    request_threaded_irq(dev_info->irq, perfstat_interrupt_handler,
				 NULL, IRQF_NO_SUSPEND, "pertstat-irq",
				 dev_info);
	if (ret < 0) {
		pr_err("[%s] requset irq error\n", __func__);
		return -ENODEV;
	}

	suggest_clk = PERF_SUGGEST_CLK;
	suggest_aclk = PERF_SUGGEST_ACLK;
	ret = fake_of_get_perfstat_attribute(np, dev_info);
	if (ret < 0) {
		pr_err("[%s] get hisilicon dts fail\n", __func__);
		return -ENOENT;
	}
	ret = perfstat_clk_init(pdev, fpga_flag);
	if (ret < 0) {
		pr_err("[%s] perfstat clk init failed.\n", __func__);
		return -ENODEV;
	}

	ret = perf_pmu_register(&perf_stat_pmu, "HiPERFSTAT", PERF_TYPE_RAW);
	if (ret < 0) {
		pr_err("[%s] register perf pmu fail\n", __func__);
		return -ENODEV;
	}

	INIT_DELAYED_WORK(&dev_info->hs_record_data_work,
			  perfstat_record_sample_data);
	init_completion(&perf_comp);
	mutex_init(&perfstat_transaction_mutex);
	spin_lock_init(&dev_info->synclock);
	wake_lock_init(&dev_info->wakelock, WAKE_LOCK_SUSPEND,
		       "perf-stat-wakelock");
	dev_info->status = PERF_TASK_DONE;
	is_probe_comp = true;
	pr_err("================[hisi perf probe e]================\n");

	return 0;
}

static int perfstat_remove(struct platform_device *pdev)
{
	return 0;
}

/*******************************************************************************
 * Function:		create_perf_event
 * Description:		a function to create perf_event
 * Input:		int sample_period: sample period
 * 			unsigned long sample_port: sample port
 * 			int sample_time: sample time(mode 1 means monitoring
 * 				sample_time seconds, mode 2 means monitoring sample_time Mb)
 * 			int mode: monitor mode, 1 is high speed mode, 2 is cycle mode
 * Output:		NA
 * Return:		perf_event: NULL means creating perf_event failed
 * Other:		NA
 * ********************************************************************************/
static struct perf_event *create_perf_event(int sample_period,
					    unsigned long sample_port,
					    int sample_time, int mode)
{
	struct perf_event *event;
	if (sample_period <= 0) {
		pr_err("[%s] sample_period is negative: %d\n", __func__,
		       sample_period);
		goto err;
	}
	if (sample_port >= MAX_PORT_NUM) {
		pr_err
		    ("[%s] sample_port is out of range: %lu, max_port_num is: %ld\n",
		     __func__, sample_port, MAX_PORT_NUM);
		goto err;
	}
	if (sample_time < 0 || sample_time >= 1 << 16) {
		pr_err("[%s] sample_time is out of range: %d\n", __func__,
		       sample_time);
		goto err;
	}
	if (mode != PERF_SAMPLE_HSCYCLE && mode != PERF_SAMPLE_HIGHSPEED) {
		pr_err("[%s] use mode error. mode=%d\n", __func__, mode);
		goto err;
	}
	event = kmalloc(sizeof(struct perf_event), GFP_KERNEL);
	if (NULL == event)
		goto err;

	event->attr.type = PERF_TYPE_RAW;
	event->attr.sample_period = sample_period;
	event->attr.config =
	    (u64) sample_port + ((u64) sample_time << 32) +
	    ((u64) (0x8000 + mode) << 48);
	return event;

err:
	return NULL;
}

/*******************************************************************************
 * Function:		perfstat_start_sampling
 * Description:		an interface for other module use, which is used to start
 * Input:		int sample_period: sample period
 *			unsigned long sample_port: sample port
 *			int sample_time: sample time(mode 1 means monitoring
 *				sample_time seconds, mode 2 means monitoring sample_time Mb)
 *			int mode: monitor mode, 1 is high speed mode, 2 is cycle mode
 *			this driver. Moreover, it's a couple with perfstat_stop_sampling
 * Output:		NA
 * Return:		PERF_ERROR:	 start failed
 *			PERF_OK:	 start successfully
 * Other:		NA
 * ********************************************************************************/
int perfstat_start_sampling(int sample_period, unsigned long sample_port,
			    int sample_time, int mode)
{
	struct perf_event *event;
	int ret;
	struct perfstat_dev_info *dev_info = perfstat_info;
	pr_err("[%s] perfstat_start_sampling begin.\n", __func__);
	ret = PERF_ERROR;
	if (false == is_probe_comp || NULL == dev_info) {
		pr_err("[%s] perfstat is not intialized", __func__);
		goto out_err;
	}

	if (sample_period <= 0) {
		pr_err("[%s] sample_period is negative: %d\n", __func__,
		       sample_period);
		goto out_err;
	}
	if (sample_port >= MAX_PORT_NUM) {
		pr_err
		    ("[%s] sample_port is out of range: %lu, max_port_num is:%ld\n",
		     __func__, sample_port, MAX_PORT_NUM);
		goto out_err;
	}
	if (sample_time < 0 || sample_time >= 1 << 16) {
		pr_err("[%s] sample_time is out of range: %d\n", __func__,
		       sample_time);
		goto out_err;
	}
	if (mode != PERF_SAMPLE_HSCYCLE && mode != PERF_SAMPLE_HIGHSPEED) {
		pr_err("[%s] use mode error. mode=%d\n", __func__, mode);
		goto out_err;
	}

	event =
	    create_perf_event(sample_period, sample_port, sample_time, mode);
	if (NULL == event) {
		pr_err("[%s] creating perf_event fails.\n", __func__);
		goto out_err;
	}

	ret = perfstat_event_init(event);

	if (ret < 0) {
		pr_err("[%s] perfstat init fails.\n", __func__);
		ret = -ENOENT;
		goto event_err;
	}

	ret = perfstat_add(event, 0);

	kfree(event);
	event = NULL;
	pr_err("[%s] perfstat_start_sampling end.\n", __func__);
	return ret;

event_err:
	if (NULL != event) {
		kfree(event);
		event = NULL;
	}
out_err:
	pr_err("[%s] perfstat_start_sampling fails.\n", __func__);
	return ret;
}

EXPORT_SYMBOL_GPL(perfstat_start_sampling);

/*******************************************************************************
 * Function:		perfstat_stop_sampling
 * Description:		an interface for other module use, which is used to stop
 *			this driver. Moreover, it's a couple with perfstat_start_sampling
 *			and is allowed to use only after perfstat_start_sampling being called
 * Input:		NA
 * Output:		NA
 * Return:		PERF_ERROR: stop failed
 *			PERF_OK: stop successfully
 * Other:		NA
 * ********************************************************************************/
int perfstat_stop_sampling(void)
{
	struct perfstat_dev_info *dev_info = perfstat_info;
	pr_err("[%s] perfstat_stop_sampling begin.\n", __func__);
	if (false == is_probe_comp || NULL == dev_info) {
		pr_err("[%s] perfstat is not intialized.", __func__);
		return PERF_ERROR;
	}
	if (PERF_TASK_DONE == dev_info->status) {
		pr_err("[%s] perfstat is not running.\n", __func__);
		return PERF_ERROR;
	}
	perfstat_del(NULL, 0);
	perfstat_event_destroy(NULL);
	pr_err("[%s] perfstat_stop_sampling end.\n", __func__);
	return PERF_OK;
}

EXPORT_SYMBOL_GPL(perfstat_stop_sampling);

static struct platform_driver perfstat_driver = {
	.driver = {
		   .name = "HiPERFSTAT",
		   .owner = THIS_MODULE,
		   .of_match_table = of_perfstat_match_tbl,
		   },
	.probe = perfstat_probe,
	.remove = perfstat_remove,
};

static __init int perfstat_init(void)
{
	int ret = PERF_OK;

	ret = platform_driver_register(&perfstat_driver);
	if (ret) {
		pr_err("[%s] platform_driver_register failed %d\n",
		       __func__, ret);
	}

	return ret;
}

static void __exit perfstat_exit(void)
{
	platform_driver_unregister(&perfstat_driver);
}

module_init(perfstat_init);
module_exit(perfstat_exit);

/*sc:sample cnt, sn:sample num, sp:sample port, al:actual len*/
void test_seq_addr_mode(u32 sc, u32 sn, u32 sp, u32 al)
{
	static void *sample_data_addr;
	static u64 phy_addr;
	static u32 flag;
	s32 ret;
	struct perfstat_dev_info *dev_info;

	printk("sc: 0x%x, sn: 0x%x, sp: 0x%x, lm: 0x%x\n", sc, sn, sp, al);
	dev_info = perfstat_info;

	perfstat_reset_dis();
	dev_info->status = PERF_INIT_DONE;

	if (!flag) {
		sample_data_addr = kmalloc(SAMPLE_DATA_SIZE_SEQ, GFP_KERNEL);
		if (!sample_data_addr) {
			printk(KERN_ERR "[%s]malloc sample data area fail.\n",
			       __func__);
			return;
		}
		phy_addr = (u64) __virt_to_phys(sample_data_addr);
		flag = 1;
	}
	printk(KERN_ERR "[%s]vir addr is %p.\n", __func__,
	       sample_data_addr);
	printk(KERN_ERR "[%s]phy addr is 0x%llx.\n", __func__, phy_addr);

	perfstat_set_sample_period(sc);
	perfstat_set_sample_num(sn);
	perfstat_set_sample_port(sp);
	perfstat_set_addr_mode(PERF_SEQ_ADDR_MODE);

	ret = perfstat_set_seq_mode_addr(phy_addr);
	if (PERF_ERROR == ret) {
		return;
	}

	ret = perfstat_set_seq_mode_len(al);
	if (PERF_ERROR == ret) {
		return;
	}

	perfstat_clear_interrupt(PERF_INT_ALL);
	perfstat_disable_interrupt(PERF_INT_ALL);
	perfstat_enable_interrupt(PERF_INT_SAMP_DONE | PERF_INT_OVER_TIME);
	perfstat_set_timeout_duration(200);

	perfstat_trigger_sample_start();

	return;
}

void test_list_addr_mode(u32 sc, u32 sn, u32 sp, u32 lm)
{
	perf_list_descriptor *list_desc;
	u64 phy_list_head_addr;
	perf_list_descriptor temp;
	u32 index;
	void *list_data_addr;
	u64 phy_addr;
	s32 ret;
	struct perfstat_dev_info *dev_info;

	printk("sc: 0x%x, sn: 0x%x, sp: 0x%x, lm: 0x%x\n", sc, sn, sp, lm);
	dev_info = perfstat_info;

	perfstat_reset_dis();

	temp.value.value0 = LIST_CFG_NORMAL;
	temp.bit_config.length = LIST_DATA_SIZE;

	list_desc = (perf_list_descriptor *)
	    kmalloc((LIST_NUM * sizeof(perf_list_descriptor)),
		    GFP_DMA | GFP_KERNEL);
	phy_list_head_addr = (u64) __virt_to_phys(list_desc);
	printk(KERN_ERR "[%s]phy_list_head_addr is 0x%llx.\n", __func__,
	       phy_list_head_addr);
	ret = perfstat_set_list_mode_addr(phy_list_head_addr);
	if (PERF_ERROR == ret) {
		return;
	}

	if (lm == 1) {		/*limited list mode */
		for (index = 0; index < LIST_NUM; index++) {
			list_data_addr = kzalloc(LIST_DATA_SIZE, GFP_KERNEL);
			if (!list_data_addr) {
				printk(KERN_ERR "[%s]malloc list[%d] fail.\n",
				       __func__, index);
				return;
			}
			phy_addr = (u32) __virt_to_phys(list_data_addr);
			printk(KERN_ERR "[%s]phyaddr[%d] is 0x%llx\n",
			       __func__, index, phy_addr);

			temp.bit_config.address = phy_addr;
			list_desc[index] = temp;
		}
	} else if (lm == 2) {	/*cycled list mode */
		for (index = 0; index < (LIST_NUM - 1); index++) {
			list_data_addr = kmalloc(LIST_DATA_SIZE, GFP_KERNEL);
			if (!list_data_addr) {
				printk(KERN_ERR "[%s]malloc list[%d] fail.\n",
				       __func__, index);
				return;
			}
			phy_addr = (u64) __virt_to_phys(list_data_addr);

			temp.bit_config.address = phy_addr;
			list_desc[index] = temp;
		}

		/*last descriptor is a linker to the head descriptor */
		temp.value.value0 = LIST_CFG_LINK;
		temp.bit_config.address = phy_list_head_addr;
		list_desc[LIST_NUM - 1] = temp;
	} else if (lm == 3) {	/*LIST_CFG_END_ERROR */
		dev_info->status = PERF_INIT_DONE;
		temp.value.value0 = LIST_CFG_END_ERROR;
		for (index = 0; index < LIST_NUM; index++) {
			list_data_addr = kzalloc(LIST_DATA_SIZE, GFP_KERNEL);
			if (!list_data_addr) {
				printk(KERN_ERR "[%s]malloc list[%d] fail.\n",
				       __func__, index);
				return;
			}
			phy_addr = (u64) __virt_to_phys(list_data_addr);
			printk(KERN_ERR "[%s]phyaddr[%d] is 0x%llx\n",
			       __func__, index, phy_addr);

			temp.bit_config.address = phy_addr;
			list_desc[index] = temp;
		}
	} else if (lm == 4) {	/*LIST_CFG_VALID_ERROR */
		dev_info->status = PERF_INIT_DONE;
		temp.value.value0 = LIST_CFG_VALID_ERROR;
		for (index = 0; index < LIST_NUM; index++) {
			list_data_addr = kzalloc(LIST_DATA_SIZE, GFP_KERNEL);
			if (!list_data_addr) {
				printk(KERN_ERR "[%s]malloc list[%d] fail.\n",
				       __func__, index);
				return;
			}
			phy_addr = (u64) __virt_to_phys(list_data_addr);
			printk(KERN_ERR "[%s]phyaddr[%d] is 0x%llx\n",
			       __func__, index, phy_addr);

			temp.bit_config.address = phy_addr;
			list_desc[index] = temp;
		}
	} else if (lm == 5) {	/*cycled list mode */
		dev_info->status = PERF_INIT_DONE;
		for (index = 0; index < (LIST_NUM - 1); index++) {
			list_data_addr = kmalloc(LIST_DATA_SIZE, GFP_KERNEL);
			if (!list_data_addr) {
				printk(KERN_ERR "[%s]malloc list[%d] fail.\n",
				       __func__, index);
				return;
			}
			phy_addr = (u64) __virt_to_phys(list_data_addr);

			temp.bit_config.address = phy_addr;
			list_desc[index] = temp;
		}

		/*last descriptor is a linker to the head descriptor */
		temp.value.value0 = PERF_LIST_CFG_INT;
		temp.bit_config.address = phy_list_head_addr;
		list_desc[LIST_NUM - 1] = temp;
	}

	perfstat_set_sample_period(sc);
	perfstat_set_sample_num(sn);
	perfstat_set_sample_port(sp);
	perfstat_set_addr_mode(PERF_LIST_ADDR_MODE);

	perfstat_clear_interrupt(PERF_INT_ALL);
	perfstat_enable_interrupt(PERF_INT_ALL);
	perfstat_set_timeout_duration(200);

	flush_cache_all();

	perfstat_trigger_sample_start();

	return;
}

void test_perfstat_stop(void)
{
	perfstat_trigger_sample_stop();
}

void test_perfstat_powerdonw(void)
{
	perfstat_reset_en();
}
