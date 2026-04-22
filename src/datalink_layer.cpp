/**
 * @file datalink_layer.cpp
 * @brief Implementation of the Data Link Layer (Layer 2).
 *
 * Provides:
 *  - Frame encapsulation / decapsulation with MAC addressing.
 *  - CRC-32 computation using the reflected polynomial 0xEDB88320.
 *  - CRC verification on received frames.
 *  - ACK / NACK control frame creation for Stop-and-Wait ARQ.
 */

#include "datalink_layer.h"
#include "colors.h"

#include <sstream>
#include <iomanip>
#include <vector>

// ═══════════════════════════════════════════════════════════════════════════
//  CRC-32 Lookup Table (generated once, used for all computations)
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief Build a 256-entry CRC-32 lookup table using the reflected
 *        polynomial 0xEDB88320 (standard Ethernet / ZIP / PNG CRC).
 * @return A vector of 256 precomputed CRC values.
 */
static std::vector<uint32_t> buildCRCTable() {
    std::vector<uint32_t> table(256);
    for (uint32_t i = 0; i < 256; ++i) {
        uint32_t crc = i;
        for (int bit = 0; bit < 8; ++bit) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320u;   // reflected polynomial
            else
                crc >>= 1;
        }
        table[i] = crc;
    }
    return table;
}

/// Global CRC table — built once at program startup.
static const std::vector<uint32_t> CRC_TABLE = buildCRCTable();

// ═══════════════════════════════════════════════════════════════════════════
//  CRC-32 Computation
// ═══════════════════════════════════════════════════════════════════════════

uint32_t DataLinkLayer::computeCRC(const std::string& data) {
    uint32_t crc = 0xFFFFFFFFu;   // Initial value (all 1s)

    for (unsigned char byte : data) {
        uint8_t index = static_cast<uint8_t>((crc ^ byte) & 0xFF);
        crc = (crc >> 8) ^ CRC_TABLE[index];
    }

    return crc ^ 0xFFFFFFFFu;     // Final XOR
}

// ═══════════════════════════════════════════════════════════════════════════
//  Encapsulate — Sender Side
// ═══════════════════════════════════════════════════════════════════════════

Frame DataLinkLayer::encapsulate(const std::string& serializedPacket,
                                 const std::string& srcMAC,
                                 const std::string& dstMAC,
                                 int seqNum) {
    Frame frame;
    frame.srcMAC           = srcMAC;
    frame.dstMAC           = dstMAC;
    frame.type             = FrameType::DATA;
    frame.seqNum           = seqNum;
    frame.encapsulatedData = serializedPacket;

    // Compute CRC over the data that the receiver will also hash.
    // We hash:  srcMAC + dstMAC + seqNum + encapsulatedData
    std::string toHash = srcMAC + dstMAC +
                         std::to_string(seqNum) + serializedPacket;
    frame.crc = computeCRC(toHash);

    // ── Log ──────────────────────────────────────────────────────────────
    std::ostringstream oss;
    oss << "Framing packet into DATA Frame.\n"
        << "                        Src MAC: " << srcMAC
        << "  ->  Dst MAC: " << dstMAC << "\n"
        << "                        Seq #: " << seqNum
        << "  |  Calculated CRC-32: 0x"
        << std::uppercase << std::hex << std::setfill('0')
        << std::setw(8) << frame.crc;
    logDataLink(oss.str());

    return frame;
}

// ═══════════════════════════════════════════════════════════════════════════
//  Decapsulate — Receiver Side
// ═══════════════════════════════════════════════════════════════════════════

std::string DataLinkLayer::decapsulate(const Frame& frame) {
    std::ostringstream oss;
    oss << "Decapsulating Frame.\n"
        << "                        Src MAC: " << frame.srcMAC
        << "  ->  Dst MAC: " << frame.dstMAC << "\n"
        << "                        Seq #: " << frame.seqNum
        << "  |  Frame type: " << frameTypeToString(frame.type);
    logDataLink(oss.str());

    return frame.encapsulatedData;
}

// ═══════════════════════════════════════════════════════════════════════════
//  CRC Verification
// ═══════════════════════════════════════════════════════════════════════════

bool DataLinkLayer::verifyCRC(const Frame& frame) {
    // Recompute the CRC from the same fields that were hashed at send time.
    std::string toHash = frame.srcMAC + frame.dstMAC +
                         std::to_string(frame.seqNum) +
                         frame.encapsulatedData;
    uint32_t computed = computeCRC(toHash);

    std::ostringstream oss;
    oss << "CRC Verification:  Received CRC = 0x"
        << std::uppercase << std::hex << std::setfill('0')
        << std::setw(8) << frame.crc
        << "  |  Computed CRC = 0x"
        << std::setw(8) << computed;

    if (computed == frame.crc) {
        oss << "  =>  MATCH [OK]";
        logDataLink(oss.str());
        return true;
    } else {
        oss << "  =>  MISMATCH [FAIL]";
        logDataLink(oss.str());
        logError("CRC mismatch detected -- frame is CORRUPTED!");
        return false;
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//  ARQ Control Frames
// ═══════════════════════════════════════════════════════════════════════════

Frame DataLinkLayer::createACK(const std::string& srcMAC,
                               const std::string& dstMAC,
                               int seqNum) {
    Frame ack;
    ack.srcMAC           = srcMAC;
    ack.dstMAC           = dstMAC;
    ack.type             = FrameType::ACK;
    ack.seqNum           = seqNum;
    ack.encapsulatedData = "";   // No payload in control frames

    std::string toHash = srcMAC + dstMAC + std::to_string(seqNum);
    ack.crc = computeCRC(toHash);

    logDataLink("Created ACK frame for Seq #" + std::to_string(seqNum));
    return ack;
}

Frame DataLinkLayer::createNACK(const std::string& srcMAC,
                                const std::string& dstMAC,
                                int seqNum) {
    Frame nack;
    nack.srcMAC           = srcMAC;
    nack.dstMAC           = dstMAC;
    nack.type             = FrameType::NACK;
    nack.seqNum           = seqNum;
    nack.encapsulatedData = "";

    std::string toHash = srcMAC + dstMAC + std::to_string(seqNum);
    nack.crc = computeCRC(toHash);

    logDataLink("Created NACK frame for Seq #" + std::to_string(seqNum));
    return nack;
}
