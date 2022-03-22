//
// Created by lenz on 3/17/20.
//
#include <sstream>
#include <oslib/Toolboxtypes.h>
#include <tbx/mouseclicklistener.h>
#include <tbx/button.h>
#include <tbx/application.h>
#include <tbx/stringset.h>
#include <tbx/textarea.h>
#include <tbx/textchangedlistener.h>
#include <tbx/writablefield.h>
#include <tbx/actionbutton.h>
#include <tbx/radiobutton.h>
#include <tbx/posteventlistener.h>
#include <tbx/postpolllistener.h>
#include <tbx/keylistener.h>
#include "cloverleaf/IdleTask.h"
#include "cloverleaf/CLGraphics.h"
#include "cloverleaf/CLRadioButton.h"
#include "cloverleaf/Logger.h"
#include "cloverleaf/CLUtf8.h"
#include "EditProfileSettings.h"
#include "../service/IKConfig.h"
#include "TelegramRegisterOrChangePhoneDialog.h"
#include "DeleteAccountDialog.h"
#include "../global.h"
#include "../model/AppDataModel.h"

#define SETTINGS_NESTED_AVATAR_PADDING 32

class SettingsMock {
public:
    bool chats_settings__thumbnail = false;
    bool chats_settings__autoplay = false;

    bool privacy_settings__chatcubeid = false;
    bool privacy_settings__phone = false;
    bool privacy_settings__email = false;
    bool privacy_settings__public = true;
};
static SettingsMock settings;

class NotificationUnreadChangedListener : public tbx::TextChangedListener {
public:
    void text_changed(tbx::TextChangedEvent &event) override {
        tbx::StringSet gadget = event.id_block().self_component();
        std::string unread_age = "48";
        switch (gadget.selected_index()) {
            case 0:
                unread_age = "12";
                break;
            case 1:
                unread_age = "24";
                break;
            case 2:
                unread_age = "48";
                break;
            case 3:
                unread_age = "96";
                break;
            case 4:
                unread_age = "168";
                break;
            case 5:
                unread_age = "-1";
                break;
        }
        CLStringsMap data;
        data["notification_settings.unread_age"] = unread_age;
        g_app_data_model.change_my_profile(data);
    }
} notification_unread_changed;

EditProfileSettings* EditProfileSettings::instance = nullptr;

EditProfileSettings::EditProfileSettings(int tab) :
    main_win(tbx::Window("optionsMain")),
    left_win(tbx::Window("opt_left")),
    current_tab_id(tab) {
//    Logger::debug("EditProfileSettings::EditProfileSettings");

    my_pic_medium_changed_event = g_app_events.listen<AppEvents::MyChatcubeAvatarChanged>(
            std::bind(&EditProfileSettings::on_chatcube_avatar_changed, this, std::placeholders::_1));
    my_telegram_pic_changed_event = g_app_events.listen<AppEvents::MyTelegramAvatarChanged>(
            std::bind(&EditProfileSettings::on_telegram_avatar_changed, this, std::placeholders::_1));

    std::string tab_tmpl;
    for (int i = 0; i < tabs_count; i++) {
        tab_tmpl = "opt_tab" + to_string(i + 1);
        tabs_win[i] = tbx::Window(tab_tmpl);
        tabs_win[i].add_all_key_listener(this);
        tabs_win[i].add_all_mouse_click_listener(this);
    }

    // setup countries
    std::ostringstream imploded;
    int ci = 0;
    for (auto c : g_choices.get_countries()) {
        if (ci != 0) {
            imploded << ",";
        }
        imploded << c.name;
        ci++;
    }
    tbx::StringSet countries = tbx::StringSet(tabs_win[0].gadget(14));
    // version history set autoscroll
    //tbx::TextArea(tabs_win[5].gadget(6)).auto_scroll(true);

    countries.available(imploded.str());
//    countries.add_text_changed_listener(this);

    tbx::ActionButton(tabs_win[0].gadget(0x11)).add_selected_listener(this);
    tbx::ActionButton(tabs_win[0].gadget(0x14)).add_selected_listener(this);
    tbx::ActionButton(tabs_win[6].gadget(0xd)).add_selected_listener(this);
    tbx::ActionButton(tabs_win[6].gadget(0xe)).add_selected_listener(this);
    tbx::Button(tabs_win[0].gadget(0x12)).add_mouse_click_listener(this);

    tbx::StringSet(tabs_win[1].gadget(7)).add_text_changed_listener(&notification_unread_changed);

//    avatar.win.add_mouse_click_listener(this);
    left_win.add_all_mouse_click_listener(this);
    main_win.add_has_been_hidden_listener(this);

    tbx::RadioButton(left_win.gadget(current_tab_id)).on(true);
    fill_values();
    main_win.show_centered();
    setup_subwindows();
    refresh_avatars();

    auto do_setup = [this]() {
        setup_subwindows();
    };
    g_idle_task.run_at_next_idle(do_setup);
}

