// SPDX-License-Identifier: AGPL-3.0-or-later
#include <game.h>
#include <sfx.h>

#define COLOR16(r, g, b, a) (r) + ((g) << 5) + ((b) << 10) + ((a) << 15)
#define COLOR32(r, g, b) COLOR16(r >> 3, g >> 3, b >> 3, 1)

// MMX stuff
#define MMX_WALK_SPEED FIX(1.5)
#define MMX_DASH_SPEED FIX(3.5)
#define MMX_JUMP_SPEED FIX(4.5) // maybe 5?
#define MMX_FALL_SPEED FIX(5.75)
#define MMX_WALL_SPEED FIX(1.0)
#define MMX_WALL_JUMP_FORCE FIX(8)
#define MMX_FALL_MAX_VELOCITY FIX(5.75)
#define MMX_MAX_HEALTH 320

#define PAD_DASH (PAD_CIRCLE | PAD_L1)
#define IS_DASHING 0x8000

#define JUMP_WITH_DASH 1
#define JUMP_FROM_WALL 2

#define GAME_OVER 0x80000

#define CHECK_GROUND 1
#define CHECK_FALL 4
#define CHECK_FACING 8
#define CHECK_JUMP 0x10
// #define CHECK_CRASH 0x40
#define CHECK_80 0x80
#define CHECK_GRAVITY_HIT 0x200
#define CHECK_ATTACK 0x1000
#define CHECK_CROUCH 0x2000
#define CHECK_GRAVITY_FALL 0x8000
#define CHECK_GRAVITY_JUMP 0x10000
#define CHECK_GROUND_AFTER_HIT 0x20000
#define CHECK_SLIDE 0x40000
#define CHECK_WALL 0x80000

enum MmxSteps {
    PL_S_STAND,
    PL_S_WALK,
    PL_S_CROUCH,
    PL_S_FALL,
    PL_S_JUMP,
    PL_S_5, // unused
    PL_S_6, // unused
    PL_S_7, // unused
    PL_S_HIGHJUMP,
    PL_S_9, // unused
    PL_S_HIT,
    PL_S_11, // unused
    PL_S_BOSS_GRAB,
    PL_S_13, // unused
    PL_S_14, // unused
    PL_S_15, // unused
    PL_S_DEAD,
    PL_S_17, // unused
    PL_S_STAND_IN_AIR,
    PL_S_FLAME_WHIP,
    PL_S_HYDROSTORM,
    PL_S_THROW_DAGGERS,
    PL_S_DEAD_PROLOGUE,
    PL_S_SLIDE,
    PL_S_BLADEDASH,
    PL_S_RUN,
    PL_S_SLIDE_KICK,
    PL_S_SUBWPN_CRASH,
    PL_S_WALL,
    PL_S_DASH,
    PL_S_DASH_AIR,
    PL_S_31, // unused
    PL_S_INIT,
    PL_S_DEBUG = 0xF0,
};

enum RicSfxs {
    SFX_GRUNT_A = 0x6F9,
    SFX_GRUNT_B,
    SFX_GRUNT_C,
    SFX_GRUNT_SUBWPN_A,
    SFX_HYDROSTORM = 0x700,
    SFX_CROSS_CRASH,
};

typedef enum {
    PL_JUMP_NONE,
    PL_JUMP_ASCENDING,
    PL_JUMP_DESCENDING,
} MmxJumpState;

enum MmxTimers {
    PL_T_POISON,
    PL_T_CURSE,
    PL_T_2,
    PL_T_3,
    PL_T_4,
    PL_T_5,
    PL_T_FALL,
    PL_T_7,
    PL_T_8,
    PL_T_ATTACK,
    PL_T_10,
    PL_T_RUN,
    PL_T_12,
    PL_T_INVINCIBLE_SCENE, // "dying" in the prologue
    PL_T_INVINCIBLE,
    PL_T_AFTERIMAGE_DISABLE,
};

