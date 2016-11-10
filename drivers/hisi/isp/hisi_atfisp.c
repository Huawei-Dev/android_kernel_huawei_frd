/*
 * hisilicon efuse driver, hisi_atfisp.c
 *
 * Copyright (c) 2013 Hisilicon Technologies CO., Ltd.
 *
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/regulator/consumer.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/device.h>
#include <linux/clk.h>
#include <linux/of.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/dma-mapping.h>
#include <linux/fcntl.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/scatterlist.h>
#include <linux/printk.h>
#include <linux/file.h>
#include <linux/kthread.h>
#include <linux/rproc_share.h>
#include <linux/remoteproc.h>
#include <linux/hisi/hisi_ion.h>
#include <linux/hisi/hisi-iommu.h>
#include <linux/iommu.h>
#include <asm/uaccess.h>
#include <linux/hisi/hisi_atfdriver.h>
#include <global_ddr_map.h>
#include "teek_client_id.h"
#include <linux/hisi/hisi_load_image.h>
#include <linux/hisi/kirin_partition.h>
#include "hisi_partition.h"
#include "partition.h"

#define MAX_SIZE    64
#define ISP_RESERVED_BASE (HISI_RESERVED_ISP_BOOT_PHYMEM_BASE)

#define A7_REQUEST_ADDR         0xD0000000
#define A7_REQUEST_BUF_SIZE     0x96000
#define A7_REQUEST_BUF_NUM      16
#define A7_GET_OTP_ADDR         0xD0A00000
#define A7_CAPABILITY_ADDR      0xD0B00000
#define A7_ACQUIRE_ADDR         0xD0C00000
#define A7_CALIBRATION_ADDR     0xD0D00000
#define A7_TONEMAP_ADDR         0xD0E00000
#define A7_MAP_BUFFER_ADDR      0xD1000000
#define A7_MAP_BUFFER_SIZE      0xF000000

#define ISP_FN_SET_PARAMS   (0xC500AB00)
#define ISP_FN_JPEG_INIT    (0xC500AB13)
#define ISP_FN_JPEG_EXIT    (0xC500AB21)
#define ISP_FN_A7_INIT      (0xC500AB22)
#define ISP_FN_A7_EXIT      (0xC500AB23)
#define ISP_FN_A7_MAP       (0xC500AB24)
#define ISP_FN_A7_UNMAP     (0xC500AB25)

enum mapType
{
    MAP_TYPE_REQUEST = 0,
    MAP_TYPE_GET_OTP,
    MAP_TYPE_CAPABILITY,
    MAP_TYPE_ACQUIRE_CAMERA,
    MAP_TYPE_CALIBRATION,
    MAP_TYPE_MAP_BUFFER,
    MAP_TYPE_TONEMAP,
    MAP_TYPE_MAX,
};

struct hisi_isp_ops {
#define UNINITIAL   0
	unsigned int refs_isp;
    unsigned int refs_jpeg;
    unsigned int refs_a7;
    unsigned int refs_dts;
    unsigned int refs_rsc;
    unsigned int refs_fw;
	unsigned int refs_setparams;
	int (*ispup)(struct hisi_isp_ops *);
	int (*ispdn)(struct hisi_isp_ops *);
    int (*jpegup)(struct hisi_isp_ops *);
    int (*jpegdn)(struct hisi_isp_ops *);
    int (*a7up)(struct hisi_isp_ops *);
    int (*a7dn)(struct hisi_isp_ops *);
    int (*loaddts)(struct hisi_isp_ops *);
    int (*loadrsc)(struct hisi_isp_ops *);
    int (*loadfw)(struct hisi_isp_ops *);
	int (*setparams)(struct hisi_isp_ops *, unsigned int, unsigned int, unsigned int);
};

struct hisi_a7mapping_s {
    unsigned int a7va;
    unsigned int size;
    unsigned int prot;
    unsigned int reserve;
    unsigned long a7pa;
    void *apva;
};

struct hisi_atfshrdmem_s {
    struct hisi_a7mapping_s a7mapping[MAXA7MAPPING];
};

enum HISP_CLK_TYPE {
    ISP_A7_CLK = 0,
    ISP_FUNC_CLK = 1,
    ISP_CLK_MAX
};

struct hisi_atfisp_s {
    struct device *device;
    unsigned int boardid;
    int use_secisp;
    void *atfshrd_vaddr;
    unsigned long long atfshrd_paddr;
    void *rsctable_vaddr;
    unsigned long long rsctable_paddr;
    struct hisi_atfshrdmem_s *shrdmem;
    struct regulator *isp_supply;
    struct clk *ispclk[ISP_CLK_MAX];
    unsigned int ispclk_value[ISP_CLK_MAX];
    struct hisi_isp_ops *ispops;
    struct task_struct *secisp_kthread;
    wait_queue_head_t secisp_wait;
    bool secisp_wake;
    struct mutex pwrlock;
    unsigned long long phy_pgd_base;
    struct iommu_domain *domain;
    struct ion_handle *handle;
    struct ion_client *client;
    unsigned int a7va_a7ispmem;
    unsigned int ispva_a7ispmem;
    void *ap_dyna_array;
    struct hisi_a7mapping_s *ap_dyna;
    int map_req_flag;
} atfisp_dev;

struct map_type_info {
    unsigned int va;
    unsigned int size;
};

unsigned int map_type_info[MAP_TYPE_MAX];
extern u64 get_isprdr_addr(void);

int usesecisp(void)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;
    return dev->use_secisp;
}

int hisp_getsglist(void *listmem, void **vaddr, int size)
{
    struct scatterlist *sg;
    struct sg_table *table;
    dma_addr_t dma_addr = 0;
    struct page **pages, **tmppages;
    int npages = 0, ret, i;
    unsigned long long *plist = listmem;
    pgprot_t pageprot = PAGE_KERNEL;

    if ((npages = PAGE_ALIGN(size) / PAGE_SIZE) <= 0) {
        pr_err("[%s] Failed : npages.0x%x, size.0x%x\n", __func__, npages, size);
        return -EINVAL;
    }

    if ((table = kzalloc(sizeof(struct sg_table), GFP_KERNEL)) == NULL) {
        pr_err("[%s] Failed : kzalloc.%p\n", __func__, table);
        return -ENOMEM;
    }

    if ((ret = sg_alloc_table(table, npages, GFP_KERNEL)) != 0) {
        pr_err("[%s] Failed : sg_alloc_table.%d, table.%p, npages.%d\n", __func__, ret, table, npages);
        kfree(table);
        return -ENOMEM;
    }

    if ((pages = vmalloc(npages * sizeof(struct page *))) == NULL) {
        pr_err("[%s] Failed : vmalloc.%p\n", __func__, pages);
        sg_free_table(table);
        kfree(table);
        return -ENOMEM;
    }

    for (i = 0; i < npages; i ++)
        pages[i] = alloc_page(GFP_KERNEL | GFP_DMA | __GFP_ZERO);

    pageprot = pgprot_noncached(pageprot);
    if ((*vaddr = vmap(pages, npages, VM_MAP, pageprot)) == NULL) {
        pr_err("[%s] Failed : vmap.%p\n", __func__, *vaddr);
        sg_free_table(table);
        kfree(table);
        vfree(pages);
        return -ENOMEM;
    }

    tmppages = pages;
    sg = table->sgl;
    while (sg) {
        struct page *page = *tmppages;
        sg_set_page(sg, page, PAGE_SIZE, 0);
        if ((dma_addr = sg_dma_address(sg)) == 0)
            dma_addr = sg_phys(sg);

        *plist = (unsigned long long)dma_addr;
        //pr_err("[%s] size.(0x%x, 0x%x), vmap.%p, dma_addr.0x%llx, tmppages.%p, *plist.(%p, 0x%llx, %p)\n", __func__, size, npages, *vaddr, dma_addr, tmppages, *plist, *plist, plist);
        sg = sg_next(sg);
        plist ++;
        tmppages ++;
    }
    sg_free_table(table);
    kfree(table);

    return npages;
}
/*  
static int hisp_unmap_a7ispmem(void *listmem, void **vaddr, int size)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;

    ion_unmap_iommu(dev->client, dev->handle);
    ion_free(dev->client, dev->handle);

    return 0;
}

static int hisp_map_a7ispmem(void *listmem, void **ispvaddr, int size)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;
    unsigned long long *plist = listmem;
    unsigned int next, sglen;
    struct iommu_map_format format = {0};
    struct sg_table *table = NULL;
    struct scatterlist *sg;
    dma_addr_t dma_addr = 0;
    char name[64];
    int ret;

    snprintf(name, 64, "%u", task_pid_nr(current->group_leader));

    dev->client = hisi_ion_client_create(name);
    if (IS_ERR(dev->client)) {
        pr_err("[%s] Failed : ion_client_create.%s\n", __func__, name);
        return PTR_ERR(dev->client);
    }

    dev->handle = ion_alloc(dev->client, size, SZ_4K, ION_HEAP(ION_SYSTEM_HEAP_ID), 0x0);
    if (IS_ERR(dev->handle)) {
        pr_err("[%s] Failed : ion_alloc.%ld\n", __func__, PTR_ERR(dev->handle));
        return PTR_ERR(dev->handle);
    }

    if ((ret = ion_map_iommu(dev->client, dev->handle, &format)) < 0) {
        pr_err("[%s] Failed : ion_map_iommu.%d\n", __func__, ret);
        return ret;
    }

    table = ion_sg_table(dev->client, dev->handle);
    if (IS_ERR_OR_NULL(table)) {
        pr_err("[%s] Failed : ion_sg_table.%ld\n", __func__, PTR_ERR(table));
        return PTR_ERR(table);
    }

    sg = table->sgl;
    while (sg) {
        if ((dma_addr = sg_dma_address(sg)) == 0)
            dma_addr = sg_phys(sg);

        sglen = sg_dma_len(sg);
        for (next = 0; next < sglen; next += SZ_4K, plist ++)
            *plist = (unsigned long long)(dma_addr + next);

        sg = sg_next(sg);
    }

    *ispvaddr = (void *)format.iova_start;
    pr_info("[%s] ion_map_iommu.(0x%lx, 0x%lx), ispmem size.0x%x)\n", __func__, format.iova_start, format.iova_size, size);

    return 0;
}
*/
void dump_mem(void *addr, int size)
{
    int i;
    pr_err("[%s] addr.%p, size.0x%x\n", __func__, addr, size);
    for (i = 0; i < size; i += 16)
        printk("0x%08x 0x%08x 0x%08x 0x%08x\n", __raw_readl(addr + i + 0), __raw_readl(addr + i + 4), __raw_readl(addr + i + 8), __raw_readl(addr + i + 12));
}

