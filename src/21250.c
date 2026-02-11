// SPDX-License-Identifier: AGPL-3.0-or-later
#include "pl.h"
#include "sfx.h"

s32 func_8015D250(s32 unused_arg) {
    SubweaponDef subweapon;
    s16 subweaponId;

    if (!(g_Player.padPressed & PAD_UP)) {
        return 1;
    }

    subweaponId = func_8015FB84(&subweapon, false, false);
    if (subweaponId <= 0) {
        return 1;
    }
    if (subweapon.blueprintNum == 0) {
        return 4;
    }
    if (RicCheckSubwpnChainLimit(subweaponId, subweapon.chainLimit) < 0) {
        return 2;
    }
    subweaponId = func_8015FB84(&subweapon, false, true);
    if (subweaponId <= 0) {
        return 3;
    }
    if (g_Player.unk72) {
        return 5;
    }

    RicCreateEntFactoryFromEntity(g_CurrentEntity, subweapon.blueprintNum, 0);
    g_Player.timers[PL_T_10] = 4;

    switch (PLAYER.step) {
    case PL_S_RUN:
        PLAYER.step = PL_S_STAND;
        RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_SKID_SMOKE, 0);
        MmxSetAnimation(PL_A_DUMMY);
        break;
    case PL_S_STAND:
    case PL_S_WALK:
    case PL_S_CROUCH:
    case PL_S_DASH:
        PLAYER.step = PL_S_STAND;
        MmxSetAnimation(PL_A_DUMMY);
        break;
    case PL_S_FALL:
    case PL_S_JUMP:
        PLAYER.step = PL_S_JUMP;
        MmxSetAnimation(PL_A_DUMMY);
        break;
    }
    g_Player.unk46 = 3;
    PLAYER.step_s = 0x42;
    g_Player.timers[PL_T_10] = 4;
    return 0;
}

void MmxSetDash(void) {
    g_Player.unk44 |= IS_DASHING;
    RicSetStep(PL_S_DASH);
    MmxSetAnimation(PL_A_DASH);
    RicSetSpeedX(MMX_DASH_SPEED);
    g_DashTimer = 0;
}
void MmxSetDashAir(void) {
    g_Player.unk44 |= IS_DASHING;
    RicSetStep(PL_S_DASH_AIR);
    MmxSetAnimation(PL_A_DASH);
    RicSetSpeedX(MMX_DASH_SPEED);
    PLAYER.velocityY = 0; // comment this for a wild run :)))
    g_DashTimer = 0;
}

void RicSetDeadPrologue() { RicSetStep(PL_S_DEAD_PROLOGUE); }

void RicSetSlide(void) {
    RicCheckFacing();
    RicSetStep(PL_S_SLIDE);
    MmxSetAnimation(PL_A_DUMMY);
    g_CurrentEntity->velocityY = 0;
    RicSetSpeedX(FIX(5.5));
    func_8015CC28();
    RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_25, 0);
    g_api.PlaySfx(0x707);
    g_Player.timers[PL_T_12] = 4;
}

void RicSetSlideKick(void) {
    g_Player.unk44 = 0;
    RicSetStep(PL_S_SLIDE_KICK);
    MmxSetAnimation(PL_A_DUMMY);
    g_CurrentEntity->velocityY = FIX(-2);
    RicSetSpeedX(FIX(5.5));
    func_8015CC28();
    RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_25, 0);
    g_api.PlaySfx(SFX_GRUNT_B);
    g_Player.timers[PL_T_12] = 4;
    RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_31, 0);
}

void RicSetBladeDash(void) {
    RicSetStep(PL_S_BLADEDASH);
    MmxSetAnimation(PL_A_DUMMY);
    g_CurrentEntity->velocityY = 0;
    RicSetSpeedX(FIX(5.5));
    g_Player.unk46 = 5;
    g_Player.timers[PL_T_12] = 4;
    RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_BLADE_DASH, 0);
    func_8015CC28();
    g_api.PlaySfx(SFX_GRUNT_C);
    g_api.PlaySfx(0x707);
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

static void MmxSetWall(void) {
    MmxSetAnimation(PL_A_WALL);
    g_api.PlaySfx(SFX_STOMP_SOFT_A);
    RicSetStep(PL_S_WALL);
    g_CurrentEntity->velocityY = MMX_WALL_SPEED;
    g_WallSlideTimer = 0;
    g_DashAirUsed = false;
}

