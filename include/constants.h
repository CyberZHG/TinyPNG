#ifndef TINYPNG_CONSTANTS_H
#define TINYPNG_CONSTANTS_H

#include <cstdint>

namespace tiny_png {

    constexpr std::uint16_t LZ77_MIN_MATCH_LEN = 3;
    constexpr std::uint16_t LZ77_MAX_MATCH_LEN = 258;
    constexpr std::uint16_t LZ77_MAX_MATCH_OFFSET = 32768;

}

#endif //TINYPNG_CONSTANTS_H