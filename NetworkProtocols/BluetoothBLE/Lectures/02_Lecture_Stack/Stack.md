# Bluetooth LE Stack
* * *

In the second article, we're going to break down the architecture of Bluetooth Low Energy. Think of this as the "blueprint" that explains how all the different parts of a BLE device work together to create a connection, send data, and manage power efficiently.

Let's start with the big picture. The entire BLE system is organized into a **stack**—a set of layered components where each layer has a specific job. The higher layers talk to your application, and the lower layers handle the radio waves.

Here’s a visual overview of the entire stack to keep in mind as we go through each part:

![4ce698e66f2c78937420595a52403160.png](../../../../_resources/4ce698e66f2c78937420595a52403160.png)

---

## 1. The Application Layer

This is the layer **you** will interact with most. It consists of the software running on the device that defines what the device actually does. Serving as the bridge between the user and the Bluetooth stack, it relies on APIs to call functions from the lower layers.


## 2. The Host

The Host is the brain of the operation. It handles the logic of *how* devices find each other, connect, and exchange data. It contains several important protocols.

### Generic Access Profile (GAP)
GAP is responsible for:

*   **Device Discovery & Advertising:** GAP defines how devices broadcast their presence ("Here I am!") and how others scan to find them.
*   **Connection Management:** It sets the rules for establishing a connection.

Most importantly, GAP defines two fundamental **roles**:

*   **Peripheral (Server):**
    *   This is typically a small, low-power device like a heart rate sensor, or a smartwatch.
    *   Its main job is to **advertise** its presence and wait for a Central device to connect to it.
    *   Think of it as the "waiting" device.

*   **Central (Client):**
    *   This is usually a powerful device like a smartphone, laptop, or tablet.
    *   Its main job is to **scan** for advertising Peripherals and **initiate** connections.
    *   It can connect to multiple Peripherals at once and manages those connections.
    *   Think of it as the "controlling" device.

![2856cc80697a90b44a119343bda1bbc3.png](../../../../_resources/2856cc80697a90b44a119343bda1bbc3.png)

> **Key Takeaway:** The Peripheral *advertises*, and the Central *connects*.



### Attribute Protocol (ATT)
ATT is the foundation of all data exchange in BLE. It's a simple client-server protocol.

*   **Server:** The device that holds the data (e.g., a temperature sensor holds the temperature value). This is almost always the **Peripheral**.
*   **Client:** The device that wants to read or write that data (e.g., your phone reading the temperature). This is almost always the **Central**.
*   **Attribute:** The basic unit of data. It's a piece of information, like "21.5°C", along with a handle (like an ID number) and a set of permissions (read-only, write, etc.). We will talk about that in more detail later.

### Generic Attribute Profile (GATT)
If ATT defines *how* to talk, GATT defines *what* to say. It gives structure and meaning to the raw data defined by ATT.

GATT organizes attributes into a logical hierarchy that makes sense to developers:
```
Profile -> Service -> Characteristic -> Attribute
```
![8d2ea4d1a923ff6c755e9efbec501f8b.png](../../../../_resources/8d2ea4d1a923ff6c755e9efbec501f8b.png)

Imagine a filing cabinet in a doctor's office. This is how GATT organizes data:

#### 1. The Attribute: A Single Piece of Paper
*   This is the **smallest unit of data** in BLE. It's just a piece of information with a "handle" (like an ID number), a type, a value, and permissions.
*   *Example:* The number `23.5` (a temperature value).

#### 2. The Characteristic: A Single File Folder
*   A **Characteristic** is a cohesive piece of data *about one thing*. It's like a folder that contains:
    *   **The Declaration (The tab on the folder):** This is a special attribute that acts as metadata. It declares, "This folder contains a Temperature Measurement," and defines its properties (e.g., read, notify, write).
    *   **The Value (The paper inside the folder):** This is the attribute that holds the *actual data* (e.g., the value `23.5`).
    *   **Optional Descriptors (Sticky notes on the folder):** These are extra attributes that describe how to use the value (e.g., "Report new values every 1000 milliseconds").
*   *Example:* The **`Temperature Measurement`** characteristic. It's the folder dedicated solely to temperature.

#### 3. The Service: A Drawer in the Cabinet
*   A **Service** is a collection of related characteristics. It groups everything needed for a specific *function*.
*   *Example:* The **`Health Thermometer`** service. This drawer contains all the folders for the thermometer function: the `Temperature Measurement` characteristic, a `Measurement Interval` characteristic, and maybe a `Temperature Type` characteristic.

