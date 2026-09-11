#include <iostream>
using namespace std;

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION 
#include <raygui.h>

#include "salute.h"

#define BLINE 12
#define BCOL 12

#define SEACOLOR {00,85,119,255}  
#define SHIPCOLOR {192,192,192,255}

#define SEA 0 
#define SHIP 1
#define HIT 2
#define MISS 3
#define HIDDEN 4
#define P2SEA 5

#define MENU_PHASE 0
#define PLACE_PHASE_P1 1
#define PLACE_PHASE_BOT 2
#define GAMEPLAY_BOT 3
#define P1_WON_BOT 4
#define BOT_WON 5

#define SHOOTMSG 30

typedef struct{
    int size;
    int qnt;
    bool toggle;
}Ship;

int mainMenu(void);
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
//Returns the state of the actual board to the gamePhase;
void hackerText(int gameBoard[BLINE][BCOL], int step, Vector2 pos);
//Debugging tool, prints the actual value of the gameBoard over the visualBoard, useful for cheaters too!
bool autoPopulate(int gameBoard[BLINE][BCOL], Ship *ship);
//Tries to place a ship into the gameBoard, returns true when it's done
bool playerShoot(Rectangle *cursor, Vector2 pos, int gameBoard[BLINE][BCOL], int step, char msg[SHOOTMSG]);
//Calculates and shoots where the cursor is, returns false if the shot is invalid
void generateShot(Vector2 *shot);
//Generates a random number limited by BCOL and BLINE
bool checkBotShot(Vector2 *shot, int gameBoard[BLINE][BCOL]);
//Companion to generateShot, checks if the coordinates are valid
bool checkWin(int gameBoard[BLINE][BCOL]);
//returns true if there are no SHIP or HIDDEN on the board


