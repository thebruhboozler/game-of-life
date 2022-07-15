#include "commands.h"
#include "chunkSys.h"
#include "gtk/gtk.h"

#include <stdlib.h>
#include <stdbool.h>


extern node* head;
extern int chunkNum;
extern int argc;
extern char** argv;
extern cordentry *hashTable[hashSize];
extern int cameraX;
extern int cameraY;

bool helpClicked = false;


void import(void);
void exportf(void);
void clearAll(void);
void teleport(void);
void help(void);
void popUp (GtkWidget *wid, GtkWidget *win);
void* unclickHelp(void* arg);

void import(void){

};
void exportf(void){

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
            cordentry* tmp = e->prev;
            free(e);
            e=e->prev;
        }while(e != NULL);     

        hashTable[i] = NULL;

    };

};

void teleport(void){

    cameraX = rand()%10000;
    cameraY = rand()%10000;

};

void help(void){

    helpClicked = true;

};