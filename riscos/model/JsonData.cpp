#include <string.h>
#include <string>
#include <stdexcept>
#include "JsonData.h"
#include "FileCacheDownloader.h"
#include "../utils.h"
#include "cloverleaf/CLException.h"
#include "cloverleaf/Logger.h"
#include "../libs/cJSON/cJSON.h"

bool JsonData::has_value(const cJSON *jsonobj, const char *name) {
    if (jsonobj && cJSON_IsObject(jsonobj)) {
        const cJSON *tmp = cJSON_GetObjectItemCaseSensitive(jsonobj, name);
        return tmp != NULL;
    }
    return false;
}

bool JsonData::has_int_value(const cJSON *jsonobj, const char *name) {
    if (jsonobj && cJSON_IsObject(jsonobj)) {
        const cJSON *tmp = cJSON_GetObjectItemCaseSensitive(jsonobj, name);
        return cJSON_IsInt(tmp);
    }
    return false;
}

bool JsonData::has_float_value(const cJSON *jsonobj, const char *name) {
    if (jsonobj && cJSON_IsObject(jsonobj)) {
        const cJSON *tmp = cJSON_GetObjectItemCaseSensitive(jsonobj, name);
        return cJSON_IsFloat(tmp);
    }
    return false;
}

bool JsonData::has_object_value(const cJSON *jsonobj, const char *name) {
    if (jsonobj && cJSON_IsObject(jsonobj)) {
        const cJSON *tmp = cJSON_GetObjectItemCaseSensitive(jsonobj, name);
        return cJSON_IsObject(tmp);
    }
    return false;
}

int JsonData::get_int_value(const cJSON *jsonobj, const char *name, const int defval)
{
    if (jsonobj && cJSON_IsObject(jsonobj))
    {
        const cJSON *tmp = cJSON_GetObjectItemCaseSensitive(jsonobj, name);
        if (cJSON_IsInt(tmp))
        {
            return tmp->valueint;
//        } else {
//            std::string errmsg = "JsonData::get_int_value ["+std::string(name)+"] is not exist or empty";
//            debug_print_string(errmsg);
        }
    } else {
        Logger::debug("JsonData::get_int_value [%s] jsonobj is not an object", name);
    }
    return defval;
}

int64_t JsonData::get_int64_value(const cJSON *jsonobj, const char *name, const int64_t defval)
{
    if (jsonobj && cJSON_IsObject(jsonobj))
    {
        const cJSON *tmp = cJSON_GetObjectItemCaseSensitive(jsonobj, name);
        if (cJSON_IsInt(tmp))
        {
            return tmp->valueint64;
//        } else {
//            std::string errmsg = "JsonData::get_int64_value ["+std::string(name)+"] is not exist or empty";
//            debug_print_string(errmsg);
        }
    } else {
//        std::string errmsg = "JsonData::get_int64_value ["+std::string(name)+"] jsonobj is not an object";
        Logger::debug("JsonData::get_int64_value [%s] jsonobj is not an object", name);
    }
    return defval;
}

int64_t JsonData::get_int64_value(const cJSON *jsonobj, const char *name)
{
    if (jsonobj && cJSON_IsObject(jsonobj))
    {
        const cJSON *tmp = cJSON_GetObjectItemCaseSensitive(jsonobj, name);
        if (cJSON_IsInt(tmp))
        {
            return tmp->valueint64;
        } else {
            char* p = cJSON_Print(jsonobj);
            std::string errmsg = "JsonData::get_int64_value ["+std::string(name)+"] is not exist or empty ["+std::string(tmp->valuestring)+"]";
            Logger::error("%s\n%s", errmsg.c_str(), p);
            free(p);
            throw_exception(errmsg);
        }
    } else {
        char* p = cJSON_Print(jsonobj);
        std::string errmsg = "JsonData::get_int64_value ["+std::string(name)+"] jsonobj is not an object";
        Logger::error("%s\n%s", errmsg.c_str(), p);
        free(p);
        throw_exception(errmsg);
    }
}

bool JsonData::get_bool_value(const cJSON *jsonobj, const char *name, const bool defval)
{
    if (jsonobj && cJSON_IsObject(jsonobj))
    {
        const cJSON *tmp = cJSON_GetObjectItemCaseSensitive(jsonobj, name);
        if (cJSON_IsBool(tmp))
        {
            return cJSON_IsTrue(tmp);
//        } else {
//            std::string errmsg = "JsonData::get_bool_value ["+std::string(name)+"] is not exist or empty";
//            debug_print_string(errmsg);
        }
    } else {
        Logger::debug("JsonData::get_bool_value [%s] jsonobj is not an object", name);
    }
    return defval;
}

