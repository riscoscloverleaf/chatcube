//
// Created by lenz on 2/25/20.
//

#include "CLException.h"

std::string get_os_error_string(os_error* err) {
    char message[1024];
    snprintf(message, 1023, "OS Error: %s (%d)", err->errmess, err->errnum);
    return std::string(message);
}
