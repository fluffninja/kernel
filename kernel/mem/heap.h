#ifndef _INC_HEAP
#define _INC_HEAP 1

#include <stdlib.h>

void *heap_alloc(size_t size);
int heap_free(void *ptr);

#endif /* _INC_HEAP */
