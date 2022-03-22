//
// Created by lenz on 1/31/20.
//

#include "AppDataModel.h"
#include "NetworkRequests.h"
#include "TelegramData.h"
#include <cloverleaf/Logger.h>

std::shared_ptr<MyMemberData> AppDataModel::update_my_member_data(const cJSON* json) {
    std::string old_pic_medium, old_pic_small, old_tg_pic;
    if (me == nullptr) {
        me = std::make_shared<MyMemberData>(json);
        _members_map[me->id] = me;
    } else {
        old_pic_medium = me->pic_medium;
        old_pic_small = me->pic_small;
        old_tg_pic = me->telegram_account.pic;
        me->update_from_json(json);
    }
    if (!me->pic_medium.empty()) {
        me->pic_medium_cached = g_file_cache_downloader.get_cached_file_for_url(me->pic_medium);
        if (me->pic_medium_cached.empty()) {
            auto on_pic_loaded = [this](const std::string &filename) {
                me->pic_medium_cached = filename;
                g_app_events.notify(AppEvents::MyChatcubeAvatarChanged{});
            };
            g_file_cache_downloader.download_url(me->pic_medium, on_pic_loaded);
        } else if (old_pic_medium != me->pic_medium) {
            g_app_events.notify(AppEvents::MyChatcubeAvatarChanged{});
        }
    }
    if (!me->pic_small.empty()) {
        me->pic_small_cached = g_file_cache_downloader.get_cached_file_for_url(me->pic_small);
        if (me->pic_small_cached.empty()) {
            auto on_pic_small_loaded = [this](const std::string &filename) {
                me->pic_small_cached = filename;
                g_app_events.notify(AppEvents::MyChatcubeAvatarChanged{});
            };
            g_file_cache_downloader.download_url(me->pic_small, on_pic_small_loaded);
        }
    }
    if (!me->telegram_account.pic.empty()) {
        me->telegram_account.pic_cached = g_file_cache_downloader.get_cached_file_for_url(me->telegram_account.pic);
        if (me->telegram_account.pic_cached.empty()) {
            auto on_tg_pic_loaded = [this](const std::string &filename) {
                me->telegram_account.pic_cached = filename;
                g_app_events.notify(AppEvents::MyTelegramAvatarChanged{});
            };
            g_file_cache_downloader.download_url(me->telegram_account.pic, on_tg_pic_loaded);
        } else if (old_tg_pic != me->telegram_account.pic_cached) {
            g_app_events.notify(AppEvents::MyTelegramAvatarChanged{});
        }
    }
    return me;
}

MemberDataPtr AppDataModel::update_or_create_member_data(const cJSON* json, bool do_send_update_event, bool cache_member) {
    std::string id = MemberData::get_string_value(json, "id");
    if (id.empty()) {
        Logger::error("update_member id is empty");
        return nullptr;
    }
    unsigned int changes = 0;
    MemberDataPtr mem = get_member(id);
    bool is_new_member = false;
    if (mem) {
        if (mem == me) {
            return update_my_member_data(json);
        }
        changes = mem->update_from_json(json);
    } else {
        is_new_member = true;
        mem = make_shared<MemberData>(json);
        if (cache_member) {
            _members_map[id] = mem;
        }
    }

//    Logger::debug("%s my %s %d", id.c_str(), me->telegram_account.user_id.c_str(), me->has_telegram());
    if (id[0] == 'T' && me->has_telegram() &&  id == me->telegram_account.user_id) {
        Logger::debug("Update my telegram account");
        me->telegram_account.first_name = mem->first_name;
        me->telegram_account.last_name = mem->last_name;
        me->telegram_account.username = mem->userid;
        me->telegram_account.pic = mem->pic;
        if (!me->telegram_account.pic.empty()) {
            std::string old_tg_pic_cached = me->telegram_account.pic_cached;
            me->telegram_account.pic_cached = g_file_cache_downloader.get_cached_file_for_url(me->telegram_account.pic);
            if (me->telegram_account.pic_cached.empty()) {
                auto on_tg_pic_loaded = [this](const std::string &filename) {
                    me->telegram_account.pic_cached = filename;
                    Logger::debug("my telegram account on_tg_pic_loaded MyTelegramAvatarChanged");
                    g_app_events.notify(AppEvents::MyTelegramAvatarChanged{});
                };
                g_file_cache_downloader.download_url(me->telegram_account.pic, on_tg_pic_loaded);
            } else if (old_tg_pic_cached != me->telegram_account.pic_cached) {
                Logger::debug("my telegram account MyTelegramAvatarChanged");
                g_app_events.notify(AppEvents::MyTelegramAvatarChanged{});
            }
        }
    }

    if (!mem->pic.empty()) {
        mem->pic_cached = g_file_cache_downloader.get_cached_file_for_url(mem->pic);
        if (mem->pic_cached.empty()) {
            changes &= ~MEMBER_CHANGES_PIC;
            auto on_pic_loaded = [mem, do_send_update_event](const std::string &filename) {
                mem->pic_cached = filename;
                g_app_events.notify(AppEvents::MemberChanged{.mem=mem, .changes=MEMBER_CHANGES_PIC});
            };
            g_file_cache_downloader.download_url(mem->pic, on_pic_loaded);
        }
    }
    if (!mem->pic_small.empty()) {
        mem->pic_small_cached = g_file_cache_downloader.get_cached_file_for_url(mem->pic_small);
        if (mem->pic_small_cached.empty()) {
            changes &= ~MEMBER_CHANGES_PIC_SMALL;
            auto on_pic_small_loaded = [mem](const std::string &filename) {
                mem->pic_small_cached = filename;
                g_app_events.notify(AppEvents::MemberChanged{.mem=mem, .changes=MEMBER_CHANGES_PIC_SMALL});
            };
            g_file_cache_downloader.download_url(mem->pic_small, on_pic_small_loaded);
        }
    }

    if (is_new_member && cache_member) {
        g_app_events.notify(AppEvents::MemberLoaded{.mem=mem});
    } else if (do_send_update_event) {
        if (changes & MEMBER_CHANGES_PROFILE && chats_list_ordering == CHATS_LIST_ORDERING_MEMBER_NAME) {
            chats_list_needs_reorder = true;
        }
        g_app_events.notify(AppEvents::MemberChanged{.mem=mem, .changes=changes});
    }

    return mem;
}

