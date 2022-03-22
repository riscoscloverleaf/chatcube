//
// Created by lenz on 2/26/20.
//

#include "../model/AppDataModel.h"
#include "LoginDialog.h"
#include "RegistrationDialog.h"
#include "ResetPasswordDialog.h"
#include <tbx/deleteonhidden.h>

LoginDialog::LoginDialog():
        win("LoginWindow"),
        cancel_cmd(this, &LoginDialog::on_cancel),
        register_cmd(this, &LoginDialog::on_register),
        reset_password_cmd(this, &LoginDialog::on_reset_password),
        login_cmd(this, &LoginDialog::on_login)
{
    tbx::ActionButton(win.gadget(4)).add_selected_command(&login_cmd);
    tbx::ActionButton(win.gadget(6)).add_selected_command(&register_cmd);
    tbx::ActionButton(win.gadget(5)).add_selected_command(&cancel_cmd);
    tbx::ActionButton(win.gadget(8)).add_selected_command(&reset_password_cmd);
    win.show_centered();
}

LoginDialog::~LoginDialog() {
    win.delete_object();
}

void LoginDialog::on_cancel() {
    delete this;
}

void LoginDialog::on_register() {
    new RegistrationDialog();
    delete this;
}

void LoginDialog::on_reset_password() {
    new ResetPasswordSendCodeDialog();
    delete this;
}

void LoginDialog::on_login() {
    std::string login = tbx::WritableField(win.gadget(1)).text();
    std::string password = tbx::WritableField(win.gadget(3)).text();
    bool stay_logged_in = tbx::OptionButton(win.gadget(9)).on();

    //Logger::debug("LoginDialog::on_login %s - %s", login.c_str(), password.c_str());

    auto on_fail = [this](const HttpRequestError& err) {
        auto found = err.field_errors.find("email");
//        Logger::debug("LoginDialog::on_fail");
        if (found != err.field_errors.end()) {
            show_alert_error(("Email: " + found->second.error_message).c_str());
            win.show();
            return true;
        }
        found = err.field_errors.find("password");
        if (found != err.field_errors.end()) {
            show_alert_error(("Password: " + found->second.error_message).c_str());
            win.show();
            return true;
        }
        win.show();
        return false;
    };
    auto on_success = [this]() {
        delete this;
    };
    g_app_data_model.login(login, password, (bool)stay_logged_in, on_success, on_fail);
}