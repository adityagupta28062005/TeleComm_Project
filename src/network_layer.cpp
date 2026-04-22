/**
 * @file network_layer.cpp
 * @brief Implementation of the Network Layer (Layer 3).
 *
 * Handles encapsulation (adding IP headers + TTL) and decapsulation
 * (extracting the payload after verifying TTL).
 */

#include "network_layer.h"
#include "colors.h"

#include <sstream>
#include <iomanip>

// ═══════════════════════════════════════════════════════════════════════════
//  Encapsulate — Sender Side (walking DOWN the stack)
// ═══════════════════════════════════════════════════════════════════════════

Packet NetworkLayer::encapsulate(const std::string& payload,
                                 const std::string& srcIP,
                                 const std::string& dstIP) {
    Packet pkt;
    pkt.srcIP   = srcIP;
    pkt.dstIP   = dstIP;
    pkt.ttl     = 64;          // Standard initial TTL value
    pkt.payload = payload;

    // ── Log the operation ────────────────────────────────────────────────
    std::ostringstream oss;
    oss << "Packaging data into Packet.\n"
        << "                        Src IP: " << srcIP
        << "  ->  Dst IP: " << dstIP << "\n"
        << "                        TTL: " << pkt.ttl
        << "  |  Payload size: " << payload.size() << " bytes\n"
        << "                        Payload: \"" << payload << "\"";
    logNetwork(oss.str());

    return pkt;
}

// ═══════════════════════════════════════════════════════════════════════════
//  Decapsulate — Receiver Side (walking UP the stack)
// ═══════════════════════════════════════════════════════════════════════════

std::string NetworkLayer::decapsulate(const Packet& pkt) {
    // ── Log the received packet info ─────────────────────────────────────
    std::ostringstream oss;
    oss << "Received Packet.\n"
        << "                        Src IP: " << pkt.srcIP
        << "  ->  Dst IP: " << pkt.dstIP << "\n"
        << "                        TTL: " << pkt.ttl
        << "  |  Payload size: " << pkt.payload.size() << " bytes";
    logNetwork(oss.str());

    // ── TTL check ────────────────────────────────────────────────────────
    if (pkt.ttl <= 0) {
        logError("Packet dropped -- TTL expired!");
        return "";
    }

    logNetwork("TTL valid. Extracting payload and passing to Application.");
    return pkt.payload;
}
