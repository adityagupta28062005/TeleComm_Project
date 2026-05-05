/**
 * @file main.cpp
 * @brief Orchestrator -- Interactive OSI Protocol Stack Simulation.
 *
 * Features:
 *  - Go-Back-N Sliding Window Protocol (replaces Stop-and-Wait)
 *  - Packet Fragmentation & Reassembly (Layer 3)
 *  - Transmission Statistics Dashboard
 *  - Interactive menu-driven mode
 *  - ASCII Sequence Diagram
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <limits>
#include "colors.h"
#include "node.h"
#include "medium.h"
#include "statistics.h"
#include "sequence_diagram.h"

#ifdef _WIN32
#include <windows.h>
#endif

// -- Global configurable parameters --
static int    g_windowSize = 4;
static int    g_mtu        = 20;
static int    g_maxRounds  = 15;
static int    g_fragmentIDCounter = 0;

// =====================================================================
//  Banner
// =====================================================================

static void printBanner() {
    std::cout << "\n";
    std::cout << Color::BOLD_CYAN;
    std::cout << "  +============================================================+\n";
    std::cout << "  |                                                            |\n";
    std::cout << "  |     OSI  PROTOCOL  STACK  SIMULATION   (C++17)             |\n";
    std::cout << "  |     Network  |  Data Link  |  Physical  Layers             |\n";
    std::cout << "  |                                                            |\n";
    std::cout << "  |  Features: Go-Back-N | Fragmentation | Statistics          |\n";
    std::cout << "  |                                                            |\n";
    std::cout << "  +============================================================+\n";
    std::cout << Color::RESET << "\n";
}

// =====================================================================
//  Go-Back-N Sliding Window Transmission
// =====================================================================

/**
 * @brief Send a message using Go-Back-N sliding window protocol
 *        with packet fragmentation.
 *
 * 1. Fragment the message at L3 (MTU-sized chunks).
 * 2. Use Go-Back-N to send all fragments reliably.
 * 3. Receiver reassembles fragments after all are received.
 *
 * @return TransmissionStats for this transmission.
 */
