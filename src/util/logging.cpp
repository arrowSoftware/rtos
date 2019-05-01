#include "util/logging.h"
#include <string.h>

const char *DEFAULT_LOGGER = "defaultlogger";
logging *logging::defaultlogger = 0;

logging *logging::getLogger(const char *argName)
{
    if(strcmp(argName, logging::DEFAULT_LOGGER) == 0)
    {
        if (logging::defaultlogger == 0)
        {
            logging::defaultlogger = new logging(argName);
            return logging::defaultlogger;
        }
    }
    return new logging(argName);
}

void logging::shutdown(void)
{
    delete this;
}

logging::logging(void)
{

}

logging::~logging(void)
{

}
