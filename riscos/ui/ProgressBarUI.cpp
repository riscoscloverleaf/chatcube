//
// Created by lenz on 6/1/20.
//

#include <tbx/application.h>
#include <tbx/displayfield.h>
#include <tbx/slider.h>
#include <cloverleaf/Logger.h>
#include "ProgressBarUI.h"
#include "../global.h"

ProgressBarUI::ProgressBarUI() : win("progressBar") {
    g_app_events.listen<AppEvents::ProgressBarControl>(std::bind(&ProgressBarUI::on_progress_bar_control, this, std::placeholders::_1));
    g_app_events.listen<AppEvents::DownloadingFilesProgress>(std::bind(&ProgressBarUI::on_downloading_files_progress, this, std::placeholders::_1));
    g_app_events.listen<AppEvents::DownloadingProgress>(std::bind(&ProgressBarUI::on_downloading_file_progress, this, std::placeholders::_1));
    g_app_events.listen<AppEvents::UploadingProgress>(std::bind(&ProgressBarUI::on_uploading_file_progress, this, std::placeholders::_1));

    win.add_has_been_hidden_listener(this);

    tbx::Application::instance()->add_timer(100, this);
}

ProgressBarUI::~ProgressBarUI() {
    tbx::Application::instance()->remove_timer(this);
}

void ProgressBarUI::on_progress_bar_control(const AppEvents::ProgressBarControl &ev) {
    label = ev.label;
    req = ev.req;
    download_url = ev.download_url;
    if (ev.label.empty()) {
        win.hide();
        visible = false;
        Logger::debug("on_progress_bar_control Hide progress");
    } else {
        tbx::Slider(win.gadget(2)).value(ev.percent_done);
        tbx::DisplayField(win.gadget(1)).text(ev.label);
        if (ev.percent_done > 0 && !visible && (!g_app_state.start_hidden || g_app_state.is_main_window_shown)) {
            Logger::debug("on_progress_bar_control Show progress: %s pd:%d", label.c_str(), ev.percent_done);
            win.show_centered();
            visible = true;
        }

        if (ev.estimated_time) {
            start_time = os_read_monotonic_time();
            start_progress = ev.percent_done;
            estimated_time = ev.estimated_time * 100;
            estimated_progress = ev.estimated_progress;
        } else {
            estimated_time = 0;
        }
    }
}

void ProgressBarUI::update_progress(int percent_done) {
    if (percent_done < 100) {
        Logger::debug("update_progress() Show progress: %s %d", label.c_str(), percent_done);
        tbx::Slider(win.gadget(2)).value(percent_done);
        if (!visible && (!g_app_state.start_hidden || g_app_state.is_main_window_shown)) {
            win.show_centered();
            visible = true;
        }
    } else {
        Logger::debug("update_progress() Hide progress");
        win.hide();
        label.clear();
        req = nullptr;
        visible = false;
        download_url.clear();
    }
    estimated_time = 0;
}

void ProgressBarUI::on_downloading_files_progress(const AppEvents::DownloadingFilesProgress &ev) {
    Logger::debug("on_downloading_files_progress %d", ev.percent_done);
    if (!label.empty()) {
        update_progress(ev.percent_done);
    }
}

void ProgressBarUI::on_downloading_file_progress(const AppEvents::DownloadingProgress &ev) {
//    Logger::debug("on_downloading_file_progress %d label:%s ev.req=%p req=%p download_url:%s req.url:%s", ev.percent_done, label.c_str(), ev.req, req, download_url.c_str(), ev.req->url.c_str());
    if (!label.empty()) {
        if (!req && ev.req->url == download_url) {
            req = ev.req;
        }
        if (ev.req == req) {
            update_progress(ev.percent_done);
        }
    }
}

void ProgressBarUI::on_uploading_file_progress(const AppEvents::UploadingProgress &ev) {
    if (!label.empty() && req == ev.req) {
        update_progress(ev.percent_done);
    }
}

void ProgressBarUI::has_been_hidden(const tbx::EventInfo &event_info) {
    visible = false;
    g_hourglass_off();
}

void ProgressBarUI::timer(unsigned int not_used) {
    if (estimated_time) {
        os_t now = os_read_monotonic_time();
        if (start_time > now) {
            start_time = 0;
        }
        os_t elapsed_time = now - start_time;
        if (elapsed_time > estimated_time) {
            elapsed_time = estimated_time;
        }
        int percent_done = start_progress + (elapsed_time * estimated_progress / estimated_time);
        Logger::debug("ProgressBarUI::timer() estimated progress %s %d", label.c_str(), percent_done);
        tbx::Slider(win.gadget(2)).value(percent_done);
    }
}


