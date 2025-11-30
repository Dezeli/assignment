# Hello Kernel – Assignment README 

This repository contains the code for the **Hello Kernel** assignment in the System Programming course.  
This README focuses only on how to build, load, run, and unload the kernel module.

> It is strongly recommended to run this on a **Linux VM / test environment**, not on your main OS.

---

## 1. Build

From the project root:

```bash
make
```

If the build succeeds, you should see (example):

- kernel module: `./.build/hello_kernel.ko`
- user program: `./.build/hello_user`

---

## 2. Insert the kernel module

After building, load the module into the kernel:

```bash
sudo insmod .build/hello_kernel.ko
```

Check the kernel log:

```bash
sudo dmesg | tail
```

You should see a message indicating that the module was loaded (e.g., `module loaded`).

---

## 3. Check the device node

If the module creates a character device such as `/dev/hello_kernel`:

```bash
ls -l /dev/hello_kernel
```

You should see something like:

```text
crw-rw---- 1 root root <major> <minor> /dev/hello_kernel
```

(The exact device name/permissions depend on your implementation.)

---

## 4. Run the user program

Assuming the user program was built as `.build/hello_user`:

```bash
sudo ./.build/hello_user
```

- The program will send `ioctl()` or other requests to the kernel module.
- See the assignment description and source code for the exact behavior.

Check the kernel log again:

```bash
sudo dmesg | tail
```

- Verify that the kernel side printed the expected messages in response to the user program.

---

## 5. Remove the kernel module

When you are done, remove the module from the kernel:

```bash
sudo rmmod hello_kernel
sudo dmesg | tail
```

You should see a message indicating that the module was unloaded.

Also check that the device node is gone:

```bash
ls -l /dev/hello_kernel
# → No such file or directory
```

---

## 6. Clean build artifacts

To remove all build outputs:

```bash
make clean
```

This will remove:

- intermediate kernel build files
- the `.build/` directory and its contents

(If the module is still loaded, run `sudo rmmod hello_kernel` first, then `make clean`.)
