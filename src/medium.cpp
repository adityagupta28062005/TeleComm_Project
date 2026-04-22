/**
 * @file medium.cpp
 * @brief Implementation of the simulated transmission Medium.
 *
 * Uses the <random> library to model a noisy communication channel.
 * Each bit in the transmitted stream has a configurable probability
 * of being flipped, simulating real-world interference.
 */

#include "medium.h"
#include "colors.h"

#include <sstream>
#include <iomanip>
#include <chrono>

// ═══════════════════════════════════════════════════════════════════════════
//  Constructor
// ═══════════════════════════════════════════════════════════════════════════

Medium::Medium(double noiseProbability)
    : noiseProbability_(noiseProbability)
    , rng_(static_cast<unsigned>(
          std::chrono::steady_clock::now().time_since_epoch().count()))
    , dist_(0.0, 1.0)
{
}

// ═══════════════════════════════════════════════════════════════════════════
//  Transmit — pass a bit stream through the noisy channel
// ═══════════════════════════════════════════════════════════════════════════

std::string Medium::transmit(const std::string& bitStream) {
    logMedium("Transmitting " + std::to_string(bitStream.size()) +
              " characters through the medium...");
    std::ostringstream probOss;
    probOss << std::fixed << std::setprecision(1) << (noiseProbability_ * 100) << "%";
    logMedium("Noise probability: " + probOss.str());

    std::string output = bitStream;
    int flippedCount = 0;

    for (size_t i = 0; i < output.size(); ++i) {
        // Only flip actual bit characters ('0' or '1'), not spaces
        if (output[i] != '0' && output[i] != '1') {
            continue;
        }

        double roll = dist_(rng_);
        if (roll < noiseProbability_) {
            // ── Flip the bit ─────────────────────────────────────────────
            char original = output[i];
            output[i] = (output[i] == '0') ? '1' : '0';
            ++flippedCount;

            std::ostringstream oss;
            oss << "[!] NOISE DETECTED at bit position " << i
                << ":  " << original << " -> " << output[i];
            logMedium(oss.str());
        }
    }

    if (flippedCount == 0) {
        logMedium("Transmission complete -- no bit errors introduced.");
    } else {
        std::ostringstream oss;
        oss << "Transmission complete -- " << flippedCount
            << " bit(s) flipped by noise!";
        logMedium(oss.str());
    }

    return output;
}

// ═══════════════════════════════════════════════════════════════════════════
//  Accessors
// ═══════════════════════════════════════════════════════════════════════════

void Medium::setNoiseProbability(double prob) {
    noiseProbability_ = prob;
}

double Medium::getNoiseProbability() const {
    return noiseProbability_;
}
