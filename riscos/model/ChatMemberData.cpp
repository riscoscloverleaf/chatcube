//
// Created by lenz on 8/28/20.
//

#include "ChatMemberData.h"

void ChatMemberData::update_from_json(const cJSON *json) {
    chat_id = get_string_value(json, "chat_id");
    status = get_int_value(json, "status", status);
}