#### 4. The Profile: The Entire Filing Cabinet
*   A **Profile** is a complete definition of a *use case*. It's a pre-defined collection of services that ensures devices from different manufacturers can work together.

---

Before a client starts interacting with a server, the client is not aware of the nature of the attributes stored on that server. Therefore, the client first performs what’s called **Service Discovery**, where it inquires from the server about the attributes. Only after this discovery process is complete does the client know how to interact with the server. It can then read the values, subscribe to notifications, or write commands based on what it found. Vendors define their own profiles for use cases not covered by the SIG-defined profiles

The complete list of GATT profiles defined by the Bluetooth SIG can be found [here](https://www.bluetooth.com/specifications/specs/)




### Security Manager Protocol (SMP)
This is our security guard. SMP handles the **pairing and bonding** process, which includes:
*   Generating and exchanging encryption keys.
*   Authenticating devices to ensure they are who they say they are.
*   Establishing a secure, encrypted link to prevent eavesdropping.


### Logical Link Control & Adaptation Protocol (L2CAP)
Think of L2CAP as the post office. It's not concerned with the *content* of the message, just its delivery.
*   It takes data from the upper layers, chops it up into manageable **packets**, and passes it down to the Link Layer.
*   It also reassembles incoming packets into coherent data for the upper layers.

### Host Controller Interface (HCI)
This is the literal **interface**—often a hardware protocol like UART or USB—that allows the Host (usually software on a main processor) to communicate with the Controller (a separate Bluetooth chip or module).


## 3. The Controller
The Controller is the muscle. It handles all the real-time, low-level radio operations.

### Link Layer (LL)
The LL manages the state of the radio and directly controls the Physical Layer. It can be in one of several states: **standby, advertising, scanning, initiating, or connected.**

A critical concept at this layer is the **Bluetooth Device Address (BD_ADDR)**. This is a device's unique 48-bit identifier, much like a MAC address for Wi-Fi.
![afd8432dfc239780659747e3e0f57729.png](../../../../_resources/afd8432dfc239780659747e3e0f57729.png)


Let's dissect a typical Public `BD_ADDR`: **`C8:C9:A3:FA:F1:6A`** 

*   **NAP (`C8:C9`):** Non-significant Address Part. The first 16 bits of the manufacturer's OUI. It doesn't affect radio operations like frequency hopping.
*   **UAP (`A3`):** Upper Address Part. The next 8 bits of the OUI.
*   **LAP (`FA:F1:6A`):** Lower Address Part. The last 24 bits, which uniquely identify the device from that manufacturer.

There are two main types of addresses:
*   **Public Address:** A fixed, factory-programmed address registered with the IEEE. It's globally unique.
*   **Random Address:** More common. It doesn't require registration and can be configured by the user. It has sub-types:
    *   **Static Random:** Fixed until the next power cycle. A cheap alternative to a public address.
    *   **Private Random (Resolvable):** Changes frequently using a special key "Identity Resolving Key (IRK)" to prevent tracking. Only trusted devices (that have the IRK) can figure out the device's real identity. This is key for privacy.
    *   **Private Random (Non-Resolvable):** Also changes, but cannot be resolved to a real identity. Rarely used.
![b14c74744289d52444f1857a140ef86f.png](../../../../_resources/b14c74744289d52444f1857a140ef86f.png)


### Physical Layer (PHY)
This is the actual radio wave. The PHY defines how digital ones and zeros are modulated onto a radio frequency carrier. Bluetooth LE defines three PHYs, which are essentially different "gears" for communication:

*   **LE 1M PHY (The Standard Gear):**
    *   The original and mandatory mode. Data rate: **1 Mbps**.
    *   Balanced between range and data rate. This is the default mode for connections.

*   **LE 2M PHY (The Fast Gear):**
    *   Introduced in Bluetooth 5. Data rate: **2 Mbps**.
    *   **Pros:** Twice the speed! Faster data transfer and lower power consumption per bit.
    *   **Cons:** Reduced range and slightly less robust against interference.

*   **LE Coded PHY (The Long-Range Gear):**
    *   Also introduced in Bluetooth 5. It uses forward error correction (FEC) to greatly extend range.
    *   **S=2:** Data rate is **500 Kbps**. Range is about 2x that of 1M PHY.
    *   **S=8:** Data rate is **125 Kbps**. Range is about 4x that of 1M PHY.
    *   Perfect for applications like smart home sensors where devices are far apart and only need to send small amounts of data.

---

I hope this breakdown makes the Bluetooth LE stack clearer. Remember, each layer has a specific job, and they all work together to make wireless communication possible.