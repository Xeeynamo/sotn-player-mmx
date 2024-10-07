// SPDX-License-Identifier: AGPL-3.0-or-later
#include "pl.h"
#include "player.h"
#include "sfx.h"

#ifndef HARD_LINK
void DestroyEntity(Entity* entity) {
    s32 i;
    s32 length;
    u32* ptr;

    if (entity->flags & FLAG_HAS_PRIMS) {
        g_api.FreePrimitives(entity->primIndex);
    }

    ptr = (u32*)entity;
    length = sizeof(Entity) / sizeof(u32);
    for (i = 0; i < length; i++)
        *ptr++ = 0;
}
#endif

static void RicHandleStand(void);
static void RicHandleWalk(void);
static void RicHandleCrouch(void);
static void RicHandleFall(void);
void RicHandleJump(void);
static void RicHandleRun(void);
static void RicHandleBossGrab(void);
static void RicHandleStandInAir(void);
static void RicHandleEnableFlameWhip(void);
static void RicHandleHydrostorm(void);
static void RicHandleGenericSubwpnCrash(void);
static void RicHandleThrowDaggers(void);
static void RicHandleDeadPrologue(void);
static void RicHandleSlide(void);
static void RicHandleSlideKick(void);
static void RicHandleHighJump(void);
void RicSetDeadPrologue(void);
static void func_8015BCD0(void);
int RicDoCrash(void);
static bool MmxIsHuggingWall();
void MmxSetJump(int jumpDash);
static void MmxHandleJump(void);
static void RicHandleFall(void);
void MmxSetDash(void);
static void MmxHandleDash(void);
static void MmxHandleDashAir(void);
static void MmxSetWall(void);
static void MmxHandleWall(void);
void DisableAfterImage(s32 resetAnims, s32 arg1);
void RicSetInvincibilityFrames(s32 kind, s16 invincibilityFrames);
void RicHandleHit(s32 damageEffect, u32 damageKind, s16 prevStep);
void RicHandleDead(
    s32 damageEffects, s32 damageKind, s32 prevStep, s32 prevStepS);

static TeleportCheck GetTeleportToOtherCastle(void) {
    // Is player in the pose when pressing UP?
    if (PLAYER.step != PL_S_STAND || PLAYER.step_s != 1) {
        return TELEPORT_CHECK_NONE;
    }

    // Check for X/Y boundaries in TOP
    if (g_StageId == STAGE_TOP) {
        if (abs((g_Tilemap.left << 8) + g_PlayerX - 8000) < 4 &&
            abs((g_Tilemap.top << 8) + g_PlayerY - 2127) < 4) {
            return TELEPORT_CHECK_TO_RTOP;
        }
    }

    // Check for X/Y boundaries in RTOP
    if (g_StageId == STAGE_RTOP) {
        if (abs((g_Tilemap.left << 8) + g_PlayerX - 8384) < 4 &&
            abs((g_Tilemap.top << 8) + g_PlayerY) - 14407 < 4) {
            return TELEPORT_CHECK_TO_TOP;
        }
    }

    return TELEPORT_CHECK_NONE;
}

static s16 func_80156DE4(void) {
    // Variables that change during execution
    Collider collider;
    s32 yvar;
    s32 collisions;
    s32 i;
    // Values that are set once and never again (but not const for some reason)
    s32 xpos = PLAYER.posX.i.hi;
    s32 xp4 = xpos + 4;
    s32 xm4 = xpos - 4;
    s32 coll_flags = EFFECT_SOLID_FROM_ABOVE | EFFECT_SOLID;

    for (i = 0; i < 3; i++) {
        yvar = PLAYER.posY.i.hi + D_80154568[i];
        g_api.CheckCollision(xpos, yvar, &collider, 0);
        collisions = 0;
        if ((collider.effects & coll_flags) == EFFECT_SOLID) {
            collisions += 1;
        }
        g_api.CheckCollision(xp4, yvar, &collider, 0);
        if ((collider.effects & coll_flags) == EFFECT_SOLID) {
            collisions += 1;
        }
        g_api.CheckCollision(xm4, yvar, &collider, 0);
        if ((collider.effects & coll_flags) == EFFECT_SOLID) {
            collisions += 1;
        }
        if (collisions != 0) {
            return i + 1;
        }
    }
    return 0;
}

static void RicDebugOff();

// Duplicate of DRA func_80109594
void RicInit(s16 isPrologue) {
    Entity* e;
    s32 radius;
    s32 intensity;
    s32 primIndex;
    Primitive* prim;
    s32 i;
    s32 val;
    s32 memset_len;
    s32* memset_ptr;
    SpriteParts** spriteptr;
    Entity* playerPtr = &PLAYER;

    DestroyEntity(&PLAYER);
    PLAYER.posX.val = FIX(32);
    PLAYER.posY.val = FIX(32);
    PLAYER.animSet = ANIMSET_OVL(0x10);
    PLAYER.palette = PAL_PLAYER | 0x8000;
    PLAYER.rotX = PLAYER.rotY = 0x100;
    PLAYER.facingLeft = 0;
    PLAYER.rotPivotY = 0x18;
    PLAYER.zPriority = g_unkGraphicsStruct.g_zEntityCenter;
    memset_len = sizeof(PlayerState) / sizeof(s32);
    memset_ptr = &g_Player;
    for (i = 0; i < memset_len; i++) {
        *memset_ptr++ = 0;
    }
    g_Player.unk04 = 1;
    g_Player.pl_vram_flag = 1;
    RicSetStand(0);
    MmxSetAnimation(PL_A_DUMMY);
    g_Player.unk5C = isPrologue;
    if (g_StageId == STAGE_ST0) {
        g_IsPrologueStage = true;
    } else {
        g_IsPrologueStage = false;
    }
    g_PlayerDraw->enableColorBlend = 0;
    RicDebugOff();
    for (i = 0; i < LEN(D_80175958); i++) {
        radius = (rand() & 0x3FF) + 0x100;
        intensity = (rand() & 0xFF) + 0x100;
        val = rcos(radius) * 0x10;
        D_80175958[i] = +((val * intensity) >> 8);
        val = rsin(radius) * 0x10;
        D_801759D8[i] = -((val * intensity) >> 7);
    }
    spriteptr = g_api.o.spriteBanks;
    spriteptr += 0x10;
    *spriteptr++ = g_MmxPlSprites;
    *spriteptr++ = g_SpritesWeapons;
    *spriteptr++ = g_SpritesItems;
    *spriteptr++ = D_801541A8;
    for (e = &g_Entities[1], i = 0; i < 3; i++, e++) {
        DestroyEntity(e);
        e->animSet = ANIMSET_OVL(0x10);
        e->unk5A = i + 1;
        e->palette = 0x8120;
        e->flags = FLAG_UNK_20000 | FLAG_POS_CAMERA_LOCKED;
    }
    primIndex = g_api.AllocPrimitives(PRIM_TILE, 6);

    g_Entities[1].primIndex = primIndex;
    g_Entities[1].flags |= FLAG_HAS_PRIMS;
    for (prim = &g_PrimBuf[primIndex]; prim != NULL; prim = prim->next) {
        prim->drawMode = 0x102 | DRAW_HIDE;
    }
    if (D_80097C98 == 6) {
        RicCreateEntFactoryFromEntity(playerPtr, FACTORY(BP_TELEPORT, 1), 0);
        func_8015CC70(1);
    }
    if (D_80097C98 == 4) {
        RicCreateEntFactoryFromEntity(playerPtr, FACTORY(BP_TELEPORT, 3), 0);
        func_8015CC70(3);
    }
    if (D_80097C98 == 5) {
        RicCreateEntFactoryFromEntity(playerPtr, FACTORY(BP_TELEPORT, 5), 0);
        func_8015CC70(5);
    }
}

static void func_801572A8(bool arg0) {
    Collider collider;
    s16 argX;
    s16 argY;
    s32 xVel;
    s32 i;
    s32 j;
    s32 unk0C;

    s32* vram_ptr = &g_Player.pl_vram_flag;
    s32* unk04_ptr = &g_Player.unk04;

    g_Player.unk04 = *vram_ptr;
    *vram_ptr = 0;
    unk0C = g_Player.status;

    if (arg0) {
        for (i = 0; i < LEN(D_801545E4); i++) {
            if (unk0C & 0x20) {
                D_801545F4[i].y = D_80154644[i];
                D_801545E4[i].y = D_8015465C[i];
            } else {
                D_801545F4[i].y = D_80154644[i];
                D_801545E4[i].y = D_8015463C[i];
            }
        }
        for (i = 0; i < 7; i++) {
            if (unk0C & 0x20) {
                D_80154604[i].y = D_80154664[i];
                D_80154604[i + 7].y = D_80154664[i];
            } else {
                D_80154604[i].y = D_8015464C[i];
                D_80154604[i + 7].y = D_8015464C[i];
            }
        }
    }
    xVel = PLAYER.velocityX;
    if (PLAYER.velocityX < 0) {
        if (!(*unk04_ptr & 8)) {
            if ((*unk04_ptr & 0xF000) == 0xC000) {
                xVel = xVel * 10 / 16;
            }
            if ((*unk04_ptr & 0xF000) == 0xD000) {
                xVel = xVel * 13 / 16;
            }
            PLAYER.posX.val += xVel;
        }
    }
    if (PLAYER.velocityX > 0) {
        if (!(*unk04_ptr & 4)) {
            if ((*unk04_ptr & 0xF000) == 0x8000) {
                xVel = xVel * 10 / 16;
            }
            if ((*unk04_ptr & 0xF000) == 0x9000) {
                xVel = xVel * 13 / 16;
            }
            PLAYER.posX.val += xVel;
        }
    }
    if ((PLAYER.velocityY < 0) && !(*unk04_ptr & 2)) {
        PLAYER.posY.val += PLAYER.velocityY;
    }
    if ((PLAYER.velocityY > 0) && !(*unk04_ptr & 1)) {
        PLAYER.posY.val += PLAYER.velocityY;
    }
    for (i = 0; i < 4; i++) {
        argX = PLAYER.posX.i.hi + D_801545F4[i].x;
        argY = PLAYER.posY.i.hi + D_801545F4[i].y;
        g_api.CheckCollision(argX, argY, &g_Player.colliders[i], 0);
        if (g_Player.timers[PL_T_7] && (g_Player.colliders[i].effects & 0x40)) {
            g_api.CheckCollision(argX, argY + 0xC, &collider, 0);
            if (!(collider.effects & EFFECT_SOLID)) {
                g_Player.colliders[i].effects = 0;
            }
        }
    }
    func_8015E800();
    for (i = 0; i < 4; i++) {
        argX = PLAYER.posX.i.hi + D_801545E4[i].x;
        argY = PLAYER.posY.i.hi + D_801545E4[i].y;
        g_api.CheckCollision(argX, argY, &g_Player.colliders2[i], 0);
    }
    func_8015EE28();
    if ((*vram_ptr & 1) && (PLAYER.velocityY >= 0)) {
        PLAYER.posY.i.lo = 0;
    }
    if ((*vram_ptr & 2) && (PLAYER.velocityY <= 0)) {
        PLAYER.posY.i.lo = 0;
    }
    for (i = 0; i < 14; i++) {
        argX = PLAYER.posX.i.hi + D_80154604[i].x;
        argY = PLAYER.posY.i.hi + D_80154604[i].y;
        g_api.CheckCollision(argX, argY, &g_Player.colliders3[i], 0);
    }
    func_8015F414();
    func_8015F680();
    if ((*vram_ptr & 4) && (PLAYER.velocityX > 0)) {
        PLAYER.posX.i.lo = 0;
    }
    if ((*vram_ptr & 8) && (PLAYER.velocityX < 0)) {
        PLAYER.posX.i.lo = 0;
    }
}

static void CheckBladeDashInput(void) {
    s32 up;
    s32 down;
    s32 temp_down = PAD_DOWN;
    s32 directionsPressed =
        g_Player.padPressed & (PAD_UP | PAD_RIGHT | PAD_DOWN | PAD_LEFT);

    s32 down_forward = temp_down + (!PLAYER.facingLeft ? PAD_RIGHT : PAD_LEFT);
    up = PAD_UP;
    down = PAD_DOWN;

    switch (g_bladeDashButtons.buttonsCorrect) {
    case 0:
        if (g_Player.padTapped == up) {
            g_bladeDashButtons.timer = 20;
            g_bladeDashButtons.buttonsCorrect++;
        }
        break;
    case 1:
        if (directionsPressed == down) {
            g_bladeDashButtons.timer = 20;
            g_bladeDashButtons.buttonsCorrect++;
            break;
        }
        if (--g_bladeDashButtons.timer == 0) {
            g_bladeDashButtons.buttonsCorrect = 0;
        }
        break;
    case 2:
        if (directionsPressed == down_forward) {
            g_bladeDashButtons.timer = 20;
            g_bladeDashButtons.buttonsCorrect++;
            break;
        }
        if (--g_bladeDashButtons.timer == 0) {
            g_bladeDashButtons.buttonsCorrect = 0;
        }
        break;
    case 3:
        if (--g_bladeDashButtons.timer == 0) {
            g_bladeDashButtons.buttonsCorrect = 0;
        }
        if (PLAYER.step == PL_S_STAND || PLAYER.step == PL_S_WALK ||
            PLAYER.step == PL_S_CROUCH ||
            (PLAYER.step == PL_S_FALL || PLAYER.step == PL_S_JUMP)) {
            if (g_Player.unk72) {
                g_bladeDashButtons.buttonsCorrect = 0;
            } else if (
                (g_Player.unk46 == 0) && (g_Player.padTapped & PAD_SQUARE)) {
                RicSetBladeDash();
            }
        }
        break;
    }
}

