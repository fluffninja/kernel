#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <ctype.h>

#include <kernel/kernel.h>
#include <kernel/compiler.h>

#include "kio.h"
#include "console.h"

#define OUTBUF_SIZE PAGE_SIZE

static PAGE_ALIGNED char s_outbuf[OUTBUF_SIZE];
static int s_outbuf_zeroed = 0;

extern int con_write_str(const char *);
extern int con_write_char(char);

size_t __sputs(char **dst, const char *src, size_t sz)
{
    size_t i = 0;

    while (*src) {
        **dst = *src;
        ++i;
        if (!--sz) {
            **dst = 0;
            break;
        }
        src++;
        (*dst)++;
    }

    return i;
}

size_t __itoa10(int val, char *dst)
{
    int buf[10];
    size_t i = 0;
    size_t count;
    int is_negative = 0;

    if (val < 0) {
        is_negative = 1;
        val = -val;
    }

    while (i < ARRLEN(buf)) {
        buf[i++] = val % 10;
        val /= 10;

        if (!val) {
            break;
        }
    }

    if (is_negative) {
        *(dst++) = '-';
    }

    count = i;

    while (i) {
        *(dst++) = (char) ('0' + buf[--i]);
    }

    *dst = '\0';

    return count;
}

size_t __itoa16(int val, char *dst)
{
    static const char HEX_DIGIT_CHARS[16] = "0123456789abcdef";

    int buf[8];
    size_t i = 0;

    while (i < ARRLEN(buf)) {
        buf[i++] = val & 0x0f;
        val >>= 4;
    }

    while (i) {
        *(dst++) = HEX_DIGIT_CHARS[buf[--i]];
    }

    *dst = '\0';

    return 8;
}

// Format specifiers (the type of the var arg)
// Note that differences in case ('X' vs 'x') are signified by fmtinfo.is_upper
enum fmttype {
    FT_INVALID,
    FT_SIGNED,              // 'd', 'i'
    FT_UNSIGNED,            // 'u'
    FT_OCTAL,               // 'o'
    FT_HEX,                 // 'x', 'X'
    FT_FLOAT,               // 'f', 'F'
    FT_SCIENTIFIC,          // 'e', 'E'
    FT_SHORTEST,            // 'g', 'G': Shortest out of float and scientific
    FT_HEXFLOAT,            // 'a', 'A': Hexadecimal as float
    FT_CHAR,                // 'c'
    FT_STR,                 // 's'
    FT_PTR,                 // 'p'
    FT_STORE_CHAR_COUNT,    // 'n'
    FT_ESCAPE,              // '%'
};

// Format specifier flags
enum fmtflags {
    FF_NONE     = 0x00,
    FF_LJUSTIFY = 0x10, // '-': Left-justify within width
    FF_ZEROPAD  = 0x01, // '0': Zero-pad within width
    FF_POSSIGN  = 0x02, // '+': Prefix positive values with a plus
    FF_PADSIGN  = 0x04, // ' ': Prefix positive values with a space
    FF_EXPLICIT = 0x08, // '#': Prefix hex with '0x' or always show decimal dot
};

// Format specifier size (long vs short int, etc.)
enum fmtlen {
    FL_INT,         // (none)
    FL_CHAR,        // hh
    FL_SHORT,       // h
    FL_LONG,        // l
    FL_LONG_LONG,   // ll
    FL_INTMAX,      // j
    FL_SIZE,        // z
    FL_PTRDIFF,     // t
    FL_LONG_DOUBLE, // L
};

// Pack all of this information up neatly
struct fmtinfo {
    enum fmttype    ft;
    enum fmtflags   ff;
    enum fmtlen     fl;
    int             width;      // -1 if specified in vararg, 0 if unspecified
    int             precision;  // -1 if specified in vararg, 0 if unspecified
    int             upper;      // Make letter digit chars uppercase (eg 0xFF)
};

// Note: this whole monolithic mess of a function will eventually get redone
// in a far nicer manner. For now, I'm just trying to get it working.

