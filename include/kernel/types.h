#ifndef _INC_KERNEL_TYPES
#define _INC_KERNEL_TYPES 1

typedef char                s8;
typedef unsigned char       u8;
typedef short               s16;
typedef unsigned short      u16;
typedef int                 s32;
typedef unsigned int        u32;
typedef long long           s64;
typedef unsigned long long  u64;

#ifndef __cplusplus
    #define bool    _Bool
    #define true    (1)
    #define false   (0)
#endif

#ifndef slice_of
#define slice_of(T) \
struct {\
    T* ptr;\
    size_t length;\
}
#endif

#endif /* _INC_KERNEL_TYPES */
