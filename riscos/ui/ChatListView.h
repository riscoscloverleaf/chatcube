//
// Created by lenz on 3/10/20.
//

#ifndef ROCHAT_CHATLISTVIEW_H
#define ROCHAT_CHATLISTVIEW_H

#include <list>
#include <functional>
#include <tbx/window.h>
#include <tbx/actionbutton.h>
#include <tbx/writablefield.h>
#include <tbx/optionbutton.h>
#include <tbx/objectdelete.h>
#include <tbx/redrawlistener.h>
#include <tbx/mouseclicklistener.h>
#include <tbx/scrollrequestlistener.h>
#include <tbx/command.h>
#include "../model/AppDataModel.h"
#include "cloverleaf/CLGraphics.h"
#include "cloverleaf/BaseViews.h"
#include "cloverleaf/Logger.h"

class ChatListViewItem : public ListViewItem<ChatDataPtr> {
public:
    ChatListViewItem() : ListViewItem() {};
    ChatListViewItem(ChatDataPtr _value) : ListViewItem(_value) {};

    ChatListViewItem* get_next() { return (ChatListViewItem*) next; }
    ChatListViewItem* get_prev() { return (ChatListViewItem*) prev; }

    void paint(CLGraphics &g, int idx);
    int get_height(const tbx::Size& visible_size) override { return 130; }
};


class ChatsListView : public BaseView<ChatListViewItem, ChatDataPtr>, public ListViewMixin<ChatsListView>,
                      public tbx::ScrollRequestListener {
public:
    ChatsListView() : BaseView("MemberList") {
        win.add_scroll_request_listener(this);
    };

    tbx::BBox recalc_layout(const tbx::BBox& visible_bounds, const tbx::BBox& current_extent) override;

    void paint(const tbx::BBox &redraw_work_area, const tbx::VisibleArea &visible_area) override;

    void scroll_request(const tbx::ScrollRequestEvent &event) override;
};


typedef ViewItemClickEvent<ChatListViewItem> ChatListItemClickEvent;
typedef ViewItemClickListener<ChatListViewItem> ChatListItemClickListener;

#endif //ROCHAT_CHATLISTVIEW_H
