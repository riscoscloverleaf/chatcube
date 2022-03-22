//
// Created by lenz on 3/1/20.
//

#ifndef ROCHAT_CHOICESMODEL_H
#define ROCHAT_CHOICESMODEL_H

#include <vector>
#include <string>
#include "JsonData.h"

class CountryData : public JsonData {
public:
    std::string code;
    std::string phone;
    std::string name;
    int index;

    CountryData() {};
    CountryData(const cJSON *jsonobj) {
        update_from_json(jsonobj);
    };

    void update_from_json(const cJSON *jsonobj);

};


class ChoicesModel {
private:
    std::vector<CountryData> _countries;

public:
    void                                    load(const cJSON* json);

    int                                     get_country_idx_by_code(const std::string& code);
    const CountryData*                      get_country_by_code(const std::string& code);
    const std::vector<CountryData> &        get_countries() { return _countries; }
};

extern ChoicesModel g_choices;

#endif //ROCHAT_CHOICESMODEL_H
