#include "pl.h"

#define B_A_LOT 3 // can spawn a maximum of 48 concurrent entities!
#define B_STAGE 7 // unique to the MMX mod, serves to replace stage entities

static FactoryBlueprint blueprints[] = {
    B_MAKE(0, 0, 0, false, false, 0, 0, 0, 0),
    B_MAKE(E_SMOKE_PUFF, 5, 1, true, true, 2, 0, 0, 0),
    B_MAKE(E_SMOKE_PUFF_WHEN_SLIDING, 1, 1, true, true, 2, 0, 0, 0),
    B_MAKE(E_SMOKE_PUFF, 1, 1, true, true, 2, 0, 0, 0),
    B_MAKE(E_W_LEMON, 1, 1, false, true, 1, B_WPN, 0, 0),
    {E_W_CUCUMBER, 0, 0, 0, 0, 0},
    {E_W_CHARJELLY, 0, 0, 0, 0, 0},
    {E_W_SHOTGUN_ICE, 0, 0, 0, 0, 0},
    {E_W_ELECTRIC_SPARK, 0, 0, 0, 0, 0},
    {E_W_ROLLING_SHIELD, 0, 0, 0, 0, 0},
    {E_W_HOMING_TORPEDO, 0, 0, 0, 0, 0},
    {E_W_BOOMERANG_CUTTER, 0, 0, 0, 0, 0},
    {E_W_CHAMELEON_STING, 0, 0, 0, 0, 0},
    {E_W_STORM_TORNADO, 0, 0, 0, 0, 0},
    {E_W_FIRE_WAVE, 0, 0, 0, 0, 0},
    {E_W_HADOUKEN, 0, 0, 0, 0},
    B_MAKE(E_MMX_PRIZE_DROP, 1, 1, false, true, 1, B_STAGE, 0, 0),
    B_MAKE(E_POWER_CAPSULE_SMALL, 1, 1, false, true, 1, B_STAGE, 0, 0),
    B_MAKE(E_POWER_CAPSULE_BIG, 1, 1, false, true, 1, B_STAGE, 0, 0),
    B_MAKE(E_ENERGY_CAPSULE_SMALL, 1, 1, false, true, 1, B_STAGE, 0, 0),
    B_MAKE(E_ENERGY_CAPSULE_BIG, 1, 1, false, true, 1, B_STAGE, 0, 0),
    B_MAKE(E_LIFE_UP, 1, 1, false, true, 1, B_STAGE, 0, 0),
    B_MAKE(E_HEART_TANK, 1, 1, false, true, 1, B_STAGE, 0, 0),
    B_MAKE(E_ENERGY_TANK, 1, 1, false, true, 1, B_STAGE, 0, 0),
    B_MAKE(E_DEATH_PARTICLE, 32, 8, true, true, 64, B_A_LOT, 0, 0),
    B_MAKE(E_DEATH_SCREEN_HANDLER, 1, 1, false, true, 1, B_A_LOT, 0, 0),
};
STATIC_ASSERT(
    LEN(blueprints) == (NUM_BLUEPRINTS - B_DUMMY), "bp array wrong size");

void RicEntityDummy(Entity* self) {}

