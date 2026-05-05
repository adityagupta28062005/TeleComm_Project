/**
 * @file node.h
 * @brief A network Node that encapsulates the three OSI layers.
 */

#ifndef NODE_H
#define NODE_H

#include <string>
#include "network_layer.h"
#include "datalink_layer.h"
#include "physical_layer.h"

class Node {
public:
    Node(const std::string& name,
         const std::string& ip,
         const std::string& mac);

    // -- Sending (top -> bottom of the stack) --

    /// Walk a message down L3 -> L2 -> L1 (single packet, no fragmentation)
    std::string prepareSend(const std::string& message,
                            const std::string& destIP,
                            const std::string& destMAC,
                            int seqNum);

    /// Encapsulate a pre-built Packet at L2 -> L1 (used by Go-Back-N with fragments)
    std::string prepareSendPacket(const Packet& pkt,
                                  const std::string& destMAC,
                                  int seqNum);

    // -- Receiving (bottom -> top of the stack) --

    /// Walk a received bit stream up L1 -> L2 -> L3
    std::string receiveData(const std::string& bitStream,
                            bool& crcOK,
                            int& receivedSeqNum);

    /// Receive and extract the raw Packet (for fragmentation reassembly)
    Packet receivePacket(const std::string& bitStream,
                         bool& crcOK,
                         int& receivedSeqNum);

    /// Create an ACK bit stream
    std::string createACKBitStream(const std::string& destMAC, int seqNum);

    /// Create a NACK bit stream
    std::string createNACKBitStream(const std::string& destMAC, int seqNum);

    /// Decode an ACK/NACK bit stream and return the frame type + seq num
    FrameType receiveControl(const std::string& bitStream, int& ackSeqNum);

    /// Backward compat overload
    FrameType receiveControl(const std::string& bitStream);

    // -- Accessors --
    const std::string& getName() const { return name_; }
    const std::string& getIP()   const { return ip_;   }
    const std::string& getMAC()  const { return mac_;  }

    /// Public access to network layer for fragmentation
    NetworkLayer& getNetworkLayer() { return networkLayer_; }

private:
    std::string   name_;
    std::string   ip_;
    std::string   mac_;

    NetworkLayer  networkLayer_;
    DataLinkLayer dataLinkLayer_;
    PhysicalLayer physicalLayer_;
};

#endif // NODE_H
