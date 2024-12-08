#include "header.h"
#include "game.h"
#include "player.h"


void okButton(Rectangle pos, Color c1, Color c2, char text[], int key, int screen) {
    bool buttonHovered = CheckCollisionPointRec(GetMousePosition(), pos);
    Color buttonColor = buttonHovered ? c2 : c1;

    if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && buttonHovered) || (IsKeyPressed(key))) {
        currentScreen = screen;
    }

    // Gradiente
    DrawRectangleGradientEx(pos, buttonHovered ? GRAY : buttonColor, buttonHovered ? GRAY : DARKBLUE, buttonHovered ? GRAY : DARKBLUE, buttonHovered ? GRAY : buttonColor);

    
    DrawRectangleLinesEx(pos, 2, buttonHovered ? BLUE : WHITE);

    float textWidth = MeasureText(text, 20);
    float textX = pos.x + (pos.width - textWidth) / 2;
    DrawText(text, textX, pos.y + (pos.height - 20) / 2, 20, buttonHovered ? BLUE : WHITE);
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
    ClearBackground(DARKGREEN);

    okButton((Rectangle){SCREEN_W / 2 - 75, SCREEN_H / 2 - 150, 150, 50}, BLUE, GRAY, "Mapa 1", KEY_ONE, 1);
    okButton((Rectangle){SCREEN_W / 2 - 75, SCREEN_H / 2 - 80, 150, 50}, BLUE, GRAY, "Mapa 2", KEY_TWO, 2);
    okButton((Rectangle){SCREEN_W / 2 - 75, SCREEN_H / 2 - 10, 150, 50}, BLUE, GRAY, "Nomes", KEY_TAB, 3);

    EndDrawing();
}

void nameEdit(game *g) {
    // ClearBackground(WHITE);
    DrawRectangle(200, 50, 400, 350, GREEN);
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

        initGame(g, mapa);
        
        gameTimer = 0;

        currentScreen = MAINMENU;
    }

    EndDrawing();
}
