//
// Created by lenz on 3/12/20.
//
#include <tbx/sprite.h>
#include "MessagesListView.h"
#include "FontStyles.h"
#include <cloverleaf/CLImageCache.h>
#include <cloverleaf/CLUtf8.h>
#include <cloverleaf/Logger.h>

// all dimensions is os units
const int MIN_HEIGHT             =20; // min height to fit avatar
const int TEXT_BOX_WIDTH_PERCENT = 85; // percentage from full width
const int TEXT_BOX_BORDER_RADIUS =16;
const int TEXT_BOX_INNER_PADDING =14;
const int TEXT_BOX_TOP_BOTTOM_INNER_PADDING =6;
const int TEXT_BOX_SIDE_PADDING  =10;
const int TEXT_BOX_BULB_OFFSET   =34;
const int TEXT_BOX_BULB_SIZE     =20;
const int TEXT_FONT_LINEHEIGHT   =36;
const int ENTRIES_MARGIN =12;
const int OUTGOING_CHECK_WIDTH   =20;
const int OUTGOING_CHECK_HEIGHT  =22;
const int OUTGOING_CHECK_MARGIN  =16;
const int ATTACHMENT_ICON_PADDING =16;
const int AVATAR_SIZE            =112;
const int AVATAR_MARGIN          =16;
const int DATE_BOX_TOPBOTTOM_PADDING        =20;
const int DATE_LINEHEIGHT = 30;
const int REPLY_PADDING = 26;

const tbx::Colour TEXT_COLOR             = tbx::Colour::black;
const tbx::Colour TIME_COLOR             = tbx::Colour(0x9f, 0x9f, 0x9f);
const tbx::Colour INCOMING_COLOR         = tbx::Colour(0xd6, 0xea, 0xfe);
const tbx::Colour OUTGOING_COLOR         = tbx::Colour(0xd8, 0xf4, 0xd2);
const tbx::Colour SEARCH_FOUND_OUTGOING_COLOR = tbx::Colour(0xc8, 0xe4, 0xb2);
const tbx::Colour SEARCH_FOUND_INCOMING_COLOR = tbx::Colour(0xc6, 0xba, 0xfe);
const tbx::Colour MESSAGE_AUTHOR_FONT_COLOR = tbx::Colour::wimp_grey4;
const tbx::Colour MESSAGE_FORWARDED_AUTHOR_FONT_COLOR = tbx::Colour(0x44, 0xb0, 0x44);
const tbx::Colour MESSAGE_REPLIED_AUTHOR_FONT_COLOR = tbx::Colour::wimp_grey3;
const tbx::Colour DATE_COLOR             = tbx::Colour::black;
const tbx::Colour EDITED_MARK_COLOR      = tbx::Colour::wimp_red;

const CLTextStyle MESSAGE_NORMAL_STYLE = {
        .fg_color = tbx::Colour::black,
        .bg_color = tbx::Colour::no_colour,
        .font_style = MESSAGE_FONT_STYLE
};

const CLTextStyle MESSAGE_BOLD_STYLE = {
        .fg_color = tbx::Colour::black,
        .bg_color = tbx::Colour::no_colour,
        .font_style = MESSAGE_BOLD_FONT_STYLE
};

const CLTextStyle MESSAGE_ITALIC_STYLE = {
        .fg_color = tbx::Colour::black,
        .bg_color = tbx::Colour::no_colour,
        .font_style = MESSAGE_ITALIC_FONT_STYLE
};

const CLTextStyle MESSAGE_REPLIED_TEXT_STYLE = {
        .fg_color = tbx::Colour::black,
        .bg_color = tbx::Colour::no_colour,
        .font_style = CLFontStyle("DejaVuSans", rufl_WEIGHT_400, 24)
};

const CLTextStyle MESSAGE_TEXT_URL_STYLE = {
        .fg_color = tbx::Colour(0x18, 0x8a, 0xd7),
        .bg_color = tbx::Colour::no_colour,
        .font_style = CLFontStyle("DejaVuSans", rufl_WEIGHT_400, 28)
};

static CLTextStyle MESSAGE_FOUND_STYLE = {
        .fg_color = tbx::Colour::black,
        .bg_color = tbx::Colour::no_colour,
        .font_style = MESSAGE_FONT_STYLE
};

const CLTextStyle& get_entity_style(const TextEntity& ent) {
    if (ent.type & (ENTITY_SEARCH_FOUND)) {
        MESSAGE_FOUND_STYLE = MESSAGE_NORMAL_STYLE;
        switch(ent.type & ~(ENTITY_SEARCH_FOUND)) {
            case ENTITY_BOLD:
                MESSAGE_FOUND_STYLE = MESSAGE_BOLD_STYLE;
                break;
            case ENTITY_ITALIC:
                MESSAGE_FOUND_STYLE = MESSAGE_ITALIC_STYLE;
                break;
            case ENTITY_URL:
            case ENTITY_TEXT_URL:
                MESSAGE_FOUND_STYLE = MESSAGE_TEXT_URL_STYLE;
                break;
        }
        MESSAGE_FOUND_STYLE.bg_color = tbx::Colour::yellow;
//        Logger::debug("get_entity_style ent.type=%x %x %x font=%d", ent.type, (ent.type & ~(ENTITY_SEARCH_FOUND_CURRENT | ENTITY_SEARCH_FOUND)),  ~(ENTITY_SEARCH_FOUND_CURRENT | ENTITY_SEARCH_FOUND), MESSAGE_FOUND_STYLE.font_style.style);
        return MESSAGE_FOUND_STYLE;
    } else {
        switch(ent.type) {
            case ENTITY_BOLD:
                return MESSAGE_BOLD_STYLE;
            case ENTITY_ITALIC:
                return MESSAGE_ITALIC_STYLE;
            case ENTITY_URL:
            case ENTITY_TEXT_URL:
                return MESSAGE_TEXT_URL_STYLE;
            default:
                return MESSAGE_NORMAL_STYLE;
        }
    }
}

