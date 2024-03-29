#include "renderSys.h"
#include "misc.h"
#include "chunkSys.h"
#include "utils.h"
#include "commands.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>


extern int windowW; // width and height of the window
extern int windowH;
extern int cameraX; // coordinates of the camera
extern int cameraY;
extern int squareSize;  // size of cells
extern bool rightClicked;
extern int menuX,menuY; // menu cords
extern int menuSize;    //menu size as in left to right in pixels
extern int menuOptionsNum;  // number of options
extern char **menuOptionsTxt;   // the text to the menu
extern int chunkNum;
extern GLFWwindow* window;
extern bool helpClicked;


void drawCells(); // draws the cells which are visable on screen
void drawGrid();    // draws the gird lines
void drawMenu(int x,int y);    // draws the menu when you right click
void drawSquare(int x,int y,int sizeX,int sizeY);
void drawText(char* str,int x, int y, float r, float g, float b,void* font); //prints text onto the screen
void display(); // manages the display
char* generateCoordinateString();   // generates the coordinate string
char* generateTurnNumString();  //  generates the turn numString
void helpMenu(); // displays help text
void drawWaypoints();


void display(){

	drawGrid();
	drawCells();
	drawWaypoints();

	char* cords = generateCoordinateString();
	char* turnNum = generateTurnNumString();

	drawText(cords,30,30,0.1,0.9,0.1,GLUT_BITMAP_TIMES_ROMAN_24);   // Drawwing the coordinates and turns to the screen
	drawText(turnNum,30,55,0.1,0.9,0.1,GLUT_BITMAP_TIMES_ROMAN_24);
	
	free((void*)cords);
	free((void*)turnNum);

	if(rightClicked) drawMenu(menuX,menuY); // drawing the menu if right clicked

	if(helpClicked){

		glColor3f(0,0,0);
		drawSquare(0,0,windowW,windowH);
		helpMenu();
		return;
	};

};

void drawCells(){

	glColor3f(1.0,1.0,1.0);

	int len;    // the number of chunks visable

	chunk** screenChunks = getVisableChunks(&len); //double pointer to the chunks

	if(screenChunks == NULL) return;

	for(int i = 0; i < len; i++){
		chunk* visableChunk = screenChunks[i];  // get the chunk

		int px = visableChunk->x*squareSize;    // get its cordinates
		int py = visableChunk->y*squareSize;

		int sx = px - cameraX;  // get their cordinates on the screen
		int sy = py - cameraY;

		for(int j = 0; j < visableChunk->numOfCells; j++){
			int cellIndex = visableChunk->aliveCells[j];    // get the index of a cell

			int scy = (int)(roundUp(cellIndex,chunkLength)-chunkLength)/chunkLength;    // get the y position
			int scx = (cellIndex - ((scy - 1) * chunkLength)) % (chunkLength + 1);  // get the x position of the cell with respect to squareSize 

			int pcx = sx + squareSize * scx;    // get their on screen cordinates in pixels
			int pcy = sy - squareSize * scy;

			int tpx = (pcx + windowW/2);        // translate them from centered cordinates space to top left cordinate space
			int tpy = (windowH/2) - pcy;

			int xOffset = (tpx - (-cameraX) % squareSize) % squareSize; // calculate their off set so that they match the grid
			int yOffset = (tpy + (-cameraY) % squareSize) % squareSize;

			int cpx = tpx - xOffset;    // get final postion 
			int cpy = tpy + yOffset;

			drawSquare(cpx,cpy,squareSize,squareSize); 
		};
	};
	free(screenChunks);
};


void drawGrid(){
	
	int shiftX = (-cameraX)%squareSize; //calculate offset of the camrea to grid
	int shiftY = (-cameraY)%squareSize;

	int verticalLines = (roundUp(squareSize,windowW)/squareSize) + 2;   // calculate the number of vertical lines
	int startingX = shiftX; //start at offset

	glColor3f(0.7,0.1,0.1);

	for(int i = 0 ; i < verticalLines;i++){ // drawing the vertical lines
		float cx = startingX - windowW/2;
		cx = 2*cx/windowW;  // calculate the coordinate of the line and tranlsate it into glfw space
		glBegin(GL_LINE_LOOP);
		glVertex2f(cx,-1.0);    //drawing the line
		glVertex2f(cx,1.0);
		glEnd();
		startingX +=squareSize; // move the line to the right 
	}
	

	int horizontalLines = (roundUp(squareSize,windowH)/squareSize) + 2;
	int startingY = shiftY;

	for(int i = 0 ; i < horizontalLines;i++){   // drawing the horiznotal lines
		float cy = startingY - windowH/2;
		cy = 2*cy/windowH;
		glBegin(GL_LINE_LOOP);
		glVertex2f(1.0,cy);
		glVertex2f(-1.0,cy);
		glEnd();
		startingY +=squareSize;
	}
};

void drawMenu(int x,int y){ // draws menu when you right click

	glColor3f(1.0,1.0,1.0); 
	drawSquare(x,y,menuSize,5+menuSlotSize*menuOptionsNum); // drawing menu background

	double xpos, ypos;

	glfwGetCursorPos(window, &xpos, &ypos);

	float cx = x - windowW/2;
	cx = 2*cx/windowW;  // translate the x position

	float lcx = (x+menuSize) - windowW/2; // translate right x positon
	lcx = 2*lcx/windowW;

	float r,g,b;

	for(int i = 0 ; i < menuOptionsNum;i++){


		r = g = b = 0;

		if(xpos > x && xpos < x + menuSize && ypos > y && ypos  < y +menuSlotSize){
			glColor3f(0.1,0.1,0.8);
			r = 0.8 , g = 0.1 , b = 0.1;
			drawSquare(x,y + 2,menuSize,menuSlotSize + 1);
		};

		drawText(menuOptionsTxt[i],x+2,y + menuSlotSize,r,g,b,GLUT_BITMAP_9_BY_15);
		glColor3f(1.0,1.0,1.0);

		y+=menuSlotSize;    // move down the text

		if(i == menuOptionsNum-1) continue; // dont draw the last line 

		float cy = windowH/2-(y+3);
		cy = 2 * cy/windowH;    // translate y coordinate
		glColor3f(0,0,0);
		glBegin(GL_LINE_LOOP);
		glVertex2f(cx,cy);
		glVertex2f(lcx,cy);
		glEnd();    // drawing the line seperating slots
	}

};

