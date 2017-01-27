#ifndef _INC_KERNEL_KERNEL
#define _INC_KERNEL_KERNEL 1


#include <stddef.h>
#include <stdint.h>

#include <compiler.h>


#ifdef __cplusplus
extern "C" {
#endif


static INLINE void *kmemset32(void *pointer, size_t size, uint32_t val)
{
    uint32_t *pointer32 = (uint32_t *) pointer;

    size /= sizeof(uint32_t);

    while (size--) {
        *(pointer32++) = val;
    }

    return pointer;
}


static INLINE void *kmemset16(void *pointer, size_t size, uint16_t val)
{
    uint16_t *pointer16 = (uint16_t *) pointer;

    size /= sizeof(uint16_t);

    while (size--) {
        *(pointer16++) = val;
    }

    return pointer;
}


static INLINE void *kmemset(void *pointer, size_t size, uint16_t val)
{
    uint8_t *pointer8 = (uint8_t *) pointer;

    while (size--) {
        *(pointer8++) = val;
    }

    return pointer;
}


static INLINE void *kmemcpy32(const void *source, void *dest, size_t size)
{
    const uint32_t *source32 = source;
    uint32_t *dest32 = dest;

    size /= sizeof(uint32_t);

    while (size--) {
        *(dest32++) = *(source32++);
    }

    return dest;
}


static INLINE void *kmemcpy16(const void *source, void *dest, size_t size)
{
    const uint16_t *source16 = source;
    uint16_t *dest16 = dest;

    size /= sizeof(uint16_t);

    while (size--) {
        *(dest16++) = *(source16++);
    }
        return dest;
}


static INLINE void *kmemcpy(const void *source, void *dest, size_t size)
{
    const uint8_t *source8 = source;
    uint8_t *dest8 = dest;

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


#ifdef __cplusplus
}
#endif


#endif /* _INC_KERNEL_KERNEL */
