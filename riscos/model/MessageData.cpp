#include "MessageData.h"
#include "cloverleaf/CLUtf8.h"
#include "cloverleaf/CLException.h"
#include "cloverleaf/Logger.h"

//typedef std::shared_ptr<MessageData> MessageDataPtr;

AttachmentFile::AttachmentFile(const AttachmentFile& other) {
    url = other.url;
    name = other.name;
    size = other.size;
    duration = other.duration;
    file_type = other.file_type;
    thumb_url = other.thumb_url;
    thumb_width = other.thumb_width;
    thumb_height = other.thumb_height;
    width = other.width;
    height = other.height;
}

AttachmentFile::AttachmentFile(const cJSON *json) {
    update_from_json(json);
}

void AttachmentFile::update_from_json(const cJSON *json) {
    url = get_string_value(json, "url", url);
    name = get_string_value(json, "name", name);
    size = get_int_value(json, "size", size);
    duration = get_int_value(json, "duration", duration);
    file_type = get_int_value(json, "file_type", file_type);
    thumb_url = get_string_value(json, "thumb_url", thumb_url);
    thumb_height = get_int_value(json, "thumb_height", thumb_height);
    thumb_width = get_int_value(json, "thumb_width", thumb_width);
    height = get_int_value(json, "height", height);
    width = get_int_value(json, "width", width);
}


AttachmentImage::AttachmentImage(const AttachmentImage& att_image) {
    url = att_image.url;
    thumb_url = att_image.thumb_url;
    size = att_image.size;
    thumb_height = att_image.thumb_height;
    thumb_width = att_image.thumb_width;
    height = att_image.height;
    width = att_image.width;
}

AttachmentImage::AttachmentImage(const cJSON *json) {
    update_from_json(json);
}

void AttachmentImage::update_from_json(const cJSON *json) {
    url = get_string_value(json, "url", url);
    thumb_url = get_string_value(json, "thumb_url", thumb_url);
    size = get_int_value(json, "size", size);
    thumb_height = get_int_value(json, "thumb_height", thumb_height);
    thumb_width = get_int_value(json, "thumb_width", thumb_width);
    height = get_int_value(json, "height", height);
    width = get_int_value(json, "width", width);
}


ReplyInfo::ReplyInfo(const cJSON *json) {
    update_from_json(json);
}

ReplyInfo::ReplyInfo(const ReplyInfo& other) {
    id = other.id;
    type = other.type;
    text = other.text;
    author_id = other.author_id;
    author = other.author;
    if (att_file) {
        delete att_file;
        att_file = nullptr;
    }
    if (other.att_file) {
        att_file = new AttachmentFile(*other.att_file);
    }
    if (att_image) {
        delete att_image;
        att_image = nullptr;
    }
    if (other.att_image) {
        att_image = new AttachmentImage(*other.att_image);
    }
}

void ReplyInfo::update_from_json(const cJSON *json) {
    id = get_int64_value(json, "id");
    type = get_int_value(json, "type", type);
    text = get_string_value(json, "text", text);
    author_id = get_string_value(json, "author_id", "");

    const cJSON *reply_json = get_json_object_or_null(json, "attachment_file");
    if (reply_json) {
        if (!att_file) {
            att_file = new AttachmentFile(reply_json);
        } else {
            att_file->update_from_json(reply_json);
        }
    } else {
        delete att_file;
        att_file = nullptr;
    }

    reply_json = get_json_object_or_null(json, "attachment_image");
    if (reply_json) {
        if (!att_image) {
            att_image = new AttachmentImage(reply_json);
        } else {
            att_image->update_from_json(reply_json);
        }
    } else {
        delete att_image;
        att_image = nullptr;
    }
}

ReplyInfo::~ReplyInfo() {
    delete att_image;
    delete att_file;
    author = nullptr;
}

ForwardInfo::ForwardInfo(const ForwardInfo& other) {
    title = other.title;
    user_id = other.user_id;
    chat_id = other.chat_id;
}

ForwardInfo::ForwardInfo(const cJSON *json) {
    update_from_json(json);
}

