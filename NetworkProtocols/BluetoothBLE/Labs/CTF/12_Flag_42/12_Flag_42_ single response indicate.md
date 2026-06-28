# 12_Flag_42: enable indications
* * *
Check out handle 0x0042 and google search gatt indicate. For single response indicate messages, like this challenge, tools such as gatttool will work just fine.

```
char-read-hnd 0x0042
Characteristic value/descriptor: 4c 69 73 74 65 6e 20 74 6f 20 68 61 6e 64 6c 65 20 30 78 30 30 34 34 20 66 6f 72 20 61 20 73 69 6e 67 6c 65 20 69 6e 64 69 63 61 74 69 6f 6e 

./hex_to_ascii.sh "4c 69 73 74 65 6e 20 74 6f 20 68 61 6e 64 6c 65 20 30 78 30 30 34 34 20 66 6f 72 20 61 20 73 69 6e 67 6c 65 20 69 6e 64 69 63 61 74 69 6f 6e"
Listen to handle 0x0044 for a single indication  
```

to listen
```
char-write-req 0x0044 0200
Characteristic value was written successfully
Indication   handle = 0x0044 value: 63 37 62 38 36 64 64 31 32 31 38 34 38 63 37 37 63 31 31 33

./hex_to_ascii.sh "63 37 62 38 36 64 64 31 32 31 38 34 38 63 37 37 63 31 31 33"
c7b86dd121848c77c113

./submit_flag.sh C8:C9:A3:FA:F1:6A "c7b86dd121848c77c113"
```