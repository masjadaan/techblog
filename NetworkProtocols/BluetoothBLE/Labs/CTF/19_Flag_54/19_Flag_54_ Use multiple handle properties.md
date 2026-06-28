# 19_Flag_54: Use multiple handle properties
* * *
Check out all of the handle properties on 0x0054! Poke around with all of them and find pieces to your flag.

```
char-read-hnd 0x0054
Characteristic value/descriptor: 53 6f 20 6d 61 6e 79 20 70 72 6f 70 65 72 74 69 65 73 21

./hex_to_ascii.sh "53 6f 20 6d 61 6e 79 20 70 72 6f 70 65 72 74 69 65 73 21"
So many properties!
```


```
char-write-req 0x0054 0100
Characteristic value was written successfully
Notification handle = 0x0054 value: 30 37 65 34 61 30 63 63 34 38

char-read-hnd 0x0054
Characteristic value/descriptor: 66 62 62 39 36 36 39 35 38 66

./hex_to_ascii.sh "66 62 62 39 36 36 39 35 38 66 30 37 65 34 61 30 63 63 34 38"

./submit_flag.sh C8:C9:A3:FA:F1:6A "fbb966958f07e4a0cc48"
```

hackgnar