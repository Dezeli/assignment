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
MODULE_AUTHOR("Minseong Park");
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
    struct student_packet pkt; // IOWR용 패킷 구조체
    switch (cmd) {
    case IOCTL_PRINT:    /* _IO */
        // 단순 출력: 현재 저장된 학생 ID를 커널 로그로 출력
        pr_info("hello_kernel: student id = %d\n", current_id);
        return 0;
    case IOCTL_GET_ID:   /* _IOR */
        // 유저 프로그램이 요청한 ID 값을 유저 영역으로 전달
        if (copy_to_user((int __user *)arg, &current_id, sizeof(int)))
            return -EFAULT;
        pr_info("hello_kernel: GET_ID = %d\n", current_id);
        return 0;
    case IOCTL_SET_ID:   /* _IOW */
        // 유저 프로그램이 보낸 새로운 ID 값을 커널 영역에 저장
        if (copy_from_user(&current_id, (int __user *)arg, sizeof(int)))
            return -EFAULT;
        pr_info("hello_kernel: SET_ID = %d\n", current_id);
        return 0;
    case IOCTL_EXCHANGE: /* _IOWR */
        // 유저가 전달한 구조체 데이터를 커널로 복사
        if (copy_from_user(&pkt, (void __user *)arg, sizeof(pkt)))
            return -EFAULT;
        // 커널에서 output 값을 생성 (간단하게 input을 그대로 echo하는 것으로 설계했습니다.)
        pkt.output = pkt.input;
        // 커널 로그로 처리 내용을 출력
        pr_info("hello_kernel: EXCHANGE input=%d output=%d\n",
                pkt.input, pkt.output);
        // 수정된 구조체를 다시 유저 영역으로 전달
        if (copy_to_user((void __user *)arg, &pkt, sizeof(pkt)))
            return -EFAULT;
        return 0;
    default:
        // 지원하지 않는 IOCTL 명령 번호
        return -ENOTTY;
    }
}


static int __init hello_init(void) {
    int ret;
    // 1) 문자 디바이스 번호(major/minor) 자동 할당
    ret = alloc_chrdev_region(&_dev, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("hello_kernel: alloc_chrdev_region failed\n");
        return ret;
    }
    // 2) cdev 구조체 초기화 및 file_operations 연결
    cdev_init(&_cdev, &_fops);
    // 3) 커널에 cdev 등록
    ret = cdev_add(&_cdev, _dev, 1);
    if (ret < 0) {
        pr_err("hello_kernel: cdev_add failed\n");
        unregister_chrdev_region(_dev, 1);
        return ret;
    }
    // 4) /sys/class/hello 생성
    _class = class_create(CLASS_NAME);
    if (IS_ERR(_class)) {
        pr_err("hello_kernel: class_create failed\n");
        cdev_del(&_cdev);
        unregister_chrdev_region(_dev, 1);
        return PTR_ERR(_class);
    }
    // 5) /dev/hello_kernel 디바이스 파일 생성
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
    // /dev/hello_kernel 제거
    device_destroy(_class, _dev);
    // /sys/class/hello 제거
    class_destroy(_class);
    // cdev 제거
    cdev_del(&_cdev);
    // 문자 디바이스 번호 반환
    unregister_chrdev_region(_dev, 1);
    pr_info("hello_kernel: module unloaded\n");
}