void *getsec_a7sharedmem_addr(void)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;
    return dev->shrdmem->a7mapping[A7SHARED].apva;
}

int get_ispops(struct hisi_isp_ops **ops)
{
	struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;

	*ops = dev->ispops;

	return 0;
}

int hisp_jpeg_powerdn(void)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;
    int ret = 0;

    if (!dev->ispops) {
        pr_err("[%s] Failed : ispops.%p\n", __func__, dev->ispops);
        return -EINVAL;
    }

    if (dev->ispops->jpegdn)
        if ((ret = dev->ispops->jpegdn(dev->ispops)) < 0)
            pr_err("[%s] Failed : jpegdn.%d\n", __func__, ret);

    if (dev->ispops->ispdn)
        if ((ret = dev->ispops->ispdn(dev->ispops)) < 0)
            pr_err("[%s] Failed : ispdn.%d\n", __func__, ret);

    return 0;
}
EXPORT_SYMBOL(hisp_jpeg_powerdn);

int hisp_a7isp_powerdn(void)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;
    int ret = 0;

    if (!dev->ispops) {
        pr_err("[%s] Failed : ispops.%p\n", __func__, dev->ispops);
        return -EINVAL;
    }

    if (dev->ispops->a7dn)
        if ((ret = dev->ispops->a7dn(dev->ispops)) < 0)
            pr_err("[%s] Failed : a7dn.%d\n", __func__, ret);

	return (ret | hisp_jpeg_powerdn());
}

int hisp_jpeg_powerup(void)
{
	struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;
	int ret = 0;

    if (!dev->ispops) {
        pr_err("[%s] Failed : ispops.%p\n", __func__, dev->ispops);
        return -EINVAL;
    }

	if (dev->ispops->ispup) {
		if ((ret = dev->ispops->ispup(dev->ispops)) < 0) {
			pr_err("[%s] Failed : ispup.%d\n", __func__, ret);
			return ret;
		}
	}

    if (dev->ispops->jpegup) {
        if ((ret = dev->ispops->jpegup(dev->ispops)) < 0) {
            pr_err("[%s] Failed : jpegup.%d\n", __func__, ret);
            goto err_jpegup;
        }
    }

	return 0;

err_jpegup:
    if (dev->ispops->ispdn)
        if (dev->ispops->ispdn(dev->ispops) < 0)
            pr_err("[%s] Failed : err_jpegup ispdn\n", __func__);

	return ret;
}
EXPORT_SYMBOL(hisp_jpeg_powerup);

int hisp_a7isp_powerup(void)
{
	struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;
	int ret = 0;

	if (!dev->ispops) {
		pr_err("[%s] Failed : ispops.%p\n", __func__, dev->ispops);
		return -EINVAL;
	}

	if ((ret = hisp_jpeg_powerup()) < 0) {
		pr_err("[%s] Failed : hisp_jpeg_powerup.%d\n", __func__, ret);
		return ret;
	}

    if (dev->ispops->a7up) {
        if ((ret = dev->ispops->a7up(dev->ispops)) < 0) {
            pr_err("[%s] Failed : a7up.%d\n", __func__, ret);
            goto err_a7up;
        }
    }

	if (dev->ispops->loadfw) {
		if ((ret = dev->ispops->loadfw(dev->ispops)) < 0) {
			pr_err("[%s] Failed : loadfw.%d\n", __func__, ret);
			goto err_loadfw;
		}
	}

	return 0;

err_loadfw:
    if (dev->ispops->a7dn)
        if (dev->ispops->a7dn(dev->ispops) < 0)
            pr_err("[%s] Failed : err_loadfw a7dn\n", __func__);
err_a7up:
    if (hisp_jpeg_powerdn() < 0)
        pr_err("[%s] Failed : err_a7up hisp_jpeg_powerdn\n", __func__);

    return ret;
}

