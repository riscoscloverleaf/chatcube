//
// Created by lenz on 8/28/20.
//
#include <oslib/Toolboxtypes.h>
#include <tbx/writablefield.h>
#include <tbx/actionbutton.h>
#include <cloverleaf/CLUtf8.h>
#include "cloverleaf/utf8.h"
#include "ManageGroupDialog.h"
#include "AddMembersInChatDialog.h"

static ManageGroupDialog* _instance = nullptr;

ManageGroupDialog::ManageGroupDialog(const ChatDataPtr &chat) :
    win(tbx::Window("ManageGroup")),
    chat(chat)
{
    _instance = this;
    win.title("Manage group: "+utf8_to_riscos_local(chat->title));
    tbx::WritableField(win.gadget(2)).text(utf8_to_riscos_local(chat->title));
    tbx::ActionButton(win.gadget(3)).add_selected_listener(this);
    tbx::ActionButton(win.gadget(5)).add_selected_listener(this);
    win.add_has_been_hidden_listener(this);

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

    g_app_data_model.get_chat_members(chat->id, std::bind(&ManageGroupDialog::on_load_chat_members, this, std::placeholders::_1));
}

void ManageGroupDialog::on_load_chat_members(const std::vector<ChatMemberDataPtr> &_chat_members) {
    if (!_instance) {
        return;
    }
    std::sort(_chat_members.begin(), _chat_members.end(), [](const ChatMemberDataPtr &a, const ChatMemberDataPtr &b) -> bool {
        if (utf8casecmp(a->member->displayname.c_str(), b->member->displayname.c_str()) < 0) {
            return true;
        }
        return false;
    });

    loaded_chat_members = _chat_members;
    member_list_view.assign_chat_members(loaded_chat_members);
}

ManageGroupDialog::~ManageGroupDialog() {
    Logger::debug("ManageGroupDialog::~ManageGroupDialog");
    win.delete_object();
    _instance = nullptr;
}

void ManageGroupDialog::has_been_hidden(const tbx::EventInfo &event_info) {
    Logger::debug("ManageGroupDialog::has_been_hidden");
    auto delete_this = [this]() {
        delete this;
    };
    g_idle_task.run_at_next_idle(delete_this);
}

void ManageGroupDialog::button_selected(tbx::ButtonSelectedEvent &event) {
    if (event.id_block().self_component().id() == 5) {
        new AddMembersInChatDialog(chat);
        win.hide();
    } else {
        // set title
        std::string new_title = tbx::WritableField(win.gadget(2)).text();
        if (new_title != chat->title) {
            auto on_success = [this]() {
                this->win.hide();
            };
            g_app_data_model.set_chat_title(chat->id, new_title, on_success);
        } else {
            win.hide();
        }
    }
}

