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

    if (*start == MAP_FAILED) {
        perror("mmap failed");
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

    // not enough room left over to make a usable free block,
    // so give the whole current block to the caller instead of splitting
    if (current->size < size + sizeof(Block) + MIN_BLOCK_SIZE) {
        if (current == *first) {
            *first = current->next;
        }
        if (current->prev != NULL) current->prev->next = current->next;
        if (current->next != NULL) current->next->prev = current->prev;
        current->next = NULL;   // optional: clears stale links
        current->prev = NULL;   // optional: clears stale links
        current->free = false;
        return;
    }

    size_t leftover = current->size - size - sizeof(Block);

    Block * NewBlock = (Block*)((char*)current + sizeof(Block) + size);

    if (current == *first) {
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
    if (*traverse != NULL) {
        (*traverse)->next = *newMem;
    } else {
        first = *newMem;
    }

    *traverse = *newMem;

}

void *myalloc(int x) {

    //starting pointer and size (bytes)
    if (x <= 0) return NULL;
    int aligned_size = (x + 15) & ~15; //rounds up to next multiple of 16

    if (heap_start == NULL) {
        void *start;
        size_t size = 4096;

        //condition check
        int res = init(size, &start);
        if (res == 1) return NULL;
    }
    
   
    //find block with space
    if (first == NULL) {
        Block *t = NULL;
        Block *fresh = NULL;
        moreSpace(aligned_size, 4096, &t, &fresh);
        if (fresh == NULL) return NULL;
    }

    Block * traverse = first;
    while (traverse->next != NULL) {
        if (traverse->size < aligned_size) { //if size available is left then size we want
            traverse = traverse->next;
        } else {
            break; //when we find block with enough space
        }
    }

    if (traverse->next == NULL && traverse->size < aligned_size) { // no available blocks work
        Block * newMem = NULL;
        size_t size = 4096;
        moreSpace(aligned_size, size, &traverse, &newMem);
        if (newMem == NULL) {
            return NULL; // out of memory
        }
    }
    
    //split current block into return block of requested size and block with leftoversize
    split(traverse, &first, aligned_size);

    //same logic as before
    //we return the adress that is the size of metadata past traverse's starting address
    return (char*)traverse + sizeof(Block);
}