static int bsp_read_bin(const char *partion_name, unsigned int offset,
				unsigned int length, char *buffer)
{
	int ret          = -1;
	char *pathname   = NULL;
	struct file *fp;
	mm_segment_t fs;
	loff_t pos = 0;

	if ((NULL == partion_name) || (NULL == buffer)) {
		pr_err("partion_name(%p) or buffer(%p) is null", partion_name, buffer);
		return -1;
	}

    /*get resource*/
	pathname = kmalloc(PART_NAMELEN, GFP_KERNEL);
	if (!pathname) {
		pr_err("pathname malloc failed\n");
		return -1;
	}

	ret = flash_find_ptn((const char *)partion_name, pathname);
	if (ret < 0) {
		pr_err("partion_name(%s) is not in partion table!\n", partion_name);
		goto error;
	}

	fp = filp_open(pathname, O_RDONLY, 0600);
	if (IS_ERR(fp)) {
		pr_err("filp_open(%s) failed", pathname);
		goto error;
	}

	ret = vfs_llseek(fp, offset, SEEK_SET);
	if (ret < 0) {
		pr_err("seek ops failed, ret %d", ret);
		goto error2;
	}

	fs = get_fs();
	set_fs(KERNEL_DS);

	pos = fp->f_pos;
	ret = vfs_read(fp, (char __user *)buffer, length, &pos);
	if (ret != length) {
		pr_err("read ops failed, ret=%d(len=%d)", ret, length);
		set_fs(fs);
		goto error2;
	}
	set_fs(fs);

    filp_close(fp, NULL);

    /*free resource*/
    if(NULL != pathname) {
       kfree(pathname);
       pathname = NULL;
    }

    return 0;

error2:
    filp_close(fp, NULL);

error:
    if(NULL != pathname) {
       kfree(pathname);
       pathname = NULL;
    }

    pr_err("failed");
    return -1;
}

typedef enum {
    ISPIMG_DTS = 0,
    ISPIMG_RSC,
    ISPIMG_FW,
    ISPIMG_MAX
} ISPIMG_TYPE_E;

typedef enum {
    ISPPWR_JPEGUP = 0,
    ISPPWR_JPEGDN,
    ISPPWR_A7UP,
    ISPPWR_A7DN,
    ISPPWR_MAX
} ISPPWR_TYPE_E;

struct hisi_isp_loadimg_s {
#define ISP_SECBASE     (0x1AC00000)
#define ISPDTS_BASE     (0x00000000)
#define ISPFW_BASE      (0x00010000 + ISP_SECBASE)
#define ISPRSC_BASE     (0x00003000)
#define ISPDTS_SIZE     (0x02000000)
#define ISPRSC_SIZE     (0x00001000)
#define ISPFW_SIZE      (0x00800000)
    unsigned int addr;
    unsigned int size;
    char *partition;
} loadimg[] = {
    {ISPDTS_BASE,   ISPDTS_SIZE,    "isp_dts"},
    {ISPRSC_BASE,   ISPRSC_SIZE,    "isp_firmware"},
    {ISPFW_BASE,    ISPFW_SIZE,     "isp_firmware"}
};

static int atfisp_loaddts(struct hisi_isp_ops *data)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;
    unsigned int offset, size;
	int ret = 0;
	char *pname;

    pname = loadimg[ISPIMG_DTS].partition;
    offset = loadimg[ISPIMG_DTS].addr;
    size = dev->shrdmem->a7mapping[A7DTS].size;
    if ((ret = bsp_read_bin(pname, offset, size, dev->shrdmem->a7mapping[A7DTS].apva)) < 0) {
        pr_err("[%s] bsp_read_bin.%d\n", __func__, ret);
        return ret;
    }

	return 0;
}

static int atfisp_loadrsctable(struct hisi_isp_ops *data)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;
    unsigned int offset, size;
    int ret = 0;
    char *pname;

    pname = loadimg[ISPIMG_RSC].partition;
    offset = loadimg[ISPIMG_RSC].addr;
    size = loadimg[ISPIMG_RSC].size;
    if ((ret = bsp_read_bin(pname, offset, size, dev->rsctable_vaddr)) < 0) {
        pr_err("[%s] bsp_read_bin.%d\n", __func__, ret);
        return ret;
    }

	return 0;
}

static int atfisp_loadfw(struct hisi_isp_ops *data)
{
    struct load_image_info loadinfo;
	int ret = 0;

    loadinfo.ecoretype      = ISP;
	loadinfo.image_addr     = loadimg[ISPIMG_FW].addr;
	loadinfo.image_size     = loadimg[ISPIMG_FW].size;
	loadinfo.partion_name   = loadimg[ISPIMG_FW].partition;
	pr_err("[%s] + %s.(0x%lx, 0x%x), init.%x\n", __func__, loadinfo.partion_name, loadinfo.image_addr, loadinfo.image_size, data->refs_fw);
    if ((ret = bsp_load_and_verify_image(&loadinfo)) < 0) {
	    pr_err("[%s] Failed : bsp_load_and_verify_image.%d, %s.(0x%lx, 0x%x)\n", __func__, ret, loadinfo.partion_name, loadinfo.image_addr, loadinfo.image_size);
	    return ret;
	}

    data->refs_fw++;

    pr_err("[%s] - bsp_load_and_verify_image.%d, %s.(0x%lx, 0x%x), init.%x\n", __func__, ret, loadinfo.partion_name, loadinfo.image_addr, loadinfo.image_size, data->refs_fw);

	return 0;
}

#define HISP_PARAMS_SHRDLIST    (0xCCCE0000)

#define RSC_OFFSET              (0x00003000)
#define RSC_SIZE                (0x00001000)
#define LISTENTRY_SIZE          (0x000A0000)

static int atfisp_setparams(struct hisi_isp_ops *data, unsigned int type, unsigned int param1, unsigned int param2)
{
	atfd_hisi_service_isp_smc(ISP_FN_SET_PARAMS, type, param1, param2);
	return 0;
}

static int ispa7_setclk_enable(struct hisi_atfisp_s *dev)
{
	int ret;

    if ((ret = clk_set_rate(dev->ispclk[ISP_A7_CLK], dev->ispclk_value[ISP_A7_CLK])) < 0) {
        pr_err("[%s] Failed: clk_set_rate.%d\n", __func__, ret);
        return ret;
    }

    if ((ret = clk_prepare_enable(dev->ispclk[ISP_A7_CLK])) < 0) {
        pr_err("[%s] Failed: clk_prepare_enable.%d\n", __func__, ret);
        return ret;
    }

    return 0;
}

static int ispfunc_setclk_enable(struct hisi_atfisp_s *dev)
{
    int ret;

    if ((ret = clk_set_rate(dev->ispclk[ISP_FUNC_CLK], dev->ispclk_value[ISP_FUNC_CLK])) < 0) {
        pr_err("[%s] Failed: clk_set_rate.%d\n", __func__, ret);
        return ret;
    }

    if ((ret = clk_prepare_enable(dev->ispclk[ISP_FUNC_CLK])) < 0) {
        pr_err("[%s] Failed: clk_prepare_enable.%d\n", __func__, ret);
        return ret;
    }

    return 0;
}

static void ispa7_clk_disable(struct hisi_atfisp_s *dev)
{
    clk_disable_unprepare(dev->ispclk[ISP_A7_CLK]);
}

static void ispfunc_clk_disable(struct hisi_atfisp_s *dev)
{
    clk_disable_unprepare(dev->ispclk[ISP_FUNC_CLK]);
}

