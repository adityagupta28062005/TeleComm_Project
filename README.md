# 🌐 OSI Protocol Stack Simulation

> A fully functional simulation of the **Network Layer (L3)**, **Data Link Layer (L2)**, and **Physical Layer (L1)** of the OSI model, built in **Modern C++17** using only the Standard Template Library.

---

## ✨ Features

| Feature | Description |
|---|---|
| **Three-Layer Simulation** | Network (IP addressing, TTL), Data Link (MAC framing, CRC-32), Physical (bit encoding) |
| **CRC-32 Error Detection** | Industry-standard polynomial `0xEDB88320` with a 256-entry lookup table |
| **Stop-and-Wait ARQ** | Sender waits for ACK/NACK; automatic retransmission on corruption |
| **Noisy Medium** | Configurable per-bit flip probability using `<random>` (Mersenne Twister) |
| **Colorful Console Output** | ANSI escape codes trace every layer operation in vivid color |
| **Pure C++17** | No external libraries — only the STL |

---

## 📁 Project Structure

```
Telecomm_Project/
├── CMakeLists.txt              # CMake build configuration
├── README.md                   # This file
├── include/
│   ├── colors.h                # ANSI color helpers & layer-tagged logging
│   ├── packet.h                # Layer 3 — Packet struct (serialize/deserialize)
│   ├── frame.h                 # Layer 2 — Frame struct (serialize/deserialize)
│   ├── network_layer.h         # NetworkLayer class header
│   ├── datalink_layer.h        # DataLinkLayer class header
│   ├── physical_layer.h        # PhysicalLayer class header
│   ├── medium.h                # Simulated noisy channel header
│   └── node.h                  # Node class header (encapsulates all layers)
└── src/
    ├── main.cpp                # Entry point & orchestrator
    ├── network_layer.cpp       # Layer 3 implementation
    ├── datalink_layer.cpp      # Layer 2 implementation (CRC-32 + ARQ)
    ├── physical_layer.cpp      # Layer 1 implementation
    ├── medium.cpp              # Noisy medium implementation
    └── node.cpp                # Node implementation
```

---

## 🔨 Building & Running

### Option 1: CMake (Recommended)

```bash
# From the project root directory:
mkdir build && cd build
cmake ..
cmake --build .

# Run:
./protocol_sim        # Linux / macOS
protocol_sim.exe      # Windows
```

### Option 2: Direct g++ Compilation

```bash
g++ -std=c++17 -Wall -Wextra -Iinclude \
    src/main.cpp src/network_layer.cpp src/datalink_layer.cpp \
    src/physical_layer.cpp src/medium.cpp src/node.cpp \
    -o protocol_sim
./protocol_sim
```

### Option 3: MSVC (Windows)

```powershell
cl /std:c++17 /EHsc /W4 /Iinclude ^
    src\main.cpp src\network_layer.cpp src\datalink_layer.cpp ^
    src\physical_layer.cpp src\medium.cpp src\node.cpp ^
    /Fe:protocol_sim.exe
.\protocol_sim.exe
```

---

## 🏗️ Architecture Overview

```
  ┌──────────────────────┐              ┌──────────────────────┐
  │       NODE  A        │              │       NODE  B        │
  │                      │              │                      │
  │ ┌──────────────────┐ │              │ ┌──────────────────┐ │
  │ │  Application     │ │              │ │  Application     │ │
  │ │  (Text Message)  │ │              │ │  (Text Message)  │ │
  │ └────────┬─────────┘ │              │ └────────▲─────────┘ │
  │          │  payload   │              │          │  payload   │
  │ ┌────────▼─────────┐ │              │ ┌────────┴─────────┐ │
  │ │  Layer 3         │ │              │ │  Layer 3         │ │
  │ │  NETWORK         │ │              │ │  NETWORK         │ │
  │ │  +IP, +TTL       │ │              │ │  -IP, -TTL check │ │
  │ └────────┬─────────┘ │              │ └────────▲─────────┘ │
  │          │  packet    │              │          │  packet    │
  │ ┌────────▼─────────┐ │              │ ┌────────┴─────────┐ │
  │ │  Layer 2         │ │              │ │  Layer 2         │ │
  │ │  DATA LINK       │ │              │ │  DATA LINK       │ │
  │ │  +MAC, +CRC      │ │◄── ACK/NACK──│ │  CRC verify      │ │
  │ └────────┬─────────┘ │              │ └────────▲─────────┘ │
  │          │  frame     │              │          │  frame     │
  │ ┌────────▼─────────┐ │              │ ┌────────┴─────────┐ │
  │ │  Layer 1         │ │              │ │  Layer 1         │ │
  │ │  PHYSICAL        │ │              │ │  PHYSICAL        │ │
  │ │  byte → bits     │ │              │ │  bits → byte     │ │
  │ └────────┬─────────┘ │              │ └────────▲─────────┘ │
  │          │            │              │          │            │
  └──────────┼────────────┘              └──────────┼────────────┘
             │          TRANSMISSION MEDIUM          │
             │  ┌───────────────────────────────┐   │
             └──►  ~~~ Noisy Channel ~~~  ⚡ 🔀  ├───┘
                │  (configurable bit-flip rate)  │
                └───────────────────────────────┘
```

