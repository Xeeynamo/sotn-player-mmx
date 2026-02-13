#include "pl.h"

// https://megaman.fandom.com/wiki/Mega_Man_X1_Damage_Data_Chart
static SubweaponDef weapons[] = {
    // dummy
    {0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0},
    // buster
    {1, 0, ELEMENT_HIT, 0, 4, 4, 0, 20, 2, 0x81, 0, 0, 0},
    // buster charge LV1
    {2, 0, ELEMENT_HIT, 0, 4, 4, 0, 21, 2, 0x81, 0, 0, 0},
    // buster charge X1 LV2
    {3, 0, ELEMENT_HIT, 0, 4, 4, 0, 22, 2, 0x81, 0, 0, 0},
    // buster charge X1 LV3
    {4, 0, ELEMENT_HIT, 0, 4, 4, 0, 22, 2, 0x81, 0, 0, 0},
    // shotgun ice
    {1, 1, ELEMENT_ICE, 0, 4, 4, 0, 23, 2, 0x81, 0, 0, 0},
    // electric spark
    {1, 1, ELEMENT_THUNDER, 0, 4, 4, 0, 24, 2, 0x81, 0, 0, 0},
    // rolling shield
    {1, 1, ELEMENT_STONE, 0, 4, 4, 0, 25, 2, 0x81, 0, 0, 0},
    // homing torpedo
    {1, 1, ELEMENT_DARK, 0, 4, 4, 0, 26, 2, 0x81, 0, 0, 0},
    // boomerang cutter
    {1, 1, ELEMENT_CUT, 0, 4, 4, 0, 27, 2, 0x81, 0, 0, 0},
    // chameleon sting
    {0, 1, ELEMENT_POISON, 0, 4, 4, 0, 28, 2, 0x81, 0, 0, 0},
    // storm tornado
    {1, 1, ELEMENT_WATER, 0, 4, 4, 0, 29, 2, 0x81, 0, 0, 0},
    // fire wave
    {1, 1, ELEMENT_FIRE, 0, 4, 4, 0, 30, 2, 0x81, 0, 0, 0},
    // hadouken
    {32, 0, ELEMENT_HOLY, 0, 4, 4, 0, 31, 2, 0x81, 0, 0, 0},
};
STATIC_ASSERT(LEN(weapons) == NUM_WEAPONS, "weapon array wrong size");

// very similar to RicSetSubweaponParams
static void SetWeaponParams(Entity* e, int weaponId) {
    SubweaponDef* w = &weapons[weaponId];
    e->attack = w->attack;
    e->attackElement = w->attackElement;
    e->hitboxState = w->hitboxState;
    e->nFramesInvincibility = w->nFramesInvincibility;
    e->stunFrames = w->stunFrames;
    e->hitEffect = w->hitEffect;
    e->entityRoomIndex = w->entityRoomIndex;
    e->attack = g_api.func_800FD664(w->attack);

    // multiply the base attack by a certain amount to not make it far too weak
    int attack;
    if (e->attack > 0) {
        attack = e->attack;
    } else {
        attack = 1;
    }
    attack = (attack << 3) + attack;

    // almost double the damage when attacked as revenge attack
    if (g_Player.timers[PL_T_INVINCIBLE]) {
        attack = (attack << 1) - (attack >> 1);
    }

    // exponentially increase damage on reverse castle, otherwise the game would
    // be too hard
    if (g_StageId & STAGE_INVERTEDCASTLE_FLAG) {
        attack <<= 1;
    }

    // prevent 16-bit overflow
    if (attack < 32767) {
        e->attack = (s16)attack;
    } else {
        e->attack = 32767;
    }
}

