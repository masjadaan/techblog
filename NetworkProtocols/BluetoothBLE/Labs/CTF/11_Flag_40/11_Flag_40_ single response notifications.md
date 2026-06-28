# 11_Flag_40: single response notifications
* * *

Check out handle 0x0040 and google search gatt notify. Some tools like gatttool have the ability to subscribe to gatt notifications

```
char-read-hnd 0x0040
Characteristic value/descriptor: 4c 69 73 74 65 6e 20 74 6f 20 6d 65 20 66 6f 72 20 61 20 73 69 6e 67 6c 65 20 6e 6f 74 69 66 69 63 61 74 69 6f 6e

echo "4c 69 73 74 65 6e 20 74 6f 20 6d 65 20 66 6f 72 20 61 20 73 69 6e 67 6c 65 20 6e 6f 74 69 66 69 63 61 74 69 6f 6e" | xxd -r -p
Listen to me for a single notification 
```

Enable Notifications:
writes 0x0100 to the handle 0x0040, enabling notifications.
```
char-write-req 0x0040 0100
Characteristic value was written successfully
Notification handle = 0x0040 value: 35 65 63 33 37 37 32 62 63 64 30 30 63 66 30 36 64 38 65 62

./hex_to_ascii.sh "35 65 63 33 37 37 32 62 63 64 30 30 63 66 30 36 64 38 65 62"
5ec3772bcd00cf06d8eb

./submit_flag.sh C8:C9:A3:FA:F1:6A "5ec3772bcd00cf06d8eb"
```

read Client characteristic configuration descriptor (CCCD)
https://academy.nordicsemi.com/courses/bluetooth-low-energy-fundamentals/lessons/lesson-4-bluetooth-le-data-exchange/topic/services-and-characteristics/