void ForwardInfo::update_from_json(const cJSON *json) {
    title = get_string_value(json, "title");
    user_id = get_string_value(json, "user_id", "");
    chat_id = get_string_value(json, "chat_id", "");
}

void MessageData::update_from_json(const cJSON *json)
{
    const cJSON *tmp_json, *json_item;

    id = get_int64_value(json, "new_id", get_int64_value(json, "id"));
    type = get_int_value(json, "type", type);
    flags = get_int_value(json, "flags", flags);
    text = get_string_value(json, "text", text);
    author_id = get_string_value(json, "author_id", author_id);
    sendtime = get_time_value(json, "sendtime", sendtime);
    changedtime = get_time_value(json, "changedtime", changedtime);
    sending_state = get_int_value(json, "sending_state", sending_state);
//    Logger::debug("MessageData::update_from_json sending_state=%d", sending_state);

    tmp_json = get_json_object_or_null(json, "attachment_image");
    if (tmp_json) {
        if (!att_image) {
            att_image = new AttachmentImage(tmp_json);
        } else {
            att_image->update_from_json(tmp_json);
        }
    }

    tmp_json = get_json_object_or_null(json, "attachment_file");
    if (tmp_json) {
        if (!att_file) {
            att_file = new AttachmentFile(tmp_json);
        } else {
            att_file->update_from_json(tmp_json);
        }
    }

    tmp_json = get_json_object_or_null(json, "reply_info");
    if (tmp_json) {
        if (!reply_info) {
            reply_info = new ReplyInfo(tmp_json);
        } else {
            reply_info->update_from_json(tmp_json);
        }
    }

    tmp_json = get_json_object_or_null(json, "forward_info");
    if (tmp_json) {
        if (!forward_info) {
            forward_info = new ForwardInfo(tmp_json);
        } else {
            forward_info->update_from_json(tmp_json);
        }
    }

    tmp_json = get_json_array(json, "entities");
    text_entities.clear();
    if (tmp_json) {
        cJSON_ArrayForEach(json_item, tmp_json) {
            unsigned int start = get_int_value(json_item, "s", 0),
                len = get_int_value(json_item, "l", 0),
                start_b = 0;
            if (start > 0) {
                start_b = utf8_len_bytes_substr(text, 0, start);
            }
            text_entities.emplace_back(TextEntity{
                    .type = static_cast<unsigned int>(get_int_value(json_item, "t", 0)),
                    .start = start_b,
                    .len = utf8_len_bytes_substr(text, start, len),
                    .value = get_string_value(json_item, "v", "")
            });
        }
    }
}

void MessageData::update_from_message(MessageDataPtr msg) {
    id = msg->id;
    type = msg->type;
    flags = msg->flags;
    text = msg->text;
    sendtime = msg->sendtime;
    changedtime = msg->changedtime;
    sending_state = msg->sending_state;

    delete att_image;
    if (msg->att_image != nullptr) {
        att_image = new AttachmentImage(*msg->att_image);
    } else {
        att_image = nullptr;
    }

    delete att_file;
    if (msg->att_file != nullptr) {
        att_file = new AttachmentFile(*msg->att_file);
    } else {
        att_file = nullptr;
    }

    text_entities = msg->text_entities;

    delete reply_info;
    if (msg->reply_info) {
        reply_info = new ReplyInfo(*msg->reply_info);
    } else {
        reply_info = nullptr;
    }

    delete forward_info;
    if (msg->forward_info) {
        forward_info = new ForwardInfo(*msg->forward_info);
    } else {
        forward_info = nullptr;
    }
}

std::string MessageData::get_entity_value(const TextEntity& e) {
    if (e.value.empty()) {
        return utf8_substr(text, e.start, e.len);
    } else {
        return e.value;
    }
}

