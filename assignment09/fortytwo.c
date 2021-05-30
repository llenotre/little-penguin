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

struct mount {
	struct hlist_node mnt_hash;
	struct mount *mnt_parent;
	struct dentry *mnt_mountpoint;
	struct vfsmount mnt;
	union {
		struct rcu_head mnt_rcu;
		struct llist_node mnt_llist;
	};
#ifdef CONFIG_SMP
	struct mnt_pcp __percpu *mnt_pcp;
#else
	int mnt_count;
	int mnt_writers;
#endif
	struct list_head mnt_mounts;
	struct list_head mnt_child;
	struct list_head mnt_instance;
	const char *mnt_devname;
	struct list_head mnt_list;
	struct list_head mnt_expire;
	struct list_head mnt_share;
	struct list_head mnt_slave_list;
	struct list_head mnt_slave;
	struct mount *mnt_master;
	struct mnt_namespace *mnt_ns;
	struct mountpoint *mnt_mp;
	union {
		struct hlist_node mnt_mp_list;
		struct hlist_node mnt_umount;
	};
	struct list_head mnt_umounting;
#ifdef CONFIG_FSNOTIFY
	struct fsnotify_mark_connector __rcu *mnt_fsnotify_marks;
	__u32 mnt_fsnotify_mask;
#endif
	int mnt_id;
	int mnt_group_id;
	int mnt_expiry_mark;
	struct hlist_head mnt_pins;
	struct hlist_head mnt_stuck_children;
} __randomize_layout;

struct ns_common {
	atomic_long_t stashed;
	const struct proc_ns_operations *ops;
	unsigned int inum;
	refcount_t count;
};

struct mnt_namespace {
	struct ns_common	ns;
	struct mount *	root;
	struct list_head	list;
	spinlock_t		ns_lock;
	struct user_namespace	*user_ns;
	struct ucounts		*ucounts;
	u64			seq;
	wait_queue_head_t poll;
	u64 event;
	unsigned int		mounts;
	unsigned int		pending_mounts;
} __randomize_layout;

static void buf_push(char **buf, size_t *old_len, const char *str)
{
	size_t l;
	char *b;

	l = strlen(str);
	b = krealloc(*buf, *old_len + l, GFP_KERNEL);
	if (b) {
		memcpy(b + *old_len, str, l);
	} else {
		kfree(*buf);
	}
	*buf = b;
	*old_len += l;
}

static char *get_path(char *buff, struct mount *mnt) {
	struct path mnt_path = {
		.dentry = mnt->mnt.mnt_root,
		.mnt = &mnt->mnt
	};

	return d_path(&mnt_path, buff, PATH_MAX + 1);
}

static char *get_mounts_str(size_t *len)
{
	struct mnt_namespace *ns = current->nsproxy->mnt_ns;
	struct mount *mnt;
	char *buff = NULL;
	char *path = kmalloc(PATH_MAX + 1, GFP_KERNEL);

	if (!path)
		goto end;
	*len = 0;
	list_for_each_entry(mnt, &ns->list, mnt_list) {
		buf_push(&buff, len, mnt->mnt_devname);
		if (!buff)
			goto end;

		buf_push(&buff, len, " ");
		if (!buff)
			goto end;

		buf_push(&buff, len, get_path(path, mnt));
		if (!buff)
			goto end;

		buf_push(&buff, len, "\n");
		if (!buff)
			goto end;
	}

end:
	kfree(path);
	if (!buff)
		*len = 0;
	return buff;
}

static ssize_t mounts_read(struct file *filep, char __user *buffer, size_t len, loff_t *off)
{
	ssize_t l;
	char *str;

	str = get_mounts_str(&l);
	if (!str)
		return -ENOMEM;

	l = simple_read_from_buffer(buffer, len, off, str, l);
	kfree(str);
	return l;
}

static ssize_t mounts_write(struct file *filep, const char __user *buffer, size_t len, loff_t *off)
{
	return len;
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
