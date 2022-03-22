//
// Created by lenz on 5/26/20.
//

#include <tbx/displayfield.h>
#include "cloverleaf/CLUtf8.h"
#include "cloverleaf/utf8.h"
#include "ForwardMessageDialog.h"

#define MAX_TITLE_LENGTH       46
static ForwardMessageDialog* _instance = nullptr;

ForwardMessageDialog::ForwardMessageDialog(MessageDataPtr _message) :
        win(tbx::Window("MembersWin")),
        message(_message)
{
    _instance = this;
    win.title("Forward message");

    win.add_has_been_hidden_listener(this);
    recipient_list_view.set_multiselect(true);
    recipient_list_view.add_click_listener(this);
    recipient_list_view.add_selection_changed_listener(this);
    tbx::WritableField(win.gadget(0xc)).add_key_listener(this);
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
        members_subwindow.visible_area.bounds().max.y = vis_area_bbox.max.y - 248;
        members_subwindow.visible_area.bounds().max.x = vis_area_bbox.max.x - 80;
        members_subwindow.visible_area.bounds().min.y = vis_area_bbox.min.y + 124;

        members_subwindow.wimp_window = -1;
        members_subwindow.wimp_parent = win.window_handle();
        members_subwindow.flags = 0
                                  | tbx::ALIGN_LEFT_VISIBLE_LEFT
                                  | tbx::ALIGN_BOTTOM_VISIBLE_BOTTOM
                                  | tbx::ALIGN_RIGHT_VISIBLE_RIGHT
                                  | tbx::ALIGN_TOP_VISIBLE_TOP
                ;
        recipient_list_view.win.extent(tbx::BBox(0, -members_subwindow.visible_area.bounds().height(), members_subwindow.visible_area.bounds().width(), 0));
        recipient_list_view.win.show_as_subwindow(members_subwindow);
        load_chats();
    };

    win.show_centered();

    tbx::WindowState win_state;
    win.get_state(win_state);
    win.extent(tbx::BBox(0, -win_state.visible_area().bounds().height(), win_state.visible_area().bounds().width(), 0));
    g_idle_task.run_at_next_idle(setup_subwindow);
}

ForwardMessageDialog::~ForwardMessageDialog() {
    Logger::debug("ForwardMessageDialog::~ForwardMessageDialog");
    win.delete_object();
    selected_recipients.clear();
    for(Recipient* r : loaded_recipients) {
        delete r;
    }
    Logger::debug("ForwardMessageDialog::~ForwardMessageDialog finished");
    _instance = nullptr;
}

void ForwardMessageDialog::load_chats() {
    if (!_instance) {
        return;
    }
    auto message_chat = message->get_chat();
    auto chats_list = g_app_data_model.get_chats_list();
//    std::vector<Recipient*> recipients;
    for(Recipient* r : loaded_recipients) {
        delete r;
    }
    loaded_recipients.clear();
    loaded_recipients.reserve(chats_list.size());

    for(auto &_chat : g_app_data_model.get_chats_list()) {
        if (!_chat->title.empty() && _chat != message_chat && _chat->my_status <= CHAT_MEMBER_STATUS_NORMAL) {
            loaded_recipients.push_back(new Recipient(nullptr, _chat));
        }
    }
    selected_recipients.clear();

    MemberDataPtr me = g_app_data_model.get_my_member_data();
    std::sort(loaded_recipients.begin(), loaded_recipients.end(), [](const Recipient *a, const Recipient *b) -> bool {
        MessageDataPtr &a_msg = a->chat->last_message;
        MessageDataPtr &b_msg = b->chat->last_message;
        bool has_a = (a_msg != nullptr && a_msg->is_outgoing());
        bool has_b = (b_msg != nullptr && b_msg->is_outgoing());
        if (has_a && has_b) {
            return (a_msg->sendtime > b_msg->sendtime);
        }
        if (has_a) {
            return true;
        }
        if (has_b) {
            return false;
        }
        return (utf8casecmp(a->chat->title.c_str(), b->chat->title.c_str()) < 0);
    });
    recipient_list_view.assign_recipients(loaded_recipients);
}

void ForwardMessageDialog::button_selected(tbx::ButtonSelectedEvent &event) {
    Logger::debug("ForwardMessageDialog::button_selected");
    if (selected_recipients.empty()) {
        show_alert_error("Please select at least one member to forward message!");
        return;
    } else {
        auto on_success = [this]() {
            show_alert_info("Message forwarded successfully.");
            win.hide();
        };

        std::vector<ChatDataPtr> chats;
        for(auto item : selected_recipients) {
            chats.push_back(item->chat);
        }
        g_app_data_model.forward_message(chats, message, on_success);
    }
}

void ForwardMessageDialog::has_been_hidden(const tbx::EventInfo &event_info) {
    Logger::debug("ForwardMessageDialog::has_been_hidden");
    auto delete_this = [this]() {
        delete this;
    };
    g_idle_task.run_at_next_idle(delete_this);
}

void ForwardMessageDialog::item_clicked(ViewItemClickEvent<RecipientListViewItem> &event) {
    recipient_list_view.set_selected_item(event.view_item().value, !event.view_item().selected);
}

void ForwardMessageDialog::selection_changed(RecipientListViewItem* selected_item) {
    bool found = false;
    Logger::debug("ForwardMessageDialog::selection_changed item %p %p", selected_item->value->member.get(), selected_item->value->chat.get());
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
    Logger::debug("ForwardMessageDialog::selection_changed1 item %s", selected_item->value->chat->title.c_str());
    if (selected_item->selected && !found) {
        selected_recipients.push_back(selected_item->value);
    }
    Logger::debug("ForwardMessageDialog::selection_changed2 item %s", selected_item->value->chat->title.c_str());
    std::string new_title;
    int i = 0;
    for(auto item: selected_recipients) {
        if (i > 0) {
            new_title += ", ";
        }
        new_title += item->chat->title;
        i++;
    }
    Logger::debug("ForwardMessageDialog::selection_changed3 item %s", selected_item->value->chat->title.c_str());
    new_title = utf8_to_riscos_local(new_title);
    if (new_title.size() > MAX_TITLE_LENGTH) {
        new_title = new_title.substr(0, MAX_TITLE_LENGTH) + "...";
    }
    tbx::DisplayField(win.gadget(0xa)).text(new_title);
    tbx::DisplayField(win.gadget(0x9)).text(to_string((long)selected_recipients.size()));
}

void ForwardMessageDialog::key(tbx::KeyEvent &event) {
    if (!_instance) {
        return;
    }
    std::string search = riscos_local_to_utf8(tbx::WritableField(win.gadget(0xc)).text());
    std::vector<Recipient*> recipients;
    for(auto r : loaded_recipients) {
        if (search.empty()
            || utf8casestr(r->chat->title.c_str(), search.c_str()) != NULL) {
            recipients.push_back(r);
        }
    }
    recipient_list_view.assign_recipients(recipients);
    recipient_list_view.set_selected_items(selected_recipients);
}
