#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#define SCREEN_W 800
#define SCREEN_H 600
#define TILESIZE 40
#define NUMTILES_H 15
#define NUMTILES_W 15

#define MAINMENU 0
#define GAMESCREEN 1
#define GAMEOVERSCREEN 9

typedef struct Bomb {
    Rectangle pos;
    Rectangle explosion_right;
    Rectangle explosion_left;
    Rectangle explosion_up;
    Rectangle explosion_down;
    float indexTop;
    float indexRight;
    float indexBottom;
    float indexLeft;
    int isActive;
    int collisionActive;
    int time;
} Bomb;

typedef struct {
    Vector2 pos;
    float size;
    int life;
    float velX;
    float velY;
    float acc;
    float fric;
    int put_bomb;
    int num_bombs;
    int bomb_distance;
    float bomb_vel;
    Bomb bombs[10];
    char name[16];
} PlayerObj;

typedef struct {
    int x;
    int y;
} Vector2Int;

typedef struct {
    PlayerObj player;
    PlayerObj player2;
    Texture2D bombTexture;
    Texture2D explosionTexture;
    Texture2D playerTexture;
    Texture2D player2Texture;
    Texture mapTextures[10];
} game;

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

// Funções relacionadas ao mapa
void initRandomMap(short int mapa[][NUMTILES_W]);
void drawMap(game *g, short int mapa[][NUMTILES_W]);
void initMap(short int mapa[][NUMTILES_H][NUMTILES_W]);
Vector2 getCoords(int x, int y);
Vector2Int getIndex(float x, float y);

// Funções relacionadas ao jogador
void initPlayer(PlayerObj *player, Vector2 pos, char name[]);
void drawPlayers(game *g, int lastKeyP1, int lastKeyP2);
void updateMove(PlayerObj *player, int key[], int lastKeyP1);
void prevCollision(PlayerObj *player, short int mapa[][NUMTILES_W]);
void resolveCollision(PlayerObj *player, Vector2 coords);

// Funções do jogo
void updateGame(game *g, short int mapa[][NUMTILES_W]);
void getLastKey(int key[], int* lastKey);


// Telas
void gameOver(game *g, short int mapa[][NUMTILES_H][NUMTILES_W]);
void mainMenu(game *g);
void nameEdit(game *g);
void editText(char* text, Rectangle position);
void okButton(Rectangle pos, Color c1, Color c2, char text[], int key, int screen);
void updateMainMenu(game *g);
void drawMainMenu();

// Funções da bomba
void initBombs(PlayerObj *player, int n);
void draw_bomb(PlayerObj *player, Texture2D bombTexture, Texture2D explosionTexture);
void update_bomb(game *g, PlayerObj *player, short int mapa[][NUMTILES_W], int key);
void bombDamage(game *g);
void bombCollision(PlayerObj *player, PlayerObj *player2, short int mapa[][NUMTILES_W]);
void bombToBombCollision(game *g);

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

// Inicialização do jogador
void initPlayer(PlayerObj *player, Vector2 pos, char name[]) {
    player->pos = pos;
    player->size = TILESIZE;
    player->life = 1; 
    player->velX = 0.0;
    player->velY = 0.0;
    player->acc = 2;
    player->fric = 0.5;
    player->put_bomb = 0;
    player->num_bombs = 2;
    player->bomb_distance = 2;
    player->bomb_vel = 0.5;
    strcpy(player->name, name);
}

