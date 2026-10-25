/**
 * PNG color types.
 */
export declare const ColorType: {
    readonly Grayscale: 0;
    readonly RGB: 2;
    readonly GrayscaleAlpha: 4;
    readonly RGBA: 6;
};

export type ColorType = (typeof ColorType)[keyof typeof ColorType];

export interface PNGData {
    width: number;
    height: number;
    colorType: ColorType;
    /** Row-major pixels of size width * height * channels. */
    buffer: Uint8Array;
    /** Empty on success. */
    error: string;
}

/**
 * Encode raw pixels into PNG file bytes.
 *
 * @param width - Image width in pixels.
 * @param height - Image height in pixels.
 * @param data - Row-major pixels of size width * height * channels.
 * @param colorType - The color type of the pixels (default: ColorType.RGBA).
 * @returns The PNG file bytes.
 *
 * @example
 * ```typescript
 * import { ColorType, encodeImage } from "tiny-png-wasm";
 *
 * const png = encodeImage(1, 1, new Uint8Array([255, 0, 0, 255]), ColorType.RGBA);
 * ```
 */
export function encodeImage(
    width: number,
    height: number,
    data: Uint8Array | Uint8ClampedArray | ArrayLike<number>,
    colorType?: ColorType,
): Uint8Array;

/**
 * Decode PNG file bytes into raw pixels.
 *
 * @param data - The PNG file bytes.
 * @returns The decoded image. If decoding fails, `error` is non-empty.
 */
export function decodeImage(data: Uint8Array | ArrayBuffer): PNGData;
