//
// Created by lenz on 1/31/20.
//

#ifndef ROCHAT_CHATDATA_H
#define ROCHAT_CHATDATA_H
#include <map>
#include <cstdint>
#include <string>
#include <vector>
#include "JsonData.h"
#include "ChatMemberData.h"
#include "AppDataModelTypes.h"

#define CHAT_TYPE_PRIVATE  1
#define CHAT_TYPE_SECRET   2
#define CHAT_TYPE_GROUP    50
#define CHAT_TYPE_CHANNEL  100

#define CHAT_ACTION_CANCEL    0
#define CHAT_ACTION_TYPING    1

#define CHAT_CHANGES_TITLE                    1
#define CHAT_CHANGES_PIC_SMALL                2
#define CHAT_CHANGES_OUTGOING_SEEN_MESSAGE_ID 4
#define CHAT_CHANGES_INCOMING_SEEN_MESSAGE_ID 8
#define CHAT_CHANGES_MY_STATUS                16
#define CHAT_CHANGES_MEMBERS_COUNT            32
#define CHAT_CHANGES_UNREAD_COUNT             64
#define CHAT_CHANGES_LAST_MSG                 128
#define CHAT_CHANGES_ONLINE                   256
#define CHAT_CHANGES_ACTION                   512

typedef std::vector<MessageDataPtr> MessagesVector;

class ChatData : public JsonData {
public:
    MessagesVector messages;
    MessagesVector messages_pending_outgoing;
    std::string messages_load_older_url;
    std::string messages_load_newer_url;
    bool messages_was_loaded = false;
    int messages_filter = 0;

    std::string id;
    std::string title;
    std::string pic_small;
    std::string pic_small_cached;
    std::string action_line;
    int64_t outgoing_seen_message_id = 0;
    int64_t incoming_seen_message_id = 0;
    int my_status = 0;
    int members_count = 0;
    int unread_count = 0;
    int type = 0;

    MemberDataPtr member;
    MessageDataPtr last_message;

    ChatData() {};
    ChatData(const cJSON *jsonobj) {
        update_from_json(jsonobj);
    };

    unsigned int update_from_json(const cJSON *jsonobj);
    void sort_messages();
    MessageDataPtr get_message(int64_t msg_id);
    int get_message_index(int64_t msg_id);
    std::string get_last_message_text(int len);
    bool is_online();
    bool is_member_active();

    inline char messenger() {
        return id[0];
    }

    inline bool is_private() {
        return type == CHAT_TYPE_PRIVATE || type == CHAT_TYPE_SECRET;
    }

    inline bool has_message(int64_t msg_id) {
        return (get_message(msg_id) != nullptr);
    }

    inline bool has_newer_messages() {
        return !messages_load_newer_url.empty();
    }

    inline bool has_older_messages() {
        return !messages_load_older_url.empty();
    }

    inline MessagesVector& get_messages() {
        return messages;
    }
};

#endif //ROCHAT_CHATDATA_H
