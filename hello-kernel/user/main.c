#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "../include/abi.h"

static void usage(const char *prog) {
    fprintf(stderr,
        "Usage:\n"
        "  %s print             # _IO   : 커널에 Hello 메시지 요청\n"
        "  %s get               # _IOR  : 커널에서 id 읽기\n"
        "  %s set <id>          # _IOW  : 커널의 id 설정\n",
        prog, prog, prog);
}

static int open_device(void) {
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("open");
        fprintf(stderr,
                "hint: 커널 모듈이 로드되어 있고 %s 이 존재하는지 확인하세요.\n",
                DEVICE_PATH);
    }
    return fd;
}

int main(int argc, char *argv[]) {
    int fd;
    if (argc < 2) { usage(argv[0]); return EXIT_FAILURE; }

    fd = open_device();
    if (fd < 0) return EXIT_FAILURE;

    if (strcmp(argv[1], "print") == 0) {

        // to be implemented 

    } else if (strcmp(argv[1], "get") == 0) {

        // to be implemented 

    } else if (strcmp(argv[1], "set") == 0) {

        // to be implemented 

    } else {
        usage(argv[0]);
        close(fd);
        return EXIT_FAILURE;
    }

    close(fd);
    return EXIT_SUCCESS;
}