#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <functional>

#include "kernel.h"
#include "oslib/hourglass.h"
#include "rufl.h"
#include <tbx/application.h>
#include <tbx/swixcheck.h>
#include <tbx/monotonictime.h>
#include <tbx/command.h>
#include <tbx/button.h>
#include <tbx/autocreate.h>
#include <tbx/timer.h>
#include <tbx/path.h>
#include <tbx/uncaughthandler.h>
#include <tbx/oserror.h>
#include <tbx/taskmanager.h>
#include "global.h"
#include "utils.h"
#include "model/AppDataModel.h"
#include "service/IKConfig.h"
#include "cloverleaf/IdleTask.h"
#include "cloverleaf/CLImageCache.h"
#include "cloverleaf/CLImage.h"
#include "cloverleaf/CLSound.h"
#include "AppEventHandlers.h"
#include "ui/AppMenu.h"
#include "ui/AppIconbar.h"
#include "ui/ChatMainUI.h"
#include "ui/LoginDialog.h"
#include "ui/EditProfileSettings.h"
#include "ui/AvatarChanger.h"
#include "ui/CreateChatDialog.h"
#include "ui/TelegramRegisterOrChangePhoneDialog.h"
#include "ui/ProgressBarUI.h"
#include "ui/LogoutDialog.h"
#include "ui/FeedbackDialog.h"

#define CMD_QUIT_APP            1
#define CMD_OPEN_MAIN_WINDOW 0x101
#define CMD_SHOW_PROG_INFO 0x102
#define CMD_CHANGE_AVATAR  0x450
#define CMD_EDIT_PROFILE  0x451
#define CMD_LOGOUT  0x452
#define CMD_ADD_MESSENGER_TELEGRAM  0x453
#define CMD_OPEN_DOWNLOADS  0x454
#define CMD_FEEDBACK  0x455
#define CMD_CREATE_CHAT_CHATCUBE  0x470
#define CMD_CREATE_CHAT_TELERGAM  0x471

AppState g_app_state;

class MyUncaughtHandler : public tbx::UncaughtHandler {
public:
    void uncaught_exception(std::exception *e, int event_code) override {
        char errstr[1024];
        tbx::OsError *oserr = dynamic_cast<tbx::OsError*>(e);
        if (oserr) {
            snprintf(errstr, sizeof(errstr) - 1, "ChatCube: Uncaught Exception: %s Code %d (event: %d)", oserr->what(), oserr->number(), event_code);
        } else {
            if (e) {
                snprintf(errstr, sizeof(errstr) - 1, "ChatCube: Uncaught Exception: %s  (event: %d)", e->what(), event_code);
            } else {
                snprintf(errstr, sizeof(errstr) - 1, "ChatCube: Uncaught Exception! (event %d)", event_code);
            }
        }
        Logger::error(errstr);
        show_alert_error(errstr);
    }
} my_uncaught_handler;

class AppTimer : public tbx::Timer {
private:
    bool http_was_connected = true;
public:
    void timer(unsigned int elapsed) override {
//        int x,y, b;
//        os_t t;
//        xos_mouse(&x,&y,&b, &t);
//        Logger::debug("AppTimer %d %x", elapsed, b);
        g_idle_task.process_tasks();
        g_http_service.process();
        bool now_connected = g_http_service.connected();
        if (http_was_connected != now_connected) {
            http_was_connected = now_connected;
            Logger::debug("AppTimer connected %d", http_was_connected);
            g_app_events.notify(AppEvents::ConnectionStateChanged{.connected = now_connected});
        }

        os_t now = os_read_monotonic_time();
        int period = 3;
        if (g_app_state.startup_time > now) {
            g_app_state.startup_time = 0;
        }
        if (now - g_app_state.startup_time < 1000) {
            period = 3;
        } else {
            if (!g_app_state.is_main_window_shown) {
                period = 50;
            } else {
                if (g_http_service.is_online) {
                    period = 5;
                } else {
                    period = 50;
                }
            }
        }
        set_app_poll_period(period);

        // cancel typing notify on timeout
        if (ChatMainUI::instance) {
            ChatMainUI::instance->check_typing_notify_timeout();
        }

    }
} app_poll_task;

class AppShowMainUICommand : public tbx::Command {
public:
    void execute() override {
        if (g_app_data_model.is_logged_in()) {
            ChatMainUI::open();
        } else {
            new LoginDialog();
        }
    }
} app_show_main_ui_cmd;

class AppQuitCommand : public tbx::Command {
public:
    void execute() override {
        tbx::Application::instance()->quit();
    }
} app_quit_cmd;

class AppLogoutCommand : public tbx::Command {
public:
    void execute() override {
        new LogoutDialog();
    }
} app_logout_cmd;

class AppChangeAvatarCommand : public tbx::Command {
public:
    void execute() override {
        show_avatar_changer(MESSENGER_CHATCUBE);
    }
} app_change_avatar_cmd;

class AppAddMessengerTelegramCommand : public tbx::Command {
public:
    void execute() override {
        if (!g_app_data_model.get_my_member_data()->has_telegram()) {
            new TelegramRegisterOrChangePhoneDialog();
        } else {
            EditProfileSettings::open(EditProfileSettings::TAB_TELEGRAM);
        }
    }
} app_add_messenger_telegram_cmd;

class AppEditPorfileSettingsCommand : public tbx::Command {
public:
    void execute() override {
        EditProfileSettings::open(EditProfileSettings::TAB_GENERAL);
    }
} app_edit_profile_settings_cmd;

class AppCreateTelegramChatCommand : public tbx::Command {
public:
    void execute() override {
        new CreateChatDialog(MESSENGER_TELEGRAM);
    }
};

