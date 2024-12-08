#ifndef SCREENS_H
#define SCREENS_H

#include "header.h"

// Protótipos de funções relacionadas às telas
void gameOver(game *g, short int mapa[][NUMTILES_H][NUMTILES_W]);
void mainMenu(game *g);
void nameEdit(game *g);
void editText(char* text, Rectangle position);
void okButton(Rectangle pos, Color c1, Color c2, char text[], int key, int screen);
void updateMainMenu(game *g);
void drawMainMenu();

#endif
