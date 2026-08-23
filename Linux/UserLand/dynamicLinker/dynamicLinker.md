---
layout: single
title: "Have You Met The Dynamic Linker"
date: 2023-11-18
categories: [Linux]
tags: [elf, dynamic-linker, ld.so, runtime-linking]
author_profile: true
toc: true
toc_sticky: true
read_time: true
show_date: true
---

## Introduction 
A binary undergoes multiple phases before it becomes executable. Typically, the process begins with source code, followed by compilation, static linking, loading/dynamic linking, and ultimately execution. This article focuses on dynamic linking, a mechanism that allows a program to use external libraries at runtime. This means that the code from these libraries is not integrated into the executable during the compilation phase. Instead, the program includes references to external functions, and the linking with the libraries takes place dynamically during runtime when the program is loaded into memory.

Before exploring the mechanics of this process, let's introduce three key concepts: Executable and Linking Format (ELF), the Global Offset Table (GOT), and the Procedure Linkage Table (PLT).
* * *

## Executable and Linking Format (ELF)
The ELF (Executable and Linking Format) file format is used on Unix-like operating systems and it consists of:

| ELF File Format |
| --- 		|
|Header|
|Program Header Table|
|Section Header Table|
|Sections|
|String Table|
|Symbol Table|
|Relocation Information|
|Dynamic Section |

We won't dive too deep into the details of the ELF file format, but it's important to understand that the header contains information like the Magic Number and Endianness (whether it's Little or Big). Typically, the header is 52 bytes for ELF files on 32-bit systems and 64 bytes for ELF files on 64-bit systems.

Now, let's talk about the sections within an ELF file. These include:
- The .text section (which holds executable code)
- The .data (for initialized data)
- The .bss (uninitialized data)
- The .rodata (read-only data)
- The .plt (Procedure Linkage Table)
- The .got (Global Offset Table).

To make things more tangible, let's create our own ELF file for the demonstrations in this article. We'll name it "linker," and we'll use the C programming language to put together a simple program.

### <span style="color: #7f7fff">The C code</span>
We've written a very straightforward C code that does something simple. It prints the words "Hello, Dynamic Linker!" on the screen. In addition, we've added two variables—one uninitialized and the other initialized to 1.
```C
#include <stdio.h>

void main() {
	int uninitialized;
	int initialized = 1;
	printf("Hello, Dynamic Linker!\n");
}
```

### <span style="color: #7f7fff">Compilation</span>
In the compilation stage, we us GCC compiler with the -g flag. This flag ensures that the code is compiled with debugging information, enabling us to delve into the details during the debugging process.
```sh
# compile linker.c
gcc linker.c -g -o linker
	-g flag to include debugging info
```

### <span style="color: #7f7fff"> linker File Information</span>
Once the compilation finishes, we obtain the ELF file named "linker." Now, let's kick things off by exploring information about this linker file. For this, we'll make use of three handy tools: xxd, readelf and objdump

Simply execute the following xxd command and grab first few lines:
```sh
xxd linker | head
```

![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/Linux/dynamicLinker/images/ELF/xxdLinker.png)

Now, let's dissect this output and unravel the meaning behind some of these bytes.
| Byte | Description|
| ---   | --- |
|Byte 0-3: 7F45	4c46   | ELF Magic Number|
|Byte 4: 02           | 64-bit format (01 for 32-bit format)|
|Byte 5: 01           | Little endian encoding (02 big enidan)|
|Byte 6: 01           | The ELF version: 1|
|Byte 7: 00           | The target operating system (system V)|
|Byte 24: 4000 3004   |The entry point (it is actually 0x400430 because of little endian encoding)|

However, a simpler method to get this information is by using readelf to read the header file.
```sh
readelf -h linker
```

![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/Linux/dynamicLinker/images/ELF/readelf-h.png)


As we mentioned before, ELF files consist of sections. Now, let's use the readelf tool to display a list of all sections, with a special focus on two sections we'll be referring to later.
```sh
readelf -S linker
```

![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/Linux/dynamicLinker/images/ELF/readelf-S_sections.png)


- **".plt"**
	- Start address: `0x4003F0`
	- End address: `0x400420`
	- Size: `0x30`
- **".got.plt"**
	- Start address: `0x601000`
	- End address: `0x601028`
	- Size: `0x28`


The last command we will use here is to showcase the assembly code. Since readelf lacks this functionality, we'll turn to "objdump" to display the assembly code within the ".text" section. I prefer using the Intel syntax, so here's the command:
```sh
objdump -j .text -d -M intel linker
```

![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/Linux/dynamicLinker/images/ELF/objdump-j.text.png)



## Procedure Linkage Table (PLT)
As we have seen, the PLT has a dedicated section called as ".plt". The Procedure Linkage Table (PLT) is a read-only data structure used in the ELF file format. It is used in the dynamic linking process that resolve the addresses for requested functions. This dynamic resolution is necessary because the addresses of functions from shared libraries are not known during compile time. Instead, the resolution is postponed to the run-time, more precisely, until the first time a call to the requested function is made (lazy linking).

Each program has its own unique PLT, and when a function's address needs to be resolved, the calling function makes a request to the PLT. 

To provide a high-level overview of the process:
- Step 1: A program initiates a call to an external function within a shared library

-  Step 2: The request is forwarded to the Procedure Linkage Table (PLT).

-  Step 3: The PLT directs control to the corresponding entry in the Global Offset Table (GOT) associated with the requested function.
	-  If this is the first instance of requesting that function, then proceed to the following step;
	-  otherwise, skip ahead to step 5.

- Step 4: As this represents the first request for the function, the control transitions back from the Global Offset Table (GOT) to the Procedure Linkage Table (PLT). Then, the PLT invokes the dynamic linker to resolve the symbol. Once the resolution is successfully completed, the address of the requested function is inserted into the corresponding GOT entry.

-  Step 5: If the Global Offset Table (GOT) already contains the address of the requested function, control can proceed without requiring the intervention of the dynamic linker.
***


## Global Offset Table
As we have seen, the GOT has a dedicated section called as ".got". The Global Offset Table (GOT) is a data structure used in the ELF binary file format. The GOT stores the addresses of global symbols both functions and variables after they have been resolved.

During program runtime, the dynamic linker obtains the absolute addresses of these symbols and populates the Global Offset Table accordingly.




## GDB: Symbol Resolution At Runtime
Alright, let's put theory into action with some practical examples. To see symbol resolution in action during runtime, we'll use GDB. We'll set the syntax to intel and peek into the assembly code of the main function.
```sh
gdb linker
(gdb) set disassembly-flavor intel
(gdb) disas main
```

Look for the call to the puts() function. Now, puts() is a function that prints a string to standard output (stdout), much like printf(), but without the use of format strings. The reason we see puts() instead of printf() is that the compiler has optimized the printf() call to the simpler function, puts(). This is a common optimization strategy to reduce code size and improve performance. However, this raises the question, where does this puts function come from?

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Linux/dynamicLinker/images/GDB/assembly/disasMain.png)

