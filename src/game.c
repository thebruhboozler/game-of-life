#include "chunkSys.h"
#include "game.h"
#include "utils.h"

#include <stdlib.h>
#include <stdbool.h>


extern cordentry* hashTable[hashSize];
extern bool paused;

int numOfTurns = 0;
bool IndexCleared = true;
int savedIndexes[indexLookUpSize] = {noCell};


void playTurn();
void updateChunk(cordentry* checkedChunk);
void toggleCell(chunk* c,int index, int action);
int findIndex(chunk* c, int index);
void moveIndexToBuff(int index, int comparisonIndex , chunk* c , unsigned short* buff , int* buffPos);
bool hasIndex(chunk *c , int index);
bool trimChunk(cordentry* entry);


void playTurn(){
	if(!paused) return;

	numOfTurns++;

	//go through each chunk and update them
	for(int i = 0; i < hashSize; i++){

		cordentry* e = hashTable[i];

		while(e !=NULL){
			updateChunk(e);
			e = e->next;
		};
	};
};

void updateChunk(cordentry* checkedChunk){

	chunk* updatingChunk = checkedChunk->segment;

	if(!trimChunk(checkedChunk)) return;

	//swap current turn and new turn

	updatingChunk->aliveCells =  (unsigned short*)((unsigned long) updatingChunk->aliveCells ^ (unsigned long) updatingChunk->prevTurn);
	updatingChunk->prevTurn =  (unsigned short*)((unsigned long) updatingChunk->prevTurn ^ (unsigned long)updatingChunk->aliveCells);
	updatingChunk->aliveCells =  (unsigned short*)((unsigned long) updatingChunk->aliveCells ^ (unsigned long) updatingChunk->prevTurn);
	

	// clear index buffer
	for(int i = 0; i < indexLookUpSize; i++) savedIndexes[i] = noCell;
	
	int deadCellNum = 0; 
	static unsigned short deadCells[chunkLength*chunkLength/2];

	//check only the alive cells and their neighbours 

	//check the alive cells and change their dead neighbors 
	//if the dead neighbors dont border at least one alive cell with an index which is less than the current one we are checking 
	//move the cell to the dead cell buffer 

	int nextTurnCellNum = 0;
	
	//check alive cells and turn off
	for(int i = 0; i < updatingChunk->numOfCells; i++){

		int currentIndex = updatingChunk->aliveCells[i];
		int aliveNeighboursNum = 0 ;

		int checkIndex = calculateNeighbourIndex(currentIndex , upperLeft);
		if(!hasIndex(updatingChunk ,checkIndex)) moveIndexToBuff(checkIndex , currentIndex, updatingChunk , deadCells, &deadCellNum, prevBuff);
		else aliveNeighboursNum++;
		
		checkIndex = calculateNeighbourIndex(currentIndex , upper);
		if(!hasIndex(updatingChunk ,checkIndex)) moveIndexToBuff(checkIndex , currentIndex, updatingChunk , deadCells, &deadCellNum, prevBuff);
		else aliveNeighboursNum++;

		checkIndex = calculateNeighbourIndex(currentIndex , upperRight);
		if(!hasIndex(updatingChunk ,checkIndex)) moveIndexToBuff(checkIndex , currentIndex, updatingChunk , deadCells, &deadCellNum, prevBuff);
		else aliveNeighboursNum++;
	
		checkIndex = calculateNeighbourIndex(currentIndex , left);
		if(!hasIndex(updatingChunk ,checkIndex)) moveIndexToBuff(checkIndex , currentIndex, updatingChunk , deadCells, &deadCellNum, prevBuff);
		else aliveNeighboursNum++;

		checkIndex = calculateNeighbourIndex(currentIndex , right);
		if(!hasIndex(updatingChunk ,checkIndex)) moveIndexToBuff(checkIndex , currentIndex, updatingChunk , deadCells, &deadCellNum, prevBuff);
		else aliveNeighboursNum++;
		
		checkIndex = calculateNeighbourIndex(currentIndex , lowerLeft);
		if(!hasIndex(updatingChunk ,checkIndex)) moveIndexToBuff(checkIndex , currentIndex, updatingChunk , deadCells, &deadCellNum, prevBuff);
		else aliveNeighboursNum++;
		
		checkIndex = calculateNeighbourIndex(currentIndex , lowerRight);
		if(!hasIndex(updatingChunk ,checkIndex)) moveIndexToBuff(checkIndex , currentIndex, updatingChunk , deadCells, &deadCellNum, prevBuff);
		else aliveNeighboursNum++;
		
		checkIndex = calculateNeighbourIndex(currentIndex , lower);
		if(!hasIndex(updatingChunk ,checkIndex)) moveIndexToBuff(checkIndex , currentIndex, updatingChunk , deadCells, &deadCellNum, prevBuff);
		else aliveNeighboursNum++;
	

		// the case when the cell survives 
		if(aliveNeighboursNum == 2 || aliveNeighboursNum == 3){
			updatingChunk->aliveCells[nextTurnCellNum++] = currentIndex;
			continue;
		};

	};

	//check the dead Neighbours and turn on
	
	for(int i = 0 ; i < deadCellNum ;i++){
		int currentIndex = deadCells[i];	
		
		int aliveNeighboursNum = 0;

		if(hasIndex(updatingChunk, calculateNeighbourIndex(currentIndex , upperLeft) , prevBuff)) aliveNeighbours++;

		if(hasIndex(updatingChunk, calculateNeighbourIndex(currentIndex , upper) , prevBuff)) aliveNeighbours++;

		if(hasIndex(updatingChunk, calculateNeighbourIndex(currentIndex , upperRight) , prevBuff)) aliveNeighbours++;

		if(hasIndex(updatingChunk, calculateNeighbourIndex(currentIndex , left) , prevBuff)) aliveNeighbours++;

		if(hasIndex(updatingChunk, calculateNeighbourIndex(currentIndex , right) , prevBuff)) aliveNeighbours++;

		if(hasIndex(updatingChunk, calculateNeighbourIndex(currentIndex , lowerLeft) , prevBuff)) aliveNeighbours++;

		if(hasIndex(updatingChunk, calculateNeighbourIndex(currentIndex , lower) , prevBuff)) aliveNeighbours++;

		if(hasIndex(updatingChunk, calculateNeighbourIndex(currentIndex , lowerRight) , prevBuff)) aliveNeighbours++;

		if(aliveNeighboursNum == 3){
			toggleCell(updatingChunk , currentIndex , absoluteOn);
			nextTurnCellNum++;
		};
	};

	updatingChunk->numOfCells = nextTurnCellNum;
};
// standard binary search algorithm
int findIndex(chunk* c, int index,int buffOption){

	unsigned short* buffer;

	switch(buffOption){
		case prevBuff:
			buffer = c->prevTurn;
		default:
			buffer = c->aliveCells;
	};

	int lowerIndex = 0 , upperIndex = c->numOfCells;
	
	while(lowerIndex <= upperIndex){

		int currentIndex = (lowerIndex + upperIndex)/2;

		if(buffer[currentIndex] == index) return currentIndex;
		else if( index > buffer[currentIndex]) lowerIndex = currentIndex + 1;
		else upperIndex = currentIndex - 1;
	};

	return indexNotFound;
};

