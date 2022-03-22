//
// Created by lenz on 2/3/20.
//
#include <sstream>
#include <errno.h>
#include <unistd.h>
#include <cloverleaf/Logger.h>
#include "NetworkRequests.h"
#include "AppEvents.h"
#include "JsonData.h"

#define FILE_TYPE_PNG 0xB60
#define FILE_TYPE_JPEG 0xC85
#define FILE_TYPE_GIF 0x695
#define DEFAULT_TIMEOUT 60

const static char* _tmp_downloads_dir = "<Choices$Write>.ChatCube.temp_downloads";

CLChatRequest::CLChatRequest(const char* a_method, const string &a_url) :
        CLHTTPRequest(a_method, a_url) {
    set_timeout(DEFAULT_TIMEOUT);
};

CLChatRequest::CLChatRequest(const char* a_method, const string &a_url, RequestSuccessCallbackType success_callback) :
        CLHTTPRequest(a_method, a_url) {
    set_success_callback(success_callback);
    set_timeout(DEFAULT_TIMEOUT);
}

CLChatRequest::CLChatRequest(const char* a_method, const string &a_url, RequestSuccessCallbackType success_callback, RequestFailCallbackType fail_callback) :
        CLHTTPRequest(a_method, a_url) {
    set_success_callback(success_callback);
    set_fail_callback(fail_callback);
    set_timeout(DEFAULT_TIMEOUT);
}


void CLChatRequest::set_success_callback(RequestSuccessCallbackType& on_success) {
    success_callback = on_success;
    has_success_callback = true;
}
void CLChatRequest::set_fail_callback(RequestFailCallbackType& on_fail) {
    fail_callback = on_fail;
    has_fail_callback = true;
}

void CLChatRequest::on_success() {
    if (has_success_callback) {
        success_callback(this);
    }
}

int CLChatRequest::on_progress(curl_off_t dltotal, curl_off_t dlnow,
                               curl_off_t ultotal, curl_off_t ulnow) {
    //Logger::debug("CLChatRequest::on_progress req=%p dltotal=%lld dlnow=%lld ultotal=%lld ulnow=%lld", this, dltotal, dlnow, ultotal, ulnow);
    if (upload_files.empty() && dltotal > 0) {
        AppEvents::DownloadingProgress ev;
        ev.req = this;
        ev.percent_done = ((dlnow * 100) / dltotal);
        if (ev.percent_done >= 100) {
            ev.percent_done = 99;
        }
        //Logger::debug("CLChatRequest::on_progress send event pct:%d", ev.percent_done);
        g_app_events.notify(ev);
    } else if (ultotal > 0) {
        AppEvents::UploadingProgress ev;
        ev.req = this;
        ev.percent_done = ((ulnow * 100) / ultotal);
        if (ev.percent_done >= 100) {
            ev.percent_done = 99;
        }
        g_app_events.notify(ev);
    }
}

void CLChatRequest::process_response() {
    if (needs_progress) {
        if (upload_files.empty()) {
            AppEvents::DownloadingProgress ev;
            ev.req = this;
            ev.percent_done = 100;
            g_app_events.notify(ev);
        } else {
            AppEvents::UploadingProgress ev;
            ev.req = this;
            ev.percent_done = 100;
            g_app_events.notify(ev);
        }
    }
    CLHTTPRequest::process_response();
}

HttpRequestError CLChatRequest::parse_error() {
    if (response_code == 404) {
        return HttpRequestError(response_code, "Resource not found");
    }

    if (!response_json) {
        return HttpRequestError(response_code, std::move(response_text));
    }

    if (!JsonData::has_value(response_json, "validation_error")) {
        std::string message = JsonData::get_string_value(response_json, "message", response_text);
        std::string code = JsonData::get_string_value(response_json, "code", "");
        return HttpRequestError(response_code, std::move(message), std::move(code));
    }

    const cJSON *validation_error = JsonData::get_json_object_or_null(response_json, "validation_error");
    HttpRequestError http_validation_error;
    http_validation_error.http_code = response_code;
    if (cJSON_IsObject(validation_error)) {
        cJSON *field_item;
        cJSON_ArrayForEach(field_item, validation_error) {
            std::string name = field_item->string;
            cJSON *message_item;
            HttpFieldValidationError field = {.error_code = "", .error_message = ""};
            cJSON_ArrayForEach(message_item, field_item) {
                if (!field.error_message.empty()) {
                    field.error_message += "; ";
                }
                field.error_message += JsonData::get_string_value(message_item, "message");
                field.error_code = JsonData::get_string_value(message_item, "code", "");
            }
            http_validation_error.field_errors[name] = field;
            if (!http_validation_error.error_message.empty()) {
                http_validation_error.error_message += "; ";
            }
            http_validation_error.error_message += name;
            http_validation_error.error_message += ": ";
            http_validation_error.error_message += field.error_message;
        }
    } else {
        cJSON *message_item;
        cJSON_ArrayForEach(message_item, validation_error) {
            if (!http_validation_error.error_message.empty()) {
                http_validation_error.error_message += "; ";
            }
            http_validation_error.error_message += JsonData::get_string_value(message_item, "message");
            http_validation_error.error_code = JsonData::get_string_value(message_item, "code", "");
        }
    }
    return http_validation_error;
}

