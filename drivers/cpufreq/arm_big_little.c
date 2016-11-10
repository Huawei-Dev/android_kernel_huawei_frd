/*
 * ARM big.LITTLE Platforms CPUFreq support
 *
 * Copyright (C) 2013 ARM Ltd.
 * Sudeep KarkadaNagesha <sudeep.karkadanagesha@arm.com>
 *
 * Copyright (C) 2013 Linaro.
 * Viresh Kumar <viresh.kumar@linaro.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/clk.h>
#include <linux/cpu.h>
#include <linux/cpufreq.h>
#include <linux/cpumask.h>
#include <linux/export.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of_platform.h>
#include <linux/opp.h>
#include <linux/slab.h>
#include <linux/topology.h>
#include <linux/types.h>
#include <asm/bL_switcher.h>
#include <linux/thermal.h>

#include "arm_big_little.h"

#ifdef CONFIG_HISI_BIG_MAXFREQ_HOTPLUG
#include "cpufreq_governor.h"
#include <linux/sched.h>
#include <linux/sched/rt.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#endif

/* Currently we support only two clusters */
#define A15_CLUSTER	0
#define A7_CLUSTER	1
#define MAX_CLUSTERS	2

#ifdef CONFIG_BL_SWITCHER
static bool bL_switching_enabled;
#define is_bL_switching_enabled()	bL_switching_enabled
#define set_switching_enabled(x)	(bL_switching_enabled = (x))
#else
#define is_bL_switching_enabled()	false
#define set_switching_enabled(x)	do { } while (0)
#endif

#define ACTUAL_FREQ(cluster, freq)	((cluster == A7_CLUSTER) ? freq << 1 : freq)
#define VIRT_FREQ(cluster, freq)	((cluster == A7_CLUSTER) ? freq >> 1 : freq)

#ifdef CONFIG_ARCH_HISI_MAXFREQ
extern void of_target_cpu(int cluster, struct device *cpu_dev);
#endif

static struct cpufreq_arm_bL_ops *arm_bL_ops;
static struct clk *clk[MAX_CLUSTERS];
static struct cpufreq_frequency_table *freq_table[MAX_CLUSTERS + 1];
static atomic_t cluster_usage[MAX_CLUSTERS + 1] = {ATOMIC_INIT(0),
	ATOMIC_INIT(0)};

static unsigned int clk_big_min;	/* (Big) clock frequencies */
static unsigned int clk_little_max;	/* Maximum clock frequency (Little) */

static DEFINE_PER_CPU(unsigned int, physical_cluster);
static DEFINE_PER_CPU(unsigned int, cpu_last_req_freq);

static struct mutex cluster_lock[MAX_CLUSTERS];

#ifdef CONFIG_HISI_BIG_MAXFREQ_HOTPLUG
#define HOTPLUG_UP		1
#define HOTPLUG_DOWN		0
#define THRESHOLD_FREQ		2112000
#define DEF_UP_NR_THRESHOLD	4
#define DEF_DOWN_NR_THRESHOLD	3
#define DEF_UP_DELAY		0
#define DEF_DOWN_DELAY		0
#define DEF_UP_TIMES		1
#define DEF_DOWN_TIMES		2

#define DELAY_MODE		0
#define TIMES_MODE		1

static struct task_struct *hotplug_task;
static struct cpumask hotplug_cpumask;
struct driver_data {
	spinlock_t hotplug_lock;
	struct cpufreq_freqs freqs;
	unsigned int cpu;
	unsigned int up_threshold;
	unsigned int down_threshold;
	unsigned int up_delay;
	unsigned int down_delay;
	unsigned int up_times;
	unsigned int down_times;
	unsigned int req_up_times;
	unsigned int req_down_times;
	unsigned int mode;
	u64 last_up_time;
	u64 last_down_time;
	bool hotplug_up;
	bool hotplug_down;
	bool hotplugged_by_cpufreq;
};

static struct driver_data bL_cpufreq_data;

#define is_bL_hotplug_sysfs_enabled()	false
static bool bL_hotplug_enabled;
#define set_bL_hotplug_enabled(x)	(bL_hotplug_enabled = (x))

bool is_bL_hotplug_enabled()
{
	return bL_hotplug_enabled;
}
EXPORT_SYMBOL_GPL(is_bL_hotplug_enabled);

bool cpufreq_hotplugged(unsigned int cpu)
{
	if (cpumask_test_cpu(cpu, &hotplug_cpumask))
		return bL_cpufreq_data.hotplugged_by_cpufreq;
	else
		return false;
}
EXPORT_SYMBOL_GPL(cpufreq_hotplugged);

#ifdef CONFIG_SCHED_HMP_BOOST
extern int set_hmp_boostpulse(int duration);
#endif

/*********************************************************************
 *                          SYSFS INTERFACE                          *
 *********************************************************************/
static ssize_t show_up_threshold(struct kobject *kobj,
				 struct attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", bL_cpufreq_data.up_threshold);
}

static ssize_t show_down_threshold(struct kobject *kobj,
				 struct attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", bL_cpufreq_data.down_threshold);
}

