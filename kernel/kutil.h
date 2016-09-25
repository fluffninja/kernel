#ifndef _INC_KUTIL
#define _INC_KUTIL

#include <cccompat.h>

#define PAGE_SIZE       4096
#define PAGE_ALIGNED    ALIGN(PAGE_SIZE)

#ifndef ARRLEN
#define ARRLEN(A)       (sizeof(A) / sizeof(*(A)))
#endif

int atoi(const char *str);
char *itoa(int val, char *str, int radix);

#endif /* _INC_KUTIL */
