#include <vector>
#include <fstream>
#include "png_chunk.h"
#include "deflate.h"
#include "tiny_png.h"


namespace tiny_png {

    const std::uint8_t PNG_SIGNATURE[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};

    bool save_image(const std::string& filePath,
        const std::int32_t width,
        const std::int32_t height,
        const std::uint8_t* data,
        const ColorType colorType) {
        std::vector<std::uint8_t> buffer;
        for (const auto byte : PNG_SIGNATURE) {
            buffer.push_back(byte);
        }
        addHeaderChunk(buffer, width, height, colorType);
        std::int32_t bytesPerPixel = 4;
        switch (colorType) {
            case ColorType::RGBA: bytesPerPixel = 4; break;
            default: break;
        }
        std::vector<std::uint8_t> filteredData(static_cast<std::size_t>(height) * (width * bytesPerPixel + 1));
        for (std::size_t i = 0, pos = 0; i < static_cast<std::size_t>(height); ++i) {
            filteredData[pos++] = 1;  // Always use SUB
            for (int j = 0; j < width; ++j) {
                for (int k = 0; k < bytesPerPixel; ++k) {
                    const auto currentByte = data[(i * width + j) * bytesPerPixel + k];
                    if (j == 0) {
                        filteredData[pos++] = currentByte;
                    } else {
                        const auto prevByte = data[(i * width + (j - 1)) * bytesPerPixel + k];
                        filteredData[pos++] = currentByte - prevByte;
                    }
                }
            }
        }
        const auto compressed = deflate_encode(filteredData.data(), filteredData.size());
        addDataChunk(buffer, compressed);
        addEndChunk(buffer);

        std::ofstream fs(filePath, std::ios::out | std::ios::binary);
        fs.write(reinterpret_cast<const std::ostream::char_type*>(buffer.data()), static_cast<std::streamsize>(buffer.size()));
        fs.close();
        return true;
    }
}
