#include "pl.h"

#ifdef VERSION_PC
#ifndef DEBUG
#define DEBUG
#endif
#endif

SubweaponDef D_80154688[] = {
    {0, 30000, 0, 0, 0, 0, 0, 0, 2, 1, 15, 0, 0},
    {20, 1, 4160, 3, 4, 4, 0, 43, 2, 129, 21, 0, 32},
    {50, 1, 4160, 3, 32, 4, 0, 19, 2, 129, 20, 0, 32},
    {5, 1, 32, 1, 32, 4, 0, 6, 2, 1, 16, 0, 0},
    {40, 1, 4128, 4, 32, 4, 0, 2, 2, 129, 12, 0, 0},
    {5, 5, 4096, 2, 24, 4, 0, 61, 2, 1, 26, 0, 0},
    {0, 10, 0, 1, 50, 0, 0, 63, 0, 0, 27, 0, 0},
    {40, 1, 4128, 3, 32, 4, 0, 50, 2, 1, 25, 0, 0},
    {20, 1, 4096, 3, 10, 4, 0, 49, 2, 1, 24, 0, 0},
    {20, 10, 20480, 1, 10, 10, 0, 51, 2, 129, 28, 0, 0},
    {15, 30000, 4096, 1, 10, 4, 0, 0, 2, 1, 10, 0, 0},
    {8, 1, 36864, 2, 16, 4, 0, 9, 2, 1, 0, 0, 0},
    {40, 20, 4128, 1, 32, 4, 0, 13, 2, 1, 0, 0, 0},
    {60, 0, 4128, 1, 24, 4, 0, 0, 2, 1, 0, 0, 0},
    {32, 5, 4128, 1, 32, 4, 0, 0, 2, 1, 0, 0, 0},
    {64, 15, 36896, 1, 32, 4, 0, 0, 2, 1, 0, 0, 32},
    {80, 15, 4128, 1, 32, 24, 0, 12, 130, 1, 0, 0, 0},
    {80, 0, 32, 1, 32, 4, 0, 0, 2, 1, 0, 0, 32},
    {20, 0, 32, 1, 32, 4, 0, 0, 2, 1, 0, 0, 0},
    {5, 0, 4128, 1, 32, 4, 0, 0, 2, 1, 0, 0, 0},
    {40, 10, 4160, 1, 32, 4, 0, 41, 2, 129, 0, 0, 32},
    {20, 10, 4160, 1, 4, 4, 0, 44, 2, 129, 0, 0, 32},
    {80, 0, 32, 1, 32, 4, 0, 0, 2, 1, 0, 0, 32},
    {60, 0, 32, 1, 32, 4, 0, 0, 2, 1, 0, 0, 128},
    {20, 10, 4128, 1, 32, 4, 0, 54, 2, 1, 0, 0, 0},
    {20, 10, 32, 1, 32, 4, 0, 56, 2, 1, 0, 0, 0},
    {80, 15, 4128, 1, 32, 4, 0, 59, 2, 1, 0, 0, 0},
    {0, 20, 32, 1, 32, 32, 0, 65, 2, 1, 0, 0, 0},
    {80, 20, 16416, 1, 32, 4, 0, 67, 2, 1, 0, 0, 0},
    {10, 30000, 32, 1, 32, 4, 0, 0, 2, 1, 0, 0, 0},
    {15, 30000, 16416, 1, 32, 64, 0, 0, 2, 1, 0, 0, 0},
};
FactoryBlueprint g_RicFactoryBlueprints[NUM_BLUEPRINTS];

MmxJumpState g_JumpState = PL_JUMP_NONE;
u32 g_PadReleased = 0;
u32 g_ChargeTimer = 0;
u32 g_WallSlideTimer = 0;
u32 g_DashTimer = 0;
u32 g_DashAirUsed = 0;

static void InitSpritesheet(u_long* ptr, size_t num) {
#ifdef VERSION_PC
    memcpy(g_PlOvlSpritesheet, ptr, sizeof(u_long*) * num);
#endif
}

static void InitAnimations() {}
static void InitCollisions() {}
void MmxInitGraphics();

