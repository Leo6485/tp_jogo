#ifndef MAP_H
#define MAP_H

#include "header.h"

// Protótipos de funções relacionadas ao mapa
void drawMap(game *g, short int mapa[][NUMTILES_W]);
void initMap(short int mapa[][NUMTILES_H][NUMTILES_W]);
Vector2 getCoords(int x, int y);
Vector2Int getIndex(float x, float y);

#endif
