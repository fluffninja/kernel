#include <string.h>

char *
strpbrk(char *str1, const char *str2)
{    
    if (!str1 || !str2) {
        goto done;
    }

    // Compare every character in str1 with 
    // every character in str2, O(n*m) style    
    register size_t i = 0;
    while (str1[i]) {
        register size_t j = 0;
        while (str2[j]) {
            if (str1[i] == str2[j]) {
                return (str1 + i);
            }
            ++j;
        }
        ++i;
    }

done:
    return NULL;    
}
