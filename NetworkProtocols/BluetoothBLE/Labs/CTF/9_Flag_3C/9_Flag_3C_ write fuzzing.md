# 9_Flag_3C: write fuzzing
* * *
Take a look at handle 0x003c and do what it says. You should script up a solution for this one. Also keep in mind that some tools write faster than others.

```
char-read-hnd 0x003c
Characteristic value/descriptor: 42 72 75 74 65 20 66 6f 72 63 65 20 6d 79 20 76 61 6c 75 65 20 30 30 20 74 6f 20 66 66

echo "42 72 75 74 65 20 66 6f 72 63 65 20 6d 79 20 76 61 6c 75 65 20 30 30 20 74 6f 20 66 66" | xxd -r -p
Brute force my value 00 to ff 
```

```
cat fuzz_3c.sh 
#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <BLE_DEVICE_ADDRESS>"
    exit 1
fi

BLE_ADDRESS="$1"
HANDLE=0x003c

for i in $(seq 0 255); do
    hex=$(printf "%02x" $i)
    echo "Writing $hex to handle $HANDLE"
    gatttool -b "$BLE_ADDRESS" --char-write-req -a $HANDLE -n $hex
done

```

```
gatttool -I
connect C8:C9:A3:FA:F1:6A

char-read-hnd 0x003c
Characteristic value/descriptor: 39 33 33 63 31 66 63 66 61 38 65 64 35 32 64 32 65 63 30 35

echo "39 33 33 63 31 66 63 66 61 38 65 64 35 32 64 32 65 63 30 35" | xxd -r -p 
933c1fcfa8ed52d2ec05

./submit_flag.sh C8:C9:A3:FA:F1:6A "933c1fcfa8ed52d2ec05"

```