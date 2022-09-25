#include "chunkSys.h"
#include "utils.h"
#include "game.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
 

extern int cameraX;
extern int cameraY;
extern int squareSize;
extern int windowW;
extern int windowH;

int chunkNum = 0;
cordentry *hashTable[hashSize];


chunk* createChunk(int x,int y);
chunk** getVisableChunks(int* len);
void handleClicks(int x ,int y);
void enterCord(chunk *c);
cordentry* findCord(int x, int y);
void deleteEntry(int x, int y);
chunk* findCordChunk(int x, int y);
void deleteChunk(cordentry* e);


chunk* createChunk(int x,int y){

	chunkNum++;

	chunk* tmp = (chunk*) calloc(sizeof(chunk),1);  // alloacte chunk on the heap

	tmp->aliveCells = (unsigned short*) calloc(sizeof(unsigned short),startingChunkSize);
	tmp->prevTurn = (unsigned short*) calloc(sizeof(unsigned short), startingChunkSize);

	tmp->x = x;
	tmp->y = y;
	tmp->upSized = false;
	tmp->numOfCells = 0;
	tmp->lastUpSized = 0;
	tmp->cellArrSize = startingChunkSize;   //set parameters

	for(int i = 0; i < 8; i++) tmp->neighbours[i] = noNeighbour;  // by default no neigbours

	int rightX = x + chunkLength;
	int leftX = x - chunkLength;    // neighbour x and y cords

	int upperY = y + chunkLength;
	int lowerY = y - chunkLength;

	cordentry* neighbour = findCord(leftX,upperY);
	if(neighbour != NULL) tmp->neighbours[upperLeft] = neighbour -> segment;    // finding the neighbours
	neighbour = findCord(x,upperY);
	if(neighbour != NULL) tmp->neighbours[upper] = neighbour -> segment;
	neighbour = findCord(rightX,upperY);
	if(neighbour != NULL) tmp->neighbours[upperRight] = neighbour -> segment;
	neighbour = findCord(leftX,y);
	if(neighbour != NULL) tmp->neighbours[left] = neighbour -> segment;
	neighbour = findCord(rightX,y);
	if(neighbour != NULL) tmp->neighbours[right] = neighbour -> segment;
	neighbour = findCord(leftX,lowerY);
	if(neighbour != NULL) tmp->neighbours[lowerLeft] = neighbour -> segment;
	neighbour = findCord(x,lowerY);
	if(neighbour != NULL) tmp->neighbours[lower] = neighbour -> segment;
	neighbour = findCord(rightX,lowerY);
	if(neighbour != NULL) tmp->neighbours[lowerRight] = neighbour -> segment;

	return tmp;
};

chunk** getVisableChunks(int* len){

	chunk* tmp[256]; 

	int chunkCount = 0;

	int squareX, squareY ;
	globalPcordsToScords( cameraX , cameraY , &squareX , &squareY);

	int numOfHorSquares = (int) roundUp(windowW,squareSize)/squareSize;
	int numOfVerSquares = (int) roundUp(windowH,squareSize)/squareSize;

	int leftX = squareX - chunkLength - numOfHorSquares/2;
	int lowerY= squareY - chunkLength - numOfVerSquares/2;

	int rightX = squareX + numOfHorSquares/2;
	int topY = squareY + chunkLength + numOfVerSquares/2;

	int topLeftChunkX , topLeftChunkY, lowerRightChunkX , lowerRightChunkY;

	calcChunkCord(leftX ,topY ,&topLeftChunkX ,&topLeftChunkY);
	calcChunkCord(rightX, lowerY, &lowerRightChunkX ,&lowerRightChunkY);

	int topLeftChunkXStart = topLeftChunkX;

	for(topLeftChunkY; topLeftChunkY >= lowerRightChunkY - chunkLength; topLeftChunkY -= chunkLength){

		for(topLeftChunkX = topLeftChunkXStart; topLeftChunkX < lowerRightChunkX + chunkLength ; topLeftChunkX += chunkLength){

			chunk* temp = findCordChunk(topLeftChunkX ,topLeftChunkY);

			if(temp == NULL) continue;
			tmp[chunkCount++] = temp;
		};
	};

	*len = chunkCount;  //set len

	chunk** result = calloc(sizeof(chunk*),*len);

	for(int i = 0 ; i < *len;i++) result[i] = tmp[i];   // copy results

	return result;
};

void handleClicks(int x ,int y){

	int sx,sy;
	screenToGlobaPixelCords( x , y , &sx , &sy);

	int squareX, squareY ;
	globalPcordsToScords( sx , sy , &squareX , &squareY);

	int targetX,targetY;
	calcChunkCord( squareX , squareY , &targetX , &targetY);

	int ix = squareX - targetX;
	int iy = targetY - squareY;
	unsigned short index = (chunkLength*iy)+ix;

	chunk* clickedChunk = findCordChunk(targetX,targetY);

	if(clickedChunk != NULL){
		toggleCell(clickedChunk, index, cellToggle);
		return;
	};

	chunk* newChunk = createChunk(targetX,targetY);

	enterCord(newChunk);

	toggleCell(newChunk,index,absoluteOn);
};

void enterCord(chunk *c){

	int slot = hash(c->x, c->y) % hashSize;

	cordentry* cord = (cordentry*) calloc(1,sizeof(cordentry));

	cord->x = c->x;
	cord->y = c->y;
	cord->segment = c;  // copy data

	if(hashTable[slot] != NULL){    //check if the slot is avaliable 

		cordentry* e = hashTable[slot];
		
		while(e->next != NULL) e = e->next;

		e->next = cord;

		cord->prev = e;

		return;
	};

	hashTable[slot] = cord; // populate the slot
};

cordentry*  findCord(int x, int y){

	int slot = hash(x, y) % hashSize;

	cordentry* entry = hashTable[slot];

	while(entry != NULL){   // check if its in the slot
		if(entry->x == x && entry->y == y) return entry;
		entry = entry -> next;  
	};

	return NULL;
};

chunk* findCordChunk(int x, int y){
	
	cordentry* tmp = findCord(x , y);

	if(tmp == NULL) return NULL;
	return tmp->segment;
};

void deleteEntry(int x, int y){
	cordentry* temp = findCord(x,y);

	if(temp == NULL) return;
	
	if(temp->prev != NULL) temp->prev->next = temp->next;
	else{

		int slot = hash(x , y) % hashSize;

		hashTable[slot] = temp->next; 
	};

	free(temp);
};

void deleteChunk(cordentry* e){
	deleteEntry(e->segment->x , e->segment->y);
};
