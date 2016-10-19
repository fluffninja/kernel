#ifndef _INC_KUTIL
#define _INC_KUTIL

#include <cccompat.h>

#include "kstring.h"

#define PAGE_SIZE       4096
#define PAGE_ALIGNED    ALIGN(PAGE_SIZE)

#ifndef ARRLEN
#define ARRLEN(A)       (sizeof(A) / sizeof(*(A)))
#endif

INLINE void *
kzeromem(void *ptr, size_t sz)
{
    return kmemset(ptr, sz, 0);
}

#endif /* _INC_KUTIL */
