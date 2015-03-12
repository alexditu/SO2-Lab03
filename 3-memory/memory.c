/*
 * SO2 lab3 - task 3
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sched.h>

MODULE_DESCRIPTION("Memory processing");
MODULE_AUTHOR("SO2");
MODULE_LICENSE("GPL");

#define LOG_LEVEL	KERN_ALERT

struct task_info {
	pid_t pid;
	unsigned long timestamp;
};

static struct task_info *ti1, *ti2, *ti3, *ti4;

static struct task_info *task_info_alloc(int pid)
{
	struct task_info *ti;

	/* TODO 2: Allocate and initialize ti. */
	ti = kmalloc (sizeof(*ti), GFP_KERNEL);
	ti->pid = pid;
	ti->timestamp = 0;

	return ti;
}

static int memory_init(void)
{
	struct task_struct *t;

	/* TODO 1: current PID */
	ti1 = task_info_alloc(current->pid);

	/* TODO 1: parent PID */
	ti2 = task_info_alloc(current->parent->pid);

	/* TODO 1: next process PID */
	t = next_task(current);
	ti3 = task_info_alloc(t->pid);

	/* TODO 1: next process of next process PID */
	t = next_task(t);
	ti4 = task_info_alloc(t->pid);

	return 0;
}

static void memory_exit(void)
{
	/* TODO 3: Print ti* field values. */
	printk ("ti1: pid = %d, time = %lu\n", ti1->pid, ti1->timestamp);
	printk ("ti2: pid = %d, time = %lu\n", ti2->pid, ti2->timestamp);
	printk ("ti3: pid = %d, time = %lu\n", ti3->pid, ti3->timestamp);
	printk ("ti4: pid = %d, time = %lu\n", ti4->pid, ti4->timestamp);

	/* TODO 4: Free ti*. */
	kfree(ti1);
	kfree(ti2);
	kfree(ti3);
	kfree(ti4);
}

module_init(memory_init);
module_exit(memory_exit);
