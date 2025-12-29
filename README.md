# 🐧 Linux Kernel Message Slot (IPC)

A custom **Linux Kernel Module** implementing a character device driver for Inter-Process Communication (IPC). This project demonstrates low-level systems programming, kernel memory management, and synchronization.

## 🚀 Key Features
* **Multi-Channel IPC:** Supports up to $2^{20}$ message channels per device file.
* **Kernel-Level Logic:** Implements a message censorship mechanism directly in the driver.
* **Atomic Operations:** Ensures thread-safe `read()` and `write()` operations for message integrity.
* **Custom IOCTL Interface:** Specialized interface for dynamic channel switching.
* **Efficient Memory Management:** Uses `kmalloc` with proper cleanup routines to prevent memory leaks in the kernel.

## 🛠️ Technical Stack
* **Language:** C (Kernel-space & User-space)
* **Frameworks:** Linux Kernel API
* **Build Tools:** Makefile, GCC

## 📁 Project Structure
* `message_slot.c / .h` - Core kernel module implementation.
* `message_sender.c` - User-space utility to send messages.
* `message_reader.c` - User-space utility to receive messages.
* `Makefile` - Automates the build process.

## 💻 How to Run
1. **Compile the project:**
   ```bash
   make