void toggleCell(chunk* c,int index, int action){
	//to speed up the index finding	
	int indexOfIndex = findIndex(c , index , currBuff);

	switch(action){
		case absoluteOn:
			if(indexOfIndex == indexNotFound) goto turnOn;
			return;
		case absoluteOff:
			if(indexOfIndex != indexNotFound) goto turnOff;
			return;
		default:
			if(indexOfIndex == indexNotFound) goto turnOn;
			else goto turnOff;
			return;
	};

turnOn:
	//up size the memory if the number of cells gets too big 
	if(c->numOfCells >= c->cellArrSize){
		c->cellArrSize *= 2;
		c->aliveCells = realloc((void*) c-> aliveCells, c->cellArrSize);
		c->underCapTurnNum = 0;
		for(int i = c->numOfCells ; i < c->cellArrSize ; i++) c->aliveCells[i] = 0;
	};

	//TODO switch from linear search algorithm to binary 

	int k;
	//create speace for the index to be inserted 
	for( k = 0; k < c->numOfCells ; k++) if(c->aliveCells[k] > index) break;

	for(int i = c->numOfCells ; i > k; i--) c->aliveCells[i] = c->aliveCells[i - 1];

	c->aliveCells[k] = index;

	c->numOfCells++;

	return;

turnOff:

	for(int i = indexOfIndex ; i < c->numOfCells ;i++) c->aliveCells[i] = c->aliveCells[i + 1];
	c->numOfCells--;

	return;
};

