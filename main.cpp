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
#define HIDDEN 4
#define ENEMYSEA 5

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
void placeShip (Ship *ship, Vector2 startPos, int sqSize, int step, bool rotateShip, Rectangle *cursor, int gameBoard[BLINE][BCOL]);
//Draws outlines and places ships on the actual board;
bool checkPlace(Ship *ship, Vector2 pos, Rectangle *cursor, bool rotateShip, int step, int gameBoard[BLINE][BCOL]);
//Checks if the place where the cursor is located is  able to accomodade a certain ship
Vector2 translateCursorToIndex(Rectangle *cursor, Vector2 pos, int step);
//Returns a vector with the position relative to the cursor position
void boardFull(int gameBoard[BLINE][BCOL], int *gamePhase, int shipSum, bool *cursorState);
//returns the state of the actual board to the gamePhase;

int main (){

    InitWindow(800, 600, "Batalha");
    SetTargetFPS(60);

    int gamePhase = 0; 

    int p1gameBoard[BLINE][BCOL] = {SEA};
    int botgameBoard[BLINE][BCOL] = {SEA};
    Rectangle visualBoard[BLINE][BCOL];
    Vector2 startPos = {190, 90};
    int step = 35;
    int sqSize = 30;

    Ship sub = {1, 3, false};
    Ship cruizer = {2, 2, false};
    Ship carrier = {4, 2, false};
    bool rotateShip = false;
    int shipSum = sub.size * sub.qnt + cruizer.size * cruizer.qnt + carrier.size * carrier.qnt;

    bool cursorIsSetd = false;
   Rectangle cursor;
    
    while(!WindowShouldClose()){
        BeginDrawing();

        if(gamePhase == 0){ 
            gamePhase = mainMenu();
        }
        ClearBackground(BLACK);

        if(gamePhase == 1){ 
            printBoard(gamePhase, p1gameBoard, visualBoard, startPos, step, sqSize);
            char guideText[110];
            snprintf(guideText, (sizeof(guideText)/sizeof(char)), "[S]ubmarines: %d \n\nCrui[Z]ers: %d \n\n[C]arriers: %d\n\n\n"
                                                                  "   ^\n"
                                                                  " <   > Move\n"
                                                                  "   v\n\n"
                                                                  "Put [D]own\n\n"
                                                                  "[R]otate\n\n"
                                                                  "R[E]set", sub.qnt, cruizer.qnt, carrier.qnt);
            DrawText(guideText, 620, 150, 20, GRAY);
            DrawText("Place your ships: ", 220, 20, 40, MAROON);
             
            if(!cursorIsSetd) {
            cursorSetUp(startPos, step, sqSize, &cursor);
            cursorIsSetd = true;
            }
            moveCursor(startPos, step, sqSize, &cursor);
            DrawRectangleRec(cursor,VIOLET);

            if (IsKeyPressed(KEY_R)) rotateShip = !rotateShip;

            if (IsKeyPressed(KEY_E)){ 
                for (int i = 0; i < BLINE; i++)
                    for(int j = 0; j < BCOL; j++)
                        p1gameBoard[i][j] = SEA;
                sub = {1, 3, false};
                cruizer = {2, 2, false};
                carrier = {4, 2, false};
            }

            if (IsKeyPressed(KEY_C)) carrier.toggle = !carrier.toggle;
            if (carrier.toggle){
                placeShip(&carrier, startPos, sqSize, step, rotateShip, &cursor, p1gameBoard);
                sub.toggle = false; cruizer.toggle = false;
            }
            if (IsKeyPressed(KEY_S)) sub.toggle = !sub.toggle;
            if (sub.toggle){
                placeShip(&sub, startPos, sqSize, step, rotateShip, &cursor, p1gameBoard);
                carrier.toggle = false; cruizer.toggle = false;
            }
            if (IsKeyPressed(KEY_Z)) cruizer.toggle = !cruizer.toggle;
            if (cruizer.toggle){
                placeShip(&cruizer, startPos, sqSize, step, rotateShip, &cursor, p1gameBoard);
                carrier.toggle = false; sub.toggle = false;
            }

            boardFull(p1gameBoard, &gamePhase, shipSum, &cursorIsSetd);
        }
         
        if (gamePhase == 2){
            step = 30; sqSize = 28;
            Vector2 leftPos = {step, step};
            printBoard(gamePhase, p1gameBoard, visualBoard, leftPos, step, sqSize);
             
            Vector2 rightPos = {20 + (12 * step) + step, leftPos.y};
            printBoard(gamePhase, p1gameBoard, visualBoard, rightPos, step, sqSize);
             
            if(!cursorIsSetd){
            cursorSetUp(rightPos, step, sqSize, &cursor);
            cursorIsSetd = true;
            }
            moveCursor(rightPos, step, sqSize, &cursor);
            DrawRectangleRec(cursor,VIOLET);
        }

        EndDrawing(); 
    }

    CloseWindow(); 
}
void boardFull(int gameBoard[BLINE][BCOL], int *gamePhase, int shipSum, bool *cursorState){
    int sum = 0;
    for(int i = 0; i < BLINE; i++)
        for(int j = 0; j < BCOL; j++)
            sum += gameBoard[i][j];
//TODO 
    if (sum != shipSum) {
        DrawText("Board's ready! Press [ENTER] to continue.", 180, 530, 20, GRAY);
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)){
            *cursorState = false;
            (*gamePhase)++;
        }
    }
}

