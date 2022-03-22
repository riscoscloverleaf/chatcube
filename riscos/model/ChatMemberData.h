//
// Created by lenz on 8/28/20.
//

#ifndef ROCHAT_CHATMEMBERDATA_H
#define ROCHAT_CHATMEMBERDATA_H

#include <string>
#include <vector>
#include "JsonData.h"
#include "AppDataModelTypes.h"

#define CHAT_MEMEBER_STATUS_CREATOR  1
#define CHAT_MEMBER_STATUS_ADMIN     2
#define CHAT_MEMBER_STATUS_NORMAL    3
#define CHAT_MEMEBER_STATUS_READONLY 4
#define CHAT_MEMEBER_STATUS_BANNED   5
#define CHAT_MEMEBER_STATUS_DELETED  6

class ChatMemberData : public JsonData {
public:
    std::string chat_id;
    int status = 0;
    MemberDataPtr member;

    ChatMemberData() {};
    ChatMemberData(const cJSON *jsonobj) {
        update_from_json(jsonobj);
    };
    void update_from_json(const cJSON *jsonobj);
};

#endif //ROCHAT_CHATMEMBERDATA_H
