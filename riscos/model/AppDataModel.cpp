//
// Created by lenz on 1/31/20.
//

#include <strstream>
#include <algorithm>
#include <unistd.h>
#include <cloverleaf/utf8.h>
#include <tbx/path.h>
#include <tbx/fileraction.h>
#include <tbx/application.h>
#include <tbx/modeinfo.h>
#include <tbx/window.h>
#include "AppDataModel.h"
#include "AppEvents.h"
#include "NetworkRequests.h"
#include "ChatData.h"
#include "ChatMemberData.h"
#include "MessageData.h"
#include "cloverleaf/IdleTask.h"
#include "cloverleaf/Logger.h"
#include "cloverleaf/CLUtf8.h"
#include "cloverleaf/utf8.h"
#include "cloverleaf/rosprite.h"
#include "cloverleaf/CLImageJPGLoader.h"
#include "ChoicesModel.h"
#include "../utils.h"
#include "../global.h"

void AppDataModel::init(const std::string& baseUrl, const std::string &lang, const std::string& app_version) {
    g_http_service.init(baseUrl, lang, "ChatCube/"+app_version);
    using namespace std::placeholders;
    _app_version = app_version;
    if (!initialized) {
        g_http_service.set_events_handler(std::bind(&AppDataModel::on_pushstream_message, this, _1));

        g_app_events.listen<AppEvents::MemberChanged>(std::bind(&AppDataModel::on_member_changed, this, _1));
        g_app_events.listen<AppEvents::MemberLoaded>(std::bind(&AppDataModel::on_member_loaded, this, _1));
        g_app_events.listen<AppEvents::TelegramReady>(std::bind(&AppDataModel::on_telegram_ready, this, _1));
        initialized = true;
    }
}

void AppDataModel::start() {
    auto on_initial_data_load = [this](CLHTTPRequest* request) {
        cJSON *response_json = request->response_json;
        auto on_my_profile_loaded = [this](CLHTTPRequest* request) {
            this->update_my_member_data(request->response_json);
            assert(!me->push_channel.empty());

            g_http_service.start_event_stream(me->push_channel, JsonData::get_string_value(request->response_json, "push_channel_start_date"));
            g_app_events.notify(AppEvents::LoggedIn {});
            //load_chat_list();
        };

        auto on_initial_loaded = [this, on_my_profile_loaded](){
            for(auto &av : _avatars) {
                av.pic_small = g_file_cache_downloader.get_cached_file_for_url(av.pic_small);
            }
            for(auto &sgroup : _stickers) {
                sgroup.pic_small = g_file_cache_downloader.get_cached_file_for_url(sgroup.pic_small);
                for(auto &st : sgroup.items) {
                    st.pic_small = g_file_cache_downloader.get_cached_file_for_url(st.pic_small);
                    st.pic = g_file_cache_downloader.get_cached_file_for_url(st.pic);
                }
            }
            g_app_events.notify(AppEvents::InitialDataLoaded {});

            AppEvents::ProgressBarControl pbreq;
            pbreq.label = "";
            pbreq.percent_done = 100;
            g_app_events.notify(pbreq);

            std::string token = load_auth_token();
            if (token.empty()) {
                g_app_events.notify(AppEvents::LoginRequired {});
            } else {
                g_http_service.set_auth_token(token);
                CLStringsMap getData = {
                        {"initial", "1"},
                        {"app_version", _app_version}
                };
                auto *req = new CLChatApiRequest("GET", "/profile/my/", on_my_profile_loaded);
                req->set_url_parameters(getData);
                req->needs_progress = true;

                AppEvents::ProgressBarControl pbreq;
                pbreq.label = "Loading my profile";
                pbreq.percent_done = 30;
                pbreq.req = req;
                pbreq.estimated_progress = 20;
                pbreq.estimated_time = 8;

                g_app_events.notify(pbreq);

                g_http_service.submit(req);
            }
        };


        cJSON* json_item;
        _latest_app_version = JsonData::get_string_value(response_json, "app_version","1.00");
        g_choices.load(JsonData::get_json_object(response_json, "choices"));

        AppEvents::ProgressBarControl pbreq;
        pbreq.label = "Downloading avatars and stickers";
        g_app_events.notify(pbreq);

        const cJSON* json_avatars = JsonData::get_json_array(response_json, "avatars");
        _avatars.clear();
        _avatars.reserve(cJSON_GetArraySize(json_avatars));
        cJSON_ArrayForEach(json_item, json_avatars)
        {
            AvatarData avatar = AvatarData(json_item);
            if (! g_file_cache_downloader.is_url_cached(avatar.pic_small)) {
                auto on_avatar_downloaded = [this, avatar, on_initial_loaded](const std::string& saved_to) {
                    this->_avatars.push_back(avatar);
//                    Logger::debug("on_avatar_downloaded is_downloading() = %d", g_file_cache_downloader.is_downloading());
                    if (!g_file_cache_downloader.is_downloading()) {
                        on_initial_loaded();
                    }
                };
                g_file_cache_downloader.download_url(avatar.pic_small, on_avatar_downloaded);
            } else {
                _avatars.push_back(avatar);
            }
        }

        _stickers.clear();
        const cJSON* json_stickers = JsonData::get_json_array(response_json, "stickers");
        cJSON_ArrayForEach(json_item, json_stickers)
        {
            StickerGroupData sg = StickerGroupData(json_item);
            _stickers.push_back(sg);
        }

        auto on_sticker_downloaded = [on_initial_loaded](const string& saved_to) {
//            Logger::debug("on_sticker_downloaded is_downloading() = %d", g_file_cache_downloader.is_downloading());
            if (!g_file_cache_downloader.is_downloading()) {
                on_initial_loaded();
            }
        };
        for(auto &sgroup : _stickers) {
            if (!g_file_cache_downloader.is_url_cached(sgroup.pic_small)) {
                g_file_cache_downloader.download_url(sgroup.pic_small, on_sticker_downloaded);
            }
            for(auto &st : sgroup.items) {
                if (!g_file_cache_downloader.is_url_cached(st.pic_small)) {
                    g_file_cache_downloader.download_url(st.pic_small, on_sticker_downloaded);
                }
                if (!g_file_cache_downloader.is_url_cached(st.pic)) {
                    g_file_cache_downloader.download_url(st.pic, on_sticker_downloaded);
                }
            }
        }

        if (!g_file_cache_downloader.is_downloading()) {
            on_initial_loaded();
        }
    };


    auto *req = new CLChatApiRequest("GET", "/initial/", on_initial_data_load);
    req->needs_progress = true;

    AppEvents::ProgressBarControl pbreq;
    pbreq.label = "Loading initial data";
    pbreq.percent_done = 10;
    pbreq.req = req;
    pbreq.estimated_progress = 20;
    pbreq.estimated_time = 5;
    g_app_events.notify(pbreq);

    g_http_service.submit(req);

}

