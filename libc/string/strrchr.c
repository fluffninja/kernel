#include <string.h>

char *
strrchr(char *str, int c)
{
    char *cloc = NULL;

    if (!str) {
        goto done;
    }

    register const char cc = (const char) c;

    char val;
    while ((val = *(str++))) {
        if (val == cc) {
            cloc = str;
        }
    }

    if (!cloc) {
        cloc = str;
    }

done:
    return cloc;    
}
