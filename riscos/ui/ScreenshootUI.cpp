//
// Created by lenz on 3/25/20.
//

#include <string>
#include <tbx/window.h>
#include <tbx/timer.h>
#include <tbx/toolaction.h>
#include <tbx/questionwindow.h>
#include <tbx/modeinfo.h>
#include <tbx/pointerinfo.h>
#include <tbx/draghandler.h>
#include <tbx/swixcheck.h>
#include "oslib/compressjpeg.h"
#include <cloverleaf/CLException.h>
#include <cloverleaf/Logger.h>
#include "cloverleaf/rosprite.h"
#include "cloverleaf/CLImageJPGLoader.h"
#include "../utils.h"
#include "ScreenshootUI.h"
#include "ChatMainUI.h"

const static char* _screenshoot_file = "<Choices$Write>.ChatCube.cache.screenshoot";

static void save_screenshot(struct rosprite* sprite) {
    Logger::debug("screenshoot sprite %dx%d", sprite->width, sprite->height);

    CLImageJPGLoader::save(sprite, _screenshoot_file, 75);
    set_file_type(_screenshoot_file, 0xC85);
}


class SendScreenshootCmd : public tbx::Command {
public:
    void execute() override {
        g_app_data_model.send_message_photo(_screenshoot_file, "");
    }
} send_screenshoot_cmd;

class ConfirmSendClosedListener : public tbx::HasBeenHiddenListener {
public:
    void has_been_hidden(const tbx::EventInfo &event_info) override {
//        Logger::debug("ConfirmSendClosedListener has_been_hidden");
        event_info.id_block().self_object().delete_object();
    }
} confirm_send_close_listener;

static void ask_send_screesnhoot() {
    tbx::Window win("confirmSend");
    win.add_has_been_hidden_listener(&confirm_send_close_listener);
    tbx::ActionButton(win.gadget(2)).add_selected_command(&send_screenshoot_cmd);
    win.show_centered();
};


static void capture_screenshoot(tbx::BBox box) {
    struct rosprite* sprite;
    tbx::Application::instance()->yield();
    box.normalise();
    if (rosprite_capture_screenshoot(&sprite, box.min.x/2, box.min.y/2, box.max.x/2, box.max.y/2) == ROSPRITE_OK) {
        save_screenshot(sprite);
        rosprite_destroy_sprite(sprite);
        ChatMainUI::open();
        ask_send_screesnhoot();
    } else {
        ChatMainUI::open();
    };
}

class ScreenshootAreaSelector : public tbx::DragHandler, public tbx::KeyListener, public tbx::MouseClickListener {
public:
    tbx::Window win;

    ScreenshootAreaSelector() :
            win("transparent")
    {
        tbx::ModeInfo mi;
        tbx::Size screen_size = mi.screen_size();
        tbx::WindowState state;

        win.add_all_key_listener(this);
        win.add_all_mouse_click_listener(this);

        win.get_state(state);
        state.visible_area().bounds() = tbx::BBox(0,0,screen_size.width, screen_size.height);
        win.open_window(state);
    }

    ~ScreenshootAreaSelector() {
//        Logger::debug("~ScreenshootAreaSelector");
        win.delete_object();
    }

    void key(tbx::KeyEvent &event) override {
//        Logger::debug("ScreenshootAreaSelector::key");
        if (event.key() == tbx::KeyEvent::SpecialKeys::Escape) {
//            Logger::debug("ScreenshootAreaSelector::key cancel");
            win.cancel_drag();
            event.key_used();
        }
    }

    void drag_cancelled()  override {
//        Logger::debug("ScreenshootAreaSelector::drag_cancelled");
        delete this;
    }

    void drag_finished(const tbx::BBox &final) override  {
//        Logger::debug("ScreenshootAreaSelector::drag_finished");
        win.hide();
        capture_screenshoot(final);
        delete this;
    }

    void mouse_click(tbx::MouseClickEvent &event) override {
        if (event.is_select()) {
//            Logger::debug("win.drag_rubber_box %d,%d", event.point().x, event.point().y);
            win.drag_rubber_box(event.point(), this);
        }
    }
};


class ScreenshootWindowSelector : public tbx::KeyListener, public tbx::MouseClickListener, public tbx::Timer {
private:
    wimp_w selected_window;
    wimp_w selector_window;
public:
    static ScreenshootWindowSelector* instance;
    tbx::Window win;

