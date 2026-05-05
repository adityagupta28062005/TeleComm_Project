/**
 * @file network_layer.h
 * @brief Network Layer (Layer 3) -- logical addressing, packetization,
 *        and packet fragmentation / reassembly.
 */

#ifndef NETWORK_LAYER_H
#define NETWORK_LAYER_H

#include <string>
#include <vector>
#include "packet.h"

class NetworkLayer {
public:
    /**
     * @brief Encapsulate a payload string into a single Layer 3 Packet.
     * (Used when no fragmentation is needed.)
     */
    Packet encapsulate(const std::string& payload,
                       const std::string& srcIP,
                       const std::string& dstIP);

    /**
     * @brief Decapsulate a received Packet and extract the payload.
     */
    std::string decapsulate(const Packet& pkt);

    // -- Fragmentation / Reassembly --

    /**
     * @brief Fragment a payload into multiple Packets of at most `mtu` bytes each.
     *
     * @param payload     The full message text.
     * @param srcIP       Source IP address.
     * @param dstIP       Destination IP address.
     * @param fragmentID  A unique ID for this message (all fragments share it).
     * @param mtu         Maximum payload bytes per fragment (default 20).
     * @return A vector of Packets, each carrying at most `mtu` bytes.
     */
    std::vector<Packet> fragment(const std::string& payload,
                                 const std::string& srcIP,
                                 const std::string& dstIP,
                                 int fragmentID,
                                 int mtu = 20);

    /**
     * @brief Reassemble a complete set of fragments back into the original payload.
     *
     * Fragments must be sorted by fragmentOffset before calling this.
     *
     * @param fragments  A vector of Packets belonging to the same fragmentID.
     * @return The reconstructed payload string.
     */
    std::string reassemble(const std::vector<Packet>& fragments);
};

#endif // NETWORK_LAYER_H
