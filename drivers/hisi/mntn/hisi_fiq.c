#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/smp.h>
#include <linux/kmsg_dump.h>
#ifdef CONFIG_HISI_BB
#include <linux/hisi/rdr_hisi_platform.h>
#endif

#ifdef CONFIG_HISI_CORESIGHT_TRACE
#include <linux/coresight.h>
#endif

#include <asm/stacktrace.h>
#include <asm/exception.h>
#include <asm/system_misc.h>
#include <asm/cacheflush.h>

#ifdef CONFIG_HISI_DDRC_SEC
#include <linux/hisi/hisi_ddr.h>
#endif

void hisi_mntn_inform(void)
{
#ifdef CONFIG_HISI_DDRC_SEC
	dmss_ipi_handler();
#endif
}

asmlinkage void fiq_dump(struct pt_regs *regs, unsigned int esr)
{
	pr_crit("fiq_dump begin!\n");

#ifdef CONFIG_HISI_DDRC_SEC
	dmss_fiq_handler();
#endif
	console_verbose();
	show_regs(regs);
	dump_stack();
	smp_send_stop();
#ifdef CONFIG_HISI_BB
	hisiap_nmi_notify_lpm3();
	last_task_stack_dump();
	regs_dump(); /*"sctrl", "pctrl", "peri_crg", "gic"*/
#endif

#ifdef CONFIG_HISI_CORESIGHT_TRACE
	etm4_disable_all();
#endif
#ifdef CONFIG_HISI_BB
	save_module_dump_mem();
#endif
	pr_crit("fiq_dump end\n");
	kmsg_dump(KMSG_DUMP_PANIC);
	flush_cache_all();

	while (1)
		;
}