static TransmissionStats sendWithGoBackN(
        Node& sender, Node& receiver,
        const std::string& message, Medium& medium) {

    TransmissionStats stats;
    stats.sender   = sender.getName();
    stats.receiver = receiver.getName();
    stats.windowSize  = g_windowSize;
    stats.mtu         = g_mtu;
    stats.payloadBytes = static_cast<int>(message.size());

    SequenceDiagram diagram;

    printThickSeparator();
    logInfo(sender.getName() + " -> " + receiver.getName() +
            ":  \"" + message + "\"");
    printThickSeparator();

    // -- Step 1: Fragment at Layer 3 --
    int fragID = ++g_fragmentIDCounter;
    std::vector<Packet> fragments = sender.getNetworkLayer().fragment(
        message, sender.getIP(), receiver.getIP(), fragID, g_mtu);

    int N = static_cast<int>(fragments.size());
    stats.fragmentCount = N;
    logInfo("Total fragments: " + std::to_string(N) +
            ", Window size: " + std::to_string(g_windowSize));
    printSeparator();

    // -- Step 2: Go-Back-N sliding window --
    int base = 0;           // First unacknowledged frame
    int W = g_windowSize;

    // Storage for correctly received fragments at receiver side
    std::vector<Packet> receivedFragments(N);
    std::vector<bool>   receivedOK(N, false);
    int expectedSeq = 0;    // Receiver's expected next sequence number

    int round = 0;

    while (base < N && round < g_maxRounds) {
        ++round;
        stats.roundsUsed = round;

        std::cout << "\n";
        logInfo("=== Go-Back-N Round " + std::to_string(round) +
                " | base=" + std::to_string(base) +
                " | window=[" + std::to_string(base) + ".." +
                std::to_string(std::min(base + W - 1, N - 1)) + "] ===");
        printSeparator();

        // Send all frames in the current window
        int windowEnd = std::min(base + W, N);
        bool anyCorrupted = false;

        for (int i = base; i < windowEnd; ++i) {
            std::cout << "\n";

            // Sender: L3 packet -> L2 frame -> L1 bits
            std::string bitStream = sender.prepareSendPacket(
                fragments[i], receiver.getMAC(), i);

            diagram.addEvent(sender.getName(), receiver.getName(),
                             SequenceDiagram::Event::SEND, i);

            // Medium: transmit with noise
            int flipped = 0;
            std::string received = medium.transmit(bitStream, flipped);
            stats.totalFramesSent++;
            stats.totalBitsTransmitted += static_cast<int>(bitStream.size());
            stats.totalBitsFlipped += flipped;

            if (flipped > 0) {
                diagram.addEvent(sender.getName(), receiver.getName(),
                                 SequenceDiagram::Event::NOISE, i,
                                 std::to_string(flipped));
            }

            // Receiver: L1 -> L2 -> L3
            bool crcOK = false;
            int recvSeqNum = -1;
            Packet recvPkt = receiver.receivePacket(received, crcOK, recvSeqNum);

            if (crcOK && recvSeqNum == expectedSeq) {
                // In-order delivery
                receivedFragments[recvSeqNum] = recvPkt;
                receivedOK[recvSeqNum] = true;
                expectedSeq++;
                stats.framesDelivered++;

                diagram.addEvent(receiver.getName(), sender.getName(),
                                 SequenceDiagram::Event::CRC_OK, recvSeqNum);

                logSuccess("Frame seq=" + std::to_string(recvSeqNum) +
                           " received correctly (in-order).");

                // Send cumulative ACK for expectedSeq - 1
                std::string ackBits = receiver.createACKBitStream(
                    sender.getMAC(), expectedSeq - 1);

                diagram.addEvent(receiver.getName(), sender.getName(),
                                 SequenceDiagram::Event::ACK, expectedSeq - 1);

                int ackSeq = -1;
                sender.receiveControl(ackBits, ackSeq);

                // Slide window: base moves to ackSeq + 1
                if (ackSeq + 1 > base) {
                    base = ackSeq + 1;
                }

            } else if (crcOK && recvSeqNum != expectedSeq) {
                // Out of order -- discard, send ACK for last good
                logError("Frame seq=" + std::to_string(recvSeqNum) +
                         " out-of-order (expected " +
                         std::to_string(expectedSeq) + "). Discarding.");

                diagram.addEvent(receiver.getName(), sender.getName(),
                                 SequenceDiagram::Event::CRC_OK, recvSeqNum);

                int ackFor = expectedSeq - 1;
                if (ackFor >= 0) {
                    std::string ackBits = receiver.createACKBitStream(
                        sender.getMAC(), ackFor);
                    diagram.addEvent(receiver.getName(), sender.getName(),
                                     SequenceDiagram::Event::ACK, ackFor);
                    int ackSeq = -1;
                    sender.receiveControl(ackBits, ackSeq);
                }
                anyCorrupted = true;
            } else {
                // CRC failed
                stats.framesCorrupted++;
                anyCorrupted = true;

                diagram.addEvent(receiver.getName(), sender.getName(),
                                 SequenceDiagram::Event::CRC_FAIL, i);

                logError("Frame seq=" + std::to_string(i) +
                         " CORRUPTED. Receiver discards.");

                // Send ACK for last good
                int ackFor = expectedSeq - 1;
                if (ackFor >= 0) {
                    std::string ackBits = receiver.createACKBitStream(
                        sender.getMAC(), ackFor);
                    diagram.addEvent(receiver.getName(), sender.getName(),
                                     SequenceDiagram::Event::ACK, ackFor);
                    int ackSeq = -1;
                    sender.receiveControl(ackBits, ackSeq);
                }
            }
        }

        // Check if we need to go back
        if (base < N && anyCorrupted) {
            stats.totalRetransmissions += (std::min(base + W, N) - base);
            logError("Go-Back-N: going back to seq=" + std::to_string(base) +
                     " for retransmission.");
            diagram.addEvent(sender.getName(), receiver.getName(),
                             SequenceDiagram::Event::RETRANSMIT, base);
        }
    }

    // -- Step 3: Check if all delivered --
    bool allOK = (base >= N);
    stats.delivered = allOK;

    if (allOK) {
        // Reassemble at receiver
        std::string reassembled = receiver.getNetworkLayer().reassemble(receivedFragments);
        diagram.addEvent(sender.getName(), receiver.getName(),
                         SequenceDiagram::Event::DELIVER, -1);
        std::cout << "\n";
        logSuccess(receiver.getName() + " reassembled message: \"" +
                   reassembled + "\"");
    } else {
        std::cout << "\n";
        logError("Transmission FAILED after " + std::to_string(g_maxRounds) +
                 " rounds. Not all fragments delivered.");
    }

    // Print sequence diagram
    diagram.print(sender.getName(), receiver.getName());

    // Print statistics
    stats.print();

    return stats;
}

