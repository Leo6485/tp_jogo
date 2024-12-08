#ifndef HEADER_H
#define HEADER_H

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
    Texture2D mapTextures[7];
    Sound explosionSound;
    Music music;
} game;

extern const short int sceneWidth;
extern const short int sceneHeight;
extern const Vector2 scene;

extern int currentScreen;
extern int currentMap;
extern int gameOverScreenTime;

extern int lastKeyP1;
extern int lastKeyP2;

extern Vector2Int pontos;
extern double gameTimer;

void drawMap(game *g, short int mapa[][NUMTILES_W]);
void initMap(short int mapa[][NUMTILES_H][NUMTILES_W]);
Vector2 getCoords(int x, int y);
Vector2Int getIndex(float x, float y);

void initPlayer(PlayerObj *player, Vector2 pos, char name[]);
void drawPlayers(game *g, int lastKeyP1, int lastKeyP2);
void updateMove(PlayerObj *player, int key[], int lastKeyP1);
void skillCollect(PlayerObj *player, short int mapa[][NUMTILES_W]);
void prevCollision(PlayerObj *player, short int mapa[][NUMTILES_W]);
void resolveCollision(PlayerObj *player, Vector2 coords);

void gameOver(game *g, short int mapa[][NUMTILES_H][NUMTILES_W]);
void mainMenu(game *g);
void nameEdit(game *g);
void editText(char* text, Rectangle position);
void okButton(Rectangle pos, Color c1, Color c2, char text[], int key, int screen);

void initBombs(PlayerObj *player, int n);
void draw_bomb(game *g, PlayerObj *player);
void update_bomb(game *g, PlayerObj *player, short int mapa[][NUMTILES_W], int key);
void bombDamage(game *g);
void bombCollision(PlayerObj *player, PlayerObj *player2, short int mapa[][NUMTILES_W]);
void bombToBombCollision(game *g);

void updateGame(game *g, short int mapa[][NUMTILES_W]);
void initGame(game *g, short int mapa[][NUMTILES_H][NUMTILES_W]);
void loadAssets(game *g);
void unloadAssets(game *g);
void getLastKey(int key[], int* lastKey);

#endif  // HEADER_H
