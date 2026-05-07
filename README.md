<div align="center">

# 🧬 Reliable Genome Data Transmission Over Noisy Channels

### A Multi-Layer OSI Protocol Stack Simulation for Distributed Genome Sequencing

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue?style=for-the-badge&logo=cplusplus)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-green?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-yellow?style=for-the-badge)
![Build](https://img.shields.io/badge/Build-Passing-brightgreen?style=for-the-badge)

*A from-scratch implementation of OSI Layers 1–3 in Modern C++17 — featuring Go-Back-N Sliding Window ARQ, IP Fragmentation, CRC-32 Error Detection, and a real-world application in reliable genome sequence data delivery over lossy communication channels.*

---

</div>

## 🎯 Project Objective

Modern genome sequencing machines like Illumina HiSeq and Oxford Nanopore produce **terabytes of raw DNA sequence data** (FASTQ format) that must be transmitted to remote analysis servers for alignment, variant calling, and annotation. These data links are subject to **electromagnetic interference, cable crosstalk, and signal attenuation** — any of which can corrupt individual bits.

**A single bit-flip in genome data can misidentify a nucleotide base pair** (e.g., an `A` becomes a `C`), leading to:
- False single-nucleotide polymorphism (SNP) calls
- Misidentification of disease-causing mutations (e.g., BRCA1 in breast cancer)
- Invalid alignment against the reference genome

This project simulates the **reliable transmission of genome sequencing data** across a noisy communication channel using a three-layer OSI protocol stack built from scratch. It demonstrates how networking protocols solve the fundamental problem of **guaranteed data integrity over unreliable channels** — the same problem faced by every distributed bioinformatics pipeline.

---

## 🌍 Real-World Application: Distributed Genome Sequencing

<table>
  <tr>
    <th>Component</th>
    <th>Simulation</th>
    <th>Real-World Equivalent</th>
  </tr>
  <tr>
    <td><strong>Node A (Sequencer)</strong></td>
    <td>Reads a FASTQ file and transmits raw DNA reads</td>
    <td>Illumina HiSeq / NovaSeq / Oxford Nanopore sequencer</td>
  </tr>
  <tr>
    <td><strong>Node B (Analysis Server)</strong></td>
    <td>Receives, reassembles, and verifies genome data</td>
    <td>Cloud bioinformatics server (AWS, Google Genomics, local HPC cluster)</td>
  </tr>
  <tr>
    <td><strong>Noisy Channel</strong></td>
    <td>Configurable bit-flip rate (0–50%)</td>
    <td>EMI on Ethernet cables, signal attenuation on long-haul fiber, wireless interference</td>
  </tr>
  <tr>
    <td><strong>IP Fragmentation</strong></td>
    <td>FASTQ data split into MTU-sized packets</td>
    <td>TCP segmentation of large genomic files for network transport</td>
  </tr>
  <tr>
    <td><strong>CRC-32</strong></td>
    <td>Detects bit errors in each frame</td>
    <td>Ethernet FCS (IEEE 802.3), TCP checksums</td>
  </tr>
  <tr>
    <td><strong>Go-Back-N ARQ</strong></td>
    <td>Retransmits corrupted frames automatically</td>
    <td>TCP sliding window retransmission (RFC 793)</td>
  </tr>
  <tr>
    <td><strong>Integrity Verification</strong></td>
    <td>Byte-by-byte comparison of original vs received</td>
    <td>MD5/SHA-256 checksums used by NCBI SRA, EBI ENA</td>
  </tr>
</table>

### Data Flow: Sequencer to Analysis Server

```
  ┌──────────────────────────────────────────────────────────────┐
  │              GENOME DATA TRANSMISSION PIPELINE               │
  │                                                              │
  │  1. READ       FASTQ file loaded from disk                   │
  │       |        (DNA reads + quality scores)                  │
  │       |                                                      │
  │  2. FRAGMENT   Split into MTU-sized packets (e.g. 20 bytes)  │
  │       |        Each packet: srcIP + dstIP + TTL + fragInfo   │
  │       |                                                      │
  │  3. FRAME      MAC headers + CRC-32 checksum added           │
  │       |        Sequence numbers for Go-Back-N tracking       │
  │       |                                                      │
  │  4. ENCODE     Bytes serialized to binary bit streams        │
  │       |                                                      │
  │  5. TRANSMIT   Bits travel through noisy channel             │
  │       |        EM interference may flip bits!                │
  │       |        A->C, T->G mutations possible!               │
  │       |                                                      │
  │  6. DETECT     CRC-32 catches corrupted frames               │
  │       |        Go-Back-N retransmits automatically           │
  │       |                                                      │
  │  7. REASSEMBLE Fragments collected and joined in order        │
  │       |                                                      │
  │  8. WRITE      Verified genome data saved to disk            │
  │       |                                                      │
  │  9. VERIFY     Byte-by-byte: original == received            │
  │                Zero base-pair errors. Safe for analysis.     │
  └──────────────────────────────────────────────────────────────┘
```

### Why This Matters in Genomics

```
  Without error detection/correction:
  ┌──────────────────────────────────────────────────────────┐
  │  Original DNA read:   ...GATTACA CTGAATTC GCTAGC...     │
  │  After noise:         ...GATTACA CCGAATTC GCTAGC...     │
  │                                  ^                       │
  │                         T -> C (single bit-flip)         │
  │                                                          │
  │  This could be misidentified as a real mutation!         │
  │  In the BRCA1 gene, this causes a false positive for    │
  │  breast cancer susceptibility.                           │
  └──────────────────────────────────────────────────────────┘

  With our protocol stack (CRC-32 + Go-Back-N ARQ):
  ┌──────────────────────────────────────────────────────────┐
  │  Frame received with bit-flip -> CRC-32 MISMATCH        │
  │  Frame DISCARDED -> Go-Back-N retransmits from sender   │
  │  New frame arrives -> CRC-32 MATCH -> accepted           │
  │  Result: EXACT original data, zero false mutations       │
  └──────────────────────────────────────────────────────────┘
```

---

## ✨ Key Features

<table>
  <tr>
    <th>Feature</th>
    <th>Description</th>
    <th>Layer</th>
  </tr>
  <tr>
    <td>🧬 <strong>Genome Data Transfer</strong></td>
    <td>Transmit real FASTQ sequencing files through the protocol stack. Data is fragmented, transmitted over a noisy channel, reassembled, and verified byte-by-byte — ensuring zero base-pair errors.</td>
    <td>Application</td>
  </tr>
  <tr>
    <td>🔀 <strong>Go-Back-N Sliding Window ARQ</strong></td>
    <td>Reliable delivery using a configurable sliding window. Sender transmits up to <em>W</em> frames before waiting, slides on cumulative ACKs, retransmits from the last unacknowledged frame on errors.</td>
    <td>Layer 2</td>
  </tr>
  <tr>
    <td>🧩 <strong>Packet Fragmentation & Reassembly</strong></td>
    <td>FASTQ data split into MTU-sized fragments with <code>fragmentID</code>, <code>fragmentOffset</code>, and <code>moreFragments</code> fields — mimicking real IPv4 fragmentation.</td>
    <td>Layer 3</td>
  </tr>
  <tr>
    <td>🛡️ <strong>CRC-32 Error Detection</strong></td>
    <td>Ethernet-standard CRC-32 (polynomial <code>0xEDB88320</code>) with a 256-entry lookup table. Critical for catching noise-induced base-pair errors.</td>
    <td>Layer 2</td>
  </tr>
  <tr>
    <td>📻 <strong>Noisy Channel Simulation</strong></td>
    <td>Models electromagnetic interference using per-bit flip probability with <code>std::mt19937</code> Mersenne Twister PRNG. Adjustable at runtime (0–50%).</td>
    <td>Physical</td>
  </tr>
  <tr>
    <td>🖥️ <strong>Interactive Mode</strong></td>
    <td>9-option menu: transmit genome data, send custom messages, adjust noise/window/MTU, run demos, view statistics.</td>
    <td>All</td>
  </tr>
  <tr>
    <td>📊 <strong>Statistics Dashboard</strong></td>
    <td>Per-transmission and session metrics: Bit Error Rate (BER), efficiency, frames sent/corrupted/retransmitted.</td>
    <td>All</td>
  </tr>
  <tr>
    <td>📈 <strong>Sequence Diagram</strong></td>
    <td>Auto-generated ASCII diagram visualizing every DATA frame, ACK, noise event, and retransmission.</td>
    <td>All</td>
  </tr>
  <tr>
    <td>🎨 <strong>Color-Coded Output</strong></td>
    <td>ANSI escape codes produce layer-tagged, color-coded console logs for full protocol visibility.</td>
    <td>All</td>
  </tr>
</table>

---

## 🏗️ System Architecture

```
  +------------------------+                          +------------------------+
  |      SEQUENCER         |                          |    ANALYSIS  SERVER     |
  |      (10.0.1.1)        |                          |      (10.0.1.2)        |
  |                        |                          |                        |
  | +--------------------+ |                          | +--------------------+ |
  | |   APPLICATION      | |                          | |   APPLICATION      | |
  | |  FASTQ genome data | |                          | |  Reassembled reads | |
  | +--------+-----------+ |                          | +--------+-----------+ |
  |          |              |                          |          ^              |
  | +--------v-----------+ |                          | +--------+-----------+ |
  | |  LAYER 3 - NETWORK | |                          | |  LAYER 3 - NETWORK | |
  | |  - IP addressing   | |                          | |  - Reassemble      | |
  | |  - TTL (64)        | |                          | |  - TTL check       | |
  | |  - Fragment FASTQ  | |                          | |  - Defragment      | |
  | +--------+-----------+ |                          | +--------+-----------+ |
  |     | N fragments      |                          |     ^ N fragments      |
  | +--------v-----------+ |      Cumulative ACK      | +--------+-----------+ |
  | |  LAYER 2 - DATALINK| |<------------------------| |  LAYER 2 - DATALINK| |
  | |  - MAC framing     | |                          | |  - CRC-32 verify   | |
  | |  - CRC-32          | |                          | |  - Seq# check      | |
  | |  - Go-Back-N ARQ   | |                          | |  - ACK generation  | |
  | +--------+-----------+ |                          | +--------+-----------+ |
  |     | N frames         |                          |     ^ N frames         |
  | +--------v-----------+ |                          | +--------+-----------+ |
  | |  LAYER 1 - PHYSICAL| |                          | |  LAYER 1 - PHYSICAL| |
  | |  byte -> bits      | |                          | |  bits -> byte      | |
  | +--------+-----------+ |                          | +--------+-----------+ |
  +----------+--------------+                          +----------+--------------+
             |                                                    ^
             |          +--------------------------------+        |
             +--------->|     NOISY DATA LINK            |--------+
                        |  (electromagnetic interference)|
                        |  Configurable bit-flip rate    |
                        +--------------------------------+
```

---

## 📂 Project Structure

```
Telecomm_Project/
│
├── CMakeLists.txt                  # CMake build configuration
├── README.md                       # This file
├── genome_sample.fastq             # Sample FASTQ genome data (8 reads)
│
├── include/                        # Header files (10 files)
│   ├── colors.h                    # ANSI color codes & layer-tagged logging
│   ├── packet.h                    # Layer 3 Packet (IP + fragmentation fields)
│   ├── frame.h                     # Layer 2 Frame (MAC + CRC + seq#)
│   ├── network_layer.h             # NetworkLayer: encapsulate, fragment, reassemble
│   ├── datalink_layer.h            # DataLinkLayer: CRC-32, MAC framing, ACK/NACK
│   ├── physical_layer.h            # PhysicalLayer: byte <-> binary encoding
│   ├── medium.h                    # Noisy transmission medium
│   ├── node.h                      # Node: encapsulates all 3 OSI layers
│   ├── statistics.h                # Transmission & session statistics (header-only)
│   └── sequence_diagram.h          # ASCII sequence diagram renderer (header-only)
│
└── src/                            # Source files (6 files)
    ├── main.cpp                    # Interactive orchestrator + Go-Back-N + genome transfer
    ├── network_layer.cpp           # Layer 3: encapsulate, fragment, reassemble
    ├── datalink_layer.cpp          # Layer 2: CRC-32, MAC framing, ACK/NACK
    ├── physical_layer.cpp          # Layer 1: byte <-> binary bit encoding
    ├── medium.cpp                  # Bit-flip noise simulation
    └── node.cpp                    # Node: stack orchestration
```

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

```
  +----------------------------------------------------------+
  |                    INTERACTIVE  MODE                      |
  +----------------------------------------------------------+
  |  [1] Send data     (Sequencer -> Server)                 |
  |  [2] Send data     (Server -> Sequencer)                 |
  |  [3] Change noise level      (current: 0.2%)             |
  |  [4] Change window size      (current: 4)                |
  |  [5] Change MTU              (current: 20 bytes)         |
  |  [6] Transmit genome data    (FASTQ file)                |
  |  [7] Run demo (automatic)                                |
  |  [8] View session statistics                             |
  |  [9] Exit                                                |
  +----------------------------------------------------------+
```

**Suggested experiments:**
| Experiment | How | What You'll See |
|---|---|---|
| Perfect channel | Set noise to 0% | All reads delivered in 1 round, 100% efficiency |
| Heavy interference | Set noise to 2%+ | Go-Back-N retransmissions, low efficiency, high BER |
| Stop-and-Wait behavior | Set window to 1 | One frame at a time — much slower than Go-Back-N |
| Large window | Set window to 8 | More frames in-flight, higher throughput on clean channels |
| Small MTU | Set MTU to 10 | More fragments, more frames, more overhead |
| Large MTU | Set MTU to 50 | Fewer fragments, but each corrupted frame costs more |

---

## 📚 Technical Deep Dives

### 🔀 Go-Back-N Sliding Window Protocol

```
  Sequencer                                Analysis Server
     |                                          |
     |--- DATA [seq=0] ----------------------->|  CRC OK, accept
     |<------------- ACK [seq=0] --------------|  expected -> 1
     |--- DATA [seq=1] ----------------------->|  CRC FAIL (noise!)
     |                                          |  discard, send ACK[0]
     |<------------- ACK [seq=0] --------------|
     |                                          |
     |  *** GO-BACK-N: retransmit from 1 ***    |
     |--- DATA [seq=1] ----------------------->|  CRC OK, accept
     |<------------- ACK [seq=1] --------------|
     |--- DATA [seq=2] ----------------------->|  CRC OK (last read)
     |<------------- ACK [seq=2] --------------|
     |                                          |
     |   [ GENOME DATA DELIVERED INTACT ]       |
```

| Property | Behavior |
|---|---|
| Window Size (W) | Configurable at runtime (default 4) |
| ACK Strategy | Cumulative — ACK[n] acknowledges all frames ≤ n |
| Error Handling | Receiver discards out-of-order or corrupted frames |
| Retransmission | Sender "goes back" to first unacknowledged frame |
| Max Rounds | Default 15 — prevents infinite retransmission loops |

---

### 🧩 IP Fragmentation & Reassembly

```
  FASTQ Read: "@SEQ_RUN_001:LANE4:TILE1101..." (200 bytes)
  MTU: 20 bytes -> 10 fragments

  +---------------------------------------------------------------------+
  | Frag 0 | "@SEQ_RUN_001:LANE4:" (20 bytes) | moreFragments=1         |
  | Frag 1 | "TILE1101:1056:2843 " (20 bytes) | moreFragments=1         |
  | ...    | ...                               | ...                     |
  | Frag 9 | "IIHI"                ( 4 bytes) | moreFragments=0 (LAST)  |
  +---------------------------------------------------------------------+
```

Each fragment carries `fragmentID`, `fragmentOffset`, and `moreFragments` — identical to the fields in a real **IPv4 header** (RFC 791).

---

### 🛡️ CRC-32 Error Detection

Uses the **exact Ethernet CRC-32 algorithm (IEEE 802.3)**:

```cpp
// Polynomial: 0xEDB88320 (bit-reversed 0x04C11DB7)
// 256-entry precomputed lookup table for O(1)-per-byte processing
uint32_t DataLinkLayer::computeCRC(const std::string& data) {
    uint32_t crc = 0xFFFFFFFF;
    for (unsigned char byte : data) {
        crc = crcTable_[(crc ^ byte) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFF;
}
```

In genomics, this is critical: a CRC mismatch means a potential base-pair error. The frame is **discarded** and **retransmitted** rather than risk corrupted DNA data entering the analysis pipeline.

---

### 📊 Statistics Dashboard

```
  +----------------------------------------------------------+
  |              TRANSMISSION STATISTICS DASHBOARD            |
  +----------------------------------------------------------+
  | Direction                   Sequencer -> Analysis-Server  |
  | Payload Size                            1200 bytes        |
  | MTU                                       20 bytes        |
  | Fragments                                       60        |
  | Window Size (Go-Back-N)                          4        |
  | Rounds Used                                     18        |
  +----------------------------------------------------------+
  | Total DATA Frames Sent                          72        |
  | Frames Delivered (OK)                           60        |
  | Frames Corrupted                                12        |
  | Retransmissions                                 12        |
  +----------------------------------------------------------+
  | Bit Error Rate (BER)                         0.12%        |
  | Transmission Efficiency                       83.3%       |
  +----------------------------------------------------------+
  | Final Status                              DELIVERED       |
  +----------------------------------------------------------+
```

---

### 📈 Auto-Generated Sequence Diagram

```
          Sequencer                           Analysis-Server
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
          |                                       |
          [ MESSAGE DELIVERED SUCCESSFULLY ]
```

---

## 🧮 Networking Concepts Demonstrated

| Concept | How It's Demonstrated | Real-World Equivalent |
|---|---|---|
| **OSI Layered Architecture** | Data flows down L3→L2→L1, up L1→L2→L3 | Every networked device |
| **Encapsulation** | Each layer adds headers (IP, MAC, CRC) | HTTP→TCP→IP→Ethernet |
| **IP Addressing** | Packets carry source/destination IPs | IPv4/IPv6 |
| **MAC Addressing** | Frames carry source/destination MACs | Ethernet NICs |
| **TTL** | Each packet starts with TTL=64 | IPv4 TTL / IPv6 Hop Limit |
| **CRC-32 Error Detection** | Checksum verified per frame | Ethernet FCS (IEEE 802.3) |
| **Go-Back-N Flow Control** | Sliding window with cumulative ACKs | TCP sliding window (RFC 793) |
| **IP Fragmentation** | Large data split at L3, reassembled at receiver | IPv4 fragmentation (RFC 791) |
| **Reliable Data Transfer** | Real FASTQ files delivered error-free | FTP, HTTP, SCP |
| **Channel Noise** | Random bit-flips model EMI/attenuation | Wireless, long-haul fiber |
| **ARQ** | Corrupted frames trigger retransmission | TCP retransmission |
| **Cumulative ACKs** | ACK[n] confirms all frames ≤ n | TCP (RFC 793) |
| **Data Integrity** | Post-transfer byte-level verification | MD5/SHA checksums (NCBI SRA) |

---

## 🛠️ Technologies & Design Choices

| Choice | Rationale |
|---|---|
| **C++17** | Modern features: structured bindings, `inline` variables, `std::optional` |
| **Zero External Libraries** | Pure STL — no Boost, no sockets, no OS-specific APIs |
| **Object-Oriented Design** | Each OSI layer is a separate class with clear responsibilities |
| **Header-Only Modules** | Statistics and Sequence Diagram — zero build friction |
| **ANSI Escape Codes** | Cross-platform colored output for protocol visibility |
| **Mersenne Twister PRNG** | High-quality noise simulation (`std::mt19937`) |
| **CRC-32 Lookup Table** | 256-entry table for O(1)-per-byte checksumming |
| **Binary File I/O** | `std::ios::binary` for correct FASTQ/genome data handling |
| **FASTQ Format** | Industry-standard genome sequencing output format |

---

## 📝 License

MIT License — see [LICENSE](LICENSE) for details.
