// SPDX-License-Identifier: AGPL-3.0-or-later
#include "pl.h"
#include "sfx.h"

// RIC Entity # 49. Comes from blueprint 57. Factory call in
// RicEntityCrashReboundStone. RicEntityCrashReboundStone is RIC Entity #48.
// Comes from blueprint 56. Blueprint 56 is subweapon 25. And subweapon 25 is
// the crash of subweapon 7. And subweapon 7 is the rebound stone. This is an
// entity spawned from the rebound stone crash.
static s32 angles_80155EE0[] = {0x00000F80, 0x00000100, 0x00000700, 0x00000880};
void func_8016D9C4(Entity* self) {
    PrimLineG2* prim;
    Primitive* prim2;
    s32 i;
    long angle;
    s32 var_s6;
    s32 var_s5;
    s32 var_s7;
    s32 brightness;

    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_LINE_G2, 20);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags = FLAG_KEEP_ALIVE_OFFCAMERA | FLAG_HAS_PRIMS;
        prim = (PrimLineG2*)&g_PrimBuf[self->primIndex];
        for (i = 0; i < 4; i++) {
            prim->preciseX.val = PLAYER.posX.val;
            prim->preciseY.val = PLAYER.posY.val - FIX(40);
            prim->priority = 194;
            prim->drawMode = DRAW_HIDE;
            prim->x0 = prim->x1 = PLAYER.posX.i.hi;
            prim->y0 = prim->y1 = PLAYER.posY.i.hi - 0x1C;
            prim->r0 = prim->g0 = prim->b0 = 0x80;
            prim->r1 = prim->g1 = prim->b1 = 0x70;
            prim->angle = angles_80155EE0[i];
            prim->delay = 1;
            prim = (PrimLineG2*)prim->next;
        }
        for (brightness = 0x80; i < 20; i++) {
            if (!(i % 4)) {
                brightness -= 0x10;
                switch (i / 4) {
                case 1:
                    self->ext.et_8016D9C4.lines[0] = prim;
                    break;
                case 2:
                    self->ext.et_8016D9C4.lines[1] = prim;
                    break;
                case 3:
                    self->ext.et_8016D9C4.lines[2] = prim;
                    break;
                case 4:
                    self->ext.et_8016D9C4.lines[3] = prim;
                    break;
                }
            }
            prim->priority = 0xC2;
            prim->drawMode = DRAW_HIDE;
            prim->x0 = prim->x1 = PLAYER.posX.i.hi;
            prim->y0 = prim->y1 = PLAYER.posY.i.hi - 0x1C;
            prim->r0 = prim->g0 = prim->b0 = brightness;
            prim->r1 = prim->g1 = prim->b1 = brightness - 0x10;
            prim = (PrimLineG2*)prim->next;
        }
        self->ext.et_8016D9C4.unk90 = 4;
        self->ext.et_8016D9C4.unk8C = self->ext.et_8016D9C4.unk8E = 0;
        g_api.PlaySfx(SFX_RIC_RSTONE_TINK);
        self->step++;
        break;
    case 1:
        self->ext.et_8016D9C4.unk8E = 1;
        switch (self->ext.et_8016D9C4.unk8C) {
        case 0:
        default:
            prim = (PrimLineG2*)&g_PrimBuf[self->primIndex];
            break;
        case 1:
            prim = self->ext.et_8016D9C4.lines[0];
            break;
        case 2:
            prim = self->ext.et_8016D9C4.lines[1];
            break;
        case 3:
            prim = self->ext.et_8016D9C4.lines[2];
            break;
        case 4:
            prim = self->ext.et_8016D9C4.lines[3];
            break;
        }
        for (i = 0; i < 4; i++) {
            prim->drawMode &= ~DRAW_HIDE;
            prim = (PrimLineG2*)prim->next;
        }
        self->ext.et_8016D9C4.unk8C++;
        if (self->ext.et_8016D9C4.unk8C > 4) {
            self->step++;
        }
        break;
    case 2:
        if (!self->ext.et_8016D9C4.unk90) {
            self->step++;
            break;
        }
        break;
    case 3:
        self->ext.et_8016D9C4.unk90++;
        if (self->ext.et_8016D9C4.unk90 > 4) {
            DestroyEntity(self);
            return;
        }
        break;
    }
    if (!self->ext.et_8016D9C4.unk8E) {
        return;
    }
    prim = (PrimLineG2*)&g_PrimBuf[self->primIndex];
    for (i = 0; i < 4; i++) {
        if (prim->delay) {
            prim->x1 = prim->x0;
            prim->y1 = prim->y0;
            prim->x0 = prim->preciseX.i.hi;
            prim->y0 = prim->preciseY.i.hi;
            var_s7 = ratan2(prim->preciseY.val, FIX(128) - prim->preciseX.val) &
                     0xFFF;
            angle = prim->angle - var_s7;
            if (abs(angle) > 0x800) {
                if (angle < 0) {
                    angle += 0x1000;
                } else {
                    angle -= 0x1000;
                }
            }
            if (angle >= 0) {
                if (angle > 0x80) {
                    var_s6 = 0x80;
                } else {
                    var_s6 = angle;
                }
                angle = var_s6;
            } else {
                if (angle < -0x80) {
                    var_s5 = -0x80;
                } else {
                    var_s5 = angle;
                }
                angle = var_s5;
            }
            prim->angle = prim->angle - angle;
            prim->angle &= 0xFFF;
            prim->velocityX.val = (rcos(prim->angle) << 4 << 4);
            prim->velocityY.val = -(rsin(prim->angle) << 4 << 4);
            prim->preciseX.val += prim->velocityX.val;
            prim->preciseY.val += prim->velocityY.val;
            self->posX.i.hi = prim->preciseX.i.hi;
            self->posY.i.hi = prim->preciseY.i.hi;
            RicCreateEntFactoryFromEntity(
                self, BP_CRASH_REBOUND_STONE_PARTICLES, 0);
            if (prim->preciseY.val < 0) {
                prim->delay = 0;
                prim->drawMode |= DRAW_HIDE;
                self->ext.et_8016D9C4.unk90--;
            }
        }
        prim = (PrimLineG2*)prim->next;
    }
    prim = self->ext.et_8016D9C4.lines[0];
    prim2 = &g_PrimBuf[self->primIndex];
    for (i = 0; i < 16; i++) {
        prim->x1 = prim->x0;
        prim->y1 = prim->y0;
        prim->x0 = prim2->x1;
        prim->y0 = prim2->y1;
        prim = (PrimLineG2*)prim->next;
        prim2 = prim2->next;
    }
}

