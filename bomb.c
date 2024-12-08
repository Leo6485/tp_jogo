#include "bomb.h"
#include "game.h"
#include <raylib.h>

// Inicia as bombas com os campos "isActive" e "collisionActive" igual a 0, para não pegar lixo de memória
void initBombs(PlayerObj *player, int n) {
    for (int i = 0; i < n; i++) {
        player->bombs[i].isActive = 0;
        player->bombs[i].collisionActive = 0;
        player->bombs[i].time = 0;
    }
}

// Desenha as bombas ativas
void draw_bomb(game *g, PlayerObj *player) {
    Texture2D bombTexture = g->bombTexture;
    Texture2D explosionTexture = g->explosionTexture;

    float frameSize = bombTexture.width / 3;
    for (int i = 0; i < 10; i++) {
        if (player->bombs[i].isActive == 1) {

            if (fabs(player->bombs[i].time - GetTime()) > 1 && fabs(player->bombs[i].time - GetTime()) < 2) PlaySound(g->explosionSound);

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

    draw_bomb(g, &g->player);
    draw_bomb(g, &g->player2);
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