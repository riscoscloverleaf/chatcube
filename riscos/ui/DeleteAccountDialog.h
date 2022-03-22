//
// Created by lenz on 1/15/21.
//

#ifndef ROCHAT_DELETEACCOUNTDIALOG_H
#define ROCHAT_DELETEACCOUNTDIALOG_H

#include "../model/AppDataModel.h"
#include <tbx/window.h>
#include <tbx/actionbutton.h>
#include <tbx/objectdelete.h>
#include <tbx/hasbeenhiddenlistener.h>
#include <tbx/command.h>


class DeleteAccountDialog : public tbx::HasBeenHiddenListener {
public:
    tbx::Window win;
    tbx::CommandMethod<DeleteAccountDialog> delete_account_cmd;

    DeleteAccountDialog();
    ~DeleteAccountDialog() override ;
    void on_delete_account();
    void has_been_hidden(const tbx::EventInfo &event_info) override;
};

#endif //ROCHAT_DELETEACCOUNTDIALOG_H
