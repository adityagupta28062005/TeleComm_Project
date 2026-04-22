/**
 * @file network_layer.h
 * @brief Network Layer (Layer 3) — logical addressing and packetization.
 *
 * The NetworkLayer class is responsible for encapsulating a text
 * payload into a Packet (adding IP headers and TTL) and for
 * decapsulating a received Packet back into the original payload.
 */

#ifndef NETWORK_LAYER_H
#define NETWORK_LAYER_H

#include <string>
#include "packet.h"

class NetworkLayer {
public:
    /**
     * @brief Encapsulate a payload string into a Layer 3 Packet.
     *
     * Adds source IP, destination IP, and sets TTL to 64.
     *
     * @param payload The message text to send.
     * @param srcIP   Source IP address string.
     * @param dstIP   Destination IP address string.
     * @return A fully formed Packet ready for Layer 2 framing.
     */
    Packet encapsulate(const std::string& payload,
                       const std::string& srcIP,
                       const std::string& dstIP);

    /**
     * @brief Decapsulate a received Packet and extract the payload.
     *
     * Performs a TTL validity check and logs the addressing info.
     *
     * @param pkt The Packet received from Layer 2.
     * @return The original payload string.
     */
    std::string decapsulate(const Packet& pkt);
};

#endif // NETWORK_LAYER_H
