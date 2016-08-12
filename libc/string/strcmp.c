#include <string.h>

int
strcmp(const char *str1, const char *str2)
{
    if (str1 == str2 || !str1 || !str2) {
        return 0;
    }

    // Standard dictates that comparisons be done as though memory is of type
    // unsigned char.
    register const unsigned char *str1uc = (const unsigned char *) str1;
    register const unsigned char *str2uc = (const unsigned char *) str2;

    while (*str1uc == *str2uc) {
        if (!*str1uc) {
            return 0;
        }
        ++str1uc;
        ++str2uc;
    }

    return ((*str1uc < *str2uc) ? -1 : 1);
}