ChatDataPtr AppDataModel::update_or_create_chat_data(const cJSON* json, bool only_update_existing, bool do_send_update_event, bool add_to_chatlist) {
    string id = MemberData::get_string_value(json, "id");
    if (id.empty()) {
        Logger::debug("AppDataModel::update_chat id is empty");
        return nullptr;
    }
    bool is_new_chat = false;
    unsigned int changes = 0;
    ChatDataPtr chat = get_chat(id);
    if (chat) {
        changes = chat->update_from_json(json);
    } else {
        if (only_update_existing) {
            Logger::info("update_or_create_chat_data update but chat not found: %s", id.c_str());
            return nullptr;
        }
        is_new_chat = true;
        chat = make_shared<ChatData>(json);
        if (add_to_chatlist) {
            _chats_map[id] = chat;
            chats_list_needs_reorder = true;
        }
    }
//    debug_print_json(json);
    if (JsonData::has_object_value(json, "member")) {
        chat->member = update_or_create_member_data(JsonData::get_json_object(json, "member"), false, true);
        chat->member->chat = chat;
    }

    if (JsonData::has_object_value(json, "last_msg")) {
        // update_or_create_message_data will append message to chat messages list and it become the last_message
        chat->last_message = make_shared<MessageData>(JsonData::get_json_object(json, "last_msg"), chat);
        set_or_download_message_thumbnail(chat, chat->last_message);
        changes |= CHAT_CHANGES_LAST_MSG;
    }

    if (!chat->pic_small.empty()) {
        chat->pic_small_cached = g_file_cache_downloader.get_cached_file_for_url(chat->pic_small);
//        Logger::debug("chat %s pic_small_cached=%s", chat->title.c_str(), chat->pic_small_cached.c_str());
        if (chat->pic_small_cached.empty()) {
            auto on_success = [chat](const std::string& filename) {
                chat->pic_small_cached = filename;
                Logger::debug("chat %s downloaded pic_small_cached=%s", chat->title.c_str(), chat->pic_small_cached.c_str());
                g_app_events.notify(AppEvents::ChatChanged{.chat=chat, .ordering_changed=false, .changes=CHAT_CHANGES_PIC_SMALL});
            };
            g_file_cache_downloader.download_url(chat->pic_small, on_success);
        }
    }

    if (do_send_update_event && add_to_chatlist) {
        if (is_new_chat) {
            g_app_events.notify(AppEvents::ChatAdded{.chat=chat});
        } else {
            g_app_events.notify(AppEvents::ChatChanged{.chat=chat, .ordering_changed=false, .changes=changes});
        }
    }

    return chat;
}

