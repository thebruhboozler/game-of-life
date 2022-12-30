#include "utils.h"
#include "chunkSys.h"

#include <stdlib.h>


extern int squareSize;
extern int windowW;
extern int windowH;
extern int cameraX;
extern int cameraY;


float roundUp(int numToRound, int multiple);
void globalPcordsToScords(int gpx , int gpy , int* gsx , int *gsy); // converts global pixel coordinates to square cordinates
void screenToGlobaPixelCords(int x , int y, int* gx , int * gy); // take the coords from screen space and converts them to global space
unsigned int hash(int x, int y); 
static inline unsigned int advanceNum(unsigned int  seed);
int calculateNeighbourIndex(int index , int dir);
void calcChunkCord(int gsx , int gsy , int* gcx , int* gcy);


float roundUp(int numToRound, int multiple){    //rounds to the nearest multipe of a number
	if (multiple == 0) return numToRound;

	int remainder = abs(numToRound) % multiple;
	if (remainder == 0) return numToRound;

	if (numToRound < 0) return -(abs(numToRound) - remainder);
	else return numToRound + multiple - remainder;
};

void globalPcordsToScords(int gpx , int gpy , int* gsx , int* gsy){

	*gsx = (int)(roundUp(gpx,squareSize) - squareSize) / squareSize + 1;
	*gsy = (int)(roundUp(gpy,squareSize) - squareSize) / squareSize + 2;
};

void screenToGlobaPixelCords(int x , int y, int* gx , int * gy){


	float tcx = x - windowW/2;
	float tcy = windowH/2 - y;

	int shiftX = (-cameraX)%squareSize;
	int shiftY = (-cameraY)%squareSize;

	*gx = cameraX + tcx + shiftX;
	*gy = cameraY + tcy + shiftY;

};

unsigned int hash(int x , int y){


	int xBytes = (x + 0x820361EB6D31F) * 0x11316CD4EC960F;
	
	int yBytes = (y + 0x3D828F4C4810D) * 0x1AA8A49C6E291B;

	unsigned int result = xBytes ^ yBytes;

	result ^= (x ^ y);

	for(int i = 0; i < result % 16 ; i++) result = advanceNum(result);

	result ^= advanceNum(result);

	result *= xBytes - yBytes;

	result |= advanceNum(xBytes) ^ (advanceNum(xBytes) & advanceNum(yBytes));

	return result;

};

static inline unsigned int advanceNum(unsigned int  seed){

	return (0x1ECAA688858293 * seed + 0xE2ABC303DD947) % 0x176798A1742063;

};

void calcChunkCord(int gsx , int gsy , int* gcx , int* gcy){

	*gcx = roundUp(gsx , chunkLength) - chunkLength;  	
	*gcy = roundUp(gsy , chunkLength);

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