static ssize_t show_up_delay(struct kobject *kobj,
				 struct attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", bL_cpufreq_data.up_delay);
}

static ssize_t show_down_delay(struct kobject *kobj,
				 struct attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", bL_cpufreq_data.down_delay);
}

static ssize_t show_up_times(struct kobject *kobj,
				 struct attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", bL_cpufreq_data.up_times);
}

static ssize_t show_down_times(struct kobject *kobj,
				 struct attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", bL_cpufreq_data.down_times);
}

static ssize_t show_mode(struct kobject *kobj,
				 struct attribute *attr, char *buf)
{
	if (bL_cpufreq_data.mode == DELAY_MODE)
		return sprintf(buf, "delay\n");
	else if (bL_cpufreq_data.mode == TIMES_MODE)
		return sprintf(buf, "times\n");

	return -EINVAL;
}

static ssize_t store_down_threshold(struct kobject *kobj, struct attribute *attr,
				  const char *buf, size_t count)
{
	int ret;
	unsigned int val;

	ret = kstrtoul(buf, 0, &val);
	if (ret < 0)
		return ret;

	if (val > bL_cpufreq_data.up_threshold)
		return count;

	bL_cpufreq_data.down_threshold = val;
	return count;
}

static ssize_t store_up_threshold(struct kobject *kobj, struct attribute *attr,
				  const char *buf, size_t count)
{
	int ret;
	unsigned int val;

	ret = kstrtoul(buf, 0, &val);
	if (ret < 0)
		return ret;

	if (val < bL_cpufreq_data.down_threshold)
		return count;

	bL_cpufreq_data.up_threshold = val;
	return count;
}

static ssize_t store_down_delay(struct kobject *kobj, struct attribute *attr,
				  const char *buf, size_t count)
{
	int ret;
	unsigned int val;

	ret = kstrtoul(buf, 0, &val);
	if (ret < 0)
		return ret;

	bL_cpufreq_data.down_delay = val;
	return count;
}

static ssize_t store_up_delay(struct kobject *kobj, struct attribute *attr,
				  const char *buf, size_t count)
{
	int ret;
	unsigned int val;

	ret = kstrtoul(buf, 0, &val);
	if (ret < 0)
		return ret;

	bL_cpufreq_data.up_delay = val;
	return count;
}

static ssize_t store_down_times(struct kobject *kobj, struct attribute *attr,
				  const char *buf, size_t count)
{
	int ret;
	unsigned int val;

	ret = kstrtoul(buf, 0, &val);
	if (ret < 0)
		return ret;

	bL_cpufreq_data.down_times = val;
	return count;
}

static ssize_t store_up_times(struct kobject *kobj, struct attribute *attr,
				  const char *buf, size_t count)
{
	int ret;
	unsigned int val;

	ret = kstrtoul(buf, 0, &val);
	if (ret < 0)
		return ret;

	bL_cpufreq_data.up_times = val;
	return count;
}

static ssize_t store_mode(struct kobject *kobj, struct attribute *attr,
				  const char *buf, size_t count)
{
	int ret;
	char mode[16];

	ret = sscanf(buf, "%15s", mode);
	if (ret != 1)
		return -EINVAL;

	if (!strncasecmp(mode, "delay", 6)) {
		bL_cpufreq_data.mode = DELAY_MODE;
		ret = 0;
	} else if (!strncasecmp(mode, "times", 6)) {
		bL_cpufreq_data.mode = TIMES_MODE;
		ret = 0;
	}

	if (ret)
		return ret;
	else
		return count;
}

define_one_global_rw(up_threshold);
define_one_global_rw(down_threshold);
define_one_global_rw(up_delay);
define_one_global_rw(down_delay);
define_one_global_rw(up_times);
define_one_global_rw(down_times);
define_one_global_rw(mode);
#endif

static inline int raw_cpu_to_cluster(int cpu)
{
	return topology_physical_package_id(cpu);
}

static inline int cpu_to_cluster(int cpu)
{
	return is_bL_switching_enabled() ?
		MAX_CLUSTERS : raw_cpu_to_cluster(cpu);
}

static unsigned int find_cluster_maxfreq(int cluster)
{
	int j;
	u32 max_freq = 0, cpu_freq;

	for_each_online_cpu(j) {
		cpu_freq = per_cpu(cpu_last_req_freq, j);

		if ((cluster == per_cpu(physical_cluster, j)) &&
				(max_freq < cpu_freq))
			max_freq = cpu_freq;
	}

	pr_debug("%s: cluster: %d, max freq: %d\n", __func__, cluster,
			max_freq);

	return max_freq;
}

static unsigned int clk_get_cpu_rate(unsigned int cpu)
{
	u32 cur_cluster = per_cpu(physical_cluster, cpu);
	u32 rate = clk_get_rate(clk[cur_cluster]) / 1000;

	/* For switcher we use virtual A15 clock rates */
	if (is_bL_switching_enabled())
		rate = VIRT_FREQ(cur_cluster, rate);

	pr_debug("%s: cpu: %d, cluster: %d, freq: %u\n", __func__, cpu,
			cur_cluster, rate);

	return rate;
}

