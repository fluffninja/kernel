// string.h for Isaac OS

#ifndef _INC_STRING
#define _INC_STRING 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

void *memcpy(void *dest, const void *src, size_t len);
void *memmove(void *dest, const void *src, size_t len);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t len);

char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, size_t len);

int memcmp(const void *ptr1, const void *ptr2, size_t len);
int strcmp(const char *str1, const char *str2);
int strcoll(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, size_t len);
size_t strxfrm(char *dest, const char *src, size_t len);

void *memchr(void *str, int value, size_t len);
char *strchr(char *str, int c);
size_t strcspn(const char *str1, const char *str2);
char *strpbrk(char *str1, const char *str2);
char *strrchr(char *str, int c);
size_t strspn(const char *str1, const char *str2);
char *strstr(char *str1, const char *str2);
char *strtok(char *str, const char *del);

void *memset(void *ptr, int value, size_t len);
char *strerror(int errnum);
size_t strlen(const char *str);

#ifdef __cplusplus
}
#endif

#endif /* _INC_STRING */
