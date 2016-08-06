// stdarg.h for Isaac OS

#ifndef _INC_STDARG
#define _INC_STDARG

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char *va_list;

// Code follows that of www.brokenthorn.com/Resources/OSDev14.html
#define __VA_SIZE(type)		((sizeof(type) + sizeof(int) - 1) & ~(sizeof(int) - 1))
#define va_start(ap, arg) 	(ap = ((va_list) &(arg) + __VA_SIZE(arg))
#define va_end(ap)
#define va_arg(ap, type)	(ap += __VA_SIZE(type), *((type *) (ap - __VA_SIZE(type)))

#ifdef __cplusplus
}
#endif

#endif /* _INC_STDARG */