static void CheckHighJumpInput(void) {
    switch (D_801758E4.buttonsCorrect) {
    case 0:
        if (g_Player.padTapped & PAD_DOWN) {
            if (g_Player.padHeld == 0) {
                D_801758E4.timer = 16;
                D_801758E4.buttonsCorrect++;
                return;
            }
        }
        return;
    case 1:
        if (g_Player.padTapped & PAD_UP) {
            D_801758E4.timer = 16;
            D_801758E4.buttonsCorrect++;
            return;
        }
        if (--D_801758E4.timer == 0) {
            D_801758E4.buttonsCorrect = 0;
        }
        break;
    case 2:
        if ((D_801758E4.timer != 0) && (--D_801758E4.timer == 0)) {
            D_801758E4.buttonsCorrect = 0;
            return;
        }
        if ((g_Player.padTapped & PAD_CROSS) && (g_Player.unk46 == 0) &&
            ((PLAYER.step == PL_S_CROUCH) || (PLAYER.step == PL_S_STAND) ||
             ((PLAYER.step == PL_S_JUMP) && (PLAYER.velocityY > FIX(1))) ||
             (PLAYER.step == PL_S_FALL))) {
            if (!g_Player.unk72) {
                RicSetHighJump();
            }
            D_801758E4.buttonsCorrect = 0;
        }
        break;
    }
}

static void CheckHadoukenInput(void) { return; }

static enum MmxAnims cur_anim = PL_A_STAND;
static void ChangeAnimToAttack() {
    if (cur_anim >= PL_A_STAND && cur_anim <= PL_A_WALL) {
        if (cur_anim == PL_A_STAND) {
            // The stand animation is longer than the attack one, so we need to
            // reset the animation before switching. All the other animations
            // have the same length, so we can just flip PLAYER.anim
            PLAYER.animFrameDuration = 0;
            PLAYER.animFrameIdx = 0;
        }
        cur_anim += (PL_A_STAND_W - PL_A_STAND);
        PLAYER.anim = mmx_anims[cur_anim];
    }
}
static void RevertAnimFromAttack() {
    if (cur_anim >= PL_A_STAND_W && cur_anim <= PL_A_WALL_W) {
        cur_anim -= (PL_A_STAND_W - PL_A_STAND);
        PLAYER.anim = mmx_anims[cur_anim];
    }
}
void MmxSetAnimation(enum MmxAnims anim) {
    cur_anim = anim;
    PLAYER.anim = mmx_anims[anim];
    PLAYER.animFrameDuration = 0;
    PLAYER.animFrameIdx = 0;
    if (g_Player.timers[PL_T_ATTACK]) {
        ChangeAnimToAttack();
    }
}

bool func_8015885C(void);
void RicHandleDead(s32 damageEffects, s32 arg1, s32 arg2, s32 arg3);

static void UpdateTimers() {
    s32 i;
    PlayerDraw* playerDraw = g_PlayerDraw;

    for (i = 0; i < LEN(g_Player.timers); i++) {
        if (!g_Player.timers[i]) {
            continue;
        }
        switch (i) {
        case PL_T_POISON:
        case PL_T_CURSE:
        case PL_T_3:
        case PL_T_5:
        case PL_T_FALL:
        case PL_T_7:
        case PL_T_8:
            break;
        case PL_T_ATTACK:
            ChangeAnimToAttack();
            break;
        case PL_T_10:
        case PL_T_RUN:
        case PL_T_12:
        case PL_T_INVINCIBLE:
            break;
        case PL_T_2:
            PLAYER.palette = g_Player.unk40;
            break;
        case PL_T_4: {
            s32 temp_s0 = (g_GameTimer & 0xF) << 8;
            playerDraw->r0 = playerDraw->b0 = playerDraw->g0 =
                (rsin((s16)temp_s0) + 0x1000) / 64 + 0x60;
            playerDraw->r1 = playerDraw->b1 = playerDraw->g1 =
                (rsin(temp_s0 + 0x200) + 0x1000) / 64 + 0x60;
            playerDraw->r3 = playerDraw->b3 = playerDraw->g3 =
                (rsin(temp_s0 + 0x400) + 0x1000) / 64 + 0x60;
            playerDraw->r2 = playerDraw->b2 = playerDraw->g2 =
                (rsin(temp_s0 + 0x600) + 0x1000) / 64 + 0x60;
            playerDraw->enableColorBlend = 1;
            break;
        }
        case PL_T_INVINCIBLE_SCENE:
            g_Player.timers[PL_T_INVINCIBLE_SCENE] = 4;
            break;
        case PL_T_AFTERIMAGE_DISABLE:
            DisableAfterImage(false, 0);
            break;
        }
        if (--g_Player.timers[i]) {
            continue;
        }
        switch (i) {
        case PL_T_POISON:
            break;
        case PL_T_2:
            PLAYER.palette = 0x8120;
            break;
        case PL_T_4:
            playerDraw->enableColorBlend = 0;
            break;
        case PL_T_ATTACK:
            RevertAnimFromAttack();
            break;
        case PL_T_INVINCIBLE_SCENE:
            RicSetInvincibilityFrames(1, 16);
            break;
        case PL_T_FALL:
            if (PLAYER.step == PL_S_FALL) {
                MmxSetAnimation(PL_A_FALL);
                g_Player.unk44 &= ~0x10;
            }
            break;
        case PL_T_AFTERIMAGE_DISABLE:
            func_8015CC28();
            break;
        }
    }
}

static void UpdateInput() {
    g_Player.padHeld = g_Player.padPressed;
    if (g_Player.D_80072EFC) {
        g_Player.D_80072EFC--;
        g_Player.padPressed = g_Player.padSim;
    } else {
        g_Player.padPressed = g_pads[0].pressed;
    }
    g_Player.padTapped =
        (g_Player.padHeld ^ g_Player.padPressed) & g_Player.padPressed;
    g_PadReleased = (g_Player.padHeld ^ g_Player.padPressed) & g_Player.padHeld;
}

AnimationFrame mmx_anim_stand[] = {{160, FRAME(3, 2)}, A_END};
static AnimationFrame* D_8015538C[] = {mmx_anim_stand, mmx_anim_stand};
static FrameProperty D_80155964[] = {
    0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x06, 0x14, 0x02, 0x05, 0x07, 0x0C,
    0x03, 0x0C, 0x08, 0x0C, 0x00, 0xFF, 0x06, 0x10, 0x00, 0x01, 0x08, 0x08,
    0x00, 0xFB, 0x07, 0x05, 0xC0, 0xA8, 0x00, 0x00, 0xFC, 0x00, 0x06, 0x11,
    0x00, 0x00, 0x09, 0x10, 0xFF, 0x11, 0x0C, 0x07, 0xF9, 0x0A, 0x08, 0x05,
    0xFE, 0x06, 0x08, 0x08, 0xFA, 0x06, 0x07, 0x11, 0x04, 0x06, 0x0A, 0x09,
    0xFD, 0x03, 0x09, 0x14, 0x00, 0x08, 0x08, 0x08, 0xFF, 0x0A, 0x0C, 0x07,
};

void MmxMain(void) {
    DamageParam damage;
    s32 temp_s0;
    s32 var_s4;
    s32 damageKind;
    s32 damageEffects;
    s16 playerStep;
    s16 playerStepS;
    s32 i;
    bool isDamageTakenDeadly;
    f32* playerY;

    g_CurrentEntity = &PLAYER;
    g_Player.unk4C = 0;
    g_Player.unk72 = func_80156DE4();
    UpdateTimers();
    UpdateInput();
    if (PLAYER.step != PL_S_DEAD) {
        // Reuse the i variable here even though we aren't iterating
        i = GetTeleportToOtherCastle();
        if (i != TELEPORT_CHECK_NONE) {
            func_8015CC70(i);
        }
        // Richter must use step #32 for something else, look into it!
        if (PLAYER.step != PL_S_INIT) {
            if (g_DebugPlayer && RicDebug()) {
                return;
            }
            if (g_Player.unk60 >= 2) {
                goto check_input_combo;
            }
            if (g_Player.unk60 == 1) {
                playerStep = PLAYER.step;
                playerStepS = PLAYER.step_s;
                RicSetStep(PL_S_BOSS_GRAB);
                goto skip_input_combo;
            }
            if ((g_Player.timers[PL_T_INVINCIBLE_SCENE] |
                 g_Player.timers[PL_T_INVINCIBLE]) ||
                !PLAYER.hitParams) {
                goto check_input_combo;
            }
            // handle received damage
            playerStep = PLAYER.step;
            playerStepS = PLAYER.step_s;
            damage.effects = PLAYER.hitParams & ~0x1F;
            damage.damageKind = PLAYER.hitParams & 0x1F;
            damage.damageTaken = PLAYER.hitPoints;
            isDamageTakenDeadly = g_api.CalcPlayerDamage(&damage);
            damageKind = damage.damageKind;
            damageEffects = damage.effects;
            g_Player.timers[PL_T_INVINCIBLE] = 106; // tested on MMX
            if (isDamageTakenDeadly) {
                if (!g_Player.unk5C) {
                    RicSetStep(PL_S_DEAD);
                } else {
                    g_Status.hp = 1;
                    RicSetStep(PL_S_HIT);
                }
            } else {
                RicSetStep(PL_S_HIT);
            }
        }
    } else {
    check_input_combo:
        // CheckBladeDashInput();
        // CheckHighJumpInput();
        CheckHadoukenInput();
    }
skip_input_combo:
    g_Player.prev_step = PLAYER.step;
    g_Player.prev_step_s = PLAYER.step_s;
    switch (PLAYER.step) {
    case PL_S_STAND:
        RicHandleStand();
        break;
    case PL_S_WALK:
        RicHandleWalk();
        break;
    case PL_S_CROUCH:
        RicHandleCrouch();
        break;
    case PL_S_FALL:
        RicHandleFall();
        break;
    case PL_S_JUMP:
        MmxHandleJump();
        break;
    case PL_S_HIGHJUMP:
        RicHandleHighJump();
        break;
    case PL_S_HIT:
        RicHandleHit(damageEffects, damageKind, playerStep);
        break;
    case PL_S_BOSS_GRAB:
        RicHandleBossGrab();
        break;
    case PL_S_DEAD:
        RicHandleDead(damageEffects, damageKind, playerStep, playerStepS);
        break;
    case PL_S_STAND_IN_AIR:
        RicHandleStandInAir();
        break;
    case PL_S_FLAME_WHIP:
        RicHandleEnableFlameWhip();
        break;
    case PL_S_HYDROSTORM:
        RicHandleHydrostorm();
        break;
    case PL_S_THROW_DAGGERS:
        RicHandleThrowDaggers();
        break;
    case PL_S_SUBWPN_CRASH:
        RicHandleGenericSubwpnCrash();
        break;
    case PL_S_DEAD_PROLOGUE:
        RicHandleDeadPrologue();
        break;
    case PL_S_SLIDE:
        RicHandleSlide();
        break;
    case PL_S_RUN:
        RicHandleRun();
        break;
    case PL_S_SLIDE_KICK:
        RicHandleSlideKick();
        break;
    case PL_S_BLADEDASH:
        RicHandleBladeDash();
        break;
    case PL_S_DASH:
        MmxHandleDash();
        break;
    case PL_S_WALL:
        MmxHandleWall();
        break;
    case PL_S_DASH_AIR:
        MmxHandleDashAir();
        break;
    case PL_S_INIT:
        func_8015BCD0();
        break;
    }
    g_Player.unk08 = g_Player.status;
    var_s4 = 0;
    switch (PLAYER.step) {
    case PL_S_STAND:
    case PL_S_WALK:
        var_s4 = NO_AFTERIMAGE;
        break;
    case PL_S_CROUCH:
        var_s4 = NO_AFTERIMAGE;
        if (PLAYER.step_s != 2) {
            var_s4 = NO_AFTERIMAGE | PLAYER_STATUS_UNK_20;
        }
        break;
    case PL_S_FALL:
    case PL_S_JUMP:
        var_s4 = NO_AFTERIMAGE | PLAYER_STATUS_UNK2000;
        break;
    case PL_S_HIGHJUMP:
        RicSetInvincibilityFrames(1, 4);
        break;
    case PL_S_HIT:
        var_s4 = NO_AFTERIMAGE | PLAYER_STATUS_UNK10000;
    case PL_S_STAND_IN_AIR:
        RicSetInvincibilityFrames(1, 16);
        break;
    case PL_S_BOSS_GRAB:
        var_s4 = NO_AFTERIMAGE | PLAYER_STATUS_UNK100000 |
                 PLAYER_STATUS_UNK10000 | PLAYER_STATUS_UNK40;
        RicSetInvincibilityFrames(1, 16);
        break;
    case PL_S_DEAD:
        var_s4 =
            NO_AFTERIMAGE | PLAYER_STATUS_UNK40000 | PLAYER_STATUS_UNK10000;
        if (PLAYER.step_s == 0x80) {
            var_s4 = NO_AFTERIMAGE | PLAYER_STATUS_UNK80000 |
                     PLAYER_STATUS_UNK40000 | PLAYER_STATUS_UNK10000;
        }
        RicSetInvincibilityFrames(1, 16);
        break;
    case PL_S_SLIDE:
    case PL_S_SLIDE_KICK:
        var_s4 = 0x20;
        break;
    case PL_S_RUN:
    case PL_S_BLADEDASH:
        break;
    case PL_S_FLAME_WHIP:
    case PL_S_HYDROSTORM:
    case PL_S_THROW_DAGGERS:
    case PL_S_DEAD_PROLOGUE:
    case PL_S_SUBWPN_CRASH:
    case PL_S_INIT:
        var_s4 = NO_AFTERIMAGE;
        RicSetInvincibilityFrames(1, 16);
        break;
    case PL_S_DASH_AIR:
    case PL_S_DASH:
        if (PLAYER.step_s != 2) {
            var_s4 = 0x20;
        }
        break;
    case PL_S_WALL:
        var_s4 = NO_AFTERIMAGE;
        break;
    }
    if (g_Player.timers[PL_T_ATTACK]) {
        var_s4 |= PLAYER_STATUS_UNK400;
    }
    if (g_Player.timers[PL_T_10]) {
        var_s4 |= PLAYER_STATUS_UNK800;
    }
    if (g_Player.timers[PL_T_12]) {
        var_s4 |= PLAYER_STATUS_UNK1000;
    }
    if (*D_80097448) {
        var_s4 |= PLAYER_STATUS_UNK20000;
    }
    var_s4 |= PLAYER_STATUS_UNK10000000;
    g_Player.status = var_s4;

    // TODO move this stuff into the switch above
    var_s4 |= NO_AFTERIMAGE;
    if (g_Player.unk44 & IS_DASHING) {
        var_s4 &= ~NO_AFTERIMAGE;
    }

    if (g_Player.unk08 & PLAYER_STATUS_UNK10000) {
        if (!(var_s4 & PLAYER_STATUS_UNK10000)) {
            if (g_Player.unk5C != 0) {
                if (g_Status.hp < 2) {
                    RicSetDeadPrologue();
                    RicSetInvincibilityFrames(1, 16);
                }
            } else {
                RicSetInvincibilityFrames(1, 16);
                g_Player.timers[PL_T_4] = 0x10;
                PLAYER.palette = 0x8120;
            }
        }
    }
    if (var_s4 & NO_AFTERIMAGE) {
        DisableAfterImage(1, 4);
    }
    if (g_Player.timers[PL_T_INVINCIBLE_SCENE] |
        g_Player.timers[PL_T_INVINCIBLE]) {
        g_Player.status |= 0x100;
    }
    g_api.UpdateAnim(D_80155964, D_8015538C);
    PLAYER.hitboxState = 1;
    PLAYER.hitParams = 0;
    PLAYER.hitPoints = 0;
    g_Player.unk7A = 0;
    if ((PLAYER.step == PL_S_DEAD) && (PLAYER.animFrameDuration < 0)) {
        PLAYER.animCurFrame |= ANIM_FRAME_LOAD;
    }
    if (g_Player.status & 0x50) {
        return;
    }
    func_8015C4AC();
    if ((*D_80097448 >= 0x29) && (g_CurrentEntity->nFramesInvincibility == 0)) {
        PLAYER.velocityY = PLAYER.velocityY * 3 / 4;
        PLAYER.velocityX = PLAYER.velocityX * 3 / 4;
    }
    playerY = &PLAYER.posY.i;
    temp_s0 = g_Player.pl_vram_flag;
    if ((abs(PLAYER.velocityY) > FIX(2)) || (abs(PLAYER.velocityX) > FIX(2))) {
        PLAYER.velocityY = PLAYER.velocityY >> 2;
        PLAYER.velocityX = PLAYER.velocityX >> 2;
        if ((playerY->i.hi < 0) || (func_801572A8(1), (playerY->i.hi < 0)) ||
            (func_801572A8(0), (playerY->i.hi < 0)) ||
            (func_801572A8(0), (playerY->i.hi < 0)) ||
            (func_801572A8(0), (playerY->i.hi < 0))) {
            PLAYER.posY.val = FIX(-1);
        }
        PLAYER.velocityX *= 4;
        PLAYER.velocityY *= 4;
    } else {
        func_801572A8(1);
    }
    g_Player.unk04 = temp_s0;
    if ((*D_80097448 >= 0x29) && (g_CurrentEntity->nFramesInvincibility == 0)) {
        PLAYER.velocityY = (PLAYER.velocityY * 4) / 3;
        PLAYER.velocityX = (PLAYER.velocityX * 4) / 3;
    }
    g_CurrentEntity->nFramesInvincibility = 0;
    func_8015C6D4();
}

