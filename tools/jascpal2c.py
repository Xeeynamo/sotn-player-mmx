import os
import sys
import yaml


def parse_jasc_pal(file_name):
    with open(file_name, "r") as f:
        assert f.readline().strip() == "JASC-PAL"  # signature
        assert f.readline().strip() == "0100"  # version
        color_count = int(f.readline().strip())
        assert color_count == 16 or color_count == 256
        colors = []
        for _ in range(color_count):
            r, g, b = map(int, f.readline().strip().split())
            colors.append((r, g, b))
    return colors


def convert_32bit_to_16bit_color(rgb_tuple):
    r, g, b = rgb_tuple
    return (r >> 3) | (g >> 3 << 5) | (b >> 3 << 10) | 0x8000


config = yaml.safe_load(sys.stdin.buffer)
palettes = []
for pal_file_name in config["palettes"]:
    palettes.extend(parse_jasc_pal(pal_file_name))
for i in range(0, len(palettes) >> 4):
    pal = palettes[i * 16 :]
    str = "{0,"  # first color always transparent
    for j in range(1, 15):
        str += f"0x{convert_32bit_to_16bit_color(pal[j]):04X},"
    str += f"0x{convert_32bit_to_16bit_color(pal[15]):04X}"
    str += "},"
    print(str)