size_t __va_str_format_proc(const char *fmt, struct fmtinfo *fip)
{
    // Format specifiers follow this scheme (fixed order, thank God):
    //  %[flags][width]['.', precision][length]<specifier>
    //
    // Each part can be:
    //  Flags:      -, +, ' ', #, 0
    //  Width:      an unsigned number (cannot start with zero), or a '*'.
    //  Precision:  '.' followed by an unsigned number, or followed by a '*'.
    //  Length:     hh, h, l, ll, j, z, t, L
    //  Specifier:  d, i, u, o, x, X, f, F, e, E, g, G, a, A, c, s, p, n, %
    //
    // Note that we only implement part of this mess. Also the actual formatter
    // may also ignore some of the information we acquire.

    struct fmtinfo fi = { 0 };

    enum {
        could_be_flags      = 1,
        could_be_width      = 2,
        could_be_presision  = 3,
        could_be_length     = 4,
        could_be_spec       = 5,
    } could_be = could_be_flags;

    size_t  fmti = 0;
    char    fmtc;
    while (++fmti, (fmtc = *(fmt++))) {

        // Could this char be a flag specifier?
        if (could_be <= could_be_flags) {

            // Let's assume it is unless we find out otherwise
            int is_flags = 1;

            switch (fmtc) {
            case '-':
                fi.ff |= FF_LJUSTIFY;
                break;
            case '+':
                fi.ff |= FF_POSSIGN;
                break;
            case ' ':
                fi.ff |= FF_PADSIGN;
                break;
            case '#':
                fi.ff |= FF_EXPLICIT;
                break;
            case '0':
                fi.ff |= FF_ZEROPAD;
                break;
            default:
                // If this char was none of the above, then it wasn't a flag
                is_flags = 0;
                break;
            }

            // If this _was_ a flag, then move onto the next character.
            if(is_flags) {
                continue;
            }

            // Otherwise, fall through to the next check
        }

        // Not a flag. Could it be the first char of the width specifier?
        // If so, it's going to be a string of digits.
        if (could_be <= could_be_width) {
            could_be = could_be_width;

            // There's a chance it's a width, let's assume so.
            int is_width = 1;

            // Is it even a digit? If so, try to parse it. Otherwise move on
            if (isdigit(fmtc)) {
                int width = 0;

                // Parse the number string. Just continue to loop over fmt
                // here, to reduce complications (in theory).
                while (isdigit(fmtc)) {
                    width *= 10;
                    width += (fmtc - '0');

                    // Advance fmt pointer, and check for null-term
                    if (++fmti, !(fmtc = *(fmt++))) {
                        // Null-term, bail out
                        break;
                    }
                }

                fi.width = width;

                // We'll have overshot by one, so sneakily backtrack
                --fmti;
                --fmt;

            } else if (fmtc == '*') {
                // Asterisk denotes that the width is specified by the next
                // var arg
                fi.width = -1;
            } else {
                is_width = 0;
            }

            // Same drill as before: move onto next char if this was actually
            // width, otherwise fall-through to the next check.
            if (is_width) {
                continue;
            }
        }

        // Same ordeal as above
        if (could_be <= could_be_presision) {
            could_be = could_be_presision;

            int is_precision = 1;

            // Precision _must_ start with a '.'
            if (fmtc == '.') {

                // Advance fmt pointer over the '.', and check for null-term
                if (++fmti, !(fmtc = *(fmt++))) {
                    // Null-term, cannot be precision
                    is_precision = 0;
                } else {
                    if (isdigit(fmtc)) {
                        int precision = 0;
                        // Parse this number
                        while (isdigit(fmtc)) {
                            precision *= 10;
                            precision += (fmtc - '0');

                            // Advance fmt pointer, and check for null-term
                            if (++fmti, !(fmtc = *(fmt++))) {
                                // Null-term, bail out
                                break;
                            }
                        }

                        fi.precision = precision;
                    } else if (fmtc == '*') {
                        // Asterisk denotes that the precision is specified by
                        // the next var arg.
                        fi.width = -1;
                    } else {
                        is_precision = 0;
                    }
                }

                // Backtrack by 1 (accounts for null-term check overshot)
                --fmti;
                --fmt;

            } else {
                is_precision = 0;
            }

            if (is_precision) {
                continue;
            }
        }

        if (could_be <= could_be_length) {
            could_be = could_be_length;

            int is_length = 1;

            switch (fmtc) {
            case 'h':
                fi.fl = FL_SHORT;
                // Check to see if the next char is another h (for 'hh')
                if (++fmti, (fmtc = *(fmt++)) && fmtc == 'h') {
                    fi.fl = FL_CHAR;
                } else {
                    // If not, backtrack
                    --fmti;
                    --fmt;
                }
                break;
            case 'l':
                fi.fl = FL_LONG_LONG;
                // Check to see if the next char is another l (for 'll')
                if (++fmti, (fmtc = *(fmt++)) && fmtc == 'l') {
                    fi.fl = FL_LONG;
                } else {
                    // If not, backtrack
                    --fmti;
                    --fmt;
                }
                break;
            case 'j':
                fi.fl = FL_INTMAX;
                break;
            case 'z':
                fi.fl = FL_SIZE;
                break;
            case 't':
                fi.fl = FL_PTRDIFF;
                break;
            case 'L':
                fi.fl = FL_LONG_DOUBLE;
                break;
            default:
                // If we had no matches, then this was not a length specifier
                is_length = 0;
                break;
            }

            if (is_length) {
                continue;
            }
        }

        if (could_be <= could_be_spec) {
            could_be = could_be_spec;

            int is_spec = 1;

            switch (fmtc) {
            case 'd':
            case 'i':
                fi.ft = FT_SIGNED;
                break;
            case 'u':
                fi.ft = FT_UNSIGNED;
                break;
            case 'o':
                fi.ft = FT_OCTAL;
                break;
            case 'X':
                fi.upper = 1;
            case 'x':
                fi.ft = FT_HEX;
                break;
            case 'F':
                fi.upper = 1;
            case 'f':
                fi.ft = FT_FLOAT;
                break;
            case 'E':
                fi.upper = 1;
            case 'e':
                fi.ft = FT_SCIENTIFIC;
                break;
            case 'G':
                fi.upper = 1;
            case 'g':
                fi.ft = FT_SHORTEST;
                break;
            case 'A':
                fi.upper = 1;
            case 'a':
                fi.ft = FT_HEXFLOAT;
                break;
            case 'c':
                fi.ft = FT_CHAR;
                break;
            case 's':
                fi.ft = FT_STR;
                break;
            case 'p':
                fi.ft = FT_PTR;
                break;
            case 'n':
                fi.ft = FT_STORE_CHAR_COUNT;
                break;
            case '%':
                fi.ft = FT_ESCAPE;
                break;
            default:
                is_spec = 0;
                break;
            }

            if (is_spec) {
                break;
            }
        }

        // We've fallen through all checks. This was an invalid format
        // specifier, or it at least contains unaccounted-for characters.
        fi.ft = FT_INVALID;
        break;
    }

    *fip = fi;
    return fmti;
}

