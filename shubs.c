#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION 
#include <raygui.h>

#define BLINE 12
#define BCOL 12

int mainMenu();
//returns a number which represents the type of game that will be played
void setUpBoard(int line, int col, int gameBoard[line][col]);
//gets the values on each tile of the gameBoard

int main (){

    InitWindow(800, 600, "Batalha");
    SetTargetFPS(60);

    int gameStyle = 0; 
    int p1gameBoard[BLINE][BCOL];

    while(!WindowShouldClose()){
        BeginDrawing();

        if (gameStyle == 0){ 
            gameStyle = mainMenu();
        }
         
        switch (gameStyle){
            case 1: setUpBoard(BLINE, BCOL, p1gameBoard); break;
        }

        
        EndDrawing(); 
    }

    CloseWindow(); 
}


void setUpBoard(int line, int col, int gameBoard[line][col]){
     
    Rectangle visualBoard[BLINE][BCOL];

    for (int i = 0; i < line; i++){    /* First we'll initialize the postition */
        for (int j = 0; j < col; j++){ /* of every square, than we'll draw them */
            visualBoard[i][j].x = 20 + 30*i; 
            visualBoard[i][j].y = 20 + 30*j;
            visualBoard[i][j].width = 20;
            visualBoard[i][j].height = 20;
        }
    }
     
    for (int i = 0; i < line; i++){
        for (int j = 0; j < col; j++){
            if (GuiButton(visualBoard[i][j], ""));
        }
    }
}

int mainMenu(){

    bool gameStarted = false; 
    int chosenGameStyle = 0;

    if (!gameStarted){
        DrawText("Start playing!", 400, 20, 20, LIGHTGRAY);

        if (GuiButton((Rectangle){400, 200, 60, 20}, "1 player") || IsKeyPressed(KEY_ENTER)){
            DrawText("Game start!", 600, 20, 40, RED);
            chosenGameStyle = 1; 
            gameStarted = true;
        }

        if (GuiButton((Rectangle){400, 550, 60, 20}, "Quit") || IsKeyPressed(KEY_Q)){
            DrawText("Quit!", 600, 20, 40, RED);
            CloseWindow(); 
        }
    }
    return chosenGameStyle;
}
