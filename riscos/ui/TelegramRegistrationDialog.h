//
// Created by lenz on 3/20/20.
//

#ifndef ROCHAT_TELEGRAMREGISTRATIONDIALOG_H
#define ROCHAT_TELEGRAMREGISTRATIONDIALOG_H

#include <tbx/window.h>
#include <tbx/command.h>

class TelegramRegistrationDialog {
private:
    std::string tos_text;
    static TelegramRegistrationDialog *instance;
public:
    tbx::Window win;
    tbx::CommandMethod<TelegramRegistrationDialog> continue_cmd;
    tbx::CommandMethod<TelegramRegistrationDialog> cancel_cmd;

    TelegramRegistrationDialog(const std::string& _tos_text);
    ~TelegramRegistrationDialog();

    void on_continue();
    void on_cancel();

    static void show(const std::string& _tos_text);
};


#endif //ROCHAT_TELEGRAMREGISTRATIONDIALOG_H
