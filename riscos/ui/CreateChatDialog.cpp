//
// Created by lenz on 4/16/20.
//

#include "cloverleaf/CLUtf8.h"
#include "CreateChatDialog.h"
#include "cloverleaf/utf8.h"

#define MAX_TITLE_LENGTH       46

static CreateChatDialog* _instance = nullptr;

CreateChatDialog::CreateChatDialog(char _messenger) :
        win(tbx::Window("CrtChatWin")),
        title_edited(false),
        messenger(_messenger)
{
    _instance = this;
    switch(messenger) {
        case MESSENGER_CHATCUBE:
            win.title("Create new Chatcube chat");
            break;
        case MESSENGER_TELEGRAM:
            win.title("Create new Telegram chat");
            break;
    }

    win.add_has_been_hidden_listener(this);
    member_list_view.add_click_listener(this);
    member_list_view.add_selection_changed_listener(this);
    tbx::RadioButton(win.gadget(0)).add_state_changed_listener(this);
    tbx::RadioButton(win.gadget(1)).add_state_changed_listener(this);
    tbx::WritableField(win.gadget(3)).add_text_changed_listener(this);
    tbx::WritableField(win.gadget(8)).add_key_listener(this);
    tbx::ActionButton(win.gadget(4)).add_selected_listener(this);

    auto setup_subwindow = [this]() {
        tbx::WindowState win_state;
        win.get_state(win_state);
        tbx::BBox vis_area_bbox = win_state.visible_area().bounds();
//        win.extent(tbx::BBox(0, -win_state.visible_area().bounds().height(), win_state.visible_area().bounds().width(), 0));
//        win.get_state(win_state);
//        win.force_redraw(win.extent());


        tbx::ShowSubWindowSpec members_subwindow;
        members_subwindow.visible_area = win_state.visible_area();

        members_subwindow.visible_area.bounds().min.x = vis_area_bbox.min.x + 36;
        members_subwindow.visible_area.bounds().max.y = vis_area_bbox.max.y - 286;
        members_subwindow.visible_area.bounds().max.x = vis_area_bbox.max.x - 80;
        members_subwindow.visible_area.bounds().min.y = vis_area_bbox.min.y + 120;

        members_subwindow.wimp_window = -1;
        members_subwindow.wimp_parent = win.window_handle();
        members_subwindow.flags = 0
                                  | tbx::ALIGN_LEFT_VISIBLE_LEFT
                                  | tbx::ALIGN_BOTTOM_VISIBLE_BOTTOM
                                  | tbx::ALIGN_RIGHT_VISIBLE_RIGHT
                                  | tbx::ALIGN_TOP_VISIBLE_TOP
                ;
        member_list_view.win.extent(tbx::BBox(0, -members_subwindow.visible_area.bounds().height(), members_subwindow.visible_area.bounds().width(), 0));
        member_list_view.win.show_as_subwindow(members_subwindow);
    };

    win.show_centered();

    tbx::WindowState win_state;
    win.get_state(win_state);
    win.extent(tbx::BBox(0, -win_state.visible_area().bounds().height(), win_state.visible_area().bounds().width(), 0));
    g_idle_task.run_at_next_idle(setup_subwindow);

    g_app_data_model.get_contacts(messenger, "", std::bind(&CreateChatDialog::on_load_members, this, std::placeholders::_1));
}

CreateChatDialog::~CreateChatDialog() {
    Logger::debug("CreateChatDialog::~CreateChatDialog %p win=%p", this, win.handle());
    win.delete_object();
    selected_recipients.clear();
    for(Recipient* r : loaded_recipients) {
        delete r;
    }
    Logger::debug("CreateChatDialog::~CreateChatDialog finished");
    _instance = nullptr;
}

void CreateChatDialog::on_load_members(const std::vector<MemberDataPtr>& _members) {
    if (!_instance) {
        return;
    }
    std::sort(_members.begin(), _members.end(), [](const MemberDataPtr &a, const MemberDataPtr &b) -> bool {
        if (utf8casecmp(a->displayname.c_str(), b->displayname.c_str()) < 0) {
            return true;
        }
    });

    for(Recipient* r : loaded_recipients) {
        delete r;
    }
    loaded_recipients.clear();
    loaded_recipients.reserve(_members.size());
    for(auto &_mem : _members) {
        loaded_recipients.push_back(new Recipient(_mem, nullptr));
    }

    selected_recipients.clear();
    member_list_view.assign_recipients(loaded_recipients);
}

