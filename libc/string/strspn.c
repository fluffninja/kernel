#include <string.h>

size_t
strspn(const char *str1, const char *str2)
{
    register size_t i = 0;
    
    if (!str1 || !str2) {
        goto done;
    }

    while (str1[i]) {
        register size_t j = 0;
        while (str2[j]) {
            if (str1[i] != str2[j]) {
                goto done;
            }
            ++j;
        }
        ++i;
    }

done:
    return i;
}
