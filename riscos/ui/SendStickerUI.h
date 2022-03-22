//
// Created by lenz on 3/24/20.
//

#ifndef ROCHAT_SENDSTICKERUI_H
#define ROCHAT_SENDSTICKERUI_H

#include <tbx/hasbeenhiddenlistener.h>
#include <tbx/scrollrequestlistener.h>
#include "../model/AppDataModel.h"
#include <cloverleaf/BaseViews.h>

#define STICKERLIST_GROUP_PADDING           8
#define STICKERLIST_STICKER_SIZE            112

class StickerGroupViewItem : public TileViewItem<StickerGroupData> {
public:
    const int sticker_size = STICKERLIST_STICKER_SIZE + STICKERLIST_GROUP_PADDING + STICKERLIST_GROUP_PADDING;

    StickerGroupViewItem() {}
    StickerGroupViewItem(const StickerGroupData &value) : TileViewItem(value) {}

    StickerGroupViewItem* get_next() { return (StickerGroupViewItem*) next; }
    StickerGroupViewItem* get_prev() { return (StickerGroupViewItem*) prev; }

    void paint(CLGraphics &g, int idx);
    int get_height(const tbx::Size& visible_size) override { return sticker_size; }
    int get_width(const tbx::Size& visible_size) override { return sticker_size; }
};


class StickerViewItem : public TileViewItem<StickerData> {
public:
    const int sticker_size = STICKERLIST_STICKER_SIZE + STICKERLIST_GROUP_PADDING + STICKERLIST_GROUP_PADDING;

    StickerViewItem() {}
    StickerViewItem(const StickerData &value) : TileViewItem(value) {}

    StickerViewItem* get_next() { return (StickerViewItem*) next; }
    StickerViewItem* get_prev() { return (StickerViewItem*) prev; }

    void paint(CLGraphics &g, int idx);
    int get_height(const tbx::Size& visible_size) override { return sticker_size; }
    int get_width(const tbx::Size& visible_size) override { return sticker_size; }
};

class StickerGroupView : public BaseView<StickerGroupViewItem, StickerGroupData>, public TileViewMixin<StickerGroupView> {;
public:
    StickerGroupView() : BaseView("pane") {};

    tbx::BBox recalc_layout(const tbx::BBox& visible_bounds, const tbx::BBox& current_extent) override;

    void paint(const tbx::BBox &redraw_work_area, const tbx::VisibleArea &visible_area) override;
};

class StickerView : public BaseView<StickerViewItem, StickerData>, public TileViewMixin<StickerView> {;
public:
    StickerView() : BaseView("paneVScr") {};


    tbx::BBox recalc_layout(const tbx::BBox& visible_bounds, const tbx::BBox& current_extent) override;

    void paint(const tbx::BBox &redraw_work_area, const tbx::VisibleArea &visible_area) override;
};


class SendStickerUI : public ViewItemClickListener<StickerViewItem>,
                      public ViewItemClickListener<StickerGroupViewItem>,
                      public tbx::HasBeenHiddenListener,
                      public tbx::ScrollRequestListener {
private:
    const int groups_height = (STICKERLIST_STICKER_SIZE + STICKERLIST_GROUP_PADDING + STICKERLIST_GROUP_PADDING + 8);

public:
    tbx::Window main_win;
    StickerView sticker_view;
    StickerGroupView sticker_group_view;
    const std::vector<StickerGroupData> &stickers;
    static int selected_group;

    SendStickerUI();
    ~SendStickerUI();

    void setup_subwindows();
    void item_clicked(ViewItemClickEvent<StickerViewItem> &event) override;
    void item_clicked(ViewItemClickEvent<StickerGroupViewItem> &event) override;

    void has_been_hidden(const tbx::EventInfo &event_info) override;
    void scroll_request(const tbx::ScrollRequestEvent &event) override;
};


#endif //ROCHAT_SENDSTICKERUI_H