void AppDataModel::change_my_profile(CLStringsMap &post_data, MyMemberCallbackType callback) {
    auto success_callback = [this, callback](CLHTTPRequest* req) {
        callback(this->update_my_member_data(req->response_json));
    };
    for(auto &item : post_data) {
        item.second = riscos_local_to_utf8(item.second);
    }
    auto *req = new CLChatApiRequest("POST", "/profile/my/", success_callback);
    req->post_data = post_data;
    g_http_service.submit(req);
}

void AppDataModel::change_my_profile(CLStringsMap &post_data) {
    auto fake_callback = [](const MyMemberDataPtr me) {};
    change_my_profile(post_data, fake_callback);
}

void AppDataModel::upload_profile_image(const std::string& file_path, const char* field_name, MyMemberCallbackType callback) {
    auto success_callback = [this, callback](CLHTTPRequest* req) {
        callback(this->update_my_member_data(req->response_json));
    };
    auto *req = new CLChatApiRequest("POST", "/profile/my/", success_callback);
    req->upload_files[field_name] = file_path;
    req->needs_progress = true;
    g_http_service.submit(req);
}

MemberDataPtr AppDataModel::get_member(const std::string& member_id) {
    auto found =  _members_map.find(member_id);
    if (found != _members_map.end()) {
        return found->second;
    }
    return nullptr;
}

ChatDataPtr AppDataModel::get_chat(const std::string& chat_id) {
    auto found =  _chats_map.find(chat_id);
    if (found != _chats_map.end()) {
        return found->second;
    }
    return nullptr;
}

std::vector<ChatDataPtr>& AppDataModel::get_chats_list() {
    if (_chats_list.empty()) {
        chats_list_needs_reorder = true;
    }
    if (chats_list_needs_reorder) {
        _chats_list.clear();
        _chats_list.reserve(_chats_map.size());
        int i = 0;
//        Logger::debug("size %d %d", _chats_map.size(), _chats_list.size());
        for(auto item: _chats_map) {
//            Logger::debug("_chats_list.push_back chat %p key=%s %d", item.second.get(), item.first.c_str(), i);
            if (_chats_filter_title.empty()
                || utf8casestr(item.second->title.c_str(), _chats_filter_title.c_str()) != NULL) {
                _chats_list.push_back(item.second);
            }
            i++;
        }
        if (chats_list_ordering == CHATS_LIST_ORDERING_LAST_MESSAGE) {
            std::sort(_chats_list.begin(), _chats_list.end(), [](const ChatDataPtr &a, const ChatDataPtr &b) -> bool {
//                if (a == b) {
//                    Logger::debug("SAME sort chat a=%p chat b=%p", a.get(), b.get());
//                }
                MessageDataPtr &a_msg = a->last_message;
                MessageDataPtr &b_msg = b->last_message;
                bool has_a = (a_msg != nullptr);
                bool has_b = (b_msg != nullptr);
                if (has_a && has_b) {
                    return (a_msg->sendtime > b_msg->sendtime);
                }
                if (has_a) {
                    return true;
                }
                if (has_b) {
                    return false;
                }
//                if (a->unread_count > 0 && b->unread_count == 0) {
//                    return true;
//                }
//                if (b->unread_count > 0 && a->unread_count == 0) {
//                    return false;
//                }
                return (utf8casecmp(a->title.c_str(), b->title.c_str()) < 0);
            });
        } else if (chats_list_ordering == CHATS_LIST_ORDERING_ONLINE) {
            std::sort(_chats_list.begin(), _chats_list.end(), [](const ChatDataPtr &a, const ChatDataPtr &b) -> bool {
                //Logger::debug("a onl %d b onl %d", a->is_online(), b->is_online());
                if (a == b) {
                    Logger::debug("SAME sort chat a=%p chat b=%p", a.get(), b.get());
                }
                MessageDataPtr &a_msg = a->last_message;
                MessageDataPtr &b_msg = b->last_message;
                bool a_online = a->is_online();
                bool b_online = b->is_online();
                bool a_active = a->is_member_active();
                bool b_active = b->is_member_active();
                //Logger::debug("a1 onl %d b onl %d", a->is_online(), b->is_online());
                if (a_online && !b_online) {
//                    Logger::debug("%s online %s not online", a->title.c_str(), b->title.c_str());
                    return true;
                }
                if (!a_online && b_online) {
//                    Logger::debug("%s online %s not online", a->title.c_str(), b->title.c_str());
                    return false;
                }
                if (a_active && !b_active) {
                    return true;
                }
                if (!a_active && b_active) {
//                    Logger::debug("%s online %s not online", a->title.c_str(), b->title.c_str());
                    return false;
                }
                bool has_a_msg = (a_msg != nullptr);
                bool has_b_msg = (b_msg != nullptr);
                if (has_a_msg && has_b_msg) {
                    return (a_msg->sendtime > b_msg->sendtime);
                }
                if (has_a_msg) {
                    return true;
                }
                if (has_b_msg) {
                    return false;
                }
                return (utf8casecmp(a->title.c_str(), b->title.c_str()) < 0);
            });
        } else if (chats_list_ordering == CHATS_LIST_ORDERING_MEMBER_NAME) {
            std::sort(_chats_list.begin(), _chats_list.end(), [](const ChatDataPtr &a, const ChatDataPtr &b) -> bool {
                if (a->unread_count > 0 && b->unread_count == 0) {
                    return true;
                }
                if (a->unread_count == 0 && b->unread_count > 0) {
                    return false;
                }
                return (utf8casecmp(a->title.c_str(), b->title.c_str()) < 0);
            });
        }

        chats_list_needs_reorder = false;
    }
    return _chats_list;
}

