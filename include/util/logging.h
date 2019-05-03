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
    int m_time;
    void *function;
    const char *funcName;
    const char buffer[80];
} logDetail_t;

void log_init(void);
static void log_log(loglevel_t argLevel, void *argFunc, const char *argFuncName, const char *argFormat, va_list args);
void _log_critical(void *argFunc, const char *funcName, const char *argMessage, ...);
void _log_error(void *argFunc, const char *funcName, const char *argMessage, ...);
void _log_warning(void *argFunc, const char *funcName, const char *argMessage, ...);
void _log_info(void *argFunc, const char *funcName, const char *argMessage, ...);
void _log_debug(void *argFunc, const char *funcName, const char *argMessage, ...);
int log_show(int argNumLogs);
const char *log_levelToStr(loglevel_t argLevel);
loglevel_t log_setLevel(loglevel_t argLevel);

#define log_critical(func, format, ...) _log_critical((void*)func, #func, format, ##__VA_ARGS__);
#define log_error(func, format, ...) _log_error((void*)func, #func, format, ##__VA_ARGS__);
#define log_warning(func, format, ...) _log_warning((void*)func, #func, format, ##__VA_ARGS__);
#define log_info(func, format, ...) _log_info((void*)func, #func, format, ##__VA_ARGS__);
#define log_debug(func, format, ...) _log_debug((void*)func, #func, format, ##__VA_ARGS__);

int log_initTime;
logDetail_t log_detailBuf[500];
int log_count;
int running_count;
loglevel_t log_activeLevel;

#endif /* INCLUDE_UTIL_LOGGING_H_ */
