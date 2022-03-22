#include "MemberData.h"
#include "FileCacheDownloader.h"
#include <cloverleaf/Logger.h>

unsigned int MemberData::update_from_json(const cJSON *jsonobj)
{
    unsigned int changes = 0;
    id = get_string_value(jsonobj, "id");
    if (country) {
        country = g_choices.get_country_by_code(get_string_value(jsonobj, "country", country->code));
    } else {
        country = g_choices.get_country_by_code(get_string_value(jsonobj, "country", ""));
    }
    date_joined = get_time_value(jsonobj, "date_joined", date_joined);
    was_online = get_time_value(jsonobj, "was_online", was_online);
    last_action = get_time_value(jsonobj, "last_action", was_online); // if last_action missing (For telegram) then take was_online as default

    if (get_set_value(jsonobj, "first_name", first_name)) {
        changes |= MEMBER_CHANGES_PROFILE;
    }
    if (get_set_value(jsonobj, "last_name", last_name)) {
        changes |= MEMBER_CHANGES_PROFILE;
    }
    if (get_set_value(jsonobj, "userid", userid)) {
        changes |= MEMBER_CHANGES_PROFILE;
    }
    if (get_set_value(jsonobj, "displayname", displayname)) {
        changes |= MEMBER_CHANGES_PROFILE;
    }
    if (get_set_value(jsonobj, "email", email)) {
        changes |= MEMBER_CHANGES_PROFILE;
    }
    if (get_set_value(jsonobj, "phone", phone)) {
        changes |= MEMBER_CHANGES_PROFILE;
    }
    if (get_set_value(jsonobj, "city", city)) {
        changes |= MEMBER_CHANGES_PROFILE;
    }
    if (get_set_value(jsonobj, "website", website)) {
        changes |= MEMBER_CHANGES_PROFILE;
    }
    if (get_set_value(jsonobj, "pic", pic)) {
        changes |= MEMBER_CHANGES_PIC;
    }
    if (get_set_value(jsonobj, "pic_small", pic_small)) {
        changes |= MEMBER_CHANGES_PIC_SMALL;
    }
    if (get_set_value(jsonobj, "online", online)) {
        changes |= MEMBER_CHANGES_ONLINE;
    }

    const cJSON *tmp = cJSON_GetObjectItemCaseSensitive(jsonobj, "active"); // active may be absent (for Telegram)
    if (tmp) {
        bool act = cJSON_IsTrue(tmp);
        if (act != active) {
            changes |= MEMBER_CHANGES_ONLINE;
        }
        active = act;
    } else {
        active = true;
    }
    return changes;
}

std::string MemberData::get_cached_pic_small() {
    return g_file_cache_downloader.get_cached_file_for_url(pic_small);
}

std::string MemberData::get_cached_pic() {
    return g_file_cache_downloader.get_cached_file_for_url(pic);
}

unsigned int MyMemberData::update_from_json(const cJSON *jsonobj)
{
    const cJSON *inner_json;

//    char* p = cJSON_Print(jsonobj);
//    Logger::debug("MyMemberData::update_from_json json=%s", p)
//    free(p);

    unsigned int changes = MemberData::update_from_json(jsonobj);

    if (get_set_value(jsonobj, "pic_medium", pic_medium)) {
        changes |= MEMBER_CHANGES_PIC_MEDIUM;
    }
    push_channel = get_string_value(jsonobj, "push_channel", push_channel);

    inner_json = get_json_object_or_null(jsonobj, "notification_settings");
    if (inner_json) {
        notification_settings.popup = get_bool_value(inner_json, "popup", notification_settings.popup);
//        Logger::debug("MyMemberData::update_from_json popup=%d",notification_settings.popup);
        notification_settings.taskbar = get_bool_value(inner_json, "taskbar", notification_settings.taskbar);
        notification_settings.sound = get_bool_value(inner_json, "sound", notification_settings.sound);
        notification_settings.unread_age = get_int_value(inner_json, "unread_age", notification_settings.unread_age);
    }

    inner_json = get_json_object_or_null(jsonobj, "telegram_account");
    if (inner_json) {
        telegram_account.phone = get_string_value(inner_json, "phone", telegram_account.phone);
        telegram_account.first_name = get_string_value(inner_json, "first_name", telegram_account.first_name);
        telegram_account.last_name = get_string_value(inner_json, "last_name", telegram_account.last_name);
        telegram_account.username = get_string_value(inner_json, "username", telegram_account.username);
        telegram_account.pic = get_string_value(inner_json, "pic", telegram_account.pic);
        telegram_account.tg_user_id = get_int64_value(inner_json, "tg_user_id", telegram_account.tg_user_id);
        telegram_account.user_id = "T"+to_string(telegram_account.tg_user_id);
    }
    return changes;
}

void MyMemberData::clear_telegram() {
    telegram_account.phone.clear();
    telegram_account.tg_user_id = 0;
    telegram_account.user_id.clear();
}
