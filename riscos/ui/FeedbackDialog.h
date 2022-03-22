//
// Created by lenz on 4/16/21.
//

#ifndef ROCHAT_FEEDBACKDIALOG_H
#define ROCHAT_FEEDBACKDIALOG_H

#include <tbx/window.h>
#include <tbx/actionbutton.h>
#include <tbx/objectdelete.h>
#include <tbx/hasbeenhiddenlistener.h>
#include <tbx/buttonselectedlistener.h>
#include <tbx/textarea.h>

class FeedbackDialog : public tbx::ButtonSelectedListener,
                       public tbx::HasBeenHiddenListener
{
    bool can_delete_on_hide = true;
public:
    tbx::Window win;

    FeedbackDialog();
    ~FeedbackDialog();

    static void open();
    void button_selected(tbx::ButtonSelectedEvent &event) override;
    void has_been_hidden(const tbx::EventInfo &event_info) override;

};


#endif //ROCHAT_FEEDBACKDIALOG_H
