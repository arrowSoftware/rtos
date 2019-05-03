#include "util/logging.h"
#include <string.h>
#include <stdio.h>
#include <time.h>

void log_init(void)
{
    memset(&log_detailBuf[0], 0x0, sizeof(log_detailBuf));
    log_initTime = 0;
    log_count = 0;
    running_count = 0;
    log_activeLevel = LOG_DEBUG;
}

void log_log(loglevel_t argLevel,
             void *argFunc,
             const char *argFuncName,
             const char *argFormat,
             va_list args)
{
    if (log_count >= 500)
    {
        log_count = 0;
        memset(&log_detailBuf[log_count], 0x0, sizeof(logDetail_t));
    }

    log_detailBuf[log_count].logNumber = running_count++;
    log_detailBuf[log_count].loglevel = argLevel;
    log_detailBuf[log_count].m_time = 0;
    log_detailBuf[log_count].function = argFunc;
    log_detailBuf[log_count].funcName = argFuncName;
    vsprintf((char*)log_detailBuf[log_count].buffer, argFormat, args);
    log_count++;
}

void _log_critical(void *argFunc, const char *argFuncName,const char *argFormat, ...)
{
    va_list args;
    va_start(args, argFormat);
    log_log(LOG_CRITICAL, argFunc, argFuncName, argFormat, args);
    va_end(args);
}

void _log_error(void *argFunc, const char *argFuncName,const char *argFormat, ...)
{
    va_list args;
    va_start(args, argFormat);
    log_log(LOG_ERROR, argFunc, argFuncName, argFormat, args);
    va_end(args);
}

void _log_warning(void *argFunc, const char *argFuncName,const char *argFormat, ...)
{
    va_list args;
    va_start(args, argFormat);
    log_log(LOG_WARNING, argFunc, argFuncName, argFormat, args);
    va_end(args);
}

void _log_info(void *argFunc, const char *argFuncName,const char *argFormat, ...)
{
    va_list args;
    va_start(args, argFormat);
    log_log(LOG_INFO, argFunc, argFuncName, argFormat, args);
    va_end(args);
}

void _log_debug(void *argFunc, const char *argFuncName,const char *argFormat, ...)
{
    va_list args;
    va_start(args, argFormat);
    log_log(LOG_DEBUG, argFunc, argFuncName, argFormat, args);
    va_end(args);
}

const char *log_levelToStr(loglevel_t argLevel)
{
    switch(argLevel)
    {
        case LOG_CRITICAL:
            return "CRITICAL";
            break;
        case LOG_ERROR:
            return "ERROR";
            break;
        case LOG_WARNING:
            return "WARNING";
            break;
        case LOG_INFO:
            return "INFO";
            break;
        case LOG_DEBUG:
            return "DEBUG";
            break;
    }
    return "UNKNOWN";
}

int log_show(int argNumLogs)
{
    int i = 0;

    printf("num-level-sec-func-buf\n");
    for (i = 0; i < log_count; i++)
    {
        if (log_detailBuf[i].loglevel >= log_activeLevel)
        {
            printf("%d-%s-%d-0x%x::%s(%s)\n", log_detailBuf[i].logNumber,
                                          log_levelToStr(log_detailBuf[i].loglevel),
                                          log_detailBuf[i].m_time,
                                          log_detailBuf[i].function,
                                          log_detailBuf[i].funcName,
                                          log_detailBuf[i].buffer);
        }
    }
    return log_count;
}

loglevel_t log_setLevel(loglevel_t argLevel)
{
    log_activeLevel = argLevel;

    return log_activeLevel;
}
