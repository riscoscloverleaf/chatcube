//
// Created by lenz on 2/27/20.
//

#ifndef ROCHAT_CHATMAINUI_H
#define ROCHAT_CHATMAINUI_H
#include <list>
#include <functional>
#include "../model/AppDataModel.h"
#include "ChatListView.h"
#include "MessagesListView.h"
#include "cloverleaf/CLGraphics.h"
#include <tbx/window.h>
#include <tbx/actionbutton.h>
#include <tbx/writablefield.h>
#include <tbx/optionbutton.h>
#include <tbx/objectdelete.h>
#include <tbx/redrawlistener.h>
#include <tbx/mouseclicklistener.h>
#include <tbx/menuselectionlistener.h>
#include <tbx/hasbeenhiddenlistener.h>
#include <tbx/offsetgraphics.h>
#include <tbx/toolaction.h>
#include <tbx/loader.h>
#include <tbx/gadget.h>
#include <tbx/command.h>
#include <tbx/keylistener.h>

#define TOOLBAR_MODE_INPUT 1
#define TOOLBAR_MODE_REPLY 2
#define TOOLBAR_MODE_SEARCH 3

class ChatMainUI {
    tbx::Point input_textarea_pos;
    int64_t editing_message_id = 0;
    int64_t replying_message_id = 0;
    int messages_toolbar_height = 0;
    std::list<tbx::CommandMethod<ChatMainUI>*> command_methods;
public:
    tbx::CommandMethod<ChatMainUI>* make_command_method(void (ChatMainUI::*method)());

    ChatsListView chatlist_view;
    MessagesListView messages_view;
    tbx::Window chats_toolbar_win;
    tbx::Window input_toolbar_win;
    tbx::Window search_toolbar_win;
    tbx::Window reply_toolbar_win;
    tbx::Window messages_down_win;

    tbx::Menu msg_menu;
    tbx::Menu chat_menu;
    MessageListViewItem *clicked_msg_view_item;
    ChatListViewItem *clicked_chat_view_item;
    std::list<int64_t> found_message_ids;
    bool searching_up = false;
    unsigned int last_type_time;
    int prev_scroll_y = 0;
    int toolbar_mode;
    bool is_messages_needs_reload;
    bool is_windows_set_up;
    bool is_messages_down_arrow_shown = false;

    static ChatMainUI* instance;
    ChatMainUI();
    ~ChatMainUI();

    static void open();
    static void hide();
    void setup_subwindows();
    void setup_messages_toolbar_subwindow();
    void setup_messages_down_arrow(int show = -1);
    void handle_scroll_y(int scroll_y, int visible_height);

    void on_chat_added(const AppEvents::ChatAdded& ev);
    void on_chat_changed(const AppEvents::ChatChanged& ev);
    void on_chat_list_loaded(const AppEvents::ChatListLoaded& ev);
    void on_chat_deleted(const AppEvents::ChatDeleted& ev);
    void on_chat_cleared(const AppEvents::ChatCleared& ev);
    void on_opened_chat_changed(const AppEvents::OpenedChatChanged& ev);
    void on_messages_loaded(const AppEvents::MessagesLoaded& ev);
    void on_message_added(const AppEvents::MessageAdded& ev);
    void on_message_changed(const AppEvents::MessageChanged& ev);
    void on_message_deleted(const AppEvents::MessageDeleted& ev);
    void on_login(const AppEvents::LoggedIn& ev);

    void typing_notify(bool start);
    void check_typing_notify_timeout();

    void submit_message();

    inline bool is_editing() {
        return editing_message_id != 0;
    }
    inline bool is_replying() {
        return toolbar_mode == TOOLBAR_MODE_REPLY;
    }
    inline bool is_searching() {
        return toolbar_mode == TOOLBAR_MODE_SEARCH;
    }
    void screenshoot_full();
    void screenshoot_hide_full();
    void screenshoot_area();
    void screenshoot_hide_area();
    void screenshoot_window();
    void start_send_sticker();
    void delete_msg();
    void unsend_msg();
    void forward_msg();
    void manage_group();
    void view_profile();
    void create_private_chat();
    void enter_editing();
    void leave_editing();
    void enter_replying();
    void leave_replying();
    void enter_searching();
    void leave_searching();
    void search_begin();
    void search_reset_found();
    void search_next();
    void search_prev();
    void submit_edited_message();
    void submit_replyed_message();
    void download_chat_history();
    void copy_message_to_clipboard();

    bool has_input_focus();
};


#endif //ROCHAT_CHATMAINUI_H
