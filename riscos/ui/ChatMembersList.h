//
// Created by lenz on 8/29/20.
//

#ifndef ROCHAT_CHATMEMBERSLIST_H
#define ROCHAT_CHATMEMBERSLIST_H

#include <tbx/redrawlistener.h>
#include <tbx/mouseclicklistener.h>
#include <tbx/scrollrequestlistener.h>
#include <tbx/command.h>
#include <cloverleaf/BaseViews.h>
#include <cloverleaf/CLGraphics.h>
#include <cloverleaf/Logger.h>
#include "../model/AppDataModel.h"

class ChatMemberListViewItem : public ListViewItem<ChatMemberDataPtr> {
public:
    ChatMemberListViewItem() : ListViewItem() {};
    ChatMemberListViewItem(ChatMemberDataPtr _value) : ListViewItem(_value) {};

    ChatMemberListViewItem* get_next() { return reinterpret_cast<ChatMemberListViewItem*>(next); }
    ChatMemberListViewItem* get_prev() { return reinterpret_cast<ChatMemberListViewItem*>(prev); }

    void paint(CLGraphics &g, int idx);
    int get_height(const tbx::Size& visible_size) override { return 130; }
};

class ChatMembersListView : public BaseView<ChatMemberListViewItem, ChatMemberDataPtr>,
                          public ListViewMixin<ChatMembersListView>, public tbx::ScrollRequestListener {
private:
    int member_changed_listener_id;
    std::vector<ChatMemberDataPtr> chat_members;

public:
    ChatMembersListView();
    ~ChatMembersListView() override ;

    tbx::BBox recalc_layout(const tbx::BBox& visible_bounds, const tbx::BBox& current_extent) override;
    void paint(const tbx::BBox &redraw_work_area, const tbx::VisibleArea &visible_area) override;

    void scroll_request(const tbx::ScrollRequestEvent &event) override;

    void on_member_changed(const AppEvents::MemberChanged& ev);

    void assign_chat_members(const std::vector<ChatMemberDataPtr>& new_chat_members);
};


#endif //ROCHAT_CHATMEMBERSLIST_H