void AppDataModel::update_chat_outbox_data(const cJSON* json) {
    string id = MemberData::get_string_value(json, "id");
    if (id.empty()) {
        Logger::error("AppDataModel::update_chat_outbox_data id is empty");
        return;
    }
    ChatDataPtr chat = get_chat(id);
    if (chat) {
        int64_t new_outgoing_seen_message_id = JsonData::get_int64_value(json, "outgoing_seen_message_id",0);
        if (new_outgoing_seen_message_id != 0) {
            int64_t old_outgoing_seen_message_id = chat->outgoing_seen_message_id;
            chat->outgoing_seen_message_id = new_outgoing_seen_message_id;
            if (currently_opened_chat == chat) {
                for(auto &msg : chat->messages) {
                    if (msg->id <= new_outgoing_seen_message_id && msg->id > old_outgoing_seen_message_id) {
                        g_app_events.notify(AppEvents::MessageChanged {.chat=chat, .msg=msg});
                    }
                }
            }
        }
    }
}

void AppDataModel::delete_chat_data(const cJSON* json, bool do_send_update_event) {
    string id = MemberData::get_string_value(json, "chat_id");
    if (id.empty()) {
        Logger::debug("AppDataModel::delete_chat_data id is empty");
        return;
    }
    ChatDataPtr chat = get_chat(id);
    if (!chat) {
        return;
    }
    _chats_map.erase(id);
    chats_list_needs_reorder = true;
    if (currently_opened_chat == chat) {
        currently_opened_chat = nullptr;
    }
    if (do_send_update_event) {
        g_app_events.notify(AppEvents::ChatDeleted{.chat=chat});
    }
}


void AppDataModel::set_or_download_message_thumbnail(const ChatDataPtr chat, const MessageDataPtr msg) {
    if (msg->att_image != nullptr && !msg->att_image->thumb_url.empty()) {
        msg->att_image->thumb_url_cached = g_file_cache_downloader.get_cached_file_for_url(msg->att_image->thumb_url);
        if (msg->att_image->thumb_url_cached.empty()) {
            auto on_image_loaded = [chat, msg](const std::string& filename) {
                msg->att_image->thumb_url_cached = filename;
                g_app_events.notify(AppEvents::MessageChanged {.chat=chat, .msg=msg});
            };
            g_file_cache_downloader.download_url(msg->att_image->thumb_url, on_image_loaded);
        }
    }
    if (msg->att_file != nullptr && !msg->att_file->thumb_url.empty()) {
        msg->att_file->thumb_url_cached = g_file_cache_downloader.get_cached_file_for_url(msg->att_file->thumb_url);
        if (msg->att_file->thumb_url_cached.empty()) {
            auto on_image_loaded = [chat, msg](const std::string& filename) {
                msg->att_file->thumb_url_cached = filename;
                g_app_events.notify(AppEvents::MessageChanged {.chat=chat, .msg=msg});
            };
            g_file_cache_downloader.download_url(msg->att_file->thumb_url, on_image_loaded);
        }
    }
}

static bool msg_same_as_instant_and_sent_by_me(MessageData &msg, MessageData &instant_msg,  MyMemberDataPtr& me) {
    Logger::debug("t:%d-%d, a:%p-%p, fl:%d-%d t:[%s-%s]", msg.type, instant_msg.type, msg.author.get(), instant_msg.author.get(), msg.flags, instant_msg.flags, msg.text.c_str(), instant_msg.text.c_str());
    if (msg.att_image && instant_msg.att_image) {
        Logger::debug("Att size%d-%d", msg.att_image->size, instant_msg.att_image->size);
    }
    if (instant_msg.author.get() == me.get() && instant_msg.is_outgoing() && msg.is_outgoing() && msg.type == instant_msg.type) {
        std::string instant_author_id;
        char messenger = msg.get_chat()->messenger();
        if (messenger == MESSENGER_CHATCUBE) {
            instant_author_id = me->id;
            if (instant_author_id != msg.author->id) {
                return false;
            }
            if (msg.type == MESSAGE_TYPE_STICKER) {
                return (msg.att_image->url.find(instant_msg.att_image->url) != std::string::npos);
            } else {
                return !((msg.att_file == nullptr && msg.att_image == nullptr && msg.text != instant_msg.text)
                         || (msg.att_file != nullptr && (msg.att_file->size != instant_msg.att_file->size || msg.att_file->file_type != instant_msg.att_file->file_type))
                         || (msg.att_image != nullptr && msg.att_image->size != instant_msg.att_image->size));
            }
        } else {
            instant_author_id = me->telegram_account.user_id;
            return (instant_author_id == msg.author->id && (
                     msg.att_image != nullptr
                     || msg.att_file != nullptr
                     || msg.text == instant_msg.text));
        }

    } else {
        return false;
    }
}

