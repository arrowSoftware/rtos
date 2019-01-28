#ifndef __TIME_H
#define __TIME_H

/* includes */

#include <sys/types.h>

/* defines */
#define CLOCK_REALTIME			0x00		/* System time */
#define CLOCK_ABSTIME			0x01		/* Absolute time */

/* typedefs */

/* structs */

struct tm {
    int  tm_sec;    /* seconds           - [0..60]  */
    int  tm_min;    /* minutes           - [0..59]  */
    int  tm_hour;   /* hours             - [0..23]  */
    int  tm_mday;   /* day of the month  - [1..31]  */
    int  tm_mon;    /* month of the year - [0..11]  */
    int  tm_year;   /* years since 1900.            */
    int  tm_wday;   /* day of the week   - [0..6]   */
    int  tm_yday;   /* day of the year   - [0..365] */
    int  tm_isdst;  /* daylight savings flag        */
};

struct timespec {
    time_t  tv_sec;
    long    tv_nsec;
};

struct itimerspec {
    struct timespec  it_interval;    /* timer period */
    struct timespec  it_value;       /* timer expiration */
};

/* externs */
extern time_t  time (time_t *);
extern time_t  mktime (struct tm *);
extern struct tm *  localtime (const time_t *);
extern int localtime_r (const time_t *, struct tm *);
extern size_t  strftime (char *, size_t, const char *, const struct tm*);
extern int  clock_settime (clockid_t, const struct timespec *);
extern int  clock_gettime (clockid_t, struct timespec *);

#if 0
extern char *  asctime (const struct tm *);
extern char *  asctime_r (const struct tm*, char *);
extern clock_t  clock (void);
extern int  clock_getres (clockid_t, struct timespec *);
extern int  clock_nanosleep (clockid_t, int, const struct timespec *,
                             struct timespec *);
extern char *  ctime (const time_t *);
extern char *  ctime_r (const time_t *, char *);
extern double  diftime (time_t, time_t);
extern struct tm *  getdate (const char *);
extern struct tm *  gmtime (const time_t *);
extern struct tm *  gmtime_r (const time_t *, struct tm *);
extern int  nanosleep (const struct timespec *, struct timespec *);
extern char *  strptime (const char *, const char *, struct tm *);
extern int  timer_create (clockid_t, struct sigevent *, timer_t *);
extern int  timer_delete (timer_t);
extern int  timer_gettime (timer_t, struct itimerspec *);
extern int  timer_getoverrun (timer_t);
extern int  time_settime (timer_t, int, const struct itimerspec *,
                          struct itimerspec *);
extern void  tzset (void);
#endif


#endif