// =====================================================================
//  Interactive Menu
// =====================================================================

static void printMenu(double noisePct) {
    // Each menu line content (between the leading "  |  " and the trailing " |")
    // must be exactly 54 characters wide so the right border | always aligns.
    // Total line: "  |  " (5) + 54 chars + " |" (2) = 61 chars + newline.
    const int INNER = 54;

    auto menuLine = [&](const std::string& content) {
        // Pad or truncate content to INNER characters
        std::string s = content;
        if ((int)s.size() < INNER)
            s += std::string(INNER - s.size(), ' ');
        std::cout << Color::CYAN << "  |  " << s << " |\n" << Color::RESET;
    };

    // Build dynamic strings first so padding is exact
    std::ostringstream noiseStr, winStr, mtuStr;
    noiseStr << "[3] Change noise level      (current: "
             << std::fixed << std::setprecision(1) << noisePct << "%)";
    winStr   << "[4] Change window size      (current: "
             << g_windowSize << ")";
    mtuStr   << "[5] Change MTU              (current: "
             << g_mtu << " bytes)";

    std::cout << Color::BOLD_WHITE;
    std::cout << "  +----------------------------------------------------------+\n";
    std::cout << "  |                    INTERACTIVE  MODE                     |\n";
    std::cout << "  +----------------------------------------------------------+\n";
    std::cout << Color::RESET;
    menuLine("[1] Send message  (Node A -> Node B)");
    menuLine("[2] Send message  (Node B -> Node A)");
    menuLine(noiseStr.str());
    menuLine(winStr.str());
    menuLine(mtuStr.str());
    menuLine("[6] Run demo (automatic)");
    menuLine("[7] View session statistics");
    menuLine("[8] Exit");
    std::cout << Color::BOLD_WHITE;
    std::cout << "  +----------------------------------------------------------+\n";
    std::cout << Color::RESET;
}

