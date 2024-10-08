// SPDX-License-Identifier: AGPL-3.0-or-later
#include "pl.h"

#define PAGE(quadrant, half) ((quadrant) << 2 | ((half) & 3))

static s16 D_80153400[] = {0x00 | 0x8000, -16, -24, 0};
static s16 D_80153408[] = {0x01 | 0x8000, -16, -24, 0};
static s16 D_80153410[] = {0x02 | 0x8000, -16, -24, 0};
static s16 D_80153418[] = {0x03 | 0x8000, -16, -24, 0};
static s16 D_80153420[] = {0x04 | 0x8000, -16, -24, 0};
static s16 D_80153428[] = {0x05 | 0x8000, -16, -24, 0};
static s16 D_80153430[] = {0x06 | 0x8000, -16, -24, 0};
static s16 D_80153438[] = {0x07 | 0x8000, -16, -24, 0};
static s16 D_80153440[] = {0x08 | 0x8000, -16, -24, 0};
static s16 D_80153448[] = {0x09 | 0x8000, -16, -24, 0};
static s16 D_80153450[] = {0x0A | 0x8000, -16, -24, 0};
static s16 D_80153458[] = {0x0B | 0x8000, -16, -24, 0};
static s16 D_80153460[] = {0x0C | 0x8000, -16, -24, 0};
static s16 D_80153468[] = {0x0D | 0x8000, -16, -24, 0};
static s16 D_80153470[] = {0x0E | 0x8000, -16, -24, 0};
static s16 D_80153478[] = {0x0F | 0x8000, -16, -24, 0};
static s16 D_80153480[] = {0x10 | 0x8000, -16, -24, 0};
static s16 D_80153488[] = {0x11 | 0x8000, -16, -24, 0};
static s16 D_80153490[] = {0x12 | 0x8000, -16, -24, 0};
static s16 D_80153498[] = {0x13 | 0x8000, -16, -24, 0};
static s16 D_801534A0[] = {0x14 | 0x8000, -16, -24, 0};
static s16 D_801534A8[] = {0x15 | 0x8000, -16, -24, 0};
static s16 D_801534B0[] = {0x16 | 0x8000, -16, -24, 0};
static s16 D_801534B8[] = {0x17 | 0x8000, -16, -24, 0};
static s16 D_801534C0[] = {0x18 | 0x8000, -16, -24, 0};
static s16 D_801534C8[] = {0x19 | 0x8000, -16, -24, 0};
static s16 D_801534D0[] = {0x1A | 0x8000, -16, -24, 0};
static s16 D_801534D8[] = {0x1B | 0x8000, -16, -24, 0};
static s16 D_801534E0[] = {0x1C | 0x8000, -16, -24, 0};
static s16 D_801534E8[] = {0x1D | 0x8000, -16, -24, 0};
static s16 D_801534F0[] = {0x1E | 0x8000, -16, -24, 0};
static s16 D_801534F8[] = {0x1F | 0x8000, -16, -24, 0};
static s16 D_80153500[] = {0x20 | 0x8000, -16, -24, 0};
static s16 D_80153508[] = {0x21 | 0x8000, -16, -24, 0};
static s16 D_80153510[] = {0x22 | 0x8000, -16, -24, 0};
static s16 D_80153518[] = {0x23 | 0x8000, -16, -24, 0};
static s16 D_80153520[] = {0x24 | 0x8000, -16, -24, 0};
static s16 D_80153528[] = {0x25 | 0x8000, -16, -24, 0};
static s16 D_80153530[] = {0x26 | 0x8000, -16, -24, 0};
static s16 D_80153538[] = {0x27 | 0x8000, -16, -24, 0};
static s16 D_80153540[] = {0x28 | 0x8000, -16, -24, 0};
static s16 D_80153548[] = {0x29 | 0x8000, -16, -24, 0};
static s16 D_80153550[] = {0x2A | 0x8000, -16, -24, 0};
static s16 D_80153558[] = {0x2B | 0x8000, -16, -24, 0};
static s16 D_80153560[] = {0x2C | 0x8000, -16, -24, 0};
static s16 D_80153568[] = {0x2D | 0x8000, -16, -24, 0};
static s16 D_80153570[] = {0x2E | 0x8000, -16, -24, 0};
static s16 D_80153578[] = {0x2F | 0x8000, -16, -24, 0};
static s16 D_80153580[] = {0x30 | 0x8000, -16, -24, 0};
static s16 D_80153588[] = {0x31 | 0x8000, -16, -24, 0};
static s16 D_80153590[] = {0x32 | 0x8000, -16, -24, 0};
static s16 D_80153598[] = {0x33 | 0x8000, -16, -24, 0};
static s16 D_801535A0[] = {0x34 | 0x8000, -16, -24, 0};
static s16 D_801535A8[] = {0x35 | 0x8000, -16, -24, 0};
static s16 D_801535B0[] = {0x36 | 0x8000, -16, -24, 0};
static s16 D_801535B8[] = {0x37 | 0x8000, -17, -24, 0};
static s16 D_801535C0[] = {0x38 | 0x8000, -18, -24, 0};
static s16 D_801535C8[] = {0x39 | 0x8000, -16, -24, 0};
static s16 D_801535D0[] = {0x3A | 0x8000, -16, -24, 0};
static s16 D_801535D8[] = {0x3B | 0x8000, -16, -24, 0};
static s16 D_801535E0[] = {0x3C | 0x8000, -16, -24, 0};
static s16 D_801535E8[] = {0x3D | 0x8000, -16, -24, 0};
static s16 D_801535F0[] = {0x3E | 0x8000, -16, -24, 0};
static s16 D_801535F8[] = {0x3F | 0x8000, -16, -24, 0};
static s16 D_80153600[] = {0x40 | 0x8000, -16, -24, 0};
static s16 D_80153608[] = {0x41 | 0x8000, -16, -24, 0};
static s16 D_80153610[] = {0x42 | 0x8000, -16, -24, 0};
static s16 D_80153618[] = {0x43 | 0x8000, -16, -24, 0};
static s16 D_80153620[] = {0x44 | 0x8000, -16, -24, 0};
static s16 D_80153628[] = {0x45 | 0x8000, -16, -24, 0};
static s16 D_80153630[] = {0x46 | 0x8000, -16, -24, 0};
static s16 D_80153638[] = {0x47 | 0x8000, -16, -24, 0};
static s16 D_80153640[] = {0x48 | 0x8000, -16, -24, 0};
static s16 D_80153648[] = {0x49 | 0x8000, -16, -24, 0};
static s16 D_80153650[] = {0x4A | 0x8000, -16, -24, 0};
static s16 D_80153658[] = {0x4B | 0x8000, -16, -24, 0};
static s16 D_80153660[] = {0x4C | 0x8000, -16, -24, 0};
static s16 D_80153668[] = {0x4D | 0x8000, -16, -24, 0};
static s16 D_80153670[] = {0x4E | 0x8000, -16, -24, 0};
static s16 D_80153678[] = {0x4F | 0x8000, -16, -24, 0};
static s16 D_80153680[] = {0x50 | 0x8000, -16, -24, 0};
static s16 D_80153688[] = {0x51 | 0x8000, -16, -24, 0};
static s16 D_80153690[] = {0x52 | 0x8000, -8, -24, 0};
static s16 D_80153698[] = {0x53 | 0x8000, -8, -24, 0};
static s16 D_801536A0[] = {0x54 | 0x8000, -8, -24, 0};
static s16 D_801536A8[] = {0x55 | 0x8000, -16, -24, 0};
static s16 D_801536B0[] = {0x56 | 0x8000, -24, -24, 0};
static s16 D_801536B8[] = {0x57 | 0x8000, -16, -24, 0};
static s16 D_801536C0[] = {0x58 | 0x8000, -16, -24, 0};
static s16 D_801536C8[] = {0x59 | 0x8000, -16, -24, 0};
static s16 D_801536D0[] = {0x5A | 0x8000, -16, -24, 0};
static s16 D_801536D8[] = {0x5B | 0x8000, -7, -20, 0};
static s16 D_801536E0[] = {0x5C | 0x8000, -16, -16, 0};
static s16 D_801536E8[] = {0x5D | 0x8000, -16, -24, 0};
static s16 D_801536F0[] = {0x5E | 0x8000, -12, -28, 0};
static s16 D_801536F8[] = {0x5F | 0x8000, -19, -27, 0};
static s16 D_80153700[] = {0x38 | 0x8000, -16, -20, 0};
static s16 D_80153708[] = {0x39 | 0x8000, -28, -21, 0};
static s16 D_80153710[] = {0x3A | 0x8000, -32, -24, 0};
static s16 D_80153718[] = {0x60 | 0x8000, -32, -24, 0};
static s16 D_80153720[] = {0x3C | 0x8000, -21, -24, 0};
static s16 D_80153728[] = {0x3C | 0x8000, -22, -24, 0};
static s16 D_80153730[] = {0x3E | 0x8000, -20, -24, 0};
static s16 D_80153738[] = {0x3E | 0x8000, -21, -24, 0};
static s16 D_80153740[] = {0x61 | 0x8000, -16, -24, 0};
static s16 D_80153748[] = {0x62 | 0x8000, -16, -24, 0};
static s16 D_80153750[] = {0x63 | 0x8000, -16, -24, 0};
static s16 D_80153758[] = {0x64 | 0x8000, -16, -24, 0};
static s16 D_80153760[] = {0x65 | 0x8000, -16, -24, 0};
static s16 D_80153768[] = {0x66 | 0x8000, -16, -24, 0};
static s16 D_80153770[] = {0x67 | 0x8000, -16, -24, 0};
static s16 D_80153778[] = {0x68 | 0x8000, -16, -24, 0};
static s16 D_80153780[] = {0x69 | 0x8000, -16, -24, 0};
static s16 D_80153788[] = {0x6A | 0x8000, -16, -24, 0};
static s16 D_80153790[] = {0x6B | 0x8000, -8, -24, 0};
static s16 D_80153798[] = {0x6C | 0x8000, -13, -19, 0};
static s16 D_801537A0[] = {0x38 | 0x8000, -10, -20, 0};
static s16 D_801537A8[] = {0x39 | 0x8000, -20, -21, 0};
static s16 D_801537B0[] = {0x3A | 0x8000, -25, -24, 0};
static s16 D_801537B8[] = {0x3B | 0x8000, -26, -24, 0};
static s16 D_801537C0[] = {0x6D | 0x8000, -15, -24, 0};
static s16 D_801537C8[] = {0x6D | 0x8000, -16, -24, 0};
static s16 D_801537D0[] = {0x6E | 0x8000, -14, -24, 0};
static s16 D_801537D8[] = {0x6E | 0x8000, -15, -24, 0};
static s16 D_801537E0[] = {0x6F | 0x8000, -8, -24, 0};
static s16 D_801537E8[] = {0x70 | 0x8000, -16, -32, 0};
static s16 D_801537F0[] = {0x71 | 0x8000, -24, -24, 0};
static s16 D_801537F8[] = {0x72 | 0x8000, -24, -24, 0};
static s16 D_80153800[] = {0x73 | 0x8000, -16, -24, 0};
static s16 D_80153808[] = {0x74 | 0x8000, -24, -24, 0};
static s16 D_80153810[] = {0x75 | 0x8000, -24, -24, 0};
static s16 D_80153818[] = {0x76 | 0x8000, -24, -24, 0};
static s16 D_80153820[] = {0x77 | 0x8000, -16, -24, 0};
static s16 D_80153828[] = {0x78 | 0x8000, -16, -24, 0};
static s16 D_80153830[] = {0x79 | 0x8000, -16, -24, 0};
static s16 D_80153838[] = {0x7A | 0x8000, -16, -8, 0};
static s16 D_80153840[] = {0x7B | 0x8000, -16, 0, 0};
static s16 D_80153848[] = {0x7C | 0x8000, -24, 0, 0};
static s16 D_80153850[] = {0x7D | 0x8000, -16, -8, 0};
static s16 D_80153858[] = {0x7E | 0x8000, -24, 0, 0};
static s16 D_80153860[] = {0x7F | 0x8000, -24, 0, 0};
static s16 D_80153868[] = {0x80 | 0x8000, -16, -8, 0};
static s16 D_80153870[] = {0x81 | 0x8000, -24, -8, 0};
static s16 D_80153878[] = {0x82 | 0x8000, -24, -16, 0};
static s16 D_80153880[] = {0x83 | 0x8000, -24, -16, 0};
static s16 D_80153888[] = {0x84 | 0x8000, -24, -7, 0};
static s16 D_80153890[] = {0x85 | 0x8000, -32, -24, 0};
static s16 D_80153898[] = {0x86 | 0x8000, -32, -24, 0};
static s16 D_801538A0[] = {0x87 | 0x8000, -32, -16, 0};
static s16 D_801538A8[] = {0x88 | 0x8000, -32, -24, 0};
static s16 D_801538B0[] = {0x89 | 0x8000, -32, -16, 0};
static s16 D_801538B8[] = {0x8A | 0x8000, -16, -8, 0};
static s16 D_801538C0[] = {0x8B | 0x8000, -8, -16, 0};
static s16 D_801538C8[] = {0x8C | 0x8000, -8, -16, 0};
static s16 D_801538D0[] = {0x8D | 0x8000, -8, -48, 0};
static s16 D_801538D8[] = {0x8E | 0x8000, -8, -48, 0};
static s16 D_801538E0[] = {0x8F | 0x8000, -16, -24, 0};
static s16 D_801538E8[] = {0x90 | 0x8000, -16, -8, 0};
static s16 D_801538F0[] = {0x91 | 0x8000, -16, -8, 0};
static s16 D_801538F8[] = {0x92 | 0x8000, -16, -8, 0};
static s16 D_80153900[] = {0x93 | 0x8000, -16, -24, 0};
static s16 D_80153908[] = {0x94 | 0x8000, -16, -24, 0};
static s16 D_80153910[] = {0x95 | 0x8000, -16, -24, 0};
static s16 D_80153918[] = {0x96 | 0x8000, -16, -24, 0};
static s16 D_80153920[] = {0x97 | 0x8000, -16, -24, 0};
static s16 D_80153928[] = {0x98 | 0x8000, -16, -32, 0};
static s16 D_80153930[] = {0x99 | 0x8000, -16, -32, 0};
static s16 D_80153938[] = {0x9A | 0x8000, -16, -24, 0};
static s16 D_80153940[] = {0x9B | 0x8000, -16, -24, 0};
static s16 D_80153948[] = {0x9C | 0x8000, -16, -24, 0};
static s16 D_80153950[] = {0x9D | 0x8000, -16, -24, 0};
static s16 D_80153958[] = {0x9E | 0x8000, -16, -24, 0};
static s16 D_80153960[] = {0x9F | 0x8000, -16, -24, 0};
static s16 D_80153968[] = {0xA0 | 0x8000, -16, -24, 0};
static s16 D_80153970[] = {0xA1 | 0x8000, -16, -24, 0};
static s16 D_80153978[] = {0xA2 | 0x8000, -16, -24, 0};
static s16 D_80153980[] = {0xA3 | 0x8000, -16, -24, 0};
static s16 D_80153988[] = {0xA4 | 0x8000, -16, -24, 11};
static s16 D_80153990[] = {0xA5 | 0x8000, -16, -24, 12};
static s16 D_80153998[] = {0xA6 | 0x8000, -16, -24, 13};
static s16 D_801539A0[] = {0xA7 | 0x8000, -16, -24, 0};
static s16 D_801539A8[] = {0xA8 | 0x8000, -16, -24, 0};
static s16 D_801539B0[] = {0xA9 | 0x8000, -16, -24, 15};
static s16 D_801539B8[] = {0xAA | 0x8000, -16, -24, 15};
static s16 D_801539C0[] = {0xAB | 0x8000, -24, -24, 15};
static s16 D_801539C8[] = {0xAC | 0x8000, -16, -40, 15};
static s16 D_801539D0[] = {0xAD | 0x8000, -16, -24, 14};
static s16 D_801539D8[] = {0xAE | 0x8000, -24, -24, 14};
static s16 D_801539E0[] = {0xAF | 0x8000, -19, -25, 17};
static s16 D_801539E8[] = {0xAF | 0x8000, -19, -25, 18};
static s16 D_801539F0[] = {0xAF | 0x8000, -19, -25, 19};
static s16 D_801539F8[] = {0xB0 | 0x8000, -16, -24, 16};
static s16 D_80153A00[] = {0xB1 | 0x8000, -16, -24, 16};
static s16 D_80153A08[] = {0xB2 | 0x8000, -16, -24, 16};
static s16 D_80153A10[] = {0xB3 | 0x8000, -16, -24, 16};
static s16 D_80153A18[] = {0xB4 | 0x8000, -16, -24, 0};
static s16 D_80153A20[] = {0xB5 | 0x8000, -16, -24, 0};
static s16 D_80153A28[] = {0xB6 | 0x8000, -16, -24, 0};
static s16 D_80153A30[] = {0xB7 | 0x8000, -16, -24, 0};
static s16 D_80153A38[] = {0xB8 | 0x8000, -16, -8, 0};
static s16 D_80153A40[] = {0xB9 | 0x8000, -16, -8, 0};
static s16 D_80153A48[] = {0xBA | 0x8000, -16, 0, 0};
static s16 D_80153A50[] = {0xBB | 0x8000, -16, -8, 0};
static s16 D_80153A58[] = {0xBC | 0x8000, -16, 0, 0};
static s16 D_80153A60[] = {0xBD | 0x8000, -16, -8, 0};
static s16 D_80153A68[] = {0xBE | 0x8000, -16, -24, 0};
static s16 D_80153A70[] = {0xBF | 0x8000, -16, -24, 0};
static s16 D_80153A78[] = {0xC0 | 0x8000, -16, -24, 0};
static s16 D_80153A80[] = {0xC1 | 0x8000, -16, -24, 0};
static s16 D_80153A88[] = {0xC2 | 0x8000, -8, -24, 0};
static s16 D_80153A90[] = {0xC3 | 0x8000, -8, -24, 0};
static s16 D_80153A98[] = {0xC4 | 0x8000, -16, -24, 0};
s16* g_MmxPlSprites[] = {
    NULL,       D_80153400, D_80153408, D_80153410, D_80153418, D_80153420,
    D_80153428, D_80153430, D_80153438, D_80153440, D_80153448, D_80153450,
    D_80153458, D_80153460, D_80153468, D_80153470, D_80153478, D_80153480,
    D_80153488, D_80153490, D_80153498, D_801534A0, D_801534A8, D_801534B0,
    D_801534B8, D_801534C0, D_801534C8, D_801534D0, D_801534D8, D_801534E0,
    D_801534E8, D_801534F0, D_801534F8, D_80153500, D_80153508, D_80153510,
    D_80153518, D_80153520, D_80153528, D_80153530, D_80153538, D_80153540,
    D_80153548, D_80153550, D_80153558, D_80153560, D_80153568, D_80153570,
    D_80153578, D_80153580, D_80153588, D_80153590, D_80153598, D_801535A0,
    D_801535A8, D_801535B0, D_801535B8, D_801535C0, D_801535C8, D_801535D0,
    D_801535D8, D_801535E0, D_801535E8, D_801535F0, D_801535F8, D_80153600,
    D_80153608, D_80153610, D_80153618, D_80153620, D_80153628, D_80153630,
    D_80153638, D_80153640, D_80153648, D_80153650, D_80153658, D_80153660,
    D_80153668, D_80153670, D_80153678, D_80153680, D_80153688, D_80153690,
    D_80153698, D_801536A0, D_801536A8, D_801536B0, D_801536B8, D_801536C0,
    D_801536C8, D_801536D0, D_801536D8, D_801536E0, D_801536E8, D_801536F0,
    D_801536F8, D_80153700, D_80153708, D_80153710, D_80153718, D_80153720,
    D_80153728, D_80153730, D_80153738, D_80153740, D_80153748, D_80153750,
    D_80153758, D_80153760, D_80153768, D_80153770, D_80153778, D_80153780,
    D_80153788, D_80153790, D_80153798, D_801537A0, D_801537A8, D_801537B0,
    D_801537B8, D_801537C0, D_801537C8, D_801537D0, D_801537D8, D_801537E0,
    D_801537E8, D_801537F0, D_801537F8, D_80153800, D_80153808, D_80153810,
    D_80153818, D_80153820, D_80153828, D_80153830, D_80153838, D_80153840,
    D_80153848, D_80153850, D_80153858, D_80153860, D_80153868, D_80153870,
    D_80153878, D_80153880, D_80153888, D_80153890, D_80153898, D_801538A0,
    D_801538A8, D_801538B0, D_801538B8, D_801538C0, D_801538C8, D_801538D0,
    D_801538D8, D_801538E0, D_801538E8, D_801538F0, D_801538F8, D_80153900,
    D_80153908, D_80153910, D_80153918, D_80153920, D_80153928, D_80153930,
    D_80153938, D_80153940, D_80153948, D_80153950, D_80153958, D_80153960,
    D_80153968, D_80153970, D_80153978, D_80153980, D_80153988, D_80153990,
    D_80153998, D_801539A0, D_801539A8, D_801539B0, D_801539B8, D_801539C0,
    D_801539C8, D_801539D0, D_801539D8, D_801539E0, D_801539E8, D_801539F0,
    D_801539F8, D_80153A00, D_80153A08, D_80153A10, D_80153A18, D_80153A20,
    D_80153A28, D_80153A30, D_80153A38, D_80153A40, D_80153A48, D_80153A50,
    D_80153A58, D_80153A60, D_80153A68, D_80153A70, D_80153A78, D_80153A80,
    D_80153A88, D_80153A90, D_80153A98};