int MessageListViewItem::text_box_width(int visible_width) {
    if (value->get_chat()->type == CHAT_TYPE_CHANNEL) {
        return (visible_width - TEXT_BOX_SIDE_PADDING*2);
    } else {
        return ((visible_width * TEXT_BOX_WIDTH_PERCENT) / 100) - (AVATAR_MARGIN * 2 + AVATAR_SIZE + TEXT_BOX_BULB_SIZE);
    }
}

std::vector<TextEntity> MessageListViewItem::_make_search_marks_entities() {
    std::vector<TextEntity> entities;
    if (value->is_filter_matched(view->search_filter)) {
        if (!view->search_substring.empty()) {
            const char *haystack = value->text.c_str();
            const char *needle = view->search_substring.c_str();
            char *found = (char*) utf8casestr(haystack, needle);
            unsigned int found_start, found_len;
            if (found) {
                found_len = strlen(needle);
                while (found) {
                    found_start = found - haystack;
                    entities.emplace_back(TextEntity{
                            .type = ENTITY_SEARCH_FOUND,
                            .start = found_start,
                            .len = found_len,
                            .value = ""});
                    found = (char*) utf8casestr(found + found_len, needle);
                }
            }
        } else {
            entities.emplace_back(TextEntity{
                    .type = ENTITY_SEARCH_FOUND,
                    .start = 0,
                    .len = value->text.length(),
                    .value = ""});
        }
    }
//    Logger::debug("_make_search_marks_entities %d", entities.size());
    return entities;
}

void MessageListViewItem::_append_to_spans(const std::list<CLSpan> &appended_spans, const CLTextStyle &style, const TextEntity& ent, int &x) {
    for(auto &spn: appended_spans) {
        spans.emplace_back(StyledSpan(style, spn, ent));
        if (spn.newline) {
            x = 0;
        } else {
            x += spn.width;
        }
    }
}

void MessageListViewItem::make_styled_spans(int box_width) {
    int cur_text_pos = 0, x = 0;
    // split message text by entities to message_text_parts
    spans.clear();

    if (value->text.empty()) {
        return;
    }

    std::vector<TextEntity> entities;
    if (view->search_filter != 0 || !view->search_substring.empty()) {
        entities = merge_entities(value->text_entities, _make_search_marks_entities());
    } else {
        entities = value->text_entities;
    }
//    Logger::debug("make_styled_spans msg=%lld entities cnt=%d", value->id, entities.size());
    if (!entities.empty()) {
        for(auto &ent : entities) {
            if (cur_text_pos < ent.start) {
//                Logger::debug("make_styled_spans normal text cur_text_pos < ent.start %s", value->text.substr(cur_text_pos, ent.start - cur_text_pos).c_str());
                _append_to_spans(
                        CLGraphics::split_text_to_spans(value->text,
                                 cur_text_pos,
                                 ent.start - cur_text_pos,
                                 x,
                                 box_width,
                                 MESSAGE_NORMAL_STYLE.font_style),
                         MESSAGE_NORMAL_STYLE,
                         EMPTY_TEXT_ENTITY,
                         x);
            }
            {
                const CLTextStyle& style = get_entity_style(ent);
//                Logger::debug("make_styled_spans entity %d x:%d text=%s style_bg=%d style_fg=%d", ent.type, x, value->text.substr(ent.start, ent.len).c_str(), style.bg_color, style.fg_color);
                _append_to_spans(
                        CLGraphics::split_text_to_spans(value->text,
                                                        ent.start,
                                                        ent.len,
                                                        x,
                                                        box_width,
                                                        style.font_style),
                        style,
                        ent,
                        x);
                cur_text_pos = ent.start + ent.len;
            }
        }
        if (cur_text_pos + 1 < value->text.size()) {
//            Logger::debug("make_styled_spans remaining text x:%d text %s", x, value->text.substr(cur_text_pos, value->text.size() - cur_text_pos).c_str());
            _append_to_spans(
                    CLGraphics::split_text_to_spans(value->text,
                                                    cur_text_pos,
                                                    value->text.size() - cur_text_pos,
                                                    x,
                                                    box_width,
                                                    MESSAGE_NORMAL_STYLE.font_style),
                    MESSAGE_NORMAL_STYLE,
                    EMPTY_TEXT_ENTITY,
                    x);
        }
    } else {
        _append_to_spans(
                CLGraphics::split_text_to_spans(value->text,
                                                0,
                                                value->text.size(),
                                                x,
                                                box_width,
                                                MESSAGE_NORMAL_STYLE.font_style),
                MESSAGE_NORMAL_STYLE,
                EMPTY_TEXT_ENTITY,
                x);
    }
    //Logger::debug("Split message finished");
}

