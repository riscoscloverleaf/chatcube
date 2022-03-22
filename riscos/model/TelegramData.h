//
// Created by lenz on 2/10/20.
//

#ifndef ROCHAT_TELEGRAMDATA_H
#define ROCHAT_TELEGRAMDATA_H

#include <string>
#include "JsonData.h"
#include "AppDataModelTypes.h"


class TelegramAuthCodeData : public JsonData {
public:
    TelegramAuthCodeData() {};
    TelegramAuthCodeData(const cJSON *jsonobj) {
        update_from_json(jsonobj);
    };

    std::string phone;
    std::string auth_type;
    int resend_timeout;

    void update_from_json(const cJSON *json);
};

class TelegramAuthPasswordData : public JsonData {
public:
    TelegramAuthPasswordData() {};
    TelegramAuthPasswordData(const cJSON *jsonobj) {
        update_from_json(jsonobj);
    };

    std::string password_hint;

    void update_from_json(const cJSON *json);
};

class TelegramAuthRegistrationData : public JsonData {
public:
    TelegramAuthRegistrationData() {};
    TelegramAuthRegistrationData(const cJSON *jsonobj) {
        update_from_json(jsonobj);
    };

    std::string tos_text;

    void update_from_json(const cJSON *json);
};

class TelegramTermsOfServiceData : public JsonData {
public:
    TelegramTermsOfServiceData() {};
    TelegramTermsOfServiceData(const cJSON *jsonobj) {
        update_from_json(jsonobj);
    };

    std::string tos_text;
    std::string tos_id;

    void update_from_json(const cJSON *json);
};

#endif //ROCHAT_TELEGRAMDATA_H
