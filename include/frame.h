/**
 * @file frame.h
 * @brief Layer 2 (Data Link) data structure — the Frame.
 *
 * A Frame wraps a serialized Layer 3 Packet with physical (MAC)
 * addressing, a CRC-32 checksum for error detection, a frame type
 * indicator (DATA / ACK / NACK), and a sequence number used by the
 * Stop-and-Wait ARQ protocol.
 */

#ifndef FRAME_H
#define FRAME_H

#include <cstdint>
#include <string>
#include <sstream>
#include <stdexcept>

/**
 * @enum FrameType
 * @brief Distinguishes between data frames and control frames.
 */
enum class FrameType : int {
    DATA = 0,   ///< Carries actual payload data
    ACK  = 1,   ///< Positive acknowledgement — frame received correctly
    NACK = 2    ///< Negative acknowledgement — frame corrupted, retransmit
};

/**
 * @brief Convert a FrameType to a human-readable string.
 */
inline std::string frameTypeToString(FrameType ft) {
    switch (ft) {
        case FrameType::DATA: return "DATA";
        case FrameType::ACK:  return "ACK";
        case FrameType::NACK: return "NACK";
    }
    return "UNKNOWN";
}

/**
 * @struct Frame
 * @brief Represents a Data Link Layer (Layer 2) Protocol Data Unit.
 *
 * Serialization uses '#' as the field delimiter.  The CRC field is
 * stored as a decimal integer in the serialized form.
 */
struct Frame {
    std::string srcMAC;             ///< Source MAC address      (e.g. "AA:BB:CC:DD:EE:01")
    std::string dstMAC;             ///< Destination MAC address (e.g. "AA:BB:CC:DD:EE:02")
    FrameType   type   = FrameType::DATA;  ///< Frame type
    int         seqNum = 0;         ///< Sequence number for Stop-and-Wait ARQ
    std::string encapsulatedData;   ///< Serialized Layer 3 Packet (or empty for ACK/NACK)
    uint32_t    crc    = 0;         ///< CRC-32 checksum computed over the payload fields

    /**
     * @brief Serialize the Frame into a delimited string.
     * @return A string of the form "srcMAC#dstMAC#type#seqNum#data#crc".
     */
    std::string serialize() const {
        std::ostringstream oss;
        oss << srcMAC << "#"
            << dstMAC << "#"
            << static_cast<int>(type) << "#"
            << seqNum << "#"
            << encapsulatedData << "#"
            << crc;
        return oss.str();
    }

    /**
     * @brief Reconstruct a Frame from a serialized string.
     * @param data The serialized string produced by serialize().
     * @return A fully populated Frame struct.
     * @throws std::runtime_error if the format is invalid.
     */
    static Frame deserialize(const std::string& data) {
        Frame frame;

        // We need to find exactly 5 '#' delimiters.
        size_t pos1 = data.find('#');
        size_t pos2 = data.find('#', pos1 + 1);
        size_t pos3 = data.find('#', pos2 + 1);
        size_t pos4 = data.find('#', pos3 + 1);
        // The 5th delimiter separates encapsulatedData from CRC.
        // encapsulatedData may itself contain '#' — but we serialized CRC
        // as the LAST field, so we search from the end.
        size_t pos5 = data.rfind('#');

        if (pos1 == std::string::npos || pos2 == std::string::npos ||
            pos3 == std::string::npos || pos4 == std::string::npos ||
            pos5 == std::string::npos || pos5 == pos4) {
            throw std::runtime_error("Frame::deserialize — invalid format");
        }

        frame.srcMAC = data.substr(0, pos1);
        frame.dstMAC = data.substr(pos1 + 1, pos2 - pos1 - 1);
        frame.type   = static_cast<FrameType>(std::stoi(
                            data.substr(pos2 + 1, pos3 - pos2 - 1)));
        frame.seqNum = std::stoi(data.substr(pos3 + 1, pos4 - pos3 - 1));
        frame.encapsulatedData = data.substr(pos4 + 1, pos5 - pos4 - 1);
        frame.crc    = static_cast<uint32_t>(std::stoul(
                            data.substr(pos5 + 1)));

        return frame;
    }
};

#endif // FRAME_H
