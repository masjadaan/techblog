---
layout: single
title: "JTAG Pinout Identification"
date: 2024-02-04
categories: [Debug Interfaces]
tags: [jtag, jtagulator, embedded, hardware]
author_profile: true
toc: true
toc_sticky: true
read_time: true
show_date: true
---

## Introduction
Hi everyone, our article's focus is the JTAG interface, a topic that may be unfamiliar to those not engaged in the embedded systems domain. Generally, the JTAG interface can be found in many embedded system devices across various industries, including Automotive, Aerospace, Railway, Medical Devices, IoT, and more. As our interest lies in security, exploring this interface becomes useful in conducting our research. This article will concentrate on discovering JTAG pins using a multi-meter and the Jtagulator board. Additionally, we will show you how to dump the contents of any memory on the device. Let's get into it.

## What is JTAG
JTAG, an IEEE 1149.1 industry standard, was first issued in 1990 with the aim of simplifying the testing of Printed Circuit Boards (PCBs). It is commonly implemented in complex integrated circuits. Currently, JTAG has become widely used for testing and debugging electronic circuits, finding its way into processors of various architectures, such as ARM, x86, MIPS, and PowerPC. It's worth noting that JTAG specifications don't prescribe a specific connector design; hence, you might encounter various connector shapes.

So, why do we, as security researchers, care about JTAG? There are two main reasons. First, we can read and write the content of device memory, essentially allowing us to dump the entire memory content. Second, we can break into the boot cycle, utilizing the JTAG interface with a debugger to debug the real firmware on the physical hardware.

Now, the JTAG interface has four mandatory pins and one optional:
- Test Data In
- Test Data Out
- Test Clock (TCK)
- Test Mode Select (TMS)
- Optional: Test Reset (TRST)

To interact with a device via JTAG, we need three things:
- Location: Find the JTAG interface pins on the board.
- Hardware: JTAG adapter to connect a PC to the JTAG interface.
- Software: such as OpenOCD to dump the memory content or enable in-circuit debugging.

Locating the JTAG interface isn't a walk in the park. There are no standardized connectors and pinouts, but you can find some popular pinouts on the [JTAGTest website](http://www.jtagtest.com/pinouts/). A general recommendation is to follow these steps:

1. Search for pins labeled with JTAG names like TCK, TDI, TDO, etc., on the PCB (though you might not find them).
2. Look for pin headers arranged in a single row of 5 or 6 pins or in a double row of 10, 12, 14, or 20 pins.
3. Upon finding potential pins, use a multimeter or auxiliary tools like the Jtagulator board to identify the pins' functionality.

However, it's easier said than done. So, let's introduce our target and explore how we can locate those JTAG interface pins.

## The Target
Our target, in this article, is the ESP32 System on Chip, which comes with integrated Wi-Fi and Bluetooth connectivity, making it suitable for a wide range of applications. It's important to note that our chosen target is well-documented on the internet, with a lot of information available, including details about the JTAG pins. However, we will use it as an example to learn how to approach the JTAG interface.

- Front-side
  
![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/DebugInterfaces/JTAG/Images/TargetFrondSide.png)

- Back-side
  
![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/DebugInterfaces/JTAG/Images/TargetBackSide.png)

## Identifying Chips
To start, identify the main chips on the board. Generally, when examining the top of any chip, you'll find the manufacturer's name, and beneath it, the part number. In our scenario, there are two chips—one from Espressif and the other from Silicon Labs, which provides USB-to-UART functionality and is not of interest to us.

As we are left with one chip, 'WiFi ESP-WROOM-32', let's search for its datasheet on the manufacturer's website or on Google.

|Manufacturer| Family| Part Number| Image| DataSheet|
| -- | -- | -- | -- | -- |
|espressif |WiFi ESP-WROOM-32 |FCC ID: 2AC7Z-ESP32WROOM32 | ![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/DebugInterfaces/JTAG/Images/espressif.png)| [esp32.pdf](https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf)|
|Silicon Labs | USB to UART Bridge| CP2102|![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/DebugInterfaces/JTAG/Images/siliconlabs.png)|--|

