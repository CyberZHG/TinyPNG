#include <gtest/gtest.h>
#include "deflate.h"
using namespace std::string_literals;
using namespace tiny_png;

static void expectSameSeqs(const std::vector<LZ77Seq>& a, const std::vector<LZ77Seq>& b) {
    ASSERT_EQ(a.size(), b.size());
    for (std::size_t i = 0; i < a.size(); ++i) {
        EXPECT_EQ(a[i], b[i]);
    }
}

TEST(Huffman, Empty) {
    constexpr std::vector<LZ77Seq> seqs;
    auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "110"s + "0000000" + "000000");
    const auto decoded = huffman_decode(bs);
    expectSameSeqs(seqs, decoded);
}

TEST(Huffman, Literal0) {
    const std::vector<LZ77Seq> seqs = {{0, 0, 0}};
    auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "110"s + "00110000" + "0000000" + "000000");
    const auto decoded = huffman_decode(bs);
    expectSameSeqs(seqs, decoded);
}

TEST(Huffman, Literal143) {
    const std::vector<LZ77Seq> seqs = {{143, 0, 0}};
    auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "110"s + "10111111" + "0000000" + "000000");
    const auto decoded = huffman_decode(bs);
    expectSameSeqs(seqs, decoded);
}

TEST(Huffman, Literal144) {
    const std::vector<LZ77Seq> seqs = {{144, 0, 0}};
    auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "110"s + "110010000" + "0000000" + "00000");
    const auto decoded = huffman_decode(bs);
    expectSameSeqs(seqs, decoded);
}

TEST(Huffman, Literal255) {
    const std::vector<LZ77Seq> seqs = {{255, 0, 0}};
    auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "110"s + "111111111" + "0000000" + "00000");
    const auto decoded = huffman_decode(bs);
    expectSameSeqs(seqs, decoded);
}

TEST(Huffman, Length3Offset1) {
    const std::vector<LZ77Seq> seqs = {{0, 3, 1}};
    auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "110"s + "0000001" + "00000" + "0000000" + "00");
    const auto decoded = huffman_decode(bs);
    expectSameSeqs(seqs, decoded);
}

TEST(Huffman, Length11Offset1) {
    const std::vector<LZ77Seq> seqs = {{0, 11, 1}};
    auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "110"s + "0001001" + "0" + "00000" + "0000000" + "0");
    const auto decoded = huffman_decode(bs);
    expectSameSeqs(seqs, decoded);
}

TEST(Huffman, Length12Offset1) {
    const std::vector<LZ77Seq> seqs = {{0, 12, 1}};
    auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "110"s + "0001001" + "1" + "00000" + "0000000" + "0");
    const auto decoded = huffman_decode(bs);
    expectSameSeqs(seqs, decoded);
}

TEST(Huffman, Length227ffset1) {
    const std::vector<LZ77Seq> seqs = {{0, 227, 1}};
    auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "110"s + "11000100" + "00000" + "00000" + "0000000" + "0000");
    const auto decoded = huffman_decode(bs);
    expectSameSeqs(seqs, decoded);
}

TEST(Huffman, Length257ffset1) {
    const std::vector<LZ77Seq> seqs = {{0, 257, 1}};
    auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "110"s + "11000100" + "01111" + "00000" + "0000000" + "0000");
    const auto decoded = huffman_decode(bs);
    expectSameSeqs(seqs, decoded);
}

TEST(Huffman, Length258ffset1) {
    const std::vector<LZ77Seq> seqs = {{0, 258, 1}};
    auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "110"s + "11000101" + "00000" + "0000000" + "0");
    const auto decoded = huffman_decode(bs);
    expectSameSeqs(seqs, decoded);
}

TEST(Huffman, Length3Offset5) {
    const std::vector<LZ77Seq> seqs = {{0, 3, 5}};
    auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "110"s + "0000001" + "00100" + "0" + "0000000" + "0");
    const auto decoded = huffman_decode(bs);
    expectSameSeqs(seqs, decoded);
}

TEST(Huffman, Length3Offset6) {
    const std::vector<LZ77Seq> seqs = {{0, 3, 6}};
    auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "110"s + "0000001" + "00100" + "1" + "0000000" + "0");
    const auto decoded = huffman_decode(bs);
    expectSameSeqs(seqs, decoded);
}

TEST(Huffman, Length3Offset24577) {
    const std::vector<LZ77Seq> seqs = {{0, 3, 24577}};
    auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "110"s + "0000001" + "11101" + "0000000000000" + "0000000" + "00000");
    const auto decoded = huffman_decode(bs);
    expectSameSeqs(seqs, decoded);
}

TEST(Huffman, Length3Offset32768) {
    const std::vector<LZ77Seq> seqs = {{0, 3, 32768}};
    auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "110"s + "0000001" + "11101" + "1111111111111" + "0000000" + "00000");
    const auto decoded = huffman_decode(bs);
    expectSameSeqs(seqs, decoded);
}
