# Flag 1
* * *


## Scan
let's get our own MAC and if you notice it is down now
```
hciconfig
hci1:   Type: Primary  Bus: USB
        BD Address: 00:1A:7D:DA:71:13  ACL MTU: 310:10  SCO MTU: 64:8
        DOWN 
        RX bytes:582 acl:0 sco:0 events:30 errors:0
        TX bytes:367 acl:0 sco:0 commands:30 errors:0
```

we can turn it on
```
systemctl status bluetooth
systemctl enable bluetooth
systemctl start bluetooth

hciconfig
hci1:   Type: Primary  Bus: USB
        BD Address: 00:1A:7D:DA:71:13  ACL MTU: 310:10  SCO MTU: 64:8
        UP RUNNING 
        RX bytes:1186 acl:0 sco:0 events:65 errors:0
        TX bytes:1067 acl:0 sco:0 commands:65 errors:0

```

the target BD_address 00:1A:7D:DA:71:13
```
bluetoothctl
scan on
```
![8d92be2ff1623618dac06677087e6ef3.png](../../../../../../_resources/8d92be2ff1623618dac06677087e6ef3.png)

now we have the target BD_addr: C8:C9:A3:FA:F1:6A
## Check our score
- run the following script to check your score
```
chmod +x get_score.sh
./get_socre.sh <MAC>
```
- 
```
#!/bin/bash

MAC="$1"

if [ -z "$MAC" ]; then
    echo "Usage: $0 <BLE-MAC>"
    exit 1
fi

gatttool -b "$MAC" --char-read -a 0x002a 2>/dev/null \
    | awk -F':' '{print $2}' \
    | tr -d ' ' \
    | xxd -r -p

echo
```


## Submit a flag
to submit a flag
```
chmod +x submit_flag.sh
./submit_flag.sh <MAC> <FLAG>
```


```
#!/bin/bash

MAC="$1"
FLAG="$2"

if [ -z "$MAC" ] || [ -z "$FLAG" ]; then
    echo "Usage: $0 <BLE-MAC> <FLAG>"
    exit 1
fi

gatttool -b "$MAC" --char-write-req -a 0x002c -n $(echo -n "$FLAG" | xxd -ps)
```


```
./submit_flag.sh C8:C9:A3:FA:F1:6A "12345678901234567890"

./get_socre.sh C8:C9:A3:FA:F1:6A
```

![c23d86011aaaf2df15f39ca019fc2a74.png](../../../../../../_resources/c23d86011aaaf2df15f39ca019fc2a74.png)

hex_to_ascii.sh
```
#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <HEX_STRING>"
    exit 1
fi

HEX_STRING="$1"
echo "$HEX_STRING" | xxd -r -p
```