#include "pl.h"

// Ensures the normal animation has the same amount of frames of the attack
// equivalent animation. This is important for animations like walking, where
// we want to have continuity between MMX walking and MMX shooting and similar
// animations.
// Not all the animations are synced. There are exceptions in the function
// ChangeAnimToAttack, which uses this specific feature.
#define SYNC_W_ANIM(normal_anim, weapon_anim)                                  \
    STATIC_ASSERT(LEN(normal_anim) == LEN(weapon_anim), "unsynced attack "     \
                                                        "anim")

// clang-format off
static AnimationFrame anim_dummy[] = {
    POSE(160, 3, 1),
    POSE_END};
static AnimationFrame anim_stand[] = {
    POSE(160, 3, 1),
    POSE(4, 3, 1),
    POSE(4, 4, 1),
    POSE(4, 3, 1),
    POSE(80, 2, 1),
    POSE(4, 3, 1),
    POSE(4, 4, 1),
    POSE(4, 3, 1),
    POSE(4, 3, 1),
    POSE(4, 4, 1),
    POSE(4, 3, 1),
    POSE_LOOP(0)};
static AnimationFrame anim_walk[] = {
    POSE(4, 6, 1),
    POSE(4, 9, 1),
    POSE(4, 10, 1),
    POSE(4, 11, 1),
    POSE(4, 12, 1),
    POSE(4, 13, 1),
    POSE(4, 14, 1),
    POSE(4, 15, 1),
    POSE(4, 16, 1),
    POSE(4, 7, 1),
    POSE(4, 8, 1),
    POSE_LOOP(1)};
static AnimationFrame anim_jump[] = {
    POSE(4, 17, 4),
    POSE(4, 18, 2),
    POSE(80, 19, 2),
    POSE_END};
static AnimationFrame anim_fall[] = {
    POSE(8, 20, 2),
    POSE(80, 21, 2),
    POSE_END};
static AnimationFrame anim_land[] = {
    POSE(4, 22, 1),
    POSE(4, 23, 1),
    POSE(160, 3, 1),
    POSE(4, 3, 1),
    POSE(4, 4, 1),
    POSE(4, 3, 1),
    POSE(80, 2, 1),
    POSE(4, 3, 1),
    POSE(4, 4, 1),
    POSE(4, 3, 1),
    POSE(4, 3, 1),
    POSE(4, 4, 1),
    POSE(4, 3, 1),
    POSE_LOOP(2)};
static AnimationFrame anim_dash[] = {
    POSE(4, 24, 1),
    POSE(80, 25, 1),
    POSE_END};
static AnimationFrame anim_wall[] = {
    POSE(4, 26, 2),
    POSE(4, 27, 2),
    POSE(80, 28, 2),
    POSE_END};
static AnimationFrame anim_stand_w[] = {
    POSE(4, 33, 1),
    POSE(4, 34, 1),
    POSE_END,
};
// anim_stand_w exception

static AnimationFrame anim_walk_w[] = {
    POSE(4, 35, 1),
    POSE(4, 38, 1),
    POSE(4, 39, 1),
    POSE(4, 40, 1),
    POSE(4, 41, 1),
    POSE(4, 42, 1),
    POSE(4, 43, 1),
    POSE(4, 44, 1),
    POSE(4, 45, 1),
    POSE(4, 36, 1),
    POSE(4, 37, 1),
    POSE_LOOP(1)};
SYNC_W_ANIM(anim_walk, anim_walk_w);

static AnimationFrame anim_jump_w[] = {
    POSE(4, 46, 4),
    POSE(4, 47, 2),
    POSE(80, 48, 2),
    POSE_END};
SYNC_W_ANIM(anim_jump, anim_jump_w);

static AnimationFrame anim_fall_w[] = {
    POSE(8, 49, 2),
    POSE(80, 50, 2),
    POSE_END};
SYNC_W_ANIM(anim_jump, anim_jump_w);

static AnimationFrame anim_land_w[] = {
    POSE(4, 51, 1),
    POSE(4, 52, 1),
    POSE(4, 33, 1),
    POSE(4, 34, 1),
    POSE_LOOP(2)};
// anim_land_w exception

static AnimationFrame anim_dash_w[] = {
    POSE(4, 53, 1),
    POSE(80, 54, 1),
    POSE_END};
SYNC_W_ANIM(anim_dash, anim_dash_w);

static AnimationFrame anim_wall_w[] = {
    POSE(4, 55, 2),
    POSE(4, 56, 2),
    POSE(80, 57, 2),
    POSE_END};
SYNC_W_ANIM(anim_wall, anim_wall_w);

static AnimationFrame anim_hit_stun[] = {
    POSE(4, 58, 2),
    POSE(2, 59, 2),
    POSE(2, 63, 2),
    POSE(2, 59, 2),
    POSE(2, 63, 2),
    POSE(2, 59, 2),
    POSE(2, 63, 2),
    POSE(14, 59, 2),
    POSE(2, 58, 2),
    POSE_END};

static AnimationFrame anim_hit_small[] = {
    POSE(4, 61, 2),
    POSE(2, 62, 2),
    POSE(2, 64, 2),
    POSE(2, 62, 2),
    POSE(2, 64, 2),
    POSE(2, 62, 2),
    POSE(2, 64, 2),
    POSE(14, 62, 2),
    POSE(2, 61, 2),
    POSE_END};

static AnimationFrame anim_dead[] = {
    POSE(64, 58, 2),
    POSE(2, 0, 0),
    POSE_END};

// Remember to update `MmxAnims` in `pl.h`
AnimationFrame* mmx_anims[PL_POSE_END] = {
    anim_dummy,
    anim_stand,
    anim_walk,
    anim_fall,
    anim_jump,
    anim_land,
    anim_dash,
    anim_wall,
    anim_stand_w,
    anim_walk_w,
    anim_fall_w,
    anim_jump_w,
    anim_land_w,
    anim_dash_w,
    anim_wall_w,
    anim_hit_stun,
    anim_hit_small,
    anim_dead,
};
STATIC_ASSERT(LEN(mmx_anims) == PL_POSE_END, "anims array wrong size");
