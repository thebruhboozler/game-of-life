#include "chunkSys.h"
#include "game.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>


extern node* head;
extern bool paused;

int numOfTurns = 0;


void playTurn();
void updateChunk(node* chunk);
void toggleCell(chunk* c,int index);


void playTurn(){
	if(!paused) return;

	numOfTurns++;

	node* n = head;

	while(n != NULL){
		updateChunk(n);
		n = n->next;
	};
};

void updateChunk(node* chunk){
	if(chunk->segment->numOfCells == 0){
		chunk->segment->lastUpdated++;
		if(chunk->segment->lastUpdated == 5) deleteChunkNode(chunk);
		return;
	}
};

void toggleCell(chunk* c,int index){

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
};
