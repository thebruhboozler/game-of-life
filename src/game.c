#include "chunkSys.h"
#include "game.h"
#include "utils.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>


extern cordentry* hashTable[hashSize];
extern bool paused;

int numOfTurns = 0;
bool IndexCleared = true;
//int savedIndexes[indexLookUpSize] = {noCell};


void playTurn();
void updateChunk(cordentry* checkedChunk);
void toggleCell(chunk* c,int index, int action);
int findIndex(chunk* c, int index, int buffOption);
void moveIndexToBuff(int index, int comparisonIndex , chunk* c , unsigned short* buff , int* buffPos, int option);
bool hasIndex(chunk *c , int index);
bool trimChunk(cordentry* entry);
int indexStatus(int index);
void getNeighbours(int index, int* buffer);
int calculateNeighbourIndex(int index , int dir);
void subtractOverlap(int* iNeighbours,int *neighbourSize, int* dif, int diffSize);


void playTurn(){
	if(!paused) return;

	numOfTurns++;

	//go through each chunk and update them
	for(int i = 0; i < hashSize; i++){

		cordentry* e = hashTable[i];

		while(e !=NULL){
			e->segment->updated = false;
			updateChunk(e);
			e = e->next;
		};
	};
};

void updateChunk(cordentry* checkedChunk){

	chunk* updatingChunk = checkedChunk->segment;

//	if(!trimChunk(checkedChunk)) return;
	int nextBuffCellNum = 0;



	//go through the alive cells and update them 
	
	for(int i = 0 ; i < updatingChunk->numOfCells; i++){

		int index = updatingChunk->aliveCells[i];
		int iStatus = indexStatus(index);

		if(iStatus != midCell){
			//TODO deal with neighbours
			continue;
		};
	

		int iNeighboursSize = 8;
		int iNeighbours[8];
		getNeighbours(index, iNeighbours);

		int aliveNeighbours[8] = {noCell};
		int aliveNeighboursNum = 0;

		// check alive neighbours
		for(int j = 0; j < 8; j++ ){
			if(hasIndex(updatingChunk, iNeighbours[j] ) )aliveNeighbours[aliveNeighboursNum++] = iNeighbours[j];
		};


		// grab all pervious overlapping cells and check the overlaps 
		for(int j = i ; j > 0 ; j--){
			int compIndex = updatingChunk->aliveCells[j];
			int iDif = index - compIndex;
			if(( iDif >= 2*chunkLength-2 && iDif <= 2*chunkLength+2) || (iDif >= chunkLength- 2 && iDif <= chunkLength + 2) || iDif == 2 || iDif == 1){
				int compNeighbours[8];
				getNeighbours(compIndex, compNeighbours);
				subtractOverlap(iNeighbours, &iNeighboursSize, compNeighbours , 8);
				continue;
			};
			if(iDif > 2*chunkLength - 3) break;
		};

		subtractOverlap(iNeighbours,&iNeighboursSize, aliveNeighbours, aliveNeighboursNum);

		if(aliveNeighboursNum == 2 || aliveNeighboursNum == 3){
			updatingChunk->nextTurn[nextBuffCellNum++] = index;
		};

		printf("%d	%d	%d\n",index,aliveNeighboursNum,nextBuffCellNum);

		for(int j = 0 ; j < iNeighboursSize; j++){
			printf("%d \t",iNeighbours[j]);
		};
		printf("\n");
	};





	unsigned short* tmp = updatingChunk->nextTurn;
	updatingChunk->nextTurn = updatingChunk->aliveCells;
	updatingChunk->aliveCells = updatingChunk->nextTurn;
	
	updatingChunk->numOfCells = nextBuffCellNum;
	updatingChunk->updated = true;

};
// standard binary search algorithm
int findIndex(chunk* c, int index,int buffOption){

	unsigned short* buffer;

//	printChunk(c);


	switch(buffOption){
		case nextBuff:
			buffer = c->nextTurn;
		default:
			buffer = c->aliveCells;
	};

	int lowerIndex = 0 , upperIndex = c->numOfCells;

	while(lowerIndex <= upperIndex){

		int currentIndex = (lowerIndex + upperIndex)/2;

//		printf("current search index = %hu \n",buffer[currentIndex]);

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

bool hasIndex(chunk *c ,int index){
	return (findIndex(c,index, currBuff) != indexNotFound);
};

bool trimChunk(cordentry* entry){

	//downsize if the current number of cells has been less than half of the array capacity for more than limit number turns 
	//delete the chunk if its been empty for the previous turns 

	chunk* checkedChunk = entry->segment;

	if(checkedChunk->numOfCells == underCapLimit){
		checkedChunk->cellArrSize /= 2;
		checkedChunk->aliveCells = realloc((void*)checkedChunk->aliveCells, checkedChunk->cellArrSize * sizeof(unsigned short)); 
		checkedChunk->nextTurn = realloc((void*)checkedChunk->nextTurn, checkedChunk->cellArrSize * sizeof(unsigned short));

		checkedChunk->underCapTurnNum = 0 ; 
		return true; 
	}

	if(checkedChunk->numOfCells < checkedChunk->cellArrSize/2 && checkedChunk->numOfCells >= startingChunkSize) {
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

void moveIndexToBuff(int index, int comparisonIndex , chunk* c , unsigned short* buff , int* buffPos, int option){

	printf("dead cell check \n");

	int neighbours[8] = {upperLeft , upper, upperRight , left , right , lowerLeft , lower , lowerRight}; 

	for(int i = 0 ; i < 8 ; i++){
		int currentNeighbour = calculateNeighbourIndex(neighbours[i], index);
		if( currentNeighbour < 0) continue;
		if( currentNeighbour < comparisonIndex && hasIndex(c,currentNeighbour))continue;
		buff[*buffPos++] = (unsigned short)index;
		return;
	};
	return;
};

int indexStatus(int index){

	if(index == 0 ) return upperLeft ;
	if(index == chunkLength - 1) return upperRight;
	if(index == chunkLength*chunkLength - chunkLength) return lowerLeft;
	if(index == chunkLength*chunkLength) return lowerRight;

	if(index < chunkLength - 1) return upper;
	if(index > chunkLength*chunkLength - chunkLength) return lower;

	if(index % chunkLength == 0) return left;
	if(index % chunkLength == chunkLength - 1) return right;

	return midCell;
};

void getNeighbours(int index, int* buffer){
	buffer[upperLeft] = calculateNeighbourIndex(index, upperLeft);
	buffer[upper] = calculateNeighbourIndex(index, upper);
	buffer[upperRight] = calculateNeighbourIndex(index, upperRight);
	buffer[left] = calculateNeighbourIndex(index, left);
	buffer[right] = calculateNeighbourIndex(index,right);
	buffer[lowerLeft] = calculateNeighbourIndex(index, lowerLeft);
	buffer[lower] = calculateNeighbourIndex(index, lower);
	buffer[lowerRight] = calculateNeighbourIndex(index, lowerRight);
};

int calculateNeighbourIndex(int index , int dir){

	switch(dir){
		case upperLeft:
			return index - chunkLength - 1;
		case upper:
			return index - chunkLength;
		case upperRight:
			return index - chunkLength + 1;
		case left:
			return index - 1;
		case right:
			return index + 1;
		case lowerLeft:
			return index + chunkLength - 1;
		case lowerRight:
			return index + chunkLength + 1;
		case lower:
			return index + chunkLength;
		default:
			return index;
	};
};

void subtractOverlap(int* iNeighbours,int *neighbourSize, int* dif, int diffSize){

	int res[8] = {noCell};
	int resCount = 0;

	for(int i = 0 ; i < *neighbourSize ; i++){
		bool passed = true;
		for(int j = 0; j < diffSize ; j++){
			if(iNeighbours[i] == dif[j]){
				passed = false;
				break;
			};
		};
		if(passed) res[resCount++] = iNeighbours[i];
	};


	for(int i = 0 ; i < resCount; i++) iNeighbours[i] = res[i];
	*neighbourSize = resCount;
};