EditProfileSettings::~EditProfileSettings() {
    g_app_events.unlisten<AppEvents::MyChatcubeAvatarChanged>(my_pic_medium_changed_event);
    g_app_events.unlisten<AppEvents::MyTelegramAvatarChanged>(my_telegram_pic_changed_event);
    left_win.delete_object();
    main_win.delete_object();
    for(int i = 0; i < tabs_count; i++) {
        tabs_win[i].delete_object();
    }
    tbx::app()->set_post_event_listener(nullptr);
    instance = nullptr;
}

void EditProfileSettings::open(int tab) {
    if (instance) {
        instance->switch_tab(tab);
    } else {
        instance = new EditProfileSettings(tab);
    }
}

void EditProfileSettings::has_been_hidden(const tbx::EventInfo &event_info) {
    delete this;
}

void EditProfileSettings::switch_tab(int tab_id) {
//    Logger::debug("EditProfileSettings::switch_tab %d", tab_id);
    tabs_win[current_tab_id].hide();
    current_tab_id = tab_id;
    tbx::RadioButton(left_win.gadget(current_tab_id)).on(true);
//    Logger::debug("EditProfileSettings::switch_tab setup_subwindows %d", tab_id);
    setup_subwindows();
    refresh_avatars();
    fill_values();
}


void EditProfileSettings::setup_subwindows() {
//    Logger::debug("EditProfileSettings win = %d", current_tab_id);

    tbx::WindowState main_win_state;
    main_win.get_state(main_win_state);

    tbx::ShowSubWindowSpec left_subwindow;
    left_subwindow.wimp_parent = main_win.window_handle();
    left_subwindow.visible_area = main_win_state.visible_area();
//        left_subwindow.visible_area.bounds().min.x = 0;
    left_subwindow.flags = tbx::ALIGN_LEFT_VISIBLE_LEFT
                           | tbx::ALIGN_TOP_VISIBLE_TOP
                           | tbx::ALIGN_RIGHT_VISIBLE_LEFT
                           | tbx::ALIGN_BOTTOM_VISIBLE_BOTTOM
                           | tbx::ALIGN_Y_SCROLL_VISIBLE_TOP;
    left_win.show_as_subwindow(left_subwindow);

    tbx::ShowSubWindowSpec tab_subwindow;
    tbx::BBox left_bbox = left_win.outline();
    tab_subwindow.wimp_parent = main_win.window_handle();
    tab_subwindow.wimp_window = -1;
    tab_subwindow.visible_area = main_win_state.visible_area();
    tab_subwindow.visible_area.bounds().min.x += left_bbox.width();
//        tab_subwindow.visible_area.bounds().max.x = main_win_state.visible_area().bounds().width();
//        tab_subwindow.visible_area.bounds().min.x = tab_subwindow.visible_area.bounds().max.x - tabs_win[current_tab_id].outline().width();
    tab_subwindow.flags = 0
                          | tbx::ALIGN_LEFT_VISIBLE_LEFT
                          | tbx::ALIGN_RIGHT_VISIBLE_RIGHT
                          | tbx::ALIGN_TOP_VISIBLE_TOP
                          | tbx::ALIGN_BOTTOM_VISIBLE_BOTTOM;
    tabs_win[current_tab_id].show_as_subwindow(tab_subwindow);

//    if (current_tab_id == 0 || current_tab_id == 6) {
//        tbx::WindowState tab_win_state;
//        tabs_win[current_tab_id].get_state(tab_win_state);
//
//        tbx::ShowSubWindowSpec avatar_subwindow;
//        avatar_subwindow.wimp_parent = tabs_win[current_tab_id].window_handle();
//        avatar_subwindow.wimp_window = -1;
//        avatar_subwindow.visible_area = tab_win_state.visible_area();
//        avatar_subwindow.visible_area.bounds().min.x += SETTINGS_NESTED_AVATAR_PADDING;
//        avatar_subwindow.visible_area.bounds().max.y -= SETTINGS_NESTED_AVATAR_PADDING;
//        avatar_subwindow.visible_area.bounds().min.y -= (SETTINGS_NESTED_AVATAR_PADDING + avatar.win.bounds().height());
//        avatar_subwindow.visible_area.bounds().max.x += SETTINGS_NESTED_AVATAR_PADDING + avatar.win.bounds().width();
//        avatar_subwindow.flags = 0
//                                 | tbx::ALIGN_LEFT_VISIBLE_LEFT
//                                 | tbx::ALIGN_RIGHT_VISIBLE_LEFT
//                                 | tbx::ALIGN_TOP_VISIBLE_TOP
//                                 | tbx::ALIGN_BOTTOM_VISIBLE_TOP;
//        avatar.win.show_as_subwindow(avatar_subwindow);
//    }
}