static void DebugDrawRect(int x, int y, int w, int h, int color) {
    OT_TYPE* ot = g_CurrentBuffer->ot;
    TILE* tile = &g_CurrentBuffer->tiles[g_GpuUsage.tile];
    DR_MODE* drMode = &g_CurrentBuffer->drawModes[g_GpuUsage.drawModes];
    int otIdx = 0x1F0;
    if (g_GpuUsage.tile >= MAX_TILE_COUNT) {
        return;
    }
    if (g_GpuUsage.drawModes >= MAX_DRAW_MODES) {
        return;
    }

    tile->r0 = (color >> 16) & 0xFF;
    tile->g0 = (color >> 8) & 0xFF;
    tile->b0 = (color >> 0) & 0xFF;
    tile->x0 = x;
    tile->y0 = y;
    tile->w = w;
    tile->h = h;
    SetSemiTrans(tile, 1);
    AddPrim(&ot[otIdx], tile);
    g_GpuUsage.tile++;

    RECT screen = {x, y, w, h};
    SetDrawMode(drMode, 0, 0, 0, &screen);
    AddPrim(&ot[otIdx], drMode);
    g_GpuUsage.drawModes++;
}

extern s32 g_DebugHitboxViewMode;
extern s32 g_DebugEnabled;
void InitSettings();
void RicInit(s16 isPrologue);
void PlayerInit(s16 isPrologue) {
    RicInit(isPrologue);
    InitSpritesheet(pl_sprites, LEN(pl_sprites));
    InitAnimations();
    InitCollisions();
    MmxInitGraphics();
    InitSettings();

    // cap health if the save is loaded from an existing file prior the mod
    if (g_Status.hpMax > MMX_MAX_HEALTH) {
        g_Status.hpMax = MMX_MAX_HEALTH;
        if (g_Status.hp > g_Status.hpMax) {
            g_Status.hp = g_Status.hpMax;
        }
    }

#ifdef DEBUG
    g_DebugEnabled = 1;
    // g_DebugHitboxViewMode = 1;
#endif
}

void MmxHudHandler();
void MmxMain(void);

#ifdef DEBUG
static bool DebugAnimFrame() {
    static int debug_frame = -1;
    if (debug_frame >= 0 && !(g_pads[0].pressed & PAD_SELECT)) {
        if (g_pads[0].tapped & PAD_LEFT) {
            if (debug_frame > 0) {
                debug_frame--;
            }
        }
        if (g_pads[0].tapped & PAD_RIGHT) {
            debug_frame++;
        }
        if (g_pads[0].tapped & PAD_UP) {
            debug_frame = 63;
        }
        if (g_pads[0].tapped & PAD_DOWN) {
            debug_frame = 59;
        }
        PLAYER.animCurFrame = debug_frame;
        return true;
    }
    return false;
}
static bool DebugAnimation() {
    static int debug_anim = -1;
    if (debug_anim >= 0) {
        PLAYER.animFrameIdx = debug_anim;
        return true;
    }
    return false;
}
static bool DebugFrameByFrame() {
    if (!(g_pads[0].tapped & PAD_L2) && !(g_pads[0].pressed & PAD_R2)) {
        return true;
    }
    return false;
}
#endif

static void AdjustHealthAndDamage() {
    unsigned int remainder;

    // ensures health is processed in multiple of 10 since this mod uses 10 hp
    // for every entry bar.
    if (g_Status.hp > 0) {
        remainder = (unsigned)g_Status.hp % 10U;
        if (remainder > 0) {
            g_Status.hp -= (signed)remainder;
            if (g_Status.hp == 0) {
                g_Status.hp = 10;
            }
        }
    }

    // now also adjust the damage received; this will make the game
    // significantly harder as the damage it will be rounded up by 10 steps at
    // the time!
    if (PLAYER.hitPoints > 0) {
        remainder = (unsigned)PLAYER.hitPoints % 10U;
        if (remainder > 0) {
            PLAYER.hitPoints = PLAYER.hitPoints - remainder + 10;
        }
    }
}

