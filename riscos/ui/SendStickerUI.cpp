//
// Created by lenz on 3/24/20.
//
#include <cloverleaf/Logger.h>
#include "SendStickerUI.h"
const tbx::Colour STICKERLIST_SELECTED_BG       = tbx::Colour::wimp_grey2;
const tbx::Colour STICKERLIST_BG                = tbx::Colour::white;
const tbx::Colour STICKERLIST_LINE_COLOR        = tbx::Colour::wimp_red;

void StickerGroupViewItem::paint(CLGraphics &g, int idx) {
    tbx::Colour color;
    if (selected) {
        color = STICKERLIST_SELECTED_BG;
    } else {
        color = STICKERLIST_BG;
    }
    g.foreground(color);
    g.fill_rectangle(0, -sticker_size, sticker_size, 0);
    g.draw_cached_image(value.pic_small,
            STICKERLIST_GROUP_PADDING, -(STICKERLIST_GROUP_PADDING + STICKERLIST_STICKER_SIZE),
            color);
}

void StickerViewItem::paint(CLGraphics &g, int idx) {
    g.draw_cached_image(value.pic_small,
                        STICKERLIST_GROUP_PADDING, -(STICKERLIST_GROUP_PADDING + STICKERLIST_STICKER_SIZE),
                        STICKERLIST_BG);
}

void StickerGroupView::paint(const tbx::BBox &redraw_work_area, const tbx::VisibleArea &visible_area) {
    CLGraphics g(visible_area);
    g.foreground(tbx::Colour::white);
    g.tbx::Graphics::fill_rectangle(redraw_work_area);
    BaseView::paint(redraw_work_area, visible_area);

    tbx::BBox ext = win.extent();
    g.foreground(STICKERLIST_LINE_COLOR);
    g.fill_rectangle(0, -ext.height(), ext.width(), -(ext.height() - 4));
}

tbx::BBox StickerGroupView::recalc_layout(const tbx::BBox &visible_bounds, const tbx::BBox& current_extent) {
//    Logger::info("StickerGroupView::recalc_layout %d,%dx%d,%d %d,%d", visible_bounds.min.x, visible_bounds.min.y, visible_bounds.max.x, visible_bounds.max.y, visible_bounds.width(), visible_bounds.height());
    tbx::BBox bounds = TileViewMixin::recalc_layout(visible_bounds, current_extent);
    bounds.min.y -= 8;
    if (bounds.height() < visible_bounds.height())
        bounds.min.y =  -visible_bounds.height();
//    Logger::info("StickerGroupView::recalc_layout %d,%dx%d,%d %d,%d", bounds.min.x, bounds.min.y, bounds.max.x, bounds.max.y, bounds.width(), bounds.height());
    return bounds;
}


tbx::BBox StickerView::recalc_layout(const tbx::BBox &visible_bounds, const tbx::BBox& current_extent) {
//    Logger::info("StickerView::recalc_layout %d,%dx%d,%d %d,%d", visible_bounds.min.x, visible_bounds.min.y, visible_bounds.max.x, visible_bounds.max.y, visible_bounds.width(), visible_bounds.height());
    tbx::BBox bounds = TileViewMixin::recalc_layout(visible_bounds, current_extent);
    if (bounds.height() < visible_bounds.height())
        bounds.min.y =  -visible_bounds.height();
//    Logger::info("StickerView::recalc_layout %d,%dx%d,%d %d,%d", bounds.min.x, bounds.min.y, bounds.max.x, bounds.max.y, bounds.width(), bounds.height());
    return bounds;
}

void StickerView::paint(const tbx::BBox &redraw_work_area, const tbx::VisibleArea &visible_area) {
    CLGraphics g(visible_area);
    g.foreground(STICKERLIST_BG);
    g.tbx::Graphics::fill_rectangle(redraw_work_area);
    BaseView::paint(redraw_work_area, visible_area);
}


int SendStickerUI::selected_group = 0;

SendStickerUI::SendStickerUI() :
        main_win("stickers"),
        stickers(g_app_data_model.get_strickers())
{
    sticker_view.add_click_listener(this);
    sticker_group_view.add_click_listener(this);
    main_win.add_has_been_hidden_listener(this);
//    main_win.add_scroll_request_listener(this);
    sticker_view.win.add_scroll_request_listener(this);

    main_win.show_centered();

    sticker_group_view.reload_items(stickers.begin(), stickers.end(), true);
    sticker_group_view.set_selected_item(stickers[selected_group], true);
    sticker_view.reload_items(stickers[selected_group].items.begin(), stickers[selected_group].items.end(), true);

    setup_subwindows();
}

