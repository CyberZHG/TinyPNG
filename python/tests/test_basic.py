import os
import random

import pytest
from tiny_png import ColorType, load_image, save_image

TEST_DATA_DIR = os.path.join(os.path.dirname(__file__), "..", "..", "tests", "data")

BYTES_PER_PIXEL = {
    ColorType.GRAYSCALE: 1,
    ColorType.RGB: 3,
    ColorType.GRAYSCALE_ALPHA: 2,
    ColorType.RGBA: 4,
}


def test_save_load_chessboard(tmp_path):
    width, height = 200, 150
    pixels = bytearray(width * height * 4)
    for i in range(height):
        for j in range(width):
            pos = (i * width + j) * 4
            pixels[pos] = 255 if (i // 10) % 2 == 0 else 0
            pixels[pos + 1] = 255 if (j // 10) % 2 == 0 else 0
            pixels[pos + 2] = 255 if (i // 10 + j // 10) % 2 == 0 else 0
            pixels[pos + 3] = 255 if (i // 50 + j // 50) % 2 == 0 else 0
    file_path = str(tmp_path / "chessboard.png")
    assert save_image(file_path, width, height, pixels)

    image = load_image(file_path)
    assert image.error == ""
    assert image.width == width
    assert image.height == height
    assert image.color_type == ColorType.RGBA
    assert image.buffer == bytes(pixels)


@pytest.mark.parametrize("color_type", list(BYTES_PER_PIXEL.keys()))
def test_random(tmp_path, color_type):
    rng = random.Random(42)
    file_path = str(tmp_path / "random.png")
    for _ in range(10):
        width, height = rng.randint(1, 50), rng.randint(1, 50)
        pixels = bytes(rng.randrange(256) for _ in range(width * height * BYTES_PER_PIXEL[color_type]))
        assert save_image(file_path, width, height, pixels, color_type)

        image = load_image(file_path)
        assert image.error == ""
        assert image.width == width
        assert image.height == height
        assert image.color_type == color_type
        assert image.buffer == pixels


@pytest.mark.parametrize(
    "name, color_type",
    [
        ("gray_none", ColorType.GRAYSCALE),
        ("gray_alpha_sub", ColorType.GRAYSCALE_ALPHA),
        ("rgb_up", ColorType.RGB),
        ("rgba_average", ColorType.RGBA),
        ("rgba_paeth", ColorType.RGBA),
        ("rgba_sub_dynamic", ColorType.RGBA),
        ("rgba_sub_stored", ColorType.RGBA),
    ],
)
def test_load_external(name, color_type):
    image = load_image(os.path.join(TEST_DATA_DIR, name + ".png"))
    assert image.error == ""
    assert image.width > 0
    assert image.height > 0
    assert image.color_type == color_type
    assert len(image.buffer) == image.width * image.height * BYTES_PER_PIXEL[color_type]


def test_load_unsupported():
    image = load_image(os.path.join(TEST_DATA_DIR, "error_signature.png"))
    assert image.error == "Wrong PNG signature"
    image = load_image(os.path.join(TEST_DATA_DIR, "error_bit_depth_16.png"))
    assert image.error == "Unsupported bit depth: 16"


def test_load_missing_file(tmp_path):
    image = load_image(str(tmp_path / "not_exist.png"))
    assert image.error != ""


def test_save_size_mismatch(tmp_path):
    with pytest.raises(ValueError):
        save_image(str(tmp_path / "mismatch.png"), 2, 2, bytes(15))
    with pytest.raises(ValueError):
        save_image(str(tmp_path / "mismatch.png"), 2, 2, bytes(16), ColorType.RGB)
