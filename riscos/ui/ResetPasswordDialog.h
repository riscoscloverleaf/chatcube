//
// Created by lenz on 3/27/20.
//

#ifndef ROCHAT_RESETPASSWORDDIALOG_H
#define ROCHAT_RESETPASSWORDDIALOG_H


#include <tbx/window.h>
#include <tbx/hasbeenhiddenlistener.h>
#include <tbx/command.h>
#include <tbx/actionbutton.h>

class ResetPasswordSendCodeDialog {
public:
    tbx::Window win;
    tbx::CommandMethod<ResetPasswordSendCodeDialog> cmd_ok;
    tbx::CommandMethod<ResetPasswordSendCodeDialog> cmd_cancel;

    ResetPasswordSendCodeDialog();
    ~ResetPasswordSendCodeDialog();
    void on_cancel();
    void on_ok();
};

class ResetPasswordVerifyCodeDialog {
public:
    std::string email;
    std::string phone;
    tbx::Window win;
    tbx::CommandMethod<ResetPasswordVerifyCodeDialog> cmd_ok;
    tbx::CommandMethod<ResetPasswordVerifyCodeDialog> cmd_cancel;

    ResetPasswordVerifyCodeDialog(const std::string& email, const std::string& phone);
    ~ResetPasswordVerifyCodeDialog();
    void on_cancel();
    void on_ok();
};

class ResetPasswordSetPasswordDialog {
public:
    std::string email;
    std::string phone;
    std::string code;

    tbx::Window win;
    tbx::CommandMethod<ResetPasswordSetPasswordDialog> cmd_ok;
    tbx::CommandMethod<ResetPasswordSetPasswordDialog> cmd_cancel;

    ResetPasswordSetPasswordDialog(const std::string& email, const std::string& phone, const std::string& code);
    ~ResetPasswordSetPasswordDialog();
    void on_cancel();
    void on_ok();
};

#endif //ROCHAT_RESETPASSWORDDIALOG_H