void func_8016F198(Entity* self) {
    const int PrimCount = 16;
    Primitive* prim;
    s16 unk7C;
    s16 temp_s6;
    s16 temp_a0;
    s16 temp_a1;
    s16 temp_a2;
    s16 temp_v1;
    s16 var_s0_2;
    s32 sine;
    s32 cosine;
    s32 i;
    u16 tpage;

    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, PrimCount);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            g_Player.unk4E = 1;
            return;
        }
        self->flags = FLAG_KEEP_ALIVE_OFFCAMERA | FLAG_HAS_PRIMS;
        prim = &g_PrimBuf[self->primIndex];
        for (i = 0; i < 16; i++) {
            prim->priority = 0xC2;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
        self->step++;
        break;
    case 1:
        prim = &g_PrimBuf[self->primIndex];
        for (i = 0; i < 16; i++) {
            prim->drawMode &= ~DRAW_HIDE;
            prim = prim->next;
        }
        self->step++;
    case 2:
        if (++self->ext.factory.unk7C >= 0x18) {
            self->step++;
        }
        break;
    case 3:
        g_Player.unk4E = 1;
        DestroyEntity(self);
        return;
    }
    if (self->ext.factory.unk7C == 0) {
        return;
    }
    if (g_CurrentBuffer == g_GpuBuffers) {
        tpage = 0x104;
    } else {
        tpage = 0x100;
    }
    prim = &g_PrimBuf[self->primIndex];
    for (i = 0; i < PrimCount; i++) {
        sine = rsin(i << 8);
        cosine = rcos(i << 8);
        unk7C = self->ext.factory.unk7C;
        var_s0_2 = 0;
        temp_s6 = unk7C * 8;
        if (unk7C >= 4) {
            var_s0_2 = (unk7C - 4) * 8;
        }
        temp_a1 = ((cosine * (s16)(unk7C * 8)) >> 0xC) + 0x80;
        temp_v1 = ((cosine * var_s0_2) >> 0xC) + 0x80;
        temp_a0 = ((sine * (s16)(unk7C * 8)) >> 0xC) + 0x78;
        temp_a2 = ((sine * var_s0_2) >> 0xC) + 0x78;

        temp_a1 = temp_a1 >= 0 ? MIN(temp_a1, 0xFF) : 0;
        prim->x0 = temp_a1;

        temp_v1 = temp_v1 >= 0 ? MIN(temp_v1, 0xFF) : 0;
        prim->x2 = temp_v1;

        temp_a0 = temp_a0 >= 0 ? MIN(temp_a0, 0xF0) : 0;
        prim->y0 = temp_a0;

        temp_a2 = temp_a2 >= 0 ? MIN(temp_a2, 0xF0) : 0;
        prim->y2 = temp_a2;

        prim->u0 = ~prim->x0;
        prim->u2 = ~prim->x2;
        prim->v0 = -0x10 - prim->y0;
        prim->v2 = -0x10 - prim->y2;

        sine = rsin((i + 1) << 8);
        cosine = rcos((i + 1) << 8);
        temp_a1 = ((cosine * temp_s6) >> 0xC) + 0x80;
        temp_v1 = ((cosine * var_s0_2) >> 0xC) + 0x80;
        temp_a0 = ((sine * temp_s6) >> 0xC) + 0x78;
        temp_a2 = ((sine * var_s0_2) >> 0xC) + 0x78;

        temp_a1 = temp_a1 >= 0 ? MIN(temp_a1, 0xFF) : 0;
        prim->x1 = temp_a1;

        temp_v1 = temp_v1 >= 0 ? MIN(temp_v1, 0xFF) : 0;
        prim->x3 = temp_v1;

        temp_a0 = temp_a0 >= 0 ? MIN(temp_a0, 0xF0) : 0;
        prim->y1 = temp_a0;

        temp_a2 = temp_a2 >= 0 ? MIN(temp_a2, 0xF0) : 0;
        prim->y3 = temp_a2;

        prim->tpage = tpage;
        prim->u1 = ~prim->x1;
        prim->u3 = ~prim->x3;
        prim->v1 = -0x10 - prim->y1;
        prim->v3 = -0x10 - prim->y3;
        prim = prim->next;
    }
}

void func_801705EC(Entity* entity) {
    u16 temp;

    switch (entity->step) {
    case 0:
        entity->flags = FLAG_KEEP_ALIVE_OFFCAMERA;
        entity->ext.circleExpand.height = 0;
        entity->step++;
    case 1:
    case 3:
    case 5:
    case 7:
        temp = entity->ext.circleExpand.height + 1;
        entity->ext.circleExpand.height = temp;
        RicCreateEntFactoryFromEntity(
            entity, FACTORY(BP_SUBWPN_STOPWATCH, temp), 0);
        entity->ext.circleExpand.width = 0;
        entity->step++;
        break;
    case 2:
    case 4:
    case 6:
        entity->ext.circleExpand.width++;
        if (entity->ext.circleExpand.width >= 16) {
            entity->step++;
        }
        break;
    case 8:
        DestroyEntity(entity);
        break;
    }
}
