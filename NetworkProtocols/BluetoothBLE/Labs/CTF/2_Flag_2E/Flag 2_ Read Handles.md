# Flag 2E: Read Handles
* * *

Lets start with the first challenge, it is designed to teach you how to read and interpret the data under a speciic handle. The channlges instruction says "Check out the ascii value of handle 0x002e and submit it to the flag submision handle 0x002c."

So what we need to to is use the gatttool to connect to the device using this comand

```
gatttool -I -b C8:C9:A3:FA:F1:6A
help
connect
char-read-hnd 0x002e
Characteristic value/descriptor: 64 32 30 35 33 30 33 65 30 39 39 63 65 66 66 34 34 38 33 35 
```

when you type help, you see that gatttool offers many commands, one of them is `char-read-hnd` 
```
help

```

we will use it to read the value stored under this handle
```
char-read-hnd 0x002e
Characteristic value/descriptor: 64 32 30 35 33 30 33 65 30 39 39 63 65 66 66 34 34 38 33 35 
```

we see we got a stream of hex values, now it is the time to use our handy script  to conver it to ascii

convert it to hex
```
./hex_to_ascii.sh "64 32 30 35 33 30 33 65 30 39 39 63 65 66 66 34 34 38 33 35"  
d205303e099ceff44835 
```

This ASCII value is the flag that we need to submit it ot the GATT server lets do it and check our score.
```
./submit_flag.sh C8:C9:A3:FA:F1:6A "d205303e099ceff44835"
Characteristic value was written successfully

./get_score.sh C8:C9:A3:FA:F1:6A                         
Score:2 /20

```

as you see now we have 2 out of 20. I should mention that in case you disconnect the ESP32 the sore will be reset :). Cool now we know how to read and interpret a value
![264e16649a1b9f75a8e211b9df90359c.png](../../../../../../_resources/264e16649a1b9f75a8e211b9df90359c.png)

