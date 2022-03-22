//
// Created by lenz on 5/31/20.
//

#include "../model/AppDataModel.h"
#include <tbx/textarea.h>
#include <tbx/button.h>
#include <tbx/actionbutton.h>
#include <tbx/writablefield.h>
#include <tbx/optionbutton.h>
#include <cloverleaf/Logger.h>
#include "TelegramPasswordDialog.h"

TelegramPasswordDialog* TelegramPasswordDialog::instance = nullptr;

TelegramPasswordDialog::TelegramPasswordDialog(const std::string &password_hint) :
    win("tg_passw"),
    continue_cmd(this, &TelegramPasswordDialog::on_continue),
    cancel_cmd(this, &TelegramPasswordDialog::on_cancel)
{
    std::string label = "Please enter your Telegram password.";
    if (!password_hint.empty()) {
        label += " Password hint: "+password_hint;
    }
    tbx::Button(win.gadget(1)).value(label);

    tbx::ActionButton(win.gadget(3)).add_selected_command(&continue_cmd);
    tbx::ActionButton(win.gadget(4)).add_selected_command(&cancel_cmd);
    win.show_centered();
    instance = this;
}

TelegramPasswordDialog::~TelegramPasswordDialog() {
    win.delete_object();
    instance = nullptr;
}

void TelegramPasswordDialog::on_continue() {
    std::string passw = tbx::WritableField(win.gadget(2)).text();
    auto on_fail = [this](const HttpRequestError& err) {
        Logger::error("Telegram password error: %s code: %s", err.error_message.c_str(), err.error_code.c_str());
        show_alert_error((std::string("Password verification error: ") + err.error_message).c_str());
        win.show();
        return true;
    };
    auto on_success = [this]() {
        delete this;
    };

    Logger::info("Send password for TG auth");
    g_app_data_model.telegram_auth_password(passw, on_success, on_fail);
}

void TelegramPasswordDialog::on_cancel() {
    delete this;
}

void TelegramPasswordDialog::show(const std::string &password_hint) {
    if (!instance) {
        new TelegramPasswordDialog(password_hint);
    } else {
        instance->win.show();
    }
}