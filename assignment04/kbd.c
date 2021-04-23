// SPDX-License-Identifier: GPL-3.0-or-later

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/hid.h>

MODULE_LICENSE("GPL");

static int kbd_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
	printk(KERN_INFO "Keyboard plugged in :)\n");
	return 0;
}

static void kbd_disconnect(struct usb_interface *intf)
{
	printk(KERN_INFO "Keyboard plugged out :<\n");
}

static struct usb_device_id kbd_table[] = {
	{
		USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID,
							USB_INTERFACE_SUBCLASS_BOOT,
							USB_INTERFACE_PROTOCOL_KEYBOARD)
	},
	{}
};

static struct usb_driver kbd_driver = {
	.name = "kbd",
	.id_table = kbd_table,
	.probe = kbd_probe,
	.disconnect = kbd_disconnect,
};

MODULE_DEVICE_TABLE(usb, kbd_table);

static int __init keyboard_init(void)
{
	int result;

	printk(KERN_INFO "Hello world!\n");
	result = usb_register(&kbd_driver);
	if (result < 0) {
		printk(KERN_ERR "Failed to register keyboard driver!\n");
		return -1;
	}
	return 0;
}

static void __exit keyboard_exit(void)
{
	printk(KERN_INFO "Cleaning up module.\n");
	usb_deregister(&kbd_driver);
}

module_init(keyboard_init);
module_exit(keyboard_exit);
