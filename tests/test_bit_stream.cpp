#include <gtest/gtest.h>
#include "bit_stream.h"
using namespace tiny_png;

TEST(BitStream, Empty) {
    const BitStream bs;
    EXPECT_EQ(bs.toString(), "");
}

TEST(BitStream, AddEmpty) {
    BitStream bs;
    bs.writeInt(42, 0, false);
    EXPECT_EQ(bs.toString(), "");
}

TEST(BitStream, AddOneBit) {
    BitStream bs;
    bs.writeInt(0b11, 1, false);
    EXPECT_EQ(bs.toString(), "10000000");
}

TEST(BitStream, AddTwoBits) {
    BitStream bs;
    bs.writeInt(0b11, 2, false);
    EXPECT_EQ(bs.toString(), "11000000");
}

TEST(BitStream, AddNineBits) {
    BitStream bs;
    bs.writeInt(0b11, 9, false);
    EXPECT_EQ(bs.toString(), "1100000000000000");
}

TEST(BitStream, Append) {
    BitStream bs;
    bs.writeInt(0b10101, 5, false);
    bs.writeInt(0b11010, 5, false);
    EXPECT_EQ(bs.toString(), "1010101011000000");
}

