#ifndef _COMMANDS_
#define _COMMANDS_

#define sizeOfwaypoints 64

#include <stdio.h>

void import(void);
void exportf(void);
void clearAll(void);
void teleport(void);
void help(void);
void addWaypoint(void);
void clearWaypoint(void);
void createFile(int startX, int startY , int endX , int endY);
void importStructure(FILE *fp, int x , int y);

typedef struct waypoint{
    int x,y;
}waypoint;

#endif 
