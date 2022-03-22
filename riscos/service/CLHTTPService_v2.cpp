#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <curl/multi.h>
#include <curl/curl.h>
#include <oslib/hourglass.h>
#include <oslib/os.h>
#include "oslib/resolver.h"
#include <cloverleaf/Logger.h>
#include "CLHTTPService_v2.h"
#include "../utils.h"
#include "../global.h"

#define DEFAULT_CONN_TIMEOUT 5
static const char *resolved_ip_file_path =  "<Choices$Write>.ChatCube.cache.resolved";

//extern AppState g_app_state;
static void get_host_port_from_url(const char* url, char* host) {
    char *last_pos;
    int len;
    memset(host, 0, sizeof(host));
    if (memcmp(url,"https://",8) == 0) {
        last_pos = strchr(url + 8,'/');
        len = last_pos - url - 8;
        strncpy(host, url + 8, len);
        host[len] = 0;
        strcat(host, ":443");
    } else if (memcmp(url,"http://",7) == 0) {
        last_pos = strchr(url + 7,'/');
        len = last_pos - url - 7;
        strncpy(host, url + 7, len);
        host[len] = 0;
        strcat(host, ":80");
    }
}

static void get_host_from_url(const char* url, char* host) {
    char *last_pos;
    int len;
    memset(host, 0, sizeof(host));
    if (memcmp(url,"https://",8) == 0) {
        last_pos = strchr(url + 8,'/');
        len = last_pos - url - 8;
        strncpy(host, url + 8, len);
        host[len] = 0;
    } else if (memcmp(url,"http://",7) == 0) {
        last_pos = strchr(url + 7,'/');
        len = last_pos - url - 7;
        strncpy(host, url + 7, len);
        host[len] = 0;
    }
}

CLHTTPService::CLHTTPService() {
    curl_global_init(CURL_GLOBAL_ALL);
    _curl_multi = curl_multi_init();
}

CLHTTPService::~CLHTTPService() {
    while (!_running_requests.empty()) {
        std::map<CURL *, CLHTTPRequest*>::iterator cb = _running_requests.begin();
        curl_multi_remove_handle(_curl_multi, cb->first);
        delete(cb->second);
        _running_requests.erase(cb);
    }
    curl_multi_cleanup(_curl_multi);

    if (_resolved_addrs) {
        curl_slist_free_all(_resolved_addrs);
    }

    curl_global_cleanup();
}

void CLHTTPService::init(const string &base, const string &lang, const string &user_agent) {
    _base_url = base;
    _lang = lang;
    _user_agent = user_agent;
    event_stream.base_url = base;
    event_stream.user_agent = user_agent;
}

bool CLHTTPService::resolve_server_hostname() {
    int err, port;
    os_error *os_err;
    char host[130];
    resolver_host_details *host_details;

    get_host_from_url(_base_url.c_str(), host);

//    os_err = xresolver_cache_control(resolvercachecontrolreason_FLUSH_ALL);
//    if (os_err) {
//        Logger::error("Error in resolve cache control, os error:%d: %s ", os_err->errnum, os_err->errmess);
//        is_online = false;
//        return false;
//    }

    os_err = xresolver_get_host(host, &err, &host_details);
    if (err) {
        Logger::error("Error resolve hostname: %s error:%d", host, err);
        is_online = false;
        return false;
    }
    if (os_err) {
        Logger::error("Error resolve hostname: %s os error:%d: %s ", host, os_err->errnum, os_err->errmess);
        is_online = false;
        return false;
    }

//    if (memcmp(_base_url.c_str(),"https://",8) == 0) {
//        port = 443;
//    } else {
//        port = 80;
//    }

//    sprintf(host, "%s:%d:%d.%d.%d.%d", host, port, host_details->addresses[0][0], host_details->addresses[0][1], host_details->addresses[0][2], host_details->addresses[0][3]);
//    curl_slist_free_all(_resolved_addrs);
//    _resolved_addrs = curl_slist_append(_resolved_addrs, host);
//    Logger::debug("resolve hostname: %s", host);
    Logger::debug("hostname resolved %s:%d.%d.%d.%d", host, host_details->addresses[0][0], host_details->addresses[0][1], host_details->addresses[0][2], host_details->addresses[0][3]);
    is_online = true;

    return true;
}

