#include <string.h>

char *
strcat(char *dest, const char *src)
{
    if (!dest || !src) {
        goto done;
    }

    const size_t dest_len = strlen(dest);
    const size_t src_len  = strlen(src);

    // The first character of src needs to be
    // placed on top of dest's null-terminator.
    // src needs to retain its original null-
    // terminator.
    memcpy(dest + dest_len, src, src_len + 1);

done:
    return dest;
}