//void MessageListViewItem::split_message_to_parts(int boxwidth) {
//    // split message text by entities to message_text_parts
//    parts.clear();
//    //Logger::debug("Split message started");
//    if (value->text_entities) {
//        int cur_text_pos = 0;
//        std::string text;
//        TextEntity* current_entity = value->text_entities;
//        while(current_entity) {
//            if (cur_text_pos < current_entity->start) {
//                text = utf8_substr(value->text, cur_text_pos, current_entity->start - cur_text_pos);
//                text = rtrim(text, " \n");
//                MessageTextPart part = MessageTextPart(MESSAGE_NORMAL_STYLE, nullptr);
//                CLGraphics::split_text_to_width(text,
//                                                boxwidth - TEXT_BOX_INNER_PADDING * 2,
//                                                part.style.font_style, part.lines);
//                parts.push_back(part);
//            }
//            {
//                MessageTextPart part = MessageTextPart(get_entity_style(current_entity), current_entity);
//                text = utf8_substr(value->text, current_entity->start, current_entity->len);
//                CLGraphics::split_text_to_width(text,
//                                                boxwidth - TEXT_BOX_INNER_PADDING * 2,
//                                                part.style.font_style, part.lines);
//                parts.push_back(part);
//                cur_text_pos = current_entity->len + current_entity->start;
//            }
//            current_entity = current_entity->next;
//        }
//        if (cur_text_pos + 1 < value->text.size()) {
//            text = utf8_substr(value->text, cur_text_pos, std::string::npos);
//            text = rtrim(text, " \n");
//            MessageTextPart part = MessageTextPart(MESSAGE_NORMAL_STYLE, nullptr);
//            CLGraphics::split_text_to_width(text,
//                                            boxwidth - TEXT_BOX_INNER_PADDING * 2,
//                                            part.style.font_style, part.lines);
//            parts.push_back(part);
//        }
//    } else {
//        MessageTextPart part = MessageTextPart(MESSAGE_NORMAL_STYLE, nullptr);
//        CLGraphics::split_text_to_width(value->text,
//                                        boxwidth - TEXT_BOX_INNER_PADDING * 2,
//                                        part.style.font_style, part.lines);
//        parts.push_back(part);
//    }
//    //Logger::debug("Split message finished");
//}

