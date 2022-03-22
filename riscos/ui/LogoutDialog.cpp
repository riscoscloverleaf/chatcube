//
// Created by lenz on 10/30/20.
//

#include "LogoutDialog.h"
#include "ChatMainUI.h"

LogoutDialog::LogoutDialog() :
    win("logoutCnfrm"),
    logout_cmd(this, &LogoutDialog::on_logout),
    quit_cmd(this, &LogoutDialog::on_quit)
{
    tbx::ActionButton(win.gadget(2)).add_selected_command(&logout_cmd);
    tbx::ActionButton(win.gadget(4)).add_selected_command(&quit_cmd);
    win.add_has_been_hidden_listener(this);
    win.show_centered();
}

LogoutDialog::~LogoutDialog() {
    win.delete_object();
}

void LogoutDialog::has_been_hidden(const tbx::EventInfo &event_info) {
    delete this;
}

void LogoutDialog::on_logout() {
    g_app_data_model.logout();
}

void LogoutDialog::on_quit() {
    tbx::Application::instance()->quit();
}
