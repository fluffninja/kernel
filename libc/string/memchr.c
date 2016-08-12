#include <string.h>

void *
memchr(void *str, int value, size_t len)
{
    // Standard dicates that the sought value should be treated as an
    // unsigned char.
    register const unsigned char valueuc = (const unsigned char) value;

    if (!str || len == 0) {
        goto done;
    }

    register const unsigned char *struc = (const unsigned char *) str;
    while (len) {
        if (*struc == valueuc) {
            return ((void *) struc);
        }
        ++struc;
        --len;
    }

done:
    return NULL;
}
