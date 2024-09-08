#include "pl.h"

void PlayerMain(void);
void PlayerInit(s16 arg0);
void RicUpdatePlayerEntities(void);
void func_8015E7B4(Unkstruct_8010BF64* arg0);

extern unsigned char* pl_sprites[197];
#ifdef VERSION_PC
u_long* MMX_player[] = {
#else
u_long* MMX_player[] = {
#endif
    PlayerMain,    PlayerInit, RicUpdatePlayerEntities,
    func_8015E7B4, pl_sprites, pl_sprites,
    pl_sprites,    pl_sprites};
