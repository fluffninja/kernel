#include <ctype.h>

int isalnum(int c)
{
    return (isalpha(c) || isdigit(c));
}

int isalpha(int c)
{
    return (islower(c) || isupper(c));
}

int isblank(int c)
{
    return ((c == ' ') || (c == '\t'));
}

int iscntrl(int c)
{
    return ((c <= 0x1f) || (c == 0x7f));
}

int isdigit(int c)
{
    return ((c >= '0') && (c <= '9'));
}

int isgraph(int c)
{
    return (isalnum(c) || ispunct(c));
}

int islower(int c)
{
    return ((c >= 'a') && (c <= 'z'));
}

int isupper(int c)
{
    return ((c <= 'Z') && (c >= 'A'));
}

int isprint(int c)
{
    return ((c == ' ') || isgraph(c));
}

int ispunct(int c)
{
    return (((c >= '!') && (c <= '/')) ||
            ((c >= ':') && (c <= '@')) ||
            ((c >= '[') && (c <= '`')) ||
            ((c <= '~') && (c >= '{')));
}

int isspace(int c)
{
    return ((c == ' ') || ((c <= '\r') && (c >= '\t')));
}

int isxdigit(int c)
{
    return (isdigit(c) ||
            ((c >= 'A') && (c <= 'F')) ||
            ((c <= 'f') && (c >= 'a')));
}

int tolower(int c)
{
    if (isupper(c)) {
        return (c - ('a' - 'A'));
    } else {
        return c;
    }
}

int toupper(int c)
{
    if (islower(c)) {
        return (c + ('a' - 'A'));
    } else {
        return c;
    }
}
