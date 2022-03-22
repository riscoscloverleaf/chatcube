//
// Created by lenz on 2/3/20.
//
#include <algorithm>
#include "AppDataModelTypes.h"
#include "ChatData.h"
#include "MemberData.h"
#include "MessageData.h"
#include "FileCacheDownloader.h"
#include <cloverleaf/Logger.h>
//#include "../libs/drsl/drsl.h"

unsigned int ChatData::update_from_json(const cJSON *json)
{
    unsigned int changes = 0;

    id = get_string_value(json, "id");
    type = get_int_value(json, "type", type);

    if (get_set_value(json, "title", title)) {
        changes |= CHAT_CHANGES_TITLE;
    }

    if (get_set_value(json, "pic_small", pic_small)) {
        changes |= CHAT_CHANGES_PIC_SMALL;
    }

    if (get_set_value(json, "outgoing_seen_message_id", outgoing_seen_message_id)) {
        changes |= CHAT_CHANGES_OUTGOING_SEEN_MESSAGE_ID;
    }

    if (get_set_value(json, "incoming_seen_message_id", incoming_seen_message_id)) {
        changes |= CHAT_CHANGES_INCOMING_SEEN_MESSAGE_ID;
    }

    if (get_set_value(json, "my_status", my_status)) {
        changes |= CHAT_CHANGES_MY_STATUS;
    }

    if (get_set_value(json, "members_count", members_count)) {
        changes |= CHAT_CHANGES_MEMBERS_COUNT;
    }

    if (get_set_value(json, "unread_count", unread_count)) {
        changes |= CHAT_CHANGES_UNREAD_COUNT;
    }
    return changes;
}

bool ChatData::is_online() {
//    Logger::debug("ChatData::is_online %s", title.c_str());
    if (member != nullptr) {
//        Logger::debug("ChatData::is_online %s %d", title.c_str(), member->online);
        return member->online;
    }
    return false;
}

bool ChatData::is_member_active() {
//    Logger::debug("ChatData::is_online %s", title.c_str());
    if (member != nullptr) {
//        Logger::debug("ChatData::is_online %s %d", title.c_str(), member->online);
        return member->active;
    }
    return false;
}

MessageDataPtr ChatData::get_message(int64_t msg_id) {
    for(auto m : messages) {
        if (m->id == msg_id) {
            return m;
        }
    }
    return nullptr;
}

int ChatData::get_message_index(int64_t msg_id) {
    int messages_count = messages.size();
    for(int n = 0; n < messages_count; n++) {
        if (messages[n]->id == msg_id) {
            return n;
        }
    }
    return -1;
}

std::string ChatData::get_last_message_text(int len) {
    if (last_message == nullptr) {
        //Logger::debug("chat %s last_message empty", title.c_str());
        return std::string("");
    }
    std::string txt;
    //drsl::string text(last_message->text.c_str());
    if (last_message->text.size() > len) {
        int pos = last_message->text.find(' ');
        txt = last_message->text.substr(0, pos+1);
    } else {
        txt = last_message->text;
    }
//    for(size_t i = 0; i < txt.size(); i++) {
//        if (txt[i] == '\n' || txt[i] == '\r' || txt[i] == '\t') {
//            txt[i] = ' ';
//        }
//    }
//    txt = str_replace_all(txt, "⚡️", " ");
    return txt;
}

void ChatData::sort_messages() {
    std::sort(messages.begin(), messages.end(), [](const std::shared_ptr<MessageData> a, const std::shared_ptr<MessageData> b) {
        if (a->sendtime == b->sendtime) {
            return a->id < b->id;
        }
        return (a->sendtime < b->sendtime);
    });
}
