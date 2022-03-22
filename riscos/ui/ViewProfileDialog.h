//
// Created by lenz on 11/16/20.
//

#ifndef ROCHAT_VIEWPROFILEDIALOG_H
#define ROCHAT_VIEWPROFILEDIALOG_H

#include <tbx/hasbeenhiddenlistener.h>
#include <tbx/mouseclicklistener.h>
#include <tbx/window.h>
#include <tbx/component.h>
#include "../model/AppDataModel.h"

class ViewProfileDialog;
static ViewProfileDialog *_instance = nullptr;

class ViewProfileDialog : public tbx::HasBeenHiddenListener,
    public tbx::MouseClickListener
{
private:
    MemberDataPtr member;
    tbx::Window win;
    bool opening_avatar = false;
    void open_avatar();
public:
    ViewProfileDialog(MemberDataPtr mem);
    ~ViewProfileDialog() override;

    void update(MemberDataPtr mem);

    static void open(MemberDataPtr mem);

    void has_been_hidden(const tbx::EventInfo &event_info) override;
    void mouse_click(tbx::MouseClickEvent &event) override;
};


#endif //ROCHAT_VIEWPROFILEDIALOG_H
