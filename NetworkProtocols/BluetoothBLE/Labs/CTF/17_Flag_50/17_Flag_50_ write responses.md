# 17_Flag_50: write responses
* * *
Check out handle 0x0050 and do what it says. This chalange differs from other write chalanges as your tool that does the write needs to have write response ack messages implemente correctly. This flag is also tricky as the flag will come back as notification response data even though there is no "NOTIFY" property.

```
gatttool -I -b C8:C9:A3:FA:F1:6A
connect

char-read-hnd 0x0050
Characteristic value/descriptor: 57 72 69 74 65 2b 72 65 73 70 20 27 68 65 6c 6c 6f 27 20 20

./hex_to_ascii.sh "57 72 69 74 65 2b 72 65 73 70 20 27 68 65 6c 6c 6f 27 20 20"
Write+resp 'hello' 
```


Since this handle requires a write with response, you need --char-write-req instead of --char-write-cmd. Also, the response will arrive as a notification even though no NOTIFY property is set.

```
echo -n "hello" | xxd -p
68656c6c6f

char-write-req 0x050 68656c6c6f

char-read-hnd 0x0050
Characteristic value/descriptor: 64 34 31 64 38 63 64 39 38 66 30 30 62 32 30 34 65 39 38 30 00

./hex_to_ascii.sh "64 34 31 64 38 63 64 39 38 66 30 30 62 32 30 34 65 39 38 30 00"
d41d8cd98f00b204e980

./submit_flag.sh C8:C9:A3:FA:F1:6A "d41d8cd98f00b204e980"
```