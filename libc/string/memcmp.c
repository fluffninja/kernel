#include <string.h>

int
memcmp(const void *ptr1, const void *ptr2, size_t len)
{
    if (ptr1 == ptr2 || len == 0 || !ptr1 || !ptr2) {
        return 0;
    }

    while (len--) {
        // Standard dictates that comparisons be done as though memory is of 
        // type unsigned char.
        register const unsigned char c1 = *((const unsigned char *) ptr1);
        register const unsigned char c2 = *((const unsigned char *) ptr2);
        if (c1 != c2) {
            return ((c1 < c2) ? -1 : 1);
        }
    }

    return 0;
}