After a quick review of the WiFi ESP-WROOM-32 datasheet, we've learned that the ESP32 is equipped with two powerful Xtensa cores, and the CPU clock frequency is adjustable, ranging from 80 MHz to 240 MHz. The ESP32 features an industry-standard JTAG port, which lacks (and does not require) the TRST pin. Notably, the JTAG I/O pins are all powered from the VDD_3P3_RTC pin, typically supplied by a 3.3 V. This implies that our JTAG adapter needs to be compatible with JTAG pins within that voltage range. The chosen operating system for ESP32 is freeRTOS.

Looking at the "Pin Definitions" section under Pin Layout reveals all the pins, as illustrated in the image below.

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/DebugInterfaces/JTAG/Images/pinDefinitions.png)

We observe multiple ground pins and note that the operating voltage is 3.3V. However, our focus is on identifying the JTAG interface pins, which are not visible in the pin layout. Yet, by examining the 'Peripheral Schematics' chapter, we can locate the JTAG interface and see the specific pins to which they are connected.

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/DebugInterfaces/JTAG/Images/peripheralSchematics.png)

|JTAG Pins| ESP32 Pins|
| -- | -- |
|Mode Select| IO14 (MTMS)|
|Data Input | IO12 (MTDI)|
|Clock      | IO13 (MTCK)|
|Data output| IO15 (MTDO)|

Up to this point, we have established the presence of JTAG pin headers on the chip. Now, our next objective is to trace these JTAG pins on the chip to determine their connections on the board's header pins. Typically, this proves to be a challenging task for two main reasons: firstly, the system-on-chip is often mounted on the board with pins beneath the package, making identification on the printed circuit board impossible. Even if the system-on-chip has a package that displays its pins, following traces on a multi-layer board can still be difficult, and in today's scenario, most boards are multi-layered.

However, in our case, the task is simplified because the ESP32 labels each header pin. Let's take a closer look at a few header pins and explore their functionalities. Among the easiest pins to identify are the ground (GND) pins, which can be discovered through a continuity test.

## Finding JTAG Interface
### Continuity Test
A continuity test is usually used to check the integrity of cables, and it's a very simple test that requires a multimeter. When you set the multimeter to continuity test mode and touch both probes, you can hear a beep, which indicates that the circuit is complete. What we will do is fix one probe (black) on a known ground and use the other probe (red) to check all pins on the board. Upon hearing a beep, it signifies that the corresponding pin is a ground (GND).

Repeat this process for all pins and make a note of your findings. The two images below show the process and the found GNDs

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/DebugInterfaces/JTAG/Images/ContinuityTest.png)

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/DebugInterfaces/JTAG/Images/GND.png)









### JTAGulator
So, have you ever heard of Jtagulator? It's a great an open-source hardware tool that can be used to automatically identify the pinout of the JTAG interface (and a bunch of other interfaces too). Jtagulator supports a target voltage from 1.2V to 3.3V and has 24 programmable I/O pins that you can connect to potential JTAG pins. Then, it will run some automatic scanning logic to identify the JTAG pinout for you.

Jtagulator is connected to a PC using a USB interface and it is powered by this interface and controlled through a serial terminal emulator like Putty, picocom, etc. you can grab Jtagulator from various sources, including Adafruit.

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/DebugInterfaces/JTAG/Images/jtagulator.png)

We won't be utilizing Jtagulator on all pins; instead, we'll selectively choose a few, as indicated in the image below (as mentioned before, JTAG pins are already known on the Internet).

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/DebugInterfaces/JTAG/Images/SelectedPins.png)



#### Connecting target to Jtagulator
Now we need jumper wires, first connect GND on Jtagulator to GND on our target. Then connect the candidates pin on our target to the different channels on Jtagulator. Make sure not to connect the V on Jtagulator to anything.

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/DebugInterfaces/JTAG/Images/targetToJtagulator.png)

#### Connecting Jtagulatro to PC
We are ready to connect the Jtagulator board to a PC using the USB interface to power up the board. Linux will identify it as a serail device

```sh
lsusb
ls /dev/ttyUSB*
# /dev/ttyUSB0
```

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/DebugInterfaces/JTAG/Images/lsusb.png)

We will use picocom as a terminal emulator to talk to the Jtagulator. On my Kali Linux system, the serial device is "/dev/ttyUSB0," and the baud rate is 115200 bits/sec. 

```sh
sudo apt install picocom
picocom /dev/ttyUSB0 -b 115200
```
Once connected, we press enter to be greeted by the Jtagulator, then type H for help to print the available commands. We can see there are two types of commands: Target Interface to choose what interface to examine and General Commands to set the target system voltage.

