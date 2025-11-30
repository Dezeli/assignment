#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "../include/abi.h"

int main(void) {

    int fd;
    fd = open(DEVICE_PATH, O_RDWR);

    if (fd < 0) {
        perror("open");
        fprintf(stderr,
                "hint: 커널 모듈이 로드되어 있고 %s 가 존재하는지 확인하세요.\n",
                DEVICE_PATH);
        return EXIT_FAILURE;
    }

    if (ioctl(fd, IOCTL_HELLO) == -1) {
        perror("ioctl");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("Done. dmesg 에서 커널 로그를 확인해보세요.\n");

    close(fd);
    return EXIT_SUCCESS;
}