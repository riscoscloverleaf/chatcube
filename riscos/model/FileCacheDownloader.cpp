#include <string>
#include <cloverleaf/Logger.h>
#include <tbx/path.h>
#include "FileCacheDownloader.h"

const static char* _cache_dir = "<Choices$Write>.ChatCube.cache";

FileCacheDownloader::FileCacheDownloader() {
    if (!is_directory_exist (_cache_dir)) {
        mkdir(_cache_dir, 0777);
    }
}

std::string FileCacheDownloader::get_filename_for_url(const std::string& url) {
//    const std::string& baseurl = g_http_service.get_base_url();
    if (url.back() == '/') {
        return std::string();
    }
    std::string cuturl = trim_prefix(url, "https://");
    cuturl = trim_prefix(cuturl, "http://");
    std::string dirname;
    int url_size = cuturl.size();
    for(int i = 0; i < url_size; i++) {
        if (cuturl[i] == '/') {
            cuturl[i] = '.';
        } else if (cuturl[i] == '.') {
            cuturl[i] = '/';
        }
    }
    return std::string(_cache_dir)+"."+cuturl;
}

std::string FileCacheDownloader::get_cached_file_for_url(const std::string & url) {
    std::string filename = get_filename_for_url(url);
    if (!filename.empty() && get_filesize(filename.c_str()) > 10) {
        return filename;
    }
    return std::string();
}

bool FileCacheDownloader::is_url_cached(const std::string& url) {
    std::string filename = get_filename_for_url(url);
    return (!filename.empty() && get_filesize(filename.c_str()) > 10);
}

void FileCacheDownloader::possibly_send_downloading_event() {
    Logger::debug("possibly_send_downloading_event qlen:%d max:%d concurr_downloads:%d", download_requests.size(), max_concurrent_downloads, concurrent_downloads);
    if (concurrent_downloads > 1) {
        if (max_concurrent_downloads < download_requests.size()) {
            max_concurrent_downloads = download_requests.size();
        }
        AppEvents::DownloadingFilesProgress ev;
        ev.percent_done = 100 - (((download_requests.size() * 100) / max_concurrent_downloads) + 1);
        g_app_events.notify(ev);
    } else if (concurrent_downloads == 0 && max_concurrent_downloads > 0) {
//        Logger::debug("possibly_send_downloading_event 100%");
        max_concurrent_downloads = 0;
        AppEvents::DownloadingFilesProgress ev;
        ev.percent_done = 100;
        g_app_events.notify(ev);
    }
}

void FileCacheDownloader::download_url(const std::string& url, FileDownloadSuccessCallbackType success_callback, bool needs_progress, bool force_download, curl_off_t total_size_hint) {
    const std::string cached_file = get_cached_file_for_url(url);
    if (!force_download && !cached_file.empty()) {
        Logger::debug("download_url return already downloaded %s", url.c_str());
        success_callback(cached_file);
        return;
    }

    auto found = download_requests.find(url);
    if (found != download_requests.end()) {
        Logger::debug("download_url found request, add callback %s qlen:%d", url.c_str(), download_requests.size());
        found->second->push_back(success_callback);
    } else {
        Logger::debug("download_url new request %s force:%d qlen:%d", url.c_str(), force_download, download_requests.size());
        download_requests[url] = new FileDownloadRequest(success_callback, needs_progress, total_size_hint);
    }

    process_queue();
}

void FileCacheDownloader::do_download(const std::string &url, FileDownloadRequest *req) {
    auto on_success_callback = [this, url, req](CLHTTPRequest* httpreq) {
        CLDownloadFileRequest* downloadreq = dynamic_cast<CLDownloadFileRequest*>(httpreq);
        std::string save_to = get_filename_for_url(downloadreq->response_url);
        downloadreq->move_file(save_to.c_str());
        concurrent_downloads--;
        for(auto cb : req->callbacks) {
            cb(save_to);
        }
        download_requests.erase(url);
        Logger::error("FileCacheDownloader::do_download on_success_callback, url:%s", url.c_str());
        delete req;
        possibly_send_downloading_event();
        process_queue();
    };

    auto on_fail_callback = [this, url, req](const HttpRequestError& err) {
        Logger::error("Download failed, code: %d url:%s", err.http_code, url.c_str());
        concurrent_downloads--;
        for(auto cb : req->callbacks) {
            cb("");
        }
        download_requests.erase(url);
        delete req;
        possibly_send_downloading_event();
        process_queue();
        return true;
    };

    concurrent_downloads++;
    Logger::debug("do_download %s progress=%d qlen=%d", url.c_str(), req->needs_progress, download_requests.size());
    auto *downloadreq = new CLDownloadFileRequest(url, on_success_callback, on_fail_callback);
    downloadreq->needs_progress = req->needs_progress;
    downloadreq->total_size_hint = req->total_size_hint;
    g_http_service.submit(downloadreq);
}

void FileCacheDownloader::process_queue() {
    if (!download_requests.empty() && concurrent_downloads < MAX_CONCURRENT_DOWNLOADS) {
        for(auto &it : download_requests) {
            if (!it.second->is_downloading) {
                it.second->is_downloading = true;
                Logger::debug("process_queue download %s qlen:%d", it.first.c_str(), download_requests.size());
                do_download(it.first, it.second);
            }
            if (concurrent_downloads >= MAX_CONCURRENT_DOWNLOADS) {
                break;
            }
        }
    }
}

FileCacheDownloader g_file_cache_downloader;