void AppDataModel::set_chats_list_ordering(int new_ordering) {
    chats_list_ordering = new_ordering;
    chats_list_needs_reorder = true;
    g_app_events.notify(AppEvents::ChatChanged {.chat=nullptr, .ordering_changed=true, .changes=0});
}

int AppDataModel::get_chats_list_ordering() {
    return chats_list_ordering;
}

void AppDataModel::filter_chats(const std::string& search_for) {
    if (search_for.empty()) {
        _chats_filter_title.clear();
    } else {
        _chats_filter_title = riscos_local_to_utf8(search_for);
    }
    chats_list_needs_reorder = true;
    g_app_events.notify(AppEvents::ChatChanged {.chat=nullptr, .ordering_changed=true, .changes=0});
}

MessageDataPtr AppDataModel::make_instant_outgoing_message(int type, const std::string& text, int reply_to_id) {
    MessageDataPtr msg = make_shared<MessageData>();
    msg->chat = currently_opened_chat;
    msg->flags = MESSAGE_FLAG_OUTGOING;
    msg->type = type;
    msg->text = text;
    msg->author = me;
    msg->sendtime = time(0);
    msg->sending_state = MESSAGE_SENDING_STATE_PENDING;
    return msg;
}

void AppDataModel::send_message(const CLStringsMap& post_data, MessageDataPtr instant_nessage) {
    auto on_send_callback = [this](CLHTTPRequest* req) {
//        this->update_or_create_message_data(response_json, false);
        if (currently_opened_chat->has_newer_messages()) { // if we in the muddle of history then load latest messages
            load_messages_in_chat(currently_opened_chat, false, 0);
        }
    };
    auto on_fail = [this, instant_nessage](const HttpRequestError& err) {
        remove_pending_outgoing_message(instant_nessage);
        return false;
    };

    auto *req = new CLChatApiRequest("POST", "/chat/send/", on_send_callback, on_fail);
    req->post_data = post_data;
    if (currently_opened_chat->has_newer_messages()) {
        auto send_after_success_load = [this, req, instant_nessage]() {
            g_http_service.submit(req);
            append_pending_outgoing_message(instant_nessage);
        };
        auto clean_after_fail_load = [req]() {
            delete req;
        };
        load_messages_in_chat(currently_opened_chat, 0, 0, send_after_success_load, clean_after_fail_load);
    } else {
        g_http_service.submit(req);
        append_pending_outgoing_message(instant_nessage);
    }
}

void AppDataModel::send_message_upload_file(const CLStringsMap& post_data, const char* field_name, const std::string & file_name, MessageDataPtr instant_nessage) {
    auto on_send_callback = [this](CLHTTPRequest* req) {
        this->update_or_create_message_data(req->response_json, nullptr, false, true, false);
    };
    auto on_fail = [this, instant_nessage](const HttpRequestError& err) {
        remove_pending_outgoing_message(instant_nessage);
        return false;
    };
    auto *req = new CLChatApiRequest("POST", "/chat/send/", on_send_callback, on_fail);
    req->post_data = post_data;
    req->upload_files[field_name] = file_name;
    req->set_timeout(0);
    req->set_lowspeed_limit(30,10);
    req->needs_progress = true;

    AppEvents::ProgressBarControl pbreq;
    pbreq.label = "Uploading file";
    pbreq.req = req;
    g_app_events.notify(pbreq);

    if (currently_opened_chat->has_newer_messages()) {
        auto send_after_success_load = [this, req, instant_nessage]() {
            g_http_service.submit(req);
            append_pending_outgoing_message(instant_nessage);
        };
        auto clean_after_fail_load = [req]() {
            // hide progress bar as label is empty
            AppEvents::ProgressBarControl pbreq;
            pbreq.req = req;
            g_app_events.notify(pbreq);
            delete req;
        };
        load_messages_in_chat(currently_opened_chat, 0, 0, send_after_success_load, clean_after_fail_load);
    } else {
        g_http_service.submit(req);
        append_pending_outgoing_message(instant_nessage);
    }
}

void AppDataModel::send_message_text(const string& text, int reply_id) {
    if (text.empty()) {
        return;
    }
    std::string txt = text;
    txt = riscos_local_to_utf8(rtrim(txt," \t\n"));
    if (txt.empty()) {
        return;
    }
    CLStringsMap postData = {
            {"reply_to_id", to_string(reply_id)},
            {"chat_id", currently_opened_chat->id},
            {"type", to_string(MESSAGE_TYPE_TEXT)},
            {"text", txt}
    };
    send_message(postData, make_instant_outgoing_message(MESSAGE_TYPE_TEXT, txt, reply_id));
}

void AppDataModel::send_message_sticker(const StickerData* sticker, int reply_to_id) {
    CLStringsMap postData = {
            {"reply_to_id", to_string(reply_to_id)},
            {"chat_id", currently_opened_chat->id},
            {"type", to_string(MESSAGE_TYPE_STICKER)},
            {"text", sticker->name}
    };
    MessageDataPtr msg = make_instant_outgoing_message(MESSAGE_TYPE_STICKER, "", reply_to_id);
    msg->att_image = new AttachmentImage();
    msg->att_image->url = sticker->name;
    msg->att_image->thumb_url_cached = sticker->pic;
    msg->att_image->thumb_height = msg_STICKER_THUMB_HEIGHT;
    msg->att_image->thumb_width = msg_STICKER_THUMB_WIDTH;

    send_message(postData, msg);
}

