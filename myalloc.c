#include <stdio.h>

void *myalloc(int x) {
    //call mmap (or sbrk) once, at the start, to reserve a large fixed-size block — 
    // say a few megabytes — and store a pointer to the start of it 
    // somewhere your allocator can access globally.

    
}