To answer this question, we'll set up two breakpoints—one just before calling puts() and the other immediately after the call. This way, we can dissect the program's behavior at these critical points.
```sh
(gdb) b *0x000000000040052f # before calling puts()
(gdb) b *0x0000000000400534 # after calling puts()
```

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Linux/dynamicLinker/images/GDB/assembly/breakPoints.png)


### <span style="color: #7f7fff">Before Calling puts()</span>
Now, simply type "run" to start the program execution. The execution will pause at breakpoint 1 (0x40053a). 
```sh
run
disas main
```

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Linux/dynamicLinker/images/GDB/beforeCallingPuts/run.png)

In the assembly code for the "main" function, we spot a call instruction at this address, directing us to address 0x400400. This address corresponds to the puts function in the Procedure Linkage Table (PLT), and interestingly, it falls within the range of the ".plt" section. Since it's a call to a function, let's inspect the first four instructions to get a closer look.
```sh
(gdb) x/4i 0x400400
```

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Linux/dynamicLinker/images/GDB/beforeCallingPuts/examin400400.png)

We observe a jump to address 0x601018. This address falls within the ".got.plt" we discussed earlier. Let's take a closer look at it. Inside, we find the address 0x400406, which is an address in the PLT from our previous images.
```sh
(gdb) x/4x 0x601018
```

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Linux/dynamicLinker/images/GDB/beforeCallingPuts/examin601018.png)

