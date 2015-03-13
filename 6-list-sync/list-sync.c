/*
 * SO2 lab3 - task 5
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/sched.h>

MODULE_DESCRIPTION("Full list processing");
MODULE_AUTHOR("SO2");
MODULE_LICENSE("GPL");

#define LOG_LEVEL	KERN_ALERT

spinlock_t lock;

struct task_info {
	pid_t pid;
	unsigned long timestamp;
	atomic_t count;
	struct list_head list;
};

static struct list_head head;

static struct task_info *task_info_alloc(int pid)
{
	struct task_info *ti;

	/* TODO 0: Copy from 3-memory and 4-list. */
	ti = kmalloc(sizeof(*ti), GFP_KERNEL);
	ti->pid = pid;
	ti->timestamp = jiffies;
	atomic_set(&ti->count, 0);

	return ti;
}

/*
 * Return pointer to struct task_info structure or NULL in case
 * pid argument isn't in the list.
 */

static struct task_info *task_info_find_pid(int pid)
{
	struct list_head *p;
	struct task_info *ti;

	/* TODO 1: Implement as in description. */
	spin_lock(&lock);
	list_for_each(p, &head) {
		ti = list_entry(p, struct task_info, list);
		if (ti->pid == pid) {
			spin_unlock(&lock);
			return ti;
		}
	}
	spin_unlock(&lock);
	return NULL;
}

static void task_info_add_to_list(int pid)
{
	struct task_info *ti;

	ti = task_info_find_pid(pid);
	if (ti != NULL) {
		ti->timestamp = jiffies;
		atomic_inc(&ti->count);
		return;
	}

	/* TODO 0: Allocate item and then add to list. */
	/* Call task_info_alloc for allocation. */
	/* Copy from 4-memmory. */
	ti = task_info_alloc(pid);
	spin_lock(&lock);
	list_add(&ti->list, &head);
	spin_unlock(&lock);
}

static void task_info_add_for_current(void)
{
	/* TODO 0: Copy from 3-memory and 4-list. */
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

EXPORT_SYMBOL(task_info_add_for_current);

static void task_info_print_list(const char *msg)
{
	struct list_head *p;
	struct task_info *ti;

	printk(LOG_LEVEL "%s: [ ", msg);
	spin_lock(&lock);
	list_for_each(p, &head) {
		ti = list_entry(p, struct task_info, list);
		printk("(%d, %lu) ", ti->pid, ti->timestamp);
	}
	spin_unlock(&lock);
	printk("]\n");
}

static void task_info_remove_expired(void)
{
	struct list_head *p, *q;
	struct task_info *ti;

	spin_lock(&lock);
	list_for_each_safe(p, q, &head) {
		ti = list_entry(p, struct task_info, list);
		if (jiffies - ti->timestamp > 3 * HZ && atomic_read(&ti->count) < 5) {
			list_del(p);
			kfree(ti);
		}
	}
	spin_unlock(&lock);
}

EXPORT_SYMBOL(task_info_remove_expired);

static void task_info_purge_list(void)
{
	struct list_head *p, *q;
	struct task_info *ti;

	spin_lock(&lock);
	list_for_each_safe(p, q, &head) {
		ti = list_entry(p, struct task_info, list);
		list_del(p);
		kfree(ti);
	}
	spin_unlock(&lock);

}

static int list_full_init(void)
{
	INIT_LIST_HEAD(&head);

	spin_lock_init(&lock);

	task_info_add_for_current();
	task_info_print_list("after first add");

	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(5 * HZ);

	return 0;
}

EXPORT_SYMBOL(task_info_print_list);

static void list_full_exit(void)
{
	struct task_info *ti;

	/* TODO 2: Ensure that at least one task is still available
	 * after calling task_info_remove_expired().
	 */
	/* ... */

	// spin_lock(&lock);
	// ti = list_entry(head.next, struct task_info, list);
	// atomic_set(&ti->count, 6);
	// spin_unlock(&lock);

	task_info_remove_expired();
	task_info_print_list("after removing expired");
	task_info_purge_list();

}

module_init(list_full_init);
module_exit(list_full_exit);
