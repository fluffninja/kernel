#include <ctype.h>

#include "kutil.h"

char *itoa10(int val, char *str)
{
    char buff[10];  // Store digits in reverse (UINT_MAX is 10 dec digits long)
    int is_neg = 0; // Is the value negative?
    int i = 0;      // Buffer index
    int j = 0;      // Output string index

    if (!str) {
        goto done;
    }

    if (val < 0) {
        is_neg = 1;
        val = -val;
    }

    while (i < (int) sizeof(buff)) {
        buff[i++] = (char) ('0' + (val % 10));
        val /= 10;

        if (!val) {
            break;
        }
    }

    if (is_neg) {
        str[j++] = '-';
    }

    while (i) {
        str[j++] = buff[--i];
    }

    str[j] = '\0';

done:
    return str;
}

char *itoa16(int val, char *str, unsigned int nibble_count)
{
    static const char HEX_DIGITS[16] = "0123456789abcdef";

    char buff[8];
    unsigned int i = 0;
    unsigned int j = 0;

    if (!str) {
        goto done;
    }

    while ((i < (int) sizeof(buff))) {
        if (nibble_count) {
            if (i >= nibble_count) {
                break;
            }
        } else {
            if (!(val & 0x0f)) {
                break;
            }
        }
        buff[i++] = HEX_DIGITS[val & 0x0f];
        val >>= 4;
    }

    while (i) {
        str[j++] = buff[--i];
    }

    str[j] = '\0';

done:
    return str;
}

int atoi10(const char *str)
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
