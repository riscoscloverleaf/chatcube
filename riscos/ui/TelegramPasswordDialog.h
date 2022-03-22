//
// Created by lenz on 5/31/20.
//

#ifndef ROCHAT_TELEGRAMPASSWORDDIALOG_H
#define ROCHAT_TELEGRAMPASSWORDDIALOG_H

#include <tbx/window.h>
#include <tbx/command.h>
#include "TelegramRegistrationDialog.h"

class TelegramPasswordDialog {
private:
    static TelegramPasswordDialog *instance;
public:
    tbx::Window win;
    tbx::CommandMethod<TelegramPasswordDialog> continue_cmd;
    tbx::CommandMethod<TelegramPasswordDialog> cancel_cmd;

    TelegramPasswordDialog(const std::string& password_hint);
    ~TelegramPasswordDialog();

    void on_continue();
    void on_cancel();

    static void show(const std::string& password_hint);
};


#endif //ROCHAT_TELEGRAMPASSWORDDIALOG_H
