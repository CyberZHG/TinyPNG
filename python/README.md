# TinyPNG

A lightweight, minimal PNG library.

## Installation

```bash
pip install lib-tiny-png
```

## Usage

```python
from tiny_png import ColorType, load_image, save_image

width, height = 4, 2
pixels = bytes([255, 0, 0, 255] * (width * height))  # Red RGBA pixels
save_image("red.png", width, height, pixels)

image = load_image("red.png")
if image.error:
    print(image.error)
else:
    print(image.width, image.height, image.color_type)  # 4 2 ColorType.RGBA
    print(image.buffer == pixels)  # True

gray = bytes(range(16))
save_image("gray.png", 4, 4, gray, ColorType.GRAYSCALE)
```

## API Reference

### `ColorType`

`GRAYSCALE` (1 byte per pixel), `GRAYSCALE_ALPHA` (2), `RGB` (3), `RGBA` (4).

### `save_image(file_path, width, height, data, color_type=ColorType.RGBA)`

Saves row-major 8-bit pixels as a PNG file.

**Parameters:**
- `file_path` (`str`) - The output file path.
- `width`, `height` (`int`) - The image size in pixels.
- `data` (`bytes`, `bytearray` or any uint8 buffer) - Pixels of size `width * height * channels`.
- `color_type` (`ColorType`, optional) - The color type of the pixels.

**Returns:**
- `bool` - `True` if the image is saved.

**Raises:**
- `ValueError` - If the size of `data` does not match the image size.

### `load_image(file_path)`

Loads a PNG file.

**Returns:**
- `PNGData` - With fields `width`, `height`, `color_type`, `buffer` (`bytes`) and `error` (`str`, empty on success).