static unsigned int bL_cpufreq_get_rate(unsigned int cpu)
{
	if (is_bL_switching_enabled()) {
		pr_debug("%s: freq: %d\n", __func__, per_cpu(cpu_last_req_freq,
					cpu));

		return per_cpu(cpu_last_req_freq, cpu);
	} else {
		return clk_get_cpu_rate(cpu);
	}
}

static int
bL_cpufreq_set_rate(u32 cpu, u32 old_cluster, u32 new_cluster, u32 rate)
{
	u32 new_rate, prev_rate;
	int ret;
	bool bLs = is_bL_switching_enabled();

	mutex_lock(&cluster_lock[new_cluster]);

	if (bLs) {
		prev_rate = per_cpu(cpu_last_req_freq, cpu);
		per_cpu(cpu_last_req_freq, cpu) = rate;
		per_cpu(physical_cluster, cpu) = new_cluster;

		new_rate = find_cluster_maxfreq(new_cluster);
		new_rate = ACTUAL_FREQ(new_cluster, new_rate);
	} else {
		new_rate = rate;
	}

	pr_debug("%s: cpu: %d, old cluster: %d, new cluster: %d, freq: %d\n",
			__func__, cpu, old_cluster, new_cluster, new_rate);

	ret = clk_set_rate(clk[new_cluster], new_rate * 1000);
	if (WARN_ON(ret)) {
		pr_err("clk_set_rate failed: %d, new cluster: %d\n", ret,
				new_cluster);
		if (bLs) {
			per_cpu(cpu_last_req_freq, cpu) = prev_rate;
			per_cpu(physical_cluster, cpu) = old_cluster;
		}

		mutex_unlock(&cluster_lock[new_cluster]);

		return ret;
	}

	mutex_unlock(&cluster_lock[new_cluster]);

	/* Recalc freq for old cluster when switching clusters */
	if (old_cluster != new_cluster) {
		pr_debug("%s: cpu: %d, old cluster: %d, new cluster: %d\n",
				__func__, cpu, old_cluster, new_cluster);

		/* Switch cluster */
		bL_switch_request(cpu, new_cluster);

		mutex_lock(&cluster_lock[old_cluster]);

		/* Set freq of old cluster if there are cpus left on it */
		new_rate = find_cluster_maxfreq(old_cluster);
		new_rate = ACTUAL_FREQ(old_cluster, new_rate);

		if (new_rate) {
			pr_debug("%s: Updating rate of old cluster: %d, to freq: %d\n",
					__func__, old_cluster, new_rate);

			if (clk_set_rate(clk[old_cluster], new_rate * 1000))
				pr_err("%s: clk_set_rate failed: %d, old cluster: %d\n",
						__func__, ret, old_cluster);
		}
		mutex_unlock(&cluster_lock[old_cluster]);
	}

	return 0;
}

#ifdef CONFIG_HISI_BIG_MAXFREQ_HOTPLUG
static int cpu_hotplug(int cpuid, int onoff)
{
	struct device *dev = get_cpu_device(cpuid);
	struct cpu *cpu = container_of(dev, struct cpu, dev);
	int from_nid, to_nid;
	int ret;

#ifdef CONFIG_ARCH_HISI
	hisi_cpu_hotplug_lock();
#endif

	cpu_hotplug_driver_lock();
	switch (onoff) {
	case 0:
		cpuidle_pause();

		ret = cpu_down(cpuid);
		cpuidle_resume();
		kick_all_cpus_sync();
		if (!ret)
			kobject_uevent(&dev->kobj, KOBJ_OFFLINE);
		break;
	case 1:
		from_nid = cpu_to_node(cpuid);
		cpuidle_pause();
		ret = cpu_up(cpuid);
		cpuidle_resume();
		kick_all_cpus_sync();

		/*
		 * When hot adding memory to memoryless node and enabling a cpu
		 * on the node, node number of the cpu may internally change.
		 */
		to_nid = cpu_to_node(cpuid);
		if (from_nid != to_nid)
			change_cpu_under_node(cpu, from_nid, to_nid);

		if (!ret)
			kobject_uevent(&dev->kobj, KOBJ_ONLINE);
		break;
	default:
		ret = -EINVAL;
	}
	cpu_hotplug_driver_unlock();

#ifdef CONFIG_ARCH_HISI
	hisi_cpu_hotplug_unlock();
#endif

	return ret;
}

