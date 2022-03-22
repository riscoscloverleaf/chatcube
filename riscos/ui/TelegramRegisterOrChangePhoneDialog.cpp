//
// Created by lenz on 3/20/20.
//

#include "../model/AppDataModel.h"
#include "TelegramRegisterOrChangePhoneDialog.h"
#include <cloverleaf/Logger.h>
#include <tbx/writablefield.h>
#include <tbx/actionbutton.h>

TelegramRegisterOrChangePhoneDialog::TelegramRegisterOrChangePhoneDialog() :
    win("tg_phone"),
    cancel_cmd(this, &TelegramRegisterOrChangePhoneDialog::on_cancel),
    continue_cmd(this, &TelegramRegisterOrChangePhoneDialog::on_continue)
{
    MyMemberDataPtr me = g_app_data_model.get_my_member_data();
    if (!me->telegram_account.phone.empty()) {
        tbx::WritableField(win.gadget(2)).text(me->telegram_account.phone);
    }
    tbx::ActionButton(win.gadget(3)).add_selected_command(&continue_cmd);
    tbx::ActionButton(win.gadget(4)).add_selected_command(&cancel_cmd);

    win.show_centered();
}

TelegramRegisterOrChangePhoneDialog::~TelegramRegisterOrChangePhoneDialog() {
    win.delete_object();
}

void TelegramRegisterOrChangePhoneDialog::on_cancel() {
    delete this;
}

void TelegramRegisterOrChangePhoneDialog::on_continue() {
    auto on_fail = [this](const HttpRequestError& err) {
        Logger::error("Telegram error: %s code: %s", err.error_message.c_str(), err.error_code.c_str());
        win.show();
        return false;
    };
    auto on_success = [this]() {
        delete this;
    };
    std::string phone = tbx::WritableField(win.gadget(2)).text();
    Logger::info("regsiter TG phone=%s", phone.c_str());
    g_app_data_model.telegram_register_or_change_phone(phone, on_success, on_fail);
}