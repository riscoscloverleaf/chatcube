//
// Created by lenz on 8/29/20.
//

#include "FontStyles.h"
#include "AppColours.h"
#include "ChatMembersList.h"

#define INNER_PADDING          10
#define TEXT_FONT_LINEHEIGHT   40
#define AVATAR_SIZE            112
#define AVATAR_MARGIN          16

void ChatMemberListViewItem::paint(CLGraphics &g, int idx) {
    tbx::Colour color;
    tbx::Colour bgcolor;
    std::string pic;
    std::string title;

    if (selected) {
        color = SELECTED_TEXT_COLOR;
        bgcolor = SELECTED_COLOR;
    } else {
        color = TEXT_COLOR;
        bgcolor = BG_COLOR;
    }
    g.foreground(bgcolor);
    g.fill_rectangle(0,- _bounds.height(), _bounds.width(), -2);

    pic = value->member->pic_small_cached;
    title = value->member->displayname;

    if (!pic.empty()) {
        g.draw_image_circled(AVATAR_MARGIN, -(INNER_PADDING + AVATAR_SIZE), AVATAR_SIZE / 2, pic, bgcolor);
    }

    int put_x = AVATAR_MARGIN * 2 + AVATAR_SIZE;
    int width = _bounds.width() - (put_x + AVATAR_MARGIN);
    g.draw_text_cut_to_width(put_x, -(TEXT_FONT_LINEHEIGHT + INNER_PADDING), width,
                             title, CHAT_NAME_FONT_STYLE,
                             color, bgcolor);

    std::string online_str;
    switch(value->status) {
        case CHAT_MEMEBER_STATUS_CREATOR:
            online_str = "(CREATOR) ";
            break;
        case CHAT_MEMBER_STATUS_ADMIN:
            online_str = "(ADMIN) ";
            break;
        case CHAT_MEMEBER_STATUS_READONLY:
            online_str = "(READ ONLY) ";
            break;
        case CHAT_MEMEBER_STATUS_BANNED:
            online_str = "(BANNED) ";
            break;
    }
    if (value->member->online) {
        online_str += "online";
    } else if (value->member->was_online) {
        char time_str[200];
        timet_to_local_time(&value->member->was_online, "%d.%m.%Y %H:%M", time_str, sizeof(time_str));
        online_str += "was online at ";
        online_str += time_str;
    }

    g.draw_text_cut_to_width(put_x, -(TEXT_FONT_LINEHEIGHT * 2 + INNER_PADDING), width,
                             online_str,
                             CHAT_MESSAGE_FONT_STYLE,
                             color, bgcolor);
    if (value->member->online) {
        g.draw_circle(AVATAR_MARGIN + AVATAR_SIZE - 24, -(INNER_PADDING + AVATAR_SIZE - 12), 14, 4, ONLINE_DOT_BGCOLOR,
                      BG_COLOR);
    } else {
        g.draw_circle(AVATAR_MARGIN + AVATAR_SIZE - 24, -(INNER_PADDING + AVATAR_SIZE - 12), 14, 4, OFFLINE_DOT_BGCOLOR,
                      BG_COLOR);
    }
}

ChatMembersListView::ChatMembersListView() : BaseView("MemberList") {
    win.add_scroll_request_listener(this);
    member_changed_listener_id = g_app_events.listen<AppEvents::MemberChanged>(std::bind(&ChatMembersListView::on_member_changed, this, std::placeholders::_1));
}

ChatMembersListView::~ChatMembersListView() {
    g_app_events.unlisten<AppEvents::MemberChanged>(member_changed_listener_id);
}

tbx::BBox ChatMembersListView::recalc_layout(const tbx::BBox &visible_bounds, const tbx::BBox& current_extent) {
    return ListViewMixin::recalc_layout(visible_bounds, current_extent);
}

void ChatMembersListView::paint(const tbx::BBox &redraw_work_area, const tbx::VisibleArea &visible_area) {
    if (content_height > -visible_area.bounds().height()) {
        CLGraphics g(visible_area);
        g.foreground(tbx::Colour::white);
        g.tbx::Graphics::fill_rectangle(visible_area.work(visible_area.bounds()));
    }
    BaseView::paint(redraw_work_area, visible_area);
}

void ChatMembersListView::scroll_request(const tbx::ScrollRequestEvent &event) {
    tbx::WindowOpenInfo open_info = event.open_info();
    switch(event.y_scroll()) {
        case tbx::ScrollRequestEvent::ScrollYDirection::PAGE_DOWN:
            open_info.visible_area().scroll().y -= (open_info.visible_area().bounds().height() - 130);
            break;
        case tbx::ScrollRequestEvent::ScrollYDirection::PAGE_UP:
            open_info.visible_area().scroll().y += (open_info.visible_area().bounds().height() - 130);
            break;
        case tbx::ScrollRequestEvent::ScrollYDirection::DOWN:
            open_info.visible_area().scroll().y -= 130;
            break;
        case tbx::ScrollRequestEvent::ScrollYDirection::UP:
            open_info.visible_area().scroll().y += 130;
            break;
    }
    win.open_window(open_info);
}

void ChatMembersListView::on_member_changed(const AppEvents::MemberChanged &ev) {
    if (ev.changes & (MEMBER_CHANGES_PIC_SMALL | MEMBER_CHANGES_PROFILE | MEMBER_CHANGES_ONLINE)) {
        auto view_item = get_first_item();
        while(view_item) {
            if (view_item->value->member == ev.mem) {
                change_view_item(view_item);
                break;
            }
            view_item = view_item->get_next();
        }
    }
}

void ChatMembersListView::assign_chat_members(const std::vector<ChatMemberDataPtr> &new_chat_members) {
    chat_members = new_chat_members;
    reload_items(chat_members.begin(), chat_members.end(), true);
}