static size_t _IKHTTPRequest_Write(void *content, size_t size, size_t nmemb, void *userp)
{
    return ((CLHTTPRequest *) userp)->on_append_content((char *) content, size * nmemb);
    //Logger::debug("_IKHTTPRequest_Write content=%s", ((IKHTTPResponse*)userp)->text.c_str());
}

static int _IKHTTPRequest_XFER(void *p,
                               curl_off_t dltotal, curl_off_t dlnow,
                               curl_off_t ultotal, curl_off_t ulnow)
{
    Logger::debug("_IKHTTPRequest_XFER p=%p %lld %lld %lld %lld", p, dltotal, dlnow, ultotal, ulnow);
    return ((CLHTTPRequest *)p)->on_progress(dltotal, dlnow, ultotal, ulnow);
}

void CLHTTPService::submit(CLHTTPRequest* req) {
    string url, req_url = req->url;
    CURL *curl_handle = req->curl_handle;

    if (_auth_token.length()) {
        req->headers_map["Authorization"] = "Token " + _auth_token;
    }

    if (req_url.substr(0, 7) != "http://" && req_url.substr(0, 8) != "https://") {
        url = _base_url + _lang + "/api" + req_url;
    } else {
        url = req_url;
    }

    if (!req->on_before_submit()) {
        req->on_fail();
        delete(req);
        return;
    }

    Logger::debug("URL %s handle: %x req: %x", url.c_str(), curl_handle, req);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, _user_agent.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, _IKHTTPRequest_Write);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, req);
    curl_easy_setopt(curl_handle, CURLOPT_PRIVATE, req);
    curl_easy_setopt(curl_handle, CURLOPT_CAINFO, "<ChatCube$Dir>.ssl.chain");
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, false);
    if (!req->upload_files.empty() && req->needs_progress) {
        curl_easy_setopt(curl_handle, CURLOPT_XFERINFOFUNCTION, _IKHTTPRequest_XFER);
        curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl_handle, CURLOPT_PROGRESSDATA, req);
    }
    if (req->needs_hourglass) {
        g_hourglass_on();
    }

//    if (_resolved_addrs) {
//        //Logger::debug("Use resolved");
//        curl_easy_setopt(curl_handle, CURLOPT_RESOLVE, _resolved_addrs);
//    }
    _running_requests.insert(std::make_pair(curl_handle, req));
    curl_multi_add_handle(_curl_multi, curl_handle);
}

void CLHTTPService::start_event_stream(const std::string &channel, const std::string &start_date) {
    assert(!channel.empty());
    event_stream.channel = channel;
    event_stream.set_last_event_date(start_date);
    event_stream.start();
    if (event_stream.get_curl_handle()) {
        curl_multi_remove_handle(_curl_multi, event_stream.get_curl_handle());
        event_stream.clear_curl_handle();
    }
    CURL *handle;
    // start event stream listener if it not started yet
    handle = event_stream.make_curl_handle(_resolved_addrs);
    curl_multi_add_handle(_curl_multi, handle);
    Logger::debug("make event stream handle %p", handle);
}

void CLHTTPService::stop_event_stream() {
    event_stream.stop();
    if (event_stream.get_curl_handle()) {
        curl_multi_remove_handle(_curl_multi, event_stream.get_curl_handle());
        event_stream.clear_curl_handle();
    }
}

//void CLHTTPService::_process_request(CURLMsg* msg) {
//    CURL *handle = msg->easy_handle;
//    CLHTTPRequest *req;
//    char *url;
//
//    curl_easy_getinfo(handle, CURLINFO_EFFECTIVE_URL, &url);
//    curl_easy_getinfo(handle, CURLINFO_PRIVATE, &req);
//    if (req) {
//        if (req->needs_hourglass) {
//            g_hourglass_off();
//        }
//        _last_connection_error.clear();
//        req->response_url = url;
//        curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &req->response_code);
//        req->process_response();
//        delete(req);
//    } else {
//        Logger::error("CLHTTPService::_process_request() missing req (%s)", url);
//    }
//}

