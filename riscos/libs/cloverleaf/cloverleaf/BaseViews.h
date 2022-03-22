//
// Created by lenz on 3/10/20.
//

#ifndef ROCHAT_BASEVIEWS_H
#define ROCHAT_BASEVIEWS_H

#include <list>
#include <functional>
#include <tbx/window.h>
#include <tbx/actionbutton.h>
#include <tbx/writablefield.h>
#include <tbx/optionbutton.h>
#include <tbx/objectdelete.h>
#include <tbx/redrawlistener.h>
#include <tbx/mouseclicklistener.h>
#include <tbx/offsetgraphics.h>
#include <tbx/safelist.h>
#include "cloverleaf/CLGraphics.h"
#include "cloverleaf/Logger.h"
#include "IdleTask.h"


/**
 * Event for click on ItemView
 */
template <typename VIEWITEMTYPE>
class ViewItemClickEvent
{
    VIEWITEMTYPE &_view_item;
    tbx::Point &_workarea_hit_point;
    const tbx::MouseClickEvent &_click_event;

public:
    /**
     * Constructor
     */
    ViewItemClickEvent(VIEWITEMTYPE &item, tbx::Point & hit_point, const tbx::MouseClickEvent &click_event) :
            _view_item(item), _workarea_hit_point(hit_point), _click_event(click_event) {}

    /**
     * Return item view this event occurred on
     */
    VIEWITEMTYPE &view_item() const {return _view_item;}

    const tbx::Point& workarea_hit_point() const { return _workarea_hit_point; }

    /**
     * Returns original mouse click event on the window containing this item view
     */
    const tbx::MouseClickEvent &click_event() const {return _click_event;}
};

/**
 * Listener for clicks on the item view
 */
template <typename VIEWITEMTYPE>
class ViewItemClickListener : public tbx::Listener
{
public:
    ViewItemClickListener() {}
    virtual ~ViewItemClickListener() {}

    /**
     * Method called when an item in the view is clicked
     *
     * @param event details of item hit (if any)
     */
    virtual void item_clicked(ViewItemClickEvent<VIEWITEMTYPE> &event) = 0;
};

template <typename VIEWITEMTYPE>
class ViewSelectionChangedListener : public tbx::Listener
{
public:
    ViewSelectionChangedListener() {}
    virtual ~ViewSelectionChangedListener() {}

    /**
     * Method called when an item in the view is clicked
     *
     * @param event details of item hit (if any)
     */
    virtual void selection_changed(VIEWITEMTYPE *item) = 0;
};

class BaseViewItem {
protected:
    tbx::BBox _bounds;
public:
    BaseViewItem *prev;
    BaseViewItem *next;

    bool was_changed;
    bool selected;

    BaseViewItem() : prev(nullptr), next(nullptr), selected(false), was_changed(true) {};

    inline const tbx::BBox &bounds() const { return _bounds; }

    virtual int get_width(const tbx::Size& visible_size) { return 0; }
    virtual int get_height(const tbx::Size& visible_size) { return 0; }

    virtual bool calc_bounds(const tbx::Point& top_left, const tbx::Size& visible_size) = 0;
};

template <typename DATAITEMTYPE>
class TileViewItem : public BaseViewItem {
public:
    DATAITEMTYPE value = nullptr;

    TileViewItem() : BaseViewItem(), value(nullptr)  {};
    TileViewItem(DATAITEMTYPE _value) : BaseViewItem(), value(_value) {};

    bool calc_bounds(const tbx::Point& top_left, const tbx::Size& visible_size) override {
        tbx::BBox prev_bounds = _bounds;
        _bounds.min.x = top_left.x;
        _bounds.max.y = top_left.y;
        _bounds.max.x = top_left.x + get_width(visible_size);
        _bounds.min.y = top_left.y - get_height(visible_size);
        return (prev_bounds != _bounds);
    }
};

template <typename DATAITEMTYPE>
class ListViewItem : public BaseViewItem {
public:
    DATAITEMTYPE value = nullptr;

    ListViewItem() : BaseViewItem(), value(nullptr)  {};
    ListViewItem(DATAITEMTYPE _value) : BaseViewItem(), value(_value) {};

