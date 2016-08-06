// stdio.h

#ifndef _INC_STDIO
#define _INC_STDIO 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdarg.h>

typedef long fpos_t;

extern struct __FILE_struct_def;
typedef struct __FILE_struct_def FILE;

#define BUFSIZ          512
#define EOF             (-1)
#define FILENAME_MAX    256     /* TODO */
#define FOPEN_MAX       8       /* TODO: Minumum allowed value */
#define L_tmpnam        FILENAME_MAX
#define TMP_MAX         25      /* TODO: Minimum allowed value */

extern const FILE *stdin;
extern const FILE *stdout;
extern const FILE *stderr;

int remove(const char *filename);
int rename(const char *oldname, const char *newname);
FILE *tmpfile(void);
char *tmpnam(char *str);

int fclose(FILE *stream);
int fflush(FILE *stream);
FILE *fopen(const char *filename, const char *mode);
FILE *freopen(const char *filename, const char *mode, FILE *stream);
void setbuf(FILE *stream, char *buffer);
int setvbuf(FILE *stream, char *buffer, int mode, size_t size);

int fprintf(FILE *stream, const char *format, ...);
int fscanf(FILE *stream, const char *format, ...);
int printf(const char *format, ...);
int scanf(const char *format, ...);
int snprintf(char *str, size_t n, const char *format, ...);
int sprintf(char *str, const char *format, ...);
int sscanf(const char *str, const char *format, ...);
int vfprintf(FILE *stream, const char *format, va_list arg);
int vfscanf(FILE *stream, const char *format, va_list arg);
int vprintf(const char *format, va_list arg);
int vscanf(const char *format, va_list arg);
int vsnprintf(char *str, size_t n, const char *format, va_list arg);
int vsprintf(char *str, const char *format, va_list arg);
int vsscanf(const char *str, const char *format, va_list arg);

int fgetc(FILE *stream);
char *fgets(char *str, int num, FILE *stream);
int fputc(int character, FILE *stream);
int fputs(const char *str, FILE *stream);
int getc(FILE *stream);
int getchar(void);
char *gets(char *str);
int putc(int character, FILE *stream);
int putchar(int character);
int puts(const char *str);
int ungetc(int character, FILE *stream);

size_t fread(void *ptr, size_t size, size_t count, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream);

int fgetpos(FILE *stream, fpos_t *pos);
int fseek(FILE *stream, long offset, int origin);
int fsetpos(FILE *stream, const fpos_t *pos);
long ftell(FILE *stream);
void rewind(FILE *stream);

void clearerr(FILE *stream);
int feof(FILE *stream);
int ferror(FILE *stream);
void perror(const char *str);

#ifdef __cplusplus
}
#endif

#endif /* _INC_STDIO */

