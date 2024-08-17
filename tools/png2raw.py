#!/usr/bin/python3

import png
import sys


def decode_png(fin, fout):
    img = png.Reader(file=fin)
    width, height, rows, info = img.read()
    if width != 128 or height != 128:
        return f"image must be 128x128 but it was {width}x{height}"
    if info["planes"] != 1 or (info["bitdepth"] != 8 and info["bitdepth"] != 4):
        return f"image must be a 4bpp 16-colors indexed image"
    bytes_per_row = int(width / 2)
    for row in rows:
        packed_row = bytearray()
        for x in range(0, bytes_per_row):
            packed_row.append((row[x * 2 + 0] & 0xF) | ((row[x * 2 + 1] & 0xF) << 4))
        fout.write(packed_row)
    return None


err = decode_png(sys.stdin.buffer, sys.stdout.buffer)
if err is not None:
    print(err, file=sys.stderr)
