//
// Created by lenz on 2/3/20.
//
#include "cloverleaf/CLException.h"
#include "AvatarData.h"

void AvatarData::update_from_json(const cJSON *json) {
    if (!json || !cJSON_IsArray(json)) {
        throw_exception("json is invalid");
    }
    cJSON* jurl = cJSON_GetArrayItem(json, 0);
    cJSON* jname = cJSON_GetArrayItem(json, 1);
    if (jurl) {
        pic_small = jurl->valuestring;
    } else {
        throw_exception("json is invalid, url == null");
    }
    if (jname) {
        name = jname->valuestring;
    } else {
        throw_exception("json is invalid, name == null");
    }
}