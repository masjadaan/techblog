# 13_Flag_46: multi response notifications
* * *

Check out handle 0x0046 and do what it says. Keep in mind that this notification clallange requires you to recieve multiple responses in order to complete.

For multiple notifications:

Notifications (unlike indications) don’t require acknowledgment.

To enable them, you must write 0x0100 (little-endian → 01 00) into the Client Characteristic Configuration Descriptor (CCCD, UUID 0x2902) associated with the characteristic (here, handle 0x0046).


```
char-read-hnd 0x0046
Characteristic value/descriptor: 4c 69 73 74 65 6e 20 74 6f 20 6d 65 20 66 6f 72 20 6d 75 6c 74 69 20 6e 6f 74 69 66 69 63 61 74 69 6f 6e 73

./hex_to_ascii.sh "4c 69 73 74 65 6e 20 74 6f 20 6d 65 20 66 6f 72 20 6d 75 6c 74 69 20 6e 6f 74 69 66 69 63 61 74 69 6f 6e 73"
Listen to me for multi notifications 
```


```
char-write-req 0x0046 0100 --listen

./hex_to_ascii.sh "55 20 6e 6f 20 77 61 6e 74 20 74 68 69 73 20 6d 73 67 00 00 63 39 34 35 37 64 65 35 66 64 38 63 61 66 65 33 34 39 66 64"
U no want this msgc9457de5fd8cafe349fd

./submit_flag.sh C8:C9:A3:FA:F1:6A "c9457de5fd8cafe349fd"
```