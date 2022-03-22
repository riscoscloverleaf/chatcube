//
// Created by lenz on 5/26/20.
//

#ifndef ROCHAT_FORWARDMESSAGEDIALOG_H
#define ROCHAT_FORWARDMESSAGEDIALOG_H

#include <list>
#include <vector>
#include <functional>
#include <tbx/window.h>
#include <tbx/actionbutton.h>
#include <tbx/objectdelete.h>
#include <tbx/mouseclicklistener.h>
#include <tbx/buttonselectedlistener.h>
#include <tbx/hasbeenhiddenlistener.h>
#include <tbx/keylistener.h>
#include <tbx/command.h>
#include <cloverleaf/Logger.h>
#include "../model/AppDataModel.h"
#include "RecipinetList.h"

class ForwardMessageDialog :
        public tbx::ButtonSelectedListener,
        public tbx::HasBeenHiddenListener,
        public tbx::KeyListener,
        public ViewItemClickListener<RecipientListViewItem>,
        public ViewSelectionChangedListener<RecipientListViewItem> {
public:
    MessageDataPtr message;
    tbx::Window win;
    std::vector<Recipient*> loaded_recipients;
    std::vector<Recipient*> selected_recipients;
    RecipientListView recipient_list_view;

    ForwardMessageDialog(MessageDataPtr message);
    ~ForwardMessageDialog() override;

    void button_selected(tbx::ButtonSelectedEvent &event) override;
    void has_been_hidden(const tbx::EventInfo &event_info) override;
    void item_clicked(ViewItemClickEvent<RecipientListViewItem> &event) override;
    void key(tbx::KeyEvent &event);

    void selection_changed(RecipientListViewItem* item) override;

    void load_chats();
};

#endif //ROCHAT_FORWARDMESSAGEDIALOG_H
