#ifndef _INC_KERNEL_KERNEL
#define _INC_KERNEL_KERNEL 1

#include <stddef.h>

#include <kernel/compiler.h>
#include <kernel/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PAGE_SIZE       4096
#define PAGE_ALIGNED    ALIGN(PAGE_SIZE)

#ifndef ARRLEN
#define ARRLEN(A)       (sizeof(A) / sizeof(*(A)))
#endif

#ifndef BITFLAG
#define BITFLAG(N)      (1UL << (N))
#endif

#ifndef MAX
#define MAX(A, B)       (((A) > (B)) ? (A) : (B))
#endif

#ifndef MIN
#define MIN(A, B)       (((A) < (B)) ? (A) : (B))
#endif

// CONTAINER_OF:
//   Cast a pointer to a member within a container to a pointer to the
//   container of the container type.
// MEMBER_POINTER   - pointer to a member.
// CONTAINER_TYPE   - type of the container to which the member belongs.
// MEMBER_NAME      - name of the member field within the container type.
#ifndef CONTAINER_OF
#define CONTAINER_OF(MEMBER_POINTER, CONTAINER_TYPE, MEMBER_NAME)             \
    (                                                                         \
        (CONTAINER_TYPE *) (                                                  \
            (char *) (MEMBER_POINTER) - offsetof(CONTAINER_TYPE, MEMBER_NAME) \
        )                                                                     \
    )
#endif

static INLINE void *kmemset32(void *pointer, size_t size, u32 val)
{
    u32 *pointer32 = (u32 *) pointer;

    size /= sizeof(u32);

    while (size--) {
        *(pointer32++) = val;
    }

    return pointer;
}

static INLINE void *kmemset16(void *pointer, size_t size, u16 val)
{
    u16 *pointer16 = (u16 *) pointer;

    size /= sizeof(u16);

    while (size--) {
        *(pointer16++) = val;
    }

    return pointer;
}

static INLINE void *kmemset(void *pointer, size_t size, u16 val)
{
    u8 *pointer8 = (u8 *) pointer;

    while (size--) {
        *(pointer8++) = val;
    }

    return pointer;
}

static INLINE void *kmemcpy32(const void *source, void *dest, size_t size)
{
    const u32 *source32 = source;
    u32 *dest32 = dest;

    size /= sizeof(u32);

    while (size--) {
        *(dest32++) = *(source32++);
    }

    return dest;
}

static INLINE void *kmemcpy16(const void *source, void *dest, size_t size)
{
    const u16 *source16 = source;
    u16 *dest16 = dest;

    size /= sizeof(u16);

    while (size--) {
        *(dest16++) = *(source16++);
    }

    return dest;
}

static INLINE void *kmemcpy(const void *source, void *dest, size_t size)
{
    const u8 *source8 = source;
    u8 *dest8 = dest;

    while (size--) {
        *(dest8++) = *(source8++);
    }

    return dest;
}

static INLINE char *kstrcpy(const char *source, char *dest)
{
    while (*source) {
        *(dest++) = *(source++);
    }

    return dest;
}

#ifndef KZEROMEM
#define KZEROMEM(POINTER, SIZE) kmemset((POINTER), (SIZE), 0)
#endif

void hexdump(int *ptr, int words_per_row, int rows);

#ifdef __cplusplus
}
#endif

#endif /* _INC_KERNEL_KERNEL */
