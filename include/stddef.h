/* stddef.h - C Standard Library */

#ifndef _INC_STDDEF
#define _INC_STDDEF 1

#define NULL                    ((void *) 0)

#define offsetof(type, member)  ((unsigned int) &(((type *) 0)->member))

typedef unsigned int            size_t;
typedef int                     ptrdiff_t;
typedef long double             max_align_t;

#endif /* _INC_STDDEF */