enum MmxEntities {
    E_NONE,
    E_FACTORY,                       // RicEntityFactory
    E_SMOKE_PUFF,                    // RicRicEntitySmokePuff
    E_SUBWPN_CROSS,                  // RicEntitySubwpnCross
    E_80169C10,                      // func_80169C10
    E_HIT_BY_CUT_BLOOD,              // RicEntityHitByCutBlood
    E_SUBWPN_CROSS_TRAIL,            // RicEntitySubwpnCrossTrail
    E_SUBWPN_HOLY_WATER,             // RicEntitySubwpnHolyWater
    E_SUBWPN_HOLY_WATER_FLAME,       // RicEntitySubwpnHolyWaterFlame
    E_80161C2C,                      // func_80161C2C
    E_WHIP,                          // RicEntityWhip
    E_CRASH_HYDROSTORM,              // RicEntityCrashHydroStorm
    E_CRASH_CROSS_BEAM,              // RicEntityCrashCrossBeam
    E_CRASH_CROSS_ROTATING,          // RicEntitySubwpnCrashCross
    E_NOT_IMPLEMENTED_1,             // RicEntityNotImplemented1
    E_NOT_IMPLEMENTED_2,             // RicEntityNotImplemented2
    E_ARM_BRANDISH_WHIP,             // RicEntityArmBrandishWhip
    E_80167964,                      // func_80167964
    E_DUMMY_18,                      // RicEntityDummy
    E_80161EF8,                      // func_80161EF8
    E_NOT_IMPLEMENTED_3,             // RicEntityNotImplemented3
    E_REVIVAL_COLUMN,                // RicEntityRevivalColumn
    E_APPLY_MARIA_POWER_ANIM,        // RicEntityApplyMariaPowerAnim
    E_80160C38,                      // RicEntitySlideKick
    E_BLADE_DASH,                    // RicEntityBladeDash
    E_801623E0,                      // func_801623E0
    E_80162604,                      // func_80162604
    E_MARIA,                         // RicEntityMaria
    E_MARIA_POWERS,                  // RicEntityMariaPowers
    E_80160F0C,                      // func_80160F0C
    E_NOT_IMPLEMENTED_4,             // RicEntityNotImplemented4
    E_BLINK_WHITE,                   // RicEntityPlayerBlinkWhite
    E_SUBWPN_CRASH_CROSS_PARTICLES,  // RicEntitySubwpnCrashCrossParticles
    E_801641A0,                      // func_801641A0
    E_SHRINKING_POWERUP_RING,        // RicRicEntityShrinkingPowerUpRing
    E_80167A70,                      // func_80167A70
    E_SUBWPN_AXE,                    // RicEntitySubwpnAxe
    E_CRASH_AXE,                     // RicEntityCrashAxe
    E_SUBWPN_DAGGER,                 // RicEntitySubwpnDagger
    E_80160D2C,                      // func_80160D2C
    E_HIT_BY_ICE,                    // RicEntityHitByIce
    E_HIT_BY_LIGHTNING,              // RicEntityHitByLightning
    E_SUBWPN_REBOUND_STONE,          // RicEntitySubwpnReboundStone
    E_SUBWPN_VIBHUTI,                // RicEntitySubwpnThrownVibhuti
    E_SUBWPN_AGUNEA,                 // RicEntitySubwpnAgunea
    E_SUBWPN_AGUNEA_HIT_ENEMY,       // RicEntityAguneaHitEnemy
    E_CRASH_VIBHUTI,                 // RicEntityCrashVibhuti
    E_CRASH_VIBHUTI_CLOUD,           // RicEntityVibhutiCrashCloud
    E_CRASH_REBOUND_STONE,           // RicEntityCrashReboundStone
    E_8016D9C4,                      // func_8016D9C4
    E_CRASH_REBOUND_STONE_EXPLOSION, // RicEntityCrashReboundStoneExplosion
    E_CRASH_BIBLE,                   // RicEntityCrashBible
    E_CRASH_BIBLE_BEAM,              // RicEntityCrashBibleBeam
    E_SUBWPN_BIBLE,                  // RicEntitySubpwnBible
    E_SUBWPN_BIBLE_TRAIL,            // RicEntitySubpwnBibleTrail
    E_SUBWPN_STOPWATCH,              // RicEntitySubwpnStopwatch
    E_SUBWPN_STOPWATCH_CIRCLE,       // RicEntitySubwpnStopwatchCircle
    E_801705EC,                      // func_801705EC
    E_8016F198,                      // func_8016F198
    E_AGUNEA_CIRCLE,                 // RicEntityAguneaCircle
    E_AGUNEA_LIGHTNING,              // RicEntityAguneaLightning
    E_CRASH_REBOUND_STONE_PARTICLES, // RicEntityCrashReboundStoneParticles
    E_HIT_BY_DARK,                   // RicEntityHitByDark
    E_HIT_BY_HOLY,                   // RicEntityHitByHoly
    E_CRASH_STOPWATCH_DONE_PARTICLE, // RicEntityCrashStopwatchDoneSparkle
    E_80170548,                      // func_80170548
    E_TELEPORT,                      // RicEntityTeleport
    E_DUMMY_66,                      // RicEntityDummy
    NUM_RIC_ENTITIES,
    E_SMOKE_PUFF_WHEN_SLIDING = 68,
    E_W_LEMON,
    E_W_CUCUMBER,
    E_W_CHARJELLY,
    E_W_SHOTGUN_ICE,
    E_W_ELECTRIC_SPARK,
    E_W_ROLLING_SHIELD,
    E_W_HOMING_TORPEDO,
    E_W_BOOMERANG_CUTTER,
    E_W_CHAMELEON_STING,
    E_W_STORM_TORNADO,
    E_W_FIRE_WAVE,
    E_W_HADOUKEN,
    E_MMX_PRIZE_DROP,
    E_POWER_CAPSULE_SMALL,
    E_POWER_CAPSULE_BIG,
    E_ENERGY_CAPSULE_SMALL,
    E_ENERGY_CAPSULE_BIG,
    E_LIFE_UP,
    E_HEART_TANK,
    E_ENERGY_TANK,
    NUM_ENTITIES,
};

