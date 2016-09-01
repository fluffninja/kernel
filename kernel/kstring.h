#ifndef _INC_KSTRING
#define _INC_KSTRING 1

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <cccompat.h>

#define EMPLACEB32(V)   ((V) | ((V) << 8) | ((V) << 16) | ((V) << 24))
#define EMPLACEB16(V)   ((V) | ((V) << 8))
#define EMPLACEW32(V)   ((V) | ((V) << 16))

#ifndef ARRLEN
#define ARRLEN(A)   (sizeof(A) / sizeof(*(A)))
#endif

char *itoa10(int val, char *str);
char *itoa16(int val, char *str, unsigned int nibble_count);

// TODO: add ksprintf and kvsprintf to print a string into a buffer, rework
// these funcs to use those instead.
void kvprintf(const char *format, va_list args);
void kprintf(const char *format, ...);

// Memset in 32-bit (4 byte) blocks
INLINE void
kmemset32(void *ptr, size_t sz, uint32_t val)
{
    uint32_t *ptr32 = (uint32_t *) ptr;
    sz /= sizeof(uint32_t);
    while (sz--) {
        *(ptr32++) = val;
    }
}

// Memset in 16-bit (2 byte) blocks
INLINE void
kmemset16(void *ptr, size_t sz, uint16_t val)
{
    uint16_t *ptr16 = (uint16_t *) ptr;
    sz /= sizeof(uint16_t);
    while (sz--) {
        *(ptr16++) = val;
    }
}

// Regular memset
INLINE void
kmemset(void *ptr, size_t sz, uint16_t val) 
{
    uint8_t *ptr8 = (uint8_t *) ptr;
    while (sz--) {
        *(ptr8++) = val;
    }
}

// Memcpy in 32-bit (4 byte) blocks
INLINE void
kmemcpy32(const void *src, void *dst, size_t sz)
{
    const uint32_t  *src32 = (const uint32_t *) src;
    uint32_t        *dst32 = (uint32_t *)       dst;
    sz /= sizeof(uint32_t);
    while (sz--) {
        *(dst32++) = *(src32++);
    }
}

// Memcpy in 16-bit (2 byte) blocks
INLINE void
kmemcpy16(const void *src, void *dst, size_t sz)
{
    const uint16_t  *src16 = (const uint16_t *) src;
    uint16_t        *dst16 = (uint16_t *)       dst;
    sz /= sizeof(uint16_t);
    while (sz--) {
        *(dst16++) = *(src16++);
    }
}

// Regular memcpy
INLINE void
kmemcpy(const void *src, void *dst, size_t sz)
{
    const uint8_t   *src8   = (const uint8_t *) src;
    uint8_t         *dst8   = (uint8_t *)       dst;
    while (sz--) {
        *(dst8++) = *(src8++);
    }
}

#endif /* _INC_KSTRING */