static int cpufreq_bL_hotplug_task(void *data)
{
	struct cpufreq_policy policy;
	int ret;
	unsigned long flags;
	struct driver_data tmp_data;
	unsigned int cpu, value;

	while (1) {
		set_current_state(TASK_INTERRUPTIBLE);
		spin_lock_irqsave(&(bL_cpufreq_data.hotplug_lock), flags);

		if ((!(bL_cpufreq_data.hotplug_up)) && (!(bL_cpufreq_data.hotplug_down))) {
			spin_unlock_irqrestore(&(bL_cpufreq_data.hotplug_lock), flags);
			schedule();

			if (kthread_should_stop())
				break;

			spin_lock_irqsave(&(bL_cpufreq_data.hotplug_lock), flags);
		}

		set_current_state(TASK_RUNNING);
		tmp_data = bL_cpufreq_data;
		bL_cpufreq_data.hotplug_down = false;
		bL_cpufreq_data.hotplug_up = false;
		spin_unlock_irqrestore(&(bL_cpufreq_data.hotplug_lock), flags);

		ret = cpufreq_get_policy(&policy, tmp_data.cpu);
		if (ret)
			continue;

		if (tmp_data.hotplug_down) {
			for_each_cpu(cpu, &hotplug_cpumask) {
				if (cpu_online(cpu))
					cpu_hotplug(cpu, HOTPLUG_DOWN);
			}
			bL_cpufreq_data.hotplugged_by_cpufreq = true;
		}

		if (tmp_data.freqs.old != tmp_data.freqs.new) {
			cpufreq_notify_transition(&policy, &(tmp_data.freqs), CPUFREQ_PRECHANGE);
			ret = bL_cpufreq_set_rate(tmp_data.cpu, A7_CLUSTER, A7_CLUSTER, tmp_data.freqs.new);
			if (ret)
				continue;
			policy.cur = tmp_data.freqs.new;
			cpufreq_notify_transition(&policy, &(tmp_data.freqs), CPUFREQ_POSTCHANGE);
		}

		if (tmp_data.hotplug_up) {
			/* need delay to wait freq down */
			if (tmp_data.freqs.old > THRESHOLD_FREQ)
				usleep_range(1500, 2500);

			if (bL_cpufreq_get_rate(tmp_data.cpu) <= THRESHOLD_FREQ) {
				for_each_cpu(cpu, &hotplug_cpumask) {
					if (cpu_is_offline(cpu))
						cpu_hotplug(cpu, HOTPLUG_UP);
				}
				bL_cpufreq_data.hotplugged_by_cpufreq = false;
#ifdef CONFIG_SCHED_HMP_BOOST
				set_hmp_boostpulse(20000);
#endif
			}
		}
	}
	return 0;
}
#endif