bool RicCheckInput(s32 checks) {
    s32 velYChange;
    u32 effects;

    if (checks & CHECK_FACING) {
        if (g_Player.unk46 == 0) {
            RicCheckFacing();
        }
    }
    velYChange = (checks & CHECK_GRAVITY_FALL) ? FIX(28.0 / 128) : 0;
    if (checks & CHECK_GRAVITY_JUMP) {
        if (FIX(-1.0 / 8) < PLAYER.velocityY &&
            PLAYER.velocityY < FIX(3.0 / 8) && !(g_Player.unk44 & 0x20) &&
            (g_Player.padPressed & PAD_CROSS)) {
            // Note that 5.6 is precisely 1/5 of 28.
            velYChange = FIX(5.6 / 128);
        } else {
            velYChange = FIX(28.0 / 128);
        }
    }
    if (checks & CHECK_GRAVITY_HIT) {
        if (FIX(-1.0 / 8) < PLAYER.velocityY &&
            PLAYER.velocityY < FIX(3.0 / 8)) {
            velYChange = FIX(14.0 / 128);
        } else {
            velYChange = FIX(28.0 / 128);
        }
    }
    if (*D_80097448 >= 0x29) {
        velYChange /= 4;
    }
    PLAYER.velocityY += velYChange;
    if (PLAYER.velocityY > FIX(7)) {
        PLAYER.velocityY = FIX(7);
    }
    if ((checks & CHECK_80) && (g_Player.vram_flag & 2) &&
        (PLAYER.velocityY < FIX(-1))) {
        PLAYER.velocityY = FIX(-1);
    }
    if (PLAYER.velocityY >= 0) {
        if ((checks & CHECK_GROUND) && (g_Player.vram_flag & 1)) {
            g_JumpState = PL_JUMP_NONE;
            // 👇 maybe not needed any more
            // g_Player.unk46 = 0; // xeeynamo: forces to go to case 0
            switch (g_Player.unk46) {
            case 0:
            default:
                if (g_Player.unk44 & 8) {
                    RicSetWalkFromJump(3, PLAYER.velocityX);
                    g_api.PlaySfx(SFX_STOMP_SOFT_A);
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, BP_SKID_SMOKE, 0);
                    return true;
                }
                if (PLAYER.velocityY > FIX(6.875)) {
                    RicSetWalkFromJump(1, 0);
                    g_api.PlaySfx(SFX_STOMP_SOFT_A);
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, BP_SKID_SMOKE, 0);
                } else if (g_Player.padPressed & (PAD_LEFT | PAD_RIGHT)) {
                    RicSetWalk(0);
                } else {
                    MmxPrepareStandFromJump(0);
                }
                return true;
            case 1: // this seems to be related when landing while attacking?
                if (PLAYER.velocityY > FIX(6.875)) {
                    PLAYER.step = PL_S_CROUCH;
                    MmxSetAnimation(PL_A_DUMMY);
                    g_api.PlaySfx(SFX_STOMP_HARD_B);
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, BP_SKID_SMOKE, 0);
                } else {
                    PLAYER.step = PL_S_STAND;
                    MmxSetAnimation(PL_A_DUMMY);
                    if (g_Player.unk44 & 8) {
                        RicCreateEntFactoryFromEntity(
                            g_CurrentEntity, BP_SKID_SMOKE, 0);
                        g_api.PlaySfx(SFX_STOMP_HARD_B);
                    } else {
                        PLAYER.velocityX = 0;
                    }
                }
                PLAYER.velocityY = 0;
                g_Player.unk44 = 0;
                return true;
            case 2:
                if (PLAYER.velocityY > FIX(6.875)) {
                    PLAYER.step = PL_S_CROUCH;
                    MmxSetAnimation(PL_A_DUMMY);
                    g_api.PlaySfx(SFX_STOMP_SOFT_A);
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, BP_SKID_SMOKE, 0);
                } else {
                    PLAYER.step = PL_S_STAND;
                    MmxSetAnimation(PL_A_DUMMY);
                    MmxSetAnimation(PL_A_DUMMY);
                    if (!(g_Player.unk44 & 8)) {
                        PLAYER.velocityX = 0;
                    } else {
                        RicCreateEntFactoryFromEntity(
                            g_CurrentEntity, BP_SKID_SMOKE, 0);
                        g_api.PlaySfx(SFX_STOMP_SOFT_A);
                    }
                }
                PLAYER.velocityY = 0;
                g_Player.unk44 = 0;
                return true;
            case 3:
                if ((PLAYER.velocityY > FIX(6.875)) || (g_Player.unk44 & 8)) {
                    g_api.PlaySfx(SFX_STOMP_SOFT_A);
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, BP_SKID_SMOKE, 0);
                }
                PLAYER.velocityY = 0;
                if (!(g_Player.unk44 & 8)) {
                    PLAYER.velocityX = 0;
                }
                PLAYER.step = PL_S_STAND;
                MmxSetAnimation(PL_A_DUMMY);
                g_Player.unk44 = 0;
                return true;
            }
        } else if (
            checks & CHECK_GROUND_AFTER_HIT && (g_Player.vram_flag & 1)) {
            RicSetWalkFromJump(1, PLAYER.velocityX);
            g_api.PlaySfx(SFX_STOMP_SOFT_A);
            if (g_Player.unk5C && (g_Status.hp < 2)) {
                RicSetDeadPrologue();
                return true;
            } else {
                RicCreateEntFactoryFromEntity(
                    g_CurrentEntity, BP_SKID_SMOKE, 0);
            }
            return true;
        }
    }
    if (checks & CHECK_FALL && !(g_Player.vram_flag & 1)) {
        if (g_Player.unk46 != 0) {
            if (g_Player.unk46 == 1) {
                PLAYER.step_s = 0x40;
                PLAYER.step = PL_S_JUMP;
                MmxSetAnimation(PL_A_DUMMY);
                return true;
            }
            if (g_Player.unk46 == 2) {
                PLAYER.step_s = 0x41;
                PLAYER.step = PL_S_JUMP;
                MmxSetAnimation(PL_A_DUMMY);
                return true;
            }
            if (g_Player.unk46 == 3) {
                PLAYER.step_s = 0x42;
                PLAYER.step = PL_S_JUMP;
                MmxSetAnimation(PL_A_DUMMY);
                return true;
            }
        } else {
            RicSetFall();
            return true;
        }
    }
    if (checks & CHECK_WALL) {
        // check if the player is hugging the wall
        // ignore when pressing both left and right, like the original game
        if (MmxIsHuggingWall()) {
            MmxSetWall();
        }
    }
    if (g_Player.unk46 != 0) {
        return false;
    }
    if (checks & CHECK_SLIDE) {
        if (!PLAYER.facingLeft) {
            effects = g_Player.colFloor[2].effects & EFFECT_UNK_8000;
        } else {
            effects = g_Player.colFloor[3].effects & EFFECT_UNK_8000;
        }
        if (!effects && g_Player.padTapped & PAD_DASH) {
            if (PLAYER.step == PL_S_JUMP || PLAYER.step == PL_S_FALL) {
                // check for air-dash
                if (!g_DashAirUsed) {
                    g_DashAirUsed = true;
                    MmxSetDashAir();
                    return true;
                }
            } else {
                MmxSetDash();
                return true;
            }
        }
    }
    if (checks & CHECK_JUMP && g_Player.padTapped & PAD_CROSS) {
        bool dash = PLAYER.step == PL_S_DASH || g_Player.padHeld & PAD_DASH;
        MmxSetJump(dash ? JUMP_WITH_DASH : 0);
        return true;
    }
    if (checks & CHECK_ATTACK &&
        ((g_Player.padTapped | g_PadReleased) & PAD_SQUARE)) {
        bool result;
        s32 originalPosX;
        s32 originalPosY;
        bool isAboutToAttack = false;

        // if the attack button is tapped with no charge, then shoot
        if (g_ChargeLevel == CHARGE_NONE && g_Player.padTapped & PAD_SQUARE) {
            isAboutToAttack |= true;
        }

        // if the attack button is release while charging, then shoot
        if (g_ChargeLevel > CHARGE_NONE && (g_PadReleased & PAD_SQUARE)) {
            isAboutToAttack = true;
        }

        if (isAboutToAttack) {
            switch (PLAYER.step) {
            case PL_S_DASH:
                // when dashing, the projectile is emitted on different coords
                originalPosX = PLAYER.posX.val;
                originalPosY = PLAYER.posX.val;
                if (PLAYER.facingLeft) {
                    PLAYER.posX.i.hi -= 24;
                } else {
                    PLAYER.posX.i.hi += 24;
                }
                PLAYER.posY.i.hi += 4;
                result = MmxPerformAttack();
                PLAYER.posX.val = originalPosX;
                PLAYER.posX.val = originalPosY;
                break;
            case PL_S_WALL:
                // the animation is flipped when sliding to the wall
                PLAYER.facingLeft = !PLAYER.facingLeft;
                result = MmxPerformAttack();
                PLAYER.facingLeft = !PLAYER.facingLeft;
                break;
            default:
                result = MmxPerformAttack();
                break;
            }
            if (result) {
                return true;
            }
        }
    }
    if (checks & CHECK_CROUCH && (g_Player.padPressed & PAD_DOWN)) {
        RicSetWalkFromJump(2, 0);
        return true;
    }
    return false;
}