void RicEntityDummy(Entity* self);
void RicEntityFactory(Entity* self);
void RicEntitySmokePuff(Entity* self);
void RicEntitySubwpnCross(Entity* self);
void func_80169C10(Entity* self);
void RicEntityHitByCutBlood(Entity* self);
void RicEntitySubwpnCrossTrail(Entity* self);
void RicEntitySubwpnHolyWater(Entity* self);
void RicEntitySubwpnHolyWaterFlame(Entity* self);
void func_80161C2C(Entity* self);
void RicEntityWhip(Entity* self);
void RicEntityCrashHydroStorm(Entity* self);
void RicEntityGiantSpinningCross(Entity* self);
void RicEntitySubwpnCrashCross(Entity* self);
void RicEntityNotImplemented1(Entity* self);
void RicEntityNotImplemented2(Entity* self);
void RicEntityArmBrandishWhip(Entity* self);
void func_80167964(Entity* self);
void RicEntityDummy(Entity* self);
void func_80161EF8(Entity* self);
void RicEntityNotImplemented3(Entity* self);
void RicEntityRevivalColumn(Entity* self);
void RicEntityApplyMariaPowerAnim(Entity* self);
void RicEntitySlideKick(Entity* self);
void RicEntityBladeDash(Entity* self);
void func_801623E0(Entity* self);
void func_80162604(Entity* self);
void RicEntityMaria(Entity* self);
void RicEntityMariaPowers(Entity* self);
void func_80160F0C(Entity* self);
void RicEntityNotImplemented4(Entity* self);
void RicEntityPlayerBlinkWhite(Entity* self);
void RicEntitySubwpnCrashCrossParticles(Entity* self);
void func_801641A0(Entity* self);
void RicEntityShrinkingPowerUpRing(Entity* self);
void func_80167A70(Entity* self);
void RicEntitySubwpnAxe(Entity* self);
void RicEntityCrashAxe(Entity* self);
void RicEntitySubwpnDagger(Entity* self);
void func_80160D2C(Entity* self);
void RicEntityHitByIce(Entity* self);
void RicEntityHitByLightning(Entity* self);
void RicEntitySubwpnReboundStone(Entity* self);
void RicEntitySubwpnThrownVibhuti(Entity* self);
void RicEntitySubwpnAgunea(Entity* self);
void RicEntityAguneaHitEnemy(Entity* self);
void RicEntityCrashVibhuti(Entity* self);
void RicEntityVibhutiCrashCloud(Entity* self);
void RicEntityCrashReboundStone(Entity* self);
void func_8016D9C4(Entity* self);
void RicEntityCrashReboundStoneExplosion(Entity* self);
void RicEntityCrashBible(Entity* self);
void RicEntityCrashBibleBeam(Entity* self);
void RicEntitySubwpnBible(Entity* self);
void RicEntitySubpwnBibleTrail(Entity* self);
void RicEntitySubwpnStopwatch(Entity* self);
void RicEntitySubwpnStopwatchCircle(Entity* self);
void func_801705EC(Entity* self);
void func_8016F198(Entity* self);
void RicEntityAguneaCircle(Entity* self);
void RicEntityAguneaLightning(Entity* self);
void RicEntityCrashReboundStoneParticles(Entity* self);
void RicEntityHitByDark(Entity* self);
void RicEntityHitByHoly(Entity* self);
void RicEntityCrashStopwatchDoneSparkle(Entity* self);
void RicEntityStopwatchCrashLightning(Entity* self);
void RicEntityTeleport(Entity* self);
void RicEntityDummy(Entity* self);

void RicEntitySmokePuffWhenSliding(Entity* self);
void EntityLemon(Entity* self);
void EntityCucumber(Entity* self);
void EntityCharjelly(Entity* self);
void EntityShotgunIce(Entity* self);
void EntityElectricSpark(Entity* self);
void EntityRollingShield(Entity* self);
void EntityHomingTorpedo(Entity* self);
void EntityBoomerangCutter(Entity* self);
void EntityChameleonSting(Entity* self);
void EntityStormTornado(Entity* self);
void EntityFireWave(Entity* self);
void EntityHadouken(Entity* self);
void EntityMmxPrizeDrop(Entity* self);
void EntityPowerCapsuleSmall(Entity* self);
void EntityPowerCapsuleBig(Entity* self);
void EntityEnergyCapsuleSmall(Entity* self);
void EntityEnergyCapsuleBig(Entity* self);
void EntityLifeUp(Entity* self);
void EntityHeartTank(Entity* self);
void EntityEnergyTank(Entity* self);
void EntityDeathParticle(Entity* self);
void EntityDeathScreenHandler(Entity* self);

