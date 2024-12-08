#include "header.h"
#include "map.h"



// Converte índices para coordenadas
Vector2 getCoords(int x, int y) {
    float coordX = x * TILESIZE + scene.x;
    float coordY = y * TILESIZE + scene.y;
    Vector2 coords = {coordX, coordY};
    return coords;
}

// Converte coordenadas para índices
Vector2Int getIndex(float x, float y) {
    int indexX = ((x + TILESIZE / 2) - scene.x) / TILESIZE;
    int indexY = ((y + TILESIZE / 2) - scene.y) / TILESIZE;
    Vector2Int index = {indexX, indexY};
    return index;
}




// Desenha o mapa, inteirando sobre a matriz
void drawMap(game *g, short int mapa[][NUMTILES_W]) {
    for (int i = 0; i < NUMTILES_H; i++) {
        for (int j = 0; j < NUMTILES_W; j++) {
            short int tileX = scene.x + j * TILESIZE;
            short int tileY = scene.y + i * TILESIZE;

            if(currentMap == 0) DrawTexturePro(g->mapTextures[0], (Rectangle){0, 0, 32, 32}, (Rectangle){tileX, tileY, TILESIZE, TILESIZE}, (Vector2){0, 0}, 0.0f, WHITE);

            if(currentMap == 1) DrawTexturePro(g->mapTextures[2], (Rectangle){0, 0, 32, 32}, (Rectangle){tileX, tileY, TILESIZE, TILESIZE}, (Vector2){0, 0}, 0.0f, WHITE);


            if (currentMap == 0) {
                if (mapa[i][j] == 1) DrawTexturePro(g->mapTextures[1], (Rectangle){32, 64, 32, 32}, (Rectangle){tileX, tileY, TILESIZE, TILESIZE}, (Vector2){0, 0}, 0.0f, WHITE);
                if (mapa[i][j] == 2) DrawTexturePro(g->mapTextures[0], (Rectangle){1*32, 5*32, 32, 32}, (Rectangle){tileX, tileY, TILESIZE, TILESIZE}, (Vector2){0, 0}, 0.0f, WHITE);
            }

            if (currentMap == 1) {
                if (mapa[i][j] == 1) DrawTexturePro(g->mapTextures[0], (Rectangle){3*32, 8*32, 32, 32}, (Rectangle){tileX, tileY, TILESIZE, TILESIZE}, (Vector2){0, 0}, 0.0f, WHITE);
                if (mapa[i][j] == 2) DrawTexturePro(g->mapTextures[0], (Rectangle){3*32, 109*32, 32, 32}, (Rectangle){tileX, tileY, TILESIZE, TILESIZE}, (Vector2){0, 0}, 0.0f, WHITE);
                if (mapa[i][j] == 7) DrawTexturePro(g->mapTextures[0], (Rectangle){6*32, 70*32, 32, 32}, (Rectangle){tileX, tileY, TILESIZE, TILESIZE}, (Vector2){0, 0}, 0.0f, WHITE);
            }

            if (mapa[i][j] == 4) DrawTexturePro(g->mapTextures[4], (Rectangle){0, 0, 64, 64}, (Rectangle){tileX, tileY, TILESIZE, TILESIZE}, (Vector2){0, 0}, 0.0f, WHITE);
            if (mapa[i][j] == 5) DrawTexturePro(g->mapTextures[5], (Rectangle){0, 0, 64, 64}, (Rectangle){tileX, tileY, TILESIZE, TILESIZE}, (Vector2){0, 0}, 0.0f, WHITE);
            if (mapa[i][j] == 6) DrawTexturePro(g->mapTextures[6], (Rectangle){0, 0, 64, 64}, (Rectangle){tileX, tileY, TILESIZE, TILESIZE}, (Vector2){0, 0}, 0.0f, WHITE);
        }
    }
}



// Gera um mapa aleatório (Não utilizado)
void initMap(short int mapa[][NUMTILES_H][NUMTILES_W]) {
    for (int i = 0; i < NUMTILES_H; i++) {
        for (int j = 0; j < NUMTILES_W; j++) {
            if (i % 2 && j % 2) {
                mapa[0][i][j] = 1;
                mapa[1][i][j] = 1;
            } else {
                mapa[0][i][j] = 0;
                mapa[1][i][j] = 0;
            }
        }
    }
    for (int i = 0; i < NUMTILES_H; i += 2) {
        int x;
        for (int j = 0; j < NUMTILES_W; j += 2) {
            x = rand() % 2;
            if (x && (i > 1 || j > 1) && (i < NUMTILES_H - 1 || j < NUMTILES_W - 1)) {
                mapa[0][i][j] = 2;
            } else {
                mapa[0][i][j] = 0;
            }
        }
    }

    short int mapa2[][NUMTILES_W] = {
                        {0, 0, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0, 0, 0},
                        {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                        {2, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 2, 0, 0},
                        {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 2},
                        {0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0},
                        {0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 0, 0, 2, 0, 0},
                        {0, 1, 0, 1, 0, 7, 7, 7, 7, 7, 0, 1, 0, 1, 0},
                        {0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 0, 0, 2, 0, 0},
                        {0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0},
                        {2, 0, 2, 0, 2, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0},
                        {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                        {2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2},
                        {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                        {0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 2, 0, 0, 0, 0},
                     };
    memcpy(mapa[1], mapa2, NUMTILES_H * NUMTILES_W * 2);
}

