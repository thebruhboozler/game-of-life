#include "controls.h"
#include "include/glfw3.h"
#include "misc.h"
#include "chunkSys.h"
#include "commands.h"

#include <stdbool.h>
#include <stdio.h>


#define maxSquareSize 50
#define minSquareSize 4


extern int cameraX;
extern int cameraY;
extern int squareSize;
extern int windowW;
extern int windowH;
extern int menuX;
extern int menuY;
extern int menuSize;
extern int menuOptionsNum;
extern bool helpClicked;
extern bool exportClicked;
extern bool importing;


bool rightClicked = false;
bool paused = true;
bool shiftPressed = false; //checking if shift is pressed
bool selecting = false;
int selectionX, selectionY; // cords where the selection for the export was first clicked 
int clickX,clickY;
int xVelocity = 0,yVelocity = 0;


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


void setControls(GLFWwindow* window){   //setting all the controls 
	glfwSetKeyCallback(window,key_callback);
	glfwSetMouseButtonCallback(window,mouse_button_callback);
	glfwSetScrollCallback(window,scroll_callback);
};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){

	if(key == GLFW_KEY_LEFT_SHIFT){ // checking if the key is left shift and if it is toggoling shiftPressed 
		switch(action){
			case GLFW_PRESS:
				shiftPressed = true;
				break;
			case GLFW_RELEASE:
				shiftPressed = false;
				break;
		}
		return;
	};

	if(key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		paused = !paused;  // checking if the key is space and if it is toggoling paused
		return;
	}

	int velocity = 1 + shiftPressed*4;  //checking if shift is pressed and then setting the velocity
	int keyActive = (action != GLFW_RELEASE);   //checking if the keys have been released . if they have than the var becomes 0

	switch (key){   //setting velocities 
		case GLFW_KEY_W:
			yVelocity = velocity * keyActive;
			break;
		case GLFW_KEY_A:
			xVelocity = -(velocity * keyActive);
			break;
		case GLFW_KEY_S:
			yVelocity = -(velocity * keyActive);
			break;
		case GLFW_KEY_D:
			xVelocity = velocity * keyActive;
			break;
		case GLFW_KEY_ESCAPE:
			helpClicked = false;
			return;
		default:
			break;
	}

	cameraX +=xVelocity;    // adding velocities
	cameraY +=yVelocity;
	
};

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){

	double x,y;
	glfwGetCursorPos(window,&x,&y); //getting cursor postion

	int cursorX = (int) x;

	int cursorY = (int) y; 

	if(action == GLFW_RELEASE) {
		cursorX = notClicked, cursorY = notClicked;
		return;  
	}

	switch (button){
		case GLFW_MOUSE_BUTTON_1:
			clickX = cursorX;
			clickY = cursorY;   //setting clicked postions
			
			if(rightClicked == true && clickX > menuX && clickX < menuX+menuSize && clickY > menuY && clickY < menuY+menuSlotSize*menuOptionsNum){  //checking if click is on the menu 
				manageMenu(clickX,clickY);
				return;
			}
			if(exportClicked == true && selecting == false){

				selecting = true;
				selectionX = clickX;
				selectionY = clickY;
				return;
			}else if(exportClicked == true && selecting == true){
		
				exportClicked = false;
				selecting = false;
				createFile(selectionX, selectionY, clickX,clickY);
				return;
			};
			if(importing == true){
				importStructure(clickX, clickY);
				return;
			};
			handleClicks(clickX,clickY);

			break;
		case GLFW_MOUSE_BUTTON_2:
			clickX = cursorX;
			clickY = cursorY;

			createMenu(clickX,clickY);
			break;
		default:
			break;
	}

};

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
	
	squareSize+= (int)yoffset; //changing the square size so its like zomming in and out

	int squareStatus = (squareSize <= maxSquareSize) - (squareSize >= minSquareSize); //checking if it's within the bounds (from 4 to 50).if it is too large status will be -1 if it's ok status will be 0 and 1 if it's too small 
	
	switch (squareStatus){
		case 0:
			return;
		case -1:
			squareSize = maxSquareSize;
			break;
		case 1:
			squareSize = minSquareSize;
			break;
	}
};