void CLChatRequest::on_fail() {
    Logger::error("CLChatRequest::on_fail: HTTP url: %s, response_code: %d text: %s\n", response_url, response_code, response_text.c_str());

    if (needs_progress) {
        AppEvents::ProgressBarControl pbreq;
        pbreq.label = "";
        g_app_events.notify(pbreq);
    }

    HttpRequestError err = parse_error();
    if (has_fail_callback) {
        if (fail_callback(err)) { // fail_callback handle error, no need to use default handlers
            return;
        }
    }

    if (err.http_code == 0) {
//        Logger::error("network error. URL: %s, response_code=0\n", response_url);
        show_alert_error(("You have problem with your internet connection. Please check and try again. " +
                          err.error_message).c_str());
    } else if (err.http_code == 401) {
        g_app_events.notify(AppEvents::LoginRequired {});
    }

    show_alert_error(("HTTP request failed: "+err.error_message.substr(0, 200)).c_str());
}

CLDownloadFileRequest::CLDownloadFileRequest(const string &url, RequestSuccessCallbackType success_callback) :
        CLChatRequest("GET", url, success_callback) {
    set_timeout(0);
}

CLDownloadFileRequest::CLDownloadFileRequest(const string &url, RequestSuccessCallbackType success_callback, RequestFailCallbackType fail_callback) :
        CLChatRequest("GET", url, success_callback, fail_callback) {
    set_timeout(0);
}
int CLDownloadFileRequest::file_id = 0;

bool CLDownloadFileRequest::on_before_submit() {
    if (!CLChatRequest::on_before_submit()) {
        return false;
    }
    curl_easy_setopt(curl_handle, CURLOPT_ACCEPT_ENCODING, "");
    set_lowspeed_limit(30,10);
    return true;
}

curl_off_t CLDownloadFileRequest::get_total_size() {
    if (!total_size) {
        double content_length;
        curl_easy_getinfo(curl_handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &content_length);
        if (content_length != -1) {
            total_size = content_length;
        } else {
            total_size = total_size_hint;
        }
    }
    return total_size;
}

unsigned long CLDownloadFileRequest::on_append_content(char* c, unsigned long size) {
    unsigned long written;
    if (!saved_file) {
        std::stringstream ss;
        if (!is_directory_exist (_tmp_downloads_dir)) {
            mkdir(_tmp_downloads_dir, 0777);
        }
        char tmp[255];
        sprintf(tmp, "%s.temp%dXXXXXX", _tmp_downloads_dir, file_id++);
        mktemp(tmp);
        saved_file_path = tmp;
        saved_file = fopen(saved_file_path.c_str(), "wb");
        if (!saved_file) {
            char err_buffer[ 256 ];
            strerror_r(errno, err_buffer, 256 ); // get string message from errno, XSI-compliant version
            response_text = "Can't write to: " + saved_file_path + " error:" + err_buffer;
            response_code = 0;
            Logger::error("CLDownloadFileRequest::on_append_content %s", response_text.c_str());
            return 0;
        } else {
            Logger::debug("CLDownloadFileRequest::on_append_content created tmp file: %s", saved_file_path.c_str());
        }
    }
    written = fwrite(c, 1, size, saved_file);
    if (needs_progress) {
        curl_off_t total = get_total_size();
        unsigned long saved_size = ftell(saved_file);
        // if total is unknown then use some predefined values just to draw something on progress bar
        if (total == 0) {
            if (saved_size < 10000) {
                total = 10000;
            } else if (saved_size < 1000000) {
                total = 1000000;
            } else if (saved_size < 100000000) {
                total = 100000000;
            } else {
                total = 0xffffffff;
            }
        }
        on_progress(total, saved_size, 0, 0);
        //Logger::error("CLDownloadFileRequest::on_append_content %ld %lld", saved_size, total_size);
    }
    return written;
}

void CLDownloadFileRequest::process_response() {
    fclose(saved_file);
    saved_file = NULL;
    CLChatRequest::process_response();
}

CLDownloadFileRequest::~CLDownloadFileRequest() {
    Logger::debug("~CLDownloadFileRequest %s this:%p", saved_file_path.c_str(), this);
    if (!saved_file_path.empty()) {
        Logger::debug("~CLDownloadFileRequest removed: %s", saved_file_path.c_str());
        unlink(saved_file_path.c_str());
    }
}

int CLDownloadFileRequest::get_riscos_file_type() {
    char *ctype;
    curl_easy_getinfo(curl_handle, CURLINFO_CONTENT_TYPE, &ctype);
    if (ctype != NULL) {
        if (stricmp(ctype, "image/jpeg") == 0) {
            return FILE_TYPE_JPEG;
        }
        if (stricmp(ctype, "image/png") == 0) {
            return FILE_TYPE_PNG;
        }
        if (stricmp(ctype, "image/gif") == 0) {
            return FILE_TYPE_GIF;
        }
        if (stricmp(ctype, "application/zip") == 0) {
            return FILE_TYPE_ZIP;
        }
        if (strnicmp(ctype, "text/html", 9) == 0) {
            return FILE_TYPE_HTML;
        }
    }
    return file_type;
}

bool CLDownloadFileRequest::move_file(const char* to) {
    if (is_file_exist(saved_file_path)) {
        unlink(to);
        create_directories_for_file(to);
        if (rename(saved_file_path.c_str(), to) != -1) {
            Logger::debug("Downloaded %s -> %s", response_url, to);
            return true;
        } else {
            Logger::error("Failed to rename downloaded file %s -> %s Error: %s", saved_file_path.c_str(), to, strerror(errno));
        };
    }
    Logger::error("Downloaded file missing %s -> %s this:%p", response_url, to, this);
    return false;
}

void CLDownloadFileRequest::on_success() {
    set_file_type(saved_file_path.c_str(), get_riscos_file_type());
    CLChatRequest::on_success();
}


bool CLChatApiRequest::on_before_submit() {
    headers_map["Accept"] = "application/json";
    return CLChatRequest::on_before_submit();
}