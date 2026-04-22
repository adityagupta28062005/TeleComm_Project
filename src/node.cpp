/**
 * @file node.cpp
 * @brief Implementation of the Node class.
 *
 * A Node orchestrates the three OSI layers internally.
 */

#include "node.h"
#include "colors.h"
#include <sstream>
#include <stdexcept>

// Constructor
Node::Node(const std::string& name, const std::string& ip, const std::string& mac)
    : name_(name), ip_(ip), mac_(mac) {}

// prepareSend — walk the message DOWN the stack (L3 -> L2 -> L1)
std::string Node::prepareSend(const std::string& message,
                              const std::string& destIP,
                              const std::string& destMAC,
                              int seqNum) {
    logInfo(name_ + " is sending data DOWN the protocol stack...");
    printSeparator();

    // Layer 3 — Network
    Packet pkt = networkLayer_.encapsulate(message, ip_, destIP);
    std::string serializedPacket = pkt.serialize();
    printSeparator();

    // Layer 2 — Data Link
    Frame frame = dataLinkLayer_.encapsulate(serializedPacket, mac_, destMAC, seqNum);
    std::string serializedFrame = frame.serialize();
    printSeparator();

    // Layer 1 — Physical
    std::string bitStream = physicalLayer_.encode(serializedFrame);
    printSeparator();

    return bitStream;
}

// receiveData — walk a bit stream UP the stack (L1 -> L2 -> L3)
std::string Node::receiveData(const std::string& bitStream,
                              bool& crcOK, int& receivedSeqNum) {
    logInfo(name_ + " is receiving data UP the protocol stack...");
    printSeparator();

    // Layer 1 — Physical
    std::string decodedBytes = physicalLayer_.decode(bitStream);
    printSeparator();

    // Layer 2 — Data Link
    // Noise may corrupt delimiter characters, making deserialization fail.
    // We treat any parse failure the same as a CRC mismatch (corrupted frame).
    Frame frame;
    try {
        frame = Frame::deserialize(decodedBytes);
    } catch (const std::exception& e) {
        logError("Frame deserialization failed (corrupted delimiters): "
                 + std::string(e.what()));
        crcOK = false;
        receivedSeqNum = 0;
        printSeparator();
        return "";
    }

    receivedSeqNum = frame.seqNum;
    dataLinkLayer_.decapsulate(frame);
    crcOK = dataLinkLayer_.verifyCRC(frame);
    printSeparator();

    if (!crcOK) return "";

    // Layer 3 — Network
    // Noise could also corrupt the inner packet delimiters.
    try {
        Packet pkt = Packet::deserialize(frame.encapsulatedData);
        std::string payload = networkLayer_.decapsulate(pkt);
        printSeparator();
        return payload;
    } catch (const std::exception& e) {
        logError("Packet deserialization failed: " + std::string(e.what()));
        crcOK = false;
        printSeparator();
        return "";
    }
}

// createACKBitStream
std::string Node::createACKBitStream(const std::string& destMAC, int seqNum) {
    Frame ack = dataLinkLayer_.createACK(mac_, destMAC, seqNum);
    return physicalLayer_.encode(ack.serialize());
}

// createNACKBitStream
std::string Node::createNACKBitStream(const std::string& destMAC, int seqNum) {
    Frame nack = dataLinkLayer_.createNACK(mac_, destMAC, seqNum);
    return physicalLayer_.encode(nack.serialize());
}

// receiveControl — decode an ACK/NACK bit stream
FrameType Node::receiveControl(const std::string& bitStream) {
    std::string decoded = physicalLayer_.decode(bitStream);
    Frame frame = Frame::deserialize(decoded);

    if (frame.type == FrameType::ACK) {
        logSuccess("Received ACK for Seq #" + std::to_string(frame.seqNum));
    } else if (frame.type == FrameType::NACK) {
        logError("Received NACK for Seq #" + std::to_string(frame.seqNum) +
                 " -- must RETRANSMIT!");
    }
    return frame.type;
}
