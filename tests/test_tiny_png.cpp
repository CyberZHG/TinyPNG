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

struct ColorTypeInfo {
    ColorType colorType;
    std::uint32_t bytesPerPixel;
    std::string name;
};

static const std::vector<ColorTypeInfo> COLOR_TYPES = {
    {ColorType::Grayscale, 1, "gray"},
    {ColorType::RGB, 3, "rgb"},
    {ColorType::GrayscaleAlpha, 2, "gray_alpha"},
    {ColorType::RGBA, 4, "rgba"},
};

TEST(TinyPNG, RandomTest) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> size(1, 100);
    std::uniform_int_distribution<> byte(0, 255);
    for (int t = 0; t < 100; ++t) {
        const auto filePath = "tiny_png_random_test.png";
        const auto& [colorType, bytesPerPixel, name] = COLOR_TYPES[t % COLOR_TYPES.size()];
        const std::uint32_t width = size(gen), height = size(gen);
        std::vector<std::uint8_t> pixels(width * height * bytesPerPixel);
        for (auto& p : pixels) {
            p = static_cast<std::uint8_t>(byte(gen));
        }
        ASSERT_TRUE(save_image(filePath, width, height, pixels.data(), colorType));

        const auto image = load_image(filePath);
        ASSERT_EQ(image.error, "") << name;
        ASSERT_EQ(image.width, width) << name;
        ASSERT_EQ(image.height, height) << name;
        ASSERT_EQ(image.colorType, colorType) << name;
        ASSERT_EQ(image.buffer, pixels) << name;
    }
}

TEST(TinyPNG, LoadSaveReloadImages) {
    const std::string dataDir = TINY_PNG_TEST_DATA_DIR;
    for (const auto& [colorType, bytesPerPixel, name] : COLOR_TYPES) {
        std::vector<std::uint8_t> noneFilterPixels;
        std::vector<std::string> variants = {"none", "sub", "up"};
        if (colorType == ColorType::RGBA) {
            variants.emplace_back("sub_stored");
            variants.emplace_back("sub_dynamic");
            variants.emplace_back("average");
            variants.emplace_back("paeth");
        }
        for (const auto& variant : variants) {
            const auto fileName = name + "_" + variant + ".png";
            const auto image = load_image(dataDir + "/" + fileName);
            ASSERT_EQ(image.error, "") << fileName;
            ASSERT_EQ(image.colorType, colorType) << fileName;
            ASSERT_EQ(image.buffer.size(), image.width * image.height * bytesPerPixel) << fileName;

            if (noneFilterPixels.empty()) {
                noneFilterPixels = image.buffer;
            } else {
                ASSERT_EQ(image.buffer, noneFilterPixels) << fileName;
            }

            const auto filePath = "tiny_png_reload_test.png";
            ASSERT_TRUE(save_image(filePath, image.width, image.height, image.buffer.data(), image.colorType)) << fileName;
            const auto reloaded = load_image(filePath);
            ASSERT_EQ(reloaded.error, "") << fileName;
            ASSERT_EQ(reloaded.width, image.width) << fileName;
            ASSERT_EQ(reloaded.height, image.height) << fileName;
            ASSERT_EQ(reloaded.colorType, image.colorType) << fileName;
            ASSERT_EQ(reloaded.buffer, image.buffer) << fileName;
        }
    }
}

TEST(TinyPNG, LoadUnsupportedImages) {
    const std::string dataDir = TINY_PNG_TEST_DATA_DIR;
    for (const std::string name : {
         "signature",
         "bit_depth_16",
         "color_type_palette",
         "compression_method",
         "filter_method",
         "interlace",
         "no_idat",
         "filter_type_invalid",
     }) {
        const auto fileName = "error_" + name + ".png";
        const auto image = load_image(dataDir + "/" + fileName);
        EXPECT_NE(image.error, "") << fileName;
    }
}
