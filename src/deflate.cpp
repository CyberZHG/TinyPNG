#include "lz77.h"
#include "huffman.h"
#include "deflate.h"

namespace tiny_png {

    static std::uint32_t adler32(const std::uint8_t* data, std::size_t len) {
        constexpr std::size_t MAX_CHUNK = 5552;
        std::uint32_t a = 1, b = 0;
        for (std::size_t start = 0; start < len; start += MAX_CHUNK) {
            constexpr std::uint32_t MOD = 65521;
            const std::size_t end = std::min(len, start + MAX_CHUNK);
            for (std::size_t i = start; i < end; i++) {
                a += data[i];
                b += a;
            }
            a %= MOD;
            b %= MOD;
        }
        return (b << 16) | a;
    }

    std::vector<std::uint8_t> deflate_encode(const std::uint8_t* data, std::size_t len) {
        std::vector<std::uint8_t> encoded;
        encoded.push_back(0b01111000);  // CM = 8, CINFO = 7, LZ77 with 32K window
        encoded.push_back(0b00000001);  // FCHECK = 1, FDICT = 0, FLEVEL = 0
        const auto seqs = lz77_encode(data, len);
        const auto bits = huffman_encode(seqs);
        encoded.insert(encoded.end(), bits.getBytes().begin(), bits.getBytes().end());
        const auto checksum = adler32(data, len);
        encoded.push_back(checksum >> 24);
        encoded.push_back(checksum >> 16);
        encoded.push_back(checksum >> 8);
        encoded.push_back(checksum);
        return encoded;
    }

    std::vector<std::uint8_t> deflate_decode(const std::vector<std::uint8_t>& data) {
        if (data.size() <= 6) {
            return {};
        }
        BitStream bits;
        bits.setBytes(data.begin() + 2, data.begin() + data.size() - 4);  // Ignore head and checksum
        const auto seqs = huffman_decode(bits);
        return lz77_decode(seqs);
    }

}