    bool calc_bounds(const tbx::Point& top_left, const tbx::Size& visible_size) override {
        if (was_changed) {
            tbx::BBox prev_bounds = _bounds;
            _bounds.min.x = top_left.x;
            _bounds.max.y = top_left.y;
            _bounds.max.x = top_left.x + visible_size.width;
            _bounds.min.y = top_left.y - get_height(visible_size);
            return (prev_bounds != _bounds);
        } else {
            if (top_left != _bounds.top_left()) {
                int bx = top_left.x - _bounds.min.x;
                int by = top_left.y - _bounds.max.y;
                _bounds.move(bx, by);
                return true;
            } else {
                return false;
            }
        }
    }
};

enum ScrollPosition {
    UNCHANGED,
    FIXED_FROM_BOTTOM,
    GO_TO_BOTTOM
};

template <typename VIEWCLASS>
class ListViewMixin {
public:
    int content_height = 0;
    virtual tbx::BBox recalc_layout(const tbx::BBox& visible_bounds, const tbx::BBox& current_extent) {
        VIEWCLASS & self = static_cast<VIEWCLASS &>(*this);
        tbx::Size visible_size = visible_bounds.size();
        tbx::Point top_left(self.margin_left, -self.margin_top);
        visible_size.width -= (self.margin_left + self.margin_right);
        visible_size.height -= (self.margin_top + self.margin_bottom);

        bool prev_item_was_changed = false;
        for(auto item = self.get_first_item(); item != nullptr; item = item->get_next()) {
            if (item->was_changed || prev_item_was_changed) {
                self.calc_item_bounds(*item, top_left, visible_size);
                prev_item_was_changed = true;
                item->was_changed = false;
            }
            top_left.y -= item->bounds().height();
        }
        content_height = top_left.y - self.margin_bottom - self.margin_top;
        int ret_heigh;
        //Logger::debug("content_height %d visible_bounds.height() %d", content_height, visible_bounds.height());
        if (content_height > -visible_bounds.height()) {
            ret_heigh = -visible_bounds.height();
        } else {
            ret_heigh = content_height;
        }
        //Logger::debug("1full_height %d visible_bounds.height() %d", content_height, visible_bounds.height());

        return tbx::BBox(0, ret_heigh, current_extent.width(), 0);
    };
};


template <typename VIEWCLASS>
class TileViewMixin {
public:
    virtual tbx::BBox recalc_layout(const tbx::BBox& visible_bounds, const tbx::BBox& current_extent) {
        VIEWCLASS & self = static_cast<VIEWCLASS &>(*this);
        tbx::Size visible_size = visible_bounds.size();
        tbx::Size item_size(0, 0);
        tbx::Point top_left(self.margin_left, -self.margin_top);
        visible_size.width -= (self.margin_left + self.margin_right);
        visible_size.height -= (self.margin_top + self.margin_bottom);
        bool bounds_changed = false;
        int idx = 0;
        for(auto item = self.get_first_item(); item != nullptr; item = item->get_next()) {
            if (idx == 0) {
                item_size.width = item->get_width(visible_size);
                item_size.height = item->get_height(visible_size);
                //Logger::debug("item_size.width x item_size.height %d x %d", item_size.width, item_size.height);
            }
            self.calc_item_bounds(*item, top_left, visible_size);

            top_left.x += item_size.width;
            if ((top_left.x + item_size.width) > visible_size.width) {
                top_left.y -= item_size.height;
                top_left.x = 0;
            }
            //Logger::debug("top_left.x top_left.y  %d x %d", top_left.x, top_left.y);
            idx ++;
        }
        top_left.y -= item_size.height;

        int full_height = top_left.y - self.margin_bottom - self.margin_top;

        return tbx::BBox(0, full_height, current_extent.width(), 0);
    };
};

template <typename VIEWITEMTYPE, typename DATAITEMTYPE>
class BaseView : public tbx::RedrawListener, public tbx::MouseClickListener {
protected:
    tbx::SafeList<ViewItemClickListener<VIEWITEMTYPE>> _click_listeners;
    tbx::SafeList<ViewSelectionChangedListener<VIEWITEMTYPE>> _selection_changed_listeners;
    VIEWITEMTYPE *_first_item;
    VIEWITEMTYPE *_last_item;
    int _maintain_scroll_position;
    bool _needs_update_at_next_tick;

    void delete_view_item(VIEWITEMTYPE* item) {
        if (item->prev) {
            item->prev->next = item->get_next();
        } else {
            _first_item = item->get_next();
        }
        if (item->next) {
            item->next->prev = item->get_prev();
        } else {
            _last_item = item->get_prev();
        }
        delete item;
    }

public:
    int margin_left, margin_right, margin_top, margin_bottom;
    bool is_multiselect;
//    Dexode::EventBus events;
    tbx::Window win;

