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
Rectangle cursorSetUp (int startX, int startY, int step, int size);
//Draws the cursor and moves it around, returns the coordinates chosen.
void printBoard (int gameStyle);
//Prints the board differently depending on the state of the game



int main (){

    InitWindow(800, 600, "Batalha");
    SetTargetFPS(60);

    int gameStyle = 0; 
    int p1gameBoard[BLINE][BCOL];
    Rectangle visualBoard[BLINE][BCOL];

    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(BLACK);

        if (gameStyle == 0){ 
            gameStyle = mainMenu();
        }
        
        PrintBoard(1);

        EndDrawing(); 
    }

    CloseWindow(); 
}

void printBoard (int gameStyle, int gameBoard, int visualBoard){
    const int step = 25;
    const int sqSize = 20;

    for (int i = 0; i < line; i++){    /* First we'll initialize the postition */
        for (int j = 0; j < col; j++){ /* of every square, than we'll draw them */
            visualBoard[i][j].x = sqSize + step*i; 
            visualBoard[i][j].y = sqSize + step*j;
            visualBoard[i][j].width = sqSize;
            visualBoard[i][j].height = sqSize;
        }
    }

    for (int i = 0; i < line; i++){
        for (int j = 0; j < col; j++){
            DrawRectangleRec(visualBoard[i][j], GRAY);
        }
    }


bool setUpBoard(int line, int col, int gameBoard[BLINE][BCOL]){

    double frame = GetFrameTime();
    if (frame > 1.0f) {
        Rectangle answer = cursorSetUp(visualBoard[0][0].x, visualBoard[0][0].y, step, sqSize);
        DrawRectangleRec(answer, YELLOW);
        frame = 0;
    }
}

Rectangle cursorSetUp (int startX, int startY, int step, int size){
    Rectangle cursor;
    bool isDefined = false;

    while(!isDefined){
        cursor.x = startX;   /*Initialize a cursor rectangle in the first square*/
        cursor.y = startY;
        cursor.width = size;
        cursor.height = cursor.width;
        isDefined = true;
    }

    DrawRectangleRec(cursor, RED);

    Rectangle cursorOld = cursor;

    switch (GetKeyPressed()){
        case KEY_H: /*if (cursor.x != startX)*/ cursor.x += step; break;
        case KEY_J: /*if (cursor.y != startY)*/ cursor.y += step; break;
        case KEY_K: /*if (cursor.x != startX)*/ cursor.x -= step; break;
        case KEY_L: /*if (cursor.y != startY)*/ cursor.y -= step; break;
        case KEY_ENTER: return cursor;
    }
    if (cursorOld.x != cursor.x || cursor.y != cursorOld.y)
        DrawRectangleRec(cursorOld, GRAY);
    DrawRectangleRec(cursor, RED);
    return cursor;
}


int mainMenu(){

    bool gameStarted = false; 
    int chosenGameStyle = 0;

    if (!gameStarted){
        DrawText("Start playing!", 400, 20, 20, LIGHTGRAY);

        if (GuiButton((Rectangle){400, 200, 60, 20}, "1 player") || IsKeyDown(KEY_ENTER)){
            DrawText("Game start!", 600, 20, 40, RED);
            chosenGameStyle = 1; 
            gameStarted = true;
            ClearBackground(BLACK);
        }

        if (GuiButton((Rectangle){400, 550, 60, 20}, "Quit") || IsKeyDown(KEY_Q)){
            DrawText("Quit!", 600, 20, 40, RED);
            CloseWindow(); 
        }
    }
    return chosenGameStyle;
}
