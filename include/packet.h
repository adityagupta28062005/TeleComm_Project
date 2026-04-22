/**
 * @file packet.h
 * @brief Layer 3 (Network) data structure — the Packet.
 *
 * A Packet carries the original text payload along with logical
 * addressing information (source/destination IP) and a TTL field.
 * It provides serialization/deserialization so that it can be
 * encapsulated inside a Layer 2 Frame.
 */

#ifndef PACKET_H
#define PACKET_H

#include <string>
#include <sstream>
#include <stdexcept>

/**
 * @struct Packet
 * @brief Represents a Network Layer (Layer 3) Protocol Data Unit.
 *
 * Fields are separated by a pipe '|' character during serialization
 * to allow easy parsing at the receiving end.
 */
struct Packet {
    std::string srcIP;    ///< Source IP address      (e.g. "192.168.1.1")
    std::string dstIP;    ///< Destination IP address (e.g. "192.168.1.2")
    int         ttl;      ///< Time-To-Live hop counter
    std::string payload;  ///< The actual message data

    /**
     * @brief Serialize the Packet into a delimited string.
     * @return A string of the form "srcIP|dstIP|ttl|payload".
     *
     * This string can be embedded inside a Frame's data field.
     */
    std::string serialize() const {
        std::ostringstream oss;
        oss << srcIP << "|" << dstIP << "|" << ttl << "|" << payload;
        return oss.str();
    }

    /**
     * @brief Reconstruct a Packet from a serialized string.
     * @param data The serialized string produced by serialize().
     * @return A fully populated Packet struct.
     * @throws std::runtime_error if the format is invalid.
     */
    static Packet deserialize(const std::string& data) {
        Packet pkt;

        // Find delimiter positions (first three '|' characters)
        size_t pos1 = data.find('|');
        size_t pos2 = data.find('|', pos1 + 1);
        size_t pos3 = data.find('|', pos2 + 1);

        if (pos1 == std::string::npos ||
            pos2 == std::string::npos ||
            pos3 == std::string::npos) {
            throw std::runtime_error("Packet::deserialize — invalid format");
        }

        pkt.srcIP   = data.substr(0, pos1);
        pkt.dstIP   = data.substr(pos1 + 1, pos2 - pos1 - 1);
        pkt.ttl     = std::stoi(data.substr(pos2 + 1, pos3 - pos2 - 1));
        pkt.payload = data.substr(pos3 + 1);

        return pkt;
    }
};

#endif // PACKET_H