/* Set clock frequency */
static int bL_cpufreq_set_target(struct cpufreq_policy *policy,
		unsigned int target_freq, unsigned int relation)
{
	struct cpufreq_freqs freqs;
	u32 cpu = policy->cpu, freq_tab_idx, cur_cluster, new_cluster,
	    actual_cluster;
	int ret = 0;
#ifdef CONFIG_HISI_BIG_MAXFREQ_HOTPLUG
	unsigned long flags;
	unsigned int nr_running;
	u64 now;
#endif

	cur_cluster = cpu_to_cluster(cpu);
	new_cluster = actual_cluster = per_cpu(physical_cluster, cpu);

	freqs.old = bL_cpufreq_get_rate(cpu);

	/* Determine valid target frequency using freq_table */
	ret = cpufreq_frequency_table_target(policy, freq_table[cur_cluster],
			target_freq, relation, &freq_tab_idx);
	if (ret)
		return ret;

	freqs.new = freq_table[cur_cluster][freq_tab_idx].frequency;

	pr_debug("%s: cpu: %d, cluster: %d, oldfreq: %d, target freq: %d, new freq: %d\n",
			__func__, cpu, cur_cluster, freqs.old, target_freq,
			freqs.new);

	if (is_bL_switching_enabled()) {
		if ((actual_cluster == A15_CLUSTER) &&
				(freqs.new < clk_big_min)) {
			new_cluster = A7_CLUSTER;
		} else if ((actual_cluster == A7_CLUSTER) &&
				(freqs.new > clk_little_max)) {
			new_cluster = A15_CLUSTER;
		}
	}

#ifdef CONFIG_HISI_BIG_MAXFREQ_HOTPLUG
	if (cur_cluster == A7_CLUSTER) {
		if (!is_bL_hotplug_enabled())
			goto normal_set;

		if (!hotplug_task)
			goto normal_set;

		/* we assume 4 cpus in THRESHOLD_FREQ is the performance mode */
		if (!strncasecmp(policy->governor, "performance", CPUFREQ_NAME_LEN)) {
			freqs.new = min(freqs.new, THRESHOLD_FREQ);
			goto normal_set;
		}

		nr_running = cpu_nr_runnings(policy->cpus);
		now = ktime_to_us(ktime_get());

		if (nr_running < bL_cpufreq_data.down_threshold) {
			/* if has been hotpluged down, change frequency only */
			if (bL_cpufreq_data.hotplugged_by_cpufreq)
				goto print_info;

			/* if any tasks on the cpus, we don't hotplug them down */
			if (cpu_nr_runnings(&hotplug_cpumask) > 0) {
				freqs.new = min(freqs.new, THRESHOLD_FREQ);
				goto clear_data;
			}

			if (bL_cpufreq_data.mode == DELAY_MODE) {
				if (now - bL_cpufreq_data.last_up_time < bL_cpufreq_data.up_delay)
					goto print_info;
			} else {
				bL_cpufreq_data.req_down_times++;
				if (bL_cpufreq_data.req_down_times < bL_cpufreq_data.down_times)
					goto print_info;
			}

			bL_cpufreq_data.last_down_time = now;
			bL_cpufreq_data.req_up_times = 0;
			bL_cpufreq_data.req_down_times = 0;

			spin_lock_irqsave(&(bL_cpufreq_data.hotplug_lock), flags);
			bL_cpufreq_data.cpu = cpu;
			bL_cpufreq_data.freqs.new = policy->max;
			bL_cpufreq_data.freqs.old = freqs.old;
			bL_cpufreq_data.hotplug_down = true;
			spin_unlock_irqrestore(&(bL_cpufreq_data.hotplug_lock), flags);
		} else if (nr_running > bL_cpufreq_data.up_threshold) {
			if (!bL_cpufreq_data.hotplugged_by_cpufreq) {
				freqs.new = min(freqs.new, THRESHOLD_FREQ);
				goto print_info;
			}

			if (bL_cpufreq_data.mode == DELAY_MODE) {
				if (now - bL_cpufreq_data.last_down_time < bL_cpufreq_data.down_delay) {
					freqs.new = THRESHOLD_FREQ;
					goto print_info;
				}
			} else {
				bL_cpufreq_data.req_up_times++;
				if (bL_cpufreq_data.req_up_times < bL_cpufreq_data.up_times)
					goto print_info;
			}

			bL_cpufreq_data.last_up_time = now;
			bL_cpufreq_data.req_up_times = 0;
			bL_cpufreq_data.req_down_times = 0;

			spin_lock_irqsave(&(bL_cpufreq_data.hotplug_lock), flags);
			bL_cpufreq_data.cpu = cpu;
			bL_cpufreq_data.freqs.new = THRESHOLD_FREQ;
			bL_cpufreq_data.freqs.old = freqs.old;
			bL_cpufreq_data.hotplug_up = true;
			spin_unlock_irqrestore(&(bL_cpufreq_data.hotplug_lock), flags);
		} else {
			if (!bL_cpufreq_data.hotplugged_by_cpufreq)
				freqs.new = min(freqs.new, THRESHOLD_FREQ);

			goto clear_data;
		}

		pr_debug("old:%d, new:%d, nr:%d\n", bL_cpufreq_data.freqs.old, bL_cpufreq_data.freqs.new, nr_running);

		wake_up_process(hotplug_task);

		return 0;

clear_data:
		bL_cpufreq_data.req_up_times = 0;
		bL_cpufreq_data.req_down_times = 0;
print_info:
		pr_debug("old:%d, new:%d, nr:%d\n", freqs.old, freqs.new, nr_running);
	}
normal_set:
#endif

#ifdef CONFIG_HISI_IPA_THERMAL
	freqs.new = ipa_freq_limit(cur_cluster, freqs.new);
#endif

	if (freqs.old == freqs.new)
		return 0;

	cpufreq_notify_transition(policy, &freqs, CPUFREQ_PRECHANGE);
	ret = bL_cpufreq_set_rate(cpu, actual_cluster, new_cluster, freqs.new);
	if (ret)
		return ret;

	policy->cur = freqs.new;

	cpufreq_notify_transition(policy, &freqs, CPUFREQ_POSTCHANGE);

	return ret;
}

static inline u32 get_table_count(struct cpufreq_frequency_table *table)
{
	u32 count;

	for (count = 0; table[count].frequency != CPUFREQ_TABLE_END; count++)
		;

	return count;
}

/* get the minimum frequency in the cpufreq_frequency_table */
static inline u32 get_table_min(struct cpufreq_frequency_table *table)
{
	struct cpufreq_frequency_table *pos;
	uint32_t min_freq = ~0;
	cpufreq_for_each_entry(pos, table)
		if (pos->frequency < min_freq)
			min_freq = pos->frequency;
	return min_freq;
}

/* get the maximum frequency in the cpufreq_frequency_table */
static inline u32 get_table_max(struct cpufreq_frequency_table *table)
{
	struct cpufreq_frequency_table *pos;
	uint32_t max_freq = 0;
	cpufreq_for_each_entry(pos, table)
		if (pos->frequency > max_freq)
			max_freq = pos->frequency;
	return max_freq;
}

static int merge_cluster_tables(void)
{
	int i, j, k = 0, count = 1;
	struct cpufreq_frequency_table *table;

	for (i = 0; i < MAX_CLUSTERS; i++)
		count += get_table_count(freq_table[i]);

	table = kzalloc(sizeof(*table) * count, GFP_KERNEL);
	if (!table)
		return -ENOMEM;

	freq_table[MAX_CLUSTERS] = table;

	/* Add in reverse order to get freqs in increasing order */
	for (i = MAX_CLUSTERS - 1; i >= 0; i--) {
		for (j = 0; freq_table[i][j].frequency != CPUFREQ_TABLE_END;
				j++) {
			table[k].frequency = VIRT_FREQ(i,
					freq_table[i][j].frequency);
			pr_debug("%s: index: %d, freq: %d\n", __func__, k,
					table[k].frequency);
			k++;
		}
	}

	table[k].driver_data = k;
	table[k].frequency = CPUFREQ_TABLE_END;

	pr_debug("%s: End, table: %p, count: %d\n", __func__, table, k);

	return 0;
}

