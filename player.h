#ifndef PLAYER_H
#define PLAYER_H

#include "header.h"

// Protótipos de funções relacionadas ao jogador
void initPlayer(PlayerObj *player, Vector2 pos, char name[]);
void drawPlayers(game *g, int lastKeyP1, int lastKeyP2);
void updateMove(PlayerObj *player, int key[], int lastKeyP1);
void skillCollect(PlayerObj *player, short int mapa[][NUMTILES_W]);
void prevCollision(PlayerObj *player, short int mapa[][NUMTILES_W]);
void resolveCollision(PlayerObj *player, Vector2 coords);

#endif
