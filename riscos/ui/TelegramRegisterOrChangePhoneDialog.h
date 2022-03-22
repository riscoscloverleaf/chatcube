//
// Created by lenz on 3/20/20.
//

#ifndef ROCHAT_TELEGRAMREGISTERORCHANGEPHONEDIALOG_H
#define ROCHAT_TELEGRAMREGISTERORCHANGEPHONEDIALOG_H

#include <tbx/window.h>
#include <tbx/command.h>
#include <tbx/hasbeenhiddenlistener.h>

class TelegramRegisterOrChangePhoneDialog {
public:
    tbx::Window win;
    tbx::CommandMethod<TelegramRegisterOrChangePhoneDialog> continue_cmd;
    tbx::CommandMethod<TelegramRegisterOrChangePhoneDialog> cancel_cmd;

    TelegramRegisterOrChangePhoneDialog();
    ~TelegramRegisterOrChangePhoneDialog();

    void on_continue();
    void on_cancel();

    static void check_for_tg_account_and_create();
};


#endif //ROCHAT_TELEGRAMREGISTERORCHANGEPHONEDIALOG_H