static PfnEntityUpdate entity_functions[] = {
    RicEntityDummy,
    RicEntityFactory,
    RicEntitySmokePuff,
    RicEntitySubwpnCross,
    func_80169C10,
    RicEntityHitByCutBlood,
    RicEntitySubwpnCrossTrail,
    RicEntitySubwpnHolyWater,
    RicEntitySubwpnHolyWaterFlame,
    func_80161C2C,
    RicEntityWhip,
    RicEntityCrashHydroStorm,
    RicEntityDummy,
    RicEntitySubwpnCrashCross,
    RicEntityNotImplemented1,
    RicEntityNotImplemented2,
    RicEntityArmBrandishWhip,
    func_80167964,
    RicEntityDummy,
    func_80161EF8,
    RicEntityNotImplemented3,
    RicEntityRevivalColumn,
    RicEntityApplyMariaPowerAnim,
    RicEntitySlideKick,
    RicEntityBladeDash,
    func_801623E0,
    func_80162604,
    RicEntityMaria,
    RicEntityMariaPowers,
    func_80160F0C,
    RicEntityNotImplemented4,
    RicEntityPlayerBlinkWhite,
    RicEntitySubwpnCrashCrossParticles,
    func_801641A0,
    RicEntityShrinkingPowerUpRing,
    func_80167A70,
    RicEntitySubwpnAxe,
    RicEntityCrashAxe,
    RicEntitySubwpnDagger,
    func_80160D2C,
    RicEntityHitByIce,
    RicEntityHitByLightning,
    RicEntitySubwpnReboundStone,
    RicEntitySubwpnThrownVibhuti,
    RicEntitySubwpnAgunea,
    RicEntityAguneaHitEnemy,
    RicEntityCrashVibhuti,
    RicEntityVibhutiCrashCloud,
    RicEntityCrashReboundStone,
    func_8016D9C4,
    RicEntityCrashReboundStoneExplosion,
    RicEntityCrashBible,
    RicEntityCrashBibleBeam,
    RicEntitySubwpnBible,
    RicEntitySubpwnBibleTrail,
    RicEntitySubwpnStopwatch,
    RicEntitySubwpnStopwatchCircle,
    func_801705EC,
    func_8016F198,
    RicEntityAguneaCircle,
    RicEntityAguneaLightning,
    RicEntityCrashReboundStoneParticles,
    RicEntityHitByDark,
    RicEntityHitByHoly,
    RicEntityCrashStopwatchDoneSparkle,
    RicEntityStopwatchCrashLightning,
    RicEntityTeleport,
    RicEntityDummy,

    RicEntitySmokePuffWhenSliding,
    EntityLemon,
    EntityCucumber,
    EntityCharjelly,
    EntityShotgunIce,
    EntityElectricSpark,
    EntityRollingShield,
    EntityHomingTorpedo,
    EntityBoomerangCutter,
    EntityChameleonSting,
    EntityStormTornado,
    EntityFireWave,
    EntityHadouken,
    EntityMmxPrizeDrop,
    EntityPowerCapsuleSmall,
    EntityPowerCapsuleBig,
    EntityEnergyCapsuleSmall,
    EntityEnergyCapsuleBig,
    EntityLifeUp,
    EntityHeartTank,
    EntityEnergyTank,
    EntityDeathParticle,
    EntityDeathScreenHandler};
STATIC_ASSERT(LEN(entity_functions) == NUM_ENTITIES, "entity array wrong size");

void InitSettings() {
    // mainly used to expand the existing blueprint struct instead of totally
    // replace the existing one.
    memcpy(g_RicFactoryBlueprints + B_DUMMY, blueprints, sizeof(blueprints));
}

