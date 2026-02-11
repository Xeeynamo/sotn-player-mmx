// SPDX-License-Identifier: AGPL-3.0-or-later
#include "pl.h"

void RicEntitySlideKick(Entity* entity) {}

void func_80160D2C(Entity* self) {}

// created from a blueprint, #24
void RicEntityBladeDash(Entity* self) {
    if (PLAYER.step != PL_S_BLADEDASH) {
        DestroyEntity(self);
    } else {
        self->posX.i.hi = PLAYER.posX.i.hi;
        self->posY.i.hi = PLAYER.posY.i.hi;
        self->facingLeft = PLAYER.facingLeft;
        if (self->step == 0) {
            self->flags = FLAG_UNK_20000 | FLAG_POS_PLAYER_LOCKED |
                          FLAG_KEEP_ALIVE_OFFCAMERA;
            self->hitboxHeight = 20;
            self->hitboxWidth = 20;
            self->hitboxOffY = 0;
            self->hitboxOffX = 0;
            self->ext.subweapon.subweaponId = 17;
            RicSetSubweaponParams(self);
            self->step++;
        }
        if (PLAYER.pose >= 19) {
            DestroyEntity(self);
        }
    }
}

void func_80160F0C(Entity* self) {
    if (PLAYER.step != PL_S_HIGHJUMP) {
        DestroyEntity(self);
        return;
    }
    self->posX.i.hi = PLAYER.posX.i.hi;
    self->posY.i.hi = PLAYER.posY.i.hi;
    self->facingLeft = PLAYER.facingLeft;
    if (self->step == 0) {
        self->flags =
            FLAG_UNK_20000 | FLAG_POS_PLAYER_LOCKED | FLAG_KEEP_ALIVE_OFFCAMERA;
        self->hitboxOffX = 0xC;
        self->hitboxOffY = -0x1A;
        self->hitboxWidth = 12;
        self->hitboxHeight = 12;
        self->ext.subweapon.subweaponId = 22;
        RicSetSubweaponParams(self);
        self->step++;
    }
}

static u16 pos_x_80154C50[] = {0, -4, -8, -12, -16, -20};
static s32 velocity_x_80154C5C[] = {
    -0x3000, -0x4000, -0x6000, -0x8000, -0xA000, -0xC000};
static u16 rot_x_80154C74[] = {0x0030, 0x0040, 0x0050, 0x0060, 0x0070, 0x0080};
static AnimationFrame anim_smoke_puff[] = {
    POSE(1, 1, 0),  POSE(1, 2, 0),  POSE(1, 3, 0),  POSE(1, 4, 0),
    POSE(1, 5, 0),  POSE(1, 6, 0),  POSE(1, 7, 0),  POSE(1, 8, 0),
    POSE(1, 9, 0),  POSE(1, 10, 0), POSE(1, 11, 0), POSE(1, 12, 0),
    POSE(1, 13, 0), POSE(1, 14, 0), POSE(1, 15, 0), POSE(1, 16, 0),
    POSE(1, 17, 0), POSE(1, 18, 0), POSE(1, 19, 0), POSE(1, 20, 0),
    POSE(1, 21, 0), POSE(1, 22, 0), POSE(1, 23, 0), POSE(1, 24, 0),
    POSE_END};