// Função principal
int main() {
    srand(time(NULL));
    game g;

    initPlayer(&g.player, (Vector2){scene.x, scene.y}, "MAGO\0");
    initPlayer(&g.player2, (Vector2){scene.x + (NUMTILES_W - 1) * TILESIZE, scene.y + (NUMTILES_H - 1) * TILESIZE}, "ESQUELETO\0");

    initBombs(&g.player, 10);
    initBombs(&g.player2, 10);

    short int mapa[2][NUMTILES_H][NUMTILES_W];
    initMap(mapa);

    InitWindow(SCREEN_W, SCREEN_H, "Projeto");
    SetTargetFPS(60);



    //Texturas
    Image image;
    image = LoadImage("assets/textures/bomb.png");
    g.bombTexture = LoadTextureFromImage(image);

    image = LoadImage("assets/textures/explosion.png");
    g.explosionTexture = LoadTextureFromImage(image);

    image = LoadImage("assets/textures/player.png");
    g.playerTexture = LoadTextureFromImage(image);

    image = LoadImage("assets/textures/player2.png");
    g.player2Texture = LoadTextureFromImage(image);


    //Texturas do mapa
    image = LoadImage("assets/textures/map/tileset.png");
    g.mapTextures[0] = LoadTextureFromImage(image);

    image = LoadImage("assets/textures/map/Decorations.png");
    g.mapTextures[1] = LoadTextureFromImage(image);

    image = LoadImage("assets/textures/map/map2_floor.png");
    g.mapTextures[2] = LoadTextureFromImage(image);

    image = LoadImage("assets/textures/map/box.png");
    g.mapTextures[3] = LoadTextureFromImage(image);

    image = LoadImage("assets/textures/map/skills/max_bombs.png");
    g.mapTextures[4] = LoadTextureFromImage(image);

    image = LoadImage("assets/textures/map/skills/vel_up.png");
    g.mapTextures[5] = LoadTextureFromImage(image);

    image = LoadImage("assets/textures/map/skills/bomb_distance.png");
    g.mapTextures[6] = LoadTextureFromImage(image);

    image = LoadImage("assets/textures/map/tileset2.png");
    g.mapTextures[9] = LoadTextureFromImage(image);

    while (!WindowShouldClose()) {

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


    UnloadTexture(g.bombTexture);
    UnloadTexture(g.playerTexture);
    UnloadTexture(g.player2Texture);

    for(int i = 0; i < 10; i++) {
        UnloadTexture(g.mapTextures[i]);
    }

    CloseWindow();

    return 0;
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

void drawPlayers(game *g, int lastKeyP1, int lastKeyP2) {
    Vector2 frameSize = {32, 32};

    int animP1 = 0;
    int animP2 = 0;

    static Vector2Int frameP1 = {0, 0};
    static Vector2Int frameP2 = {0, 0};

    if (lastKeyP1 == KEY_W) {
        frameP1.y = 3; // Altera a posição y no sprite atual
        animP1 = 1;
    } else if (lastKeyP1 == KEY_S) {
        frameP1.y = 0;
        animP1 = 1;
    } else if (lastKeyP1 == KEY_A) {
        frameP1.y = 1;
        animP1 = 1;
    } else if (lastKeyP1 == KEY_D) {
        frameP1.y = 2;
        animP1 = 1;
    }

    if (lastKeyP2 == KEY_UP) {
        frameP2.y = 3;
        animP2 = 1;
    } else if (lastKeyP2 == KEY_DOWN) {
        frameP2.y = 0;
        animP2 = 1;
    } else if (lastKeyP2 == KEY_LEFT) {
        frameP2.y = 1;
        animP2 = 1;
    } else if (lastKeyP2 == KEY_RIGHT) {
        frameP2.y = 2;
        animP2 = 1;
    }

    if (animP1) {
        frameP1.x = 2 - fabs(((int)(GetTime() * 10) % 4) - 2);  // Gera um intervalo que oscila entre 0 e 2
    } else {
        frameP1.x = 1;
    }

    if (animP2) {
        frameP2.x = 2 - fabs(((int)(GetTime() * 10) % 4) - 2);
    } else {
        frameP2.x = 1;
    }

    Rectangle frameRecP1 = {frameP1.x * frameSize.x, frameP1.y * frameSize.y, frameSize.x, frameSize.y};
    DrawTexturePro(g->playerTexture, frameRecP1, (Rectangle){g->player.pos.x, g->player.pos.y, TILESIZE, TILESIZE},
                    (Vector2){0, 0}, 0.0f, WHITE);

    Rectangle frameRecP2 = {frameP2.x * frameSize.x, frameP2.y * frameSize.y, frameSize.x, frameSize.y};
    DrawTexturePro(g->player2Texture, frameRecP2, (Rectangle){g->player2.pos.x, g->player2.pos.y, TILESIZE, TILESIZE},
                    (Vector2){0, 0}, 0.0f, WHITE);
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

    //Explode todas as bombas
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

    //Trava a movimentaçã
    if (GetTime() - gameTimer < 115) {
        updateMove(&g->player2, (int[]){KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT}, lastKeyP2);
        updateMove(&g->player, (int[]){KEY_W, KEY_S, KEY_A, KEY_D}, lastKeyP1);
    }

    prevCollision(&g->player, mapa);
    prevCollision(&g->player2, mapa);

    ClearBackground(BLACK);

    BeginDrawing();

    DrawRectangle(scene.x, scene.y, sceneWidth, sceneHeight, GREEN);

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

// Move o player como uma física baseada em aceleração e atrito
void updateMove(PlayerObj *player, int key[], int lastKey) {
    if (lastKey == key[0] && player->pos.y > scene.y)
        player->velY -= player->acc;
    else if (player->pos.y <= scene.y)
        player->pos.y = scene.y;

    if (lastKey == key[1] && player->pos.y < sceneHeight + scene.y - player->size)
        player->velY += player->acc;
    else if (player->pos.y >= sceneHeight + scene.y - player->size)
        player->pos.y = sceneHeight + scene.y - player->size;

    if (lastKey == key[2] && player->pos.x >= scene.x)
        player->velX -= player->acc;
    else if (player->pos.x < scene.x)
        player->pos.x = 0 + scene.x;

    if (lastKey == key[3] && player->pos.x <= sceneWidth + scene.x - player->size)
        player->velX += player->acc;
    else if (player->pos.x >= sceneWidth + scene.x - player->size)
        player->pos.x = sceneWidth + scene.x - player->size;

    player->pos.x += player->velX;
    player->pos.y += player->velY;

    player->velX *= player->fric;
    player->velY *= player->fric;
}

// Inicia as bombas com os campos "isActive" e "collisionActive" igual a 0, para não pegar lixo de memória
void initBombs(PlayerObj *player, int n) {
    for (int i = 0; i < n; i++) {
        player->bombs[i].isActive = 0;
        player->bombs[i].collisionActive = 0;
        player->bombs[i].time = 0;
    }
}

// Desenha as bombas ativas
void draw_bomb(PlayerObj *player, Texture2D bombTexture, Texture2D explosionTexture) {
    float frameSize = bombTexture.width / 3;
    for (int i = 0; i < 10; i++) {
        if (player->bombs[i].isActive == 1) {

            if(fabs(player->bombs[i].time - GetTime()) < 3) {
                // Frame que oscila no intervalo 0 e 2
                int currentFrame = 2 - fabs(((int)(GetTime() * 2) % 4) - 2);
                Rectangle frameRec = {currentFrame * frameSize, 0, frameSize, bombTexture.height};
                DrawTexturePro(bombTexture, frameRec, (Rectangle){player->bombs[i].pos.x, player->bombs[i].pos.y, TILESIZE, TILESIZE},
                                (Vector2){0, 0}, 0.0f, WHITE);

            }

            if (fabs(player->bombs[i].time - GetTime()) > 3 && fabs(player->bombs[i].time - GetTime()) < 4) {

                // Obtêm os frame x e y da grade 4x4
                Vector2Int currentFrame = {(int)((GetTime() - player->bombs[i].time - 3) * 16) % 4, (int)((GetTime() - player->bombs[i].time - 3) * 16) / 4};
                Rectangle frameRec = {currentFrame.x * 64, currentFrame.y * 64, 64, 64};

                int indexRight = player->bombs[i].explosion_right.width / TILESIZE;
                int indexLeft = player->bombs[i].explosion_left.width / TILESIZE;
                int indexUp = player->bombs[i].explosion_up.height / TILESIZE;
                int indexDown = player->bombs[i].explosion_down.height / TILESIZE;

                // Explosão para a direção direita
                for(int j = 0; j < indexRight; j++) {
                    DrawTexturePro(explosionTexture, frameRec, (Rectangle){player->bombs[i].pos.x + j * TILESIZE, player->bombs[i].pos.y, TILESIZE, TILESIZE}, (Vector2){0, 0}, 0.0f, WHITE);
                }

                // Explosão para a direção esquerda
                for(int j = indexLeft - 1; j > 0 ; j--) {
                    DrawTexturePro(explosionTexture, frameRec, (Rectangle){player->bombs[i].pos.x - j * TILESIZE, player->bombs[i].pos.y, TILESIZE, TILESIZE}, (Vector2){0, 0}, 0.0f, WHITE);
                }

                // Explosão para a direção para cima
                for(int j = indexUp - 1; j > 0; j--) {
                    DrawTexturePro(explosionTexture, frameRec, (Rectangle){player->bombs[i].pos.x, player->bombs[i].pos.y - j * TILESIZE, TILESIZE, TILESIZE}, (Vector2){0, 0}, 0.0f, WHITE);
                }

                // Explosão para a direção para baixo
                for(int j = 0; j < indexDown; j++) {
                    DrawTexturePro(explosionTexture, frameRec, (Rectangle){player->bombs[i].pos.x, player->bombs[i].pos.y + j * TILESIZE, TILESIZE, TILESIZE}, (Vector2){0, 0}, 0.0f, WHITE);
                }
            }
        }
    }
}
// Causa dano ao player caso seja atingido pela bomba
void bombDamage(game *g) {
    short int CollisionP1 = 0;
    short int CollisionP2 = 0;

    Rectangle playerRec = {g->player.pos.x, g->player.pos.y, g->player.size, g->player.size};
    Rectangle player2Rec = {g->player2.pos.x, g->player2.pos.y, g->player2.size, g->player2.size};

    for (int i = 0; i < 10; i++) {
        if (g->player.bombs[i].isActive == 1) {

            if (fabs(g->player.bombs[i].time - GetTime()) > 3 && fabs(g->player.bombs[i].time - GetTime()) < 4) {

                CollisionP1 += CheckCollisionRecs(g->player.bombs[i].explosion_right, playerRec);
                CollisionP1 += CheckCollisionRecs(g->player.bombs[i].explosion_left, playerRec);
                CollisionP1 += CheckCollisionRecs(g->player.bombs[i].explosion_up, playerRec);
                CollisionP1 += CheckCollisionRecs(g->player.bombs[i].explosion_down, playerRec);

                CollisionP2 += CheckCollisionRecs(g->player.bombs[i].explosion_right, player2Rec);
                CollisionP2 += CheckCollisionRecs(g->player.bombs[i].explosion_left, player2Rec);
                CollisionP2 += CheckCollisionRecs(g->player.bombs[i].explosion_up, player2Rec);
                CollisionP2 += CheckCollisionRecs(g->player.bombs[i].explosion_down, player2Rec);
            }
        }
    }

    for (int i = 0; i < 10; i++) {
        if (g->player2.bombs[i].isActive == 1) {

            if (fabs(g->player2.bombs[i].time - GetTime()) > 3 && fabs(g->player2.bombs[i].time - GetTime()) < 4) {

                CollisionP1 += CheckCollisionRecs(g->player2.bombs[i].explosion_right, playerRec);
                CollisionP1 += CheckCollisionRecs(g->player2.bombs[i].explosion_left, playerRec);
                CollisionP1 += CheckCollisionRecs(g->player2.bombs[i].explosion_up, playerRec);
                CollisionP1 += CheckCollisionRecs(g->player2.bombs[i].explosion_down, playerRec);

                CollisionP2 += CheckCollisionRecs(g->player2.bombs[i].explosion_right, player2Rec);
                CollisionP2 += CheckCollisionRecs(g->player2.bombs[i].explosion_left, player2Rec);
                CollisionP2 += CheckCollisionRecs(g->player2.bombs[i].explosion_up, player2Rec);
                CollisionP2 += CheckCollisionRecs(g->player2.bombs[i].explosion_down, player2Rec);
            }
        }
    }

    if (CollisionP1) {
        g->player.life = 0;
    }
    if (CollisionP2) {
        g->player2.life = 0;
    }
}

// Ativa a colisão de entidades com a bomba, após o player sair de "dentro" dela
void bombCollision(PlayerObj *player, PlayerObj *player2, short int mapa[][NUMTILES_W]) {
    for (int i = 0; i < 10; i++) {
        if (player->bombs[i].isActive == 1) {
            if (player->bombs[i].collisionActive == 0) {

                // Se a colisão estiver desativada e o player ter saído de "dentro" da bomba, ative a colisão colocando um bloco "3" na matriz do mapa
                if (!CheckCollisionRecs(player->bombs[i].pos, (Rectangle){player->pos.x, player->pos.y, player->size, player->size})) {
                    Vector2Int bombIndex = getIndex(player->bombs[i].pos.x, player->bombs[i].pos.y);
                    player->bombs[i].collisionActive = 1;
                }
            } else {
                resolveCollision(player, (Vector2){player->bombs[i].pos.x, player->bombs[i].pos.y});
                resolveCollision(player2, (Vector2){player->bombs[i].pos.x, player->bombs[i].pos.y});
            }
        }
    }
}

void bombToBombCollision(game *g) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (g->player.bombs[i].isActive == 1) {

                if (fabs(g->player.bombs[i].time - GetTime()) > 3 && fabs(g->player.bombs[i].time - GetTime()) < 4) {

                    short int CollisionP1 = 0;
                    short int CollisionP2 = 0;

                    Rectangle bombP1 = g->player.bombs[j].pos;
                    Rectangle bombP2 = g->player2.bombs[j].pos;

                    CollisionP1 += CheckCollisionRecs(g->player.bombs[i].explosion_right, bombP1);
                    CollisionP1 += CheckCollisionRecs(g->player.bombs[i].explosion_left, bombP1);
                    CollisionP1 += CheckCollisionRecs(g->player.bombs[i].explosion_up, bombP1);
                    CollisionP1 += CheckCollisionRecs(g->player.bombs[i].explosion_down, bombP1);

                    CollisionP2 += CheckCollisionRecs(g->player.bombs[i].explosion_right, bombP2);
                    CollisionP2 += CheckCollisionRecs(g->player.bombs[i].explosion_left, bombP2);
                    CollisionP2 += CheckCollisionRecs(g->player.bombs[i].explosion_up, bombP2);
                    CollisionP2 += CheckCollisionRecs(g->player.bombs[i].explosion_down, bombP2);

                    if (CollisionP1 && (GetTime() - g->player.bombs[j].time) < 2.8) {
                        g->player.bombs[j].time = GetTime() - 3;
                    }

                    if (CollisionP2 && (GetTime() - g->player2.bombs[j].time) < 2.8) {
                        g->player2.bombs[j].time = GetTime() - 3;
                    }
                }
            }
        }
    }

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (g->player2.bombs[i].isActive == 1) {

                if (fabs(g->player2.bombs[i].time - GetTime()) > 3 && fabs(g->player2.bombs[i].time - GetTime()) < 4) {

                    short int CollisionP1 = 0;
                    short int CollisionP2 = 0;

                    Rectangle bombP1 = g->player.bombs[j].pos;
                    Rectangle bombP2 = g->player2.bombs[j].pos;

                    CollisionP1 += CheckCollisionRecs(g->player2.bombs[i].explosion_right, bombP1);
                    CollisionP1 += CheckCollisionRecs(g->player2.bombs[i].explosion_left, bombP1);
                    CollisionP1 += CheckCollisionRecs(g->player2.bombs[i].explosion_up, bombP1);
                    CollisionP1 += CheckCollisionRecs(g->player2.bombs[i].explosion_down, bombP1);

                    CollisionP2 += CheckCollisionRecs(g->player2.bombs[i].explosion_right, bombP2);
                    CollisionP2 += CheckCollisionRecs(g->player2.bombs[i].explosion_left, bombP2);
                    CollisionP2 += CheckCollisionRecs(g->player2.bombs[i].explosion_up, bombP2);
                    CollisionP2 += CheckCollisionRecs(g->player2.bombs[i].explosion_down, bombP2);

                    if (CollisionP1 && (GetTime() - g->player.bombs[j].time) < 2.8) {
                        g->player.bombs[j].time = GetTime() - 3;
                    }

                    if (CollisionP2 && (GetTime() - g->player2.bombs[j].time) < 2.8) {
                        g->player2.bombs[j].time = GetTime() - 3;
                    }
                }
            }
        }
    }
}
// Spawna, explode e desativa as bombas
void update_bomb(game *g, PlayerObj *player, short int mapa[][NUMTILES_W], int key) {

    draw_bomb(&g->player, g->bombTexture, g->explosionTexture);
    draw_bomb(&g->player2, g->bombTexture, g->explosionTexture);
    bombDamage(g);
    bombCollision(&g->player, &g->player2, mapa);
    bombCollision(&g->player2, &g->player, mapa);
    bombToBombCollision(g);

    if (IsKeyPressed(key)) {
        player->put_bomb = 1;
    }

    // Adiciona uma bomba no mesmo índice do player
    if (player->put_bomb == 1) {
        for (int i = 0; i < player->num_bombs; i++) {
            if (player->bombs[i].isActive == 0) {
                player->bombs[i].isActive = 1;

                Vector2Int index = getIndex(player->pos.x, player->pos.y);
                Vector2 coords = getCoords(index.x, index.y);

                player->bombs[i].pos = (Rectangle){coords.x, coords.y, TILESIZE, TILESIZE};
                player->bombs[i].explosion_right = player->bombs[i].pos;
                player->bombs[i].explosion_left = player->bombs[i].pos;
                player->bombs[i].explosion_down = player->bombs[i].pos;
                player->bombs[i].explosion_up = player->bombs[i].pos;

                player->bombs[i].time = GetTime();

                Vector2Int bombIndex = getIndex(player->bombs[i].pos.x, player->bombs[i].pos.y);

                player->bombs[i].indexBottom = bombIndex.y;
                player->bombs[i].indexRight = bombIndex.x;
                player->bombs[i].indexTop = bombIndex.y;
                player->bombs[i].indexLeft = bombIndex.x;

                break;
            }
        }
    }

    for (int i = 0; i < 10; i++) {
        if (player->bombs[i].isActive == 1) {
            if (fabs(player->bombs[i].time - GetTime()) > 3 && fabs(player->bombs[i].time - GetTime()) < 4) {
                int grow_tax = player->bomb_distance;

                Vector2Int bombIndex = getIndex(player->bombs[i].pos.x, player->bombs[i].pos.y);

                // Right
                if (player->bombs[i].explosion_right.width < (player->bomb_distance + 1) * TILESIZE) {
                    float indexRight = player->bombs[i].indexRight;

                    int currentBlock = mapa[(int)bombIndex.y][(int)(indexRight + 1)];

                    if (currentBlock != 2 && currentBlock != 1 && (indexRight + 1) < NUMTILES_W
                        && (int)(indexRight + 1) < NUMTILES_W) {
                        player->bombs[i].indexRight += player->bomb_vel;
                        player->bombs[i].explosion_right.width += TILESIZE * player->bomb_vel;
                    }
                }

                // Left
                if (player->bombs[i].explosion_left.width < (player->bomb_distance + 1) * TILESIZE) {
                    float indexLeft = player->bombs[i].indexLeft;

                    int currentBlock = mapa[(int)bombIndex.y][(int)(indexLeft - player->bomb_vel)];

                    if (currentBlock != 2 && currentBlock != 1 && indexLeft - player->bomb_vel >= 0
                        && (int)(indexLeft - player->bomb_vel) >= 0) {
                        player->bombs[i].indexLeft -= player->bomb_vel;
                        player->bombs[i].explosion_left.width += TILESIZE * player->bomb_vel;
                        player->bombs[i].explosion_left.x -= TILESIZE * player->bomb_vel;
                    }
                }

                // Top
                if (player->bombs[i].explosion_up.height < (player->bomb_distance + 1) * TILESIZE) {
                    float indexTop = player->bombs[i].indexTop;

                    int currentBlock = mapa[(int)(indexTop - player->bomb_vel)][(int)bombIndex.x];

                    if (currentBlock != 2 && currentBlock != 1 && indexTop - player->bomb_vel >= 0
                        && (int)(indexTop - player->bomb_vel) >= 0) {
                        player->bombs[i].indexTop -= player->bomb_vel;
                        player->bombs[i].explosion_up.height += TILESIZE * player->bomb_vel;
                        player->bombs[i].explosion_up.y -= TILESIZE * player->bomb_vel;
                    }
                }
                // Bottom
                if (player->bombs[i].explosion_down.height < (player->bomb_distance + 1) * TILESIZE) {
                    float indexBottom = player->bombs[i].indexBottom;

                    int currentBlock = mapa[(int)(indexBottom + 1)][(int)bombIndex.x];

                    if (currentBlock != 2 && currentBlock != 1 && (indexBottom + 1) < NUMTILES_H
                        && (int)(indexBottom + 1) < NUMTILES_H) {
                        player->bombs[i].indexBottom += player->bomb_vel;
                        player->bombs[i].explosion_down.height += TILESIZE * player->bomb_vel;
                    }
                }
            } else if (fabs(player->bombs[i].time - GetTime()) > 3) {

                Vector2Int bombIndex = getIndex(player->bombs[i].pos.x, player->bombs[i].pos.y);
                int y = bombIndex.y;
                int x = bombIndex.x;

                short int *mapaRight = &mapa[y][(int)(player->bombs[i].indexRight) + 1];
                short int *mapaLeft = &mapa[y][(int)(player->bombs[i].indexLeft) - 1];
                short int *mapaTop = &mapa[(int)(player->bombs[i].indexTop) - 1][x];
                short int *mapaBottom = &mapa[(int)(player->bombs[i].indexBottom + 1)][x];

                if((int)(player->bombs[i].indexRight) < NUMTILES_W - 1)
                    if (*mapaRight == 2 && abs(x - (int)player->bombs[i].indexRight) < player->bomb_distance) {
                        int r = rand() % 7; // 4, 5, e 6 são blocos especiais
                        r = r < 4 ? 0 : r;
                        *mapaRight = r;
                    }

                if((int)(player->bombs[i].indexLeft) - 1 >= 0)
                    if (*mapaLeft == 2 && abs(x - (int)player->bombs[i].indexLeft) < player->bomb_distance) {
                        int r = rand() % 7;
                        r = r < 4 ? 0 : r;
                        *mapaLeft = r;
                    }
                
                if((int)(player->bombs[i].indexTop) - 1 >= 0)
                    if (*mapaTop == 2 && abs(y - (int)player->bombs[i].indexTop) < player->bomb_distance) {
                        int r = rand() % 7;
                        r = r < 4 ? 0 : r;
                        *mapaTop = r;
                    }
                
                if((int)(player->bombs[i].indexBottom) < NUMTILES_H - 1)
                if (*mapaBottom == 2 && abs(y - (int)player->bombs[i].indexBottom) < player->bomb_distance) {
                    int r = rand() % 7;
                    r = r < 4 ? 0 : r;
                    *mapaBottom = r;
                }

                player->bombs[i].collisionActive = 0;
                player->bombs[i].isActive = 0;
            }
        }
    }
    player->put_bomb = 0;
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
                mapa[h][i][j] = 2;
            } else {
                mapa[h][i][j] = 0;
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

// Gerencia as colisões do player
void skillCollect(PlayerObj *player, short int mapa[][NUMTILES_W]) {
    Vector2Int playerIndex = getIndex(player->pos.x, player->pos.y);

    for (int y = playerIndex.y - 1; y <= playerIndex.y + 1; y++) {
        for (int x = playerIndex.x - 1; x <= playerIndex.x + 1; x++) { // Obtém os blocos em torno do player

            if (x >= 0 && x < NUMTILES_W && y >= 0 && y < NUMTILES_H) {
                if (mapa[y][x] == 4) {
                    mapa[y][x] = 0;
                    if(player->num_bombs < 10)player->num_bombs += 1;
                }
                if (mapa[y][x] == 5) {
                    mapa[y][x] = 0;
                    if (player->acc < 3.5) player->acc += 0.25;
                }
                if (mapa[y][x] == 6) {
                    mapa[y][x] = 0;
                    if(player->bomb_distance < NUMTILES_H)player->bomb_distance += 1;
                }
                if (mapa[y-1][x-1] == 7) {
                    player->acc = 0.5;
                } else {
                    player->acc = 2;
                }
            }
        }
    }
}
void prevCollision(PlayerObj *player, short int mapa[][NUMTILES_W]) {
    skillCollect(player, mapa);

    Vector2Int playerIndex = getIndex(player->pos.x, player->pos.y);

    for (int y = playerIndex.y - 1; y <= playerIndex.y + 1; y++) {
        for (int x = playerIndex.x - 1; x <= playerIndex.x + 1; x++) {
            if (x >= 0 && x < NUMTILES_W && y >= 0 && y < NUMTILES_H) {
                if (mapa[y][x] == 2 || mapa[y][x] == 1 || mapa[y][x] == 3) {
                    Vector2 coords = getCoords(x, y);
                    resolveCollision(player, coords);
                }
            }
        }
    }
}

void resolveCollision(PlayerObj *player, Vector2 coords) {
    int playerLeft = player->pos.x;
    int playerRight = player->pos.x + player->size;
    int playerTop = player->pos.y;
    int playerBottom = player->pos.y + player->size;

    int tileLeft = coords.x;
    int tileRight = coords.x + TILESIZE;
    int tileTop = coords.y;
    int tileBottom = coords.y + TILESIZE;

    Rectangle playerRect = {player->pos.x, player->pos.y, player->size, player->size};
    Rectangle tileRect = {coords.x, coords.y, TILESIZE, TILESIZE};

    if (CheckCollisionRecs(playerRect, tileRect)) {
        int tolerance = TILESIZE - 10;
        int move_on_collision = 2.8;

        // Top
        int top_dist = tileTop - playerTop;
        if (top_dist >= tolerance) {
            player->pos.y = tileTop - player->size;
        } else if (top_dist < tolerance && top_dist > 5) {
            player->pos.y -= move_on_collision;
        }

        // Left
        int left_dist = tileLeft - playerLeft;
        if (left_dist >= tolerance) {
            player->pos.x = tileLeft - player->size;
        } else if (left_dist < tolerance && left_dist > 5) {
            player->pos.x -= move_on_collision;
        }

        // Bottom
        int bottom_dist = playerBottom - tileBottom;
        if (bottom_dist >= tolerance) {
            player->pos.y = tileBottom;
        } else if (bottom_dist < tolerance && bottom_dist > 5) {
            player->pos.y += move_on_collision;
        }

        // Right
        int right_dist = playerRight - tileRight;
        if (right_dist >= tolerance) {
            player->pos.x = tileRight;
        } else if (right_dist < tolerance && right_dist > 5) {
            player->pos.x += move_on_collision;
        }
    }
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

// Cria um botão funcional
void okButton(Rectangle pos, Color c1, Color c2, char text[], int key, int screen) {
    bool buttonHovered = CheckCollisionPointRec(GetMousePosition(), pos);
    Color buttonColor = buttonHovered ? c2 : c1;

    if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && buttonHovered) ||
        (IsKeyPressed(key) && !buttonHovered)) {
        currentScreen = screen;
    }

    float textWidth = MeasureText(text, 20);
    float textX = pos.x + (pos.width - textWidth) / 2;
    DrawRectangleRec(pos, buttonColor);
    DrawText(text, textX, pos.y + (pos.height - 20) / 2, 20, WHITE);
}

