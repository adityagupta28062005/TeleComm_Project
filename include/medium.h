/**
 * @file medium.h
 * @brief Simulated transmission medium with configurable noise.
 *
 * The Medium class models the physical cable / wireless channel
 * between two nodes.  It accepts a binary bit stream and, with a
 * configurable probability, flips individual bits to simulate
 * real-world noise and interference.
 */

#ifndef MEDIUM_H
#define MEDIUM_H

#include <string>
#include <random>

class Medium {
public:
    /**
     * @brief Construct a Medium with a given noise probability.
     * @param noiseProbability Probability (0.0 – 1.0) that any single
     *                         bit will be flipped during transmission.
     *                         A value of 0.0 means a perfect channel.
     */
    explicit Medium(double noiseProbability = 0.20);

    /**
     * @brief Transmit a bit stream through the noisy medium.
     *
     * Iterates over every '0' or '1' character in the bit stream.
     * For each bit, a random number is drawn; if it falls below the
     * noise probability, the bit is flipped and a warning is logged.
     *
     * @param bitStream The binary string to transmit.
     * @return The (possibly corrupted) binary string.
     */
    std::string transmit(const std::string& bitStream);

    /**
     * @brief Update the noise probability at runtime.
     * @param prob New probability (0.0 – 1.0).
     */
    void setNoiseProbability(double prob);

    /**
     * @brief Get the current noise probability.
     */
    double getNoiseProbability() const;

private:
    double                              noiseProbability_;  ///< Per-bit flip probability
    std::mt19937                        rng_;               ///< Mersenne Twister PRNG
    std::uniform_real_distribution<>    dist_;              ///< Uniform [0, 1) distribution
};

#endif // MEDIUM_H