static int D_80153AF4[] = {0x00300001, 0xFFF5FFF5, 0x00180018, 0x00000000,
                           0x00000000, 0x00180018, 0x00000000};
static int D_80153B10[] = {0x00200001, 0xFFF5FFF5, 0x00180018, 0x00000000,
                           0x00000018, 0x00180030, 0x00000000};
static int D_80153B2C[] = {0x00200001, 0xFFF5FFF5, 0x00180018, 0x00000000,
                           0x00000030, 0x00180048, 0x00000000};
static int D_80153B48[] = {0x00200001, 0xFFF5FFF5, 0x00180018, 0x00000000,
                           0x00000048, 0x00180060, 0x00000000};
static int D_80153B64[] = {0x00100001, 0xFFF5FFF5, 0x00180018, 0x00000000,
                           0x00180000, 0x00300018, 0x00000000};
static int D_80153B80[] = {0x00230001, 0xFFF5FFF5, 0x00180018, 0x00000000,
                           0x00000048, 0x00180060, 0x00000000};
static int D_80153B9C[] = {0x00230001, 0xFFF5FFF5, 0x00180018, 0x00000000,
                           0x00000030, 0x00180048, 0x00000000};
static int D_80153BB8[] = {0x00230001, 0xFFF5FFF5, 0x00180018, 0x00000000,
                           0x00000018, 0x00180030, 0x00000000};
