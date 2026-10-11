---
layout: single
title: "Tech Security Blog"
author_profile: true
toc: false
---

Articles on Linux internals, cryptography attacks, network protocols, and hardware debug interfaces — all for educational purposes.

---

## Linux

### UserLand

| Article | Topics |
|---------|--------|
| [Hijacking Shared Libraries](Linux/UserLand/HijackingSharedLibraries/HijackingSharedLibraries) | LD_PRELOAD, shared objects, privilege escalation |
| [Have You Met The Dynamic Linker](Linux/UserLand/dynamicLinker/dynamicLinker) | ELF loading, runtime linking, ld.so |
| [ShellCodes](Linux/UserLand/shellcodes/shellcodes) | Assembly, syscalls, null-byte removal |
| [What is the password?](Linux/UserLand/whatIsPassword/WhatIsThePassword) | /etc/passwd, /etc/shadow, hashing |
| [Backdoor Via Vim](Linux/UserLand/vimHacks/BackdoorViaVim/Backdoor_via_vim) | .vimrc, persistence, autocommands |

### KernelLand

| Article | Topics |
|---------|--------|
| [Walking the Page Table](Linux/KernelLand/WalkingPageTable/WalkingPageTable) | x86_64 paging, virtual memory, kernel, GDB, QEMU |
| [Abusing modprobe_path](Linux/KernelLand/ModprobePath/modprobe) | modprobe_path, kernel module, privilege escalation, binfmt |

### Sandboxing

| Article | Topics |
|---------|--------|
| [chroot Part 1: How It Works and Setting Up a Jail](Linux/Sandboxing/01_chroot/chroot_p1) | chroot syscall, jail setup, BusyBox |
| [chroot Part 2: Escaping and Limitations](Linux/Sandboxing/01_chroot/chroot_p2) | chroot escapes, namespaces, cgroups, seccomp |

---

## Cryptography

| Article | Topics |
|---------|--------|
| [CBC Bit-Flipping Attack](Cryptography/CBCBitFlipAttack/CBCBitFlipAttack) | CBC mode, XOR manipulation, block ciphers |
| [Oracle Padding Attack](Cryptography/OraclePaddingAttack/oraclePaddingAttack) | PKCS#7, padding oracles, CBC decryption |

---

## Network Protocols

| Article | Topics |
|---------|--------|
| [CAN Frames Spoofing](NetworkProtocols/CANBus/FrameSpoofing/SpoofingCANFrames) | CAN bus, automotive, frame injection |
| [Bluetooth Low Energy (BLE)](NetworkProtocols/BluetoothBLE/) | BLE protocol, GATT, sniffing, CTF challenges |

---

## Debug Interfaces

| Article | Topics |
|---------|--------|
| [JTAG Pinout Identification](DebugInterfaces/JTAG/JTAG%20Identification) | JTAG, JTAGulator, multimeter, embedded |

---

> **Disclaimer:** All articles are for educational purposes only.
