#include "pl.h"

#define LZ4_STATIC_LINKING_ONLY
#include "lz4/lz4.h"

static unsigned short palette[][16] = {
#include "assets/pal.inc"
};

static unsigned char img_hud[] = {
#include "assets/hud.png.inc"
};
static unsigned char img_items[] = {
#include "assets/items.png.inc"
};
static unsigned char img_particles[] = {
#include "assets/particles.png.inc"
};

#define G_MAKE(x, y, data) {data, sizeof(data), x, y}
struct GraphicsInit {
    unsigned char* data;
    unsigned int len;
    unsigned short vramX, vramY;
};
struct GraphicsInit gfx_init[] = {
    G_MAKE(512 + 64, 256, img_hud),
    G_MAKE(512 + 96, 256, img_items),
    G_MAKE(512 + 64, 384, img_particles),
};

static bool DecompressData(void* dst, void* src, size_t srcLen, size_t dstLen) {
    return LZ4_decompress_safe((const char*)src, (char*)dst, srcLen, dstLen) ==
           dstLen;
}

static void InitPalette() {
    memcpy(g_Clut + 0x1200, palette, sizeof(palette));
    RECT vramPalette = {0, 240, 256, 16};
    LoadImage(&vramPalette, g_Clut + 0x1000);
}
void MmxInitGraphics() {
    // we divide Width by 2 because the bitmaps are 4bpp
    const int W = 128;
    const int H = 128;
    const int DEC_LEN = W / 2 * H;
    unsigned char* dst = g_Pix[0];

    for (int i = 0; i < LEN(gfx_init); i++) {
        if (!DecompressData(dst, gfx_init[i].data, gfx_init[i].len, DEC_LEN)) {
            ERRORF("failed to decompress gfx_init[%d]", i);
            continue;
        }
        RECT vramDst = {gfx_init[i].vramX, gfx_init[i].vramY, W >> 2, H};
        LoadImage(&vramDst, dst);
    }

    InitPalette();
}
