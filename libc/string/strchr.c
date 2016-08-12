#include <string.h>

char *
strchr(char *str, int c)
{
    register const char cc = (const char) c;

    if (!str) {
        goto done;
    }

    while (*str) {
        if (*str == cc) {
            return str;
        }
        ++str;
    }

done:
    return NULL;
}
