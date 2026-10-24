#include <random>
#include <gtest/gtest.h>
#include "lz77.h"
using namespace tiny_png;

static void expectSameBytes(const std::vector<std::uint8_t>& a, const std::vector<std::uint8_t>& b) {
    ASSERT_EQ(a.size(), b.size());
    for (std::size_t i = 0; i < a.size(); ++i) {
        EXPECT_EQ(a[i], b[i]);
    }
}

TEST(LZ77Test, Empty) {
    constexpr std::vector<std::uint8_t> bytes;
    const auto seqs = lz77_encode(bytes.data(), bytes.size());
    ASSERT_EQ(seqs.size(), 0);
    expectSameBytes(lz77_decode(seqs), bytes);
}

TEST(LZ77Test, SingleByte) {
    const std::vector<std::uint8_t> bytes = {'A'};
    const auto seqs = lz77_encode(bytes.data(), bytes.size());
    ASSERT_EQ(seqs.size(), 1);
    ASSERT_EQ(seqs[0], LZ77Seq('A', 0, 0));
    expectSameBytes(lz77_decode(seqs), bytes);
}

TEST(LZ77Test, TwoSameBytes) {
    const std::vector<std::uint8_t> bytes = {'A', 'A'};
    const auto seqs = lz77_encode(bytes.data(), bytes.size());
    ASSERT_EQ(seqs.size(), 2);
    ASSERT_EQ(seqs[0], LZ77Seq('A', 0, 0));
    ASSERT_EQ(seqs[1], LZ77Seq('A', 0, 0));
    expectSameBytes(lz77_decode(seqs), bytes);
}

TEST(LZ77Test, ThreeSameBytes) {
    const std::vector<std::uint8_t> bytes = {'A', 'A', 'A'};
    const auto seqs = lz77_encode(bytes.data(), bytes.size());
    ASSERT_EQ(seqs.size(), 3);
    for (int i = 0; i < 3; ++i) {
        ASSERT_EQ(seqs[i], LZ77Seq('A', 0, 0));
    }
    expectSameBytes(lz77_decode(seqs), bytes);
}

TEST(LZ77Test, FourToFiveSameBytes) {
    std::vector<std::uint8_t> bytes = {'A', 'A', 'A', 'A'};
    for (int k = 0; k < 2; ++k) {
        const auto seqs = lz77_encode(bytes.data(), bytes.size());
        ASSERT_EQ(seqs.size(), 2);
        ASSERT_EQ(seqs[0], LZ77Seq('A', 0, 0));
        ASSERT_EQ(seqs[1], LZ77Seq(0, 1, 3 + k));
        expectSameBytes(lz77_decode(seqs), bytes);
        bytes.push_back('A');
    }
}

TEST(LZ77Test, SixSameBytes) {
    const std::vector<std::uint8_t> bytes = {'A', 'A', 'A', 'A', 'A', 'A'};
    const auto seqs = lz77_encode(bytes.data(), bytes.size());
    ASSERT_EQ(seqs.size(), 2);
    ASSERT_EQ(seqs[0], LZ77Seq('A', 0, 0));
    ASSERT_EQ(seqs[1], LZ77Seq(0, 1, 5));
    expectSameBytes(lz77_decode(seqs), bytes);
}

TEST(LZ77Test, ABCABCABC) {
    const std::vector<std::uint8_t> bytes = {'A', 'B', 'C', 'A', 'B', 'C', 'A', 'B', 'C'};
    const auto seqs = lz77_encode(bytes.data(), bytes.size());
    ASSERT_EQ(seqs.size(), 4);
    ASSERT_EQ(seqs[0], LZ77Seq('A', 0, 0));
    ASSERT_EQ(seqs[1], LZ77Seq('B', 0, 0));
    ASSERT_EQ(seqs[2], LZ77Seq('C', 0, 0));
    ASSERT_EQ(seqs[3], LZ77Seq(0, 3, 6));
    expectSameBytes(lz77_decode(seqs), bytes);
}

TEST(LZ77Test, ABCDABCDE) {
    const std::vector<std::uint8_t> bytes = {'A', 'B', 'C', 'D', 'A', 'B', 'C', 'D', 'E'};
    const auto seqs = lz77_encode(bytes.data(), bytes.size());
    ASSERT_EQ(seqs.size(), 6);
    ASSERT_EQ(seqs[0], LZ77Seq('A', 0, 0));
    ASSERT_EQ(seqs[1], LZ77Seq('B', 0, 0));
    ASSERT_EQ(seqs[2], LZ77Seq('C', 0, 0));
    ASSERT_EQ(seqs[3], LZ77Seq('D', 0, 0));
    ASSERT_EQ(seqs[4], LZ77Seq(0, 4, 4));
    ASSERT_EQ(seqs[5], LZ77Seq('E', 0, 0));
    expectSameBytes(lz77_decode(seqs), bytes);
}

TEST(LZ77Test, ABCDABCABCD) {
    const std::vector<std::uint8_t> bytes = {'A', 'B', 'C', 'D', 'A', 'B', 'C', 'A', 'B', 'C', 'D'};
    const auto seqs = lz77_encode(bytes.data(), bytes.size());
    ASSERT_EQ(seqs.size(), 6);
    ASSERT_EQ(seqs[0], LZ77Seq('A', 0, 0));
    ASSERT_EQ(seqs[1], LZ77Seq('B', 0, 0));
    ASSERT_EQ(seqs[2], LZ77Seq('C', 0, 0));
    ASSERT_EQ(seqs[3], LZ77Seq('D', 0, 0));
    ASSERT_EQ(seqs[4], LZ77Seq(0, 4, 3));
    ASSERT_EQ(seqs[5], LZ77Seq(0, 7, 4));
    expectSameBytes(lz77_decode(seqs), bytes);
}

TEST(LZ77Test, Ax10000) {
    const std::vector<std::uint8_t> bytes(10000, 'A');
    const auto seqs = lz77_encode(bytes.data(), bytes.size());
    ASSERT_EQ(seqs.size(), 40);
    ASSERT_EQ(seqs[0], LZ77Seq('A', 0, 0));
    for (int i = 1; i < 39; ++i) {
        ASSERT_EQ(seqs[i], LZ77Seq(0, 1, 258));
    }
    ASSERT_EQ(seqs[39], LZ77Seq(0, 1, 195));
    expectSameBytes(lz77_decode(seqs), bytes);
}

TEST(LZ77Test, RandomTest) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> bytesLen(1, 100000);
    std::uniform_int_distribution<> byte(0, 255);
    for (int i = 0; i < 100; ++i) {
        std::vector<std::uint8_t> bytes(bytesLen(gen));
        for (unsigned char& j : bytes) {
            j = static_cast<std::uint8_t>(byte(gen));
        }
        const auto seqs = lz77_encode(bytes.data(), bytes.size());
        for (const auto& seq : seqs) {
            if (seq.offset != 0) {
                ASSERT_GE(seq.offset, 1);
                ASSERT_LE(seq.offset, 32768);
                ASSERT_GE(seq.len, 3);
                ASSERT_LE(seq.len, 258);
            }
        }
        expectSameBytes(lz77_decode(seqs), bytes);
    }
}