static void RicDebugOff() { g_IsRicDebugEnter = false; }

static void RicDebugEnter(void) {
    g_IsRicDebugEnter = true;
    g_RicDebugCurFrame = PLAYER.animCurFrame;
    g_RicDebugDrawFlags = PLAYER.drawFlags;
    g_RicDebugPalette = PLAYER.palette;
}

static void RicDebugExit(void) {
    g_IsRicDebugEnter = false;
    PLAYER.hitParams = 0;
    PLAYER.animCurFrame = g_RicDebugCurFrame;
    PLAYER.drawFlags = g_RicDebugDrawFlags;
    PLAYER.palette = g_RicDebugPalette;
}

int RicDebug(void) {
    if (!g_IsRicDebugEnter) {
        if (g_Player.padTapped & PAD_L2) {
            if (g_Player.D_80072EFC == 0) {
                RicDebugEnter();
                return true;
            }
        }
        return false;
    }

    if (g_Player.D_80072EFC || g_Player.padTapped & PAD_L2) {
        RicDebugExit();
        return false;
    }

    if (g_Player.padPressed & PAD_CROSS) {
        if (g_Player.padPressed & PAD_RIGHT) {
            g_Entities->posX.val += FIX(16.0);
        }
        if (g_Player.padPressed & PAD_LEFT) {
            g_Entities->posX.val -= FIX(16.0);
        }
        if (g_Player.padPressed & PAD_UP) {
            PLAYER.posY.val -= FIX(16.0);
        }
        if (g_Player.padPressed & PAD_DOWN) {
            PLAYER.posY.val += FIX(16.0);
        }

    } else {
        if (g_Player.padTapped & PAD_RIGHT) {
            g_Entities->posX.val += FIX(16.0);
        }
        if (g_Player.padTapped & PAD_LEFT) {
            g_Entities->posX.val -= FIX(16.0);
        }
        if (g_Player.padTapped & PAD_UP) {
            PLAYER.posY.val -= FIX(16.0);
        }
        if (g_Player.padTapped & PAD_DOWN) {
            PLAYER.posY.val += FIX(16.0);
        }
    }

    if (g_Player.padTapped & PAD_CIRCLE) {
        PLAYER.animCurFrame--;
    }
    if (g_Player.padTapped & PAD_SQUARE) {
        PLAYER.animCurFrame++;
    }

    if (PLAYER.animCurFrame <= 0) {
        PLAYER.animCurFrame = 1;
    }
    if (PLAYER.animCurFrame < 212 == 0) {
        PLAYER.animCurFrame = 211;
    }
    FntPrint("charal:%02x\n", PLAYER.animCurFrame);
    return true;
}

void func_80158B04(s32 arg0) {
    s32 var_s0;

    if (PLAYER.facingLeft) {
        var_s0 = -3;
    } else {
        var_s0 = 3;
    }

    PLAYER.posY.i.hi -= 16;
    PLAYER.posX.i.hi = var_s0 + PLAYER.posX.i.hi;
    RicCreateEntFactoryFromEntity(g_CurrentEntity, FACTORY(BP_EMBERS, 1), 0);
    PLAYER.posY.i.hi += 16;
    PLAYER.posX.i.hi = PLAYER.posX.i.hi - var_s0;

    if (arg0 & 1) {
        g_api.func_80102CD8(3);
        g_api.PlaySfx(SFX_WALL_DEBRIS_B);
    }

    if (arg0 & 2) {
        PLAYER.velocityX = 0;
        PLAYER.velocityY = 0;
    }
}

static void RicHandleStand(void) {
    s32 var_s0;

    g_Player.unk44 &= ~IS_DASHING;
    if (PLAYER.step < 64) {
        if (D_8015459C != 0) {
            D_8015459C--;
        } else if (D_80097448[0] >= 49) {
            if (PLAYER.facingLeft) {
                var_s0 = -4;
            } else {
                var_s0 = 4;
            }
            PLAYER.posX.i.hi = var_s0 + PLAYER.posX.i.hi;
            PLAYER.posY.i.hi -= 16;
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(BP_EMBERS, 8), 0);
            D_8015459C = 0x60;
            PLAYER.posY.i.hi += 16;
            PLAYER.posX.i.hi = PLAYER.posX.i.hi - var_s0;
        }
    }

    if (!RicCheckInput(CHECK_FALL | CHECK_FACING | CHECK_JUMP | CHECK_ATTACK |
                       CHECK_SLIDE)) {
        RicDecelerateX(0x2000);
        switch (PLAYER.step_s) {
        case 0:
            if (RicCheckFacing() == 0) {
                if (g_Player.padPressed & PAD_UP) {
                    PLAYER.step_s = 1;
                    break;
                }
            } else {
                RicSetWalk(0);
            }
            break;
        case 1:
            if (RicCheckFacing() != 0) {
                RicSetWalk(0);
                break;
            } else if (g_Player.padPressed & PAD_UP) {
                break;
            } else {
                RicSetStand(0);
                break;
            }
        case 64:
            DisableAfterImage(1, 1);
            if (PLAYER.animFrameIdx < 3) {
                RicCheckFacing();
                if (g_Player.padPressed & PAD_DOWN) {
                    PLAYER.step = PL_S_CROUCH;
                    MmxSetAnimation(PL_A_DUMMY);
                    break;
                }
            }
            if (PLAYER.animFrameDuration < 0) {
                if (g_Player.padPressed & PAD_SQUARE) {
                    g_Player.unk46 = 2;
                    PLAYER.step_s++;
                    MmxSetAnimation(PL_A_DUMMY);
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, BP_ARM_BRANDISH_WHIP, 0);
                    break;
                }
                g_Player.unk46 = 0;
                RicSetStand(0);
            }
            break;
        case 65:
            DisableAfterImage(1, 1);
            if (g_Player.padPressed & PAD_SQUARE) {
                break;
            }
            g_Player.unk46 = 0;
            RicSetStand(0);
            break;
        case 66:
            DisableAfterImage(1, 1);
            if (PLAYER.animFrameIdx < 3) {
                RicCheckFacing();
            }
            if (PLAYER.animFrameDuration < 0) {
                g_Player.unk46 = 0;
                RicSetStand(0);
            }
        }
    }
}

static void RicHandleWalk(void) {
    if (PLAYER.step_s == 0) {
        RicSetSpeedX(MMX_WALK_SPEED);
    }
    if (!RicCheckInput(CHECK_FALL | CHECK_FACING | CHECK_JUMP | CHECK_ATTACK |
                       CHECK_SLIDE)) {
        RicDecelerateX(0x2000);
        if (!RicCheckFacing()) {
            RicSetStand(0);
            return;
        }
    }
}

// holding down the dash button will move MMX by 114 pixels in about 34 frames
// 7E:0BAC PosX
static void MmxHandleDash(void) {
    const int MAX_DASH_TIMER = 32;

    // if (g_Player.unk7A) {
    //     RicSetWalk(0);
    //     return;
    // }
    g_Player.timers[PL_T_8] = 8;
    g_Player.timers[PL_T_CURSE] = 8;

    // if (!RicCheckInput(0x305C)) {
    if (!RicCheckInput(CHECK_FALL | CHECK_JUMP | CHECK_ATTACK)) {
        // end dash if at least one of the following conditions are met:
        //   maximum dash timer is reached
        //   the dash button is released
        //   bumped to a wall
        if (g_DashTimer > MAX_DASH_TIMER || !(g_Player.padPressed & PAD_DASH) ||
            g_Player.pl_vram_flag & 0xC) {
            g_Player.unk44 &= ~IS_DASHING;
            RicSetStand(0);
            if (g_Player.timers[PL_T_RUN] == 0) {
                if (!(g_Player.pl_vram_flag & 0xC)) {
                    // if bumping to a wall?
                }
            }
            return;
        }
    }
    if ((++g_DashTimer & 3) == 0) {
        RicCreateEntFactoryFromEntity(g_CurrentEntity, B_P_DASH, 0);
    }
}

static void MmxHandleDashAir(void) {
    const int MAX_DASH_TIMER = 32;

    // if (g_Player.unk7A) {
    //     RicSetWalk(0);
    //     return;
    // }
    g_Player.timers[8] = 8;
    g_Player.timers[1] = 8;

    // if (!RicCheckInput(0x305C)) {
    // if CHECK_JUMP is set, we can jump while doing an air-dash 🤯
    if (!RicCheckInput(CHECK_ATTACK)) {
        // end dash if at least one of the following conditions are met:
        //   maximum dash timer is reached
        //   the dash button is released
        //   bumped to a wall
        if (g_DashTimer > MAX_DASH_TIMER || !(g_Player.padPressed & PAD_DASH) ||
            g_Player.pl_vram_flag & 0xC) {
            g_Player.unk44 &= ~IS_DASHING;
            MmxSetAnimation(PL_A_FALL);
            RicSetStep(PL_S_FALL);
            return;
        }
    }
    g_DashTimer++;
}

static void MmxSetWall(void) {
    MmxSetAnimation(PL_A_WALL);
    g_api.PlaySfx(SFX_STOMP_SOFT_A);
    RicSetStep(PL_S_WALL);
    g_CurrentEntity->velocityY = MMX_WALL_SPEED;
    g_WallSlideTimer = 0;
    g_DashAirUsed = false;
}

static void MmxHandleWall(void) {
    if (!MmxIsHuggingWall()) {
        g_CurrentEntity->facingLeft = !g_CurrentEntity->facingLeft;
        // MmxPrepareFall();
        MmxSetAnimation(PL_A_FALL);
        RicSetStep(PL_S_FALL);
        return;
    }
    if (RicCheckInput(CHECK_GROUND | CHECK_ATTACK | CHECK_GRAVITY_JUMP)) {
        return;
    }
    if (g_Player.padTapped & PAD_CROSS) {
        RicCreateEntFactoryFromEntity(g_CurrentEntity, B_P_JUMP_FROM_WALL, 0);
        MmxSetJump(0);
        return;
    }
    if (++g_WallSlideTimer == 4) {
        g_WallSlideTimer = 0;
        RicCreateEntFactoryFromEntity(g_CurrentEntity, B_P_WALL, 0);
    }
    g_CurrentEntity->velocityY = MMX_WALL_SPEED;
}

