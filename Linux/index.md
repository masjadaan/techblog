---
layout: single
title: "Linux"
author_profile: true
toc: false
permalink: /Linux/
---

Articles covering Linux internals, binary exploitation, and persistence techniques.

## UserLand

| Article | Topics |
|---------|--------|
| [Hijacking Shared Libraries](UserLand/HijackingSharedLibraries/HijackingSharedLibraries) | LD_PRELOAD, shared objects, privilege escalation |
| [Have You Met The Dynamic Linker](UserLand/dynamicLinker/dynamicLinker) | ELF loading, runtime linking, ld.so |
| [ShellCodes](UserLand/shellcodes/shellcodes) | Assembly, syscalls, null-byte removal |
| [What is the password?](UserLand/whatIsPassword/WhatIsThePassword) | /etc/passwd, /etc/shadow, hashing |
| [Backdoor Via Vim](UserLand/vimHacks/BackdoorViaVim/Backdoor_via_vim) | .vimrc, persistence, autocommands |

## KernelLand

| Article | Topics |
|---------|--------|
| [Walking the Page Table](KernelLand/WalkingPageTable/WalkingPageTable) | x86_64 paging, virtual memory, kernel, GDB, QEMU |
| [Abusing modprobe_path](KernelLand/ModprobePath/modprobe) | modprobe_path, kernel module, privilege escalation, binfmt |

## Sandboxing

| Article | Topics |
|---------|--------|
| [chroot Part 1: How It Works and Setting Up a Jail](Sandboxing/01_chroot/chroot_p1) | chroot syscall, jail setup, BusyBox |
| [chroot Part 2: Escaping and Limitations](Sandboxing/01_chroot/chroot_p2) | chroot escapes, namespaces, cgroups, seccomp |
