# 18_Flag_52: Hidden notify property
* * *

Take a look at handle 0x0052. Notice it does not have a notify property. Do a write here and listen for notifications anyways! Things are not always what they seem!

```
char-read-hnd 0x0052
Characteristic value/descriptor: 4e 6f 20 6e 6f 74 69 66 69 63 61 74 69 6f 6e 73 20 68 65 72 65 21 20 72 65 61 6c 6c 79 3f

./hex_to_ascii.sh "4e 6f 20 6e 6f 74 69 66 69 63 61 74 69 6f 6e 73 20 68 65 72 65 21 20 72 65 61 6c 6c 79 3f"
No notifications here! really? 
```


```
char-write-req 0x0052 68656c6c6f
Characteristic value was written successfully
Notification handle = 0x0052 value: 66 63 39 32 30 63 36 38 62 36 30 30 36 31 36 39 34 37 37 62

./hex_to_ascii.sh "66 63 39 32 30 63 36 38 62 36 30 30 36 31 36 39 34 37 37 62"
fc920c68b6006169477b

/submit_flag.sh C8:C9:A3:FA:F1:6A "fc920c68b6006169477b"
```