    BaseView(const std::string& template_window) :
        _needs_update_at_next_tick(false),
        _first_item(nullptr), _last_item(nullptr),
        margin_left(0), margin_right(0), margin_top(0), margin_bottom(0),
        is_multiselect(false),
        win(template_window),
        _maintain_scroll_position(ScrollPosition::UNCHANGED) {
        win.add_redraw_listener(this);
//        win.add_start_hidden_listener(this);
    };

    ~BaseView() {
        delete_all_view_items();
        win.delete_object();
    }

    VIEWITEMTYPE* get_first_item() {
        return _first_item;
    }

    VIEWITEMTYPE* get_last_item() {
        return _last_item;
    }

    virtual void pre_initial_load() {};
    virtual void post_add_item(VIEWITEMTYPE& item, bool initial_load) {};
    virtual void post_change_item(VIEWITEMTYPE& item) {};
    virtual tbx::BBox recalc_layout(const tbx::BBox& visible_bounds, const tbx::BBox& current_extent) = 0;
    virtual void paint_item(CLGraphics &g, VIEWITEMTYPE& item, int idx) {
        item.paint(g, idx);
    };
    virtual bool calc_item_bounds(VIEWITEMTYPE& item, tbx::Point top_left, tbx::Size visible_size) {
        return item.calc_bounds(top_left, visible_size);
    }

    virtual void paint(const tbx::BBox& redraw_work_area, const tbx::VisibleArea &visible_area) {
        CLGraphics g(0, 0);
        int idx = 0;
        bool painted = false;
//        Logger::debug("paint redraw_work_area off=%dx%d %d:%d - %d:%d", redraw_work_area.top_left().x, redraw_work_area.top_left().y,
//                     redraw_work_area.min.x, redraw_work_area.max.y,
//                     redraw_work_area.max.x, redraw_work_area.min.y);
        for(auto item = _first_item; item != nullptr; item = item->get_next()) {
//            Logger::debug("paint item idx=%d off=%dx%d %d:%d - %d:%d %p",
//                    idx, item->bounds().top_left().x, item->bounds().top_left().y,
//                    item->bounds().min.x, item->bounds().max.y,
//                         item->bounds().max.x, item->bounds().min.y, item);
            if (item->bounds().intersects(redraw_work_area)) {
                g.offset(visible_area.screen(item->bounds().top_left()));
//                Logger::debug("paint_item idx=%d offset =%d:%d item top-left=%d:%d %p", idx, g.offset_x(), g.offset_y(), item->bounds().top_left().x, item->bounds().top_left().y,item);
                paint_item(g, *item, idx);
// does not work in the tile
//                painted = true;
//                Logger::debug("painted_item idx=%d offset =%dx%d", idx, g.offset_x(), g.offset_y());
//            } else if (painted) {
//                break;
            }
//            if (item->bounds().max.y < redraw_work_area.min.y) {
//                break;
//            }
            idx++;
        }
    }

    void delete_all_view_items() {
        VIEWITEMTYPE *p_item = _first_item;
        VIEWITEMTYPE *p_next;
        while(p_item != nullptr) {
            p_next = p_item->get_next();
            delete p_item;
            p_item = p_next;
        }
        _first_item = nullptr;
        _last_item = nullptr;
    }

    void redraw(const tbx::RedrawEvent &e) override {
        paint(e.visible_area().work(e.clip()), e.visible_area());
    }

//    void open_window(tbx::OpenWindowEvent &event) override {
//        update_window_extent();
//        update_visible();
//    }

    VIEWITEMTYPE* get_view_item(const DATAITEMTYPE& value) {
        for(auto item = _first_item; item != nullptr; item = item->get_next()) {
            if (item->value == value) {
                return item;
            }
        }
        return nullptr;
    }

    VIEWITEMTYPE* get_pointed_item(const int x, const int y) const {
        for(auto item = _first_item; item != nullptr; item = item->get_next()) {
            if (item->bounds().contains(x,y)) {
                return item;
            }
        }
        return nullptr;
    }

/**
 * Add a listener for a click on the item view.
 *
 * The click listeners are run after any selection process.
 */
    void add_click_listener(ViewItemClickListener<VIEWITEMTYPE> *listener)
    {
        if (_click_listeners.empty()) {
            win.add_mouse_click_listener(this);
        }
        _click_listeners.push_back(listener);
    }

/**
 * Remove a listener for a click on the item view
 */
    void remove_click_listener(ViewItemClickListener<VIEWITEMTYPE> *listener)
    {
        _click_listeners.remove(listener);
        if (_click_listeners.empty()) {
            win.remove_mouse_click_listener(this);
        }
    }

