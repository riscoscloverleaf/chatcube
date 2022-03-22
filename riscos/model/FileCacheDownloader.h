/* FileCacheDownloader.h

   Single instance that download files in background and cache them to disk

 */
 
#ifndef ROCHAT_FILECACHEDOWNLOADER_H
#define ROCHAT_FILECACHEDOWNLOADER_H

#include <string>
#include <map>
#include <set>
#include <queue>
#include <functional>
#include "NetworkRequests.h"

#define MAX_CONCURRENT_DOWNLOADS 3

// tuple have (url, save_to)
typedef std::tuple<std::string, std::string> DownloadRequestType;

class FileDownloadRequest {
public:
    std::vector<FileDownloadSuccessCallbackType> callbacks;
    bool is_downloading;
    bool needs_progress;
    curl_off_t total_size_hint;

    FileDownloadRequest() = default;
    FileDownloadRequest(FileDownloadSuccessCallbackType callback, bool _needs_progress, curl_off_t _total_size_hint) :
            is_downloading(false),
            needs_progress(_needs_progress),
            total_size_hint(_total_size_hint)
    {
        callbacks.push_back(callback);
    };

    void push_back(FileDownloadSuccessCallbackType callback) {
        callbacks.push_back(callback);
    };
};

class FileCacheDownloader {
private:
//    std::queue<DownloadRequestType> download_queue;
    std::map<std::string, FileDownloadRequest*>  download_requests;
    int concurrent_downloads = 0;
    int max_concurrent_downloads = 0;
//    bool isReady(const std::string& url, const std::string& folder);
//    bool isDownloading(const std::string& url);
//    void runDownload(const std::string& url, const std::string& folder, int file_type);
    void possibly_send_downloading_event();
    void process_queue();
    void do_download(const std::string &url, FileDownloadRequest *req);
public:

    FileCacheDownloader();
    /**
     *  download file to folder if needed or return path to file if ready
     *  @return path to file if ready or empty string if still downloading
     */
    bool is_downloading() { return concurrent_downloads > 0; }
    static std::string get_cached_file_for_url(const std::string& url);
    static std::string get_filename_for_url(const std::string& url);
    static bool is_url_cached(const std::string& url);
    void download_url(const std::string& url, FileDownloadSuccessCallbackType success_callback, bool needs_progress=false, bool force_download=false, curl_off_t total_size_hint=0);
};

extern FileCacheDownloader g_file_cache_downloader;
#endif