void AppDataModel::send_message_photo(const std::string& photo_file, const std::string& caption, int reply_to_id) {
    std::string cap = caption;
    cap = riscos_local_to_utf8(rtrim(cap," \t\n"));
    CLStringsMap postData = {
            {"reply_to_id", to_string(reply_to_id)},
            {"chat_id", currently_opened_chat->id},
            {"type", to_string(MESSAGE_TYPE_PHOTO)},
            {"text", cap}
    };
    MessageDataPtr msg = make_instant_outgoing_message(MESSAGE_TYPE_PHOTO, cap, reply_to_id);
    msg->att_image = new AttachmentImage();
    msg->att_image->size = get_filesize(photo_file.c_str());
    msg->text = "Photo";
    send_message_upload_file(postData, "file", photo_file, msg);
}

void AppDataModel::send_message_file(const std::string& file_path, const std::string& caption, const int file_type, int reply_to_id) {
    if (file_type ==  0xC85 || file_type == 0xB60) {
        send_message_photo(file_path, caption, reply_to_id);
        return;
    }
    std::string filename = riscos_local_to_utf8(file_basename(file_path));
    std::string cap = caption;
    cap = riscos_local_to_utf8(rtrim(cap," \t\n"));
    CLStringsMap postData = {
            {"reply_to_id", to_string(reply_to_id)},
            {"chat_id", currently_opened_chat->id},
            {"type", to_string(MESSAGE_TYPE_FILE)},
            {"file_type", to_string(file_type)},
            {"file_name", filename},
            {"text", cap}
    };
    MessageDataPtr msg = make_instant_outgoing_message(MESSAGE_TYPE_FILE, cap, reply_to_id);
    msg->att_file = new AttachmentFile();
    msg->att_file->name = filename;
    msg->att_file->file_type = file_type;
    msg->att_file->size = get_filesize(file_path.c_str());
    msg->text = filename;

    send_message_upload_file(postData, "file", file_path, msg);
}

void AppDataModel::send_message_text_to_member(const string &member_id, const string& text) {
// fixme! not implemented yet
//    CLStringsMap postData = {
//            {"reply_to_id", "0"},
//            {"recipient_id", member_id},
//            {"type", to_string(MESSAGE_TYPE_TEXT)},
//            {"text", riscos_local_to_utf8(text)}
//    };
//
//    send_message(postData);
}

//ChatDataPtr AppDataModel::get_private_chat_for_member(const MemberDataPtr mem) {
//    for(auto const &elem : _chats_map) {
//        if (elem.second->member == mem) {
//            return elem.second;
//        }
//    }
//    return nullptr;
//}

void AppDataModel::send_chat_action(int action) {
    if (currently_opened_chat) {
        auto *req = new CLChatApiRequest("POST", "/chat/" + currently_opened_chat->id + "/action/");
        req->post_data = {
                {"action", to_string(action)}
        };
        g_http_service.submit(req);
    }
}

void AppDataModel::request_missing_author(std::string &author_id, MessageDataPtr msg) {
    std::vector<MessageDataPtr> *messages;
    auto found = messages_waiting_for_author.find(author_id);
    if (found == messages_waiting_for_author.end()) {
        messages = new std::vector<MessageDataPtr>();
        messages_waiting_for_author[author_id] = messages;
    } else {
        messages = found->second;
    }
    messages->push_back(msg);
//    Logger::debug("AppDataModel::request_missing_author %s for msg: %lld chat:%s", author_id.c_str(), msg->id, msg->get_chat()->id.c_str());
    if (!loading_missing_authors) {
        loading_missing_authors = true;

        auto do_load_missing_authors = [this]() {
            if (!messages_waiting_for_author.empty()) {
                Logger::debug("AppDataModel::request_missing_author do_load_missing_authors");
                std::vector<std::string> author_ids;
                for(auto it : messages_waiting_for_author) {
                    author_ids.push_back(it.first);
                }
                load_members(author_ids);
            }
            loading_missing_authors = false;
        };
        g_idle_task.run_at_next_idle(do_load_missing_authors);
    }
}

void AppDataModel::mark_seen_last_message(const ChatDataPtr& chat) {
    if (chat == nullptr) {
        return;
    }

    // find latest incoming message and mark it as seen
    if (!chat->messages.empty()) {
        for(int i = chat->messages.size() - 1; i >= 0; --i) {
//            Logger::debug("chat->messages[%d].out = %d id=%lld", i, chat->messages[i]->is_outgoing(), chat->messages[i]->id);
            if (!chat->messages[i]->is_outgoing()) {
                g_http_service.submit(new CLChatApiRequest("GET", "/chat/" + chat->id + "/messages/" + to_string(chat->messages[i]->id) + "/seen/"));
                break;
            }
        }
    }
}

void AppDataModel::mark_seen_message(const MessageDataPtr& msg) {
    ChatDataPtr chat = msg->get_chat();
    // find latest incoming message and mark it as seen
    if (chat && !msg->is_outgoing() && chat->incoming_seen_message_id < msg->id) {
        if (!marking_seen_msg || marking_seen_msg->id < msg->id) {
            if (!marking_seen_msg) {
                auto do_mark_seen = [this]() {
                    if (this->marking_seen_msg) {
                        ChatDataPtr chat = marking_seen_msg->get_chat();
                        if (chat) {
                            chat->incoming_seen_message_id = marking_seen_msg->id;
                            g_http_service.submit(new CLChatApiRequest("GET", "/chat/" + chat->id + "/messages/" + to_string(marking_seen_msg->id) + "/seen/"));
                        }
                        marking_seen_msg = nullptr;
                    }
                };
                g_idle_task.run_at_next_idle(do_mark_seen);
            }
            marking_seen_msg = msg;
        }
    }
}