static int atfisp_subsysup(struct hisi_isp_ops *data)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;
    int ret;

    pr_err("[%s] + refs.%d\n", __func__, dev->ispops->refs_isp);

    if ((ret = ispfunc_setclk_enable(dev)) < 0) {
        pr_err("Failed: ispfunc_setclk_enable.%d\n", ret);
        return ret;
    }

    if ((ret = ispa7_setclk_enable(dev)) < 0) {
        pr_err("Failed: ispa7_setclk_enable.%d\n", ret);
        ispfunc_clk_disable(dev);
        return ret;
    }

    if ((ret = regulator_enable(dev->isp_supply)) != 0) {
        pr_err("Failed: regulator_enable.%d\n", ret);
        goto err_clk;
    }

    dev->ispops->refs_isp++;
    pr_err("[%s] - refs.%d\n", __func__, dev->ispops->refs_isp);

    return 0;

err_clk:
    ispa7_clk_disable(dev);
    ispfunc_clk_disable(dev);

    return ret;
}

static int atfisp_subsysdown(struct hisi_isp_ops *data)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;
    int ret = 0;

    pr_err("[%s] + refs.%d\n", __func__, dev->ispops->refs_isp);
    if ((ret = regulator_disable(dev->isp_supply)) != 0)
        pr_err("Failed: regulator_disable.%d\n", ret);

    ispa7_clk_disable(dev);
    ispfunc_clk_disable(dev);

    dev->ispops->refs_isp--;
    pr_err("[%s] - refs.%d\n", __func__, dev->ispops->refs_isp);

    return 0;
}

static int atfjpeg_subsysup(struct hisi_isp_ops *data)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;

    pr_err("[%s] + refs.%d\n", __func__, dev->ispops->refs_jpeg);
    atfd_hisi_service_isp_smc(ISP_FN_JPEG_INIT, dev->phy_pgd_base, 0, 0);
    dev->ispops->refs_jpeg++;
    pr_err("[%s] - refs.%d\n", __func__, dev->ispops->refs_jpeg);

    return 0;
}

static int atfjpeg_subsysdown(struct hisi_isp_ops *data)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;

    pr_err("[%s] + refs.%d\n", __func__, dev->ispops->refs_jpeg);
    atfd_hisi_service_isp_smc(ISP_FN_JPEG_EXIT, 0, 0, 0);
    dev->ispops->refs_jpeg--;
    pr_err("[%s] - refs.%d\n", __func__, dev->ispops->refs_jpeg);

    return 0;
}

static int atfa7_subsysup(struct hisi_isp_ops *data)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;
	unsigned int isp_base;

    pr_err("[%s] + refs.%d\n", __func__, dev->ispops->refs_a7);
	isp_base = ISP_RESERVED_BASE;
    atfd_hisi_service_isp_smc(ISP_FN_A7_INIT, isp_base, 0, 0);
    dev->ispops->refs_a7++;
    pr_err("[%s] - refs.%d\n", __func__, dev->ispops->refs_a7);

    return 0;
}

static int atfa7_subsysdown(struct hisi_isp_ops *data)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;
	unsigned int type;

    pr_err("[%s] + refs.%d\n", __func__, dev->ispops->refs_a7);
	type = 1;
    atfd_hisi_service_isp_smc(ISP_FN_A7_EXIT, type, 0, 0);
    dev->ispops->refs_a7--;
    pr_err("[%s] - refs.%d\n", __func__, dev->ispops->refs_a7);

    return 0;
}

struct hisi_isp_ops atfisp_ops = {
	.ispup      = atfisp_subsysup,
	.ispdn      = atfisp_subsysdown,
    .jpegup     = atfjpeg_subsysup,
    .jpegdn     = atfjpeg_subsysdown,
    .a7up       = atfa7_subsysup,
    .a7dn       = atfa7_subsysdown,
    .loaddts    = atfisp_loaddts,
    .loadrsc    = atfisp_loadrsctable,
    .loadfw     = atfisp_loadfw,
	.setparams  = atfisp_setparams,
};

int secisp_device_enable(void)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;

    if (!dev->secisp_kthread) {
        pr_err("[%s] Failed : secisp_kthread.%p\n", __func__, dev->secisp_kthread);
        return -ENXIO;
    }

    dev->secisp_wake = 1;
    wake_up(&dev->secisp_wait);

    return 0;
}

static void isp_a7_qos_cfg(void)
{
    //unsigned int prio, mode;
    void __iomem* vivobus_base;

    vivobus_base = (void __iomem*)ioremap(REG_BASE_VIVOBUS_PHY, SZ_4K);
    if (vivobus_base == NULL) {
        pr_err("vivobus_base remap fail");
        return;
    }

    __raw_writel(QOS_PRIO_3,            (volatile void __iomem*)(vivobus_base + ISP_RD_QOS_PRIORITY));
    __raw_writel(QOS_BYPASS_MODE,            (volatile void __iomem*)(vivobus_base + ISP_RD_QOS_MODE));
    __raw_writel(QOS_PRIO_3,      (volatile void __iomem*)(vivobus_base + ISP_WR_QOS_PRIORITY));
    __raw_writel(QOS_BYPASS_MODE, (volatile void __iomem*)(vivobus_base + ISP_WR_QOS_MODE));
    __raw_writel(QOS_PRIO_4,      (volatile void __iomem*)(vivobus_base + A7T0VIVOBUS_RD_QOS_PRIORITY));
    __raw_writel(QOS_FIX_MODE,    (volatile void __iomem*)(vivobus_base + A7T0VIVOBUS_RD_QOS_MODE));
    __raw_writel(QOS_PRIO_4,      (volatile void __iomem*)(vivobus_base + A7T0VIVOBUS_WR_QOS_PRIORITY));
    __raw_writel(QOS_FIX_MODE,    (volatile void __iomem*)(vivobus_base + A7T0VIVOBUS_WR_QOS_MODE));
    __raw_writel(QOS_PRIO_3,            (volatile void __iomem*)(vivobus_base + ISP1_RD_QOS_PRIORITY));
    __raw_writel(QOS_BYPASS_MODE,            (volatile void __iomem*)(vivobus_base + ISP1_RD_QOS_MODE));
    __raw_writel(QOS_PRIO_3,      (volatile void __iomem*)(vivobus_base + ISP1_WR_QOS_PRIORITY));
    __raw_writel(QOS_BYPASS_MODE, (volatile void __iomem*)(vivobus_base + ISP1_WR_QOS_MODE));

    pr_info("QOS : ISP.rd.(prio.0x%x, mode.0x%x), ISP.wr.(prio.0x%x, mode.0x%x), A7.rd.(prio.0x%x, mode.0x%x), A7.wr.(prio.0x%x, mode.0x%x),ISP1.rd.(prio.0x%x, mode.0x%x), ISP1.wr.(prio.0x%x, mode.0x%x)\n",
    __raw_readl((volatile void __iomem*)(vivobus_base + ISP_RD_QOS_PRIORITY)), __raw_readl((volatile void __iomem*)(vivobus_base + ISP_RD_QOS_MODE)),
    __raw_readl((volatile void __iomem*)(vivobus_base + ISP_WR_QOS_PRIORITY)), __raw_readl((volatile void __iomem*)(vivobus_base + ISP_WR_QOS_MODE)),
    __raw_readl((volatile void __iomem*)(vivobus_base + A7T0VIVOBUS_RD_QOS_PRIORITY)), __raw_readl((volatile void __iomem*)(vivobus_base + A7T0VIVOBUS_RD_QOS_MODE)),
    __raw_readl((volatile void __iomem*)(vivobus_base + A7T0VIVOBUS_WR_QOS_PRIORITY)), __raw_readl((volatile void __iomem*)(vivobus_base + A7T0VIVOBUS_WR_QOS_MODE)),
    __raw_readl((volatile void __iomem*)(vivobus_base + ISP1_RD_QOS_PRIORITY)), __raw_readl((volatile void __iomem*)(vivobus_base + ISP1_RD_QOS_MODE)),
    __raw_readl((volatile void __iomem*)(vivobus_base + ISP1_WR_QOS_PRIORITY)), __raw_readl((volatile void __iomem*)(vivobus_base + ISP1_WR_QOS_MODE)));

    iounmap(vivobus_base);
    vivobus_base = NULL;
}

