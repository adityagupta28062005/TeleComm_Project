/**
 * @file statistics.h
 * @brief Transmission Statistics Dashboard (header-only).
 *
 * Collects metrics during a Go-Back-N transmission and prints
 * a formatted ASCII table summarizing performance.
 */

#ifndef STATISTICS_H
#define STATISTICS_H

#include <iostream>
#include <iomanip>
#include <string>
#include "colors.h"

/**
 * @struct TransmissionStats
 * @brief Holds per-transmission and cumulative statistics.
 */
struct TransmissionStats {
    std::string sender;
    std::string receiver;
    int totalFramesSent      = 0;  ///< Total DATA frames sent (including retransmissions)
    int totalRetransmissions = 0;  ///< Number of retransmitted frames
    int totalBitsTransmitted = 0;  ///< Total bit characters sent through medium
    int totalBitsFlipped     = 0;  ///< Total bits corrupted by noise
    int framesCorrupted      = 0;  ///< Frames that failed CRC check
    int framesDelivered      = 0;  ///< Frames successfully delivered
    int fragmentCount        = 0;  ///< Number of fragments in this message
    int windowSize           = 0;  ///< Go-Back-N window size used
    int roundsUsed           = 0;  ///< Number of Go-Back-N rounds
    int mtu                  = 0;  ///< MTU used for fragmentation
    int payloadBytes         = 0;  ///< Original message size in bytes
    bool delivered           = false;

    void reset() {
        totalFramesSent = totalRetransmissions = 0;
        totalBitsTransmitted = totalBitsFlipped = 0;
        framesCorrupted = framesDelivered = 0;
        fragmentCount = windowSize = roundsUsed = mtu = payloadBytes = 0;
        delivered = false;
        sender.clear();
        receiver.clear();
    }

    /**
     * @brief Print a formatted statistics dashboard to the console.
     */
    void print() const {
        std::cout << "\n";
        std::cout << Color::BOLD_CYAN
                  << "  +----------------------------------------------------------+\n"
                  << "  |              TRANSMISSION STATISTICS DASHBOARD            |\n"
                  << "  +----------------------------------------------------------+\n"
                  << Color::RESET;

        auto row = [](const std::string& label, const std::string& value) {
            std::cout << Color::CYAN << "  | " << Color::WHITE
                      << std::left << std::setw(32) << label
                      << Color::BOLD_GREEN << std::right << std::setw(24) << value
                      << Color::CYAN << " |\n" << Color::RESET;
        };

        row("Direction", sender + " -> " + receiver);
        row("Payload Size", std::to_string(payloadBytes) + " bytes");
        row("MTU", std::to_string(mtu) + " bytes");
        row("Fragments", std::to_string(fragmentCount));
        row("Window Size (Go-Back-N)", std::to_string(windowSize));
        row("Rounds Used", std::to_string(roundsUsed));

        std::cout << Color::CYAN
                  << "  +----------------------------------------------------------+\n"
                  << Color::RESET;

        row("Total DATA Frames Sent", std::to_string(totalFramesSent));
        row("Frames Delivered (OK)", std::to_string(framesDelivered));
        row("Frames Corrupted", std::to_string(framesCorrupted));
        row("Retransmissions", std::to_string(totalRetransmissions));

        std::cout << Color::CYAN
                  << "  +----------------------------------------------------------+\n"
                  << Color::RESET;

        row("Total Bits Transmitted", std::to_string(totalBitsTransmitted));
        row("Total Bits Flipped", std::to_string(totalBitsFlipped));

        // Computed metrics
        double ber = (totalBitsTransmitted > 0)
            ? static_cast<double>(totalBitsFlipped) / totalBitsTransmitted * 100.0
            : 0.0;
        std::ostringstream berStr;
        berStr << std::fixed << std::setprecision(3) << ber << "%";
        row("Bit Error Rate (BER)", berStr.str());

        double efficiency = (totalFramesSent > 0)
            ? static_cast<double>(framesDelivered) / totalFramesSent * 100.0
            : 0.0;
        std::ostringstream effStr;
        effStr << std::fixed << std::setprecision(1) << efficiency << "%";
        row("Transmission Efficiency", effStr.str());

        std::cout << Color::CYAN
                  << "  +----------------------------------------------------------+\n"
                  << Color::RESET;

        std::string status = delivered ? "DELIVERED" : "FAILED";
        std::string statusColor = delivered ? Color::BOLD_GREEN : Color::BOLD_RED;
        std::cout << Color::CYAN << "  | " << Color::WHITE
                  << std::left << std::setw(32) << "Final Status"
                  << statusColor << std::right << std::setw(24) << status
                  << Color::CYAN << " |\n" << Color::RESET;

        std::cout << Color::CYAN
                  << "  +----------------------------------------------------------+\n"
                  << Color::RESET;
        std::cout << "\n";
    }
};

/**
 * @struct SessionStats
 * @brief Accumulates statistics across all transmissions in a session.
 */
struct SessionStats {
    int totalTransmissions   = 0;
    int successfulDeliveries = 0;
    int totalFramesSent      = 0;
    int totalRetransmissions = 0;
    int totalBitsTransmitted = 0;
    int totalBitsFlipped     = 0;

    void accumulate(const TransmissionStats& ts) {
        totalTransmissions++;
        if (ts.delivered) successfulDeliveries++;
        totalFramesSent      += ts.totalFramesSent;
        totalRetransmissions += ts.totalRetransmissions;
        totalBitsTransmitted += ts.totalBitsTransmitted;
        totalBitsFlipped     += ts.totalBitsFlipped;
    }

    void print() const {
        std::cout << "\n";
        std::cout << Color::BOLD_CYAN
                  << "  +----------------------------------------------------------+\n"
                  << "  |              SESSION CUMULATIVE STATISTICS                |\n"
                  << "  +----------------------------------------------------------+\n"
                  << Color::RESET;

        auto row = [](const std::string& label, const std::string& value) {
            std::cout << Color::CYAN << "  | " << Color::WHITE
                      << std::left << std::setw(32) << label
                      << Color::BOLD_GREEN << std::right << std::setw(24) << value
                      << Color::CYAN << " |\n" << Color::RESET;
        };

        row("Total Transmissions", std::to_string(totalTransmissions));
        row("Successful Deliveries",
            std::to_string(successfulDeliveries) + "/" + std::to_string(totalTransmissions));
        row("Total DATA Frames Sent", std::to_string(totalFramesSent));
        row("Total Retransmissions", std::to_string(totalRetransmissions));
        row("Total Bits Transmitted", std::to_string(totalBitsTransmitted));
        row("Total Bits Flipped", std::to_string(totalBitsFlipped));

        double ber = (totalBitsTransmitted > 0)
            ? static_cast<double>(totalBitsFlipped) / totalBitsTransmitted * 100.0
            : 0.0;
        std::ostringstream berStr;
        berStr << std::fixed << std::setprecision(3) << ber << "%";
        row("Overall BER", berStr.str());

        std::cout << Color::CYAN
                  << "  +----------------------------------------------------------+\n"
                  << Color::RESET;
        std::cout << "\n";
    }
};

#endif // STATISTICS_H