void EditProfileSettings::refresh_avatars() {
//    Logger::debug("EditProfileSettings::refresh_avatars");
    tbx::Window &current_win = tabs_win[current_tab_id];
    if (current_tab_id == 0) {
        tbx::Button(current_win.gadget(0x12)).value("prof_empty");
        Logger::debug("EditProfileSettings::refresh_avatars pic_medium_cached %s", g_app_data_model.get_my_member_data()->pic_medium_cached.c_str());
        if (!g_app_data_model.get_my_member_data()->pic_medium_cached.empty()) {
            CLImage profile(g_app_data_model.get_my_member_data()->pic_medium_cached);
            Logger::debug("EditProfileSettings::refresh_avatars CLImage profile [%dx%d]", profile.width(), profile.height());
            profile.plot_to_app_sprite("prof_my", tbx::Colour::white);
            tbx::Button(current_win.gadget(0x12)).value("prof_my");
        }
    }
    if (current_tab_id == 6) {
        tbx::Button(current_win.gadget(0xf)).value("prof_empty");
        if (g_app_data_model.get_my_member_data()->has_telegram()) {
            if (!g_app_data_model.get_my_member_data()->telegram_account.pic_cached.empty()) {
                CLImage profile(g_app_data_model.get_my_member_data()->telegram_account.pic_cached);
                profile.plot_to_app_sprite("prof_my", tbx::Colour::white);
                tbx::Button(current_win.gadget(0xf)).value("prof_my");
            }
        }
    }
}