static void RicHandleRun(void) {
    if (g_Player.unk7A != 0) {
        RicSetWalk(0);
        return;
    }
    g_Player.timers[PL_T_8] = 8;
    g_Player.timers[PL_T_CURSE] = 8;

    if (!RicCheckInput(CHECK_FALL | CHECK_FACING | CHECK_JUMP | CHECK_ATTACK |
                       CHECK_CROUCH)) {
        RicDecelerateX(0x2000);
        if (RicCheckFacing() == 0) {
            RicSetStand(0);
            if (!g_Player.timers[PL_T_RUN]) {
                if (!(g_Player.pl_vram_flag & 0xC)) {
                    MmxSetAnimation(PL_A_DUMMY);
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, BP_SKID_SMOKE, 0);
                }
            } else {
                PLAYER.velocityX = 0;
            }
        } else if (PLAYER.step_s == 0) {
            RicSetSpeedX(0x24000);
        }
    }
}

static void MmxHandleJump(void) {
    if (!g_IsPrologueStage && (PLAYER.velocityY < FIX(-1)) &&
        !(g_Player.unk44 & 0x40) && !(g_Player.padPressed & PAD_CROSS)) {
        PLAYER.velocityY = FIX(-1);
    }
    if ((g_Player.pl_vram_flag & 2) && (PLAYER.velocityY < FIX(-1))) {
        PLAYER.velocityY = FIX(-0.25);
        g_Player.unk44 |= 0x20;
    }

    switch (g_JumpState) {
    case PL_JUMP_NONE:
        if (RicCheckInput(CHECK_GROUND | CHECK_FACING | CHECK_ATTACK |
                          CHECK_GRAVITY_JUMP | CHECK_SLIDE)) {
            return;
        }
        if (PLAYER.velocityY < 0) {
            g_JumpState = PL_JUMP_ASCENDING;
        }
        break;
    case PL_JUMP_ASCENDING:
        if (RicCheckInput(CHECK_GROUND | CHECK_FACING | CHECK_ATTACK |
                          CHECK_GRAVITY_JUMP | CHECK_SLIDE)) {
            return;
        }
        if (PLAYER.velocityY > FIX(0.25)) {
            MmxSetAnimation(PL_A_FALL);
            g_JumpState = PL_JUMP_DESCENDING;
        }
        break;
    case PL_JUMP_DESCENDING:
        if (RicCheckInput(CHECK_GROUND | CHECK_FACING | CHECK_ATTACK |
                          CHECK_GRAVITY_JUMP | CHECK_SLIDE | CHECK_WALL)) {
            return;
        }
        break;
    }
    if (!RicCheckFacing()) {
        // if not moving in air, immediately stop. Like the original MMX.
        g_CurrentEntity->velocityX = 0;
    } else {
        if (g_Player.unk44 & IS_DASHING) {
            RicSetSpeedX(MMX_DASH_SPEED);
        } else {
            RicSetSpeedX(MMX_WALK_SPEED);
        }
        g_Player.unk44 &= ~4;
    }
}

static void RicHandleFall(void) {
    if (RicCheckInput(CHECK_GROUND | CHECK_FACING | CHECK_ATTACK |
                      CHECK_GRAVITY_FALL | CHECK_WALL)) {
        return;
    }
    RicDecelerateX(0x1000);
    if (PLAYER.step_s != 0) {
        return;
    }
    if (g_Player.timers[PL_T_5] && g_Player.padTapped & PAD_CROSS) {
        MmxSetJump(0);
    } else if (RicCheckFacing()) {
        // when falling, sometimes the current speed can be higher than the
        // one set when moving in the air
        // if pressing the same directional button of the direction where the
        // player is falling to, we want to keep maintaining the highest speed
        const int FALL_MOV_X_SPEED = FIX(0.75);
        if (!PLAYER.facingLeft && g_Player.padPressed & PAD_RIGHT) {
            if (PLAYER.velocityX < FALL_MOV_X_SPEED) {
                RicSetSpeedX(FALL_MOV_X_SPEED);
            }
        } else if (PLAYER.facingLeft && g_Player.padPressed & PAD_LEFT) {
            if (PLAYER.velocityX > -FALL_MOV_X_SPEED) {
                RicSetSpeedX(FALL_MOV_X_SPEED);
            }
        } else {
            RicSetSpeedX(FALL_MOV_X_SPEED);
        }
    }
}

static void RicHandleCrouch(void) {
    s32 i;
    s16 xShift;

    if ((g_Player.padTapped & PAD_CROSS) && (g_Player.unk46 == 0) &&
        (g_Player.padPressed & PAD_DOWN)) {
        for (i = 0; i < 4; i++) {
            if (g_Player.colliders[i].effects & EFFECT_SOLID_FROM_ABOVE) {
                g_Player.timers[PL_T_7] = 8;
                return;
            }
        }
    }
    if (RicCheckInput(CHECK_FALL | CHECK_FACING | CHECK_ATTACK)) {
        return;
    }
    if ((g_Player.padTapped & PAD_CROSS) && (g_Player.unk46 == 0) &&
        (!g_Player.unk72)) {
        MmxSetJump(0);
        return;
    } else if ((!g_Player.unk72) && (g_Player.unk46 == 0) &&
               (g_Player.padTapped & PAD_TRIANGLE) && RicDoCrash()) {
        return;
    }
    RicDecelerateX(0x2000);
    switch (PLAYER.step_s) {
    case 0x0:
        if (D_8015459C != 0) {
            D_8015459C--;
        } else if ((*D_80097448 >= 0x19) && (g_Player.unk48 == 0)) {
            if (PLAYER.facingLeft) {
                xShift = -9;
            } else {
                xShift = 9;
            }
            PLAYER.posX.i.hi += xShift;
            PLAYER.posY.i.hi += 2;
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(BP_EMBERS, 8), 0);
            D_8015459C = 0x60;
            PLAYER.posY.i.hi -= 2;
            PLAYER.posX.i.hi -= xShift;
        }
        if (!(g_Player.padPressed & PAD_DOWN) &&
            ((!g_Player.unk72) || !(g_Player.pl_vram_flag & 0x40))) {
            MmxSetAnimation(PL_A_DUMMY);
            PLAYER.step_s = 2;
            return;
        }
        break;
    case 0x1:
        if (!(g_Player.padPressed & PAD_DOWN) &&
            ((!g_Player.unk72) || !(g_Player.pl_vram_flag & 0x40))) {
            if (RicCheckFacing() == 0) {
                MmxSetAnimation(PL_A_DUMMY);
                PLAYER.step_s = 2;
                PLAYER.animFrameDuration = 1;
                PLAYER.animFrameIdx = 2 - PLAYER.animFrameIdx;
                return;
            }
            RicSetWalk(0);
            return;
        }
    case 0x4:
        if (PLAYER.animFrameDuration != -1) {
            return;
        }
        MmxSetAnimation(PL_A_DUMMY);
        PLAYER.step_s = 0;
        return;
    case 0x2:
        if ((!g_Player.unk72) || !(g_Player.pl_vram_flag & 0x40)) {
            if (RicCheckFacing() != 0) {
                RicSetWalk(0);
                return;
            }
            if (PLAYER.animFrameDuration == -1) {
                RicSetStand(0);
                return;
            }
        }
        break;
    case 0x3:
        if (PLAYER.animFrameDuration < 0) {
            MmxSetAnimation(PL_A_DUMMY);
            PLAYER.step_s = 0;
            return;
        }
        break;
    case 0x40:
        DisableAfterImage(1, 1);
        if (PLAYER.animFrameIdx < 3) {
            RicCheckFacing();
            if (!(g_Player.padPressed & PAD_DOWN) && (!g_Player.unk72)) {
                PLAYER.step = PL_S_STAND;
                MmxSetAnimation(PL_A_DUMMY);
                return;
            }
        }
        if (PLAYER.animFrameDuration < 0) {
            if (g_Player.padPressed & PAD_SQUARE) {
                g_Player.unk46 = 2;
                PLAYER.step_s++;
                MmxSetAnimation(PL_A_DUMMY);
                RicCreateEntFactoryFromEntity(
                    g_CurrentEntity, BP_ARM_BRANDISH_WHIP, 0);
                return;
            }
            g_Player.unk46 = 0;
            PLAYER.step_s = 0;
            MmxSetAnimation(PL_A_DUMMY);
        }
        break;
    case 0x41:
        DisableAfterImage(1, 1);
        if (!(g_Player.padPressed & PAD_SQUARE)) {
            g_Player.unk46 = 0;
            PLAYER.step_s = 0;
            MmxSetAnimation(PL_A_DUMMY);
        }
        break;
    }
}

void func_80159BC8(void) {
    PLAYER.animFrameDuration = 0;
    PLAYER.animFrameIdx = 0;
    g_Player.unk44 = 0;
    g_Player.unk46 = 0;
    PLAYER.drawFlags &= ~FLAG_DRAW_ROTZ;
}

void func_80159C04(void) {
    Entity* entity = PLAYER.unkB8;
    s16 temp_v0;
    s32 var_a0;
    s32 var_a2;

    if (entity->facingLeft) {
        var_a2 = -entity->hitboxOffX;
    } else {
        var_a2 = entity->hitboxOffX;
    }

    if (PLAYER.facingLeft) {
        var_a0 = -PLAYER.hitboxOffX;
    } else {
        var_a0 = PLAYER.hitboxOffX;
    }

    temp_v0 = var_a0 + PLAYER.posX.i.hi - entity->posX.i.hi - var_a2;

    if (abs(temp_v0) < 16) {
        if (entity->velocityX != 0) {
            if (entity->velocityX < 0) {
                PLAYER.entityRoomIndex = 0;
                return;
            } else {
                PLAYER.entityRoomIndex = 1;
                return;
            }
        }
    }

    if (temp_v0 < 0) {
        PLAYER.entityRoomIndex = 0;
    } else {
        PLAYER.entityRoomIndex = 1;
    }
}

