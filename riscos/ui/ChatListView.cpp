//
// Created by lenz on 3/10/20.
//

#include "ChatListView.h"
#include "FontStyles.h"
#include "AppColours.h"
#include <cloverleaf/CLImageCache.h>

static const int INNER_PADDING          =10;
static const int TEXT_FONT_LINEHEIGHT   =40;
static const int AVATAR_SIZE            =112;
static const int AVATAR_MARGIN          =16;
static const int UNREAD_COUNT_MARGIN    =8;
static const int UNREAD_COUNT_PADDING   =8;

void ChatListViewItem::paint(CLGraphics &g, int idx) {
//    Logger::debug("ChatListViewItem::paint %s %d onl:%d", value->title.c_str(), idx, value->is_online());
    tbx::Colour color;
    tbx::Colour bgcolor;
//    Logger::debug("paint it=%s, idx=%d offset %d:%d", value->title.c_str(), idx, g.offset_x(), g.offset_y());
    if (selected) {
        color = SELECTED_TEXT_COLOR;
        bgcolor = SELECTED_COLOR;
    } else {
        color = TEXT_COLOR;
        bgcolor = BG_COLOR;
    }
    g.foreground(bgcolor);
    g.fill_rectangle(0,- _bounds.height(), _bounds.width(), -2);

    if (!value->is_private() && value->pic_small.empty()) {
        g.draw_image_circled(AVATAR_MARGIN, -(INNER_PADDING + AVATAR_SIZE), AVATAR_SIZE / 2, "<ChatCube$Dir>.icons.groupchat", bgcolor);
    } else {
        if (!value->pic_small_cached.empty()) {
            g.draw_image_circled(AVATAR_MARGIN, -(INNER_PADDING + AVATAR_SIZE), AVATAR_SIZE / 2, value->pic_small_cached, bgcolor);
        }
    }

    int put_x = AVATAR_MARGIN * 2 + AVATAR_SIZE;
    int width = _bounds.width() - (put_x + AVATAR_MARGIN);
    std::string chatname = "  " + value->title;
    g.draw_text_cut_to_width(put_x, -(TEXT_FONT_LINEHEIGHT + INNER_PADDING), width,
                             chatname, CHAT_NAME_FONT_STYLE,
                             color, bgcolor);

    if (!value->action_line.empty()) {
        g.draw_text_cut_to_width(put_x, -(TEXT_FONT_LINEHEIGHT * 2 + INNER_PADDING), width,
                                 value->action_line,
                                 CHAT_ACTION_FONT_STYLE,
                                 color, bgcolor);
    } else if (value->last_message) {
//        Logger::debug("value->last_message->text [%s]", value->last_message->text.c_str());
        if (value->last_message->text.empty()) {
            int req_h = TEXT_FONT_LINEHEIGHT + 8;
            int req_w;
            if (value->last_message->att_image && !value->last_message->att_image->thumb_url_cached.empty()) {
//            Logger::debug("value->last_message->att_image [%s]", value->last_message->att_image->thumb_url_cached.c_str());
                req_w = req_h * value->last_message->att_image->thumb_width /
                            value->last_message->att_image->thumb_height;
                g.draw_cached_image_scaled(value->last_message->att_image->thumb_url_cached,
                                           put_x, -(TEXT_FONT_LINEHEIGHT + INNER_PADDING + req_h + 4), bgcolor,
                                           req_w, req_h);
            } else if (value->last_message->att_file && !value->last_message->att_file->thumb_url_cached.empty()) {
                req_w = req_h * value->last_message->att_file->thumb_width /
                            value->last_message->att_file->thumb_height;
                g.draw_cached_image_scaled(value->last_message->att_file->thumb_url_cached,
                                           put_x, -(TEXT_FONT_LINEHEIGHT + INNER_PADDING + req_h + 4), bgcolor,
                                           req_w, req_h);
            }
        } else {
            g.draw_text_cut_to_width(put_x, -(TEXT_FONT_LINEHEIGHT * 2 + INNER_PADDING), width,
                                     value->last_message->text,
                                     CHAT_MESSAGE_FONT_STYLE,
                                     color, bgcolor);
        }
    }

    if (value->unread_count) {
        std::string unread_count_str = to_string(value->unread_count);
        int txt_square;
        int txt_width = CLGraphics::get_text_width(unread_count_str, CHAT_NAME_FONT_STYLE);
        if (txt_width < CHAT_NAME_FONT_STYLE.get_font_height()) {
            txt_square = CHAT_NAME_FONT_STYLE.get_font_height();
        } else {
            txt_square = txt_width;
        }
        int radius = txt_square / 2 + UNREAD_COUNT_PADDING;
        int unread_x = _bounds.width() - radius - UNREAD_COUNT_MARGIN * 2;
        int unread_y = -(radius + INNER_PADDING);
        g.foreground(UNREAD_COUNT_BGCOLOR);
        g.fill_circle(unread_x, unread_y, radius);
//        Logger::debug("unread_x = %d unread_y=%d", unread_x, unread_y);
        unread_x = unread_x - txt_width/2;
        unread_y = unread_y - 10;

        g.draw_text(unread_x, unread_y, unread_count_str, CHAT_NAME_FONT_STYLE,
                    UNREAD_COUNT_COLOR, UNREAD_COUNT_BGCOLOR);
    }

    if (value->is_private()) {
        if (value->is_online()) {
            if (value->is_member_active()) {
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

    CLImage *messenger_logo;
    if (value->messenger() == 'T') {
        messenger_logo = load_cached_image("<ChatCube$Dir>.icons.small-tg");
    } else {
        messenger_logo = load_cached_image("<ChatCube$Dir>.icons.small-cq");
    }
    if (messenger_logo) {
        g.draw_image(*messenger_logo, AVATAR_MARGIN + AVATAR_SIZE, -(INNER_PADDING + 10 + messenger_logo->height()), color);
    }
}

tbx::BBox ChatsListView::recalc_layout(const tbx::BBox &visible_bounds, const tbx::BBox& current_extent) {
    return ListViewMixin::recalc_layout(visible_bounds, current_extent);
}

void ChatsListView::scroll_request(const tbx::ScrollRequestEvent &event) {
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

void ChatsListView::paint(const tbx::BBox &redraw_work_area, const tbx::VisibleArea &visible_area) {
//    Logger::debug("ChatsListView::paint content_height=%d -visible_area.bounds().height()=%d", content_height, -visible_area.bounds().height());
    if (content_height > -visible_area.bounds().height()) {
        CLGraphics g(visible_area);
        g.foreground(tbx::Colour::white);
        g.tbx::Graphics::fill_rectangle(visible_area.work(visible_area.bounds()));
    }
    BaseView::paint(redraw_work_area, visible_area);
}