static int do_secisp_device_enable(void)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;
    int ret, err_ret;;

    if ((ret = atfisp_subsysup(dev->ispops)) < 0) {
        pr_err("[%s] Failed : atfisp_subsysup.%d\n", __func__, ret);
        return ret;
    }

    if ((ret = atfjpeg_subsysup(dev->ispops)) < 0) {
        pr_err("[%s] Failed : atfjpeg_subsysup.%d\n", __func__, ret);
        if ((err_ret = atfisp_subsysdown(dev->ispops)) < 0)
            pr_err("[%s] Failed : atfisp_subsysdown.%d\n", __func__, err_ret);
        return ret;
    }

    isp_a7_qos_cfg();

    if ((ret = atfa7_subsysup(dev->ispops)) < 0) {
        pr_err("[%s] Failed : atfa7_subsysup.%d\n", __func__, ret);
        if ((err_ret = atfjpeg_subsysdown(dev->ispops)) < 0)
            pr_err("[%s] Failed : atfjpeg_subsysdown.%d\n", __func__, err_ret);
        if ((err_ret = atfisp_subsysdown(dev->ispops)) < 0)
            pr_err("[%s] Failed : atfisp_subsysdown.%d\n", __func__, err_ret);
        return ret;
    }

    if ((ret = atfisp_loadfw(dev->ispops)) < 0) {
        pr_err("[%s] Failed : atfisp_loadfw.%d\n", __func__, ret);
        if ((err_ret = atfa7_subsysdown(dev->ispops)) < 0)
            pr_err("[%s] Failed : atfa7_subsysdown.%d\n", __func__, err_ret);
        if ((err_ret = atfjpeg_subsysdown(dev->ispops)) < 0)
            pr_err("[%s] Failed : atfjpeg_subsysdown.%d\n", __func__, err_ret);
        if ((err_ret = atfisp_subsysdown(dev->ispops)) < 0)
            pr_err("[%s] Failed : atfisp_subsysdown.%d\n", __func__, err_ret);
        return ret;
    }

    return 0;
}

int secisp_device_disable(void)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;
    int ret;

    mutex_lock(&dev->pwrlock);
    if ((ret = atfa7_subsysdown(dev->ispops)) < 0)
        pr_err("[%s] Failed : atfa7_subsysdown.%d\n", __func__, ret);

    if ((ret = atfjpeg_subsysdown(dev->ispops)) < 0)
        pr_err("[%s] Failed : atfjpeg_subsysdown.%d\n", __func__, ret);

    if ((ret = atfisp_subsysdown(dev->ispops)) < 0)
        pr_err("[%s] Failed : atfisp_subsysdown.%d\n", __func__, ret);
    mutex_unlock(&dev->pwrlock);

    return 0;
}

static ssize_t atfisp_show(struct device *, struct device_attribute *, char *);
static ssize_t atfisp_store(struct device *, struct device_attribute *, const char *, size_t);
static ssize_t regs_show(struct device *, struct device_attribute *, char *);
static ssize_t regs_store(struct device *, struct device_attribute *, const char *, size_t);
static DEVICE_ATTR(atfisp, (S_IRUGO | S_IWUSR | S_IWGRP), atfisp_show, atfisp_store);
static DEVICE_ATTR(regs, (S_IRUGO | S_IWUSR | S_IWGRP), regs_show, regs_store);

static struct miscdevice atf_ispdev = {
    .minor = 255,
    .name = "atf_ispdev",
};

static unsigned long hisp_getreservemem(unsigned int etype, unsigned long paddr)
{
#define ISP_MEM_BASE		(0x1AC00000)
#define MEM_TEXT_PA     	(ISP_MEM_BASE + 0x00010000)
#define MEM_DATA_PA     	(ISP_MEM_BASE + 0x00310000)
#define MEM_HEAP_PA      	(ISP_MEM_BASE + 0x00A10000)
#define REG_BASE		    (0xE0000000)
    unsigned long addr;

    switch(etype) {
        case A7BOOT:
            addr = MEM_TEXT_PA;
            break;
        case A7TEXT:
            addr = MEM_TEXT_PA;
            break;
        case A7DATA:
            addr = MEM_DATA_PA;
            break;
        case A7HEAP:
            addr = MEM_HEAP_PA;
            break;
        case A7REGS:
            addr = REG_BASE;
            break;
        case A7VQ:
        case A7VRING0:
        case A7VRING1:
            addr = paddr;
            break;
        case A7RDR:
            addr = get_isprdr_addr();
            pr_err("[%s] A7RDR.0x%lx\n", __func__, addr);
            break;
        default:
            pr_err("[%s] Failed : etype.0x%x\n", __func__, etype);
            return 0;
    }

    return addr;
}