static u8 sensors1_80154CE4[] = {2, 9, 3, 10, 1, 8, 4, 11, 0, 7, 5, 12, 6, 13};
static u8 sensors2_80154CF4[] = {2, 9, 3, 10, 4, 11, 5, 12, 6, 13};
void RicEntitySmokePuff(Entity* self) {
    s16 posX;
    s32 i;

    s16 paramsLo = self->params & 0xFF;
    s16 paramsHi = self->params >> 8;

    if ((g_Player.status & PLAYER_STATUS_UNK20000) && (paramsHi != 9)) {
        DestroyEntity(self);
        return;
    }
    switch (self->step) {
    case 0:
        self->animSet = 5;
        self->anim = anim_smoke_puff;
        self->zPriority = PLAYER.zPriority + 2;
        self->flags = FLAG_POS_CAMERA_LOCKED | FLAG_UNK_100000 | FLAG_UNK_10000;
        self->drawMode = DRAW_TPAGE2 | DRAW_TPAGE;
        self->drawFlags = FLAG_DRAW_SCALEX | FLAG_DRAW_SCALEY | FLAG_DRAW_UNK40;
        self->opacity = 0x60;
        posX = pos_x_80154C50[paramsLo];
        if (paramsHi == 0) {
            posX += 6;
        }
        if (paramsHi == 1) {
            posX -= 8;
        }
        if (paramsHi == 2) {
            posX -= 6;
        }
        if (paramsHi == 5) {
            posX = -6;
        }
        if (paramsHi == 3) {
            self->posY.i.hi -= 8;
        }
        if (paramsHi == 4) {
            for (i = paramsLo * 2; i < LEN(sensors1_80154CE4); i++) {
                if (g_Player.colWall[sensors1_80154CE4[i]].effects & 3) {
                    break;
                }
            }
            if (i == LEN(sensors1_80154CE4)) {
                DestroyEntity(self);
                return;
            }
            self->posX.i.hi =
                PLAYER.posX.i.hi + g_MmxSensorsWall[sensors1_80154CE4[i]].x;
            self->posY.i.hi =
                PLAYER.posY.i.hi + g_MmxSensorsWall[sensors1_80154CE4[i]].y;
            self->velocityY = FIX(-0.25);
            self->scaleX = self->scaleY = rot_x_80154C74[1] + 0x40;
            self->step++;
            return;
        }
        if (paramsHi == 8) {
            for (i = paramsLo * 2; i < LEN(sensors2_80154CF4); i++) {
                if (g_Player.colWall[sensors2_80154CF4[i]].effects & 3) {
                    break;
                }
            }
            if (i == LEN(sensors2_80154CF4)) {
                DestroyEntity(self);
                return;
            }
            self->posX.i.hi =
                PLAYER.posX.i.hi + g_MmxSensorsWall[sensors2_80154CF4[i]].x;
            self->posY.i.hi =
                PLAYER.posY.i.hi + g_MmxSensorsWall[sensors2_80154CF4[i]].y;
            self->velocityY = velocity_x_80154C5C[paramsLo];
            self->scaleX = self->scaleY = rot_x_80154C74[paramsLo] + 0x20;
            self->step++;
            return;
        }
        if (paramsHi == 1) {
            if (g_Player.vram_flag & 0x8000) {
                posX /= 2;
            }
        }
        if (self->facingLeft) {
            posX = -posX;
        }
        self->posX.i.hi += posX;
        self->posY.i.hi += 0x18;
        self->scaleX = rot_x_80154C74[paramsLo] + 0x40;
        self->velocityY = velocity_x_80154C5C[paramsLo];
        if (paramsHi == 1) {
            self->velocityY = FIX(-0.25);
            RicSetSpeedX(-0x3000);
            self->scaleX = rot_x_80154C74[1] + 0x40;
        }
        if (paramsHi == 5) {
            self->velocityY = velocity_x_80154C5C[4 - paramsLo * 2];
        }
        if (paramsHi == 2) {
            self->velocityY = FIX(-0.5);
            RicSetSpeedX(-0x3000);
            self->scaleX = rot_x_80154C74[1] + 0x40;
        }
        self->scaleX = self->scaleY;
        if (paramsHi == 10) {
            self->posY.i.hi -= 6;
        }
        self->step++;
        return;
    case 1:
        self->opacity += 0xFE;
        self->posY.val += self->velocityY;
        self->posX.val += self->velocityX;
        if (self->poseTimer < 0) {
            DestroyEntity(self);
            return;
        }
        break;
    }
}

void RicEntitySmokePuffWhenSliding(Entity* self) {
    if (self->step == 0) {
        if (!self->facingLeft) {
            self->posX.i.hi += 8;
        } else {
            self->posX.i.hi -= 8;
        }
        self->posY.i.hi += 8;
    }
    RicEntitySmokePuff(self);
}

// Corresponding DRA function is func_8011E4BC
static unkStr_8011E4BC D_80154D00 = {
    0x08, 0xC0, 0x60, 0x00, 0x01, 0x01, 0x0004, 0x0033, 0x0003, 0x08800000};
static unkStr_8011E4BC D_80154D10 = {
    0x10, 0x7F, 0x7F, 0x7F, 0x01, 0x01, 0x0002, 0x0033, 0x0001, 0x0C800000};
static unkStr_8011E4BC D_80154D20 = {
    0x08, 0x7F, 0x7F, 0x7F, 0x02, 0x02, 0x0002, 0x0033, 0x0000, 0x08800000};
static unkStr_8011E4BC D_80154D30 = {
    0x06, 0x7F, 0xFF, 0xFF, 0x01, 0x01, 0x0004, 0x0073, 0x0003, 0x08800000};
static unkStr_8011E4BC D_80154D40 = {
    0x0C, 0xC0, 0x60, 0x00, 0x01, 0x01, 0x0004, 0x0033, 0x0003, 0x08800000};
static unkStr_8011E4BC D_80154D50 = {
    0x0C, 0x7F, 0x00, 0x00, 0x03, 0x03, 0x0002, 0x0002, 0x0004, 0x0C800000};
static unkStr_8011E4BC D_80154D60 = {
    0x08, 0x1F, 0x1F, 0x7F, 0x01, 0x01, 0x0004, 0x0033, 0x0006, 0x0C800000};
static unkStr_8011E4BC D_80154D70 = {
    0x14, 0x7F, 0x7F, 0xC0, 0x01, 0x01, 0xFFFE, 0x0033, 0x0007, 0x0C800000};
