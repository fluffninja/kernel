#include <kernel/dlist.h>
#include <stdint.h>

#include "heap.h"

int heap_alloc(void **ptr, size_t size, unsigned int flags)
{
    if (!ptr) {
        return HEAP_ERROR_NULL;
    }

    if (size == 0) {
        return HEAP_ERROR_BAD_SIZE;
    }

    (void) flags;

    return 0;
}

int heap_move(void **dest, void *src, size_t size, unsigned int flags)
{
    if (!dest || !src) {
        return HEAP_ERROR_NULL;
    }

    if (size == 0) {
        return HEAP_ERROR_BAD_SIZE;
    }

    (void) flags;

    return 0;
}

int heap_free(void *ptr, unsigned int flags)
{
    if (!ptr) {
        return HEAP_ERROR_NULL;
    }

    (void) flags;

    return 0;
}
