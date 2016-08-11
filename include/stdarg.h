// stdarg.h

#ifndef _INC_STDARG
#define _INC_STDARG 1

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char *va_list;

#define _VA_SIZEOF(t)   ( (sizeof(t) + sizeof(int) - 1) & ~(sizeof(int) - 1) )

#define va_start(ap, v) ( ap = (va_list) &(v) + __VA_SIZEOF(v) )
#define va_end(ap)      ( ap = (va_list) 0 )
#define va_arg(ap, t)   ( *(t *) ((ap += _VA_SIZEOF(t)) - _VA_SIZEOF(t))

#ifdef __cplusplus
}
#endif

#endif /* _INC_STDARG */
