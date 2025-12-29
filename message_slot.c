#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/slab.h>
#include "message_slot.h"
#include <linux/module.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
static msg_slot_t* slots[MINOR_NUMBER_MAX];

static int device_open(struct inode* inode, struct file* file);
static ssize_t device_read(struct file* file, char __user* buffer, size_t length, loff_t* offset);
static ssize_t device_write(struct file* file, const char __user* buffer, size_t length, loff_t* offset);
static long device_ioctl(struct file* file, unsigned int ioctl_command_id, unsigned long ioctl_param);
static int device_release(struct inode* inode, struct file* file);

struct file_operations fops = {
    .owner          = THIS_MODULE,
    .open           = device_open,
    .unlocked_ioctl = device_ioctl,
    .read           = device_read,
    .write          = device_write,
    .release        = device_release,
};


static int device_open(struct inode* inode, struct file* file) {
    msg_slot_t* slot;
    fd_state_t* state;
    int minor = iminor(inode);

    if (slots[minor] == NULL) {
        slot = (msg_slot_t*)kmalloc(sizeof(msg_slot_t), GFP_KERNEL);
        if (slot == NULL) {
            return -ENOMEM;
        }
        slot->minor = minor;
        slot->channels = NULL;
        slots[minor] = slot;
    } else {
        slot = slots[minor];
    }

    state = (fd_state_t*)kmalloc(sizeof(fd_state_t), GFP_KERNEL);
    if (state == NULL) {
        return -ENOMEM;
    }

    state->slot = slot;
    state->channel_id = 0;
    state->filter = 0;

    file->private_data = (void*)state;

    return 0;
}

static long device_ioctl(struct file* file, unsigned int ioctl_command_id, unsigned long ioctl_param) {
    fd_state_t* state = (fd_state_t*)file->private_data;
    msg_slot_t* slot = state->slot;
    msg_channel_t *curr, *prev;

    if (ioctl_command_id == MSG_SLOT_CHANNEL) {
        if (ioctl_param == 0) return -EINVAL;

        curr = slot->channels;
        prev = NULL;

        while (curr != NULL) {
            if (curr->id == ioctl_param) {
                state->channel_id = ioctl_param;
                return 0;
            }
            prev = curr;
            curr = curr->next;
        }

        curr = (msg_channel_t*)kmalloc(sizeof(msg_channel_t), GFP_KERNEL);
        if (curr == NULL) return -ENOMEM;

        curr->id = ioctl_param;
        curr->len = 0;
        curr->next = NULL;

        if (prev == NULL) slot->channels = curr;
        else prev->next = curr;

        state->channel_id = ioctl_param;
        return 0;
    }

    if (ioctl_command_id == MSG_SLOT_SET_CEN) {
        if (ioctl_param != 0 && ioctl_param != 1) return -EINVAL;
        state->filter = (unsigned int)ioctl_param;
        return 0;
    }

    return -EINVAL;
}

static ssize_t device_read(struct file* file, char __user* buffer, size_t length, loff_t* offset) {
    fd_state_t* state = (fd_state_t*)file->private_data;
    msg_slot_t* slot = state->slot;
    msg_channel_t* curr;

    if (state->channel_id == 0) return -EINVAL;

    curr = slot->channels;
    while (curr != NULL) {
        if (curr->id == state->channel_id) break;
        curr = curr->next;
    }

    if (curr == NULL || curr->len == 0) return -EWOULDBLOCK;
    if (length < curr->len) return -ENOSPC;

    if (copy_to_user(buffer, curr->msg, curr->len) != 0) return -EFAULT;

    return curr->len;
}

static ssize_t device_write(struct file* file, const char __user* buffer, size_t length, loff_t* offset) {
    fd_state_t* state = (fd_state_t*)file->private_data;
    msg_slot_t* slot = state->slot;
    msg_channel_t* curr;
    int i;

    if (state->channel_id == 0) return -EINVAL;
    if (length == 0 || length > MAX_MSG_LEN) return -EMSGSIZE;

    curr = slot->channels;
    while (curr != NULL) {
        if (curr->id == state->channel_id) break;
        curr = curr->next;
    }

    if (curr == NULL) return -EINVAL;

    if (copy_from_user(curr->msg, buffer, length) != 0) return -EFAULT;
    curr->len = length;

    if (state->filter == 1) {
        for (i = 3; i < length; i += 4) {
            curr->msg[i] = '#';
        }
    }

    return length;
}

static int device_release(struct inode* inode, struct file* file) {
    kfree(file->private_data);
    return 0;
}

static int __init message_slot_init(void) {
    int retval;

    retval = register_chrdev(MAJOR_NUM, "message_slot", &fops);
    if (retval < 0) {
        return retval;
    }

    memset(slots, 0, sizeof(slots));

    return 0;
}

static void __exit message_slot_exit(void) {
    int i;
    msg_channel_t *curr, *next;
    for (i = 0; i < MINOR_NUMBER_MAX; i++) {
        if (slots[i] != NULL) {
            curr = slots[i]->channels;
            while (curr != NULL) {
                next = curr->next;
                kfree(curr);
                curr = next;
            }
            kfree(slots[i]);
        }
    }
    unregister_chrdev(MAJOR_NUM, "message_slot");
}

module_init(message_slot_init);
module_exit(message_slot_exit);