void CreateChatDialog::radiobutton_state_changed(const tbx::RadioButtonStateChangedEvent &state_changed_event) {
    Logger::debug("CreateChatDialog::radiobutton_state_changed %d", state_changed_event.on() && state_changed_event.id_block().self_component().id() == 1);
    if (state_changed_event.on() && state_changed_event.id_block().self_component().id() == 1) {
        member_list_view.set_multiselect(true);
        tbx::WritableField(win.gadget(3)).fade(false);
        if (!selected_recipients.empty() && !title_edited) {
            tbx::WritableField(win.gadget(3)).text(utf8_to_riscos_local(selected_recipients[0]->member->displayname));
        }
    } else {
        member_list_view.set_multiselect(false);
        if (!title_edited) {
            tbx::WritableField(win.gadget(3)).text("");
        }
        tbx::WritableField(win.gadget(3)).fade(true);
    }
}

void CreateChatDialog::button_selected(tbx::ButtonSelectedEvent &event) {
    Logger::debug("CreateChatDialog::button_selected");
    if (selected_recipients.empty()) {
        show_alert_error("Please select a contact to create chat!");
        return;
    } else {
        auto on_success = [this](const ChatDataPtr chat) {
            g_app_data_model.open_chat(chat);
            win.hide();
        };

        if (selected_recipients.size() == 1) {
            Logger::debug("CreateChatDialog::button_selected create chat: %s", selected_recipients[0]->member->displayname.c_str());
            g_app_data_model.create_private_chat(selected_recipients[0]->member, on_success);
        } else {
            std::string title = tbx::WritableField(win.gadget(3)).text();
            if (title == "") {
                show_alert_error("Please specify the title of group chat!");
                return;
            }
            std::vector<MemberDataPtr> members;
            for(auto item : selected_recipients) {
                members.push_back(item->member);
                Logger::debug("CreateChatDialog::button_selected create group chat: %s", item->member->displayname.c_str());
            }
            g_app_data_model.create_group_chat(members, title, on_success);
        }
    }
}

void CreateChatDialog::has_been_hidden(const tbx::EventInfo &event_info) {
    Logger::debug("CreateChatDialog::has_been_hidden");
    auto delete_this = [this]() {
        delete this;
    };
    g_idle_task.run_at_next_idle(delete_this);
}

void CreateChatDialog::item_clicked(ViewItemClickEvent<RecipientListViewItem> &event) {
    member_list_view.set_selected_item(event.view_item().value, !event.view_item().selected);
}

void CreateChatDialog::selection_changed(RecipientListViewItem* selected_item) {
    bool found = false;
    for(auto it = selected_recipients.begin(); it != selected_recipients.end();) {
        if ((*it) == selected_item->value) {
            if (selected_item->selected) {
                found = true;
            } else {
                selected_recipients.erase(it);
            }
            break;
        }
        it++;
    }
    if (selected_item->selected && !found) {
        selected_recipients.push_back(selected_item->value);
    }

    if (!title_edited && tbx::RadioButton(win.gadget(1)).on()) {
        std::string new_title;
        int i = 0;
        for(auto item: selected_recipients) {
            if (i > 0) {
                new_title += ", ";
            }
            new_title += item->member->displayname;
            i++;
        }
        new_title = utf8_to_riscos_local(new_title);
        if (new_title.size() > MAX_TITLE_LENGTH) {
            new_title = new_title.substr(0, MAX_TITLE_LENGTH) + "...";
        }
        tbx::WritableField(win.gadget(3)).text(new_title);
    }
}

void CreateChatDialog::text_changed(tbx::TextChangedEvent &event) {
    if (!_instance) {
        return;
    }
    Logger::debug("CreateChatDialog::text_changed");
    title_edited = true;
}

void CreateChatDialog::key(tbx::KeyEvent &event) {
    if (!_instance) {
        return;
    }
    std::string search = riscos_local_to_utf8(tbx::WritableField(win.gadget(8)).text());
    std::vector<Recipient*> recipients;
    for(auto r : loaded_recipients) {
        if (search.empty() || utf8casestr(r->member->displayname.c_str(), search.c_str()) != NULL) {
            recipients.push_back(r);
        }
    }
    member_list_view.assign_recipients(recipients);
    member_list_view.set_selected_items(selected_recipients);
}

