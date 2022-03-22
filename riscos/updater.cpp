#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "kernel.h"
#include <tbx/path.h>
#include <tbx/timer.h>
#include <tbx/command.h>
#include <tbx/application.h>
#include <cloverleaf/Logger.h>
#include "utils.h"

#define CMD_QUIT_APP            1

void replace_app() {
    if (is_file_exist("<ChatCube$Dir>.update.!ChatCube.!RunImage")) {
        Logger::info("Start replacing");
        tbx::Path src_path = tbx::Path("<ChatCube$Dir>");
        for (auto leaf = src_path.begin(); leaf != src_path.end(); leaf++) {
            if (*leaf == "update" || *leaf == "backup") {
                continue;
            }
            auto leaf_path = tbx::Path("<ChatCube$Dir>", *leaf);
            if (leaf_path.directory()) {
                Logger::debug("remove_recursive Dir: %s", leaf_path.name().c_str());
                remove_recursive(leaf_path.name());
            } else {
                leaf_path.remove();
                Logger::debug("remove_recursive File: %s", leaf_path.name().c_str());
            }
        }
        Logger::info("Old version removed");
        copy_dir("<ChatCube$Dir>.update.!ChatCube", "<ChatCube$Dir>", "");
        Logger::info("New version copied. Update finished. Starting new instance");
        tbx::Application::instance()->start_wimp_task("<ChatCube$Dir>.!Run -updated");
    } else {
        Logger::error("There is no update, 'update' directory is missing!");
    }
    tbx::Application::instance()->quit();
}

class AppTimer : public tbx::Timer {
public:
    void timer(unsigned int elapsed) override {
        tbx::Application::instance()->remove_timer(this);
        replace_app();
    }
} app_timer_task;

class AppQuitCommand : public tbx::Command {
public:
    void execute() override {
        tbx::Application::instance()->quit();
    }
} app_quit_cmd;

int main(int argc, char* argv[])
{
    char tmstr[200];
    time_t t = time(NULL);
    strftime(tmstr, sizeof(tmstr), "%Y-%m-%d %H:%M:%S", localtime(&t));
    Logger::init("<ChatCube$ChoicesDir>.updater-log");
    Logger::debug("======= ChatCube Updater started at %s =======", tmstr);
    tbx::Application my_app("<ChatCube$Dir>.update.updater");
    my_app.add_command(CMD_QUIT_APP, &app_quit_cmd);
    tbx::Application::instance()->add_timer(30, &app_timer_task);
    my_app.run();
    Logger::debug("======= ChatCube Updater exited =======");
}
