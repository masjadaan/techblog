---
layout: single
title: "Solving Flag 32: Reading and Writing Handles"
date: 2026-06-28
categories: [Network Protocols]
tags: [bluetooth, ble, ctf, gatt, handles]
author_profile: true
toc: true
toc_sticky: true
read_time: true
show_date: true
---
![0fcba9213b27b74801190ebeaeb73553.png](../../_resources/0fcba9213b27b74801190ebeaeb73553.png)


Great job on the first read-based challenge! Now let's level up. **Flag 32** introduces a another concept: **writing data back to a GATT characteristic**. This is how you send commands, change settings, or—in our case—trigger a response that contains a flag.

The challenge instruction is simple:
> **"Read handle 0x0032 and do what it says."**

This means the handle itself contains the instructions for solving the challenge. Let's get started.

---

## Step 1: Connect and Read the Instruction

First, we need to see what the handle wants us to do.

1.  Connect to the device using `gatttool` in interactive mode:
```bash
gatttool -I -b C8:C9:A3:FA:F1:6A
[  ][LE]> connect
```

2.  Read the value of handle `0x0032`:
```bash
[  ][LE]> char-read-hnd 0x0032
Characteristic value/descriptor: 57 72 69 74 65 20 61 6e 79 74 68 69 6e 67 20 68 65 72 65
```

3.  Exit `gatttool` for a moment (type `quit`) or open a new terminal to decode this hex value using our script:
```bash
./hex_to_ascii.sh "577269746520616e797468696e672068657265"
Write anything here
```

**Perfect! The instruction is clear: "Write anything here".** The characteristic is waiting for us to send it some data.

---

## Step 2: Write to the Handle

Now we will perform a **GATT Write operation**. We need to send any data we want. The `gatttool` command for this is `char-write-req` (write request).

However, GATT operations require data to be sent in **hexadecimal format**. We need to convert our text into hex first.

**Example:** Let's write the word `Test`.
1.  Convert "Test" to hex. You can use Python, or use xxd
```bash
echo -n "Test" | xxd -ps
# 54657374
```

2.  Back in your `gatttool` interactive session, execute the write command:
```bash
[  ][LE]> char-write-req 0x0032 54657374
Characteristic value was written successfully
```

This command tells the device to write the hex value `54657374` ("Test") to handle `0x0032`.

> **Note:** The success message means the write operation was accepted by the device. It doesn't necessarily mean it was "correct"—in this case, *any* write is correct!

---

## Step 3: Read the Handle Again to Get the Flag

Writing to the handle triggers a change. The characteristic's value has now been updated to contain our flag.

1.  Read the same handle (`0x0032`) again:
```bash
[  ][LE]> char-read-hnd 0x0032
Characteristic value/descriptor: 33 38 37 33 63 30 32 37 30 37 36 33 35 36 38 63 66 37 61 61
```

2.  Decode this new hex string to find your flag:
```bash
./hex_to_ascii.sh "3338373363303237303736333536386366376161"
3873c0270763568cf7aa
```

**This string, `3873c0270763568cf7aa`, is the flag for this challenge.**

---

## Step 4: Submit the Flag and Check Your Score

1.  **Disconnect** from `gatttool` by typing `quit`. It's important to end your connection before trying to submit from another script.
2.  Use your `submit_flag.sh` script to send the flag you just found:
```bash
./submit_flag.sh C8:C9:A3:FA:F1:6A "3873c0270763568cf7aa"
```

3.  Verify your success by checking your updated score!
```bash
./get_score.sh C8:C9:A3:FA:F1:6A
# Score:3 /20
```

### Key Takeaways from This Challenge

1.  **Write Operations:** You learned how to use `char-write-req` to send data to a characteristic.
2.  **Data Conversion:** You practiced converting plain text to hexadecimal format for transmission.
3.  **Interactive Logic:** Some characteristics are interactive; writing to them changes their state or reveals new information. This is a very common pattern in IoT devices for sending commands.

You're now comfortable with the two most fundamental GATT operations: **Read** and **Write**. Well done! Keep this workflow in mind as you move to more complex challenges.