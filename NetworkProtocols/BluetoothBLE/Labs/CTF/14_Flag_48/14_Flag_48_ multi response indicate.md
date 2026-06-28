# 14_Flag_48: multi response indicate
* * *

Check out handle 0x0048 and google search gatt indicate. Keep in mind that this chalange will require you to parse multiple indicate responses in order to complete the chalange.

For multi indications on handle 0x004a:

Indications require acknowledgment from the client (unlike notifications).

To enable them, you must write 0x0200 (little-endian → 02 00) into the CCCD (UUID 0x2902) of the characteristic at handle 0x004a.

You then need to stay listening to capture all indication messages.

This subscribes to multiple indications on handle 0x004a and prints each incoming indication until you stop it.


```
char-read-hnd 0x0048
Characteristic value/descriptor: 4c 69 73 74 65 6e 20 74 6f 20 68 61 6e 64 6c 65 20 30 78 30 30 34 61 20 66 6f 72 20 6d 75 6c 74 69 20 69 6e 64 69 63 61 74 69 6f 6e 73

./hex_to_ascii.sh "4c 69 73 74 65 6e 20 74 6f 20 68 61 6e 64 6c 65 20 30 78 30 30 34 61 20 66 6f 72 20 6d 75 6c 74 69 20 69 6e 64 69 63 61 74 69 6f 6e 73" 
Listen to handle 0x004a for multi indications 
```

to listen
```
char-write-req 0x004a 0200 --listen

./hex_to_ascii.sh "55 20 6e 6f 20 77 61 6e 74 20 74 68 69 73 20 6d 73 67 00 00 62 36 66 33 61 34 37 66 32 30 37 64 33 38 65 31 36 66 66 61"
U no want this msgb6f3a47f207d38e16ffa

./submit_flag.sh C8:C9:A3:FA:F1:6A "b6f3a47f207d38e16ffa"

```

