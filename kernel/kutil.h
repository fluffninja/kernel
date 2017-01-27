#ifndef _INC_KUTIL
#define _INC_KUTIL

#include <compiler.h>
#include <stddef.h>
#include <kernel/asm/cpustat.h>

#include "kstring.h"

#define PAGE_SIZE       4096
#define PAGE_ALIGNED    ALIGN(PAGE_SIZE)

#ifndef ARRLEN
#define ARRLEN(A)       (sizeof(A) / sizeof(*(A)))
#endif

// Casts a type's member to the containing type
// ITEM_PTR:    - pointer to a member
// CONT_TYPE:   - type of the containing type
// MEMBER:      - name of the member within the type
#ifndef CONTAINER_OF
#define CONTAINER_OF(ITEM_PTR, CONT_TYPE, MEMBER) \
    (CONT_TYPE *) ((char *) (ITEM_PTR) - offsetof(CONT_TYPE, MEMBER))
#endif

#define SMART_KZEROMEM 1

// Fill the region of memory pointed to by 'ptr' of size 'sz' with zeroes.
static INLINE void *kzeromem(void *ptr, size_t sz)
{
    #ifdef SMART_KZEROMEM
    // The compiler _should_ be clever enough to optimise this out
    if (sz % 4 == 0) {
        return kmemset32(ptr, sz, 0);
    } else if (sz % 2 == 0) {
        return kmemset16(ptr, sz, 0);
    } else {
        return kmemset(ptr, sz, 0);
    }
    #else
    return kmemset(ptr, sz, 0);
    #endif /* SMARK_KZEROMEM */
}

void print_cpustat(const struct cpustat cs);
void hexdump(int *ptr, int words_per_row, int rows);

#endif /* _INC_KUTIL */
