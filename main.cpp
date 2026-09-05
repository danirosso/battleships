#include <iostream>
using namespace std;

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION 
#include <raygui.h>

#define BLINE 12
#define BCOL 12

#define SEA 0 
#define SHIP 1
#define HIT 2
#define MISS 3

typedef struct{
    int size;
    int qnt;
    bool toggle;
}Ship;

int mainMenu();
//Returns a number which represents the type of game that will be played;
void cursorSetUp (Vector2 startPos, int step, int size, Rectangle *cursor);
//Initializes the cursor;
void printBoard (int gamePhase, int gameBoard[BLINE][BCOL], Rectangle visualBoard[BLINE][BCOL], Vector2 startPos, int step, int sqSize);
//Prints the board differently depending on the state of the game;
void moveCursor (Vector2 startPos, int step, int size, Rectangle *cursor);
//Moves the position of the cursor around;
void placeShip (Ship ship, Vector2 startPos, int sqSize, int step, bool rotateShip, Rectangle *cursor, int gameBoard[BLINE][BCOL]);
//Draws outlines and places ships on the actual board;
bool checkPlace (Ship ship, Vector2 pos, Rectangle *cursor, bool rotateShip, int step);
//Checks if the place where the cursor is able to accomodade a certain ship

int main (){

    InitWindow(800, 600, "Batalha");
    SetTargetFPS(60);

    int gamePhase = 0; 

    int p1gameBoard[BLINE][BCOL] = {SEA};
    Rectangle visualBoard[BLINE][BCOL];
    Vector2 startPos = {190, 90};
    int step = 35;
    int sqSize = 30;

    Ship sub = {1, 3, false};
    Ship cruizer = {2, 2, false};
    Ship carrier = {4, 2, false};
    bool rotateShip = false;

    Rectangle cursor;
    cursorSetUp(startPos, step, sqSize, &cursor);

    while(!WindowShouldClose()){
        BeginDrawing();

        if(gamePhase == 0){ 
            gamePhase = mainMenu();
        }
        ClearBackground(BLACK);

        if(gamePhase == 1){ 
            printBoard(gamePhase, p1gameBoard, visualBoard, startPos, step, sqSize);
            moveCursor(startPos, step, sqSize, &cursor);
            DrawRectangleRec(cursor,GREEN);
            if (IsKeyPressed(KEY_C)) carrier.toggle = !carrier.toggle;
            if (carrier.toggle){
                placeShip(carrier, startPos, sqSize, step, rotateShip, &cursor, p1gameBoard);
                sub.toggle = false; cruizer.toggle = false;
            }
            if (IsKeyPressed(KEY_S)) sub.toggle = !sub.toggle;
            if (sub.toggle){
                placeShip(sub, startPos, sqSize, step, rotateShip, &cursor, p1gameBoard);
                carrier.toggle = false; cruizer.toggle = false;
            }
            if (IsKeyPressed(KEY_K)) cruizer.toggle = !cruizer.toggle;
            if (cruizer.toggle){
                placeShip(cruizer, startPos, sqSize, step, rotateShip, &cursor, p1gameBoard);
                carrier.toggle = false; sub.toggle = false;
            }
            if (IsKeyPressed(KEY_R)) rotateShip = !rotateShip; 
        }

        EndDrawing(); 
    }

    CloseWindow(); 
}

bool checkPlace(Ship ship, Vector2 pos, Rectangle *cursor, bool rotateShip, int step){

    if(!rotateShip){
        if (cursor->y <= (pos.y + step * (BLINE-ship.size))) return true;
    } else{
        if (cursor->x <= (pos.x + step * (BCOL-ship.size))) return true;
    } 
    return false;
}

void placeShip (Ship ship, Vector2 startPos, int sqSize, int step, bool rotateShip, Rectangle *cursor, int gameBoard[BLINE][BCOL]){

    bool canPlace = checkPlace(ship, startPos, cursor, rotateShip, step); 

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
         
        Vector2 posIndex;
        posIndex.x = (cursor->x - startPos.x)/step;
        posIndex.y = (cursor->y - startPos.y)/step;
         
        char xPos[6];
        snprintf(xPos, 6, "X: %d", (int)(cursor->x - startPos.x)/step);
        DrawText(xPos, 100, 100, 20, WHITE);
        char yPos[6];
        snprintf(yPos, 6, "Y: %d", (int)(cursor->y - startPos.y)/step);
        DrawText(yPos, 100, 130, 20, WHITE);
         
    if(canPlace && IsKeyPressed(KEY_D)){
        if(rotateShip){
            for (int i = 0; i < ship.size; i++) gameBoard[(int)posIndex.x + i][(int)posIndex.y] = SHIP;
        } else{
            for (int i = 0; i < ship.size; i++) gameBoard[(int)posIndex.x][(int)posIndex.y + i] = SHIP;
        }
    }
}

void printBoard (int gamePhase, int gameBoard[BLINE][BCOL], Rectangle visualBoard[BLINE][BCOL], Vector2 startPos, int step, int sqSize){

    for (int i = 0; i < BLINE; i++){ 
        for (int j = 0; j < BCOL; j++){ 
            visualBoard[i][j].x = startPos.x + step*i; 
            visualBoard[i][j].y = startPos.y + step*j;
            visualBoard[i][j].width = sqSize;
            visualBoard[i][j].height = visualBoard[i][j].width;
        }
    }

    for (int i = 0; i < BCOL; i++){
        for (int j = 0; j < BLINE; j++){
            if (gameBoard[i][j] == SEA) DrawRectangleRec(visualBoard[i][j], GRAY);
            if (gameBoard[i][j] == SHIP) DrawRectangleRec(visualBoard[i][j], PURPLE);
            if (gameBoard[i][j] == HIT) DrawRectangleRec(visualBoard[i][j], ORANGE);
            if (gameBoard[i][j] == MISS) DrawRectangleRec(visualBoard[i][j], BROWN);
        }
    }
}

void cursorSetUp (Vector2 startPos, int step, int size, Rectangle *cursor){
    cursor->x = startPos.x;
    cursor->y = startPos.y;
    cursor->width = size;
    cursor->height = cursor->width;
}

void moveCursor (Vector2 startPos, const int step, const int size, Rectangle *cursor){
    /*NGL, this took a while. */

    Rectangle cursorOld = *cursor; 

    if (IsKeyPressed(KEY_UP)) cursor->y -= step;
    if (IsKeyPressed(KEY_DOWN)) cursor->y += step;
    if (IsKeyPressed(KEY_LEFT)) cursor->x -= step;
    if (IsKeyPressed(KEY_RIGHT)) cursor->x += step;

    if (cursor->x < startPos.x || cursor->x > startPos.x + step * (BCOL-1)) cursor->x = cursorOld.x;
    if (cursor->y < startPos.y || cursor->y > startPos.y + step * (BLINE-1)) cursor->y = cursorOld.y;

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
