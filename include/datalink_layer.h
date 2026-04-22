/**
 * @file datalink_layer.h
 * @brief Data Link Layer (Layer 2) — MAC addressing, CRC-32, and ARQ helpers.
 *
 * The DataLinkLayer class handles:
 *  - Encapsulating a serialized Packet into a Frame (adding MAC headers).
 *  - Computing a CRC-32 checksum for error detection.
 *  - Verifying the CRC on a received Frame.
 *  - Creating ACK / NACK control frames for Stop-and-Wait ARQ.
 */

#ifndef DATALINK_LAYER_H
#define DATALINK_LAYER_H

#include <cstdint>
#include <string>
#include "frame.h"

class DataLinkLayer {
public:
    // ── Encapsulation / Decapsulation ────────────────────────────────────

    /**
     * @brief Wrap a serialized Layer 3 Packet into a Layer 2 Frame.
     *
     * Adds source and destination MAC addresses, sets the sequence
     * number, computes the CRC-32 over the payload fields, and logs
     * the operation.
     *
     * @param serializedPacket The serialized Packet string from Layer 3.
     * @param srcMAC           Source MAC address.
     * @param dstMAC           Destination MAC address.
     * @param seqNum           Sequence number for this frame.
     * @return A fully formed Frame with CRC attached.
     */
    Frame encapsulate(const std::string& serializedPacket,
                      const std::string& srcMAC,
                      const std::string& dstMAC,
                      int seqNum);

    /**
     * @brief Extract the encapsulated data from a Frame.
     *
     * Does NOT verify the CRC — use verifyCRC() separately.
     *
     * @param frame The Frame to decapsulate.
     * @return The serialized Packet string carried by this frame.
     */
    std::string decapsulate(const Frame& frame);

    // ── CRC-32 ──────────────────────────────────────────────────────────

    /**
     * @brief Compute the CRC-32 checksum of an arbitrary data string.
     *
     * Uses the standard CRC-32 polynomial (0xEDB88320, reflected).
     *
     * @param data The input data bytes.
     * @return The 32-bit CRC value.
     */
    uint32_t computeCRC(const std::string& data);

    /**
     * @brief Verify the CRC of a received Frame.
     *
     * Recomputes the CRC from the frame's payload fields and compares
     * it against the CRC stored in the frame.  Logs the result.
     *
     * @param frame The received Frame.
     * @return true if the CRC matches (frame is intact), false otherwise.
     */
    bool verifyCRC(const Frame& frame);

    // ── ARQ Control Frame Helpers ────────────────────────────────────────

    /**
     * @brief Create an ACK frame in response to a correctly received DATA frame.
     *
     * @param srcMAC Sender's (receiver node's) MAC address.
     * @param dstMAC Original sender's MAC address.
     * @param seqNum The sequence number being acknowledged.
     * @return An ACK Frame.
     */
    Frame createACK(const std::string& srcMAC,
                    const std::string& dstMAC,
                    int seqNum);

    /**
     * @brief Create a NACK frame in response to a corrupted DATA frame.
     *
     * @param srcMAC Sender's (receiver node's) MAC address.
     * @param dstMAC Original sender's MAC address.
     * @param seqNum The sequence number being negatively acknowledged.
     * @return A NACK Frame.
     */
    Frame createNACK(const std::string& srcMAC,
                     const std::string& dstMAC,
                     int seqNum);
};

#endif // DATALINK_LAYER_H
