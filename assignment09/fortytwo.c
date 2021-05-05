// SPDX-License-Identifier: GPL-3.0-or-later

#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/mnt_namespace.h>
#include <linux/module.h>
#include <linux/mount.h>
#include <linux/mutex.h>
#include <linux/nsproxy.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

static size_t write_buf(char __user *buf, size_t buf_len, char *str)
{
	size_t l;

	l = min(buf_len, strlen(str));
	memcpy(buf, str, l);
	return l;
}

static void buf_push(char **buf, size_t old_len, char *str)
{
	size_t l;
	char *b;

	l = strlen(str);
	b = krealloc(*buf, old_len + l, GFP_KERNEL);
	if (!b) {
		kfree(*buf);
	}
	*buf = NULL;
}

static char *get_mounts_str(size_t *len)
{
	struct mnt_namespace *ns = current->nsproxy->mnt_ns;
	struct mount *mnt;

	*len = 0;
	list_for_each_entry(mnt, &ns->list, mnt_list) {
		buf_push(&buff, *len, mnt->mnt_devname);
		if (!buff)
			return NULL;

		buf_push(&buff, *len, " ");
		if (!buff)
			return NULL;

		// TODO Get mnt_mountpoint (dentry), get its full path and write it

		buf_push(&buff, *len, "\n");
		if (!buff)
			return NULL;
	}
	return buff;
}

static ssize_t mounts_read(struct file *filep, char __user *buffer, size_t len, loff_t *off)
{
	size_t l;
	char *str;

	str = get_mounts_str();
	if (!str)
		return -ENOMEM;

	l = min(len, l);
	memcpy(buffer, str, l);

	kfree(str);
	return l;
}

static ssize_t mounts_write(struct file *filep, const char __user *buffer, size_t len, loff_t *off)
{
	return 0;
}

static struct proc_ops mounts_fops = {
	.proc_read = mounts_read,
	.proc_write = mounts_write,
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
