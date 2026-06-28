# Flag 3: Read handle puzzle fun
* * *
Check out the ascii value of handle 0x0030. Do what it tells you and submit the flag you find to 0x002c.

![256fdfccae4abe36d7e443568eeab5aa.png](../../../../../../_resources/256fdfccae4abe36d7e443568eeab5aa.png)

```
[C8:C9:A3:FA:F1:6A][LE]> char-read-hnd 0x0030
Characteristic value/descriptor: 4d 44 35 20 6f 66 20 44 65 76 69 63 65 20 4e 61 6d 65 

# convert
./hex_to_ascii.sh "4d 44 35 20 6f 66 20 44 65 76 69 63 65 20 4e 61 6d 65" 
MD5 of Device Name
```

Device name 

```
echo -n "BLECTF" | md5sum
5cd56d74049ae40f442ece036c6f4f06

./submit_flag.sh C8:C9:A3:FA:F1:6A "5cd56d74049ae40f442ece036c6f4f06"
```


##### Resources
- [Assigned Numbers](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Assigned_Numbers/out/en/Assigned_Numbers.pdf)
- https://www.bluetooth.com/specifications/assigned-numbers/