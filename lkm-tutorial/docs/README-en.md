# LKM Tutorial – ioctl(_IO) basics

This repository is a **minimal Linux kernel module tutorial** used in the System Programming course.

The goal is very focused:

- build and load a simple out-of-tree kernel module,
- create a character device `/dev/tutorial`,
- issue a single `_IO` ioctl from user space,
- and verify the result via `dmesg`.

This project is **only** for the `_IO` tutorial.  
A separate project will be used for the actual assignment (with more ioctl types).

---

## 1. Layout

```text
lkm-tutorial/
  .build/             # build output (created by Makefile)
  include/
    abi.h             # shared ABI: device name, ioctl magic/number, etc.
  kernel/
    tutorial_kernel.c # kernel module implementation
    Makefile          # build for module binary
  user/
    main.c            # user-space test program
  Makefile            # top-level build (module + user binary)
```

At a glance:

- `include/abi.h`
  - `DEVICE_NAME`, `DEVICE_PATH`, `CLASS_NAME`
  - `IOCTL_MAGIC`
  - `IOCTL_HELLO` (`_IO`, no payload)

- `kernel/tutorial_kernel.c`
  - registers a char device (`alloc_chrdev_region`, `cdev_add`)
  - creates `/dev/tutorial` via `class_create` / `device_create`
  - implements `open`, `release`, `unlocked_ioctl`
  - on `IOCTL_HELLO`, prints a greeting to the kernel log

- `user/main.c`
  - opens `/dev/tutorial`
  - calls `ioctl(fd, IOCTL_HELLO)`
  - prints a simple “check dmesg” style message

---

## 2. Environment (recommended)

To avoid breaking your main OS, **use a Linux VM**.

This tutorial was developed and tested on:

- Ubuntu 24.04 (VM)
- Linux kernel **6.8.0-88-generic**

You can check your kernel version with:

```bash
uname -r
# e.g. 6.8.0-88-generic
```

Inside the VM, install the required packages:

```bash
sudo apt update
sudo apt install -y build-essential gcc make linux-headers-$(uname -r)
```

> **Note**
> - If `linux-headers-$(uname -r)` is missing, kernel module build/insmod will fail.
> - Some kernel APIs used in this tutorial (for example, the `class_create()` helper)
>   have changed across kernel versions. Around Linux 6.4, the signature was updated,
>   so if you build against an older kernel you may need to adjust certain calls
>   according to that kernel’s headers and documentation.

---

## 3. Build

From the project root:

```bash
make
```

If it succeeds, you should get:

- kernel module: `./.build/tutorial_kernel.ko`
- user program: `./.build/tutorial_user`

---

## 4. Load the module and check `/dev/tutorial`

### 4.1 Load the module (`insmod`)

```bash
sudo insmod .build/tutorial_kernel.ko
```

If there is no error, the module is now loaded into the kernel.

### 4.2 Check the device node

```bash
ls -l /dev/tutorial
```

You should see something like:

```text
crw-rw---- 1 root root <major>, <minor> /dev/tutorial
```

### 4.3 Inspect the kernel log (`dmesg`)

```bash
sudo dmesg | tail
```

On load, the module prints a short message:

```text
tutorial_kernel: module loaded (major=..., minor=...)
```

---

## 5. Test the `_IO` ioctl from user space

### 5.1 Run the user program

```bash
sudo ./.build/tutorial_user
```

Expected behavior:

- open `/dev/tutorial`
- issue `IOCTL_HELLO` (`_IO` ioctl with no payload)
- print a short hint to check `dmesg`

### 5.2 Verify the result in `dmesg`

```bash
sudo dmesg | tail
```

You should see something like:

```text
tutorial_kernel: device opened
tutorial_kernel: Hello from tutorial kernel module! :)
tutorial_kernel: device closed
```

This confirms that:

- `/dev/tutorial` is correctly hooked up to your module,
- `file_operations` is configured as expected,
- and the `_IO` ioctl path works end-to-end.

---

## 6. Unload the module and clean up

### 6.1 Unload the module (`rmmod`)

```bash
sudo rmmod tutorial_kernel
```

Then:

```bash
sudo dmesg | tail
```

You should see:

```text
tutorial_kernel: module unloaded
```

And `/dev/tutorial` should disappear:

```bash
ls -l /dev/tutorial
# → No such file or directory
```

### 6.2 Clean build artifacts

```bash
make clean
```

This will:

- clean kernel build artifacts under `kernel/`
- remove the `.build/` directory and its contents

(Remember: `rmmod` must be called before `make clean` if the module is still loaded.)

---

## 7. What you should learn from this tutorial

This project covers **only one ioctl** (`_IO`), but it is enough to see the full flow:

- how to:
  - allocate a device number (`alloc_chrdev_region`)
  - register a `cdev` (`cdev_add`)
  - create a `/dev` node (`class_create`, `device_create`)
- how `file_operations` connects `/dev/tutorial` to:
  - `open`, `release`, `unlocked_ioctl`
- how a user-space `ioctl()` call ends up in your `unlocked_ioctl` implementation,
  and how to verify it via `dmesg`.

This is intentionally minimal; it is meant as a warm-up before extending the ABI with
other ioctl types (`_IOR`, `_IOW`, `_IOWR`) in a separate assignment project.
