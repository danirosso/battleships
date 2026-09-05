#include <iostream>
using namespace std;

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION 
#include <raygui.h>

#define BLINE 12
#define BCOL 12

int mainMenu();
//Returns a number which represents the type of game that will be played
void setUpBoard(int line, int col, int gameBoard[BLINE][BCOL]);
//Gets the values on each tile of the gameBoard
void cursorSetUp (int startX, int startY, int step, int size, Rectangle *cursor);
//Initializes the cursor
void printBoard (int gamePhase, Rectangle visualBoard[BLINE][BCOL], int line, int col, int step, int sqSize);
//Prints the board differently depending on the state of the game
void moveCursor (int startX, int startY, int step, int size, Rectangle *cursor);
//Moves the position of the cursor around, returns where enter was pressed;

int main (){

    InitWindow(800, 600, "Batalha");
    SetTargetFPS(60);

    int gamePhase = 0; 

    int p1gameBoard[BLINE][BCOL];
    Rectangle visualBoard[BLINE][BCOL];
    const int step = 25;
    const int sqSize = 20;

    bool teste = false;
    Rectangle cursor;
    if(!teste) cursorSetUp(sqSize, sqSize, step, sqSize, &cursor);

    while(!WindowShouldClose()){
        BeginDrawing();

        if(gamePhase == 0){ 
            gamePhase = mainMenu();
        }

        ClearBackground(BLACK);

        if(gamePhase == 1){ 
            printBoard(gamePhase, visualBoard, BLINE, BCOL, step, sqSize);
            moveCursor(20, 20, step, sqSize, &cursor);
            DrawRectangleRec(cursor,GREEN);
        }

        EndDrawing(); 
    }

    CloseWindow(); 
}

void printBoard (int gamePhase, Rectangle visualBoard[BLINE][BCOL], int line, int col, int step, int sqSize){

    for (int i = 0; i < line; i++){    /* First we'll initialize the postition */
        for (int j = 0; j < col; j++){ /* of every square, than we'll draw them */
            visualBoard[i][j].x = sqSize + step*i; 
            visualBoard[i][j].y = sqSize + step*j;
            visualBoard[i][j].width = sqSize;
            visualBoard[i][j].height = visualBoard[i][j].width;
        }
    }

    for (int i = 0; i < line; i++){
        for (int j = 0; j < col; j++){
            DrawRectangleRec(visualBoard[i][j], GRAY);
        }
    }
}
 
void cursorSetUp (int startX, int startY, int step, int size, Rectangle *cursor){
    cursor->x = startX;
    cursor->y = startY;
    cursor->width = size;
    cursor->height = cursor->width;
}
 
void moveCursor (int startX, int startY, const int step, const int size, Rectangle *cursor){
    /*NGL, this took a while. */

    Rectangle cursorOld = *cursor; 

    if (IsKeyPressed(KEY_UP)) cursor->y -= step;
    if (IsKeyPressed(KEY_DOWN)) cursor->y += step;
    if (IsKeyPressed(KEY_LEFT)) cursor->x -= step;
    if (IsKeyPressed(KEY_RIGHT)) cursor->x += step;

    if (cursor->x < startX || cursor->x > startX + step * (BCOL-1)) cursor->x = cursorOld.x;
    if (cursor->y < startY || cursor->y > startY + step * (BLINE-1)) cursor->y = cursorOld.y;
     
    //if (cursorOld.x != cursor->x || cursor->y != cursorOld.y){
    //    DrawRectangleRec(cursorOld, RED);
    //}
}

int mainMenu(){

    bool gameStarted = false; 
    int chosenGameStyle = 0;

    if (!gameStarted){
        DrawText("Start playing!", 400, 20, 20, LIGHTGRAY);

        if (GuiButton((Rectangle){400, 200, 60, 20}, "1 player") || IsKeyDown(KEY_ENTER)){
            chosenGameStyle = 1; 
            gameStarted = true;
        }

        if (GuiButton((Rectangle){400, 550, 60, 20}, "Quit") || IsKeyDown(KEY_Q)){
            DrawText("Quit!", 600, 20, 40, RED);
            CloseWindow(); 
        }
    }
    return chosenGameStyle;
}