static int D_80153BD4[] = {0x00000001, 0xFFF5FFF5, 0x00180018, 0x00000000,
                           0x00180018, 0x00300030, 0x00000000};
static int D_80153BF0[] = {0x00000001, 0xFFF5FFF5, 0x00180018, 0x00000001,
                           0x00180018, 0x00300030, 0x00000000};
static int D_80153C0C[] = {0x00000001, 0xFFF5FFF5, 0x00180018, 0x00000000,
                           0x00180030, 0x00300048, 0x00000000};
static int D_80153C28[] = {0x00000001, 0xFFF5FFF5, 0x00180018, 0x00000001,
                           0x00180030, 0x00300048, 0x00000000};
static int D_80153C44[] = {0x00100001, 0xFFF9FFF9, 0x00100010, 0x00190000,
                           0x00400000, 0x00500010, 0x00000000};
static int D_80153C60[] = {0x00000001, 0xFFF9FFF9, 0x00100010, 0x00190000,
                           0x00400010, 0x00500020, 0x00000000};
static int D_80153C7C[] = {0x00100001, 0xFFF9FFF9, 0x00100010, 0x00190000,
                           0x00500000, 0x00600010, 0x00000000};
static int D_80153C98[] = {0x00000001, 0xFFF9FFF9, 0x00100010, 0x00190000,
                           0x00500010, 0x00600020, 0x00000000};
