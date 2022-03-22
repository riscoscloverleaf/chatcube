//
// Created by lenz on 3/17/20.
//

#ifndef ROCHAT_EDITPROFILESETTINGS_H
#define ROCHAT_EDITPROFILESETTINGS_H

#include <tbx/window.h>
#include <tbx/hasbeenhiddenlistener.h>
#include <tbx/redrawlistener.h>
#include <tbx/keylistener.h>
#include <tbx/textchangedlistener.h>
#include <tbx/buttonselectedlistener.h>
#include <tbx/jpeg.h>
#include <cloverleaf/CLImage.h>
#include "CallbackListeners.h"
#include "AvatarChanger.h"
#include "../model/AppEvents.h"

class EditProfileSettings : public tbx::HasBeenHiddenListener, tbx::ButtonSelectedListener,
                            tbx::KeyListener,
                            tbx::MouseClickListener {
private:
    static const int tabs_count = 7;
    static EditProfileSettings *instance;
    int my_pic_medium_changed_event;
    int my_telegram_pic_changed_event;

public:
    static const int TAB_PROFILE=0;
    static const int TAB_GENERAL=1;
    static const int TAB_CHATS=2;
    static const int TAB_PRIVACY=4;
    static const int TAB_VERSION=5;
    static const int TAB_TELEGRAM=6;

    int current_tab_id = 1;
    tbx::Window tabs_win[tabs_count];
    tbx::Window main_win;
    tbx::Window left_win;

    EditProfileSettings(int tab);
    ~EditProfileSettings();
    static void open(int tab);
    void switch_tab(int tab_id);
    void has_been_hidden(const tbx::EventInfo &event_info) override;
    void setup_subwindows();
    void refresh_avatars();
    void fill_values();

    void mouse_click(tbx::MouseClickEvent &event) override;
    void key(tbx::KeyEvent &event) override ;
//    void text_changed(tbx::TextChangedEvent &event) override;

private:
    void button_selected(tbx::ButtonSelectedEvent &event) override;

public:
    void on_chatcube_avatar_changed(const AppEvents::MyChatcubeAvatarChanged& ev) {
//        Logger::debug("on_chatcube_avatar_changed");
        refresh_avatars();
    }
    void on_telegram_avatar_changed(const AppEvents::MyTelegramAvatarChanged& ev) {
//        Logger::debug("on_telegram_avatar_changed");
        refresh_avatars();
    }
};

#endif //ROCHAT_EDITPROFILESETTINGS_H
