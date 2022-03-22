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

void AppDataModel::telegram_register_or_change_phone(const std::string& phone, const std::function<void()>& on_success, RequestFailCallbackType on_fail) {
    auto success_callback = [this, on_success](CLHTTPRequest* req) {
        load_chat_list();
        on_success();
    };
    auto *req = new CLChatApiRequest("POST", "/auth/telegram/phone/", success_callback, on_fail);
    req->post_data = {
            {"phone", phone},
    };
    g_http_service.submit(req);
}

void AppDataModel::telegram_auth_code(const std::string& code, const std::function<void()>& on_success, RequestFailCallbackType on_fail) {
    auto success_callback = [this, on_success](CLHTTPRequest* req) {
        load_chat_list();
        on_success();
    };
    auto *req = new CLChatApiRequest("POST", "/auth/telegram/code/", success_callback, on_fail);
    req->post_data = {
            {"code", code},
    };
    g_http_service.submit(req);
}

void AppDataModel::telegram_auth_password(const std::string& password, const std::function<void()>& on_success, RequestFailCallbackType on_fail) {
    auto success_callback = [this, on_success](CLHTTPRequest* req) {
        load_chat_list();
        on_success();
    };
    auto *req = new CLChatApiRequest("POST", "/auth/telegram/password/", success_callback, on_fail);
    req->post_data = {
            {"password", password},
    };
    g_http_service.submit(req);
}

void AppDataModel::telegram_auth_register(const std::string& first_name, const std::string& last_name, const std::function<void()>& on_success, RequestFailCallbackType on_fail) {
    auto success_callback = [this, on_success](CLHTTPRequest* req) {
        load_chat_list();
        on_success();
    };
    auto *req = new CLChatApiRequest("POST", "/auth/telegram/register/", success_callback, on_fail);
    req->post_data = {
            {"first_name", first_name},
            {"last_name", last_name}
    };
    g_http_service.submit(req);
}

void AppDataModel::telegram_delete_account(const std::string& reason, const std::function<void()>& on_success) {
    auto success_callback = [this, on_success](CLHTTPRequest* req) {
        me->clear_telegram();
        load_chat_list();
        on_success();
    };
    auto *req = new CLChatApiRequest("POST", "/auth/telegram/delete/", success_callback);
    req->post_data = {
            {"reason", reason},
    };
    g_http_service.submit(req);
}

void AppDataModel::telegram_accept_tos(const std::string& tos_id) {
    auto success_callback = [this](CLHTTPRequest* req) {
        load_chat_list();
    };
    auto *req = new CLChatApiRequest("POST", "/auth/telegram/accepttos/", success_callback);
    req->post_data = {
            {"tos_id", tos_id},
    };
    g_http_service.submit(req);
}

void AppDataModel::on_telegram_ready(const AppEvents::TelegramReady &ev) {
    // reload my profile if telegram account is not set yet (happens when new TG account added)
    if (!me->has_telegram()) {
        auto on_success = [](MyMemberDataPtr me) {

        };
        load_me(on_success);
    }
}

void AppDataModel::telegram_unregister_account(const std::function<void()>& on_success) {
    auto success_callback = [this, on_success](CLHTTPRequest* req) {
        me->clear_telegram();
        load_chat_list();
        on_success();
    };
    auto *req = new CLChatApiRequest("POST", "/auth/telegram/unregister/", success_callback);
    g_http_service.submit(req);
}
