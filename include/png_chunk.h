#ifndef TINYPNG_PNG_CHUNK_H
#define TINYPNG_PNG_CHUNK_H

#include <cstdint>
#include <vector>
#include <utility>
#include "constants.h"

namespace tiny_png {

    enum class ChunkType {
        I_HEADER, I_DATA, I_END, OTHER
    };

    struct Header {
        std::uint32_t width, height;
        std::uint8_t bitDepth, colorType, compressionMethod, filterMethod, interlaceMethod;
    };

    void addHeaderChunk(std::vector<std::uint8_t>& buffer, std::uint32_t width, std::uint32_t height, ColorType colorType);
    void addDataChunk(std::vector<std::uint8_t>& buffer, const std::vector<std::uint8_t>& data);
    void addEndChunk(std::vector<std::uint8_t>& buffer);

    std::pair<std::uint32_t, ChunkType> readChunkInfo(const std::vector<std::uint8_t>& buffer, std::size_t& pos);
    Header readHeaderChunk(const std::vector<std::uint8_t>& buffer, std::size_t& pos);

}

#endif //TINYPNG_PNG_CHUNK_H