static s32 ric_hit_stun_timer;
void RicHandleHit(s32 damageEffect, u32 damageKind, s16 prevStep) {
    DamageParam damage;
    s32 xShift;
    s32 i;
    bool step_s_zero = false;

    if (ric_hit_stun_timer) {
        ric_hit_stun_timer--;
    }
    switch (PLAYER.step_s) {
    case 0:
        step_s_zero = true;
        func_80159BC8();
        if (damageKind < 16) {
            func_80159C04();
        } else {
            PLAYER.entityRoomIndex = PLAYER.facingLeft;
        }
        if (damageEffect & ELEMENT_THUNDER) {
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, BP_HIT_BY_THUNDER, 0);
            PLAYER.velocityY = FIX(-4);
            func_8015CAAC(FIX(-1.25));
            PLAYER.step_s = 1;
            MmxSetAnimation(PL_A_DUMMY);
            g_Player.unk40 = 0x8120;
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, BP_HIT_BY_THUNDER, 0);
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(BP_RIC_BLINK, 0x46), 0);
            g_Player.timers[PL_T_2] = 6;
            g_api.PlaySfx(0x703);
            break;
        } else {
            if (damageEffect & ELEMENT_ICE) {
                damageKind = 3;
            }
            switch (damageKind - 2) {
            case 0:
                switch (prevStep) {
                case PL_S_STAND:
                case PL_S_WALK:
                    PLAYER.velocityY = 0;
                    func_8015CAAC(FIX(-1.25));
                    PLAYER.step_s = 6;
                    MmxSetAnimation(PL_A_HIT_SMALL);
                    g_api.PlaySfx(0x702);
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, BP_SKID_SMOKE, 0);
                    break;
                case PL_S_CROUCH:
                    PLAYER.velocityY = 0;
                    func_8015CAAC(FIX(-1.25));
                    PLAYER.step_s = 7;
                    MmxSetAnimation(PL_A_DUMMY);
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, BP_SKID_SMOKE, 0);
                    g_api.PlaySfx(0x703);
                    break;
                case PL_S_DASH:
                case PL_S_DASH_AIR:
                    PLAYER.velocityY = 0;
                    func_8015CAAC(FIX(-1.25));
                    MmxSetAnimation(PL_A_HIT_STUN);
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, FACTORY(BP_SKID_SMOKE, 0), 0);
                    g_api.PlaySfx(0x703);
                    break;
                case PL_S_FALL:
                case PL_S_JUMP:
                    PLAYER.velocityY = FIX(-3);
                    func_8015CAAC(FIX(-1.25));
                    PLAYER.step_s = 1;
                    MmxSetAnimation(PL_A_HIT_STUN);
                    g_api.PlaySfx(0x702);
                    break;
                }
                break;
            case 2:
            case 3:
                PLAYER.velocityY = FIX(-0.5);
                g_Player.damageTaken = PLAYER.hitPoints;
                PLAYER.posY.val -= 1;
                func_8015CAAC(FIX(-8));
                PLAYER.step_s = 2;
                MmxSetAnimation(PL_A_HIT_STUN);
                g_Player.timers[PL_T_2] = 0x200;
                PLAYER.facingLeft = PLAYER.entityRoomIndex;
                break;
            default:
            case 1:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
                switch (prevStep) {
                default:
                case PL_S_STAND:
                case PL_S_WALK:
                    PLAYER.velocityY = FIX(-4);
                    func_8015CAAC(FIX(-1.25));
                    PLAYER.step_s = 1;
                    MmxSetAnimation(PL_A_HIT_SMALL);
                    g_api.PlaySfx(0x702);
                    break;
                case PL_S_CROUCH:
                    PLAYER.velocityY = 0;
                    func_8015CAAC(FIX(-1.25));
                    PLAYER.step_s = 7;
                    MmxSetAnimation(PL_A_DUMMY);
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, BP_SKID_SMOKE, 0);
                    g_api.PlaySfx(0x703);
                    break;
                case PL_S_DASH:
                case PL_S_DASH_AIR:
                    PLAYER.velocityY = 0;
                    func_8015CAAC(FIX(-1.25));
                    MmxSetAnimation(PL_A_HIT_STUN);
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, FACTORY(BP_SKID_SMOKE, 0), 0);
                    g_api.PlaySfx(0x703);
                    break;
                case PL_S_FALL:
                case PL_S_JUMP:
                    PLAYER.velocityY = FIX(-3);
                    func_8015CAAC(FIX(-1.25));
                    PLAYER.step_s = 1;
                    MmxSetAnimation(PL_A_HIT_STUN);
                    g_api.PlaySfx(0x702);
                    break;
                }
                break;
            }
            g_Player.unk40 = 0x8166;
            g_Player.timers[PL_T_2] = 6;
            if (damageEffect & ELEMENT_FIRE) {
                RicCreateEntFactoryFromEntity(
                    g_CurrentEntity, FACTORY(BP_HIT_BY_FIRE, 1), 0);
                RicCreateEntFactoryFromEntity(g_CurrentEntity, 9, 0);
                RicCreateEntFactoryFromEntity(
                    g_CurrentEntity, FACTORY(BP_RIC_BLINK, 0x43), 0);
                g_Player.unk40 = 0x8160;
                g_Player.timers[PL_T_2] = 0x10;
                break;
            } else if (damageEffect & ELEMENT_CUT) {
                RicCreateEntFactoryFromEntity(
                    g_CurrentEntity, FACTORY(BP_HIT_BY_CUT, 5), 0);
                RicCreateEntFactoryFromEntity(
                    g_CurrentEntity, FACTORY(BP_RIC_BLINK, 0x44), 0);
                g_Player.unk40 = 0x8166;
                g_Player.timers[PL_T_2] = 0x10;
                break;
            } else if (damageEffect & ELEMENT_ICE) {
                RicCreateEntFactoryFromEntity(
                    g_CurrentEntity, BP_HIT_BY_ICE, 0);
                g_Player.timers[PL_T_2] = 0xC;
                g_Player.unk40 = 0x8169;
                break;
            } else {
                if (damageEffect & ELEMENT_DARK) {
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, BP_HIT_BY_DARK, 0);
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, FACTORY(BP_RIC_BLINK, 0x56), 0);
                    g_Player.timers[PL_T_2] = 0x10;
                    g_Player.unk40 = 0x8164;
                }
                if (damageEffect & ELEMENT_HOLY) {
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, BP_HIT_BY_HOLY, 0);
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, FACTORY(BP_RIC_BLINK, 0x57), 0);
                    g_Player.timers[PL_T_2] = 8;
                    g_Player.unk40 = 0x8168;
                }
                if (!(damageEffect & 0xF840)) {
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, FACTORY(BP_RIC_BLINK, 0x53), 0);
                }
            }
        }
        break;
    case 1:
        if ((g_Player.pl_vram_flag & 2) && (PLAYER.velocityY < FIX(-1))) {
            PLAYER.velocityY = FIX(-1);
        }
        if (RicCheckInput(
                CHECK_80 | CHECK_GRAVITY_HIT | CHECK_GROUND_AFTER_HIT)) {
            return;
        }
        break;
    case 2:
        if ((g_Player.unk04 & 0x8000) && !(g_Player.pl_vram_flag & 0x8000)) {
            goto block_6dc;
        }
        if ((g_Player.pl_vram_flag & 0x8000) && !(g_GameTimer & 1)) {
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(BP_SMOKE_PUFF_2, 10), 0);
        }
        if (!(g_Player.pl_vram_flag & 0xE)) {
            break;
        }
        if (g_Player.pl_vram_flag & 2) {
            func_80158B04(1);
            ric_hit_stun_timer = 0x18;
            PLAYER.velocityX /= 2;
            PLAYER.velocityY = 0;
            PLAYER.step_s = 5;
            damage.effects = 0;
            damage.damageKind = 1;
            damage.damageTaken = g_Player.damageTaken;
            if (g_api.CalcPlayerDamage(&damage)) {
                RicSetStep(PL_S_DEAD);
                RicHandleDead(0, 2, PL_S_HIT, 2);
                return;
            }
            break;
        } else {
            if ((g_StageId != STAGE_BO6) && (g_StageId != STAGE_RBO6) &&
                (g_StageId != STAGE_DRE)) {
                for (i = 2; i < 7; i++) {
                    if (g_Player.colliders3[i].effects & 2) {
                        break;
                    }
                }
                if (i == 7) {
                    for (i = 9; i < 0xE; i++) {
                        if (g_Player.colliders3[i].effects & 2) {
                            break;
                        }
                    }
                }
                if (i == 14) {
                block_6dc:
                    PLAYER.velocityY = FIX(-4);
                    func_8015CAAC(FIX(-1.25));
                    xShift = -3;
                    if (PLAYER.velocityX != 0) {
                        xShift = 3;
                    }
                    PLAYER.posY.i.hi += 20;
                    PLAYER.posX.i.hi = xShift + PLAYER.posX.i.hi;
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, FACTORY(BP_EMBERS, 9), 0);
                    PLAYER.posY.i.hi -= 20;
                    PLAYER.posX.i.hi -= xShift;
                    g_api.PlaySfx(SFX_WALL_DEBRIS_B);
                    g_api.func_80102CD8(2);
                    PLAYER.step_s = 1;
                    damage.effects = 0;
                    damage.damageKind = 1;
                    damage.damageTaken = g_Player.damageTaken;
                    if (g_api.CalcPlayerDamage(&damage)) {
                        RicSetStep(PL_S_DEAD);
                        RicHandleDead(0, 2, PL_S_HIT, 2);
                        return;
                    }
                    break;
                }
            }
            ric_hit_stun_timer = 8;
            g_api.PlaySfx(SFX_WALL_DEBRIS_B);
            PLAYER.velocityY = FIX(-2.5);
            g_api.func_80102CD8(2);
            PLAYER.step_s = 3;
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(BP_SKID_SMOKE_2, 8), 0);
        }
        damage.effects = 0;
        damage.damageKind = 1;
        damage.damageTaken = g_Player.damageTaken;
        if (g_api.CalcPlayerDamage(&damage)) {
            RicSetStep(PL_S_DEAD);
            RicHandleDead(0, 2, PL_S_HIT, 2);
            return;
        }
        break;
    case 3:
        if (!ric_hit_stun_timer) {
            RicSetSpeedX(FIX(0.75));
            if (RicCheckInput(
                    CHECK_80 | CHECK_GRAVITY_HIT | CHECK_GROUND_AFTER_HIT)) {
                return;
            }
        }
        break;
    case 5:
        RicDecelerateX(0x2000);
        if (ric_hit_stun_timer) {
            if ((g_Player.pl_vram_flag & 2) && !(g_GameTimer & 3)) {
                func_80158B04(0);
            }
            break;
        } else if (g_Player.pl_vram_flag & 0xC) {
            if (!(g_Player.pl_vram_flag & 0xFF03)) {
                PLAYER.velocityY += FIX(12.0 / 128);
                if (PLAYER.velocityY > FIX(7)) {
                    PLAYER.velocityY = FIX(7);
                }
                if (!(g_GameTimer & 3)) {
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, FACTORY(BP_SKID_SMOKE_3, 4), 0);
                }
                break;
            }
        }
        PLAYER.step_s = 1;
        PLAYER.animFrameIdx = 0;
        PLAYER.animFrameDuration = 0;
        break;
    case 6:
        RicDecelerateX(0x2000);
        if (!(g_Player.pl_vram_flag & 1)) {
            RicSetFall();
        }
        if (PLAYER.animFrameDuration < 0) {
            if (!g_Player.unk5C || g_Status.hp > 0) {
                RicSetStand(PLAYER.velocityX);
                break;
            }
            RicSetDeadPrologue();
            return;
        }
        break;
    case 7:
        RicDecelerateX(0x2000);
        if (!(g_Player.pl_vram_flag & 1)) {
            RicSetFall();
        }
        if (PLAYER.animFrameDuration < 0) {
            if (g_Player.unk5C && g_Status.hp <= 0) {
                RicSetDeadPrologue();
                return;
            }
            RicSetWalkFromJump(0, PLAYER.velocityX);
        }
        break;
    }
    if (step_s_zero && (g_Player.unk72)) {
        PLAYER.velocityY = 0;
    }
}

static void RicHandleBossGrab(void) {
    DamageParam damage;
    switch (g_CurrentEntity->step_s) {
    case 0:
        func_80159BC8();
        if (g_Player.unk62 == 0) {
            MmxSetAnimation(PL_S_HIT);
            g_Player.timers[PL_T_2] = 8;
            g_Player.unk40 = 0x8166;
            g_api.PlaySfx(0x702);
        }
        PLAYER.step_s = 1;
        PLAYER.velocityX = PLAYER.velocityY = 0;
        g_Player.unk60 = 2;
        return;
    case 1:
        // Effectively a switch on g_Player.unk60
        if (g_Player.unk60 == 3) {
            damage.effects = 0;
            damage.damageKind = 1;
            damage.damageTaken = g_Player.damageTaken;
            if (g_api.CalcPlayerDamage(&damage)) {
                RicSetStep(PL_S_DEAD);
                RicHandleDead(0, 2, PL_S_BOSS_GRAB, 1);
                return;
            }
            if (g_Player.unk62 == 0) {
                g_Player.timers[PL_T_2] = 4;
                g_Player.unk40 = 0x8166;
                g_api.PlaySfx(0x703);
            }
            g_Player.unk60 = 2;
        } else if (g_Player.unk60 == 4) {
            PLAYER.step = PL_S_HIT;
            PLAYER.step_s = 2;
            g_Player.unk60 = 0;
            g_Player.damageTaken = g_Player.unk64;
            g_api.PlaySfx(0x704);
        } else if (g_Player.unk60 == 0) {
            PLAYER.step = PL_S_HIT;
            PLAYER.step_s = 1;
        }
    }
}

// Compare to DRA func_80115394
enum DeathKind {
    DEATH_GENERIC,
    DEATH_BY_FIRE,
    DEATH_BY_THUNDER,
    DEATH_BY_ICE,
};
static u8 dead_dissolve_bmp[0x1400];
static s16 D_80174F68;
static s16 D_80174F6C;
static enum DeathKind death_kind;
void RicHandleDead(s32 damageEffects, s32 arg1, s32 arg2, s32 arg3) {
    s32 j;
    s32 i;
    u8* s2;
    u8* imgPtr;
    s32 disableColorChange = 0;
    PlayerDraw* playerDraw = &g_PlayerDraw[0];

    switch (PLAYER.step_s) {
    case 0:
        func_80159BC8();
        func_80159C04();
        func_8015CAAC(FIX(-0.75));
        MmxSetAnimation(PL_A_DUMMY);
        PLAYER.velocityY = FIX(-1.625);
        g_api.PlaySfx(SFX_UNK_6FF);
        if (damageEffects & ELEMENT_FIRE) {
            func_8015FA5C(0);
            // RIC blueprint 33 has child 31, EntityPlayerBlinkWhite
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(BP_RIC_BLINK, 0x48), 0);
            // RIC blueprint 53 has child 9, func_80161C2C
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(BP_DEATH_BY_FIRE, 2), 0);
            death_kind = DEATH_BY_FIRE;
        } else if (damageEffects & ELEMENT_THUNDER) {
            func_8015FA5C(2);
            // RIC blueprint 33 has child 31, EntityPlayerBlinkWhite
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(BP_RIC_BLINK, 0x4C), 0);
            // RIC blueprint 48 has child 41, EntityHitByLightning
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(BP_HIT_BY_THUNDER, 1), 0);
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(BP_HIT_BY_THUNDER, 2), 0);
            death_kind = DEATH_BY_THUNDER;
        } else if (damageEffects & ELEMENT_ICE) {
            func_8015FA5C(3);
            // RIC blueprint 33 has child 31, EntityPlayerBlinkWhite
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(BP_RIC_BLINK, 0x4D), 0);
            // RIC blueprint 47 has child 40, EntityHitByIce
            RicCreateEntFactoryFromEntity(g_CurrentEntity, 47, 0);
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(BP_HIT_BY_ICE, 1), 0);
            death_kind = DEATH_BY_ICE;
            PLAYER.drawMode = DRAW_TPAGE2 | DRAW_TPAGE;
        } else {
            func_8015FA5C(1);
            // RIC blueprint 33 has child 31, EntityPlayerBlinkWhite
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(BP_RIC_BLINK, 0x4A), 0);
            // RIC blueprint 11 has child 5, RicEntityHitByCutBlood
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(BP_MULTIPLE_EMBERS, 5), 0);
            death_kind = DEATH_GENERIC;
        }
        playerDraw->r0 = playerDraw->b0 = playerDraw->g0 = playerDraw->r1 =
            playerDraw->b1 = playerDraw->g1 = playerDraw->r2 = playerDraw->b2 =
                playerDraw->g2 = playerDraw->r3 = playerDraw->b3 =
                    playerDraw->g3 = 0x80;
        playerDraw->enableColorBlend = 1;
        PLAYER.step_s++;
        break;
    case 1:
        if (PLAYER.animFrameIdx != 4) {
            break;
        }
        PLAYER.step_s++;
        break;
    case 2:
        // Unusual, FIX usually has denominator of 128
        PLAYER.velocityY += FIX(11.0 / 256);
        if (PLAYER.velocityY > FIX(1.0 / 16)) {
            PLAYER.velocityY >>= 2;
            PLAYER.velocityX >>= 3;
            StoreImage(&D_801545A0, dead_dissolve_bmp);
            D_80174F6C = 0;
            D_80174F68 = 0x40;
            PLAYER.step_s++;
        }
        break;
    case 3:
        if (g_Timer & 1) {
            break;
        }
        if (D_80174F68 > 16) {
            // Another really weird velocityY value.
            // It's 0x233.
            PLAYER.velocityY += FIX(0.0086);
        } else {
            PLAYER.velocityX = 0;
            PLAYER.velocityY = 0;
        }
        for (i = 0; i < 4; i++) {
            s2 = imgPtr = dead_dissolve_bmp;
            s2 += ((D_80174F6C >> 1) & 7);
            s2 += ((D_80174F6C & 0xFF) >> 4) << 6;
            for (j = 0; j < 0x28; j++) {
                if (D_80174F6C & 1) {
                    *(s2 + ((j & 7) * 8) + ((j >> 3) * 0x400)) &= 0xF0;
                } else {
                    *(s2 + ((j & 7) * 8) + ((j >> 3) * 0x400)) &= 0x0F;
                }
            }
            D_80174F6C += 0x23;
            D_80174F6C &= 0xFF;
        }
        LoadImage(&D_801545A0, imgPtr);
        if (--D_80174F68 == 0) {
            PLAYER.velocityY = 0;
            playerDraw->enableColorBlend = 0;
            PLAYER.step_s = 0x80;
        }
        break;
    case 0x80:
        break;
    }
    // Always happens, kind of weird
    if (!disableColorChange) {
        if (death_kind == DEATH_GENERIC) {
            if (playerDraw->r0 < 0xF8) {
                playerDraw->r0 += 2;
            }
            if (playerDraw->g0 > 8) {
                playerDraw->g0 -= 2;
            }

            playerDraw->r3 = playerDraw->r2 = playerDraw->r1 = playerDraw->r0;
            playerDraw->b0 = playerDraw->b1 = playerDraw->g1 = playerDraw->b2 =
                playerDraw->g2 = playerDraw->b3 = playerDraw->g3 =
                    playerDraw->g0;
        }
        if (death_kind == DEATH_BY_FIRE || death_kind == DEATH_BY_THUNDER) {
            if (playerDraw->g0 > 8) {
                playerDraw->g0 -= 2;
            }
            playerDraw->r3 = playerDraw->r2 = playerDraw->r1 = playerDraw->r0 =
                playerDraw->b0 = playerDraw->b1 = playerDraw->g1 =
                    playerDraw->b2 = playerDraw->g2 = playerDraw->b3 =
                        playerDraw->g3 = playerDraw->g0;
        }
        if (death_kind == DEATH_BY_ICE) {
            if ((playerDraw->r0 > 8) && (g_Timer & 1)) {
                playerDraw->r0 -= 1;
            }
            playerDraw->r3 = playerDraw->r2 = playerDraw->r1 = playerDraw->b3 =
                playerDraw->b2 = playerDraw->b1 = playerDraw->b0 =
                    playerDraw->r0;
        }
    }
}

