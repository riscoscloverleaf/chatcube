/*
 * Message.h
 *
 * Message model representation
 */

#ifndef __ModelMessageData_h
#define __ModelMessageData_h

#include <string>
#include <list>
#include <cloverleaf/Logger.h>
#include "JsonData.h"
#include "AppDataModelTypes.h"

using namespace std;

#define MESSAGE_TYPE_TEXT 1
#define MESSAGE_TYPE_FILE  2
#define MESSAGE_TYPE_PHOTO  3
#define MESSAGE_TYPE_STICKER  4
#define MESSAGE_TYPE_AUDIO  5
#define MESSAGE_TYPE_VIDEO  6
#define MESSAGE_TYPE_CONTACT  7
#define MESSAGE_TYPE_LOCATION  8
#define MESSAGE_TYPE_JOIN  9
#define MESSAGE_TYPE_LEAVE  10
#define MESSAGE_TYPE_CALL  11
#define MESSAGE_TYPE_POLL  12
#define MESSAGE_TYPE_CUSTOM_TYPE  100

#define MESSAGE_FLAG_OUTGOING  1
#define MESSAGE_FLAG_SYSTEM  2
#define MESSAGE_FLAG_DELETED  4
#define MESSAGE_FLAG_CAN_BE_EDITED  8

#define MESSAGE_SENDING_STATE_OK 0
#define MESSAGE_SENDING_STATE_PENDING  1
#define MESSAGE_SENDING_STATE_FAILED  2

#define MESSAGES_FILTER_ATTACHMENTS 1
#define MESSAGES_FILTER_LINKS       2
#define MESSAGES_FILTER_EMAILS      3

#define ENTITY_PLAIN 1
#define ENTITY_BOLD 2
#define ENTITY_ITALIC 3
#define ENTITY_UNDERLINE 4
#define ENTITY_EMAIL 9
#define ENTITY_URL 10
#define ENTITY_TEXT_URL 11
#define ENTITY_SEARCH_FOUND 64


struct TextEntity {
    unsigned int type;
    unsigned int start;
    unsigned int len;
    std::string value;

    inline unsigned int end() const { return start + len; }
};

std::vector<TextEntity> merge_entities(const std::vector<TextEntity>& first_entities, const std::vector<TextEntity>& merge_with_entities);
extern TextEntity EMPTY_TEXT_ENTITY;

class AttachmentFile : public JsonData {
public:
    std::string url;
    std::string name;
    std::string thumb_url;
    std::string thumb_url_cached;
    int thumb_width = 0;
    int thumb_height = 0;
    int width = 0;
    int height = 0;
    int file_type = 0;
    int duration = 0;
    long size = 0;

    AttachmentFile() {};
    AttachmentFile(const cJSON *json);
    AttachmentFile(const AttachmentFile& other);
    void update_from_json(const cJSON *json);
};

class AttachmentImage : public JsonData {
public:
    std::string url;
    std::string thumb_url;
    std::string thumb_url_cached;
    long size = 0;
    int thumb_width = 0;
    int thumb_height = 0;
    int width = 0;
    int height = 0;

    AttachmentImage() {};
    AttachmentImage(const cJSON *json);
    AttachmentImage(const AttachmentImage& att_image);
    void update_from_json(const cJSON *json);
};

class ReplyInfo : public JsonData {
public:
    int64_t id = 0;
    int type = 0;
    AttachmentFile *att_file = nullptr;
    AttachmentImage *att_image = nullptr;
    MemberDataPtr author = nullptr;
    std::string author_id;
    std::string text;

    ReplyInfo(const cJSON *json);
    ReplyInfo(const ReplyInfo& other);
    ~ReplyInfo();
    void update_from_json(const cJSON *json);
};

class ForwardInfo : public JsonData {
public:
    std::string title;
    std::string user_id;
    std::string chat_id;

    ForwardInfo(const cJSON *json);
    ForwardInfo(const ForwardInfo& other);

    void update_from_json(const cJSON *json);
};
//class ChatData;

class MessageData : public JsonData {
private:
    void delete_text_entities();
public:
    int64_t id = 0;
    int type = 0;
    uint32_t flags;
    int sending_state = 0;
    std::string text;
    time_t sendtime = 0;
    time_t changedtime = 0;

    std::string author_id;
    MemberDataPtr author = nullptr;
    AttachmentFile *att_file = nullptr;
    AttachmentImage *att_image = nullptr;
    ReplyInfo *reply_info = nullptr;
    ForwardInfo *forward_info = nullptr;
    std::weak_ptr<ChatData> chat;
    std::vector<TextEntity> text_entities;

    MessageData() {};
    MessageData(const cJSON *jsonobj, ChatDataPtr chat_) {
        update_from_json(jsonobj);
        chat = chat_;
        //Logger::debug("MessageData::MessageData id=%lld", id);
    };
    ~MessageData();

    ChatDataPtr get_chat() { return chat.lock(); }

    std::string get_entity_value(const TextEntity& entity);
    void update_from_json(const cJSON *json);
    void update_from_message(MessageDataPtr msg);
    bool is_system()  const { return ((flags & MESSAGE_FLAG_SYSTEM) != 0); }
    bool is_outgoing() const { return ((flags & MESSAGE_FLAG_OUTGOING) != 0); }
    bool is_deleted() const { return ((flags & MESSAGE_FLAG_DELETED) != 0); }
    bool can_be_edited()  const { return ((flags & MESSAGE_FLAG_CAN_BE_EDITED) != 0); }
    bool is_send_success() const { return sending_state == 0; }
    bool has_attachment() const { return (type == MESSAGE_TYPE_FILE
                            || type == MESSAGE_TYPE_PHOTO
                            || type == MESSAGE_TYPE_STICKER
                            || type == MESSAGE_TYPE_AUDIO
                            || type == MESSAGE_TYPE_VIDEO); }
    bool has_image() { return type == MESSAGE_TYPE_PHOTO; }

    bool is_filter_and_query_matched(int filter, const std::string& substring_query);
    bool is_filter_matched(int filter);
};

#endif