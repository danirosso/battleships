
void populateBoard(int gameBoard[BLINE][BCOL], Ship *ship);
//bruteforces values until all ships of a type are placed in the gameBoard, calls itself *recursively*
//!!DANGER!! THIS THING IS A BOMB  WAITING TO EXPLODE!! !!DANGER!!

void populateBoard(int gameBoard[BLINE][BCOL], Ship *ship){
    SetRandomSeed(time(NULL));
    bool canInsert = false;

    for(int i = 0; i < ship->qnt; i++){
        bool rotate = GetRandomValue(0,1);
        if(rotate) {
            Vector2 posble;
            posble.y = BCOL - ship->size; //!rotated, we are limited by the y axis
            posble.x = BLINE;

            Vector2 place;
            place.y = GetRandomValue(0, posble.y);
            place.x = GetRandomValue(0, posble.x);

            for(int j = 0; j < ship->size; j++){
                if (gameBoard[(int)place.x][(int)place.y+j] != P2SEA && ship->qnt > 0) populateBoard(gameBoard, ship);
                else canInsert = true;//this isn't ideal, there's a lot that can go wrong here
            }
            if(canInsert) {
                gameBoard[(int)place.x][(int)place.y] = HIDDEN;
                (ship->qnt)--;
            }
        }

        if(!rotate){
            Vector2 posble;
            posble.y = BCOL;
            posble.x = BLINE - ship->size;

            Vector2 place;
            place.y = GetRandomValue(0, posble.y);
            place.x = GetRandomValue(0, posble.x);

            for (int i = 0; i < ship->size; i++){
                if (gameBoard[(int)place.x+i][(int)place.y] != P2SEA && ship->qnt > 0) populateBoard(gameBoard, ship);
                else canInsert = true;//this isn't ideal, there's a lot that can go wrong here
            }
            if(canInsert) {
                gameBoard[(int)place.x][(int)place.y] = HIDDEN;
                (ship->qnt)--;
            }
        }
    }
}
