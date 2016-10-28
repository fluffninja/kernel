#ifndef _INC_HEAP
#define _INC_HEAP 1

#include <stddef.h>

enum {
    HEAP_ZERO,
};

enum {
    HEAP_ERROR_NULL = 1,
    HEAP_ERROR_OUT_OF_MEMORY = 2,
    HEAP_ERROR_BAD_SIZE = 3,
    HEAP_ERROR_BAD_FLAGS = 4,
};

int heap_alloc(void **ptr, size_t size, unsigned int flags);
int heap_move(void **dest, void *src, size_t size, unsigned int flags);
int heap_free(void *ptr, unsigned int flags);

#endif /* _INC_HEAP */