    virtual void mouse_click(tbx::MouseClickEvent &event)
    {
        tbx::WindowState state;
        win.get_state(state);
        tbx::Point hit_point(state.visible_area().work_x(event.x()), state.visible_area().work_y(event.y()));
        VIEWITEMTYPE *item = get_pointed_item(hit_point.x, hit_point.y);
//        Logger::debug("mouse_click item=%p %dx%d", item, event.x(), event.y());
        if (item != nullptr) {
            typename tbx::SafeList<ViewItemClickListener<VIEWITEMTYPE>>::Iterator i(_click_listeners);
            ViewItemClickListener<VIEWITEMTYPE> *l;
//            Logger::debug("mouse_click ITEM %d,%d %d,%d %d,%d", hit_point.x, hit_point.y, item->bounds().min.x, item->bounds().max.y, item->bounds().max.x, item->bounds().min.y);
            hit_point.x -= item->bounds().min.x;
            hit_point.y -= item->bounds().max.y;
            ViewItemClickEvent<VIEWITEMTYPE> ev(*item, hit_point, event);
            while ((l = i.next())!= 0) {
                //Logger::debug("l = %p", l );
                l->item_clicked(ev);
            }
            //Logger::debug("mouse_click item=%s %dx%d", item->value->title.c_str(), event.x(), event.y());
//            events.notify(ev);
        }
    }

    void add_selection_changed_listener(ViewSelectionChangedListener<VIEWITEMTYPE> *listener)
    {
        _selection_changed_listeners.push_back(listener);
    }

/**
 * Remove a listener for a click on the item view
 */
    void remove_selection_changed_listener(ViewSelectionChangedListener<VIEWITEMTYPE> *listener)
    {
        _selection_changed_listeners.remove(listener);
    }

    void fire_selection_changed(VIEWITEMTYPE* item)
    {
        typename tbx::SafeList<ViewSelectionChangedListener<VIEWITEMTYPE>>::Iterator i(_selection_changed_listeners);
        ViewSelectionChangedListener<VIEWITEMTYPE> *l;
        while ((l = i.next())!= 0)
        {
            l->selection_changed(item);
        }
    }

    void set_multiselect(bool multiselect_enabled) {
        is_multiselect = multiselect_enabled;
        if (!multiselect_enabled) {
            int num_selected = 0;
            bool selection_changed = false;
            for(auto item = _first_item; item != nullptr; item = item->get_next()) {
                if (item->selected) {
                    num_selected++;
                    if (num_selected > 1) {
                        item->selected = false;
                        win.update(item->bounds());
                        fire_selection_changed(item);
                    }
                }
            }
        }
    }

    void set_selected_all(bool selected) {
        bool selection_changed = false;
        for(auto item = _first_item; item != nullptr; item = item->get_next()) {
            if (item->selected != selected) {
                item->selected = selected;
                fire_selection_changed(item);
            }
        }
        update_visible();
    }

    void set_selected_item(const DATAITEMTYPE& value, bool selected) {
        bool selection_changed = false;
        if (selected && !is_multiselect) {
            for(auto item = _first_item; item != nullptr; item = item->get_next()) {
                if (item->value != value && item->selected == true) {
                    item->selected = false;
                    win.update(item->bounds());
                    fire_selection_changed(item);
                }
                if (item->value == value && item->selected == false) {
                    item->selected = true;
                    win.update(item->bounds());
                    fire_selection_changed(item);
                }
            }
        } else {
            for(auto item = _first_item; item != nullptr; item = item->get_next()) {
                if (item->value == value && item->selected != selected) {
                    item->selected = selected;
                    win.update(item->bounds());
                    fire_selection_changed(item);
                    break;
                }
            }
        }
    }

    std::vector<DATAITEMTYPE> get_selected_items() {
        std::vector<DATAITEMTYPE> selected_items;
        for(auto item = _first_item; item != nullptr; item = item->get_next()) {
            if (item->selected) {
                selected_items.push_back(item->value);
                if (!is_multiselect) {
                    break;
                }
            }
        }
        return selected_items;
    }

