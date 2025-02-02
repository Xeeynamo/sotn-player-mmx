import ctypes
import os
import sys

subchar81_dict = {
    0x44: 0x2E,  # '.'
    0x43: 0x2C,  # ','
    0x48: 0x3F,  # '?'
    0x49: 0x21,  # '!'
    0x66: 0x27,  # '''
    0x68: 0x22,  # '"'
    0x69: 0x28,  # '('
    0x6A: 0x29,  # ')'
    0x6D: 0x5B,  # '['
    0x6E: 0x5D,  # ']'
    0x7B: 0x2B,  # '+'
    0x7C: 0x2D,  # '-'
    0x93: 0x25,  # '%'
}

def from_s32(num):
    return num.to_bytes(4, byteorder="little", signed=True)


def to_s32(data: bytearray):
    return int.from_bytes(data[:4], byteorder="little", signed=True)


def from_u32(num):
    return bytes([(num >> i) & 0xFF for i in range(0, 32, 8)])


def to_u32(data: bytearray):
    return (data[0] << 0) | (data[1] << 8) | (data[2] << 16) | (data[3] << 24)


def from_16(num):
    return bytes([num & 0xFF, (num >> 8) & 0xFF])


def to_s16(data: bytearray):
    return ctypes.c_int16(data[0] | (data[1] << 8)).value


def to_u16(data: bytearray):
    return data[0] | (data[1] << 8)


def from_s8(num):
    if num < 0:
        num += 256
    return bytes([num])


def to_s8(data: bytearray):
    raw = data[0]
    if raw >= 128:
        return raw - 256
    return raw


def from_u8(num):
    return bytes([num])


def to_u8(data: bytearray):
    return data[0]


def from_bool(val):
    return bytes([int(val)])


def to_bool(data):
    return bool(data[0])


def from_ptr_str(ptr_str):
    return from_u32(int(ptr_str[2:], 16))


def to_ptr_str(data):
    return f"0x{to_u32(data):08X}"