void EditProfileSettings::fill_values() {
    MyMemberDataPtr me = g_app_data_model.get_my_member_data();
    tbx::Window &current_win = tabs_win[current_tab_id];
    switch (current_tab_id)  {
        case 0: //basic tab
            tbx::WritableField(current_win.gadget(7)).text(utf8_to_riscos_local(me->first_name));
            tbx::WritableField(current_win.gadget(8)).text(utf8_to_riscos_local(me->last_name));
            tbx::WritableField(current_win.gadget(9)).text(utf8_to_riscos_local(me->displayname));
            tbx::WritableField(current_win.gadget(10)).text(me->userid);
            tbx::WritableField(current_win.gadget(11)).text(me->email);
            tbx::WritableField(current_win.gadget(12)).text(me->phone);
            tbx::WritableField(current_win.gadget(13)).text(utf8_to_riscos_local(me->city));
            tbx::WritableField(current_win.gadget(0x15)).text(utf8_to_riscos_local(me->website));
            if (me->country) {
                tbx::StringSet(current_win.gadget(14)).selected_index(me->country->index);
            }
            break;

        case 1: //notification tab
            CLRadioButton(current_win.gadget(0)).on(me->notification_settings.popup);
//            CLRadioButton(current_win.gadget(1)).on(me->notification_settings.taskbar);
            CLRadioButton(current_win.gadget(2)).on(me->notification_settings.sound);
//            CLRadioButton(current_win.gadget(0)).fade();
//            CLRadioButton(current_win.gadget(1)).fade();

            CLRadioButton(current_win.gadget(4)).on(IKConfig::get_value("general", "start_hidden", 0));

            switch(me->notification_settings.unread_age) {
                case 12:
                    tbx::StringSet(current_win.gadget(7)).selected_index(0);
                    break;
                case 24:
                    tbx::StringSet(current_win.gadget(7)).selected_index(1);
                    break;
                case 48:
                    tbx::StringSet(current_win.gadget(7)).selected_index(2);
                    break;
                case 96:
                    tbx::StringSet(current_win.gadget(7)).selected_index(3);
                    break;
                case 168:
                    tbx::StringSet(current_win.gadget(7)).selected_index(4);
                    break;
                case -1:
                    tbx::StringSet(current_win.gadget(7)).selected_index(3);
                    break;
            }
            break;
        case 2: //_chat tab
            // set checkboxes states
//            CLRadioButton(current_win.gadget(1)).on(settings.chats_settings__thumbnail);
//            CLRadioButton(current_win.gadget(2)).on(settings.chats_settings__autoplay);
            current_win.gadget(1).fade();
            current_win.gadget(2).fade();
            current_win.gadget(5).fade();
            break;
        case 4:
            // set checkboxes states
//            CLRadioButton(current_win.gadget(0)).on(settings.privacy_settings__chatcubeid);
//            CLRadioButton(current_win.gadget(1)).on(settings.privacy_settings__phone);
//            CLRadioButton(current_win.gadget(2)).on(settings.privacy_settings__email);
//            CLRadioButton(current_win.gadget(3)).on(settings.privacy_settings__public);
            CLRadioButton(current_win.gadget(0)).fade();
            CLRadioButton(current_win.gadget(1)).fade();
            CLRadioButton(current_win.gadget(2)).fade();
            CLRadioButton(current_win.gadget(3)).fade();
            break;
        case 5: // version
            if (is_file_exist("<ChatCube$Dir>.CHANGELOG")) {
                tbx::TextArea ta = current_win.gadget(6);
                ta.set_colour(tbx::Colour(0), tbx::Colour(0xEF, 0xED, 0xED));
                ta.allow_selection(false);
                ta.text(get_file_contents("<ChatCube$Dir>.CHANGELOG"));
                ta.set_cursor_position(0, 1);
            }
            char latest_version[30];
            char current_version[30];
            sprintf(latest_version, "Latest version %s", g_app_data_model.get_latest_app_version().c_str());
            sprintf(current_version, "Your version %s", g_app_data_model.get_app_version().c_str());
            tbx::Button(current_win.gadget(2)).value(latest_version);
            tbx::Button(current_win.gadget(3)).value(current_version);
            current_win.gadget(4).fade(strcmp(latest_version, current_version) == 0);
            break;
        case 6: // telegram
            if (!g_app_data_model.get_my_member_data()->has_telegram()) {
                current_win.gadget(7).fade();
                current_win.gadget(8).fade();
                current_win.gadget(9).fade();
                current_win.gadget(10).fade();
                current_win.gadget(0xe).fade();
                int result = show_question("You don't have associated Telegram account yet! Do you want to create or add Telegram?","Add Telegram,Cancel");
                if (result == 3) {
                    main_win.hide();
                    new TelegramRegisterOrChangePhoneDialog();
                }
            } else {
                Logger::debug("me->telegram_account.first_name %s", me->telegram_account.first_name.c_str());
                current_win.gadget(7).unfade();
                current_win.gadget(8).unfade();
                current_win.gadget(9).unfade();
                current_win.gadget(10).unfade();
                current_win.gadget(0xe).unfade();
                tbx::WritableField(current_win.gadget(7)).text(utf8_to_riscos_local(me->telegram_account.first_name));
                tbx::WritableField(current_win.gadget(8)).text(utf8_to_riscos_local(me->telegram_account.last_name));
                tbx::WritableField(current_win.gadget(9)).text(me->telegram_account.username);
                tbx::WritableField(current_win.gadget(10)).text(me->telegram_account.phone);
            }
            break;
    }

}