static unkStr_8011E4BC D_80154D80 = {
    0x06, 0xC0, 0xC0, 0xC0, 0x02, 0x02, 0x0002, 0x007B, 0x0008, 0x08800000};
static unkStr_8011E4BC D_80154D90 = {
    0x10, 0x7F, 0x7F, 0x7F, 0x01, 0x01, 0x0002, 0x0033, 0x0009, 0x08800000};
static unkStr_8011E4BC* D_80154DA0[] = {
    &D_80154D00, &D_80154D10, &D_80154D20, &D_80154D30, &D_80154D40,
    &D_80154D50, &D_80154D60, &D_80154D70, &D_80154D80, &D_80154D90};
void RicEntityHitByCutBlood(Entity* self) {
    u8 thickness;
    s16 variant;
    s16 velocityScale;
    s16 velocityRnd;
    s16 x;
    s16 y;
    s16 rnd;
    s32 i;
    s32 timer;
    unkStr_8011E4BC* props;
    FakePrim* tilePrim;

    variant = (self->params) >> 8;
    props = D_80154DA0[variant];
    x = self->posX.i.hi;
    y = self->posY.i.hi;
    switch (self->step) {
    case 0:
        self->primIndex = g_api.func_800EDB58(PRIM_TILE_ALT, props->count + 1);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags = props->flags;
        switch (props->unkA) {
        case 8:
            self->ext.et_8011E4BC.unk7C = 0x100;
            break;
        case 7:
            self->ext.et_8011E4BC.unk7C = 0x3F;
            break;
        case 4:
            self->ext.et_8011E4BC.unk7C = 0x2F;
            break;
        case 3:
        case 6:
            self->ext.et_8011E4BC.unk7C = 0x1F;
            break;
        }
        tilePrim = (FakePrim*)&g_PrimBuf[self->primIndex];
        i = 0;
        while (1) {
            tilePrim->drawMode = props->drawMode;
            tilePrim->priority = PLAYER.zPriority + props->priority;
            if (tilePrim->next == NULL) {
                tilePrim->drawMode &= ~DRAW_HIDE;
                tilePrim->y0 = tilePrim->x0 = tilePrim->w = 0;
                break;
            }
            tilePrim->posX.i.hi = x;
            tilePrim->posY.i.hi = y;
            tilePrim->posX.i.lo = tilePrim->posY.i.lo = 0;
            switch (props->unkA) {
            case 0:
                velocityRnd = rand() & PSP_RANDMASK;
                velocityScale = (velocityRnd & 1) + 2;
                tilePrim->velocityX.val = (rcos(velocityRnd) << velocityScale);
                tilePrim->velocityY.val = -(rsin(velocityRnd) << velocityScale);
                break;
            case 1:
            case 9:
                tilePrim->velocityX.val = ((rand() & 0x1FF) - 0x100) << 8;
                tilePrim->velocityY.val = ((rand() & 0x1FF) - 0x100) << 8;
                break;
            case 3:
                tilePrim->posX.i.hi = x + (rand() & 0xF) - 7;
                tilePrim->posY.i.hi = y - (rand() & 0xF);
                tilePrim->velocityY.val = FIX(-0.75) - (rand() & 0x7FFF);
                tilePrim->velocityX.val =
                    self->ext.et_8011E4BC.parent->velocityX;
                tilePrim->delay = (rand() & 0xF) + 0x10;
                break;
            case 4:
                rnd = rand() & 0x1F;
                tilePrim->posX.i.hi = x + rnd - 0x10;
                rnd = rand() & 0x1F;
                tilePrim->posY.i.hi = y + rnd - 0x14;
                rnd = rand() & 0x1F;
                tilePrim->velocityX.val = D_80175958[rnd];
                tilePrim->velocityY.val = D_801759D8[rnd];
                break;
            case 6:
                tilePrim->posX.i.hi = self->posX.i.hi + (rand() & 0xF) - 8;
                tilePrim->posY.i.hi = self->posY.i.hi + (rand() & 0xF) - 4;
                tilePrim->velocityY.val = (rand() & PSP_RANDMASK) + FIX(0.5);
                tilePrim->delay = (rand() & 0xF) + 0x10;
                break;
            case 7:
                tilePrim->posX.i.hi = x;
                tilePrim->posY.i.hi = y;
                if (i < 10) {
                    tilePrim->velocityY.val =
                        -((i * i * FIX(0.09375)) + FIX(0.125));
                } else {
                    tilePrim->velocityY.val =
                        (i - 10) * (i - 10) * FIX(0.09375) + FIX(0.125);
                }
                tilePrim->delay = 0x3F;
                break;
            case 8:
                tilePrim->posX.i.hi = x;
                tilePrim->posY.i.hi = y;
#if defined(VERSION_PSP)
                tilePrim->velocityX.val = ((rand() & 0x7FFF) - FIX(0.25)) >> 1;
#else
                tilePrim->velocityX.val = (rand() - FIX(0.25)) >> 1;
#endif
                tilePrim->velocityY.val = -((rand() & 0x1FFF) + FIX(0.375));
                tilePrim->timer = (i * 4);
                break;
            }
            tilePrim->x0 = tilePrim->posX.i.hi;
            tilePrim->y0 = tilePrim->posY.i.hi;
            tilePrim->r0 = props->r;
            tilePrim->g0 = props->g;
            tilePrim->b0 = props->b;
            tilePrim->w = props->w;
            tilePrim->h = props->h;
            i++;
            tilePrim = tilePrim->next;
        }
        self->step++;
        break;
    case 1:
        thickness = 0;
        timer = self->ext.et_8011E4BC.unk7C;
        switch (props->unkA) {
        case 3:
        case 6:
        case 7:
        case 8:
            if (--self->ext.et_8011E4BC.unk7C == 0) {
                DestroyEntity(self);
                return;
            }
        case 4:
            if (--self->ext.et_8011E4BC.unk7C == 0) {
                DestroyEntity(self);
                return;
            }
            thickness = 3;
            if (timer < 9) {
                thickness--;
            }
            if (timer < 4) {
                thickness--;
            }
        }
        tilePrim = (FakePrim*)&g_PrimBuf[self->primIndex];
        i = 0;
        while (1) {
            if (tilePrim->next == NULL) {
                tilePrim->drawMode &= ~DRAW_HIDE;
                tilePrim->y0 = tilePrim->x0 = tilePrim->w = 0;
                return;
            }
            tilePrim->posX.i.hi = tilePrim->x0;
            tilePrim->posY.i.hi = tilePrim->y0;
            switch (props->unkA) {
            case 0:
                tilePrim->posY.val += tilePrim->velocityY.val;
                tilePrim->posX.val += tilePrim->velocityX.val;
                tilePrim->r0 -= 6;
                tilePrim->g0 -= 6;
                tilePrim->b0 -= 6;
                if (tilePrim->r0 < 8) {
                    DestroyEntity(self);
                    return;
                }
                break;
            case 1:
            case 9:
                tilePrim->posY.val += tilePrim->velocityY.val;
                tilePrim->posX.val += tilePrim->velocityX.val;
                tilePrim->velocityY.val =
                    (tilePrim->velocityY.val + FIX(0.15625));
                tilePrim->r0 -= 3;
                tilePrim->g0 -= 3;
                tilePrim->b0 -= 3;
                if (tilePrim->r0 < 8) {
                    DestroyEntity(self);
                    return;
                }
                break;
            case 3:
            case 6:
            case 7:
                tilePrim->posY.val += tilePrim->velocityY.val;
                if (--tilePrim->delay < 0) {
                    tilePrim->drawMode |= DRAW_HIDE;
                }
                break;
            case 4:
                tilePrim->posX.val += tilePrim->velocityX.val;
                tilePrim->posY.val += tilePrim->velocityY.val;
                // There is probably a clever way to write this
                tilePrim->velocityY.val -= tilePrim->velocityY.val >> 5;
                if (!(timer & 7)) {
                    tilePrim->velocityX.val >>= 1;
                    tilePrim->velocityY.val >>= 1;
                    if (timer & 0x20) {
                        tilePrim->velocityY.val >>= 1;
                    }
                    if (timer == 0x18) {
                        tilePrim->drawMode = DRAW_UNK02;
                    }
                }
                tilePrim->w = tilePrim->h = thickness;
                break;
            case 8:
                if (tilePrim->timer == 0) {
                    tilePrim->drawMode &= ~DRAW_HIDE;
                    tilePrim->r0 -= 1;
                    tilePrim->g0 -= 1;
                    tilePrim->b0 -= 1;
                    tilePrim->posY.val += tilePrim->velocityY.val;
                    tilePrim->posX.val += tilePrim->velocityX.val;
                    if (*D_80097448 == 0 ||
                        !(tilePrim->posY.i.hi >
                          (PLAYER.posY.i.hi - *D_80097448 + 0x19))) {
                        tilePrim->drawMode |= DRAW_HIDE;
                    }
                } else {
                    tilePrim->timer--;
                }
                break;
            }
            tilePrim->x0 = tilePrim->posX.i.hi;
            tilePrim->y0 = tilePrim->posY.i.hi;
            i++;
            tilePrim = tilePrim->next;
        }
    }
}

