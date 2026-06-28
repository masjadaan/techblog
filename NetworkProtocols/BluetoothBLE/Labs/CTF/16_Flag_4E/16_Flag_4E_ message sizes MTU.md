# 16_Flag_4E: message sizes MTU
* * *

Read handle 0x004E and do what it says. Setting MTU can be a tricky thing. Some tools may provide mtu flags, but they dont seem to really trigger MTU negotiations on servers. Try using gatttool's interactive mode for this task. By default, the BLECTF server is set to force an MTU size of 20. The server will listen for MTU negotiations, and look at them, but we dont really change the MTU in the code. We just trigger the flag code if you trigger an MTU event with the value specified in handle 0x0048. GLHF!

```
gatttool -I -b C8:C9:A3:FA:F1:6A
char-read-hnd 0x004e
Characteristic value/descriptor: 53 65 74 20 79 6f 75 72 20 63 6f 6e 6e 65 63 74 69 6f 6e 20 4d 54 55 20 74 6f 20 34 34 34 

./hex_to_ascii.sh "53 65 74 20 79 6f 75 72 20 63 6f 6e 6e 65 63 74 69 6f 6e 20 4d 54 55 20 74 6f 20 34 34 34"
Set your connection MTU to 444  
```


Change MTU
```
mtu 444

char-read-hnd 0x004e
Characteristic value/descriptor: 62 31 65 34 30 39 65 35 61 34 65 61 66 39 66 65 35 31 35 38

./hex_to_ascii.sh "62 31 65 34 30 39 65 35 61 34 65 61 66 39 66 65 35 31 35 38"

./submit_flag.sh C8:C9:A3:FA:F1:6A "b1e409e5a4eaf9fe5158"
```