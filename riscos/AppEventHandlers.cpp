//
// Created by lenz on 2/9/20.
//
#include "model/AppDataModel.h"
#include "cloverleaf/IdleTask.h"
#include "utils.h"
#include "global.h"
#include "service/IKConfig.h"
#include "ui/ChatMainUI.h"
#include "ui/LoginDialog.h"
#include "ui/TelegramRegisterOrChangePhoneDialog.h"
#include "ui/TelegramAuthCodeDialog.h"
#include "ui/TelegramRegistrationDialog.h"
#include "ui/TelegramAcceptTOSDialog.h"
#include "ui/TelegramPasswordDialog.h"
#include "ui/AvatarChanger.h"

void on_init_data_loaded(const AppEvents::InitialDataLoaded& ev) {
    Logger::debug("init_data_loaded");
    g_hourglass_off();
}

void on_signed_up(const AppEvents::SignedUp& ev) {
    Logger::debug("new member signed up");
    ChatMainUI::open();
    g_app_data_model.load_chat_list(true);

    auto select_avatar = []() {
        show_avatar_changer(MESSENGER_CHATCUBE);
    };
    g_idle_task.run_at_next_idle(select_avatar);
}

void on_logged_in(const AppEvents::LoggedIn& ev) {
    Logger::debug("logged_in");
    ChatMainUI::open();
    if (g_app_state.ISUPDATED) {
        remove_recursive("<ChatCube$Dir>.update");
        remove_recursive("<ChatCube$Dir>.updater");
        show_alert_info("ChatCube updated successfully!");
    }
    g_app_data_model.load_chat_list(g_app_state.start_hidden);
}

void on_login_required(const AppEvents::LoginRequired& ev) {
    Logger::debug("login_required");
    ChatMainUI::hide();
    new LoginDialog();
}

void on_downloading_files_progress(const AppEvents::DownloadingFilesProgress& ev) {
    if (ev.percent_done < 100) {
        g_hourglass_on();
        g_hourglass_percentage(ev.percent_done);
    } else {
        g_hourglass_off();
    }
    Logger::debug("downloading %d", ev.percent_done);
}

void on_uploading_file_progress(const AppEvents::UploadingProgress& ev) {
    if (ev.percent_done < 100) {
        g_hourglass_on();
        g_hourglass_percentage(ev.percent_done);
    } else {
        g_hourglass_off();
    }
    Logger::debug("on_uploading_file_progress %s -> %d %", ev.req->url.c_str(), ev.percent_done);
}

void on_downloading_file_progress(const AppEvents::DownloadingProgress& ev) {
    if (ev.percent_done < 100) {
//        Logger::debug("ev.percent_done=%d", ev.percent_done);
        g_hourglass_on();
        g_hourglass_percentage(ev.percent_done);
    } else {
        g_hourglass_off();
    }
//    Logger::debug("on_downloading_file_progress %s -> %d %", ev.url.c_str(), ev.percent_done);
}

void on_show_alert(const AppEvents::ShowAlert& ev) {
    Logger::debug("on_show_alert  %s", ev.message.c_str());
    show_alert_error(ev.message.c_str());
}

void on_telegram_auth_code(const AppEvents::TelegramAuthCode& ev) {
    Logger::debug("on_telegram_auth_code, phone = %s", ev.data.phone.c_str());
    //TelegramAuthCodeDialog::open(ev.data.phone, ev.data.auth_type);
    TelegramAuthCodeDialog::show(ev.data.phone, ev.data.auth_type);
}

void on_telegram_auth_password(const AppEvents::TelegramAuthPassword& ev) {
    Logger::debug("on_telegram_auth_password");
    TelegramPasswordDialog::show(ev.data.password_hint);
}

void on_telegram_auth_registration(const AppEvents::TelegramAuthRegistration& ev) {
    Logger::debug("on_telegram_auth_registration");
    TelegramRegistrationDialog::show(ev.data.tos_text);
}

void on_telegram_accept_tos(const AppEvents::TelegramTermsOfService& ev) {
    Logger::debug("on_telegram_accept_tos");
    TelegramAcceptTOSDialog::show(ev.data.tos_text, ev.data.tos_id);
}

void setup_app_event_handlers() {
    g_app_events.listen<AppEvents::InitialDataLoaded>(&on_init_data_loaded);
    g_app_events.listen<AppEvents::DownloadingFilesProgress>(&on_downloading_files_progress);
    g_app_events.listen<AppEvents::UploadingProgress>(&on_uploading_file_progress);
    g_app_events.listen<AppEvents::DownloadingProgress>(&on_downloading_file_progress);

    g_app_events.listen<AppEvents::SignedUp>(&on_signed_up);
    g_app_events.listen<AppEvents::LoggedIn>(&on_logged_in);
    g_app_events.listen<AppEvents::LoginRequired>(&on_login_required);

    g_app_events.listen<AppEvents::ShowAlert>(&on_show_alert);

    g_app_events.listen<AppEvents::TelegramAuthCode>(&on_telegram_auth_code);
    g_app_events.listen<AppEvents::TelegramAuthPassword>(&on_telegram_auth_password);
    g_app_events.listen<AppEvents::TelegramAuthRegistration>(&on_telegram_auth_registration);
    g_app_events.listen<AppEvents::TelegramTermsOfService>(&on_telegram_accept_tos);
}