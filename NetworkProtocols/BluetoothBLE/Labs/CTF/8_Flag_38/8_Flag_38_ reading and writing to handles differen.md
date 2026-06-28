# 8_Flag_38: reading and writing to handles differently
* * *

Follow the instructions found from reading handle 0x0038. Pay attention to handles here. Keep in mind handles can be refrenced by integer or hex. Most tools such as gatttool and bleah allow you to specify handles both ways.

```
char-read-hnd 0x0038
Characteristic value/descriptor: 57 72 69 74 65 20 30 78 43 39 20 74 6f 20 68 61 6e 64 6c 65 20 35 38

echo "57 72 69 74 65 20 30 78 43 39 20 74 6f 20 68 61 6e 64 6c 65 20 35 38" | xxd -r -p 
Write 0xC9 to handle 58 

# hex(58)= 0x3A
char-write-req 0x003A C9
Characteristic value was written successfully

char-read-hnd 0x0038
Characteristic value/descriptor: 66 38 62 31 33 36 64 39 33 37 66 61 64 36 61 32 62 65 39 66

echo "66 38 62 31 33 36 64 39 33 37 66 61 64 36 61 32 62 65 39 66" | xxd -r -p 
f8b136d937fad6a2be9f

./submit_flag.sh C8:C9:A3:FA:F1:6A "f8b136d937fad6a2be9f"
```