static Entity* RicGetFreeEntity(s16 start, s16 end) {
    Entity* entity = &g_Entities[start];
    s16 i;

    for (i = start; i < end; i++, entity++) {
        if (entity->entityId == E_NONE) {
            return entity;
        }
    }
    return NULL;
}

static Entity* RicGetFreeEntityReverse(s16 start, s16 end) {
    Entity* entity = &g_Entities[end - 1];
    s16 i;
    for (i = end - 1; i >= start; i--, entity--) {
        if (entity->entityId == E_NONE) {
            return entity;
        }
    }
    return NULL;
}

static s32 D_80174F80[11];
void func_8015F9F0(Entity* entity) {
    s32 i;
    s32 enemyId;

    if (entity < &g_Entities[32]) {
        entity->enemyId = 1;
        return;
    }

    for (i = 0;; i++) {
        for (enemyId = 2; enemyId < LEN(D_80174F80); ++enemyId) {
            if (D_80174F80[enemyId] == i) {
                ++D_80174F80[enemyId];
                entity->enemyId = enemyId;
                return;
            }
        }
    }
}

static u8 entity_ranges[][2] = {
    {0x30, 0x3F}, {0x20, 0x2F}, {0x10, 0x1E}, {0x10, 0x3F},
    {0x1F, 0x1F}, {0x30, 0x30}, {0x10, 0x2F}, {0x00, 0x00}};
