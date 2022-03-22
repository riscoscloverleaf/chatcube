//
// Created by lenz on 2/9/20.
//
#include <set>
#include "../global.h"
#include "AppDataModel.h"
#include "NetworkRequests.h"

void AppDataModel::load_me(MyMemberCallbackType callback) {
    auto app_callback = [this, callback](CLHTTPRequest* req) {
        callback(this->update_my_member_data(req->response_json));
    };
    auto req = new CLChatApiRequest("GET", "/profile/my/", app_callback);
    req->needs_progress = true;

    AppEvents::ProgressBarControl pbreq;
    pbreq.label = "Loading my profile";
    pbreq.percent_done = 30;
    pbreq.estimated_progress = 20;
    pbreq.estimated_time = 8;
    pbreq.req = req;
    g_app_events.notify(pbreq);

    g_http_service.submit(req);
}

void AppDataModel::load_chat_list(bool needs_progress) {
    auto on_chatlist_loaded = [this](CLHTTPRequest* req) {
        cJSON *response_json = req->response_json;
        if (response_json && cJSON_IsArray(response_json)) {
            cJSON *json_item;
            ChatDataPtr chat;
            this->_chats_map.clear();
            for(json_item = response_json->child; json_item != nullptr; json_item = json_item->next) {
                if (json_item && cJSON_IsObject(json_item))
                {
                    update_or_create_chat_data(json_item, false, false, true);
//                    chat = std::make_shared<ChatData>(json_item);
//                    this->_chats_map[chat->id] = chat;
                } else {
                    Logger::warn("parseChatList: array but not an objects");
                }
            }
            if (!_pending_updates.empty()) {
                process_pending_updates();
            }
            is_chat_list_loaded = true;
        } else {
            Logger::warn("parseChatList: not an array. resp=%s", req->response_text.c_str());
        }
        g_app_events.notify(AppEvents::ChatListLoaded {});
    };

    auto req = new CLChatApiRequest("GET", "/chat/", on_chatlist_loaded);
    if (needs_progress) {
        req->needs_progress = true;

        AppEvents::ProgressBarControl pbreq;
        pbreq.label = "Loading chats list";
        pbreq.percent_done = 70;
        pbreq.req = req;
        pbreq.estimated_progress = 20;
        pbreq.estimated_time = 20;
        g_app_events.notify(pbreq);
    }

    g_http_service.submit(req);
}

void AppDataModel::load_member(const string& member_id) {
    auto suceess_callback = [this, member_id](CLHTTPRequest* req) {
        _members_currently_loading.erase(member_id);
        update_or_create_member_data(req->response_json, true, true);
    };
    auto fail_callback  = [this, member_id](const HttpRequestError& err) {
        _members_currently_loading.erase(member_id);
        return false;
    };
    auto found = _members_currently_loading.find(member_id);
    if (found != _members_currently_loading.end()) {
        g_http_service.submit(new CLChatApiRequest("GET", "/profile/" + member_id + "/", suceess_callback, fail_callback));
    } else {
        Logger::debug("Member id %s already loading (load skipped).", member_id.c_str());
    }
}

void AppDataModel::load_members(const vector<std::string>& member_ids) {
    if (member_ids.empty()) {
        return;
    }
    vector<std::string> to_be_loaded_member_ids;
    for(const auto &memid : member_ids) {
        auto found = _members_currently_loading.find(memid);
        if (found != _members_currently_loading.end()) {
            Logger::debug("Member id %s already loading (skipped).", memid.c_str());
            continue;
        }
        to_be_loaded_member_ids.push_back(memid);
    }

    auto success_callback = [this, to_be_loaded_member_ids](CLHTTPRequest* req) {
        cJSON *json_item;
        for(const auto &memid : to_be_loaded_member_ids) {
            _members_currently_loading.erase(memid);
        }
        cJSON_ArrayForEach(json_item, req->response_json) {
            update_or_create_member_data(json_item, true, true);
        }
    };
    auto fail_callback  = [this, to_be_loaded_member_ids](const HttpRequestError& err) {
        for(const auto &memid : to_be_loaded_member_ids) {
            _members_currently_loading.erase(memid);
        }
        return false;
    };

    if (!to_be_loaded_member_ids.empty()) {
        g_http_service.submit(new CLChatApiRequest("GET", "/profile/?ids=" + str_join(to_be_loaded_member_ids, ","), success_callback, fail_callback));
    } else {
        Logger::debug("All members is already loading (load skipped).");
    }
}

