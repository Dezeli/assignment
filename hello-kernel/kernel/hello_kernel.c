#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

#include "../include/abi.h"

#define STUDENT_ID 202224210 // Don't modify

MODULE_LICENSE("GPL");
MODULE_AUTHOR(TBD);
MODULE_DESCRIPTION("Hello Kernel ioctl example (IO/IOR/IOW/IOWR + ABI)");
MODULE_VERSION("0.1");

static dev_t _dev;
static struct cdev _cdev;
static struct class *_class;

static int  hello_open   (struct inode *inode, struct file *file);
static int  hello_release(struct inode *inode, struct file *file);
static long hello_ioctl  (struct file *file, unsigned int cmd, unsigned long arg);

static int  __init hello_init(void);
static void __exit hello_exit(void);

static const struct file_operations _fops = {
    .owner          = THIS_MODULE,
    .open           = hello_open,
    .release        = hello_release,
    .unlocked_ioctl = hello_ioctl,
};

module_init(hello_init);
module_exit(hello_exit);

static int current_id = STUDENT_ID;

static int hello_open(struct inode *inode, struct file *file) {
    pr_info("hello_kernel: device opened\n");
    return 0;
}

static int hello_release(struct inode *inode, struct file *file) {
    pr_info("hello_kernel: device closed\n");
    return 0;
}

static long hello_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    struct student_packet pkt;

    switch (cmd) {

    case IOCTL_PRINT:    /* _IO */
        pr_info("hello_kernel: student id = %d\n", current_id);
        return 0;

    case IOCTL_GET_ID:   /* _IOR */
        if (copy_to_user((int __user *)arg, &current_id, sizeof(int)))
            return -EFAULT;
        pr_info("hello_kernel: GET_ID = %d\n", current_id);
        return 0;

    case IOCTL_SET_ID:   /* _IOW */
        if (copy_from_user(&current_id, (int __user *)arg, sizeof(int)))
            return -EFAULT;
        pr_info("hello_kernel: SET_ID = %d\n", current_id);
        return 0;

    case IOCTL_EXCHANGE: /* _IOWR */
        if (copy_from_user(&pkt, (void __user *)arg, sizeof(pkt)))
            return -EFAULT;

        pkt.output = pkt.input;

        pr_info("hello_kernel: EXCHANGE input=%d output=%d\n",
                pkt.input, pkt.output);

        if (copy_to_user((void __user *)arg, &pkt, sizeof(pkt)))
            return -EFAULT;

        return 0;

    default:
        return -ENOTTY;
    }
}


static int __init hello_init(void) {

    int ret;

    ret = alloc_chrdev_region(&_dev, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("hello_kernel: alloc_chrdev_region failed\n");
        return ret;
    }

    cdev_init(&_cdev, &_fops);
    ret = cdev_add(&_cdev, _dev, 1);
    if (ret < 0) {
        pr_err("hello_kernel: cdev_add failed\n");
        unregister_chrdev_region(_dev, 1);
        return ret;
    }

    _class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(_class)) {
        pr_err("hello_kernel: class_create failed\n");
        cdev_del(&_cdev);
        unregister_chrdev_region(_dev, 1);
        return PTR_ERR(_class);
    }

    if (IS_ERR(device_create(_class, NULL, _dev, NULL, DEVICE_NAME))) {
        pr_err("hello_kernel: device_create failed\n");
        class_destroy(_class);
        cdev_del(&_cdev);
        unregister_chrdev_region(_dev, 1);
        return -1;
    }

    pr_info("hello_kernel: module loaded\n");
    return 0;
}

static void __exit hello_exit(void) {

    device_destroy(_class, _dev);
    class_destroy(_class);
    cdev_del(&_cdev);
    unregister_chrdev_region(_dev, 1);

    pr_info("hello_kernel: module unloaded\n");
}