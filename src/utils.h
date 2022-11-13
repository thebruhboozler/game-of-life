#ifndef __UTILS__
#define __UTILS__

#include "chunkSys.h"

float roundUp(int numToRound, int multiple); // rounds to the nearest mutliple 
void globalPcordsToScords(int gpx , int gpy , int* gsx , int* gsy); // converts global pixel coordinates to square cordinates
void screenToGlobaPixelCords(int x , int y, int* gx , int * gy); // take the coords from screen space and converts them to global space
unsigned int hash(int x, int y); 
void calcChunkCord(int gsx , int gsy , int* gcx , int* gcy);	// calculates chunk coordinates from global square cordinates

#endif
