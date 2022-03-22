//
// Created by lenz on 1/31/20.
//

#include <strstream>
#include "AppDataModel.h"
#include "AppEvents.h"
#include "NetworkRequests.h"
#include "ChatData.h"
#include "MessageData.h"
#include "cloverleaf/CLUtf8.h"
#include "cloverleaf/Logger.h"
//#include "../utils.h"

std::string AppDataModel::load_auth_token() {
    char token[255];
    FILE *f = nullptr;
    if (is_file_exist(saved_auth_path)) {
        f = fopen(saved_auth_path,"r");
    }
    if (f != nullptr) {
        fgets(token, sizeof(token)-1, f);
        char lastchar = token[strlen(token)-1];
        while(lastchar == 0xd || lastchar == 0xa || lastchar == 0x20) {
            token[strlen(token)-1] = 0;
            lastchar = token[strlen(token)-1];
        }
        if (token[0] != 0) {
            Logger::debug(".auth token=%s", token);
            fclose(f);
            return std::string(token);
        } else {
            Logger::debug(".auth token not found in file");
            fclose(f);
        }
    } else {
        Logger::debug(".auth not exists");
    }
    return string("");
}

void AppDataModel::save_auth_token(std::string &token) {
    FILE *f = fopen(saved_auth_path, "w");
    fprintf(f, "%s\n", token.c_str());
    fclose(f);
}

void AppDataModel::login(const std::string& email, const std::string& password, bool save_auth, std::function<void()> on_success, RequestFailCallbackType on_fail) {
    auto onLoginSuccess = [this, save_auth, on_success](CLHTTPRequest* req) {
        cJSON *response_json = req->response_json;
        std::string token = JsonData::get_string_value(response_json, "token");
        const cJSON *user_json = JsonData::get_json_object(response_json, "user");
        update_my_member_data(user_json);

        if (save_auth) {
            save_auth_token(token);
        }
        Logger::debug("Logged in %s channel %s", me->email.c_str(), me->push_channel.c_str());
        g_http_service.set_auth_token(token);
        g_http_service.start_event_stream(me->push_channel, JsonData::get_string_value(user_json, "push_channel_start_date"));
        g_app_events.notify(AppEvents::LoggedIn {});
        on_success();
    };
    Logger::debug("Loggin in %s", email.c_str());
    auto *req = new CLChatApiRequest("POST", "/auth/login/", onLoginSuccess, on_fail);
    req->post_data = {
            {"email",    email},
            {"password", password},
            {"app_version", _app_version}
    };

    g_http_service.submit(req);
}

void AppDataModel::signup(const std::string &first_name, const std::string &last_name,
                          const std::string &userid, const std::string &email, const std::string &displayname,
                          const std::string &phone, const std::string &password,
                          const std::string &platform,
                          bool newsletter,
                          std::function<void()> on_success,
                          RequestFailCallbackType on_fail) {

    auto on_success_callback = [this, on_success](CLHTTPRequest* req) {
        cJSON *response_json = req->response_json;
        std::string token = JsonData::get_string_value(response_json, "token");
        const cJSON *user_json = JsonData::get_json_object(response_json, "user");
        update_my_member_data(user_json);

        save_auth_token(token);
        g_http_service.set_auth_token(token);
        g_http_service.start_event_stream(me->push_channel, JsonData::get_string_value(user_json, "push_channel_start_date"));
        g_app_events.notify(AppEvents::SignedUp {});
        on_success();
    };

    Logger::debug("Registering member %s", email.c_str());
    auto *req = new CLChatApiRequest("POST", "/auth/signup/", on_success_callback, on_fail);
    req->post_data = {
            {"first_name",  riscos_local_to_utf8(first_name)},
            {"last_name",   riscos_local_to_utf8(last_name)},
            {"userid",      userid},
            {"email",       email},
            {"displayname", riscos_local_to_utf8(displayname)},
            {"password",    password},
            {"phone",       phone},
            {"platform",    std::string("riscos")},
            {"riscos",      platform},
            {"newsletter", (newsletter ? "true" : "false")},
            {"app_version", _app_version}
    };

    g_http_service.submit(req);
}

void AppDataModel::logout() {
    remove(saved_auth_path);
    _chats_list.clear();
    _chats_map.clear();
    _chats_filter_title.clear();
    _members_map.clear();
    _pending_updates.clear();
    _members_currently_loading.clear();
    messages_waiting_for_author.clear();
    me = nullptr;
    currently_opened_chat = nullptr;
    is_chat_list_loaded = false;
    loading_messages_pending = false;
    chats_list_needs_reorder = true;
    loading_missing_authors = false;
    g_http_service.set_auth_token("");
    g_http_service.stop_event_stream();
    g_app_events.notify(AppEvents::LoginRequired{});
}

void AppDataModel::send_verify_code(const std::string& phone, const std::string& email, RequestSuccessCallbackType on_success, RequestFailCallbackType on_fail) {
    Logger::debug("AppDataModel::send_verify_code  phone:%s email:%s", phone.c_str(), email.c_str());
    auto *req = new CLChatApiRequest("POST", "/auth/sendcode/", on_success, on_fail);
    req->post_data = {
            {"phone",  phone},
            {"email",  email}
    };
    g_http_service.submit(req);
}

void AppDataModel::verify_sent_code(const std::string& code, const std::string& phone, const std::string& email, RequestSuccessCallbackType on_success, RequestFailCallbackType on_fail) {
    Logger::debug("AppDataModel::verify_sent_code  code:%s phone:%s email:%s", code.c_str(), phone.c_str(), email.c_str());
    auto *req = new CLChatApiRequest("POST", "/auth/verifycode/", on_success);
    req->post_data = {
            {"code",  code},
            {"phone",  phone},
            {"email",  email}
    };
    g_http_service.submit(req);
}

void AppDataModel::reset_password_by_code(const std::string& code, const std::string& phone, const std::string& email, const std::string& password1, const std::string& password2, RequestSuccessCallbackType on_success, RequestFailCallbackType on_fail) {
    Logger::debug("AppDataModel::reset_password_by_code  password:%s code:%s phone:%s email:%s", password1.c_str(), code.c_str(), phone.c_str(), email.c_str());
    auto *req = new CLChatApiRequest("POST", "/auth/resetpassword/", on_success, on_fail);
    req->post_data = {
            {"password1",  password1},
            {"password2",  password2},
            {"code",  code},
            {"phone",  phone},
            {"email",  email}
    };
    g_http_service.submit(req);
}