static std::string getInput(const std::string& prompt) {
    std::cout << Color::BOLD_WHITE << "  " << prompt << Color::RESET;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

static int getIntInput(const std::string& prompt, int minVal, int maxVal) {
    while (true) {
        std::string s = getInput(prompt);
        try {
            int val = std::stoi(s);
            if (val >= minVal && val <= maxVal) return val;
            std::cout << Color::RED << "  Please enter a value between "
                      << minVal << " and " << maxVal << ".\n" << Color::RESET;
        } catch (...) {
            std::cout << Color::RED << "  Invalid number. Try again.\n"
                      << Color::RESET;
        }
    }
}

static double getDoubleInput(const std::string& prompt, double minVal, double maxVal) {
    while (true) {
        std::string s = getInput(prompt);
        try {
            double val = std::stod(s);
            if (val >= minVal && val <= maxVal) return val;
            std::cout << Color::RED << "  Please enter a value between "
                      << minVal << " and " << maxVal << ".\n" << Color::RESET;
        } catch (...) {
            std::cout << Color::RED << "  Invalid number. Try again.\n"
                      << Color::RESET;
        }
    }
}

// =====================================================================
//  main
// =====================================================================

int main() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif

    printBanner();

    // Create nodes
    Node nodeA("Node A", "192.168.1.1", "AA:BB:CC:DD:EE:01");
    Node nodeB("Node B", "192.168.1.2", "AA:BB:CC:DD:EE:02");

    logInfo("Created " + nodeA.getName() +
            "  [IP: " + nodeA.getIP() + "  MAC: " + nodeA.getMAC() + "]");
    logInfo("Created " + nodeB.getName() +
            "  [IP: " + nodeB.getIP() + "  MAC: " + nodeB.getMAC() + "]");
    std::cout << "\n";

    // Create medium
    Medium medium(0.0015);  // 0.15% per bit default

    // Session statistics
    SessionStats session;

    // Interactive loop
    bool running = true;
    while (running) {
        double noisePct = medium.getNoiseProbability() * 100.0;
        printMenu(noisePct);

        std::string choice = getInput("Choice> ");
        std::cout << "\n";

        if (choice == "1" || choice == "2") {
            // Send message
            Node& sender   = (choice == "1") ? nodeA : nodeB;
            Node& receiver = (choice == "1") ? nodeB : nodeA;

            std::string msg = getInput("Enter message: ");
            if (msg.empty()) {
                logError("Empty message. Cancelled.");
                continue;
            }

            TransmissionStats ts = sendWithGoBackN(sender, receiver, msg, medium);
            session.accumulate(ts);

        } else if (choice == "3") {
            double pct = getDoubleInput("New noise level (0.0 - 50.0 %): ", 0.0, 50.0);
            medium.setNoiseProbability(pct / 100.0);
            logInfo("Noise probability set to " +
                    std::to_string(static_cast<int>(pct * 10) / 10.0).substr(0, 4) + "%");

        } else if (choice == "4") {
            g_windowSize = getIntInput("New window size (1 - 16): ", 1, 16);
            logInfo("Window size set to " + std::to_string(g_windowSize));

        } else if (choice == "5") {
            g_mtu = getIntInput("New MTU in bytes (5 - 100): ", 5, 100);
            logInfo("MTU set to " + std::to_string(g_mtu) + " bytes");

        } else if (choice == "6") {
            // Auto demo
            logInfo("Running automatic demo...");
            std::cout << "\n";

            std::string msg1 = "Hello from Node A! This is a test message.";
            TransmissionStats ts1 = sendWithGoBackN(nodeA, nodeB, msg1, medium);
            session.accumulate(ts1);

            std::cout << "\n\n";

            std::string msg2 = "Acknowledged! Reply from Node B.";
            TransmissionStats ts2 = sendWithGoBackN(nodeB, nodeA, msg2, medium);
            session.accumulate(ts2);

            std::cout << "\n";
            printThickSeparator();
            std::cout << Color::BOLD_CYAN
                      << "  DEMO COMPLETE\n" << Color::RESET;
            std::cout << Color::CYAN
                      << "  A -> B: " << (ts1.delivered ? "DELIVERED" : "FAILED") << "\n"
                      << "  B -> A: " << (ts2.delivered ? "DELIVERED" : "FAILED") << "\n"
                      << Color::RESET;
            printThickSeparator();

        } else if (choice == "7") {
            session.print();

        } else if (choice == "8" || choice == "q" || choice == "Q") {
            // Exit
            if (session.totalTransmissions > 0) {
                session.print();
            }
            logInfo("Exiting simulation. Goodbye!");
            running = false;

        } else {
            logError("Invalid choice. Please enter 1-8.");
        }

        std::cout << "\n";
    }

    return 0;
}
