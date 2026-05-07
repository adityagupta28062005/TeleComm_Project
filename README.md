<div align="center">

# 📡 OSI Protocol Stack Simulation

### A fully interactive, multi-layer network protocol simulator
### built from scratch in Modern C++17

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue?style=for-the-badge&logo=cplusplus)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-green?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-yellow?style=for-the-badge)
![Build](https://img.shields.io/badge/Build-Passing-brightgreen?style=for-the-badge)

*Simulates the **Network (L3)**, **Data Link (L2)**, and **Physical (L1)** layers of the OSI model — featuring Go-Back-N ARQ, IP fragmentation, CRC-32 error detection, a noisy transmission medium, and **real file transfer with integrity verification** — with zero external dependencies.*

---

</div>

## 🎯 Project Objective

This project provides a **hands-on, educational simulation** of how data traverses the lower layers of the OSI networking model. Two virtual nodes (`Node A` and `Node B`) exchange data through a simulated noisy channel. Each message travels **down** the protocol stack on the sender side (Application → Network → Data Link → Physical), crosses a **noisy medium** that can flip bits, and travels **up** the stack on the receiver side — exactly mirroring real-world network communication.

Beyond being an academic exercise, the simulation includes a **real-world application**: transferring actual files between nodes through the full protocol stack, with automatic integrity verification — demonstrating the same principles used by TCP/IP when you download a file or send an email.

The simulation is fully **interactive**: users can type their own messages, transfer files, adjust noise levels, configure protocol parameters, and observe the effects in real time through color-coded, layer-tagged console output.

---

## 🌍 Real-World Application: Reliable File Transfer

> **This is not just a theoretical simulation — it solves a real problem.**

The simulator includes a fully functional **file transfer system** that demonstrates exactly how real-world protocols like **TCP/IP** deliver data reliably over unreliable networks. When you transfer a file:

```
  ┌──────────────────────────────────────────────────────────────┐
  │                   FILE TRANSFER PIPELINE                     │
  │                                                              │
  │  1. READ       File is read from disk as raw bytes           │
  │       |                                                      │
  │  2. FRAGMENT   Split into MTU-sized chunks (e.g. 20 bytes)   │
  │       |        Each chunk gets IP headers + fragment metadata │
  │       |                                                      │
  │  3. FRAME      Each fragment wrapped with MAC + CRC-32       │
  │       |                                                      │
  │  4. ENCODE     Frames serialized to binary bit streams       │
  │       |                                                      │
  │  5. TRANSMIT   Bits travel through the noisy channel         │
  │       |        Random bit-flips may corrupt data!            │
  │       |                                                      │
  │  6. DETECT     CRC-32 catches corrupted frames               │
  │       |        Go-Back-N retransmits automatically            │
  │       |                                                      │
  │  7. REASSEMBLE Fragments collected and joined in order        │
  │       |                                                      │
  │  8. WRITE      Reconstructed file saved to disk              │
  │       |                                                      │
  │  9. VERIFY     Byte-by-byte comparison: original == received │
  └──────────────────────────────────────────────────────────────┘
```

**This is identical to what happens when you:**
- Download a file from the internet (TCP fragments, checksums, retransmits)
- Send an email attachment (SMTP/TCP ensures reliable delivery)
- Stream a video (UDP/RTP handles packetization, though without retransmission)
- Transfer files over Bluetooth or Wi-Fi (L2 CRC + ARQ handle noise)

### How to Use It

```
  Choice> 6
  Enter path to file: test_file.txt

  [INFO]    Read 404 bytes from: test_file.txt
  [LAYER 3] Fragmenting message (404 bytes) with MTU=20 -> 21 fragment(s)
  [LAYER 3]   Fragment [0/20]: "This is a sample tes" (20 bytes) [MORE]
  [LAYER 3]   Fragment [1/20]: "t file for the OSI P" (20 bytes) [MORE]
  ...
  [LAYER 2] CRC-32 computed for each frame
  [LAYER 1] Encoded to binary bit streams
  [MEDIUM]  Transmitted through noisy channel (0.15% bit-flip rate)
  ...
  [+ SUCCESS] Node B reassembled message (404 bytes)
  [+ SUCCESS] Received file written to: test_file.txt.received

  ================================================================
    FILE INTEGRITY VERIFICATION
  [+ SUCCESS] PASS -- Original and received files are IDENTICAL (404 bytes)
  ================================================================
```

### Why This Matters

| Real-World Concept | How This Simulation Demonstrates It |
|---|---|
| **TCP Segmentation** | Messages are split into MTU-sized fragments, just like TCP segments |
| **IP Fragmentation** | Each fragment carries `fragmentID`, `offset`, and `moreFragments` — identical to IPv4 |
| **Reliable Delivery** | Go-Back-N ARQ ensures every fragment arrives correctly, like TCP's sliding window |
| **Error Detection** | CRC-32 catches bit errors, mirroring Ethernet FCS and TCP checksums |
| **Data Integrity** | Post-transfer verification confirms the received file is byte-identical to the original |
| **Noise Resilience** | The system automatically retransmits corrupted data — the user gets a perfect file despite channel noise |

---

## ✨ Key Features at a Glance

<table>
  <tr>
    <th>Feature</th>
    <th>Description</th>
    <th>Layer</th>
  </tr>
  <tr>
    <td>📁 <strong>File Transfer + Integrity Verification</strong></td>
    <td>Transfer real files through the protocol stack. Files are fragmented, transmitted over the noisy channel, reassembled, and verified byte-by-byte — demonstrating real-world TCP/IP behavior.</td>
    <td>All</td>
  </tr>
  <tr>
    <td>🔀 <strong>Go-Back-N Sliding Window ARQ</strong></td>
    <td>Reliable delivery using a configurable sliding window. Sender transmits up to <em>W</em> frames before waiting, slides on cumulative ACKs, and retransmits from the last unacknowledged frame on errors.</td>
    <td>Layer 2</td>
  </tr>
  <tr>
    <td>🧩 <strong>Packet Fragmentation & Reassembly</strong></td>
    <td>Messages exceeding the MTU are automatically split into smaller fragments with <code>fragmentID</code>, <code>fragmentOffset</code>, and <code>moreFragments</code> fields — mimicking real IP fragmentation. Reassembled at the receiver.</td>
    <td>Layer 3</td>
  </tr>
  <tr>
    <td>🛡️ <strong>CRC-32 Error Detection</strong></td>
    <td>Industry-standard CRC-32 using the Ethernet polynomial <code>0xEDB88320</code> with a precomputed 256-entry lookup table for O(1)-per-byte checksumming.</td>
    <td>Layer 2</td>
  </tr>
  <tr>
    <td>📻 <strong>Noisy Transmission Medium</strong></td>
    <td>Simulates real-world channel impairment with configurable per-bit error probability using <code>std::mt19937</code> Mersenne Twister PRNG.</td>
    <td>Physical</td>
  </tr>
  <tr>
    <td>🖥️ <strong>Interactive Mode</strong></td>
    <td>9-option menu-driven interface: send messages, transfer files, adjust noise/window/MTU at runtime, run automated demos, and view session statistics.</td>
    <td>All</td>
  </tr>
  <tr>
    <td>📊 <strong>Statistics Dashboard</strong></td>
    <td>Per-transmission and session-wide metrics: Bit Error Rate (BER), transmission efficiency, frames sent/corrupted/retransmitted.</td>
    <td>All</td>
  </tr>
  <tr>
    <td>📈 <strong>Sequence Diagram</strong></td>
    <td>Auto-generated ASCII sequence diagram after each transmission, visualizing every DATA frame, ACK, noise event, and retransmission.</td>
    <td>All</td>
  </tr>
  <tr>
    <td>🎨 <strong>Color-Coded Console Output</strong></td>
    <td>ANSI escape codes produce vivid, layer-tagged logs — making it easy to trace data as it flows through each layer.</td>
    <td>All</td>
  </tr>
</table>

---

## 🏗️ System Architecture

```
  +----------------------+                           +----------------------+
  |       NODE  A        |                           |       NODE  B        |
  |    (192.168.1.1)     |                           |    (192.168.1.2)     |
  |                      |                           |                      |
  | +------------------+ |                           | +------------------+ |
  | |  APPLICATION     | |                           | |  APPLICATION     | |
  | | Text / File Data | |                           | |  (Reassembled)   | |
  | +--------+---------+ |                           | +--------+---------+ |
  |          |            |                           |          ^            |
  | +--------v---------+ |                           | +--------+---------+ |
  | |  LAYER 3         | |                           | |  LAYER 3         | |
  | |  NETWORK         | |                           | |  NETWORK         | |
  | |  - IP addressing | |                           | |  - Reassemble    | |
  | |  - TTL (64)      | |                           | |  - TTL check     | |
  | |  - Fragment msg  | |                           | |  - Defragment    | |
  | +--------+---------+ |                           | +--------+---------+ |
  |     | N fragments    |                           |     ^ N fragments    |
  | +--------v---------+ |                           | +--------+---------+ |
  | |  LAYER 2         | |       Cumulative ACK      | |  LAYER 2         | |
  | |  DATA LINK       | |<--------------------------| |  DATA LINK       | |
  | |  - MAC framing   | |                           | |  - CRC verify    | |
  | |  - CRC-32        | |                           | |  - Seq# check    | |
  | |  - Go-Back-N ARQ | |                           | |  - ACK/NACK      | |
  | +--------+---------+ |                           | +--------+---------+ |
  |     | N frames       |                           |     ^ N frames       |
  | +--------v---------+ |                           | +--------+---------+ |
  | |  LAYER 1         | |                           | |  LAYER 1         | |
  | |  PHYSICAL        | |                           | |  PHYSICAL        | |
  | |  byte -> bits    | |                           | |  bits -> byte    | |
  | +--------+---------+ |                           | +--------+---------+ |
  +----------+------------+                           +----------+------------+
             |                                                   ^
             |          +-------------------------------+        |
             +--------->|   TRANSMISSION  MEDIUM        |--------+
                        |   ~~ Noisy Channel ~~         |
                        |   (configurable bit-flip %)   |
                        +-------------------------------+
```

---

## 📂 Project Structure

```
Telecomm_Project/
│
├── CMakeLists.txt                  # CMake build configuration
├── README.md                       # This file
├── test_file.txt                   # Sample file for file transfer demo
│
├── include/                        # Header files (10 files)
│   ├── colors.h                    # ANSI color codes & layer-tagged logging functions
│   ├── packet.h                    # Layer 3 Packet struct (IP + fragmentation fields)
│   ├── frame.h                     # Layer 2 Frame struct (MAC + CRC + seq#)
│   ├── network_layer.h             # NetworkLayer class declaration
│   ├── datalink_layer.h            # DataLinkLayer class declaration
│   ├── physical_layer.h            # PhysicalLayer class declaration
│   ├── medium.h                    # Noisy transmission medium
│   ├── node.h                      # Node class (encapsulates all 3 layers)
│   ├── statistics.h                # Transmission & session statistics (header-only)
│   └── sequence_diagram.h          # ASCII sequence diagram renderer (header-only)
│
└── src/                            # Source files (6 files)
    ├── main.cpp                    # Interactive orchestrator + Go-Back-N + file transfer
    ├── network_layer.cpp           # Layer 3: encapsulate, fragment, reassemble
    ├── datalink_layer.cpp          # Layer 2: CRC-32, MAC framing, ACK/NACK
    ├── physical_layer.cpp          # Layer 1: byte <-> binary bit encoding
    ├── medium.cpp                  # Bit-flip noise simulation
    └── node.cpp                    # Node: stack orchestration
```

> **19 files total** — 10 headers + 6 source files + CMakeLists.txt + README + test file

---

## 🔧 Building & Running

### Prerequisites

- A C++17 compatible compiler (g++ 7+, clang++ 5+, or MSVC 2017+)
- No external libraries required — only the C++ Standard Template Library (STL)

### Option 1: g++ (Recommended)

```bash
g++ -std=c++17 -Wall -Wextra -Iinclude \
    src/main.cpp src/network_layer.cpp src/datalink_layer.cpp \
    src/physical_layer.cpp src/medium.cpp src/node.cpp \
    -o protocol_sim

./protocol_sim
```

### Option 2: CMake

```bash
mkdir build && cd build
cmake ..
cmake --build .
./protocol_sim
```

### Option 3: Windows (PowerShell)

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude `
    src/main.cpp src/network_layer.cpp src/datalink_layer.cpp `
    src/physical_layer.cpp src/medium.cpp src/node.cpp `
    -o protocol_sim.exe

.\protocol_sim.exe
```

---

## 🖥️ Interactive Mode

The simulation starts with a fully interactive, 9-option menu-driven interface:

```
  +----------------------------------------------------------+
  |                    INTERACTIVE  MODE                      |
  +----------------------------------------------------------+
  |  [1] Send message  (Node A -> Node B)                    |
  |  [2] Send message  (Node B -> Node A)                    |
  |  [3] Change noise level      (current: 0.2%)             |
  |  [4] Change window size      (current: 4)                |
  |  [5] Change MTU              (current: 20 bytes)         |
  |  [6] Transfer a file         (A -> B)                    |
  |  [7] Run demo (automatic)                                |
  |  [8] View session statistics                             |
  |  [9] Exit                                                |
  +----------------------------------------------------------+
```

Users can experiment with different configurations to observe how noise, window size, and MTU affect protocol behavior — making it a powerful educational and experimentation tool.

**Example experiments a user can run:**
- Set noise to 0% → observe perfect delivery with no retransmissions
- Set noise to 5% → watch Go-Back-N retransmit aggressively, see BER rise in statistics
- Change window size from 1 (Stop-and-Wait behavior) to 8 → compare throughput
- Change MTU from 10 to 50 → observe how fragment count affects efficiency
- Transfer a file at different noise levels → verify integrity always passes when delivered

---

## 📚 Technical Deep Dives

### 📁 File Transfer with Integrity Verification

The file transfer feature bridges the gap between academic simulation and real-world application. It demonstrates that the protocol stack built in this project is capable of **reliably delivering arbitrary binary data** — the same fundamental requirement that every networking protocol must satisfy.

**Complete Transfer Flow:**

```
                    NODE A (Sender)                         NODE B (Receiver)
                    ===============                         =================

  [disk]  test_file.txt (404 bytes)
     |
     v
  [L3 NETWORK]  Fragment into 21 chunks
     |           (20 bytes each, last = 4 bytes)
     |           Each gets: srcIP, dstIP, TTL=64,
     |           fragmentID=1, offset=0..20, moreFragments
     v
  [L2 DATALINK]  Frame each fragment
     |            Add: srcMAC, dstMAC, seqNum, CRC-32
     v
  [L1 PHYSICAL]  Encode to binary bit stream
     |
     v
  [MEDIUM]  ~~~~~~~~~~~~~ noisy channel ~~~~~~~~~~~~~
     |       Some bits flip randomly (0.15% per bit)
     v
                                                     [L1 PHYSICAL]  Decode bits -> bytes
                                                          |
                                                          v
                                                     [L2 DATALINK]  Verify CRC-32
                                                          |          - OK?  -> ACK, accept
                                                          |          - FAIL? -> ACK(last good)
                                                          |                    sender retransmits!
                                                          v
                                                     [L3 NETWORK]  Collect all 21 fragments
                                                          |         Reassemble in order
                                                          v
                                                     [disk]  test_file.txt.received (404 bytes)
                                                          |
                                                          v
                                                     [VERIFY]  SHA-like byte comparison
                                                               Original == Received?
                                                               -> PASS: Files are IDENTICAL
```

**Real-world protocols this mirrors:**

| This Simulation | Real Protocol |
|---|---|
| `fragment()` with MTU | TCP segmentation / IPv4 fragmentation |
| `fragmentID` + `offset` + `moreFragments` | IPv4 header fields (Identification, Fragment Offset, MF flag) |
| Go-Back-N window | TCP sliding window (RFC 793) |
| CRC-32 checksum | Ethernet FCS (IEEE 802.3) |
| Cumulative ACK | TCP cumulative acknowledgement |
| Noisy medium | Wireless channels, congested networks, long-haul fiber |
| Integrity verification | MD5/SHA checksums used by download managers |

---

### 🔀 Go-Back-N Sliding Window Protocol

The Go-Back-N ARQ protocol ensures **reliable, in-order delivery** over a noisy channel. It replaces the simpler Stop-and-Wait ARQ with a windowed approach for higher throughput.

**Algorithm:**

```
  Sender (A)                              Receiver (B)
     |                                         |
     |--- DATA [seq=0] ---------------------->|  CRC OK, expected=0, accept
     |<------------- ACK [seq=0] -------------|  expected -> 1
     |--- DATA [seq=1] ---------------------->|  CRC FAIL (noise!)
     |                                         |  discard, re-send ACK[0]
     |<------------- ACK [seq=0] -------------|
     |                                         |
     |  *** GO-BACK-N: retransmit from 1 ***   |
     |                                         |
     |--- DATA [seq=1] ---------------------->|  CRC OK, accept
     |<------------- ACK [seq=1] -------------|
     |--- DATA [seq=2] ---------------------->|  CRC OK (last fragment)
     |<------------- ACK [seq=2] -------------|
     |                                         |
     |      [ MESSAGE DELIVERED ]              |
```

**Key behaviors:**
| Property | Behavior |
|---|---|
| Window Size (W) | Configurable at runtime (default 4), sender transmits up to W frames before waiting |
| ACK Strategy | Cumulative — ACK[n] acknowledges all frames ≤ n |
| Error Handling | Receiver discards out-of-order or corrupted frames |
| Retransmission | Sender "goes back" to first unacknowledged frame and retransmits entire window |
| Max Rounds | Configurable limit (default 15) prevents infinite loops |

---

### 🧩 IP Packet Fragmentation & Reassembly

Messages larger than the **Maximum Transmission Unit (MTU)** are automatically fragmented at Layer 3, transmitted as individual frames, and reassembled at the receiver:

```
  Original message: "Hello from Node A! This is a test message."  (42 bytes)
  MTU: 20 bytes

  +------------------------------------------------------------------+
  | Fragment 0 | "Hello from Node A! T"  (20 bytes) | moreFragments=1 |
  | Fragment 1 | "his is a test messag"  (20 bytes) | moreFragments=1 |
  | Fragment 2 | "e."                    ( 2 bytes) | moreFragments=0 |
  +------------------------------------------------------------------+

  Each fragment carries:
    - fragmentID     : Links all fragments of the same message
    - fragmentOffset : Ordering index (0, 1, 2, ...)
    - moreFragments  : Boolean flag (1 = more follow, 0 = last fragment)
```

Each fragment is independently framed at Layer 2 with its own CRC-32 and sequence number, transmitted via Go-Back-N, and reassembled in order at the destination.

---

### 🛡️ CRC-32 Error Detection

The simulation implements the **exact CRC-32 algorithm used by Ethernet (IEEE 802.3)**:

- **Polynomial:** `0xEDB88320` (bit-reversed representation of `0x04C11DB7`)
- **Lookup Table:** Precomputed 256-entry table for O(1)-per-byte processing
- **Process:** CRC is computed over the entire frame payload at the sender and verified at the receiver
- **On mismatch:** Frame is marked as corrupted → receiver discards → sender retransmits via Go-Back-N

```cpp
// CRC-32 core computation (from datalink_layer.cpp)
uint32_t DataLinkLayer::computeCRC(const std::string& data) {
    uint32_t crc = 0xFFFFFFFF;
    for (unsigned char byte : data) {
        crc = crcTable_[(crc ^ byte) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFF;
}
```

---

### 📻 Noisy Transmission Medium

The medium simulates real-world channel noise using **independent bit-flip errors**:

- Each bit has a configurable probability `p` of being flipped (0 → 1 or 1 → 0)
- Implemented using `std::mt19937` (Mersenne Twister) + `std::uniform_real_distribution`
- Default: 0.15% per bit — produces realistic mix of clean and corrupted frames
- Adjustable at runtime via the interactive menu (0% to 50%)

**Real-world noise sources this models:**
- Electromagnetic interference in copper cables
- Signal attenuation in long-distance fiber optics
- Multipath fading in wireless (Wi-Fi, Bluetooth, cellular)
- Cosmic ray bit-flips in satellite communications

---

### 📊 Transmission Statistics Dashboard

After every transmission, a comprehensive statistics table is displayed:

```
  +----------------------------------------------------------+
  |              TRANSMISSION STATISTICS DASHBOARD            |
  +----------------------------------------------------------+
  | Direction                        Node A -> Node B        |
  | Payload Size                              42 bytes       |
  | MTU                                       20 bytes       |
  | Fragments                                        3       |
  | Window Size (Go-Back-N)                          4       |
  | Rounds Used                                      3       |
  +----------------------------------------------------------+
  | Total DATA Frames Sent                           5       |
  | Frames Delivered (OK)                            3       |
  | Frames Corrupted                                 2       |
  | Retransmissions                                  2       |
  +----------------------------------------------------------+
  | Total Bits Transmitted                        4950       |
  | Total Bits Flipped                               4       |
  | Bit Error Rate (BER)                         0.081%      |
  | Transmission Efficiency                       60.0%      |
  +----------------------------------------------------------+
  | Final Status                              DELIVERED      |
  +----------------------------------------------------------+
```

**Computed Metrics:**
- **Bit Error Rate (BER):** `bits_flipped / bits_transmitted × 100%` — the fundamental measure of channel quality
- **Transmission Efficiency:** `frames_delivered / frames_sent × 100%` — measures protocol overhead from retransmissions

Session-wide cumulative statistics track all transmissions across the entire interactive session.

---

### 📈 Auto-Generated Sequence Diagram

After each transmission, the simulation renders an ASCII sequence diagram showing the exact message flow:

```
  +----------------------------------------------------------+
  |                   SEQUENCE  DIAGRAM                       |
  +----------------------------------------------------------+

          Node A                                  Node B
          |                                       |
          |-- DATA [seq=0] ---------------------->|
          |                                       |-- CRC: OK [seq=0]
          |<----------------------- ACK [seq=0] --|
          |-- DATA [seq=1] ---------------------->|
          |        [!] noise (2 bit flips)        |
          |                                       |-- CRC: FAIL [seq=1]
          |<----------------------- ACK [seq=0] --|
          |*** GO-BACK-N: retransmit from seq=1 ***|
          |-- DATA [seq=1] ---------------------->|
          |                                       |-- CRC: OK [seq=1]
          |<----------------------- ACK [seq=1] --|
          |-- DATA [seq=2] ---------------------->|
          |                                       |-- CRC: OK [seq=2]
          |<----------------------- ACK [seq=2] --|
          |                                       |
          [ MESSAGE DELIVERED SUCCESSFULLY ]
```

This makes it visually obvious when noise corrupts a frame, how the receiver rejects it, and how Go-Back-N triggers retransmission — providing the same visibility that tools like **Wireshark** offer for real network traffic.

---

## 🧮 Networking Concepts Demonstrated

| Concept | How It's Demonstrated | Real-World Equivalent |
|---|---|---|
| **OSI Layered Architecture** | Data flows down L3→L2→L1 at sender, up L1→L2→L3 at receiver | Every networked device follows this model |
| **Encapsulation / Decapsulation** | Each layer wraps data with its own headers (IP, MAC, CRC) | HTTP→TCP→IP→Ethernet header stacking |
| **Logical Addressing (IP)** | Packets carry source/destination IP addresses | IPv4/IPv6 addressing |
| **Physical Addressing (MAC)** | Frames carry source/destination MAC addresses | Ethernet MAC addresses |
| **Time-To-Live (TTL)** | Each packet starts with TTL=64; checked at receiver | IPv4 TTL / IPv6 Hop Limit |
| **Error Detection (CRC-32)** | Frames include a checksum verified before acceptance | Ethernet FCS, Wi-Fi FCS |
| **Flow Control (Go-Back-N)** | Sliding window manages frame pipelining and retransmission | TCP sliding window (RFC 793) |
| **IP Fragmentation** | Large payloads split at L3, reassembled at destination | IPv4 fragmentation (RFC 791) |
| **Reliable File Transfer** | Real files sent through the stack with integrity verification | FTP, HTTP downloads, email (SMTP) |
| **Bit-Level Encoding** | Bytes serialized into binary bit strings at L1 | Manchester encoding, 8b/10b |
| **Channel Noise** | Random bit-flips model real-world transmission errors | EMI, signal attenuation, crosstalk |
| **ARQ (Automatic Repeat reQuest)** | Corrupted frames trigger automatic retransmission | TCP retransmission, Wi-Fi ARQ |
| **Cumulative Acknowledgements** | ACK[n] confirms receipt of all frames up to n | TCP cumulative ACK (RFC 793) |
| **Data Integrity Verification** | Post-transfer byte-by-byte comparison | MD5/SHA checksums on downloads |

---

## 🛠️ Technologies & Design Choices

| Choice | Rationale |
|---|---|
| **C++17** | Modern language features: structured bindings, `std::optional`, `inline` variables |
| **Zero External Libraries** | Pure STL — no Boost, no sockets, no OS-specific APIs |
| **Object-Oriented Design** | Each OSI layer is a separate class with clear responsibilities |
| **Header-Only Modules** | Statistics and Sequence Diagram are header-only for easy integration |
| **ANSI Escape Codes** | Cross-platform colored output without external libraries |
| **Mersenne Twister PRNG** | High-quality randomness for noise simulation (`std::mt19937`) |
| **CRC-32 Lookup Table** | 256-entry precomputed table for efficient checksumming |
| **Binary File I/O** | File transfer uses `std::ios::binary` for correct handling of any file type |

---

## 📝 License

MIT License — see [LICENSE](LICENSE) for details.
