#include <string.h>

char *
strncat(char *dest, const char *src, size_t len)
{
    if (!dest || !src || len == 0) {
        goto done;
    }

    const size_t dest_len = strlen(dest);
    const size_t src_len  = strlen(src);

    // The first character of src needs to be
    // placed on top of dest's null-terminator.
    // src needs to retain its original null-
    // terminator.
    if (src_len <= len) {
        // Same as strcat
        memcpy(dest + dest_len, src, src_len + 1);
    } else {
        memcpy(dest + dest_len, src, len);
        dest[dest_len + len + 1] = '\0';    
    }

done:
    return dest;
}
