// SPDX-License-Identifier: GPL-3.0-or-later

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

static int __init keyboard_init(void)
{
	printk(KERN_INFO "Keyboard plugged in :)\n");
	return 0;
}

static void __exit keyboard_exit(void)
{
	printk(KERN_INFO "Keyboard plugged out :<\n");
}

module_init(keyboard_init);
module_exit(keyboard_exit);