static int D_80153CB4[] = {0x00040001, 0xFFF9FFF9, 0x00100010, 0x00190000,
                           0x00400070, 0x00500080, 0x00000000};
static int D_80153CD0[] = {0x00000001, 0xFFFDFFFD, 0x00080008, 0x00190000,
                           0x00300070, 0x00380078, 0x00000000};
static int D_80153CEC[] = {0x00040001, 0xFFFDFFFD, 0x00080008, 0x00190000,
                           0x00300078, 0x00380080, 0x00000000};
static int D_80153D08[] = {0x00000001, 0xFFFDFFFD, 0x00080008, 0x00190000,
                           0x00380070, 0x00400078, 0x00000000};

static u16 sprt_lemon[] = {
    1, // count
    0, -4, -3, 8, 6, PAL_HUD, PAGE(0x19, 2), 48, 1, 56, 7,
};
static u16 sprt_lemon_impact1[] = {
    1, // count
    0, -6, -8, 12, 16, PAL_HUD, PAGE(0x19, 2), 2, 0, 14, 16,
};
static u16 sprt_lemon_impact2[] = {
    1, // count
    0, -8, -8, 16, 16, PAL_HUD, PAGE(0x19, 2), 16, 0, 32, 16,
};
static u16 sprt_lemon_impact3[] = {
    1, // count
    0, -8, -8, 16, 16, PAL_HUD, PAGE(0x19, 2), 32, 0, 48, 16,
};
static u16 sprt_charge_mmx1_lv1_1[] = {
    1, // count
    0, -1, -1, 2, 2, PAL_PARTICLES, PAGE(0x19, 2), 56, 6, 58, 8,
};
static u16 sprt_charge_mmx1_lv1_2[] = {
    1, // count
    0, -1, -1, 2, 2, PAL_PARTICLES, PAGE(0x19, 2), 56, 4, 58, 6,
};
static u16 sprt_charge_mmx1_lv2_1[] = {
    1, // count
    0, -2, -2, 4, 4, PAL_HUD, PAGE(0x19, 2), 48, 8, 52, 12,
};
static u16 sprt_charge_mmx1_lv2_2[] = {
    1, // count
    0, -1, -1, 3, 3, PAL_HUD, PAGE(0x19, 2), 52, 8, 55, 11,
};
static u16 sprt_charge_mmx1_lv2_3[] = {
    1, // count
    0, -1, -1, 2, 2, PAL_HUD, PAGE(0x19, 2), 48, 14, 50, 16,
};
static u16 sprt_charge_mmx1_lv2_4[] = {
    1, // count
    0, -1, -1, 2, 2, PAL_HUD, PAGE(0x19, 2), 48, 12, 50, 14,
};
static u16 sprt_charge_mmx1_lv3_1[] = {
    1, // count
    0, -2, -2, 4, 4, PAL_PARTICLES, PAGE(0x19, 2), 56, 8, 60, 12,
};
static u16 sprt_charge_mmx1_lv3_2[] = {
    1, // count
    0, -1, -1, 3, 3, PAL_PARTICLES, PAGE(0x19, 2), 60, 8, 63, 11,
};
static u16 sprt_charge_mmx1_lv3_3[] = {
    1, // count
    0, -1, -1, 2, 2, PAL_PARTICLES, PAGE(0x19, 2), 56, 14, 58, 16,
};
static u16 sprt_charge_mmx1_lv3_4[] = {
    1, // count
    0, -1, -1, 2, 2, PAL_PARTICLES, PAGE(0x19, 2), 56, 12, 58, 14,
};