void AppDataModel::on_member_loaded(const AppEvents::MemberLoaded& ev) {
    if (!messages_waiting_for_author.empty()) {
        const std::string &member_id = ev.mem->id;
        auto found = messages_waiting_for_author.find(member_id);
        if (found != messages_waiting_for_author.end()) {
            std::vector<MessageDataPtr> *messages = found->second;
            for(MessageDataPtr msg : *messages) {
                bool message_changed = false;
                if (msg->author == nullptr && msg->author_id == member_id) {
                    msg->author = ev.mem;
                    message_changed = true;
                }
                if (msg->reply_info && msg->reply_info->author == nullptr && msg->reply_info->author_id == member_id) {
                    msg->reply_info->author = ev.mem;
                    message_changed = true;
                }
                if (message_changed) {
                    ChatDataPtr chat = msg->get_chat();
                    if (chat) {
                        Logger::debug("Sent author for message %lld in _chat %s", msg->id, msg->get_chat()->id.c_str());
                        AppEvents::MessageChanged ev;
                        ev.msg = msg;
                        ev.chat = chat;
                        g_app_events.notify(ev);
                    }
                }
            }
            Logger::debug("Deleting messages waited for author_id=%s", member_id.c_str());
            delete messages;
            messages_waiting_for_author.erase(member_id);
        }
    }
}

void AppDataModel::on_member_changed(const AppEvents::MemberChanged& ev) {
//    ChatDataPtr _chat = get_private_chat_for_member(ev.mem);
    ChatDataPtr chat = ev.mem->get_chat();
    if (chat) {
        unsigned int changes = 0;
        bool ordering_changed = false;
        if (ev.changes & MEMBER_CHANGES_PIC_SMALL) {
            chat->pic_small = ev.mem->pic_small;
            chat->pic_small_cached = ev.mem->pic_small_cached;
            changes |= CHAT_CHANGES_PIC_SMALL;
        }
        if (ev.changes & MEMBER_CHANGES_PROFILE && chat->title != ev.mem->displayname) {
            chat->title = ev.mem->displayname;
            changes |= CHAT_CHANGES_TITLE;
        }
        if (ev.changes & MEMBER_CHANGES_ONLINE) {
            changes |= CHAT_CHANGES_ONLINE;
            ordering_changed = (chats_list_ordering == CHATS_LIST_ORDERING_ONLINE);
            if (ordering_changed) {
                chats_list_needs_reorder = true;
            }
        }
        if (changes) {
            g_app_events.notify(AppEvents::ChatChanged {.chat=chat, .ordering_changed=ordering_changed, .changes=changes});
        }
    }
}

void AppDataModel::delete_message(MessageDataPtr msg, bool unsend) {
    std::vector<MessageDataPtr> msgs;
    msgs.push_back(msg);
    delete_messages(msg->get_chat(), msgs, unsend);
}