// DRA function is func_8011EDA8
static AnimationFrame anim_80154DC8[] = {
    POSE(2, 1, 0), POSE(2, 2, 0), POSE(2, 3, 0), POSE(2, 4, 0), POSE(2, 5, 0),
    POSE(2, 4, 0), POSE(2, 3, 0), POSE(2, 4, 0), POSE(2, 3, 0), POSE(2, 4, 0),
    POSE(2, 5, 0), POSE(1, 6, 0), POSE(1, 7, 0), POSE(1, 8, 0), POSE_END};
static AnimationFrame anim_80154E04[] = {
    POSE(1, 9, 0),  POSE(2, 10, 0), POSE(2, 11, 0), POSE(2, 12, 0),
    POSE(2, 13, 0), POSE(2, 14, 0), POSE(2, 15, 0), POSE(2, 16, 0),
    POSE(2, 17, 0), POSE(2, 18, 0), POSE(3, 19, 0), POSE(4, 20, 0),
    POSE_END};
void func_80161C2C(Entity* self) {
    u16 params = self->params;
    s16 paramsHi = self->params >> 8;
    s32 step = self->step;
    s32 rnd;

    switch (step) {
    case 0:
        if (paramsHi == 1) {
            self->scaleX = 0xC0;
            self->scaleY = 0xC0;
            self->drawFlags = FLAG_DRAW_SCALEX | FLAG_DRAW_SCALEY;
            self->animSet = ANIMSET_DRA(2);
            self->anim = anim_80154E04;
        }

        if ((paramsHi == 0) || (paramsHi == 2)) {
            if (params & 3) {
                self->anim = anim_80154DC8;
                self->scaleX = 0x120;
                self->scaleY = 0x120;
                self->drawFlags = FLAG_DRAW_SCALEX | FLAG_DRAW_SCALEY;
                self->animSet = ANIMSET_DRA(2);
            } else {
                self->animSet = ANIMSET_DRA(5);
                self->anim = anim_smoke_puff;
                self->palette = 0x8170;
            }
        }
        self->flags = FLAG_UNK_20000 | FLAG_UNK_100000 | FLAG_POS_CAMERA_LOCKED;

        if (rand() % 4) {
            self->zPriority = PLAYER.zPriority + 2;
        } else {
            self->zPriority = PLAYER.zPriority - 2;
        }

        if (paramsHi == 2) {
            self->posX.i.hi = PLAYER.posX.i.hi + (rand() % 44) - 22;
        } else {
            self->posX.i.hi = PLAYER.posX.i.hi + (rand() & 15) - 8;
        }

        rnd = rand() & 31;
        self->posY.i.hi = PLAYER.posY.i.hi + PLAYER.hitboxOffY + rnd - 16;
        self->velocityY = FIX(-0.5);
        self->velocityX = PLAYER.velocityX >> 2;
        self->step++;
        break;

    case 1:
        self->scaleX -= 4;
        self->scaleY -= 4;
        self->posY.val += self->velocityY;
        self->posX.val += self->velocityX;
        if ((self->pose == 8) && (self->anim != anim_smoke_puff)) {
            self->drawMode = DRAW_TPAGE;
            if (!(params & 1) && (self->poseTimer == step)) {
                RicCreateEntFactoryFromEntity(self, FACTORY(BP_EMBERS, 4), 0);
            }
        }

        if ((self->pose == 16) && (self->anim == anim_smoke_puff)) {
            self->drawMode = DRAW_TPAGE;
        }

        if (self->poseTimer < 0) {
            DestroyEntity(self);
        }
        break;
    }
}

