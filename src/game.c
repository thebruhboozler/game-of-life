#include "chunkSys.h"
#include "game.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

extern cordentry* hashTable[hashSize];
extern bool paused;

int numOfTurns = 0;


void playTurn();
void updateChunk(cordentry* chunk);
void toggleCell(chunk* c,int index, int action);
int findIndex(chunk* c, int index);


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

void updateChunk(cordentry* chunk){
	// delete chunks which have not had a cell for 5 turns
	if(chunk->segment->numOfCells == 0){
		chunk->segment->lastUpdated++;
		if(chunk->segment->lastUpdated == deadChunkLimit) deleteChunk(chunk);
		return;
	};
	//down size chunks which have been out of their range for 5 turns
	if(chunk->segment->upSized) chunk->segment->upSized = false;
	else{
		chunk->segment->lastUpSized--;
		if(chunk->segment->lastUpSized == 0){
			
			if(chunk->segment->numOfCells < chunk->segment->cellArrSize / 2 && chunk->segment->cellArrSize > startingChunkSize + startingChunkSize/2){
				chunk->segment->cellArrSize /= 2;
				chunk->segment->aliveCells = realloc(chunk->segment->aliveCells, chunk->segment-> cellArrSize);
			};
		};
	};

};
// standard binary search algorithm
int findIndex(chunk* c, int index){

	int lowerIndex = 0 , upperIndex = c->numOfCells;
	
	while(lowerIndex <= upperIndex){

		int currentIndex = (lowerIndex + upperIndex)/2;

		if(c->aliveCells[currentIndex] == index) return currentIndex;
		else if( index > c->aliveCells[currentIndex]) lowerIndex = currentIndex + 1;
		else upperIndex = currentIndex - 1;
	};

	return indexNotFound;
};

void toggleCell(chunk* c,int index, int action){
	//to speed up the index finding	
	int indexOfIndex = findIndex(c , index);

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
		c->prevTurn = realloc((void*) c-> prevTurn , c->cellArrSize);
		c->upSized = true;
		c->lastUpSized = upSizeLimit;

		for(int i = c->numOfCells ; i < c->cellArrSize ; i++) c->aliveCells[i] = 0, c->prevTurn[i] = 0;
	};

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