SpriteParts* g_SpritesWeapons[] = {
    NULL,
    sprt_lemon,
    sprt_lemon_impact1,
    sprt_lemon_impact2,
    sprt_lemon_impact3,
    sprt_charge_mmx1_lv1_1,
    sprt_charge_mmx1_lv1_2,
    sprt_charge_mmx1_lv2_1,
    sprt_charge_mmx1_lv2_2,
    sprt_charge_mmx1_lv2_3,
    sprt_charge_mmx1_lv2_4,
    sprt_charge_mmx1_lv3_1,
    sprt_charge_mmx1_lv3_2,
    sprt_charge_mmx1_lv3_3,
    sprt_charge_mmx1_lv3_4,
    (SpriteParts*)D_80153AF4,
    (SpriteParts*)D_80153B10,
    (SpriteParts*)D_80153B2C,
    (SpriteParts*)D_80153B48,
    (SpriteParts*)D_80153B64,
    (SpriteParts*)D_80153B80,
    (SpriteParts*)D_80153B9C,
    (SpriteParts*)D_80153BB8,
    (SpriteParts*)D_80153BD4,
    (SpriteParts*)D_80153BF0,
    (SpriteParts*)D_80153C0C,
    (SpriteParts*)D_80153C28,
    (SpriteParts*)D_80153C44,
    (SpriteParts*)D_80153C60,
    (SpriteParts*)D_80153C7C,
    (SpriteParts*)D_80153C98,
    (SpriteParts*)D_80153CB4,
    (SpriteParts*)D_80153CD0,
    (SpriteParts*)D_80153CEC,
    (SpriteParts*)D_80153D08};