int hisp_meminit(unsigned int etype, unsigned long paddr)
{
    const char *propname[] = {
        "a7-vaddr-boot",
        "a7-vaddr-text",
        "a7-vaddr-data",
        "a7-vaddr-rdr",
        "a7-vaddr-shrd",
        "a7-vaddr-vq",
        "a7-vaddr-vr0",
        "a7-vaddr-vr1",
        "a7-vaddr-heap",
        "a7-vaddr-a7dyna",
        "a7-vaddr-dts",
        "a7-vaddr-regs"
    };
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;
    struct device_node *np = dev->device->of_node;
    unsigned int offset = 0, size = 0;
    void *addr = NULL, *vaddr = NULL;
	int ret, index;

    if (!np) {
        pr_err("[%s] Failed : np.%p\n", __func__, np);
        return -EINVAL;	
    }

    if (etype >= MAXA7MAPPING) {
        pr_err("[%s] Failed : etype.(0x%x >= 0x%x)\n", __func__, etype, MAXA7MAPPING);
        return -EINVAL;	
    }

    if (dev->shrdmem == NULL) {
        pr_err("[%s] Failed : shrdmem.(%p = %p)\n", __func__, dev->shrdmem, dev->atfshrd_vaddr);
        return -ENOMEM;	
    }

    if((ret = of_property_read_u32_array(np, propname[etype], (u32 *)&dev->shrdmem->a7mapping[etype], 4)) < 0) {
        pr_err("[%s] Failed : propname.%s, of_property_read_u32_array.%d\n",
            __func__, propname[etype], ret);
        return -EINVAL;	
	}
    pr_info("[%s] propname.%s, of_property_read_u32_array.%d.(0x%x, 0x%x, 0x%x, 0x%x)\n", __func__, propname[etype], ret,
        dev->shrdmem->a7mapping[etype].a7va, dev->shrdmem->a7mapping[etype].size, dev->shrdmem->a7mapping[etype].prot, dev->shrdmem->a7mapping[etype].reserve);

    if (dev->shrdmem->a7mapping[etype].reserve) {
        dev->shrdmem->a7mapping[etype].a7pa = hisp_getreservemem(etype, paddr);
    } else {
        index = 4;
        if((ret = of_property_read_u32_index(np, propname[etype], index, &offset)) < 0) {
            pr_err("[%s] Failed : propname.%s, %d.of_property_read_u32_index.%d\n", __func__, propname[etype], index, ret);
            return -EINVAL;	
    	}

        index = 5;
        if((ret = of_property_read_u32_index(np, propname[etype], index, &size)) < 0) {
            pr_err("[%s] Failed : propname.%s, %d.of_property_read_u32_index.%d\n", __func__, propname[etype], index, ret);
            return -EINVAL;	
    	}

        dev->shrdmem->a7mapping[etype].a7pa = dev->atfshrd_paddr + offset;
        addr = offset + dev->atfshrd_vaddr;
        pr_err("[%s] atfshrd_vaddr.0x%llx, addr.0x%p, vaddr.0x%p, offset.0x%x, size.0x%x\n", __func__, (long long unsigned int)dev->atfshrd_vaddr, addr, vaddr, offset, size);
        switch (etype) {
            case A7DYNAMIC:
                dev->ap_dyna_array = addr;
                dev->ap_dyna = (struct hisi_a7mapping_s *)&dev->shrdmem->a7mapping[etype];
#if 0
                if ((ret = hisp_map_a7ispmem(addr, &vaddr, dev->shrdmem->a7mapping[etype].size)) < 0) {
                    pr_err("[%s] hisp_map_a7ispmem.0x%p\n", __func__, vaddr);
                    return -ENOMEM;
                }
                dev->a7va_a7ispmem = dev->shrdmem->a7mapping[etype].a7va;
                dev->ispva_a7ispmem = vaddr;
                pr_err("[%s] a7va_a7ispmem.(0x%x = 0x%x), ispva_a7ispmem.(0x%x = 0x%x)\n", __func__, dev->a7va_a7ispmem, dev->shrdmem->a7mapping[etype].a7pa, dev->ispva_a7ispmem, vaddr);
#endif
                pr_err("[%s] dyna_array.0x%p\n", __func__, dev->ap_dyna_array);
                break;
            default :
                if ((ret = hisp_getsglist(addr, &vaddr, size)) < 0) {
                    pr_err("[%s] hisp_getsglist.0x%p\n", __func__, vaddr);
                    return -ENOMEM;
                }
                dev->shrdmem->a7mapping[etype].apva = vaddr;
                break;
        }
    }

    return 0;
}

int hisp_apisp_map(unsigned int *a7addr, unsigned int *ispaddr, unsigned int size)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;

    if ((dev->a7va_a7ispmem == 0) || (dev->ispva_a7ispmem == 0))
        return -EINVAL;	

    *a7addr = dev->a7va_a7ispmem;
    *ispaddr = dev->ispva_a7ispmem;
    pr_info("[%s] X a7va.(0x%x, 0x%x), ispva.(0x%x, 0x%x)\n", __func__, *a7addr, dev->a7va_a7ispmem, *ispaddr, dev->ispva_a7ispmem);

    return 0;
}
EXPORT_SYMBOL(hisp_apisp_map);

int hisp_apisp_unmap(void)
{
    return 0;
}
EXPORT_SYMBOL(hisp_apisp_unmap);

static int hisp_sharedmem_init(struct hisi_atfisp_s *dev)
{
    dma_addr_t dma_addr = 0;
    int ret;

    if ((dev->atfshrd_vaddr = dma_alloc_coherent(dev->device, LISTENTRY_SIZE, &dma_addr, GFP_KERNEL)) == NULL) {
        pr_err("[%s] atfshrd_vaddr.0x%p\n", __func__, dev->atfshrd_vaddr);
        return -ENOMEM;
    }
    dev->atfshrd_paddr = (unsigned long long)dma_addr;
    dev->shrdmem = (struct hisi_atfshrdmem_s *)dev->atfshrd_vaddr;

    if ((ret = atfisp_setparams(dev->ispops, HISP_PARAMS_SHRDLIST, dev->atfshrd_paddr, LISTENTRY_SIZE)) < 0) {
	    pr_err("[%s] atfisp_setparams.%d, (type.0x%x, atfshrd_paddr.(0x%llx >> 0x%llx))\n", __func__, ret, HISP_PARAMS_SHRDLIST, dev->atfshrd_paddr, (unsigned long long)dev->atfshrd_vaddr);
	    kfree(dev->atfshrd_vaddr);
	    return -ENODEV;
	}

	pr_info("[%s] atfisp_setparams.%d, (type.0x%x, atfshrd_paddr.(0x%llx >> 0x%llx))\n", __func__, ret, HISP_PARAMS_SHRDLIST, dev->atfshrd_paddr, (unsigned long long)dev->atfshrd_vaddr);

	return 0;
}

#if 0  //not use
static int hisp_sharedmem_deinit(struct hisi_atfisp_s *dev)
{
    if (dev->atfshrd_vaddr) {
        dma_free_coherent(dev->device, LISTENTRY_SIZE, dev->atfshrd_vaddr, dev->atfshrd_paddr);
        dev->atfshrd_vaddr = NULL;
        dev->shrdmem = NULL;
    }

	return 0;
}
#endif

static int hisi_atf_getdts(struct platform_device *pdev)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;
    struct device *device = &pdev->dev;
    struct device_node *np = device->of_node;
    //unsigned int ispa7_clk;
    //const char *name = NULL;
    const char *clk_name[ISP_CLK_MAX];
    int ret, i;

    if (!np) {
        pr_err("[%s] Failed : np.%p.\n", __func__, np);
        return -ENODEV;
    }

    dev->isp_supply = devm_regulator_get(device, "isp-subsys-rproc");
    if (IS_ERR(dev->isp_supply)) {
        pr_err("[%s] Failed : devm_regulator_get.%p\n", __func__, dev->isp_supply);
        return -EINVAL;
    }


    if ((ret = of_property_read_string_array(np, "clock-names", clk_name, ISP_CLK_MAX)) < 0) {
        pr_err("[%s] Failed : of_property_read_string_array.%d, boardid.0x%x\n", __func__, ret, dev->boardid);
        return -EINVAL;
    }

    if ((ret = of_property_read_u32_array(np, "clock-value", (u32 *)&dev->ispclk_value[0], ISP_CLK_MAX)) < 0) {
        pr_err("[%s] Failed: of_property_read_u32_array.%d\n", __func__, ret);
        return -EINVAL;
    }

    for (i = 0; i < ISP_CLK_MAX; i ++) {
        dev->ispclk[i] = devm_clk_get(device, clk_name[i]);
        if (IS_ERR_OR_NULL(dev->ispclk[i])) {
            pr_err("[%s] Failed : ispclk.%s.%d.%li\n", __func__, clk_name[i], i, PTR_ERR(dev->ispclk[i]));
            return -EINVAL;
        }
        pr_info("[%s] ISP clock.%d.%s: %d M\n", __func__, i, clk_name[i], dev->ispclk_value[i]);
    }

    if (of_machine_is_compatible("hisilicon,hi3660"))
        dev->use_secisp = 1;
    pr_info("use_secisp.%d\n", dev->use_secisp);

    return 0;
}

void *get_rsctable(int *tablesz)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;

    *tablesz = RSC_SIZE;
    return dev->rsctable_vaddr;
}

