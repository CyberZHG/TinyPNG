#ifndef TINYPNG_TINY_PNG_H
#define TINYPNG_TINY_PNG_H

#include <string>
#include <vector>
#include "constants.h"

namespace tiny_png {

    struct PNGData {
        std::size_t width;
        std::size_t height;
        ColorType colorType;
        std::vector<std::uint8_t> buffer;
        std::string error;

        PNGData();
    };

    bool save_image(const std::string& filePath, std::uint32_t width, std::uint32_t height, const std::uint8_t* data, ColorType colorType = ColorType::RGBA);
    PNGData load_image(const std::string& filePath);

}

#endif //TINYPNG_TINY_PNG_H