static void RicHandleStandInAir(void) {
    if (PLAYER.step_s == 0) {
        PLAYER.velocityY += 0x3800;
        if (PLAYER.velocityY > 0) {
            PLAYER.velocityY = 0;
            PLAYER.step_s = 1;
        }
    } else if (g_Player.unk4E != 0) {
        g_Player.unk46 = 0;
        RicSetStep(PL_S_JUMP);
        MmxSetAnimation(PL_A_DUMMY);
        g_Player.unk44 = 0;
    }
    if (g_Player.unk72) {
        PLAYER.velocityY = 0;
    }
}

static void RicHandleEnableFlameWhip(void) {
    if ((PLAYER.animCurFrame == 0xB5) && (PLAYER.animFrameDuration == 1)) {
        RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_35, 0);
        g_api.PlaySfx(SFX_WEAPON_APPEAR);
    }

    if (PLAYER.animFrameDuration < 0) {
        RicSetStand(0);
        g_Player.unk46 = 0;
        RicCreateEntFactoryFromEntity(
            g_CurrentEntity, FACTORY(BP_RIC_BLINK, 0x45), 0);
        g_Player.timers[PL_T_POISON] = 0x800;
    }

    if (!(g_Player.pl_vram_flag & 1)) {
        RicSetFall();
        g_Player.unk46 = 0;
        RicCreateEntFactoryFromEntity(
            g_CurrentEntity, FACTORY(BP_RIC_BLINK, 0x45), 0);
        g_Player.timers[PL_T_POISON] = 0x800;
    }
}

static void RicHandleHydrostorm(void) {
    if (PLAYER.animFrameDuration < 0) {
        RicSetStand(0);
        g_Player.unk46 = 0;
    }

    if ((g_Player.pl_vram_flag & 1) == 0) {
        RicSetFall();
        g_Player.unk46 = 0;
    }
}

static void RicHandleGenericSubwpnCrash(void) {
    if (g_Player.unk4E) {
        RicSetStand(0);
        g_Player.unk46 = 0;
    }

    if ((g_Player.pl_vram_flag & 1) == 0) {
        RicSetFall();
        g_Player.unk46 = 0;
    }
}

static s32 throw_dagger_timer;
static void RicHandleThrowDaggers(void) {
    if (PLAYER.step_s == 0) {
        throw_dagger_timer = 0x200;
        PLAYER.step_s++;
    } else {
        RicCheckFacing();
        if (!--throw_dagger_timer) {
            g_Player.unk46 = 0;
            RicSetStand(0);
            g_Player.unk4E = 1;
        }
    }
    if (g_Player.padTapped & PAD_CROSS) {
        MmxSetJump(0);
        g_Player.unk46 = 0;
        g_Player.unk4E = 1;
        throw_dagger_timer = 0;
    }
    if (!(g_Player.pl_vram_flag & 1)) {
        RicSetFall();
        g_Player.unk46 = 0;
        g_Player.unk4E = 1;
        throw_dagger_timer = 0;
    }
}

// This happens when he dies in prologue and gets saved by Maria.
static s32 dead_prologue_timer;
static void RicHandleDeadPrologue(void) {
    switch (PLAYER.step_s) {
    case 0:
        g_CurrentEntity->flags |= FLAG_UNK_10000;
        g_unkGraphicsStruct.unk20 = 4;
        RicDecelerateX(0x2000);
        if (PLAYER.velocityX == 0) {
            MmxSetAnimation(PL_A_DUMMY);
            g_Player.timers[PL_T_INVINCIBLE_SCENE] = 4;
            RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_MARIA, 0);
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(BP_RIC_BLINK, 9), 0);
            D_801545AC = 0;
            PLAYER.step_s++;
        }
        break;
    case 1:
        PLAYER.drawFlags = FLAG_DRAW_ROTY;
        D_801545AA += 64;
        PLAYER.rotY = (rsin(D_801545AA) >> 0xA) + 256;
        if (D_801545AC != 0) {
            D_801545AC--;
            if ((D_801545AC) == 0) {
                PLAYER.drawFlags = 0;
                PLAYER.rotY = 0x100;
                RicCreateEntFactoryFromEntity(
                    g_CurrentEntity, BP_MARIA_POWERS_APPLIED, 0);
                g_DeadPrologueTimer = 0x90;
                PLAYER.step_s++;
            }
        }
        break;
    case 2:
        // In this step, Richter rises into the air, similar to when he uses the
        // Cross subweapon crash.
        if (!--g_DeadPrologueTimer) {
            MmxSetAnimation(PL_A_DUMMY);
            PLAYER.palette = 0x814E;
            g_CurrentEntity->velocityY = FIX(-1);
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, BP_REVIVAL_COLUMN, 0);
            g_DeadPrologueTimer = 0x30;
            g_api.PlaySfx(0x6E2);
            dead_prologue_timer = 0xA0;
            PLAYER.step_s++;
        }
        break;
    case 3:
        if (!--g_DeadPrologueTimer) {
            PLAYER.velocityY = 0;
            g_DeadPrologueTimer = 0xC0;
            RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_36, 0);
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(BP_37, 2), 0);
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(BP_38, 3), 0);
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(BP_39, 4), 0);
            PLAYER.palette = 0x813D;
            g_Player.timers[PL_T_INVINCIBLE_SCENE] = 0;
            PLAYER.step_s++;
        }
        if (g_Status.hp < g_Status.hpMax) {
            g_Status.hp++;
        }
        dead_prologue_timer--;
        if ((dead_prologue_timer >= 0) && (dead_prologue_timer % 20 == 0)) {
            g_api.PlaySfx(NA_SE_PL_RIC_UNK_6E2);
        }
        break;
    case 4:
        if (!--g_DeadPrologueTimer) {
            g_DeadPrologueTimer = 0x10;
            PLAYER.step_s++;
        }
        if (g_Status.hp < g_Status.hpMax) {
            g_Status.hp++;
        }
        dead_prologue_timer--;
        if ((dead_prologue_timer >= 0) && (dead_prologue_timer % 20 == 0)) {
            g_api.PlaySfx(NA_SE_PL_RIC_UNK_6E2);
        }
        break;
    case 5:
        if (g_DeadPrologueTimer == 5) {
            PLAYER.animFrameIdx = 6;
            PLAYER.palette = 0x8120;
            RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_BLUE_SPHERE, 0);
        }
        if (!--g_DeadPrologueTimer) {
            MmxSetAnimation(PL_A_DUMMY);
            g_api.PlaySfx(SFX_WEAPON_APPEAR);
            RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_BLUE_CIRCLE, 0);
            PLAYER.step_s++;
            break;
        }
        if (g_Status.hp < g_Status.hpMax) {
            g_Status.hp++;
        }
        break;
    case 6:
        if (PLAYER.animFrameDuration < 0) {
            MmxSetAnimation(PL_A_DUMMY);
            PLAYER.step_s++;
        }
        break;
    case 7:
        if (PLAYER.animFrameDuration < 0) {
            g_CurrentEntity->flags &= ~FLAG_UNK_10000;
            g_unkGraphicsStruct.unk20 = 0;
            RicSetFall();
            MmxSetAnimation(PL_A_DUMMY);
            g_Player.timers[PL_T_INVINCIBLE_SCENE] = 4;
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(BP_RIC_BLINK, 9), 0);
        }
        break;
    }
}

static void RicHandleSlide(void) {
    s32 isTouchingGround = 0;

    if (!PLAYER.facingLeft) {
        isTouchingGround = (g_Player.pl_vram_flag & 4) != isTouchingGround;
    } else if (g_Player.pl_vram_flag & 8) {
        isTouchingGround = 1;
    }
    if ((PLAYER.posX.i.hi > (u8)-5) && (!PLAYER.facingLeft)) {
        isTouchingGround = 1;
    }
    if (PLAYER.posX.i.hi < 5 && (PLAYER.facingLeft)) {
        isTouchingGround = 1;
    }
    if ((!PLAYER.facingLeft &&
         (g_Player.colliders[2].effects & EFFECT_UNK_8000)) ||
        (PLAYER.facingLeft &&
         (g_Player.colliders[3].effects & EFFECT_UNK_8000))) {
        isTouchingGround = 1;
    }
    if (isTouchingGround && (PLAYER.animFrameIdx < 6)) {
        PLAYER.animFrameIdx = 6;
        if (PLAYER.velocityX > FIX(1)) {
            PLAYER.velocityX = FIX(2);
        }
        if (PLAYER.velocityX < FIX(-1)) {
            PLAYER.velocityX = FIX(-2);
        }
        RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_SKID_SMOKE, 0);
    }
    if (PLAYER.animFrameIdx < 5) {
        if (RicCheckInput(CHECK_FALL)) {
            return;
        }
        if ((!g_Player.unk72) && (g_Player.padTapped & PAD_CROSS)) {
            PLAYER.posY.i.hi -= 4;
            RicSetSlideKick();
            return;
        }
    } else if (PLAYER.animFrameIdx < 7) {
        if (RicCheckInput(CHECK_FALL | CHECK_SLIDE)) {
            return;
        }
    } else if (RicCheckInput(CHECK_FALL | CHECK_FACING | CHECK_SLIDE)) {
        return;
    }

    RicDecelerateX(0x2000);
    if (PLAYER.step_s == 0) {
        if (!(g_GameTimer & 3) && (2 < PLAYER.animFrameIdx) &&
            (PLAYER.animFrameIdx < 6)) {
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(BP_SLIDE, 2), 0);
        }
        if (PLAYER.animFrameIdx == 6 && PLAYER.animFrameDuration == 1) {
            RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_SKID_SMOKE, 0);
        }
        if (PLAYER.animFrameDuration < 0) {
            RicSetWalkFromJump(0, PLAYER.velocityX);
        }
    }
}

// same as DRA/func_80115C50
void func_8015BB80(void) {
    if (g_StageId == STAGE_TOP) {
        if (abs(g_Tilemap.left * 256 + g_PlayerX) - 8000 > 0) {
            PLAYER.posX.i.hi--;
        }

        if (abs(g_Tilemap.left * 256 + g_PlayerX) - 8000 < 0) {
            PLAYER.posX.i.hi++;
        }
    }

    if (g_StageId == (STAGE_TOP | STAGE_INVERTEDCASTLE_FLAG)) {

        if (abs(g_Tilemap.left * 256 + g_PlayerX) - 8384 > 0) {
            PLAYER.posX.i.hi--;
        }

        if (abs(g_Tilemap.left * 256 + g_PlayerX) - 8384 < 0) {
            PLAYER.posX.i.hi++;
        }
    }
}

// Corresponding DRA function is func_80115DA0
static void func_8015BCD0(void) {
    PLAYER.velocityY = 0;
    PLAYER.velocityX = 0;
    g_Player.padSim = 0;
    g_Player.D_80072EFC = 4;
    switch (PLAYER.step_s) {
    case 0:
        if (PLAYER.animFrameIdx == 5 && PLAYER.animFrameDuration == 1 &&
            RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_TELEPORT, 0) ==
                NULL) {
            PLAYER.animFrameDuration = 2;
        }
        if (PLAYER.animFrameDuration < 0) {
            RicSetStand(0);
        }
        break;
    case 2:
        func_8015BB80();
        if (PLAYER.animFrameIdx == 5 && PLAYER.animFrameDuration == 1 &&
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(BP_TELEPORT, 2), 0) == NULL) {
            PLAYER.animFrameDuration = 2;
        }
        if (PLAYER.animFrameDuration < 0) {
            RicSetStand(0);
        }
        break;
    case 4:
        func_8015BB80();
        if (PLAYER.animFrameIdx == 5 && PLAYER.animFrameDuration == 1 &&
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(BP_TELEPORT, 4), 0) == NULL) {
            PLAYER.animFrameDuration = 2;
        }
        if (PLAYER.animFrameDuration < 0) {
            RicSetStand(0);
        }
        break;
    case 1:
    case 3:
    case 5:
        if (PLAYER.animFrameDuration < 0) {
            RicSetStand(0);
        }
        if (g_Player.unk1C != 0) {
            RicSetStand(0);
        }
    }
}

