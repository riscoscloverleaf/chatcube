//
// Created by lenz on 3/20/20.
//

#include "../model/AppDataModel.h"
#include <tbx/textarea.h>
#include <tbx/actionbutton.h>
#include <tbx/writablefield.h>
#include <tbx/optionbutton.h>
#include <cloverleaf/Logger.h>
#include "TelegramRegistrationDialog.h"
#include "../utils.h"

TelegramRegistrationDialog* TelegramRegistrationDialog::instance = nullptr;

TelegramRegistrationDialog::TelegramRegistrationDialog(const std::string& _tos_text) :
    win("tg_reg"),
    tos_text(_tos_text),
    continue_cmd(this, &TelegramRegistrationDialog::on_continue),
    cancel_cmd(this, &TelegramRegistrationDialog::on_cancel)
{
    tbx::TextArea(win.gadget(7)).text(tos_text);

    tbx::ActionButton(win.gadget(4)).add_selected_command(&continue_cmd);
    tbx::ActionButton(win.gadget(5)).add_selected_command(&cancel_cmd);
    win.show_centered();
    instance = this;
}

TelegramRegistrationDialog::~TelegramRegistrationDialog() {
    win.delete_object();
    instance = nullptr;
}

void TelegramRegistrationDialog::on_cancel() {
    delete this;
}

void TelegramRegistrationDialog::on_continue() {
    std::string first_name = tbx::WritableField(win.gadget(1)).text();
    std::string last_name = tbx::WritableField(win.gadget(3)).text();
    bool tos_accept = tbx::OptionButton(win.gadget(6)).on();

    if (!tos_accept) {
        show_alert_error("You must accept Terms Of Service to register in the Telegram");
        win.show();
        return;
    }

    auto on_fail = [this](const HttpRequestError& err) {
        Logger::error("Telegram registration error: %s code: %s", err.error_message.c_str(), err.error_code.c_str());
        show_alert_error((std::string("Registration error: ") + err.error_message).c_str());
        win.show();
        return true;
    };
    auto on_success = [this]() {
        delete this;
    };

    Logger::info("Registering TG account=%s %s", first_name.c_str(), last_name.c_str());
    g_app_data_model.telegram_auth_register(first_name, last_name, on_success, on_fail);
}

void TelegramRegistrationDialog::show(const std::string& _tos_text) {
    if (!instance) {
        new TelegramRegistrationDialog(_tos_text);
    } else {
        instance->win.show();
    }
}
