#include <ctype.h>

#include "kstring.h"

extern void con_write_str(const char *);
extern void con_write_char(char);

static void (*const puts_ptr)(const char *) = con_write_str;
static void (*const putc_ptr)(char)         = con_write_char;

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

void kvprintf(const char *format, va_list args)
{
    char buff[64];
    char c;
    void *ptr;
    const char *str;
    int i32;  

    while ((c = *(format++))) {
        if (c == '%') {
            switch (c = *(format++)) {
            default: // Includes '%%' escape case
                putc_ptr(c);
                break;
            case 's':
                str = va_arg(args, const char *);
                puts_ptr(str);
                break;
            case 'u':
            case 'i':
            case 'd':
                i32 = va_arg(args, int32_t);
                itoa10(i32, buff);
                puts_ptr(buff);
                break;
            case 'x':
                i32 = va_arg(args, uint32_t);
                itoa16(i32, buff, 8);
                puts_ptr(buff);
                break;
            case 'X':
                i32 = va_arg(args, uint32_t);
                itoa16(i32, buff, 0);
                for (size_t i = 0; i < sizeof(buff); ++i) {
                    buff[i] = toupper(buff[i]);
                }
                puts_ptr(buff);
                break;
            case 'p':
                ptr = va_arg(args, void *);
                itoa16((int32_t) ptr, buff, sizeof(void *) * 2);
                puts_ptr(buff);
                break;
            }
        } else {
            putc_ptr(c);
        }
    }    
}

void kprintf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    kvprintf(format, args);
    va_end(args);
}
