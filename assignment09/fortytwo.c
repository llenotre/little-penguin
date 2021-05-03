// SPDX-License-Identifier: GPL-3.0-or-later

#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>

#define DEVICE_NAME "fortytwo"
#define CLASS_NAME "ft"

MODULE_LICENSE("GPL");

static ssize_t mounts_read(struct file *filep, char __user *buffer, size_t len, loff_t *off)
{
	// TODO
	return 0;
}

static ssize_t mounts_write(struct file *filep, const char __user *buffer, size_t len, loff_t *off)
{
	// TODO
	return 0;
}

static struct file_operations mounts_fops = {
	.read = mounts_read,
	.write = mounts_write,
};

static struct proc_dir_entry * mounts_entry = NULL;

static void cleanup(void)
{
	proc_remove(mounts_entry);
}

static int __init fortytwo_init(void)
{
	printk(KERN_INFO "Hello world!\n");

	mounts_entry = proc_create("mymounts", 0444, NULL, &mounts_fops);
	if (!mounts_entry) {
		cleanup();
		return -1;
	}
	return 0;
}

static void __exit fortytwo_exit(void)
{
	printk(KERN_INFO "Cleaning up module.\n");
	cleanup();
}

module_init(fortytwo_init);
module_exit(fortytwo_exit);
