//
// Created by lenz on 8/29/20.
//
#include <cloverleaf/CLUtf8.h>
#include <cloverleaf/utf8.h>
#include "AddMembersInChatDialog.h"

#define MAX_TITLE_LENGTH       46
static AddMembersInChatDialog* _instance = nullptr;
AddMembersInChatDialog::AddMembersInChatDialog(const ChatDataPtr &chat) :
    chat(chat),
    win(tbx::Window("UsersDialog"))
{
    _instance = this;
    win.title("Add members to: "+utf8_to_riscos_local(chat->title));
    win.add_has_been_hidden_listener(this);
    member_list_view.add_click_listener(this);
    member_list_view.set_multiselect(true);
    tbx::WritableField(win.gadget(3)).add_key_listener(this);
    tbx::ActionButton(win.gadget(5)).add_selected_listener(this);

    auto setup_subwindow = [this]() {
        tbx::WindowState win_state;
        win.get_state(win_state);
        tbx::BBox vis_area_bbox = win_state.visible_area().bounds();

        tbx::ShowSubWindowSpec members_subwindow;
        members_subwindow.visible_area = win_state.visible_area();

        members_subwindow.visible_area.bounds().min.x = vis_area_bbox.min.x + 36;
        members_subwindow.visible_area.bounds().max.y = vis_area_bbox.max.y - 110;
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

    g_app_data_model.get_contacts(chat->messenger(), chat->id, std::bind(&AddMembersInChatDialog::on_load_members, this, std::placeholders::_1));
}

AddMembersInChatDialog::~AddMembersInChatDialog() {
    Logger::debug("AddMembersInChatDialog::~AddMembersInChatDialog");
    win.delete_object();
    for(Recipient* r : loaded_recipients) {
        delete r;
    }
    Logger::debug("AddMembersInChatDialog::~AddMembersInChatDialog finished");
    _instance = nullptr;
}

void AddMembersInChatDialog::on_load_members(const std::vector<MemberDataPtr> &_members) {
    if (!_instance) {
        return;
    }
    std::sort(_members.begin(), _members.end(), [](const MemberDataPtr &a, const MemberDataPtr &b) -> bool {
        if (utf8casecmp(a->displayname.c_str(), b->displayname.c_str()) < 0) {
            return true;
        }
        return false;
    });

    for(Recipient* r : loaded_recipients) {
        delete r;
    }
    loaded_recipients.clear();
    loaded_recipients.reserve(_members.size());
    for(auto &_mem : _members) {
        loaded_recipients.push_back(new Recipient(_mem, nullptr));
    }
    member_list_view.assign_recipients(loaded_recipients);
}

void AddMembersInChatDialog::has_been_hidden(const tbx::EventInfo &event_info) {
    Logger::debug("AddMembersInChatDialog::has_been_hidden");
    auto delete_this = [this]() {
        delete this;
    };
    g_idle_task.run_at_next_idle(delete_this);
}

void AddMembersInChatDialog::item_clicked(ViewItemClickEvent<RecipientListViewItem> &event) {
    member_list_view.set_selected_item(event.view_item().value, !event.view_item().selected);
}

void AddMembersInChatDialog::selection_changed(RecipientListViewItem* selected_item) {
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
}

void AddMembersInChatDialog::button_selected(tbx::ButtonSelectedEvent &event) {
    if (!_instance) {
        return;
    }
    Logger::debug("AddMembersInChatDialog::button_selected");
    if (selected_recipients.empty()) {
        show_alert_error("Please select a contact to add to chat!");
        return;
    } else {
        auto on_success = [this]() {
            g_app_data_model.open_chat(chat);
            win.hide();
        };

        std::vector<std::string> member_ids;
        for(auto item : selected_recipients) {
            member_ids.push_back(item->member->id);
        }
        g_app_data_model.add_members_to_chat(chat->id, member_ids, on_success);
    }
}

void AddMembersInChatDialog::key(tbx::KeyEvent &event) {
    if (!_instance) {
        return;
    }
    std::string search = riscos_local_to_utf8(tbx::WritableField(win.gadget(3)).text());
    std::vector<Recipient*> recipients;
    for(auto &r : loaded_recipients) {
        if (search.empty() || utf8casestr(r->member->displayname.c_str(), search.c_str()) != NULL) {
            recipients.push_back(r);
        }
    }
    member_list_view.assign_recipients(recipients);
    member_list_view.set_selected_items(selected_recipients);
}