static u16 sprt_item_power_capsule_small_closed[] = {
    1, // count
    0, -4, -8, 8, 8, PAL_HUD, PAGE(0x19, 1), 4, 8, 12, 16,
};
static u16 sprt_item_power_capsule_small_1[] = {
    1, // count
    0, -5, -8, 10, 8, PAL_HUD, PAGE(0x19, 1), 19, 8, 29, 16,
};
static u16 sprt_item_power_capsule_small_2[] = {
    1, // count
    0, -5, -8, 10, 8, PAL_HUD, PAGE(0x19, 1), 35, 8, 45, 16,
};
static u16 sprt_item_power_capsule_small_3[] = {
    1, // count
    0, -5, -8, 10, 8, PAL_HUD, PAGE(0x19, 1), 51, 8, 61, 16,
};
static u16 sprt_item_power_capsule_big_closed[] = {
    1, // count
    0, -7, -12, 14, 12, PAL_HUD, PAGE(0x19, 1), 113, 4, 127, 16,
};
static u16 sprt_item_power_capsule_big_1[] = {
    1, // count
    0, -8, -12, 16, 12, PAL_HUD, PAGE(0x19, 1), 80, 4, 96, 16,
};
static u16 sprt_item_power_capsule_big_2[] = {
    1, // count
    0, -8, -12, 16, 12, PAL_HUD, PAGE(0x19, 1), 96, 4, 112, 16,
};
static u16 sprt_item_power_capsule_big_3[] = {
    1, // count
    0, -8, -12, 16, 12, PAL_HUD, PAGE(0x19, 1), 64, 4, 80, 16,
};
static u16 sprt_item_energy_capsule_small_1[] = {
    1, // count
    0, -4, -8, 8, 8, PAL_HUD, PAGE(0x19, 1), 4, 24, 12, 32,
};
static u16 sprt_item_energy_capsule_small_2[] = {
    1, // count
    0, -4, -8, 8, 8, PAL_HUD, PAGE(0x19, 1), 20, 24, 28, 32,
};
static u16 sprt_item_energy_capsule_small_3[] = {
    1, // count
    0, -4, -8, 8, 8, PAL_HUD, PAGE(0x19, 1), 36, 24, 44, 32,
};
static u16 sprt_item_energy_capsule_big_1[] = {
    1, // count
    0, -7, -14, 14, 14, PAL_HUD, PAGE(0x19, 1), 81, 17, 95, 31,
};
static u16 sprt_item_energy_capsule_big_2[] = {
    1, // count
    0, -7, -14, 14, 14, PAL_HUD, PAGE(0x19, 1), 97, 17, 111, 31,
};
static u16 sprt_item_energy_capsule_big_3[] = {
    1, // count
    0, -7, -14, 14, 14, PAL_HUD, PAGE(0x19, 1), 113, 17, 127, 31,
};
static u16 sprt_item_life_up_1[] = {
    1, // count
    0, -8, -16, 16, 16, PAL_PLAYER, PAGE(0x19, 1), 48, 16, 64, 32,
};
static u16 sprt_item_life_up_2[] = {
    1, // count
    0, -8, -16, 16, 16, PAL_PLAYER, PAGE(0x19, 1), 64, 16, 80, 32,
};
static u16 sprt_item_heart_tank_1[] = {
    1, // count
    0, -7, -15, 14, 15, PAL_HEART, PAGE(0x19, 1), 1, 33, 15, 48,
};
static u16 sprt_item_heart_tank_2[] = {
    1, // count
    0, -6, -15, 12, 15, PAL_HEART, PAGE(0x19, 1), 18, 33, 30, 48,
};
static u16 sprt_item_heart_tank_3[] = {
    1, // count
    0, -5, -15, 10, 15, PAL_HEART, PAGE(0x19, 1), 35, 33, 45, 48,
};
static u16 sprt_item_heart_tank_4[] = {
    1, // count
    0, -6, -15, 12, 15, PAL_HEART, PAGE(0x19, 1), 50, 33, 62, 48,
};
static u16 sprt_death_particle_1[] = {
    1, // count
    0, -3, -3, 6, 6, PAL_PLAYER, PAGE(0x19, 1), 90, 112, 96, 118,
};
static u16 sprt_death_particle_2[] = {
    1, // count
    0, -4, -4, 8, 8, PAL_PLAYER, PAGE(0x19, 1), 88, 119, 96, 127,
};
static u16 sprt_death_particle_3[] = {
    1, // count
    0, -5, -5, 9, 9, PAL_PLAYER, PAGE(0x19, 1), 80, 112, 89, 121,
};
static u16 sprt_death_particle_4[] = {
    1, // count
    0, -6, -6, 11, 11, PAL_PLAYER, PAGE(0x19, 1), 96, 112, 107, 123,
};
static u16 sprt_death_particle_5[] = {
    1, // count
    0, -8, -8, 15, 15, PAL_PLAYER, PAGE(0x19, 1), 112, 112, 127, 127,
};