static s32 debug_wait_info_timer;
static void DebugShowWaitInfo(const char* msg) {
    g_CurrentBuffer = g_CurrentBuffer->next;
    FntPrint(msg);
    if (debug_wait_info_timer++ & 4) {
        FntPrint("\no\n");
    }
    DrawSync(0);
    VSync(0);
    PutDrawEnv(&g_CurrentBuffer->draw);
    PutDispEnv(&g_CurrentBuffer->disp);
    FntFlush(-1);
}

static void DebugInputWait(const char* msg) {
    while (PadRead(0))
        DebugShowWaitInfo(msg);
    while (PadRead(0) == 0)
        DebugShowWaitInfo(msg);
}

void func_8015E484(void) {
    s32 i;
    s32 collision = 0;
    s16 startingPosY = PLAYER.posY.i.hi;

    if (g_Player.vram_flag & 1 || g_IsRicDebugEnter || g_Player.unk78 == 1) {
        return;
    }
    if (PLAYER.posY.i.hi < 0x30) {
        PLAYER.posY.i.hi -= 0x10;
        while (true) {
            for (i = 0; i < 4; ++i) {
                g_api.CheckCollision(
                    (s16)(PLAYER.posX.i.hi + g_MmxSensorsFloor[i].x),
                    (s16)(PLAYER.posY.i.hi + g_MmxSensorsFloor[i].y),
                    &g_Player.colFloor[i], 0);
            }

            if ((g_Player.colFloor[1].effects & 0x81) == 1 ||
                (g_Player.colFloor[2].effects & 0x81) == 1 ||
                (g_Player.colFloor[3].effects & 0x81) == 1) {
                PLAYER.velocityY = 0;
                PLAYER.posY.i.hi -= 1;
                collision = 1;
            } else if (collision == 0) {
                PLAYER.posY.i.hi += 8;
                if (PLAYER.posY.i.hi >= 0x30) {
                    PLAYER.posY.i.hi = startingPosY;
                    return;
                }
            } else {
                return;
            }
        }

    } else if (PLAYER.posY.i.hi >= 0xB1) {
        PLAYER.posY.i.hi += 0x20;
        while (true) {
            for (i = 0; i < 4; ++i) {
                g_api.CheckCollision(
                    (s16)(PLAYER.posX.i.hi + g_MmxSensorsCeiling[i].x),
                    (s16)(PLAYER.posY.i.hi + g_MmxSensorsCeiling[i].y),
                    &g_Player.colCeiling[i], 0);
            }

            if ((g_Player.colCeiling[1].effects & 0x41) == 1 ||
                (g_Player.colCeiling[2].effects & 0x41) == 1 ||
                (g_Player.colCeiling[3].effects & 0x41) == 1) {
                PLAYER.velocityY = 0;
                PLAYER.posY.i.hi += 1;
                collision = 1;
            } else if (collision != 0) {
                PLAYER.posY.i.hi -= 1;
                return;
            } else {
                PLAYER.posY.i.hi -= 8;
                if (PLAYER.posY.i.hi < 0xB1) {
                    PLAYER.posY.i.hi = startingPosY;
                    return;
                }
            }
        }
    }
}

void RicGetPlayerSensor(Collider* col) {
    col->unk14 = g_MmxSensorsWall[0].x;
    col->unk1C = g_MmxSensorsWall[0].y;
    col->unk18 = g_MmxSensorsFloor[1].y - 1;
    col->unk20 = g_MmxSensorsCeiling[1].y + 1;
}

