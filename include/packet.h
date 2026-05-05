/**
 * @file packet.h
 * @brief Layer 3 (Network) data structure -- the Packet.
 *
 * A Packet carries the original text payload along with logical
 * addressing information (source/destination IP), a TTL field,
 * and fragmentation metadata for large message support.
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
 * Fields are separated by a pipe '|' character during serialization.
 * Format: srcIP|dstIP|ttl|fragmentID|fragmentOffset|moreFragments|payload
 */
struct Packet {
    std::string srcIP;          ///< Source IP address
    std::string dstIP;          ///< Destination IP address
    int         ttl = 64;       ///< Time-To-Live hop counter

    // -- Fragmentation fields --
    int  fragmentID     = 0;    ///< Identifies which message this fragment belongs to
    int  fragmentOffset = 0;    ///< Fragment index (0, 1, 2, ...)
    bool moreFragments  = false;///< true if more fragments follow this one

    std::string payload;        ///< The actual message data (or fragment thereof)

    /**
     * @brief Serialize the Packet into a delimited string.
     * @return "srcIP|dstIP|ttl|fragID|fragOff|moreFrag|payload"
     */
    std::string serialize() const {
        std::ostringstream oss;
        oss << srcIP << "|" << dstIP << "|" << ttl << "|"
            << fragmentID << "|" << fragmentOffset << "|"
            << (moreFragments ? 1 : 0) << "|" << payload;
        return oss.str();
    }

    /**
     * @brief Reconstruct a Packet from a serialized string.
     * @throws std::runtime_error if the format is invalid.
     */
    static Packet deserialize(const std::string& data) {
        Packet pkt;

        // Find delimiter positions (first six '|' characters)
        size_t pos1 = data.find('|');
        size_t pos2 = data.find('|', pos1 + 1);
        size_t pos3 = data.find('|', pos2 + 1);
        size_t pos4 = data.find('|', pos3 + 1);
        size_t pos5 = data.find('|', pos4 + 1);
        size_t pos6 = data.find('|', pos5 + 1);

        if (pos1 == std::string::npos || pos2 == std::string::npos ||
            pos3 == std::string::npos || pos4 == std::string::npos ||
            pos5 == std::string::npos || pos6 == std::string::npos) {
            throw std::runtime_error("Packet::deserialize -- invalid format");
        }

        pkt.srcIP          = data.substr(0, pos1);
        pkt.dstIP          = data.substr(pos1 + 1, pos2 - pos1 - 1);
        pkt.ttl            = std::stoi(data.substr(pos2 + 1, pos3 - pos2 - 1));
        pkt.fragmentID     = std::stoi(data.substr(pos3 + 1, pos4 - pos3 - 1));
        pkt.fragmentOffset = std::stoi(data.substr(pos4 + 1, pos5 - pos4 - 1));
        pkt.moreFragments  = (std::stoi(data.substr(pos5 + 1, pos6 - pos5 - 1)) != 0);
        pkt.payload        = data.substr(pos6 + 1);

        return pkt;
    }
};

#endif // PACKET_H
