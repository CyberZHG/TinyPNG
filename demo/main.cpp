#include <iostream>
#include <vector>
#include "tiny_png/tiny_png.h"
using namespace tiny_png;

int main() {
    constexpr std::uint32_t width = 320, height = 200;
    std::vector<std::uint8_t> pixels(width * height * 4);
    for (std::uint32_t i = 0; i < height; ++i) {
        for (std::uint32_t j = 0; j < width; ++j) {
            pixels[(i * width + j) * 4] = (i / 20) % 2 == 0 ? 255 : 0;
            pixels[(i * width + j) * 4 + 1] = (j / 20) % 2 == 0 ? 255 : 0;
            pixels[(i * width + j) * 4 + 2] = (i / 20 + j / 20) % 2 == 0 ? 255 : 0;
            pixels[(i * width + j) * 4 + 3] = (i / 40 + j / 40) % 2 == 0 ? 255 : 0;
        }
    }
    save_image("demo.png", width, height, pixels.data());

    const auto image = load_image("demo.png");
    if (!image.error.empty()) {
        std::cerr << image.error << std::endl;
        return 1;
    }
    return image.buffer == pixels ? 0 : 1;
}