void RicCheckFloor(void) {
    Collider sp10;
    s32 temp_a0;
    s32 temp_a1;
    s32 temp_s0;
    s32 temp_s7;
    s32 i;
    s32 var_s5;
    s32 var_v1;
    u16 var_a1;
    s16 argX;
    s16 argY;
    s32 sp10effects;
    s16 temp_s16;

    u16* yPosPtr = &PLAYER.posY.i.hi;
    u16* xPosPtr = &PLAYER.posX.i.hi;
    s32* vram_ptr = &g_Player.vram_flag;

    var_s5 = 0;
    i = 0;
    if (g_unkGraphicsStruct.unk18) {
        *vram_ptr = 1;
        return;
    }
    if ((PLAYER.velocityY == 0) && (g_Player.unk04 & 1)) {
        var_s5 = 4;
    } else {
        i = 1;
    }
    for (; i < 4; i++, var_s5 = 0) {
        temp_a0 = g_Player.colFloor[i].effects;
        if (temp_a0 & EFFECT_SOLID_FROM_BELOW) {
            continue;
        }
        if ((temp_a0 & EFFECT_UNK_0002) || (PLAYER.velocityY >= 0) ||
            (PLAYER.step == PL_S_SLIDE_KICK && (temp_a0 & EFFECT_UNK_8000))) {
            temp_s0 = g_Player.colFloor[i].effects &
                      (EFFECT_UNK_8000 | EFFECT_UNK_0800 | EFFECT_SOLID);
            if ((temp_s0 == EFFECT_SOLID) ||
                (g_Player.colFloor[i].effects & EFFECT_UNK_0800)) {
                argX = *xPosPtr + g_MmxSensorsFloor[i].x;
                argY = *yPosPtr + g_MmxSensorsFloor[i].y +
                       (g_Player.colFloor[i].unk18 - 1);
                g_api.CheckCollision(argX, argY, &sp10, 0);
                sp10effects = sp10.effects;
                if (!(sp10effects & EFFECT_SOLID)) {
                    if (g_Player.colFloor[i].effects != EFFECT_SOLID ||
                        PLAYER.velocityY >= 0 ||
                        PLAYER.step == PL_S_SLIDE_KICK) {
                        if (temp_s0 & EFFECT_UNK_0800) {
                            *yPosPtr += var_s5 + g_Player.colFloor[i].unk8;
                        } else {
                            *yPosPtr += var_s5 + g_Player.colFloor[i].unk18;
                        }
                        *vram_ptr |= 1;
                        return;
                    }
                    continue;
                } else {
                    temp_a1 = sp10effects & (EFFECT_UNK_8000 | EFFECT_UNK_0002 |
                                             EFFECT_SOLID);
                    if ((temp_a1) == (EFFECT_UNK_8000 | EFFECT_SOLID)) {
                        if (i < 2) {
                            *vram_ptr |= ((sp10effects &
                                           (EFFECT_UNK_4000 | EFFECT_UNK_2000 |
                                            EFFECT_UNK_1000)) +
                                          temp_a1);
                            *yPosPtr += g_Player.colFloor[i].unk8 + sp10.unk18 +
                                        (var_s5 - 1);
                            return;
                        }
                        if ((i == 2) &&
                            ((sp10effects & (EFFECT_UNK_8000 | EFFECT_UNK_4000 |
                                             EFFECT_SOLID)) ==
                             (EFFECT_UNK_8000 | EFFECT_SOLID))) {
                            g_Player.colFloor[2].effects = sp10effects;
                            g_Player.colFloor[2].unk10 =
                                g_Player.colFloor[2].unk8;
                        }
                        if ((i == 3) &&
                            ((sp10effects & (EFFECT_UNK_8000 | EFFECT_UNK_4000 |
                                             EFFECT_SOLID)) ==
                             (EFFECT_UNK_8000 | EFFECT_UNK_4000 |
                              EFFECT_SOLID))) {
                            g_Player.colFloor[3].effects = sp10effects;
                            g_Player.colFloor[3].unk10 =
                                g_Player.colFloor[3].unk8;
                        }
                    }
                }
            }
            if ((temp_s0 != (EFFECT_UNK_8000 | EFFECT_SOLID)) || (i >= 2)) {
                continue;
            }
            *vram_ptr |= g_Player.colFloor[i].effects &
                         (EFFECT_UNK_8000 | EFFECT_UNK_4000 | EFFECT_UNK_2000 |
                          EFFECT_UNK_1000 | EFFECT_SOLID);
            *yPosPtr += var_s5 + g_Player.colFloor[i].unk18;
            return;
        }
    }
    if (g_Player.colFloor[1].effects & 4) {
        *vram_ptr |= 0x11;
        if ((g_Timer & 3) == 0) {
            (*yPosPtr)++;
        }
        return;
    }
    if (g_Player.colFloor[1].effects & 8) {
        *vram_ptr |= 0x80;
    }
    if (PLAYER.velocityY < 0) {
        return;
    }
    argX = *xPosPtr + g_MmxSensorsFloor[0].x;
    argY = *yPosPtr + g_MmxSensorsFloor[0].y + 10;
    g_api.CheckCollision(argX, argY, &sp10, 0);
    if ((sp10.effects & (EFFECT_UNK_8000 | EFFECT_SOLID)) != 0) {
        return;
    }

    for (i = 2; i < 4; i++) {
        if ((g_Player.colFloor[3].effects & EFFECT_UNK_8000) &&
            (g_Player.colFloor[2].effects & EFFECT_UNK_8000)) {
            return;
        }
        temp_s0 = g_Player.colFloor[i].effects;
        temp_s7 = ((g_Player.colFloor[i].effects &
                    (EFFECT_UNK_4000 | EFFECT_UNK_2000 | EFFECT_UNK_1000)) |
                   (EFFECT_UNK_8000 | EFFECT_SOLID));
        if (!(temp_s0 & EFFECT_UNK_8000)) {
            continue;
        }
        if (i == 2) {
            temp_a0 = EFFECT_UNK_4000;
            var_a1 = g_Player.colFloor[2].unk4;
            temp_s16 = g_Player.colFloor[2].unk4;
            var_v1 = temp_s16 + 8;
        } else {
            temp_a0 = 0;
            var_a1 = g_Player.colFloor[3].unkC;
            temp_s16 = g_Player.colFloor[3].unkC;
            var_v1 = 8 - temp_s16;
        }
        if ((temp_s0 & EFFECT_UNK_4000) == temp_a0) {
            argX = var_a1 + (*xPosPtr + g_MmxSensorsFloor[i].x);
            argY = *yPosPtr + g_MmxSensorsFloor[i].y;
            g_api.CheckCollision(argX, argY, &sp10, 0);
            if (sp10.effects & 1) {
                *yPosPtr += sp10.unk18;
                *vram_ptr |= temp_s7;
                return;
            }
            continue;
        }
        if (var_v1 <= 0) {
            continue;
        }
        if (!(temp_s0 & 1)) {
            continue;
        }
        argX = var_a1 + (*xPosPtr + g_MmxSensorsFloor[i].x);
        argY = *yPosPtr + g_MmxSensorsFloor[i].y + g_Player.colFloor[i].unk10;
        g_api.CheckCollision(argX, argY, &sp10, 0);
        if (sp10.effects & 1) {
            *yPosPtr += (sp10.unk18 + g_Player.colFloor[i].unk10);
            *vram_ptr |= temp_s7;
            return;
        }
    }
}

