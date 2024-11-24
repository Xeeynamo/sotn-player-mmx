#include "pl.h"

void PlayerMain(void);
void PlayerInit(s16 arg0);
void RicUpdatePlayerEntities(void);
void RicGetPlayerSensor(Collider* col);

extern unsigned char* pl_sprites[197];
#ifdef VERSION_PC
u_long* MMX_player[] = {
#else
u_long* MMX_player[] = {
#endif
    PlayerMain,    PlayerInit, RicUpdatePlayerEntities,
    RicGetPlayerSensor, pl_sprites, pl_sprites,
    pl_sprites,    pl_sprites};
