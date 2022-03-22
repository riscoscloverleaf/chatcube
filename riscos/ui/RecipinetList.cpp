//
// Created by lenz on 5/26/20.
//


#include "RecipinetList.h"
#include "cloverleaf/CLUtf8.h"
#include "AppColours.h"
#include "FontStyles.h"

#define INNER_PADDING          10
#define TEXT_FONT_LINEHEIGHT   40
#define AVATAR_SIZE            112
#define AVATAR_MARGIN          16

void RecipientListViewItem::paint(CLGraphics &g, int idx) {
    tbx::Colour color;
    tbx::Colour bgcolor;
    std::string pic;
    std::string title;
    MemberDataPtr member = nullptr;

    if (selected) {
        color = SELECTED_TEXT_COLOR;
        bgcolor = SELECTED_COLOR;
    } else {
        color = TEXT_COLOR;
        bgcolor = BG_COLOR;
    }
    g.foreground(bgcolor);
    g.fill_rectangle(0,- _bounds.height(), _bounds.width(), -2);

    if (value->member) {
        member = value->member;
        pic = member->pic_small_cached;
        title = member->displayname;
    } else {
        if (value->chat->is_private()) {
            member = value->chat->member;
        }
        pic = value->chat->pic_small_cached;
        title = value->chat->title;
    }

    if (!pic.empty()) {
        g.draw_image_circled(AVATAR_MARGIN, -(INNER_PADDING + AVATAR_SIZE), AVATAR_SIZE / 2, pic, bgcolor);
    }

    int put_x = AVATAR_MARGIN * 2 + AVATAR_SIZE;
    int width = _bounds.width() - (put_x + AVATAR_MARGIN);
    g.draw_text_cut_to_width(put_x, -(TEXT_FONT_LINEHEIGHT + INNER_PADDING), width,
                             title, CHAT_NAME_FONT_STYLE,
                             color, bgcolor);

    std::string online_str;
    if (member != nullptr) {
        if (member->online) {
            if (member->active) {
                online_str = "online";
            } else {
                online_str = "online inactive";
            }
        } else if (member->was_online) {
            char time_str[200];
            Logger::debug("%s value->member->was_online = %u", member->displayname.c_str(), member->was_online);
            timet_to_local_time(&member->was_online, "%d.%m.%Y %H:%M", time_str, sizeof(time_str));
            online_str = "was online at ";
            online_str += time_str;
        }
    } else {
        online_str = "group";
    }
    g.draw_text_cut_to_width(put_x, -(TEXT_FONT_LINEHEIGHT * 2 + INNER_PADDING), width,
                             online_str,
                             CHAT_MESSAGE_FONT_STYLE,
                             color, bgcolor);
    if (member) {
        if (member->online) {
            if (member->active) {
                g.draw_circle(AVATAR_MARGIN + AVATAR_SIZE - 24, -(INNER_PADDING + AVATAR_SIZE - 12), 14, 4, ONLINE_DOT_BGCOLOR,
                              BG_COLOR);
            } else {
                g.draw_circle(AVATAR_MARGIN + AVATAR_SIZE - 24, -(INNER_PADDING + AVATAR_SIZE - 12), 14, 4, INACTIVE_DOT_BGCOLOR,
                              BG_COLOR);
            }
        } else {
            g.draw_circle(AVATAR_MARGIN + AVATAR_SIZE - 24, -(INNER_PADDING + AVATAR_SIZE - 12), 14, 4, OFFLINE_DOT_BGCOLOR,
                          BG_COLOR);
        }
    }
}

RecipientListView::RecipientListView() :
BaseView("MemberList") {
    win.add_scroll_request_listener(this);
    member_changed_listener_id = g_app_events.listen<AppEvents::MemberChanged>(std::bind(&RecipientListView::on_member_changed, this, std::placeholders::_1));
    chat_changed_listener_id = g_app_events.listen<AppEvents::ChatChanged>(std::bind(&RecipientListView::on_chat_changed, this, std::placeholders::_1));
}

RecipientListView::~RecipientListView() {
    Logger::debug("RecipientListView::~RecipientListView() %p win=%p", this, win.handle());
    g_app_events.unlisten<AppEvents::MemberChanged>(member_changed_listener_id);
    g_app_events.unlisten<AppEvents::ChatChanged>(chat_changed_listener_id);
    Logger::debug("RecipientListView::~RecipientListView() finished");
}

tbx::BBox RecipientListView::recalc_layout(const tbx::BBox &visible_bounds, const tbx::BBox& current_extent) {
    return ListViewMixin::recalc_layout(visible_bounds, current_extent);
}

void RecipientListView::scroll_request(const tbx::ScrollRequestEvent &event) {
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

void RecipientListView::on_member_changed(const AppEvents::MemberChanged& ev) {
    if (ev.changes & (MEMBER_CHANGES_PIC_SMALL | MEMBER_CHANGES_PROFILE | MEMBER_CHANGES_ONLINE)) {
        auto recepient_view_item = get_first_item();
        while(recepient_view_item) {
            if (recepient_view_item->value->member == ev.mem) {
                change_view_item(recepient_view_item);
                break;
            }
            recepient_view_item = recepient_view_item->get_next();
        }
    }
}

void RecipientListView::on_chat_changed(const AppEvents::ChatChanged& ev) {
    if (ev.changes & (CHAT_CHANGES_PIC_SMALL | CHAT_CHANGES_TITLE)) {
        auto recepient_view_item = get_first_item();
        while(recepient_view_item) {
            if (recepient_view_item->value->chat == ev.chat) {
                change_view_item(recepient_view_item);
                break;
            }
            recepient_view_item = recepient_view_item->get_next();
        }
    }
}

void RecipientListView::assign_recipients(const std::vector<Recipient *>& new_recipients) {
    recipients = new_recipients;
    reload_items(recipients.begin(), recipients.end(), true);
}

void RecipientListView::paint(const tbx::BBox &redraw_work_area, const tbx::VisibleArea &visible_area) {
    if (content_height > -visible_area.bounds().height()) {
        CLGraphics g(visible_area);
        g.foreground(tbx::Colour::white);
        g.tbx::Graphics::fill_rectangle(visible_area.work(visible_area.bounds()));
    }
    BaseView::paint(redraw_work_area, visible_area);
}