void a7_map_set_pa_list(void *listmem, struct scatterlist *sg, unsigned int size)
{
    dma_addr_t dma_addr = 0;
    unsigned int len, set_size = 0;
    unsigned long long *plist = listmem;

    while (sg) {
        if ((dma_addr = sg_dma_address(sg)) == 0)
            dma_addr = sg_phys(sg);

        len = sg->length;
        if (len == 0) {
            pr_err("[%s] break len.0x%x\n", __func__, len);
            break;
        }
        set_size += len;
        if (set_size > size) {
            pr_err("[%s] break size.(0x%x > 0x%x), len.0x%x\n", __func__, set_size, size, len);
            break;
        }

        do {
            *plist = (unsigned long long)dma_addr;
            plist++;
            len -= PAGE_SIZE;
            dma_addr += PAGE_SIZE;
        } while (len > 0);
        sg = sg_next(sg);
    }

    pr_info("%s: size.0x%x == set_size.0x%x\n", __func__, size, set_size);
}

unsigned int a7_mmu_map(struct scatterlist *sgl, unsigned int size, unsigned int prot, unsigned int flag)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;
    unsigned int offset = 0, map_size = 0, map_prot;
    struct hisi_a7mapping_s *map_info = dev->ap_dyna;
    void *addr = dev->ap_dyna_array;
    //int ret = 0;

    map_size = PAGE_ALIGN(size);
    map_prot = map_info->prot;
    switch(flag) {
        case MAP_TYPE_REQUEST:
            offset = dev->map_req_flag *  A7_REQUEST_BUF_SIZE;
            map_info->a7va = A7_REQUEST_ADDR + offset;
            map_info->size = map_size;

            if (++dev->map_req_flag == A7_REQUEST_BUF_NUM)
                dev->map_req_flag = 0;

            break;
        case MAP_TYPE_GET_OTP:
            map_info->a7va = A7_GET_OTP_ADDR;
            map_info->size = map_size;
            break;
        case MAP_TYPE_CAPABILITY:
            map_info->a7va = A7_CAPABILITY_ADDR;
            map_info->size = map_size;
            break;
        case MAP_TYPE_ACQUIRE_CAMERA:
            map_info->a7va = A7_ACQUIRE_ADDR;
            map_info->size = map_size;
            break;
        case MAP_TYPE_CALIBRATION:
            map_info->a7va = A7_CALIBRATION_ADDR;
            map_info->size = map_size;
            break;
        case MAP_TYPE_TONEMAP:
            map_info->a7va = A7_TONEMAP_ADDR;
            map_info->size = map_size;
            break;
        case MAP_TYPE_MAP_BUFFER:
            map_info->a7va = A7_MAP_BUFFER_ADDR;
            map_info->size = A7_MAP_BUFFER_SIZE;
            break;
        default:
            pr_err("[%s] Failed : type.0x%x\n", __func__, flag);
            return -EINVAL;
    }
    pr_info("[%s] type.0x%x, a7va.0x%x, size.0x%x, map_size.0x%x, map_prot.0x%x\n", __func__,
                            flag, map_info->a7va, size, map_size, map_prot);
    a7_map_set_pa_list(addr, sgl, map_size);

    atfd_hisi_service_isp_smc(ISP_FN_A7_MAP, 0, 0, 0);
    return map_info->a7va;
}

void a7_mmu_unmap(unsigned int va, unsigned int size)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;
    struct hisi_a7mapping_s *map_info = dev->ap_dyna;
    unsigned int map_size;

    map_size = PAGE_ALIGN(size);
    pr_info("%s: va = 0x%x, size = 0x%x, map_size = 0x%x\n", __func__, va, size, map_size);
    map_info->a7va = va;
    map_info->size = map_size;

    atfd_hisi_service_isp_smc(ISP_FN_A7_UNMAP, 0, 0, 0);
}

int hisp_rsctable_init(void)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;
    dma_addr_t dma_addr = 0;
    int ret;

    if ((dev->rsctable_vaddr = dma_alloc_coherent(dev->device, RSC_SIZE, &dma_addr, GFP_KERNEL)) == NULL) {
        pr_err("[%s] atfshrd_vaddr.0x%p\n", __func__, dev->rsctable_vaddr);
        return -ENOMEM;
    }
    dev->rsctable_paddr = (unsigned long long)dma_addr;

    if ((ret = bsp_read_bin("isp_firmware", RSC_OFFSET, RSC_SIZE, dev->rsctable_vaddr)) < 0) {
        pr_err("[%s] bsp_read_bin.%d\n", __func__, ret);
        return ret;
    }

    pr_info("[%s] [A7DTS].apva.0x%p\n", __func__, dev->shrdmem->a7mapping[A7DTS].apva);
    if ((ret = bsp_read_bin("isp_dts", 0x0, dev->shrdmem->a7mapping[A7DTS].size, dev->shrdmem->a7mapping[A7DTS].apva)) < 0) {
        pr_err("[%s] bsp_read_bin.%d\n", __func__, ret);
        return ret;
    }

    return 0;
}

static int secisp_work_fn(void *data)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;
    int ret;

    pr_info("[%s] +\n", __func__);
    while (1) {
        if (kthread_should_stop())
            break;
        wait_event(dev->secisp_wait, dev->secisp_wake);
        mutex_lock(&dev->pwrlock);
        if ((ret = do_secisp_device_enable()) < 0) {
            pr_err("[%s] Failed : do_secisp_device_enable.%d.\n", __func__, ret);
        }
        mutex_unlock(&dev->pwrlock);
        dev->secisp_wake = 0;
    }
    pr_info("[%s] -\n", __func__);

    return 0;
}

int hisi_atfisp_probe(struct platform_device *pdev)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;
    struct iommu_domain_data *info = NULL;
    struct device *device = &pdev->dev;
    int ret = 0, index = 0;

    dev->map_req_flag = 0;
    dev->device = &pdev->dev;
    atfisp_ops.refs_isp         = UNINITIAL;
    atfisp_ops.refs_jpeg        = UNINITIAL;
    atfisp_ops.refs_a7          = UNINITIAL;
    atfisp_ops.refs_dts  = UNINITIAL;
    atfisp_ops.refs_fw   = UNINITIAL;
    dev->ispops                 = &atfisp_ops;

    if ((ret = misc_register(&atf_ispdev)) != 0) {
        pr_err("[%s] Failed : misc_register.%d.\n", __func__, ret);
        return ret;
	}

    if ((ret = device_create_file(atf_ispdev.this_device, &dev_attr_atfisp)) != 0)
        pr_err("[%s] Faield : atfisp device_create_file.%d\n", __func__, ret);
    if ((ret = device_create_file(atf_ispdev.this_device, &dev_attr_regs)) != 0)
        pr_err("[%s] Faield : regs device_create_file.%d\n", __func__, ret);

    dev->use_secisp = -1;

    if ((ret = hisi_atf_getdts(pdev)) < 0)
        pr_err("[%s] Failed : hisi_atf_getdts.%d.\n", __func__, ret);

	if ((ret = hisp_sharedmem_init(dev)) < 0)
		pr_err("[%s] Failed : hisp_sharedmem_init.%d.\n", __func__, ret);

    for (index = 0; index < MAXA7MAPPING; index ++)
        if ((ret = hisp_meminit(index, 0)) < 0)
            pr_err("Failed : index.%d, hisp_meminit.%d\n", index, ret);

    isp_share_para = (struct rproc_shared_para *)dev->shrdmem->a7mapping[A7SHARED].apva;

    if ((dev->domain = iommu_domain_alloc(device->bus)) == NULL) {
        pr_err("[%s] Failed : iommu_domain_alloc.%p\n", __func__, dev->domain);
        return -ENODEV;
    }

    if ((ret = iommu_attach_device(dev->domain, device)) != 0) {
        iommu_domain_free(dev->domain);
        pr_err("[%s] Failed : iommu_attach_device.%d\n", __func__, ret);
        return -ENODEV;
    }
    
    if ((info = (struct iommu_domain_data *)dev->domain->priv) == NULL) {
        iommu_domain_free(dev->domain);
        pr_err("[%s] Failed : info.%p\n",__func__, info);
        return -ENODEV;
    }
    dev->phy_pgd_base = info->phy_pgd_base;
    pr_err("[%s] info.iova.(0x%x, 0x%x) phy_pgd_base.0x%llx\n", __func__,
            info->iova_start,
            info->iova_size,
            dev->phy_pgd_base);

    mutex_init(&dev->pwrlock);
    init_waitqueue_head(&dev->secisp_wait);
    dev->secisp_kthread = kthread_create(secisp_work_fn, NULL, "secispwork");
    if (IS_ERR(dev->secisp_kthread)) {
        pr_err("[%s] Failed : kthread_create.%ld\n", __func__, PTR_ERR(dev->secisp_kthread));
        return PTR_ERR(dev->secisp_kthread);
    }
    wake_up_process(dev->secisp_kthread);

    return 0;
}

