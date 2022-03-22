//
// Created by lenz on 5/24/20.
//

#ifndef ROCHAT_CLUTF8_H
#define ROCHAT_CLUTF8_H

#include <string>
#include "utf8.h"

std::string utf8_to_riscos_local(const std::string &in_utf8_str);
std::string riscos_local_to_utf8(const std::string &in_str);
// functions missing in the utf8.h
std::string utf8_substr(const std::string& str, unsigned int start, unsigned int leng);
std::string utf8_substr2(const std::string &str,int start, int length);
unsigned int utf8_len_bytes_substr(const std::string& str, unsigned int start, unsigned int leng);

int utf8_to_ucs4(u_int32_t *dest, int sz, const char *src, int srcsz);
int ucs4_to_utf8(char *dest, int sz, u_int32_t *src, int srcsz);
int ucs4_wc_to_utf8(char *dest, u_int32_t ch);

#endif //ROCHAT_CLUTF8_H