#include "assets/anims_buster.inc"
void EntityBuster(Entity* self) {
    const int Width = 8;
    Collider col;
    s32 sensorX;

    switch (self->step) {
    case 0:
        self->animSet = ANIMSET_OVL(0x11);
        self->ext.player.anim = Buster_Shot;
        self->drawFlags = 0;
        self->flags |= FLAG_POS_CAMERA_LOCKED;
        self->posX.i.hi += self->facingLeft ? -16 : 16;
        self->posY.i.hi += 5;
        self->zPriority++;
        SetWeaponParams(self, W_BUSTER);
        RicSetSpeedX(FIX(5));
        g_api.PlaySfx(SFX_WEAPON_SWISH_C);
        self->step++;
        break;
    case 1:
        self->posX.val += self->velocityX;
        if (self->hitFlags == 1 || self->hitFlags == 2) {
            goto dispose;
        }
        if (self->velocityX > 0) {
            sensorX = self->posX.i.hi + Width / 2;
        } else {
            sensorX = self->posX.i.hi - Width / 2;
        }
        g_api.CheckCollision(sensorX, self->posY.i.hi, &col, 0);
        if (col.effects & (EFFECT_SOLID | EFFECT_UNK_0002)) {
            goto dispose;
        }
        break;
    dispose:
        self->ext.player.anim = Buster_Impact;
        self->poseTimer = 0;
        self->pose = 0;
        self->hitboxState = 0;
        g_api.PlaySfx(SFX_UI_SUBWEAPON_TINK);
        self->step++;
        break;
    case 2:
        if (self->poseTimer < 0) {
            DestroyEntity(self);
            return;
        }
        break;
    }
    g_api.PlayAnimation((s8*)buster_hitboxes, buster_anims);
}

#include "assets/anims_charge_lv1.inc"
void EntityBusterChargeLv1(Entity* self) {
    switch (self->step) {
    case 0:
        self->animSet = ANIMSET_OVL(0x11);
        self->ext.player.anim = ChargeLv1_Shot;
        self->drawFlags = 0;
        self->flags |= FLAG_POS_CAMERA_LOCKED;
        self->posX.i.hi += self->facingLeft ? -16 : 16;
        self->posY.i.hi += 5;
        self->zPriority++;
        SetWeaponParams(self, W_BUSTER_CHARGE_LV1);
        g_api.PlaySfx(SFX_WEAPON_SWISH_C);
        self->step++;
        break;
    case 1:
        // in the first two anim frames, the projectile does not move
        if (self->pose == 4) {
            RicSetSpeedX(FIX(5));
            self->step++;
        }
        // fall back to case 2 as we still want to check for collisions
    case 2:
        self->posX.val += self->velocityX;
        if (self->hitFlags == 1 || self->hitFlags == 2) {
            // if hitting an enemy, and it's not dead yet, dissolve the bullet
            if (self->unkB8 && self->unkB8->enemyId &&
                self->unkB8->hitPoints > 0) {
                if (self->facingLeft) {
                    self->posX.i.hi -= 24;
                } else {
                    self->posX.i.hi += 24;
                }
                self->facingLeft = !self->facingLeft; // animation is flipped
                self->ext.player.anim = ChargeLv1_Impact;
                self->poseTimer = 0;
                self->pose = 0;
                self->hitboxState = 0;
                self->step++;
                g_api.PlaySfx(SFX_UI_SUBWEAPON_TINK);
            }
            // otherwise the projectile will pass through
        }
        break;
    case 3:
        if (self->poseTimer < 0) {
            DestroyEntity(self);
            return;
        }
        break;
    }
    g_api.PlayAnimation((s8*)charge_lv1_hitboxes, charge_lv1_anims);
}

#include "assets/anims_charge_x1_lv2.inc"
void EntityBusterChargeX1Lv2(Entity* self) {
    switch (self->step) {
    case 0:
        self->animSet = ANIMSET_OVL(0x11);
        self->ext.player.anim = ChargeX1Lv2_Shot;
        self->drawFlags = 0;
        self->flags |= FLAG_POS_CAMERA_LOCKED;
        self->posX.i.hi += self->facingLeft ? -16 : 16;
        self->posY.i.hi += 5;
        self->zPriority++;
        SetWeaponParams(self, W_BUSTER_CHARGE_X1_LV2);
        g_api.PlaySfx(SFX_WEAPON_SWISH_C);
        self->step++;
        break;
    case 1:
        // in the first two anim frames, the projectile does not move
        if (self->pose == 2) {
            RicSetSpeedX(FIX(8));
            self->step++;
        }
        // fall back to case 2 as we still want to check for collisions
    case 2:
        self->posX.val += self->velocityX;
        if (self->hitFlags == 1 || self->hitFlags == 2) {
            // if hitting an enemy, and it's not dead yet, dissolve the bullet
            if (self->unkB8 && self->unkB8->enemyId &&
                self->unkB8->hitPoints > 0) {
                // TODO implement impact animation
                DestroyEntity(self);
            }
            // otherwise the projectile will pass through
        }
        break;
    case 3:
        if (self->poseTimer < 0) {
            DestroyEntity(self);
            return;
        }
        break;
    }
    g_api.PlayAnimation((s8*)charge_x1_lv2_hitboxes, charge_x1_lv2_anims);
}

