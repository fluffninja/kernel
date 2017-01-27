#include <stdarg.h>

#include <kernel/klog.h>

#include "kio.h"

int klog_printf(const char *format, ...)
{
    int return_val;
    va_list args;

    va_start(args, format);
    return_val = kvprintf(format, args);
    va_end(args);

    return return_val;
}
