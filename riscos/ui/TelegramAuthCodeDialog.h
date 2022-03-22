//
// Created by lenz on 3/20/20.
//

#ifndef ROCHAT_TELEGRAMAUTHCODEDIALOG_H
#define ROCHAT_TELEGRAMAUTHCODEDIALOG_H

#include <tbx/window.h>
#include <tbx/command.h>

class TelegramAuthCodeDialog {
private:
    static TelegramAuthCodeDialog* instance;
public:
    tbx::Window win;
    tbx::CommandMethod<TelegramAuthCodeDialog> send_code_cmd;
    tbx::CommandMethod<TelegramAuthCodeDialog> change_phone_cmd;
    tbx::CommandMethod<TelegramAuthCodeDialog> cancel_cmd;

    TelegramAuthCodeDialog(const std::string& phone, const std::string& auth_type);
    ~TelegramAuthCodeDialog();

    void on_send_code();
    void on_change_phone();
    void on_cancel();

    static void show(const std::string& phone, const std::string& auth_type);
};


#endif //ROCHAT_TELEGRAMAUTHCODEDIALOG_H