// do_send_message_event=false - when message updated as 'last_message' of chat
MessageDataPtr AppDataModel::update_or_create_message_data(const cJSON* json, ChatDataPtr chat, bool only_update_existing, bool do_send_update_event, bool coming_from_event) {
    int64_t id = MessageData::get_int64_value(json, "id", 0);
    if (!id) {
        Logger::debug("AppDataModel::update_message id is empty");
        return nullptr;
    }
    MessageDataPtr msg;
    if (chat == nullptr) {
        std::string chat_id = MessageData::get_string_value(json, "chat_id");
        chat = get_chat(chat_id);
        if (chat == nullptr) {
            // telegram may send lot of new messages in advance for chats which I not subscribed
            Logger::debug("update_or_create_message_data message but chat not found, skipped. chat_id=%s msgid=%lld", chat_id.c_str(), id);
            return nullptr;
        }
    }
    msg = chat->get_message(id);

    if (only_update_existing && msg == nullptr) {
        Logger::debug("update_or_create_message_data UPDATE message but message not found in chat, skipped. chat_id=%s msgid=%lld", chat->id.c_str(), id);
        return nullptr;
    }

    bool is_new_message = false;
    if (msg == nullptr) {
        msg = std::make_shared<MessageData>(json, chat);
        is_new_message = true;
    } else {
        msg->update_from_json(json);
    }

    if (msg->author == nullptr && !msg->author_id.empty()) {
        MemberDataPtr author = get_member(msg->author_id);
        if (author == nullptr) {
            request_missing_author(msg->author_id, msg);
        } else {
            msg->author = author;
        }
    }

    if (msg->reply_info != nullptr && msg->reply_info->author == nullptr && !msg->reply_info->author_id.empty()) {
        MemberDataPtr author = get_member(msg->reply_info->author_id);
        if (author == nullptr) {
            request_missing_author(msg->reply_info->author_id, msg);
        } else {
            msg->reply_info->author = author;
        }
    }

    if (is_new_message) {
        unsigned int chat_changes = 0;
        if (!chat->has_message(msg->id)) {
            // append message with possible replacing instant outgoing message
            if (msg->is_outgoing()) {
                if (!chat->messages_pending_outgoing.empty()) {
                    // search and replace pending outgoing message by real message got from server
                    int found_idx = -1;
                    for(int i = 0; i < chat->messages_pending_outgoing.size(); i++) {
                        if (msg_same_as_instant_and_sent_by_me(*msg, *chat->messages_pending_outgoing[i], me)) {
                            found_idx = i;
                            break;
                        }
                    }
//                    Logger::debug("found_idx=%d", found_idx);
                    if (found_idx != -1) {
                        MessageDataPtr pending_msg = chat->messages_pending_outgoing[found_idx];
                        chat->messages_pending_outgoing.erase(chat->messages_pending_outgoing.begin() + found_idx);
//                        Logger::debug("Erased pending=%d", found_idx);
                        for (int i = chat->messages.size() - 1; i >= 0; --i) {
//                            Logger::debug("Msg %p - %p", chat->messages[i].get(), pending_msg.get());
                            if (chat->messages[i] == pending_msg) {
//                                Logger::debug("found msg=%s", chat->messages[i]->text.c_str());
                                chat->messages[i]->update_from_message(msg);
                                msg = chat->messages[i];
                                set_or_download_message_thumbnail(chat, msg);

                                chat->last_message = msg;
                                if (chats_list_ordering == CHATS_LIST_ORDERING_LAST_MESSAGE) {
                                    chats_list_needs_reorder = true;
                                }
                                g_app_events.notify(AppEvents::ChatChanged {.chat=chat, .ordering_changed=chats_list_needs_reorder, .changes=CHAT_CHANGES_LAST_MSG});
                                g_app_events.notify(AppEvents::MessageChanged {.chat=chat, .msg=msg});
                                return msg;
                            }
                        }
                    }
                }
                if (!coming_from_event || !chat->has_newer_messages()) {
                    chat->messages.push_back(msg);
                }
            } else {
                if (chat->messenger() == MESSENGER_CHATCUBE && msg->id > chat->incoming_seen_message_id) {
                    chat->unread_count++;
                    chat_changes |= CHAT_CHANGES_UNREAD_COUNT;
                }
                if (!coming_from_event || !chat->has_newer_messages()) {
                    chat->messages.push_back(msg);
                }
            }
            if (chat->last_message == nullptr || chat->last_message->id < msg->id) {
                Logger::debug("chat->last_message->id < msg->id %lld < %lld", chat->last_message->id, msg->id);
                chat->last_message = msg;
                chat_changes |= CHAT_CHANGES_LAST_MSG;
            }
            set_or_download_message_thumbnail(chat, msg);
            if (do_send_update_event) {
                if (chats_list_ordering == CHATS_LIST_ORDERING_LAST_MESSAGE) {
                    chats_list_needs_reorder = true;
                }
                g_app_events.notify(AppEvents::ChatChanged {.chat=chat, .ordering_changed=chats_list_needs_reorder, .changes=chat_changes});
                g_app_events.notify(AppEvents::MessageAdded {.chat=chat, .msg=msg});
            }
        }
    } else {
        set_or_download_message_thumbnail(chat, msg);
        if (do_send_update_event) {
            g_app_events.notify(AppEvents::MessageChanged {.chat=chat, .msg=msg});
        }
    }
    return msg;
}