SpriteParts* g_SpritesItems[] = {
    (SpriteParts*)NULL,
    sprt_item_power_capsule_small_closed,
    sprt_item_power_capsule_small_1,
    sprt_item_power_capsule_small_2,
    sprt_item_power_capsule_small_3,
    sprt_item_power_capsule_big_closed,
    sprt_item_power_capsule_big_1,
    sprt_item_power_capsule_big_2,
    sprt_item_power_capsule_big_3,
    sprt_item_energy_capsule_small_1,
    sprt_item_energy_capsule_small_2,
    sprt_item_energy_capsule_small_3,
    sprt_item_energy_capsule_big_1,
    sprt_item_energy_capsule_big_2,
    sprt_item_energy_capsule_big_3,
    sprt_item_life_up_1,
    sprt_item_life_up_2,
    sprt_item_heart_tank_1,
    sprt_item_heart_tank_2,
    sprt_item_heart_tank_3,
    sprt_item_heart_tank_4,
    sprt_death_particle_1,
    sprt_death_particle_2,
    sprt_death_particle_3,
    sprt_death_particle_4,
    sprt_death_particle_5};

static int D_8015421C[] = {0x00300001, 0xFFEAFFF5, 0x00280018, 0x00010000,
                           0x00000000, 0x00280018, 0x00000000};
static int D_80154238[] = {0x00200001, 0xFFEAFFF5, 0x00280018, 0x00010000,
                           0x00000018, 0x00280030, 0x00000000};
static int D_80154254[] = {0x00200001, 0xFFEAFFF2, 0x00280018, 0x00010000,
                           0x00000030, 0x00280048, 0x00000000};
static int D_80154270[] = {0x00200001, 0xFFEAFFF2, 0x00280018, 0x00010000,
                           0x00000048, 0x00280060, 0x00000000};
static int D_8015428C[] = {0x00200001, 0xFFEAFFF3, 0x00280018, 0x00010000,
                           0x00000060, 0x00280078, 0x00000000};
static int D_801542A8[] = {0x00100001, 0xFFEAFFF2, 0x00280018, 0x00010000,
                           0x00280000, 0x00500018, 0x00000000};
