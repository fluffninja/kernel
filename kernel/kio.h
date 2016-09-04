#ifndef _INC_KIO
#define _INC_KIO 1

#include <stdarg.h>
#include <stddef.h>

int kvsnpritnf(char *str, size_t sz, const char *fmt, va_list args);
int ksnprintf(char *str, size_t sz, const char *fmt, ...);
int kvsprintf(char *str, const char *fmt, va_list args);
int ksprintf(char *str, const char *fmt, ...);
int kvprintf(const char *fmt, va_list args);
int kprintf(const char *fmt, ...);

#endif /* _INC_KIO */