void EntityBusterChargeX1Lv3(Entity* self) { DestroyEntity(self); }

void EntityShotgunIce(Entity* self) { DestroyEntity(self); }
void EntityElectricSpark(Entity* self) { DestroyEntity(self); }
void EntityRollingShield(Entity* self) { DestroyEntity(self); }
void EntityHomingTorpedo(Entity* self) { DestroyEntity(self); }
void EntityBoomerangCutter(Entity* self) { DestroyEntity(self); }
void EntityChameleonSting(Entity* self) { DestroyEntity(self); }
void EntityStormTornado(Entity* self) { DestroyEntity(self); }
void EntityFireWave(Entity* self) { DestroyEntity(self); }
void EntityHadouken(Entity* self) { DestroyEntity(self); }

#include "assets/anims_charging.inc"
struct ExtChargeWeapon {
    s32 timer;
    s32 startAngle;
};
void EntityChargeWeaponParticle(Entity* self) {
    struct ExtChargeWeapon* ext = (struct ExtChargeWeapon*)&self->ext;

    // if the player dies, kill all the charging particles
    switch (PLAYER.step) {
    case PL_S_DEAD:
    case PL_S_DEAD_PROLOGUE:
        DestroyEntity(self);
        return;
    }

    // if the attack button is released, kill all the charging particles
    if (g_ChargeTimer < CHARGE_TIMER_LV1) {
        DestroyEntity(self);
        return;
    }

    switch (self->step) {
    case 0:
        self->animSet = ANIMSET_OVL(0x11);
        switch (g_ChargeLevel) {
        default:
        case CHARGE_MMX1_LV1:
            self->ext.player.anim = Charging_ChargingType1;
            break;
        case CHARGE_MMX1_LV2:
            self->ext.player.anim = Charging_ChargingType2;
            break;
        case CHARGE_MMX1_LV3:
            self->ext.player.anim = Charging_ChargingType3;
            break;
        }
        self->flags = FLAG_UNK_2000 | FLAG_UNK_00200000 | FLAG_NOT_AN_ENEMY |
                      FLAG_POS_CAMERA_LOCKED;
        self->zPriority++;
        self->enemyId = 0;
        self->hitboxState = 0;
        ext->timer = 16;
        ext->startAngle = (self->params & 0x1) * 0x800;
        ext->startAngle |= ((self->params & 0x2) != 0) * 0x400;
        ext->startAngle |= ((self->params & 0x4) != 0) * 0x200;
        ext->startAngle |= ((self->params & 0x8) != 0) * 0x100;
        self->step++;
        break;
    case 1:
        self->posX.i.hi = PLAYER.posX.i.hi +
                          ((rsin(ext->startAngle) * ext->timer * 0x3) >> 13);
        self->posY.i.hi = PLAYER.posY.i.hi +
                          ((rcos(ext->startAngle) * ext->timer * 0x3) >> 13);
        self->posY.i.hi += 4;
        if (!--ext->timer) {
            self->step++;
        }
        break;
    case 2:
        DestroyEntity(self);
        break;
    }
    g_api.PlayAnimation((s8*)charging_hitboxes, charging_anims);
}

static bool FallUntilFloorIsTouched(Entity* self) {
    Collider col;
    if (self->velocityY < 0) {
        g_api.CheckCollision(
            self->posX.i.hi, self->posY.i.hi - self->hitboxHeight, &col, 0);
        if (col.effects & EFFECT_NOTHROUGH) {
            self->velocityY = 0;
        }
    }
    self->posX.val += self->velocityX;
    self->posY.val += self->velocityY;
    g_api.CheckCollision(self->posX.i.hi, self->posY.i.hi, &col, 0);
    if (col.effects & EFFECT_NOTHROUGH && self->velocityY > 0) {
        self->velocityX = 0;
        self->velocityY = 0;
        self->posY.i.hi += col.unk18;
        return true;
    } else {
        if (self->velocityY < FALL_TERMINAL_VELOCITY) {
            self->velocityY += FALL_GRAVITY;
        }
    }
    return false;
}