void CLHTTPService::process() {
    bool event_stream_restarted = false;
    int running = 1, messagesLeft = -1;
    long curl_timeo;
    CURLMsg *curlMsg;
    CURL *handle;
    fd_set fdread;
    fd_set fdwrite;
    fd_set fdexcep;
    int maxfd = -1;

    if (!is_online && !resolve_server_hostname()) {
        Logger::debug("Server hostname still not resolved, run requests next time");
        return;
    }

//    Logger::debug("es started=%d", event_stream.started());
    if (event_stream.started() && !event_stream.get_curl_handle()) {
        // start event stream listener if it not started yet
        handle = event_stream.make_curl_handle(_resolved_addrs);
        curl_multi_add_handle(_curl_multi, handle);
        Logger::debug("make event stream handle %p", handle);
    }

    curl_multi_perform(_curl_multi, &running);
//    Logger::debug("curl_multi_perform running=%d", running);

    while ((curlMsg = curl_multi_info_read(_curl_multi, &messagesLeft))) {
        CURL *e = curlMsg->easy_handle;
//        Logger::debug("curlMsg=%x msg=%d done=%d, handle=%x, left=%d", curlMsg, curlMsg->msg, CURLMSG_DONE, e, messagesLeft);
        if (curlMsg->msg == CURLMSG_DONE) {
            char *url;
            char host[200];
            curl_easy_getinfo(e, CURLINFO_EFFECTIVE_URL, &url);
            get_host_port_from_url(url, host);
            curl_multi_remove_handle(_curl_multi, e);
            Logger::debug("get curl response from handle: %p", e);
            if (curlMsg->data.result != CURLE_OK) {
                is_online = false;
            }
//            if (curlMsg->data.result == CURLE_OK) {
//                if (!_is_host_resolved(host)) {
//                    char *ip;
//                    curl_easy_getinfo(e, CURLINFO_PRIMARY_IP, &ip);
//                    _add_resolved(host, ip);
//                }
//            } else {
//                // connection errors may cause by old resolved addr so clear resolved addresses
//                is_online = false;
//            }
            if (e == event_stream.get_curl_handle()) {
//                curl_multi_remove_handle(_curl_multi, e);
                event_stream.clear_curl_handle();
                if (event_stream.started()) {
                    // start event stream listener again
                    Logger::debug("Restart eventstream handle: %p", e);
                    handle = event_stream.make_curl_handle(_resolved_addrs);
                    curl_multi_add_handle(_curl_multi, handle);
                    event_stream_restarted = true;
                }
            } else {
                CLHTTPRequest *req;
                curl_easy_getinfo(e, CURLINFO_PRIVATE, &req);
                int erased = _running_requests.erase(e);
                if (!erased) {
                    Logger::error("CLHTTPService::_process_request() req not erased from _running_requests (url:%s)", url);
                }
                if (curlMsg->data.result == CURLE_OK) {
                    if (req) {
                        if (req->needs_hourglass) {
                            g_hourglass_off();
                        }
                        req->response_url = url;
                        curl_easy_getinfo(e, CURLINFO_RESPONSE_CODE, &req->response_code);
                        req->process_response();
                        delete (req);
                    } else {
                        Logger::error("CLHTTPService::_process_request() missing req (%s)", url);
                    }
                } else {
                    if (req) {
                        const char* connection_error = curl_easy_strerror(curlMsg->data.result);
                        Logger::error("CLHTTPService::process() CURL error: %s resubmit request %s",
                                      connection_error, url);
                        req->cleanup();
                        req->init();
                        submit(req);
                    }
                }
            }
        } else {
            Logger::error("CLHTTPService::process() CURL MSG error: %d", curlMsg->msg);
        }
    }

    if (!event_stream_restarted && event_stream.started() && !event_stream.is_active()) {
        Logger::warn("CLHTTPService::event_stream inactive (restarting)");
        if (resolve_server_hostname()) {
            curl_multi_remove_handle(_curl_multi, event_stream.get_curl_handle());
            event_stream.clear_curl_handle();
            handle = event_stream.make_curl_handle(_resolved_addrs);
            curl_multi_add_handle(_curl_multi, handle);
        } else {
            is_online = false;
        }
    }
}

bool CLHTTPService::connected() {
    return (event_stream.started() && event_stream.is_connected());
}

bool CLHTTPService::_is_host_resolved(char* host) {
    struct curl_slist *addr = _resolved_addrs;
    int len = strlen(host);
    while(addr) {
        if (memcmp(host, addr->data, len) == 0) {
            return true;
        }
        addr = addr->next;
    }
    return false;
}