void EditProfileSettings::key(tbx::KeyEvent &event) {
    //Logger::debug("EditProfileSettings::key win=%d cmp=%d", event.id_block().self_object().handle(), event.gadget().id());
    if (event.key() == wimp_KEY_TAB) {
        tbx::Window win = tbx::Window(event.id_block().self_object());
        if (win == tabs_win[current_tab_id] && event.gadget().toolbox_class() == tbx::WritableField::TOOLBOX_CLASS) {
            tbx::WritableField fld(event.gadget());
//            std::string val = fld.text();
//            CLStringsMap data;
            if (current_tab_id == 0) { // basic
                switch (fld.id()) {
                    case 7:
//                        data["first_name"] = val;
//                        g_app_data_model.change_my_profile(data);
                        win.gadget(8).focus();
                        break;
                    case 8:
//                        data["last_name"] = val;
//                        g_app_data_model.change_my_profile(data);
                        win.gadget(9).focus();
                        break;
                    case 9:
//                        data["displayname"] = val;
//                        g_app_data_model.change_my_profile(data);
                        win.gadget(10).focus();
                        break;
                    case 10:
//                        data["userid"] = val;
//                        g_app_data_model.change_my_profile(data);
                        win.gadget(11).focus();
                        break;
                    case 11:
//                        data["email"] = val;
//                        g_app_data_model.change_my_profile(data);
                        win.gadget(12).focus();
                        break;
                    case 12:
//                        data["phone"] = val;
//                        g_app_data_model.change_my_profile(data);
                        win.gadget(13).focus();
                        break;
                    case 13:
//                        data["city"] = val;
//                        g_app_data_model.change_my_profile(data);
                        win.gadget(14).focus();
                        break;
                }
            }
            if (current_tab_id == 6) { // telegram
                switch (fld.id()) {
                    case 7:
//                        data["telegram_account.first_name"] = val;
//                        g_app_data_model.change_my_profile(data);
                        win.gadget(8).focus();
                        break;
                    case 8:
//                        data["telegram_account.last_name"] = val;
//                        g_app_data_model.change_my_profile(data);
                        win.gadget(9).focus();
                        break;
                    case 9:
//                        data["telegram_account.username"] = val;
//                        g_app_data_model.change_my_profile(data);
                        win.gadget(7).focus();
                        break;
                }
            }
        }
    }
}

void EditProfileSettings::mouse_click(tbx::MouseClickEvent &event) {
    tbx::Window win = tbx::Window(event.id_block().self_object());
    Logger::debug("EditProfileSettings::mouse_click win=%d cmp=%d", win.handle(), event.id_block().self_component().id());

    if (event.id_block().self_component().id() == -1) {
        return;
    }

    tbx::Gadget cmp(event.id_block().self_component());
    auto cmp_id = cmp.id();
    if (win == left_win) {
        switch_tab(cmp_id);
        return;
    }
    if (win == tabs_win[current_tab_id] && cmp_id != -1 && cmp.toolbox_class() == tbx::ActionButton::TOOLBOX_CLASS) {
        if (current_tab_id == 6) {
            if (cmp_id == 0xc) {
                main_win.hide();
                new TelegramRegisterOrChangePhoneDialog();
            }
        }
        return;
    }
    if (win == tabs_win[current_tab_id] && cmp_id != -1 && cmp.toolbox_class() == tbx::Button::TOOLBOX_CLASS) {
        tbx::Button btn = cmp;
        switch(btn.button_type()) {
            case 3:
                switch(current_tab_id) {
                    case 0:
                        if (cmp_id == 0x12 || cmp_id == 0x13) {
                            show_avatar_changer(MESSENGER_CHATCUBE);
                        }
                        break;
                    case 6:
                        if (cmp_id == 0x10 || cmp_id == 0xf) {
                            show_avatar_changer(MESSENGER_TELEGRAM);
                        }
                        break;
                }
            case 11: { // checkboxes
                CLRadioButton fld(cmp);
                switch(current_tab_id) {
                    case 1: // notifications
                    {
                        CLStringsMap data;
                        MyMemberDataPtr me = g_app_data_model.get_my_member_data();
                        switch (cmp_id) {
                            case 0:
                                data["notification_settings.popup"] = me->notification_settings.popup ? "0" : "1";
                                g_app_data_model.change_my_profile(data);
                                break;
                            case 1:
                                data["notification_settings.taskbar"] = me->notification_settings.taskbar ? "0" : "1";
                                g_app_data_model.change_my_profile(data);
                                break;
                            case 2:
                                data["notification_settings.sound"] = me->notification_settings.sound ? "0" : "1";
                                g_app_data_model.change_my_profile(data);
                                break;
                            case 4:
                                IKConfig::set_value("general", "start_hidden",
                                                    IKConfig::get_value("general", "start_hidden", 0) ? 0 : 1);
                                break;
                        }
                    }
                        break;
                    case 2: // chats
                        switch (cmp_id) {
                            case 1:
                                settings.chats_settings__thumbnail = !settings.chats_settings__thumbnail;
                                break;
                            case 2:
                                settings.chats_settings__autoplay = !settings.chats_settings__autoplay;
                                break;
                        }
                        break;
                    case 4: // privacy tab
                        switch (cmp_id) {
                            case 0:
                                settings.privacy_settings__chatcubeid = !settings.privacy_settings__chatcubeid;
                                break;
                            case 1:
                                settings.privacy_settings__phone = !settings.privacy_settings__phone;
                                break;
                            case 2:
                                settings.privacy_settings__email = !settings.privacy_settings__email;
                                break;
                            case 3:
                                settings.privacy_settings__public = !settings.privacy_settings__public;
                                break;
                        }
                        break;
                    case 5:  // version
                        if (fld.id() == 4) {
                            if (g_app_data_model.get_latest_app_version() != g_app_data_model.get_app_version()) {
                                g_app_data_model.update_app();
                            } else {
                                show_alert_info("You already have latest version. No need to update.");
                            }
                        }
                        break;
                }
            }
            break;
        }
    }
}