void MessageListViewItem::paint(CLGraphics &g, int idx) {
    int x, y = 0, content_height = 0;
    tbx::Colour line_bg_color;
    tbx::Colour default_bg_color;
    ChatDataPtr chat = value->get_chat();
    bool is_channel = (chat->type == CHAT_TYPE_CHANNEL);
    bool is_group = (chat->type == CHAT_TYPE_GROUP);
    bool is_outgoing = value->is_outgoing();
    bool is_draw_profile_name = (!is_outgoing && is_group);
    std::string prevdate;

    clickable_parts.clear();

    if (prev) {
        prevdate = convert_time_full_to_DMY(get_prev()->value->sendtime);
    }
    std::string msgdate = convert_time_full_to_DMY(value->sendtime);
    if (prevdate != msgdate) {
//        Logger::debug("prev %s msg %s prev:%p", prevdate.c_str(), msgdate.c_str(), prev);
        y = -(DATE_FONT_STYLE.get_font_height() + DATE_BOX_TOPBOTTOM_PADDING);
        g.draw_text_centered(0, y, bounds().width(),
                             msgdate, DATE_FONT_STYLE, DATE_COLOR, DATE_COLOR);

        y -= DATE_BOX_TOPBOTTOM_PADDING;
    }

    if (view->_last_seen_message_id > 0
        && !value->is_outgoing()
        && value->id > view->_last_seen_message_id
        && (!prev || get_prev()->value->id <= view->_last_seen_message_id)) {
        g.foreground(0xA0FFA000);
        g.fill_rectangle(TEXT_BOX_SIDE_PADDING, y - TEXT_FONT_LINEHEIGHT, bounds().width() - TEXT_BOX_SIDE_PADDING, y);

        g.draw_text_centered(0, y - 28, bounds().width(),
                             "Unread messages", UNREADED_FONT_STYLE, DATE_COLOR, DATE_COLOR);

        y -= (DATE_BOX_TOPBOTTOM_PADDING + TEXT_FONT_LINEHEIGHT);
    }

    int boxwidth = text_box_width(bounds().width());
    std::string msg_time_str = convert_time_full_to_HM(value->changedtime > 0 ? value->changedtime : value->sendtime);

//    Logger::debug("paint msg %lld type=%d %d:%d %dx%d has attacmhent=%d text=%s", value->id, value->type, bounds().top_left().x, bounds().top_left().y, bounds().width(), bounds().height(), value->has_attachment(), value->text.c_str());

    content_height = content_text_height;

    if (value->forward_info) {
        content_height += TEXT_FONT_LINEHEIGHT;
    }

    if (value->reply_info) {
        content_height += TEXT_FONT_LINEHEIGHT*2;
    }

    if (value->has_attachment()) {
        if (value->att_image && !value->att_image->thumb_url_cached.empty()) {
//            Logger::debug("Has attacmhent %s->%s", value->att_image->thumb_url.c_str(), value->att_image->thumb_url_cached.c_str());
            content_height += (value->att_image->thumb_height * 2 + TEXT_FONT_LINEHEIGHT);
            if (value->text.empty()) { // text is empty (no lines)
                boxwidth = value->att_image->thumb_width * 2 + TEXT_BOX_INNER_PADDING * 2;
            }
        } else if (value->att_file && value->att_file->thumb_height > 0) {
//            Logger::debug("Has attacmhent %s->%s", value->att_image->thumb_url.c_str(), value->att_image->thumb_url_cached.c_str());
            int att_height = (value->att_file->thumb_height * 2 + TEXT_FONT_LINEHEIGHT);
            if (att_height < TEXT_FONT_LINEHEIGHT*3) {
                att_height = TEXT_FONT_LINEHEIGHT*3;
            }
            content_height += att_height;
        } else {
            content_height += TEXT_FONT_LINEHEIGHT*3;
        }
    }

    int box_height = content_height + DATE_LINEHEIGHT + TEXT_BOX_TOP_BOTTOM_INNER_PADDING * 2;
    if (is_outgoing) {
        default_bg_color = (view->search_found_current_id != 0 && view->search_found_current_id == value->id ? SEARCH_FOUND_OUTGOING_COLOR : OUTGOING_COLOR);
        x = bounds().width() - boxwidth - TEXT_BOX_SIDE_PADDING - AVATAR_MARGIN * 2 - AVATAR_SIZE - TEXT_BOX_BULB_SIZE;
    } else {
        default_bg_color = (view->search_found_current_id != 0 && view->search_found_current_id == value->id ? SEARCH_FOUND_INCOMING_COLOR : INCOMING_COLOR);
        if (is_draw_profile_name) {
            box_height += TEXT_FONT_LINEHEIGHT;
        }
        if (is_channel) {
            x = TEXT_BOX_SIDE_PADDING;
        } else {
            x = TEXT_BOX_SIDE_PADDING + AVATAR_MARGIN * 2 + AVATAR_SIZE + TEXT_BOX_BULB_SIZE;
        }
    }
    int x_with_inner_padding = x + TEXT_BOX_INNER_PADDING;

    if (box_height < AVATAR_SIZE) {
        box_height = AVATAR_SIZE;
    }

    if (value->type == MESSAGE_TYPE_STICKER) {
        default_bg_color = tbx::Colour::white;
    }

    g.draw_rounded_box(x, y - box_height, x + boxwidth, y,
                       TEXT_BOX_BORDER_RADIUS, 0, default_bg_color, tbx::Colour::black);

    if (!is_channel) {
        tbx::DrawPath pointer_path;
        g.foreground(default_bg_color);
        pointer_path.move(0, 0);
        if (is_outgoing) {
            pointer_path.line(TEXT_BOX_BULB_SIZE, -TEXT_BOX_BULB_SIZE);
            pointer_path.line(0, -TEXT_BOX_BULB_SIZE * 2);
            pointer_path.close_line();
            pointer_path.end_path();
            g.fill(x + boxwidth, y - TEXT_BOX_BULB_OFFSET, pointer_path);
        } else {
            pointer_path.line(-TEXT_BOX_BULB_SIZE, -TEXT_BOX_BULB_SIZE);
            pointer_path.line(0, -TEXT_BOX_BULB_SIZE * 2);
            pointer_path.close_line();
            pointer_path.end_path();
            g.fill(x, y - TEXT_BOX_BULB_OFFSET, pointer_path);
        }

        //string avatar = value->author->pic_small;
        if (value->author) {
//        Logger::debug("msg %lld has author", value->id);
            std::string avatar = value->author->pic_small_cached;
            if (!avatar.empty()) {
//            Logger::debug("msg %lld has avatar %s", value->id, avatar.c_str());
                tbx::BBox clickable_avatar;
                if (is_outgoing) {
                    g.draw_image_circled(x + TEXT_BOX_BULB_SIZE + boxwidth + AVATAR_MARGIN,
                                         y - AVATAR_SIZE, AVATAR_SIZE / 2,
                                         avatar, tbx::Colour::white);
                    clickable_avatar.min.x = x + TEXT_BOX_BULB_SIZE + boxwidth + AVATAR_MARGIN;
                    clickable_avatar.max.y = y;
                    clickable_avatar.max.x = clickable_avatar.min.x + AVATAR_SIZE;
                    clickable_avatar.min.y = clickable_avatar.max.y - AVATAR_SIZE;

                } else {
                    g.draw_image_circled(x - TEXT_BOX_BULB_SIZE - AVATAR_MARGIN - AVATAR_SIZE,
                                         y - AVATAR_SIZE, AVATAR_SIZE / 2, avatar, tbx::Colour::white);

                    clickable_avatar.min.x = x - TEXT_BOX_BULB_SIZE - AVATAR_MARGIN - AVATAR_SIZE;
                    clickable_avatar.max.y = y;
                    clickable_avatar.max.x = clickable_avatar.min.x + AVATAR_SIZE;
                    clickable_avatar.min.y = clickable_avatar.max.y - AVATAR_SIZE;
                }
                clickable_parts.emplace_back(ClickableMessagePart(CLICKABLE_AVATAR, clickable_avatar, EMPTY_TEXT_ENTITY));
            }
        }
    }

    if (is_draw_profile_name) {
        y -= (TEXT_FONT_LINEHEIGHT - 4);
        if (value->author) {
            g.draw_text_cut_to_width(x_with_inner_padding, y, boxwidth,
                                     value->author->displayname,
                                     MESSAGE_AUTHOR_FONT_STYLE, MESSAGE_AUTHOR_FONT_COLOR, default_bg_color);
        }
    }

    if (value->forward_info) {
        y -= TEXT_FONT_LINEHEIGHT;
        if (!value->forward_info->title.empty()) {
            g.draw_text_cut_to_width(x_with_inner_padding, y, boxwidth,
                                     "Forwarded from: "+value->forward_info->title,
                                     MESSAGE_FORWAREDED_AUTHOR_FONT_STYLE, MESSAGE_FORWARDED_AUTHOR_FONT_COLOR, default_bg_color);
        }
    }

    if (value->reply_info) {
        y -= TEXT_FONT_LINEHEIGHT;
        int reply_text_offset = REPLY_PADDING;
        if (value->reply_info->author && !value->reply_info->author->displayname.empty()) {
            g.draw_text_cut_to_width(x_with_inner_padding + reply_text_offset, y, boxwidth - reply_text_offset - TEXT_BOX_INNER_PADDING,
                                     value->reply_info->author->displayname,
                                     MESSAGE_REPLIED_AUTHOR_FONT_STYLE, MESSAGE_REPLIED_AUTHOR_FONT_COLOR, default_bg_color);
        }
        y -= TEXT_FONT_LINEHEIGHT;
//        Logger::debug("reply text [%s]", value->reply_info->text.c_str());
        int first_text_line_pos = value->reply_info->text.find('\n');
        std::string first_line;
        if (first_text_line_pos != std::string::npos) {
            first_line = value->reply_info->text.substr(0, first_text_line_pos) + "...";
        } else {
            first_line = value->reply_info->text;
        }
//        Logger::debug("reply text first_line [%s]", first_line.c_str());
        g.draw_text_cut_to_width(x_with_inner_padding + reply_text_offset, y, boxwidth  - reply_text_offset - TEXT_BOX_INNER_PADDING,
                                 first_line,
                                 MESSAGE_REPLIED_TEXT_STYLE.font_style, MESSAGE_REPLIED_TEXT_STYLE.fg_color, default_bg_color);
        g.foreground(tbx::Colour(0x37, 0xa1, 0xde));
        g.fill_rectangle(x_with_inner_padding,y, x_with_inner_padding + 4, y + TEXT_FONT_LINEHEIGHT*2 - TEXT_BOX_BORDER_RADIUS);
    }

    y -= (TEXT_FONT_LINEHEIGHT);
    for(auto &spn : spans) {
        line_bg_color = spn.style.bg_color;
        if (line_bg_color == tbx::Colour::no_colour) {
            line_bg_color = default_bg_color;
        }
        g.draw_text(x_with_inner_padding + spn.span.x, y,
                    value->text.substr(spn.span.start, spn.span.len),
                    spn.style.font_style,
                    spn.style.fg_color, line_bg_color, line_bg_color != default_bg_color);
//        Logger::debug("draw_text x=%d y=%d fg=%x bg=%x lbg=%x ent=%d start=%d len=%d txt=%s", spn.span.x, y, spn.style.fg_color, spn.style.bg_color, line_bg_color, spn.entity.type, spn.span.start, spn.span.len, value->text.substr(spn.span.start, spn.span.len).c_str());
        switch (spn.entity.type) {
            case ENTITY_TEXT_URL:
            case ENTITY_URL:
                tbx::BBox bbox = tbx::BBox(x_with_inner_padding + spn.span.x,
                                           y,
                                           x + spn.span.width,
                                           y + TEXT_FONT_LINEHEIGHT);

                clickable_parts.emplace_back(ClickableMessagePart(
                        CLICKABLE_ENTITY,
                        bbox,
                        spn.entity
                ));
                break;
        }
        if (spn.span.newline) {
            y -= TEXT_FONT_LINEHEIGHT;
        }
    }
//    for(auto &part : parts) {
//        line_bg_color = part.style.bg_color;
//        if (line_bg_color == tbx::Colour::no_colour) {
//            line_bg_color = default_bg_color;
//        }
//        int start_y = y;
//        for(auto &line : part.lines) {
//            g.draw_text(x_with_inner_padding, y,
//                        line,
//                        part.style.font_style,
//                        part.style.fg_color, line_bg_color);
//            y -= TEXT_FONT_LINEHEIGHT;
//        }
//
//        if (part.entity) {
//            switch (part.entity->type) {
//                case ENTITY_TEXT_URL:
//                case ENTITY_URL:
//                    tbx::BBox bbox = tbx::BBox(x_with_inner_padding,
//                                               y + TEXT_FONT_LINEHEIGHT,
//                                               x + boxwidth - TEXT_BOX_INNER_PADDING,
//                                               start_y + TEXT_FONT_LINEHEIGHT);
//
//                    clickable_parts.push_back(ClickableMessagePart(
//                            CLICKABLE_ENTITY,
//                            bbox,
//                            part.entity
//                    ));
//                    break;
//            }
//        }
//    }
     if (value->has_attachment()) {
        int thumb_x;
        tbx::BBox clickable_attachment;
//        if (is_outgoing) {
//            thumb_x = x + boxwidth - TEXT_BOX_INNER_PADDING - value->att_image->thumb_width;
//        } else {
//            thumb_x = x_with_inner_padding;
//        }
        thumb_x = x_with_inner_padding;
        if (value->att_image) {
            string &thumb = value->att_image->thumb_url_cached;
//            Logger::debug("attachment image %s->%s thumb %s", value->att_image->thumb_url.c_str(), value->att_image->thumb_url_cached.c_str(), thumb.c_str());
            if (!thumb.empty()) {
                y -= (TEXT_BOX_INNER_PADDING + value->att_image->thumb_height * 2);
                g.draw_image(thumb, thumb_x, y, default_bg_color);

                clickable_attachment.min.x = thumb_x;
                clickable_attachment.min.y = y;
                clickable_attachment.max.y = y +  value->att_image->thumb_height * 2;
                clickable_attachment.max.x = clickable_attachment.min.x + value->att_image->thumb_width * 2;
            }
        } else if (value->att_file) {
            if (!value->att_file->thumb_url_cached.empty()) {
                int max_y = y - TEXT_BOX_INNER_PADDING;
                int x_att = thumb_x + value->att_file->thumb_width * 2 + ATTACHMENT_ICON_PADDING;
                y -= TEXT_FONT_LINEHEIGHT;
                g.draw_text(x_att, y,
                            value->att_file->name, MESSAGE_BOLD_FONT_STYLE,
                            TEXT_COLOR, default_bg_color);
                y -= TEXT_FONT_LINEHEIGHT;
                g.draw_text(x_att, y,
                            file_size_to_displayed_string(value->att_file->size), MESSAGE_FONT_STYLE,
                            TEXT_COLOR, default_bg_color);
                int thumb_y = max_y - (value->att_file->thumb_height * 2);
//                Logger::debug("Draw_image %s %d:%d max_y:%d height:%d", value->att_file->thumb_url_cached.c_str(), thumb_x, thumb_y, max_y, value->att_file->thumb_height);
                g.draw_image(value->att_file->thumb_url_cached, thumb_x, thumb_y, default_bg_color);
                if (thumb_y < y) {
                    y = thumb_y;
                }
                clickable_attachment.min.x = thumb_x;
                clickable_attachment.min.y = y;
                clickable_attachment.max.y = max_y;
                clickable_attachment.max.x = clickable_attachment.min.x + (boxwidth - thumb_x);
            } else {
                y -= (TEXT_BOX_INNER_PADDING + TEXT_FONT_LINEHEIGHT);
                tbx::WimpSprite file_sprite(value->att_file->file_type);
                int x_att = thumb_x + file_sprite.width() + ATTACHMENT_ICON_PADDING;

                g.draw_text(x_att, y,
                            value->att_file->name, MESSAGE_BOLD_FONT_STYLE,
                            TEXT_COLOR, default_bg_color);
                y -= TEXT_FONT_LINEHEIGHT;
                g.draw_text(x_att, y,
                            file_size_to_displayed_string(value->att_file->size), MESSAGE_FONT_STYLE,
                            TEXT_COLOR, default_bg_color);
                try {
                    g.image(thumb_x, y, file_sprite);
                } catch (std::exception &e) {
                    Logger::error("Error draw file type icon (type %d): %s", value->att_file->file_type, e.what());
                }

//            int attachment_rendered_width = draw_attachment(x_with_inner_padding, renderY, value->att_file);
                clickable_attachment.min.x = thumb_x;
                clickable_attachment.min.y = y;
                clickable_attachment.max.y = y + TEXT_FONT_LINEHEIGHT*2;
                clickable_attachment.max.x = clickable_attachment.min.x + (boxwidth - thumb_x);
            }
        }
        clickable_parts.emplace_back(ClickableMessagePart(CLICKABLE_ATTACHMENT, clickable_attachment, EMPTY_TEXT_ENTITY));
    }

    int time_text_width = CLGraphics::get_text_width(msg_time_str, DATE_FONT_STYLE);

    if (is_outgoing && (value->sending_state == MESSAGE_SENDING_STATE_OK || value->sending_state == MESSAGE_SENDING_STATE_FAILED)) {
        time_text_width += (OUTGOING_CHECK_WIDTH + OUTGOING_CHECK_WIDTH + OUTGOING_CHECK_MARGIN + 8);
    }

    y = -(bounds().height() - TEXT_BOX_TOP_BOTTOM_INNER_PADDING - ENTRIES_MARGIN - 6);
    x = x + (boxwidth - TEXT_BOX_INNER_PADDING - time_text_width);
    g.draw_text(x, y,
                msg_time_str, DATE_FONT_STYLE, TIME_COLOR, default_bg_color);

    if (value->changedtime > 0) {
        char *edited = "edited ";
        int changed_text_width = CLGraphics::get_text_width(edited, DATE_FONT_STYLE);
        int x_c = x - changed_text_width;
        g.draw_text(x_c, y,
                    edited, DATE_FONT_STYLE, EDITED_MARK_COLOR, default_bg_color);

    }

    if (is_outgoing) {
        tbx::DrawPath checkmark;
        tbx::DrawCapAndJoin caps;
        caps.join(tbx::DrawCapAndJoin::BEVELLED_JOINS);
        caps.leading_cap(tbx::DrawCapAndJoin::ROUND_CAPS);
        caps.trailing_cap(tbx::DrawCapAndJoin::ROUND_CAPS);

        if (value->sending_state == MESSAGE_SENDING_STATE_OK) {
            checkmark.move(0, OUTGOING_CHECK_HEIGHT / 2);
            checkmark.line(OUTGOING_CHECK_WIDTH/2, 0);
            checkmark.line(OUTGOING_CHECK_WIDTH, OUTGOING_CHECK_HEIGHT);
            checkmark.end_path();
            g.foreground(tbx::Colour::wimp_dark_green);
            g.stroke(x + (time_text_width - (OUTGOING_CHECK_WIDTH + OUTGOING_CHECK_WIDTH + 8)), y, checkmark, tbx::WINDING_NON_ZERO, 1, 6, &caps);

            if (chat->outgoing_seen_message_id >= value->id) {
                g.stroke(x + (time_text_width - OUTGOING_CHECK_WIDTH), y, checkmark, tbx::WINDING_NON_ZERO, 1, 6, &caps);
            }
        } else if (value->sending_state == MESSAGE_SENDING_STATE_FAILED) {
            checkmark.move(0, OUTGOING_CHECK_HEIGHT);
            checkmark.line(OUTGOING_CHECK_WIDTH, 0);
            checkmark.move(0, 0);
            checkmark.line(OUTGOING_CHECK_WIDTH, OUTGOING_CHECK_HEIGHT);
            checkmark.end_path();
            g.foreground(tbx::Colour::wimp_red);
            g.stroke(x + (time_text_width - OUTGOING_CHECK_WIDTH), y, checkmark, tbx::WINDING_NON_ZERO, 1, 6, &caps);
        }
    }

    g_app_data_model.mark_seen_message(value);
}