static void _put_cluster_clk_and_freq_table(struct device *cpu_dev)
{
	u32 cluster = raw_cpu_to_cluster(cpu_dev->id);

	if (!freq_table[cluster])
		return;

	clk_put(clk[cluster]);
	opp_free_cpufreq_table(cpu_dev, &freq_table[cluster]);
	dev_dbg(cpu_dev, "%s: cluster: %d\n", __func__, cluster);
}

static void put_cluster_clk_and_freq_table(struct device *cpu_dev)
{
	u32 cluster = cpu_to_cluster(cpu_dev->id);
	int i;

	if (atomic_dec_return(&cluster_usage[cluster]))
		return;

	if (cluster < MAX_CLUSTERS)
		return _put_cluster_clk_and_freq_table(cpu_dev);

	for_each_present_cpu(i) {
		struct device *cdev = get_cpu_device(i);
		if (!cdev) {
			pr_err("%s: failed to get cpu%d device\n", __func__, i);
			return;
		}

		_put_cluster_clk_and_freq_table(cdev);
	}

	/* free virtual table */
	kfree(freq_table[cluster]);
}

static int _get_cluster_clk_and_freq_table(struct device *cpu_dev)
{
	u32 cluster = raw_cpu_to_cluster(cpu_dev->id);
	char name[14] = "cpu-cluster.X";
	int ret;
#ifdef CONFIG_HISI_BIG_MAXFREQ_HOTPLUG
	u32 clk_big_max;
#endif

	if (freq_table[cluster])
		return 0;

	ret = arm_bL_ops->init_opp_table(cpu_dev);
	if (ret) {
		dev_err(cpu_dev, "%s: init_opp_table failed, cpu: %d, err: %d\n",
				__func__, cpu_dev->id, ret);
		goto out;
	}

#ifdef CONFIG_ARCH_HISI_MAXFREQ
	of_target_cpu(cluster, cpu_dev);
#endif

	ret = opp_init_cpufreq_table(cpu_dev, &freq_table[cluster]);
	if (ret) {
		dev_err(cpu_dev, "%s: failed to init cpufreq table, cpu: %d, err: %d\n",
				__func__, cpu_dev->id, ret);
		goto out;
	}

	/*add here to separate dallas c20 and c30*/
#ifdef CONFIG_HISI_BIG_MAXFREQ_HOTPLUG
	if (cluster == A7_CLUSTER) {
		clk_big_max = get_table_max(freq_table[cluster]);
		if (clk_big_max > THRESHOLD_FREQ)
			set_bL_hotplug_enabled(true);
		else
			set_bL_hotplug_enabled(false);
	}
#endif

	name[12] = cluster + '0';

	clk[cluster] = clk_get_sys(NULL, name);
	if (!IS_ERR(clk[cluster])) {
		dev_dbg(cpu_dev, "%s: clk: %p & freq table: %p, cluster: %d\n",
				__func__, clk[cluster], freq_table[cluster],
				cluster);
		return 0;
	}

	dev_err(cpu_dev, "%s: Failed to get clk for cpu: %d, cluster: %d\n",
			__func__, cpu_dev->id, cluster);
	ret = PTR_ERR(clk[cluster]);
	opp_free_cpufreq_table(cpu_dev, &freq_table[cluster]);

out:
	dev_err(cpu_dev, "%s: Failed to get data for cluster: %d\n", __func__,
			cluster);
	return ret;
}

static int get_cluster_clk_and_freq_table(struct device *cpu_dev)
{
	u32 cluster = cpu_to_cluster(cpu_dev->id);
	int i, ret;

	if (atomic_inc_return(&cluster_usage[cluster]) != 1)
		return 0;

	if (cluster < MAX_CLUSTERS) {
		ret = _get_cluster_clk_and_freq_table(cpu_dev);
		if (ret)
			atomic_dec(&cluster_usage[cluster]);
		return ret;
	}

	/*
	 * Get data for all clusters and fill virtual cluster with a merge of
	 * both
	 */
	for_each_present_cpu(i) {
		struct device *cdev = get_cpu_device(i);
		if (!cdev) {
			pr_err("%s: failed to get cpu%d device\n", __func__, i);
			return -ENODEV;
		}

		ret = _get_cluster_clk_and_freq_table(cdev);
		if (ret)
			goto put_clusters;
	}

	ret = merge_cluster_tables();
	if (ret)
		goto put_clusters;

	/* Assuming 2 cluster, set clk_big_min and clk_little_max */
	clk_big_min = get_table_min(freq_table[0]);
	/*lint -e666*/
	clk_little_max = VIRT_FREQ(1, get_table_max(freq_table[1]));
	/*lint +e666*/

	pr_debug("%s: cluster: %d, clk_big_min: %d, clk_little_max: %d\n",
			__func__, cluster, clk_big_min, clk_little_max);

	return 0;

put_clusters:
	for_each_present_cpu(i) {
		struct device *cdev = get_cpu_device(i);
		if (!cdev) {
			pr_err("%s: failed to get cpu%d device\n", __func__, i);
			ret = -ENODEV;
		} else {
			_put_cluster_clk_and_freq_table(cdev);
		}
	}

	atomic_dec(&cluster_usage[cluster]);

	return ret;
}

