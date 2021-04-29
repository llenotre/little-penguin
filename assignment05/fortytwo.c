// SPDX-License-Identifier: GPL-3.0-or-later

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>

#define DEVICE_NAME "fortytwo"
#define CLASS_NAME "ft"

MODULE_LICENSE("GPL");

static const char *my_login = "llenotre";

static ssize_t fortytwo_read(struct file *filep, char __user *buffer, size_t len, loff_t *off)
{
	size_t l;

	l = min(len, strlen(my_login));
	memcpy(buffer, my_login, l);
	return l;
}

static ssize_t fortytwo_write(struct file *filep, const char *buffer, size_t len, loff_t *off)
{
	size_t l;

	l = strlen(my_login);
	if (len != l || memcmp(buffer, my_login, len) != 0)
		return -EINVAL;
	else
		return len;
}

static struct file_operations fops = {
	.read = fortytwo_read,
	.write = fortytwo_write,
};

static int major;
static struct class* class = NULL;
static struct device *dev = NULL;

static int __init fortytwo_init(void)
{
	printk(KERN_INFO "Hello world!\n");
	major = register_chrdev(0, DEVICE_NAME, &fops);
	if (major < 0) {
		printk(KERN_ERR "Failed to init fortytwo module!");
		return -1;
	}

	class = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(class)) {
		unregister_chrdev(major, DEVICE_NAME);
		printk(KERN_ERR "Failed to init fortytwo module!");
		return -1;
	}

	dev = device_create(class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
	if (IS_ERR(dev)) {
		class_destroy(class);
		unregister_chrdev(major, DEVICE_NAME);
		printk(KERN_ERR "Failed to init fortytwo module!");
		return -1;
	}
	return 0;
}

static void __exit fortytwo_exit(void)
{
	printk(KERN_INFO "Cleaning up module.\n");
	device_destroy(class, MKDEV(major, 0));
	class_destroy(class);
	unregister_chrdev(major, DEVICE_NAME);
}

module_init(fortytwo_init);
module_exit(fortytwo_exit);