int MessageListViewItem::get_height(const tbx::Size &visible_size) {
    int content_height = 0;
    //int boxwidth = visible_size.width  - AVATAR_MARGIN * 2 - AVATAR_SIZE - TEXT_BOX_BULB_SIZE;
    int boxwidth = text_box_width(visible_size.width);
    make_styled_spans(boxwidth - TEXT_BOX_INNER_PADDING * 2);
    content_text_height = 0;
    if (!spans.empty()) {
        for(auto &spn : spans) {
            if (spn.span.newline) {
                content_text_height += TEXT_FONT_LINEHEIGHT;
            }
        }
        if (!spans.back().span.newline) {
            content_text_height += TEXT_FONT_LINEHEIGHT;
        }
        content_height += content_text_height;
    }

    //Logger::debug("Content lines height %d", content_height);
    content_height += DATE_LINEHEIGHT + TEXT_BOX_TOP_BOTTOM_INNER_PADDING * 2;
    // draw profile name
    if (!value->is_outgoing() && value->get_chat()->type == CHAT_TYPE_GROUP) {
        content_height += TEXT_FONT_LINEHEIGHT;
    }

    //Logger::debug("MessageListViewItem::get_height init %d", content_height);
    if (value->has_attachment()) {
        if (value->att_image) {
            content_height += value->att_image->thumb_height * 2 + TEXT_FONT_LINEHEIGHT;
            //Logger::debug("MessageListViewItem::get_height att img %d", content_height);
        } else if (value->att_file && value->att_file->thumb_height > 0) {
//            Logger::debug("Has attacmhent %s->%s", value->att_image->thumb_url.c_str(), value->att_image->thumb_url_cached.c_str());
            int att_height = (value->att_file->thumb_height * 2 + TEXT_FONT_LINEHEIGHT);
            if (att_height < TEXT_FONT_LINEHEIGHT*3) {
                att_height = TEXT_FONT_LINEHEIGHT*3;
            }
            content_height += att_height;
        } else {
            content_height += TEXT_FONT_LINEHEIGHT*3;
        }
    }

    if (value->forward_info) {
        content_height += TEXT_FONT_LINEHEIGHT;
    }

    if (value->reply_info) {
        content_height += TEXT_FONT_LINEHEIGHT*2;
    }

    if (content_height < AVATAR_SIZE) {
        content_height = AVATAR_SIZE;
    }

    std::string prevdate;
    if (prev) {
        prevdate = convert_time_full_to_DMY(get_prev()->value->sendtime);
    }
    string msgdate = convert_time_full_to_DMY(value->sendtime);
    if (prevdate != msgdate) {
        content_height += DATE_FONT_STYLE.get_font_height() + DATE_BOX_TOPBOTTOM_PADDING * 2;
    }

    if (view->_last_seen_message_id > 0
    && this->value->id > view->_last_seen_message_id
    && !this->value->is_outgoing()
    && (!prev || get_prev()->value->id <= view->_last_seen_message_id)) {
        content_height += (DATE_BOX_TOPBOTTOM_PADDING + TEXT_FONT_LINEHEIGHT);
    }

//    Logger::debug("MessageListViewItem::get_height %s %lld result %d", value->text.c_str(), value->id, content_height+20);
    return content_height + ENTRIES_MARGIN; // 10 is margin between entries
}


