//
// Created by lenz on 4/16/20.
//

#ifndef ROCHAT_CREATECHATDIALOG_H
#define ROCHAT_CREATECHATDIALOG_H


#include <list>
#include <vector>
#include <functional>
#include "../utils.h"
#include "../global.h"
#include <tbx/window.h>
#include <tbx/actionbutton.h>
#include <tbx/writablefield.h>
#include <tbx/radiobutton.h>
#include <tbx/objectdelete.h>
#include <tbx/redrawlistener.h>
#include <tbx/mouseclicklistener.h>
#include <tbx/buttonselectedlistener.h>
#include <tbx/scrollrequestlistener.h>
#include <tbx/hasbeenhiddenlistener.h>
#include <tbx/textchangedlistener.h>
#include <tbx/keylistener.h>
#include <tbx/command.h>
#include <cloverleaf/Logger.h>
#include "../model/AppDataModel.h"
#include "RecipinetList.h"

class CreateChatDialog :
        public tbx::RadioButtonStateChangedListener,
        public tbx::ButtonSelectedListener,
        public tbx::HasBeenHiddenListener,
        public tbx::TextChangedListener,
        public tbx::KeyListener,
        public ViewItemClickListener<RecipientListViewItem>,
        public ViewSelectionChangedListener<RecipientListViewItem> {
public:
    bool title_edited;
    char messenger;
    tbx::Window win;
    std::vector<Recipient*> loaded_recipients;
    std::vector<Recipient*> selected_recipients;
    RecipientListView member_list_view;

    CreateChatDialog(char messenger);
    ~CreateChatDialog() override;

    void radiobutton_state_changed(const tbx::RadioButtonStateChangedEvent &state_changed_event) override;
    void button_selected(tbx::ButtonSelectedEvent &event) override;
    void has_been_hidden(const tbx::EventInfo &event_info) override;
    void item_clicked(ViewItemClickEvent<RecipientListViewItem> &event) override;
    void text_changed(tbx::TextChangedEvent &event) override;

    void key(tbx::KeyEvent &event) override;

    void selection_changed(RecipientListViewItem* item) override;

    void on_load_members(const std::vector<MemberDataPtr>& _members);
};


#endif //ROCHAT_CREATECHATDIALOG_H