struct ExtPrizeDrop {
    s32 disposeTimer;
    s32 prevFrame;
};
// return true if the prize is collected
static bool PrizeDropHelper(
    Entity* self, s8* hitboxes, AnimationFrame** anims) {
    struct ExtPrizeDrop* ext = (struct ExtPrizeDrop*)&self->ext;
    Collider col;

    if (self->step > 0 && self->hitFlags) {
        return true;
    }
    switch (self->step) {
    case 0:
        self->animSet = ANIMSET_OVL(0x12);
        self->ext.player.anim = 0;
        self->enemyId = 1;
        self->hitboxState = 1;
        self->flags = FLAG_UNK_2000 | FLAG_UNK_00200000 | FLAG_NOT_AN_ENEMY |
                      FLAG_KEEP_ALIVE_OFFCAMERA | FLAG_POS_CAMERA_LOCKED;
        self->zPriority++;
        self->step++;
        break;
    case 1:
        // avoid item getting stuck
        g_api.CheckCollision(self->posX.i.hi, self->posY.i.hi - 4, &col, 0);
        if (col.effects & EFFECT_NOTHROUGH_PLUS) {
            DestroyEntity(self);
            return false;
        }
        self->step++;
    case 2:
        if (FallUntilFloorIsTouched(self)) {
            ext->disposeTimer = 180;
            self->ext.player.anim = 1;
            self->step++;
        }
        break;
    case 3:
        if (!--ext->disposeTimer) {
            ext->disposeTimer = 60;
            ext->prevFrame = self->animCurFrame;
            self->step++;
        }
        break;
    case 4:
        if (!--ext->disposeTimer) {
            DestroyEntity(self);
            return false;
        }
        if (ext->disposeTimer & 2) {
            ext->prevFrame = self->animCurFrame;
            self->animCurFrame = 0;
            return false; // force a blank frame
        } else {
            self->animCurFrame = ext->prevFrame;
        }
        break;
    }
    g_api.PlayAnimation(hitboxes, anims);
    return false;
}

static void IncreaseHealth(s32 amount) {
    if (g_Status.hp >= g_Status.hpMax) {
        return;
    }
    g_Status.hp += amount;
    if (g_Status.hp > g_Status.hpMax) {
        g_Status.hp = g_Status.hpMax;
    }
}
static bool HasMaxHealth() { return g_Status.hpMax >= 320; }
static void IncreaseMaxHealth(s32 amount) {
    if (HasMaxHealth()) {
        return;
    }
    g_Status.hpMax += amount;
    g_Status.hp += amount;
}
static void IncreaseWeaponEnergy(s32 amount) {
    if (g_Status.hearts >= g_Status.heartsMax) {
        return;
    }
    g_Status.hearts += amount;
    if (g_Status.hearts > g_Status.heartsMax) {
        g_Status.hearts = g_Status.heartsMax;
    }
}

void EntityMmxPrizeDrop(Entity* self) { DestroyEntity(self); }

static AnimationFrame anim_power_capsule_small_fall[] = {
    POSE(4, 1, 1), POSE_LOOP(0)};
static AnimationFrame anim_power_capsule_small_open[] = {
    POSE(2, 1, 2), POSE(2, 2, 2), POSE(2, 3, 2),
    POSE(2, 4, 2), POSE(2, 3, 2), POSE_LOOP(1)};
static AnimationFrame* power_capsule_small_anims[] = {
    anim_power_capsule_small_fall,
    anim_power_capsule_small_open,
};
static s8 power_capsule_small_hitboxes[][4] = {
    {0, 0, 0, 0}, {0, -4, 4, 4}, {0, -4, 5, 4}};
void EntityPowerCapsuleSmall(Entity* self) {
    if (PrizeDropHelper(self, (s8*)power_capsule_small_hitboxes,
                        power_capsule_small_anims)) {
        IncreaseHealth(20);
        DestroyEntity(self);
    }
}

static AnimationFrame anim_power_capsule_big_fall[] = {
    POSE(4, 5, 1), POSE_LOOP(0)};
static AnimationFrame anim_power_capsule_big_open[] = {
    POSE(2, 5, 1), POSE(2, 6, 1), POSE(2, 7, 1),
    POSE(2, 8, 1), POSE(2, 7, 1), POSE_LOOP(1)};
static AnimationFrame* power_capsule_big_anims[] = {
    anim_power_capsule_big_fall,
    anim_power_capsule_big_open,
};
static s8 power_capsule_big_hitboxes[][4] = {{0, 0, 0, 0}, {0, -6, 8, 6}};
void EntityPowerCapsuleBig(Entity* self) {
    if (PrizeDropHelper(
            self, (s8*)power_capsule_big_hitboxes, power_capsule_big_anims)) {
        IncreaseHealth(80);
        DestroyEntity(self);
    }
}

