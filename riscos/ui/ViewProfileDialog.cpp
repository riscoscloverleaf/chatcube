//
// Created by lenz on 11/16/20.
//
#include <oslib/Toolboxtypes.h>
#include <tbx/button.h>
#include <tbx/displayfield.h>
#include <cloverleaf/CLImage.h>
#include <cloverleaf/CLUtf8.h>
#include <cloverleaf/Logger.h>
#include "ViewProfileDialog.h"
#include "../global.h"

ViewProfileDialog::ViewProfileDialog(MemberDataPtr mem) :
    win("ProfileInfo")
{
    win.add_has_been_hidden_listener(this);
    tbx::Button(win.gadget(1)).add_mouse_click_listener(this);
    update(mem);
}

ViewProfileDialog::~ViewProfileDialog() {
    _instance = nullptr;
    win.delete_object();
}

void ViewProfileDialog::update(MemberDataPtr mem) {
    g_hourglass_on();
    std::string title_suffix;
    member = mem;
    switch(member->messenger()) {
        case MESSENGER_CHATCUBE:
            title_suffix = " (ChatCube)";
            break;
        case MESSENGER_TELEGRAM:
            title_suffix = " (Telegram)";
            break;
    }
    win.title("Profile: " + utf8_to_riscos_local(member->displayname) + title_suffix);
    tbx::DisplayField(win.gadget(0x12)).text(utf8_to_riscos_local(mem->first_name));
    tbx::DisplayField(win.gadget(0x14)).text(utf8_to_riscos_local(mem->last_name));
    tbx::DisplayField(win.gadget(0x16)).text(utf8_to_riscos_local(mem->displayname));
    tbx::DisplayField(win.gadget(0x18)).text(utf8_to_riscos_local(mem->userid));
    tbx::DisplayField(win.gadget(0x1a)).text(utf8_to_riscos_local(mem->city));
    tbx::DisplayField(win.gadget(0x1e)).text(utf8_to_riscos_local(mem->website));
    if (mem->country) {
        tbx::DisplayField(win.gadget(0x1c)).text(mem->country->name);
    } else {
        tbx::DisplayField(win.gadget(0x1c)).text("");
    }
    std::string online_txt;
    if (mem->online) {
        online_txt = "Online";
        if (mem->messenger() == MESSENGER_CHATCUBE) {
            online_txt += ", last action at ";
            online_txt += convert_time_full_to_string(mem->last_action, "%d.%m.%Y %H:%M");
        }
    } else {
        online_txt = "Offline, was online at ";
        online_txt += convert_time_full_to_string(mem->was_online, "%d.%m.%Y %H:%M");
    }
    tbx::Button(win.gadget(0x20)).value(online_txt);

    tbx::Button(win.gadget(0x1)).value("prof_empty");
    if (!mem->pic_cached.empty()) {
        CLImage profile_pic(mem->pic_cached);
        profile_pic.plot_to_app_sprite("prof_member", tbx::Colour::wimp_grey1);
        tbx::Button(win.gadget(0x1)).value("prof_member");
    }
    g_hourglass_off();
}

void ViewProfileDialog::has_been_hidden(const tbx::EventInfo &event_info) {
    delete this;
}

void ViewProfileDialog::open(MemberDataPtr mem) {
    if (_instance == nullptr) {
        _instance = new ViewProfileDialog(mem);
    } else {
        _instance->update(mem);
    }
    _instance->win.show_centered();
}

void ViewProfileDialog::open_avatar() {
    opening_avatar = true;
    string avatar_path = g_file_cache_downloader.get_cached_file_for_url(member->pic);
    if (!avatar_path.empty()) {
        Logger::debug("open big avatar %s\n", avatar_path.c_str());
        os_cli(string("*Filer_Run " + avatar_path).c_str());
        opening_avatar = false;
    } else {
        auto callback = [this](const std::string& file_name) {
            if (!file_name.empty()) {
                Logger::debug("open downloaded big avatar %s\n", file_name.c_str());
                os_cli(string("*Filer_Run " + file_name).c_str());
            }
            opening_avatar = false;
        };

        g_file_cache_downloader.download_url(member->pic, callback, true);
    }
}

void ViewProfileDialog::mouse_click(tbx::MouseClickEvent &event) {
    ComponentId cmp_id = event.id_block().self_component().id();
    if (cmp_id == 1) {
        open_avatar();
    }
}

