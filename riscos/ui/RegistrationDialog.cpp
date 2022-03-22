//
// Created by lenz on 3/20/20.
//

#include "../model/AppDataModel.h"
#include <tbx/actionbutton.h>
#include <tbx/writablefield.h>
#include <tbx/stringset.h>
#include <tbx/optionbutton.h>
#include "RegistrationDialog.h"
#include "../global.h"

RegistrationDialog::RegistrationDialog() :
    win("Register"),
    register_cmd(this, &RegistrationDialog::do_register),
    cancel_cmd(this, &RegistrationDialog::do_cancel)
{
    tbx::ActionButton(win.gadget(8)).add_selected_command(&register_cmd);
    tbx::ActionButton(win.gadget(9)).add_selected_command(&cancel_cmd);
    tbx::WritableField(win.gadget(0x11)).add_text_changed_listener(this);
    tbx::WritableField(win.gadget(0x12)).add_text_changed_listener(this);
    win.show_centered();
}

RegistrationDialog::~RegistrationDialog() {
    win.delete_object();
}

void RegistrationDialog::do_cancel() {
    delete this;
}

void RegistrationDialog::do_register() {
    std::string fname = tbx::WritableField(win.gadget(0x11)).text();
    std::string lname = tbx::WritableField(win.gadget(0x12)).text();
    std::string displayname = tbx::WritableField(win.gadget(3)).text();
    std::string userid = tbx::WritableField(win.gadget(0x13)).text();
    std::string email = tbx::WritableField(win.gadget(1)).text();
    std::string password = tbx::WritableField(win.gadget(5)).text();
    std::string confirm_password = tbx::WritableField(win.gadget(7)).text();
    std::string country_code = tbx::WritableField(win.gadget(0x15)).text();
    std::string phone = tbx::WritableField(win.gadget(0xc)).text();
    std::string platform = tbx::StringSet(win.gadget(0xa)).selected();
    bool newsletter = tbx::OptionButton(win.gadget(0x16)).on();

    if (password != confirm_password) {
        show_alert_error("Password fields should match");
        win.show();
    } else {
        auto on_fail = [this](const HttpRequestError& err) {
            show_alert_error(err.error_message.c_str());
            win.show();
            return true;
        };

        auto on_success = [this]() {
            delete this;
        };

        g_app_data_model.signup(fname, lname, userid, email, displayname, phone,
                                password, platform, newsletter, on_success, on_fail);
    }
}

void RegistrationDialog::text_changed(tbx::TextChangedEvent &event) {
    std::string fname = tbx::WritableField(win.gadget(0x11)).text();
    std::string lname = tbx::WritableField(win.gadget(0x12)).text();
    if (fname.size() > 1 && lname.size() > 1) {
        std::string timestr = to_string(time(NULL));
        std::string userid;
        userid += toupper(fname[0]);
        userid += toupper(fname[1]);
        userid += toupper(lname[0]);
        userid += toupper(lname[1]);
        userid += timestr.substr(timestr.size()-5, 5);
        tbx::WritableField(win.gadget(0x13)).text(userid);

        std::string displayname = fname;
        displayname += " ";
        displayname += lname;
        tbx::WritableField(win.gadget(0x3)).text(displayname);
    }
}