// append loaded messages.
void AppDataModel::append_loaded_messages(const ChatDataPtr chat, const cJSON* json) {
    MessagesVector messages;
    MemberDataPtr author;
    MessageDataPtr msg;
    std::set<std::string> authors_needs_be_loaded;
    const cJSON *json_items = JsonData::get_json_array(json, "items");

    if (!json_items) {
        Logger::debug("AppDataModel::append_loaded_messages. Chat %s have no items", chat->title.c_str());
        return;
    }

    cJSON *json_item;
    cJSON_ArrayForEach(json_item, json_items) {
        update_or_create_message_data(json_item, chat, false, false, false);
    }
    int is_first_load = JsonData::get_int_value(json, "first", 0);
    std::string next_url = JsonData::get_string_value(json, "next", "");
    std::string prev_url = JsonData::get_string_value(json, "prev", "");
    Logger::debug("append_loaded_messages reordering");
    chat->sort_messages();
    if (is_first_load) {
        chat->messages_load_older_url = next_url;
        chat->messages_load_newer_url = prev_url;
    } else {
        std::string dir = JsonData::get_string_value(json, "dir", "o");
        if (dir[0] == 'o') {
            chat->messages_load_older_url = next_url;
        } else {
            chat->messages_load_newer_url = prev_url;
        }
    }

    chat->messages_was_loaded = true;
    g_app_events.notify(AppEvents::MessagesLoaded {.chat=chat, .is_first_load=(is_first_load != 0) });

    Logger::debug("append_loaded_messages AppEvents::MessagesLoaded sent");

    MessageDataPtr prev_last_msg = chat->last_message;
    if (prev_last_msg == nullptr && !chat->messages.empty() && chat->messages_load_newer_url.empty()) {
        chat->last_message = chat->messages.back();
        g_app_events.notify(AppEvents::ChatChanged {.chat=chat, .ordering_changed=true, .changes=CHAT_CHANGES_LAST_MSG});
        Logger::debug("append_loaded_messages AppEvents::ChatChanged sent");
    }
}

void AppDataModel::open_chat(const ChatDataPtr chat, bool force_reopen) {
    if (currently_opened_chat != chat || force_reopen) {
        currently_opened_chat = chat;
        g_app_events.notify(AppEvents::OpenedChatChanged {.chat=chat });
        if (chat->messages_was_loaded && !chat->messages_filter) {
            g_http_service.submit(new CLChatApiRequest("GET", "/chat/" + chat->id + "/open/"));
            g_app_events.notify(AppEvents::MessagesLoaded {.chat=chat, .is_first_load=true });
        } else {
            load_messages_in_chat(chat, true,
                    (chat->unread_count == 0 ? 0 : chat->incoming_seen_message_id)
                    );
        }
    }
}

void AppDataModel::load_messages_in_chat(const ChatDataPtr chat, bool open_chat, int64_t starting_from_id, const std::function<void()> &on_success_callback, const std::function<void()> &on_fail_callback) {
    Logger::debug("AppDataModel::load_messages_in_chat pending:%d chat:%d", loading_messages_pending, chat != nullptr);
    if (!loading_messages_pending && chat != nullptr) {
        g_hourglass_on();
        auto success_callback = [this, chat, on_success_callback](CLHTTPRequest* req) {
            Logger::debug("loaded messages in chat=%s", chat->title.c_str());
            loading_messages_pending = false;
            chat->messages.clear();
            append_loaded_messages(chat, req->response_json);
            on_success_callback();
            g_hourglass_off();
        };
        auto fail_callback = [this, on_fail_callback](const HttpRequestError& err) {
            loading_messages_pending = false;
            on_fail_callback();
            g_hourglass_off();
            return false;
        };
        char url[1024];
        chat->messages_filter = 0;
        if (open_chat) {
            if (starting_from_id) {
                snprintf(url, sizeof(url), "/chat/%s/messages/?first_load=1&open=1&from_message_id=%lld", chat->id.c_str(), starting_from_id);
            } else {
                snprintf(url, sizeof(url), "/chat/%s/messages/?first_load=1&open=1", chat->id.c_str());
            }
        } else {
            if (starting_from_id) {
                snprintf(url, sizeof(url), "/chat/%s/messages/?first_load=1&from_message_id=%lld", chat->id.c_str(), starting_from_id);
            } else {
                snprintf(url, sizeof(url), "/chat/%s/messages/?first_load=1", chat->id.c_str());
            }
        }
        Logger::debug("loading messages in chat=%s", chat->title.c_str());
        loading_messages_pending = true;
        g_http_service.submit(new CLChatApiRequest("GET", url, success_callback, fail_callback));
    }
}

void AppDataModel::load_messages_in_chat(const ChatDataPtr chat, bool open_chat, int64_t starting_from_id) {
    auto empty_callback = []() {};
    load_messages_in_chat(chat, open_chat, starting_from_id, empty_callback, empty_callback);
}

void AppDataModel::load_more_messages_in_chat(const ChatDataPtr chat, bool load_older) {
    if (chat != nullptr && (load_older ? chat->has_older_messages() : chat->has_newer_messages())) {
        if (loading_messages_pending) {
            Logger::debug("AppDataModel::load_more_messages_in_chat chat messages already loading...");
            return;
        }
        g_hourglass_on();
        loading_messages_pending = true;
        auto success_callback = [this, chat](CLHTTPRequest* req) {
            append_loaded_messages(chat, req->response_json);
            loading_messages_pending = false;
            g_hourglass_off();
        };
        auto fail_callback = [this](const HttpRequestError& err) {
            loading_messages_pending = false;
            g_hourglass_off();
            return false;
        };
        g_http_service.submit(new CLChatApiRequest("GET",
                                                   (load_older ? chat->messages_load_older_url : chat->messages_load_newer_url),
                                                   success_callback, fail_callback));
    } else {
        if (chat != nullptr) {
            Logger::debug("AppDataModel::load_more_messages_in_chat chat [%s] have no more messages to load", chat->id.c_str());
        } else {
            Logger::debug("AppDataModel::load_more_messages_in_chat have no more messages to load (chat == null)");
        }
    }
}