static AnimationFrame anim_energy_capsule_small[] = {
    POSE(2, 9, 1), POSE(2, 10, 1), POSE(2, 11, 1), POSE(2, 10, 1),
    POSE_LOOP(0)};
static AnimationFrame* energy_capsule_small_anims[] = {
    anim_energy_capsule_small,
    anim_energy_capsule_small,
};
static s8 energy_capsule_small_hitboxes[][4] = {{0, 0, 0, 0}, {0, -4, 4, 4}};
void EntityEnergyCapsuleSmall(Entity* self) {
    if (PrizeDropHelper(self, (s8*)energy_capsule_small_hitboxes,
                        energy_capsule_small_anims)) {
        IncreaseWeaponEnergy(20);
        DestroyEntity(self);
    }
}

static AnimationFrame anim_energy_capsule_big[] = {
    POSE(2, 12, 1), POSE(2, 13, 1), POSE(2, 14, 1), POSE(2, 13, 1),
    POSE_LOOP(0)};
static AnimationFrame* energy_capsule_big_anims[] = {
    anim_energy_capsule_big,
    anim_energy_capsule_big,
};
static s8 energy_capsule_big_hitboxes[][4] = {{0, 0, 0, 0}, {0, -7, 7, 7}};
void EntityEnergyCapsuleBig(Entity* self) {
    if (PrizeDropHelper(
            self, (s8*)energy_capsule_big_hitboxes, energy_capsule_big_anims)) {
        IncreaseWeaponEnergy(80);
        DestroyEntity(self);
    }
}

static AnimationFrame anim_life_up[] = {
    POSE(4, 15, 1), POSE(4, 16, 1), POSE_LOOP(0)};
static AnimationFrame* life_up_anims[] = {
    anim_life_up,
    anim_life_up,
};
static s8 life_up_hitboxes[][4] = {{0, 0, 0, 0}, {0, -8, 8, 8}};
void EntityLifeUp(Entity* self) {
    if (PrizeDropHelper(self, (s8*)life_up_hitboxes, life_up_anims)) {
        IncreaseHealth(g_Status.hpMax - g_Status.hp);
        DestroyEntity(self);
    }
}

static AnimationFrame anim_heart_tank[] = {
    POSE(4, 17, 1), POSE(4, 18, 1), POSE(4, 19, 1), POSE(4, 20, 1),
    POSE_LOOP(0)};
static AnimationFrame* anim_heart_tank_anims[] = {
    anim_heart_tank,
    anim_heart_tank,
};
static s8 heart_tank_hitboxes[][4] = {{0, 0, 0, 0}, {0, -8, 8, 8}};
void EntityHeartTank(Entity* self) {
    // life-up should not show if the health is already capped
    if (HasMaxHealth()) {
        EntityPowerCapsuleBig(self);
        return;
    }

    // like SOTN upgrades, they are unique
    if (!self->step) {
        s32 castleFlag =
            g_CastleFlags[(self->params >> 3) + 0x100] >> (self->params & 7);
        if (castleFlag & 1) {
            DestroyEntity(self);
            return;
        }
    }

    // once spawned it should never disappear unless it is taken
    ((struct ExtPrizeDrop*)&self->ext)->disposeTimer = 99;
    if (PrizeDropHelper(
            self, (s8*)heart_tank_hitboxes, anim_heart_tank_anims)) {
        // once taken it will disappear for the rest of the playthrough
        g_CastleFlags[(self->params >> 3) + 0x100] |= 1 << (self->params & 7);
        IncreaseMaxHealth(10);
        DestroyEntity(self);
    }
}

void EntityEnergyTank(Entity* self) { DestroyEntity(self); }

static AnimationFrame anim_death_particle[] = {
    POSE(16, 21, 0), POSE(16, 22, 0), POSE(8, 23, 0), POSE(8, 23, 0),
    POSE(8, 24, 0),  POSE(8, 25, 0),  POSE_LOOP(3)};