// Cria um editText funcional
void editText(char *text, Rectangle position) {
    int buttonHovered = CheckCollisionPointRec(GetMousePosition(), position);
    Color color_edit_text = buttonHovered ? RED : GRAY;

    if (CheckCollisionPointRec(GetMousePosition(), position)) {
        int key = GetKeyPressed();
        if (key) {
            if ((key >= 32) && (key <= 125) && (strlen(text) < 15)) {
                int len = strlen(text);
                text[len] = (char)key;
                text[len + 1] = '\0';
            } else if ((key == KEY_BACKSPACE) && (strlen(text) > 0)) {
                text[strlen(text) - 1] = '\0';
            }
        }
    }

    float textWidth = MeasureText(text, 20);
    float textX = position.x + (position.width - textWidth) / 2;

    DrawRectangleLinesEx(position, 2, color_edit_text);
    DrawText(text, textX, position.y + (position.height - 20) / 2, 20, color_edit_text);
}

void mainMenu(game *g) {
    currentMap = 0;
    BeginDrawing();
    ClearBackground(WHITE);
    DrawText("Bomb SUS", SCREEN_W / 2 - 125, 55, 50, BLUE);

    okButton((Rectangle){SCREEN_W / 2 - 75, SCREEN_H / 2 - 150, 150, 50}, BLUE, GRAY, "Mapa 1", KEY_ONE, 1);
    okButton((Rectangle){SCREEN_W / 2 - 75, SCREEN_H / 2 - 80, 150, 50}, BLUE, GRAY, "Mapa 2", KEY_TWO, 2);
    okButton((Rectangle){SCREEN_W / 2 - 75, SCREEN_H / 2 - 10, 150, 50}, BLUE, GRAY, "Nomes", KEY_TAB, 3);

    EndDrawing();
}