static AnimationFrame anim_80154E38[] = {
    POSE(2, 1, 0), POSE(2, 2, 0), POSE(2, 3, 0), POSE(2, 4, 0), POSE(2, 5, 0),
    POSE(2, 6, 0), POSE(2, 7, 0), POSE(2, 8, 0), POSE_END};
void func_80161EF8(Entity* self) {
    switch (self->step) {
    case 0:
        self->animSet = ANIMSET_DRA(2);
        self->anim = anim_80154E38;
        self->flags = FLAG_UNK_20000 | FLAG_UNK_100000 | FLAG_UNK_10000 |
                      FLAG_POS_PLAYER_LOCKED;
        self->zPriority = PLAYER.zPriority + 4;
        self->velocityY = (rand() & 0x3FFF) - 0x10000;
        self->step++;
        break;
    case 1:
        if ((self->pose == 6) && (self->poseTimer == self->step) &&
            (rand() & 1)) {
            RicCreateEntFactoryFromEntity(self, BP_EMBERS, 0);
        }
        self->posY.val += self->velocityY;
        if (self->poseTimer < 0) {
            DestroyEntity(self);
        }
        break;
    }
}

typedef struct {
    s16 xPos;
    s16 yPos;
    s32 velocityX;
    s32 velocityY;
    s16 timerInit;
    s16 tpage;
    s16 clut;
    u8 uBase;
    u8 vBase;
} Props_80161FF0; // size = 0x14
static Props_80161FF0 D_80154E5C[] = {
    {-0x40, 0, +FIX(2.5), FIX(0), 0x0060, 0x1B, 0x0118, 128, 0},
    {+0x40, 0, -FIX(2.5), FIX(0), 0x0048, 0x1B, 0x0119, 0, 128},
    {0, -0x40, FIX(0), +FIX(2.5), 0x0030, 0x19, 0x011A, 0, 0},
    {0, +0x40, FIX(0), -FIX(2.5), 0x0018, 0x19, 0x011B, 128, 0}};