static AnimationFrame* anim_death_particle_anims[] = {
    anim_death_particle,
};
static s8 anim_death_particle_hitboxes[][4] = {{0, 0, 0, 0}};
struct ExtDeathParticle {
    s32 timer;
    s32 startAngle;
    s16 originX, originY;
};
void EntityDeathParticle(Entity* self) {
    struct ExtDeathParticle* ext = (struct ExtDeathParticle*)&self->ext;
    s32 angle;
    switch (self->step) {
    case 0:
        self->animSet = ANIMSET_OVL(0x12);
        self->ext.player.anim = 0;
        self->flags = FLAG_UNK_2000 | FLAG_UNK_00200000 | FLAG_NOT_AN_ENEMY |
                      FLAG_POS_CAMERA_LOCKED;
        self->zPriority = PLAYER.zPriority;
        self->enemyId = 1;
        self->hitboxState = 1;
        ext->timer = 0;
        // a full angle is 0x1000; since there are 8 particles where each one
        // populates a separate params, we divide 0x1000 by 8
        ext->startAngle = self->params * 0x200;
        ext->originX = PLAYER.posX.i.hi;
        ext->originY = PLAYER.posY.i.hi;
        self->step++;
        break;
    case 1:
        ext->timer++;
        angle = -ext->timer * 16 + 0x800 + ext->startAngle;
        self->posX.i.hi = ext->originX + ((rsin(angle) * ext->timer) >> 11);
        self->posY.i.hi = ext->originY + ((rcos(angle) * ext->timer) >> 11);
        break;
    }
    g_api.PlayAnimation(
        (s8*)anim_death_particle_hitboxes, anim_death_particle_anims);
}

void EntityDeathScreenHandler(Entity* self) {
    Primitive* prim;
    int i;

    switch (self->step) {
    case 0:
        self->zPriority = 0x1F0;
        self->primIndex = g_api.AllocPrimitives(PRIM_G4, 2);
        self->flags = FLAG_HAS_PRIMS;
        prim = &g_PrimBuf[self->primIndex];
        while (prim) { // two prims, one for white and the other one for black
            prim->x0 = prim->x2 = 0;
            prim->y0 = prim->y1 = 0;
            prim->x1 = prim->x3 = DISP_STAGE_W;
            prim->y2 = prim->y3 = DISP_STAGE_H;
            prim->r0 = prim->r1 = prim->r2 = prim->r3 = prim->g0 = prim->g1 =
                prim->g2 = prim->g3 = prim->b0 = prim->b1 = prim->b2 =
                    prim->b3 = 0;
            prim->priority = self->zPriority++;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
        self->step++;
        self->step_s = 60;
        RicCreateEntFactoryFromEntity(g_CurrentEntity, B_P_DEATH_PARTICLES, 0);
        break;
    case 1: // wait before fading to white
        if (!--self->step_s) {
            self->step++;
            self->step_s = 0;
            g_PrimBuf[self->primIndex].drawMode =
                DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
        }
        break;
    case 2: // fade to white
        prim = &g_PrimBuf[self->primIndex];
        prim->r1 = prim->r2 = prim->r3 = prim->g0 = prim->g1 = prim->g2 =
            prim->g3 = prim->b0 = prim->b1 = prim->b2 = prim->b3 = prim->r0 =
                prim->b3 + 2;
        if (prim->r0 >= 254) {
            prim->drawMode = DRAW_DEFAULT;
            prim = prim->next;
            prim->drawMode =
                DRAW_UNK_40 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
            self->step++;
        }
        break;
    case 3: // fade from white to black
        prim = g_PrimBuf[self->primIndex].next;
        prim->r1 = prim->r2 = prim->r3 = prim->g0 = prim->g1 = prim->g2 =
            prim->g3 = prim->b0 = prim->b1 = prim->b2 = prim->b3 = prim->r0 =
                prim->b3 + 4;
        if (prim->r0 >= 252) {
            self->step++;
        }
        break;
    case 4: // return back to the title screen
        // does the same stuff from HandleGameOver, it might not be as accurate
        // but it can skip the original game over screen
        for (i = 0; i < LEN(g_Entities); i++) {
            DestroyEntity(&g_Entities[i]);
        }
        for (i = 0; i < LEN(g_PrimBuf); i++) {
            g_api.FreePrimitives(i);
        }
        for (i = 0; i < LEN(g_GfxLoad); i++) {
            g_GfxLoad[i].kind = GFX_BANK_NONE;
        }
        g_Tilemap.flags = 0;
        for (i = 0; i < LEN(g_BgLayers); i++) {
            g_BgLayers[i].flags = 0;
        }
        g_GameState = 1;
        g_GameStep = 0;
        g_backbufferX = 0;
        g_backbufferY = 0;
        break;
    }
}
