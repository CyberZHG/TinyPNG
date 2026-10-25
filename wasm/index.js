import createModule from "./wasm/TinyPNGWASM.js";

const TinyPNGWASM = await createModule();

/**
 * PNG color types.
 * @enum {number}
 */
export const ColorType = Object.freeze({
    Grayscale: 0,
    RGB: 2,
    GrayscaleAlpha: 4,
    RGBA: 6,
});

const BYTES_PER_PIXEL = {
    [ColorType.Grayscale]: 1,
    [ColorType.RGB]: 3,
    [ColorType.GrayscaleAlpha]: 2,
    [ColorType.RGBA]: 4,
};

/**
 * Encode raw pixels into PNG file bytes.
 *
 * @param {number} width - Image width in pixels.
 * @param {number} height - Image height in pixels.
 * @param {Uint8Array|Uint8ClampedArray|number[]} data - Row-major pixels of size width * height * channels.
 * @param {ColorType} [colorType=ColorType.RGBA] - The color type of the pixels.
 * @returns {Uint8Array} The PNG file bytes.
 */
export function encodeImage(width, height, data, colorType = ColorType.RGBA) {
    const bytesPerPixel = BYTES_PER_PIXEL[colorType];
    if (bytesPerPixel === undefined) {
        throw new Error(`Unsupported color type: ${colorType}`);
    }
    const expected = width * height * bytesPerPixel;
    if (data.length !== expected) {
        throw new Error(`data size mismatch: expected ${expected} bytes, got ${data.length}`);
    }
    return TinyPNGWASM._encodeImage(width, height, data, colorType);
}

/**
 * Decode PNG file bytes into raw pixels.
 *
 * @param {Uint8Array|ArrayBuffer} data - The PNG file bytes.
 * @returns {{width: number, height: number, colorType: ColorType, buffer: Uint8Array, error: string}}
 *     The decoded image. If decoding fails, `error` is non-empty.
 */
export function decodeImage(data) {
    if (data instanceof ArrayBuffer) {
        data = new Uint8Array(data);
    }
    return TinyPNGWASM._decodeImage(data);
}
