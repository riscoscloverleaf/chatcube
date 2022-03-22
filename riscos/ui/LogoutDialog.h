//
// Created by lenz on 10/30/20.
//

#ifndef ROCHAT_LOGOUTDIALOG_H
#define ROCHAT_LOGOUTDIALOG_H

#include "../model/AppDataModel.h"
#include <tbx/window.h>
#include <tbx/actionbutton.h>
#include <tbx/objectdelete.h>
#include <tbx/hasbeenhiddenlistener.h>
#include <tbx/command.h>

class LogoutDialog : public tbx::HasBeenHiddenListener {
public:
    tbx::Window win;
    tbx::CommandMethod<LogoutDialog> logout_cmd;
    tbx::CommandMethod<LogoutDialog> quit_cmd;

    LogoutDialog();
    ~LogoutDialog() override ;
    void on_logout();
    void on_quit();
    void has_been_hidden(const tbx::EventInfo &event_info) override;
};


#endif //ROCHAT_LOGOUTDIALOG_H
