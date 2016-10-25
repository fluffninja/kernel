#ifndef _INC_KSTRING
#define _INC_KSTRING 1

#include <cccompat.h>
#include <stdint.h>
#include <stddef.h>

// Memset in 4 byte blocks
static INLINE void *kmemset32(void *ptr, size_t sz, uint32_t val)
{
    uint32_t *ptr32 = (uint32_t *) ptr;
    sz /= sizeof(uint32_t);
    while (sz--) {
        *(ptr32++) = val;
    }
    return ptr;
}

// Memset in 2 byte blocks
static INLINE void *kmemset16(void *ptr, size_t sz, uint16_t val)
{
    uint16_t *ptr16 = (uint16_t *) ptr;
    sz /= sizeof(uint16_t);
    while (sz--) {
        *(ptr16++) = val;
    }
    return ptr;
}

// Memset in 1 byte blocks
static INLINE void *kmemset(void *ptr, size_t sz, uint16_t val)
{
    uint8_t *ptr8 = (uint8_t *) ptr;
    while (sz--) {
        *(ptr8++) = val;
    }
    return ptr;
}

// Memcpy in 4 byte blocks
static INLINE void *kmemcpy32(const void *src, void *dst, size_t sz)
{
    const uint32_t  *src32 = (const uint32_t *) src;
    uint32_t        *dst32 = (uint32_t *)       dst;
    sz /= sizeof(uint32_t);
    while (sz--) {
        *(dst32++) = *(src32++);
    }
    return dst;
}

// Memcpy in 2 byte blocks
static INLINE void *kmemcpy16(const void *src, void *dst, size_t sz)
{
    const uint16_t  *src16 = (const uint16_t *) src;
    uint16_t        *dst16 = (uint16_t *)       dst;
    sz /= sizeof(uint16_t);
    while (sz--) {
        *(dst16++) = *(src16++);
    }
    return dst;
}

// Memset in 1 byte blocks
static INLINE void *kmemcpy(const void *src, void *dst, size_t sz)
{
    const uint8_t   *src8   = (const uint8_t *) src;
    uint8_t         *dst8   = (uint8_t *)       dst;
    while (sz--) {
        *(dst8++) = *(src8++);
    }
    return dst;
}

static INLINE char *kstrcpy(const char *src, char *dst)
{
    while (*src) {
        *(dst++) = *(src++);
    }
    return dst;
}

#endif /* _INC_KSTRING */