/* Per-CPU initialization */
static int bL_cpufreq_init(struct cpufreq_policy *policy)
{
	u32 cur_cluster = cpu_to_cluster(policy->cpu);
	struct device *cpu_dev;
	int ret;

	cpu_dev = get_cpu_device(policy->cpu);
	if (!cpu_dev) {
		pr_err("%s: failed to get cpu%d device\n", __func__,
				policy->cpu);
		return -ENODEV;
	}

	ret = get_cluster_clk_and_freq_table(cpu_dev);
	if (ret)
		return ret;

	ret = cpufreq_table_validate_and_show(policy, freq_table[cur_cluster]);
	if (ret) {
		dev_err(cpu_dev, "CPU %d, cluster: %d invalid freq table\n",
				policy->cpu, cur_cluster);
		put_cluster_clk_and_freq_table(cpu_dev);
		return ret;
	}

	if (cur_cluster < MAX_CLUSTERS) {
		int cpu;

		cpumask_copy(policy->cpus, topology_core_cpumask(policy->cpu));

		for_each_cpu(cpu, policy->cpus)
			per_cpu(physical_cluster, cpu) = cur_cluster;
	} else {
		/* Assumption: during init, we are always running on A15 */
		per_cpu(physical_cluster, policy->cpu) = A15_CLUSTER;
	}

	if (arm_bL_ops->get_transition_latency)
		policy->cpuinfo.transition_latency =
			arm_bL_ops->get_transition_latency(cpu_dev);
	else
		policy->cpuinfo.transition_latency = CPUFREQ_ETERNAL;

	if (is_bL_switching_enabled())
		per_cpu(cpu_last_req_freq, policy->cpu) = clk_get_cpu_rate(policy->cpu);

	dev_info(cpu_dev, "%s: CPU %d initialized\n", __func__, policy->cpu);
	return 0;
}

static int bL_cpufreq_exit(struct cpufreq_policy *policy)
{
	struct device *cpu_dev;

	cpu_dev = get_cpu_device(policy->cpu);
	if (!cpu_dev) {
		pr_err("%s: failed to get cpu%d device\n", __func__,
				policy->cpu);
		return -ENODEV;
	}

	put_cluster_clk_and_freq_table(cpu_dev);
	dev_dbg(cpu_dev, "%s: Exited, cpu: %d\n", __func__, policy->cpu);

	return 0;
}

static struct cpufreq_driver bL_cpufreq_driver = {
	.name			= "arm-big-little",
	.flags			= CPUFREQ_STICKY |
					CPUFREQ_HAVE_GOVERNOR_PER_POLICY |
					CPUFREQ_NEED_INITIAL_FREQ_CHECK,
	.verify			= cpufreq_generic_frequency_table_verify,
	.target			= bL_cpufreq_set_target,
	.get			= bL_cpufreq_get_rate,
	.init			= bL_cpufreq_init,
	.attr			= cpufreq_generic_attr,
};

static int bL_cpufreq_switcher_notifier(struct notifier_block *nfb,
					unsigned long action, void *_arg)
{
	pr_debug("%s: action: %ld\n", __func__, action);

	switch (action) {
	case BL_NOTIFY_PRE_ENABLE:
	case BL_NOTIFY_PRE_DISABLE:
		cpufreq_unregister_driver(&bL_cpufreq_driver);
		break;

	case BL_NOTIFY_POST_ENABLE:
		set_switching_enabled(true);
		cpufreq_register_driver(&bL_cpufreq_driver);
		break;

	case BL_NOTIFY_POST_DISABLE:
		set_switching_enabled(false);
		cpufreq_register_driver(&bL_cpufreq_driver);
		break;

	default:
		return NOTIFY_DONE;
	}

	return NOTIFY_OK;
}

static struct notifier_block bL_switcher_notifier = {
	.notifier_call = bL_cpufreq_switcher_notifier,
};

#ifdef CONFIG_HISI_BIG_MAXFREQ_HOTPLUG
void bL_hotplug_sysfs_create()
{
	int ret;

	if (!is_bL_hotplug_sysfs_enabled())
		return;

	ret = cpufreq_sysfs_create_file(&down_threshold.attr);
	if (ret)
		goto err_create_sysfs;

	ret = cpufreq_sysfs_create_file(&up_threshold.attr);
	if (ret)
		goto err_create_sysfs;

	ret = cpufreq_sysfs_create_file(&down_delay.attr);
	if (ret)
		goto err_create_sysfs;

	ret = cpufreq_sysfs_create_file(&up_delay.attr);
	if (ret)
		goto err_create_sysfs;

	ret = cpufreq_sysfs_create_file(&down_times.attr);
	if (ret)
		goto err_create_sysfs;

	ret = cpufreq_sysfs_create_file(&up_times.attr);
	if (ret)
		goto err_create_sysfs;

	ret = cpufreq_sysfs_create_file(&mode.attr);
	if (ret)
		goto err_create_sysfs;

	return;

err_create_sysfs:
	pr_err("%s: cannot create sysfs file\n", __func__);
}