void SendStickerUI::setup_subwindows() {
    unsigned  int groups_subwindow_flags;
    tbx::BBox main_bounds;
    tbx::WindowState win_info;

//    Logger::debug("SendStickerUI::setup_subwindows");

    tbx::WindowState main_win_state;
    main_win.get_state(main_win_state);

    main_bounds = main_win_state.visible_area().bounds();
    tbx::BBox main_extent(0, -main_bounds.height(), main_bounds.width(), 0);
    sticker_group_view.win.extent(main_extent);
    sticker_view.win.extent(main_extent);

    sticker_group_view.win.get_state(win_info);
    win_info.visible_area() = main_win_state.visible_area();
    win_info.visible_area().bounds().min.y = win_info.visible_area().bounds().max.y - groups_height;
    groups_subwindow_flags = tbx::ALIGN_LEFT_VISIBLE_LEFT
                                           | tbx::ALIGN_TOP_VISIBLE_TOP
                                           | tbx::ALIGN_RIGHT_VISIBLE_RIGHT
                                           | tbx::ALIGN_BOTTOM_VISIBLE_TOP
            ;
    sticker_group_view.win.open_subwindow(win_info, main_win, groups_subwindow_flags);

    sticker_group_view.update_window_extent();
    sticker_group_view.update_visible();


    sticker_view.win.get_state(win_info);
    win_info.visible_area() = main_win_state.visible_area();
    win_info.visible_area().bounds().max.y = win_info.visible_area().bounds().max.y - groups_height;
    groups_subwindow_flags = tbx::ALIGN_LEFT_VISIBLE_LEFT
                                           | tbx::ALIGN_TOP_VISIBLE_TOP
                                           | tbx::ALIGN_RIGHT_VISIBLE_RIGHT
                                           | tbx::ALIGN_BOTTOM_VISIBLE_BOTTOM
    ;
    sticker_view.win.open_subwindow(win_info, main_win, groups_subwindow_flags);

    sticker_view.update_window_extent();
    sticker_view.update_visible();

    //main_win.extent(tbx::BBox(0, -(sticker_view.win.extent().height() + groups_height), main_extent.width(), 0));
}

SendStickerUI::~SendStickerUI() {
//    Logger::debug("SendStickerUI::~SendStickerUI");
    main_win.delete_object();
}

void SendStickerUI::item_clicked(ViewItemClickEvent<StickerViewItem> &event) {
    g_app_data_model.send_message_sticker(&event.view_item().value);
    main_win.hide();
}

void SendStickerUI::item_clicked(ViewItemClickEvent<StickerGroupViewItem> &event) {
    StickerGroupData &data = event.view_item().value;
    for(selected_group = 0; selected_group < stickers.size(); selected_group++) {
        if (stickers[selected_group] == data) break;
    }
    sticker_group_view.set_selected_item(data, true);
    sticker_view.reload_items(data.items.begin(), data.items.end(), true);

    tbx::BBox ext = main_win.extent();
    ext.min.y = -(sticker_view.win.extent().height() + groups_height);
    main_win.extent(ext);
}

void SendStickerUI::has_been_hidden(const tbx::EventInfo &event_info) {
//    Logger::debug("SendStickerUI::has_been_hidden");
    delete this;
}

void SendStickerUI::scroll_request(const tbx::ScrollRequestEvent &event) {
    tbx::WindowOpenInfo open_info = event.open_info();
    switch(event.y_scroll()) {
        case tbx::ScrollRequestEvent::ScrollYDirection::PAGE_DOWN:
            open_info.visible_area().scroll().y -= (open_info.visible_area().bounds().height() - 200);
            break;
        case tbx::ScrollRequestEvent::ScrollYDirection::PAGE_UP:
            open_info.visible_area().scroll().y += (open_info.visible_area().bounds().height() - 200);
            break;
        case tbx::ScrollRequestEvent::ScrollYDirection::DOWN:
            open_info.visible_area().scroll().y -= 40;
            break;
        case tbx::ScrollRequestEvent::ScrollYDirection::UP:
            open_info.visible_area().scroll().y += 40;
            break;
    }
    sticker_view.win.open_window(open_info);
}
