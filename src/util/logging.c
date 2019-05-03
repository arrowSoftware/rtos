#include "util/logging.h"
#include <string.h>
#include <stdio.h>
#include <time.h>

void log_init(void)
{
    memset(log_detailBuf, 0x0, sizeof(log_detailBuf));
    time( &log_rawtime );
    log_timeInfo = localtime( &log_rawtime );
}

void log_log(loglevel_t argLevel,
             void *argFunc,
             const char *argFormat,
             va_list args)
{
    time_t temp;

    if (log_count >= 5000)
    {
        log_count = 0;
        memset(log_detailBuf[log_count], 0x0, sizeof(logDetail_t));
    }

    log_detailBuf[log_count]->logNumber++;
    log_detailBuf[log_count]->loglevel = argLevel;
    time(&temp);
    log_detailBuf[log_count]->time = localtime(&temp);
    log_detailBuf[log_count]->function = argFunc;
    vsprintf((char*)log_detailBuf[log_count]->buffer, argFormat, args);
    log_count++;
}

void critical(void *argFunc, const char *argFormat, ...)
{
    va_list args;
    va_start(args, argFormat);
    log_log(LOG_CRITICAL, argFunc, argFormat, args);
    va_end(args);
}

void log_error(void *argFunc, const char *argFormat, ...)
{
    va_list args;
    va_start(args, argFormat);
    log_log(LOG_ERROR, argFunc, argFormat, args);
    va_end(args);
}

void log_warning(void *argFunc, const char *argFormat, ...)
{
    va_list args;
    va_start(args, argFormat);
    log_log(LOG_WARNING, argFunc, argFormat, args);
    va_end(args);
}

void log_info(void *argFunc, const char *argFormat, ...)
{
    va_list args;
    va_start(args, argFormat);
    log_log(LOG_INFO, argFunc, argFormat, args);
    va_end(args);
}

void log_debug(void *argFunc, const char *argFormat, ...)
{
    va_list args;
    va_start(args, argFormat);
    log_log(LOG_DEBUG, argFunc, argFormat, args);
    va_end(args);
}

int log_show(int argNumLogs)
{
    int i = 0;
    for (i = log_count; i > 0; i--)
    {
        printf("%d-%d-%d-0x%x(%s)\n", log_detailBuf[log_count]->logNumber,
                                      log_detailBuf[log_count]->loglevel,
                                      log_detailBuf[log_count]->time.tm_sec,
                                      log_detailBuf[log_count]->function,
                                      log_detailBuf[log_count]->buffer);
    }
    return log_count;
}

loglevel_t log_setLevel(loglevel_t argLevel)
{
    log_activeLevel = argLevel;

    return log_activeLevel;
}
