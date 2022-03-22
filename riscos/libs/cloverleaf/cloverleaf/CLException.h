//
// Created by lenz on 12/24/19.
//

#ifndef ROCHAT_CLEXCEPTION_H
#define ROCHAT_CLEXCEPTION_H

#include <stdexcept>
#include <stdio.h>
#include "oslib/os.h"
#include "Logger.h"

class CLException : public std::runtime_error {
    char msg[1024];
public:
    CLException(const std::string &arg, const char *file, int line) :
            std::runtime_error(arg) {
        snprintf(msg, 1023, "%s:%d: %s", file, line, arg.c_str());
        Logger::crit("Exception: %s", msg);
    }
    ~CLException() throw() {}
    const char *what() const throw() {
        return msg;
    }
};

std::string get_os_error_string(os_error* err);

#define throw_exception(arg) throw CLException(arg, __FILE__, __LINE__);
#define throw_bad_alloc_exception() throw CLException("bad_alloc", __FILE__, __LINE__);


#endif //ROCHAT_CLEXCEPTION_H
