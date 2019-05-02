#include "util/logging.h"
#include <string.h>
#include <sys/time.h>
#include <stdio.h>

logging *logging::defaultlogger = 0;

logging *logging::getLogger(void)
{
    if (logging::defaultlogger == 0)
    {
        logging::defaultlogger = new logging();
    }

    return logging::defaultlogger;
}

void logging::shutdown(void)
{
    delete this;
    logging::defaultlogger = 0;
}

logging::logging(void)
{
    gettimeofday(&startTime, NULL);
    logCount = 0;
    memset(logDetail, 0x0, sizeof(logDetail));
    logLevel = logging::loglevel_t::NOTSET;
}

logging::~logging(void)
{
    memset(this->logDetail, 0x0, sizeof(this->logDetail));
}

void logging::log(logging::loglevel_t argLevel,
                  void *argFunc,
                  const char *argFormat,
                  va_list args)
{
    if (logCount <= 5000)
    {
        logCount = 0;
        memset(this->logDetail[logCount], 0x0, sizeof(logDetail_t));
    }

    this->logDetail[logCount]->m_logNumber++;
    this->logDetail[logCount]->m_loglevel = argLevel;
    gettimeofday(&this->logDetail[logCount]->m_timeval, NULL);
    this->logDetail[logCount]->m_function = argFunc;
    vsprintf((char*)this->logDetail[logCount]->m_buffer, argFormat, args);
    logCount++;
}

void logging::critical(void *argFunc, const char *argFormat, ...)
{
    va_list args;
    va_start(args, argFormat);
    this->log(logging::loglevel_t::CRITICAL, argFunc, argFormat, args);
    va_end(args);
}

void logging::error(void *argFunc, const char *argFormat, ...)
{
    va_list args;
    va_start(args, argFormat);
    this->log(logging::loglevel_t::ERROR, argFunc, argFormat, args);
    va_end(args);
}

void logging::warning(void *argFunc, const char *argFormat, ...)
{
    va_list args;
    va_start(args, argFormat);
    this->log(logging::loglevel_t::WARNING, argFunc, argFormat, args);
    va_end(args);
}

void logging::info(void *argFunc, const char *argFormat, ...)
{
    va_list args;
    va_start(args, argFormat);
    this->log(logging::loglevel_t::INFO, argFunc, argFormat, args);
    va_end(args);
}

void logging::debug(void *argFunc, const char *argFormat, ...)
{
    va_list args;
    va_start(args, argFormat);
    this->log(logging::loglevel_t::DEBUG, argFunc, argFormat, args);
    va_end(args);
}

extern "C" int logShow(int argNumLogs)
{
    logging *temp = logging::getLogger();

    for (int i = temp->logCount; i > 0; i--)
    {
        printf("%d-%d-%d-0x%x(%s)\n", temp->logDetail[temp->logCount]->m_logNumber,
                                      temp->logDetail[temp->logCount]->m_loglevel,
                                      temp->logDetail[temp->logCount]->m_timeval.tv_sec,
                                      temp->logDetail[temp->logCount]->m_function,
                                      temp->logDetail[temp->logCount]->m_buffer);
    }
    return temp->logCount;
}

extern "C" logging::loglevel_t setLogLevel(logging::loglevel_t argLevel)
{
    logging *temp = logging::getLogger();

    temp->logLevel = argLevel;

    return temp->logLevel;
}
