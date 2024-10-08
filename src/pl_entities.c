#include "pl.h"

// https://megaman.fandom.com/wiki/Mega_Man_X1_Damage_Data_Chart
static SubweaponDef weapons[] = {
    // dummy
    {0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0},
    // lemon
    {1, 0, ELEMENT_HIT, 0, 4, 4, 0, 20, 2, 0x81, 0, 0, 0},
    // cucumber
    {2, 0, ELEMENT_HIT, 0, 4, 4, 0, 21, 2, 0x81, 0, 0, 0},
    // charjelly
    {3, 0, ELEMENT_HIT, 0, 4, 4, 0, 22, 2, 0x81, 0, 0, 0},
    // shotgun ice
    {1, 0, ELEMENT_ICE, 0, 4, 4, 0, 23, 2, 0x81, 0, 0, 0},
    // electric spark
    {1, 0, ELEMENT_THUNDER, 0, 4, 4, 0, 24, 2, 0x81, 0, 0, 0},
    // rolling shield
    {1, 0, ELEMENT_STONE, 0, 4, 4, 0, 25, 2, 0x81, 0, 0, 0},
    // homing torpedo
    {1, 0, ELEMENT_DARK, 0, 4, 4, 0, 26, 2, 0x81, 0, 0, 0},
    // boomerang cutter
    {1, 0, ELEMENT_CUT, 0, 4, 4, 0, 27, 2, 0x81, 0, 0, 0},
    // chameleon sting
    {0, 0, ELEMENT_POISON, 0, 4, 4, 0, 28, 2, 0x81, 0, 0, 0},
    // storm tornado
    {1, 0, ELEMENT_WATER, 0, 4, 4, 0, 29, 2, 0x81, 0, 0, 0},
    // fire wave
    {1, 0, ELEMENT_FIRE, 0, 4, 4, 0, 30, 2, 0x81, 0, 0, 0},
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
    e->attack <<= 2;

    // exponentially increase damage to not make the game too hard
    if (g_Player.timers[PL_T_INVINCIBLE]) {
        e->attack <<= 1;
    }
    if (g_StageId & STAGE_INVERTEDCASTLE_FLAG) {
        e->attack <<= 1;
    }
}

static AnimationFrame anim_lemon_shoot[] = {{64, FRAME(1, 2)}, A_END};
static AnimationFrame anim_lemon_impact[] = {
    {2, FRAME(2, 0)},
    {2, FRAME(3, 0)},
    {2, FRAME(4, 0)},
    {64, FRAME(0, 0)},
    A_END};
static AnimationFrame* lemon_anims[] = {
    anim_lemon_shoot,
    anim_lemon_impact,
};
static u8 lemon_hitboxes[][4] = {
    {0, 0, 0, 0},
    {0, 0, 4, 3},
};
struct ExtLemon {
    s32 disposeTimer;
};

void EntityLemon(Entity* self) {
    const int Width = 8;
    struct ExtLemon* ext = (struct ExtLemon*)&self->ext;
    Collider col;
    s32 sensorX;

    switch (self->step) {
    case 0:
        self->animSet = ANIMSET_OVL(0x11);
        self->ext.player.anim = 0;
        self->drawFlags = 0;
        self->flags |= FLAG_POS_CAMERA_LOCKED;
        self->posX.i.hi += self->facingLeft ? -16 : 16;
        self->posY.i.hi += 5;
        self->zPriority++;
        SetWeaponParams(self, W_LEMON);
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
        self->ext.player.anim = 1;
        g_CurrentEntity->animFrameDuration = 0;
        self->hitboxState = 0;
        g_api.PlaySfx(SFX_UI_TINK);
        ext->disposeTimer = 32;
        self->step++;
        break;
    case 2:
        if (!--ext->disposeTimer) {
            DestroyEntity(self);
            return;
        }
        break;
    }
    g_api.PlayAnimation(lemon_hitboxes, lemon_anims);
}

void EntityCucumber(Entity* self) { DestroyEntity(self); }
void EntityCharjelly(Entity* self) { DestroyEntity(self); }
void EntityShotgunIce(Entity* self) { DestroyEntity(self); }
void EntityElectricSpark(Entity* self) { DestroyEntity(self); }
void EntityRollingShield(Entity* self) { DestroyEntity(self); }
void EntityHomingTorpedo(Entity* self) { DestroyEntity(self); }
void EntityBoomerangCutter(Entity* self) { DestroyEntity(self); }
void EntityChameleonSting(Entity* self) { DestroyEntity(self); }
void EntityStormTornado(Entity* self) { DestroyEntity(self); }
void EntityFireWave(Entity* self) { DestroyEntity(self); }
void EntityHadouken(Entity* self) { DestroyEntity(self); }

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
    Entity* self, u8* hitboxes, AnimationFrame** anims) {
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
    {4, FRAME(1, 2)}, A_LOOP_AT(0)};
static AnimationFrame anim_power_capsule_small_open[] = {
    {2, FRAME(1, 4)}, {2, FRAME(2, 4)}, {2, FRAME(3, 4)},
    {2, FRAME(4, 4)}, {2, FRAME(3, 4)}, A_LOOP_AT(1)};
static AnimationFrame* power_capsule_small_anims[] = {
    anim_power_capsule_small_fall,
    anim_power_capsule_small_open,
};
static u8 power_capsule_small_hitboxes[][4] = {
    {0, 0, 0, 0}, {0, -4, 4, 4}, {0, -4, 5, 4}};