---

## 📚 Technical Details

### Layer 3 — Network Layer

- **Encapsulation**: Wraps the text payload in a `Packet` struct with source IP, destination IP, and TTL (set to 64).
- **Decapsulation**: Extracts the payload after verifying the TTL > 0.
- **Serialization**: Uses pipe (`|`) delimited format: `srcIP|dstIP|TTL|payload`.

### Layer 2 — Data Link Layer

- **Framing**: Wraps the serialized packet with source/destination MAC addresses, a sequence number, and a CRC-32 checksum.
- **CRC-32**: Computed using the standard Ethernet polynomial `0xEDB88320` (reflected form) with a 256-entry precomputed lookup table.
- **Frame Types**: `DATA`, `ACK`, `NACK` — used by the ARQ protocol.

### Layer 1 — Physical Layer

- **Encoding**: Each byte is converted to 8 binary digits using `std::bitset<8>`. Bytes are space-separated for readability.
- **Decoding**: Strips spaces, groups bits into octets, converts back to bytes.

### Transmission Medium

- Uses `std::mt19937` (Mersenne Twister PRNG) seeded from `std::chrono::steady_clock`.
- For each bit in the stream, draws from `std::uniform_real_distribution<>(0, 1)`.
- If the random value falls below the configured noise probability, the bit is flipped.

---

## 🔄 Stop-and-Wait ARQ Protocol

```
  Sender (A)                    Receiver (B)
     │                               │
     │──── DATA (seq=0) ────────────►│
     │           ⚡ noise ⚡          │
     │                               │── CRC check: FAIL
     │◄──── NACK (seq=0) ───────────│
     │                               │
     │──── DATA (seq=0) [retx] ────►│
     │          (clean)              │
     │                               │── CRC check: PASS
     │◄──── ACK  (seq=0) ───────────│
     │                               │
     ▼                               ▼
   Done!                          Message received ✓
```

1. **Sender** encapsulates the message (L3 → L2 → L1) and transmits.
2. **Medium** introduces random bit flips.
3. **Receiver** decodes (L1 → L2 → L3) and checks CRC-32.
4. If CRC **matches** → sends **ACK** → transmission complete.
5. If CRC **mismatches** → sends **NACK** → sender retransmits (up to 5 times).

---

## 🎨 Sample Console Output

```
[LAYER 3 - NETWORK]   Packaging data into Packet.
                       Src IP: 192.168.1.1  ->  Dst IP: 192.168.1.2
                       TTL: 64  |  Payload size: 42 bytes
[LAYER 2 - DATALINK]  Framing packet. Calculated CRC-32: 0xA4F27C3E
[LAYER 1 - PHYSICAL]  Encoding to binary: 01001000 01100101 ...
[MEDIUM]              ⚠️ NOISE DETECTED at bit position 47: 0 -> 1
[LAYER 2 - DATALINK]  CRC mismatch => MISMATCH ✗
[✗ ERROR]             CRC mismatch detected — frame is CORRUPTED!
[LAYER 2 - DATALINK]  Node B sending NACK back to Node A...
[✗ ERROR]             Received NACK for Seq #0 — must RETRANSMIT!
[LAYER 2 - DATALINK]  Retransmitting frame...
[✓ SUCCESS]           Transmission SUCCESSFUL after 2 attempt(s).
```

> **Note**: Output uses ANSI escape codes for color. Use a modern terminal (Windows Terminal, iTerm2, GNOME Terminal) for the best experience.

---

## 📜 License

This project is released under the **MIT License**. See below:

```
MIT License

Copyright (c) 2026

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
