//
// Created by lenz on 2/26/20.
//

#ifndef ROCHAT_LOGINDIALOG_H
#define ROCHAT_LOGINDIALOG_H

#include "../model/AppEvents.h"
#include <tbx/window.h>
#include <tbx/actionbutton.h>
#include <tbx/writablefield.h>
#include <tbx/optionbutton.h>
#include <tbx/objectdelete.h>
#include <tbx/command.h>

class LoginDialog {
private:
    static LoginDialog *instance;
public:
    tbx::Window win;
    tbx::CommandMethod<LoginDialog> cancel_cmd;
    tbx::CommandMethod<LoginDialog> login_cmd;
    tbx::CommandMethod<LoginDialog> register_cmd;
    tbx::CommandMethod<LoginDialog> reset_password_cmd;

    LoginDialog();
    ~LoginDialog();
    static void open();
    void on_login();
    void on_cancel();
    void on_register();
    void on_reset_password();
};


#endif //ROCHAT_LOGINDIALOG_H
