#include <gtest/gtest.h>
#include "deflate.h"
using namespace std::string_literals;
using namespace tiny_png;

TEST(Huffman, Empty) {
    constexpr std::vector<LZ77Seq> seqs;
    const auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "101"s + "0000000" + "000000");
}

TEST(Huffman, Literal0) {
    const std::vector<LZ77Seq> seqs = {{0, 0, 0}};
    const auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "101"s + "00001100" + "0000000" + "000000");
}

TEST(Huffman, Literal143) {
    const std::vector<LZ77Seq> seqs = {{143, 0, 0}};
    const auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "101"s + "11111101" + "0000000" + "000000");
}

TEST(Huffman, Literal144) {
    const std::vector<LZ77Seq> seqs = {{144, 0, 0}};
    const auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "101"s + "000010011" + "0000000" + "00000");
}

TEST(Huffman, Literal255) {
    const std::vector<LZ77Seq> seqs = {{255, 0, 0}};
    const auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "101"s + "111111111" + "0000000" + "00000");
}

TEST(Huffman, Length3Offset1) {
    const std::vector<LZ77Seq> seqs = {{0, 1, 3}};
    const auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "101"s + "1000000" + "00000" + "0000000" + "00");
}

TEST(Huffman, Length11Offset1) {
    const std::vector<LZ77Seq> seqs = {{0, 1, 11}};
    const auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "101"s + "1001000" + "0" + "00000" + "0000000" + "0");
}

TEST(Huffman, Length12Offset1) {
    const std::vector<LZ77Seq> seqs = {{0, 1, 12}};
    const auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "101"s + "1001000" + "1" + "00000" + "0000000" + "0");
}

TEST(Huffman, Length227ffset1) {
    const std::vector<LZ77Seq> seqs = {{0, 1, 227}};
    const auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "101"s + "00100011" + "00000" + "00000" + "0000000" + "0000");
}

TEST(Huffman, Length257ffset1) {
    const std::vector<LZ77Seq> seqs = {{0, 1, 257}};
    const auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "101"s + "00100011" + "01111" + "00000" + "0000000" + "0000");
}

TEST(Huffman, Length258ffset1) {
    const std::vector<LZ77Seq> seqs = {{0, 1, 258}};
    const auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "101"s + "10100011" + "00000" + "0000000" + "0");
}

TEST(Huffman, Length3Offset5) {
    const std::vector<LZ77Seq> seqs = {{0, 5, 3}};
    const auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "101"s + "1000000" + "00100" + "0" + "0000000" + "0");
}

TEST(Huffman, Length3Offset6) {
    const std::vector<LZ77Seq> seqs = {{0, 6, 3}};
    const auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "101"s + "1000000" + "00100" + "1" + "0000000" + "0");
}

TEST(Huffman, Length3Offset24577) {
    const std::vector<LZ77Seq> seqs = {{0, 24577, 3}};
    const auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "101"s + "1000000" + "10111" + "0000000000000" + "0000000" + "00000");
}

TEST(Huffman, Length3Offset32768) {
    const std::vector<LZ77Seq> seqs = {{0, 32768, 3}};
    const auto bs = huffman_encode(seqs);
    EXPECT_EQ(bs.toString(), "101"s + "1000000" + "10111" + "1111111111111" + "0000000" + "00000");
}
