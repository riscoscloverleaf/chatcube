/*
 * JsonModel.h
 *
 * helper class which will help to parse json data faster
 */

#ifndef ROCHAT_JSONDATA_H
#define ROCHAT_JSONDATA_H

#include <string>
#include "../libs/cJSON/cJSON.h"

class JsonData {
public:
    static bool has_value(const cJSON *jsonobj, const char *name);
    static bool has_int_value(const cJSON *jsonobj, const char *name);
    static bool has_float_value(const cJSON *jsonobj, const char *name);
    static bool has_object_value(const cJSON *jsonobj, const char *name);

    static int get_int_value(const cJSON *jsonobj, const char *name,  int defval);
    static int64_t get_int64_value(const cJSON *jsonobj, const char *name,  int64_t defval);
    static int64_t get_int64_value(const cJSON *jsonobj, const char *name);
    static bool get_bool_value(const cJSON *jsonobj, const char *name,  bool defval);
    static std::string get_string_value(const cJSON *jsonobj, const char *name, const std::string &defval);
    static std::string get_string_value(const cJSON *jsonobj, const char *name);
    static time_t get_time_value(const cJSON *jsonobj, const char *name, time_t defval);
    static const cJSON* get_json_object(const cJSON *jsonobj, const char *name);
    static const cJSON* get_json_object_or_null(const cJSON *jsonobj, const char *name);
    static const cJSON* get_json_array(const cJSON *jsonobj, const char *name);

    static bool get_set_value(const cJSON *jsonobj, const char *name, std::string &current_val);
    static bool get_set_value(const cJSON *jsonobj, const char *name, int &current_val);
    static bool get_set_value(const cJSON *jsonobj, const char *name, int64_t &current_val);
    static bool get_set_value(const cJSON *jsonobj, const char *name,  bool& defval);
};

#endif