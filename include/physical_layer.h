/**
 * @file physical_layer.h
 * @brief Physical Layer (Layer 1) — bit-level encoding and decoding.
 *
 * The PhysicalLayer class converts a Frame's serialized byte string
 * into a stream of '0' and '1' characters (encoding) and back again
 * (decoding).  This simulates the conversion of data into electrical
 * signals on the wire.
 */

#ifndef PHYSICAL_LAYER_H
#define PHYSICAL_LAYER_H

#include <string>

class PhysicalLayer {
public:
    /**
     * @brief Encode a byte string into a binary bit stream.
     *
     * Each byte is converted to an 8-character string of '0'/'1'.
     * Bytes are separated by spaces for readability in the logs.
     *
     * @param data The serialized Frame string.
     * @return A binary string like "01001000 01100101 ...".
     */
    std::string encode(const std::string& data);

    /**
     * @brief Decode a binary bit stream back into a byte string.
     *
     * Spaces between octets are stripped before conversion.
     *
     * @param bitStream The binary string produced by encode().
     * @return The original byte string.
     */
    std::string decode(const std::string& bitStream);
};

#endif // PHYSICAL_LAYER_H
