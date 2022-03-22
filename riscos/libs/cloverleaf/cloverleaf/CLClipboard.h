//
// Created by lenz on 7/8/21.
//

#ifndef ROCHAT_CLCLIPBOARD_H
#define ROCHAT_CLCLIPBOARD_H

#include <string>
#include <functional>
#include <tbx/wimpmessagelistener.h>

typedef struct
{
    int len;
    int write_offset;
    char text[0];
} clipboard_text;

class CLClipboard : public tbx::WimpUserMessageListener, public tbx::WimpRecordedMessageListener, public tbx::WimpAcknowledgeMessageListener
{
private:
    static CLClipboard* _instance;

    bool own_clipboard = false;
    bool has_get_clipboard_success_callback = false;
    int paste_msg = 0;
    int paste_offset = 0;
    int check_msg = 0;
    int fetch_msg = 0;
    std::function<void(char*)> clipboard_get_success_callback;
    clipboard_text *clip_text = nullptr;

    clipboard_text* buffer_alloc(const char *text, int len, int remain);
    void buffer_free();
    void handle_event(tbx::WimpMessageEvent &event);


public:
    CLClipboard();
    ~CLClipboard();

    static CLClipboard* instance();
    void clipboard_set(const std::string& text);
    void clipboard_get(std::function<void(char*)> on_success);

    virtual void user_message(tbx::WimpMessageEvent &event) { handle_event(event); }
    virtual void recorded_message(tbx::WimpMessageEvent &event, int reply_to) { handle_event(event); }
    virtual void acknowledge_message(tbx::WimpMessageEvent &event) { handle_event(event); }
};


#endif //ROCHAT_CLCLIPBOARD_H