// MessagesListView
MessagesListView::MessagesListView(): BaseView("ChatMsgList"),
                                      _last_seen_message_id(0),
                                      clickable_logo_bbox(0,0,0,0)
{
    margin_bottom = 148;
    margin_top = 20;
    g_app_events.listen<AppEvents::MemberChanged>(std::bind(&MessagesListView::on_member_changed, this, std::placeholders::_1));
}

void MessagesListView::paint(const tbx::BBox& redraw_work_area, const tbx::VisibleArea &visible_area) {
    CLGraphics g(visible_area);
    g.foreground(tbx::Colour::white);
    auto work_box = visible_area.work(visible_area.bounds());
    g.fill_rectangle(work_box.min.x, work_box.min.y, work_box.max.x, work_box.max.y);

    if (g_app_data_model.get_currently_opened_chat()) {
        BaseView::paint(redraw_work_area, visible_area);
        //Logger::debug("work_box %d:%d %d:%d scroll %d:%d", work_box.min.x, work_box.max.y, work_box.max.x, work_box.min.y, visible_area.scroll().x, visible_area.scroll().y);
        //g.draw_cached_image("<ChatCube$Dir>.icons.arr_down_c", work_box.max.x - 76, work_box.min.y + 172, tbx::Colour::white);
    } else {
        int y;
        g.offset(tbx::Point(g.offset_x()+margin_left, g.offset_y()-margin_top));
        int canvas_width = visible_area.bounds().width() - margin_left - margin_right;
        //hack to calc a height of ChatToolbar
        // im->width_px() in pixels (no need multiplication)
        CLImage im;
        im.load("<ChatCube$Dir>.logo_big");
        y = -(60 + im.height());
//        Logger::debug("im w %d %d %d %d", im.width_px(), im.height_px(), im.width(), im.height());
        g.draw_image(im, canvas_width / 2 - im.width_px(), y, tbx::Colour::white);

        im.load("<ChatCube$Dir>.cloverleaf");
        //hack to calc a height of ChatToolbar
        // im->width() in pixels (no need multiplication)
        y -= (40 + im.height());
        g.draw_image(im, canvas_width / 2 - im.width_px(), y, tbx::Colour::white);

        y -= 40;
        g.draw_text_centered(0, y, canvas_width, std::string("www.RISCOSCloverleaf.com"),
                             MESSAGE_FONT_STYLE, tbx::Colour(0x18, 0x8a, 0xd7), tbx::Colour::white);
        clickable_logo_bbox = tbx::BBox(0, y + margin_top - 40, visible_area.bounds().width(), y + margin_top);

        y -= 40;

        std::string ver = std::string("Version ") + g_app_data_model.get_app_version();
        g.draw_text_centered(0, y, canvas_width, ver,
                             MESSAGE_FONT_STYLE, TEXT_COLOR, tbx::Colour::white);
    }
}