Our first step is setting the target voltage. Just press 'V' and type '3.3' for our case, as that's the voltage we're working with.


![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/DebugInterfaces/JTAG/Images/picocom.png)


Now that we're all set up, let's choose the JTAG interface by typing 'J,' and then type 'H' to see the available commands. You will see a list of available tests.

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/DebugInterfaces/JTAG/Images/J.png)


We are interested in Boundary Scan, so type B and follow the instructions. Depends on how many pins are connected to Jtagulator the test might take some time, once complet, Jtagulator present the result where it shows each channel on Jtagulator and the functionality on the target as depected in the image.

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/DebugInterfaces/JTAG/Images/B.png)

We know now all JTAG interface pins, let's summarize that in this table:

|Jtagulator Channels| Functionality| ESP32 Pins
| -- | -- | -- |
|CH0 | TDO | IO15 (MTDO) |
|CH1 | TMS | IO14 (MTMS) |
|CH2 | TCK | IO13 (MTCK) |
|CH3 | TDI | IO12 (MTDI) |

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/DebugInterfaces/JTAG/Images/identifiedPins.png)



## Dump Firmware
### OpenOCD
OpenOCD supports a fair amount of JTAG adapters. See [here](https://openocd.org/doc/html/Debug-Adapter-Hardware.html) for a list of the adapters OpenOCD works with. If you don't have it on your machine you can install it using this command.

```
sudo apt install openocd
```


If you decide to use separate JTAG adapter, look for one that is compatible with both the voltage levels on the ESP32 as well as with the OpenOCD software. 
Now we know the JTAG interface pins, we need a serial based JTAG adapter board to attach a PC to the JTAG interface. Connect the pins on JTAG adapter board to the JTAG pins on ESP32 as follow:

|JTAG Adapter Pins| ESP32 JTAG Pins|
| --  | -- |
|GDN  | GND |
|MISO |IO15 (MTDO) |
|CS   |IO14 (MTMS) |
|CLK  |IO13 (MTCK) |
|MOSI |IO12 (MTDI) |



In order to work with OpenOCD we need two configuration files, one for the debug adapter board and the other for the chip we are targeting:

- JTAG adapter board Configuration file

```sh
adapter driver ftdi
ftdi vid_pid 0x0403 0x6014 
ftdi layout_init 0x0c08 0x0f1b
adapter speed 2000
```

- ESP32 Configuration file
When no target is defined, OpenOCD uses auto probing to discover TAPs. Howerver, it is better to define the configuration file for you chip, also OpenOCD comes already with configuration files for many chips which can be found

```sh
ll /usr/share/openocd/scripts/target | grep -i esp32
-rw-r--r-- 1 root root  2582 Sep  5 23:16 esp32.cfg
```

As you can see in the image below on top we launched the openocd with the two configuration files, at the bottom, we connect to the openocd session using telnet on port 4444

```sh
sudo openocd -f jtag_adapter.cfg -f /usr/share/openocd/scripts/target/esp32.cfg
```

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/DebugInterfaces/JTAG/Images/openocd.png)

The reason we can use telnet is because when an OpenOCD session is established, it also starts two aditional services; Telenet on port 4444 and GDBServer on port 3333 (GDBServer maybe for another article). The first thing we need to do is halt the CPU, then wer are ready to dump the memory content. For this exercise, I chose the Internal ROM memoery which has 64KB. This information is available in "Table 12. Embedded Memory Address Mapping" in the [ESP32 Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf):

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/DebugInterfaces/JTAG/Images/memoryMap.png)

Let's see that in action:
```sh
halt
dump_image firmware_dump.bin 0x3ff90000 0xffff
```

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/DebugInterfaces/JTAG/Images/dumpFirmware.png)

The dumped file will be stored in /tmp folder and we can examine it using xxd command

![alt text](https://github.com/masjadaan/TechSecurityArticles/blob/main/DebugInterfaces/JTAG/Images/examinFirmware.png)

Happy Learning <br>
Mahmoud Jadaan

## Test Environment
### Hardware
- ESP32
- Multi-meter
- Jtagulator
- JTAG Adapter

### Software
- Kali x86_64 GNU/Linux Rolling
- OpenOCD
- Telnet
