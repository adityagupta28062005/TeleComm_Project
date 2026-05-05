/**
 * @file medium.cpp
 * @brief Implementation of the simulated transmission Medium.
 */

#include "medium.h"
#include "colors.h"

#include <sstream>
#include <iomanip>
#include <chrono>

Medium::Medium(double noiseProbability)
    : noiseProbability_(noiseProbability)
    , rng_(static_cast<unsigned>(
          std::chrono::steady_clock::now().time_since_epoch().count()))
    , dist_(0.0, 1.0)
{
}

// Primary transmit -- returns flipped count via out-param
std::string Medium::transmit(const std::string& bitStream, int& flippedBitCount) {
    logMedium("Transmitting " + std::to_string(bitStream.size()) +
              " characters through the medium...");
    std::ostringstream probOss;
    probOss << std::fixed << std::setprecision(1) << (noiseProbability_ * 100) << "%";
    logMedium("Noise probability: " + probOss.str());

    std::string output = bitStream;
    flippedBitCount = 0;

    for (size_t i = 0; i < output.size(); ++i) {
        if (output[i] != '0' && output[i] != '1') {
            continue;
        }

        double roll = dist_(rng_);
        if (roll < noiseProbability_) {
            char original = output[i];
            output[i] = (output[i] == '0') ? '1' : '0';
            ++flippedBitCount;

            std::ostringstream oss;
            oss << "[!] NOISE DETECTED at bit position " << i
                << ":  " << original << " -> " << output[i];
            logMedium(oss.str());
        }
    }

    if (flippedBitCount == 0) {
        logMedium("Transmission complete -- no bit errors introduced.");
    } else {
        std::ostringstream oss;
        oss << "Transmission complete -- " << flippedBitCount
            << " bit(s) flipped by noise!";
        logMedium(oss.str());
    }

    return output;
}

// Backward-compatible overload
std::string Medium::transmit(const std::string& bitStream) {
    int dummy = 0;
    return transmit(bitStream, dummy);
}

void Medium::setNoiseProbability(double prob) {
    noiseProbability_ = prob;
}

double Medium::getNoiseProbability() const {
    return noiseProbability_;
}
