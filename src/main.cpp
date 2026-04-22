/**
 * @file main.cpp
 * @brief Orchestrator — simulates two nodes exchanging messages over a noisy medium.
 *
 * This is the entry point of the OSI Protocol Stack Simulation.
 * It creates Node A and Node B, then demonstrates:
 *  1. Node A sending a message to Node B (with Stop-and-Wait ARQ).
 *  2. Node B sending a reply back to Node A.
 *
 * The noise probability is set to 20% per-bit to ensure that
 * corrupted frames, NACKs, and retransmissions are demonstrated.
 */

#include <iostream>
#include <string>
#include "colors.h"
#include "node.h"
#include "medium.h"

#ifdef _WIN32
#include <windows.h>
#endif

// ═══════════════════════════════════════════════════════════════════════════
//  Helper: Print a styled banner
// ═══════════════════════════════════════════════════════════════════════════

static void printBanner() {
    std::cout << "\n";
    std::cout << Color::BOLD_CYAN;
    std::cout << "  +============================================================+\n";
    std::cout << "  |                                                            |\n";
    std::cout << "  |     OSI  PROTOCOL  STACK  SIMULATION   (C++17)             |\n";
    std::cout << "  |     Network  |  Data Link  |  Physical  Layers             |\n";
    std::cout << "  |                                                            |\n";
    std::cout << "  +============================================================+\n";
    std::cout << Color::RESET << "\n";
}

// ═══════════════════════════════════════════════════════════════════════════
//  Helper: Send a message from sender to receiver with Stop-and-Wait ARQ
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief Execute a full Stop-and-Wait ARQ transmission from sender to receiver.
 *
 * @param sender   The sending Node.
 * @param receiver The receiving Node.
 * @param message  The text message to deliver.
 * @param medium   The noisy transmission Medium.
 * @param maxRetries Maximum number of retransmission attempts.
 * @return true if the message was delivered, false if all retries exhausted.
 */
static bool sendWithARQ(Node& sender, Node& receiver,
                        const std::string& message, Medium& medium,
                        int maxRetries = 5) {
    int seqNum = 0;

    printThickSeparator();
    logInfo(sender.getName() + " -> " + receiver.getName() +
            ":  \"" + message + "\"");
    printThickSeparator();

    for (int attempt = 0; attempt <= maxRetries; ++attempt) {
        if (attempt > 0) {
            std::cout << "\n";
            printThickSeparator();
            logError(sender.getName() + " is RETRANSMITTING (attempt " +
                     std::to_string(attempt) + "/" +
                     std::to_string(maxRetries) + ")...");
            printThickSeparator();
        }

        // ── SENDER: walk message down L3 -> L2 -> L1 ────────────────────
        std::cout << "\n";
        std::string bitStream = sender.prepareSend(
            message, receiver.getIP(), receiver.getMAC(), seqNum);

        // ── MEDIUM: transmit (with possible noise) ───────────────────────
        std::cout << "\n";
        std::string received = medium.transmit(bitStream);
        std::cout << "\n";

        // ── RECEIVER: walk bits up L1 -> L2 -> L3 ───────────────────────
        bool crcOK = false;
        int recvSeqNum = -1;
        std::string payload = receiver.receiveData(received, crcOK, recvSeqNum);

        if (crcOK) {
            // ── Send ACK back (perfect channel for control frames) ───────
            logSuccess(receiver.getName() + " received message: \"" +
                       payload + "\"");
            std::cout << "\n";

            logDataLink(receiver.getName() +
                        " sending ACK back to " + sender.getName() + "...");
            std::string ackBits = receiver.createACKBitStream(
                sender.getMAC(), recvSeqNum);
            // ACK travels back on a clean channel (no noise on control)
            FrameType ft = sender.receiveControl(ackBits);
            (void)ft;  // We already logged inside receiveControl
            printSeparator();
            logSuccess("Transmission SUCCESSFUL after " +
                       std::to_string(attempt + 1) + " attempt(s).");
            return true;
        } else {
            // ── Send NACK back ───────────────────────────────────────────
            std::cout << "\n";
            logDataLink(receiver.getName() +
                        " sending NACK back to " + sender.getName() + "...");
            std::string nackBits = receiver.createNACKBitStream(
                sender.getMAC(), recvSeqNum);
            sender.receiveControl(nackBits);
        }
    }

    logError("All " + std::to_string(maxRetries) +
             " retransmission attempts FAILED. Giving up.");
    return false;
}

// ═══════════════════════════════════════════════════════════════════════════
//  main
// ═══════════════════════════════════════════════════════════════════════════

int main() {
#ifdef _WIN32
    // Enable ANSI escape code processing on Windows 10+
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif

    // -- Print banner ---------------------------------------------------------
    printBanner();

    // ── Create two network nodes ─────────────────────────────────────────
    Node nodeA("Node A", "192.168.1.1", "AA:BB:CC:DD:EE:01");
    Node nodeB("Node B", "192.168.1.2", "AA:BB:CC:DD:EE:02");

    logInfo("Created " + nodeA.getName() +
            "  [IP: " + nodeA.getIP() +
            "  MAC: " + nodeA.getMAC() + "]");
    logInfo("Created " + nodeB.getName() +
            "  [IP: " + nodeB.getIP() +
            "  MAC: " + nodeB.getMAC() + "]");
    std::cout << "\n";

    // ── Create the noisy transmission medium (20% bit-flip chance) ───────
    Medium medium(0.002);  // 0.2% per bit — balanced for ~1000-bit frames

    // ══════════════════════════════════════════════════════════════════════
    //  Transmission 1:  Node A  ─────>  Node B
    // ══════════════════════════════════════════════════════════════════════
    std::string msgAtoB = "Hello from Node A! This is a test message.";
    bool ok1 = sendWithARQ(nodeA, nodeB, msgAtoB, medium);
    std::cout << "\n\n";

    // ══════════════════════════════════════════════════════════════════════
    //  Transmission 2:  Node B  ─────>  Node A
    // ══════════════════════════════════════════════════════════════════════
    std::string msgBtoA = "Acknowledged! Reply from Node B.";
    bool ok2 = sendWithARQ(nodeB, nodeA, msgBtoA, medium);
    std::cout << "\n\n";

    // ── Final summary ────────────────────────────────────────────────────
    printThickSeparator();
    std::cout << Color::BOLD_CYAN
              << "  SIMULATION COMPLETE\n" << Color::RESET;
    std::cout << Color::CYAN
              << "  A -> B: " << (ok1 ? "DELIVERED" : "FAILED") << "\n"
              << "  B -> A: " << (ok2 ? "DELIVERED" : "FAILED") << "\n"
              << Color::RESET;
    printThickSeparator();

    return 0;
}
