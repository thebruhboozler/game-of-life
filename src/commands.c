#include "commands.h"
#include "chunkSys.h"
#include "gtk/gtk.h"
#include "utils.h"

#include <stdlib.h>
#include <stdbool.h>


extern node* head;
extern int chunkNum;
extern cordentry *hashTable[hashSize];
extern int cameraX;
extern int cameraY;
extern int squareSize;
extern int windowH;
extern int windowW;

bool helpClicked = false;
bool exportClicked = false;
int currentWaypoint = 0;
waypoint waypoints[sizeOfwaypoints];
int numOfWaypoints = 0;


void import(void);
void exportf(void);
void clearAll(void);
void teleport(void);
void help(void);
void addWaypoint(void);
void clearWaypoint(void);
void createFile(int startX, int startY, int endX, int endY);


void import(void){

};

void exportf(void){

	exportClicked = true;

};

void createFile(int startX,int startY, int endX, int endY){

	char *filename;

	// choose file to save to

	GtkWidget *dialog;
	GtkFileChooser *chooser;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
	gint res;

	dialog = gtk_file_chooser_dialog_new("Save File", NULL, action, "_Cancel", GTK_RESPONSE_CANCEL ,"_Save" ,GTK_RESPONSE_ACCEPT, NULL);
	chooser = GTK_FILE_CHOOSER(dialog);

	gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);

	gtk_file_chooser_set_current_name(chooser, "Untitled document.txt");

	res = gtk_dialog_run(GTK_DIALOG(dialog));
	if (res == GTK_RESPONSE_ACCEPT) filename = gtk_file_chooser_get_filename(chooser);
	

	gtk_widget_destroy(dialog);


	// translate cords from screen space to global space

	int gx,gy,egx,egy;
	
	screenToGlobaPixelCords( startX , startY , &gx , &gy);
	screenToGlobaPixelCords( endX , endY , &egx , &egy);

	//convert the cords from global pixel space to global square space
	//need to ensure that we select the correct chunks 
	globalPcordsToScords( startX , startY , &startX , &startY);
	globalPcordsToScords( endX , endY , &endX , &endY);

	//calculate the top left and the bottom right coordintades of the rectange we've been provided
	//needed to ensure that we are able select the chunks from the top left to bottom right 
	//the user may select the area inversly so we need to check

	int topLeftX = (startX < endX) * startX + (endX < startX) * endX + (startX == endX) * startX; //take the lowest x value

	int topLeftY = (startY > endY) * startY + (endY > startY) * endY + (startY == endY) * startY; //take the highest y value

	int lowerRightX = (topLeftX != startX) * startX + (topLeftX != endX) * endX;

	int lowerRightY = (topLeftY != startY) * startY + (topLeftY != endY) * endY;

	int topLeftChunkX, topLeftChunkY , lowerRightChunkX , lowerRightChunkY;

	calcChunkCord( topLeftX , topLeftY , &topLeftChunkX , &topLeftChunkY );
	calcChunkCord( lowerRightX , lowerRightY , &lowerRightChunkX , &lowerRightChunkY);

	int crossedChunkWidth = ((lowerRightChunkX - topLeftChunkX)/chunkLength ) + (topLeftChunkX == lowerRightChunkX);
	int crossedChunkHeight =((topLeftChunkY - lowerRightChunkY)/chunkLength ) + (topLeftChunkY == lowerRightChunkY);

	chunk** chunks = (chunk**) calloc( crossedChunkWidth * crossedChunkHeight , sizeof(chunk**));
	int chunkCount = 0;
	
	int numOfCells = 0;

	for(int i = 0; i < crossedChunkWidth;i++){

		for(int j = 0 ; j < crossedChunkHeight; j++){

			chunk* c = findCordChunk( topLeftChunkX, topLeftChunkY);
			
			topLeftChunkX += chunkLength;

			if(c == NULL) continue;

			chunks[chunkCount++] = c;

			numOfCells += c->numOfCells;

		};
		topLeftChunkY -= chunkLength;
	};


	unsigned short* cellIndexes = (unsigned short*) calloc(numOfCells, sizeof(unsigned short));
	int cellCount = 0;

	int squareWidth = lowerRightX - topLeftX;

	int squareHeight = topLeftY - lowerRightY;

	for(int i = 0; i < chunkCount; i++){
		
		chunk* tmp = chunks[i];

		for(int j = 0; j < tmp->numOfCells; j++){
			
			int index = tmp->aliveCells[j];

			int cellGlobalX = tmp->x + index%chunkLength;

			int cellGlobalY = tmp->y - (int) (roundUp(index,chunkLength) - chunkLength) / chunkLength;

			// checking if the current cell selected is within the selected area
			if(cellGlobalX < topLeftX || cellGlobalX > lowerRightX || cellGlobalY > topLeftY || cellGlobalY < lowerRightY) continue;

			//calculate the index within the selected area

			unsigned short selectedX = (unsigned short) cellGlobalX - topLeftX;

			unsigned short selectedY = (unsigned short) topLeftY - cellGlobalY;

			cellIndexes[cellCount++] = (unsigned short)((selectedY - 1)* squareWidth + selectedX); 

		};

	};

	// move the data to a file
	FILE* fp = fopen(filename,"w");

	fprintf(fp,"%d %d ",squareWidth,squareHeight);

	g_free(filename);

	for(int i = 0; i < cellCount; i++) fprintf(fp," %d ", cellIndexes[i]);

	free(chunks);
	free(cellIndexes);
};


void clearAll(void){

	GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;

	GtkWidget *dialog = gtk_message_dialog_new (NULL,flags,GTK_MESSAGE_QUESTION,GTK_BUTTONS_YES_NO,"Are you sure you want to clear all ?");

	int result = gtk_dialog_run (GTK_DIALOG (dialog));

	gtk_widget_destroy(dialog);

	if(result != GTK_RESPONSE_YES) return;

	chunkNum = 0;

	node* n = head;

	while (n != NULL){
		free(n->segment->aliveCells);
		free(n->segment);
		node* l = n->next;
		free(n);
		n = l;
	};

	head = NULL;

	for(int i = 0 ; i < hashSize;i++){

		cordentry* e = hashTable[i];

		if(e == NULL) continue;

		while(e->next != NULL) e = e->next;

		do{ 
			free(e);
			e=e->prev;
		}while(e != NULL);     

		hashTable[i] = NULL;

	};

};

void teleport(void){

	if(numOfWaypoints == 0) return;

	cameraX = waypoints[currentWaypoint % numOfWaypoints].x;
	cameraY = waypoints[currentWaypoint++ % numOfWaypoints].y;
};

void help(void){

	helpClicked = true;

};

void addWaypoint(void){

	waypoints[numOfWaypoints].x = cameraX;
	waypoints[numOfWaypoints++].y = cameraY;

};

void clearWaypoint(void){

	numOfWaypoints = 0;
	currentWaypoint = 0;
};