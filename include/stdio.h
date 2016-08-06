// stdio.h for Isaac OS

#ifndef _INC_STDIO
#define _INC_STDIO

#include <stddef.h>
#include <stdarg.h>

typedef long fpos_t;

typedef struct {} FILE;

#define	BUFSIZ		512
#define	EOF		(-1)
#define	FILENAME_MAX	256		/* TODO */
#define	FOPEN_MAX	8		/* TODO: Minumum allowed value */
#define	L_tmpnam	FILENAME_MAX
#define	TMP_MAX		25		/* TODO: Minimum allowed value */

extern const FILE *stdin;
extern const FILE *stdout;
extern const FILE *stderr;

void clearerr(FILE *stream);
int fclose(FILE *stream);
int feof(FILE *stream);
int ferror(FILE *stream);
int fflush(FILE *stream);
int fgetc(FILE *stream);
int fgetpos(FILE *stream, fpos_t *pos);
char *fgets(char *str, int num, FILE *stream);
FILE *fopen(const char *filename, const char *mode);
int fprintf(FILE *stream, const char *format, ...);
int fputc(int character, FILE *stream);
int fputs(const char *str, FILE *stream);
size_t fread(void *ptr, size_t size, size_t count, FILE *stream);
FILE *reopen(const char *filename, const char *mode, FILE *stream);
int fscanf(FILE *stream, const char *format, ...);
int fseek(FILE *stream, long offset, int origin);
int fsetpos(FILE *stream, const fpos_t *pos);
long ftell(FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream);
int getc(FILE *stream);
int getchar(void);
char *gets(char *str);
void perror(const char *str);
int printf(const char *format, ...);
int putc(int character, FILE *stream);
int putchar(int character);
int puts(const char *str);
int remove(const char *filename);
int rename(const char *oldname, const char *newname);
void rewind(FILE *stream);
int scanf(const char *format, ...);
void setbuf(FILE *stream, char *buffer);
int setvbuf(FILE *stream, char *buffer, int mode, size_t size);
int snprintf(char *str, size_t n, const char *format, ...);
int sprintf(char *str, const char *format, ...);
int sscanf(const char *str, const char *format, ...);
FILE *tmpfile(void);
char *tmpnam(char *str);
int ungetc(int character, FILE *stream);
int vfprintf(FILE *stream, const char *format, va_list arg);
int vfscanf(FILE *stream, const char *format, va_list arg);
int vprintf(const char *format, va_list arg);
int vscanf(const char *format, va_list arg);
int vsnprintf(char *str, size_t n, const char *format, va_list arg);
int vsprintf(char *str, const char *format, va_list arg);
int vsscanf(const char *str, const char *format, va_list arg);

#endif /* _INC_STDIO */