static void RicHandleSlideKick(void) {
    // If we are pressing square while in contact with an enemy
    // (as detected in g_Player.unk44), we will bounce back.
    if (g_Player.padPressed & PAD_SQUARE && g_Player.unk44 & 0x80) {
        PLAYER.step = PL_S_JUMP;
        MmxSetAnimation(PL_A_DUMMY);
        RicSetSpeedX(FIX(-1.5));
        PLAYER.velocityY = 0;
        if (!g_Player.unk72) {
            PLAYER.velocityY = FIX(-4.5);
        }
        PLAYER.step_s = 2;
        g_Player.unk44 |= (8 + 2);
        g_Player.unk44 &= ~4;
        return;
    }
    RicDecelerateX(0x1000);
    PLAYER.velocityY += 0x1000;

    if (g_Player.pl_vram_flag & 1) {
        g_CurrentEntity->velocityX /= 2;
        RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_SKID_SMOKE, 0);
        PLAYER.facingLeft = (PLAYER.facingLeft + 1) & 1;
        RicSetCrouch(3, PLAYER.velocityX);
        g_api.PlaySfx(SFX_STOMP_SOFT_A);
        return;
    }

    if (g_Player.pl_vram_flag & 0xC) {
        PLAYER.velocityX = 0;
    }

    if (PLAYER.velocityX < 0) {
        if (g_Player.padPressed & PAD_RIGHT) {
            RicDecelerateX(0x2000);
        }
        if ((PLAYER.velocityX > (s32)0xFFFD0000) ||
            (g_Player.pl_vram_flag & 8)) {
            PLAYER.velocityX /= 2;
            PLAYER.facingLeft = (PLAYER.facingLeft + 1) & 1;
            MmxSetAnimation(PL_A_DUMMY);
            g_Player.unk44 = 0xA;
            PLAYER.step_s = 2;
            PLAYER.step = PL_S_JUMP;
        }
    }

    if (PLAYER.velocityX > 0) {
        if (g_Player.padPressed & PAD_LEFT) {
            RicDecelerateX(0x2000);
        }
        if ((PLAYER.velocityX <= 0x2FFFF) || (g_Player.pl_vram_flag & 4)) {
            PLAYER.velocityX /= 2;
            PLAYER.facingLeft = (PLAYER.facingLeft + 1) & 1;
            MmxSetAnimation(PL_A_DUMMY);
            g_Player.unk44 = 0xA;
            PLAYER.step_s = 2;
            PLAYER.step = PL_S_JUMP;
        }
    }
}

void RicHandleBladeDash(void) {
    RicDecelerateX(0x1C00);

    if (PLAYER.animFrameDuration < 0) {
        g_Player.unk46 = 0;
        RicSetStand(0);
    } else if (PLAYER.animFrameIdx >= 0x12 && !(g_Player.pl_vram_flag & 1)) {
        g_Player.unk46 = 0;
        RicSetFall();
    } else {
        if (!(g_GameTimer & 3) && PLAYER.animFrameIdx < 0x12 &&
            g_Player.pl_vram_flag & 1) {
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(BP_SLIDE, 2), 0);
        }

        if (PLAYER.animFrameIdx == 18 && PLAYER.animFrameDuration == 1 &&
            (g_Player.pl_vram_flag & 1)) {
            RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_SKID_SMOKE, 0);
        }
    }
}

static void RicHandleHighJump(void) {
    bool loadAnim = 0;
    s32 temp;

    g_Player.pl_high_jump_timer++;

    switch (PLAYER.step_s) {
    case 0:
        if (g_Player.padPressed & (PAD_LEFT | PAD_RIGHT)) {
            if (!PLAYER.facingLeft) {
                temp = g_Player.padPressed & PAD_RIGHT;
            } else {
                temp = g_Player.padPressed & PAD_LEFT;
            }
            if (temp == 0) {
                RicDecelerateX(0x1000);
            }
        } else {
            RicDecelerateX(0x1000);
        }

        if (g_Player.pl_vram_flag & 2) {
            func_80158B04(3);
            g_Player.pl_high_jump_timer = 0;
            PLAYER.step_s = 2;
        } else if (g_Player.pl_high_jump_timer >= 0x1D) {
            PLAYER.step_s = 1;
            PLAYER.velocityY = -0x60000;
        }
        break;

    case 1:
        if (g_Player.pl_vram_flag & 2) {
            PLAYER.step_s = 2;
            func_80158B04(3);
            g_Player.pl_high_jump_timer = 0;
        } else {
            PLAYER.velocityY += 0x6000;
            if (PLAYER.velocityY > 0x8000) {
                loadAnim = true;
            }
        }
        break;

    case 2:
        if (g_Player.pl_high_jump_timer >= 5) {
            loadAnim = true;
        }
        break;
    }

    if (loadAnim) {
        MmxSetAnimation(PL_A_DUMMY);
        RicSetStep(PL_S_JUMP);
    }
}

// Same function in DRA is func_8010D59C
void func_8015C4AC(void) {
    byte stackpad[40];
    Primitive* prim;
    s32 i;

    if (g_Entities[1].ext.entSlot1.unk0 != 0) {
        return;
    }
    if ((g_Player.padTapped & GAMEBUTTONS) ||
        ((g_Player.padHeld ^ g_Player.padPressed) & g_Player.padHeld &
         GAMEBUTTONS) ||
        (PLAYER.velocityY > FIX(0.5))) {
        g_Entities[1].ext.entSlot1.unk2 = 0;
        g_Entities[1].ext.entSlot1.unk3 = 0;
    } else {
        if (g_Entities[1].ext.entSlot1.unk2 >= 10) {
            return;
        }
        if (g_Entities[1].ext.entSlot1.unk3 == 0) {
            g_Entities[1].ext.entSlot1.unk3 =
                D_801545B0[g_Entities[1].ext.entSlot1.unk2];
        }
        if (!(--g_Entities[1].ext.entSlot1.unk3 & 0xFF)) {
            g_Entities[1].ext.entSlot1.unk2++;
            g_Entities[1].ext.entSlot1.unk3 =
                D_801545B0[g_Entities[1].ext.entSlot1.unk2];
        }
    }
    if (g_Entities[1].animFrameIdx != 0) {
        g_Entities[1].animFrameIdx--;
        return;
    }
    prim = &g_PrimBuf[g_Entities[1].primIndex];
    for (prim = &g_PrimBuf[g_Entities[1].primIndex], i = 0; prim != NULL;
         prim = prim->next, i++) {
        if (i == g_Entities[1].entityId) {
            prim->r0 = prim->g0 = prim->b0 = 0x80;
            prim->x0 = PLAYER.posX.i.hi;
            prim->y0 = PLAYER.posY.i.hi;
            prim->x1 = PLAYER.animCurFrame;
            prim->y1 = 0;
            prim->x2 = PLAYER.facingLeft;
            prim->y2 = PLAYER.palette;
        }
    }
    g_Entities[1].animFrameIdx = 2;
    g_Entities[1].entityId++;
    if (g_Entities[1].entityId >= 6) {
        g_Entities[1].entityId = 0;
    }
}

// Extremely similar to func_8010D800
void func_8015C6D4(void) {
    byte pad[0x28];
    PlayerDraw* plDraw;
    Primitive* prim;
    s32 entNum;
    s32 i;
    u8 temp_t0;
    u8 temp_t1;
    u8 temp_t2;

    prim = &g_PrimBuf[g_Entities[1].primIndex];
    temp_t2 = g_Entities[1].ext.entSlot1.unk1;
    temp_t1 = D_801545C4[g_Entities[1].ext.entSlot1.unk2];
    temp_t0 = D_801545D4[g_Entities[1].ext.entSlot1.unk2];

    plDraw = &g_PlayerDraw[1];
    for (i = 0; prim != NULL; prim = prim->next, i++) {
        if (temp_t0 < prim->r0) {
            prim->r0 -= temp_t1;
        }
        if (prim->r0 < 112 && prim->b0 < 240) {
            prim->b0 += 6;
        }
        if (prim->r0 < 88) {
            prim->y1 = 16;
        } else {
            prim->y1 = 0;
        }
        if (temp_t0 >= prim->r0) {
            prim->x1 = 0;
        }
        if (!((i ^ g_Timer) & 1)) {
            continue;
        }

        entNum = (i / 2) + 1;
        g_Entities[entNum].posX.i.hi = prim->x0;
        g_Entities[entNum].posY.i.hi = prim->y0;
        g_Entities[entNum].animCurFrame = prim->x1;
        g_Entities[entNum].drawMode = prim->y1;
        g_Entities[entNum].facingLeft = prim->x2;
        g_Entities[entNum].palette = prim->y2;
        g_Entities[entNum].zPriority = PLAYER.zPriority - 2;
        if (temp_t2) {
            g_Entities[entNum].animCurFrame = 0;
            prim->x1 = 0;
        }

        plDraw->r0 = plDraw->r1 = plDraw->r2 = plDraw->r3 = plDraw->b0 =
            plDraw->b1 = plDraw->b2 = plDraw->b3 = prim->r0;
        plDraw->g0 = plDraw->g1 = plDraw->g2 = plDraw->g3 = prim->b0;
        plDraw->enableColorBlend = true;
        plDraw++;
    }
}

void RicSetStep(PlayerSteps step) {
    PLAYER.step = step;
    PLAYER.step_s = 0;
}

void RicSetAnimation(AnimationFrame* anim) {
    g_CurrentEntity->anim = anim;
    g_CurrentEntity->animFrameDuration = 0;
    g_CurrentEntity->animFrameIdx = 0;
}

void RicDecelerateX(s32 speed) {
    if (g_CurrentEntity->velocityX < 0) {
        g_CurrentEntity->velocityX += speed;
        if (g_CurrentEntity->velocityX > 0) {
            g_CurrentEntity->velocityX = 0;
        }
    } else {
        g_CurrentEntity->velocityX -= speed;
        if (g_CurrentEntity->velocityX < 0)
            g_CurrentEntity->velocityX = 0;
    }
}

static void DecelerateY(s32 speed) {
    if (g_CurrentEntity->velocityY < 0) {
        g_CurrentEntity->velocityY += speed;
        if (g_CurrentEntity->velocityY > 0) {
            g_CurrentEntity->velocityY = 0;
        }
    } else {
        g_CurrentEntity->velocityY -= speed;
        if (g_CurrentEntity->velocityY < 0) {
            g_CurrentEntity->velocityY = 0;
        }
    }
}

s32 RicCheckFacing(void) {
    if (g_Player.unk44 & 2) {
        return 0;
    }

    // In the original game, Mega Man X stops walking when touching a wall
    bool checkWall = PLAYER.step == PL_S_STAND || PLAYER.step == PL_S_WALK;

    if (PLAYER.facingLeft == 1) {
        if (g_Player.padPressed & PAD_RIGHT) {
            PLAYER.facingLeft = 0;
            g_Player.unk4C = 1;
            if (checkWall && g_Player.unk04 & 4) {
                // ignore if moving to the right while touching the right wall
                return 0;
            }
            return -1;
        } else if (g_Player.padPressed & PAD_LEFT) {
            if (checkWall && g_Player.unk04 & 8) {
                // ignore if moving to the left while touching the left wall
                return 0;
            }
            return 1;
        }
    } else {
        if (g_Player.padPressed & PAD_RIGHT) {
            if (checkWall && g_Player.unk04 & 4) {
                // ignore if moving to the right while touching the right wall
                return 0;
            }
            return 1;
        }
        if (g_Player.padPressed & PAD_LEFT) {
            PLAYER.facingLeft = 1;
            g_Player.unk4C = 1;
            if (checkWall && g_Player.unk04 & 8) {
                // ignore if moving to the left while touching the left wall
                return 0;
            }
            return -1;
        }
    }
    return 0;
}

static bool MmxIsPressingBothLeftAndRight() {
    return g_Player.padPressed &
           (PAD_RIGHT | PAD_LEFT) == (PAD_RIGHT | PAD_LEFT);
}

static bool MmxIsHuggingWall() {
    if (MmxIsPressingBothLeftAndRight()) {
        return false;
    }
    if (!PLAYER.facingLeft) {
        return PLAYER.velocityX >= 0 && g_Player.padPressed & PAD_RIGHT &&
               g_Player.unk04 & 4;
    } else {
        return PLAYER.velocityX <= 0 && g_Player.padPressed & PAD_LEFT &&
               g_Player.unk04 & 8;
    }
}

void RicSetSpeedX(s32 speed) {
    if (g_CurrentEntity->facingLeft == 1)
        speed = -speed;
    g_CurrentEntity->velocityX = speed;
}

void func_8015CAAC(s32 speed) {
    if (PLAYER.entityRoomIndex == 1)
        speed = -speed;
    PLAYER.velocityX = speed;
}

void RicSetInvincibilityFrames(s32 kind, s16 invincibilityFrames) {
    if (!kind) {
        RicCreateEntFactoryFromEntity(
            g_CurrentEntity, FACTORY(BP_CRASH_DAGGER, 0x15), 0);
        if (g_Player.timers[PL_T_INVINCIBLE_SCENE] <= invincibilityFrames) {
            g_Player.timers[PL_T_INVINCIBLE_SCENE] = invincibilityFrames;
        }
    } else if (g_Player.timers[PL_T_INVINCIBLE] <= invincibilityFrames) {
        g_Player.timers[PL_T_INVINCIBLE] = invincibilityFrames;
    }
}

