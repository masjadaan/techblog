# 10_Flag_3E: read and write speeds
* * *

Talke a look at handle 0x003e and do what it says. Keep in mind that some tools have better connection speeds than other for doing reads and writes. This has to do with the functionality the tool provides or how it uses cached BT connections on the host OS. Try testing different tools for this flag. Once you find the fastest one, whip up a script or bash 1 liner to complete the task. FYI, once running, this task takes roughly 90 seconds to complete if done right.

```
char-read-hnd 0x003e
Characteristic value/descriptor: 52 65 61 64 20 6d 65 20 31 30 30 30 20 74 69 6d 65 73

echo "52 65 61 64 20 6d 65 20 31 30 30 30 20 74 69 6d 65 73" | xxd -r -p 
Read me 1000 times 
```


```
cat read_3E.sh 
#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <BLE_DEVICE_ADDRESS>"
    exit 1
fi

BLE_ADDRESS="$1"
HANDLE=0x003e

for i in $(seq 0 1000); do
    echo "Reading $hex to handle $HANDLE"
    gatttool -b "$BLE_ADDRESS" --char-read -a $HANDLE
done

```


```
echo "36 66 66 63 64 32 31 34 66 66 65 62 64 63 30 64 30 36 39 65" | xxd -r -p 
6ffcd214ffebdc0d069e

./submit_flag.sh C8:C9:A3:FA:F1:6A "6ffcd214ffebdc0d069e"
```