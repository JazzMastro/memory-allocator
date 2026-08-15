#ifndef SHARED_H
#define SHARED_H
#include <stddef.h>
#include <stdbool.h>


typedef struct Block {
    size_t size;
    bool free;
    struct Block * next;
    struct Block * prev;
} Block;

extern Block * first;

#endif