# 15_Flag_4C: client device attributes
* * *

Check out handle 0x004c and do what it says. Much like ethernet or wifi devices, you can also change your bluetooth devices mac address.

```
char-read-hnd 0x004C
Characteristic value/descriptor: 43 6f 6e 6e 65 63 74 20 77 69 74 68 20 42 54 20 4d 41 43 20 61 64 64 72 65 73 73 20 31 31 3a 32 32 3a 33 33 3a 34 34 3a 35 35 3a 36 36 

./hex_to_ascii.sh "43 6f 6e 6e 65 63 74 20 77 69 74 68 20 42 54 20 4d 41 43 20 61 64 64 72 65 73 73 20 31 31 3a 32 32 3a 33 33 3a 34 34 3a 35 35 3a 36 36"
Connect with BT MAC address 11:22:33:44:55:66 
```

change BD Address
```
sudo btmgmt -i hci1 power off
sudo btmgmt -i hci1 public-addr 11:22:33:44:55:66
sudo btmgmt -i hci1 power on


gatttool -I
connect C8:C9:A3:FA:F1:6A
char-read-hnd 0x004c
Characteristic value/descriptor: 61 63 61 31 36 39 32 30 35 38 33 65 34 32 62 64 63 66 35 66

./hex_to_ascii.sh "61 63 61 31 36 39 32 30 35 38 33 65 34 32 62 64 63 66 35 66"
aca16920583e42bdcf5f

./submit_flag.sh C8:C9:A3:FA:F1:6A "aca16920583e42bdcf5f"
```