// stddef.h for Isaac OS

#ifndef _INC_STDDEF
#define _INC_STDDEF 1

#define NULL    ((void *) 0)

#define offsetof(type, member)  ((unsigned int) &(((type *) 0)->member))

typedef unsigned int    size_t;
typedef int             ptrdiff_t;

// Currently omitting max_align_t from C11.
// nullptr_t is C++ only.

#endif /* _INC_STDDEF */
