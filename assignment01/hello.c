/*
 * Implementation of a very simple Hello World! Linux kernel module.
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_AUTHOR("Luc Lenôtre");
MODULE_DESCRIPTION("Yet another hello world module");
MODULE_VERSION("0.1");

static int hello_init(void)
{
	printk("Hello world!");
	return 0;
}

static void hello_exit(void)
{
	printk("Cleaning up module.");
}

module_init(hello_init);
module_exit(hello_exit);
