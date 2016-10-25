#include "kmem.h"
#include "mm/heap.h"

void *kmalloc(size_t size)
{
    void *ptr = NULL;

    if (heap_alloc(&ptr, size, 0)) {
        return NULL;
    }

    return ptr;
}

void *kcalloc(size_t size)
{
    void *ptr = NULL;

    if (heap_alloc(&ptr, size, HEAP_ZERO)) {
        return NULL;
    }

    return ptr;
}

void *krealloc(void *src, size_t size)
{
    void *dest = NULL;

    if (heap_move(&dest, src, size, 0)) {
        return NULL;
    }

    return dest;
}

void kfree(void *ptr)
{
    (void) heap_free(ptr, 0);
}
