#include <algorithm>
#include "tiny_png/png_chunk.h"

namespace tiny_png {

    constexpr std::size_t DATA_CHUNK_BUFFER_SIZE = 8192;

    constexpr std::uint32_t I_HEADER_TYPE = (static_cast<std::uint32_t>('I') << 24) |
        (static_cast<std::uint32_t>('H') << 16) |
            (static_cast<std::uint32_t>('D') << 8) | 'R';

    constexpr std::uint32_t I_DATA_TYPE = (static_cast<std::uint32_t>('I') << 24) |
        (static_cast<std::uint32_t>('D') << 16) |
            (static_cast<std::uint32_t>('A') << 8) | 'T';

    constexpr std::uint32_t I_END_TYPE = (static_cast<std::uint32_t>('I') << 24) |
        (static_cast<std::uint32_t>('E') << 16) |
        (static_cast<std::uint32_t>('N') << 8) | 'D';

    static std::uint32_t crc32(const std::vector<std::uint8_t>& buffer, const std::size_t start = 0) {
        std::uint32_t crc = 0xFFFFFFFF;
        for (auto i = start; i < buffer.size(); i++) {
            crc ^= buffer[i];
            for (int bit = 0; bit < 8; ++bit) {
                if (crc & 1) {
                    crc = (crc >> 1) ^ 0xEDB88320u;
                } else {
                    crc >>= 1;
                }
            }
        }
        return crc ^ 0xFFFFFFFFu;
    }

    static void addInt32(std::vector<std::uint8_t>& buffer, const std::uint32_t value) {
        buffer.push_back(value >> 24);
        buffer.push_back(value >> 16);
        buffer.push_back(value >> 8);
        buffer.push_back(value);
    }

    static void addCrc32(std::vector<std::uint8_t>& buffer, const std::size_t start) {
        addInt32(buffer, crc32(buffer, start));
    }

    void addHeaderChunk(std::vector<std::uint8_t>& buffer,
        const std::uint32_t width,
        const std::uint32_t height,
        ColorType colorType) {
        addInt32(buffer, 13);  // Length: 13
        const auto start = buffer.size();
        addInt32(buffer, I_HEADER_TYPE);

        addInt32(buffer, width);
        addInt32(buffer, height);
        buffer.push_back(8);  // The bit depth
        buffer.push_back(static_cast<std::uint8_t>(colorType));
        buffer.push_back(0);  // The compression method: deflate
        buffer.push_back(0);  // The filter method: adaptive filtering
        buffer.push_back(0);  // The interlace method: no interlace

        addCrc32(buffer, start);
    }

    void addDataChunk(std::vector<std::uint8_t>& buffer, const std::vector<std::uint8_t>& data) {
        for (size_t i = 0; i < data.size(); i += DATA_CHUNK_BUFFER_SIZE) {
            const auto dataSize = std::min(data.size() - i, DATA_CHUNK_BUFFER_SIZE);
            addInt32(buffer, dataSize);
            const auto start = buffer.size();
            addInt32(buffer, I_DATA_TYPE);

            buffer.insert(buffer.end(), data.begin() + i, data.begin() + i + dataSize);

            addCrc32(buffer, start);
        }
    }

    void addEndChunk(std::vector<std::uint8_t>& buffer) {
        addInt32(buffer, 0);
        const auto start = buffer.size();
        addInt32(buffer, I_END_TYPE);

        addCrc32(buffer, start);
    }

    static std::uint32_t readInt32(const std::vector<std::uint8_t>& buffer, std::size_t& pos) {
        std::uint32_t value = 0;
        for (std::size_t i = 0; i < 4; i++) {
            value = (value << 8) | buffer.at(pos++);
        }
        return value;
    }

    std::pair<std::uint32_t, ChunkType> readChunkInfo(const std::vector<std::uint8_t>& buffer, std::size_t& pos) {
        const auto size = readInt32(buffer, pos);
        const auto type = readInt32(buffer, pos);
        auto chunkType = ChunkType::OTHER;
        switch (type) {
            case I_HEADER_TYPE: chunkType = ChunkType::I_HEADER; break;
            case I_DATA_TYPE: chunkType = ChunkType::I_DATA; break;
            case I_END_TYPE: chunkType = ChunkType::I_END; break;
            default: break;
        }
        return {size, chunkType};
    }

    Header readHeaderChunk(const std::vector<std::uint8_t>& buffer, std::size_t& pos) {
        Header header;
        header.width = readInt32(buffer, pos);
        header.height = readInt32(buffer, pos);
        header.bitDepth = buffer.at(pos++);
        header.colorType = buffer.at(pos++);
        header.compressionMethod = buffer.at(pos++);
        header.filterMethod = buffer.at(pos++);
        header.interlaceMethod = buffer.at(pos++);
        pos += 4;  // Skip CRC32
        return header;
    }
}
