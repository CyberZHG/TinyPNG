#include "huffman.h"

namespace tiny_png {

    // For the fixed Huffman tree, see: https://www.rfc-editor.org/rfc/rfc1951.pdf
    // 3.2.5 Compressed blocks (length and distance codes)

    constexpr std::uint16_t DEFLATE_END = 256;

    constexpr std::int32_t HUFFMAN_LENGTH_CODE_INDEX = 257;
    constexpr std::int32_t HUFFMAN_LENGTH_CODE_LENGTH = 29;
    constexpr std::uint16_t HUFFMAN_LENGTH_BASE[] = {
        3, 4, 5, 6, 7, 8, 9, 10,  // 257 - 264
        11, 13, 15, 17,  // 265 - 268
        19, 23, 27, 31,  // 269 - 272
        35, 43, 51, 59,  // 273 - 276
        67, 83, 99, 115,  // 277 - 280
        131, 163, 195, 227,  // 281 - 284
        258,  // 285
    };
    constexpr std::uint8_t HUFFMAN_LENGTH_EXTRA_BITS[] = {
        0, 0, 0, 0, 0, 0, 0, 0,  // 257 - 264
        1, 1, 1, 1,  // 265 - 268
        2, 2, 2, 2,  // 269 - 272
        3, 3, 3, 3,  // 273 - 276
        4, 4, 4, 4,  // 277 - 280
        5, 5, 5, 5,  // 281 - 284
        0,  // 285
    };

    constexpr std::int32_t HUFFMAN_DISTANCE_CODE_LENGTH = 30;
    constexpr std::int32_t HUFFMAN_DISTANCE_BASE[] = {
        1, 2, 3, 4,
        5, 7, 9, 13,
        17, 25, 33, 49,
        65, 97, 129, 193,
        257, 385, 513, 769,
        1025, 1537, 2049, 3073,
        4097, 6145, 8193, 12289,
        16385, 24577,
    };
    constexpr std::int32_t HUFFMAN_DISTANCE_EXTRA_BITS[] = {
        0, 0, 0, 0,
        1, 1, 2, 2,
        3, 3, 4, 4,
        5, 5, 6, 6,
        7, 7, 8, 8,
        9, 9, 10, 10,
        11, 11, 12, 12,
        13, 13,
    };

}
