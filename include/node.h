/**
 * @file node.h
 * @brief A network Node that encapsulates the three OSI layers.
 *
 * Each Node owns a NetworkLayer, DataLinkLayer, and PhysicalLayer.
 * It exposes high-level methods to send data down the stack and
 * receive data coming up the stack.
 */

#ifndef NODE_H
#define NODE_H

#include <string>
#include "network_layer.h"
#include "datalink_layer.h"
#include "physical_layer.h"

class Node {
public:
    /**
     * @brief Construct a Node with a name, IP, and MAC address.
     * @param name  Human-readable node identifier (e.g. "Node A").
     * @param ip    IP address string   (e.g. "192.168.1.1").
     * @param mac   MAC address string  (e.g. "AA:BB:CC:DD:EE:01").
     */
    Node(const std::string& name,
         const std::string& ip,
         const std::string& mac);

    // ── Sending (top → bottom of the stack) ──────────────────────────────

    /**
     * @brief Walk a message down L3 → L2 → L1 and return the bit stream.
     *
     * @param message   The text payload to send.
     * @param destIP    Destination node's IP address.
     * @param destMAC   Destination node's MAC address.
     * @param seqNum    Sequence number for Stop-and-Wait ARQ.
     * @return The encoded binary bit stream ready for the Medium.
     */
    std::string prepareSend(const std::string& message,
                            const std::string& destIP,
                            const std::string& destMAC,
                            int seqNum);

    // ── Receiving (bottom → top of the stack) ─────────────────────────────

    /**
     * @brief Walk a received bit stream up L1 → L2 → L3.
     *
     * @param bitStream The binary string received from the Medium.
     * @param[out] crcOK Set to true if the CRC check passed.
     * @param[out] receivedSeqNum The sequence number of the received frame.
     * @return The extracted text payload (valid only if crcOK is true).
     */
    std::string receiveData(const std::string& bitStream,
                            bool& crcOK,
                            int& receivedSeqNum);

    /**
     * @brief Create an ACK bit stream to send back to the sender.
     */
    std::string createACKBitStream(const std::string& destMAC, int seqNum);

    /**
     * @brief Create a NACK bit stream to send back to the sender.
     */
    std::string createNACKBitStream(const std::string& destMAC, int seqNum);

    /**
     * @brief Decode an ACK/NACK bit stream and return the frame type.
     */
    FrameType receiveControl(const std::string& bitStream);

    // ── Accessors ────────────────────────────────────────────────────────
    const std::string& getName() const { return name_; }
    const std::string& getIP()   const { return ip_;   }
    const std::string& getMAC()  const { return mac_;  }

private:
    std::string   name_;
    std::string   ip_;
    std::string   mac_;

    NetworkLayer  networkLayer_;
    DataLinkLayer dataLinkLayer_;
    PhysicalLayer physicalLayer_;
};

#endif // NODE_H
