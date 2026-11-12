# Custom Memory Allocator

A custom heap memory allocator implemented in C using the POSIX `mmap()` API. The project explores low-level memory management, block metadata, memory alignment, and free-list manipulation.

## Features

- **Memory Mapping:** Uses `mmap()` to request memory regions from the operating system, initially allocating a 4 KB region.
- **Block Metadata:** Maintains allocation metadata through a custom `Block` structure containing block size, allocation status, and doubly linked list pointers.
- **Memory Alignment:** Rounds allocation requests up to 8-byte boundaries.
- **Block Splitting:** Splits larger free blocks to accommodate allocation requests while retaining usable leftover space.
- **Free-List Management:** Maintains a doubly linked list of available blocks, inserting freed blocks according to their memory addresses.
- **Dynamic Memory Expansion:** Requests additional memory regions when the available free space cannot satisfy an allocation request.
- **Custom Deallocation:** Implements `myfree()` to return allocated blocks to the free list.

## Implementation

The allocator is built around two primary operations:

**`myalloc(int size)`**

Searches the free list for a sufficiently large block. If necessary, requests additional memory using `mmap()`. Selected blocks are split when enough space remains for another usable block, and the function returns a pointer to the allocated memory.

**`myfree(void *address)`**

Retrieves the block metadata associated with an allocated pointer and reinserts the block into the free list while maintaining address-based ordering.

## Technologies

- C
- Linux / POSIX API
- `mmap()`
- GCC

## Building and Running

Compile the allocator and included test program:

bash
gcc -Wall -Wextra -o test myalloctestfile.c myalloc.c myfree.c


Run the executable:

bash
./test

The included test program demonstrates allocating memory for a custom `student` structure and accessing its fields.

## Project Structure

memory-allocator/
├── myalloc.c           # Allocation and memory management
├── myfree.c            # Memory deallocation
├── shared.h            # Block structure and function declarations
└── myalloctestfile.c   # Example allocation test

## Limitations

- Adjacent free blocks are not currently coalesced.
- The allocator does not currently implement thread synchronization.
- The included test program demonstrates basic allocation rather than comprehensive allocator testing.
