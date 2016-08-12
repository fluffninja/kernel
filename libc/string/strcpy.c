#include <string.h>

char *
strcpy(char *dest, const char *src)
{
    char *old_dest = dest;

    if (dest == src || !dest || !src) {
        goto done;
    }

    while ((*(dest++) = *(src++))) ;

done:
    return old_dest;
}
