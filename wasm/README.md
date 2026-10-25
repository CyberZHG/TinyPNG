# TinyPNG

A lightweight, minimal PNG library, powered by WebAssembly.

## Installation

```bash
npm install tiny-png
```

## Usage

```javascript
import { ColorType, decodeImage, encodeImage } from "tiny-png-wasm";

const width = 4, height = 2;
const pixels = new Uint8Array(width * height * 4).fill(255);  // White RGBA pixels
const png = encodeImage(width, height, pixels);  // Uint8Array of PNG file bytes

const image = decodeImage(png);
if (image.error) {
    console.error(image.error);
} else {
    console.log(image.width, image.height, image.colorType === ColorType.RGBA);  // 4 2 true
}
```

In Node.js, read and write files with `fs`:

```javascript
import fs from "node:fs";

fs.writeFileSync("white.png", encodeImage(width, height, pixels));
const loaded = decodeImage(fs.readFileSync("white.png"));
```

## API Reference

### `ColorType`

`Grayscale` (1 byte per pixel), `GrayscaleAlpha` (2), `RGB` (3), `RGBA` (4).

### `encodeImage(width, height, data, colorType = ColorType.RGBA): Uint8Array`

Encodes row-major 8-bit pixels (`Uint8Array`, `Uint8ClampedArray` or `number[]` of size `width * height * channels`) into PNG file bytes.

**Throws:**
- `Error` if the size of `data` does not match the image size.

### `decodeImage(data: Uint8Array | ArrayBuffer): PNGData`

Decodes PNG file bytes into `{ width, height, colorType, buffer, error }`, where `buffer` is a `Uint8Array` of pixels and `error` is empty on success.
