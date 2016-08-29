#ifndef _INC_KSTRING
#define _INC_KSTRING 1

#include <stdint.h>
#include <stddef.h>
#include <cccompat.h>

char *itoa10(int val, char *str);
char *itoa16(int val, char *str, unsigned int nibble_count);
void kprintf(const char *format, ...);

INLINE void
kmemset32(void *ptr, size_t sz, uint32_t val)
{
    uint32_t *ptr32 = (uint32_t *) ptr;
    sz /= sizeof(uint32_t);
    while (sz--) {
        *(ptr32++) = val;
    }
}

INLINE void
kmemset16(void *ptr, size_t sz, uint16_t val)
{
    uint16_t *ptr16 = (uint16_t *) ptr;
    sz /= sizeof(uint16_t);
    while (sz--) {
        *(ptr16++) = val;
    }
}

INLINE void
kmemset(void *ptr, size_t sz, uint16_t val) 
{
    uint8_t *ptr8 = (uint8_t *) ptr;
    while (sz--) {
        *(ptr8++) = val;
    }
}

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
