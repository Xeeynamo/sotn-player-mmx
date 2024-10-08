#include "pl.h"

// clang-format off
static AnimationFrame anim_dummy[] = {
    {160, FRAME(3, 2)},
    A_END};
static AnimationFrame anim_stand[] = {
    {160, FRAME(3, 2)},
    {4, FRAME(3, 2)},
    {4, FRAME(4, 2)},
    {4, FRAME(3, 2)},
    {80, FRAME(2, 2)},
    {4, FRAME(3, 2)},
    {4, FRAME(4, 2)},
    {4, FRAME(3, 2)},
    {4, FRAME(3, 2)},
    {4, FRAME(4, 2)},
    {4, FRAME(3, 2)},
    A_LOOP_AT(0)};
static AnimationFrame anim_walk[] = {
    {4, FRAME(6, 2)},
    {4, FRAME(9, 2)},
    {4, FRAME(10, 2)},
    {4, FRAME(11, 2)},
    {4, FRAME(12, 2)},
    {4, FRAME(13, 2)},
    {4, FRAME(14, 2)},
    {4, FRAME(15, 2)},
    {4, FRAME(16, 2)},
    {4, FRAME(7, 2)},
    {4, FRAME(8, 2)},
    A_LOOP_AT(1)};
static AnimationFrame anim_jump[] = {
    {4, FRAME(17, 8)},
    {4, FRAME(18, 4)},
    {80, FRAME(19, 4)},
    A_END};
static AnimationFrame anim_fall[] = {
    {8, FRAME(20, 4)},
    {80, FRAME(21, 4)},
    A_END};
static AnimationFrame anim_land[] = {
    {4, FRAME(22, 2)},
    {4, FRAME(23, 2)},
    {160, FRAME(3, 2)},
    {4, FRAME(3, 2)},
    {4, FRAME(4, 2)},
    {4, FRAME(3, 2)},
    {80, FRAME(2, 2)},
    {4, FRAME(3, 2)},
    {4, FRAME(4, 2)},
    {4, FRAME(3, 2)},
    {4, FRAME(3, 2)},
    {4, FRAME(4, 2)},
    {4, FRAME(3, 2)},
    A_LOOP_AT(2)};
static AnimationFrame anim_dash[] = {
    {4, FRAME(24, 2)},
    {80, FRAME(25, 2)},
    A_END};
static AnimationFrame anim_wall[] = {
    {4, FRAME(26, 4)},
    {4, FRAME(27, 4)},
    {80, FRAME(28, 4)},
    A_END};
static AnimationFrame anim_stand_w[] = {
    {4, FRAME(33, 2)},
    {4, FRAME(34, 2)},
    A_END,
};
static AnimationFrame anim_walk_w[] = {
    {4, FRAME(35, 2)},
    {4, FRAME(38, 2)},
    {4, FRAME(39, 2)},
    {4, FRAME(40, 2)},
    {4, FRAME(41, 2)},
    {4, FRAME(42, 2)},
    {4, FRAME(43, 2)},
    {4, FRAME(44, 2)},
    {4, FRAME(45, 2)},
    {4, FRAME(36, 2)},
    {4, FRAME(37, 2)},
    A_LOOP_AT(1)};
static AnimationFrame anim_jump_w[] = {
    {4, FRAME(46, 8)},
    {4, FRAME(47, 4)},
    {80, FRAME(48, 4)},
    A_END};
static AnimationFrame anim_fall_w[] = {
    {8, FRAME(49, 4)},
    {80, FRAME(50, 4)},
    A_END};
static AnimationFrame anim_land_w[] = {
    {4, FRAME(51, 2)},
    {4, FRAME(52, 2)},
    {4, FRAME(33, 2)},
    {4, FRAME(34, 2)},
    A_LOOP_AT(2)};
static AnimationFrame anim_dash_w[] = {
    {4, FRAME(53, 2)},
    {80, FRAME(54, 2)},
    A_END};
static AnimationFrame anim_wall_w[] = {
    {4, FRAME(55, 4)},
    {4, FRAME(56, 4)},
    {80, FRAME(57, 4)},
    A_END};
static AnimationFrame anim_hit_stun[] = {
    {4, FRAME(58, 4)},
    {2, FRAME(59, 4)},
    {2, FRAME(63, 4)},
    {2, FRAME(59, 4)},
    {2, FRAME(63, 4)},
    {2, FRAME(59, 4)},
    {2, FRAME(63, 4)},
    {14, FRAME(59, 4)},
    {2, FRAME(58, 4)},
    A_END};
static AnimationFrame anim_hit_small[] = {
    {4, FRAME(61, 4)},
    {2, FRAME(62, 4)},
    {2, FRAME(64, 4)},
    {2, FRAME(62, 4)},
    {2, FRAME(64, 4)},
    {2, FRAME(62, 4)},
    {2, FRAME(64, 4)},
    {14, FRAME(62, 4)},
    {2, FRAME(61, 4)},
    A_END};
static AnimationFrame anim_dead[] = {
    {64, FRAME(58, 4)},
    {2, FRAME(0, 0)},
    A_END};

// Remember to update `MmxAnims` in `pl.h`
AnimationFrame* mmx_anims[PL_A_END] = {
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
STATIC_ASSERT(LEN(mmx_anims) == PL_A_END, "anims array wrong size");