    void set_selected_items(const std::vector<DATAITEMTYPE>& selected_items) {
        bool must_be_selected;
        for(auto item = _first_item; item != nullptr; item = item->get_next()) {
            must_be_selected = false;
            for(auto &sel_item : selected_items) {
                must_be_selected = (item->value == sel_item);
                if (must_be_selected) break;
            }
            if (must_be_selected && !item->selected) {
                item->selected = true;
                win.update(item->bounds());
                fire_selection_changed(item);
            } else if (!must_be_selected && item->selected) {
                item->selected = false;
                win.update(item->bounds());
                fire_selection_changed(item);
            }
        }
    }

    void maintain_scroll_position(ScrollPosition pos) {
        _maintain_scroll_position = pos;
    }

    tbx::BBox update_window_extent() {
        tbx::BBox cur_extent = win.extent();
        tbx::BBox new_extent = recalc_layout(win.bounds(), cur_extent);
//        Logger::debug("update_window_extent %dx%d old extent %dx%d", new_extent.width(), new_extent.height(), cur_extent.width(), cur_extent.height());
        if (cur_extent.height() != new_extent.height() || cur_extent.width() != new_extent.width()) {
//            Logger::debug("update_window_extent %dx%d", new_extent.width(), new_extent.height());
            win.extent(new_extent);
            return new_extent;
        }
        return cur_extent;
    }

    template <class Iter>
    void reload_items(Iter it, Iter end, bool initial_load) {
        if (initial_load) {
            delete_all_view_items();
            pre_initial_load();
        }
        Logger::debug("reload_items initial_load=%d", initial_load);
        VIEWITEMTYPE* new_first_item = nullptr;
        VIEWITEMTYPE* cur_item = nullptr;
        for(; it != end; ++it) {
            VIEWITEMTYPE* existed_item = get_view_item(*it);
            if (existed_item) {
                existed_item->was_changed = true;
                if (existed_item->prev) {
                    existed_item->prev->next = existed_item->get_next();
                } else {
                    _first_item = existed_item->get_next();
                }
                if (existed_item->next) {
                    existed_item->next->prev = existed_item->get_prev();
                } else {
                    _last_item = existed_item->get_prev();
                }
                existed_item->prev = cur_item;
                existed_item->next = nullptr;
                if (cur_item == nullptr) {
                    cur_item = existed_item;
                    new_first_item = cur_item;
                } else {
                    cur_item->next = existed_item;
                    cur_item = existed_item;
                }
            } else {
                if (cur_item == nullptr) {
                    cur_item = new VIEWITEMTYPE(*it);
                    new_first_item = cur_item;
                } else {
                    cur_item->next = new VIEWITEMTYPE(*it);
                    cur_item->next->prev = cur_item;
                    cur_item = cur_item->get_next();
                }
                post_add_item(*cur_item, initial_load);
            }
        }
        if (!initial_load) {
            delete_all_view_items();
        }
        _first_item = new_first_item;
        _last_item = cur_item;

        if (_maintain_scroll_position == ScrollPosition::FIXED_FROM_BOTTOM) {
            // fixme! Fix when items appended instead of prepended
            tbx::Point scroll = win.scroll();
            tbx::BBox current_extent = win.extent();
            tbx::BBox new_extent = update_window_extent();
            scroll.y -= (new_extent.height() - current_extent.height());
            scroll.x = 0;
            Logger::debug("reload_items scroll fixed");
            win.scroll(scroll);
        } else if (_maintain_scroll_position == ScrollPosition::GO_TO_BOTTOM) {
            tbx::BBox new_extent = update_window_extent();
            Logger::debug("reload_items scroll to bottom");
            win.scroll(0, -new_extent.height());
        } else {
            tbx::Point scroll = win.scroll();
            Logger::debug("reload_items scroll=%d", scroll.y);
            update_window_extent();
            win.scroll(0, scroll.y);
        }
        Logger::debug("reload_items update_visible");
        update_visible();
//        update_visible_at_next_tick();
    }

    void append_item(DATAITEMTYPE data_item) {
        if (_last_item == nullptr) {
            _last_item = new VIEWITEMTYPE(data_item);
            _first_item = _last_item;
        } else {
            _last_item->next = new VIEWITEMTYPE(data_item);
            _last_item->next->prev = _last_item;
            _last_item = _last_item->get_next();
        }

        post_add_item(*_last_item, false);
        update_visible_at_next_tick();
//        tbx::BBox new_extent = update_window_extent();
//        if (_maintain_scroll_position == ScrollPosition::GO_TO_BOTTOM) {
//            win.scroll(0, -new_extent.height());
//        }
//        update_visible();
    }

//    void item_prepended(DATAITEMTYPE data_item) {
//        _items_list.push_front(VIEWITEMTYPE(data_item));
//
//        if (_maintain_scroll_position == ScrollPosition::FIXED_FROM_BOTTOM) {
//            tbx::Point scroll = win.scroll();
//            tbx::BBox current_extent = win.extent();
//            tbx::BBox new_extent = update_window_extent();
//            scroll.y -= (new_extent.height() - current_extent.height());
//            win.scroll(scroll);
//        } else if (_maintain_scroll_position == ScrollPosition::GO_TO_BOTTOM) {
//            tbx::BBox new_extent = update_window_extent();
//            win.scroll(0, -new_extent.height());
//        } else {
//            update_window_extent();
//        }
//        update_window();
//    }