static void OverrideEntity(Entity* e, unsigned int factory) {
    RicCreateEntFactoryFromEntity(e, factory, 0);
    DestroyEntity(e);
}

static void OverridePrizeDrop(Entity* e) {
    int r;
    switch (e->params) {
    case 0: // small heart
        OverrideEntity(e, B_POWER_CAPSULE_SMALL);
        break;
    case 1: // big heart
        OverrideEntity(e, B_POWER_CAPSULE_BIG);
        break;
    case 2:  // money
    case 3:  // money
    case 4:  // money
    case 5:  // money
    case 6:  // money
    case 7:  // money
    case 8:  // money
    case 9:  // money
    case 10: // money
    case 11: // money
        r = rand();
        if (r & 31) {
            if (r & 3) {
                OverrideEntity(e, B_ENERGY_CAPSULE_SMALL);
            } else {
                // 1/4th of chance to get a big energy capsule
                OverrideEntity(e, B_ENERGY_CAPSULE_BIG);
            }
        } else {
            // 1/32th of chance to get a life-up
            OverrideEntity(e, B_LIFE_UP);
        }
        break;
    case 12: // heart tank
        OverrideEntity(e, B_ENERGY_TANK);
        break;
    case 13: // un pollo
        OverrideEntity(e, B_LIFE_UP);
        break;
    case 23: // life vessel
        OverrideEntity(e, B_HEART_TANK);
        break;
    default: // the rest are all sub-weapons
        break;
    }
}

void EntityHeartTank(Entity* self);
static void OverrideHeartDrop(Entity* self) {
    // more complex than a normal override.
    // we need the castleFlag to mark the item can only be obtained once.
    // the flag is a 16-bit number but the factory param is only 8-bit, so
    // we are forced to manually create the entity replacement
    u16 castleFlag = self->ext.heartDrop.unkB4;
    s16 posX = self->posX.i.hi;
    s16 posY = self->posY.i.hi;
    u16 zPriority = self->zPriority;
    u32 flags = self->flags;
    DestroyEntity(self);
    self->posX.i.hi = posX;
    self->posY.i.hi = posY;
    self->zPriority = zPriority;
    self->flags = flags;
    self->entityId = E_HEART_TANK;
    self->params = castleFlag;
    self->pfnUpdate = EntityHeartTank;
    self->step = 0;
}

static void OverrideStageEntities() {
    // as this character comes from a different game, we want to replace the
    // iconic SOTN entities with custom entities
    for (int i = STAGE_ENTITY_START; i < LEN(g_Entities); i++) {
        if (!g_Entities[i].pfnUpdate) {
            continue;
        }

        // the first 16 entity IDs are currently known to be the same across
        // every stage, this assumption might always hold to be true.
        // every other entity has to be done per-case basis based on the stage
        Entity* e = &g_Entities[i];
        switch (e->entityId) {
        case 1:  // the famous SOTN candle found everywhere in the castle
        case 2:  // explosion puff
        case 4:  // numeric damage
        case 5:  // red door
        case 6:  // intense explosion
        case 7:  // soul steal orbs
        case 8:  // used for static decorations
        case 9:  // stage name pop-up
        case 13: // enemy blood
        case 14: // save game pop-up
            break;
        case 3: // prize drop
            OverridePrizeDrop(e);
            continue;
        case 10: // equip prize drop
        case 11: // relic orb
            continue;
        case 12: // heart drop
            OverrideHeartDrop(&g_Entities[i]);
            continue;
        }
    }
}

void PlayerMain() {
#ifdef DEBUG
    if (DebugFrameByFrame()) {
        if (DebugAnimFrame() || DebugAnimation()) {
            return;
        }
    }
#endif
    if (g_pads[0].pressed & PAD_L2) {
        PLAYER.hitParams = 33;
        PLAYER.ext.player.unkB8 = &g_Entities[128 + 17];
    }
    MmxHudHandler();
    AdjustHealthAndDamage();
    OverrideStageEntities();
    MmxMain();
    if (!(g_Player.padPressed & PAD_DASH)) {
        g_Player.unk44 &= ~IS_DASHING;
    }
}
