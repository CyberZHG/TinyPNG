#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <gtest/gtest.h>
#include "deflate.h"
#include "tiny_png.h"
using namespace tiny_png;

TEST(TinyPNG, SaveImage) {
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
    ASSERT_TRUE(save_image("save_image.png", width, height, pixels.data()));
}
