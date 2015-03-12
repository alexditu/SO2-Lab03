/*
 * SO2 lab3 - task 4
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/sched.h>

MODULE_DESCRIPTION("Use list to process task info");
MODULE_AUTHOR("SO2");
MODULE_LICENSE("GPL");

#define LOG_LEVEL	KERN_ALERT

struct task_info {
	pid_t pid;
	unsigned long timestamp;
	struct list_head list;
};

static struct list_head head;

static struct task_info *task_info_alloc(int pid)
{
	struct task_info *ti;

	/* TODO 0: Copy from 3-memory. */
	ti = kmalloc(sizeof(*ti), GFP_KERNEL);
	ti->pid = pid;
	ti->timestamp = 0;

	return ti;
}

static void task_info_add_to_list(int pid)
{
	struct task_info *ti;

	/* TODO 1: Allocate item and then add to list. */
	/* Call task_info_alloc for allocation. */
	ti = task_info_alloc(pid);
	list_add(&ti->list, &head);
}

static void task_info_add_for_current(void)
{
	/* TODO 0: Copy from 3-memory. */
	struct task_struct *t;

	/* TODO 1: current PID */
	task_info_add_to_list(current->pid);

	/* TODO 1: parent PID */
	task_info_add_to_list(current->parent->pid);

	/* TODO 1: next process PID */
	t = next_task(current);
	task_info_add_to_list(t->pid);

	/* TODO 1: next process of next process PID */
	t = next_task(t);
	task_info_add_to_list(t->pid);
}

static void task_info_print_list(const char *msg)
{
	struct list_head *p;
	struct task_info *ti;

	printk(LOG_LEVEL "%s: [ ", msg);
	list_for_each(p, &head) {
		ti = list_entry(p, struct task_info, list);
		printk("(%d, %lu) ", ti->pid, ti->timestamp);
	}
	printk("]\n");
}

static void task_info_purge_list(void)
{
	/* TODO 2: Remove all items from list. */
	struct list_head *i, *tmp;

	list_for_each_safe(i, tmp, &head) {
		list_del(i);
	}
}

static int list_init(void)
{
	INIT_LIST_HEAD(&head);

	task_info_add_for_current();

	return 0;
}

static void list_exit(void)
{
	task_info_print_list("before exiting");
	task_info_purge_list();
}

module_init(list_init);
module_exit(list_exit);
