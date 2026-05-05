/**
 * @file network_layer.cpp
 * @brief Implementation of the Network Layer (Layer 3).
 *
 * Handles encapsulation, decapsulation, fragmentation, and reassembly.
 */

#include "network_layer.h"
#include "colors.h"

#include <sstream>
#include <iomanip>
#include <algorithm>

// =========================================================================
//  Encapsulate -- Sender Side (single packet, no fragmentation)
// =========================================================================

Packet NetworkLayer::encapsulate(const std::string& payload,
                                 const std::string& srcIP,
                                 const std::string& dstIP) {
    Packet pkt;
    pkt.srcIP   = srcIP;
    pkt.dstIP   = dstIP;
    pkt.ttl     = 64;
    pkt.payload = payload;

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

// =========================================================================
//  Decapsulate -- Receiver Side
// =========================================================================

std::string NetworkLayer::decapsulate(const Packet& pkt) {
    std::ostringstream oss;
    oss << "Received Packet.\n"
        << "                        Src IP: " << pkt.srcIP
        << "  ->  Dst IP: " << pkt.dstIP << "\n"
        << "                        TTL: " << pkt.ttl
        << "  |  Fragment [" << pkt.fragmentOffset << "]"
        << (pkt.moreFragments ? " (more follow)" : " (last fragment)")
        << "  |  Size: " << pkt.payload.size() << " bytes";
    logNetwork(oss.str());

    if (pkt.ttl <= 0) {
        logError("Packet dropped -- TTL expired!");
        return "";
    }

    return pkt.payload;
}

// =========================================================================
//  Fragment -- Split a payload into MTU-sized chunks
// =========================================================================

std::vector<Packet> NetworkLayer::fragment(const std::string& payload,
                                           const std::string& srcIP,
                                           const std::string& dstIP,
                                           int fragmentID,
                                           int mtu) {
    std::vector<Packet> fragments;
    int totalSize = static_cast<int>(payload.size());
    int numFragments = (totalSize + mtu - 1) / mtu;  // ceiling division

    std::ostringstream oss;
    oss << "Fragmenting message (" << totalSize << " bytes) with MTU=" << mtu
        << " -> " << numFragments << " fragment(s), ID=" << fragmentID;
    logNetwork(oss.str());

    for (int i = 0; i < numFragments; ++i) {
        int offset = i * mtu;
        int chunkSize = std::min(mtu, totalSize - offset);

        Packet pkt;
        pkt.srcIP          = srcIP;
        pkt.dstIP          = dstIP;
        pkt.ttl            = 64;
        pkt.fragmentID     = fragmentID;
        pkt.fragmentOffset = i;
        pkt.moreFragments  = (i < numFragments - 1);
        pkt.payload        = payload.substr(offset, chunkSize);

        std::ostringstream foss;
        foss << "  Fragment [" << i << "/" << (numFragments - 1) << "]: \""
             << pkt.payload << "\" (" << chunkSize << " bytes)"
             << (pkt.moreFragments ? " [MORE]" : " [LAST]");
        logNetwork(foss.str());

        fragments.push_back(pkt);
    }

    return fragments;
}

// =========================================================================
//  Reassemble -- Reconstruct original payload from fragments
// =========================================================================

std::string NetworkLayer::reassemble(const std::vector<Packet>& fragments) {
    logNetwork("Reassembling " + std::to_string(fragments.size()) + " fragment(s)...");

    // Sort by fragment offset (caller should have done this, but be safe)
    std::vector<Packet> sorted = fragments;
    std::sort(sorted.begin(), sorted.end(),
              [](const Packet& a, const Packet& b) {
                  return a.fragmentOffset < b.fragmentOffset;
              });

    std::string result;
    for (const auto& frag : sorted) {
        result += frag.payload;
    }

    logNetwork("Reassembled payload (" + std::to_string(result.size()) +
               " bytes): \"" + result + "\"");
    return result;
}