// Processes string 'fmt' into buffer 'str' of size 'sz' using data from 'args'
// Returns the number of characters written into buffer 'str'
size_t __va_str_format_impl(char *str, size_t sz, const char *fmt,
    va_list args)
{
    // Remember the size we were originally given, so we can report the diff.
    size_t          old_sz = sz;

    // Buffer for formatting things into - for use with atoi, etc.
    char            fmtbuf[128];

    // Format info of each formatting key
    struct fmtinfo  fi;

    // Storage place for each arg type retrieved from args.
    union {
        int32_t         i32;
        uint32_t        u32;
        int16_t         i16;
        uint16_t        u16;
        int8_t          i8;
        uint8_t         u8;
        const char      *str;
    } arg;

    // Char from fmt
    char fmtc;

    while ((fmtc = *(fmt++))) {
        if (fmtc == '%') {
            size_t count = __va_str_format_proc(fmt, &fi);
            fmt += count;

            if (fi.ft == FT_ESCAPE) {
                sz -= __sputs(&str, "%", sz);
                continue;
            }

            if (fi.ft == FT_INVALID) {
                sz -= __sputs(&str, "<invalid>", sz);
                continue;
            }

            if (fi.ft == FT_CHAR) {
                arg.i8 = va_arg(args, int);
                if (sz--) {
                    *str = arg.i8;
                    ++str;
                }
                continue;
            }

            if (fi.ft == FT_STR) {
                arg.str = va_arg(args, const char *);
                sz -= __sputs(&str, arg.str, sz);
                continue;
            }

            if (fi.ft == FT_PTR) {
                arg.u32 = va_arg(args, uint32_t);
                __itoa16(arg.u32, fmtbuf);
                sz -= __sputs(&str, "0x", sz);
                sz -= __sputs(&str, fmtbuf, sz);
                continue;
            }

            if (fi.ft == FT_SIGNED || fi.ft == FT_UNSIGNED) {
                arg.i32 = va_arg(args, int32_t);
                __itoa10(arg.i32, fmtbuf);
            }

            if (fi.ft == FT_HEX) {
                arg.u32 = va_arg(args, uint32_t);
                __itoa16(arg.u32, fmtbuf);

                if (fi.ff & FF_EXPLICIT) {
                    sz -= __sputs(&str, "0x", sz);
                }
            }

            size_t start_index = 0;

            // TODO: fix bug with this - zero padding + width combo

            if (fi.width) {
                if (fi.width > 8) {
                    fi.width = 8;
                }
                start_index = (8 - (size_t) fi.width);
            }

            if (!(fi.ff & FF_ZEROPAD)) {
                for (size_t i = 0; i < sizeof(fmtbuf) && fmtbuf[i]; ++i) {
                    if (fmtbuf[i] != '0') {
                        start_index = i;
                        break;
                    }
                }
            }

            if (fi.upper) {
                for (size_t i = start_index; i < sizeof(fmtbuf); ++i) {
                    fmtbuf[i] = toupper(fmtbuf[i]);
                }
            }

            sz -= __sputs(&str, fmtbuf + start_index, sz);

        } else {
            *(str++) = fmtc;
            --sz;
        }
    }

    // Add a null-ternimator to the end.
    *(str++) = 0;
    --sz;

    return old_sz - sz;
}