enum RicBlueprints {
    BP_SKID_SMOKE,
    BP_SMOKE_PUFF,
    BP_SUBWPN_CROSS,
    BP_SUBWPN_CROSS_PARTICLES,
    BP_EMBERS,
    BP_5,
    BP_SUBWPN_HOLYWATER,
    BP_HOLYWATER_FIRE,
    BP_HIT_BY_FIRE,
    BP_HOLYWATER_FLAMES,
    BP_WHIP,
    BP_MULTIPLE_EMBERS,
    BP_HYDROSTORM,
    BP_CRASH_CROSS,
    BP_CRASH_CROSSES_ONLY,
    BP_NOT_IMPLEMENTED_1,
    BP_NOT_IMPLEMENTED_2,
    BP_ARM_BRANDISH_WHIP,
    BP_18,
    BP_AXE,
    BP_20,
    BP_NOT_IMPLEMENTED_3,
    BP_REVIVAL_COLUMN,
    BP_MARIA_POWERS_APPLIED,
    BP_SLIDE,
    BP_25,
    BP_BLADE_DASH,
    BP_BLUE_CIRCLE,
    BP_BLUE_SPHERE,
    BP_MARIA,
    BP_MARIA_POWERS_INVOKED,
    BP_31,
    BP_NOT_IMPLEMENTED_4,
    BP_RIC_BLINK,
    BP_CRASH_CROSS_PARTICLES,
    BP_35,
    BP_36,
    BP_37,
    BP_38,
    BP_39,
    BP_UNUSED_40,
    BP_CRASH_AXE,
    BP_42,
    BP_SUBWPN_DAGGER,
    BP_CRASH_DAGGER,
    BP_HIGH_JUMP,
    BP_HIT_BY_CUT,
    BP_HIT_BY_ICE,
    BP_HIT_BY_THUNDER,
    BP_VIBHUTI,
    BP_REBOUND_STONE,
    BP_AGUNEA,
    BP_AGUNEA_HIT_ENEMY,
    BP_DEATH_BY_FIRE,
    BP_CRASH_VITHUBI,
    BP_VITHUBI_CRASH_CLOUD,
    BP_CRASH_REBOUND_STONE,
    BP_57,
    BP_CRASH_REBOUND_STONE_EXPLOSION,
    BP_CRASH_BIBLE,
    BP_CRASH_BIBLE_BEAM,
    BP_BIBLE,
    BP_BIBLE_TRAIL,
    BP_SUBWPN_STOPWATCH,
    BP_STOPWATCH_RIPPLE,
    BP_CRASH_STOPWATCH,
    BP_66,
    BP_CRASH_AGUNEA,
    BP_CRASH_AGUNEA_THUNDER,
    BP_CRASH_REBOUND_STONE_PARTICLES,
    BP_HIT_BY_DARK,
    BP_HIT_BY_HOLY,
    BP_AGUNEA_THUNDER,
    BP_73,
    BP_SMOKE_PUFF_2,
    BP_SKID_SMOKE_2,
    BP_SKID_SMOKE_3,
    BP_TELEPORT,

