#include "pl.h"

extern unsigned char* pl_sprites[197];

PlayerOvl MMX_player = {
    PlayerMain,         PlayerInit, RicUpdatePlayerEntities,
    RicGetPlayerSensor, pl_sprites, pl_sprites,
    pl_sprites,         pl_sprites,
};
