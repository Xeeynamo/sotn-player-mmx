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
void RicInit(s16 initParam) {
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
    PLAYER.palette = 0x8120;
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
    PLAYER.anim = ric_anim_stand_relax;
    g_Player.unk5C = initParam;
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
    *spriteptr++ = D_801530AC;
    *spriteptr++ = D_80153AA0;
    *spriteptr++ = D_80153D24;
    *spriteptr++ = D_801541A8;
    for (e = &g_Entities[1], i = 0; i < 3; i++, e++) {
        DestroyEntity(e);
        e->animSet = ANIMSET_OVL(0x10);
        e->unk5A = i + 1;
        e->palette = 0x8120;
        e->flags = FLAG_UNK_20000 | FLAG_UNK_08000000;
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
    unk0C = g_Player.unk0C;

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

static bool RicDebug(void);
void RicHandleDead(s32 damageEffects, s32 arg1, s32 arg2, s32 arg3);

void RicMain(void) {
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

    PlayerDraw* playerDraw = g_PlayerDraw;

    g_CurrentEntity = &PLAYER;
    g_Player.unk4C = 0;
    g_Player.unk72 = func_80156DE4();
    FntPrint("pl_head_f:%02x\n", g_Player.unk72);
    for (i = 0; i < LEN(g_Player.timers); i++) {
        if (!g_Player.timers[i]) {
            continue;
        }
        switch (i) {
        case PL_T_POISON:
        case PL_T_CURSE:
        case PL_T_3:
        case PL_T_5:
        case PL_T_6:
        case PL_T_7:
        case PL_T_8:
        case PL_T_9:
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
        case PL_T_15:
            func_8015CB58(0, 0);
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
        case PL_T_INVINCIBLE_SCENE:
            RicSetInvincibilityFrames(1, 16);
            break;
        case PL_T_6:
            if ((PLAYER.step == PL_S_FALL) && (PLAYER.anim != D_80155534)) {
                RicSetAnimation(D_80155534);
                g_Player.unk44 &= ~0x10;
            }
            break;
        case PL_T_15:
            func_8015CC28();
            break;
        }
    }
    g_Player.padHeld = g_Player.padPressed;
    if (g_Player.D_80072EFC) {
        g_Player.D_80072EFC--;
        g_Player.padPressed = g_Player.padSim;
    } else {
        g_Player.padPressed = g_pads[0].pressed;
    }
    g_Player.padTapped =
        (g_Player.padHeld ^ g_Player.padPressed) & g_Player.padPressed;
    if (PLAYER.step == PL_S_DEAD) {
        goto block_47;
    }
    // Reuse the i variable here even though we aren't iterating
    i = GetTeleportToOtherCastle();
    if (i != TELEPORT_CHECK_NONE) {
        func_8015CC70(i);
    }
    // Richter must use step #32 for something else, look into it!
    if (PLAYER.step == PL_S_INIT) {
        goto block_48;
    }
    if (g_DebugPlayer && RicDebug()) {
        return;
    }
    if (g_Player.unk60 >= 2) {
        goto block_47;
    }
    if (g_Player.unk60 == 1) {
        playerStep = PLAYER.step;
        playerStepS = PLAYER.step_s;
        RicSetStep(PL_S_BOSS_GRAB);
        goto block_48;
    }
    if ((g_Player.timers[PL_T_INVINCIBLE_SCENE] |
         g_Player.timers[PL_T_INVINCIBLE]) ||
        !PLAYER.hitParams) {
        goto block_47;
    }
    playerStep = PLAYER.step;
    playerStepS = PLAYER.step_s;
    damage.effects = PLAYER.hitParams & ~0x1F;
    damage.damageKind = PLAYER.hitParams & 0x1F;
    damage.damageTaken = PLAYER.hitPoints;
    isDamageTakenDeadly = g_api.CalcPlayerDamage(&damage);
    damageKind = damage.damageKind;
    damageEffects = damage.effects;
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
    goto block_48;
block_47:
    CheckBladeDashInput();
    CheckHighJumpInput();
block_48:
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
        RicHandleJump();
        break;
    case PL_S_HIGHJUMP:
        RicHandleHighJump();
        break;
    case PL_S_HIT:
        RicHandleHit(damageEffects, damageKind, playerStep, playerStepS);
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
    case Player_SlideKick:
        RicHandleSlideKick();
        break;
    case PL_S_BLADEDASH:
        RicHandleBladeDash();
        break;
    case PL_S_INIT:
        func_8015BCD0();
        break;
    }
    g_Player.unk08 = g_Player.unk0C;
#ifdef VERSION_PC
    var_s4 = 0;
#endif
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
    }
    if (g_Player.timers[PL_T_9]) {
        var_s4 |= PLAYER_STATUS_UNK400;
    }
    if (g_Player.timers[PL_T_10]) {
        var_s4 |= PLAYER_STATUS_UNK800;
    }
    if (g_Player.timers[PL_T_12]) {
        var_s4 |= PLAYER_STATUS_UNK1000;
    }
    if (*D_80097448 != 0) {
        var_s4 |= PLAYER_STATUS_UNK20000;
    }
    var_s4 |= PLAYER_STATUS_UNK10000000;
    g_Player.unk0C = var_s4;
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
        func_8015CB58(1, 4);
    }
    if (g_Player.timers[PL_T_INVINCIBLE_SCENE] |
        g_Player.timers[PL_T_INVINCIBLE]) {
        g_Player.unk0C |= 0x100;
    }
    g_api.UpdateAnim(D_80155964, D_8015538C);
    PLAYER.hitboxState = 1;
    PLAYER.hitParams = 0;
    PLAYER.hitPoints = 0;
    g_Player.unk7A = 0;
    if (PLAYER.anim == D_801556C4) {
        PLAYER.palette = D_80154574[PLAYER.animFrameIdx];
    }
    if ((PLAYER.anim == ric_ric_anim_stand_in_air) &&
        (PLAYER.animFrameIdx == 4)) {
        PLAYER.palette = D_80154594[PLAYER.animFrameDuration & 3];
    }
    if ((PLAYER.step == PL_S_DEAD) && (PLAYER.animFrameDuration < 0)) {
        PLAYER.animCurFrame |= ANIM_FRAME_LOAD;
    }
    if (g_Player.unk0C & 0x50) {
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

static bool RicDebug(void) {
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

void RicHandleStand(void) {
    s32 var_s0;

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

    if (!RicCheckInput(CHECK_FALL | CHECK_FACING | CHECK_JUMP | CHECK_CRASH |
                       CHECK_ATTACK | CHECK_CROUCH | CHECK_SLIDE)) {
        RicDecelerateX(0x2000);
        switch (PLAYER.step_s) {
        case 0:
            if (RicCheckFacing() == 0) {
                if (g_Player.padPressed & PAD_UP) {
                    RicSetAnimation(ric_anim_press_up);
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
            func_8015CB58(1, 1);
            if (PLAYER.animFrameIdx < 3) {
                RicCheckFacing();
                if (g_Player.padPressed & PAD_DOWN) {
                    PLAYER.step = PL_S_CROUCH;
                    PLAYER.anim = D_801555A8;
                    break;
                }
            }

            if (PLAYER.animFrameDuration < 0) {
                if (g_Player.padPressed & PAD_SQUARE) {
                    g_Player.unk46 = 2;
                    PLAYER.step_s++;
                    RicSetAnimation(ric_anim_brandish_whip);
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, BP_ARM_BRANDISH_WHIP, 0);
                    break;
                }
                g_Player.unk46 = 0;
                RicSetStand(0);
            }
            break;

        case 65:
            func_8015CB58(1, 1);
            if (g_Player.padPressed & PAD_SQUARE) {
                break;
            }
            g_Player.unk46 = 0;
            RicSetStand(0);
            break;

        case 66:
            func_8015CB58(1, 1);
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

void RicHandleWalk(void) {
    if (!RicCheckInput(CHECK_FALL | CHECK_FACING | CHECK_JUMP | CHECK_CRASH |
                       CHECK_ATTACK | CHECK_CROUCH)) {
        RicDecelerateX(0x2000);
        if (RicCheckFacing() == 0) {
            RicSetStand(0);
            return;
        }

        if (PLAYER.step_s == 0) {
            RicSetSpeedX(0x14000);
        }
    }
}

void RicHandleRun(void) {
    if (g_Player.unk7A != 0) {
        RicSetWalk(0);
        return;
    }
    g_Player.timers[PL_T_8] = 8;
    g_Player.timers[PL_T_CURSE] = 8;

    if (!RicCheckInput(CHECK_FALL | CHECK_FACING | CHECK_JUMP | CHECK_CRASH |
                       CHECK_ATTACK | CHECK_CROUCH)) {
        RicDecelerateX(0x2000);
        if (RicCheckFacing() == 0) {
            RicSetStand(0);
            if (!g_Player.timers[PL_T_RUN]) {
                if (!(g_Player.pl_vram_flag & 0xC)) {
                    RicSetAnimation(ric_anim_stop_run);
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

void RicHandleJump(void) {
    if (!g_IsPrologueStage && (PLAYER.velocityY < FIX(-1)) &&
        !(g_Player.unk44 & 0x40) && !(g_Player.padPressed & PAD_CROSS)) {
        PLAYER.velocityY = FIX(-1);
    }
    if ((g_Player.pl_vram_flag & 2) && (PLAYER.velocityY < FIX(-1))) {
        PLAYER.velocityY = FIX(-0.25);
        g_Player.unk44 |= 0x20;
    }
    if (RicCheckInput(
            CHECK_GROUND | CHECK_FACING | CHECK_ATTACK | CHECK_GRAVITY_JUMP)) {
        return;
    }
    switch (PLAYER.step_s) {
    // Need at least one fake case to make the switch match
    case 1:
        return;
    case 0:
        RicDecelerateX(0x1000);
        if (RicCheckFacing()) {
            if (g_Player.unk44 & 0x10) {
                RicSetSpeedX(FIX(2.25));
            } else {
                RicSetSpeedX(FIX(1.25));
            }
            g_Player.unk44 &= ~4;
        } else {
            g_Player.unk44 &= ~0x10;
            if ((PLAYER.animFrameIdx < 2) &&
                ((LOW(g_Player.unk44) & 0xC) == 4) &&
                (g_Player.padTapped & PAD_CROSS)) {
                RicSetAnimation(D_8015555C);
                RicSetSpeedX(FIX(-1.5));
                PLAYER.velocityY = FIX(-2.625);
                if (g_Player.unk72) {
                    PLAYER.velocityY = 0;
                }
                PLAYER.step_s = 2;
                g_Player.unk44 |= 0xA;
                g_Player.unk44 &= ~4;
            }
        }
        return;
    case 0x40:
        func_8015CB58(1, 1);
        if (PLAYER.animFrameIdx < 3) {
            if (RicCheckFacing() != 0) {
                if (g_Player.unk44 & 0x10) {
                    RicSetSpeedX(FIX(2.25));
                } else {
                    RicSetSpeedX(FIX(1.25));
                }
                g_Player.unk44 &= ~4;
            } else {
                g_Player.unk44 &= ~0x10;
            }
        } else {
            if (((g_Player.padPressed & PAD_RIGHT) && !PLAYER.facingLeft) ||
                ((g_Player.padPressed & PAD_LEFT) && PLAYER.facingLeft)) {
                if (g_Player.unk44 & 0x10) {
                    RicSetSpeedX(FIX(2.25));
                } else {
                    RicSetSpeedX(FIX(1.25));
                }
                RicSetSpeedX(FIX(1.25));
                g_Player.unk44 &= ~4;
            } else {
                g_Player.unk44 &= ~0x10;
            }
        }
        if (PLAYER.animFrameDuration < 0) {
            if (g_Player.padPressed & PAD_SQUARE) {
                g_Player.unk46 = 2;
                PLAYER.step_s += 1;
                RicSetAnimation(D_80155740);
                RicCreateEntFactoryFromEntity(
                    g_CurrentEntity, BP_ARM_BRANDISH_WHIP, 0);
            } else {
                g_Player.unk46 = 0;
                PLAYER.step_s = 0;
                RicSetAnimation(D_80155528);
            }
        }
        return;
    case 0x41:
        func_8015CB58(1, 1);
        if (!(g_Player.padPressed & PAD_SQUARE)) {
            g_Player.unk46 = 0;
            PLAYER.step_s = 0;
            RicSetAnimation(D_80155528);
        }
        return;
    case 0x42:
        func_8015CB58(1, 1);
        if (PLAYER.animFrameIdx < 3) {
            if (RicCheckFacing() != 0) {
                if (g_Player.unk44 & 0x10) {
                    RicSetSpeedX(FIX(2.25));
                } else {
                    RicSetSpeedX(FIX(1.25));
                }
                g_Player.unk44 &= ~4;
            } else {
                g_Player.unk44 &= ~0x10;
            }
        } else {
            if (((g_Player.padPressed & PAD_RIGHT) && !PLAYER.facingLeft) ||
                ((g_Player.padPressed & PAD_LEFT) && PLAYER.facingLeft)) {
                if (g_Player.unk44 & 0x10) {
                    RicSetSpeedX(FIX(2.25));
                } else {
                    RicSetSpeedX(FIX(1.25));
                }
                RicSetSpeedX(FIX(1.25));
                g_Player.unk44 &= ~4;
            } else {
                g_Player.unk44 &= ~0x10;
            }
        }
        if (PLAYER.animFrameDuration < 0) {
            g_Player.unk46 = 0;
            PLAYER.step_s = 0;
            RicSetAnimation(D_80155528);
        }
    }
}

void RicHandleFall(void) {
    if (RicCheckInput(
            CHECK_GROUND | CHECK_FACING | CHECK_ATTACK | CHECK_GRAVITY_FALL)) {
        return;
    }
    RicDecelerateX(0x1000);
    if (PLAYER.step_s != 0) {
        return;
    }
    if (g_Player.timers[PL_T_5] && g_Player.padTapped & PAD_CROSS) {
        RicSetJump();
    } else if (RicCheckFacing() != 0) {
        RicSetSpeedX(0xC000);
    }
}

void RicHandleCrouch(void) {
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
    if (RicCheckInput(CHECK_FALL | CHECK_FACING | CHECK_ATTACK | CHECK_SLIDE)) {
        return;
    }
    if ((g_Player.padTapped & PAD_CROSS) && (g_Player.unk46 == 0) &&
        (!g_Player.unk72)) {
        RicSetJump(1);
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
            RicSetAnimation(D_801554E0);
            PLAYER.step_s = 2;
            return;
        }
        break;
    case 0x1:
        if (!(g_Player.padPressed & PAD_DOWN) &&
            ((!g_Player.unk72) || !(g_Player.pl_vram_flag & 0x40))) {
            if (RicCheckFacing() == 0) {
                PLAYER.anim = D_801554E0;
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
        RicSetAnimation(ric_anim_crouch);
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
            RicSetAnimation(ric_anim_crouch);
            PLAYER.step_s = 0;
            return;
        }
        break;
    case 0x40:
        func_8015CB58(1, 1);
        if (PLAYER.animFrameIdx < 3) {
            RicCheckFacing();
            if (!(g_Player.padPressed & PAD_DOWN) && (!g_Player.unk72)) {
                PLAYER.step = PL_S_STAND;
                PLAYER.anim = D_80155588;
                return;
            }
        }
        if (PLAYER.animFrameDuration < 0) {
            if (g_Player.padPressed & PAD_SQUARE) {
                g_Player.unk46 = 2;
                PLAYER.step_s++;
                RicSetAnimation(D_80155738);
                RicCreateEntFactoryFromEntity(
                    g_CurrentEntity, BP_ARM_BRANDISH_WHIP, 0);
                return;
            }
            g_Player.unk46 = 0;
            PLAYER.step_s = 0;
            RicSetAnimation(ric_anim_crouch);
        }
        break;
    case 0x41:
        func_8015CB58(1, 1);
        if (!(g_Player.padPressed & PAD_SQUARE)) {
            g_Player.unk46 = 0;
            PLAYER.step_s = 0;
            RicSetAnimation(ric_anim_crouch);
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
    Entity* entity = PLAYER.ext.player.unkB8;
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

void RicHandleDead(
    s32 damageEffects, s32 damageKind, s32 prevStep, s32 prevStepS);

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
            PLAYER.anim = D_801556C4;
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
                    PLAYER.anim = D_8015569C;
                    g_api.PlaySfx(0x702);
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, BP_SKID_SMOKE, 0);
                    break;
                case PL_S_CROUCH:
                    PLAYER.velocityY = 0;
                    func_8015CAAC(FIX(-1.25));
                    PLAYER.step_s = 7;
                    PLAYER.anim = D_80155704;
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, BP_SKID_SMOKE, 0);
                    g_api.PlaySfx(0x703);
                    break;
                case PL_S_FALL:
                case PL_S_JUMP:
                    PLAYER.velocityY = FIX(-3);
                    func_8015CAAC(FIX(-1.25));
                    PLAYER.step_s = 1;
                    PLAYER.anim = ric_anim_stun;
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
                PLAYER.anim = ric_anim_stun;
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
                    PLAYER.anim = ric_anim_stun;
                    g_api.PlaySfx(0x702);
                    break;
                case PL_S_CROUCH:
                    PLAYER.velocityY = 0;
                    func_8015CAAC(FIX(-1.25));
                    PLAYER.step_s = 7;
                    PLAYER.anim = D_80155704;
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, BP_SKID_SMOKE, 0);
                    g_api.PlaySfx(0x703);
                    break;
                case PL_S_FALL:
                case PL_S_JUMP:
                    PLAYER.velocityY = FIX(-3);
                    func_8015CAAC(FIX(-1.25));
                    PLAYER.step_s = 1;
                    PLAYER.anim = ric_anim_stun;
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
            RicSetCrouch(0, PLAYER.velocityX);
        }
        break;
    }
    if (step_s_zero && (g_Player.unk72)) {
        PLAYER.velocityY = 0;
    }
}

void RicHandleBossGrab(void) {
    DamageParam damage;
    switch (g_CurrentEntity->step_s) {
    case 0:
        func_80159BC8();
        if (g_Player.unk62 == 0) {
            PLAYER.anim = ric_anim_stun;
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
STATIC_PAD_BSS(2);
static s16 D_80174F6C;
STATIC_PAD_BSS(2);
static enum DeathKind death_kind;
void RicHandleDead(
    s32 damageEffects, s32 damageKind, s32 prevStep, s32 prevStepS) {
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
        RicSetAnimation(D_80155544);
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

void RicHandleStandInAir(void) {
    if (PLAYER.step_s == 0) {
        PLAYER.velocityY += 0x3800;
        if (PLAYER.velocityY > 0) {
            PLAYER.velocityY = 0;
            PLAYER.step_s = 1;
        }
    } else if (g_Player.unk4E != 0) {
        g_Player.unk46 = 0;
        RicSetStep(PL_S_JUMP);
        RicSetAnimation(D_80155528);
        g_Player.unk44 = 0;
    }
    if (g_Player.unk72) {
        PLAYER.velocityY = 0;
    }
}

void RicHandleEnableFlameWhip(void) {
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

void RicHandleHydrostorm(void) {
    if (PLAYER.animFrameDuration < 0) {
        RicSetStand(0);
        g_Player.unk46 = 0;
    }

    if ((g_Player.pl_vram_flag & 1) == 0) {
        RicSetFall();
        g_Player.unk46 = 0;
    }
}

void RicHandleGenericSubwpnCrash(void) {
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
void RicHandleThrowDaggers(void) {
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
        RicSetJump();
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
void RicHandleDeadPrologue(void) {
    switch (PLAYER.step_s) {
    case 0:
        g_CurrentEntity->flags |= FLAG_UNK_10000;
        g_unkGraphicsStruct.unk20 = 4;
        RicDecelerateX(0x2000);
        if (PLAYER.velocityX == 0) {
            RicSetAnimation(D_80155748);
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
            RicSetAnimation(D_801558B4);
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
            RicSetAnimation(D_801558D4);
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
            RicSetAnimation(D_801558DC);
            PLAYER.step_s++;
        }
        break;
    case 7:
        if (PLAYER.animFrameDuration < 0) {
            g_CurrentEntity->flags &= ~FLAG_UNK_10000;
            g_unkGraphicsStruct.unk20 = 0;
            RicSetFall();
            RicSetAnimation(D_801558DC);
            g_Player.timers[PL_T_INVINCIBLE_SCENE] = 4;
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(BP_RIC_BLINK, 9), 0);
        }
        break;
    }
}

void RicHandleSlide(void) {
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
        if (RicCheckInput(CHECK_FALL | CHECK_CRASH)) {
            return;
        }
        if ((!g_Player.unk72) && (g_Player.padTapped & PAD_CROSS)) {
            PLAYER.posY.i.hi -= 4;
            RicSetSlideKick();
            return;
        }
    } else if (PLAYER.animFrameIdx < 7) {
        if (RicCheckInput(CHECK_FALL | CHECK_CRASH | CHECK_SLIDE)) {
            return;
        }
    } else if (
        RicCheckInput(CHECK_FALL | CHECK_FACING | CHECK_CRASH | CHECK_SLIDE)) {
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
            RicSetCrouch(0, PLAYER.velocityX);
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
void func_8015BCD0(void) {
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

void RicHandleSlideKick(void) {
    // If we are pressing square while in contact with an enemy
    // (as detected in g_Player.unk44), we will bounce back.
    if (g_Player.padPressed & PAD_SQUARE && g_Player.unk44 & 0x80) {
        PLAYER.step = PL_S_JUMP;
        RicSetAnimation(D_8015555C);
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
            RicSetAnimation(D_80155788);
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
            RicSetAnimation(D_80155788);
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

void RicHandleHighJump(void) {
    bool loadAnim = 0;
    s32 temp;

    FntPrint("pl_vram_flag:%04x\n", g_Player.pl_vram_flag);
    FntPrint("pl_high_jump_timer:%04x\n", g_Player.pl_high_jump_timer);
    FntPrint("pl_step_s:%02x\n", PLAYER.step_s);
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
        RicSetAnimation(D_80155534);
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

void RicSetStep(int step) {
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

    if (PLAYER.facingLeft == 1) {
        if (g_Player.padPressed & PAD_RIGHT) {
            PLAYER.facingLeft = 0;
            g_Player.unk4C = 1;
            return -1;
        } else if (g_Player.padPressed & PAD_LEFT) {
            return 1;
        }
    } else {
        if (g_Player.padPressed & PAD_RIGHT) {
            return 1;
        }
        if (g_Player.padPressed & PAD_LEFT) {
            PLAYER.facingLeft = 1;
            g_Player.unk4C = 1;
            return -1;
        }
    }
    return 0;
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

void func_8015CB58(s32 arg0, s32 arg1) {
    Primitive* prim;

    FntPrint("op disable\n");
    if (arg0 != 0) {
        g_Entities[UNK_ENTITY_1].ext.generic.unk7C.S8.unk1 = 1;
        g_Entities[UNK_ENTITY_3].animCurFrame = 0;
        g_Entities[UNK_ENTITY_2].animCurFrame = 0;
        g_Entities[UNK_ENTITY_1].animCurFrame = 0;

        prim = &g_PrimBuf[g_Entities[UNK_ENTITY_1].primIndex];
        while (prim != NULL) {
            prim->x1 = 0;
            prim = prim->next;
        }
    }
    g_Entities[UNK_ENTITY_1].ext.generic.unk7C.S8.unk0 = 1;
    g_Entities[UNK_ENTITY_1].ext.generic.unk7E.modeU8.unk0 = 0xA;
    if (arg1 != 0) {
        g_Player.timers[PL_T_15] = 4;
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
    PLAYER.animFrameDuration = 0;
    PLAYER.animFrameIdx = 0;
    if (arg0 & 1) {
        PLAYER.anim = D_80155950;
    } else {
        PLAYER.anim = D_8015591C;
    }
}

void RicSetCrouch(s32 kind, s32 velocityX) {
    RicSetStep(PL_S_CROUCH);
    RicSetAnimation(ric_anim_crouch);
    PLAYER.velocityX = velocityX;
    PLAYER.velocityY = 0;
    if (kind == 1) {
        PLAYER.anim = ric_anim_crouch_from_stand2;
        PLAYER.step_s = 4;
    }
    if (kind == 2) {
        PLAYER.anim = ric_anim_crouch_from_stand;
        PLAYER.step_s = 1;
    }
    if (kind == 3) {
        PLAYER.anim = ric_anim_land_from_air_run;
        PLAYER.step_s = 4;
    }
}

// Corresponding DRA function is func_8010E570 (much more complex)
void RicSetStand(s32 velocityX) {
    PLAYER.velocityX = velocityX;
    PLAYER.velocityY = 0;
    g_Player.unk44 = 0;
    RicSetStep(PL_S_STAND);
    RicSetAnimation(ric_anim_stand);
}

void RicSetWalk(s32 arg0) {
    if (g_Player.timers[PL_T_8] && !g_Player.unk7A) {
        RicSetRun();
        return;
    }
    g_Player.timers[PL_T_CURSE] = 8;
    g_Player.timers[PL_T_8] = 12;
    g_Player.timers[PL_T_CURSE] = 12;
    g_Player.unk44 = 0;
    RicSetStep(PL_S_WALK);
    RicSetAnimation(ric_anim_walk);
    RicSetSpeedX(FIX(1.25));
    PLAYER.velocityY = 0;
}

void RicSetRun(void) {
    if (g_Player.unk7A != 0) {
        RicSetWalk(0);
    } else {
        g_Player.unk44 = 0;
        RicSetStep(PL_S_RUN);
        RicSetAnimation(ric_anim_run);
        RicSetSpeedX(FIX(2.25));
        g_Player.timers[PL_T_RUN] = 40;
        PLAYER.velocityY = 0;
        RicCreateEntFactoryFromEntity(
            g_CurrentEntity, FACTORY(BP_SMOKE_PUFF, 5), 0);
    }
}

void RicSetFall(void) {
    /**
     * TODO: labels are !FAKE
     */
    if (g_Player.prev_step != PL_S_RUN && g_Player.prev_step != PL_S_SLIDE) {
        PLAYER.velocityX = 0;
    }
    if (g_Player.prev_step != PL_S_WALK) {
        if (g_Player.prev_step != PL_S_RUN) {
            RicSetAnimation(D_80155534);
            goto block_6;
        }
        goto block_7;
    }
block_6:
    if (g_Player.prev_step == PL_S_RUN) {
    block_7:
        g_Player.unk44 = 0x10;
    }
    RicSetStep(PL_S_FALL);
    PLAYER.velocityY = 0x20000;
    g_Player.timers[PL_T_5] = 8;
    g_Player.timers[PL_T_6] = 8;
    g_Player.timers[PL_T_CURSE] = 0;
    g_Player.timers[PL_T_8] = 0;
    if (g_Player.prev_step == PL_S_SLIDE) {
        g_Player.timers[PL_T_6] = 0;
        g_Player.timers[PL_T_5] = 0;
        PLAYER.animFrameIdx = 2;
        PLAYER.animFrameDuration = 0x10;
        PLAYER.velocityX /= 2;
    }
}

void RicSetJump(void) {
    if (g_Player.unk72) {
        RicSetFall();
        return;
    }
    if (RicCheckFacing() != 0 || PLAYER.step == Player_Slide) {
        RicSetAnimation(D_8015550C);
        if (PLAYER.step == PL_S_RUN) {
            RicSetSpeedX(FIX(2.25));
            g_Player.unk44 = 0x10;
        } else {
            RicSetSpeedX(FIX(1.25));
            g_Player.unk44 = 0;
        }
    } else {
        RicSetAnimation(D_801554F0);
        PLAYER.velocityX = 0;
        g_Player.unk44 = 4;
    }
    RicSetStep(PL_S_JUMP);
    if (g_IsPrologueStage) {
        PLAYER.velocityY = -FIX(4.6875);
    } else {
        PLAYER.velocityY = -FIX(5.4375);
    }
}

void RicSetHighJump(void) {
    RicSetStep(PL_S_HIGHJUMP);
    PLAYER.velocityX = 0;
    RicSetSpeedX(FIX(1.25));
    PLAYER.velocityY = FIX(-7.5);
    g_Player.pl_high_jump_timer = 0;
    RicSetAnimation(ric_anim_high_jump);
    func_8015CC28();
    RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_HIGH_JUMP, 0);
    g_api.PlaySfx(SFX_GRUNT_C);
    g_Player.timers[PL_T_12] = 4;
    if (g_Player.unk72) {
        PLAYER.velocityY = 0;
    }
}

s32 func_8015D1D0(s16 subWpnId, s16 maxParticles) {
    Entity* entity;
    s32 nFound;
    s32 nEmpty;
    s32 i;

    entity = &g_Entities[32];
    for (i = 0, nFound = 0, nEmpty = 0; i < 16; i++, entity++) {
        if (entity->entityId == E_NONE) {
            nEmpty++;
        }
        if (entity->ext.generic.unkB0 != 0 &&
            entity->ext.generic.unkB0 == subWpnId) {
            nFound++;
        }
        if (nFound >= maxParticles) {
            return -1;
        }
    }
    if (nEmpty == 0) {
        return -1;
    }
    return 0;
}
