# `chroot`: An Early Sandboxing Technique, Part 2: Escaping and Limitations

Part 1 covered how `chroot()` works and how to set up a basic jail, first with `bash` and its shared libraries, then with a static BusyBox binary. This part looks at why `chroot` should not be treated as a strong security boundary: the ways a jailed process can escape it, everything it fails to isolate, and the modern kernel features that replace it.

**Contents**

1. [Escaping chroot](#1-escaping-chroot)
   - 1.1 [chroot Without chdir](#11-chroot-without-chdir)
   - 1.2 [Previously Open File Descriptors](#12-previously-open-file-descriptors)
   - 1.3 [Re-chrooting](#13-re-chrooting)
2. [What chroot Doesn't Isolate](#2-what-chroot-doesnt-isolate)
   - [No PID Isolation](#no-pid-isolation)
   - [No Network Isolation](#no-network-isolation)
   - [No IPC Isolation](#no-ipc-isolation)
   - [No Syscall Filtering](#no-syscall-filtering)
3. [Why chroot Fell Out of Favor & Modern Replacements](#3-why-chroot-fell-out-of-favor--modern-replacements)
4. [Conclusion](#4-conclusion)

## 1. Escaping chroot

Now that we've seen how `chroot` works, let's look at why it should not be considered a strong security boundary.

The key point to remember is that `chroot()` only changes the meaning of `/`. It does not change everything else about the process. The escape techniques we're about to examine take advantage of exactly those things that `chroot()` leaves untouched.

### 1.1 chroot Without chdir

Let's start with one of the simplest mistakes. If a program calls the raw `chroot()` syscall but never follows it with `chdir("/")`, the process's current working directory remains wherever it was before the call. That location can still be outside the new root.

Because **relative paths are resolved from the current working directory rather than the process's root**, the process may still be able to access files outside the jail.

The [`escape.c`](PoC/escape.c) minimal program demonstrates this. It does only two things:

1. Calls `chroot("/tmp/jail")` and, deliberately, does **not** follow it with `chdir("/")`.
2. Opens the relative path `../../../etc/passwd`, and tries to read the first line

Note that `chroot()` requires root privileges, so the program must be run with `sudo`. Let's compile and run it:

```sh
student@kernel-sec:/tmp$ gcc -o escape escape.c
student@kernel-sec:/tmp$ sudo ./escape 
Escaped the jail, read: root:x:0:0:root:/root:/bin/bash
```

Looking at the result, we can see that the program successfully read the real `/etc/passwd` file.

Why did that happen? **Because `chdir("/")` was never called**. The current working directory was still the host directory where the program was launched, so `../../../etc/passwd` could walk back through that outside path and reach the real `/etc/passwd`.

The `/usr/sbin/chroot` command-line utility avoids this particular problem by calling `chdir("/")` immediately after `chroot()`. However, any program that directly uses the raw syscall, such as a custom sandboxing tool, has to perform that step itself.

A single missing `chdir("/")` can therefore leave a supposed `chroot` jail with no meaningful filesystem isolation.

### 1.2 Previously Open File Descriptors

Let's move on to another important weakness. As we saw earlier, `chroot()` does not close file descriptors that were opened before the syscall. If a file descriptor already points to a directory outside the jail, it continues to point there even after the process is chrooted.

This becomes particularly interesting when we look at the `*at` family of syscalls, such as `openat()` and `execveat()`. These syscalls can resolve relative paths using a directory file descriptor, known as a `dirfd`, rather than relying on the process's root or current working directory:

```c
int openat(int dirfd, const char *pathname, int flags);
```

So, let's consider what happens if a process opens a directory outside the jail before calling `chroot()`. It can later pass that `dirfd` to `openat()` and resolve paths relative to the outside directory.

The [`escape_fd.c`](PoC/escape_fd.c) minimal program demonstrates this. It does only three things:

1. Opens `/` with `open()` **before** entering the jail, keeping the resulting file descriptor (`outside_fd`) that points to the real host root.
2. Calls `chroot("/tmp/jail")` followed by `chdir("/")`, so, unlike the previous example, the jail is entered correctly.
3. Calls `openat(outside_fd, "etc/passwd", O_RDONLY)` and reads the first lines of the file.

Let's compile and run it:

```sh
gcc escape_fd.c -o escape_fd
sudo ./escape_fd 
Escaped the jail, read: root:x:0:0:root:/root:/bin/bash
daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
bin:x:2:2:bin:/bin:/usr/sbin/nologin
```

Looking at this output, we can see that the process reached the real `/etc/passwd` even though `chdir("/")` was correctly called.

The reason is the `outside_fd` file descriptor. **It was opened before `chroot()` and still refers to the unrestricted host root**. When that descriptor is passed to `openat()`, the path `etc/passwd` is resolved relative to that outside directory.

The same principle applies to many other `*at` syscalls, including `fstatat()`, `unlinkat()`, and `renameat()`. Once an outside `dirfd` has been captured, those syscalls can operate relative to it.

### 1.3 Re-chrooting

Now let's look at another escape technique: re-chrooting. **The kernel does not keep track of the fact that a process has already been chrooted**. If the process calls `chroot()` again, the kernel simply changes its root to the new location. It does not treat the original jail boundary as a permanent security boundary.

When combined with the fact that `chroot()` does not change the current working directory, this can allow a process to escape completely.

Let's walk through the technique, assuming the process is already inside a jail and has correctly called both `chroot()` and `chdir("/")`. The [`rechroot.c`](PoC/rechroot.c) program implements it, and it is meant to be run *inside* the jail. The program does the following:

1. It creates a subdirectory inside the current jail, for example `foo`.

2. It then calla `chroot("foo")`. The process's root is now `/foo`, but its current working directory still points to the *old* root, which is now outside the new, narrower root.

   ```text
   /
   └─ tmp/
       └─ jail/     <- process root BEFORE step 2 (and where cwd still points)
           └─ foo/  <- process root AFTER step 2 (chroot("foo"))

   process root ------------> /tmp/jail/foo
   process cwd  ------------> /tmp/jail   (outside the new root!)
   ```

3. Now, it repeatedly runs `chdir("..")`. Because the current working directory was never inside the new root's subtree, walking upward does not encounter the boundary that normally stops traversal at the process's root. The process can continue climbing into the real host filesystem.

4. Finally, it calls `chroot(".")`. The process's root is now set to the location where the current working directory points, which is the real host root. `rechroot.c` then calls `execl("/bin/sh", ...)` to hand control to a shell running with that root.

Let's compile and run it:

```sh
gcc -o rechroot rechroot.c
mv rechroot /tmp/jail/
sudo chroot /tmp/jail /bin/bash
bash-5.1# ./rechroot
/ # cat /etc/passwd
root:x:0:0:root:/root:/bin/bash
```

Looking at the final output, we can see that `/etc/passwd` now contains the real host file rather than returning a "not found" error. This confirms that the process is no longer confined to `/tmp/jail`.

This escape depends on one precondition: the `chroot()` syscall requires the `CAP_SYS_CHROOT` capability, which in practice means the calling process generally needs to be running as root or have that capability. That is why a process with an effective user ID of 0 inside a chroot jail can often break out this way, even if the jail was otherwise set up correctly, with no leftover file descriptors and `chdir("/")` called as it should be. If the process cannot call `chroot()` again, or otherwise use root-level privileges, this particular escape route is closed.


## 2. What chroot Doesn't Isolate

Even if we ignore the escape techniques we've just covered, there is another major limitation to keep in mind.

`chroot` was designed to restrict one specific thing: filesystem path resolution. Everything else remains shared with the host. The BusyBox jail we created in part one makes this easy to demonstrate.

### No PID Isolation

Let's first look at process isolation. A jailed process can still see and signal processes running on the host because `chroot` has no concept of process visibility.

The `ps` command reads process information from `/proc`, so let's mount it inside the jail:

```sh
sudo mkdir -p /tmp/jail/proc
sudo mount -t proc proc /tmp/jail/proc
```

Now, on the host, start a long-running process and note its PID:

```sh
sleep 9999 &
echo $!
1664
```

Next, let's enter the jail and check whether that process is visible:

```sh
sudo chroot /tmp/jail /busybox sh
/ # ps | grep sleep
 1664 1000       0:00 sleep 9999
 1673 0          0:00 grep sleep
   ...
/ # kill 1664
```

Looking at the output, we can see that the `sleep` process, even though it was started outside the jail, is visible from inside it.

We can also verify the result by exiting back to the host:

```sh
/ # exit
student@kernel-sec:/tmp$
[1]+  Terminated              sleep 9999
```

The host's job control confirms that the process was terminated from inside the jail. So, clearly, `chroot` provides no PID isolation.


### No Network Isolation

Let's check the network side next. A jailed process shares the host's network stack and interfaces. We can compare the host's interfaces with what we see inside the jail:

```sh
# on Host
student@kernel-sec:/tmp$ ifconfig
enp0s3: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 10.0.2.15  netmask 255.255.255.0  broadcast 10.0.2.255
lo: flags=73<UP,LOOPBACK,RUNNING>  mtu 65536
        inet 127.0.0.1  netmask 255.0.0.0

# Inside chroot
sudo chroot /tmp/jail /busybox sh
/ # ifconfig
enp0s3    Link encap:Ethernet  HWaddr 08:00:27:D9:02:8C
          inet addr:10.0.2.15  Bcast:10.0.2.255  Mask:255.255.255.0
lo        Link encap:Local Loopback
          inet addr:127.0.0.1  Mask:255.0.0.0
```

Looking at both outputs, we can see that the interfaces and IP addresses are identical. There is no separate network namespace here, so the jail is using the same network environment as the host.

### No IPC Isolation

Now let's check inter-process communication. Shared memory, message queues, and semaphores are not isolated by `chroot`. POSIX shared memory objects live in `/dev/shm`, which is simply a `tmpfs` mount.

We can demonstrate this by bind-mounting `/dev/shm` into the jail. First, on the host, run:

```sh
sudo mkdir -p /tmp/jail/dev/shm
sudo mount --bind /dev/shm /tmp/jail/dev/shm
echo "shared" > /dev/shm/msg
```

Next, let's enter the jail and read the file:

```sh
sudo chroot /tmp/jail /busybox sh
/ # cat /dev/shm/msg
shared
```

As we can see from the output, the jailed process is able to read data that was created outside the jail.

### No Syscall Filtering

Finally, let's look at system calls. `chroot` does not restrict which syscalls a process can make. Anything outside filesystem path resolution remains available, subject to the process's existing privileges.

That includes operations involving mechanisms such as `ptrace()` and, where sufficient privileges exist, loading kernel modules.

The re-chroot escape we examined earlier is itself a good demonstration. `chroot()` is just another syscall, and being inside a chroot jail does not automatically prevent a process from calling it again.

None of this is necessarily a flaw in the original design, because `chroot` was never intended to isolate all of these resources.

However, as we've seen throughout this section, being "chrooted" is not the same thing as being fully "sandboxed." A process confined only by `chroot` can still see, signal, communicate with, and potentially interfere with other processes and resources on the machine.

## 3. Why chroot Fell Out of Favor & Modern Replacements

At this point, the limitations should be clear. Filesystem isolation by itself is not enough to safely run untrusted code. `chroot` is still useful in narrow, non-adversarial situations, such as build environments or legacy service confinement, but it is no longer considered a sufficient general-purpose security boundary.

Modern Linux systems instead use several kernel features, with each one handling a different part of the isolation problem:

* **namespaces** isolate resources that `chroot` does not, including PIDs, networking, IPC, and mounts.
* **cgroups** limit and account for resources such as CPU, memory, and I/O, helping prevent a sandboxed process from exhausting the host.
* **seccomp** restricts the system calls that a process is allowed to make.

Container runtimes also typically replace `chroot` itself with `pivot_root`, a related syscall that swaps the process's root filesystem within a new mount namespace rather than simply narrowing path resolution within the existing one. This gives cleaner isolation and avoids some of the escape routes discussed in Section 1.

When we combine these technologies, we get the foundation of modern container technology, including Docker, LXC, and similar tools.

## 4. Conclusion

Let's bring everything together. `chroot` introduced an important early approach to sandboxing, but its scope has always been limited to filesystem path resolution. It does not isolate processes, networking, IPC, or system calls, and certain configurations can also allow the process to escape the filesystem boundary entirely.

This is why a `chroot` environment should not be treated as a complete security boundary, especially when dealing with untrusted code. Modern Linux isolation mechanisms, particularly namespaces, cgroups, and seccomp, exist to cover the areas that `chroot` was never designed to handle.
