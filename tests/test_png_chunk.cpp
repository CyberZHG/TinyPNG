#include <gtest/gtest.h>
#include "tiny_png/png_chunk.h"
using namespace tiny_png;

static const std::vector<std::uint8_t> IHDR_3x2_RGBA = {
    0x00, 0x00, 0x00, 0x0D,  // Length
    'I', 'H', 'D', 'R',
    0x00, 0x00, 0x00, 0x03,  // Width
    0x00, 0x00, 0x00, 0x02,  // Height
    0x08, 0x06, 0x00, 0x00, 0x00,
    0x9D, 0x74, 0x66, 0x1A,  // CRC
};

TEST(PNGChunk, HeaderChunk) {
    std::vector<std::uint8_t> data;
    addHeaderChunk(data, 3, 2, ColorType::RGBA);
    EXPECT_EQ(data, IHDR_3x2_RGBA);
}

TEST(PNGChunk, DataChunk) {
    const std::vector<std::uint8_t> data = {0x01, 0x02, 0x03};
    std::vector<std::uint8_t> buffer;
    addDataChunk(buffer, data);
    const std::vector<std::uint8_t> expected = {
        0x00, 0x00, 0x00, 0x03,  // Length
        'I', 'D', 'A', 'T',
        0x01, 0x02, 0x03,
        0x53, 0x37, 0x17, 0xAD,  // CRC
    };
    EXPECT_EQ(buffer, expected);
}

TEST(PNGChunk, DataChunkSplit) {
    const std::vector<std::uint8_t> data(8193, 0);
    std::vector<std::uint8_t> buffer;
    addDataChunk(buffer, data);
    std::vector<std::uint8_t> expected = {0x00, 0x00, 0x20, 0x00, 'I', 'D', 'A', 'T'};
    expected.insert(expected.end(), 8192, 0);
    expected.insert(expected.end(), {0x3D, 0x0F, 0xB7, 0x4F});
    expected.insert(expected.end(), {0x00, 0x00, 0x00, 0x01, 'I', 'D', 'A', 'T', 0x00, 0x28, 0x38, 0x7D, 0xE8});
    EXPECT_EQ(buffer, expected);
}

TEST(PNGChunk, EndChunk) {
    std::vector<std::uint8_t> buffer;
    addEndChunk(buffer);
    const std::vector<std::uint8_t> expected = {
        0x00, 0x00, 0x00, 0x00,  // Length
        'I', 'E', 'N', 'D',
        0xAE, 0x42, 0x60, 0x82,  // CRC
    };
    EXPECT_EQ(buffer, expected);
}
