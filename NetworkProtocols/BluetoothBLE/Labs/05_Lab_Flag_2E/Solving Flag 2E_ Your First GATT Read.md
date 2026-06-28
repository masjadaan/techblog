# Solving Flag 2E: Your First GATT Read
* * *
![cd8055088f573e39aedeb1202088bb41.png](../../../../../_resources/cd8055088f573e39aedeb1202088bb41.png)
## Hands-On with GATT Operations

Excellent! You've submitted the practice flag. Now let's tackle your first real challenge: **Flag 2E**. This flag is designed to teach you the fundamental skill of reading and interpreting data from a specific GATT handle.

The instructions for this challenge are straightforward:
> *"Check out the ASCII value of handle `0x002e` and submit it to the flag submission handle `0x002c`."*

Let's break this down into steps.

---

## Step 1: Using GATTTOOL
`gatttool` is a command-line utility for interacting with Bluetooth Low Energy (BLE) devices. It allows you to discover services, characteristics, and descriptors, as well as read and write values to a BLE device.

Now, let’s use `gatttool`. Here are the fundamental commands you’ll need to know:
```
# Start gatttool in interactive mode with a specific device MAC address
gatttool -I -b C8:C9:A3:FA:F1:6A

# Connect to the device
connect

# Display available commands
help

# Exit the tool
exit
```

![bdf2bc5603f79cec3c639583f16586d5.png](../../../../../_resources/bdf2bc5603f79cec3c639583f16586d5.png)

## Primary Service Discovery
Once connected, the first step is discovering what services the device offers:
```
[C8:C9:A3:FA:F1:6A][LE]> primary
attr handle: 0x0001, end grp handle: 0x0005 uuid: 00001801-0000-1000-8000-00805f9b34fb
attr handle: 0x0014, end grp handle: 0x001c uuid: 00001800-0000-1000-8000-00805f9b34fb
attr handle: 0x0028, end grp handle: 0xffff uuid: 000000ff-0000-1000-8000-00805f9b34fb
```

![3792dc57a9c0da8112b07a59a650eb6f.png](../../../../../_resources/3792dc57a9c0da8112b07a59a650eb6f.png)

Each line corresponds to a primary service declaration:
- **attr handle**: The starting handle of the service
- **end grp handle**: The last handle that belongs to this service
- **uuid**: The service's Universally Unique Identifier

Some UUIDs are standardized by the [Bluetooth SIG](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Assigned_Numbers/out/en/Assigned_Numbers.pdf), while others are custom/proprietary. You can reference the Bluetooth SIG specifications to identify common services.
![55bf9ac49566fabc21367fb0b3cd4fb3.png](../../../../../_resources/55bf9ac49566fabc21367fb0b3cd4fb3.png)

All handles between the start and end are "owned" by that service, and you can use them to discover or read/write characteristics.

| Start  | End    | UUID | Service                    |
| ------ | ------ | ---- | -------------------------- |
| 0x0001 | 0x0005 | 1801 | Generic Attribute (GATT)   |
| 0x0014 | 0x001C | 1800 | Generic Access (GAP)       |
| 0x0028 | 0xFFFF | 00FF | Custom/Proprietary Service |

---

## Step 2: Read the Target Handle

The challenge tells us the flag is at handle `0x002e`. Let's read it.

At the `gatttool` prompt, use the `char-read-hnd` command:
```bash
char-read-hnd 0x002e
```

You will get a response showing the raw bytes stored at that handle:
```
Characteristic value/descriptor: 64 32 30 35 33 30 33 65 30 39 39 63 65 66 66 34 34 38 33 35
```

This is a hexadecimal representation of the data.
![bca90079c5ce768a804ce78a86ae3746.png](../../../../../_resources/bca90079c5ce768a804ce78a86ae3746.png)

## Step 3: Decode the Hexadecimal Data

Our handy hex_to_ascii.sh script will convert this hex string into readable ASCII text. We need to pass it the string without spaces.
```
./hex_to_ascii.sh "64 32 30 35 33 30 33 65 30 39 39 63 65 66 66 34 34 38 33 35"  
```

The script will output the decoded flag:
```
d205303e099ceff44835 
```

This is the flag we need to submit.
![ab6c0050522ef0255f98adc059f05b2c.png](../../../../../_resources/ab6c0050522ef0255f98adc059f05b2c.png)

## Step 4: Submit the Flag and Claim Your Points

Now that we have the flag, let's submit it using our submission script. (Remember to disconnect)

Run the submit_flag.sh script with the device's MAC address and the flag you just discovered:
```
./submit_flag.sh C8:C9:A3:FA:F1:6A "d205303e099ceff44835"
```

You should see a success message: `Characteristic value was written successfully.`

Verify your success by checking your updated score!
```
./get_score.sh C8:C9:A3:FA:F1:6A
# Score:2 /20
```
![264e16649a1b9f75a8e211b9df90359c.png](../../../../../_resources/264e16649a1b9f75a8e211b9df90359c.png)

Congratulations! You've successfully solved your first challenge. You've performed a core GATT operation: reading a characteristic value by its handle and interpreting the data.
