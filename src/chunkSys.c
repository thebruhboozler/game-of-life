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


chunk* createChunk(int x,int y);
void insertChunkNode(chunk* item);
void deleteChunkNode(node* item);
chunk** getVisableChunks(int* len);
void handleClicks(int x ,int y);


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
};

chunk* createChunk(int x,int y){

    chunkNum++;

    chunk* tmp = (chunk*) calloc(sizeof(chunk),1);  // alloacte chunk on the heap

    tmp->aliveCells = (short*) calloc(sizeof(unsigned short),startingChunkSize);

    tmp->x = x;
    tmp->y = y;
    tmp->numOfCells = 0;
    tmp->cellArrSize = startingChunkSize;   //set parameters

    for(int i = 0; i < 8; i++) tmp->neighbours[i] = noNeighbour;  // by default no neigbours

    node* n = head;

    if(n == NULL) return tmp;   // if n is null than we know that there aren't any other chunks and it wont have neigbours

    int rightX = x + chunkLength;
    int leftX = x - chunkLength;    // neighbour x and y cords

    int upperY = y + chunkLength;
    int lowerY = y - chunkLength;

    int neigbourCount = 0;

    while(n->next != NULL){ // go through the linked list and check the chunks if they're neighbouring

        int currentX = n->segment->x;
        int currentY = n->segment->y;
        n = n->next;

        int neigbourIndex = 0;

        if(currentX == leftX && currentY == upperY) neigbourIndex = upperLeft;
        else if(currentX == x && currentY == upperY) neigbourIndex = upper;
        else if(currentX == rightX && currentY == upperY) neigbourIndex = upperRight;
        else if(currentX == leftX && currentY == y) neigbourIndex = left;
        else if(currentX == rightX && currentY == y) neigbourIndex = right;
        else if(currentX == leftX && currentY == lowerY) neigbourIndex = lowerLeft;
        else if(currentX == x && currentY == lowerY) neigbourIndex = lowerLeft;
        else if(currentX == rightX && currentY == lowerY) neigbourIndex = lowerLeft;
        else continue;

        tmp->neighbours[neigbourIndex] = n->segment;

        neigbourCount++;
        if(neigbourCount == 8) return tmp; // if we have 8 neigbours stop looking
    };

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

    for(int i = 0 ; i < *len;i++) result[i] = tmp[i];

    return result;
};

void handleClicks(int x ,int y){

    float tcx = x - windowW/2;
    float tcy = windowH/2 - y;

    int cx = (int) tcx;
    int cy = (int) tcy;

    int sx = cameraX + cx;
    int sy = cameraY + cy;

    int squareX = (int)(roundUp(sx,squareSize) - squareSize) / squareSize + 1;
    int squareY = (int)(roundUp(sy,squareSize) - squareSize) / squareSize + 2;

    int targetX = (int) roundUp(squareX,chunkLength) - chunkLength;
    int targetY = (int) roundUp(squareY,chunkLength);

    int ix = squareX - targetX;
    int iy = targetY - squareY;
    unsigned short index = (chunkLength*iy)+ix;

    node* n = head;

    while(n != NULL){
        if(n->segment->x == targetX && n->segment->y == targetY){
            toggleCell(n->segment,index);
            return;
        }
        n = n->next;
    };
    
    chunk* newChunk = createChunk(targetX,targetY);

    insertChunkNode(newChunk);

    newChunk->numOfCells++;

    newChunk->aliveCells[0] = index;
};