int kvsnprintf(char *str, size_t sz, const char *fmt, va_list args)
{
    return (int) __va_str_format_impl(str, sz, fmt, args);
}

int ksnprintf(char *str, size_t sz, const char *fmt, ...)
{
    int result;
    va_list args;

    va_start(args, fmt);
    result = (int) kvsnprintf(str, sz, fmt, args);
    va_end(args);

    return result;
}

int kvsprintf(char *str, const char *fmt, va_list args)
{
    return (int) __va_str_format_impl(str, 0, fmt, args);
}

int ksprintf(char *str, const char *fmt, ...)
{
    int result;
    va_list args;

    va_start(args, fmt);
    result = (int) kvsprintf(str, fmt, args);
    va_end(args);

    return result;
}

int kvprintf(const char *fmt, va_list args)
{
    if (!s_outbuf_zeroed) {
        kmemset((void *) s_outbuf, OUTBUF_SIZE, 0);
        s_outbuf_zeroed = 1;
    }

    int result = (int) __va_str_format_impl(s_outbuf, OUTBUF_SIZE, fmt, args);

    con_write_str(s_outbuf);

    return result;
}

int kprintf(const char *fmt, ...)
{
    int result;
    va_list args;

    va_start(args, fmt);
    result = (int) kvprintf(fmt, args);
    va_end(args);

    return result;
}
