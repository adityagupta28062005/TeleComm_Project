/**
 * @file medium.h
 * @brief Simulated transmission medium with configurable noise.
 */

#ifndef MEDIUM_H
#define MEDIUM_H

#include <string>
#include <random>

class Medium {
public:
    explicit Medium(double noiseProbability = 0.20);

    /**
     * @brief Transmit a bit stream through the noisy medium.
     * @param bitStream       The binary string to transmit.
     * @param flippedBitCount [out] Number of bits flipped by noise.
     * @return The (possibly corrupted) binary string.
     */
    std::string transmit(const std::string& bitStream, int& flippedBitCount);

    /**
     * @brief Overload without flipped count (backward compat).
     */
    std::string transmit(const std::string& bitStream);

    void setNoiseProbability(double prob);
    double getNoiseProbability() const;

private:
    double                              noiseProbability_;
    std::mt19937                        rng_;
    std::uniform_real_distribution<>    dist_;
};

#endif // MEDIUM_H
