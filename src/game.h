#ifndef _GAME_
#define _GAME_

#include "chunkSys.h"

//absoluteOn - makes sure the index which is given is in the array 
//absoluteOff - makes sure the index which is given isnt in the array
//cellToggle - toggles the index

#define cellToggle 0
#define absoluteOn 1
#define absoluteOff 2

#define indexNotFound (chunkLength*chunkLength) + 1
#define underCapLimit 5
#define emptyTurnLimit 5

#define indexLookUpSize 128
#define noCell -1
#define midCell 10

#define nextBuff 0
#define currBuff 1


void playTurn();
void toggleCell(chunk* c,int index, int action);



#endif
