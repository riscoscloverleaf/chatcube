//
// Created by lenz on 3/20/20.
//

#include "../model/AppDataModel.h"
#include <tbx/textarea.h>
#include <tbx/actionbutton.h>
#include "TelegramAcceptTOSDialog.h"
TelegramAcceptTOSDialog* TelegramAcceptTOSDialog::instance = nullptr;

TelegramAcceptTOSDialog::TelegramAcceptTOSDialog(const std::string& tos_text_, const std::string& tos_id_) :
        tos_text(tos_id_),
        tos_id(tos_id_),
        win("tg_tos"),
        decline_cmd(this, &TelegramAcceptTOSDialog::on_decline),
        accept_cmd(this, &TelegramAcceptTOSDialog::on_accept)
{
    tbx::TextArea(win.gadget(1)).text(tos_text);

    tbx::ActionButton(win.gadget(2)).add_selected_command(&accept_cmd);
    tbx::ActionButton(win.gadget(3)).add_selected_command(&decline_cmd);

    win.show_centered();
    instance = this;
}

TelegramAcceptTOSDialog::~TelegramAcceptTOSDialog() {
    win.delete_object();
    instance = nullptr;
}

void TelegramAcceptTOSDialog::on_accept() {
    g_app_data_model.telegram_accept_tos(tos_id);
    delete this;
}

void TelegramAcceptTOSDialog::on_decline() {
    auto on_success = []() {};
    g_app_data_model.telegram_delete_account("Decline ToS update", on_success);
    delete this;
}

void TelegramAcceptTOSDialog::show(const std::string &tos_text_, const std::string &tos_id_) {
    if (!instance) {
        new TelegramAcceptTOSDialog(tos_text_, tos_id_);
    } else {
        instance->win.show();
    }
}