void RicEntityApplyMariaPowerAnim(Entity* self) {
    Primitive* prim;

    u16 posX = self->posX.i.hi;
    u16 posY = self->posY.i.hi;
    Props_80161FF0* props = &D_80154E5C[(s16)self->params];

    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, 1);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        g_api.PlaySfx(0x881);
        self->ext.circleExpand.width = 0x100;
        prim = &g_PrimBuf[self->primIndex];
        prim->u0 = props->uBase;
        prim->v0 = props->vBase;
        prim->u1 = props->uBase + 0x7F;
        prim->v1 = props->vBase;
        prim->u2 = props->uBase;
        prim->v2 = props->vBase + 0x6F;
        prim->u3 = props->uBase + 0x7F;
        prim->v3 = props->vBase + 0x6F;
        prim->tpage = props->tpage;
        prim->clut = props->clut;
        prim->priority = PLAYER.zPriority + 8;
        prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_TRANSP;
        self->velocityX = props->velocityX;
        self->velocityY = props->velocityY;
        self->posX.i.hi += props->xPos;
        posX = self->posX.i.hi;
        posY = self->posY.i.hi + props->yPos;
        self->posY.i.hi = posY;
        self->flags =
            FLAG_KEEP_ALIVE_OFFCAMERA | FLAG_HAS_PRIMS | FLAG_UNK_10000;
        self->step++;
        break;
    case 1:
        self->ext.circleExpand.width -= 8;
        self->posX.val += self->velocityX;
        self->posY.val += self->velocityY;
        if (self->ext.circleExpand.width < 25) {
            self->ext.circleExpand.height = props->timerInit;
            self->step++;
        }
        break;
    case 2:
        if (--self->ext.circleExpand.height == 0) {
            self->step++;
        }
        break;
    case 3:
        self->ext.circleExpand.width -= 2;
        if (self->ext.circleExpand.width < 0) {
            DestroyEntity(self);
            return;
        }
        break;
    }
    prim = &g_PrimBuf[self->primIndex];
    prim->x0 =
        posX + (((rcos(0x600) >> 4) * self->ext.circleExpand.width) >> 8);
    prim->y0 =
        posY - (((rsin(0x600) >> 4) * self->ext.circleExpand.width) >> 8);
    prim->x1 =
        posX + (((rcos(0x200) >> 4) * self->ext.circleExpand.width) >> 8);
    prim->y1 =
        posY - (((rsin(0x200) >> 4) * self->ext.circleExpand.width) >> 8);
    prim->x2 =
        posX + (((rcos(0xA00) >> 4) * self->ext.circleExpand.width) >> 8);
    prim->y2 =
        posY - (((rsin(0xA00) >> 4) * self->ext.circleExpand.width) >> 8);
    prim->x3 =
        posX + (((rcos(0xE00) >> 4) * self->ext.circleExpand.width) >> 8);
    prim->y3 =
        posY - (((rsin(0xE00) >> 4) * self->ext.circleExpand.width) >> 8);
    return;
}

void func_801623E0(Entity* entity) {
    Primitive* prim;
    s16 primIndex;

    entity->posX.val = g_Entities->posX.val;
    entity->posY.val = PLAYER.posY.val;
    switch (entity->step) {
    case 0:
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 1);
        entity->primIndex = primIndex;
        if (primIndex == -1) {
            DestroyEntity(entity);
            return;
        }
        entity->ext.circleExpand.height = 32;
        entity->ext.circleExpand.width = 32;
        prim = &g_PrimBuf[entity->primIndex];
        prim->u2 = 64;
        prim->u0 = 64;
        prim->v1 = 192;
        prim->v0 = 192;
        prim->u3 = 127;
        prim->u1 = 127;
        prim->v3 = 255;
        prim->v2 = 255;
        prim->tpage = 0x1A;
        prim->clut = 0x13E;
        prim->priority = PLAYER.zPriority + 8;
        prim->drawMode = DRAW_DEFAULT;
        entity->flags = FLAG_UNK_10000 | FLAG_POS_PLAYER_LOCKED |
                        FLAG_KEEP_ALIVE_OFFCAMERA | FLAG_HAS_PRIMS;
        entity->step++;
        break;

    case 1:
        entity->ext.circleExpand.width++;
        entity->ext.circleExpand.height++;
        if (entity->ext.circleExpand.width >= 45) {
            DestroyEntity(entity);
            return;
        }
        break;
    }

    prim = &g_PrimBuf[entity->primIndex];
    prim->x0 = entity->posX.i.hi - entity->ext.circleExpand.width;
    prim->y0 = entity->posY.i.hi - entity->ext.circleExpand.height;
    prim->x1 = entity->posX.i.hi + entity->ext.circleExpand.width;
    prim->y1 = entity->posY.i.hi - entity->ext.circleExpand.height;
    prim->x2 = entity->posX.i.hi - entity->ext.circleExpand.width;
    prim->y2 = entity->posY.i.hi + entity->ext.circleExpand.height;
    prim->x3 = entity->posX.i.hi + entity->ext.circleExpand.width;
    prim->y3 = entity->posY.i.hi + entity->ext.circleExpand.height;
    prim->clut = (LOH(g_Timer) & 1) + 0x13E;
}

