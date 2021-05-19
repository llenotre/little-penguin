// SPDX-License-Identifier: GPL-3.0-or-later

#include <linux/debugfs.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>

MODULE_LICENSE("GPL");

static const char *my_login = "llenotre";

static ssize_t id_read(struct file *filep, char __user *buffer, size_t len, loff_t *off)
{
	return simple_read_from_buffer(buffer, len, off, my_login, strlen(my_login));
}

static ssize_t id_write(struct file *filep, const char __user *buffer, size_t len, loff_t *off)
{
	char tmp[9];
	ssize_t l;

	l = simple_write_to_buffer(tmp, sizeof(tmp), off, buffer, len);
	if (l == strlen(my_login) && memcmp(tmp, my_login, l) == 0)
		return len;
	else
		return -EINVAL;
}

static struct file_operations id_fops = {
	.read = id_read,
	.write = id_write,
};

static ssize_t jiffies_read(struct file *filep, char __user *buffer, size_t len, loff_t *off)
{
	volatile unsigned long j = jiffies;
	char tmp[64];
	ssize_t l;

	l = snprintf(tmp, sizeof(tmp), "%lu", j);
	return simple_read_from_buffer(buffer, len, off, tmp, l);
}

static ssize_t jiffies_write(struct file *filep, const char __user *buffer, size_t len, loff_t *off)
{
	return len;
}

static struct file_operations jiffies_fops = {
	.read = jiffies_read,
	.write = jiffies_write,
};

DEFINE_MUTEX(foo_mutex);
static char foo_buffer[PAGE_SIZE];
static size_t foo_len = 0;

static ssize_t foo_read(struct file *filep, char __user *buffer, size_t len, loff_t *off)
{
	ssize_t l;

	mutex_lock(&foo_mutex);
	l = simple_read_from_buffer(buffer, len, off, foo_buffer, foo_len);
	mutex_unlock(&foo_mutex);
	return l;
}

static ssize_t foo_write(struct file *filep, const char __user *buffer, size_t len, loff_t *off)
{
	size_t l = 0;

	mutex_lock(&foo_mutex);
	l = simple_write_to_buffer(foo_buffer, sizeof(foo_buffer), off, buffer, len);
	foo_len = l;
	mutex_unlock(&foo_mutex);
	return l;
}

static struct file_operations foo_fops = {
	.read = foo_read,
	.write = foo_write,
};

static struct dentry * dir_entry = NULL;
static struct dentry * id_entry = NULL;
static struct dentry * jiffies_entry = NULL;
static struct dentry * foo_entry = NULL;

static void cleanup(void)
{
	debugfs_remove(id_entry);
	debugfs_remove(jiffies_entry);
	debugfs_remove(foo_entry);
	debugfs_remove(dir_entry);
}

static int __init fortytwo_init(void)
{
	printk(KERN_INFO "Hello world!\n");

	dir_entry = debugfs_create_dir("fortytwo", NULL);
	if (!dir_entry) {
		cleanup();
		return -1;
	}

	id_entry = debugfs_create_file("id", 0666, dir_entry, NULL, &id_fops);
	if (!id_entry) {
		cleanup();
		return -1;
	}

	jiffies_entry = debugfs_create_file("jiffies", 0444, dir_entry, NULL, &jiffies_fops);
	if (!jiffies_entry) {
		cleanup();
		return -1;
	}

	foo_entry = debugfs_create_file("foo", 0644, dir_entry, NULL, &foo_fops);
	if (!foo_entry) {
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