void AppDataModel::delete_messages(ChatDataPtr chat, const std::vector<MessageDataPtr>& msgs, bool unsend) {
    if (chat != nullptr) {
        std::string unsend_fld;
        if (unsend) {
            unsend_fld = "1";
        } else {
            unsend_fld = "0";
        }
        std::string message_ids;
        int i = 0;
        for(auto &msg : msgs) {
            if (i > 0) {
                message_ids.append(",");
            }
            message_ids.append(to_string(msg->id));
            i++;
        }
        auto *req = new CLChatApiRequest("POST", "/chat/" + chat->id + "/messages/delete/");
        req->post_data = {
                {"message_ids", message_ids},
                {"unsend", unsend_fld},
        };
        g_http_service.submit(req);

        bool last_message_deleted = false;

        for(auto msg : msgs) {
            for(int i = 0; i < chat->messages.size(); i++) {
                if (msg == chat->messages[i]) {
                    if (chat == currently_opened_chat) {
                        g_app_events.notify<AppEvents::MessageDeleted>({.chat = chat, .msg = chat->messages[i]});
                    }
                    if (chat->last_message->id == msg->id) {
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

void AppDataModel::delete_chat_history(ChatDataPtr chat, bool remove_from_chat_list, bool unsend) {
    std::string unsend_fld, remove_fld;
    if (unsend) {
        unsend_fld = "1";
    } else {
        unsend_fld = "0";
    }
    if (remove_from_chat_list) {
        remove_fld  = "1";
    } else {
        remove_fld = "0";
    }
    auto *req = new CLChatApiRequest("POST", "/chat/" + chat->id + "/clear/");
    req->post_data = {
            {"unsend", unsend_fld},
            {"remove", remove_fld},
    };
    g_http_service.submit(req);

    chat->last_message = nullptr;
}

void AppDataModel::get_contacts(char messenger_id, const std::string& except_in_chat_id, const std::function<void(const std::vector<MemberDataPtr>)>& callback) {
    auto success_callback = [this, callback](CLHTTPRequest* req) {
        cJSON *response_json = req->response_json;
        if (response_json && cJSON_IsArray(response_json)) {
            cJSON *json_item;
            MemberDataPtr mem;
            std::vector<MemberDataPtr> members;
            members.reserve(cJSON_GetArraySize(req->response_json));
            for(json_item = req->response_json->child; json_item != nullptr; json_item = json_item->next) {
                if (json_item && cJSON_IsObject(json_item))
                {
                    mem = update_or_create_member_data(json_item, false, false);
                    if (mem != nullptr) {
                        members.push_back(mem);
                    }
                }
            }
            callback(std::move(members));
        }
    };
    char url[250];
    if (except_in_chat_id.empty()) {
        sprintf(url, "/contacts/%c/", messenger_id);
    } else {
        sprintf(url, "/contacts/%c/?except_chat_id=%s", messenger_id, except_in_chat_id.c_str());
    }
    auto *req = new CLChatApiRequest("GET", url, success_callback);
    g_http_service.submit(req);
}

void AppDataModel::get_chat_members(const std::string& chat_id, const std::function<void(const std::vector<ChatMemberDataPtr>)>& callback) {
    auto success_callback = [this, callback](CLHTTPRequest* req) {
        cJSON *response_json = req->response_json;
        if (response_json && cJSON_IsArray(response_json)) {
            cJSON *json_item;
            MemberDataPtr mem;
            std::vector<ChatMemberDataPtr> chat_members;
            chat_members.reserve(cJSON_GetArraySize(response_json));
            for(json_item = response_json->child; json_item != nullptr; json_item = json_item->next) {
                if (json_item && cJSON_IsObject(json_item))
                {
                    ChatMemberDataPtr chat_mem = make_shared<ChatMemberData>(json_item);
                    chat_mem->member = update_or_create_member_data(
                            JsonData::get_json_object(json_item, "member"),false, false);
                    chat_members.push_back(chat_mem);
                }
            }
            callback(std::move(chat_members));
        }
    };
    char url[200];
    sprintf(url, "/chat/%s/members/", chat_id.c_str());
    auto *req = new CLChatApiRequest("GET", url, success_callback);
    req->set_hourglass(true);
    g_http_service.submit(req);
}

void AppDataModel::search_public_chats(char messenger_id, std::string &query, std::function<void(const std::vector<ChatDataPtr>)> callback) {
    auto success_callback = [this, callback](CLHTTPRequest* req) {
        cJSON *response_json = req->response_json;
        if (response_json && cJSON_IsArray(response_json)) {
            cJSON *json_item;
            ChatDataPtr chat;
            std::vector<ChatDataPtr> chats;
            chats.reserve(cJSON_GetArraySize(response_json));
            for(json_item = response_json->child; json_item != nullptr; json_item = json_item->next) {
                if (json_item && cJSON_IsObject(json_item))
                {
                    chat = update_or_create_chat_data(json_item, false, false, false);
                    if (chat != nullptr) {
                        chats.push_back(chat);
                    }
                }
            }
            callback(std::move(chats));
        }
    };
    CLStringsMap data = {
            {"query", riscos_local_to_utf8(query)},
    };
    char url[200];
    sprintf(url, "/chat/search/%c/", messenger_id);
    auto *req = new CLChatApiRequest("GET", url, success_callback);
    req->set_url_parameters(data);
    g_http_service.submit(req);
}

void AppDataModel::create_private_chat(const MemberDataPtr& mem, const ChatCallbackType& on_success_callback) {
    auto on_success = [this, on_success_callback](CLHTTPRequest* req) {
        on_success_callback(update_or_create_chat_data(req->response_json, false, true, true));
    };

    auto *req = new CLChatApiRequest("POST", "/chat/create/private/", on_success);
    req->post_data = {
            {"member_id", mem->id},
    };
    g_http_service.submit(req);
}

void AppDataModel::create_group_chat(const std::vector<MemberDataPtr> &members, const std::string& title, const ChatCallbackType &on_success_callback) {
    auto on_success = [this, on_success_callback](CLHTTPRequest* req) {
        on_success_callback(update_or_create_chat_data(req->response_json, false, true, true));
    };

    std::string member_ids;
    int i = 0;
    for(auto &mem : members) {
        if (i > 0) {
            member_ids.append(",");
        }
        member_ids.append(mem->id);
        i++;
    }
    auto *req = new CLChatApiRequest("POST", "/chat/create/group/", on_success);
    req->post_data = {
            {"member_ids", member_ids},
            {"title", riscos_local_to_utf8(title)},
    };
    g_http_service.submit(req);
}

bool AppDataModel::is_author_me(MessageDataPtr msg) {
    switch (msg->author->id[0]) {
        case MESSENGER_TELEGRAM:
            if (me->has_telegram()) {
                return msg->author->id == me->telegram_account.user_id;
            }
            return false;
        case MESSENGER_CHATCUBE:
            return msg->author->id == me->id;
        default:
            return false;
    }
}

void AppDataModel::forward_message(const std::vector<ChatDataPtr> &chats, const MessageDataPtr& msg, const std::function<void()> &on_success_callback) {
    auto on_success = [this, on_success_callback](CLHTTPRequest* req) {
        on_success_callback();
    };
    std::string forward_to_chat_ids;
    int i = 0;
    for(auto &chat : chats) {
        if (i > 0) {
            forward_to_chat_ids.append(",");
        }
        forward_to_chat_ids.append(chat->id);
        i++;
    }
    std::string url = "/chat/" + msg->get_chat()->id + "/forward/";
    auto *req = new CLChatApiRequest("POST", url, on_success);
    req->post_data = {
        {"chat_ids", forward_to_chat_ids},
        {"msg_id", to_string(msg->id)},
    };
    g_http_service.submit(req);
}

void AppDataModel::update_app() {
    std::string archive_file = "<ChatCube$ChoicesDir>.ChatCube-update/zip";
    std::string url = "https://api.chatcube.org/!ChatCube/ChatCube-v"+_latest_app_version+".zip";
    Logger::info("Update to new version %s started.", _latest_app_version.c_str());
    tbx::Path archive_file_path = tbx::Path(archive_file);
    archive_file_path.remove();
    remove_recursive("<ChatCube$Dir>.backup");
    remove_recursive("<ChatCube$Dir>.update");
    copy_dir(tbx::Path("<ChatCube$Dir>"), "<ChatCube$Dir>.backup", "backup");
    Logger::info("Made backup of old version to <ChatCube$Dir>.backup");
    auto on_success_download = [archive_file](CLHTTPRequest* request) {
        CLDownloadFileRequest* downloadreq = dynamic_cast<CLDownloadFileRequest*>(request);
        if (is_file_exist(downloadreq->saved_file_path)) {
            rename(downloadreq->saved_file_path.c_str(), archive_file.c_str());
            Logger::info("New version downloaded.");
            mkdir("<ChatCube$Dir>.update", 0777);
            AppEvents::ProgressBarControl pbreq;
            pbreq.label = "Extracting update";
            pbreq.percent_done = 99;
            g_app_events.notify(pbreq);
            auto on_next_tick = []() {
                tbx::Application::instance()->start_wimp_task("<ChatCube$Dir>.utils.!extract-update");
                if (is_file_exist("<ChatCube$Dir>.update.!ChatCube.!RunImage")) {
                    copy_dir(tbx::Path("<ChatCube$Dir>.update.!ChatCube.updater"), "<ChatCube$Dir>.update.updater", "");
                    Logger::info("Extract new version finished. Start replacer and exit");
                    tbx::Application::instance()->start_wimp_task("<ChatCube$Dir>.update.updater.updater");
                    tbx::Application::instance()->quit();
                } else {
                    Logger::error("Extract failed.");
                }
            };
            g_idle_task.run_at_next_idle(on_next_tick);
        }
    };
    auto on_fail_download = [](const HttpRequestError& err) {
        show_alert_error(std::string("Download of new version failed: " + err.error_message).c_str());
        return true;
    };
    auto *req = new CLDownloadFileRequest(url, on_success_download, on_fail_download);
    req->needs_progress = true;
    g_http_service.submit(req);

    AppEvents::ProgressBarControl pbreq;
    pbreq.label = "Downloading update";
    pbreq.req = req;
    g_app_events.notify(pbreq);
}

void AppDataModel::set_chat_title(const std::string& chat_id, const std::string& title, const std::function<void()> &on_success_callback) {
    auto on_success = [on_success_callback](CLHTTPRequest* req) {
        on_success_callback();
    };

    char url[200];
    sprintf(url, "/chat/%s/set-title/", chat_id.c_str());
    auto *req = new CLChatApiRequest("POST", url, on_success);
    req->post_data = {
        {"title", riscos_local_to_utf8(title)},
    };
    g_http_service.submit(req);
}

void AppDataModel::set_chat_photo(const std::string& chat_id, const std::string& file_path, const std::function<void()> &on_success_callback) {
    auto on_success = [on_success_callback](CLHTTPRequest* req) {
        on_success_callback();
    };

    char url[200];
    sprintf(url, "/chat/%s/set-photo/", chat_id.c_str());

    auto *req = new CLChatApiRequest("POST", url, on_success);
    req->upload_files["photo"] = file_path;
    req->set_timeout(0);
    req->set_lowspeed_limit(30,10);
    req->needs_progress = true;
    g_http_service.submit(req);
}

void AppDataModel::add_members_to_chat(const std::string& chat_id, std::vector<std::string>& member_ids, const std::function<void()> &on_success_callback) {
    auto on_success = [on_success_callback](CLHTTPRequest* req) {
        on_success_callback();
    };

    char url[200];
    sprintf(url, "/chat/%s/add-members/", chat_id.c_str());
    auto *req = new CLChatApiRequest("POST", url, on_success);
    req->post_data = {
            {"member_ids", str_join(member_ids,",")},
    };
    g_http_service.submit(req);
}

void AppDataModel::edit_message_text(const int64_t message_id, const std::string& text) {
    if (currently_opened_chat) {
        MessageDataPtr msg = currently_opened_chat->get_message(message_id);
        if (msg != nullptr) {
            std::string txt = text;
            txt = riscos_local_to_utf8(trim(txt," \t\n"));
            if (!txt.empty()) {
                char url[200];
                sprintf(url, "/chat/%s/messages/%lld/edit/", currently_opened_chat->id.c_str(), message_id);
                msg->text = txt;
                auto *req = new CLChatApiRequest("POST", url);
                req->post_data = {
                        {"text", msg->text}
                };
                g_http_service.submit(req);
                g_app_events.notify(AppEvents::MessageChanged {.chat=currently_opened_chat, .msg=msg});
            }
        }
    }
}

void AppDataModel::append_pending_outgoing_message(MessageDataPtr msg) {
    ChatDataPtr chat = msg->get_chat();
    if (chat == nullptr) {
        Logger::error("AppDataModel::append_pending_outgoing_message chat==null");
        return;
    }
    chat->messages.push_back(msg);
    chat->messages_pending_outgoing.push_back(msg);
    chat->last_message = msg;
    if (chats_list_ordering == CHATS_LIST_ORDERING_LAST_MESSAGE) {
        chats_list_needs_reorder = true;
    }
    g_app_events.notify(AppEvents::MessageAdded{.chat=msg->get_chat(), .msg=msg});
    g_app_events.notify(AppEvents::ChatChanged{.chat=msg->get_chat(), .ordering_changed=chats_list_needs_reorder, .changes=CHAT_CHANGES_LAST_MSG});
}


void AppDataModel::remove_pending_outgoing_message(MessageDataPtr msg) {
    ChatDataPtr chat = msg->get_chat();
    if (chat == nullptr) {
        Logger::error("AppDataModel::remove_pending_outgoing_message chat==null");
        return;
    }
    MessagesVector& messages_pending_outgoing = chat->messages_pending_outgoing;
    MessagesVector& messages = chat->messages;

    if (!messages_pending_outgoing.empty()) {
        for (size_t i = messages_pending_outgoing.size() - 1; i >= 0; --i) {
            if (messages_pending_outgoing[i] == msg) {
//                Logger::debug("Deleted pending instant message %s", msg->text.c_str());
                messages_pending_outgoing.erase(messages_pending_outgoing.begin() + i);
                break;
            }
        }
        for (size_t i = messages.size() - 1; i >= 0; --i) {
            if (messages[i] == msg) {
//                Logger::debug("Deleted messages message %s", msg->text.c_str());
                messages.erase(messages.begin() + i);
                g_app_events.notify(AppEvents::MessageDeleted { .chat = chat, .msg = msg });
                break;
            }
        }
    }
    if (chat->last_message == msg) {
        if (!messages.empty()) {
            chat->last_message = messages.back();
        } else {
            chat->last_message = nullptr;
        }
        if (chats_list_ordering == CHATS_LIST_ORDERING_LAST_MESSAGE) {
            chats_list_needs_reorder = true;
        }
        g_app_events.notify(AppEvents::ChatChanged{.chat=msg->get_chat(), .ordering_changed=chats_list_needs_reorder, .changes=CHAT_CHANGES_LAST_MSG});
    }
}


void AppDataModel::delete_account() {
    auto on_success = [this](CLHTTPRequest* req) {
        logout();
    };
    auto *req = new CLChatApiRequest("POST", "/profile/my/delete/", on_success);
    g_http_service.submit(req);
}


void AppDataModel::search_in_chat(ChatDataPtr chat, std::string query_utf8, int filter, int64_t starting_from_msg_id, const std::function<void(MessageDataPtr msg)> &callback) {
    MessagesVector &messages = chat->messages;
    int idx, starting_from_msg_index, messages_count = messages.size();

    if (messages_count == 0) {
        callback(nullptr);
        return;
    }
    if (starting_from_msg_id) {
        starting_from_msg_index = chat->get_message_index(starting_from_msg_id);
        if (starting_from_msg_index < 0) {
            callback(nullptr);
            return;
        }
        Logger::debug("ChatMainUI::search_next [%s] starting_from_msg_id=%lld", query_utf8.c_str(), starting_from_msg_id);
    } else {
        Logger::debug("ChatMainUI::search_next [%s]", query_utf8.c_str());
        starting_from_msg_index = messages_count - 1;
    }

    for(idx = starting_from_msg_index - 1; idx >= 0; idx--) {
        if (messages[idx]->is_filter_and_query_matched(filter, query_utf8)) {
            callback(messages[idx]);
            return;
        }
    }

    auto success_callack = [this, chat, query_utf8, filter, callback](CLHTTPRequest* req) {
        loading_messages_pending = false;
        auto response_json = req->response_json;
        const cJSON *json_items = JsonData::get_json_array(response_json, "items");
        if (json_items->child == NULL) {
            callback(nullptr);
            return;
        }
        chat->messages.clear();
        append_loaded_messages(chat, response_json);

        int idx;
        MessagesVector &messages = chat->messages;
        int messages_count = messages.size();
        for(idx = messages_count - 1; idx >= 0; idx--) {
            if (messages[idx]->is_filter_and_query_matched(filter, query_utf8)) {
                callback(messages[idx]);
                return;
            }
        }
    };
    auto fail_callback = [this,callback](const HttpRequestError& err) {
        loading_messages_pending = false;
        callback(nullptr);
        return false;
    };

    CLStringsMap data = {
            {"query", query_utf8},
            { "filter", to_string(filter)},
            {"from_message_id", to_string(starting_from_msg_id)}
    };
    std::string url = "/chat/" + chat->id + "/search/";
    auto *req = new CLChatApiRequest("GET", url, success_callack, fail_callback);
    req->set_url_parameters(data);
    req->set_hourglass(true);
    g_http_service.submit(req);
}

void AppDataModel::download_chat_history(ChatDataPtr chat, std::function<void(const std::string&)> on_success) {
    std::string url = "/chat/" + chat->id + "/messages/export/";
    std::string title = utf8_to_riscos_local(chat->title);
    title = str_replace_all(title, " ", "_");
    title = str_replace_all(title, ".", "_");
    title = str_replace_all(title, "/", "_");
    title = str_replace_all(title, ":", "_");
    std::string save_to = "<Choices$Write>.ChatCube.downloads.chat-history-"+title+"-"+chat->id+"/html";
    auto success_callback = [save_to, on_success](CLHTTPRequest* httpreq) {
        CLDownloadFileRequest* downloadreq = dynamic_cast<CLDownloadFileRequest*>(httpreq);
        if (downloadreq->move_file(save_to.c_str())) {
            on_success(save_to);
        }
    };

    auto *req = new CLDownloadFileRequest(url, success_callback);
    req->needs_progress = true;
    g_http_service.submit(req);
}

extern std::string app_version;
void AppDataModel::upload_feedback(const std::string& message, const std::string& feedback_type, bool send_logs, bool send_screenshoot, const std::function<void()> &on_success_callback) {
    auto on_success = [on_success_callback](CLHTTPRequest* req) {
        on_success_callback();
    };
    std::string log_path;
    if (send_logs) {
        char buf[4096];
        size_t size;
        log_path = "<ChatCube$ChoicesDir>.feedback-log";
        FILE *out = fopen(log_path.c_str(), "wb");
        FILE *in = fopen("<ChatCube$ChoicesDir>.log", "rb");

        while ((size = fread(buf, 1, BUFSIZ, in))) {
            fwrite(buf, 1, size, out);
        }
        fclose(in);
        in = fopen("<ChatCube$ChoicesDir>.log-prev", "rb");
        if (in) {
            fputs("\nPrev log\n", out);
            while ((size = fread(buf, 1, BUFSIZ, in))) {
                fwrite(buf, 1, size, out);
            }
            fclose(in);
        }
        fclose(out);
    }

    std::string screenshoot_path;
    if (send_screenshoot) {
        struct rosprite* sprite;
        tbx::ModeInfo m;
        tbx::BBox box = tbx::BBox(tbx::Point(0,0), m.screen_size());
        box.normalise();
        tbx::Application::instance()->yield();
        if (rosprite_capture_screenshoot(&sprite, box.min.x/2, box.min.y/2, box.max.x/2, box.max.y/2) == ROSPRITE_OK) {
            screenshoot_path = "<ChatCube$ChoicesDir>.feedback-screenshoot";
            CLImageJPGLoader::save(sprite, screenshoot_path, 75);
            rosprite_destroy_sprite(sprite);
            tbx::Path scr_path = tbx::Path(screenshoot_path);
            scr_path.file_type( 0xC85);
        }
    }

    auto *req = new CLChatApiRequest("POST", "/feedback/", on_success);
    req->post_data = {
            {"app_id", "XXX"},
            {"type", feedback_type},
            {"email", get_my_member_data()->email},
            {"message", riscos_local_to_utf8(message)},
            {"extra.app_version", app_version},
            {"extra.lang", app_version},
    };

    if (!screenshoot_path.empty()) {
        req->upload_files["screenshoot"] = screenshoot_path;
    }
    if (!log_path.empty()) {
        req->upload_files["log"] = log_path;
    }

    req->set_timeout(0);
    req->set_lowspeed_limit(30,10);
    req->needs_progress = true;

    AppEvents::ProgressBarControl pbreq;
    pbreq.label = "Uploading feedback";
    pbreq.req = req;
    g_app_events.notify(pbreq);

    g_http_service.submit(req);
}

AppDataModel g_app_data_model;