void AppDataModel::delete_messages_data(const cJSON* json_data) {
    std::string chat_id = JsonData::get_string_value(json_data, "chat_id", "");
    ChatDataPtr chat = get_chat(chat_id);
    if (chat) {
        bool last_message_deleted = false;
        cJSON* json_item;
        const cJSON* msgids_json = JsonData::get_json_array(json_data, "message_ids");
        cJSON_ArrayForEach(json_item, msgids_json)
        {
            for(int i = 0; i < chat->messages.size(); i++) {
                if (json_item->valueint64 == chat->messages[i]->id) {
                    if (chat == currently_opened_chat) {
                        g_app_events.notify<AppEvents::MessageDeleted>({.chat = chat, .msg = chat->messages[i]});
                    }
                    if (chat->last_message->id == chat->messages[i]->id) {
                        last_message_deleted = true;
                    }
                    chat->messages.erase(chat->messages.begin() + i);
                    break;
                }
            }
        }
        if (last_message_deleted) {
            if (chat->messages.empty()) {
                chat->last_message = nullptr;
            } else {
                chat->last_message = chat->messages.back();
            }
            g_app_events.notify(AppEvents::ChatChanged {.chat=chat, .ordering_changed=true, .changes=CHAT_CHANGES_LAST_MSG});
        }
    }
}

void AppDataModel::process_pending_updates() {
    for(auto &upd_str : _pending_updates) {
//        Logger::debug("pending update [%s]", upd_str.c_str());
        cJSON* json = cJSON_Parse(upd_str.c_str());

        std::string evtype = JsonData::get_string_value(json, "type");
        cJSON *json_data = cJSON_GetObjectItemCaseSensitive(json, "data");

        if (evtype == "MESSAGE_CREATED") {
            update_or_create_message_data(json_data, nullptr, false, false, true);
        } else if (evtype == "MESSAGE_UPDATED") {
            update_or_create_message_data(json_data, nullptr, true, false, true);
        } else if (evtype == "MEMBER_UPDATED") {
            update_or_create_member_data(json_data, false, true);
        } else if (evtype == "CHAT_CREATED") {
            update_or_create_chat_data(json_data, false, false, true);
        } else if (evtype == "CHAT_UPDATED") {
            update_or_create_chat_data(json_data, true, false, true);
        } else if (evtype == "CHAT_UPDATED_OUTBOX") {
            update_chat_outbox_data(json_data);
        } else if (evtype == "CHAT_DELETED") {
            delete_chat_data(json_data, false);
        }
    }
    _pending_updates.clear();
}


void AppDataModel::append_to_pending_updates(const cJSON* json) {
    char *str = cJSON_PrintUnformatted(json);
    _pending_updates.push_back(std::string(str));
    free(str);
}

