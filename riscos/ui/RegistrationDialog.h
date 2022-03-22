//
// Created by lenz on 3/20/20.
//

#ifndef ROCHAT_REGISTRATIONDIALOG_H
#define ROCHAT_REGISTRATIONDIALOG_H

#include <tbx/window.h>
#include <tbx/textchangedlistener.h>
#include <tbx/command.h>

class RegistrationDialog : public tbx::TextChangedListener {
public:
    tbx::Window win;
    tbx::CommandMethod<RegistrationDialog> register_cmd;
    tbx::CommandMethod<RegistrationDialog> cancel_cmd;

    RegistrationDialog();
    ~RegistrationDialog();

    void do_register();
    void do_cancel();

    void text_changed(tbx::TextChangedEvent &event) override;
};


#endif //ROCHAT_REGISTRATIONDIALOG_H
