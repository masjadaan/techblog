# 5_Flag_32: reading and writing to handles
* * *
Okay, now lets go to challenge "Flag 32", this challenge desinged to practice reading and writingto handle. The challenge states "Read handle 0032 and do what it says."

The first steps are the same we use gatttool to connect
1.  Open a terminal and run the following command, replacing the MAC address with your device's address:
```bash
gatttool -I -b C8:C9:A3:FA:F1:6A
connect
```
    
Now we need to read handle 0032 as we did before
```
char-read-hnd 0032
Characteristic value/descriptor: 57 72 69 74 65 20 61 6e 79 74 68 69 6e 67 20 68 65 72 65 
```

next we need to convert it ot ASCII to understand what it says
```
# convert
./hex_to_ascii.sh "57 72 69 74 65 20 61 6e 79 74 68 69 6e 67 20 68 65 72 65"  
Write anything here
```

cool, it says that we need to write something to that handle. Of course gatttool provide the comand 'char-write-req' to do that. It takes the handle and the text we want to write as angrument however, we need to convert the text to hex. for example lets write the word "Test" which has the hex "54657374"

```
char-write-char-write-req 0x0032 54657374
Characteristic value was written successfully

```

it was a susccessful write, so now let's read it again, which give us different value
```
char-read-hnd 0x0032
Characteristic value/descriptor: 33 38 37 33 63 30 32 37 30 37 36 33 35 36 38 63 66 37 61 61
```

by docoding it we get the flag
```
./hex_to_ascii.sh "33 38 37 33 63 30 32 37 30 37 36 33 35 36 38 63 66 37 61 61" 
3873c0270763568cf7aa
```

this hte flag we need to submit and check our score
Note, if you are using two terminals, one connect to bluetooth device and the other for submiting the flag, don't forget to disconnect from the bluetooth device before you submit the flag.
```
./submit_flag.sh C8:C9:A3:FA:F1:6A "3873c0270763568cf7aa"
./get_score.sh C8:C9:A3:FA:F1:6A
```