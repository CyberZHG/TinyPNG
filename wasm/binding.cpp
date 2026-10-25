#include <cstdio>
#include <fstream>
#include <exception>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include "tiny_png/tiny_png.h"
using namespace emscripten;
using namespace tiny_png;

static const std::string TEMP_FILE_PATH = "/tmp/tiny_png_temp.png";

static std::vector<std::uint8_t> toVector(const val& data) {
    std::vector<std::uint8_t> buffer(data["length"].as<std::size_t>());
    val(typed_memory_view(buffer.size(), buffer.data())).call<void>("set", data);
    return buffer;
}

static val toUint8Array(const std::vector<std::uint8_t>& buffer) {
    return val::global("Uint8Array").new_(typed_memory_view(buffer.size(), buffer.data()));
}

static val encodeImage(const std::uint32_t width, const std::uint32_t height, const val& data, const int colorTypeCode) {
    const auto colorType = static_cast<ColorType>(colorTypeCode);
    const auto pixels = toVector(data);
    const auto expected = static_cast<std::size_t>(width) * height * getBytesPerPixel(colorType);
    if (pixels.size() != expected) {
        return val::null();
    }
    save_image(TEMP_FILE_PATH, width, height, pixels.data(), colorType);
    std::ifstream fs(TEMP_FILE_PATH, std::ios::binary);
    const std::vector<std::uint8_t> encoded{std::istreambuf_iterator(fs), {}};
    fs.close();
    std::remove(TEMP_FILE_PATH.c_str());
    return toUint8Array(encoded);
}

static val decodeImage(const val& data) {
    const auto encoded = toVector(data);
    std::ofstream fs(TEMP_FILE_PATH, std::ios::out | std::ios::binary);
    fs.write(reinterpret_cast<const std::ostream::char_type*>(encoded.data()), static_cast<std::streamsize>(encoded.size()));
    fs.close();
    PNGData imageData;
    try {
        imageData = load_image(TEMP_FILE_PATH);
    } catch (const std::exception& e) {
        imageData = PNGData();
        imageData.error = std::string("Failed to load image: ") + e.what();
    }
    std::remove(TEMP_FILE_PATH.c_str());
    val result = val::object();
    result.set("width", imageData.width);
    result.set("height", imageData.height);
    result.set("colorType", static_cast<int>(imageData.colorType));
    result.set("buffer", toUint8Array(imageData.buffer));
    result.set("error", imageData.error);
    return result;
}

EMSCRIPTEN_BINDINGS(TinyPNG) {
    function("_encodeImage", &encodeImage);
    function("_decodeImage", &decodeImage);
}