void MessagesListView::reload_messages(const ChatDataPtr chat, bool is_first_load) {
    _chat = chat;
    _is_first_load = is_first_load;
    auto &messages = _chat->get_messages();
    if (_is_first_load) {
        int unread_count = _chat->unread_count;
//        Logger::debug("MessagesListView::update_current_chat_messages _chat=%s last_seen_incoming_id:%lld", _chat->title.c_str(), _chat->incoming_seen_message_id);
        _last_seen_message_id = _chat->incoming_seen_message_id;
        reload_items(messages.begin(), messages.end(), true);
//        Logger::debug("MessagesListView::update_current_chat_messages _is_first_load reload_items end");

        if (unread_count >= 4) {
            maintain_scroll_position(ScrollPosition::UNCHANGED);
            MessageDataPtr first_new_msg = messages[0];
            for (auto &msg : messages) {
                if (!msg->is_outgoing() && msg->id > _last_seen_message_id) {
                    first_new_msg = msg;
                    break;
                }
            }
            Logger::debug("MessagesListView::update_current_chat_messages scroll_to_item first_new_msg %lld", first_new_msg->id);
            scroll_to_item(first_new_msg);
        } else {
            maintain_scroll_position(ScrollPosition::GO_TO_BOTTOM);
            Logger::debug("MessagesListView::update_current_chat_messages do_maintain_scroll_position");
            do_maintain_scroll_position();
        }
    } else {
        reload_items(messages.begin(), messages.end(), false);
//        Logger::debug("MessagesListView::update_current_chat_messages reload_items end");
    }
}

