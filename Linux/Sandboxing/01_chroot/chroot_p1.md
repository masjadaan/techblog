# `chroot`: An Early Sandboxing Technique, Part 1: How It Works and Setting Up a Jail

Sandboxing means running untrusted code or data in a controlled environment with limited privileges. The idea is to reduce the impact if that process is compromised. On Unix-like systems, one of the earliest techniques for doing this was `chroot`.

**Contents**

1. [Introduction to `chroot`](#1-introduction-to-chroot)
2. [How `chroot` Works](#2-how-chroot-works)
   - 2.1 [The `chroot()` Syscall](#21-the-chroot-syscall)
   - 2.2 [The `chroot` Command-Line Utility](#22-the-chroot-command-line-utility)
3. [Setting Up a Basic chroot Jail](#3-setting-up-a-basic-chroot-jail)
   - 3.1 [Preparing the Jail Directory](#31-preparing-the-jail-directory)
   - 3.2 [Running `chroot`](#32-running-chroot)
   - 3.3 [A Simpler Jail with Static BusyBox](#33-a-simpler-jail-with-static-busybox)

## 1. Introduction to `chroot`

Let's start by looking at where `chroot` came from. It was first included in Unix in 1979 and, for many years, was the accepted way to limit a process's access to the filesystem. The process was placed inside a restricted directory tree, commonly referred to as a **chroot jail**.

From the process's point of view, the directory selected as the root becomes its new filesystem root. So, as we work through this, keep in mind that the process can no longer move past that new root through the normal filesystem hierarchy.

## 2. How `chroot` Works

### 2.1 The `chroot()` Syscall

Let's take a closer look at what actually happens when we use the `chroot()` system call.

If a process calls:

```c
chroot("/tmp/jail");
```

there are two main effects we need to understand.

* **It modifies the filesystem root of the process**. Absolute paths are now resolved relative to `/tmp/jail`. So, from the process's perspective, `/flag` actually refers to `/tmp/jail/flag`.

* **It changes how `..` behaves at the new root.** Once the process reaches `/tmp/jail`, trying to move to the parent with `/tmp/jail/..` does not take it back into the original filesystem hierarchy. The new root is treated as the top of the filesystem tree visible to that process.

Now, let's check what `chroot()` does **not** do, because this is where many of its security limitations become important:

* It **does not** close resources that were already opened outside the jail, including open files and sockets.
* It **does not** change the process's current working directory. The filesystem root changes, but `chdir("/")` must be performed separately.
* It **does not** change the process's privileges.
* It **does not** restrict network access.
* It **does not** isolate processes from one another.
* It **does not** isolate other system resources.

So, looking at the bigger picture, `chroot()` only changes how the calling process interprets filesystem paths. It does not provide complete privilege separation or process isolation. As we'll see later, many of the escape techniques and security problems come directly from this limited scope.

### 2.2 The `chroot` Command-Line Utility

Now let's see what happens when we use the command-line version instead.

On Linux, the same functionality is available through `/usr/sbin/chroot`. This utility acts as a userspace wrapper around three separate operations:

1. It calls the `chroot()` syscall to change the process's filesystem root.
2. It calls `chdir("/")` to move the current working directory into the new root.
3. It calls `execve()` to launch the requested program.

This explains why the command-line utility appears to completely "enter" the jail, while calling the raw syscall alone does not. Changing the root directory and changing the current working directory are two separate operations, and the wrapper handles both for us.

## 3. Setting Up a Basic chroot Jail

Before we try to enter a jail, we need to remember one important point: a chroot jail is only as usable as the files we place inside it.

Since the process can no longer see anything outside the new root, every binary, shared library, and device node it needs must be copied or created inside the jail first.

### 3.1 Preparing the Jail Directory

Let's build a basic jail step by step.

1. **Create the jail's directory structure**, mirroring the parts of a normal filesystem that the jailed process will expect:

   ```sh
   mkdir -p /tmp/jail/{bin,lib,lib64,dev}
   ```

2. **Copy the binary** we want to run inside the jail. In this case, we'll use `bash`:

   ```sh
   cp /bin/bash /tmp/jail/bin/
   ```

3. **Copy its shared library dependencies.** We can use `ldd` to see what `bash` needs:

   ```sh
   $ ldd /bin/bash
        linux-vdso.so.1 (0x00007ffe2d396000)
        libtinfo.so.6 => /lib/x86_64-linux-gnu/libtinfo.so.6 (0x00007fb3268f7000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fb3266ce000)
        /lib64/ld-linux-x86-64.so.2 (0x00007fb326a95000)
   ```

   Looking at the output, we can see that `linux-vdso.so.1` is not an actual file. It is a virtual page that the kernel maps into every process, so there is nothing for us to copy.

   The other three dependencies, however, do need to be available inside the jail:

   ```sh
   cp /lib/x86_64-linux-gnu/libtinfo.so.6 /tmp/jail/lib/
   cp /lib/x86_64-linux-gnu/libc.so.6 /tmp/jail/lib/
   cp /lib64/ld-linux-x86-64.so.2 /tmp/jail/lib64/
   ```

   Without this step, the binary will fail to start inside the jail with a "not found" or dynamic linker error because it can no longer see the real `/lib` directory on the host.

4. **Create any device nodes the program needs**, such as `/dev/null`:

   ```sh
   sudo mknod -m 666 /tmp/jail/dev/null c 1 3
   ```

   Notice that this step is different from creating directories or copying files. Creating a device node requires the `CAP_MKNOD` capability, which is why we need to run this command as root.

### 3.2 Running `chroot`

At this point, we have populated the jail, so let's see what happens when we enter it using the command-line utility.

The `chroot()` operation requires the `CAP_SYS_CHROOT` capability, which regular unprivileged users do not have. That means we need to run it with `sudo`:

```sh
student@kernel-sec:/tmp$ sudo chroot /tmp/jail /bin/bash
bash-5.1# pwd
/
bash-5.1# cd ../
bash-5.1# pwd
/
bash-5.1# echo "hello from chroot"
hello from chroot
```

Looking at the output, notice that `/` is now the root of our jail. Even when we run `cd ../`, the working directory remains `/`.

Also, pay attention to the `/bin/bash` argument. This path is resolved **inside** the new root. In other words, it refers to `/tmp/jail/bin/bash`, which is the copy we placed inside the jail, rather than the original `/bin/bash` on the host.

Under the hood, the utility calls `chroot()` on `/tmp/jail`, changes the working directory to `/`, and then uses `execve()` to start the in-jail binary.

So, once we're inside the resulting shell, `/` represents what used to be `/tmp/jail`, and only the files we've placed there are visible.

### 3.3 A Simpler Jail with Static BusyBox

Now let's look at a simpler approach.

The `ldd`-and-copy process we followed above is necessary because `bash` is dynamically linked. A statically linked binary does not depend on shared libraries, which means we can place it inside the jail as a single file.

This is where [BusyBox](https://busybox.net) becomes useful. BusyBox provides a single static binary containing dozens of common Unix utilities, including `ls`, `cat`, `sh`, `mv`, and many others, which it exposes as "applets."

Let's build a simpler jail using it.

1. Download a static BusyBox build and confirm that it has no dynamic dependencies:

   ```sh
   wget https://busybox.net/downloads/binaries/1.21.1/busybox-x86_64
   
   ```
   > Any recent static build from the [BusyBox downloads page](https://busybox.net/downloads/binaries/) works equally well; the version pinned here is just an example.

   Looking at the `file` output, the important part is "statically linked." This tells us that we don't need to perform the `ldd` and library-copying process we needed for `bash`.
   
   ```sh
   file busybox-x86_64
   busybox-x86_64: ELF 64-bit LSB executable, x86-64, version 1 (SYSV), statically linked, stripped
   ```

2. Copy it into the jail and make it executable:

   ```sh
   mv busybox-x86_64 /tmp/jail/busybox
   chmod +x /tmp/jail/busybox
   ```

3. Use BusyBox's built-in installer to populate `/bin` inside the jail with a symlink for each supported applet:

   ```sh
   sudo chroot /tmp/jail /busybox --install -s /bin
   ```

4. Now let's start a shell inside the jail:

   ```sh
   sudo chroot /tmp/jail /busybox sh
   / # ls
   bin       busybox   dev       lib       lib64
   / #
   ```

As we can see, this jail is much more convenient than the bash-only version. Instead of having to copy individual utilities and their dependencies, we have a working `ls`, `cat`, `mv`, `grep`, and many other commands, all provided through one downloaded static binary.

---

In [Part 2: Escaping and Limitations](chroot_p2.md), we will see why a `chroot` jail is not a real security boundary, how a privileged process can break out of it, and which modern kernel features replace it.
