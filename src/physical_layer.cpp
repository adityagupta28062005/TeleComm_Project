/**
 * @file physical_layer.cpp
 * @brief Implementation of the Physical Layer (Layer 1).
 *
 * Converts between byte-level data and a binary bit stream
 * (a string of '0' and '1' characters separated by spaces).
 */

#include "physical_layer.h"
#include "colors.h"

#include <sstream>
#include <bitset>
#include <algorithm>

// ═══════════════════════════════════════════════════════════════════════════
//  Encode — bytes → bits  (Sender side, Layer 2 → wire)
// ═══════════════════════════════════════════════════════════════════════════

std::string PhysicalLayer::encode(const std::string& data) {
    std::ostringstream bitStream;

    for (size_t i = 0; i < data.size(); ++i) {
        // Convert each byte to an 8-bit binary representation
        std::bitset<8> bits(static_cast<unsigned char>(data[i]));
        bitStream << bits.to_string();

        // Separate octets with a space (except the last one)
        if (i + 1 < data.size()) {
            bitStream << ' ';
        }
    }

    std::string result = bitStream.str();

    // ── Log a preview of the first 80 bit-characters ─────────────────────
    std::string preview = (result.size() > 80)
                              ? result.substr(0, 80) + "..."
                              : result;
    logPhysical("Encoding frame to binary bit stream.");
    logPhysical("Bit stream (" + std::to_string(data.size()) +
                " bytes -> " + std::to_string(data.size() * 8) + " bits):");
    logPhysical("  " + preview);

    return result;
}

// ═══════════════════════════════════════════════════════════════════════════
//  Decode — bits → bytes  (Receiver side, wire → Layer 2)
// ═══════════════════════════════════════════════════════════════════════════

std::string PhysicalLayer::decode(const std::string& bitStream) {
    logPhysical("Decoding binary bit stream back to bytes.");

    // Strip spaces to get a contiguous stream of '0' / '1'
    std::string stripped;
    stripped.reserve(bitStream.size());
    for (char c : bitStream) {
        if (c == '0' || c == '1') {
            stripped += c;
        }
    }

    // Convert every 8 bits into a byte
    std::string decoded;
    decoded.reserve(stripped.size() / 8);
    for (size_t i = 0; i + 8 <= stripped.size(); i += 8) {
        std::bitset<8> bits(stripped.substr(i, 8));
        decoded += static_cast<char>(bits.to_ulong());
    }

    logPhysical("Decoded " + std::to_string(decoded.size()) + " bytes from bit stream.");
    return decoded;
}
