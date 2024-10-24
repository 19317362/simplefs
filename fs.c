#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netlink.h>

#include <linux/skbuff.h>
#include <net/sock.h>

#define NETLINK_USER 31

struct sock *nl_sk = NULL;
#include "simplefs.h"

//netlink ++++
static void nl_recv_msg(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    int pid;
    struct sk_buff *skb_out;
    int msg_size;
    char *msg = "Hello from kernel";
    int res;

    nlh = (struct nlmsghdr *)skb->data;
    pr_info("Kernel received msg payload: %s\n", (char *)nlmsg_data(nlh));
    pid = nlh->nlmsg_pid; /*pid of sending process */

    msg_size = strlen(msg);

    skb_out = nlmsg_new(msg_size, 0);
    if (!skb_out) {
        pr_err("Failed to allocate new skb\n");
        return;
    }

    nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
    NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
    strncpy(nlmsg_data(nlh), msg, msg_size);

    res = nlmsg_unicast(nl_sk, skb_out, pid);
    if (res < 0)
        pr_err("Error while sending back to user\n");
}
static int nl_init(void)
{
    struct netlink_kernel_cfg cfg = {
        .input = nl_recv_msg,
    };

    nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
    if (!nl_sk) {
        pr_err("Error creating socket.\n");
        return -10;
    }

    pr_info("NL initialized\n");
    return 0;
}
static void nl_release(void)
{
    if (nl_sk != NULL){
        netlink_kernel_release(nl_sk);
        nl_sk = NULL;
        pr_info("NL released\n");
    }

}
//netlink ----

/* Mount a simplefs partition */
struct dentry *simplefs_mount(struct file_system_type *fs_type,
                              int flags,
                              const char *dev_name,
                              void *data)
{
    struct dentry *dentry =
        mount_bdev(fs_type, flags, dev_name, data, simplefs_fill_super);
    if (IS_ERR(dentry))
        pr_err("'%s' mount failure\n", dev_name);
    else
        pr_info("'%s' mount success\n", dev_name);

    return dentry;
}

/* Unmount a simplefs partition */
void simplefs_kill_sb(struct super_block *sb)
{
    kill_block_super(sb);

    pr_info("unmounted disk\n");
}

static struct file_system_type simplefs_file_system_type = {
    .owner = THIS_MODULE,
    .name = "dxrfs",
    .mount = simplefs_mount,
    .kill_sb = simplefs_kill_sb,
    .fs_flags = FS_REQUIRES_DEV,
    .next = NULL,
};

static int __init simplefs_init(void)
{
    int ret;
    ret = nl_init();
    if (ret) {
        pr_err("Failed to create netlink\n");
        goto err;
    }
    ret = simplefs_init_inode_cache();
    if (ret) {
        pr_err("Failed to create inode cache\n");
        goto err;
    }

    ret = register_filesystem(&simplefs_file_system_type);
    if (ret) {
        pr_err("Failed to register file system\n");
        goto err_inode;
    }
    
    pr_info("module loaded\n");
    return 0;

err_inode:
    simplefs_destroy_inode_cache();
    /* Only after rcu_barrier() is the memory guaranteed to be freed. */
    rcu_barrier();
err:
    return ret;
}

static void __exit simplefs_exit(void)
{
    nl_release();
    int ret = unregister_filesystem(&simplefs_file_system_type);
    if (ret)
        pr_err("Failed to unregister file system\n");

    simplefs_destroy_inode_cache();
    /* Only after rcu_barrier() is the memory guaranteed to be freed. */
    rcu_barrier();

    pr_info("module unloaded\n");
}

module_init(simplefs_init);
module_exit(simplefs_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("National Cheng Kung University, Taiwan");
MODULE_DESCRIPTION("a simple file system");