//void CLHTTPService::_load_resolved() {
//    char buf[255];
//    FILE *f = nullptr;
//
//    if (_resolved_addrs) {
//        curl_slist_free_all(_resolved_addrs);
//        _resolved_addrs = NULL;
//    }
//
//    if (is_file_exist(resolved_ip_file_path)) {
//        f = fopen(resolved_ip_file_path, "r");
//    }
//    if (f != nullptr) {
//        while(fgets(buf, sizeof(buf) - 1, f)) {
//            rtrim(buf);
//            if (strlen(buf) > 0) {
//                Logger::debug("add resolved: %s", buf);
//                _resolved_addrs = curl_slist_append(_resolved_addrs, buf);
//            }
//        }
//        fclose(f);
//    }
//}

void CLHTTPService::_add_resolved(char *host, char* ip) {
    char buf[200];

    sprintf(buf, "%s:%s", host, ip);
    _resolved_addrs = curl_slist_append(_resolved_addrs, buf);

//    FILE *f = nullptr;
//    struct curl_slist *addr = _resolved_addrs;
//    f = fopen(resolved_ip_file_path, "w");
//    while(addr) {
//        if (((char *)addr->data)[0] != '-') {
//            Logger::debug("save resolved: %s", addr->data);
//            fprintf(f, "%s\n", addr->data);
//            addr = addr->next;
//        }
//    }
//    fclose(f);
}

//void CLHTTPService::_clean_resolved(char* host) {
//    if (_resolved_addrs) {
//        Logger::debug("CURL error, clear resolved addrs");
//
//        char buf[200];
//        sprintf(buf, "-%s", host);
//        int len = strlen(buf);
//
//        struct curl_slist *addr = _resolved_addrs;
//        bool found = false;
//        while(addr) {
//            if (memcmp(buf, addr->data, len) == 0) {
//                found = true;
//                break;
//            }
//            addr = addr->next;
//        }
//        if (!found) {
//            _resolved_addrs = curl_slist_append(_resolved_addrs, buf);
////
////            FILE *f = fopen(resolved_ip_file_path, "w");
////            fclose(f);
//        }
//    }
//    is_online = false;
//}

/* CLHTTPEventStream */

size_t _eventStreamWriteFunction(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    ((CLHTTPEventStream *) userdata)->parse_raw_sse_event(ptr, size * nmemb);

    return size * nmemb;
}

void CLHTTPEventStream::parse_raw_sse_event(char* c, unsigned long size) {
    _last_activity_time = time(NULL);
    _last_receive_time = _last_activity_time;
    _sse_buf.append(c, size);
//    Logger::debug("parse_raw_sse_event buf=", _sse_buf.c_str());
//    debug_print_string(_sse_buf);
//    Logger::debug("");
    if (_sse_buf.at(_sse_buf.length() - 1) != '\n') {
//        printf("unfinished _sse_buf=[%s]", _sse_buf.c_str());
        return;
    }
    istringstream input(_sse_buf);
    string line;
    while(getline(input, line)) {
//        printf("line=%s _sse_buf=[%s]\n", line.c_str(), _sse_buf.c_str());
        if (line.substr(0, 5) == "data:") {
            string ev = line.substr(5, line.length() - 5);
            trim(ev);
            if (_sse_raw.length() > 0) {
                _sse_raw += "\n";
            }
            _sse_raw += ev;
//            printf("STRART EVENT line=%s ev=%s _sse_buf=[%s]", line.c_str(), _sse_raw.c_str(), _sse_buf.c_str());
        } else if (line.length() == 0) {
            //printf("NEW EVENT line=%s ev=%s _sse_buf=[%s]", line.c_str(), _sse_raw.c_str(), _sse_buf.c_str());
            on_raw_sse_event(_sse_raw);
            _sse_raw.clear();
        } else if (_sse_raw.length() > 0) {
//            printf("bad data line=%s ev=%s _sse_buf=[%s]", line.c_str(), _sse_raw.c_str(), _sse_buf.c_str());
            _sse_raw.clear();
        }
    }
    _sse_buf.clear();
}

