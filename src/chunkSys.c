#include "chunkSys.h"
#include "renderSys.h"
#include "game.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


extern int cameraX;
extern int cameraY;
extern int squareSize;
extern int windowW;
extern int windowH;

int chunkNum = 0;
node* head  = NULL;
cordentry *hashTable[hashSize];


chunk* createChunk(int x,int y);
void insertChunkNode(chunk* item);
void deleteChunkNode(node* item);
chunk** getVisableChunks(int* len);
void handleClicks(int x ,int y);
void enterCord(chunk *c);
cordentry* findCord(int x, int y);
void deleteEntry(int x, int y);
chunk* findCordChunk(int x, int y);


void insertChunkNode(chunk* item){

	node* tmp = calloc(1,sizeof(node));

	if(head == NULL){ // check if we have allocated the head
		head = tmp;
		tmp->segment = item;
		tmp->prev = NULL;
		tmp->next = NULL;
		return;
	};

	tmp->next = head;   // insert the element
	head->prev = tmp;
	tmp->prev = NULL;
	tmp->segment = item;
	head = tmp;
};

void deleteChunkNode(node* item){
	
	int x = item->segment->x;
	int y = item->segment->y;

	free(item->segment->aliveCells);
	free(item->segment);

	node* tmp = item->prev; //take the previous node
	if(tmp == NULL){    // check if it is the head 
		head = item->next;
		if(head != NULL) head->prev = NULL;
	}else{
		tmp->next = item->next;
		if(tmp->next) item->next->prev = tmp; //check if it is the tail
	}
	
	free(item);

	deleteEntry(x,y);
};

chunk* createChunk(int x,int y){

	chunkNum++;

	chunk* tmp = (chunk*) calloc(sizeof(chunk),1);  // alloacte chunk on the heap

	tmp->aliveCells = (unsigned short*) calloc(sizeof(unsigned short),startingChunkSize);

	tmp->x = x;
	tmp->y = y;
	tmp->numOfCells = 0;
	tmp->cellArrSize = startingChunkSize;   //set parameters

	for(int i = 0; i < 8; i++) tmp->neighbours[i] = noNeighbour;  // by default no neigbours

	if(head == NULL) return tmp;   // if head is null then we know that there aren't any other chunks and it wont have neigbours

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

	enterCord(tmp);

	return tmp;
};

chunk** getVisableChunks(int* len){

	chunk* tmp[256]; 

	int chunkCount = 0;

	node* n = head;

	if(n == NULL) return NULL;

	int squareX = (int) roundUp(cameraX,squareSize)/squareSize;
	int squareY = (int) roundUp(cameraY,squareSize)/squareSize;

	int numOfHorSquares = (int) roundUp(windowW,squareSize)/squareSize;
	int numOfVerSquares = (int) roundUp(windowH,squareSize)/squareSize;

	int leftX = squareX - chunkLength - numOfHorSquares/2;
	int lowerY= squareY - chunkLength - numOfVerSquares/2;

	int rightX = squareX + numOfHorSquares/2;
	int upperY = squareY + chunkLength + numOfVerSquares/2;


	while(n != NULL){ // go throught the linked list
		int x = n->segment->x;
		int y = n->segment->y;

		if(x >  leftX && x < rightX && y > lowerY && y < upperY){
			tmp[chunkCount] = n->segment;
			chunkCount++;
		};

		n = n->next;
	};

	*len = chunkCount;  //set len

	chunk** result = calloc(sizeof(chunk*),*len);

	for(int i = 0 ; i < *len;i++) result[i] = tmp[i];   // copy results

	return result;
};

void handleClicks(int x ,int y){

	float tcx = x - windowW/2;
	float tcy = windowH/2 - y;

	int cx = (int) tcx;
	int cy = (int) tcy;

	int shiftX = (-cameraX)%squareSize;
	int shiftY = (-cameraY)%squareSize;

	int sx = cameraX + cx + shiftX;
	int sy = cameraY + cy + shiftY;

	int squareX = (int)(roundUp(sx,squareSize) - squareSize) / squareSize + 1;
	int squareY = (int)(roundUp(sy,squareSize) - squareSize) / squareSize + 2;

	int targetX = (int) roundUp(squareX,chunkLength) - chunkLength;
	int targetY = (int) roundUp(squareY,chunkLength);

	int ix = squareX - targetX;
	int iy = targetY - squareY;
	unsigned short index = (chunkLength*iy)+ix;

	chunk* clickedChunk = findCordChunk(targetX,targetY);

	if(clickedChunk != NULL){
		toggleCell(clickedChunk,index);
		return;
	};
	
	chunk* newChunk = createChunk(targetX,targetY);

	insertChunkNode(newChunk);

	newChunk->numOfCells++;

	newChunk->aliveCells[0] = index;
};

unsigned int hash(int x, int y){
	
	y = y << 3;
	x *= y;
	y ^= x;
	x &= y;
	return ((x*y) | (x - y)) ^ ((x + y*y));
};


void enterCord(chunk *c){

	int slot = hash(c->x, c->y) % hashSize;

	cordentry* cord = (cordentry*) calloc(1,sizeof(cordentry));

	cord->x = c->x;
	cord->y = c->y;
	cord->segment = c;  // copy data
	cord->next = NULL;
	cord->prev = NULL;

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
	
	if(temp->prev != NULL) temp->prev->next = temp->next;
	else{

		int slot = hash(x , y) % hashSize;

		hashTable[slot] = temp->next; 
	};

	free(temp);
};
