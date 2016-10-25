#ifndef _INC_KUTIL
#define _INC_KUTIL

#include <cccompat.h>
#include <stddef.h>

#include "kstring.h"

#define PAGE_SIZE       4096
#define PAGE_ALIGNED    ALIGN(PAGE_SIZE)

#ifndef ARRLEN
#define ARRLEN(A)       (sizeof(A) / sizeof(*(A)))
#endif

INLINE void *
kzeromem(void *ptr, size_t sz)
// Casts a type's member to the containing type
// ITEM_PTR         - pointer to a member
// CONTAINER_TYPE   - type of the containing type
// MEMBER           - name of the member within the type
#define CONTAINER_OF(ITEM_PTR, CONTAINER_TYPE, MEMBER) \
    (CONTAINER_TYPE *) ((char *) ITEM_PTR - offsetof(CONTAINER_TYPE, MEMBER)
{
    return kmemset(ptr, sz, 0);
}

#endif /* _INC_KUTIL */
