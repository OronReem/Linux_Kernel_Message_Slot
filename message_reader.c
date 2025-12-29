#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "message_slot.h"

int main(int argc, char* argv[]) {
    int fd;
    char buffer[MAX_MSG_LEN];
    int ret;

    if (argc != 4) {
        exit(1);
    }

    fd = open(argv[1], O_RDONLY);
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

    ret = read(fd, buffer, MAX_MSG_LEN);
    if (ret < 0) {
        perror("read failed");
        exit(1);
    }

    close(fd);

    if (write(1, buffer, ret) != ret) {
        perror("print failed");
        exit(1);
    }

    return 0;
}