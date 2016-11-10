

#include <linux/moduleparam.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/completion.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/pagemap.h>
#include <linux/err.h>
#include <linux/scatterlist.h>
#include <linux/version.h>

#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/hdreg.h>
#include <linux/kdev_t.h>
#include <linux/blkdev.h>
#include <linux/mutex.h>

#include <linux/mmc/ioctl.h>
#include <linux/mmc/card.h>
#include <linux/mmc/host.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/sd.h>

#include <linux/uaccess.h>

#ifdef CONFIG_HW_FEATURE_STORAGE_DIAGNOSE_LOG
#include <linux/store_log.h>
#endif
#ifdef CONFIG_HUAWEI_EMMC_DSM
#include <linux/mmc/dsm_emmc.h>
#endif

/*  system write protect flag, 0: disable(default) 1:enable */
static int *ro_secure_debuggable;
/*  system partition number is platform dependent,
 *  MUST change it according to platform */
#define PART_SYSTEM "system"
#define PART_SYSTEM_LEN 6

int blk_set_ro_secure_debuggable(int state)
{
	*ro_secure_debuggable = state;
	return 0;
}
EXPORT_SYMBOL(blk_set_ro_secure_debuggable);

static char *get_bio_part_name(struct bio *bio)
{
	if (unlikely(!bio || !bio->bi_bdev ||
				!bio->bi_bdev->bd_part ||
				!bio->bi_bdev->bd_part->info ||
				!bio->bi_bdev->bd_part->info->volname[0]))
		return NULL;
	return bio->bi_bdev->bd_part->info->volname;
}

static inline char *fastboot_lock_str(void)
{
	if (strstr(saved_command_line, "fblock=locked") ||
			strstr(saved_command_line, "userlock=locked"))
		return "locked";
	else
		return "unlock";
}

int should_trap_this_bio(int rw, struct bio *bio, unsigned int count)
{
	char *name;

	if (!(rw & WRITE))
		return 0;

	name = get_bio_part_name(bio);

	/*
	 * runmode=factory:send write request to mmc driver.
	 * bootmode=recovery:send write request to mmc driver.
	 * partition is mounted ro: file system will block write request.
	 * root user: send write request to mmc driver.
	 */
	if (!name)
		return 0;

	if (likely(strncmp(name, PART_SYSTEM, PART_SYSTEM_LEN)))
		return 0;

	if (name[PART_SYSTEM_LEN] != '\0')
		return 0;

	if ((NULL == ro_secure_debuggable) || (0 == *ro_secure_debuggable))
		return 0;

#ifdef CONFIG_HUAWEI_EMMC_DSM
	DSM_EMMC_LOG(NULL, DSM_SYSTEM_W_ERR,
#else
			printk(KERN_DEBUG
#endif
				"[HW]:EXT4_ERR_CAPS:%s(%d)[Parent: %s(%d)]: %s block %llu on %s (%u sectors) %d %s.\n",
				current->comm, task_pid_nr(current),
				current->parent->comm,
				task_pid_nr(current->parent),
				(rw & WRITE) ? "WRITE" : "READ",
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
				(u64)bio->bi_iter.bi_sector,
#else
				(u64)bio->bi_sector,
#endif
				name,
				count,
				*ro_secure_debuggable,
				fastboot_lock_str());

			bio_endio(bio, -EIO);
			return 1;
}
EXPORT_SYMBOL(should_trap_this_bio);

int __init ro_secure_debuggable_init(void)
{
	static int ro_secure_debuggable_static;

	ro_secure_debuggable = kzalloc(sizeof(int), GFP_KERNEL);
	if (!ro_secure_debuggable)
		ro_secure_debuggable = &ro_secure_debuggable_static;

	return 0;
}
late_initcall(ro_secure_debuggable_init);
