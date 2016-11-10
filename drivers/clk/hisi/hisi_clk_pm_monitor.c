#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/notifier.h>
#include <linux/suspend.h>
#include <linux/list.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/clk-provider.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 13, 0))
#include <linux/clk-private.h>
#endif
#include <linux/clkdev.h>

static HLIST_HEAD(clocks_pm);
static DEFINE_MUTEX(clocks_pm_lock);

static void pmclk_monitor_enable(void)
{
	struct clk *pmclk;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 13, 0))
	struct clk *clk;
#else
	struct clk_core *clk;
#endif
	mutex_lock(&clocks_pm_lock);
	hlist_for_each_entry(clk, &clocks_pm, child_node) {
		pmclk = __clk_lookup(__clk_get_name(clk));
		if (IS_ERR_OR_NULL(pmclk)) {
			pr_err("%s get failed!\n", __clk_get_name(pmclk));
			return;
		}
		if (__clk_get_enable_count(pmclk) > 0) {
			pr_err("The Driver Who use the %s is Exception! cnt=%d Please Check the Drv\n",
					__clk_get_name(pmclk), __clk_get_enable_count(pmclk));
			WARN_ON(1);
		}
	}
	mutex_unlock(&clocks_pm_lock);
}

static int
clk_pm_callback(struct notifier_block *nb,
			unsigned long action, void *ptr)
{
	switch (action) {
	case PM_SUSPEND_PREPARE:
		break;
	case PM_POST_SUSPEND:
		pmclk_monitor_enable();
		break;
	default:
		return NOTIFY_DONE;
	}

	return NOTIFY_OK;
}

static void pmclk_add(struct clk *clk)
{
	mutex_lock(&clocks_pm_lock);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 13, 0))
	hlist_add_head(&clk->child_node, &clocks_pm);
#else
	hlist_add_head(&clk->core->child_node, &clocks_pm);
#endif
	mutex_unlock(&clocks_pm_lock);
}

static int hisi_pmclk_monitor_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct clk *clk;
	int ret, num, i;

	if (!np) {
		pr_err("could not find pmclk node!\n");
		return -EINVAL;
	}
	ret = of_property_read_u32(np, "hisi-pmclk-num", &num);
	if (ret) {
		pr_err("cound not find hisi-pmclk-num property!\n");
		return -EINVAL;
	}
	for (i = 0; i < num; i++) {
		clk = of_clk_get(np, i);
		if (IS_ERR_OR_NULL(clk)) {
			pr_err("%s:Failed to get pmclk\n", __func__);
		}
		pmclk_add(clk);
	}
	pm_notifier(clk_pm_callback, 0);
	pr_err("pm clk monitor setup!\n");

	return ret;
}

static int hisi_pmclk_monitor_remove(struct platform_device *pdev)
{
	return 0;
}

static struct of_device_id hisi_pmclk_of_match[] = {
	{ .compatible = "hisilicon,pm-clk-monitor" },
	{ },
};
MODULE_DEVICE_TABLE(of, hisi_pmclk_of_match);

static struct platform_driver hisi_pmclk_monitor_driver = {
	.probe          = hisi_pmclk_monitor_probe,
	.remove         = hisi_pmclk_monitor_remove,
	.driver         = {
		.name   = "hisi_pmclk",
		.owner  = THIS_MODULE,
		.of_match_table = of_match_ptr(hisi_pmclk_of_match),
	},
};

static int __init hisi_pmclk_monitor_init(void)
{
	return platform_driver_register(&hisi_pmclk_monitor_driver);
}
fs_initcall(hisi_pmclk_monitor_init);