void AppDataModel::on_pushstream_message(const cJSON* json) {
    std::string evtype = JsonData::get_string_value(json, "type");
    cJSON *json_data = cJSON_GetObjectItemCaseSensitive(json, "data");

    if (evtype == "MESSAGE_CREATED") {
        if (is_chat_list_loaded) {
            update_or_create_message_data(json_data, nullptr, false, true, true);
        } else {
//            Logger::debug("Chatlist not loaded. Put message to pending_updates");
            append_to_pending_updates(json);
        }
    } else if (evtype == "MESSAGE_UPDATED") {
        if (is_chat_list_loaded) {
            update_or_create_message_data(json_data, nullptr, true, true, true);
        } else {
//            Logger::debug("Chatlist not loaded. Put message to pending_updates");
            append_to_pending_updates(json);
        }
    } else if (evtype == "MESSAGES_DELETED") {
        delete_messages_data(json_data);
    } else if (evtype == "CHAT_CREATED") {
        if (is_chat_list_loaded) {
            update_or_create_chat_data(json_data, false, true, true);
        } else {
//            Logger::debug("Chatlist not loaded. Put chat to pending_updates");
            append_to_pending_updates(json);
        }
    } else if (evtype == "CHAT_UPDATED") {
        if (is_chat_list_loaded) {
            update_or_create_chat_data(json_data, true, true, true);
        } else {
//            Logger::debug("Chatlist not loaded. Put chat to pending_updates");
            append_to_pending_updates(json);
        }
    } else if (evtype == "CHAT_UPDATED_OUTBOX") {
        if (is_chat_list_loaded) {
            update_chat_outbox_data(json_data);
        } else {
            append_to_pending_updates(json);
        }
    } else if (evtype == "CHAT_DELETED") {
        if (is_chat_list_loaded) {
            delete_chat_data(json_data);
        } else {
//            Logger::debug("Chatlist not loaded. Put delete_chat to pending_updates");
            append_to_pending_updates(json);
        }
    } else if (evtype == "MEMBER_UPDATED") {
        if (is_chat_list_loaded) {
            update_or_create_member_data(json_data, true, true);
        } else {
//            Logger::debug("Chatlist not loaded. Put nember to pending_updates");
            append_to_pending_updates(json);
        }
    //} else if (evtype == "MESSAGE_DELETED") {
    } else if (evtype == "SHOW_ALERT") {
        std::string message = JsonData::get_string_value(json_data, "message", "");
        if (!message.empty()) {
            g_app_events.notify(AppEvents::ShowAlert {.message = message });
        }

    } else if (evtype == "CHAT_ACTION") {
        int action = JsonData::get_int_value(json_data, "action", 0);
        std::string member_id = JsonData::get_string_value(json_data, "member_id", "");
        std::string chat_id = JsonData::get_string_value(json_data, "chat_id", "");
        if (!member_id.empty() && !chat_id.empty()) {
            MemberDataPtr member = get_member(member_id);
            ChatDataPtr chat = get_chat(chat_id);
            if (member && chat) {
                if (action == CHAT_ACTION_TYPING) {
                    if (chat->type == CHAT_TYPE_PRIVATE || chat->type == CHAT_TYPE_SECRET) {
                        chat->action_line = "typing...";
                    } else {
                        chat->action_line = member->first_name + " typing...";
                    }
                } else if (action == CHAT_ACTION_CANCEL) {
                    chat->action_line.clear();
                }
                AppEvents::ChatChanged ev;
                ev.chat = chat;
                ev.ordering_changed = false;
                ev.changes = CHAT_CHANGES_ACTION;
                g_app_events.notify(ev);
            }
        }
    } else if (evtype == "CHAT_CLEARED") {
        std::string chat_id = JsonData::get_string_value(json_data, "chat_id", "");
        ChatDataPtr chat = get_chat(chat_id);
        if (chat) {
            chat->messages_was_loaded = false;
            chat->messages.clear();
            chat->unread_count = 0;
            chat->last_message = nullptr;
            g_app_events.notify(AppEvents::ChatCleared {.chat=chat});
        }
    } else if (evtype == "TELEGRAM_AUTH_CODE") {
        g_app_events.notify(AppEvents::TelegramAuthCode {.data = TelegramAuthCodeData(json_data)});
    } else if (evtype == "TELEGRAM_AUTH_PASSWORD") {
        g_app_events.notify(AppEvents::TelegramAuthPassword {.data = TelegramAuthPasswordData(json_data)});
    } else if (evtype == "TELEGRAM_AUTH_REGISTRATION") {
        g_app_events.notify(AppEvents::TelegramAuthRegistration {.data = TelegramAuthRegistrationData(json_data)});
    } else if (evtype == "TELEGRAM_TERMS") {
        g_app_events.notify(AppEvents::TelegramTermsOfService {.data = TelegramTermsOfServiceData(json_data)});
    } else if (evtype == "TELEGRAM_READY") {
        g_app_events.notify(AppEvents::TelegramReady {});
    }
}