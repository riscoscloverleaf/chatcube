//
// Created by lenz on 3/20/20.
//

#ifndef ROCHAT_TELEGRAMACCEPTTOSDIALOG_H
#define ROCHAT_TELEGRAMACCEPTTOSDIALOG_H

#include <tbx/window.h>
#include <tbx/command.h>

class TelegramAcceptTOSDialog {
private:
    std::string tos_text;
    std::string tos_id;
    static TelegramAcceptTOSDialog *instance;
public:
    tbx::Window win;
    tbx::CommandMethod<TelegramAcceptTOSDialog> accept_cmd;
    tbx::CommandMethod<TelegramAcceptTOSDialog> decline_cmd;

    TelegramAcceptTOSDialog(const std::string& tos_text_, const std::string& tos_id_);
    ~TelegramAcceptTOSDialog();

    void on_accept();
    void on_decline();

    static void show(const std::string& tos_text_, const std::string& tos_id_);
};


#endif //ROCHAT_TELEGRAMACCEPTTOSDIALOG_H
