#include <filesystem>
#include <random>
#include <string>
#include <gtest/gtest.h>
#include "tiny_png/tiny_png.h"
using namespace tiny_png;

TEST(TinyPNG, SaveLoadChessboardImage) {
    const auto filePath = "chessboard.png";
    constexpr std::int32_t width = 200, height = 150;
    std::vector<std::uint8_t> pixels(width * height * 4);
    for (std::int32_t i = 0; i < height; ++i) {
        for (std::int32_t j = 0; j < width; ++j) {
            pixels[(i * width + j) * 4] = (i / 10) % 2 == 0 ? 255 : 0;
            pixels[(i * width + j) * 4 + 1] = (j / 10) % 2 == 0 ? 255 : 0;
            pixels[(i * width + j) * 4 + 2] = (i / 10 + j / 10) % 2 == 0 ? 255 : 0;
            pixels[(i * width + j) * 4 + 3] = (i / 50 + j / 50) % 2 == 0 ? 255 : 0;
        }
    }
    ASSERT_TRUE(save_image(filePath, width, height, pixels.data()));

    const auto image = load_image(filePath);
    ASSERT_EQ(image.error, "");
    EXPECT_EQ(image.width, width);
    EXPECT_EQ(image.height, height);
    EXPECT_EQ(image.colorType, ColorType::RGBA);
    EXPECT_EQ(image.buffer, pixels);
}

TEST(TinyPNG, RandomTest) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> size(1, 100);
    std::uniform_int_distribution<> byte(0, 255);
    for (int t = 0; t < 100; ++t) {
        const auto filePath = "tiny_png_random_test.png";
        const std::uint32_t width = size(gen), height = size(gen);
        std::vector<std::uint8_t> pixels(width * height * 4);
        for (auto& p : pixels) {
            p = static_cast<std::uint8_t>(byte(gen));
        }
        ASSERT_TRUE(save_image(filePath, width, height, pixels.data()));

        const auto image = load_image(filePath);
        ASSERT_EQ(image.error, "");
        ASSERT_EQ(image.width, width);
        ASSERT_EQ(image.height, height);
        ASSERT_EQ(image.colorType, ColorType::RGBA);
        ASSERT_EQ(image.buffer, pixels);
    }
}
