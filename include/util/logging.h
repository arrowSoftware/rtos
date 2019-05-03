#ifndef INCLUDE_UTIL_LOGGING_H_
#define INCLUDE_UTIL_LOGGING_H_

#include <stdarg.h>
#include <sys/time.h>
#include <time.h>

typedef enum
{
    LOG_CRITICAL = 50,
    LOG_ERROR = 40,
    LOG_WARNING = 30,
    LOG_INFO = 20,
    LOG_DEBUG = 10,
    LOG_NOTSET = 0
} loglevel_t;

typedef struct
{
    int logNumber;
    loglevel_t loglevel;
    struct tm time;
    void *function;
    const char buffer[1024];
} logDetail_t;

void log_init(void);
static void log_log(loglevel_t argLevel, void *argFunc, const char *argFormat, va_list args);
void log_critical(void *argFunc, const char *argMessage, ...);
void log_error(void *argFunc, const char *argMessage, ...);
void log_warning(void *argFunc, const char *argMessage, ...);
void log_info(void *argFunc, const char *argMessage, ...);
void log_debug(void *argFunc, const char *argMessage, ...);
int log_show(int argNumLogs);
loglevel_t log_setLevel(loglevel_t argLevel);

time_t log_rawtime;
struct tm *log_timeInfo;

logDetail_t *log_detailBuf[5000];
int log_count;
loglevel_t log_activeLevel;

#endif /* INCLUDE_UTIL_LOGGING_H_ */
