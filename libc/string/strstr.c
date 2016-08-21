#include <string.h>

char *
strstr(char *str1, const char *str2)
{
    if (!str1 || !str2) {
        return NULL;
    }

    register size_t i = 0;
    while (str1[i]) {
        if (!strcmp(str1 + i, str2)) {
            break;
        }
        ++i;
    }

    return (str1 + i);
}
