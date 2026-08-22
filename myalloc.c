#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include "shared.h"

#define MIN_BLOCK_SIZE sizeof(Block)


void * heap_start = NULL;
Block * first = NULL;

int init(int size, void **start) {

    *start = mmap (NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    // ... use the memory here ...

    if (*start == MAP_FAILED) {
        perror("munmap failed");
        return 1;
    }

    printf("Got memory at: %p\n", *start);

    first = (Block *)*start; //memory for first is now at start pointer

    //initialize
    first->size = size - sizeof(Block);
    first->free = true;
    first->next = NULL;
    first->prev = NULL;

    heap_start = first; //heap_start stores address of first block

    return 0;
}

void split(Block * current, Block ** first, int size) {

    int leftover = current->size - (size + sizeof(Block));

    // not enough room left over to make a usable free block —
    // give the whole current block to the caller instead of splitting
    if (leftover < (int)MIN_BLOCK_SIZE) {
        if (current == *first) {
            *first = current->next;
        }
        if (current->prev != NULL) current->prev->next = current->next;
        if (current->next != NULL) current->next->prev = current->prev;
        current->free = false;
        return;
    }

    Block * NewBlock = (Block*)((char*)current + sizeof(Block) + size); //char cast to get correct address displacement for next block

    if (current == *first) { //if our current block is the first block in the list
        *first = NewBlock;
    }

    NewBlock->size = leftover;
    NewBlock->free = true;
    

    NewBlock->next = current->next;
    NewBlock->prev = current->prev;
    if (current->next != NULL) current->next->prev = NewBlock;
    if (current->prev != NULL) current->prev->next = NewBlock;
    current->size = size;
    current->free = false;

}

void moreSpace(int x, int size, Block ** traverse, Block ** newMem) {

    if (x + (int)sizeof(Block) > size) size = x + sizeof(Block);

    void *memStart = mmap (NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (memStart == MAP_FAILED) {
        perror("mmap failed");
        *newMem = NULL;
        return;
    }

    *newMem = (Block *)memStart;

    //initialize new block

    (*newMem)->size = size - sizeof(Block);
    (*newMem)->free = true;
    (*newMem)->next = NULL;
    (*newMem)->prev = *traverse;
    (*newMem)->prev->next = *newMem;

    *traverse = *newMem;

}

void *myalloc(int x) {

    //starting pointer and size (bytes)

    if (heap_start == NULL) {
        void *start;
        size_t size = 4096;

        //condition check
        int res = init(size, &start);
        if (res == 1) exit(0);
    }
    
   
    //find block with space
    Block * traverse = first;
    while (traverse->next != NULL) {
        if (traverse->size < (x + sizeof(Block))) { //if size available is left then size we want
            traverse = traverse->next;
        } else {
            break; //when we find block with enough space
        }
    }

    if (traverse->next == NULL && traverse->size < (x + sizeof(Block))) { // no available blocks work
        Block * newMem = NULL;
        size_t size = 4096;
        moreSpace(x, size, &traverse, &newMem);
        if (traverse == NULL) {
            return NULL; // out of memory
        }
    }
    
    //split current block into return block of requested size and block with leftoversize
    split(traverse, &first, x);

    //same logic as before
    //we return the adress that is the size of metadata past traverse's starting address
    return (char*)traverse + sizeof(Block);
}
