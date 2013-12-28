#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <asm/io.h>

static void kbd2_do_work(struct work_struct *work);

static char scancode;
static DECLARE_WORK(kbd2_work, kbd2_do_work);

static void kbd2_do_work(struct work_struct *work)
{
	pr_info("Scan Code %x %s\n",
		scancode & 0x7F, scancode & 0x80 ? "Released" : "Pressed");
}

static irqreturn_t kbd2_isr(int irq, void *dev_id)
{
	scancode = inb(0x60);
	schedule_work(&kbd2_work);
	return IRQ_HANDLED;
}

static int __init kbd2_init(void)
{
	return request_irq(1, kbd2_isr, IRQF_SHARED, "kbd2", (void *)kbd2_isr);
}

static void __exit kbd2_cleanup(void)
{
	free_irq(1, (void *)kbd2_isr);
}

module_init(kbd2_init);
module_exit(kbd2_cleanup);

MODULE_LICENSE("GPL");
