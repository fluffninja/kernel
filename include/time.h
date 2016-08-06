// time.h

#ifndef _INC_TIME
#define _INC_TIME 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

struct tm
{
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};

typedef long long clock_t;
typedef long long time_t;

#define CLOCKS_PER_SEC  ((clock_t) 1) /* TODO */

clock_t clock(void);
double difftime(time_t end, time_t beginning);
time_t mktime(struct tm *timeptr);
time_t time(time_t *timer);

char *asctime(const struct tm *timeptr);
char *ctime(const time_t *timer);
struct tm *gmtime(const time_t *timer);
struct tm *localtime(const time_t *timer);
size_t strftime(char *ptr, size_t maxsize, const char *format, 
                const struct tm *timeptr);

#ifdef __cplusplus
}
#endif

#endif /* _INC_TIME */
