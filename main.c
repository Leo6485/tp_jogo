#include "header.h"
#include "game.h"
#include "player.h"
#include "map.h"
#include "screens.h"
#include "bomb.h"
#include <raylib.h>

const short int sceneWidth = TILESIZE * NUMTILES_W;
const short int sceneHeight = TILESIZE * NUMTILES_H;

const Vector2 scene = {(SCREEN_W - sceneWidth) / 2, (SCREEN_H - sceneHeight) / 2};

int currentScreen = MAINMENU;
int currentMap = 0;
int gameOverScreenTime = 0;

int lastKeyP1 = KEY_NULL;
int lastKeyP2 = KEY_NULL;

Vector2Int pontos = {0, 0};

double gameTimer = 0;


// Função principal
int main() {
    srand(time(NULL));

    game g;
    short int mapa[2][NUMTILES_H][NUMTILES_W];

    initGame(&g, mapa);

    InitWindow(SCREEN_W, SCREEN_H, "Projeto");
    SetTargetFPS(60);

    loadAssets(&g);
    PlayMusicStream(g.music);

    while (!WindowShouldClose()) {
        UpdateMusicStream(g.music); // Atualiza a música

        if (currentScreen == 0) { // Main Menu
            mainMenu(&g);

        } else if (currentScreen == 3) { // Name Edit
            nameEdit(&g);

        } else if (currentScreen == 1) { // Mapa 1
            currentMap = 0;
            updateGame(&g, mapa[0]);

        } else if (currentScreen == 2) { // Mapa 2
            currentMap = 1;
            updateGame(&g, mapa[1]);

        } else if (currentScreen == 9) { // Game Over
            gameOver(&g, mapa);

        }
    }
    
    FILE *f = fopen("placar.txt", "w");
    
    fprintf(f, "%s : %d\n%s : %d", g.player.name, pontos.x, g.player2.name, pontos.y);
    fclose(f);

    unloadAssets(&g);

    CloseWindow();

    return 0;
}