//void EditProfileSettings::text_changed(tbx::TextChangedEvent &event) {
//    if (current_tab_id == 0 && event.id_block().self_component().id() == 14) {
//        int select_index = tbx::StringSet(tabs_win[current_tab_id].gadget(14)).selected_index();
//        std::vector<CountryData> countries = g_choices.get_countries();
//        if (select_index >= 0 && select_index < countries.size()) {
//            auto callback = [](const MyMemberDataPtr me) {};
//            CLStringsMap data;
//            data["country"] = countries[select_index].code;
//            g_app_data_model.change_my_profile(data);
//        }
//    }
//}

void EditProfileSettings::button_selected(tbx::ButtonSelectedEvent &event) {
    CLStringsMap data;
    tbx::Window &w = tabs_win[current_tab_id];
    auto cmp_id = event.id_block().self_component().id();

    auto on_saved = [](MyMemberDataPtr me) {
        show_alert_info("Changes are saved.");
    };

    if (current_tab_id == 0) {
        if (cmp_id == 0x11) {
            int select_index = tbx::StringSet(w.gadget(14)).selected_index();
            std::vector<CountryData> countries = g_choices.get_countries();
            if (select_index >= 0 && select_index < countries.size()) {
                auto callback = [](const MyMemberDataPtr me) {};
                data["country"] = countries[select_index].code;
            }

            data["first_name"] = tbx::WritableField(w.gadget(7)).text();
            data["last_name"] = tbx::WritableField(w.gadget(8)).text();
            data["displayname"] = tbx::WritableField(w.gadget(9)).text();
            data["userid"] = tbx::WritableField(w.gadget(10)).text();
            data["email"] = tbx::WritableField(w.gadget(11)).text();
            data["phone"] = tbx::WritableField(w.gadget(12)).text();
            data["city"] = tbx::WritableField(w.gadget(13)).text();
            data["website"] = tbx::WritableField(w.gadget(0x15)).text();

            g_app_data_model.change_my_profile(data, on_saved);
        } else if (cmp_id == 0x14) {
            main_win.hide();
            new DeleteAccountDialog();
        }
    } else if(current_tab_id == 6)  {
        if (cmp_id == 0xd) {
            data["telegram_account.first_name"] = tbx::WritableField(w.gadget(7)).text();;
            data["telegram_account.last_name"] = tbx::WritableField(w.gadget(8)).text();;
            data["telegram_account.username"] = tbx::WritableField(w.gadget(9)).text();;

            g_app_data_model.change_my_profile(data, on_saved);
        } else if (cmp_id == 0xe) {
            int result = show_question("Are you sure to unregister your Telegram account from ChatCube?", "Yes,No");
            if (result == 3) {
                auto on_success = [this]() {
                    main_win.hide();
                    show_alert_info("Telegram account unregistered successfully");
                };
                g_app_data_model.telegram_unregister_account(on_success);
            }
        }
    }
}
