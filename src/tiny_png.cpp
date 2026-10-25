#include <vector>
#include <fstream>
#include "tiny_png/png_chunk.h"
#include "tiny_png/deflate.h"
#include "tiny_png/tiny_png.h"


namespace tiny_png {

    const std::uint8_t PNG_SIGNATURE[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};

    PNGData::PNGData() : width(0), height(0), colorType(ColorType::RGBA) {
    }

    static std::int32_t getBytesPerPixel(const ColorType colorType) {
        switch (colorType) {
            case ColorType::Grayscale: return 1;
            case ColorType::RGB: return 3;
            case ColorType::GrayscaleAlpha: return 2;
            case ColorType::RGBA: return 4;
        }
        return 4;
    }

    bool save_image(const std::string& filePath,
                    const std::uint32_t width,
                    const std::uint32_t height,
                    const std::uint8_t* data,
                    const ColorType colorType) {
        std::vector<std::uint8_t> buffer;
        for (const auto byte : PNG_SIGNATURE) {
            buffer.push_back(byte);
        }
        addHeaderChunk(buffer, width, height, colorType);
        const std::int32_t bytesPerPixel = getBytesPerPixel(colorType);
        std::vector<std::uint8_t> filteredData(static_cast<std::size_t>(height) * (width * bytesPerPixel + 1));
        for (std::size_t i = 0, pos = 0; i < static_cast<std::size_t>(height); ++i) {
            filteredData[pos++] = 1;  // Always use SUB
            for (std::size_t j = 0; j < width; ++j) {
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

    PNGData load_image(const std::string& filePath) {
        PNGData imageData;

        std::ifstream fs(filePath, std::ios::binary);
        const std::vector<std::uint8_t> buffer{std::istreambuf_iterator(fs), {}};
        fs.close();

        std::size_t pos = 0;
        for (const auto expected : PNG_SIGNATURE) {
            if (buffer.at(pos++) != expected) {
                imageData.error = "Wrong PNG signature";
                return imageData;
            }
        }

        std::vector<std::uint8_t> compressed;
        while (true) {
            if (auto [size, chunkType] = readChunkInfo(buffer, pos); chunkType == ChunkType::I_HEADER) {
                const auto [width, height, bitDepth, colorType, compressionMethod, filterMethod, interlaceMethod] = readHeaderChunk(buffer, pos);
                imageData.width = width;
                imageData.height = height;
                if (bitDepth != 8) {
                    imageData.error = "Unsupported bit depth: " + std::to_string(bitDepth);
                    return imageData;
                }
                switch (colorType) {
                    case 0: imageData.colorType = ColorType::Grayscale; break;
                    case 2: imageData.colorType = ColorType::RGB; break;
                    case 4: imageData.colorType = ColorType::GrayscaleAlpha; break;
                    case 6: imageData.colorType = ColorType::RGBA; break;
                    default:
                        imageData.error = "Unsupported color type: " + std::to_string(colorType);
                        return imageData;
                }
                if (compressionMethod != 0) {
                    imageData.error = "Unsupported compression method: " + std::to_string(compressionMethod);
                    return imageData;
                }
                if (filterMethod != 0) {
                    imageData.error = "Unsupported filter method: " + std::to_string(filterMethod);
                    return imageData;
                }
                if (interlaceMethod != 0) {
                    imageData.error = "Unsupported interlace method: " + std::to_string(interlaceMethod);
                    return imageData;
                }
            } else if (chunkType == ChunkType::I_DATA) {
                if (pos + size >= buffer.size()) {
                    imageData.error = "Unexpected end of file";
                    return imageData;
                }
                compressed.insert(compressed.end(), buffer.begin() + pos, buffer.begin() + pos + size);
                pos += size + 4;  // Skip CRC32
            } else if (chunkType == ChunkType::I_END) {
                break;
            } else {
                // Skip data & CRC32
                pos += size + 4;
            }
        }
        if (compressed.size() < 6) {
            imageData.error = "No image data found";
            return imageData;
        }
        auto decoded = deflate_decode(compressed);
        const std::int32_t bytesPerPixel = getBytesPerPixel(imageData.colorType);
        imageData.buffer.resize(imageData.width * imageData.height * bytesPerPixel);
        pos = 0;
        for (size_t i = 0; i < imageData.height; ++i) {
            auto filterTypeCode = decoded.at(pos++);
            FilterType filterType;
            switch (filterTypeCode) {
                case 0: filterType = FilterType::None; break;
                case 1: filterType = FilterType::Sub; break;
                case 2: filterType = FilterType::Up; break;
                case 3: filterType = FilterType::Average; break;
                case 4: filterType = FilterType::Paeth; break;
                default:
                    imageData.error = "Unsupported filter type: " + std::to_string(filterTypeCode);
                    return imageData;
            }
            for (size_t j = 0; j < imageData.width; ++j) {
                for (int k = 0; k < bytesPerPixel; ++k) {
                    auto byte = decoded.at(pos++);
                    switch (filterType) {
                        case FilterType::None:
                            break;
                        case FilterType::Sub:
                            if (j > 0) {
                                const auto prevByte = imageData.buffer[(i * imageData.width + (j - 1)) * bytesPerPixel + k];
                                byte += prevByte;
                            }
                            break;
                        case FilterType::Up:
                            if (i > 0) {
                                const auto prevByte = imageData.buffer[((i - 1) * imageData.width + j) * bytesPerPixel + k];
                                byte += prevByte;
                            }
                            break;
                        case FilterType::Average: {
                                const std::uint16_t left = j == 0 ? 0 : imageData.buffer[(i * imageData.width + (j - 1)) * bytesPerPixel + k];
                                const std::uint16_t up = i == 0 ? 0 : imageData.buffer[((i - 1) * imageData.width + j) * bytesPerPixel + k];
                                const auto average = (left + up) / 2;
                                byte += average;
                            }
                            break;
                        case FilterType::Paeth: {
                                const std::uint16_t left = j == 0 ? 0 : imageData.buffer[(i * imageData.width + (j - 1)) * bytesPerPixel + k];
                                const std::uint16_t up = i == 0 ? 0 : imageData.buffer[((i - 1) * imageData.width + j) * bytesPerPixel + k];
                                const std::uint16_t upperLeft = (i == 0 || j == 0) ? 0 : imageData.buffer[((i - 1) * imageData.width + (j - 1)) * bytesPerPixel + k];
                                const auto base = left + up - upperLeft;
                                auto ref = left;
                                if (std::abs(base - up) < std::abs(base - ref)) {
                                    ref = up;
                                }
                                if (std::abs(base - upperLeft) < std::abs(base - ref)) {
                                    ref = upperLeft;
                                }
                                byte += ref;
                            }
                            break;
                    }
                    imageData.buffer[(i * imageData.width + j) * bytesPerPixel + k] = byte;
                }
            }
        }
        return imageData;
    }
}
