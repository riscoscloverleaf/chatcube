/*
 * Member.h
 *
 * member model representation
 */

#ifndef ROCHAT_MODELMEMBERDATA_H
#define ROCHAT_MODELMEMBERDATA_H

#include <string>
#include "JsonData.h"
#include "ChoicesModel.h"
#include "AppDataModelTypes.h"

#define MEMBER_CHANGES_PROFILE                  1
#define MEMBER_CHANGES_PIC                      2
#define MEMBER_CHANGES_PIC_SMALL                4
#define MEMBER_CHANGES_PIC_MEDIUM               8
#define MEMBER_CHANGES_ONLINE                   16

#define MEMBER_INACTIVE_TIMEOUT                 240

class MemberData : public JsonData {
public:
    std::string id;
    std::string first_name;
    std::string last_name;
    std::string userid;
    std::string displayname;
    std::string email;
    std::string pic;
    std::string pic_small;
    std::string pic_cached;
    std::string pic_small_cached;
    std::string phone;
    std::string city;
    std::string website;
    const CountryData *country = nullptr;

    time_t date_joined;
    time_t was_online;
    time_t last_action;
    bool active;
    bool online;

    std::weak_ptr<ChatData> chat;

    MemberData() {
        chat.reset();
    };
    MemberData(const cJSON *jsonobj) {
        chat.reset();
        update_from_json(jsonobj);
    };

    ChatDataPtr get_chat() { return chat.lock(); }

    std::string get_cached_pic_small();
    std::string get_cached_pic();

    inline char messenger() {
        return id[0];
    }


    virtual unsigned int update_from_json(const cJSON *jsonobj);
};

class MyMemberData : public MemberData {
public:
    std::string pic_medium;
    std::string pic_medium_cached;
    std::string push_channel;

    struct NotificationSettings {
        bool popup = false;
        bool taskbar = false;
        bool sound = false;
        int unread_age = 2;
    } notification_settings;

    struct TelegramAccount {
        std::string phone;
        std::string first_name;
        std::string last_name;
        std::string username;
        std::string pic;
        std::string pic_cached;
        int64_t tg_user_id = 0;
        std::string user_id;
    } telegram_account;

    MyMemberData() {};
    MyMemberData(const cJSON *jsonobj) { update_from_json(jsonobj); }

    bool has_telegram() { return !telegram_account.phone.empty(); };
    void clear_telegram();

    virtual unsigned int update_from_json(const cJSON *jsonobj);
};

#endif