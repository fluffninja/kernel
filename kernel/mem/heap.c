#include <kernel/asm/misc.h>
#include <kernel/types.h>

#include "heap.h"

/* The generous and wasteful and, to be honest, quite awful memory allocator */
/* This is a rudimentary dynamic memory allocation implementation that takes
   place inside the physical address space - no paging! */
/* It's awful because it never frees. Yup. */

/* The kernel image currently occupies 0x00100000 (1MB) to ~ 0x00200000 (2MB),
   so the heap will begin above this to ensure that we're out of the way. */
#define HEAP_START ((void *) 0x00200000)

/* 0x00F00000 - 0x00FFFFFF can contain memory mapped hardware, so we'll avoid
   meddling with this. */
#define HEAP_END   ((void *) 0x00EFFFFF)

// This keeps track of the next free piece of memory.
static void *_Atomic volatile heap_head = HEAP_START;

void *heap_alloc(size_t size)
{
    void *alloc_addr = NULL;

    cli();

    if (((u32) (heap_head + size)) <= ((u32) HEAP_END)) {
        alloc_addr = heap_head;
        heap_head += size;
    }

    sti();

    return alloc_addr;
}

int heap_free(void *ptr)
{
    (void) ptr;

    return 0;
}
