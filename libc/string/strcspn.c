#include <string.h>

size_t
strcspn(const char *str1, const char *str2)
{    
    register size_t i = 0;

    if (!str1 || !str2) {
        goto done;
    }

    // Compare every character in str1 with 
    // every character in str2, O(n*m) style
    while (str1[i]) {
        register size_t j = 0;
        while (str2[j]) {
            if (str1[i] == str2[j]) {
                goto done;
            }
            ++j;
        }
        ++i;
    }

done:
    return i;
}
