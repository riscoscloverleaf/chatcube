//
// Created by lenz on 2/3/20.
//

#ifndef ROCHAT_NETWORKREQUESTS_H
#define ROCHAT_NETWORKREQUESTS_H
#include <functional>
#include "../service/CLHTTPService_v2.h"
#include "AppEvents.h"

struct HttpFieldValidationError {
    std::string error_code;
    std::string error_message;
};

class HttpRequestError {
public:
    HttpRequestError() {};
    HttpRequestError(const int code, std::string message)
            : http_code(code), error_message(message) {};
    HttpRequestError(const int code, std::string message, const std::string str_code)
            : http_code(code), error_message(message), error_code(str_code) {};
    int http_code = 0;
    std::string error_code;
    std::string error_message;

    std::map<std::string, HttpFieldValidationError> field_errors;
};

typedef std::function<void(CLHTTPRequest* req)> RequestSuccessCallbackType;
typedef std::function<bool(const HttpRequestError& err)> RequestFailCallbackType;
typedef std::function<void(curl_off_t, curl_off_t, curl_off_t, curl_off_t)> ProgressCallbackType;
typedef std::function<void(const std::string& file_name)> FileDownloadSuccessCallbackType;

class CLChatRequest : public CLHTTPRequest {
private:
    RequestSuccessCallbackType success_callback;
    RequestFailCallbackType fail_callback;
    bool has_success_callback = false;
    bool has_fail_callback = false;
    HttpRequestError parse_error();
public:
    CLChatRequest(const char* a_method, const std::string &a_url);
    CLChatRequest(const char* a_method, const std::string &a_url, RequestSuccessCallbackType success_callback);
    CLChatRequest(const char* a_method, const std::string &a_url, RequestSuccessCallbackType success_callback, RequestFailCallbackType fail_callback);

    void set_success_callback(RequestSuccessCallbackType& on_success);
    void set_fail_callback(RequestFailCallbackType& on_fail);
    virtual void process_response() override;
    virtual int on_progress(curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) override ;

    virtual void on_fail() override;
    virtual void on_success() override;
};


class CLDownloadFileRequest: public CLChatRequest {
protected:
    FILE *saved_file = NULL;
    int file_type = 0xFFD; // ffd - Data;
    static int file_id;
    curl_off_t total_size = 0;
public:
    curl_off_t total_size_hint = 0;
    std::string saved_file_path;

    CLDownloadFileRequest(const std::string& url, RequestSuccessCallbackType success_callback);
    CLDownloadFileRequest(const std::string& url, RequestSuccessCallbackType success_callback, RequestFailCallbackType fail_callback);
    ~CLDownloadFileRequest() override ;
    bool on_before_submit() override;
    virtual unsigned long on_append_content(char* c, unsigned long size) override;
    virtual void process_response() override;
    void on_success() override;
    int get_riscos_file_type();
    void set_default_file_type(int t) { file_type = t; };
    curl_off_t get_total_size();
    bool move_file(const char* to);
};

class CLChatApiRequest: public CLChatRequest {
public:
    CLChatApiRequest(const char* a_method, const std::string &a_url) : CLChatRequest(a_method, a_url) {};
    CLChatApiRequest(const char* a_method, const std::string &a_url, RequestSuccessCallbackType success_callback) : CLChatRequest(a_method, a_url, success_callback) {};
    CLChatApiRequest(const char* a_method, const std::string &a_url, RequestSuccessCallbackType success_callback, RequestFailCallbackType fail_callback)  : CLChatRequest(a_method, a_url, success_callback, fail_callback) {};
    bool on_before_submit() override;
};
#endif //ROCHAT_NETWORKREQUESTS_H
