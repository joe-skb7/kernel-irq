#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <asm/io.h>

static irqreturn_t kbd2_isr(int irq, void *dev_id)
{
	char scancode;

	scancode = inb(0x60);
	/* NOTE: i/o ops take a lot of time thus must be avoided in HW ISRs */
	pr_info("Scan Code %x %s\n",
		scancode & 0x7F, scancode & 0x80 ? "Released" : "Pressed");

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