void bL_cpufreq_hotplug_init()
{
	int ret;
	struct sched_param param = { .sched_priority = MAX_RT_PRIO - 21};
	struct driver_data *data;

	hotplug_task = kthread_create(cpufreq_bL_hotplug_task, NULL, "bLhotplug");
	if (IS_ERR(hotplug_task)) {
		pr_err("%s: hotplug task create fail\n", __func__);
		return;
	}

	data = &bL_cpufreq_data;

	spin_lock_init(&data->hotplug_lock);
	data->up_threshold = DEF_UP_NR_THRESHOLD;
	data->down_threshold = DEF_DOWN_NR_THRESHOLD;
	data->up_delay = DEF_UP_DELAY;
	data->down_delay = DEF_DOWN_DELAY;
	data->up_times = DEF_UP_TIMES;
	data->down_times = DEF_DOWN_TIMES;
	data->req_up_times = 0;
	data->req_down_times = 0;
	data->last_up_time = ktime_to_us(ktime_get());
	data->last_down_time = data->last_up_time;
	data->mode = TIMES_MODE;
	data->hotplug_up = false,
	data->hotplug_down = false,
	data->hotplugged_by_cpufreq = false,

	/* physical cpu 4&5(logical 6&7) to be hotplugged up and down */
	cpumask_set_cpu(6, &hotplug_cpumask);
	cpumask_set_cpu(7, &hotplug_cpumask);

	bL_hotplug_sysfs_create();

	sched_setscheduler_nocheck(hotplug_task, SCHED_FIFO, &param);
	get_task_struct(hotplug_task);
	/* NB: wake up so the thread does not look hung to the freezer */
	wake_up_process(hotplug_task);
}

void bL_cpufreq_hotplug_exit()
{
	kthread_stop(hotplug_task);
	put_task_struct(hotplug_task);
}
#endif

int bL_cpufreq_register(struct cpufreq_arm_bL_ops *ops)
{
	int ret, i;

	if (arm_bL_ops) {
		pr_debug("%s: Already registered: %s, exiting\n", __func__,
				arm_bL_ops->name);
		return -EBUSY;
	}

	if (!ops || !strlen(ops->name) || !ops->init_opp_table) {
		pr_err("%s: Invalid arm_bL_ops, exiting\n", __func__);
		return -ENODEV;
	}

	arm_bL_ops = ops;

	ret = bL_switcher_get_enabled();
	set_switching_enabled(ret);

	for (i = 0; i < MAX_CLUSTERS; i++)
		mutex_init(&cluster_lock[i]);

	ret = cpufreq_register_driver(&bL_cpufreq_driver);
	if (ret) {
		pr_info("%s: Failed registering platform driver: %s, err: %d\n",
				__func__, ops->name, ret);
		arm_bL_ops = NULL;
	} else {
		ret = bL_switcher_register_notifier(&bL_switcher_notifier);
		if (ret) {
			cpufreq_unregister_driver(&bL_cpufreq_driver);
			arm_bL_ops = NULL;
		} else {
#ifdef CONFIG_HISI_BIG_MAXFREQ_HOTPLUG
			bL_cpufreq_hotplug_init();
#endif
			pr_info("%s: Registered platform driver: %s\n",
					__func__, ops->name);
		}
	}

	bL_switcher_put_enabled();
	return ret;
}
EXPORT_SYMBOL_GPL(bL_cpufreq_register);

void bL_cpufreq_unregister(struct cpufreq_arm_bL_ops *ops)
{
	if (arm_bL_ops != ops) {
		pr_err("%s: Registered with: %s, can't unregister, exiting\n",
				__func__, arm_bL_ops->name);
		return;
	}

	bL_switcher_get_enabled();
	bL_switcher_unregister_notifier(&bL_switcher_notifier);
	cpufreq_unregister_driver(&bL_cpufreq_driver);
#ifdef CONFIG_HISI_BIG_MAXFREQ_HOTPLUG
	bL_cpufreq_hotplug_exit();
#endif
	bL_switcher_put_enabled();
	pr_info("%s: Un-registered platform driver: %s\n", __func__,
			arm_bL_ops->name);
	arm_bL_ops = NULL;
}
EXPORT_SYMBOL_GPL(bL_cpufreq_unregister);

MODULE_AUTHOR("Viresh Kumar <viresh.kumar@linaro.org>");
MODULE_DESCRIPTION("Generic ARM big LITTLE cpufreq driver");
MODULE_LICENSE("GPL v2");
