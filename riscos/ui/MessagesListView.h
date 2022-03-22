//
// Created by lenz on 3/12/20.
//

#ifndef ROCHAT_MESSAGESLISTVIEW_H
#define ROCHAT_MESSAGESLISTVIEW_H

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
#include <tbx/openwindowlistener.h>
#include <tbx/command.h>
#include "../model/AppDataModel.h"
#include "cloverleaf/CLGraphics.h"
#include "cloverleaf/BaseViews.h"
#define CLICKABLE_AVATAR 1
#define CLICKABLE_ATTACHMENT 2
#define CLICKABLE_ENTITY 3

class MessagesListView;

struct ClickableMessagePart {
    ClickableMessagePart(int type, const tbx::BBox &bbox, const TextEntity &entity) :
        type(type), bbox(bbox), entity(entity) {}

    int type;
    tbx::BBox bbox;
    const TextEntity entity;
};

class StyledSpan {
public:
    StyledSpan(const CLTextStyle& _style, const CLSpan& _span, const TextEntity _entity) :
            style(_style), span(_span), entity(_entity) {};
    const CLSpan span;
    const CLTextStyle style;
    const TextEntity entity;
};


class MessageListViewItem : public ListViewItem<MessageDataPtr> {
private:
    void _append_to_spans(const std::list<CLSpan> &appended_spans, const CLTextStyle &style, const TextEntity& ent, int &x);
    std::vector<TextEntity> _make_search_marks_entities();
    int content_text_height = 0;
public:
    std::list<ClickableMessagePart> clickable_parts;
    std::list<StyledSpan> spans;

    MessageListViewItem* get_next() { return (MessageListViewItem*) next; }
    MessageListViewItem* get_prev() { return (MessageListViewItem*) prev; }
    MessagesListView *view;

    MessageListViewItem() : ListViewItem() {};
    MessageListViewItem(MessageDataPtr _value) : ListViewItem(_value) {};
//    static int chat_font_width();
//    static int chat_font_height();
    int text_box_width(int visible_width);
    void paint(CLGraphics &g, int idx);
    int get_height(const tbx::Size& visible_size) override;

//    void split_message_to_parts(int boxwidth);
    void make_styled_spans(int boxwidth);

};


class MessagesListView : public BaseView<MessageListViewItem, MessageDataPtr>, public ListViewMixin<MessagesListView>
{
private:
    friend class MessageListViewItem;
    int64_t _last_seen_message_id;
    ChatDataPtr _chat = nullptr;
    bool _is_first_load;
public:
    tbx::BBox clickable_logo_bbox;
    std::set<MemberDataPtr> waiting_for_avatar_members;
    std::string search_substring = "";
    int search_filter = 0;
    int64_t search_found_current_id = 0;

    MessagesListView();

    tbx::BBox recalc_layout(const tbx::BBox& visible_bounds, const tbx::BBox& current_extent) override;
    void paint(const tbx::BBox& redraw_work_area, const tbx::VisibleArea &visible_area) override ;

    void post_add_item(MessageListViewItem& item, bool initial_load) override;
    void post_change_item(MessageListViewItem &item) override;

    void on_member_changed(const AppEvents::MemberChanged& ev);

    void mouse_click(tbx::MouseClickEvent &event) override;

    void reload_messages(const ChatDataPtr chat, bool is_first_load);
    inline ChatDataPtr get_chat() { return  _chat; };
    void search_set_query(const std::string& query, int filter);
    void search_show_found(MessageDataPtr msg);
    bool is_scrolled_to_bottom();
    bool is_scrolled_to_top();
};


#endif //ROCHAT_MESSAGESLISTVIEW_H
