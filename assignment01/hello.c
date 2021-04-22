// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Implementation of a very simple Hello World! Linux kernel module.
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Luc Lenôtre");
MODULE_DESCRIPTION("Yet another hello world module");
MODULE_VERSION("0.1");

static int __init hello_init(void)
{
	pr_info("Hello world!\n");
	return 0;
}

static void __exit hello_exit(void)
{
	pr_info("Cleaning up module.\n");
}

module_init(hello_init);
module_exit(hello_exit);
