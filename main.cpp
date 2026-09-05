#include <iostream>
using namespace std;

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION 
#include <raygui.h>

#define BLINE 12
#define BCOL 12

typedef struct{
    int size;
    int qnt;
    bool toggle;
}Ship;

int mainMenu();
//Returns a number which represents the type of game that will be played;
void cursorSetUp (int startX, int startY, int step, int size, Rectangle *cursor);
//Initializes the cursor;
void printBoard (int gamePhase, Rectangle visualBoard[BLINE][BCOL], int startX, int startY, int step, int sqSize);
//Prints the board differently depending on the state of the game;
void moveCursor (int startX, int startY, int step, int size, Rectangle *cursor);
//Moves the position of the cursor around;
void placeShip (Ship ship, int sqSize, int step, bool rotateShip, Rectangle *cursor, int gameBoard[BLINE][BCOL]);
//Draws outlines and places ships on the actual board;
bool checkPlace (Ship ship, Rectangle *cursor, bool rotateShip, int step);
//checks if the place where the cursor is able to accomodade a certain ship

int main (){

    InitWindow(800, 600, "Batalha");
    SetTargetFPS(60);

    int gamePhase = 0; 

    int p1gameBoard[BLINE][BCOL] = {0};
    Rectangle visualBoard[BLINE][BCOL];
    int step = 35;
    int sqSize = 30;
    int startX = 190;
    int startY = 90;

    Ship sub = {1, 3, false};
    Ship cruizer = {2, 2, false};
    Ship carrier = {4, 2, false};
    bool rotateShip = false;

    Rectangle cursor;
    cursorSetUp(startX, startY, step, sqSize, &cursor);

    while(!WindowShouldClose()){
        BeginDrawing();

        if(gamePhase == 0){ 
            gamePhase = mainMenu();
        }
        ClearBackground(BLACK);

        if(gamePhase == 1){ 
            printBoard(gamePhase, visualBoard, startX, startY, step, sqSize);
            moveCursor(startX, startY, step, sqSize, &cursor);
            DrawRectangleRec(cursor,GREEN);
            if (IsKeyPressed(KEY_C)) carrier.toggle = !carrier.toggle;
            if (carrier.toggle){
                placeShip(carrier, sqSize, step, rotateShip, &cursor, p1gameBoard);
                sub.toggle = false; cruizer.toggle = false;
            }
            if (IsKeyPressed(KEY_S)) sub.toggle = !sub.toggle;
            if (sub.toggle){
                placeShip(sub, sqSize, step, rotateShip, &cursor, p1gameBoard);
                carrier.toggle = false; cruizer.toggle = false;
            }
            if (IsKeyPressed(KEY_K)) cruizer.toggle = !cruizer.toggle;
            if (cruizer.toggle){
                placeShip(cruizer, sqSize, step, rotateShip, &cursor, p1gameBoard);
                carrier.toggle = false; sub.toggle = false;
            }
            if (IsKeyPressed(KEY_R)) rotateShip = !rotateShip; 
        }

        EndDrawing(); 
    }

    CloseWindow(); 
}

bool checkPlace(Ship ship, Rectangle *cursor, bool rotateShip, int step){

    if(!rotateShip){
        if (cursor->y <= (90 + step * (BLINE-ship.size))) return true;
    } else{
        if (cursor->x <= (190 + step * (BCOL-ship.size))) return true;
    } 
    return false;
}

void placeShip (Ship ship, int sqSize, int step, bool rotateShip, Rectangle *cursor, int gameBoard[BLINE][BCOL]){
    bool canPlace = checkPlace(ship, cursor, rotateShip, step); 

    if(rotateShip){
        for(int i = 0; i < ship.size; i++){
            if(canPlace) DrawRectangle(cursor->x + step*i, cursor->y, sqSize, sqSize, BLUE);
            else (DrawRectangle(cursor->x, cursor->y, sqSize, sqSize, RED));
        }
    }

    if(!rotateShip){
        for(int i = 0; i < ship.size; i++){
            if(canPlace) DrawRectangle(cursor->x, cursor->y + step*i, sqSize, sqSize, BLUE);
            else (DrawRectangle(cursor->x, cursor->y, sqSize, sqSize, RED));
        }
    }
}

void printBoard (int gamePhase, Rectangle visualBoard[BLINE][BCOL], int startX, int startY, int step, int sqSize){

    for (int i = 0; i < BLINE; i++){ 
        for (int j = 0; j < BCOL; j++){ 
            visualBoard[i][j].x = startX + step*i; 
            visualBoard[i][j].y = startY + step*j;
            visualBoard[i][j].width = sqSize;
            visualBoard[i][j].height = visualBoard[i][j].width;
        }
    }

    for (int i = 0; i < BCOL; i++){
        for (int j = 0; j < BLINE; j++){
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
