#ifndef INCLUDE_UTIL_LOGGING_H_
#define INCLUDE_UTIL_LOGGING_H_

class logging
{
    public:
        logging *getLogger(const char *argName);
        void shutdown(void);
        static const char *DEFAULT_LOGGER;

    private:
        logging(void);
        logging(const char *argName);
        ~logging(void);
        static logging *defaultlogger;
};

#endif /* INCLUDE_UTIL_LOGGING_H_ */
