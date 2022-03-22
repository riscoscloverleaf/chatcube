//
// Created by lenz on 4/27/20.
//

#include <oslib/Toolboxtypes.h>
#include <tbx/window.h>
#include <tbx/hasbeenhiddenlistener.h>
#include <tbx/mouseclicklistener.h>
#include <tbx/loader.h>
#include <tbx/eventinfo.h>
#include "AvatarChanger.h"
#include "SelectAvatarView.h"

class AvatarImageSelector : public tbx::HasBeenHiddenListener,
                            public tbx::Loader,
                            public ViewItemClickListener<SelectAvatarViewItem> {
public:
    char messenger;
    SelectAvatarView avatar_view;

    AvatarImageSelector(char messenger) : messenger(messenger)
    {
        avatar_view.win.add_has_been_hidden_listener(this);
        avatar_view.win.add_loader(this);
        avatar_view.add_click_listener(this);

        if (messenger == MESSENGER_TELEGRAM) {
            avatar_view.open(g_app_data_model.get_my_member_data()->telegram_account.pic_cached);
        } else {
            avatar_view.open(g_app_data_model.get_my_member_data()->pic_small_cached);

        }
    }

    void has_been_hidden(const tbx::EventInfo &event_info) override {
        delete this;
    }

    void item_clicked(ViewItemClickEvent<SelectAvatarViewItem> &event) override {
        auto on_success = [](const MyMemberDataPtr me) {
        };
        CLStringsMap postData;
        if (messenger == MESSENGER_TELEGRAM) {
            postData["telegram_account.avatar"] = event.view_item().value.name;
        } else {
            postData["avatar"] = event.view_item().value.name;
        }
        g_app_data_model.change_my_profile(postData, on_success);
        avatar_view.win.hide();
    }

    bool load_file(tbx::LoadEvent &event) override {
        if (event.from_filer() && event.file_type() != 0x1000) {
            auto on_avatar_uploaded = [this](const MyMemberDataPtr me) {
                avatar_view.win.hide();
            };
            if (messenger == MESSENGER_TELEGRAM) {
                g_app_data_model.upload_profile_image(event.file_name(), "telegram_account.profile_image", on_avatar_uploaded);
            } else {
                g_app_data_model.upload_profile_image(event.file_name(), "profile_image", on_avatar_uploaded);
            }
            return true;
        }
        return false;
    }
};


class AvatarUploadImage : public tbx::HasBeenHiddenListener,
                          public tbx::Loader {
public:
    char messenger;
    tbx::Window win;

    AvatarUploadImage(char messenger) :
        messenger(messenger),
        win(tbx::Window("dragPhoto"))
    {
        win.add_has_been_hidden_listener(this);
        win.add_loader(this);
        win.show_centered();
    }

    virtual ~AvatarUploadImage() {
        win.delete_object();
    }

    void has_been_hidden(const tbx::EventInfo &event_info) override {
        delete this;
    }

    bool load_file(tbx::LoadEvent &event) override {
        if (event.from_filer() && event.file_type() != 0x1000) {
            auto on_avatar_uploaded = [this](const MyMemberDataPtr me) {
                win.hide();
            };
            if (messenger == MESSENGER_TELEGRAM) {
                g_app_data_model.upload_profile_image(event.file_name(), "telegram_account.profile_image", on_avatar_uploaded);
            } else {
                g_app_data_model.upload_profile_image(event.file_name(), "profile_image", on_avatar_uploaded);
            }
            return true;
        }
        return false;
    }
};

class AvatarOptionSelector : public tbx::HasBeenHiddenListener, public tbx::MouseClickListener {
public:
    char messenger;
    tbx::Window win;

    AvatarOptionSelector(char messenger) :
            messenger(messenger),
            win(tbx::Window("chAvatarOpt")) {
        win.add_mouse_click_listener(this);
        win.add_has_been_hidden_listener(this);
        win.show_centered();
    }

    virtual ~AvatarOptionSelector() {
        win.delete_object();
    }

    void mouse_click(tbx::MouseClickEvent &event) override {
        switch (event.id_block().self_component().id()) {
            case 0:
                new AvatarUploadImage(messenger);
                break;
            case 1:
                new AvatarImageSelector(messenger);
                break;
        }
        win.hide();
    }

    void has_been_hidden(const tbx::EventInfo &event_info) override {
        delete this;
    }
};


void show_avatar_changer(char messenger) {
    new AvatarOptionSelector(messenger);
}