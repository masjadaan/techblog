# Solving Flag 42: Subscribing to Indications
* * *

![3fe46b22b5c1ef14bde7088190851f7c.png](../../../../../_resources/3fe46b22b5c1ef14bde7088190851f7c.png)

## Mastering Acknowledged Data Push

Excellent work on handling notifications! Now we're moving to its more reliable sibling: **Indications**. This challenge, **Flag 42**, builds directly on the previous concept but introduces a crucial difference—guaranteed delivery.

The challenge instructs us to:
> **"Check out handle 0x0042 and google search gatt indicate."**

Let's see how indications differ from notifications and how to receive them.

---

## Step 1: Connect and Read the Instruction

First, let's decode the message at the specified handle to get our instructions.

1.  Connect to the device:
    ```bash
    gatttool -I -b C8:C9:A3:FA:F1:6A
    [  ][LE]> connect
    ```

2.  Read the value of handle `0x0042`:
    ```bash
    [  ][LE]> char-read-hnd 0x0042
    Characteristic value/descriptor: 4c 69 73 74 65 6e 20 74 6f 20 68 61 6e 64 6c 65 20 30 78 30 30 34 34 20 66 6f 72 20 61 20 73 69 6e 67 6c 65 20 69 6e 64 69 63 61 74 69 6f 6e
    ```

3.  Decode the hex to understand the instruction:
    ```bash
    ./hex_to_ascii.sh "4c697374656e20746f2068616e646c652030783030343420666f7220612073696e676c6520696e6469636174696f6e"
    Listen to handle 0x0044 for a single indication
    ```

**The message is specific: We need to listen for an indication on handle `0x0044`.**

---

## Step 2: Notifications vs. Indications

Before we proceed, let's clarify the key difference between the two server-initiated operations:

| Feature | Notification | Indication |
| :--- | :--- | :--- |
| **Acknowledgment** | ❌ No | ✅ **Yes** |
| **Reliability** | **Best-effort** (may get lost) | **Guaranteed** |
| **Use Case** | High-frequency, non-critical data (e.g., sensor readings) | Critical data that must be received (e.g., a critical alert, a command confirmation) |
| **CCCD Value** | `0x0100` | `0x0200` |

An **Indication** requires the client to send an acknowledgment back to the server upon receipt. This creates a handshake that guarantees the data was delivered.

---

## Step 3: Enable Indications and Receive the Flag

The instruction tells us to listen to handle `0x0044`. This is the handle of the characteristic that *sends* the indication. To subscribe, we write the value `0x0200` to its **CCCD**.

1.  In your active `gatttool` session, execute the write command to enable indications on handle `0x0044`:
    ```bash
    [  ][LE]> char-write-req 0x0044 0200
    Characteristic value was written successfully
    ```

2.  After enabling indications, the server will send a data packet. Because it's an **indication**, `gatttool` will automatically respond with an acknowledgment and then display the data to you:
    ```
    Indication   handle = 0x0044 value: 63 37 62 38 36 64 64 31 32 31 38 34 38 63 37 37 63 31 31 33
    ```

3.  This hex string is our flag! Decode it:
    ```bash
    ./hex_to_ascii.sh "6337623836646431323138343863373763313133"
    c7b86dd121848c77c113
    ```

---

## Step 4: Submit the Flag

1.  Exit `gatttool` by typing `quit`.
2.  Use your submission script to send the flag you received via indication:
    ```bash
    ./submit_flag.sh C8:C9:A3:FA:F1:6A "c7b86dd121848c77c113"
    ```
3.  Check your score to confirm success:
    ```bash
    ./get_score.sh C8:C9:A3:FA:F1:6A
    # Score:5 /20
    ```

### Key Takeaways from This Challenge

1.  **Indications:** You learned how servers can push **critical data** with guaranteed delivery, using a simple acknowledgment handshake.
2.  **Choosing the Right Tool:** You now understand the practical difference between notifications (fast, efficient) and indications (reliable, confirmed).
3.  **CCCD Mastery:** You reinforced your knowledge of the Client Characteristic Configuration Descriptor by using a different value (`0x0200`) to enable a different feature.

You have now mastered all three primary GATT operations: **Read, Write, and Subscribe (Notify/Indicate)**. This completes your foundational training in BLE communication! Well done!