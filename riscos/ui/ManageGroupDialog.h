//
// Created by lenz on 8/28/20.
//

#ifndef ROCHAT_MANAGEGROUPDIALOG_H
#define ROCHAT_MANAGEGROUPDIALOG_H

#include <list>
#include <vector>
#include <functional>
#include "../utils.h"
#include "../global.h"
#include <tbx/window.h>
#include <tbx/actionbutton.h>
#include <tbx/writablefield.h>
#include <tbx/objectdelete.h>
#include <tbx/redrawlistener.h>
#include <tbx/buttonselectedlistener.h>
#include <tbx/mouseclicklistener.h>
#include <tbx/scrollrequestlistener.h>
#include <tbx/hasbeenhiddenlistener.h>
#include <tbx/command.h>
#include <cloverleaf/Logger.h>
#include "../model/AppDataModel.h"
#include "ChatMembersList.h"

class ManageGroupDialog :
        public tbx::ButtonSelectedListener,
        public tbx::HasBeenHiddenListener
{
public:
    ChatDataPtr chat;
    tbx::Window win;
    std::vector<ChatMemberDataPtr> loaded_chat_members;
    ChatMembersListView member_list_view;

    ManageGroupDialog(const ChatDataPtr &chat);
    ~ManageGroupDialog() override;

    void has_been_hidden(const tbx::EventInfo &event_info) override;

    void button_selected(tbx::ButtonSelectedEvent &event) override;

    void on_load_chat_members(const std::vector<ChatMemberDataPtr>& _chat_members);
};


#endif //ROCHAT_MANAGEGROUPDIALOG_H
