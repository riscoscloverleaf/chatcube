//
// Created by lenz on 3/16/20.
//

#include "AppMenu.h"
#include "../model/AppDataModel.h"
#include "../service/CLHTTPService_v2.h"
#include <tbx/application.h>

MainMenu::MainMenu(tbx::Object object) {
    menu = tbx::Menu(object);
    menu.add_about_to_be_shown_listener(this);
}

void MainMenu::about_to_be_shown(tbx::AboutToBeShownEvent &event) {
    menu.item(3).fade( !g_app_data_model.is_logged_in() || !g_http_service.connected()); // settings
    menu.item(4).fade( !g_app_data_model.is_logged_in() || !g_http_service.connected()); // logout
    menu.item(5).fade( !g_app_data_model.is_logged_in() || !g_http_service.connected()); // add messenger
    menu.item(6).fade( !g_app_data_model.is_logged_in() || !g_http_service.connected()); // new chat
//    menu.item(8).fade( !g_app_data_model.is_logged_in() || !g_http_service.connected()); // feedback
}

MessengersMenu::MessengersMenu(tbx::Object object) {
    menu = tbx::Menu(object);
    menu.add_about_to_be_shown_listener(this);
}

void MessengersMenu::about_to_be_shown(tbx::AboutToBeShownEvent &event) {
    menu.item(CID_TELEGRAM).fade(
            (!g_app_data_model.is_logged_in())
            );
}

CreateChatMenu::CreateChatMenu(tbx::Object object) {
    menu = tbx::Menu(object);
    menu.add_about_to_be_shown_listener(this);
}

void CreateChatMenu::about_to_be_shown(tbx::AboutToBeShownEvent &event) {
    menu.item(CID_TELEGRAM).fade(
            !g_app_data_model.is_logged_in() || !g_app_data_model.get_my_member_data()->has_telegram()
            );
}
