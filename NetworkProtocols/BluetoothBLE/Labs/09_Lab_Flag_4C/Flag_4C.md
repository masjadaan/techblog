---
layout: single
title: "Solving Flag 4C: Spoofing Your BD_ADDR"
date: 2026-06-28
categories: [Network Protocols]
tags: [bluetooth, ble, ctf, bd-addr, spoofing]
author_profile: true
toc: true
toc_sticky: true
read_time: true
show_date: true
---
![357b7b7af0a37610eff142248e75004a.png](../../_resources/357b7b7af0a37610eff142248e75004a.png)

This challenge, **Flag 4C**, introduces **spoofing your Bluetooth Device Address (BD_ADDR)**. Just like with Wi-Fi MAC addresses, changing your BD_ADDR can be essential for privacy testing, bypassing access control lists, or simulating different devices.

The challenge states:
> **"Check out handle 0x004c and do what it says. Much like ethernet or wifi devices, you can also change your bluetooth devices mac address."**

Let's see what the handle instructs us to do.

---

## Step 1: Read the Instruction

First, connect to the device and read the target handle to get the specific instruction.

1.  Connect to the device:
    ```bash
    gatttool -I -b C8:C9:A3:FA:F1:6A
    [  ][LE]> connect
    ```

2.  Read the value of handle `0x004C`:
    ```bash
    [  ][LE]> char-read-hnd 0x004C
    Characteristic value/descriptor: 43 6f 6e 6e 65 63 74 20 77 69 74 68 20 42 54 20 4d 41 43 20 61 64 64 72 65 73 73 20 31 31 3a 32 32 3a 33 33 3a 34 34 3a 35 35 3a 36 36
    ```

3.  Decode the hex to understand the instruction:
    ```bash
    ./hex_to_ascii.sh "436f6e6e6563742077697468204254204d414320616464726573732031313a323233333a34343a35353a3636"
    Connect with BT MAC address 11:22:33:44:55:66
    ```

**The challenge is clear: We must reconnect to the device, but we must spoof our MAC address to be `11:22:33:44:55:66`.**

---

## Step 2: Spoofing Your Bluetooth Address

Kali Linux provides the `btmgmt` tool, which is a powerful utility for managing Bluetooth interfaces. We will use it to change the public address of our Bluetooth adapter.

**Important:** You need to know the name of your Bluetooth interface. It is typically `hci0`, but if you have multiple adapters, it could be `hci1`, etc. We'll assume `hci0` is your primary interface.

1.  **Power off the interface** to make changes:
    ```bash
    sudo btmgmt -i hci0 power off
    ```

2.  **Change the public address** to the one specified in the challenge (`11:22:33:44:55:66`):
    ```bash
    sudo btmgmt -i hci0 public-addr 11:22:33:44:55:66
    ```

3.  **Power the interface back on**:
    ```bash
    sudo btmgmt -i hci0 power on
    ```

> **Note:** Some Bluetooth adapters have hardware restrictions and may not allow their public address to be changed. If this command fails, you may need to use a different adapter that supports address spoofing (many common USB dongles do).

---

## Step 3: Reconnect with the Spoofed Address

Now that your machine is presenting the spoofed MAC address, you need to reconnect to the CTF device.

1.  Start a new `gatttool` session. Since we've changed our identity, the tool will use the new spoofed address automatically.
    ```bash
    gatttool -I -b C8:C9:A3:FA:F1:6A
    [  ][LE]> connect
    ```

2.  Once connected, **read the same handle (`0x004C`) again**. The value will have changed because the server recognized your new spoofed address.
    ```bash
    [  ][LE]> char-read-hnd 0x004C
    Characteristic value/descriptor: 61 63 61 31 36 39 32 30 35 38 33 65 34 32 62 64 63 66 35 66
    ```

3.  Decode this new hex value to get the flag:
    ```bash
    ./hex_to_ascii.sh "6163613136393230353833653432626463663566"
    aca16920583e42bdcf5f
    ```

---

## Step 4: Submit the Flag

1.  Exit `gatttool`.
2.  Submit the flag you discovered:
    ```bash
    ./submit_flag.sh C8:C9:A3:FA:F1:6A "aca16920583e42bdcf5f"
    ```
3.  Check your score to confirm success:
    ```bash
    ./get_score.sh C8:C9:A3:FA:F1:6A
    # Score:6 /20
    ```

### Resetting Your Bluetooth Address (Optional)

After completing the challenge, you may want to revert to your adapter's real MAC address.

```bash
sudo btmgmt -i hci0 power off
sudo btmgmt -i hci0 public-addr <your_real_mac_address> # Or simply disconnect and reconnect the adapter
sudo btmgmt -i hci0 power on