void DisableAfterImage(s32 resetAnims, s32 arg1) {
    Primitive* prim;

    if (resetAnims) {
        g_Entities[UNK_ENTITY_1].ext.generic.unk7C.S8.unk1 = 1;
        g_Entities[UNK_ENTITY_3].animCurFrame = 0;
        g_Entities[UNK_ENTITY_2].animCurFrame = 0;
        g_Entities[UNK_ENTITY_1].animCurFrame = 0;

        prim = &g_PrimBuf[g_Entities[UNK_ENTITY_1].primIndex];
        while (prim) {
            prim->x1 = 0;
            prim = prim->next;
        }
    }
    g_Entities[UNK_ENTITY_1].ext.generic.unk7C.S8.unk0 = 1;
    g_Entities[UNK_ENTITY_1].ext.generic.unk7E.modeU8.unk0 = 0xA;
    if (arg1) {
        g_Player.timers[PL_T_AFTERIMAGE_DISABLE] = arg1;
    }
}

void func_8015CC28(void) {
    Entity* entity = &g_Entities[UNK_ENTITY_1];

    entity->ext.generic.unk7E.modeU8.unk1 = 0;
    entity->ext.generic.unk7E.modeU8.unk0 = 0;
    entity->ext.generic.unk7C.U8.unk1 = 0;
    entity->ext.generic.unk7C.U8.unk0 = 0;
}

void RicSetDebug() { RicSetStep(PL_S_DEBUG); }

void func_8015CC70(s16 arg0) {
    PLAYER.step_s = arg0;
    PLAYER.step = PL_S_INIT;
    MmxSetAnimation(PL_A_STAND);
}

void RicSetCrouch(s32 kind, s32 velocityX) {
    RicSetStep(PL_S_CROUCH);
    MmxSetAnimation(PL_A_DUMMY);
    PLAYER.velocityX = velocityX;
    PLAYER.velocityY = 0;
    if (kind == 1) {
        MmxSetAnimation(PL_A_DUMMY);
        PLAYER.step_s = 4;
    }
    if (kind == 2) {
        MmxSetAnimation(PL_A_DUMMY);
        PLAYER.step_s = 1;
    }
    if (kind == 3) {
        MmxSetAnimation(PL_A_DUMMY);
        PLAYER.step_s = 4;
    }
}

void RicSetStand(s32 velocityX) {
    PLAYER.velocityX = velocityX;
    PLAYER.velocityY = 0;
    g_Player.unk44 = 0;
    RicSetStep(PL_S_STAND);
    g_CurrentEntity = &PLAYER;
    MmxSetAnimation(PL_A_STAND);
    g_DashAirUsed = false;
}

void RicSetWalk(s32 arg0) {
    g_Player.timers[8] = 12;
    g_Player.timers[1] = 12;
    g_Player.unk44 = 0;
    RicSetStep(PL_S_WALK);
    MmxSetAnimation(PL_A_WALK);
    RicSetSpeedX(MMX_WALK_SPEED);
    PLAYER.velocityY = 0;
    g_DashAirUsed = false;
}

void RicSetRun(void) {
    if (g_Player.unk7A != 0) {
        RicSetWalk(0);
    } else {
        g_Player.unk44 = 0;
        RicSetStep(PL_S_RUN);
        MmxSetAnimation(PL_A_WALK);
        RicSetSpeedX(FIX(2.25));
        g_Player.timers[PL_T_RUN] = 40;
        PLAYER.velocityY = 0;
        RicCreateEntFactoryFromEntity(
            g_CurrentEntity, FACTORY(BP_SMOKE_PUFF, 5), 0);
    }
}

void RicSetWalkFromJump(s32 strengthOfLanding, s32 velocityX) {
    RicSetStep(PL_S_WALK);
    MmxSetAnimation(PL_A_WALK);
    PLAYER.velocityX = velocityX;
    PLAYER.velocityY = 0;
    g_DashAirUsed = false;
}

void MmxPrepareStandFromJump(s32 velocityX) {
    PLAYER.velocityX = velocityX;
    PLAYER.velocityY = 0;
    g_Player.unk44 = 0;
    RicSetStep(PL_S_STAND);
    g_CurrentEntity = &PLAYER;
    MmxSetAnimation(PL_A_LAND);
    g_DashAirUsed = false;
}

void RicSetFall(void) {
    switch (g_Player.prev_step) {
    case PL_S_RUN:
    case PL_S_SLIDE:
        break;
    default:
        PLAYER.velocityX = 0;
        break;
    }
    if (g_Player.prev_step == PL_S_RUN) {
        g_Player.unk44 = 0x10;
    }
    RicSetStep(PL_S_FALL);
    PLAYER.velocityY = FIX(2);
    switch (g_Player.prev_step) {
    case PL_S_SLIDE:
        g_Player.timers[PL_T_FALL] = 0;
        g_Player.timers[PL_T_5] = 0;
        PLAYER.animFrameIdx = 2;
        PLAYER.animFrameDuration = 0x10;
        PLAYER.velocityX /= 2;
        break;
    default:
        g_Player.timers[PL_T_5] = 4;
        g_Player.timers[PL_T_FALL] = 4;
        g_Player.timers[PL_T_CURSE] = 0;
        g_Player.timers[PL_T_8] = 0;
        MmxSetAnimation(PL_S_FALL);
        break;
    }
}

void MmxSetJump(int jumpDash) {
    if (g_Player.unk72) {
        RicSetFall();
        return;
    }
    if (PLAYER.step == PL_S_WALL) {
        RicSetSpeedX(
            PLAYER.facingLeft ? MMX_WALL_JUMP_FORCE : -MMX_WALL_JUMP_FORCE);
        g_Player.unk44 = jumpDash ? IS_DASHING : 0;
    } else if (RicCheckFacing() != 0 || PLAYER.step == Player_Slide) {
        RicSetSpeedX(jumpDash ? MMX_DASH_SPEED : MMX_WALK_SPEED);
        g_Player.unk44 = jumpDash ? IS_DASHING : 0;
    } else {
        PLAYER.velocityX = 0;
        g_Player.unk44 = 4;
    }
    MmxSetAnimation(PL_A_JUMP);
    g_JumpState = PL_JUMP_ASCENDING;
    RicSetStep(PL_S_JUMP);
    PLAYER.velocityY = -MMX_JUMP_SPEED; // gives around 48px of jump height
}

void RicSetHighJump(void) {
    RicSetStep(PL_S_HIGHJUMP);
    PLAYER.velocityX = 0;
    RicSetSpeedX(FIX(1.25));
    PLAYER.velocityY = FIX(-7.5);
    g_Player.pl_high_jump_timer = 0;
    MmxSetAnimation(PL_A_JUMP);
    func_8015CC28();
    RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_HIGH_JUMP, 0);
    g_api.PlaySfx(SFX_GRUNT_C);
    g_Player.timers[PL_T_12] = 4;
    if (g_Player.unk72) {
        PLAYER.velocityY = 0;
    }
}

s32 RicCheckSubwpnChainLimit(s16 subwpnId, s16 limit) {
    Entity* entity;
    s32 nFound;
    s32 nEmpty;
    s32 i;
    // Iterate through entities 32-48 (which hold subweapons)
    // Any that match the proposed ID increments the count.
    // If at any point the count reaches the limit, return -1.
    entity = &g_Entities[32];
    for (i = 0, nFound = 0, nEmpty = 0; i < 16; i++, entity++) {
        if (entity->entityId == E_NONE) {
            nEmpty++;
        }
        if (entity->ext.subweapon.subweaponId != 0 &&
            entity->ext.subweapon.subweaponId == subwpnId) {
            nFound++;
        }
        if (nFound >= limit) {
            return -1;
        }
    }
    // This will indicate that there is an available entity slot
    // to hold the subweapon we're trying to spawn.
    // At the end, if this is zero, there are none available so return
    // -1 to indicate there is no room for the proposed subweapon.
    if (nEmpty == 0) {
        return -1;
    }
    return 0;
}

static int GetEntityCountByEntityID(enum MmxEntities id) {
    int count = 0;
    for (int i = 32; i < 48; i++) {
        if (g_Entities[i].entityId == id) {
            count++;
        }
    }
    return count;
}
bool MmxPerformAttack(void) {
    s32 i;
    s16 poisoned;
    s32 temp_rand;
    s16 randOf6;

    // we can spawn a maximum of 3 concurrent lemons
    if (GetEntityCountByEntityID(E_W_LEMON) >= 3) {
        return false;
    }

    poisoned = g_Player.timers[0] != 0;
    for (i = 16; i < 31; i++) {
        DestroyEntity(&g_Entities[i]);
    }
    if (!RicCreateEntFactoryFromEntity(g_CurrentEntity, B_W_LEMON, 0)) {
        return false;
    }
    if (poisoned) {
        g_api.PlaySfx(0x6B5);
    } else {
        g_api.PlaySfx(0x706);
    }
    if (randOf6 == 0) {
        g_api.PlaySfx(0x6F9);
    }
    if (randOf6 == 1) {
        g_api.PlaySfx(0x6FA);
    }
    if (randOf6 == 2) {
        g_api.PlaySfx(0x6FB);
    }
    switch (PLAYER.step) {
    case PL_S_STAND:
        ChangeAnimToAttack();
        g_Player.timers[PL_T_ATTACK] = 16;
        return true;
    case PL_S_WALK:
    case PL_S_DASH:
    case PL_S_FALL:
    case PL_S_JUMP:
    case PL_S_WALL:
        ChangeAnimToAttack();
        break;
    // case PL_S_RUN:
    //     PLAYER.step = PL_S_STAND;
    //     RicSetAnimation(D_80155588);
    //     RicCreateEntFactoryFromEntity(g_CurrentEntity, FACTORY(0, 0),
    //     0); break;
    default:
        return false;
    }
    // PLAYER.step_s = 0x40;
    g_Player.timers[PL_T_ATTACK] = 16;
    return true;
}

int RicDoCrash(void) {
    SubweaponDef subWpn;
    Entity* subWpnEnt;
    s16 subWpnID;

    subWpnID = func_8015FB84(&subWpn, true, false);
    if (subWpnID < 0) {
        return 0;
    }
    if ((subWpnID == 3) && (g_Player.timers[3] != 0)) {
        return 0;
    }
    if ((subWpnID == 6) && (g_unkGraphicsStruct.D_800973FC != 0)) {
        return 0;
    }
    if (subWpn.blueprintNum != 0) {
        if (subWpnID == 1) {
            subWpnEnt = RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(subWpn.blueprintNum, 0x100), 0);
        } else {
            subWpnEnt = RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(subWpn.blueprintNum, 0), 0);
        }
    }
    if (subWpnEnt == NULL) {
        return 0;
    }
    subWpnID = func_8015FB84(&subWpn, true, true);
    g_Player.unk46 = 4;
    g_Player.unk4E = 0;
    PLAYER.velocityY = 0;
    PLAYER.velocityX = 0;
    switch (subWpnID) {
    case SUBWPN_NONE:
        RicSetStep(Player_FlameWhip);
        MmxSetAnimation(PL_A_DUMMY);
        RicCreateEntFactoryFromEntity(g_CurrentEntity, FACTORY(0x100, 36), 0);
        g_api.PlaySfx(0x6FC);
        break;
    case SUBWPN_DAGGER:
        RicSetStep(Player_ThousandBlades);
        MmxSetAnimation(PL_A_DUMMY);
        g_api.PlaySfx(0x6FC);
        RicCreateEntFactoryFromEntity(g_CurrentEntity, FACTORY(0x200, 33), 0);
        break;
    case SUBWPN_AXE:
        RicSetStep(Player_Teleport);
        MmxSetAnimation(PL_A_DUMMY);
        PLAYER.velocityY = FIX(-4.6875);
        if (g_Player.unk72 != 0) {
            PLAYER.velocityY = 0;
        }
        func_8015CC28();
        g_api.PlaySfx(0x6FC);
        RicCreateEntFactoryFromEntity(g_CurrentEntity, FACTORY(0x200, 33), 0);
        break;
    case SUBWPN_HOLYWATER:
        RicSetStep(Player_Hydrostorm);
        MmxSetAnimation(PL_A_DUMMY);
        RicCreateEntFactoryFromEntity(g_CurrentEntity, FACTORY(0x4000, 33), 0);
        RicCreateEntFactoryFromEntity(g_CurrentEntity, FACTORY(0x4700, 33), 0);
        g_api.PlaySfx(0x700);
        break;
    case SUBWPN_REBNDSTONE:
    case SUBWPN_VIBHUTI:
    case SUBWPN_AGUNEA:
        RicSetStep(Player_Unk27);
        MmxSetAnimation(PL_A_DUMMY);
        RicCreateEntFactoryFromEntity(g_CurrentEntity, FACTORY(0x4000, 33), 0);
        RicCreateEntFactoryFromEntity(g_CurrentEntity, FACTORY(0x4700, 33), 0);
        g_api.PlaySfx(0x6FC);
        break;
    case SUBWPN_BIBLE:
    case SUBWPN_STOPWATCH:
        RicSetStep(Player_Unk27);
        MmxSetAnimation(PL_A_DUMMY);
        RicCreateEntFactoryFromEntity(g_CurrentEntity, FACTORY(0x4000, 33), 0);
        RicCreateEntFactoryFromEntity(g_CurrentEntity, FACTORY(0x4700, 33), 0);
        g_api.PlaySfx(0x6FC);
        break;
    case SUBWPN_CROSS:
        RicSetStep(Player_Teleport);
        MmxSetAnimation(PL_A_DUMMY);
        PLAYER.velocityY = FIX(-4.6875);
        if (g_Player.unk72 != 0) {
            PLAYER.velocityY = 0;
        }
        func_8015CC28();
        g_api.PlaySfx(0x701);
        RicCreateEntFactoryFromEntity(g_CurrentEntity, FACTORY(0x200, 33), 0);
        break;
    }
    g_Player.timers[PL_T_12] = 4;
    return 1;
}