void RicCheckCeiling(void) {
    Collider collider;
    s32 temp_fp;
    u32 temp_s0;
    s32 temp_v1;
    s32 var_a0;
    s32 i;
    u16 var_a1;
    s16 temp_s16;

    s16 newY;

    s16 argX;
    s16 argY;

    u32 collidereffects;

    u16* yPosPtr = &PLAYER.posY.i.hi;
    u16* xPosPtr = &PLAYER.posX.i.hi;
    s32* vram_ptr = &g_Player.vram_flag;
    // weird thing where i has to get initialized first
    i = 1;

    if (g_unkGraphicsStruct.unk18) {
        return;
    }
    for (i = 1; i < 4; i++) {
        var_a0 = g_Player.colCeiling[i].effects;
        temp_s0 = var_a0 & (EFFECT_UNK_8000 | EFFECT_UNK_0800 | EFFECT_SOLID);
        if (var_a0 & EFFECT_SOLID_FROM_ABOVE) {
            continue;
        }
        if ((temp_s0 == EFFECT_SOLID) || (var_a0 & EFFECT_UNK_8000)) {
            if ((PLAYER.step == PL_S_SLIDE_KICK || PLAYER.step == PL_S_SLIDE) &&
                !(var_a0 & EFFECT_SOLID)) {
                continue;
            }
            argX = *xPosPtr + g_MmxSensorsCeiling[i].x;
            argY = *yPosPtr + g_MmxSensorsCeiling[i].y +
                   g_Player.colCeiling[i].unk10 + 1;
            g_api.CheckCollision(argX, argY, &collider, 0);
            collidereffects = collider.effects;
            if (!(collidereffects & 1)) {
                if ((g_Player.colCeiling[i].effects != EFFECT_SOLID) ||
                    (PLAYER.velocityY <= 0)) {
                    *vram_ptr |= 2;
                    if (!(*vram_ptr & 1) &&
                        ((g_Player.unk04 &
                          (EFFECT_SOLID_FROM_ABOVE | EFFECT_SOLID)) !=
                         (EFFECT_SOLID_FROM_ABOVE | EFFECT_SOLID))) {
                        if (g_Player.colCeiling[i].effects & EFFECT_UNK_8000) {
                            *yPosPtr += g_Player.colCeiling[i].unk10;
                        } else {
                            *yPosPtr += g_Player.colCeiling[i].unk20;
                        }
                    }
                    return;
                }
                continue;
            }
            if ((collider.effects &
                 (EFFECT_UNK_0800 | EFFECT_UNK_0002 | EFFECT_SOLID)) ==
                (EFFECT_UNK_0800 | EFFECT_SOLID)) {
                if (i < 2) {
                    *vram_ptr |= (EFFECT_UNK_0800 | EFFECT_UNK_0002 |
                                  ((collidereffects >> 4) &
                                   (EFFECT_UNK_0400 | EFFECT_UNK_0200 |
                                    EFFECT_UNK_0100)));
                    if (!(*vram_ptr & 1)) {
                        newY = *yPosPtr + 1 +
                               (g_Player.colCeiling[i].unk10 + collider.unk20);
                        *yPosPtr = newY;
                    }
                    return;
                }
                if ((i == 2) &&
                    ((collidereffects &
                      (EFFECT_UNK_4000 | EFFECT_UNK_0800 | EFFECT_SOLID)) ==
                     (EFFECT_UNK_0800 | EFFECT_SOLID))) {
                    g_Player.colFloor[2].effects = collidereffects;
                    g_Player.colFloor[2].unk8 = g_Player.colFloor[2].unk10;
                }
                if ((i == 3) &&
                    ((collidereffects &
                      (EFFECT_UNK_4000 | EFFECT_UNK_0800 | EFFECT_SOLID)) ==
                     (EFFECT_UNK_4000 | EFFECT_UNK_0800 | EFFECT_SOLID))) {
                    g_Player.colFloor[3].effects = collidereffects;
                    g_Player.colFloor[3].unk8 = g_Player.colFloor[3].unk10;
                }
            }
            if ((collidereffects & EFFECT_UNK_0800) == 0) {
                *vram_ptr |=
                    (EFFECT_UNK_0800 | EFFECT_UNK_0002 |
                     ((collidereffects >> 4) &
                      (EFFECT_UNK_0400 | EFFECT_UNK_0200 | EFFECT_UNK_0100)));
                if (!(*vram_ptr & 1)) {
                    newY = *yPosPtr + 1 +
                           (g_Player.colCeiling[i].unk10 + collider.unk20);
                    *yPosPtr = newY;
                }
                return;
            }
        }
        if ((temp_s0 == (EFFECT_UNK_0800 | EFFECT_SOLID)) && (i < 2)) {
            *vram_ptr |=
                (EFFECT_UNK_0800 | EFFECT_UNK_0002 |
                 ((g_Player.colCeiling[i].effects >> 4) &
                  (EFFECT_UNK_0400 | EFFECT_UNK_0200 | EFFECT_UNK_0100)));
            if (!(*vram_ptr & 1)) {
                *yPosPtr += g_Player.colCeiling[i].unk20;
            }
            return;
        }
    }

    if (PLAYER.velocityY > 0) {
        return;
    }
    argX = *xPosPtr + g_MmxSensorsCeiling[0].x;
    argY = (*yPosPtr + g_MmxSensorsCeiling[0].y) - 10;
    g_api.CheckCollision(argX, argY, &collider, 0);
    if ((collider.effects & 1) != 0) {
        return;
    }
    for (i = 2; i < 4; i++) {
        if ((g_Player.colFloor[7].effects & EFFECT_UNK_0800) &&
            (g_Player.colFloor[6].effects & EFFECT_UNK_0800)) {
            return;
        }
        temp_s0 = g_Player.colCeiling[i].effects;
        temp_fp = ((temp_s0 >> 4) &
                   (EFFECT_UNK_0400 | EFFECT_UNK_0200 | EFFECT_UNK_0100)) +
                  (EFFECT_UNK_0800 | EFFECT_UNK_0002);
        if (temp_s0 & EFFECT_UNK_0800) {
            if (i == 2) {
                var_a0 = EFFECT_UNK_4000;
                var_a1 = g_Player.colFloor[6].unk4;
                temp_s16 = g_Player.colFloor[6].unk4;
                temp_v1 = temp_s16 + 8;
            } else {
                var_a0 = 0;
                var_a1 = g_Player.colFloor[7].unkC;
                temp_s16 = g_Player.colFloor[7].unkC;
                temp_v1 = 8 - temp_s16;
            }
            if ((temp_s0 & EFFECT_UNK_4000) == var_a0) {
                argX = var_a1 + (*xPosPtr + g_MmxSensorsCeiling[i].x);
                argY = *yPosPtr + g_MmxSensorsCeiling[i].y;
                g_api.CheckCollision(argX, argY, &collider, 0);
                if (collider.effects & 1) {
                    *vram_ptr |= temp_fp;
                    if (!(*vram_ptr & 1)) {
                        *yPosPtr += collider.unk20;
                    }
                    return;
                }
            } else if ((temp_v1 > 0) && (temp_s0 & 1)) {
                argX = var_a1 + (*xPosPtr + g_MmxSensorsCeiling[i].x);
                argY = *yPosPtr + g_MmxSensorsCeiling[i].y +
                       g_Player.colCeiling[i].unk8;
                g_api.CheckCollision(argX, argY, &collider, 0);
                if (collider.effects & 1) {
                    if (!(*vram_ptr & 1)) {
                        *yPosPtr +=
                            collider.unk20 + g_Player.colCeiling[i].unk8;
                    }
                    *vram_ptr |= temp_fp;
                    return;
                }
            }
        }
    }
}

