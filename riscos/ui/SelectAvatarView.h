//
// Created by lenz on 3/16/20.
//

#ifndef ROCHAT_SELECTAVATARVIEW_H
#define ROCHAT_SELECTAVATARVIEW_H

#include <list>
#include <functional>
#include <tbx/window.h>
#include <tbx/redrawlistener.h>
#include <tbx/mouseclicklistener.h>
#include <tbx/hasbeenhiddenlistener.h>
#include <tbx/scrollrequestlistener.h>
#include <tbx/command.h>
#include <tbx/loader.h>
#include "../model/AppDataModel.h"
#include "cloverleaf/CLGraphics.h"
#include "cloverleaf/CLImage.h"
#include "cloverleaf/BaseViews.h"

class SelectAvatarViewItem : public TileViewItem<AvatarData> {
public:
    CLImage img;
    SelectAvatarViewItem() : TileViewItem() {};
    SelectAvatarViewItem(AvatarData _value);

    SelectAvatarViewItem* get_next() { return (SelectAvatarViewItem*) next; }
    SelectAvatarViewItem* get_prev() { return (SelectAvatarViewItem*) prev; }

    void paint(CLGraphics &g, int idx);
    int get_height(const tbx::Size& visible_size) override;
    int get_width(const tbx::Size& visible_size) override;
};

typedef ViewItemClickEvent<SelectAvatarViewItem> SelectAvatarClickEvent;

class SelectAvatarView : public BaseView<SelectAvatarViewItem, AvatarData>,
                         public TileViewMixin<SelectAvatarView>,
                         public tbx::ScrollRequestListener
{
private:
    CLImage current_avatar;
public:
    SelectAvatarView();

    tbx::BBox recalc_layout(const tbx::BBox& visible_bounds, const tbx::BBox& current_extent) override;
    void open(std::string& _current_avatar);
    void paint(const tbx::BBox &redraw_work_area, const tbx::VisibleArea &visible_area) override;

    void scroll_request(const tbx::ScrollRequestEvent &event) override;
};


#endif //ROCHAT_SELECTAVATARVIEW_H
