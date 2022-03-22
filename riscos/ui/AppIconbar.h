//
// Created by lenz on 3/16/20.
//

#ifndef ROCHAT_APPICONBAR_H
#define ROCHAT_APPICONBAR_H

#include "../model/AppEvents.h"
#include <tbx/iconbar.h>
#include <tbx/autocreatelistener.h>

class AppIconbar {
    static AppIconbar* _instance;
    std::string current_icon;
    tbx::Iconbar iconbar;
public:
    static AppIconbar *instance() { return _instance; };

    AppIconbar(tbx::Object obj);
    void update_appicon();

    static void on_chat_list_loaded(const AppEvents::ChatListLoaded& ev);
    static void on_chat_added(const AppEvents::ChatAdded& ev);
    static void on_chat_changed(const AppEvents::ChatChanged& ev);
    static void on_chat_deleted(const AppEvents::ChatDeleted& ev);
    static void on_connection_changed(const AppEvents::ConnectionStateChanged& ev);
};


#endif //ROCHAT_APPICONBAR_H