void CLHTTPEventStream::on_raw_sse_event(string& raw) {
    cJSON *contentJson = cJSON_Parse(raw.c_str());
    const cJSON *objId, *objTime, *objData;

    Logger::debug("parse_raw_sse_event get RAW EVENT\n%s", raw.c_str());
    if (contentJson == NULL) {
        const char *errorPtr = cJSON_GetErrorPtr();
        if (errorPtr != NULL) {
            Logger::debug("on_raw_sse_event JSON parse error: %s [%s]", errorPtr, raw.c_str());
        }
        return;
    }
    // "{"id":1,"channel":"ROCHAT.m2","text":"","tag":"1","time":"Tue, 15 Oct 2019 11:50:56 GMT","eventid":""}"
    objId = cJSON_GetObjectItemCaseSensitive(contentJson, "id");
    if (!objId || !cJSON_IsInt(objId)) {
        Logger::debug("on_raw_sse_event JSON missing 'id' key [%s]", raw.c_str());
        return;
    }

    objTime = cJSON_GetObjectItemCaseSensitive(contentJson, "time");
    if (!objTime || !cJSON_IsString(objTime)) {
        Logger::debug("on_raw_sse_event JSON missing 'time' key [%s]", raw.c_str());
        return;
    }
    _last_sse_event_time = objTime->valuestring;
    //_last_sse_event_id = objId->valueint;

    objData = cJSON_GetObjectItemCaseSensitive(contentJson, "text");
    if (objData) {
        _events_handler(objData);
    }

    cJSON_Delete(contentJson);
}

