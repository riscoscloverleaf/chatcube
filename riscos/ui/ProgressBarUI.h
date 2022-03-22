//
// Created by lenz on 6/1/20.
//

#ifndef ROCHAT_PROGRESSBARUI_H
#define ROCHAT_PROGRESSBARUI_H

#include "../model/AppEvents.h"
#include <oslib/os.h>
#include <tbx/window.h>
#include <tbx/timer.h>
#include <tbx/hasbeenhiddenlistener.h>

class ProgressBarUI : public tbx::HasBeenHiddenListener, public tbx::Timer {
private:
    tbx::Window win;
    CLHTTPRequest *req = nullptr;
    std::string download_url;
    std::string upload_url;
    std::string label;
    bool visible = false;
    os_t start_time = 0; // start time of waiting for something
    int start_progress = 0; // start progress percent value
    os_t estimated_time = 0; // estimated time to wait
    int estimated_progress = 0; // final (max) increment at end of waiting

    void update_progress(int percent);

public:
    ProgressBarUI();
    ~ProgressBarUI();
    void on_progress_bar_control(const AppEvents::ProgressBarControl& ev);
    void on_downloading_files_progress(const AppEvents::DownloadingFilesProgress& ev);
    void on_downloading_file_progress(const AppEvents::DownloadingProgress& ev);
    void on_uploading_file_progress(const AppEvents::UploadingProgress& ev);

    void has_been_hidden(const tbx::EventInfo &event_info) override;

    void timer(unsigned int elapsed) override;
};


#endif //ROCHAT_PROGRESSBARUI_H