void MessagesListView::post_add_item(MessageListViewItem& item, bool initial_load) {
    MessageDataPtr msg = item.value;
    item.view = this;
    if (msg->author != nullptr && msg->author->pic_small_cached.empty()) {
//        Logger::debug("missing avatar for author %s", msg->author->first_name.c_str());
        waiting_for_avatar_members.insert(msg->author);
    }
}

void MessagesListView::post_change_item(MessageListViewItem &item) {
    post_add_item(item, false);
}

void MessagesListView::on_member_changed(const AppEvents::MemberChanged& ev) {
    if (ev.changes & MEMBER_CHANGES_PIC_SMALL) {
        auto found = waiting_for_avatar_members.find(ev.mem);
//    Logger::debug("MessagesListView::on_member_changed mem %s", ev.mem->first_name.c_str());
        if (found != waiting_for_avatar_members.end()) {
//        Logger::debug("MessagesListView::on_member_changed found %s", ev.mem->first_name.c_str());
            update_visible_at_next_tick();
            waiting_for_avatar_members.erase(found);
        }
    }
}

tbx::BBox MessagesListView::recalc_layout(const tbx::BBox &visible_bounds, const tbx::BBox& current_extent) {
    MessageListViewItem *first_item = get_first_item();
    if (first_item) {
        return ListViewMixin::recalc_layout(visible_bounds, current_extent);
    } else {
        return tbx::BBox(0, -2600, current_extent.width(), 0);
    }
}

void MessagesListView::mouse_click(tbx::MouseClickEvent &event) {
    if (get_first_item()) {
        BaseView::mouse_click(event);
    } else if (event.button() & 1024){
        tbx::WindowState state;
        win.get_state(state);
        tbx::Point hit_point(state.visible_area().work_x(event.x()), state.visible_area().work_y(event.y()));
        //Logger::debug("MessagesListView::mouse_click %d %d:%d %d:%dx%d:%d", event.button(), hit_point.x, hit_point.y, clickable_logo_bbox.min.x, clickable_logo_bbox.min.y, clickable_logo_bbox.max.x, clickable_logo_bbox.max.y);
        if (clickable_logo_bbox.contains(hit_point.x, hit_point.y)) {
            open_browser_url("https://riscoscloverleaf.com/");
        }
    }
}

void MessagesListView::search_show_found(MessageDataPtr msg) {
    if (msg != nullptr) {
        Logger::debug("MessagesListView::search_show_found msg id=%lld", msg->id);
        search_found_current_id = msg->id;
        scroll_to_item(msg);
        update_visible();
    } else {
        search_found_current_id = 0;
    }
}

void MessagesListView::search_set_query(const std::string& query, int filter) {
    if (search_filter != filter || search_substring != query) {
        search_filter = filter;
        search_substring = query;
        repaint_all_immediate();
    }
}

bool MessagesListView::is_scrolled_to_bottom() {
    tbx::WindowInfo info;
    win.get_info(info);
//    Logger::debug("MessagesListView::is_scrolled_to_bottom ext_h=%d sc_h=%d", win.extent().height(), (-info.visible_area().scroll().y + info.visible_area().bounds().height() + 160));
    return win.extent().height()  < (-info.visible_area().scroll().y + info.visible_area().bounds().height() + 160);
}

bool MessagesListView::is_scrolled_to_top() {
    tbx::WindowInfo info;
    win.get_info(info);
    Logger::debug("MessagesListView::is_scrolled_to_top ext_h=%d scy=%d h=%d", win.extent().height(), info.visible_area().scroll().y, info.visible_area().bounds().height());
    return (win.extent().height() > info.visible_area().bounds().height() && info.visible_area().scroll().y == 0);
}