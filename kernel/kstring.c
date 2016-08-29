#include "kstring.h"

char *itoa10(int val, char *str)
{
    char buff[10];  // Store digits in reverse (UINT_MAX is 10 dec digits long)
    int isneg = 0;  // Is the value negative?
    int i = 0;      // Buffer index
    int j = 0;      // Output string index

    if (!str) {
        goto done;
    }

    if (val < 0) {
        isneg = 1;
        val = -val;
    }

    while (i < (int) sizeof(buff)) {
        buff[i++] = (char) ('0' + (val % 10));
        val /= 10;

        if (!val) {
            break;
        }
    }

    if (isneg) {
        str[j++] = '-';
    }

    while (i) {
        str[j++] = buff[--i];
    }

    str[j] = '\0';

done:
    return str;
}

char *itoa16(int val, char *str, int nibble_count)
{
    static const char HEX_DIGITS[16] = "0123456789ABCDEF";

    char buff[8];
    int i = 0;
    int j = 0;

    if (!str) {
        goto done;
    }

    while ((i < (int) sizeof(buff)) && (i < nibble_count)) {
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
