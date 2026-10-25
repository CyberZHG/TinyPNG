import { describe, it } from "mocha";
import assert from "node:assert";
import fs from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";
import { ColorType, decodeImage, encodeImage } from "../index.js";

const TEST_DATA_DIR = path.join(path.dirname(fileURLToPath(import.meta.url)), "..", "..", "tests", "data");

const BYTES_PER_PIXEL = {
    [ColorType.Grayscale]: 1,
    [ColorType.RGB]: 3,
    [ColorType.GrayscaleAlpha]: 2,
    [ColorType.RGBA]: 4,
};

describe("Encode & Decode", () => {
    it("should round trip a chessboard image", () => {
        const width = 200, height = 150;
        const pixels = new Uint8Array(width * height * 4);
        for (let i = 0; i < height; ++i) {
            for (let j = 0; j < width; ++j) {
                const pos = (i * width + j) * 4;
                pixels[pos] = Math.floor(i / 10) % 2 === 0 ? 255 : 0;
                pixels[pos + 1] = Math.floor(j / 10) % 2 === 0 ? 255 : 0;
                pixels[pos + 2] = (Math.floor(i / 10) + Math.floor(j / 10)) % 2 === 0 ? 255 : 0;
                pixels[pos + 3] = (Math.floor(i / 50) + Math.floor(j / 50)) % 2 === 0 ? 255 : 0;
            }
        }
        const encoded = encodeImage(width, height, pixels);
        assert.deepStrictEqual(Array.from(encoded.subarray(0, 8)), [0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A]);

        const image = decodeImage(encoded);
        assert.strictEqual(image.error, "");
        assert.strictEqual(image.width, width);
        assert.strictEqual(image.height, height);
        assert.strictEqual(image.colorType, ColorType.RGBA);
        assert.deepStrictEqual(image.buffer, pixels);
    });

    it("should round trip random images of all color types", () => {
        for (const colorType of Object.values(ColorType)) {
            for (let t = 0; t < 10; ++t) {
                const width = 1 + Math.floor(Math.random() * 50);
                const height = 1 + Math.floor(Math.random() * 50);
                const pixels = new Uint8Array(width * height * BYTES_PER_PIXEL[colorType]);
                for (let i = 0; i < pixels.length; ++i) {
                    pixels[i] = Math.floor(Math.random() * 256);
                }
                const image = decodeImage(encodeImage(width, height, pixels, colorType));
                assert.strictEqual(image.error, "");
                assert.strictEqual(image.width, width);
                assert.strictEqual(image.height, height);
                assert.strictEqual(image.colorType, colorType);
                assert.deepStrictEqual(image.buffer, pixels);
            }
        }
    });

    it("should accept plain arrays and ArrayBuffers", () => {
        const encoded = encodeImage(2, 1, [10, 20], ColorType.Grayscale);
        const image = decodeImage(encoded.buffer.slice(encoded.byteOffset, encoded.byteOffset + encoded.byteLength));
        assert.strictEqual(image.error, "");
        assert.deepStrictEqual(Array.from(image.buffer), [10, 20]);
    });

    it("should throw on size mismatch", () => {
        assert.throws(() => encodeImage(2, 2, new Uint8Array(15)), /size mismatch/);
        assert.throws(() => encodeImage(2, 2, new Uint8Array(16), ColorType.RGB), /size mismatch/);
    });
});

describe("Decode External Images", () => {
    const cases = [
        ["gray_none", ColorType.Grayscale],
        ["gray_alpha_sub", ColorType.GrayscaleAlpha],
        ["rgb_up", ColorType.RGB],
        ["rgba_average", ColorType.RGBA],
        ["rgba_paeth", ColorType.RGBA],
        ["rgba_sub_dynamic", ColorType.RGBA],
        ["rgba_sub_stored", ColorType.RGBA],
    ];
    for (const [name, colorType] of cases) {
        it(`should decode ${name}`, () => {
            const image = decodeImage(fs.readFileSync(path.join(TEST_DATA_DIR, `${name}.png`)));
            assert.strictEqual(image.error, "");
            assert.ok(image.width > 0 && image.height > 0);
            assert.strictEqual(image.colorType, colorType);
            assert.strictEqual(image.buffer.length, image.width * image.height * BYTES_PER_PIXEL[colorType]);
        });
    }

    it("should report unsupported images", () => {
        assert.strictEqual(decodeImage(fs.readFileSync(path.join(TEST_DATA_DIR, "error_signature.png"))).error, "Wrong PNG signature");
        assert.strictEqual(decodeImage(fs.readFileSync(path.join(TEST_DATA_DIR, "error_bit_depth_16.png"))).error, "Unsupported bit depth: 16");
    });

    it("should report malformed data", () => {
        assert.notStrictEqual(decodeImage(new Uint8Array(0)).error, "");
        assert.notStrictEqual(decodeImage(new Uint8Array([0x89, 0x50, 0x4E, 0x47])).error, "");
    });
});
