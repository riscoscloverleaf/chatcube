//
// Created by lenz on 3/20/20.
//

#include "../model/AppDataModel.h"
#include <tbx/button.h>
#include <tbx/actionbutton.h>
#include <tbx/writablefield.h>
#include <cloverleaf/Logger.h>
#include "TelegramAuthCodeDialog.h"
#include "TelegramRegisterOrChangePhoneDialog.h"

TelegramAuthCodeDialog* TelegramAuthCodeDialog::instance = nullptr;

TelegramAuthCodeDialog::TelegramAuthCodeDialog(const std::string &phone, const std::string &auth_type) :
    win("tg_authcode"),
    cancel_cmd(this, &TelegramAuthCodeDialog::on_cancel),
    send_code_cmd(this, &TelegramAuthCodeDialog::on_send_code),
    change_phone_cmd(this, &TelegramAuthCodeDialog::on_change_phone)
{
    std::string label = "Please enter authorization code.";
    if (auth_type == "authenticationCodeTypeTelegramMessage") {
        label += " Code sent as Telegram message to "+phone;
    } else {
        label += " Code sent as SMS to "+phone;
    }

    tbx::Button(win.gadget(1)).value(label);
    tbx::ActionButton(win.gadget(3)).add_selected_command(&send_code_cmd);
    tbx::ActionButton(win.gadget(4)).add_selected_command(&change_phone_cmd);
    tbx::ActionButton(win.gadget(5)).add_selected_command(&cancel_cmd);

    win.show_centered();
    instance = this;
}

TelegramAuthCodeDialog::~TelegramAuthCodeDialog() {
    win.delete_object();
    instance = nullptr;
}

void TelegramAuthCodeDialog::on_send_code() {
    std::string code = tbx::WritableField(win.gadget(2)).text();
    auto on_fail = [this](const HttpRequestError& err) {
        Logger::error("Telegram error: %s code: %s", err.error_message.c_str(), err.error_code.c_str());
        if (err.error_code == "telegram:400:PHONE_CODE_INVALID") {
            show_alert_error("You entered invalid code!");
        } else {
            show_alert_error(err.error_message.c_str());
        }
        win.show();
        return true;
    };
    auto on_success = [this]() {
        delete this;
    };
    Logger::debug("sending auth code=%s", code.c_str());
    g_app_data_model.telegram_auth_code(code, on_success, on_fail);
}

void TelegramAuthCodeDialog::on_change_phone() {
    new TelegramRegisterOrChangePhoneDialog();
    delete this;
}

void TelegramAuthCodeDialog::on_cancel() {
    delete this;
}

void TelegramAuthCodeDialog::show(const std::string &phone, const std::string &auth_type) {
    if (!instance) {
        new TelegramAuthCodeDialog(phone, auth_type);
    } else {
        instance->win.show();
    }
}
