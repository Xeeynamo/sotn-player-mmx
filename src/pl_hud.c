#include "pl.h"

static Primitive* FindPrimitive(u16 tpage, int u16) {
    Primitive* prim = &g_PrimBuf[MAX_PRIM_COUNT - 1];
    for (int i = MAX_PRIM_COUNT; i; i--) {
        if (prim->tpage == 0x1B && prim->clut == 0x101) {
            return prim;
        }
    }
    return NULL;
}
static void HidePrimitives(Primitive* prim) {
    // to completely hide the primitives, shrink their width and height to zero
    // and set the priority to the minimum for the sub-weapons
    while (prim) {
        prim->drawMode = DRAW_HIDE;
        prim->tpage = 0;
        prim->clut = 0;
        prim->x0 = prim->y0 = 0;
        prim->x1 = prim->y1 = 0;
        prim->x2 = prim->y2 = 0;
        prim->x3 = prim->y3 = 0;
        prim->priority = 0;
        prim = prim->next;
    }
}
static void HideRichterHud() {
    // we cannot destroy the primitives as there's logic relying on them
    // being allocated, so we just hide them.
    HidePrimitives(FindPrimitive(0x1B, 0x101)); // main hud
    HidePrimitives(FindPrimitive(0x1B, 0x100)); // sub-weapon
}
static s32 AllocHighPriorityPrimitives(u8 type, s32 count) {
    Primitive* prim;
    s32 i;
    s16 foundPolyIndex;

    prim = &g_PrimBuf[LEN(g_PrimBuf) - 1];
    i = LEN(g_PrimBuf) - 1;
    while (i >= 0) {
        if (prim->type == 0) {
            memset(prim, 0, sizeof(Primitive));
            if (count == 1) {
                prim->type = type;
                prim->next = NULL;
            } else {
                prim->type = type;
                foundPolyIndex = AllocHighPriorityPrimitives(type, count - 1);
                prim->next = &g_PrimBuf[foundPolyIndex];
            }
            foundPolyIndex = i;
            return foundPolyIndex;
        }
        i--;
        prim--;
    }
    return -1;
}
static inline void SetHudRect(
    Primitive* prim, s16 x, s16 y, s16 w, s16 h, u8 u, u8 v) {
    prim->x0 = prim->x2 = x;
    prim->y0 = prim->y1 = y;
    prim->x1 = prim->x3 = x + w;
    prim->y2 = prim->y3 = (s16)(y + h);
    prim->u0 = prim->u2 = u;
    prim->v0 = prim->v1 = v;
    prim->u1 = prim->u3 = u + w;
    prim->v2 = prim->v3 = v + h;
}

static int hud_step = 0;
static int hud_prim_index = 0;
void MmxHudHandler() {
    // primitive description
    // 0: the base at the bottom, where the X symbol stands, never moves
    // 1: the tip of the health bar, moves with the max health
    // 2: the health bar body, it stretches with the max health
    // 3, 4, 5, 6: health steps
    const int X = 8;
    const int Y = 28;
    const int MAX_HP = 32;
    Primitive* prim;
    int i;
    int y;
    int hpDraw;
    unsigned int hpCur;
    unsigned int hpMax;

    if (hud_prim_index == -1) {
        return;
    }
    if (hud_step == 0) {
        HideRichterHud();
        hud_prim_index = AllocHighPriorityPrimitives(PRIM_GT4, 7);
        if (hud_prim_index == -1) {
            return;
        }
        prim = &g_PrimBuf[hud_prim_index];
        SetHudRect(prim, X, Y + MAX_HP * 2, 16, 16, 0, 0);
        while (prim) {
            prim->tpage = 0x19;
            prim->clut = PAL_HUD;
            prim->priority = 0x1F0;
            prim->drawMode = DRAW_ABSPOS;
            prim = prim->next;
        }
        hud_step++;
    }

    // normalize HP values based on a normal Richter play-through
    // start hp: 50
    // health vessel hp increase: 10
    // max possible hp amount: 540 (PSX), 600 (Saturn)
    // do unsigned math to have a small performance bump on PSX
    if (g_Status.hpMax < 320) {
        hpCur = g_Status.hp / 10U;
        hpMax = g_Status.hpMax / 10U;
    } else {
        hpMax = 32;
        hpCur = ((g_Status.hp << 5) / g_Status.hpMax) / 10U;
    }
    if (hpCur == 0 && g_Status.hp > 0) {
        hpCur = 1;
    }

    prim = &g_PrimBuf[hud_prim_index];
    y = Y + MAX_HP * 2;
    prim = prim->next;

    y -= hpMax * 2;
    SetHudRect(prim, X, y, 16, 2, 16, 4);
    prim->y2 = prim->y3 = prim->y0 + hpMax * 2;
    prim = prim->next;

    y -= 4;
    SetHudRect(prim, X, y, 16, 4, 16, 0);
    prim = prim->next;

    // the maximum amount of health is 32; instead of allocating
    // 32 more prims for each life step we can just use a 16x16 sprite that
    // represents 8 steps filled and just draw a portion of it when needed;
    // this way we can reduce the required prims count to just 4!
    y = Y + MAX_HP * 2;
    hpDraw = hpCur;
    for (i = 0; i < 4; i++) {
        if (hpDraw <= 0) {
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
            continue;
        }
        SetHudRect(
            prim, X, y - MIN(hpDraw, 8) * 2, 16, MIN(hpDraw, 8) * 2, 64, 0);
        prim = prim->next;
        y -= 16;
        hpDraw -= 8;
    }
}
