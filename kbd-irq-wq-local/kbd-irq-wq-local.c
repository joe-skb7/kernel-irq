#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <asm/io.h>

struct scanwork {
	struct work_struct tsk;
	int sccode;
	unsigned char status;
};

static struct scanwork *sw;
static struct workqueue_struct *my_workqueue;

static void got_char(struct work_struct *tp)
{
	struct scanwork *ssw = container_of(tp, struct scanwork, tsk);
	printk(KERN_INFO "Scan Code %x %s.\n",
			ssw->sccode & 0x7F,
			ssw->sccode & 0x80 ? "Released" : "Pressed");
}

static irqreturn_t irq_handler(int irq, void *dev_id)
{
	static int initialised = 0;
	struct scanwork *sw = dev_id;

	sw->status = inb(0x64);
	sw->sccode = inb(0x60);

	if (initialised == 0) {
		INIT_WORK(&sw->tsk, got_char);
		initialised = 1;
	} else {
		PREPARE_WORK(&sw->tsk, got_char);
	}
	queue_work(my_workqueue, &sw->tsk);
	return IRQ_HANDLED;
}

static int __init init_kbd(void)
{
	my_workqueue = create_workqueue("Wrkque");
	sw = kzalloc(sizeof(struct scanwork), GFP_KERNEL);
	return request_irq(1, irq_handler, IRQF_SHARED, "test", sw);
}

static void __exit cleanup_kbd(void)
{
	free_irq(1, sw);
	kfree(sw);
}

module_init(init_kbd);
module_exit(cleanup_kbd);

MODULE_LICENSE("GPL");
