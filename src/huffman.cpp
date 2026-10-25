#include <algorithm>
#include <stdexcept>
#include "tiny_png/constants.h"
#include "tiny_png/huffman.h"

namespace tiny_png {

    constexpr std::uint16_t DEFLATE_END = 256;
    constexpr std::int32_t DEFLATE_BLOCK_STORED = 0;
    constexpr std::int32_t DEFLATE_BLOCK_FIXED_HUFFMAN = 1;
    constexpr std::int32_t DEFLATE_BLOCK_DYNAMIC_HUFFMAN = 2;

    constexpr std::size_t HUFFMAN_CODE_INDEX_SIZE = 4;
    constexpr std::size_t HUFFMAN_NUM_CODE = 288;
    constexpr std::int16_t HUFFMAN_CODE_INDEX[] = {0, 144, 256, 280, HUFFMAN_NUM_CODE};
    constexpr std::uint8_t HUFFMAN_CODE_BITS[] = {8, 9, 7, 8};
    constexpr std::int16_t HUFFMAN_CODE_OFFSET[] = {0b00110000, 0b110010000, 0, 0b11000000};

    constexpr std::int16_t HUFFMAN_LENGTH_CODE_START = 257;
    constexpr std::int16_t HUFFMAN_LENGTH_CODE_END = 285;
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
    static std::uint16_t huffmanCodeToLengthBase[HUFFMAN_NUM_CODE];
    static std::uint8_t huffmanCodeToLengthExtraBits[HUFFMAN_NUM_CODE];

    constexpr std::int32_t HUFFMAN_DISTANCE_EXTRA_BITS_SIZE = 30;
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
    static std::uint16_t huffmanDistanceToBase[LZ77_MAX_MATCH_OFFSET + 1];
    static std::uint16_t huffmanDistanceToCode[LZ77_MAX_MATCH_OFFSET + 1];
    static std::uint8_t huffmanDistanceToExtraBits[LZ77_MAX_MATCH_OFFSET + 1];
    static std::uint16_t huffmanDistanceToOffsetBase[HUFFMAN_DISTANCE_EXTRA_BITS_SIZE];

    constexpr std::int32_t HUFFMAN_MAX_CODE_BITS = 15;
    constexpr std::int32_t HUFFMAN_MAX_LITERAL_LENGTH_CODES = 286;
    constexpr std::int32_t HUFFMAN_NUM_CODE_LENGTH_CODES = 19;
    constexpr std::uint8_t HUFFMAN_CODE_LENGTH_ORDER[] = {
        16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15,
    };

    class HuffmanTree {
    public:
        HuffmanTree() : _nodeNum(1), _nodes(575) {}

        void addCode(const std::int16_t code, const std::int16_t value, const std::uint8_t bits) {
            addCode(0, code, value, bits);
        }

        std::int16_t findValue(BitStream& bits) const {
            return findValue(0, bits);
        }

    private:
        struct TreeNode {
            std::int32_t next[2];
            std::int16_t value;

            TreeNode() : next{-1, -1}, value(-1) {}
        };

        std::int32_t _nodeNum;
        std::vector<TreeNode> _nodes;

        void addCode(const std::size_t index, const std::int16_t code, const std::int16_t value, std::uint8_t bits) {
            if (index >= _nodes.size()) {
                _nodes.resize(index + 1);
            }
            if (bits == 0) {
                _nodes[index].value = value;
                return;
            }
            --bits;
            const bool bit = code & (1 << bits);
            if (_nodes[index].next[bit] == -1) {
                _nodes[index].next[bit] = _nodeNum++;
            }
            addCode(_nodes[index].next[bit], code, value, bits);
        }

