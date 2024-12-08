#include "game.h"
#include "player.h"
#include "bomb.h"
#include "map.h"
#include "screens.h"
#include <raylib.h>


void initGame(game *g, short int mapa[][NUMTILES_H][NUMTILES_W]) {
    initPlayer(&g->player, (Vector2){scene.x, scene.y}, "MAGO\0");
    initPlayer(&g->player2, (Vector2){scene.x + (NUMTILES_W - 1) * TILESIZE, scene.y + (NUMTILES_H - 1) * TILESIZE}, "ESQUELETO\0");

    initBombs(&g->player, 10);
    initBombs(&g->player2, 10);

    initMap(mapa);
}

void loadAssets(game *g) {
    // Carrega os assets
    Image image;
    image = LoadImage("assets/textures/bomb.png");
    g->bombTexture = LoadTextureFromImage(image);

    image = LoadImage("assets/textures/explosion.png");
    g->explosionTexture = LoadTextureFromImage(image);

    image = LoadImage("assets/textures/player.png");
    g->playerTexture = LoadTextureFromImage(image);

    image = LoadImage("assets/textures/player2.png");
    g->player2Texture = LoadTextureFromImage(image);


    // Texturas do mapa
    image = LoadImage("assets/textures/map/tileset.png");
    g->mapTextures[0] = LoadTextureFromImage(image);

    image = LoadImage("assets/textures/map/Decorations.png");
    g->mapTextures[1] = LoadTextureFromImage(image);

    image = LoadImage("assets/textures/map/map2_floor.png");
    g->mapTextures[2] = LoadTextureFromImage(image);

    image = LoadImage("assets/textures/map/skills/max_bombs.png");
    g->mapTextures[4] = LoadTextureFromImage(image);

    image = LoadImage("assets/textures/map/skills/vel_up.png");
    g->mapTextures[5] = LoadTextureFromImage(image);

    image = LoadImage("assets/textures/map/skills/bomb_distance.png");
    g->mapTextures[6] = LoadTextureFromImage(image);
    
    InitAudioDevice();

    g->explosionSound = LoadSound("assets/sounds/exp.wav");
    g->music = LoadMusicStream("assets/sounds/music.mp3");
}


void unloadAssets(game *g) {
    // Descarrega os assets
    UnloadTexture(g->bombTexture);
    UnloadTexture(g->explosionTexture);
    UnloadTexture(g->playerTexture);
    UnloadTexture(g->player2Texture);

    for (int i = 0; i < 9; i++) {
        UnloadTexture(g->mapTextures[i]);
    }

    UnloadSound(g->explosionSound);
    UnloadMusicStream(g->music);
    CloseAudioDevice();
}


// Atualiza o game
void updateGame(game *g, short int mapa[][NUMTILES_W]) {
    if (!gameTimer) gameTimer = GetTime();

    

    if (!g->player.life || !g->player2.life || (gameTimer && GetTime() - gameTimer > 120)) {
        currentScreen = GAMEOVERSCREEN;
        gameOverScreenTime = GetTime();
    }

    getLastKey((int[]){KEY_W, KEY_S, KEY_A, KEY_D}, &lastKeyP1);
    getLastKey((int[]){KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT}, &lastKeyP2);

    // Explode todas as bombas
    if (gameTimer && GetTime() - gameTimer > 115) {
        for (int i = 0; i < 10; i++) {
            if ((GetTime() - g->player.bombs[i].time) < 2.8) {
                g->player.bombs[i].time = GetTime() - 3;
            }
            if ((GetTime() - g->player2.bombs[i].time) < 2.8) {
                g->player2.bombs[i].time = GetTime() - 3;
            }
        }
    }

    // Trava a movimentação
    if (GetTime() - gameTimer < 115) {
        updateMove(&g->player2, (int[]){KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT}, lastKeyP2);
        updateMove(&g->player, (int[]){KEY_W, KEY_S, KEY_A, KEY_D}, lastKeyP1);
    }

    prevCollision(&g->player, mapa);
    prevCollision(&g->player2, mapa);

    if(currentMap == 0) {
        ClearBackground(DARKGREEN);
    } else if(currentMap == 1) {
        ClearBackground(BLACK);
    }

    BeginDrawing();

    drawMap(g, mapa);

    // Desenha e gerencia as bombas
    update_bomb(g, &g->player, mapa, KEY_SPACE);
    update_bomb(g, &g->player2, mapa, KEY_ENTER);

    // Desenha o player
    drawPlayers(g, lastKeyP1, lastKeyP2);


    float time = 120 - (GetTime() - gameTimer);

    DrawText(TextFormat("Time: %.1f", time), 0, 0, 15, WHITE);

    EndDrawing();
}


// Obtém a última tecla pressionada, para não andar em dois eixos ao mesmo tempo
void getLastKey(int key[], int *lastKey) {
    if (IsKeyPressed(key[0])) *lastKey = key[0];
    if (IsKeyPressed(key[2])) *lastKey = key[2];
    if (IsKeyPressed(key[1])) *lastKey = key[1];
    if (IsKeyPressed(key[3])) *lastKey = key[3];

    if (IsKeyReleased(key[0]) || IsKeyReleased(key[2]) || IsKeyReleased(key[1]) || IsKeyReleased(key[3])) {
        if (IsKeyDown(key[3]))
            *lastKey = key[3];
        else if (IsKeyDown(key[1]))
            *lastKey = key[1];
        else if (IsKeyDown(key[2]))
            *lastKey = key[2];
        else if (IsKeyDown(key[0]))
            *lastKey = key[0];
        else
            *lastKey = KEY_NULL;
    }
}