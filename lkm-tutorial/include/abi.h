#ifndef TUTORIAL_ABI_H 
#define TUTORIAL_ABI_H

/*
 * Userspace <-> kernel shared ABI for the tutorial device.
 * Keep this header in sync on both sides.
 */
#ifdef __KERNEL__
#include <linux/ioctl.h>
#else
#include <sys/ioctl.h> 
#endif

/* Character device metadata */
#define DEVICE_NAME "tutorial"
#define DEVICE_PATH "/dev/tutorial"
#define CLASS_NAME  "tutorial_class"

/* Arbitrary ioctl magic for this device */
#define IOCTL_MAGIC 'h'

/*
 * IOCTL_HELLO
 *
 * - No payload (_IO)
 * - Kernel side logs a greeting to dmesg
 */
#define IOCTL_HELLO _IO(IOCTL_MAGIC, 1)

#endif 