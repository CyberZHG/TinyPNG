# TinyPNG Library

A lightweight, minimal PNG library.

## Scope

- Save: images with basic compression.
- Load: only guaranteed to read files written by this library.

## Usage

```cpp
#include "tiny_png/tiny_png.h"
using namespace tiny_png;

std::vector<std::uint8_t> pixels(width * height * 4);
save_image("output.png", width, height, pixels.data(), ColorType::RGBA);

const auto image = load_image("output.png");
if (image.error.empty()) {
    // image.width, image.height, image.colorType, image.buffer
}
```

See [demo](demo) for a complete example.

## Specification

- Color type: Grayscale, Grayscale + Alpha, RGB, RGBA
- Bit depth: only 8
- Filter type: only use SUB
- Compression: only use fixed Huffman compression

## References

- PNG Signature & Chunks: https://www.libpng.org/pub/png/book/chapter08.html
- PNG Compression & Filtering: https://www.libpng.org/pub/png/book/chapter09.html
- DEFLATE: https://www.rfc-editor.org/rfc/rfc1951.pdf