        std::int16_t findValue(const std::size_t index, BitStream& bits) const {
            if (_nodes[index].value != -1) {
                return _nodes[index].value;
            }
            const auto next = _nodes[index].next[bits.nextBit()];
            if (next == -1) {
                return -1;
            }
            return findValue(next, bits);
        }
    };
    static HuffmanTree* literalLengthTree = nullptr;

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
                const auto code = HUFFMAN_LENGTH_CODE_START + i;
                const auto extraBits = HUFFMAN_LENGTH_EXTRA_BITS[i];
                huffmanLengthToBase[length] = base;
                huffmanLengthToCode[length] = code;
                huffmanLengthToExtraBits[length++] = extraBits;
                huffmanCodeToLengthBase[code] = base;
                huffmanCodeToLengthExtraBits[code] = extraBits;
            }
        }
        huffmanLengthToBase[LZ77_MAX_MATCH_LEN] = LZ77_MAX_MATCH_LEN;
        huffmanLengthToCode[LZ77_MAX_MATCH_LEN] = HUFFMAN_LENGTH_CODE_END;
        huffmanLengthToExtraBits[LZ77_MAX_MATCH_LEN] = 0;
        huffmanCodeToLengthBase[HUFFMAN_LENGTH_CODE_END] = 258;
        huffmanCodeToLengthExtraBits[HUFFMAN_LENGTH_CODE_END] = 0;
        int dist = 1;
        for (int i = 0; i < HUFFMAN_DISTANCE_EXTRA_BITS_SIZE; ++i) {
            const auto extraBits = HUFFMAN_DISTANCE_EXTRA_BITS[i];
            const int range = 1 << extraBits;
            const int base = dist;
            for (int j = 0; j < range; ++j) {
                huffmanDistanceToBase[dist] = base;
                huffmanDistanceToCode[dist] = i;
                huffmanDistanceToExtraBits[dist++] = extraBits;
            }
            huffmanDistanceToOffsetBase[i] = base;
        }
        initialized = true;
    }

    void initializeHuffmanTree() {
        if (literalLengthTree != nullptr) {
            return;
        }
        literalLengthTree = new HuffmanTree();
        for (std::size_t i = 0; i < HUFFMAN_CODE_INDEX_SIZE; ++i) {
            const auto range = static_cast<std::int16_t>(HUFFMAN_CODE_INDEX[i + 1] - HUFFMAN_CODE_INDEX[i]);
            for (std::int16_t j = 0; j < range; ++j) {
                const auto code = static_cast<std::int16_t>(HUFFMAN_CODE_OFFSET[i] + j);
                const auto value = static_cast<std::int16_t>(HUFFMAN_CODE_INDEX[i] + j);
                literalLengthTree->addCode(code, value, HUFFMAN_CODE_BITS[i]);
            }
        }
    }

    size_t findHuffmanCodeBitsIndex(const std::uint16_t value) {
        for (int index = HUFFMAN_CODE_INDEX_SIZE - 1; index >= 0; --index) {
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
        const auto base = huffmanDistanceToBase[value];
        const auto code = huffmanDistanceToCode[value];
        const auto extraBits = huffmanDistanceToExtraBits[value];
        bits.writeInt(code, 5, true);
        bits.writeInt((value - base) & ((1 << extraBits) - 1), extraBits, false);
    }

    BitStream huffman_encode(const std::vector<LZ77Seq> &seqs) {
        BitStream bits;
        initializeHuffmanLookup();
        bits.writeInt(1, 1, false); // Last block
        bits.writeInt(0b01, 2, false); // Fixed Huffman tree
        for (const auto& [byte, len, dist] : seqs) {
            if (dist == 0) {
                writeLiteral(bits, byte);
            } else {
                writeLength(bits, len);
                writeOffset(bits, dist);
            }
        }
        writeLiteral(bits, DEFLATE_END);
        return bits;
    }

    static bool decodeStoredBlock(BitStream& bits, std::vector<LZ77Seq>& seqs) {
        bits.alignToByte();
        const auto len = bits.readInt(16, false);
        if ((len ^ bits.readInt(16, false)) != 0xFFFF) {
            return false;
        }
        for (std::int32_t i = 0; i < len; ++i) {
            seqs.emplace_back(static_cast<std::uint8_t>(bits.readInt(8, false)), 0, 0);
        }
        return true;
    }

    static bool decodeHuffmanBlock(BitStream& bits, std::vector<LZ77Seq>& seqs, const HuffmanTree& literalTree, const HuffmanTree* distanceTree) {
        while (true) {
            const auto code = literalTree.findValue(bits);
            if (code < 0 || code > HUFFMAN_LENGTH_CODE_END) {
                return false;
            }
            if (code == DEFLATE_END) {
                return true;
            }
            if (code < 256) {
                seqs.emplace_back(code, 0, 0);
            } else {
                auto len = huffmanCodeToLengthBase[code];
                if (const auto lengthExtraBits = huffmanCodeToLengthExtraBits[code]) {
                    len += bits.readInt(lengthExtraBits, false);
                }
                const auto distanceCode = distanceTree == nullptr ? bits.readInt(5, true) : distanceTree->findValue(bits);
                if (distanceCode < 0 || distanceCode >= HUFFMAN_DISTANCE_EXTRA_BITS_SIZE) {
                    return false;
                }
                auto dist = huffmanDistanceToOffsetBase[distanceCode];
                if (const auto distanceExtraBits = HUFFMAN_DISTANCE_EXTRA_BITS[distanceCode]) {
                    dist += bits.readInt(distanceExtraBits, false);
                }
                seqs.emplace_back(0, len, dist);
            }
        }
    }

    static bool decodeFixedHuffmanBlock(BitStream& bits, std::vector<LZ77Seq>& seqs) {
        return decodeHuffmanBlock(bits, seqs, *literalLengthTree, nullptr);
    }

    static bool buildHuffmanTree(const std::vector<std::uint8_t>& lengths, HuffmanTree& tree, const bool allowIncomplete) {
        std::int32_t lengthCount[HUFFMAN_MAX_CODE_BITS + 1] = {};
        for (const auto length : lengths) {
            ++lengthCount[length];
        }
        lengthCount[0] = 0;
        std::int32_t left = 1;
        for (std::int32_t bits = 1; bits <= HUFFMAN_MAX_CODE_BITS; ++bits) {
            left = (left << 1) - lengthCount[bits];
            if (left < 0) {  // Over-subscribed
                return false;
            }
        }
        if (left > 0) {
            const auto numCodes = lengths.size() - std::ranges::count(lengths, 0);
            const bool singleCode = numCodes == 1 && lengthCount[1] == 1;
            if (numCodes > 0 && !(allowIncomplete && singleCode)) {
                return false;
            }
        }
        std::int32_t nextCode[HUFFMAN_MAX_CODE_BITS + 1] = {};
        for (std::int32_t bits = 1, code = 0; bits <= HUFFMAN_MAX_CODE_BITS; ++bits) {
            code = (code + lengthCount[bits - 1]) << 1;
            nextCode[bits] = code;
        }
        for (std::size_t symbol = 0; symbol < lengths.size(); ++symbol) {
            if (const auto length = lengths[symbol]) {
                tree.addCode(static_cast<std::int16_t>(nextCode[length]++), static_cast<std::int16_t>(symbol), length);
            }
        }
        return true;
    }

    static bool decodeDynamicHuffmanBlock(BitStream& bits, std::vector<LZ77Seq>& seqs) {
        const auto numLiteralCodes = bits.readInt(5, false) + 257;
        const auto numDistanceCodes = bits.readInt(5, false) + 1;
        const auto numCodeLengthCodes = bits.readInt(4, false) + 4;
        if (numLiteralCodes > HUFFMAN_MAX_LITERAL_LENGTH_CODES) {
            return false;
        }

        std::vector<std::uint8_t> codeLengthLengths(HUFFMAN_NUM_CODE_LENGTH_CODES, 0);
        for (std::int32_t i = 0; i < numCodeLengthCodes; ++i) {
            codeLengthLengths[HUFFMAN_CODE_LENGTH_ORDER[i]] = static_cast<std::uint8_t>(bits.readInt(3, false));
        }
        HuffmanTree codeLengthTree;
        if (!buildHuffmanTree(codeLengthLengths, codeLengthTree, false)) {
            return false;
        }

        const auto numLengths = static_cast<std::size_t>(numLiteralCodes + numDistanceCodes);
        std::vector<std::uint8_t> lengths;
        lengths.reserve(numLengths);
        while (lengths.size() < numLengths) {
            const auto symbol = codeLengthTree.findValue(bits);
            std::uint8_t length = 0;
            std::int32_t repeat = 1;
            if (symbol < 0) {
                return false;
            }
            if (symbol < 16) {
                length = static_cast<std::uint8_t>(symbol);
            } else if (symbol == 16) {  // Repeat the previous length 3 - 6 times
                if (lengths.empty()) {
                    return false;
                }
                length = lengths.back();
                repeat = 3 + bits.readInt(2, false);
            } else if (symbol == 17) {  // Repeat zero 3 - 10 times
                repeat = 3 + bits.readInt(3, false);
            } else {  // Repeat zero 11 - 138 times
                repeat = 11 + bits.readInt(7, false);
            }
            if (lengths.size() + repeat > numLengths) {
                return false;
            }
            lengths.insert(lengths.end(), repeat, length);
        }

        const std::vector literalLengths(lengths.begin(), lengths.begin() + numLiteralCodes);
        const std::vector distanceLengths(lengths.begin() + numLiteralCodes, lengths.end());
        if (literalLengths[DEFLATE_END] == 0) {
            return false;
        }
        HuffmanTree literalTree, distanceTree;
        if (!buildHuffmanTree(literalLengths, literalTree, true) || !buildHuffmanTree(distanceLengths, distanceTree, true)) {
            return false;
        }
        return decodeHuffmanBlock(bits, seqs, literalTree, &distanceTree);
    }

    std::vector<LZ77Seq> huffman_decode(BitStream &bits) {
        std::vector<LZ77Seq> seqs;
        initializeHuffmanLookup();
        initializeHuffmanTree();
        bits.rewind();
        bool finalBlock = false;
        while (!finalBlock) {
            finalBlock = bits.nextBit();
            bool valid = false;
            switch (bits.readInt(2, false)) {
                case DEFLATE_BLOCK_STORED: valid = decodeStoredBlock(bits, seqs); break;
                case DEFLATE_BLOCK_FIXED_HUFFMAN: valid = decodeFixedHuffmanBlock(bits, seqs); break;
                case DEFLATE_BLOCK_DYNAMIC_HUFFMAN: valid = decodeDynamicHuffmanBlock(bits, seqs); break;
                default: break;
            }
            if (!valid) {
                break;
            }
        }
        return seqs;
    }
}