void RicCheckWallRight(void) {
    Collider collider;
    s32 temp_s0;
    s32 i;

    s16 argX;
    s16 argY;

    u16* yPosPtr = &PLAYER.posY.i.hi;
    u16* xPosPtr = &PLAYER.posX.i.hi;
    s32* vram_ptr = &g_Player.vram_flag;

    if (g_unkGraphicsStruct.unk18) {
        return;
    }
    temp_s0 =
        g_Player.unk04 & (EFFECT_UNK_8000 | EFFECT_UNK_4000 | EFFECT_UNK_0800 |
                          EFFECT_UNK_0400 | EFFECT_UNK_0002 | EFFECT_SOLID);
    if ((temp_s0 == (EFFECT_UNK_8000 | EFFECT_UNK_0002 | EFFECT_SOLID)) ||
        (temp_s0 == (EFFECT_UNK_0800 | EFFECT_UNK_0002 | EFFECT_SOLID)) ||
        (temp_s0 == (EFFECT_UNK_8000 | EFFECT_UNK_0800 | EFFECT_UNK_0002 |
                     EFFECT_SOLID))) {
        *vram_ptr |= 4;
        return;
    }

    for (i = 0; i < 7; i++) {
        temp_s0 = g_Player.colWall[i].effects &
                  (EFFECT_UNK_8000 | EFFECT_UNK_4000 | EFFECT_UNK_0800 |
                   EFFECT_UNK_0002 | EFFECT_SOLID);
        if ((temp_s0 == (EFFECT_UNK_8000 | EFFECT_UNK_4000 | EFFECT_SOLID)) ||
            (temp_s0 == (EFFECT_UNK_8000 | EFFECT_UNK_4000 | EFFECT_UNK_0002 |
                         EFFECT_SOLID)) ||
            (temp_s0 == (EFFECT_UNK_4000 | EFFECT_UNK_0800 | EFFECT_SOLID)) ||
            (temp_s0 == (EFFECT_UNK_4000 | EFFECT_UNK_0800 | EFFECT_UNK_0002 |
                         EFFECT_SOLID)) ||
            (temp_s0 == (EFFECT_UNK_8000 | EFFECT_UNK_0002 | EFFECT_SOLID)) ||
            (temp_s0 == (EFFECT_UNK_0002 | EFFECT_SOLID))) {
            argX =
                *xPosPtr + g_MmxSensorsWall[i].x + g_Player.colWall[i].unk4 - 1;
            argY = *yPosPtr + g_MmxSensorsWall[i].y;
            g_api.CheckCollision(argX, argY, &collider, 0);
            if ((collider.effects & 1) == 0) {
                *vram_ptr |= 4;
                *xPosPtr += g_Player.colWall[i].unk4;
                return;
            }
        }

        if (!(*vram_ptr & 1)) {
            if ((temp_s0 & (EFFECT_UNK_8000 | EFFECT_UNK_4000 |
                            EFFECT_UNK_0800)) == EFFECT_UNK_8000 &&
                (i != 0) &&
                ((g_Player.colWall[0].effects & EFFECT_UNK_0800) ||
                 !(g_Player.colWall[0].effects &
                   (EFFECT_UNK_8000 | EFFECT_UNK_0800 | EFFECT_UNK_0002)))) {
                *vram_ptr |= 4;
                *xPosPtr += g_Player.colWall[i].unk4;
                return;
            }
            if (((temp_s0 & (EFFECT_UNK_8000 | EFFECT_UNK_4000 |
                             EFFECT_UNK_0800)) == EFFECT_UNK_0800) &&
                (i != 6) &&
                ((g_Player.colWall[6].effects & EFFECT_UNK_8000) ||
                 !(g_Player.colWall[6].effects &
                   (EFFECT_UNK_8000 | EFFECT_UNK_0800 | EFFECT_UNK_0002)))) {
                *vram_ptr |= 4;
                *xPosPtr += g_Player.colWall[i].unk4;
                return;
            }
        }
    }
}

