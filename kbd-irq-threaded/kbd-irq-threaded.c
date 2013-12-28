#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <asm/io.h>

#define KBD_IRQ			1	/* IRQ number for keyboard (i8042) */
#define KBD_DATA		0x60	/* I/O port for keyboard data */
#define KBD_SCAN_CODE_MASK	0x7f
#define KBD_STATUS_MASK		0x80

static char scancode;

static irqreturn_t kbd2_isr(int irq, void *data)
{
	char *sc = (char *)data;

	*sc = inb(KBD_DATA);

	return IRQ_WAKE_THREAD;
}

static irqreturn_t kbd2_thread(int irq, void *data)
{
	char sc = *(char *)data;

	pr_alert("### Scan Code %#x %s\n",
			sc & KBD_SCAN_CODE_MASK,
			sc & KBD_STATUS_MASK ? "Released" : "Pressed");

	return IRQ_HANDLED;
}

static int __init kbd2_init(void)
{
	return request_threaded_irq(KBD_IRQ, kbd2_isr, kbd2_thread,
			IRQF_SHARED, "masha", &scancode);
}

static void __exit kbd2_clean(void)
{
	free_irq(KBD_IRQ, &scancode);
}

module_init(kbd2_init);
module_exit(kbd2_clean);

MODULE_LICENSE("GPL");
