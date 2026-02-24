#!/usr/bin/env python3
"""Trim transparent pixels from a PNG sprite.

Crops the image to the smallest bounding box containing non-transparent pixels.
Works with both indexed (palette) and RGBA images.

Usage: python3 tools/trim_sprite.py <input.png> [output.png]

If output is omitted, the input file is overwritten.
"""

import sys
from PIL import Image


def trim(img):
    if img.mode == "P":
        pixels = img.load()
        w, h = img.size
        min_x, min_y = w, h
        max_x, max_y = -1, -1
        for y in range(h):
            for x in range(w):
                if pixels[x, y] != 0:
                    min_x = min(min_x, x)
                    min_y = min(min_y, y)
                    max_x = max(max_x, x)
                    max_y = max(max_y, y)
        if max_x < 0:
            return img
        return img.crop((min_x, min_y, max_x + 1, max_y + 1))

    if img.mode != "RGBA":
        img = img.convert("RGBA")
    bbox = img.getbbox()
    if bbox is None:
        return img
    return img.crop(bbox)


def main():
    if len(sys.argv) < 2 or len(sys.argv) > 3:
        print(f"Usage: {sys.argv[0]} <input.png> [output.png]", file=sys.stderr)
        sys.exit(1)

    input_path = sys.argv[1]
    output_path = sys.argv[2] if len(sys.argv) == 3 else input_path

    img = Image.open(input_path)
    orig_size = img.size
    trimmed = trim(img)
    trimmed.save(output_path)

    new_size = trimmed.size
    print(f"{orig_size[0]}x{orig_size[1]} -> {new_size[0]}x{new_size[1]}")


if __name__ == "__main__":
    main()
