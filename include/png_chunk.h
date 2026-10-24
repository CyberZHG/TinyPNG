#ifndef TINYPNG_PNG_CHUNK_H
#define TINYPNG_PNG_CHUNK_H

#include <cstdint>
#include <vector>

namespace tiny_png {

    enum class ColorType {
        RGBA = 6
    };

    enum class FilterType {
        // None = 0,
        Sub = 1,
        // Up = 2,
    };

    void addHeaderChunk(std::vector<std::uint8_t>& buffer, std::uint32_t width, std::uint32_t height, ColorType colorType);
    void addDataChunk(std::vector<std::uint8_t>& buffer, const std::vector<std::uint8_t>& data);
    void addEndChunk(std::vector<std::uint8_t>& buffer);

}

#endif //TINYPNG_PNG_CHUNK_H