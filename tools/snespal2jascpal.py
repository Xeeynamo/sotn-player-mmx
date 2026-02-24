import sys
import re


def bgr555_to_rgb888(hex_str):
    val = int(hex_str, 16)
    # Extract 5-bit channels
    r = (val >> 0) & 0x1F
    g = (val >> 5) & 0x1F
    b = (val >> 10) & 0x1F
    # Scale to 8-bit (bit replication)
    r8 = (r << 3) | (r >> 2)
    g8 = (g << 3) | (g >> 2)
    b8 = (b << 3) | (b >> 2)
    return f"{r8} {g8} {b8}"


# 1. Grab every $XXXX in the piped input
raw_data = sys.stdin.read()
hex_values = re.findall(r"\$([0-9A-Fa-f]{4})", raw_data)

if hex_values:
    # 2. Print JASC-PAL Header
    print("JASC-PAL")
    print("0100")
    print("16")

    # 3. Print exactly 16 colors
    for i in range(16):
        if i < len(hex_values):
            print(bgr555_to_rgb888(hex_values[i]))
        else:
            print("0 0 0")
