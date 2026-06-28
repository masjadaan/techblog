# 7_Flag_36: reading and writing hex to handles
* * *

Follow the instructions found from reading handle 0x0036. Keep in mind that some tools only write hex values while other provide methods for writing either hex or ascii
```
char-read-hnd 0x0036
Characteristic value/descriptor: 57 72 69 74 65 20 74 68 65 20 68 65 78 20 76 61 6c 75 65 20 30 78 30 37 20 68 65 72 65 

echo "57 72 69 74 65 20 74 68 65 20 68 65 78 20 76 61 6c 75 65 20 30 78 30 37 20 68 65 72 65" | xxd -r -p  
Write the hex value 0x07 here

char-write-char-write-req 0x0036 07
Characteristic value was written successfully


char-read-hnd 0x0036
Characteristic value/descriptor: 31 31 37 39 30 38 30 62 32 39 66 38 64 61 31 36 61 64 36 36

echo "31 31 37 39 30 38 30 62 32 39 66 38 64 61 31 36 61 64 36 36" | xxd -r -p  
1179080b29f8da16ad66

./submit_flag.sh C8:C9:A3:FA:F1:6A "1179080b29f8da16ad66"
Characteristic value was written successfully

```