#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "message_slot.h"

int main(int argc, char* argv[]) {
    int fd;
    int ret;

    if (argc != 5) {
        exit(1);
    }

    fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        perror("open failed");
        exit(1);
    }

    unsigned int channel_id = atoi(argv[2]);
    ret = ioctl(fd, MSG_SLOT_CHANNEL, channel_id);
    if (ret < 0) {
        perror("ioctl channel failed");
        exit(1);
    }

    unsigned int filter = atoi(argv[3]);
    ret = ioctl(fd, MSG_SLOT_SET_CEN, filter);
    if (ret < 0) {
        perror("ioctl filter failed");
        exit(1);
    }

    ret = write(fd, argv[4], strlen(argv[4]));
    if (ret < 0) {
        perror("write failed");
        exit(1);
    }

    close(fd);
    return 0;
}