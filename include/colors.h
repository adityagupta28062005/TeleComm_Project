/**
 * @file colors.h
 * @brief ANSI escape code helpers for colorful, layer-tagged console output.
 *
 * Provides constants for terminal colors and a set of logging functions
 * that prefix output with the corresponding OSI layer tag.
 */

#ifndef COLORS_H
#define COLORS_H

#include <iostream>
#include <string>

namespace Color {

    // ── ANSI Reset ──────────────────────────────────────────────────────
    constexpr const char* RESET   = "\033[0m";

    // ── Regular Colors ──────────────────────────────────────────────────
    constexpr const char* RED     = "\033[31m";
    constexpr const char* GREEN   = "\033[32m";
    constexpr const char* YELLOW  = "\033[33m";
    constexpr const char* BLUE    = "\033[34m";
    constexpr const char* MAGENTA = "\033[35m";
    constexpr const char* CYAN    = "\033[36m";
    constexpr const char* WHITE   = "\033[37m";

    // ── Bold / Bright Colors ────────────────────────────────────────────
    constexpr const char* BOLD       = "\033[1m";
    constexpr const char* BOLD_RED   = "\033[1;31m";
    constexpr const char* BOLD_GREEN = "\033[1;32m";
    constexpr const char* BOLD_YELLOW= "\033[1;33m";
    constexpr const char* BOLD_CYAN  = "\033[1;36m";
    constexpr const char* BOLD_MAG   = "\033[1;35m";
    constexpr const char* BOLD_WHITE = "\033[1;37m";

    // ── Dim ─────────────────────────────────────────────────────────────
    constexpr const char* DIM     = "\033[2m";

} // namespace Color

// ═══════════════════════════════════════════════════════════════════════════
//  Layer-Tagged Logging Helpers
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief Print a separator line to visually divide sections in the console.
 */
inline void printSeparator() {
    std::cout << Color::DIM
              << "----------------------------------------------------------------"
              << Color::RESET << "\n";
}

/**
 * @brief Print a thick separator for major section breaks.
 */
inline void printThickSeparator() {
    std::cout << Color::BOLD << Color::WHITE
              << "================================================================"
              << Color::RESET << "\n";
}

/**
 * @brief Log a message tagged with the Network Layer (Layer 3).
 * @param msg The message to print.
 */
inline void logNetwork(const std::string& msg) {
    std::cout << Color::BOLD_CYAN << "[LAYER 3 - NETWORK]   " << Color::CYAN
              << msg << Color::RESET << "\n";
}

/**
 * @brief Log a message tagged with the Data Link Layer (Layer 2).
 * @param msg The message to print.
 */
inline void logDataLink(const std::string& msg) {
    std::cout << Color::BOLD_YELLOW << "[LAYER 2 - DATALINK]  " << Color::YELLOW
              << msg << Color::RESET << "\n";
}

/**
 * @brief Log a message tagged with the Physical Layer (Layer 1).
 * @param msg The message to print.
 */
inline void logPhysical(const std::string& msg) {
    std::cout << Color::BOLD_GREEN << "[LAYER 1 - PHYSICAL]  " << Color::GREEN
              << msg << Color::RESET << "\n";
}

/**
 * @brief Log a message tagged with the Transmission Medium.
 * @param msg The message to print.
 */
inline void logMedium(const std::string& msg) {
    std::cout << Color::BOLD_MAG << "[MEDIUM]              " << Color::MAGENTA
              << msg << Color::RESET << "\n";
}

/**
 * @brief Log a success message (e.g., ACK received).
 * @param msg The message to print.
 */
inline void logSuccess(const std::string& msg) {
    std::cout << Color::BOLD_GREEN << "[+ SUCCESS]           " << Color::GREEN
              << msg << Color::RESET << "\n";
}

/**
 * @brief Log an error / failure message (e.g., NACK, CRC mismatch).
 * @param msg The message to print.
 */
inline void logError(const std::string& msg) {
    std::cout << Color::BOLD_RED << "[X ERROR]             " << Color::RED
              << msg << Color::RESET << "\n";
}

/**
 * @brief Log a general informational message.
 * @param msg The message to print.
 */
inline void logInfo(const std::string& msg) {
    std::cout << Color::BOLD_WHITE << "[INFO]                " << Color::WHITE
              << msg << Color::RESET << "\n";
}

#endif // COLORS_H