void RicCheckWallLeft(void) {
    Collider collider;
    s32 temp_s0;
    s32 i;
    s16 argX;
    s16 argY;

    u16* yPosPtr = &PLAYER.posY.i.hi;
    u16* xPosPtr = &PLAYER.posX.i.hi;
    s32* vram_ptr = &g_Player.vram_flag;

    if (g_unkGraphicsStruct.unk18) {
        return;
    }
    temp_s0 =
        g_Player.unk04 & (EFFECT_UNK_8000 | EFFECT_UNK_4000 | EFFECT_UNK_0800 |
                          EFFECT_UNK_0400 | EFFECT_UNK_0002 | EFFECT_SOLID);
    if ((temp_s0 == (EFFECT_UNK_8000 | EFFECT_UNK_4000 | EFFECT_UNK_0002 |
                     EFFECT_SOLID)) ||
        (temp_s0 == (EFFECT_UNK_0800 | EFFECT_UNK_0400 | EFFECT_UNK_0002 |
                     EFFECT_SOLID)) ||
        (temp_s0 == (EFFECT_UNK_8000 | EFFECT_UNK_4000 | EFFECT_UNK_0800 |
                     EFFECT_UNK_0400 | EFFECT_UNK_0002 | EFFECT_SOLID))) {
        *vram_ptr |= 8;
        return;
    }
    for (i = 7; i < 14; i++) {
        temp_s0 = g_Player.colWall[i].effects &
                  (EFFECT_UNK_8000 | EFFECT_UNK_4000 | EFFECT_UNK_0800 |
                   EFFECT_UNK_0002 | EFFECT_SOLID);
        if ((temp_s0 == (EFFECT_UNK_8000 | EFFECT_SOLID)) ||
            (temp_s0 == (EFFECT_UNK_8000 | EFFECT_UNK_0002 | EFFECT_SOLID)) ||
            (temp_s0 == (EFFECT_UNK_0800 | EFFECT_SOLID)) ||
            (temp_s0 == (EFFECT_UNK_0800 | EFFECT_UNK_0002 | EFFECT_SOLID)) ||
            (temp_s0 == (EFFECT_UNK_8000 | EFFECT_UNK_4000 | EFFECT_UNK_0002 |
                         EFFECT_SOLID)) ||
            (temp_s0 == (EFFECT_UNK_4000 | EFFECT_UNK_0800 | EFFECT_UNK_0002 |
                         EFFECT_SOLID)) ||
            (temp_s0 == (EFFECT_UNK_0002 | EFFECT_SOLID))) {
            argX =
                *xPosPtr + g_MmxSensorsWall[i].x + g_Player.colWall[i].unkC + 1;
            argY = *yPosPtr + g_MmxSensorsWall[i].y;
            g_api.CheckCollision(argX, argY, &collider, 0);
            if ((collider.effects & 1) == 0) {
                *vram_ptr |= 8;
                *xPosPtr += g_Player.colWall[i].unkC;
                return;
            }
        }
        if (!(*vram_ptr & 1)) {
            if (((temp_s0 &
                  (EFFECT_UNK_8000 | EFFECT_UNK_4000 | EFFECT_UNK_0800)) ==
                 (EFFECT_UNK_8000 | EFFECT_UNK_4000)) &&
                (i != 7) &&
                ((g_Player.colWall[7].effects & EFFECT_UNK_0800) ||
                 !(g_Player.colWall[7].effects &
                   (EFFECT_UNK_8000 | EFFECT_UNK_0800 | EFFECT_UNK_0002)))) {
                *vram_ptr |= 8;
                *xPosPtr += g_Player.colWall[i].unkC;
                return;
            }
            if (((temp_s0 &
                  (EFFECT_UNK_8000 | EFFECT_UNK_4000 | EFFECT_UNK_0800)) ==
                 (EFFECT_UNK_4000 | EFFECT_UNK_0800)) &&
                (i != 13) &&
                ((g_Player.colWall[13].effects & EFFECT_UNK_8000) ||
                 !(g_Player.colWall[13].effects &
                   (EFFECT_UNK_8000 | EFFECT_UNK_0800 | EFFECT_UNK_0002)))) {
                *vram_ptr |= 8;
                *xPosPtr += g_Player.colWall[i].unkC;
                return;
            }
        }
    }
}

void func_8015F9F0(Entity* entity);
void RicSetSubweaponParams(Entity* entity) {
    u16 attack;
    SubweaponDef* subwpn = &D_80154688_MMX[entity->ext.subweapon.subweaponId];

    if (g_Player.timers[PL_T_INVINCIBLE_SCENE]) {
        attack = subwpn->attack * 2;
    } else {
        attack = subwpn->attack;
    }

    entity->attack = attack;
    entity->attackElement = subwpn->attackElement;
    entity->hitboxState = subwpn->hitboxState;
    entity->nFramesInvincibility = subwpn->nFramesInvincibility;
    entity->stunFrames = subwpn->stunFrames;
    entity->hitEffect = subwpn->hitEffect;
    entity->entityRoomIndex = subwpn->entityRoomIndex;
    entity->attack = g_api.func_800FD664(entity->attack);
    func_8015F9F0(entity);
}

// We're playing as Richter and we used a subweapon (normal or crash)
s32 func_8015FB84(SubweaponDef* subwpn, s32 isItemCrash, s32 useHearts) {
    s32 pad[2]; // Needed so stack pointer moves properly
    u8 crashId;
    // Not an item crash. Just read the item in.
    if (isItemCrash == 0) {
        *subwpn = D_80154688_MMX[g_Status.subWeapon];
        if (g_Status.hearts >= subwpn->heartCost) {
            if (useHearts) {
                g_Status.hearts -= subwpn->heartCost;
            }
            return g_Status.subWeapon;
        }
    } else {
        // If it's a crash, load the subweapon by referencing our
        // subweapon's crash ID and loading that.
        crashId = D_80154688_MMX[g_Status.subWeapon].crashId;
        *subwpn = D_80154688_MMX[crashId];
        if (g_Status.hearts >= subwpn->heartCost) {
            if (useHearts) {
                g_Status.hearts -= subwpn->heartCost;
            }
            return g_Status.subWeapon;
        }
    }
    return -1;
}

// Corresponding DRA function is func_80119E78
u8 uv_anim_801548F4[6][8] = {
    {0x00, 0x50, 0x10, 0x50, 0x00, 0x60, 0x10, 0x60},
    {0x10, 0x50, 0x20, 0x50, 0x10, 0x60, 0x20, 0x60},
    {0x70, 0x40, 0x80, 0x40, 0x70, 0x50, 0x80, 0x50},
    {0x70, 0x30, 0x78, 0x30, 0x70, 0x38, 0x78, 0x38},
    {0x78, 0x30, 0x80, 0x30, 0x78, 0x38, 0x80, 0x38},
    {0x70, 0x38, 0x78, 0x38, 0x77, 0x40, 0x78, 0x40}};
