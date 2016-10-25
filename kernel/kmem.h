#ifndef _INC_KMEM
#define _INC_KMEM 1

#include <stddef.h>

void *kmalloc(size_t size);
void *kcalloc(size_t size);
void *krealloc(void *src, size_t size);
void kfree(void *ptr);

#endif /* _INC_KMEM */