inline bool hasIndex(chunk *c ,int index , int option){

	int slot = hash(index , index) % indexLookUpSize ;

	int qIndex = savedIndexes[slot];

	if(qIndex != noCell){
		if(qIndex == index) return true;
		return (findIndex(c , index , option) != indexNotFound);
	};

	if(findIndex(c , index , option) != indexNotFound){
		savedIndexes[slot] = index;
		return true;
	};
	return false;
};

bool trimChunk(cordentry* entry){

	//downsize if the current number of cells has been less than half of the array capacity for more than limit number turns 
	//delete the chunk if its been empty for the previous turns 

	chunk* checkedChunk = entry->segment;

	if(checkedChunk->numOfCells == underCapLimit){
		checkedChunk->cellArrSize /= 2;
		checkedChunk->aliveCells = realloc((void*)checkedChunk->aliveCells, checkedChunk->cellArrSize * sizeof(unsigned short)); 
		checkedChunk->prevTurn = realloc((void*)checkedChunk->prevTurn, checkedChunk->cellArrSize * sizeof(unsigned short));
		
		checkedChunk->underCapTurnNum = 0 ; 
		return true; 
	}

	if(checkedChunk->numOfCells < checkedChunk->cellArrSize && checkedChunk->numOfCells >= startingChunkSize) {
		checkedChunk->underCapTurnNum++;
		return true;
	};

	if(checkedChunk->numOfCells != 0) return true;

	if(checkedChunk->inactiveTurnNum == emptyTurnLimit){
		deleteChunk(entry);
		return false;
	};

	checkedChunk->inactiveTurnNum++;
	return true; 
};

void moveIndexToBuff(int index, int comparisonIndex , chunk* c , unsigned short* buff ,int* buffPos, int option){

	int neighbours[8];

	int uLNeighbour = calculateNeighbourIndex(index, upperLeft);
	neighbours[upperLeft] = ( uLNeighbour >= 0 && uLNeighbour != comparisonIndex && hasIndex(c , uLNeighbour, option)) ? uLNeighbour : noCell;

	int uNeighbour = calculateNeighbourIndex(index, upper);
	neighbours[upper] = ( uNeighbour >= 0 && uNeighbour != comparisonIndex && hasIndex(c , uNeighbour, option)) ? uNeighbour : noCell;

	int uRNeighbour = calculateNeighbourIndex(index, upperRight);
	neighbours[upperRight] = ( uRNeighbour >= 0 && uRNeighbour != comparisonIndex && hasIndex(c , uRNeighbour, option)) ? uRNeighbour : noCell;

	int lNeighbour = calculateNeighbourIndex(index, right);
	neighbours[left] = ( lNeighbour >= 0 && lNeighbour != comparisonIndex && hasIndex(c , lNeighbour, option)) ? lNeighbour : noCell;

	int RNeighbour =  calculateNeighbourIndex(index, left);
	neighbours[right] = ( RNeighbour >= 0 && RNeighbour != comparisonIndex && hasIndex(c , RNeighbour, option)) ? RNeighbour : noCell;

	int lLNeighbour =  calculateNeighbourIndex(index, lowerLeft);
	neighbours[lowerLeft] = ( lLNeighbour >= 0 && lLNeighbour != comparisonIndex && hasIndex(c , lLNeighbour, option)) ? lLNeighbour : noCell;

	lNeighbour =  calculateNeighbourIndex(index, lower);
	neighbours[lower] = ( lNeighbour >= 0 && lNeighbour != comparisonIndex && hasIndex(c , lNeighbour, option)) ? lNeighbour : noCell;

	int lRNeighbour =  calculateNeighbourIndex(index, lowerRight);
	neighbours[lowerRight] = ( lRNeighbour >= 0 && lRNeighbour != comparisonIndex && hasIndex(c , lRNeighbour, option)) ? lRNeighbour : noCell;

	for(int i = 0 ; i < 8 ; i++){
		if(neighbours[i] == noCell || neighbours[i] <= comparisonIndex) continue;
		buff[*buffPos++] = index;
		return;
	};
};
