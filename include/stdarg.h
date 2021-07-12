/* stdarg.h - C Standard Library */

#ifndef _INC_STDARG
#define _INC_STDARG 1

typedef __builtin_va_list va_list;

#define va_start(ap, v) __builtin_va_start(ap, v)
#define va_end(ap) __builtin_va_end(ap)
#define va_arg(ap, t) __builtin_va_arg(ap, t)

#endif /* _INC_STDARG */
