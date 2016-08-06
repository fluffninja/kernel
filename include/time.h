// time.h

#ifndef _INC_TIME
#define _INC_TIME

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef long long clock_t;
typedef long long time_t;
struct tm;

#define CLOCKS_PER_SEC	((clock_t) 1) /* TODO */

char *asctime(const struct tm *timeptr);
clock_t clock(void);
char *ctime(const time_t *timer);
double difftime(time_t end, time_t beginning);
struct tm *gmtime(const time_t *timer);
struct tm *localtime(const time_t *timer);
time_t mktime(struct tm *timeptr);
size_t strftime(char *ptr, size_t maxsize, const char *format, const struct tm *timeptr);
time_t time(time_t *timer);

#ifdef __cplusplus
}
#endif

#endif /* _INC_TIME */


