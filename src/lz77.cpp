#include <cstring>
#include "tiny_png/constants.h"
#include "tiny_png/lz77.h"

namespace tiny_png {

    constexpr std::uint16_t MAX_MATCH_NUM = 32;
    constexpr std::uint16_t MATCH_HASH_BITS = 15;

    static std::int64_t heads[1 << MATCH_HASH_BITS];

    static std::uint16_t bytesHash(const std::uint8_t* data, const std::size_t pos) {
        std::uint16_t h = data[pos];
        h = (h * 251) ^ data[pos + 1];
        h = (h * 251) ^ data[pos + 2];
        return h & ((1 << MATCH_HASH_BITS) - 1);
    }

    std::vector<LZ77Seq> lz77_encode(const std::uint8_t* data, const std::size_t len) {
        std::vector<LZ77Seq> seqs;
        std::memset(heads, -1, sizeof(heads));
        std::vector<std::int64_t> prev(len, -1);
        for (size_t i = 0, matchEnd = 0; i < len; ++i) {
            std::uint16_t hash = 0;
            if (i + LZ77_MIN_MATCH_LEN <= len) {
                hash = bytesHash(data, i);
            }
            if (i < matchEnd) {
            } else if (i + LZ77_MIN_MATCH_LEN - 1 >= len) {
                seqs.emplace_back(data[i], 0, 0);
            } else {
                LZ77Seq bestMatch = {data[i], 0, 0};
                auto head = heads[hash];
                for (size_t matchNum = 0, hasBestMatch = false;
                    head != -1 && matchNum < MAX_MATCH_NUM && i - head <= LZ77_MAX_MATCH_OFFSET && !hasBestMatch;
                    ++matchNum) {
                    const auto offset = static_cast<std::uint16_t>(i - head);
                    for (size_t j = 0; i + j <= len && j <= LZ77_MAX_MATCH_LEN; ++j) {
                        if (i + j == len || j == LZ77_MAX_MATCH_LEN) {
                            hasBestMatch = true;
                            bestMatch = {0, static_cast<std::uint16_t>(j), offset};
                            break;
                        }
                        if (data[head + j] != data[i + j]) {
                            if (j >= LZ77_MIN_MATCH_LEN && j > bestMatch.len) {
                                bestMatch = {0, static_cast<std::uint16_t>(j), offset};
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
            if (i + LZ77_MIN_MATCH_LEN <= len) {
                prev[i] = heads[hash];
                heads[hash] = static_cast<std::int64_t>(i);
            }
        }
        return seqs;
    }

    std::vector<std::uint8_t> lz77_decode(const std::vector<LZ77Seq>& seqs) {
        std::vector<std::uint8_t> data;
        for (const auto& [byte, len, dist] : seqs) {
            if (dist == 0) {
                data.push_back(byte);
            } else {
                for (std::uint16_t i = 0; i < len; ++i) {
                    if (data.size() >= dist) {
                        data.push_back(data[data.size() - dist]);
                    }
                }
            }
        }
        return data;
    }

}