CURL* CLHTTPEventStream::make_curl_handle(struct curl_slist * resolved_addrs) {
    char url[1000];
    if (_events_handler == nullptr) {
        Logger::debug("no _events_handler");
    }
    assert(_events_handler != nullptr);
    assert(!channel.empty());

    _curl_handle = curl_easy_init();
    curl_easy_setopt(_curl_handle, CURLOPT_WRITEFUNCTION, _eventStreamWriteFunction);
    curl_easy_setopt(_curl_handle, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(_curl_handle, CURLOPT_CONNECTTIMEOUT, DEFAULT_CONN_TIMEOUT);

    if (_last_sse_event_time.length() > 0) {
        char *escapedTime = curl_easy_escape(_curl_handle, _last_sse_event_time.c_str(), _last_sse_event_time.length());
        sprintf(url, "%sev/m%s?time=%s", base_url.c_str(), channel.c_str(), escapedTime);
        curl_free(escapedTime);
    } else {
        sprintf(url, "%sev/m%s", base_url.c_str(), channel.c_str());
    }
    curl_easy_setopt(_curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(_curl_handle, CURLOPT_USERAGENT, user_agent.c_str());
    curl_easy_setopt(_curl_handle, CURLOPT_CAINFO, "<ChatCube$Dir>.ssl.chain");
    curl_easy_setopt(_curl_handle, CURLOPT_SSL_VERIFYPEER, false);
    Logger::debug("CLHTTPEventStream::make_curl_handle url=%s", url);

    if (resolved_addrs) {
        curl_easy_setopt(_curl_handle, CURLOPT_RESOLVE, resolved_addrs);
        Logger::debug("CLHTTPEventStream::make_curl_handle resolved=%s", resolved_addrs->data);
    }

    _last_activity_time = time(NULL);

    return _curl_handle;
}


CLHTTPRequest::CLHTTPRequest(const char* a_method, const string &a_url) {
    url = a_url;
    method = a_method;
    init();
}


CLHTTPRequest::~CLHTTPRequest() {
    cleanup();
}

void CLHTTPRequest::init() {
    curl_handle = curl_easy_init();
//    Logger::debug("CLHTTPRequest::init: %x this: %x", curl_handle, this);
    if (!curl_handle) {
        Logger::error("CLHTTPRequest  curl_easy_init failed");
        throw(runtime_error(string("CLHTTPRequest curl_easy_init failed [")+url+"]"));
    }
}

void CLHTTPRequest::cleanup() {
    Logger::debug("Request cleanup %s curlhandle: %x this: %x", url.c_str(), curl_handle, this);
    if (_curl_formpost) {
        curl_formfree(_curl_formpost);
        _curl_formpost = NULL;
    }
    if (_curl_headers) {
        curl_slist_free_all(_curl_headers);
        _curl_headers = NULL;
    }

    if (curl_handle) {
//        Logger::debug("Request cleanup %s curl_easy_cleanup: %x this: %x", url.c_str(), curl_handle, this);
        curl_easy_cleanup(curl_handle);
        curl_handle = NULL;
    }

    if (response_json) {
        cJSON_Delete(response_json);
        response_json = NULL;
    }
}

std::string CLHTTPRequest::build_url_parameters(const CLStringsMap &post_data) {
    std::string parameters_string;
    for (auto &item: post_data) {
        if (parameters_string.length() > 0) {
            parameters_string += "&";
        }
        char *escaped  = curl_easy_escape(curl_handle, item.second.c_str(), item.second.length());
        parameters_string += item.first;
        parameters_string += "=";
        parameters_string += escaped;
        curl_free(escaped);
    }
    return parameters_string;
}

void CLHTTPRequest::set_timeout(const int _timeout) {
    timeout = _timeout;
}

void CLHTTPRequest::set_lowspeed_limit(const int time, const int limit) {
    lowspeed_limit = limit;
    lowspeed_time = timeout;
}

void CLHTTPRequest::set_hourglass(bool on) {
    needs_hourglass = on;
}

void CLHTTPRequest::set_url_parameters(CLStringsMap &data) {
    std::string fields = build_url_parameters(data);
    url = url + "?" + fields;
}

//void CLHTTPRequest::set_post_data(const CLStringsMap &_post_data) {
//    post_data = _post_data;
//}
//
//void CLHTTPRequest::add_file_upload(const char *field_name, const std::string& file_path) {
//    upload_files[field_name] = file_path;
//}

bool CLHTTPRequest::on_before_submit() {
    if (!upload_files.empty()) {
        for(auto &item: upload_files) {
            curl_formadd(&_curl_formpost,
                         &_curl_formlast,
                         CURLFORM_COPYNAME, item.first.c_str(),
                         CURLFORM_FILE, item.second.c_str(),
                         CURLFORM_FILENAME, file_basename_append_ext(item.second).c_str(),
                         CURLFORM_END);
            Logger::debug("uploading file %s->%s", item.first.c_str(), item.second.c_str());
        }
        for (auto &item: post_data) {
            curl_formadd(&_curl_formpost,
                         &_curl_formlast,
                         CURLFORM_COPYNAME, item.first.c_str(),
                         CURLFORM_COPYCONTENTS, item.second.c_str(),
                         CURLFORM_END);
            if (item.first != "password") {
                Logger::debug("POST %s: %s",  item.first.c_str(), item.second.c_str());
            }
        }
        curl_easy_setopt(curl_handle, CURLOPT_HTTPPOST, _curl_formpost);
        headers_map["Expect"] = "";

    } else if (!post_data.empty()) {
        std::string post_str = build_url_parameters(post_data);
        if (post_str.find("password=") == std::string::npos) {
            Logger::debug("POST data:%s",  post_str.c_str());
        }
        curl_easy_setopt(curl_handle, CURLOPT_COPYPOSTFIELDS, post_str.c_str());

    } else if (method == "POST") {
        curl_easy_setopt(curl_handle, CURLOPT_COPYPOSTFIELDS, "");
    }

    if (method != "POST" && method != "GET") {
        curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, method.c_str());
    }

    for (auto &item: headers_map) {
        char hout[255];
        sprintf(hout, "%s: %s", item.first.c_str(), item.second.c_str());
        _curl_headers = curl_slist_append(_curl_headers, hout);
        Logger::debug("headers: %s", hout);
    }

    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, _curl_headers);
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl_handle, CURLOPT_MAXREDIRS, 5);
    curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, DEFAULT_CONN_TIMEOUT);
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, timeout);
    curl_easy_setopt(curl_handle, CURLOPT_LOW_SPEED_TIME, lowspeed_time);
    curl_easy_setopt(curl_handle, CURLOPT_LOW_SPEED_LIMIT, lowspeed_limit);
    return true;
};

unsigned long CLHTTPRequest::on_append_content(char* c, unsigned long size) {
    response_text.append(c, size);
    return size;
}

void CLHTTPRequest::process_response() {
//    fprintf(stderr, "response=%s", response_text.c_str());
    Logger::debug("response=%s", response_text.c_str());
    if (response_code > 0) {
        response_json = cJSON_Parse(response_text.c_str());
    }
    if (response_code > 100 and response_code < 400) {
        on_success();
    } else {
        on_fail();
    }
    on_finally();
}

int CLHTTPRequest::on_progress(curl_off_t dltotal, curl_off_t dlnow,
                               curl_off_t ultotal, curl_off_t ulnow) {
    if (cancel_loading) {
        return 1;
    }
    return 0;
};


CLHTTPService g_http_service;
