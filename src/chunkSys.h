#ifndef _CHUNKSYS_
#define _CHUNKSYS_

#define chunkSize 10000

#define startingChunkSize 256

#define chunkLength 256 

#define noNeighbour NULL

#define hashSize 1024

typedef struct chunk{
	int x,y,numOfCells,cellArrSize,underCapTurnNum,inactiveTurnNum;
	unsigned short* aliveCells;
	unsigned short* prevTurn;
	struct chunk* neighbours[8];
}chunk;

typedef struct cordentry{
	int x,y;
	chunk*  segment;
	struct cordentry* next;
	struct cordentry* prev;
}cordentry;


chunk* findCordChunk(int x, int y);
void handleClicks(int x ,int y);
chunk** getVisableChunks(int* len);
chunk* createChunk(int x,int y);
void deleteChunk(cordentry* e);
void enterCord(chunk* c);


enum{
	upperLeft,
	upper,
	upperRight,
	left,
	right,
	lowerLeft,
	lower,
	lowerRight
};


#endif
