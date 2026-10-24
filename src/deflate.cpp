#include "constants.h"
#include "deflate.h"

namespace tiny_png {

    constexpr std::uint16_t DEFLATE_END = 256;

    constexpr std::size_t HUFFMAN_CODE_LENGTH = 4;
    constexpr std::int16_t HUFFMAN_CODE_INDEX[] = {0, 144, 256, 280, 288};
    constexpr std::uint8_t HUFFMAN_CODE_BITS[] = {8, 9, 7, 8};
    constexpr std::int16_t HUFFMAN_CODE_OFFSET[] = {0b00110000, 0b110010000, 0, 0b11000000};

    constexpr std::int16_t HUFFMAN_LENGTH_CODE_START = 257;
    constexpr std::int32_t HUFFMAN_LENGTH_EXTRA_BITS_SIZE = 29;
    constexpr std::uint8_t HUFFMAN_LENGTH_EXTRA_BITS[] = {
        0, 0, 0, 0, 0, 0, 0, 0,  // 257 - 264
        1, 1, 1, 1,  // 265 - 268
        2, 2, 2, 2,  // 269 - 272
        3, 3, 3, 3,  // 273 - 276
        4, 4, 4, 4,  // 277 - 280
        5, 5, 5, 5,  // 281 - 284
        0,  // 285
    };
    static std::uint16_t huffmanLengthToBase[LZ77_MAX_MATCH_LEN + 1];
    static std::uint16_t huffmanLengthToCode[LZ77_MAX_MATCH_LEN + 1];
    static std::uint8_t huffmanLengthToExtraBits[LZ77_MAX_MATCH_LEN + 1];

    constexpr std::int32_t HUFFMAN_OFFSET_EXTRA_BITS_SIZE = 30;
    constexpr std::int32_t HUFFMAN_OFFSET_EXTRA_BITS[] = {
        0, 0, 0, 0,
        1, 1, 2, 2,
        3, 3, 4, 4,
        5, 5, 6, 6,
        7, 7, 8, 8,
        9, 9, 10, 10,
        11, 11, 12, 12,
        13, 13,
    };
    static std::uint16_t huffmanOffsetToBase[LZ77_MAX_MATCH_OFFSET + 1];
    static std::uint16_t huffmanOffsetToCode[LZ77_MAX_MATCH_OFFSET + 1];
    static std::uint8_t huffmanOffsetToExtraBits[LZ77_MAX_MATCH_OFFSET + 1];

    void initializeHuffmanLookup() {
        static bool initialized = false;
        if (initialized) {
            return;
        }
        int length = LZ77_MIN_MATCH_LEN;
        for (int i = 0; i < HUFFMAN_LENGTH_EXTRA_BITS_SIZE; ++i) {
            const int range = 1 << HUFFMAN_LENGTH_EXTRA_BITS[i];
            const int base = length;
            for (int j = 0; j < range && length < LZ77_MAX_MATCH_LEN; ++j) {
                huffmanLengthToBase[length] = base;
                huffmanLengthToCode[length] = HUFFMAN_LENGTH_CODE_START + i;
                huffmanLengthToExtraBits[length++] = HUFFMAN_LENGTH_EXTRA_BITS[i];
            }
        }
        huffmanLengthToBase[LZ77_MAX_MATCH_LEN] = LZ77_MAX_MATCH_LEN;
        huffmanLengthToCode[LZ77_MAX_MATCH_LEN] = 285;
        huffmanLengthToBase[LZ77_MAX_MATCH_LEN] = 0;
        int offset = 1;
        for (int i = 0; i < HUFFMAN_OFFSET_EXTRA_BITS_SIZE; ++i) {
            const int range = 1 << HUFFMAN_OFFSET_EXTRA_BITS[i];
            const int base = offset;
            for (int j = 0; j < range; ++j) {
                huffmanOffsetToBase[offset] = base;
                huffmanOffsetToCode[offset] = i;
                huffmanOffsetToExtraBits[offset++] = HUFFMAN_OFFSET_EXTRA_BITS[i];
            }
        }
        initialized = true;
    }

    size_t findHuffmanCodeBitsIndex(const std::uint16_t value) {
        for (int index = HUFFMAN_CODE_LENGTH - 1; index >= 0; --index) {
            if (value >= HUFFMAN_CODE_INDEX[index]) {
                return index;
            }
        }
        throw std::out_of_range("Huffman code index out of range");
    }

    void writeLiteral(BitStream& bits, const std::uint16_t value) {
        const auto index = findHuffmanCodeBitsIndex(value);
        bits.writeInt(HUFFMAN_CODE_OFFSET[index] + (value - HUFFMAN_CODE_INDEX[index]), HUFFMAN_CODE_BITS[index], true);
    }

    void writeLength(BitStream& bits, const std::uint16_t value) {
        const auto base = huffmanLengthToBase[value];
        const auto code = huffmanLengthToCode[value];
        const auto extraBits = huffmanLengthToExtraBits[value];
        writeLiteral(bits, code);
        bits.writeInt((value - base) & ((1 << extraBits) - 1), extraBits, false);
    }

    void writeOffset(BitStream& bits, const std::uint16_t value) {
        const auto base = huffmanOffsetToBase[value];
        const auto code = huffmanOffsetToCode[value];
        const auto extraBits = huffmanOffsetToExtraBits[value];
        bits.writeInt(code, 5, true);
        bits.writeInt((value - base) & ((1 << extraBits) - 1), extraBits, false);
    }

    BitStream huffman_encode(const std::vector<LZ77Seq> &seqs) {
        BitStream bits;
        initializeHuffmanLookup();
        bits.writeInt(1, 1, false); // Last block
        bits.writeInt(0b01, 2, false); // Fixed Huffman tree
        for (const auto& [byte, offset, len] : seqs) {
            if (offset == 0) {
                writeLiteral(bits, byte);
            } else {
                writeLength(bits, len);
                writeOffset(bits, offset);
            }
        }
        writeLiteral(bits, DEFLATE_END);
        return bits;
    }

    std::vector<LZ77Seq> huffman_decode(BitStream &bits) {
        bits.rewind();
        return {};
    }
}
