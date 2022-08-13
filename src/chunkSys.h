#ifndef _CHUNKSYS_
#define _CHUNKSYS_

#define chunkSize 10000

#define startingChunkSize 256

#define chunkLength 100

#define noNeighbour NULL

#define hashSize 1024

typedef struct chunk{
	int x,y,numOfCells,cellArrSize;
	unsigned short* aliveCells;
	struct chunk* neighbours[8];
}chunk;


typedef struct node{
	struct node* next;
	struct node* prev;
	chunk* segment;
}node;

typedef struct cordentry{
	int x,y;
	chunk*  segment;
	struct cordentry* next;
	struct cordentry* prev;
}cordentry;


chunk* findCordChunk(int x, int y);
void deleteChunkNode(node* item);
chunk** getVisableChunks(int* len);
void insertChunkNode(chunk* item);
chunk* createChunk(int x,int y);
void handleClicks(int x ,int y);

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
