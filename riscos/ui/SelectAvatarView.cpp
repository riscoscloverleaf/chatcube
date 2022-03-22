//
// Created by lenz on 3/16/20.
//
#include <tbx/path.h>
#include <cloverleaf/CLImageCache.h>
#include "SelectAvatarView.h"
#include "FontStyles.h"
#include "../global.h"

SelectAvatarViewItem::SelectAvatarViewItem(AvatarData _value) :
    TileViewItem(_value)
{
    //Logger::debug("SelectAvatarViewItem::SelectAvatarViewItem %s", value.name.c_str());
    img.load(_value.pic_small);
}

void SelectAvatarViewItem::paint(CLGraphics &g, int idx) {
    //Logger::debug("SelectAvatarViewItem::paint img=%p %d", &img, img.is_valid());
    g.draw_image(img, 0, -img.height(), tbx::Colour::white);
}

int SelectAvatarViewItem::get_height(const tbx::Size &visible_size) {
    return img.height();
}

int SelectAvatarViewItem::get_width(const tbx::Size &visible_size) {
    return img.width();
}

// SelectAvatarView

SelectAvatarView::SelectAvatarView() : BaseView("changAvatar") {
    win.add_scroll_request_listener(this);
}

void SelectAvatarView::open(std::string& _current_avatar) {
    if (!_current_avatar.empty()) {
        current_avatar.load(_current_avatar);
    }

    win.show_centered();

    auto avatars = g_app_data_model.get_avatars();
    reload_items(avatars.begin(), avatars.end(), true);

}

tbx::BBox SelectAvatarView::recalc_layout(const tbx::BBox &visible_bounds, const tbx::BBox& current_extent) {
    if (current_avatar.is_valid()) {
        margin_top = (current_avatar.height() + 90);
    } else {
        margin_top = 50;
    }
    //Logger::debug("SelectAvatarView::recalc_layout %d", margin_top);
    tbx::BBox bounds = TileViewMixin::recalc_layout(visible_bounds, current_extent);
    if (bounds.height() < visible_bounds.height())
        bounds.min.y =  -visible_bounds.height();

    return bounds;
}

void SelectAvatarView::paint(const tbx::BBox &redraw_work_area, const tbx::VisibleArea &visible_area) {
    CLGraphics g(visible_area.screen_x(0), visible_area.screen_y(0));
    int y = 0, visible_width = visible_area.bounds().width();
    if (current_avatar.is_valid()) {
        y = -30;
        g.draw_text_centered(0, y, visible_width, "Current avatar",
                             TEXT_FONT_STYLE, tbx::Colour::black, tbx::Colour::white);
        y -= (10 + current_avatar.height());
        g.draw_image(current_avatar, visible_width/2 - current_avatar.width_px(), y, tbx::Colour::white);
    }
    y -= 40;
    g.draw_text_centered(0, y, visible_width, "Please select predefined avatar or drop image here",
                         TEXT_FONT_STYLE, tbx::Colour::black, tbx::Colour::white);

    BaseView::paint(redraw_work_area, visible_area);
}

void SelectAvatarView::scroll_request(const tbx::ScrollRequestEvent &event) {
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
    win.open_window(open_info);
}
