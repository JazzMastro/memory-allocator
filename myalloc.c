#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Block {
    size_t size;
    bool free;
    struct Block * next;
    struct Block * prev;
} Block;


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

void split(Block * current, int size) {
    int leftover = current->size - (size + sizeof(Block));

    Block * NewBlock = (Block*)((char*)current + sizeof(Block) + size); //char cast to get correct address displacement for next block

    NewBlock->size = leftover;
    NewBlock->free = true;
    NewBlock->next = current->next;
    NewBlock->prev = current->prev;

    current->size = size + sizeof(Block);
    current->free = false;

}

void *myalloc(int x) {
    //in a free list allocator, we use "blocks"
    //how we implement this, is we take a pointer that points to the first adress in a block
    //now lets say we have a block size of 8 addresses, so all 7 mem adds after the first pointer belong to the block
    //conceptually we can say that each of these points to a new block: 0x1000, 0x1008, 0x1010, 0x1018, etc.

    //starting pointer and size (bytes)

    if (heap_start == NULL) {
        void *start;
        size_t size = 4096;

        //condition check
        int res = init(size, &start);
        if (res == 1) exit(0);
    }
    
    void * header = first; //points to first block (the actual block not the first pointer)

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
        //make new block
    }

    //next step is to section some space out for our dedicated block
    //remember that each call = one block so if we take space from one block we split it into two
    
    split(traverse, x);
}