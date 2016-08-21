#include <string.h>

char *
strtok(char *str, const char *del)
{
    static char *last = NULL;
    char *old_str = str;

    if (!del) {
        return NULL;
    }

    if (!str && !(str = last)) {
        return NULL;
    }

    for (register size_t i = 0; str[i]; ++i) {
        for (register size_t j = 0; del[j]; ++j) {
            if (str[i] == del[j]) {
                str[i] = '\0';
                last = str + i + 1;
                return old_str;
            }
        }
    }

    return NULL;
}

char *
strtok_(char *str, const char *del)
{
    return strtok(str, del);
}
