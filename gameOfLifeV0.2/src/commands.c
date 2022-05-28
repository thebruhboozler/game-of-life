#include "commands.h"
#include "chunkSys.h"
#include "gtk/gtk.h"
#include "pthread.h"

#include <stdlib.h>


extern node* head;
extern int chunkNum;

void import(void);
void exportf(void);
void clearAll(void);
void teleport(void);
void help(void);


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

};

void teleport(void){

};
void help(void){

};