void func_80162604(Entity* self) {
    Primitive* prim;

    self->posX.val = PLAYER.posX.val;
    self->posY.val = PLAYER.posY.val;
    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, 1);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->ext.circleExpand.width = self->ext.circleExpand.height = 0;
        prim = &g_PrimBuf[self->primIndex];

        prim->u0 = prim->u2 = 0;
        prim->v0 = prim->v1 = 192;
        prim->u1 = prim->u3 = 63;
        prim->v2 = prim->v3 = 255;
        prim->tpage = 0x1A;

        prim->clut = 0x162;
        prim->priority = PLAYER.zPriority - 4;
        prim->drawMode = DRAW_DEFAULT;
        self->flags = FLAG_UNK_10000 | FLAG_POS_PLAYER_LOCKED |
                      FLAG_KEEP_ALIVE_OFFCAMERA | FLAG_HAS_PRIMS;
        self->step++;
        break;
    case 1:
        self->ext.circleExpand.width += 8;
        self->ext.circleExpand.height += 8;
        if (self->ext.circleExpand.width >= 0x20) {
            self->step++;
        }
        break;
    case 2:
        self->step++;
        break;
    case 3:
        self->ext.circleExpand.width -= 8;
        self->ext.circleExpand.height -= 8;
        if (self->ext.circleExpand.width < 5) {
            DestroyEntity(self);
            return;
        }
        break;
    }
    prim = &g_PrimBuf[self->primIndex];
    prim->x0 = self->posX.i.hi - self->ext.circleExpand.width;
    prim->y0 = self->posY.i.hi - self->ext.circleExpand.height;
    prim->x1 = self->posX.i.hi + self->ext.circleExpand.width;
    prim->y1 = self->posY.i.hi - self->ext.circleExpand.height;
    prim->x2 = self->posX.i.hi - self->ext.circleExpand.width;
    prim->y2 = self->posY.i.hi + self->ext.circleExpand.height;
    prim->x3 = self->posX.i.hi + self->ext.circleExpand.width;
    prim->y3 = self->posY.i.hi + self->ext.circleExpand.height;
}

static s16 D_80154EAC[] = {0x016E, 0x0161, 0x0160, 0x0162};
// 0xFFFF2AAB = -FIX(1. / 3)
// 0xFFFDAAAB = -FIX(5. / 3)
static s32 D_80154EB4[] = {FIX(5. / 3), -FIX(5. / 3), FIX(1. / 3), -0xD555};
static s32 D_80154EC4[] = {-FIX(2), -FIX(5. / 3), -FIX(3), -0x25555};
void RicEntityMariaPowers(Entity* self) {
    Primitive* prim;
    s16 params;

    params = self->params;
    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, 1);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->unk5A = 0x66;
        self->zPriority = PLAYER.zPriority - 12;
        self->palette = params + 0x149;
        self->animSet = ANIMSET_OVL(0x13);
        self->animCurFrame = params + 0x19;
        self->velocityX = D_80154EB4[params];
        self->velocityY = D_80154EC4[params];
        self->ext.et_80162870.unk7C = self->ext.et_80162870.unk7E =
            self->ext.et_80162870.unk80 = 8;

        prim = &g_PrimBuf[self->primIndex];
        prim->u0 = prim->v0 = prim->v1 = prim->u2 = 0;
        prim->u1 = prim->v2 = prim->u3 = prim->v3 = 0x1F;
        prim->tpage = 0x1A;
        prim->clut = D_80154EAC[params];
        prim->priority = PLAYER.zPriority - 16;
        prim->drawMode = DRAW_HIDE;
        self->flags = FLAG_HAS_PRIMS | FLAG_UNK_10000;
        if (params == 3) {
            self->flags |= FLAG_KEEP_ALIVE_OFFCAMERA;
        }
        g_api.PlaySfx(0x881);
        self->ext.et_80162870.unk82 = 12;
        self->step++;
        break;
    case 1:
        self->posX.val += self->velocityX;
        self->posY.val += self->velocityY;
        if (--self->ext.et_80162870.unk82 == 0) {
            self->drawFlags = 3;
            self->scaleX = self->scaleY = 0x100;
            self->ext.et_80162870.unk82 = 0x10;
            self->step++;
            g_PrimBuf[self->primIndex].drawMode =
                DRAW_TPAGE2 | DRAW_TPAGE | DRAW_TRANSP;
        }
        break;
    case 2:
        self->scaleX = self->scaleY = self->ext.et_80162870.unk82 * 0x10;
        if (--self->ext.et_80162870.unk82 == 0) {
            self->animCurFrame = 0;
            g_api.PlaySfx(0x69D);
            self->velocityY = FIX(-9);
            self->step++;
        }
        break;
    case 3:
        self->posY.val += self->velocityY;
        if (self->ext.et_80162870.unk7C > 0 && !(g_Timer & 1)) {
            self->ext.et_80162870.unk7C--;
        }
        if (!(g_Timer & 1)) {
            self->ext.et_80162870.unk7E++;
        }
        self->ext.et_80162870.unk80 += 2;
        if ((params == 3) && (self->posY.i.hi < -0x20)) {
            D_801545AC = 8;
            DestroyEntity(self);
            return;
        }
        break;
    }
    prim = &g_PrimBuf[self->primIndex];
    prim->x0 = self->posX.i.hi - self->ext.et_80162870.unk7C;
    prim->y0 = self->posY.i.hi - self->ext.et_80162870.unk7E;
    prim->x1 = self->posX.i.hi + self->ext.et_80162870.unk7C;
    prim->y1 = self->posY.i.hi - self->ext.et_80162870.unk7E;
    prim->x2 = self->posX.i.hi - self->ext.et_80162870.unk7C;
    prim->y2 = self->posY.i.hi + self->ext.et_80162870.unk80;
    prim->x3 = self->posX.i.hi + self->ext.et_80162870.unk7C;
    prim->y3 = self->posY.i.hi + self->ext.et_80162870.unk80;
}

