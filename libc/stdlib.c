#include <stdlib.h>
#include <ctype.h>

#ifndef ARRLEN
#define ARRLEN(A)   (sizeof(A) / sizeof(*(A)))
#endif

int atoi(const char *str)
{
    if (!str) {
        return -1;
    }

    int result = 0;
    
    while (*str) {
        char c = *(str++);

        if (!isdigit(c)) {
            return -1;
        }

        result *= 10;
        result += (c - '0');
    }

    return result;
}

char *itoa(int val, char *str, int radix)
{
    static const char DIGIT_CHARS[36] = "0123456789abcdefghijklmnopqrstuvwxyz";

    // The longest string we can produce is the string for highest value that
    // can be represented by a signed platform-width integer in base 2.
    int buf[8 * sizeof(int)];
    size_t i = 0;
    int is_negative = 0;

    if (!str) {
        goto done;
    }

    if (val < 0) {
        is_negative = 1;
        val = -val;
    }

    while (i < ARRLEN(buf)) {
        buf[i++] = val % radix;
        val /= radix;

        if (!val) {
            break;
        }
    }

    if (is_negative) {
        *(str++) = '-';
    }

    while (i) {
        *(str++) = DIGIT_CHARS[buf[--i]];
    }

    *str = '\0';

done:
    return str;
}
