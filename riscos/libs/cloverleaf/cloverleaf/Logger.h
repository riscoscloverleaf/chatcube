//
// Created by lenz on 3/23/20.
//

#ifndef ROCHAT_LOGGER_H
#define ROCHAT_LOGGER_H

#include <string>
#include <stdarg.h>

class Logger {
protected:
    static char log_file[255];
    static void log(const char* level, const char* cntrl_string, va_list ap);
public:
    static void init(const char* log_file);
    static void debug(const char* cntrl_string, ...);
    static void info(const char* cntrl_string, ...);
    static void warn(const char* cntrl_string, ...);
    static void error(const char* cntrl_string, ...);
    static void crit(const char* cntrl_string, ...);
};

#endif //ROCHAT_LOGGER_H