    // ==============================
    B_DUMMY,  // begin of MMX blueprints
    B_P_DASH, // particles used when dashing
    B_P_WALL, // particle used when sliding to a wall
    B_P_JUMP_FROM_WALL,
    B_W_LEMON,
    B_W_CUCUMBER,
    B_W_CHARJELLY,
    B_W_SHOTGUN_ICE,
    B_W_ELECTRIC_SPARK,
    B_W_ROLLING_SHIELD,
    B_W_HOMING_TORPEDO,
    B_W_BOOMERANG_CUTTER,
    B_W_CHAMELEON_STING,
    B_W_STORM_TORNADO,
    B_W_FIRE_WAVE,
    B_W_HADOUKEN,
    B_MMX_PRIZE_DROP,
    B_POWER_CAPSULE_SMALL,
    B_POWER_CAPSULE_BIG,
    B_ENERGY_CAPSULE_SMALL,
    B_ENERGY_CAPSULE_BIG,
    B_LIFE_UP,
    B_HEART_TANK,
    B_ENERGY_TANK,
    NUM_BLUEPRINTS,
};

enum MmxWeapons {
    W_DUMMY,
    W_LEMON,
    W_CUCUMBER,
    W_CHARJELLY,
    W_SHOTGUN_ICE,
    W_ELECTRIC_SPARK,
    W_ROLLING_SHIELD,
    W_HOMING_TORPEDO,
    W_BOOMERANG_CUTTER,
    W_CHAMELEON_STING,
    W_STORM_TORNADO,
    W_FIRE_WAVE,
    W_HADOUKEN,
    NUM_WEAPONS,
};

enum MmxPalettes {
    PAL_PLAYER = 0x120,
    PAL_HUD,
    PAL_PARTICLES,
    PAL_HEART,
};

enum MmxAnims {
    PL_A_DUMMY,
    PL_A_STAND,
    PL_A_WALK,
    PL_A_FALL,
    PL_A_JUMP,
    PL_A_LAND,
    PL_A_DASH,
    PL_A_WALL,
    PL_A_STAND_W,
    PL_A_WALK_W,
    PL_A_FALL_W,
    PL_A_JUMP_W,
    PL_A_LAND_W,
    PL_A_DASH_W,
    PL_A_WALL_W,
    PL_A_HIT_STUN,
    PL_A_HIT_SMALL,
    PL_A_END,
};

extern unsigned char* pl_sprites[197];
extern AnimationFrame* mmx_anims[];
extern u32 g_PadReleased;
extern u32 g_ChargeTimer;
extern u32 g_WallSlideTimer;
extern u32 g_DashTimer;
extern u32 g_DashAirUsed;

// END OF MMX STUFF

// Richter mostly uses the same steps as Alucard, or uses unused Alucard
// steps. There are a couple steps that mean one thing for Alucard, and
// another for Richter. This enum handles Richter's version of the ones that
// overlap.
typedef enum {
    Player_RichterCrouch = 2,
} Richter_PlayerSteps;

extern MmxJumpState g_JumpState;

extern s16* g_MmxPlSprites[];
extern SpriteParts* g_SpritesWeapons[];
extern SpriteParts* g_SpritesItems[];
extern SpriteParts* D_801541A8[];
extern void func_80159C04(void);
extern void DestroyEntity(Entity* entity);
extern void func_8015BB80(void);
extern void RicHandleBladeDash(void);
extern void RicSetStep(PlayerSteps step);
void MmxSetAnimation(enum MmxAnims anim);
void RicSetAnimation(AnimationFrame* unk0);
extern void RicDecelerateX(s32 speed);
extern s32 RicCheckFacing(void);
extern void RicSetSpeedX(s32 speed);
extern void RicSetCrouch(s32 arg0, s32 velocityX);
extern void RicSetWalkFromJump(s32 arg0, s32 velocityX);
extern void RicSetStand(s32 velocityX);
extern void MmxPrepareStandFromJump(s32 velocityX);
extern void RicSetWalk(s32);
extern void RicSetRun(void);
extern bool MmxPerformAttack(void);
extern void RicSetFall(void);
extern bool RicCheckInput(s32 checks);
extern void RicSetSubweaponParams(Entity*);
extern s32 func_8015FDB0(Primitive* poly, s16 posX, s16 posY);
extern Entity* RicCreateEntFactoryFromEntity(
    Entity* entity, u32 arg1, s32 arg2);

