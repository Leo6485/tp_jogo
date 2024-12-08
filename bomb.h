#ifndef BOMB_H
#define BOMB_H

#include "header.h"

// Protótipos de funções relacionadas à bomba
void initBombs(PlayerObj *player, int n);
void draw_bomb(game *g, PlayerObj *player);
void update_bomb(game *g, PlayerObj *player, short int mapa[][NUMTILES_W], int key);
void bombDamage(game *g);
void bombCollision(PlayerObj *player, PlayerObj *player2, short int mapa[][NUMTILES_W]);
void bombToBombCollision(game *g);

#endif