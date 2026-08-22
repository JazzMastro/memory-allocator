#include "shared.h"

void myfree(void * address) {
    Block * used = (Block *)((char*)address - sizeof(Block)); //we create a bock for the memory that is being freed

    if (first == NULL) {
        first = used;
        used->next = NULL;
        used->prev = NULL;
        return;
    }

    used->free = true;
    used->next = NULL;
    used->prev = NULL;

    if (used < first) {
        used->next = first;
        used->prev = NULL;
        first->prev = used;
        first = used;
        return;
    }

    Block * temp = first;
    while (temp->next != NULL && temp < used) {
        temp = temp->next;
    }

    used->next = temp->next;
    used->prev = temp;
    
    if (temp->next != NULL) {
        temp->next->prev = used;
    }
    temp->next = used;

}
