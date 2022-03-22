//
// Created by lenz on 3/16/20.
//

#include "AppIconbar.h"
#include "../model/AppDataModel.h"
#include "../utils.h"

AppIconbar* AppIconbar::_instance = 0;

AppIconbar::AppIconbar(tbx::Object object) : iconbar(object) {
    _instance = this;

    g_app_events.listen<AppEvents::ChatListLoaded>(&AppIconbar::on_chat_list_loaded);
    g_app_events.listen<AppEvents::ChatAdded>(&AppIconbar::on_chat_added);
    g_app_events.listen<AppEvents::ChatChanged>(&AppIconbar::on_chat_changed);
    g_app_events.listen<AppEvents::ChatDeleted>(&AppIconbar::on_chat_deleted);
    g_app_events.listen<AppEvents::ConnectionStateChanged>(&AppIconbar::on_connection_changed);
}

void AppIconbar::update_appicon() {
    std::string newicon = "!chatcube";
    if (g_http_service.connected()) {
        int unread = 0;
        for (auto chat : g_app_data_model.get_chats_list()) {
            unread += chat->unread_count;
        }
        if (unread > 0) {
            if (unread > 9) {
                newicon += "+";
            } else {
                newicon += to_string(unread);
            }
        }
    } else {
        newicon = "!chatcube_nc";
    }

    if (current_icon != newicon) {
        iconbar.sprite("!chatcube-");
        iconbar.sprite(newicon);
        current_icon = newicon;
    }
}

void AppIconbar::on_chat_list_loaded(const AppEvents::ChatListLoaded& ev) {
    instance()->update_appicon();
}

void AppIconbar::on_chat_added(const AppEvents::ChatAdded& ev) {
    instance()->update_appicon();
}

void AppIconbar::on_chat_changed(const AppEvents::ChatChanged& ev) {
    if (ev.changes & CHAT_CHANGES_UNREAD_COUNT) {
        instance()->update_appicon();
    }
}

void AppIconbar::on_chat_deleted(const AppEvents::ChatDeleted& ev) {
    instance()->update_appicon();
}

void AppIconbar::on_connection_changed(const AppEvents::ConnectionStateChanged& ev) {
    instance()->update_appicon();
}