std::string JsonData::get_string_value(const cJSON *jsonobj, const char *name, const std::string &defval)
{
    if (jsonobj && cJSON_IsObject(jsonobj))
    {
        const cJSON *tmp = cJSON_GetObjectItemCaseSensitive(jsonobj, name);
        if (cJSON_IsString(tmp) && (tmp->valuestring != NULL))
        {
            return std::string(tmp->valuestring);
//        } else {
//            std::string errmsg = "JsonData::get_string_value ["+std::string(name)+"] is not exist or empty";
//            debug_print_string(errmsg);
        }
    } else {
        Logger::debug("JsonData::get_string_value [%s] jsonobj is not an object", name);
    }
    
    return defval;
}

std::string JsonData::get_string_value(const cJSON *jsonobj, const char *name)
{
    if (jsonobj && cJSON_IsObject(jsonobj))
    {
        const cJSON *tmp = cJSON_GetObjectItemCaseSensitive(jsonobj, name);
        if (cJSON_IsString(tmp) && (tmp->valuestring != NULL))
        {
            return std::string(tmp->valuestring);
        } else {
            char* p = cJSON_Print(jsonobj);
            std::string errmsg = "JsonData::get_string_value ["+std::string(name)+"] jsonobj is not an object";
            Logger::error("%s\n%s", errmsg.c_str(), p);
            free(p);
            throw_exception(errmsg);
        }
    } else {
        char* p = cJSON_Print(jsonobj);
        std::string errmsg = "JsonData::get_string_value ["+std::string(name)+"] jsonobj is not an object";
        Logger::error("%s\n%s", errmsg.c_str(), p);
        free(p);
        throw_exception(errmsg);
    }
}

time_t JsonData::get_time_value(const cJSON *jsonobj, const char *name, time_t defval)
{
    if (jsonobj && cJSON_IsObject(jsonobj))
    {
        const cJSON *tmp = cJSON_GetObjectItemCaseSensitive(jsonobj, name);
        if (cJSON_IsInt(tmp))
        {
            return (time_t)tmp->valueint;
        } else {
//            Logger::debug("JsonData::get_time_value [%s] jsonobj is not exist or empty", name);
        }
    } else {
        Logger::debug("JsonData::get_time_value [%s] jsonobj is not an object", name);
    }

    return defval;
}

const cJSON* JsonData::get_json_object(const cJSON *jsonobj, const char *name) {
    if (jsonobj && cJSON_IsObject(jsonobj))
    {
        const cJSON *tmp = cJSON_GetObjectItemCaseSensitive(jsonobj, name);
        if (cJSON_IsObject(tmp))
        {
            return tmp;
        } else {
            std::string errmsg = "JsonData::get_json_object ["+std::string(name)+"] is not exist or empty";
            Logger::crit(errmsg.c_str());
            throw_exception(errmsg);
        }
    } else {
        std::string errmsg = "JsonData::get_json_object ["+std::string(name)+"] jsonobj is not an object";
        Logger::crit(errmsg.c_str());
        throw_exception(errmsg);
    }
}

const cJSON* JsonData::get_json_object_or_null(const cJSON *jsonobj, const char *name) {
    if (jsonobj && cJSON_IsObject(jsonobj))
    {
        return cJSON_GetObjectItemCaseSensitive(jsonobj, name);
    } else {
        char* p = cJSON_Print(jsonobj);
        std::string errmsg = "JsonData::get_json_object ["+std::string(name)+"] jsonobj is not an object";
        Logger::error("%s\n%s", errmsg.c_str(), p);
        free(p);
        throw_exception(errmsg);
    }
}

const cJSON* JsonData::get_json_array(const cJSON *jsonobj, const char *name) {
    if (jsonobj && cJSON_IsObject(jsonobj))
    {
        const cJSON *tmp = cJSON_GetObjectItemCaseSensitive(jsonobj, name);
        if (cJSON_IsArray(tmp))
        {
            return tmp;
        } else {
//            Logger::debug("JsonData::get_json_array [%s] is not array", name);
            return NULL;
        }
    } else {
        char* p = cJSON_Print(jsonobj);
        std::string errmsg = "JsonData::get_json_array ["+std::string(name)+"] jsonobj is not an object";
        Logger::error("%s\n%s", errmsg.c_str(), p);
        free(p);
        throw_exception(errmsg);
    }
}

bool JsonData::get_set_value(const cJSON *json, const char *name, std::string &current_val) {
    std::string v = get_string_value(json, name, current_val);
    if (v != current_val) {
        current_val = v;
        return true;
    }
    return false;
}

bool JsonData::get_set_value(const cJSON *json, const char *name, int &current_val) {
    int v = get_int_value(json, name, current_val);
    if (v != current_val) {
        current_val = v;
        return true;
    }
    return false;
}

bool JsonData::get_set_value(const cJSON *json, const char *name, int64_t &current_val) {
    int64_t v = get_int64_value(json, name, current_val);
    if (v != current_val) {
        current_val = v;
        return true;
    }
    return false;
}

bool JsonData::get_set_value(const cJSON *json, const char *name, bool &current_val) {
    bool v = get_bool_value(json, name, current_val);
    if (v != current_val) {
        current_val = v;
        return true;
    }
    return false;
}