void nameEdit(game *g) {
    // ClearBackground(WHITE);
    DrawRectangle(200, 50, 400, 350, WHITE);
    DrawRectangleLinesEx((Rectangle){200, 50, 400, 350}, 5, GRAY);
    BeginDrawing();

    editText(g->player.name, (Rectangle){SCREEN_W / 2 - 75, SCREEN_H / 2 - 150, 150, 50});
    editText(g->player2.name, (Rectangle){SCREEN_W / 2 - 75, SCREEN_H / 2 - 80, 150, 50});

    okButton((Rectangle){SCREEN_W / 2 - 75, SCREEN_H / 2 - 10, 150, 50}, BLUE, GRAY, "Ok", KEY_ENTER, 0);

    EndDrawing();
}

void gameOver(game *g, short int mapa[][NUMTILES_H][NUMTILES_W]) {
    ClearBackground(BLACK);
    BeginDrawing();

    if ((g->player.life + g->player2.life) != 1) {
        DrawText("EMPATE", SCREEN_W / 2 - MeasureText("EMPATE", 25) / 2, 300, 25, WHITE);
    } else if (g->player.life) {
        char text[24];
        sprintf(text, "%s GANHOU!", g->player.name);
        float textWidth = MeasureText(text, 25);
        DrawText(text, SCREEN_W / 2 - textWidth / 2, 300, 25, WHITE);
    } else if (g->player2.life) {
        char text[24];
        sprintf(text, "%s GANHOU!", g->player2.name);
        float textWidth = MeasureText(text, 25);
        DrawText(text, SCREEN_W / 2 - textWidth / 2, 300, 25, WHITE);
    }

    if (fabs(GetTime() - gameOverScreenTime) > 2.5) {
        if (g->player.life && !g->player2.life)
            pontos.x++;
        else if (g->player2.life && !g->player.life)
            pontos.y++;
        
        initMap(mapa);
        initPlayer(&g->player, (Vector2){scene.x, scene.y}, g->player.name);
        initPlayer(&g->player2, (Vector2){scene.x + (NUMTILES_W - 1) * TILESIZE, scene.y + (NUMTILES_H - 1) * TILESIZE}, g->player2.name);
        initMap(mapa);
        
        gameTimer = 0;

        currentScreen = MAINMENU;
    }

    EndDrawing();
}
