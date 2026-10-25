# TinyPNG Library

A lightweight, minimal PNG library.

## Scope

- Save: images with basic compression.
- Load: only guaranteed to read files written by this library.

## Usage

TODO

## Specification

- Color type: Grayscale, Grayscale + Alpha, RGB, RGBA
- Bit depth: only 8
- Filter type: only use SUB
- Compression: only use fixed Huffman compression

## References

- PNG Signature & Chunks: https://www.libpng.org/pub/png/book/chapter08.html
- PNG Compression & Filtering: https://www.libpng.org/pub/png/book/chapter09.html
- DEFLATE: https://www.rfc-editor.org/rfc/rfc1951.pdf
