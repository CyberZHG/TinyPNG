#ifndef TINYPNG_LZ77_H
#define TINYPNG_LZ77_H

#include <cstdint>
#include <vector>

namespace tiny_png {

    struct LZ77Seq {
        std::uint8_t byte;
        std::uint16_t offset;
        std::uint16_t len;

        bool operator==(const LZ77Seq& other) const {
            return byte == other.byte && offset == other.offset && len == other.len;
        }
    };

    std::vector<LZ77Seq> lz77_encode(const std::uint8_t* data, std::size_t len);

    std::vector<std::uint8_t> lz77_decode(const std::vector<LZ77Seq>& seqs);

}

#endif //TINYPNG_LZ77_H