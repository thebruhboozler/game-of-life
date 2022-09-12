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


void toggleCell(chunk* c,int index, int action){

	switch(action){
		case absoluteOn:
			goto TurnOn;
		case absoluteOff:
			goto TurnOff;
	};


	for(int i = 0; i < c->numOfCells;i++){
		if(c->aliveCells[i] != index) continue;

		int k = 0;
		for(int j = 0; j < c->numOfCells;j++){

			if(c->aliveCells[j] == index) continue;

			c->aliveCells[k++] = c->aliveCells[j];
		};
		c->numOfCells--;
		return;
	};

	if(c->numOfCells >= c->cellArrSize) c->cellArrSize*= 2 , c->aliveCells = realloc(c->aliveCells,c->cellArrSize);

	c->aliveCells[c->numOfCells++] = index;

	return;

TurnOn:

	for(int i = 0; i < c->numOfCells; i++)
		if(index == c->aliveCells[i]) return;

	if(c->numOfCells >= c->cellArrSize) c->cellArrSize*= 2 , c->aliveCells = realloc(c->aliveCells,c->cellArrSize);

	c->aliveCells[c->numOfCells++] = index;

	return;

TurnOff:


	for(int i = 0; i < c->numOfCells;i++){
		if(c->aliveCells[i] != index) continue;

		int k = 0;
		for(int j = 0; j < c->numOfCells;j++){

			if(c->aliveCells[j] == index) continue;

			c->aliveCells[k++] = c->aliveCells[j];
		};
		c->numOfCells--;
		return;
	};

};
