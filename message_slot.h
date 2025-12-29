#ifndef MESSAGE_SLOT_H
#define MESSAGE_SLOT_H

#include <linux/ioctl.h>
#include <linux/types.h>

#define MINOR_NUMBER_MAX 256
#define MAX_MSG_LEN 128
#define MAJOR_NUM 235
#define MSG_SLOT_MAGIC 's'
#define MSG_SLOT_CHANNEL _IOW(MSG_SLOT_MAGIC, 0, unsigned int)
#define MSG_SLOT_SET_CEN _IOW(MSG_SLOT_MAGIC, 1, unsigned int)

typedef struct msg_channel {
    unsigned int id;
    char msg[MAX_MSG_LEN];
    size_t len;
    struct msg_channel *next;
} msg_channel_t;

typedef struct msg_slot {
    int minor;
    struct msg_channel *channels;
} msg_slot_t;

typedef struct fd_state {
    msg_slot_t *slot;
    unsigned int channel_id;
    unsigned int filter;
} fd_state_t;

#endif