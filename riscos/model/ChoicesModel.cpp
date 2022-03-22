//
// Created by lenz on 3/1/20.
//

#include "ChoicesModel.h"

void CountryData::update_from_json(const cJSON *jsonobj) {
    code = get_string_value(jsonobj, "code", "");
    phone = get_string_value(jsonobj, "phone", "");
    name = get_string_value(jsonobj, "name", "");
}

const CountryData* ChoicesModel::get_country_by_code(const std::string& code) {
    size_t len = _countries.size();
    for(int i = 0; i < len; i++) {
        if (_countries[i].code == code) {
            return &_countries[i];
        }
    }
    return nullptr;
}

void ChoicesModel::load(const cJSON* json_choices) {
    int idx;
    const cJSON* json_countries = JsonData::get_json_array(json_choices, "COUNTRY_CHOICES");
    cJSON* json_item;
    _countries.clear();
    _countries.reserve(cJSON_GetArraySize(json_countries));
    idx = 0;
    cJSON_ArrayForEach(json_item, json_countries)
    {
        _countries.push_back(CountryData(json_item));
        _countries[idx].index = idx;
        idx++;
    }
}

ChoicesModel g_choices;