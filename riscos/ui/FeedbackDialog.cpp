//
// Created by lenz on 4/16/21.
//

#include "FeedbackDialog.h"
#include "../model/AppDataModel.h"
#include <cloverleaf/IdleTask.h>

static FeedbackDialog* _instance = nullptr;

FeedbackDialog::FeedbackDialog() :
        win(tbx::Window("FeedbackWin"))
{
    win.add_has_been_hidden_listener(this);
    tbx::ActionButton(win.gadget(4)).add_selected_listener(this);

}

FeedbackDialog::~FeedbackDialog() {
    win.delete_object();
    _instance = nullptr;
}

void FeedbackDialog::has_been_hidden(const tbx::EventInfo &event_info) {
    if (can_delete_on_hide) {
        delete this;
    }
}

void FeedbackDialog::open() {
    if (_instance) {
        _instance->win.show();
    } else {
        _instance = new FeedbackDialog();
        _instance->win.show_centered();
    }
}

static void on_success_submit() {
    show_alert_info("Your feedback submitted");
}

void FeedbackDialog::button_selected(tbx::ButtonSelectedEvent &event) {
    std::string message = tbx::TextArea(win.gadget(0x1)).text();
    can_delete_on_hide = false;
    win.hide();
    g_app_data_model.upload_feedback(message, "P", true, true, on_success_submit);
    delete this;
}

