#include "shared.h"

void myfree(void * address) {
    Block * used = (Block *)((char*)address - sizeof(Block)); //we create a bock for the memory that is being freed

    used->free = true;
    Block * temp = first;

    while (temp->next != NULL) {
        if (address < temp) {
            temp = temp->next;
        }
    }


}
