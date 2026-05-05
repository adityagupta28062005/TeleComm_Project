/**
 * @file node.cpp
 * @brief Implementation of the Node class.
 */

#include "node.h"
#include "colors.h"
#include <sstream>
#include <stdexcept>

Node::Node(const std::string& name, const std::string& ip, const std::string& mac)
    : name_(name), ip_(ip), mac_(mac) {}

// prepareSend -- walk a full message DOWN the stack (L3 -> L2 -> L1)
std::string Node::prepareSend(const std::string& message,
                              const std::string& destIP,
                              const std::string& destMAC,
                              int seqNum) {
    logInfo(name_ + " is sending data DOWN the protocol stack...");
    printSeparator();

    Packet pkt = networkLayer_.encapsulate(message, ip_, destIP);
    std::string serializedPacket = pkt.serialize();
    printSeparator();

    Frame frame = dataLinkLayer_.encapsulate(serializedPacket, mac_, destMAC, seqNum);
    std::string serializedFrame = frame.serialize();
    printSeparator();

    std::string bitStream = physicalLayer_.encode(serializedFrame);
    printSeparator();

    return bitStream;
}

// prepareSendPacket -- encapsulate a pre-built Packet at L2 -> L1
std::string Node::prepareSendPacket(const Packet& pkt,
                                     const std::string& destMAC,
                                     int seqNum) {
    std::ostringstream oss;
    oss << name_ << " sending fragment [" << pkt.fragmentOffset
        << "] as seq=" << seqNum;
    logInfo(oss.str());

    std::string serializedPacket = pkt.serialize();

    Frame frame = dataLinkLayer_.encapsulate(serializedPacket, mac_, destMAC, seqNum);
    std::string serializedFrame = frame.serialize();

    std::string bitStream = physicalLayer_.encode(serializedFrame);

    return bitStream;
}

// receiveData -- walk a bit stream UP the stack (L1 -> L2 -> L3)
std::string Node::receiveData(const std::string& bitStream,
                              bool& crcOK, int& receivedSeqNum) {
    logInfo(name_ + " is receiving data UP the protocol stack...");
    printSeparator();

    std::string decodedBytes = physicalLayer_.decode(bitStream);
    printSeparator();

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

// receivePacket -- walk up L1 -> L2 and return the raw Packet
Packet Node::receivePacket(const std::string& bitStream,
                            bool& crcOK, int& receivedSeqNum) {
    std::string decodedBytes = physicalLayer_.decode(bitStream);

    Frame frame;
    try {
        frame = Frame::deserialize(decodedBytes);
    } catch (const std::exception&) {
        crcOK = false;
        receivedSeqNum = 0;
        return Packet{};
    }

    receivedSeqNum = frame.seqNum;
    dataLinkLayer_.decapsulate(frame);
    crcOK = dataLinkLayer_.verifyCRC(frame);

    if (!crcOK) return Packet{};

    try {
        Packet pkt = Packet::deserialize(frame.encapsulatedData);
        networkLayer_.decapsulate(pkt);
        return pkt;
    } catch (const std::exception&) {
        crcOK = false;
        return Packet{};
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

// receiveControl -- with ackSeqNum out-param
FrameType Node::receiveControl(const std::string& bitStream, int& ackSeqNum) {
    std::string decoded = physicalLayer_.decode(bitStream);
    Frame frame = Frame::deserialize(decoded);
    ackSeqNum = frame.seqNum;

    if (frame.type == FrameType::ACK) {
        logSuccess("Received ACK for Seq #" + std::to_string(frame.seqNum));
    } else if (frame.type == FrameType::NACK) {
        logError("Received NACK for Seq #" + std::to_string(frame.seqNum) +
                 " -- must RETRANSMIT!");
    }
    return frame.type;
}

// Backward-compat overload
FrameType Node::receiveControl(const std::string& bitStream) {
    int dummy = 0;
    return receiveControl(bitStream, dummy);
}
