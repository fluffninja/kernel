#ifndef _INC_KUTIL
#define _INC_KUTIL

#include <cccompat.h>

#define PAGE_SIZE       4096
#define PAGE_ALIGNED    ALIGN(PAGE_SIZE)

#ifndef ARRLEN
#define ARRLEN(A)       (sizeof(A) / sizeof(*(A)))
#endif

char *itoa10(int val, char *str);
char *itoa16(int val, char *str);
int atoi10(const char *str);

#endif /* _INC_KUTIL */