void EntityPowerCapsuleSmall(Entity* self) {
    if (PrizeDropHelper(
            self, power_capsule_small_hitboxes, power_capsule_small_anims)) {
        IncreaseHealth(20);
        DestroyEntity(self);
    }
}

static AnimationFrame anim_power_capsule_big_fall[] = {
    {4, FRAME(5, 2)}, A_LOOP_AT(0)};
static AnimationFrame anim_power_capsule_big_open[] = {
    {2, FRAME(5, 2)}, {2, FRAME(6, 2)}, {2, FRAME(7, 2)},
    {2, FRAME(8, 2)}, {2, FRAME(7, 2)}, A_LOOP_AT(1)};
static AnimationFrame* power_capsule_big_anims[] = {
    anim_power_capsule_big_fall,
    anim_power_capsule_big_open,
};
static u8 power_capsule_big_hitboxes[][4] = {{0, 0, 0, 0}, {0, -6, 8, 6}};
void EntityPowerCapsuleBig(Entity* self) {
    if (PrizeDropHelper(
            self, power_capsule_big_hitboxes, power_capsule_big_anims)) {
        IncreaseHealth(80);
        DestroyEntity(self);
    }
}

static AnimationFrame anim_energy_capsule_small[] = {
    {2, FRAME(9, 2)},
    {2, FRAME(10, 2)},
    {2, FRAME(11, 2)},
    {2, FRAME(10, 2)},
    A_LOOP_AT(0)};
static AnimationFrame* energy_capsule_small_anims[] = {
    anim_energy_capsule_small,
    anim_energy_capsule_small,
};
static u8 energy_capsule_small_hitboxes[][4] = {{0, 0, 0, 0}, {0, -4, 4, 4}};
void EntityEnergyCapsuleSmall(Entity* self) {
    if (PrizeDropHelper(
            self, energy_capsule_small_hitboxes, energy_capsule_small_anims)) {
        IncreaseWeaponEnergy(20);
        DestroyEntity(self);
    }
}

static AnimationFrame anim_energy_capsule_big[] = {
    {2, FRAME(12, 2)},
    {2, FRAME(13, 2)},
    {2, FRAME(14, 2)},
    {2, FRAME(13, 2)},
    A_LOOP_AT(0)};
static AnimationFrame* energy_capsule_big_anims[] = {
    anim_energy_capsule_big,
    anim_energy_capsule_big,
};
static u8 energy_capsule_big_hitboxes[][4] = {{0, 0, 0, 0}, {0, -7, 7, 7}};
void EntityEnergyCapsuleBig(Entity* self) {
    if (PrizeDropHelper(
            self, energy_capsule_big_hitboxes, energy_capsule_big_anims)) {
        IncreaseWeaponEnergy(80);
        DestroyEntity(self);
    }
}

static AnimationFrame anim_life_up[] = {
    {4, FRAME(15, 2)}, {4, FRAME(16, 2)}, A_LOOP_AT(0)};
static AnimationFrame* life_up_anims[] = {
    anim_life_up,
    anim_life_up,
};
static u8 life_up_hitboxes[][4] = {{0, 0, 0, 0}, {0, -8, 8, 8}};
void EntityLifeUp(Entity* self) {
    if (PrizeDropHelper(self, life_up_hitboxes, life_up_anims)) {
        IncreaseHealth(g_Status.hpMax - g_Status.hp);
        DestroyEntity(self);
    }
}

static AnimationFrame anim_heart_tank[] = {
    {4, FRAME(17, 2)},
    {4, FRAME(18, 2)},
    {4, FRAME(19, 2)},
    {4, FRAME(20, 2)},
    A_LOOP_AT(0)};
static AnimationFrame* anim_heart_tank_anims[] = {
    anim_heart_tank,
    anim_heart_tank,
};
static u8 heart_tank_hitboxes[][4] = {{0, 0, 0, 0}, {0, -8, 8, 8}};
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
    if (PrizeDropHelper(self, heart_tank_hitboxes, anim_heart_tank_anims)) {
        // once taken it will disappear for the rest of the playthrough
        g_CastleFlags[(self->params >> 3) + 0x100] |= 1 << (self->params & 7);
        IncreaseMaxHealth(10);
        DestroyEntity(self);
    }
}

void EntityEnergyTank(Entity* self) { DestroyEntity(self); }

static AnimationFrame anim_death_particle[] = {
    {16, FRAME(21, 0)},
    {16, FRAME(22, 0)},
    {8, FRAME(23, 0)},
    {8, FRAME(23, 0)},
    {8, FRAME(24, 0)},
    {8, FRAME(25, 0)},
    A_LOOP_AT(3)};
static AnimationFrame* anim_death_particle_anims[] = {
    anim_death_particle,
};
static u8 anim_death_particle_hitboxes[][4] = {{0, 0, 0, 0}};
struct ExtDeathParticle {
    s32 timer;
    s32 startAngle;
    s16 originX, originY;
};
void EntityDeathParticle(Entity *self) {
    struct ExtDeathParticle* ext = &self->ext;
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
    g_api.PlayAnimation(anim_death_particle_hitboxes, anim_death_particle_anims);
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
            prim->r0 = prim->r1 = prim->r2 = prim->r3 = prim->g0 =
                prim->g1 = prim->g2 = prim->g3 = prim->b0 = prim->b1 =
                    prim->b2 = prim->b3 = 0;
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
