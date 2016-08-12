#include <string.h>

size_t
strxfrm(char *dest, const char *src, size_t len)
{
    size_t old_len = len;

    if (!dest || !src) {
        return 0;
    }

    // Locales are unimplemented
    // Instead, copy len's worth of src (unless a null-terminator is reached),
    // and append a null-terminator.
    while (len && *src) {
        *(dest++) = *(src++);
        --len;
    }
    if (*src) {
        *dest = '\0';
    }

    return old_len;
}
