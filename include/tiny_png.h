#ifndef TINYPNG_TINY_PNG_H
#define TINYPNG_TINY_PNG_H

#include "constants.h"

namespace tiny_png {

    bool save_image(const std::string& filePath, std::int32_t width, std::int32_t height, const std::uint8_t* data, ColorType colorType = ColorType::RGBA);

}

#endif //TINYPNG_TINY_PNG_H