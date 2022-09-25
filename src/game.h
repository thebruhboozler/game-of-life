#ifndef _GAME_
#define _GAME_

#include "chunkSys.h"

#define cellToggle 0
#define absoluteOn 1
#define absoluteOff 2

#define indexNotFound (chunkLength*chunkLength) + 1

void playTurn();
void toggleCell(chunk* c,int index, int action);




#endif