void drawText(char* str,int x, int y, float r, float g, float b,void* font){

	glColor3f(r,g,b);

	float cx = x - windowW/2;
	float cy = windowH/2 - y;

	cx = 2*cx/windowW;
	cy = 2*cy/windowH;  // translate x and y position

	glRasterPos2f(cx,cy);  // set text postion

	int len = strlen(str);

	for(int i = 0; i < len; i++) glutBitmapCharacter(font,str[i]);     // print on screen
};

void drawSquare(int x,int y,int sizeX,int sizeY){
	float cx = x - windowW/2;
	float cy = windowH/2 - y;
	float csizeX = (float)sizeX;
	float csizeY = (float)sizeY;
	float xsize = 2*csizeX/windowW;
	float ysize = 2*csizeY/windowH;

	cx = 2*cx/windowW;
	cy = 2*cy/windowH;

	
	glBegin(GL_POLYGON);
	glVertex2f(cx,cy);
	glVertex2f(cx,cy-ysize);
	glVertex2f(cx+xsize,cy-ysize);
	glVertex2d(cx+xsize,cy);
	glEnd();
};

void drawWaypoints(){

	//static variables to keep track of color which will wave
	// r = 0* , g = 45* and b = 90*
	static float rStart = 0 , gStart =  0.785398 ,bStart = 1.5708;


	// 1 degree in radians is added to each one of them
	rStart += 2 * 0.0174532925;
	gStart += 1.25 * 0.0174532925;
	bStart += 0.5 * 0.0174532925;


	float r = fabs(sin(rStart)) , g = fabs(sin(gStart)) , b = fabs(sin(bStart));

	int len;

	waypoint** visibleWaypoints = getVisibleWaypoints(&len);

	glColor3f(r ,g ,b);
	
	for(int i = 0; i < len; i++){

		float onScreenX = visibleWaypoints[i]->x - cameraX;
		float onScreenY = visibleWaypoints[i]->y - cameraY;

		float lx = onScreenX - 9 *(squareSize * 0.1);
		float uy = onScreenY + 21*(squareSize * 0.1);
		float ly = onScreenY - 9 *(squareSize * 0,1);
		float rx = onScreenX + 9 *(squareSize * 0.1);

		float clx = 2*lx/windowW;
		float cuy = 2*uy/windowH;
		float cly = 2*ly/windowH;
		float crx = 2*rx/windowW;

		float cx = 2*onScreenX/windowW;
		float cy = 2*onScreenY/windowH;

		glBegin(GL_POLYGON);
		glVertex2f(clx,cy);
		glVertex2f(cx,cuy);
		glVertex2f(crx,cy);
		glVertex2f(cx,cly);
		glEnd();
		
	};

	glColor3f(0,0,0);

	free(visibleWaypoints);
};

char* generateCoordinateString(){

	char str[50];    //allocate memory for string

	sprintf(str,"X: %d  Y: %d",cameraX,cameraY);  // fromat string

	int len = strlen(str); 

	char* result = calloc(len,sizeof(char));   // alloacte the proper ammount of memory for the string

	strcpy(result,str); //copy it into the normal sized string 

	return result;
};

char* generateTurnNumString(){

	char str[50];    //allocate memory for string

	sprintf(str,"Turns: %d",0);  // fromat string

	int len = strlen(str); 

	char* result = calloc(len,sizeof(char));   // alloacte the proper ammount of memory for the string

	strcpy(result,str); //copy it into the normal sized string 

	return result;
};

void helpMenu(){
	
	drawText("W - move up ", 20, 20 , 1.0 ,1.0, 1.0, GLUT_BITMAP_TIMES_ROMAN_24);
	drawText("S - move down ", 20, 50 , 1.0 ,1.0, 1.0, GLUT_BITMAP_TIMES_ROMAN_24);
	drawText("A - move left ", 20, 80 , 1.0 ,1.0 , 1.0, GLUT_BITMAP_TIMES_ROMAN_24);
	drawText("D - move right ", 20, 110 , 1.0 ,1.0 , 1.0, GLUT_BITMAP_TIMES_ROMAN_24);

	drawText("Left Shift - move quickly ", 20, 140 , 1.0 ,1.0 , 1.0, GLUT_BITMAP_TIMES_ROMAN_24);

	drawText("Space - pause/unpause ", 20, 170 , 1.0 ,1.0 , 1.0, GLUT_BITMAP_TIMES_ROMAN_24);

	drawText("Left Mouse Button - toggle cells ", 20, 200 , 1.0 ,1.0 , 1.0, GLUT_BITMAP_TIMES_ROMAN_24);

	drawText("Right Mouse Button - pop up menu ", 20, 230 , 1.0 ,1.0 , 1.0, GLUT_BITMAP_TIMES_ROMAN_24);

	drawText("Escape - leave help menu ", 20, 260 , 1.0 ,1.0 , 1.0, GLUT_BITMAP_TIMES_ROMAN_24);
};
