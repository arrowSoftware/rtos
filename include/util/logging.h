#ifndef INCLUDE_UTIL_LOGGING_H_
#define INCLUDE_UTIL_LOGGING_H_

#include <stdarg.h>

class logging
{
    public:
        typedef enum
        {
            CRITICAL = 50,
            ERROR = 40,
            WARNING = 30,
            INFO = 20,
            DEBUG = 10,
            NOTSET = 0
        } loglevel_t;

        typedef struct
        {
            int m_logNumber;
            loglevel_t m_loglevel;
            struct timeval m_timeval;
            void *m_function;
            const char m_buffer[1024];
        } logDetail_t;

        static logging *getLogger(void);
        void shutdown(void);

        void log(loglevel_t argLevel, void *argFunc, const char *argFormat, va_list args);
        void critical(void *argFunc, const char *argMessage, ...);
        void error(void *argFunc, const char *argMessage, ...);
        void warning(void *argFunc, const char *argMessage, ...);
        void info(void *argFunc, const char *argMessage, ...);
        void debug(void *argFunc, const char *argMessage, ...);

        struct timeval startTime;
        logDetail_t *logDetail[5000];
        int logCount;
        loglevel_t logLevel;

    private:
        logging(void);
        ~logging(void);
        static logging *defaultlogger;

};

#endif /* INCLUDE_UTIL_LOGGING_H_ */
