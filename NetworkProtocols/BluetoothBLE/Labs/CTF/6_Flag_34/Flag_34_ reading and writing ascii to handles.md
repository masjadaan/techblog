# Flag_34: reading and writing ascii to handles
* * *

Follow the instructions found from reading handle 0x0034. Keep in mind that some tools only write hex values while other provide methods for writing either hex or ascii
```
char-read-hnd 0x0034
Characteristic value/descriptor: 57 72 69 74 65 20 74 68 65 20 61 73 63 69 69 20 76 61 6c 75 65 20 22 79 6f 22 20 68 65 72 65

echo "57 72 69 74 65 20 74 68 65 20 61 73 63 69 69 20 76 61 6c 75 65 20 22 79 6f 22 20 68 65 72 65" | xxd -r -p
Write the ascii value "yo" here


echo -n "yo" | xxd -p
796f

char-write-req 0x0034 796f
Characteristic value was written successfully

char-read-hnd 0x0034
Characteristic value/descriptor: 63 35 35 63 36 33 31 34 62 33 64 62 30 61 36 31 32 38 61 66

echo "63 35 35 63 36 33 31 34 62 33 64 62 30 61 36 31 32 38 61 66" | xxd -r -p 
c55c6314b3db0a6128af

./submit_flag.sh C8:C9:A3:FA:F1:6A "c55c6314b3db0a6128af"
```
