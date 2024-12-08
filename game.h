#ifndef GAME_H
#define GAME_H

#include "header.h"

// Protótipos de funções relacionadas ao jogo
void initGame(game *g, short int mapa[][NUMTILES_H][NUMTILES_W]);
void loadAssets(game *g);
void unloadAssets(game *g);
void updateGame(game *g, short int mapa[][NUMTILES_W]);

#endif
