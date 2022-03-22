//
// Created by lenz on 2/4/20.
//

#ifndef ROCHAT_APPDATAMODEL_H
#define ROCHAT_APPDATAMODEL_H

#include <functional>
#include <map>
#include <cstdint>
#include <eventbus/EventBus.h>
#include "../service/CLHTTPService_v2.h"
#include "../service/CLHTTPService_v2.h"
#include "JsonData.h"
#include "AppDataModelTypes.h"
#include "AppEvents.h"
#include "AvatarData.h"
#include "ChatData.h"
#include "FileCacheDownloader.h"
#include "MemberData.h"
#include "ChatMemberData.h"
#include "MessageData.h"
#include "StickerData.h"
#include "NetworkRequests.h"

#define CHATS_LIST_ORDERING_ONLINE 1
#define CHATS_LIST_ORDERING_LAST_MESSAGE 2
#define CHATS_LIST_ORDERING_MEMBER_NAME 3

const char MESSENGER_CHATCUBE  = 'C';
const char MESSENGER_TELEGRAM  = 'T';

class AppDataModel {
private:
    const char *saved_auth_path =  "<Choices$Write>.ChatCube.authv2";

    std::vector<AvatarData> _avatars;
    std::vector<StickerGroupData> _stickers;
    std::vector<ChatDataPtr> _chats_list;
    std::map<std::string, ChatDataPtr> _chats_map;
    std::map<std::string, MemberDataPtr> _members_map;
    std::vector<std::string> _pending_updates;
    std::set<std::string> _members_currently_loading;
    std::map<std::string, std::vector<MessageDataPtr>*> messages_waiting_for_author;
    std::string _latest_app_version;
    std::string _app_version;
    std::string _chats_filter_title;

    MessageDataPtr marking_seen_msg = nullptr;
    MyMemberDataPtr me = nullptr;
    bool initialized = false;
    bool loading_missing_authors = false;
    bool chats_list_needs_reorder = true;
    bool loading_messages_pending = false;
    bool is_chat_list_loaded = false;
    int chats_list_ordering = CHATS_LIST_ORDERING_LAST_MESSAGE;

    ChatDataPtr currently_opened_chat = nullptr; // currently opened chat

    std::string load_auth_token();
    void save_auth_token(std::string &token);

    void request_missing_author(std::string &author_id, MessageDataPtr msg);
    void set_or_download_message_thumbnail(const ChatDataPtr chat, const MessageDataPtr msg);
    void append_loaded_messages(const ChatDataPtr chat, const cJSON* json);
    void append_to_pending_updates(const cJSON* json);
    void process_pending_updates();

    void on_pushstream_message(const cJSON* json);
    void on_member_loaded(const AppEvents::MemberLoaded& ev);
    void on_member_changed(const AppEvents::MemberChanged& ev);
    void on_telegram_ready(const AppEvents::TelegramReady& ev);

    void send_message(const CLStringsMap& post_data, MessageDataPtr instant_nessage);
    void send_message_upload_file(const CLStringsMap& post_data, const char *field_name, const std::string & file_name, MessageDataPtr instant_nessage);
    MessageDataPtr make_instant_outgoing_message(int type, const std::string& text, int reply_to_id);

    std::shared_ptr<MyMemberData> update_my_member_data(const cJSON* json);
    MemberDataPtr update_or_create_member_data(const cJSON* json, bool do_send_update_event=true, bool cache_member=true);
    ChatDataPtr update_or_create_chat_data(const cJSON* json, bool only_update_existing, bool do_send_update_event=true, bool add_to_chatlist=true);
    MessageDataPtr update_or_create_message_data(const cJSON* json, ChatDataPtr chat, bool only_update_existing, bool do_send_update_event, bool coming_from_event);
    void update_chat_outbox_data(const cJSON* json);
    void delete_chat_data(const cJSON* json, bool do_send_update_event=true);
    void delete_messages_data(const cJSON* json);
    void append_pending_outgoing_message(MessageDataPtr msg);
    void remove_pending_outgoing_message(MessageDataPtr msg);
public:
    void load_me(MyMemberCallbackType callback);
    void load_member(const std::string& member_id);
    void load_members(const vector<std::string>& member_ids);
    void load_chat_list(bool needs_progress = false);

    /* public methods */
    void init(const std::string& baseUrl, const std::string &lang, const std::string& _app_version);
    void start();

    void update_app();
    void upload_feedback(const std::string& message, const std::string& feedback_type, bool send_logs, bool send_screenshoot, const std::function<void()> &on_success_callback);

    bool is_logged_in() { return !g_http_service.get_auth_token().empty() && me != nullptr; }
    void login(const std::string& email, const std::string& password, bool save_auth, std::function<void()> on_success, RequestFailCallbackType on_fail);
    void logout();
    void delete_account();
    void signup(const std::string& first_name, const std::string& last_name,
                const std::string& userid, const std::string& email, const std::string& displayname,
                const std::string& phone, const std::string& password, const std::string& platform,
                bool newsletter,
                std::function<void()> on_success, RequestFailCallbackType on_fail);
    void change_my_profile(CLStringsMap &post_data, MyMemberCallbackType callback);
    void change_my_profile(CLStringsMap &post_data);
    void upload_profile_image(const std::string& file_path, const char* field_name, MyMemberCallbackType callback);
    void send_verify_code(const std::string& phone, const std::string& email, RequestSuccessCallbackType on_success, RequestFailCallbackType on_fail);
    void verify_sent_code(const std::string& code, const std::string& phone, const std::string& email, RequestSuccessCallbackType on_success, RequestFailCallbackType on_fail);
    void reset_password_by_code(const std::string& code, const std::string& phone, const std::string& email, const std::string& password1, const std::string& password2, RequestSuccessCallbackType on_success, RequestFailCallbackType on_fail);