bool checkPlace(Ship *ship, Vector2 pos, Rectangle *cursor, bool rotateShip, int step, int gameBoard[BLINE][BCOL]){

    bool checkBorders = false;
    if(!rotateShip){
        if (pos.y <= BCOL - (ship->size)) checkBorders = true;;
    } else{
        if (pos.x <= BLINE - (ship->size)) checkBorders = true;
    } 

    bool checkWithGameBoard = true;
    if (checkBorders) {
        if(!rotateShip){
            for(int j = 0; j < ship->size; j++){
                if (gameBoard[(int)pos.x][(int)pos.y + j] != SEA) checkWithGameBoard = false;
            }
        }

        if (rotateShip)
            for(int i = 0; i < ship->size; i++){
                if (gameBoard[(int)pos.x + i][(int)pos.y] != SEA) checkWithGameBoard = false;
            }
    }

    bool checkShipQnt = true; 
    if (ship->qnt < 1) checkShipQnt = false;

    return checkWithGameBoard && checkBorders && checkShipQnt;

}

Vector2 translateCursorToIndex(Rectangle *cursor, Vector2 pos, int step){
    Vector2 posIndex;

    posIndex.x = (cursor->x - pos.x)/step;
    posIndex.y = (cursor->y - pos.y)/step;

    return posIndex;
}

void placeShip (Ship *ship, Vector2 startPos, int sqSize, int step, bool rotateShip, Rectangle *cursor, int gameBoard[BLINE][BCOL]){

    Vector2 posIndex = translateCursorToIndex(cursor, startPos, step);
    bool canPlace = canPlace = checkPlace(ship, posIndex, cursor, rotateShip, step, gameBoard); 


    if(rotateShip){
        for(int i = 0; i < ship->size; i++){
            if(canPlace) DrawRectangle(cursor->x + step*i, cursor->y, sqSize, sqSize, LIME);
            else (DrawRectangle(cursor->x, cursor->y, sqSize, sqSize, RED));
        }
    }

    if(!rotateShip){
        for(int i = 0; i < ship->size; i++){
            if(canPlace) DrawRectangle(cursor->x, cursor->y + step*i, sqSize, sqSize, LIME);
            else (DrawRectangle(cursor->x, cursor->y, sqSize, sqSize, RED));
        }
    }

    if(canPlace && IsKeyPressed(KEY_D)){
        if(rotateShip){
            for (int i = 0; i < ship->size; i++) gameBoard[(int)posIndex.x + i][(int)posIndex.y] = SHIP;
            ship->qnt -= 1;
        } else{
            for (int i = 0; i < ship->size; i++) gameBoard[(int)posIndex.x][(int)posIndex.y + i] = SHIP;
            ship->qnt -= 1;
        }
    }

    char posInfos[20];
    snprintf(posInfos, (sizeof(posInfos)/sizeof(char)), "X: %d\n"
            "Y: %d\n", (int)(cursor->x - startPos.x)/step, (int)(cursor->y - startPos.y)/step);
    DrawText(posInfos, 100, 130, 30, GRAY);

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
            if (gameBoard[i][j] == SEA) DrawRectangleRec(visualBoard[i][j], DARKBLUE);
            if (gameBoard[i][j] == SHIP) DrawRectangleRec(visualBoard[i][j], DARKGRAY);
            if (gameBoard[i][j] == HIT) DrawRectangleRec(visualBoard[i][j], MAROON);
            if (gameBoard[i][j] == MISS) DrawRectangleRec(visualBoard[i][j], SKYBLUE);
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
}

int mainMenu(){

    bool gameStarted = false; 
    int chosenGameStyle = 0;

    if (!gameStarted){
        DrawText("~* Battle-Ships *~", 100, 60, 60, MAROON);

        if (GuiButton((Rectangle){800/2 - 120/2, 600/2 - 60/2, 120, 60}, "Single player") || IsKeyDown(KEY_ENTER)){
            chosenGameStyle = 1; 
            gameStarted = true;
        }

        if (GuiButton((Rectangle){800/2 - 60/2, 540, 60, 20}, "Quit") || IsKeyDown(KEY_Q)){
            DrawText("Quit!", 600, 20, 40, RED);
            CloseWindow(); 
        }
    }
    return chosenGameStyle;
}