static int D_801542C4[] = {0x00000001, 0xFFEAFFF2, 0x00280018, 0x00010000,
                           0x00280018, 0x00500030, 0x00000000};
static int D_801542E0[] = {0x00000001, 0xFFEAFFF2, 0x00280018, 0x00010000,
                           0x00280030, 0x00500048, 0x00000000};
static int D_801542FC[] = {0x00000001, 0xFFEAFFF5, 0x00280018, 0x00010000,
                           0x00280048, 0x00500060, 0x00000000};
static int D_80154318[] = {
    0x00000002, 0xFFEAFFF4, 0x00280018, 0x00010000, 0x00280060, 0x00500078,
    0x00000000, 0x0018FFDF, 0x00000018, 0x00180000, 0x00300038, 0x00000050};
static int D_80154348[] = {
    0x00000002, 0xFFEAFFF4, 0x00280018, 0x00010000, 0x00280060, 0x00500078,
    0x00030000, 0x0010FFE5, 0x00000010, 0x00300000, 0x00400038, 0x00000048};
static int D_80154378[] = {
    0x00000002, 0xFFEAFFF4, 0x00280018, 0x00010000, 0x00280060, 0x00500078,
    0x00050000, 0x0008FFEA, 0x00000008, 0x00480000, 0x00500028, 0x00000030};
static int D_801543A8[] = {0x00000001, 0xFFEAFFF4, 0x00280018, 0x00010000,
                           0x00280060, 0x00500078, 0x00000000};
static int D_801543C4[] = {0x00100001, 0xFFEAFFF4, 0x00280018, 0x00010000,
                           0x00500000, 0x00780018, 0x00000000};
static int D_801543E0[] = {0x00000001, 0xFFEAFFF4, 0x00280018, 0x00010000,
                           0x00500018, 0x00780030, 0x00000000};
static int D_801543FC[] = {0x00000001, 0xFFEAFFF5, 0x00280018, 0x00010000,
                           0x00500030, 0x00780048, 0x00000000};
static int D_80154418[] = {0x00000001, 0xFFEAFFF5, 0x00280018, 0x00000000,
                           0x00400050, 0x00680068, 0x00000000};
static int D_80154434[] = {0x00040001, 0xFFF2FFF5, 0x00200018, 0x00000000,
                           0x00400068, 0x00600080, 0x00000000};
static int D_80154450[] = {0x000C0001, 0xFFF2FFF5, 0x00200018, 0x00000000,
                           0x00600068, 0x00800080, 0x00000000};
static int D_8015446C[] = {0x00000001, 0xFFEAFFF8, 0x00280018, 0x00010000,
                           0x00500048, 0x00780060, 0x00000000};
static int D_80154488[] = {0x00100001, 0xFFF2FFF8, 0x00200018, 0x00000000,
                           0x00380000, 0x00580018, 0x00000000};
static int D_801544A4[] = {0x00000001, 0xFFFAFFF5, 0x00180018, 0x00010000,
                           0x00500060, 0x00680078, 0x00000000};
static int D_801544C0[] = {0x00080001, 0xFFFAFFF5, 0x00180018, 0x00010000,
                           0x00680060, 0x00800078, 0x00000000};
static int D_801544DC[] = {0x00080001, 0xFFFAFFF5, 0x00180018, 0x00000000,
                           0x00680050, 0x00800068, 0x00000000};
static int D_801544F8[] = {0x00080001, 0xFFEEFFEF, 0x00280020, 0x00000001,
                           0x00580030, 0x00800050, 0x00000000};
static int D_80154514[] = {0x00000001, 0xFFF9FFF4, 0x00100018, 0x00000004,
                           0x00500018, 0x00600030, 0x00000000};
static int D_80154530[] = {0x00000001, 0xFFF3FFF7, 0x00180010, 0x00000003,
                           0x00400040, 0x00580050, 0x00000000};
static int D_8015454C[] = {0x00000001, 0xFFF9FFF8, 0x00100010, 0x00000002,
                           0x00480030, 0x00580040, 0x00000000};

SpriteParts* D_801541A8[] = {
    (SpriteParts*)NULL,       (SpriteParts*)D_8015421C,
    (SpriteParts*)D_80154238, (SpriteParts*)D_80154254,
    (SpriteParts*)D_80154270, (SpriteParts*)D_8015428C,
    (SpriteParts*)D_801542A8, (SpriteParts*)D_801542C4,
    (SpriteParts*)D_801542E0, (SpriteParts*)D_801542FC,
    (SpriteParts*)D_80154318, (SpriteParts*)D_80154348,
    (SpriteParts*)D_80154378, (SpriteParts*)D_801543A8,
    (SpriteParts*)D_801543C4, (SpriteParts*)D_801543E0,
    (SpriteParts*)D_801543FC, (SpriteParts*)D_80154418,
    (SpriteParts*)D_80154434, (SpriteParts*)D_80154450,
    (SpriteParts*)D_8015446C, (SpriteParts*)D_80154488,
    (SpriteParts*)D_801544A4, (SpriteParts*)D_801544C0,
    (SpriteParts*)D_801544DC, (SpriteParts*)D_801544F8,
    (SpriteParts*)D_80154514, (SpriteParts*)D_80154530,
    (SpriteParts*)D_8015454C};