int hisi_atfisp_remove(struct platform_device *pdev)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;
    if(dev->domain){
        iommu_domain_free(dev->domain);
        dev->domain = NULL;
    }
    return 0;
}

MODULE_DESCRIPTION("Hisilicon atfisp module");
MODULE_AUTHOR("chentao20@huawei.com");
MODULE_LICENSE("GPL");

/* For debug */
static int debug_ispup(struct hisi_isp_ops *data)
{
    int ret;

    if ((ret = atfisp_subsysup(data)) < 0) {
        pr_err("[%s] Failed : atfisp_subsysup.%d\n", __func__, ret);
        return ret;
    }

    return 0;
}

static int debug_ispdn(struct hisi_isp_ops *data)
{
    int ret;

    if ((ret = atfisp_subsysdown(data)) < 0)
        pr_err("[%s] Failed : atfisp_subsysdown.%d\n", __func__, ret);

    return 0;
}

static int debug_jpegup(struct hisi_isp_ops *data)
{
    int ret;

    if ((ret = atfisp_subsysup(data)) < 0) {
        pr_err("[%s] Failed : atfisp_subsysup.%d\n", __func__, ret);
        return ret;
    }

    if ((ret = atfjpeg_subsysup(data)) < 0) {
        int err_ret;
        pr_err("[%s] Failed : atfjpeg_subsysup.%d\n", __func__, ret);
        if ((err_ret = atfisp_subsysdown(data)) < 0)
            pr_err("[%s] Failed : atfisp_subsysdown.%d\n", __func__, err_ret);
        return ret;
    }

    return 0;
}

static int debug_jpegdn(struct hisi_isp_ops *data)
{
    int ret;

    if ((ret = atfjpeg_subsysdown(data)) < 0)
        pr_err("[%s] Failed : atfjpeg_subsysdown.%d\n", __func__, ret);

    if ((ret = atfisp_subsysdown(data)) < 0)
        pr_err("[%s] Failed : atfisp_subsysdown.%d\n", __func__, ret);

    return 0;
}

static int debug_a7up(struct hisi_isp_ops *data)
{
    return secisp_device_enable();
}

static int debug_a7dn(struct hisi_isp_ops *data)
{
    return secisp_device_disable();
}

static int debug_rscup(struct hisi_isp_ops *data)
{
    int ret;

    if ((ret = hisp_rsctable_init()) < 0) {
        pr_err("[%s] Failed : hisp_rsctable_init.%d\n", __func__, ret);
        return ret;
    }

    return 0;
}

static int debug_rscdn(struct hisi_isp_ops *data)
{
    return 0;
}

struct isp_powerdebug_s {
    char pwrup_cmd[8];
    char pwrdn_cmd[8];
    int (*pwrup_handler)(struct hisi_isp_ops *);
    int (*pwrdn_handler)(struct hisi_isp_ops *);
    char info[32];
} pwrdbg[] = {
    {
	"up", "dn", debug_ispup, debug_ispdn, "isp"}, {
	"up", "dn", debug_jpegup, debug_jpegdn, "jpeg"}, {
	"ld", "de", atfisp_loaddts, NULL, "dts"}, {
	"ld", "de", atfisp_loadfw, NULL, "fw"}, {
	"up", "dn", debug_a7up, debug_a7dn, "a7"}, {
	"up", "dn", debug_rscup, debug_rscdn, "rsc"},};

static ssize_t atfisp_show(struct device *pdev,
			       struct device_attribute *attr, char *buf)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;
	char *s = buf;

    s += snprintf(s, MAX_SIZE, "BoardID: 0x%x\n", dev->boardid);
    if (dev->ispops) {
        s += snprintf(s, MAX_SIZE, "Vote: isp.%d\n", dev->ispops->refs_isp);
        s += snprintf(s, MAX_SIZE, "Vote: jpeg.%d\n", dev->ispops->refs_jpeg);
        s += snprintf(s, MAX_SIZE, "Vote: a7.%d\n", dev->ispops->refs_a7);
        s += snprintf(s, MAX_SIZE, "Load: dts.%d\n", dev->ispops->refs_dts);
        s += snprintf(s, MAX_SIZE, "Load: fw.%d\n", dev->ispops->refs_fw);
    } else {
        s += snprintf(s, MAX_SIZE, "Vote: NULL point!!!\n");
    }

	return (s - buf);
}

static void atfisp_usage(void)
{
	int i = 0;

	pr_info("<Usage: >\n");
	for (i = 0; i < sizeof(pwrdbg) / sizeof(struct isp_powerdebug_s); i ++)
		pr_info("echo <%s>:<%s/%s> > atfisp\n", pwrdbg[i].info, pwrdbg[i].pwrup_cmd, pwrdbg[i].pwrdn_cmd);
}

static ssize_t atfisp_store(struct device *pdev,
				struct device_attribute *attr, const char *buf,
				size_t count)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;
    int i = 0, len = 0, flag = 1;
    char *p = NULL;

    p = memchr(buf, ':', count);
    len = p ? p - buf : count;

    for (i = 0; i < sizeof(pwrdbg) / sizeof(struct isp_powerdebug_s); i++) {
        if (!strncmp(buf, pwrdbg[i].info, len)) {
            p += 1;
			flag = 0;
            if (!strncmp(p, pwrdbg[i].pwrup_cmd,
                strlen(pwrdbg[i].pwrup_cmd)))
				pwrdbg[i].pwrup_handler(dev->ispops);
            else if (!strncmp(p, pwrdbg[i].pwrdn_cmd,
                strlen(pwrdbg[i].pwrdn_cmd)))
				pwrdbg[i].pwrdn_handler(dev->ispops);
            else
				flag = 1;
            break;
        }
    }

	if (flag)
		atfisp_usage();

    return count;
}

static ssize_t regs_show(struct device *pdev,
			       struct device_attribute *attr, char *buf)
{
    struct hisi_atfisp_s *dev = (struct hisi_atfisp_s *)&atfisp_dev;
	char *s = buf;

    s += snprintf(s, MAX_SIZE, "BoardID: 0x%x\n", dev->boardid);

	return (s - buf);
}

static ssize_t regs_store(struct device *pdev,
				struct device_attribute *attr, const char *buf,
				size_t count)
{
    return count;
}