void RicEntityFactory(Entity* self) {
    Entity* newEntity;
    s16 n;
    s16 i;
    u8 endIndex;
    s16 startIndex;
    u8* data_idx;

    if (self->step == 0) {
        data_idx = &g_RicFactoryBlueprints[self->params];
        self->ext.factory.childId = *data_idx++;
        self->ext.factory.unk94 = *data_idx++;          // index 1
        self->ext.factory.unk96 = *data_idx & 0x3F;     // index 2, lower 6 bits
        self->ext.factory.unk9E = *data_idx >> 7;       // index 2, top bit
        self->ext.factory.unkA2 = *data_idx++ >> 6 & 1; // index 2, 2nd-top bit
        self->ext.factory.unk98 = *data_idx++;          // index 3
        self->ext.factory.unk9C = *data_idx & 0x7;      // index 4, lower 4 bits
        self->ext.factory.unkA4 = *data_idx++ >> 3;     // index 4, upper 4 bits
        self->ext.factory.unk9A = *data_idx;            // index 5
        self->flags |= FLAG_KEEP_ALIVE_OFFCAMERA;

        self->step++;
        switch (self->ext.factory.unkA4) {
        case 0:
            self->flags |= FLAG_POS_CAMERA_LOCKED;
            break;
        case 4:
            self->flags |= FLAG_UNK_20000;
        case 2:
        case 9:
            self->flags |= FLAG_POS_PLAYER_LOCKED;
        case 3:
        case 7:
            self->posX.val = PLAYER.posX.val;
            self->posY.val = PLAYER.posY.val;
            break;
        case 8:
            self->flags |= FLAG_POS_PLAYER_LOCKED;
            self->posX.val = self->ext.factory.parent->posX.val;
            self->posY.val = self->ext.factory.parent->posY.val;
            break;
        }
    } else {
        switch (self->ext.factory.unkA4) {
        case 0:
            break;
        case 9:
            if (g_Player.unk4E != 0) {
                DestroyEntity(self);
                return;
            }
        case 2:
            self->posX.val = PLAYER.posX.val;
            self->posY.val = PLAYER.posY.val;
            break;
        case 4:
            self->posX.val = PLAYER.posX.val;
            self->posY.val = PLAYER.posY.val;
            if (PLAYER.step != 25) {
                self->entityId = 0;
                return;
            }
            break;
        case 3:
            self->posX.val = PLAYER.posX.val;
            self->posY.val = PLAYER.posY.val;
            if (PLAYER.step == PL_S_HIT) {
                self->entityId = 0;
                return;
            }
            break;
        case 7:
            self->posX.val = PLAYER.posX.val;
            self->posY.val = PLAYER.posY.val;
            if (PLAYER.step != PL_S_HIT) {
                self->entityId = 0;
                return;
            }
            break;
        case 8:
            self->posX.val = self->ext.factory.parent->posX.val;
            self->posY.val = self->ext.factory.parent->posY.val;
            break;
        }
    }
    if (self->ext.factory.unk9A != 0) {
        self->ext.factory.unk9A--;
        if (self->ext.factory.unk9A != 0) {
            return;
        }
        self->ext.factory.unk9A = self->ext.factory.unk98;
    }
    // Save this value so we don't have to re-fetch on every for-loop cycle
    n = self->ext.factory.unk96;
    for (i = 0; i < n; i++) {
        // !FAKE, this should probably be &entity_ranges[unk9C] or similar,
        // instead of doing &entity_ranges followed by +=
        data_idx = entity_ranges;
        data_idx += self->ext.factory.unk9C * 2;

        startIndex = *data_idx;
        endIndex = *(data_idx + 1);

        if (self->ext.factory.unk9C == 0) {
            newEntity = RicGetFreeEntityReverse(startIndex, endIndex + 1);
        } else if (self->ext.factory.unk9C == 4) {
            newEntity = &g_Entities[31];
        } else if (self->ext.factory.unk9C == 5) {
            newEntity = &g_Entities[48];
        } else if (self->ext.factory.unk9C == B_STAGE) {
            newEntity = RicGetFreeEntity(STAGE_ENTITY_START, LEN(g_Entities));
        } else {
            newEntity = RicGetFreeEntity(startIndex, endIndex + 1);
        }

        if (newEntity == NULL) {
            if (self->ext.factory.unk9E == 1) {
                self->entityId = 0;
                return;
            }
            break;
        }
        DestroyEntity(newEntity);
        // unkA8 never gets set so is always zero
        newEntity->entityId =
            self->ext.factory.childId + self->ext.factory.unkA8;
        newEntity->params = self->ext.factory.unkA0;
        // The child  (newEntity) is not an ent factory, but because the
        // factory creates many entities, we can't pick a particular extension.
        // But we're not allowed to use generic, so i'll just reuse entFactory.
        newEntity->ext.factory.parent = self->ext.factory.parent;
        newEntity->posX.val = self->posX.val;
        newEntity->posY.val = self->posY.val;
        newEntity->facingLeft = self->facingLeft;
        newEntity->zPriority = self->zPriority;
        if (self->flags & FLAG_UNK_10000) {
            newEntity->flags |= FLAG_UNK_10000;
        }
        if (self->ext.factory.unkA2 != 0) {
            newEntity->params += self->ext.factory.unkA6;
        } else {
            newEntity->params += i;
        }

        // this part is exclusive to MMX:
        // the player overlay doesn't activate a new factory entity that is in
        // the stage range, and we cannot allow the stage overlay to activate it
        // on the player behalf because the two overlays have a different table
        // for their own entityId list.
        // we need to activate the B_STAGE entities immediately here
        if (self->ext.factory.unk9C == B_STAGE) {
            newEntity->pfnUpdate = entity_functions[newEntity->entityId];
        }

        if (++self->ext.factory.unkA6 == self->ext.factory.unk94) {
            self->entityId = 0;
            return;
        }
    }
    self->ext.factory.unk9A = self->ext.factory.unk98;
}

static u8 D_80174FAC;
static u8 D_80174FB0;
static u8 D_80174FB4;
static u8 D_80174FB8;
void func_8015FA5C(s32 arg0) {
    D_80174FAC = D_80154674[arg0][0];
    D_80174FB0 = D_80154674[arg0][1];
    D_80174FB4 = D_80154674[arg0][2];
    D_80174FB8 = D_80154674[arg0][3];
}

