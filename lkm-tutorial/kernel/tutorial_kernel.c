#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

#include "../include/abi.h"

/*
 * Module metadata
 *
 * Visible in `modinfo` and kernel logs.
 * The LICENSE tag also affects how the kernel treats this module
 * (e.g., access to GPL-only symbols).
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("System Programming TA");
MODULE_DESCRIPTION("LKM TUTORIAL");
MODULE_VERSION("0.1");

/*
 * Global device state
 *
 * _dev   : allocated device number (major/minor)
 * _cdev  : character device object, bound to file_operations
 * _class : used to create /dev/tutorial via sysfs/class
 */
static dev_t _dev;
static struct cdev _cdev;
static struct class *_class;

/*
 * These functions form the core entry points for this module:
 *
 * - tutorial_open()
 * - tutorial_release()
 * - tutorial_ioctl()
 *
 * They are referenced from the file_operations table below and are
 * called by the VFS when user-space operates on /dev/tutorial.
 *
 * - tutorial_init()
 * - tutorial_exit()
 *
 * These are the module's init/exit hooks. The kernel calls them once
 * when the module is inserted (insmod) and removed (rmmod),
 * via the module_init/module_exit macros.
 */
static int  tutorial_open   (struct inode *inode, struct file *file);
static int  tutorial_release(struct inode *inode, struct file *file);
static long tutorial_ioctl  (struct file *file, unsigned int cmd, unsigned long arg);

static int  __init tutorial_init(void);
static void __exit tutorial_exit(void);


/*
 * file_operations for /dev/tutorial
 *
 * Only the callbacks we care about are wired up here:
 * - open/release for tracking lifetime of the file descriptor
 * - unlocked_ioctl for handling ioctl() requests from user-space
 */
static const struct file_operations _fops = {
    .owner          = THIS_MODULE,
    .open           = tutorial_open,
    .release        = tutorial_release,
    .unlocked_ioctl = tutorial_ioctl,
};


/*
 * Register module entry/exit hooks.
 *
 * tutorial_init() runs once when the module is loaded.
 * tutorial_exit() runs once when the module is unloaded.
 */
module_init(tutorial_init);
module_exit(tutorial_exit);


// source code

/* basic open/close hooks */
static int tutorial_open(struct inode *inode, struct file *file) {
    pr_info("tutorial_kernel: device opened\n");
    return 0;
}

static int tutorial_release(struct inode *inode, struct file *file) {
    pr_info("tutorial_kernel: device closed\n");
    return 0;
}

/* ioctl entry point */
static long tutorial_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {

    switch (cmd) {
    case IOCTL_HELLO:
        pr_info("tutorial_kernel: Hello from tutorial kernel module! :)\n");
        return 0;

    default:
        return -ENOTTY; /* 지원하지 않는 ioctl */
    }
}

/* module load: register char device and create /dev node */
static int __init tutorial_init(void) {
    int ret;

    /* allocate a single char device number */
    ret = alloc_chrdev_region(&_dev, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("tutorial_kernel: failed to alloc_chrdev_region\n");
        return ret;
    }

    /* wire up file_operations and register cdev */
    cdev_init(&_cdev, &_fops);
    _cdev.owner = THIS_MODULE;

    ret = cdev_add(&_cdev, _dev, 1);
    if (ret < 0) {
        pr_err("tutorial_kernel: failed to cdev_add\n");
        unregister_chrdev_region(_dev, 1);
        return ret;
    }

    /* expose device under /dev */
    _class = class_create(CLASS_NAME);
    if (IS_ERR(_class)) {
        pr_err("tutorial_kernel: failed to create class\n");
        cdev_del(&_cdev);
        unregister_chrdev_region(_dev, 1);
        return PTR_ERR(_class);
    }

    if (IS_ERR(device_create(_class, NULL, _dev, NULL,
                             DEVICE_NAME))) {
        pr_err( "tutorial_kernel: failed to create device\n");
        class_destroy(_class);
        cdev_del(&_cdev);
        unregister_chrdev_region(_dev, 1);
        return -EINVAL;
    }

    pr_info("tutorial_kernel: module loaded (major=%d, minor=%d)\n",
            MAJOR(_dev), MINOR(_dev));
    return 0;
}

/* module unload: drop /dev node and char device */
static void __exit tutorial_exit(void) {

    device_destroy(_class, _dev);
    class_destroy(_class);
    cdev_del(&_cdev);
    unregister_chrdev_region(_dev, 1);

    pr_info("tutorial_kernel: module unloaded\n");
}
