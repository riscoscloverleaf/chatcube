/*
 * NetworkService.h
 *
 * do work for sending data using http to API server
 */

#ifndef __IKHTTPService_h
#define __IKHTTPService_h

#include <string>
#include <map>
#include <list>
#include <functional>
#include <stdexcept>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "../utils.h"
#include "curl/curl.h"
#include "../libs/cJSON/cJSON.h"

using namespace std;
typedef std::map<std::string, std::string> CLStringsMap;
typedef std::function<void(const cJSON *)> PushStreamHandlerType;
typedef std::function<int(curl_off_t dltotal, curl_off_t dlnow,
                           curl_off_t ultotal, curl_off_t ulnow)> HTTPRequestProgressCallbackType;

class CLHTTPEventStream {
private:
    std::string _sse_buf;
    std::string _sse_raw;
    std::string _last_sse_event_time;
    CURL *_curl_handle = NULL;
    PushStreamHandlerType _events_handler;
    time_t _last_activity_time = 0;
    time_t _last_receive_time = 0;
    void on_raw_sse_event(string& raw);
    bool _started = false;

public:
    std::string base_url;
    std::string user_agent;
    std::string channel;

    CURL *get_curl_handle() {
        return _curl_handle;
    }

    void clear_curl_handle() {
        if (_curl_handle) {
            curl_easy_cleanup(_curl_handle);
            _curl_handle = NULL;
        }
    }

    void set_last_event_date(const std::string& dt) {
        _last_sse_event_time = dt;
    }

    void set_events_handler(PushStreamHandlerType events_handler) {
        _events_handler = events_handler;
    }

    void start() { _started = true; }
    void stop() { _started = false; }
    bool started() { return _started; }
    bool is_active() { return ((_last_activity_time + 30) > time(NULL)); }
    bool is_connected() { return ((_last_receive_time + 30) > time(NULL)); }
    CURL* make_curl_handle(struct curl_slist * resolved_addrs);
    void parse_raw_sse_event(char* c, unsigned long size);
};


class CLHTTPRequest {
private:
    struct curl_httppost *_curl_formpost = NULL;
    struct curl_slist *_curl_headers = NULL;
    struct curl_httppost *_curl_formlast = NULL;
public:
    CLStringsMap upload_files;
    CLStringsMap post_data;
    CLStringsMap headers_map = {{"X-AppPlatform", "riscos"}};
    std::string url;
    std::string method;
    int lowspeed_limit = 0;
    int lowspeed_time = 0;
    int timeout = 0;
    bool needs_progress = false;
    bool cancel_loading = false;
    bool needs_hourglass = false;
    int response_code = 0;
    std::string response_text = "";
    char *response_url = NULL;
    cJSON *response_json = NULL;
    CURL *curl_handle;

    CLHTTPRequest(const char* a_method, const string &a_url);
    virtual ~CLHTTPRequest();
    void cleanup();
    void init();
    std::string build_url_parameters(const CLStringsMap &post_data);
    void set_url_parameters(CLStringsMap &data);
    void set_timeout(const int timeout);
    void set_lowspeed_limit(const int time, const int limit); // time in sec, limit is bytes pre second
    void set_hourglass(bool on);

    virtual int on_progress(curl_off_t dltotal, curl_off_t dlnow,
                                    curl_off_t ultotal, curl_off_t ulnow);
    virtual void process_response();
    virtual bool on_before_submit();
    virtual unsigned long on_append_content(char* c, unsigned long size);

    virtual void on_success() {};
    virtual void on_fail() {};
    virtual void on_finally() {};
};


class CLHTTPService {
private:
    CLHTTPEventStream event_stream;
    CURLM *_curl_multi = NULL;
    std::map<CURL *, CLHTTPRequest*> _running_requests;
    std::string _user_agent;
    std::string _base_url = "https://test.chatcube.org";
    struct curl_slist * _resolved_addrs;
    std::string _lang = "en";
    std::string _auth_token = "";
//    void _process_request(CURLMsg* msg);
    bool _is_host_resolved(char* host);
//    void _load_resolved();
    void _add_resolved(char *host, char* ip);
    void _clean_resolved(char *host);

public:
    bool is_online = false;

    CLHTTPService();
    ~CLHTTPService();

    void init(const string &base, const string &lang, const string& user_agent);

    void set_events_handler(PushStreamHandlerType events_handler) {
        event_stream.set_events_handler(events_handler);
    };

    void start_event_stream(const std::string &channel, const std::string& start_date);

    void stop_event_stream();

    void submit(CLHTTPRequest* req);

    const std::string &get_auth_token() const {
        return _auth_token;
    }

    void set_auth_token(const string &auth_token) {
        _auth_token = auth_token;
    }

    const std::string &get_lang() const {
        return _lang;
    }

    void set_lang(const string &lang) {
        _lang = lang;
    }

    const std::string &get_base_url() const {
        return _base_url;
    }

    void set_base_url(const string &baseUrl) {
        _base_url = baseUrl;
        event_stream.base_url = baseUrl;
    }
    bool resolve_server_hostname();

    bool connected();
    /* main processor */
    void process();

};

extern CLHTTPService g_http_service;
#endif