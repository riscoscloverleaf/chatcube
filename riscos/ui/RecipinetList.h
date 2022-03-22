//
// Created by lenz on 5/26/20.
//

#ifndef ROCHAT_RECIPINETLIST_H
#define ROCHAT_RECIPINETLIST_H


#include <tbx/redrawlistener.h>
#include <tbx/mouseclicklistener.h>
#include <tbx/scrollrequestlistener.h>
#include <tbx/command.h>
#include <cloverleaf/BaseViews.h>
#include <cloverleaf/CLGraphics.h>
#include <cloverleaf/Logger.h>
#include "../model/AppDataModel.h"

struct Recipient {
    MemberDataPtr member;
    ChatDataPtr chat;

    Recipient() : member(nullptr), chat(nullptr) {};
    Recipient(MemberDataPtr m, ChatDataPtr c): member(m), chat(c) {};
};

class RecipientListViewItem : public ListViewItem<Recipient*> {
public:
    RecipientListViewItem() : ListViewItem() {};
    RecipientListViewItem(Recipient* _value) : ListViewItem(_value) {};

    RecipientListViewItem* get_next() { return (RecipientListViewItem*) next; }
    RecipientListViewItem* get_prev() { return (RecipientListViewItem*) prev; }

    void paint(CLGraphics &g, int idx);
    int get_height(const tbx::Size& visible_size) override { return 130; }
};

class RecipientListView : public BaseView<RecipientListViewItem, Recipient*>,
                          public ListViewMixin<RecipientListView>, public tbx::ScrollRequestListener {
private:
    int member_changed_listener_id;
    int chat_changed_listener_id;
    std::vector<Recipient*> recipients;

public:
    RecipientListView();
    ~RecipientListView() override ;

    tbx::BBox recalc_layout(const tbx::BBox& visible_bounds, const tbx::BBox& current_extent) override;

    void assign_recipients(const std::vector<Recipient*>& new_recipients);
    void paint(const tbx::BBox &redraw_work_area, const tbx::VisibleArea &visible_area) override;

    void scroll_request(const tbx::ScrollRequestEvent &event) override;

    void on_member_changed(const AppEvents::MemberChanged& ev);
    void on_chat_changed(const AppEvents::ChatChanged& ev);
};

#endif //ROCHAT_RECIPINETLIST_H
