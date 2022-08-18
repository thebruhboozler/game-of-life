#ifndef __UTILS__
#define __UTILS__


float roundUp ( int  num , int multiple); // round up
void globalPcordsToScords(int gpx , int gpy , int* gsx , int gsy); // converts global pixel coordinates to square cordinates
void screenToGlobaPixelCords(int x , int y, int* gx , int * gy); // take the coords from screen space and converts them to global space
int hash(int x, int y); 
void calcChunkCord(int gsx , int gsy , int* gcx , int* gcy);	// calculates chunk coordinates from global square cordinates




#endif
