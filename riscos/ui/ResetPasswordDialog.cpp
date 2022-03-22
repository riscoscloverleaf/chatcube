//
// Created by lenz on 3/27/20.
//
#include "../model/AppDataModel.h"
#include "ResetPasswordDialog.h"
#include "LoginDialog.h"

// ResetPasswordSendCodeDialog
ResetPasswordSendCodeDialog::ResetPasswordSendCodeDialog() :
    cmd_cancel(this, &ResetPasswordSendCodeDialog::on_cancel),
    cmd_ok(this, &ResetPasswordSendCodeDialog::on_ok)
{
    win = tbx::Window("reset1");
    win.show_centered();
    tbx::ActionButton(win.gadget(4)).add_selected_command(&cmd_ok);
    tbx::ActionButton(win.gadget(5)).add_selected_command(&cmd_cancel);
}

ResetPasswordSendCodeDialog::~ResetPasswordSendCodeDialog() {
    win.delete_object();
}

void ResetPasswordSendCodeDialog::on_cancel() {
    new LoginDialog();

    delete this;
}

void ResetPasswordSendCodeDialog::on_ok() {
    std::string phone = "";
    std::string email = tbx::WritableField(win.gadget(3)).text();

    auto on_success = [this, email, phone](CLHTTPRequest* req) {
        new ResetPasswordVerifyCodeDialog(email, phone);
        delete this;
    };
    auto on_fail = [this](const HttpRequestError& error) {
        show_alert_error(error.error_message.c_str());
        win.show();
        return true;
    };

    g_app_data_model.send_verify_code(phone, email, on_success, on_fail);
}

// ResetPasswordVerifyCodeDialog
ResetPasswordVerifyCodeDialog::ResetPasswordVerifyCodeDialog(const std::string& email_, const std::string& phone_) :
    email(email_),
    phone(phone_),
    cmd_cancel(this, &ResetPasswordVerifyCodeDialog::on_cancel),
    cmd_ok(this, &ResetPasswordVerifyCodeDialog::on_ok)
{
    win = tbx::Window("reset2");
    win.show_centered();
    tbx::ActionButton(win.gadget(4)).add_selected_command(&cmd_ok);
    tbx::ActionButton(win.gadget(5)).add_selected_command(&cmd_cancel);
}

ResetPasswordVerifyCodeDialog::~ResetPasswordVerifyCodeDialog() {
    win.delete_object();
}

void ResetPasswordVerifyCodeDialog::on_cancel() {
    new ResetPasswordSendCodeDialog();

    delete this;
}

void ResetPasswordVerifyCodeDialog::on_ok() {
    std::string code = tbx::WritableField(win.gadget(1)).text();

    auto on_success = [this, code](CLHTTPRequest* req) {
        new ResetPasswordSetPasswordDialog(email, phone, code);
        delete this;
    };
    auto on_fail = [this](const HttpRequestError& error) {
        show_alert_error(error.error_message.c_str());
        win.show();
        return true;
    };

    g_app_data_model.verify_sent_code(code, phone, email, on_success, on_fail);
}


// ResetPasswordSetPasswordDialog
ResetPasswordSetPasswordDialog::ResetPasswordSetPasswordDialog(const std::string& email_, const std::string& phone_, const std::string& code_) :
        email(email_),
        phone(phone_),
        code(code_),
        cmd_cancel(this, &ResetPasswordSetPasswordDialog::on_cancel),
        cmd_ok(this, &ResetPasswordSetPasswordDialog::on_ok)
{
    win = tbx::Window("reset3");
    win.show_centered();
    tbx::ActionButton(win.gadget(4)).add_selected_command(&cmd_ok);
    tbx::ActionButton(win.gadget(5)).add_selected_command(&cmd_cancel);
}

ResetPasswordSetPasswordDialog::~ResetPasswordSetPasswordDialog() {
    win.delete_object();
}

void ResetPasswordSetPasswordDialog::on_cancel() {
    new LoginDialog();

    delete this;
}

void ResetPasswordSetPasswordDialog::on_ok() {
    std::string password1 = tbx::WritableField(win.gadget(1)).text();
    std::string password2 = tbx::WritableField(win.gadget(3)).text();

    auto on_success = [this](CLHTTPRequest* req) {
        new LoginDialog();
        delete this;
    };
    auto on_fail = [this](const HttpRequestError& error) {
        show_alert_error(error.error_message.c_str());
        win.show();
        return true;
    };

    g_app_data_model.reset_password_by_code(code, phone, email, password1, password2, on_success, on_fail);
}
