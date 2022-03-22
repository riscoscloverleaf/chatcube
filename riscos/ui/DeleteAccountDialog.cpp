//
// Created by lenz on 1/15/21.
//

#include "DeleteAccountDialog.h"

DeleteAccountDialog::DeleteAccountDialog():
        win("delAccCnfrm"),
        delete_account_cmd(this, &DeleteAccountDialog::on_delete_account)
{
    tbx::ActionButton(win.gadget(2)).add_selected_command(&delete_account_cmd);
    win.add_has_been_hidden_listener(this);
    win.show_centered();
}

DeleteAccountDialog::~DeleteAccountDialog() {
    win.delete_object();
}

void DeleteAccountDialog::has_been_hidden(const tbx::EventInfo &event_info) {
    delete this;
}

void DeleteAccountDialog::on_delete_account() {
    g_app_data_model.delete_account();
}

