#include <random>
#include <gtest/gtest.h>
#include "deflate.h"
using namespace tiny_png;

TEST(Deflate, Empty) {
    constexpr std::vector<std::uint8_t> bytes;
    const auto encoded = deflate_encode(bytes.data(), bytes.size());
    const std::vector<std::uint8_t> expected = {0x78, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01};
    EXPECT_EQ(encoded, expected);
    EXPECT_EQ(deflate_decode(encoded), bytes);
}

TEST(Deflate, RandomTest) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> bytesLen(1, 100000);
    for (int i = 0; i < 100; ++i) {
        std::uniform_int_distribution<> byte(0, i % 2 == 0 ? 3 : 255);
        std::vector<std::uint8_t> bytes(bytesLen(gen));
        for (unsigned char& j : bytes) {
            j = static_cast<std::uint8_t>(byte(gen));
        }
        const auto encoded = deflate_encode(bytes.data(), bytes.size());
        ASSERT_EQ(deflate_decode(encoded), bytes);
    }
}
