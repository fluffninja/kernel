#include <string.h>

char *
strncpy(char *dest, const char *src, size_t len)
{  
    char *old_dest = dest;

    if (dest == src || !dest || !src) {
        goto done;
    }

    // Standards dictate that no null-terminator
    // should be appended to dest should src be
    // longer than len.

    while (len && *src) {
        *(dest++) = *(src++);
        --len;
    }

    while (len) {
        *(dest++) = '\0';
        --len;
    }

done:
    return old_dest;
}