bool MessageData::is_filter_matched(int filter) {
    switch(filter) {
        case MESSAGES_FILTER_EMAILS:
            if (!text_entities.empty()) {
                for(auto &ent: text_entities) {
                    if (ent.type == ENTITY_EMAIL) {
                        return true;
                    }
                }
            }
            break;
        case MESSAGES_FILTER_LINKS:
            if (!text_entities.empty()) {
                for(auto &ent: text_entities) {
                    if (ent.type == ENTITY_URL || ent.type == ENTITY_TEXT_URL) {
                        return true;
                    }
                }
            }
            break;
        case MESSAGES_FILTER_ATTACHMENTS:
            if (att_file || att_image) {
                return true;
            }
            break;
        default:
            return true;
    }
    return false;
}

bool MessageData::is_filter_and_query_matched(int filter, const std::string& substring_query) {
    if (is_filter_matched(filter)) {
        if (substring_query.empty()) {
            return true;
        }
        return (utf8casestr(text.c_str(), substring_query.c_str()) != NULL);
    }
    return false;
}

MessageData::~MessageData() {
    delete att_file;
    delete att_image;
    delete reply_info;
    delete forward_info;
    //Logger::debug("~MessageData id=%lld", id);
}

class TextPoint {
public:
    TextPoint(int type, unsigned int pos, const string &value, bool start) :
        type(type), pos(pos), value(value), start(start) {}

    int type;
    unsigned int pos;
    std::string value;
    bool start;
};

std::vector<TextEntity> merge_entities(const std::vector<TextEntity>& first_entities, const std::vector<TextEntity>& merge_with_entities) {
    if (first_entities.empty()) {
        return merge_with_entities;
    }
    if (merge_with_entities.empty()) {
        return first_entities;
    }
//    Logger::debug("merge_entities first_entities %d merge_with_entities %d", first_entities.size(), merge_with_entities.size());
    std::vector<TextPoint> points, points1;
    points1.reserve(merge_with_entities.size()*2);
    points.reserve(merge_with_entities.size()*2 + first_entities.size() * 2);
    for(auto &ent : merge_with_entities) {
        points1.emplace_back(TextPoint(ent.type, ent.start, ent.value, true));
        points1.emplace_back(TextPoint(ent.type, ent.end(), ent.value, false));
    }

    auto points1_it = points1.begin();
    auto points1_end = points1.end();
    for(auto &ent : first_entities) {
        while (points1_it != points1_end) {
            if (ent.start > points1_it->pos) {
                points.emplace_back(*points1_it);
                points1_it++;
            } else {
                break;
            }
        }
        points.emplace_back(TextPoint(ent.type, ent.start, ent.value, true));

        auto ent_end = ent.end();
        while (points1_it != points1_end) {
            if (ent_end > points1_it->pos) {
                points.emplace_back(*points1_it);
                points1_it++;
            } else {
                break;
            }
        }
        points.emplace_back(TextPoint(ent.type, ent_end, ent.value, false));
    }
    while (points1_it != points1_end) {
        points.emplace_back(*points1_it);
        points1_it++;
    }
//    Logger::debug("merge_entities merge points end");

    std::vector<TextEntity> result;
    result.reserve(points.size() / 2);
    TextPoint &prev_pt = *points.begin();
    unsigned int depth = 0, len;
    unsigned int ent_type = prev_pt.type;
    for(auto points_it = points.begin() + 1; points_it != points.end(); points_it++) {
        TextPoint &pt = *points_it;
        if (ent_type > 0) {
            len = pt.pos - prev_pt.pos;
            if (len > 0) {
                result.emplace_back(TextEntity{
                        .type = ent_type,
                        .start = prev_pt.pos,
                        .len = len,
                        .value = pt.value
                });
                Logger::debug("type=%d pt.type=%d prev_pt.type=%d start=%d len=%d depth=%d ptstart=%d", ent_type, pt.type, prev_pt.type, prev_pt.pos, len, depth, pt.start);
            }
        }
        if (pt.start) {
            depth++;
            ent_type += pt.type;
        } else {
            ent_type -= pt.type;
            depth--;
        }
        prev_pt = pt;
    }
    return result;
}

TextEntity EMPTY_TEXT_ENTITY = TextEntity{0,0,0,""};