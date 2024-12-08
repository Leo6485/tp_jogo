#include "header.h"
#include "game.h"
#include "player.h"

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
    player->num_bombs = 1;
    player->bomb_distance = 2;
    player->bomb_vel = 0.5;
    strcpy(player->name, name);
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
