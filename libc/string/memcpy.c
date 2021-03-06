#include <string.h>
#include <stdint.h>

void *
memcpy(void *dest, const void *src, size_t len)
{
    if (dest == src || len == 0 || !dest || !src) {
        goto done;
    }

    // If word-aligned, word-wise copy
    if (((uintptr_t) dest % sizeof(unsigned int) == 0) &&
        ((uintptr_t) src  % sizeof(unsigned int) == 0) &&
        (len  % sizeof(unsigned int) == 0)) {

        register unsigned int       *dest32 = (unsigned int *)       dest;
        register const unsigned int *src32  = (const unsigned int *) src;
        register const size_t       len32   = len / sizeof(unsigned int);

        for (register size_t i = 0; i < len32; ++i) {
            dest32[i] = src32[i];
        }
    // Otherwise, byte-wise copy
    } else {
        register unsigned char       *dest8 = (unsigned char *)       dest;
        register const unsigned char *src8  = (const unsigned char *) src;

        for (register size_t i = 0; i < len; ++i) {
            dest8[i] = src8[i];
        }
    }

done:
    return dest;
}
