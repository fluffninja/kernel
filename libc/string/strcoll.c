#include <string.h>

int
strcoll(const char *str1, const char *str2) 
{
    // Locales are unimplemented
    return strcmp(str1, str2);
}