At this stage of execution, GOT points back to an address in PLT, essentially sending the execution back to the PLT. This happens because the GOT does not yet possess the address of puts. Continuing the execution after 0x400406, we encounter a jump to 0x4003F0, which happens to be the starting address of the PLT itself. Let's display the instructions there.
```sh
(gdb) x/4i 0x4003f0
```

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Linux/dynamicLinker/images/GDB/beforeCallingPuts/examin4003f0.png)

Upon inspection, we spot a push instruction followed by a jump instruction to address 0x601010, once again residing in the GOT. When we investigate this address, we uncover 0x7FFFF7DEEE40, which happens to be the address of the dynamic linker itself being invoked.
```sh
(gdb) x/2x 0x601010

(gdb) x/4i 0x7ffff7deee40
```

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Linux/dynamicLinker/images/GDB/beforeCallingPuts/examin0x7ffff7deee40.png)

We can verify that this address corresponds to the dynamic linker by executing the following command.
```sh
(gdb) info symbol 0x7ffff7deee40
```

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Linux/dynamicLinker/images/GDB/beforeCallingPuts/infoSymbol.png)


Let's summarize the entire process within this table.
| <span style="color: #007f7f">main </span>| <span style="color: #7f007f">plt</span> | <span style="color: #7f7f00">got.plt</span> |
| --   | -- | -- |
|<span style="color: #007f7f">**1.** 0x40053a-> call 0x400400 ;go to plt</span>|<span style="color: #7f007f">**2.** 0x400400-> jmp 0x601018 ;go to got.plt</span>| <span style="color: #7f7f00">**3.** 0x400406 ;go back to plt</span>|
|-- 																|<span style="color: #7f007f">**4.** 0x400406-> push 0x0</span>       | -- |
|-- 																|<span style="color: #7f007f">**5.** 0x40040b-> jmp    0x4003f0</span>| -- |
|-- 																|<span style="color: #7f007f">**6.** 0x4003f0-> push 0x601008</span>| -- |
|-- 																|<span style="color: #7f007f">**7.** 0x4003f6-> jmp 0x601010 ;go to got.plt</span>|<span style="color: #7f7f00">**8.** 0x7ffff7deee40 ;call the dynamic linker</span>|


### <span style="color: #7f7fff">After Calling puts()</span>
Now, let's proceed with the execution by typing "next". The execution reaches the second breakpoint and halts. 
```sh
(gdb) next
(gdb) disas main
```

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Linux/dynamicLinker/images/GDB/afterCallingPuts/disasMain.png)

 If we examine 0x601018 again, we'll find a different address, namely 0x7FFFF7A7C6A0. This indicates that the dynamic linker has successfully resolved the symbol and stored the actual address of puts() in the Global Offset Table (GOT).
```sh
(gdb) x/2x 0x601018
```

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Linux/dynamicLinker/images/GDB/afterCallingPuts/examine601018.png)

The address 0x7FFFF7A7C6A0 indeed corresponds to the puts() function. To validate our findings, let's inspect its first four instructions. 
```sh
(gdb) x/4i 0x7ffff7a7c6a0
```
![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Linux/dynamicLinker/images/GDB/afterCallingPuts/examine0x7ffff7a7c6a0.png)

Additionally, for confirmation, we can always use GDB's built-in functions to examine the GOT entry.
```sh
(gdb) info address puts
```
![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/Linux/dynamicLinker/images/GDB/afterCallingPuts/infoAddress.png)


At this point, the symbol has been completely resolved. Remember, initially the symbol in the Global Offset Table (GOT) is not resolved; this only occurs upon the first request. When the symbol is first requested, the dynamic linker resolves it and writes the address into the GOT. Subsequently, during the runtime of the program, any further requests to the same function will retrieve the address directly from the GOT, bypassing the dynamic linker. The Procedure Linkage Table (PLT) entry points to the corresponding GOT entry, which now holds the resolved address of the function.

Happy Learning... <br>
Mahmoud Jadaan

## Test Environment

**Software**
- Ubuntu (16.04.7 LTS (Xenial Xerus), x86_64 GNU/Linux)
- gcc
- xxd
- readelf
- objdump
- gdb