class AppCreateChatcubeChatCommand : public tbx::Command {
public:
    void execute() override {
        new CreateChatDialog(MESSENGER_CHATCUBE);
    }
};

class OpenDownloadsCmd : public tbx::Command {
public:
    tbx::Window win;

    void execute() override {
        os_cli("*Filer_OpenDir <Choices$Write>.ChatCube.downloads");
    }
};

class AppSendFeedbackCommand : public tbx::Command {
public:
    void execute() override {
        FeedbackDialog::open();
    }
};

class AppShowProgInfo : public tbx::Command {
public:
    tbx::Window win;

    AppShowProgInfo() : win("ProgInfo") {}

    void execute() override {
        tbx::Button(win.gadget(3)).value("Version "+g_app_data_model.get_app_version());
        win.show_at_pointer();
    }
};

void app_cleanup() {
    xhourglass_off();
    rufl_quit();
    /* Uninstall fonts */
    xos_cli("FontRemove ChatCube:Fonts.");
}

void set_app_poll_period(int period) {
//    Logger::debug("set_app_poll_period %d", period);
    if (g_app_state.app_poll_period != period) {
        g_app_state.app_poll_period = period;
        tbx::Application::instance()->remove_timer(&app_poll_task);
        tbx::Application::instance()->add_timer(period, &app_poll_task);
    }
}

tbx::Hourglass hourglass;

void g_hourglass_on() {
    if (g_app_state.is_main_window_shown) {
        hourglass.on();
    }
}

void g_hourglass_percentage(int percent) {
    if (g_app_state.is_main_window_shown) {
        hourglass.percentage(percent);
    }
}

void g_hourglass_off() {
    hourglass.off();
}

std::string app_version = "1.31";

int main(int argc, char* argv[])
{
    atexit(app_cleanup);

    for(int i = 0; i < argc; i++) {
        if (stricmp(argv[i], "-debug") == 0) {
            g_app_state.IKDEBUG = true;
        }
        if (stricmp(argv[i], "-updated") == 0) {
            g_app_state.ISUPDATED = true;
        }
    }


    remove_recursive("<ChatCube$ChoicesDir>.temp");
    mkdir("<ChatCube$ChoicesDir>.temp", 0777);

    tbx::TaskManager tm;
    std::vector<tbx::TaskInfo> tasks;
    if (tm.find_all(tasks,"ChatCube")) {
        show_alert_error("ChatCube already running! Exiting.");
        exit(0);
    }

    tbx::Application my_app("<ChatCube$Dir>");
    char tmstr[200];
    time_t t = time(NULL);
    strftime(tmstr, sizeof(tmstr), "%Y-%m-%d %H:%M:%S", localtime(&t));
    Logger::init("<ChatCube$ChoicesDir>.log");
    Logger::debug("======= ChatCube %s (%s %s) started at %s =======", app_version.c_str(), __DATE__, __TIME__, tmstr);
    my_app.uncaught_handler(&my_uncaught_handler);


    IKConfig::start("<ChatCube$ChoicesDir>.config/ini");
    g_app_state.start_hidden = IKConfig::get_value("general","start_hidden",0);

    init_images_cache(200);
    CLImage::detect_rgb_mode();

    set_app_poll_period(2);

    my_app.add_command(CMD_QUIT_APP, &app_quit_cmd);
    my_app.add_command(CMD_LOGOUT, &app_logout_cmd);
    my_app.add_command(CMD_CHANGE_AVATAR, &app_change_avatar_cmd);
    my_app.add_command(CMD_EDIT_PROFILE, &app_edit_profile_settings_cmd);
    my_app.add_command(CMD_OPEN_MAIN_WINDOW, &app_show_main_ui_cmd);
    my_app.add_command(CMD_ADD_MESSENGER_TELEGRAM, &app_add_messenger_telegram_cmd);
    my_app.add_command(CMD_CREATE_CHAT_CHATCUBE, new AppCreateChatcubeChatCommand());
    my_app.add_command(CMD_CREATE_CHAT_TELERGAM, new AppCreateTelegramChatCommand());
    my_app.add_command(CMD_OPEN_DOWNLOADS, new OpenDownloadsCmd());
    my_app.add_command(CMD_FEEDBACK, new AppSendFeedbackCommand());
    my_app.add_command(CMD_SHOW_PROG_INFO, new AppShowProgInfo());

    my_app.set_autocreate_listener("IbarMenu", new tbx::AutoCreateClassOnce<MainMenu>());
    my_app.set_autocreate_listener("addMessMenu", new tbx::AutoCreateClassOnce<MessengersMenu>());
    my_app.set_autocreate_listener("CrtChatMnu", new tbx::AutoCreateClassOnce<CreateChatMenu>());
    my_app.set_autocreate_listener("Iconbar", new tbx::AutoCreateClassOnce<AppIconbar>());

    setup_app_event_handlers();
    new ProgressBarUI();

    auto startup = [&my_app]() {
        Logger::info("Initializing fonts");

        rufl_init();

        Logger::info("Connecting to server");

        AppEvents::ProgressBarControl pbreq;
        pbreq.label = "Connecting..";
        pbreq.percent_done = 1;
        pbreq.estimated_progress = 9;
        pbreq.estimated_time = 10;
        g_app_events.notify(pbreq);

        g_app_state.startup_time = os_read_monotonic_time();
        if (g_app_state.IKDEBUG) {
            g_app_data_model.init("https://test.chatcube.org/", "en", app_version);
        } else {
            g_app_data_model.init("https://api3.chatcube.org/", "en", app_version);
        }
        g_app_data_model.start();
    };

    g_idle_task.run_at_next_idle(startup);

    my_app.run();

    IKConfig::stop();

    remove_recursive("<ChatCube$ChoicesDir>.temp");
    Logger::info("Exit");
}