s32 func_8015FDB0(Primitive* prim, s16 posX, s16 posY) {
    s16 offset;
    s32 ret;
    u8* uvAnim;

    ret = 0;
    uvAnim = uv_anim_801548F4;
    if (prim->b0 >= 6) {
        prim->b0 = 0;
        ret = -1;
    }

    uvAnim = &uvAnim[prim->b0 * 8];

    if (prim->b0 > 2) {
        offset = 4;
    } else {
        offset = 6;
    }

    prim->x0 = posX - offset;
    prim->y0 = posY - offset;
    prim->x1 = posX + offset;
    prim->y1 = posY - offset;
    prim->x2 = posX - offset;
    prim->y2 = posY + offset;
    prim->x3 = posX + offset;
    prim->y3 = posY + offset;
    prim->u0 = *uvAnim++;
    prim->v0 = *uvAnim++;
    prim->u1 = *uvAnim++;
    prim->v1 = *uvAnim++;
    prim->u2 = *uvAnim++;
    prim->v2 = *uvAnim++;
    prim->u3 = *uvAnim++;
    prim->v3 = *uvAnim;

    if (!(++prim->b1 & 1)) {
        prim->b0++;
    }
    return ret;
}

// Corresponding DRA function is func_80119F70
static Point16 D_80174FBC[16];
void RicEntityHitByHoly(Entity* entity) {
    Primitive* prim;
    s16 temp_xRand;
    s32 temp_yRand;
    s32 i;
    s16 hitboxY;
    s16 hitboxX;
    s32 temp;

    switch (entity->step) {
    case 0:
        entity->primIndex =
            (s16)g_api.AllocPrimitives(PRIM_GT4, LEN(D_80174FBC));
        if (entity->primIndex == -1) {
            DestroyEntity(entity);
            return;
        }
        entity->flags =
            FLAG_HAS_PRIMS | FLAG_POS_PLAYER_LOCKED | FLAG_UNK_20000;
        hitboxX = PLAYER.posX.i.hi + PLAYER.hitboxOffX;
        hitboxY = PLAYER.posY.i.hi + PLAYER.hitboxOffY;
        prim = &g_PrimBuf[entity->primIndex];
        for (i = 0; i < LEN(D_80174FBC); i++) {
            temp_xRand = hitboxX + rand() % 24 - 12;
            temp_yRand = rand();
            D_80174FBC[i].x = temp_xRand;
            D_80174FBC[i].y = hitboxY + temp_yRand % 48 - 24;
            prim->clut = 0x1B2;
            prim->tpage = 0x1A;
            prim->b0 = 0;
            prim->b1 = 0;
            prim->g0 = 0;
            prim->g1 = (rand() & 7) + 1;
            prim->g2 = 0;
            prim->priority = PLAYER.zPriority + 4;
            prim->drawMode = DRAW_UNK_100 | DRAW_TPAGE | DRAW_HIDE |
                             DRAW_UNK02 | DRAW_TRANSP;
            if (rand() & 1) {
                prim->drawMode =
                    DRAW_UNK_100 | DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE |
                    DRAW_HIDE | DRAW_UNK02 | DRAW_TRANSP;
            }
            prim = prim->next;
        }
        entity->step++;
        break;
    case 1:
        if (!(g_Player.status & 0x10000)) {
            DestroyEntity(entity);
            return;
        }
        break;
    }

    prim = &g_PrimBuf[entity->primIndex];
    for (i = 0; i < 16; i += 1) {
        switch (prim->g0) {
        case 0:
            if (!(--prim->g1 & 0xFF)) {
                prim->g0++;
            }
            break;
        case 1:
            hitboxY = D_80174FBC[i].x;
            hitboxX = D_80174FBC[i].y;
            temp = func_8015FDB0((POLY_GT4*)prim, hitboxY, hitboxX);
            D_80174FBC[i].y--;
            if (temp < 0) {
                prim->drawMode |= DRAW_HIDE;
                prim->g0++;
            } else {
                prim->drawMode &= ~DRAW_HIDE;
            }
            break;
        }
        prim = prim->next;
    }
}

// same as DRA/func_8011F074
static AnimationFrame anim_smoke_dark[] = {
    POSE(2, 1, 0),  POSE(2, 2, 0),  POSE(2, 3, 0),  POSE(2, 4, 0),
    POSE(2, 5, 0),  POSE(2, 6, 0),  POSE(2, 7, 0),  POSE(2, 8, 0),
    POSE(2, 9, 0),  POSE(2, 10, 0), POSE(2, 11, 0), POSE(2, 12, 0),
    POSE(2, 13, 0), POSE_END};
static s32 D_80174FFC;
void RicEntityHitByDark(Entity* entity) {
    s16 posX;
    s16 posY;

    switch (entity->step) {
    case 0:
        entity->flags =
            FLAG_UNK_20000 | FLAG_UNK_100000 | FLAG_POS_CAMERA_LOCKED;
        entity->unk5A = 0x79;
        entity->animSet = ANIMSET_DRA(14);
        entity->zPriority = PLAYER.zPriority + 2;
        entity->palette = 0x819F;

        if (D_80174FFC & 1) {
            entity->drawMode = DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE;
        } else {
            entity->drawMode = DRAW_TPAGE;
        }
        entity->scaleX = 0x40;
        entity->scaleY = 0x40;
        entity->anim = anim_smoke_dark;
        D_80174FFC++;
        entity->opacity = 0xFF;
        entity->drawFlags = FLAG_DRAW_SCALEX | FLAG_DRAW_SCALEY |
                            FLAG_DRAW_UNK10 | FLAG_DRAW_UNK20;
        posX = 10;
        posY = 15;
        entity->posY.i.hi = entity->posY.i.hi - posY + (rand() % 35);
        entity->posX.i.hi = entity->posX.i.hi - posX + (rand() % 20);
        entity->velocityY = -0x6000 - (rand() & 0x3FFF);
        entity->step++;
        break;

    case 1:
        if (entity->opacity >= 17) {
            entity->opacity += 248;
        }
        entity->posY.val += entity->velocityY;
        entity->scaleX += 8;
        entity->scaleY += 8;
        if (entity->poseTimer < 0) {
            DestroyEntity(entity);
        }
        break;
    }
}
