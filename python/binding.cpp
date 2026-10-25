#include <stdexcept>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "tiny_png/tiny_png.h"
using namespace tiny_png;

namespace py = pybind11;

static bool saveImage(const std::string& filePath,
                      const std::uint32_t width,
                      const std::uint32_t height,
                      const py::buffer& data,
                      const ColorType colorType) {
    const py::buffer_info info = data.request();
    if (info.itemsize != 1) {
        throw py::value_error("data must be a buffer of uint8 values");
    }
    const auto expected = static_cast<std::size_t>(width) * height * getBytesPerPixel(colorType);
    if (static_cast<std::size_t>(info.size) != expected) {
        throw py::value_error("data size mismatch: expected " + std::to_string(expected) +
                              " bytes, got " + std::to_string(info.size));
    }
    return save_image(filePath, width, height, std::span<const std::uint8_t>(static_cast<const std::uint8_t*>(info.ptr), static_cast<std::size_t>(info.size)), colorType);
}

static PNGData loadImage(const std::string& filePath) {
    try {
        return load_image(filePath);
    } catch (const std::exception& e) {
        PNGData imageData;
        imageData.error = std::string("Failed to load image: ") + e.what();
        return imageData;
    }
}

PYBIND11_MODULE(_core, m, py::mod_gil_not_used()) {
    m.doc() = "A lightweight, minimal PNG library.";

    py::enum_<ColorType>(m, "ColorType")
        .value("GRAYSCALE", ColorType::Grayscale)
        .value("RGB", ColorType::RGB)
        .value("GRAYSCALE_ALPHA", ColorType::GrayscaleAlpha)
        .value("RGBA", ColorType::RGBA);

    py::class_<PNGData>(m, "PNGData")
        .def_readonly("width", &PNGData::width)
        .def_readonly("height", &PNGData::height)
        .def_readonly("color_type", &PNGData::colorType)
        .def_property_readonly("buffer", [](const PNGData& self) {
            return py::bytes(reinterpret_cast<const char*>(self.buffer.data()), self.buffer.size());
        })
        .def_readonly("error", &PNGData::error)
        .def("__repr__", [](const PNGData& self) {
            return "<PNGData width=" + std::to_string(self.width) +
                   " height=" + std::to_string(self.height) +
                   " color_type=" + std::to_string(static_cast<int>(self.colorType)) +
                   " error='" + self.error + "'>";
        });

    m.def("save_image", &saveImage,
          py::arg("file_path"), py::arg("width"), py::arg("height"), py::arg("data"),
          py::arg("color_type") = ColorType::RGBA,
          R"pbdoc(
Save raw pixels as a PNG file.

Args:
    file_path: The output file path.
    width: Image width in pixels.
    height: Image height in pixels.
    data: Row-major pixel bytes (bytes, bytearray or any uint8 buffer) of size
          width * height * channels.
    color_type: The color type of the pixels (default: ColorType.RGBA).

Returns:
    True if the image is saved.

Raises:
    ValueError: If the size of data does not match the image size.
)pbdoc");

    m.def("load_image", &loadImage,
          py::arg("file_path"),
          R"pbdoc(
Load a PNG file.

Args:
    file_path: The input file path.

Returns:
    A PNGData object. If loading fails, its error field is non-empty.
)pbdoc");
}