void func_8015E800();
extern s32 func_8016840C(s16 x, s16 y);

extern s16 D_80154568[];
extern s32 g_IsPrologueStage;
extern s16 D_80154574[];
extern s16 D_80154594[];
extern s32 D_8015459C;
extern RECT D_801545A0;
extern s16 g_DeadPrologueTimer;
extern s16 D_801545AA;
extern s16 D_801545AC;
extern u8 D_801545B0[];
extern u8 D_801545C4[16];
extern u8 D_801545D4[16];
// These appear to be a set of collision boxes
extern Point16 D_801545E4[4];
extern Point16 D_801545F4[4];
extern Point16 D_80154604[14];
extern u16 D_8015463C[4];
extern u16 D_80154644[4];
extern u16 D_8015464C[8];
extern u16 D_8015465C[4];
extern u16 D_80154664[8];

extern u8 D_80154674[][4]; // TODO AnimationFrames*[]
extern SubweaponDef D_80154688[];
extern u8 uv_anim_801548F4[6][8];
extern FactoryBlueprint g_RicFactoryBlueprints[];

extern s8 D_80156A28;
extern s8 D_80156904;

extern ButtonComboState g_bladeDashButtons;
extern ButtonComboState D_801758E4;
extern u16 g_RicDebugCurFrame;
extern u16 g_RicDebugDrawFlags;
extern u16 g_RicDebugPalette;
extern s16 g_IsRicDebugEnter;
extern s32 D_80175958[32];
extern s32 D_801759D8[32];

// pl_anims.c
extern AnimationFrame ric_anim_press_up[];
extern AnimationFrame ric_anim_stop_run[];
extern AnimationFrame ric_anim_stand[];
extern AnimationFrame ric_anim_stand_relax[];
extern AnimationFrame ric_anim_walk[];
extern AnimationFrame ric_anim_crouch_from_stand2[];
extern AnimationFrame ric_anim_crouch[];
extern AnimationFrame ric_anim_land_from_air_run[];
extern AnimationFrame ric_anim_crouch_from_stand[];
extern AnimationFrame D_801554E0[];
extern AnimationFrame D_801554F0[];
extern AnimationFrame D_8015550C[];
extern AnimationFrame D_80155528[];
extern AnimationFrame D_80155534[];
extern AnimationFrame D_80155544[];
extern AnimationFrame D_8015555C[];
extern AnimationFrame D_80155588[];
extern AnimationFrame D_801555A8[];
extern AnimationFrame D_801555C8[];
extern AnimationFrame D_801555E8[];
extern AnimationFrame ric_anim_throw_daggers[];
extern AnimationFrame D_80155638[];
extern AnimationFrame ric_ric_anim_stand_in_air[];
extern AnimationFrame ric_anim_run[];
extern AnimationFrame ric_anim_stun[];
extern AnimationFrame D_8015569C[];
extern AnimationFrame D_801556C4[];
extern AnimationFrame D_80155704[];
extern AnimationFrame ric_anim_brandish_whip[];
extern AnimationFrame D_80155738[];
extern AnimationFrame D_80155740[];
extern AnimationFrame D_80155748[];
extern AnimationFrame D_80155750[];
extern AnimationFrame D_8015577C[];
extern AnimationFrame D_80155788[];
extern AnimationFrame ric_anim_high_jump[];
extern AnimationFrame D_80155738[];
extern AnimationFrame ric_anim_point_finger_up[];
extern AnimationFrame ric_anim_blade_dash[];
extern AnimationFrame ric_anim_flame_whip[];
extern AnimationFrame D_801558B4[];
extern AnimationFrame D_801558D4[];
extern AnimationFrame D_801558E4[];
extern AnimationFrame D_801558DC[];
extern AnimationFrame D_8015591C[];
extern AnimationFrame D_80155950[];