    void delete_item(DATAITEMTYPE data_item) {
        VIEWITEMTYPE* existed_item = get_view_item(data_item);
        if (existed_item) {
//            Logger::debug("delete_item(DATAITEMTYPE data_item)");

            VIEWITEMTYPE* next = reinterpret_cast<VIEWITEMTYPE*>(existed_item->next);
            while(next != nullptr) {
                next->was_changed = true;
                next = reinterpret_cast<VIEWITEMTYPE*>(next->next);
            }

            delete_view_item(existed_item);
            if (_maintain_scroll_position == ScrollPosition::GO_TO_BOTTOM) {
                tbx::BBox new_extent = update_window_extent();
                win.scroll(0, -new_extent.height());
            } else {
                tbx::Point scroll = win.scroll();
                update_window_extent();
                win.scroll(0, scroll.y);
            }
//            Logger::debug("delete_item(DATAITEMTYPE data_item) update_visible");
            update_visible();
        }
    }

    void change_view_item(VIEWITEMTYPE* view_item) {
        view_item->was_changed = true;
        post_change_item(*view_item);

        bool bounds_changed = calc_item_bounds(*view_item, view_item->bounds().top_left(), view_item->bounds().size());
//        Logger::debug("bounds changed=%d %p",bounds_changed, view_item);
        if (!bounds_changed) {
            win.update(view_item->bounds());
            view_item->was_changed = false;
        } else {
            update_visible_at_next_tick();
        }
    }

    void change_item(DATAITEMTYPE data_item) {
//        Logger::debug("change_item");
        VIEWITEMTYPE* existed_item = get_view_item(data_item);
        if (existed_item) {
            change_view_item(existed_item);
        }
    }

    void do_maintain_scroll_position() {
        if (_maintain_scroll_position == ScrollPosition::GO_TO_BOTTOM) {
            tbx::BBox new_extent = update_window_extent();
            win.scroll(0, -new_extent.height());
        } else {
            tbx::Point scroll = win.scroll();
            Logger::debug("do_maintain_scroll_position scrolly=%d", scroll.y);
            update_window_extent();
            win.scroll(0, scroll.y);
        }
    }

    void update_visible_at_next_tick() {
//        Logger::debug("update_visible_at_next_tick 0");
        if (!_needs_update_at_next_tick) {
            _needs_update_at_next_tick = true;
//            Logger::debug("update_visible_at_next_tick");
            auto do_update_at_next_tick = [this]() {
//                Logger::debug("do update_visible_at_next_tick");
                do_maintain_scroll_position();
                update_visible();
                _needs_update_at_next_tick = false;
            };
            g_idle_task.run_at_next_idle(do_update_at_next_tick);
        }
    }

    void update_visible() {
        win.update(win.extent());
    }

    void scroll_to_item(const DATAITEMTYPE& data_item) {
        tbx::BBox extent = win.extent();
        if (extent.height() <= win.size().height) {
            _maintain_scroll_position = ScrollPosition::GO_TO_BOTTOM;
            return;
        }
        for(auto item = get_first_item(); item != nullptr; item = item->get_next()) {
            if (item->value == data_item) {
                Logger::debug("Scroll to %d", item->bounds().max.y);
                win.scroll(0, item->bounds().max.y);
                break;
            }
        }
    }

    void repaint_all() {
        for(auto item = _first_item; item != nullptr; item = item->get_next()) {
            item->was_changed = true;
        }
        update_visible_at_next_tick();
    }

    void repaint_all_immediate() {
        for(auto item = _first_item; item != nullptr; item = item->get_next()) {
            item->was_changed = true;
        }
        tbx::Point scroll = win.scroll();
        update_window_extent();
        win.scroll(0, scroll.y);
        update_visible();
    }
};

#endif //ROCHAT_BASEVIEWS_H
