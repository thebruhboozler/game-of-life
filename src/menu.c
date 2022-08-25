#include "misc.h"
#include "controls.h"
#include "commands.h"
#include "utils.h"

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


void (*menuFunc[64])(void); // function pointers for the menu options
char **menuOptionsTxt;  // text of each option
int menuSize = 0;   // size of the pop up menu horizontally  
int menuOptionsNum = 0; // number of options
int menuX,menuY;    // screen coordinates of the menu

extern bool rightClicked;   // checking if right clicked 

void createMenu(int x, int y);  // sets the appropriate settings to create a menu 
void manageMenu(int x,int y);   // manages the menu
void menuInit();    // initializes the menu
void addMenuOptions(char* str,void (*func)(void));  // adds menu options



void menuInit(){
	menuOptionsTxt = calloc(64,sizeof(char*));
	addMenuOptions("Import",import);
	addMenuOptions("Export",exportf);
	addMenuOptions("Clear All",clearAll);
	addMenuOptions("Teleport",teleport);
	addMenuOptions("Add waypoint",addWaypoint);
	addMenuOptions("Clear waypoints",clearWaypoint);
	addMenuOptions("Help",help);
};

void addMenuOptions(char* str,void (*func)(void)){

	int len = strlen(str);

	int n = (len*9)+2;

	if(menuSize < n) menuSize = n;

	menuFunc[menuOptionsNum] = func;
	menuOptionsTxt[menuOptionsNum] = calloc(len,sizeof(char));
	strcpy(menuOptionsTxt[menuOptionsNum++],str);
};


void createMenu(int x, int y){
	if(x == notClicked) return;

	menuX = x, menuY = y;
	
	rightClicked = !rightClicked;
	return;
};

void manageMenu(int x ,int y){
	
	rightClicked = false;

	int ry = y - menuY;

	int option = (roundUp(menuSlotSize,ry)/menuSlotSize);

	(*menuFunc[option])();
};