void RicEntityNotImplemented4(Entity* self) {}

static AnimationFrame anim_maria_walk[] = {
    POSE(4, 1, 0), POSE(4, 2, 0), POSE(4, 3, 0), POSE(4, 4, 0), POSE(4, 5, 0),
    POSE(4, 6, 0), POSE(4, 7, 0), POSE(4, 8, 0), POSE_LOOP(0)};
static AnimationFrame anim_maria_offering_powers[] = {
    POSE(8, 9, 0),   POSE(8, 13, 0),  POSE(64, 10, 0),  POSE(2, 11, 0),
    POSE(2, 12, 0),  POSE(6, 13, 0),  POSE(7, 14, 0),   POSE(6, 15, 0),
    POSE(5, 14, 0),  POSE(4, 13, 0),  POSE(3, 15, 0),   POSE(3, 14, 0),
    POSE(3, 13, 0),  POSE(3, 14, 0),  POSE(3, 15, 0),   POSE(3, 14, 0),
    POSE(3, 13, 0),  POSE(4, 14, 0),  POSE(5, 15, 0),   POSE(6, 14, 0),
    POSE(7, 13, 0),  POSE(48, 14, 0), POSE(12, 9, 0),   POSE(13, 16, 0),
    POSE(8, 17, 0),  POSE(12, 18, 0), POSE(176, 19, 0), POSE(10, 20, 0),
    POSE(10, 21, 0), POSE(10, 22, 0), POSE(48, 23, 0),  POSE(208, 24, 0),
    POSE_END};
void RicEntityMaria(Entity* entity) {
    switch (entity->step) {
    case 0:
        entity->flags = FLAG_UNK_100000 | FLAG_KEEP_ALIVE_OFFCAMERA |
                        FLAG_UNK_10000 | FLAG_POS_CAMERA_LOCKED;
        entity->facingLeft = 1;
        entity->unk5A = 0x66;
        entity->zPriority = PLAYER.zPriority - 8;
        entity->palette = PAL_FLAG(0x149);
        entity->animSet = ANIMSET_OVL(19);
        RicSetAnimation(anim_maria_walk);
        entity->velocityX = FIX(-1.75);
        entity->posY.i.hi = 0xBB;
        entity->posX.i.hi = 0x148;
        entity->ext.circleExpand.height = 0;
        entity->step++;
        break;
    case 1:
        if (entity->pose == 0 && entity->poseTimer == 1) {
            g_api.PlaySfx(0x882);
        }
        if (entity->pose == 4 && entity->poseTimer == 1) {
            g_api.PlaySfx(0x883);
        }

        entity->posX.val += entity->velocityX;
        if (((s16)entity->ext.circleExpand.height == 0) &&
            (entity->posX.i.hi < 256)) {
            g_api.PlaySfx(0x87D);
            entity->ext.circleExpand.height++;
        }
        if (entity->posX.i.hi < 0xE0) {
            RicSetAnimation(anim_maria_offering_powers);
            entity->velocityX = 0;
            entity->step++;
            RicCreateEntFactoryFromEntity(entity, FACTORY(BP_SKID_SMOKE, 4), 0);
        }
        break;
    case 2:
        if (entity->pose == 16) {
            g_api.PlaySfx(0x87E);
            entity->ext.circleExpand.width = 0x80;
            entity->step++;
        }
        break;
    case 3:
        if (!--entity->ext.circleExpand.width) {
            RicCreateEntFactoryFromEntity(entity, BP_MARIA_POWERS_INVOKED, 0);
            entity->step++;
        }
        break;
    case 4:
        break;
    }
}