int main (){

    InitWindow(800, 600, "Batalha");
    SetTargetFPS(60);

    Image salute = {
        .data = salutePixelData, //thanks ImageMagick!
        .width = 150,
        .height = 300,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8,
    };

    Texture2D saluteTexture = LoadTextureFromImage(salute);

    int gamePhase = MENU_PHASE; 

    Rectangle visualBoard[BLINE][BCOL];

    Vector2 startPos = {190, 90};
    int step = 35;
    int sqSize = 30;

    int p1gameBoard[BLINE][BCOL];
    for(int i = 0; i < BLINE; i++)
        for(int j = 0; j < BCOL; j++)
            p1gameBoard[i][j] = SEA;

    Ship sub = {1, 4, false};
    Ship cruizer = {2, 3, false};
    Ship carrier = {4, 3, false};
    bool rotateShip = false;
    int shipSum = sub.size * sub.qnt + cruizer.size * cruizer.qnt + carrier.size * carrier.qnt;
    bool p1win = false;
    bool p1turn = true;

    int p2gameBoard[BLINE][BCOL];
    for(int i = 0; i < BLINE; i++)
        for(int j = 0; j < BCOL; j++)
            p2gameBoard[i][j] = P2SEA;

    Ship p2sub = {1, 4, false};
    Ship p2cruizer = {2, 3, false};
    Ship p2carrier = {4, 3, false};
    bool p2rotateShip = false;
    int p2shipSum = p2sub.size * p2sub.qnt + p2cruizer.size * p2cruizer.qnt + p2carrier.size * p2carrier.qnt;
    bool p2isBot = true;
    bool p2turn = false;
    bool p2win = false;

    bool cursorIsSetd = false;
    Rectangle cursor;

    bool hacker = false;

    char msg[SHOOTMSG] = "\0";
    bool msgSetd = false;

    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(BLACK);

        if(gamePhase == MENU_PHASE){ 
            gamePhase = mainMenu();
        }

        if(gamePhase == PLACE_PHASE_P1){ 
            printBoard(gamePhase, p1gameBoard, visualBoard, startPos, step, sqSize);
            DrawTexture(saluteTexture, 10, 310, WHITE);

            char guideText[110];
            snprintf(guideText, (sizeof(guideText)/sizeof(char)), 
                    "[S]ubmarines: %d \n\n"
                    "Crui[Z]ers: %d \n\n"
                    "[C]arriers: %d\n\n\n"
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
                sub = {1, 4, false};
                cruizer = {2, 3, false};
                carrier = {4, 3, false};
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

        if(gamePhase == PLACE_PHASE_BOT){

            EndDrawing(); 
            ClearBackground(BLACK);
            DrawText("Placing carriers...", 200, 280, 40, MAROON);
            EndDrawing(); 
            for (int i = 0; i < p2carrier.qnt;)
                if(autoPopulate(p2gameBoard, &carrier)) i++;

            ClearBackground(BLACK);
            DrawText("Placing cruizers...", 200, 280, 40, MAROON);
            EndDrawing(); 
            for (int i = 0; i < p2cruizer.qnt;)
                if(autoPopulate(p2gameBoard, &cruizer)) i++;

            ClearBackground(BLACK);
            DrawText("Placing Submarines...", 200, 280, 40, MAROON);
            EndDrawing(); 
            for (int i = 0; i < p2sub.qnt;)
                if(autoPopulate(p2gameBoard, &sub)) i++;

            gamePhase++;
        }


        if (gamePhase == GAMEPLAY_BOT){
            step = 30; sqSize = 28;
            Vector2 leftPos = {step, step * 3};
            printBoard(gamePhase, p1gameBoard, visualBoard, leftPos, step, sqSize);

            Vector2 rightPos = {20 + (BCOL * step) + step, leftPos.y};
            printBoard(gamePhase, p2gameBoard, visualBoard, rightPos, step, sqSize);

            if(!cursorIsSetd){
                cursorSetUp(rightPos, step, sqSize, &cursor);
                cursorIsSetd = true;
            }
            moveCursor(rightPos, step, sqSize, &cursor);
            DrawRectangleRec(cursor,VIOLET);

            if (IsKeyPressed(KEY_HOME)) hacker = !hacker;
            if (hacker) hackerText(p2gameBoard, step, rightPos);

            char guideText3[70];
            snprintf(guideText3, 70,
                    "              ^\n"
                    "[S]hoot   <   > Move\n"
                    "              v\n\n");
            DrawText(guideText3, 440, 500, 20, GRAY);

            char teams[35];
            snprintf(teams, 35, "Good Guys\t\t\t\t\t  Bad Guys");
            DrawText(teams, leftPos.x, leftPos.y - step*2, 40, DARKGREEN);

            if(p1turn && !p2win){ 
                if(IsKeyPressed(KEY_S)) {
                    if(playerShoot(&cursor, rightPos, p2gameBoard, step, msg)) p1turn = false;
                    msgSetd = true;
                    bool p1win = checkWin(p2gameBoard);
                    if(p1win) gamePhase = P1_WON_BOT;
                }
                if(msgSetd)DrawText(msg, 410, 460, 30, ORANGE); 
            }
            if(!p1turn && !p1win){
                Vector2 botShot;
                do{
                    DrawText("Calculating Shot", 30, 450, 20, MAROON);
                    EndDrawing();
                    generateShot(&botShot);
                }while(!checkBotShot(&botShot, p1gameBoard));
                p2win = checkWin(p2gameBoard);
                if(checkWin(p1gameBoard)) gamePhase = BOT_WON;
                else p1turn = true;
            }
        }

            if(gamePhase == P1_WON_BOT){
                DrawText("~* YOU WON!!! *~", 100, 60, 60, MAROON);
            }
             
            if(gamePhase == BOT_WON){
                DrawText("~* YOU LOST! *~", 100, 60, 60, MAROON);
            }


        EndDrawing(); 
    }
    CloseWindow(); 
}

void drawFireworks(){

    double radius = 20;
    double angleBetween = PI/6;
    int fireWorkQnt = (2*PI)/angleBetween; /* Yes, I just finished a trig studyng session */
    double t[fireWorkQnt];                 /* how did you notice? */

    for (int i = 0; i <= fireWorkQnt; i++)
        t[i] = angleBetween * i;

    double frameTime = GetFrameTime();
    if (frameTime > 1.0f) frameTime = 0.0f;

    Vector2 circle;
    for (int i = 0; i <= fireWorkQnt; i++) {
    circle.x = (radius/frameTime) * cos(t[i]);
    circle.y = (radius/frameTime) * sin(t[i]);
    DrawLine(400, 300, circle.x, circle.y, WHITE);
    }

}

bool checkWin(int gameBoard[BLINE][BCOL]){
    for(int i = 0; i < BLINE; i++)
        for(int j = 0; j < BCOL; j++)
            if(gameBoard[i][j] == SHIP || gameBoard[i][j] == HIDDEN) return false;

    return true;
}

bool checkBotShot(Vector2 *shot, int gameBoard[BLINE][BCOL]){

    if (gameBoard[(int)shot->x][(int)shot->y] == SHIP){
        gameBoard[(int)shot->x][(int)shot->y] = HIT;
        return true;
    }

    if (gameBoard[(int)shot->x][(int)shot->y] == SEA){
        gameBoard[(int)shot->x][(int)shot->y] = MISS;
        return true;
    }

    return false;
}

void generateShot (Vector2 *shot){
    SetRandomSeed(GetTime());
    shot->x = GetRandomValue(0, BLINE);
    shot->y = GetRandomValue(0, BCOL);
}                 

bool playerShoot(Rectangle *cursor, Vector2 pos, int gameBoard[BLINE][BCOL], int step, char msg[SHOOTMSG]){
    Vector2 shootPos = translateCursorToIndex(cursor, pos, step);

    if(gameBoard[(int)shootPos.x][(int)shootPos.y] == HIDDEN){
        snprintf(msg, SHOOTMSG, "That's a hit!");
        gameBoard[(int)shootPos.x][(int)shootPos.y] = HIT;
        return true;
    }

    if(gameBoard[(int)shootPos.x][(int)shootPos.y] == P2SEA){
        snprintf(msg, SHOOTMSG, "You missed!");
        gameBoard[(int)shootPos.x][(int)shootPos.y] = MISS;
        return true;
    }

    snprintf(msg, SHOOTMSG, "You already shot there!");
    return false;

}

bool autoPopulate(int gameBoard[BLINE][BCOL], Ship *ship){

    SetRandomSeed(GetTime());
    bool rotate = GetRandomValue(0,1);
    bool canPlace = true;
    Vector2 randomPlace;

    if(!rotate){
        randomPlace.x = GetRandomValue(0, BLINE);
        randomPlace.y = GetRandomValue(0, BCOL - ship->size);
    }
    if(rotate){
        randomPlace.x = GetRandomValue(0, BLINE - ship->size);
        randomPlace.y = GetRandomValue(0, BCOL);
    }

    if(!rotate){
        for(int j = 0; j < ship->size; j++){
            if(gameBoard[(int)randomPlace.x][(int)randomPlace.y + j] != P2SEA) return false;
        }
    }

    if(rotate){
        for(int i = 0; i < ship->size; i++){
            if(gameBoard[(int)randomPlace.x + i][(int)randomPlace.y] != P2SEA) return false;
        }
    }

    if(!rotate){
        for(int j = 0; j < ship->size; j++)
            gameBoard[(int)randomPlace.x][(int)randomPlace.y +j] = HIDDEN;
    }

    if(rotate){
        for(int i = 0; i < ship->size; i++)
            gameBoard[(int)randomPlace.x + i][(int)randomPlace.y] = HIDDEN;
    }

    return true;
}

void hackerText(int gameBoard[BLINE][BCOL], int step, Vector2 pos){
    for (int i = 0; i < BLINE; i++)
        for (int j = 0; j < BCOL; j++){
            char ch[6];
            snprintf(ch, 6, "%d",gameBoard[i][j]);
            if (gameBoard[i][j] == HIDDEN) DrawText(ch, 20 + (BLINE * step) + step + (step * i+1), pos.y + (step *j), 10, YELLOW);
            else DrawText(ch, 20 + (BLINE * step) + step + (step * i+1), pos.y + (step *j), 10, RED);
        }
}

void boardFull(int gameBoard[BLINE][BCOL], int *gamePhase, int shipSum, bool *cursorState){
    int sum = 0;
    for(int i = 0; i < BLINE; i++)
        for(int j = 0; j < BCOL; j++)
            sum += gameBoard[i][j];
    //TODO 
    if (sum == shipSum) {
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
    bool canPlace = checkPlace(ship, posIndex, cursor, rotateShip, step, gameBoard); 


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
    snprintf(posInfos, (sizeof(posInfos)/sizeof(char)),
            "X: %d\n"
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
            if (gameBoard[i][j] == SEA) DrawRectangleRec(visualBoard[i][j], SEACOLOR);
            if (gameBoard[i][j] == SHIP) DrawRectangleRec(visualBoard[i][j], SHIPCOLOR);
            if (gameBoard[i][j] == HIT) DrawRectangleRec(visualBoard[i][j], DARKBROWN);
            if (gameBoard[i][j] == MISS) DrawRectangleRec(visualBoard[i][j], SKYBLUE);
            if (gameBoard[i][j] == P2SEA) DrawRectangleRec(visualBoard[i][j], DARKBLUE);
            if (gameBoard[i][j] == HIDDEN) DrawRectangleRec(visualBoard[i][j], DARKBLUE);
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
        drawFireworks();

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
