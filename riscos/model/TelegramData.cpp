//
// Created by lenz on 2/10/20.
//
#include "TelegramData.h"

void TelegramAuthCodeData::update_from_json(const cJSON *json) {
    const cJSON *type_json = JsonData::get_json_object(json, "type");
    auth_type = JsonData::get_string_value(type_json, "@type");
    phone = JsonData::get_string_value(json, "phone_number");
    resend_timeout = JsonData::get_int_value(json, "timeout", 0);
}

void TelegramAuthPasswordData::update_from_json(const cJSON *json) {
    password_hint = JsonData::get_string_value(json, "password_hint");
}

void TelegramAuthRegistrationData::update_from_json(const cJSON *json) {
    tos_text = JsonData::get_string_value(json, "tos_text");
}

void TelegramTermsOfServiceData::update_from_json(const cJSON *json) {
    tos_text = JsonData::get_string_value(json, "tos_text");
    tos_id = JsonData::get_string_value(json, "tos_id");
}