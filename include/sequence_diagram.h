/**
 * @file sequence_diagram.h
 * @brief ASCII Sequence Diagram renderer (header-only).
 *
 * Records events during a Go-Back-N transmission and prints
 * a visual sequence diagram showing the message flow between
 * sender and receiver.
 */

#ifndef SEQUENCE_DIAGRAM_H
#define SEQUENCE_DIAGRAM_H

#include <iostream>
#include <string>
#include <vector>
#include "colors.h"

class SequenceDiagram {
public:
    enum class Event {
        SEND,       ///< DATA frame sent
        NOISE,      ///< Noise corrupted the frame
        CRC_OK,     ///< CRC check passed at receiver
        CRC_FAIL,   ///< CRC check failed at receiver
        ACK,        ///< ACK sent back
        NACK,       ///< NACK sent back (not used in Go-Back-N, but kept for clarity)
        RETRANSMIT, ///< Retransmission marker
        DELIVER     ///< Message fully delivered
    };

    /**
     * @brief Record an event.
     * @param from   Source node name.
     * @param to     Destination node name.
     * @param type   Event type.
     * @param seqNum Sequence number involved (-1 if N/A).
     * @param detail Optional detail string.
     */
    void addEvent(const std::string& from, const std::string& to,
                  Event type, int seqNum = -1, const std::string& detail = "") {
        entries_.push_back({from, to, type, seqNum, detail});
    }

    void clear() { entries_.clear(); }

    /**
     * @brief Print the sequence diagram to console.
     * @param senderName   Name of the sender node.
     * @param receiverName Name of the receiver node.
     */
    void print(const std::string& senderName,
               const std::string& receiverName) const {

        std::cout << "\n";
        std::cout << Color::BOLD_CYAN
                  << "  +----------------------------------------------------------+\n"
                  << "  |                   SEQUENCE  DIAGRAM                       |\n"
                  << "  +----------------------------------------------------------+\n"
                  << Color::RESET << "\n";

        // Column layout
        const std::string sCol = "  " + senderName;
        const std::string rCol = receiverName;
        const int gap = 40;

        // Header
        std::cout << Color::BOLD_WHITE << "  " << std::string(8, ' ')
                  << senderName << std::string(gap - (int)senderName.size(), ' ')
                  << receiverName << Color::RESET << "\n";
        std::cout << Color::DIM << "  " << std::string(8, ' ')
                  << "|" << std::string(gap - 1, ' ')
                  << "|" << Color::RESET << "\n";

        for (const auto& e : entries_) {
            std::string label;
            std::string color;
            bool leftToRight = true;

            switch (e.type) {
                case Event::SEND:
                    label = "DATA [seq=" + std::to_string(e.seqNum) + "]";
                    if (!e.detail.empty()) label += " " + e.detail;
                    color = Color::GREEN;
                    leftToRight = true;
                    break;
                case Event::NOISE:
                    // Special: centered annotation
                    std::cout << Color::DIM << "  " << std::string(8, ' ')
                              << "|" << Color::RESET;
                    std::cout << Color::BOLD_RED;
                    {
                        std::string noiseLabel = "[!] noise (" + e.detail + " bit flips)";
                        int pad = (gap - 1 - (int)noiseLabel.size()) / 2;
                        if (pad < 0) pad = 1;
                        std::cout << std::string(pad, ' ') << noiseLabel
                                  << std::string(std::max(1, gap - 1 - pad - (int)noiseLabel.size()), ' ');
                    }
                    std::cout << Color::RESET << Color::DIM << "|"
                              << Color::RESET << "\n";
                    continue;
                case Event::CRC_OK:
                    // Right-side annotation
                    std::cout << Color::DIM << "  " << std::string(8, ' ')
                              << "|" << std::string(gap - 1, ' ') << "|"
                              << Color::RESET;
                    std::cout << Color::BOLD_GREEN << "-- CRC: OK [seq="
                              << e.seqNum << "]" << Color::RESET << "\n";
                    continue;
                case Event::CRC_FAIL:
                    std::cout << Color::DIM << "  " << std::string(8, ' ')
                              << "|" << std::string(gap - 1, ' ') << "|"
                              << Color::RESET;
                    std::cout << Color::BOLD_RED << "-- CRC: FAIL [seq="
                              << e.seqNum << "]" << Color::RESET << "\n";
                    continue;
                case Event::ACK:
                    label = "ACK [seq=" + std::to_string(e.seqNum) + "]";
                    color = Color::BOLD_GREEN;
                    leftToRight = false;
                    break;
                case Event::NACK:
                    label = "NACK [seq=" + std::to_string(e.seqNum) + "]";
                    color = Color::BOLD_RED;
                    leftToRight = false;
                    break;
                case Event::RETRANSMIT:
                    // Centered marker
                    std::cout << Color::DIM << "  " << std::string(8, ' ')
                              << "|" << Color::RESET;
                    std::cout << Color::BOLD_YELLOW;
                    {
                        std::string rtLabel = "*** GO-BACK-N: retransmit from seq=" + std::to_string(e.seqNum) + " ***";
                        int pad = (gap - 1 - (int)rtLabel.size()) / 2;
                        if (pad < 0) pad = 0;
                        std::cout << std::string(pad, ' ') << rtLabel
                                  << std::string(std::max(0, gap - 1 - pad - (int)rtLabel.size()), ' ');
                    }
                    std::cout << Color::RESET << Color::DIM << "|"
                              << Color::RESET << "\n";
                    continue;
                case Event::DELIVER:
                    std::cout << Color::DIM << "  " << std::string(8, ' ')
                              << "|" << std::string(gap - 1, ' ') << "|"
                              << Color::RESET << "\n";
                    std::cout << Color::BOLD_GREEN << "  " << std::string(8, ' ')
                              << "[ MESSAGE DELIVERED SUCCESSFULLY ]"
                              << Color::RESET << "\n";
                    continue;
            }

            // Arrow line: left-to-right or right-to-left
            if (leftToRight) {
                int arrowLen = gap - 1 - (int)label.size() - 4;
                if (arrowLen < 2) arrowLen = 2;
                std::cout << Color::DIM << "  " << std::string(8, ' ')
                          << "|" << Color::RESET;
                std::cout << color << "-- " << label << " "
                          << std::string(arrowLen, '-') << ">"
                          << Color::RESET;
                std::cout << Color::DIM << "|" << Color::RESET << "\n";
            } else {
                int arrowLen = gap - 1 - (int)label.size() - 4;
                if (arrowLen < 2) arrowLen = 2;
                std::cout << Color::DIM << "  " << std::string(8, ' ')
                          << "|" << Color::RESET;
                std::cout << color << "<" << std::string(arrowLen, '-')
                          << " " << label << " --"
                          << Color::RESET;
                std::cout << Color::DIM << "|" << Color::RESET << "\n";
            }
        }

        // Footer
        std::cout << Color::DIM << "  " << std::string(8, ' ')
                  << "|" << std::string(gap - 1, ' ')
                  << "|" << Color::RESET << "\n\n";
    }

private:
    struct Entry {
        std::string from;
        std::string to;
        Event type;
        int seqNum;
        std::string detail;
    };
    std::vector<Entry> entries_;
};

#endif // SEQUENCE_DIAGRAM_H