// Corresponding DRA function is func_8011A4D0
s32 func_8015FB84(SubweaponDef* subwpn, s32 isItemCrash, s32 useHearts);
void RicUpdatePlayerEntities(void) {
    SubweaponDef subwpn;
    Entity* entity;
    s32 i;
    s32 i2;
    s32 i3;
    s32 isPrologueTimeStopped;
    s32 enemy;
    s32 enemy2;

    isPrologueTimeStopped = g_unkGraphicsStruct.unk20;
    entity = g_CurrentEntity = &g_Entities[4];
    for (i = 4; i < 0x40; i++, g_CurrentEntity++, entity++) {
        if (entity->entityId) {
            if (entity->step == 0) {
                entity->pfnUpdate = entity_functions[entity->entityId];
            }
            if (!isPrologueTimeStopped || (entity->flags & FLAG_UNK_10000)) {
                entity->pfnUpdate(entity);
                entity = g_CurrentEntity;
                if (entity->entityId) {
                    if (!(entity->flags & FLAG_KEEP_ALIVE_OFFCAMERA) &&
                        (entity->posX.i.hi < -32 || entity->posX.i.hi > 288 ||
                         entity->posY.i.hi < -16 || entity->posY.i.hi > 256)) {
                        DestroyEntity(entity);
                    } else if (entity->flags & FLAG_UNK_100000) {
                        g_api.UpdateAnim(0, D_80154674);
                    }
                }
            }
        }
    }

    if (D_80174FAC != 0) {
        if (--D_80174FAC & 1) {
            g_api.g_pfn_800EA5AC(1, D_80174FB0, D_80174FB4, D_80174FB8);
        }
    }

    D_80174F80[1] = D_80174F80[2] = 0;
    enemy = g_Entities[16].enemyId;
    if (enemy == 1) {
        D_80174F80[1] = 1;
    } else if (enemy == 2) {
        D_80174F80[2] = 1;
    }

    for (i2 = 3; i2 < 11; i2++) {
        D_80174F80[i2] = 0;
    }

    entity = &g_Entities[17];
    for (i3 = 17; i3 < 48; entity++, i3++) {
        enemy2 = entity->enemyId;
        if (enemy2 >= 3) {
            D_80174F80[entity->enemyId]++;
        }
    }
    // This IF will fire if we have enough hearts to use a subweapon crash.
    // No idea what it's doing here.
    if (func_8015FB84(&subwpn, true, false) >= 0) {
        g_Player.status |= PLAYER_STATUS_UNK200000;
    }
    if (g_Player.status & (PLAYER_STATUS_UNK40000 | PLAYER_STATUS_UNK80000)) {
        FntPrint("dead player\n");
        entity = &g_Entities[17]; // Weird code here. Set entity to #17 but...
        entity -= 13; // then change to #4 before the for-loop starting with 4?
        for (i = 4; i < 64; i++, entity++) {
            entity->hitboxState = 0;
        }
    }
}

// Similar to the version in DRA but with some logic removed
Entity* RicCreateEntFactoryFromEntity(
    Entity* source, u32 factoryParams, s32 arg2) {
    Entity* entity = RicGetFreeEntity(8, 0x10);

    // circumnavigate a bug that can fill-up all the factory entities
    if ((factoryParams & 0xFFF) < B_DUMMY) {
        return NULL;
    }

    if (entity != NULL) {
        DestroyEntity(entity);
        entity->entityId = E_FACTORY;
        entity->ext.factory.parent = source;
        entity->posX.val = source->posX.val;
        entity->posY.val = source->posY.val;
        entity->facingLeft = source->facingLeft;
        entity->zPriority = source->zPriority;
        entity->params = factoryParams & 0xFFF;
        entity->ext.factory.unkA0 = (factoryParams >> 8) & 0xFF00;

        if (source->flags & FLAG_UNK_10000) {
            entity->flags |= FLAG_UNK_10000;
        }
    } else {
        return NULL;
    }
    return entity;
}
