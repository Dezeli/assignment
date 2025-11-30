#ifndef HELLO_KERNEL_ABI_H
#define HELLO_KERNEL_ABI_H

#ifdef __KERNEL__
#include <linux/ioctl.h>
#else
#include <sys/ioctl.h> 
#endif

#define DEVICE_NAME "hello_kernel"
#define DEVICE_PATH "/dev/hello_kernel"
#define CLASS_NAME  "hello"

#define IOCTL_MAGIC 'h'

#define IOCTL_PRINT _IO(IOCTL_MAGIC, 1)
#define IOCTL_GET_ID _IOR(IOCTL_MAGIC, 2, int)
#define IOCTL_SET_ID _IOW(IOCTL_MAGIC, 3, int)

struct student_packet {
    int input;   // 유저 프로그램에서 전달하는 입력 값
    int output;  // 커널이 계산하여 유저에게 다시 돌려주는 값
};

// 구조체를 커널에 전달하고, 가공된 구조체를 다시 돌려받는 IOCTL
#define IOCTL_EXCHANGE _IOWR(IOCTL_MAGIC, 4, struct student_packet)

#endif 