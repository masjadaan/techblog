# Solving Flag 40: Subscribing to Notifications
* * *
![4e48ebd992a8ae070469493e4ea809a2.png](../../../../../_resources/4e48ebd992a8ae070469493e4ea809a2.png)

## Unlocking Server-Initiated Communication

Welcome to a new BLE concept! **Flag 40** introduces **notifications**, a powerful feature where the GATT Server (the ESP32) can push data to the Client (your machine) without being asked. This is essential for real-time data like sensor readings.

The challenge instructs us to:
> **"Check out handle 0x0040 and google search gatt notify."**

Let's discover what this means and how to enable this feature.

---

## Step 1: Connect and Read the Instruction

First, let's see what the handle is telling us to do.

1.  Connect to the device:
    ```bash
    gatttool -I -b C8:C9:A3:FA:F1:6A
    [  ][LE]> connect
    ```

2.  Read the value of handle `0x0040`:
    ```bash
    [  ][LE]> char-read-hnd 0x0040
    Characteristic value/descriptor: 4c 69 73 74 65 6e 20 74 6f 20 6d 65 20 66 6f 72 20 61 20 73 69 6e 67 6c 65 20 6e 6f 74 69 66 69 63 61 74 69 6f 6e
    ```

3.  Decode the hex to understand the instruction:
    ```bash
    ./hex_to_ascii.sh "4c697374656e20746f206d6520666f7220612073696e676c65206e6f74696669636174696f6e"
    Listen to me for a single notification
    ```

**The message is clear: This characteristic is designed to send us data via a notification. Our job is to listen for it.**

---

## Step 2: Understanding the CCCD

To receive notifications, we can't just read the handle. We must first **subscribe** to them. This is done by writing to a special setting called the **Client Characteristic Configuration Descriptor (CCCD)**.

*   **What is the CCCD?** It's a special descriptor (a type of attribute) that acts as a switch for notifications and indications.
*   **How does it work?** The client writes a specific value to the CCCD to tell the server, "Yes, please send me notifications."
    *   Write `0x0100` to **enable notifications**.
    *   Write `0x0200` to **enable indications** (which require acknowledgment).
    *   Write `0x0000` to disable both.

> **Important Note:** In many stacks, the CCCD is located at the handle **immediately following** the characteristic value handle. For handle `0x0040`, the CCCD is very likely at handle `0x0041`. However, some implementations, including this CTF, often allow you to enable notifications by writing directly to the value handle with a special command. The `gatttool` command `--listen` or `--char-write-req` with the value `0100` handles this abstraction for us.

---

## Step 3: Enable Notifications and Receive the Flag

We will use `gatttool` to write the magic value `0x0100` to the handle, which enables notifications.

1.  In your active `gatttool` session, execute the write command:
    ```bash
    [  ][LE]> char-write-req 0x0040 0100
    Characteristic value was written successfully
    ```

2.  Immediately after enabling notifications, the server will send a single data packet. `gatttool` will display it automatically:
    ```
    Notification handle = 0x0040 value: 35 65 63 33 37 37 32 62 63 64 30 30 63 66 30 36 64 38 65 62
    ```

3.  This hex string is our flag! Decode it:
    ```bash
    ./hex_to_ascii.sh "3565633337373262636430306366303664386562"
    5ec3772bcd00cf06d8eb
    ```

---

## Step 4: Submit the Flag

1.  Exit `gatttool` by typing `quit`.
2.  Use your submission script to send the flag you received via notification:
    ```bash
    ./submit_flag.sh C8:C9:A3:FA:F1:6A "5ec3772bcd00cf06d8eb"
    ```
3.  Check your score to confirm success:
    ```bash
    ./get_score.sh C8:C9:A3:FA:F1:6A
    # Score:4 /20
    ```

### Key Takeaways from This Challenge

1.  **Notifications:** You learned how servers can push data to clients asynchronously, which is vital for efficient IoT communication.
2.  **Subscription Model:** You implemented the subscription model by writing to the CCCD (conceptually) to enable notifications.
3.  **Real-World Application:** This is exactly how a heart rate monitor continuously sends your heartbeat to a phone app without the app having to constantly ask for it.

You've now mastered the third core GATT operation: ** subscribing to notifications**. This is a huge step forward in your BLE hacking journey!



##### Resource
https://academy.nordicsemi.com/courses/bluetooth-low-energy-fundamentals/lessons/lesson-4-bluetooth-le-data-exchange/topic/services-and-characteristics/