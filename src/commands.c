#include "commands.h"
#include "chunkSys.h"

#include <stdlib.h>

extern node* head;

void import(void);
void exportf(void);
void clearAll(void);
void teleport(void);
void help(void);


// to do
void import(void){

};
void exportf(void){

};

void clearAll(void){

    node *n = head;

    while (n != NULL){
        deleteChunkNode(n);
        n = n->next;
    };

    head = NULL;

};

void teleport(void){

};
void help(void){

};