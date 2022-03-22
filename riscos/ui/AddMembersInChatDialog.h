//
// Created by lenz on 8/29/20.
//

#ifndef ROCHAT_ADDMEMBERSINCHATDIALOG_H
#define ROCHAT_ADDMEMBERSINCHATDIALOG_H

#include <list>
#include <vector>
#include <functional>
#include "../utils.h"
#include "../global.h"
#include <tbx/window.h>
#include <tbx/actionbutton.h>
#include <tbx/writablefield.h>
#include <tbx/objectdelete.h>
#include <tbx/keylistener.h>
#include <tbx/mouseclicklistener.h>
#include <tbx/buttonselectedlistener.h>
#include <tbx/scrollrequestlistener.h>
#include <tbx/hasbeenhiddenlistener.h>
#include <tbx/command.h>
#include <cloverleaf/Logger.h>
#include "../model/AppDataModel.h"
#include "RecipinetList.h"

class AddMembersInChatDialog :
        public tbx::ButtonSelectedListener,
        public tbx::HasBeenHiddenListener,
        public tbx::KeyListener,
        public ViewItemClickListener<RecipientListViewItem>,
        public ViewSelectionChangedListener<RecipientListViewItem>
{
public:
    ChatDataPtr chat;
    tbx::Window win;
    std::vector<Recipient*> loaded_recipients;
    std::vector<Recipient*> selected_recipients;
    RecipientListView member_list_view;

    AddMembersInChatDialog(const ChatDataPtr &chat);
    ~AddMembersInChatDialog() override;

    void key(tbx::KeyEvent &event) override;

    void has_been_hidden(const tbx::EventInfo &event_info) override;
    void button_selected(tbx::ButtonSelectedEvent &event) override;
    void item_clicked(ViewItemClickEvent<RecipientListViewItem> &event) override;
    void selection_changed(RecipientListViewItem* item) override;

    void on_load_members(const std::vector<MemberDataPtr>& _members);

};


#endif //ROCHAT_ADDMEMBERSINCHATDIALOG_H
