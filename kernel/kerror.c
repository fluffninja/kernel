#include <kernel/kerror.h>

static int s_error = KERROR_NONE;

void kerror_clear(void)
{
    s_error = KERROR_NONE;
}

void kerror_set(int error)
{
    if ((error >= 0) && (error < KERROR_LAST)) {
        s_error = error;
    }
}

int kerror_get_last(void)
{
    return s_error;
}