    const std::string&                      get_latest_app_version() { return _latest_app_version; }
    const std::string&                      get_app_version() { return _app_version; }
    const std::vector<AvatarData> &         get_avatars() { return _avatars; }
    const std::vector<StickerGroupData> &   get_strickers() { return _stickers; }
    MemberDataPtr                           get_member(const std::string& member_id);
    ChatDataPtr                             get_chat(const std::string& chat_id);
    std::vector<ChatDataPtr>&               get_chats_list(); // return currently loaded chats
    std::shared_ptr<MyMemberData>           get_my_member_data() { return me; }
    ChatDataPtr                             get_currently_opened_chat() { return currently_opened_chat; };

    bool is_author_me(MessageDataPtr msg);

    void set_chats_list_ordering(int new_ordering);
    int get_chats_list_ordering();
    void filter_chats(const std::string& search_for);
    void search_in_chat(ChatDataPtr chat, std::string query_utf8, int filter, int64_t starting_from_msg_id, const std::function<void(MessageDataPtr msg)> &callback);
    void download_chat_history(ChatDataPtr chat, std::function<void(const std::string&)> on_success);
    void search_public_chats(char messenger_id, std::string& query_utf8, std::function<void(const std::vector<ChatDataPtr>)> callback);
    void get_contacts(char messenger_id, const std::string& except_in_chat_id, const std::function<void(const std::vector<MemberDataPtr>)> &callback);
    void get_chat_members(const std::string& chat_id, const std::function<void(const std::vector<ChatMemberDataPtr>)>& callback);

    void add_members_to_chat(const std::string& chat_id, std::vector<std::string>& member_ids, const std::function<void()> &on_success_callback);
    void set_chat_title(const std::string& chat_id, const std::string& title, const std::function<void()> &on_success_callback);
    void set_chat_photo(const std::string& chat_id, const std::string& file_path, const std::function<void()> &on_success_callback);


    void open_chat(const ChatDataPtr chat, bool force_reopen=false);
    void load_messages_in_chat(const ChatDataPtr chat, bool open_chat, int64_t starting_from_id, const std::function<void()> &on_success_callback, const std::function<void()> &on_fail_callback);
    void load_messages_in_chat(const ChatDataPtr chat, bool open_chat, int64_t starting_from_id);
    void load_more_messages_in_chat(const ChatDataPtr chat, bool load_older);
    void mark_seen_last_message(const ChatDataPtr& chat);
    void mark_seen_message(const MessageDataPtr& msg);
    void edit_message_text(const int64_t message_id, const std::string& text);
    void send_message_text(const std::string& text, int reply_to_id = 0);
    void send_message_sticker(const StickerData* sticker, int reply_to_id = 0);
    void send_message_photo(const std::string& photo_file, const std::string& caption, int reply_to_id = 0);
    void send_message_file(const std::string& file_path, const std::string& caption, int file_type, int reply_to_id = 0);
    void send_message_text_to_member(const string &member_id, const string& text);

    void send_chat_action(int action);

    void delete_message(MessageDataPtr msg, bool unsend=false);
    void delete_messages(ChatDataPtr chat, const std::vector<MessageDataPtr>& msgs, bool unsend=false);
    void delete_chat_history(ChatDataPtr chat, bool remove_from_chat_list, bool unsend=false);

    void create_private_chat(const MemberDataPtr& mem, const ChatCallbackType &on_success_callback);
    void create_group_chat(const std::vector<MemberDataPtr> &members, const std::string& title, const ChatCallbackType &on_success_callback);
    void forward_message(const std::vector<ChatDataPtr> &chats, const MessageDataPtr& msg, const std::function<void()> &on_success_callback);

    void telegram_register_or_change_phone(const std::string& phone, const std::function<void()>& on_success, RequestFailCallbackType on_fail);
    void telegram_auth_code(const std::string& code, const std::function<void()>& on_success, RequestFailCallbackType on_fail);
    void telegram_auth_password(const std::string& password, const std::function<void()>& on_success, RequestFailCallbackType on_fail);
    void telegram_auth_register(const std::string& first_name, const std::string& last_name, const std::function<void()>& on_success, RequestFailCallbackType on_fail);
    void telegram_accept_tos(const std::string& tos_id);
    void telegram_delete_account(const std::string& reason, const std::function<void()>& on_success);
    void telegram_unregister_account(const std::function<void()>& on_success);

    void get_local_config_value(const std::string& section, const std::string& key, const std::string& default_val);
    void get_local_config_value(const std::string& section, const std::string& key, const int default_val);
    void set_local_config_value(const std::string& section, const std::string& key, std::string& val);
    void set_local_config_value(const std::string& section, const std::string& key, int val);


};

extern AppDataModel g_app_data_model;

#endif //ROCHAT_CHATAPPMODEL_H