    ScreenshootWindowSelector() :
            win("screenshoot")
    {
        instance = this;
        tbx::ModeInfo mi;
        tbx::Size screen_size = mi.screen_size();
        tbx::WindowState state;
        selector_window = (wimp_w) win.window_handle();

        win.add_key_listener(this);
        win.add_mouse_click_listener(this);
//        prev_post_poll_listener = tbx::app()->get_post_poll_listener();
//        tbx::app()->set_post_poll_listener(this);
        tbx::app()->add_timer(2, this);

        win.get_state(state);
        state.visible_area().bounds() = tbx::BBox(0,0,0,0);
        win.open_window(state);
    }

    ~ScreenshootWindowSelector() {
        Logger::debug("~ScreenshootWindowSelector");
//        tbx::app()->set_post_poll_listener((PostPollListener *)prev_post_poll_listener);
        tbx::app()->remove_timer(this);
        win.delete_object();
        instance = nullptr;
    }

    void key(tbx::KeyEvent &event) override {
//        Logger::debug("ScreenshootAreaSelector::key");
        if (event.key() == tbx::KeyEvent::SpecialKeys::Escape) {
//            Logger::debug("ScreenshootAreaSelector::key cancel");
            event.key_used();
            ChatMainUI::open();
            delete this;
        }
    }

    void mouse_click(tbx::MouseClickEvent &event) override {
        if (event.is_select()) {
            wimp_outline o;
            o.w = selected_window;
            tbx::swix_check((_kernel_oserror*)xwimp_get_window_outline(&o));
//                    Logger::debug("ScreenshootWindowSelector xwimp_get_window_outline");
            tbx::BBox obox = tbx::BBox(o.outline.x0, o.outline.y0, o.outline.x1, o.outline.y1);
            win.hide();
            capture_screenshoot(obox);
            Logger::debug("ScreenshootWindowSelector capture_screenshoot %dx%d", obox.width(), obox.height());
            delete this;
        }
    }

    void timer(unsigned int elapsed) override {
        wimp_pointer pointer;
        os_error *err = xwimp_get_pointer_info(&pointer);
//        Logger::debug("ScreenshootWindowSelector xwimp_get_pointer_info win:%x %dx%d", pointer.w, pointer.pos.x, pointer.pos.y);
        if (!err && pointer.w != selector_window) {
            tbx::WindowState area_state;
            win.get_state(area_state);
            area_state.visible_area().bounds() = tbx::BBox(0,0,0,0);
            win.open_window(area_state);

            if (pointer.w >= 0) {
                wimp_window_state state;
                state.w = pointer.w;
                err = xwimp_get_window_state(&state);
//                Logger::debug("ScreenshootWindowSelector xwimp_get_window_info flags: %x %p", state.flags, err);
                if (!err && (state.flags & (1 << 26)) && ((state.flags & (1 << 25)) || (state.flags & (1 << 24)))) {
                    selected_window = pointer.w;
                    area_state.visible_area().bounds() = tbx::BBox(state.visible.x0, state.visible.y0, state.visible.x1, state.visible.y1);
                    auto update_window = [this, area_state]() {
                        if (instance) {
                            win.open_window(area_state);
                        }
                    };
                    g_idle_task.run_at_next_idle(update_window);
                }
            }
        }
    }
};
ScreenshootWindowSelector* ScreenshootWindowSelector::instance = nullptr;

void screenshoot(ScreenshootType scrsh_type) {
    tbx::ModeInfo m;

    switch(scrsh_type) {
        case ScreenshootType::FULL:
            capture_screenshoot(tbx::BBox(tbx::Point(0,0), m.screen_size()));
            break;
        case ScreenshootType::HIDE_FULL:
            ChatMainUI::hide();
            capture_screenshoot(tbx::BBox(tbx::Point(0,0), m.screen_size()));
            break;
        case ScreenshootType::AREA:
            new ScreenshootAreaSelector();
            break;
        case ScreenshootType::HIDE_AREA:
            ChatMainUI::hide();
            tbx::Application::instance()->yield();
            new ScreenshootAreaSelector();
            break;
        case ScreenshootType::WINDOW:
            ChatMainUI::hide();
//            tbx::Application::instance()->yield();
            new ScreenshootWindowSelector;
            break;
    }
}
