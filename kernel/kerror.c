#include <kernel/kerror.h>

static int s_error = KERROR_NONE;

void kerror_set_last(int error)
{
    if ((error >= 0) && (error < KERROR_LAST)) {
        s_error = error;
    }
}

void kerror_clear_last(void)
{
    s_error = 0;
}

int kerror_pop_last(void)
{
    int error = s_error;
    s_error = 0;
    return error;
}

int kerror_peek_last(void)
{
    return s_error;
}
