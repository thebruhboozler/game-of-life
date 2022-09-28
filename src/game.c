#include "chunkSys.h"
#include "game.h"

#include <stdlib.h>
#include <stdbool.h>


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

	for(int i = 0; i < hashSize; i++){

		cordentry* e = hashTable[i];

		while(e !=NULL){
			updateChunk(e);
			e = e->next;
		};
	};

};

void updateChunk(cordentry* chunk){
	if(chunk->segment->numOfCells == 0){
		chunk->segment->lastUpdated++;
		if(chunk->segment->lastUpdated == 5) deleteChunk(chunk);
		return;
	};
};

int findIndex(chunk* c, int index){

	int lowerIndex = 0;

	int upperIndex = c->numOfCells;


	while(true){

		int currentIndex = (lowerIndex + upperIndex) / 2;

		upperIndex -= (c->aliveCells[currentIndex] > index) * currentIndex;
		lowerIndex += ((c->aliveCells[currentIndex] < index) * currentIndex) + (currentIndex == 0);

		if(c->aliveCells[currentIndex] == index) return currentIndex;

		if(upperIndex <= lowerIndex) return indexNotFound;
	};

};


void toggleCell(chunk* c,int index, int action){
	
	int indexOfIndex = findIndex(c , index);

	switch(action){
		case absoluteOn:
			if(indexOfIndex == indexNotFound) goto turnOn;
			break;
		case absoluteOff:
			if(indexOfIndex != indexNotFound) goto turnOff;
			break;
		default:
			if(indexOfIndex == indexNotFound) goto turnOn;
			else goto turnOff;
			break;
	};

turnOn:

	if(c->numOfCells == c->cellArrSize){
		c->cellArrSize *= 2;
		c->aliveCells = realloc((void*) c-> aliveCells, c->cellArrSize);
		c->prevTurn = realloc((void*) c-> prevTurn , c->cellArrSize);
		c->upSized = true;

		for(int i = c->numOfCells ; i < c->cellArrSize ; i++) c->aliveCells[i] = 0, c->prevTurn[i] = 0;
	};

	int k;

	for( k = 0; k < c->numOfCells ; k++) if(c->aliveCells[k] > index) break;

	for(int i = c->numOfCells ; i > k; i--) c->aliveCells[i] = c->aliveCells[i - 1];

	c->numOfCells++;

	c->aliveCells[k] = index;

	return;

turnOff:

	for(int i = indexOfIndex ; i < c->numOfCells; i++) c->aliveCells[i] = c->aliveCells[i + 1];
	c->numOfCells--;
};
