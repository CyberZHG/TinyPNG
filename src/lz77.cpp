#include "lz77.h"

namespace tiny_png {

    // Never change these 3 constants
    constexpr std::uint16_t MATCH_MIN_LEN = 3;
    constexpr std::uint16_t MATCH_MAX_OFFSET = 32768;
    constexpr std::uint16_t MATCH_MAX_LEN = 258;

    constexpr std::uint16_t MATCH_MAX_NUM = 32;
    constexpr std::uint16_t HASH_BITS = 15;

    static std::int64_t heads[1 << HASH_BITS];

    static std::uint16_t bytesHash(const std::uint8_t* data, const std::size_t pos) {
        std::uint16_t h = data[pos];
        h = (h * 251) ^ data[pos + 1];
        h = (h * 251) ^ data[pos + 2];
        return h & ((1 << HASH_BITS) - 1);
    }

    std::vector<LZ77Seq> lz77_encode(const std::uint8_t* data, const std::size_t len) {
        std::vector<LZ77Seq> seqs;
        std::memset(heads, -1, sizeof(heads));
        std::vector<std::int64_t> prev(len, -1);
        for (size_t i = 0, matchEnd = 0; i < len; ++i) {
            std::uint16_t hash = 0;
            if (i + MATCH_MIN_LEN <= len) {
                hash = bytesHash(data, i);
            }
            if (i < matchEnd) {
            } else if (i < MATCH_MIN_LEN || i + MATCH_MIN_LEN - 1 >= len) {
                seqs.push_back({data[i], 0, 0});
            } else {
                LZ77Seq bestMatch = {data[i], 0, 0};
                auto head = heads[hash];
                for (size_t matchNum = 0, hasBestMatch = false;
                    head != -1 && matchNum < MATCH_MAX_NUM && i - head <= MATCH_MAX_OFFSET && !hasBestMatch;
                    ++matchNum) {
                    const auto offset = static_cast<std::uint16_t>(i - head);
                    for (size_t j = 0; i + j <= len && j <= MATCH_MAX_LEN; ++j) {
                        if (i + j == len || j == MATCH_MAX_LEN) {
                            hasBestMatch = true;
                            bestMatch = {0, offset, static_cast<std::uint16_t>(j)};
                            break;
                        }
                        if (data[head + j] != data[i + j]) {
                            if (j >= MATCH_MIN_LEN && j > bestMatch.len) {
                                bestMatch = {0, offset, static_cast<std::uint16_t>(j)};
                            }
                            break;
                        }
                    }
                    head = prev[head];
                }
                seqs.push_back(bestMatch);
                if (bestMatch.len) {
                    matchEnd = i + bestMatch.len;
                }
            }
            if (i + MATCH_MIN_LEN <= len) {
                prev[i] = heads[hash];
                heads[hash] = static_cast<std::int64_t>(i);
            }
        }
        return seqs;
    }

    std::vector<std::uint8_t> lz77_decode(const std::vector<LZ77Seq>& seqs) {
        std::vector<std::uint8_t> data;
        for (const auto& [byte, offset, len] : seqs) {
            if (offset == 0) {
                data.push_back(byte);
            } else {
                for (std::uint16_t i = 0; i < len; ++i) {
                    data.push_back(data[data.size() - offset]);
                }
            }
        }
        return data;
    }

}
