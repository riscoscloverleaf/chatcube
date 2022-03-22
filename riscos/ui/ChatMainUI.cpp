//
// Created by lenz on 2/27/20.
//
#include <string>
#include <tbx/showsubwindowspec.h>
#include <tbx/path.h>
#include <tbx/swixcheck.h>
#include <tbx/questionwindow.h>
#include <tbx/displayfield.h>
#include <tbx/button.h>
#include <tbx/buttonselectedlistener.h>
#include <tbx/window.h>
#include <tbx/abouttobeshownlistener.h>
#include <tbx/monotonictime.h>
#include <tbx/radiobutton.h>
#include <tbx/adjuster.h>
#include <tbx/openwindowlistener.h>
#include <tbx/stringset.h>
#include <tbx/textarea.h>
#include <tbx/textchangedlistener.h>
#include <tbx/posteventlistener.h>
#include <tbx/postpolllistener.h>
#include <tbx/res/resmenu.h>
#include "cloverleaf/CLClipboard.h"
#include "cloverleaf/IdleTask.h"
#include "cloverleaf/Logger.h"
#include "cloverleaf/CLUtf8.h"
#include "cloverleaf/CLSound.h"
#include "ChatMainUI.h"
#include "SendStickerUI.h"
#include "ScreenshootUI.h"
#include "ForwardMessageDialog.h"
#include "ManageGroupDialog.h"
#include "ViewProfileDialog.h"
#include "../global.h"
#include "../utils.h"
#include "../service/IKConfig.h"

#define TYPING_NOTIFY_TIMEOUT 500

static void copy_to_downloads_and_open(const std::string& src, const std::string& name, int file_type) {
    std::string dst = "<Choices$Write>.ChatCube.downloads."+str_replace_all(name, ".", "/");
    tbx::Path src_path(src);
    src_path.copy(dst);
    tbx::Path dst_path(dst);
    if (file_type) {
        dst_path.file_type(file_type);
    }
    std::string dst_dir = dst_path.parent();
    os_cli(string("*Filer_OpenDir " + dst_dir).c_str());
}

class DownloadAttachmentCmd : public tbx::Command {
public:
    MessageListViewItem *view_item = nullptr;
    bool opening_attachment = false;
    bool open = false;

    void download(MessageListViewItem *_view_item, bool open_attachment = false) {
        if (!opening_attachment) {
            open = open_attachment;
            view_item = _view_item;
            execute();
        }
    }

    void execute() override {
        if (opening_attachment || !view_item) {
            return;
        }
        opening_attachment = true;
        std::string attachment_url;
        std::string attachment_name;
        int file_type = 0;
        int file_size = 0;
        if (view_item->value->att_image) {
            attachment_url = view_item->value->att_image->url;
            file_size = view_item->value->att_image->size;
        } else if (view_item->value->att_file) {
            file_type = view_item->value->att_file->file_type;
            attachment_name = view_item->value->att_file->name;
            attachment_url = view_item->value->att_file->url;
            file_size = view_item->value->att_file->size;
        }
        Logger::debug("Download attachment, msgid:%lld size=%d open:%d", view_item->value->id, file_size, open);
        if (file_size > 50000) {
            AppEvents::ProgressBarControl pbreq;
            pbreq.label = "Downloading attachment";
            pbreq.download_url = attachment_url;
            g_app_events.notify(pbreq);
        }
//                Logger::debug("download attachment %s", attachment_url.c_str());
        auto callback = [this, attachment_name, file_type](const std::string& file_name) {
            opening_attachment = false;
            if (!file_name.empty()) {
                tbx::Path dst_path(file_name);
                int ft = file_type;
                if (file_type) {
                    dst_path.file_type(file_type);
                } else {
                    ft = dst_path.file_type();
                }
                if (open) { // image
                    if (can_run_file_type(ft)) {
                        if (!attachment_name.empty()) {
                            copy_to_downloads_and_open(file_name, attachment_name, ft);
                        }
                        os_cli(string("*Filer_Run " + file_name).c_str());
                    } else {
                        if (attachment_name.empty()) {
                            copy_to_downloads_and_open(file_name, file_basename(file_name), ft);
                        } else {
                            copy_to_downloads_and_open(file_name, attachment_name, ft);
                        }
                    }
                } else {
                    if (attachment_name.empty()) {
                        copy_to_downloads_and_open(file_name, file_basename(file_name), ft);
                    } else {
                        copy_to_downloads_and_open(file_name, attachment_name, ft);
                    }
                }
            }
            open = false;
        };
        g_file_cache_downloader.download_url(attachment_url, callback, true, true, file_size);
        view_item = nullptr;
    }
} cmd_download_attachment;

class ShowDeleteChatQuestionCommand : public tbx::Command, public tbx::HasBeenHiddenListener {
private:
    tbx::Window *win = nullptr;
public:
    ChatListViewItem *chat_view_item = nullptr;

    class DeleteChatCmd : public tbx::Command {
    public:
        ShowDeleteChatQuestionCommand *parent = nullptr;
        void execute() override {
            bool unsend = false;
            ChatDataPtr chat = parent->chat_view_item->value;
            Logger::debug("DeleteChatCmd %s", chat->title.c_str());
            if ((chat->type == CHAT_TYPE_PRIVATE || chat->type == CHAT_TYPE_SECRET)) {
                unsend = tbx::OptionButton(parent->win->gadget(1)).on();
            }
            g_app_data_model.delete_chat_history(chat, true, unsend);
        }
    } cmd_delete_chat;

    tbx::Window* make_window() {
        if (win) {
            win->delete_object();
            delete win;
        }
        if ((chat_view_item->value->type == CHAT_TYPE_PRIVATE || chat_view_item->value->type == CHAT_TYPE_SECRET)) {
            win = new tbx::Window("chatConfrm1");
        } else {
            win = new tbx::Window("chatConfrm2");
        }
        win->add_command(0x1001, &cmd_delete_chat);
        win->add_has_been_hidden_listener(this);
    }

    void execute() override {
        chat_view_item = ChatMainUI::instance->clicked_chat_view_item;
        ChatDataPtr chat = chat_view_item->value;
        cmd_delete_chat.parent = this;
        make_window();
        if ((chat->type == CHAT_TYPE_PRIVATE || chat->type == CHAT_TYPE_SECRET)) {
            auto opt = tbx::OptionButton(win->gadget(1));
            opt.label("Delete chat also for: "+utf8_to_riscos_local(chat->title));
            opt.on(false);
            tbx::Button(win->gadget(0)).value("Are you sure to delete chat and clear history?");
        } else if (chat->type == CHAT_TYPE_GROUP) {
            tbx::Button(win->gadget(0)).value("Are you sure to leave group?");
        } else {
            tbx::Button(win->gadget(0)).value("Are you sure to leave channel?");
        }
        win->show();
    }

    void has_been_hidden(const tbx::EventInfo &event_info) override {
        win->delete_object();
        delete win;
        win = nullptr;
    }
} cmd_delete_chat_question;

class ShowClearHistoryChatQuestionCommand : public tbx::Command, public tbx::HasBeenHiddenListener
{
private:
    tbx::Window *win = nullptr;
public:
    ChatListViewItem *chat_view_item = nullptr;

    class ClearChatHistoryCmd : public tbx::Command {
    public:
        ShowClearHistoryChatQuestionCommand *parent = nullptr;

        void execute() override {
            bool unsend = false;
            ChatDataPtr chat = parent->chat_view_item->value;
            Logger::debug("ClearChatHistoryCmd %s", chat->title.c_str());
            if ((chat->type == CHAT_TYPE_PRIVATE || chat->type == CHAT_TYPE_SECRET)) {
                unsend = tbx::OptionButton(parent->win->gadget(1)).on();
            }
            g_app_data_model.delete_chat_history(chat, false, unsend);
        }
    } cmd_clear_chat_history;

    tbx::Window* make_window() {
        if (win) {
            win->delete_object();
            delete win;
        }
        if ((chat_view_item->value->type == CHAT_TYPE_PRIVATE || chat_view_item->value->type == CHAT_TYPE_SECRET)) {
            win = new tbx::Window("chatConfrm1");
        } else {
            win = new tbx::Window("chatConfrm2");
        }
        win->add_command(0x1001, &cmd_clear_chat_history);
        win->add_has_been_hidden_listener(this);
    }

    void execute() override {
        chat_view_item = ChatMainUI::instance->clicked_chat_view_item;
        cmd_clear_chat_history.parent = this;
        ChatDataPtr chat = chat_view_item->value;
        make_window();
        tbx::Button(win->gadget(0)).value("Are you sure to clear history in chat?");
        if ((chat->type == CHAT_TYPE_PRIVATE || chat->type == CHAT_TYPE_SECRET)) {
            auto opt = tbx::OptionButton(win->gadget(1));
            opt.label("Clear history also for: "+utf8_to_riscos_local(chat->title));
            opt.on(false);
        }
        win->show();
    }

    void has_been_hidden(const tbx::EventInfo &event_info) override {
        win->delete_object();
        delete win;
        win = nullptr;
    }
} cmd_clear_chat_history_question;

class MessagesWindowHiddenListener : public tbx::HasBeenHiddenListener {
public:
    void has_been_hidden(const tbx::EventInfo &event_info) override {
//        ChatMainUI::instance->is_window_shown = false;
//        Logger::debug("MessagesWindowHiddenListener");
        g_app_state.is_main_window_shown = false;
    };
} messages_window_hidden_listener;

class MessagesWindowShownListener : public tbx::AboutToBeShownListener {
public:
    void about_to_be_shown(tbx::AboutToBeShownEvent &event) override {
        ChatMainUI *ui = ChatMainUI::instance;
        g_app_state.is_main_window_shown = true;
//        ui->is_window_shown = true;
        if (ui->is_messages_needs_reload) {
            ui->is_messages_needs_reload = false;
            g_app_events.notify(AppEvents::MessagesLoaded {.chat=g_app_data_model.get_currently_opened_chat(), .is_first_load=true });
        }

//        Logger::debug("MessagesWindowShownListener");
    }
} messages_window_shown_listener;

class MessageClickListener : public ViewItemClickListener<MessageListViewItem> {
public:
//    bool opening_avatar;
//    MessageClickListener() : opening_avatar(false) {}
// MessageClickListener
//    void open_avatar(MessageListViewItem *view_item) {
//        opening_avatar = true;
//        string avatar_path = g_file_cache_downloader.get_cached_file_for_url(view_item->value->author->pic);
//        if (!avatar_path.empty()) {
//            Logger::debug("open big avatar %s\n", avatar_path.c_str());
//            os_cli(string("*Filer_Run " + avatar_path).c_str());
//            opening_avatar = false;
//        } else {
//            auto callback = [this](const std::string& file_name) {
//                if (!file_name.empty()) {
//                    Logger::debug("open downloaded big avatar %s\n", file_name.c_str());
//                    os_cli(string("*Filer_Run " + file_name).c_str());
//                }
//                opening_avatar = false;
//            };
//
//            g_file_cache_downloader.download_url(view_item->value->author->pic, callback, true);
//        }
//    }

    void item_clicked(ViewItemClickEvent<MessageListViewItem> &ev)  override {
        MessageListViewItem *view_item = &ev.view_item();
        ChatMainUI::instance->clicked_msg_view_item = view_item;
        //Logger::debug("MessageClickListener %s wkhit %d,%d att %d,%d %d,%d", ev.view_item().value->text.c_str(), ev.workarea_hit_point().x, ev.workarea_hit_point().y, view_item.clickable_attachment.min.x, view_item.clickable_attachment.max.y, view_item.clickable_attachment.max.x, view_item.clickable_attachment.min.y);
        //Logger::debug("MessageClickListener %s %d", ev.view_item().value->text.c_str(), ev.click_event().is_menu());
        if (ev.click_event().is_menu()) {
            MessageDataPtr msg = view_item->value;
            ChatDataPtr chat = msg->get_chat();
            bool delete_disabled = chat->type == CHAT_TYPE_CHANNEL;
            bool unsend_disabled = delete_disabled || !g_app_data_model.is_author_me(msg);
            bool reply_disabled = delete_disabled || msg->is_outgoing() || msg->is_system() || msg->is_deleted() || msg->id == 0 || ChatMainUI::instance->is_editing();
            bool edit_disabled = !msg->is_outgoing() || msg->id == 0 || ChatMainUI::instance->is_replying();
            bool create_private_disabled = chat->type != CHAT_TYPE_GROUP || !msg->author || g_app_data_model.is_author_me(msg) || ChatMainUI::instance->is_replying() || ChatMainUI::instance->is_editing();
            //ChatMainUI::instance->msg_menu.title("Message (id:"+to_string(view_item->value->id)+")");
            tbx::MenuItem(tbx::Component(ChatMainUI::instance->msg_menu, 1)).fade(delete_disabled);
            tbx::MenuItem(tbx::Component(ChatMainUI::instance->msg_menu, 2)).fade(unsend_disabled);
            // download
            tbx::MenuItem(tbx::Component(ChatMainUI::instance->msg_menu, 3)).fade(!msg->has_attachment());
            // edit
            tbx::MenuItem(tbx::Component(ChatMainUI::instance->msg_menu, 5)).fade(edit_disabled);
            // reply
            tbx::MenuItem(tbx::Component(ChatMainUI::instance->msg_menu, 6)).fade(reply_disabled);
            // create private chat
            tbx::MenuItem(tbx::Component(ChatMainUI::instance->msg_menu, 7)).fade(create_private_disabled);

            cmd_download_attachment.view_item = view_item;
            ChatMainUI::instance->msg_menu.show_at_pointer();
        } else if (ev.click_event().button() & 1024 || ev.click_event().is_select()) {
//            Logger::debug("Click select parts: %d", view_item->clickable_parts.size());
            for(auto &part : view_item->clickable_parts) {
//                if (part.type == CLICKABLE_ENTITY) {
//                    Logger::debug("ENT URL %s bbox: %d,%d %d,%d hit %d,%d", part.entity->value.c_str(), part.bbox.min.x,
//                                  part.bbox.max.y, part.bbox.max.x, part.bbox.min.y, ev.workarea_hit_point().x, ev.workarea_hit_point().y);
//                }
                if (part.bbox.contains(ev.workarea_hit_point())) {
                    switch (part.type) {
                        case CLICKABLE_ATTACHMENT:
                            if (ev.click_event().is_select()) {
                                cmd_download_attachment.download(view_item, true);
                            }
                            break;
                        case CLICKABLE_AVATAR:
                            if (ev.click_event().is_select()) {
                                ViewProfileDialog::open(view_item->value->author);
//                                open_avatar(view_item);
                            }
                            break;
                        case CLICKABLE_ENTITY:
                            if (part.entity.type == ENTITY_TEXT_URL || part.entity.type == ENTITY_URL) {
                                open_browser_url(view_item->value->get_entity_value(part.entity));
                            }
                            break;
                    }
                }
            }
        }
    }
} message_click_listener;


class ChatClickListener : public ViewItemClickListener<ChatListViewItem> {
public:
    void item_clicked(ViewItemClickEvent<ChatListViewItem> &ev) override {
//        Logger::debug("ChatClickListener %s btn:%d shift:%d", ev.view_item().value->title.c_str(), ev.click_event().button(), ev.click_event().is_select_double());
        ChatListViewItem *view_item = &ev.view_item();
        if (ev.click_event().button() == 1024) {
            g_app_data_model.open_chat(ev.view_item().value);
        } else if (ev.click_event().button() == 4) {
            if (view_item->value->member != nullptr) {
                ViewProfileDialog::open(view_item->value->member);
            }
        } else if (ev.click_event().is_menu()) {
            ChatMainUI::instance->clicked_chat_view_item = view_item;
            auto mnu = ChatMainUI::instance->chat_menu;
            mnu.title(utf8_to_riscos_local(view_item->value->title));
            if ((view_item->value->type == CHAT_TYPE_PRIVATE || view_item->value->type == CHAT_TYPE_SECRET)) {
                //Logger::debug("ChatClickListener CHAT_TYPE_PRIVATE");
                mnu.item(1).text("Delete chat");
                mnu.item(2).fade(false);
                mnu.item(3).fade(true);
                mnu.item(4).fade(false);
            } else if (view_item->value->type == CHAT_TYPE_GROUP) {
                mnu.item(1).text("Leave Group");
                mnu.item(2).fade(false);
                mnu.item(3).fade(false);
                mnu.item(4).fade(true);
            } else {
                //Logger::debug("ChatClickListener other");
                mnu.item(1).text("Leave Channel");
                mnu.item(2).fade(true);
                mnu.item(3).fade(true);
                mnu.item(4).fade(true);
            }
            mnu.show_at_pointer();
        }
    }
} chat_click_listener;


class ChatUIOpenWindowListener : public tbx::OpenWindowListener {
public:
    tbx::BBox prev_bounds = {0,0,0,0};
    tbx::BBox new_bounds = {0,0,0,0};
    bool doing_layout = false;

    void open_window(tbx::OpenWindowEvent &event) override {
        new_bounds = event.visible_area();
        Logger::debug("ChatUIOpenWindowListener new bounds %d,%d,%d,%d prev %d,%d,%d,%d scroll %d,%d",
                new_bounds.min.x, new_bounds.min.y, new_bounds.max.x, new_bounds.max.y,
                      prev_bounds.min.x, prev_bounds.min.y, prev_bounds.max.x, prev_bounds.max.y,
                      event.scroll().x, event.scroll().y
                );
        if (prev_bounds != new_bounds && !doing_layout) {
            doing_layout = true;
            auto layout_windows = [this]() {
                doing_layout = false;
//                tbx::Window &messages_win = ChatMainUI::instance->messages_view.win;
//                if (prev_bounds.height() != new_bounds.height()) {
//                    tbx::Window &chatlist_win = ChatMainUI::instance->chatlist_view.win;
//                    tbx::WindowState chatlist_win_state;

//                    tbx::ShowSubWindowSpec chatlist_subwindow;
//                    chatlist_subwindow.wimp_parent = messages_win.window_handle();
//                    chatlist_subwindow.wimp_window = -1;
//                    chatlist_subwindow.flags = 0
//                                               | tbx::ALIGN_LEFT_VISIBLE_LEFT
//                                               | tbx::ALIGN_TOP_VISIBLE_TOP
//                                               | tbx::ALIGN_RIGHT_VISIBLE_LEFT
//                                               | tbx::ALIGN_BOTTOM_VISIBLE_BOTTOM
//                                               | tbx::ALIGN_Y_SCROLL_VISIBLE_TOP;
//
//                    chatlist_subwindow.visible_area.bounds() = tbx::BBox(
//                            new_bounds.min.x,
//                            new_bounds.min.y,
//                            new_bounds.min.x + chatlist_win.bounds().width(),
//                            new_bounds.max.y - 76);
//                    chatlist_win.show_as_subwindow(chatlist_subwindow);
//                    Logger::debug("ChatUIOpenWindowListener setup new chatlist_win");
//                }
                if (prev_bounds.width() != new_bounds.width() || prev_bounds.height() != new_bounds.height()) {
                    Logger::debug("ChatUIOpenWindowListener setup_messages_toolbar_subwindow");
                    ChatMainUI::instance->setup_messages_toolbar_subwindow();
                }
                char buf[100];
                snprintf(buf, sizeof(buf), "%d %d %d %d", new_bounds.min.x, new_bounds.min.y, new_bounds.max.x, new_bounds.max.y);
                IKConfig::set_value("general", "start_pos", buf);
                prev_bounds = new_bounds;
            };
            g_idle_task.run_at_next_idle(layout_windows);
        }

        ChatMainUI::instance->handle_scroll_y(event.scroll().y, event.visible_area().height());
    }
} chat_ui_open_win_listener;


class ChatUIScrollRequestListener : public tbx::ScrollRequestListener {
   void scroll_request(const tbx::ScrollRequestEvent &event) {
        tbx::WindowOpenInfo open_info = event.open_info();
        switch (event.y_scroll()) {
            case tbx::ScrollRequestEvent::ScrollYDirection::PAGE_DOWN:
                open_info.visible_area().scroll().y -= (open_info.visible_area().bounds().height() - 200);
                break;
            case tbx::ScrollRequestEvent::ScrollYDirection::PAGE_UP:
                open_info.visible_area().scroll().y += (open_info.visible_area().bounds().height() - 200);
                break;
            case tbx::ScrollRequestEvent::ScrollYDirection::DOWN:
                open_info.visible_area().scroll().y -= 40;
                break;
            case tbx::ScrollRequestEvent::ScrollYDirection::UP:
                open_info.visible_area().scroll().y += 40;
                break;
        }
        open_info.visible_area().scroll().x = 0;
        ChatMainUI::instance->handle_scroll_y(open_info.visible_area().scroll().y, open_info.visible_area().bounds().height());
        ChatMainUI::instance->messages_view.win.open_window(open_info);
    }
} chat_ui_scroll_request_listener;

class ChatsOrderingChangedHandler : public tbx::TextChangedListener {
public:
    void text_changed(tbx::TextChangedEvent &event) override {
        if (event.text() == "Online") {
            Logger::debug("CHATS_LIST_ORDERING_ONLINE");
            g_app_data_model.set_chats_list_ordering(CHATS_LIST_ORDERING_ONLINE);
        } else if (event.text() == "Alphabetic"){
            g_app_data_model.set_chats_list_ordering(CHATS_LIST_ORDERING_MEMBER_NAME);
            Logger::debug("CHATS_LIST_ORDERING_LAST_MESSAGE");
        } else {
            g_app_data_model.set_chats_list_ordering(CHATS_LIST_ORDERING_LAST_MESSAGE);
            Logger::debug("CHATS_LIST_ORDERING_LAST_MESSAGE");
        }
    }
} chats_ordering_changed_listener;


class MessagesDownClickListener : public tbx::MouseClickListener {
public:
    void mouse_click(tbx::MouseClickEvent &event) override {
//        Logger::debug("SearchPanelStartListener clicked %d", event.id_block().self_component().id());
        if (g_app_data_model.get_currently_opened_chat()->has_newer_messages()) {
            g_app_data_model.load_messages_in_chat(g_app_data_model.get_currently_opened_chat(), 0, 0);
        } else {
            ChatMainUI::instance->messages_view.maintain_scroll_position(ScrollPosition::GO_TO_BOTTOM);
            ChatMainUI::instance->messages_view.do_maintain_scroll_position();
        }
        ChatMainUI::instance->setup_messages_down_arrow(0);
    }
} messages_down_click_listener;


/* disabled at 06.12.2020
class InputMessageTextArea : public tbx::PostEventListener {
private:
    tbx::Window textarea_subwindow;
    tbx::Window toolbar_win;
    int prev_textarea_height = 40;
    int initial_height = 40;
public:
    tbx::TextArea input_gadget;
    InputMessageTextArea(const tbx::Window &toolsWin) :
            toolbar_win(toolsWin) {
        input_gadget = toolbar_win.gadget(1);
        tbx::TextArea(input_toolbar_win.gadget(1)).font("System.Fixed", 12*16, 12*16);
    }

    void text(const std::string& txt) {
        input_gadget.text(txt);
        autoresize();
    }

    std::string text() {
        return input_gadget.text();
    }

    void focus() {
        input_gadget.focus();
        input_gadget.set_cursor_position(0, 0);
    }

    void autoresize() {
        if (textarea_subwindow.null()) {
            return;
        }

        tbx::BBox textarea_subwindow_extent = textarea_subwindow.extent();
        if (input_gadget.text_length() == 0 && textarea_subwindow_extent.height() != initial_height) {
            textarea_subwindow_extent.min.y = textarea_subwindow_extent.max.y - initial_height;
            textarea_subwindow.extent(textarea_subwindow_extent);
        }
        int ta_height = textarea_subwindow_extent.size().height;
//        Logger::debug("ta_height = %d", ta_height);
        if (ta_height != prev_textarea_height) {
//            Logger::debug("prev_ta_height = %d", prev_textarea_height);
            int height_diff = ta_height - prev_textarea_height;
            tbx::Size toolbar_win_size = toolbar_win.size();
            toolbar_win_size.height += height_diff;
            toolbar_win.size(toolbar_win_size);

            tbx::BBox input_gadget_bbox = input_gadget.bounds();
//            Logger::debug("input_gadget_size %dx%d", input_gadget_bbox.width(), input_gadget_bbox.height());
            input_gadget_bbox.min.y -= height_diff;
            if (input_gadget_bbox.height() < 44) {
                input_gadget_bbox.min.y = input_gadget_bbox.max.y - 44;
            }
            input_gadget.bounds(input_gadget_bbox);

//            Logger::debug("resize gadget to %dx%d", input_gadget_bbox.width(), input_gadget_bbox.height());
            prev_textarea_height = ta_height;
            ChatMainUI::instance->setup_messages_toolbar_subwindow();
        }
    }

    void key(tbx::KeyEvent &event) {
//        Logger::debug("ChatInputMessageKeyListener key %d", event.key());
        switch (event.key()) {
            case wimp_KEY_RETURN: {
                // read control key state
                _kernel_swi_regs regs;
                regs.r[0] = 121;
                regs.r[1] = 0x1 ^ 0x80;
                tbx::swix_check(_kernel_swi(0x6, &regs, &regs)); // 0x6 is OS_Byte
                bool ctrl_pressed = regs.r[1] == 0xff;

                // read shift key state
                regs.r[0] = 121;
                regs.r[1] = 0x80;
                tbx::swix_check(_kernel_swi(0x6, &regs, &regs)); // 0x6 is OS_Byte
                bool shift_pressed = regs.r[1] == 0xff;

                if (!ctrl_pressed && !shift_pressed) {
                    if (g_app_data_model.get_currently_opened_chat() != nullptr) {
                        if (ChatMainUI::instance->is_editing()) {
                            ChatMainUI::instance->submit_edited_message();
                        } else if (ChatMainUI::instance->is_replying()) {
                            ChatMainUI::instance->submit_replyed_message();
                        } else {
                            ChatMainUI::instance->submit_message();
                        }
                    } else {
                        show_alert_error("Chat not opened");
                    }
                }
            }
                break;
// STUPID riscos (Toolbox?) eat ESC key
//            case wimp_KEY_ESCAPE:
//                Logger::debug("wimp_KEY_ESCAPE %d", ChatMainUI::instance->is_editing());
//                if (ChatMainUI::instance->is_editing()) {
//                    ChatMainUI::instance->leave_editing();
//                } else if (ChatMainUI::instance->is_replying()) {
//                    ChatMainUI::instance->leave_replying();
//                }
//                break;
            default:
                ChatMainUI::instance->typing_notify(true);
                break;
        }
    }

    void post_event(int reason_code, const tbx::PollBlock &poll_block, const tbx::IdBlock &id_block, int reply_to) override {
        if (reason_code != 8 && reason_code != 12) {
            return;
        }
        if (id_block.parent_object().handle() == toolbar_win.handle()
            && id_block.self_object().toolbox_class() == tbx::Window::TOOLBOX_CLASS
            && id_block.self_component().null()) {
            if (textarea_subwindow.null()) {
//                Logger::debug("Found textarea_subwindow=%p", id_block.self_object().handle());
                textarea_subwindow = tbx::Window(id_block.self_object());
//                prev_textarea_height = textarea_subwindow.extent().size().height;
//                initial_height = prev_textarea_height;
                if (input_gadget.text_length() > 0) {
                    autoresize();
                }
            }
            if (reason_code == 8) {
                tbx::KeyEvent ev((tbx::IdBlock&)id_block, (tbx::PollBlock &)poll_block);
//                Logger::debug("Key=%d", ev.key());
                key(ev);
                autoresize();
            }
        }
    }
};
static InputMessageTextArea* input_message_textarea_handler = nullptr;
*/

class InputMessageWritable : public tbx::KeyListener {
public:
    tbx::WritableField input_gadget;
// InputMessageWritable
    InputMessageWritable(tbx::WritableField fld)  {
        input_gadget = fld;
        input_gadget.add_key_listener(this);
    }

    void text(const std::string &txt) { input_gadget.text(txt); }
    std::string text() { return input_gadget.text(); }

    void focus() {
        try {
            input_gadget.focus();
        } catch (std::exception &ex) {
            Logger::error("InputMessageWritable set focus error: %s", ex.what());
        }
    }

    void key(tbx::KeyEvent &event)  override {
//        Logger::debug("ChatInputMessageKeyListener key %d", event.key());
        if (event.key() == wimp_KEY_RETURN) {
            if (ChatMainUI::instance->is_editing()) {
                ChatMainUI::instance->submit_edited_message();
            } else {
                ChatMainUI::instance->submit_message();
            }
        } else if (event.key() == wimp_KEY_ESCAPE) {
            Logger::debug("wimp_KEY_ESCAPE %d", ChatMainUI::instance->is_editing());
            if (ChatMainUI::instance->is_editing()) {
                ChatMainUI::instance->leave_editing();
            }
        } else {
            ChatMainUI::instance->typing_notify(true);
        }
    }
} *input_message_control;


class ReplyMessageWritable : public tbx::KeyListener {
public:
    tbx::WritableField input_gadget;
    ReplyMessageWritable(tbx::WritableField fld)  {
        input_gadget = fld;
        input_gadget.add_key_listener(this);
    }

    void text(const std::string &txt) { input_gadget.text(txt); }
    std::string text() { return input_gadget.text(); }

    void focus() {
        try {
            input_gadget.focus();
        } catch (std::exception &ex) {
            Logger::error("ReplyMessageWritable set focus error: %s", ex.what());
        }
    }

    void key(tbx::KeyEvent &event)  override {
//        Logger::debug("ChatInputMessageKeyListener key %d", event.key());
        if (event.key() == wimp_KEY_RETURN) {
            ChatMainUI::instance->submit_replyed_message();
        } else if (event.key() == wimp_KEY_ESCAPE) {
            ChatMainUI::instance->leave_replying();
        } else {
            ChatMainUI::instance->typing_notify(true);
        }
    }
} *reply_message_control;


class SearchPanelWritable : public tbx::KeyListener {
public:
    tbx::WritableField input_gadget;
    SearchPanelWritable(tbx::WritableField fld)  {
        input_gadget = fld;
        input_gadget.add_key_listener(this);
    }

    void text(const std::string &txt) { input_gadget.text(txt); }
    std::string text() { return input_gadget.text(); }

    void focus() {
        try {
            input_gadget.focus();
        } catch (std::exception &ex) {
            Logger::error("ReplyMessageWritable set focus error: %s", ex.what());
        }
    }

    void key(tbx::KeyEvent &event)  override {
//        Logger::debug("ChatInputMessageKeyListener key %d", event.key());
        if (event.key() == wimp_KEY_RETURN) {
            ChatMainUI::instance->search_begin();
        } else if (event.key() == wimp_KEY_ESCAPE) {
            ChatMainUI::instance->leave_searching();
        } else if (event.key() == wimp_KEY_UP) {
            ChatMainUI::instance->search_next();
        } else if (event.key() == wimp_KEY_DOWN) {
            ChatMainUI::instance->search_prev();
        } else {
            ChatMainUI::instance->search_reset_found();
        }
    }
} *search_message_control;

class SearchPanelUpDownListener : public tbx::AdjusterClickedListener {
public:
    void adjuster_clicked(const tbx::AdjusterClickedEvent &clicked_event) override {
        if (clicked_event.id_block().self_component().id() == 4) {
            ChatMainUI::instance->search_next();
        } else {
            ChatMainUI::instance->search_prev();
        }
    }
} search_panel_up_down_lister;

class SearchPanelStartListener : public tbx::MouseClickListener {
public:
    void mouse_click(tbx::MouseClickEvent &event) override {
//        Logger::debug("SearchPanelStartListener clicked %d", event.id_block().self_component().id());
        ChatMainUI::instance->enter_searching();
    }
} search_panel_start_listener;

class SearchPanelFilterChangedListener : public tbx::TextChangedListener {
public:
    void text_changed(tbx::TextChangedEvent &event) override {
        ChatMainUI::instance->search_begin();
    }
} search_panel_filter_changed_listener;


class SendAttachmentLoader : public tbx::Loader {
public:
    bool load_file(tbx::LoadEvent &event) override {
        if (event.file_name().length() > 11 && event.file_name().substr(0, 12) == std::string("<Wimp$Scrap>")) {
            int ft = event.file_type();
            tbx::Path f = event.file_name();
            if (ft == 0xfff) {
                int size;
                char *buf = f.load_file(&size);
                Logger::debug("SendAttachmentLoader::load_file set text file:%s, buf:%s", event.file_name().c_str(),
                              buf);
                if (size > 0) {
                    switch (ChatMainUI::instance->toolbar_mode) {
                        case TOOLBAR_MODE_INPUT:
                            input_message_control->text(buf);
                            break;
                        case TOOLBAR_MODE_REPLY:
                            reply_message_control->text(buf);
                            break;
                        case TOOLBAR_MODE_SEARCH:
                            search_message_control->text(buf);
                            break;
                    }
                }
                delete buf;
            } else {
                char filenamebuf[255];
                sprintf(filenamebuf, "<ChatCube$ChoicesDir>.temp.upload%d%ld", os_read_monotonic_time(), random());
                copy_file(event.file_name().c_str(), filenamebuf);
                tbx::Path uplpath = filenamebuf;
                uplpath.file_type(event.file_type());
                if (ft == 0xC85 || ft == 0xB60) {
                    g_app_data_model.send_message_photo(filenamebuf, "");
                } else {
                    g_app_data_model.send_message_file(filenamebuf, "", ft);
                }
            }
            return true;
        }
        if (ChatMainUI::instance->toolbar_mode != TOOLBAR_MODE_REPLY && event.from_filer() && event.file_type() < 0x1000) {
            tbx::Path path(event.file_name());
            int file_type = event.file_type();
            const std::string &file_name = event.file_name();
            Logger::debug("Uploading file %s", file_name.c_str());
            if (file_type ==  0xC85 || file_type == 0xB60) {
                // send photos
                g_app_data_model.send_message_photo(file_name, "");
            } else {
                g_app_data_model.send_message_file(file_name, "", file_type);
            }
            return true;
        } else {
            show_alert_error("Only files can be sent. Send directories or objects from other applications is not supported.");
            return false;
        }
    }
} send_attachment_loader;

class SearchChatButtonClickListener : public tbx::MouseClickListener {
public:
    bool state_search = false;
    tbx::Window win;
    void mouse_click(tbx::MouseClickEvent &event) override {
        tbx::Button srch_btn = win.gadget(4);
        tbx::Point label_pos, control_pos;
        if (state_search) {
            state_search = false;
            label_pos = win.gadget(5).bottom_left();
            control_pos = win.gadget(6).bottom_left();
            tbx::WritableField fld = win.gadget(5);
            fld.move_to(9999,-9999);
            fld.text("");
            win.gadget(6).move_to(9999,-9999);
            win.gadget(2).move_to(control_pos);
            win.gadget(3).move_to(label_pos);
            g_app_data_model.filter_chats("");
            srch_btn.validation("R1;Ssearch");
            input_message_control->focus();
        } else {
            state_search = true;
            label_pos = win.gadget(2).bottom_left();
            control_pos = win.gadget(3).bottom_left();
            win.gadget(3).move_to(9999,-9999);
            win.gadget(2).move_to(9999,-9999);

            tbx::WritableField fld = win.gadget(5);
            fld.move_to(control_pos);
//            if (ChatMainUI::instance->has_input_focus()) {
            try {
                fld.focus();
            } catch (std::exception &ex) {
                Logger::error("Search set focus error: %s", ex.what());
            }
//            }
            win.gadget(6).move_to(label_pos);
            srch_btn.validation("R2;Ssearch");
        }
    }
} search_chat_button_listner;

class SearchChatKeyListener : public tbx::KeyListener {
public:
    void key(tbx::KeyEvent &event) override {
        tbx::WritableField fld = tbx::WritableField(event.id_block().self_component());
        std::string search_for = fld.text();
        g_app_data_model.filter_chats(search_for);
    }
} search_chat_key_listner;


// ChatMainUI
ChatMainUI* ChatMainUI::instance;

ChatMainUI::ChatMainUI() :
        last_type_time(0),
        toolbar_mode(TOOLBAR_MODE_INPUT),
        is_messages_needs_reload(false),
//        is_window_shown(false),
        is_windows_set_up(false),
        chatlist_view(),
        messages_view(),
        msg_menu(tbx::Menu("MsgMenu")),
        chat_menu(tbx::Menu("ChatMenu")),
        messages_down_win(tbx::Window("MsgsDown")),
        chats_toolbar_win(tbx::Window("ChListTbar")),
        reply_toolbar_win(tbx::Window("ReplyTbar")),
        search_toolbar_win(tbx::Window("SearchTbar")),
        input_toolbar_win(tbx::Window("ChatToolbar"))
{
    g_app_events.listen<AppEvents::LoggedIn>(std::bind(&ChatMainUI::on_login, this, std::placeholders::_1));
    g_app_events.listen<AppEvents::ChatListLoaded>(std::bind(&ChatMainUI::on_chat_list_loaded, this, std::placeholders::_1));
    g_app_events.listen<AppEvents::ChatAdded>(std::bind(&ChatMainUI::on_chat_added, this, std::placeholders::_1));
    g_app_events.listen<AppEvents::ChatChanged>(std::bind(&ChatMainUI::on_chat_changed, this, std::placeholders::_1));
    g_app_events.listen<AppEvents::ChatDeleted>(std::bind(&ChatMainUI:: on_chat_deleted, this, std::placeholders::_1));
    g_app_events.listen<AppEvents::ChatCleared>(std::bind(&ChatMainUI:: on_chat_cleared, this, std::placeholders::_1));
    g_app_events.listen<AppEvents::OpenedChatChanged>(std::bind(&ChatMainUI::on_opened_chat_changed, this, std::placeholders::_1));
    g_app_events.listen<AppEvents::MessagesLoaded>(std::bind(&ChatMainUI::on_messages_loaded, this, std::placeholders::_1));
    g_app_events.listen<AppEvents::MessageAdded>(std::bind(&ChatMainUI::on_message_added, this, std::placeholders::_1));
    g_app_events.listen<AppEvents::MessageChanged>(std::bind(&ChatMainUI::on_message_changed, this, std::placeholders::_1));
    g_app_events.listen<AppEvents::MessageDeleted>(std::bind(&ChatMainUI::on_message_deleted, this, std::placeholders::_1));

    chatlist_view.add_click_listener(&chat_click_listener);
    messages_view.add_click_listener(&message_click_listener);
    messages_view.win.add_loader(&send_attachment_loader);
    messages_view.win.add_has_been_hidden_listener(&messages_window_hidden_listener);
    messages_view.win.add_about_to_be_shown_listener(&messages_window_shown_listener);
    messages_view.win.add_open_window_listener(&chat_ui_open_win_listener);
    messages_view.win.add_scroll_request_listener(&chat_ui_scroll_request_listener);

    messages_down_win.add_all_mouse_click_listener(&messages_down_click_listener);

    input_toolbar_win.add_loader(&send_attachment_loader);
    input_toolbar_win.add_command(0x1010, make_command_method(&ChatMainUI::start_send_sticker));
    input_toolbar_win.add_command(0x1001, make_command_method(&ChatMainUI::screenshoot_full));
    input_toolbar_win.add_command(0x1002, make_command_method(&ChatMainUI::screenshoot_area));
    input_toolbar_win.add_command(0x1003, make_command_method(&ChatMainUI::screenshoot_hide_full));
    input_toolbar_win.add_command(0x1004, make_command_method(&ChatMainUI::screenshoot_hide_area));
    input_toolbar_win.add_command(0x1005, make_command_method(&ChatMainUI::screenshoot_window));
    input_toolbar_win.add_command(0x1011, make_command_method(&ChatMainUI::leave_editing));

    tbx::Button(input_toolbar_win.gadget(0xa)).add_mouse_click_listener(&search_panel_start_listener);
    input_message_control = new InputMessageWritable(tbx::WritableField(input_toolbar_win.gadget(1)));
    input_textarea_pos = input_message_control->input_gadget.bottom_left();
    input_toolbar_win.gadget(0xb).move_by(0, -9999);
    input_toolbar_win.gadget(5).fade(true);
    input_toolbar_win.gadget(4).fade(true);
    input_toolbar_win.gadget(9).fade(true);
    input_toolbar_win.gadget(0xa).fade(true);

    reply_message_control = new ReplyMessageWritable(tbx::WritableField(reply_toolbar_win.gadget(1)));
    reply_toolbar_win.add_command(0x1012, make_command_method(&ChatMainUI::leave_replying));

    //    tbx::StringSet filter_messages =  input_toolbar_win.gadget(0xa);
//    filter_messages.fade(true);
//    filter_messages.add_text_changed_listener(&messages_filter_lister);
    search_message_control = new SearchPanelWritable(tbx::WritableField(search_toolbar_win.gadget(1)));
    search_toolbar_win.add_command(0x1014, make_command_method(&ChatMainUI::leave_searching));
    search_toolbar_win.add_loader(&send_attachment_loader);
    tbx::StringSet(search_toolbar_win.gadget(0xa)).add_text_changed_listener(&search_panel_filter_changed_listener);
    tbx::Adjuster(search_toolbar_win.gadget(4)).add_clicked_listener(&search_panel_up_down_lister);
    tbx::Adjuster(search_toolbar_win.gadget(5)).add_clicked_listener(&search_panel_up_down_lister);

    tbx::Button(chats_toolbar_win.gadget(4)).add_mouse_click_listener(&search_chat_button_listner);
    tbx::StringSet(chats_toolbar_win.gadget(3)).add_text_changed_listener(&chats_ordering_changed_listener);
    tbx::WritableField(chats_toolbar_win.gadget(5)).add_key_listener(&search_chat_key_listner);
    search_chat_button_listner.win = chats_toolbar_win;
    chats_toolbar_win.gadget(5).move_to(9999,-9999);
    chats_toolbar_win.gadget(6).move_to(9999,-9999);

    msg_menu.add_command(0x1001, make_command_method(&ChatMainUI::delete_msg));
    msg_menu.add_command(0x1002, make_command_method(&ChatMainUI::unsend_msg));
    msg_menu.add_command(0x1003, &cmd_download_attachment);
    msg_menu.add_command(0x1004, make_command_method(&ChatMainUI::forward_msg));
    msg_menu.add_command(0x1005, make_command_method(&ChatMainUI::enter_editing));
    msg_menu.add_command(0x1006, make_command_method(&ChatMainUI::enter_replying));
    msg_menu.add_command(0x1007, make_command_method(&ChatMainUI::create_private_chat));
    msg_menu.add_command(0x1008, make_command_method(&ChatMainUI::copy_message_to_clipboard));
//    msg_menu.add_command(0x1009, &show_filter_messages_menu);

    chat_menu.add_command(0x1001, &cmd_delete_chat_question);
    chat_menu.add_command(0x1002, &cmd_clear_chat_history_question);
    chat_menu.add_command(0x1003, make_command_method(&ChatMainUI::manage_group));
    chat_menu.add_command(0x1004,make_command_method(&ChatMainUI::view_profile));
    chat_menu.add_command(0x1005,make_command_method(&ChatMainUI::download_chat_history));

// disabled at 05.12.2020
//    input_message_handler = new InputMessageTextArea(input_toolbar_win);
//    tbx::app()->set_post_event_listener(input_message_handler);
//    tbx::TextArea(input_toolbar_win.gadget(1)).set_cursor_position(0,1);
    messages_view.margin_left = chatlist_view.win.outline().width();
    Logger::debug("messages_view.margin_left=%d", messages_view.margin_left);
}

ChatMainUI::~ChatMainUI() {
    tbx::Menu("scrShotMenu").delete_object();
    input_toolbar_win.delete_object();
    search_toolbar_win.delete_object();
    reply_toolbar_win.delete_object();
    chats_toolbar_win.delete_object();
    //tbx::app()->set_post_event_listener(nullptr);
    for(auto &cm : command_methods) {
        delete[] cm;
    }
    command_methods.clear();
    delete input_message_control;
    delete reply_message_control;
    delete search_message_control;
    instance = nullptr;
}

tbx::CommandMethod<ChatMainUI>* ChatMainUI::make_command_method(void (ChatMainUI::*method)()) {
    auto m = new tbx::CommandMethod<ChatMainUI>(this, method);
    command_methods.emplace_back(m);
    return m;
}

void ChatMainUI::open() {
    if (!instance) {
        instance = new ChatMainUI();
        if (g_app_state.start_hidden) {
            instance->messages_view.win.show_centered();
            auto after_show = []() {
                instance->setup_subwindows();
                auto after_setup = []() {
                    instance->hide();
                };
                g_idle_task.run_at_next_idle(after_setup);
            };
            g_idle_task.run_at_next_idle(after_show);
            return;
        }
    }

    set_app_poll_period(3);
    if (!instance->is_windows_set_up) {
        instance->messages_view.win.show_centered();

        auto after_show = []() {
            instance->setup_subwindows();
            std::string pos_str = IKConfig::get_value("general", "start_pos");
            if (!pos_str.empty()) {
                tbx::BBox box;
                if (sscanf(pos_str.c_str(), "%d %d %d %d", &box.min.x, &box.min.y, &box.max.x, &box.max.y) == 4) {
                    instance->messages_view.win.bounds(box);
                    auto after_change_bounds = []() {
                        instance->setup_messages_toolbar_subwindow();
                    };
                    g_idle_task.run_at_next_idle(after_change_bounds);
                }
            }
        };
        g_idle_task.run_at_next_idle(after_show);
        instance->is_windows_set_up = true;
    } else {
        instance->messages_view.win.show();
    }
    //Logger::debug("ChatMainUI::open instance->input_toolbar_win %x", instance->input_toolbar_win.handle());
}

void ChatMainUI::hide() {
    if (instance && g_app_state.is_main_window_shown) {
        instance->messages_view.win.hide();
        g_hourglass_off();
        g_app_state.is_main_window_shown = false;
    }
}

void ChatMainUI::setup_messages_toolbar_subwindow() {
    tbx::Window &messages_win = messages_view.win;
    tbx::BBox messages_win_bounds = messages_win.bounds();
    tbx::Window toolbar_win;
    Logger::debug("setup_messages_toolbar_subwindow toolbar_mode=%d", toolbar_mode);
    switch(toolbar_mode) {
        case TOOLBAR_MODE_INPUT:
            toolbar_win = input_toolbar_win;
            reply_toolbar_win.hide();
            search_toolbar_win.hide();
            break;
        case TOOLBAR_MODE_REPLY:
            toolbar_win = reply_toolbar_win;
            search_toolbar_win.hide();
            input_toolbar_win.hide();
            break;
        case TOOLBAR_MODE_SEARCH:
            toolbar_win = search_toolbar_win;
            reply_toolbar_win.hide();
            input_toolbar_win.hide();
            break;
    }
    tbx::BBox toolbar_bounds = toolbar_win.bounds();
    int prev_toolbar_width = toolbar_bounds.width();
    messages_toolbar_height = toolbar_bounds.height();
    tbx::ShowSubWindowSpec toolbar_subwindow;

    toolbar_subwindow.wimp_parent = messages_win.window_handle();
    toolbar_subwindow.wimp_window = -1;
    toolbar_subwindow.visible_area.bounds() = tbx::BBox(
            messages_win_bounds.min.x + chatlist_view.win.outline().width(),
            messages_win_bounds.min.y,
            messages_win_bounds.max.x,
            messages_win_bounds.min.y + messages_toolbar_height
    );
    toolbar_subwindow.visible_area.scroll().x = 0;
    toolbar_subwindow.visible_area.scroll().y = 0;
    toolbar_subwindow.flags = 0
                              | tbx::ALIGN_LEFT_VISIBLE_RIGHT
                              | tbx::ALIGN_BOTTOM_VISIBLE_BOTTOM
                              | tbx::ALIGN_RIGHT_VISIBLE_RIGHT
                              | tbx::ALIGN_TOP_VISIBLE_BOTTOM
                              | tbx::ALIGN_Y_SCROLL_VISIBLE_TOP
            ;
    toolbar_win.show_as_subwindow(toolbar_subwindow);

    int new_toolbar_width = toolbar_subwindow.visible_area.bounds().width();
    if (prev_toolbar_width != new_toolbar_width) {
        int move_by_x = new_toolbar_width - prev_toolbar_width;
        tbx::BBox box;

        switch(toolbar_mode) {
            case TOOLBAR_MODE_INPUT:
                box = toolbar_win.gadget(1).bounds();
                box.max.x = new_toolbar_width - (prev_toolbar_width - box.max.x);
                toolbar_win.gadget(1).bounds(box);  // input text
                toolbar_win.gadget(0xa).move_by(move_by_x, 0); // search btn
                toolbar_win.gadget(0xb).move_by(move_by_x, 0); // cancel btn
                break;
            case TOOLBAR_MODE_REPLY:
                box = toolbar_win.gadget(1).bounds();
                box.max.x = new_toolbar_width - (prev_toolbar_width - box.max.x);
                toolbar_win.gadget(1).bounds(box);  // input text

                box = toolbar_win.gadget(0xc).bounds();
                box.max.x = new_toolbar_width - (prev_toolbar_width - box.max.x);
                toolbar_win.gadget(0xc).bounds(box); // reply to Display

                box = toolbar_win.gadget(0xd).bounds();
                box.max.x = new_toolbar_width - (prev_toolbar_width - box.max.x);
                toolbar_win.gadget(0xd).bounds(box); // reply text Display

                toolbar_win.gadget(0xe).move_by(move_by_x, 0); // cancel reply btn
                break;
            case TOOLBAR_MODE_SEARCH:
                box = toolbar_win.gadget(1).bounds();
                box.max.x = new_toolbar_width - (prev_toolbar_width - box.max.x);
                toolbar_win.gadget(1).bounds(box);  // input text

                toolbar_win.gadget(0x4).move_by(move_by_x, 0); // up arrow
                toolbar_win.gadget(0x5).move_by(move_by_x, 0); // down arrow
                toolbar_win.gadget(0x6).move_by(move_by_x, 0); // cancel
                break;
        }
        messages_view.margin_bottom = messages_toolbar_height;
        messages_view.repaint_all();
        setup_messages_down_arrow(-1);
    } else if (messages_view.margin_bottom != messages_toolbar_height) {
        messages_view.margin_bottom = messages_toolbar_height;
        messages_view.update_visible_at_next_tick();
        setup_messages_down_arrow(-1);
    }

}

void ChatMainUI::setup_messages_down_arrow(int show) {
    if (messages_view.get_chat()) {
        if (show == -1) {
            show = messages_view.is_scrolled_to_bottom() ? 0 : 1;
        }
        if (show) {
            if (!is_messages_down_arrow_shown) {
                is_messages_down_arrow_shown = true;
                tbx::Window &messages_win = messages_view.win;
                tbx::BBox messages_win_bounds = messages_win.bounds();
                tbx::ShowSubWindowSpec messages_down_subwindow;
                messages_down_subwindow.wimp_parent = messages_win.window_handle();
                messages_down_subwindow.wimp_window = -1;
                messages_down_subwindow.flags = 0
                                                | tbx::ALIGN_RIGHT_VISIBLE_RIGHT
                                                | tbx::ALIGN_BOTTOM_VISIBLE_BOTTOM
                                                | tbx::ALIGN_LEFT_VISIBLE_RIGHT
                                                | tbx::ALIGN_TOP_VISIBLE_BOTTOM
                                                | tbx::ALIGN_Y_SCROLL_VISIBLE_TOP;

                messages_down_subwindow.visible_area.bounds() = tbx::BBox(
                        messages_win_bounds.max.x - messages_down_win.bounds().width(),
                        messages_win_bounds.min.y + messages_toolbar_height + 4,
                        messages_win_bounds.max.x,
                        messages_win_bounds.min.y + messages_toolbar_height + messages_down_win.bounds().height() + 4);
                messages_down_win.show_as_subwindow(messages_down_subwindow);
            }
        } else {
            if (is_messages_down_arrow_shown) {
                is_messages_down_arrow_shown = false;
                messages_down_win.hide();
            }
        }
    }
}

void ChatMainUI::setup_subwindows() {
//    Logger::debug("ChatMainUI::open after_show %x", input_toolbar_win.handle());
    tbx::Window &messages_win = messages_view.win;
    tbx::Window &chatlist_win = chatlist_view.win;

    messages_view.update_window_extent();
    tbx::BBox messages_win_bounds = messages_win.bounds();
//    messages_win.force_redraw(messages_win.extent());

    tbx::ShowSubWindowSpec chatlist_subwindow;
    chatlist_subwindow.wimp_parent = messages_win.window_handle();
    chatlist_subwindow.wimp_window = -1;
    chatlist_subwindow.flags = 0
                               | tbx::ALIGN_LEFT_VISIBLE_LEFT
                               | tbx::ALIGN_TOP_VISIBLE_TOP
                               | tbx::ALIGN_RIGHT_VISIBLE_LEFT
                               | tbx::ALIGN_BOTTOM_VISIBLE_BOTTOM
                               | tbx::ALIGN_Y_SCROLL_VISIBLE_TOP;

    chatlist_subwindow.visible_area.bounds() = tbx::BBox(
            messages_win_bounds.min.x,
            messages_win_bounds.min.y,
            messages_win_bounds.min.x + chatlist_win.bounds().width(),
            messages_win_bounds.max.y - 76);
    chatlist_win.show_as_subwindow(chatlist_subwindow);

    tbx::ShowSubWindowSpec chats_toolbar_subwindow;
    chats_toolbar_subwindow.wimp_parent = messages_win.window_handle();
    chats_toolbar_subwindow.wimp_window = -1;
    chats_toolbar_subwindow.flags = 0
                               | tbx::ALIGN_LEFT_VISIBLE_LEFT
                               | tbx::ALIGN_TOP_VISIBLE_TOP
                               | tbx::ALIGN_RIGHT_VISIBLE_LEFT
                               | tbx::ALIGN_BOTTOM_VISIBLE_TOP;

    chats_toolbar_subwindow.visible_area.bounds() = tbx::BBox(
            messages_win_bounds.min.x,
            messages_win_bounds.max.y - 76,
            messages_win_bounds.min.x + chatlist_win.bounds().width(),
            messages_win_bounds.max.y);
    chats_toolbar_win.show_as_subwindow(chats_toolbar_subwindow);

    setup_messages_toolbar_subwindow();
}

void ChatMainUI::on_chat_added(const AppEvents::ChatAdded& ev) {
    Logger::debug("ChatsListView::on_chat_added");
    std::vector<ChatDataPtr> &chatslist = g_app_data_model.get_chats_list();
    chatlist_view.reload_items(chatslist.begin(), chatslist.end(), false);
}

void ChatMainUI::on_chat_changed(const AppEvents::ChatChanged& ev) {
    if (ev.ordering_changed) {
        Logger::debug("ChatMainUI::on_chat_changed ordering_changed");
        std::vector<ChatDataPtr> &chatslist = g_app_data_model.get_chats_list();
        chatlist_view.reload_items(chatslist.begin(), chatslist.end(), false);
        if (g_app_data_model.get_chats_list_ordering() == CHATS_LIST_ORDERING_LAST_MESSAGE){
            chatlist_view.win.scroll(0, 0);
        }
    } else {
        Logger::debug("ChatMainUI::on_chat_changed");
        chatlist_view.change_item(ev.chat);
    }
//    Logger::debug("ChatMainUI::on_chat_changed end");
}

void ChatMainUI::on_chat_list_loaded(const AppEvents::ChatListLoaded& ev) {
    Logger::debug("ChatsListView::on_chatlist_loaded");
    std::vector<ChatDataPtr> &chatslist = g_app_data_model.get_chats_list();
    chatlist_view.reload_items(chatslist.begin(), chatslist.end(), true);
}

void ChatMainUI::on_chat_deleted(const AppEvents::ChatDeleted& ev) {
    Logger::debug("ChatsListView::on_chat_deleted");
    std::vector<ChatDataPtr> &chatslist = g_app_data_model.get_chats_list();
    chatlist_view.delete_item(ev.chat);

    if (g_app_data_model.get_currently_opened_chat() == nullptr) {
        messages_view.delete_all_view_items();
        messages_view.update_visible_at_next_tick();
    }
}

void ChatMainUI::on_chat_cleared(const AppEvents::ChatCleared& ev) {
    Logger::debug("ChatMainUI::on_chat_cleared %s", ev.chat->id.c_str());
    if (ev.chat == g_app_data_model.get_currently_opened_chat()) {
        Logger::debug("ChatMainUI::on_chat_cleared clear messages %s", ev.chat->id.c_str());
        messages_view.delete_all_view_items();
        messages_view.update_visible_at_next_tick();
//        messages_view.reload_items(ev.chat->messages.begin(), ev.chat->messages.end(), true);
    }
    chatlist_view.change_item(ev.chat);
}

void ChatMainUI::on_opened_chat_changed(const AppEvents::OpenedChatChanged& ev) {
    Logger::debug("ChatsListView::on_opened_chat_changed %s local_title %s", ev.chat->title.c_str(), utf8_to_riscos_local(ev.chat->title).c_str());
    chatlist_view.set_selected_item(ev.chat, true);
    messages_view.win.title("ChatCube: " +utf8_to_riscos_local(ev.chat->title));
    leave_editing();
    leave_replying();
    leave_searching();
    input_toolbar_win.gadget(5).fade(false);
    input_toolbar_win.gadget(4).fade(false);
    input_toolbar_win.gadget(9).fade(false);
    input_toolbar_win.gadget(0xa).fade(false);
}

void ChatMainUI::on_messages_loaded(const AppEvents::MessagesLoaded& ev) {
    auto currently_opened_chat = g_app_data_model.get_currently_opened_chat();
    if (ev.chat == currently_opened_chat) {
        messages_view.reload_messages(currently_opened_chat, ev.is_first_load);
        setup_messages_down_arrow(-1);
//        if (messages_view.is_scrolled_to_top()) {
//            g_app_data_model.load_more_messages_in_chat(currently_opened_chat, true);
//        }
    } else {
        Logger::info("Messages loaded but other chat is currently selected [loaded=%s]. Loading messages in currently opened chat again. ", ev.chat->id.c_str());
        g_app_data_model.open_chat(g_app_data_model.get_currently_opened_chat(), true);
    }
}

void ChatMainUI::on_message_added(const AppEvents::MessageAdded& ev) {
    auto currently_opened_chat = g_app_data_model.get_currently_opened_chat();
//    Logger::debug("on_message_added is_window_shown=%d", is_window_shown);
    if (g_app_state.is_main_window_shown) {
        if (ev.chat == currently_opened_chat) {
            messages_view.append_item(ev.msg);
            if (g_app_data_model.get_my_member_data()->notification_settings.sound) {
                if (ev.msg->is_outgoing()) {
                    CLSound::play_mp3_file("<ChatCube$Dir>.sound.sent/mp3");
                } else {
                    CLSound::play_mp3_file("<ChatCube$Dir>.sound.recv/mp3");
                }
            }
        }
    } else {
        if (ev.chat == g_app_data_model.get_currently_opened_chat()) {
            is_messages_needs_reload = true;
        }
        if (g_app_data_model.get_my_member_data()->notification_settings.popup) {
            open();
            if (ev.chat != g_app_data_model.get_currently_opened_chat()) {
                g_app_data_model.open_chat(ev.chat);
            }
        }
        if (g_app_data_model.get_my_member_data()->notification_settings.sound) {
            CLSound::play_mp3_file("<ChatCube$Dir>.sound.new_message/mp3");
        }
    }
}

void ChatMainUI::on_message_changed(const AppEvents::MessageChanged& ev) {
    if (ev.chat == g_app_data_model.get_currently_opened_chat()) {
        messages_view.change_item(ev.msg);
        //update_messages_window(false);
    }
}

void ChatMainUI::on_message_deleted(const AppEvents::MessageDeleted& ev) {
    if (ev.chat == g_app_data_model.get_currently_opened_chat()) {
        Logger::debug("Message deleted %lld", ev.msg->id);
        messages_view.delete_item(ev.msg);
//        update_messages_window(false);
    }
}

void ChatMainUI::on_login(const AppEvents::LoggedIn& ev) {
    leave_editing();
    leave_replying();
    messages_view.delete_all_view_items();
    chatlist_view.delete_all_view_items();
}

// commands
void ChatMainUI::start_send_sticker() {
    new SendStickerUI();
}

void ChatMainUI::screenshoot_full()  {
    screenshoot(ScreenshootType::FULL);
}

void ChatMainUI::screenshoot_hide_full() {
    screenshoot(ScreenshootType::HIDE_FULL);
}

void ChatMainUI::screenshoot_area() {
    screenshoot(ScreenshootType::AREA);
}

void ChatMainUI::screenshoot_hide_area() {
    screenshoot(ScreenshootType::HIDE_AREA);
}

void ChatMainUI::screenshoot_window() {
    screenshoot(ScreenshootType::WINDOW);
}

void ChatMainUI::delete_msg() {
    MessageDataPtr msg = clicked_msg_view_item->value;
    Logger::debug("DeleteMessageCmd %s", msg->text.c_str());
    g_app_data_model.delete_message(msg, false);
}

void ChatMainUI::unsend_msg() {
    MessageDataPtr msg = clicked_msg_view_item->value;
    Logger::debug("UnsendMessageCmd %s", msg->text.c_str());
    g_app_data_model.delete_message(msg, true);
}


void ChatMainUI::forward_msg() {
    MessageDataPtr msg = clicked_msg_view_item->value;
    Logger::debug("ForwardMessageCmd %s", msg->text.c_str());
    new ForwardMessageDialog(msg);
}

void ChatMainUI::create_private_chat() {
    MessageDataPtr msg = clicked_msg_view_item->value;
    auto on_create = [](const ChatDataPtr chat) {
        g_app_data_model.open_chat(chat);
    };
    Logger::debug("CreatePrivateChatCmd creating chat with: %s", msg->author->displayname.c_str());
    g_app_data_model.create_private_chat(msg->author, on_create);
}

void ChatMainUI::copy_message_to_clipboard() {
    MessageDataPtr msg = clicked_msg_view_item->value;
    CLClipboard::instance()->clipboard_set(msg->text);
}

void ChatMainUI::manage_group()  {
    new ManageGroupDialog(clicked_chat_view_item->value);
}

void ChatMainUI::view_profile()  {
    if (clicked_chat_view_item->value->member) {
        ViewProfileDialog::open(clicked_chat_view_item->value->member);
    }
}

void ChatMainUI::download_chat_history() {
    auto on_success = [](const std::string& saved_file) {
        tbx::Path dst_path(saved_file);
        std::string dst_dir = dst_path.parent();

        AppEvents::ProgressBarControl pbreq;
        pbreq.label = "";
        g_app_events.notify(pbreq);

        show_alert_info(std::string("Chat history saved to: "+saved_file).c_str());
        os_cli(std::string("*Filer_OpenDir " + dst_dir).c_str());
        if (can_run_file_type(dst_path.file_type())) {
            os_cli(std::string("*Filer_Run "+saved_file).c_str());
        }
    };

    AppEvents::ProgressBarControl pbreq;
    pbreq.label = "Downloading chat history";
    g_app_events.notify(pbreq);

    g_app_data_model.download_chat_history(clicked_chat_view_item->value, on_success);
}

void ChatMainUI::typing_notify(bool start) {
    if (start) {
        if (!last_type_time) {
            g_app_data_model.send_chat_action(CHAT_ACTION_TYPING);
        }
        last_type_time = tbx::monotonic_time();
    } else if (last_type_time > 0) {
        last_type_time = 0;
        g_app_data_model.send_chat_action(CHAT_ACTION_CANCEL);
    }
}

void ChatMainUI::check_typing_notify_timeout() {
    if (last_type_time) {
        auto now  = tbx::monotonic_time();
        if ((now - last_type_time) > TYPING_NOTIFY_TIMEOUT) {
            ChatMainUI::instance->typing_notify(false);
        }
    }
}

void ChatMainUI::submit_message() {
    if (g_app_data_model.get_currently_opened_chat() != nullptr) {
        messages_view.maintain_scroll_position(ScrollPosition::GO_TO_BOTTOM);
        g_app_data_model.send_message_text(input_message_control->text());
        input_message_control->text("");
        ChatMainUI::instance->typing_notify(false);
    }
}

void ChatMainUI::submit_replyed_message() {
    if (g_app_data_model.get_currently_opened_chat() != nullptr) {
        messages_view.maintain_scroll_position(ScrollPosition::GO_TO_BOTTOM);
        g_app_data_model.send_message_text(reply_message_control->text(), replying_message_id);
        reply_message_control->text("");
        ChatMainUI::instance->typing_notify(false);
    }
    leave_replying();
}

void ChatMainUI::submit_edited_message() {
    if (g_app_data_model.get_currently_opened_chat() != nullptr) {
        std::string txt = input_message_control->text();
        txt = trim(txt, " ");
        if (!txt.empty()) {
            messages_view.maintain_scroll_position(ScrollPosition::GO_TO_BOTTOM);
            g_app_data_model.edit_message_text(editing_message_id, txt);
        }
        ChatMainUI::instance->typing_notify(false);
    }
    leave_editing();
}

void ChatMainUI::leave_editing() {
    if (toolbar_mode != TOOLBAR_MODE_INPUT) {
        toolbar_mode = TOOLBAR_MODE_INPUT;
        setup_messages_toolbar_subwindow();
    }
    if (editing_message_id) {
        input_toolbar_win.gadget(0xb).move_by(0, -9999);
        editing_message_id = 0;
        input_message_control->text("");
    }
}

void ChatMainUI::enter_editing() {
    if (toolbar_mode != TOOLBAR_MODE_INPUT) {
        toolbar_mode = TOOLBAR_MODE_INPUT;
        setup_messages_toolbar_subwindow();
    }
    MessageDataPtr msg = clicked_msg_view_item->value;
    tbx::BBox box = input_toolbar_win.gadget(0xb).bounds();
    int height = box.height();
    box.min.y = -16 - height;
    box.max.y = -16;
    input_toolbar_win.gadget(0xb).bounds(box);
    editing_message_id = msg->id;
    input_message_control->text(utf8_to_riscos_local(msg->text));
    input_message_control->focus();
}

void ChatMainUI::leave_replying() {
    if (toolbar_mode != TOOLBAR_MODE_INPUT) {
        toolbar_mode = TOOLBAR_MODE_INPUT;
        setup_messages_toolbar_subwindow();

        replying_message_id = 0;
    }
}

void ChatMainUI::enter_replying() {
    if (toolbar_mode != TOOLBAR_MODE_REPLY) {
        toolbar_mode = TOOLBAR_MODE_REPLY;
        setup_messages_toolbar_subwindow();
    }
    MessageDataPtr msg = clicked_msg_view_item->value;
    tbx::DisplayField(reply_toolbar_win.gadget(0xc)).text(std::string("Reply to: ") + utf8_to_riscos_local(msg->author->displayname));
    std::string txt = msg->text;
    if (utf8len(txt.c_str()) > 65) {
        txt = utf8_substr(txt, 0, 65) + "...";
    }
    tbx::Button(reply_toolbar_win.gadget(0xd)).value(utf8_to_riscos_local(txt));
    replying_message_id = msg->id;
    reply_message_control->focus();
}

bool ChatMainUI::has_input_focus() {
    tbx::WindowState state;
    messages_view.win.get_state(state);
    return state.input_focus();
}

void ChatMainUI::leave_searching() {
    if (toolbar_mode != TOOLBAR_MODE_INPUT) {
        toolbar_mode = TOOLBAR_MODE_INPUT;
        setup_messages_toolbar_subwindow();
        if (g_app_data_model.get_currently_opened_chat()->has_newer_messages()) {
            g_app_data_model.load_messages_in_chat(g_app_data_model.get_currently_opened_chat(), 0, 0);
        }
        setup_messages_down_arrow();
        input_message_control->focus();
    }
    found_message_ids.clear();
    messages_view.search_set_query("", 0);
    messages_view.search_show_found(nullptr);
}

void ChatMainUI::enter_searching() {
    if (g_app_data_model.get_currently_opened_chat() != nullptr) {
        if (toolbar_mode != TOOLBAR_MODE_SEARCH) {
            toolbar_mode = TOOLBAR_MODE_SEARCH;
            setup_messages_toolbar_subwindow();
        }
        search_message_control->text("");
        search_message_control->focus();
        tbx::StringSet filter_control = search_toolbar_win.gadget(0xa);
        filter_control.selected("All messages");
        filter_control.fade(g_app_data_model.get_currently_opened_chat()->messenger() != MESSENGER_CHATCUBE);
        search_reset_found();
    }
}

void ChatMainUI::search_begin() {
    search_reset_found();
    search_next();
}

void ChatMainUI::search_reset_found() {
    found_message_ids.clear();
    messages_view.search_show_found(nullptr);
    searching_up = false;
    search_toolbar_win.gadget(5).fade(true);
}

void ChatMainUI::search_prev() {
    if (!found_message_ids.empty()) {
        if (searching_up) { // pop current found message because it already displayed now
            found_message_ids.pop_back();
            searching_up = false;
            if (found_message_ids.empty()) {
                search_toolbar_win.gadget(5).fade(true);
                return;
            }
        }
        int64_t msgid = found_message_ids.back();
        found_message_ids.pop_back();
        if (found_message_ids.empty()) {
            search_toolbar_win.gadget(5).fade(true);
        }
        Logger::debug("search_prev msgid=%lld empty=%d", msgid, found_message_ids.empty());
        MessageDataPtr msg = g_app_data_model.get_currently_opened_chat()->get_message(msgid);
        if (msg) {
            Logger::debug("search_prev found msgid=%lld", msgid);
            messages_view.search_show_found(msg);
        } else {
            Logger::debug("search_prev not found msgid=%lld", msgid);
            auto on_load_success = [this, msgid]() {
                search_toolbar_win.gadget(4).fade(false);
                MessageDataPtr msg = g_app_data_model.get_currently_opened_chat()->get_message(msgid);
                if (msg != nullptr) {
                    messages_view.search_show_found(msg);
                }
                Logger::debug("search_prev on_load_success msgid=%lld found:%d", msgid, msg != nullptr);
            };
            auto on_load_fail = [this]() {
                search_toolbar_win.gadget(4).fade(false);
            };
            g_app_data_model.load_messages_in_chat(g_app_data_model.get_currently_opened_chat(), 0, msgid, on_load_success, on_load_fail);
            search_toolbar_win.gadget(4).fade(true);
        }
    }
}

void ChatMainUI::search_next() {
    std::string query = search_message_control->text();
    int filter = 0;
    switch(tbx::StringSet(search_toolbar_win.gadget(0xa)).selected_index()) {
        case 1:
            filter = MESSAGES_FILTER_LINKS;
            break;
        case 2:
            filter = MESSAGES_FILTER_EMAILS;
            break;
        case 3:
            filter = MESSAGES_FILTER_ATTACHMENTS;
            break;
    }
    if (filter == 0 && query.empty()) {
        show_alert_info("Please enter the 'Search for' text or/and set the 'Search in' filter");
        return;
    }
    messages_view.search_set_query(riscos_local_to_utf8(query), filter);
    auto callback = [this](MessageDataPtr msg) {
        if (msg) {
            Logger::debug("Message found %lld %s", msg->id, msg->text.c_str());
            found_message_ids.emplace_back(msg->id);
            messages_view.search_show_found(msg);
            searching_up = true;
            search_toolbar_win.gadget(5).fade(false);
        } else {
            show_alert_info("Messages not found");
        }
    };
    if (!g_app_data_model.get_currently_opened_chat()->messages.empty()) {
        //search_message_control->input_gadget.focus();
        int64_t starting_from_id = found_message_ids.empty() ? g_app_data_model.get_currently_opened_chat()->messages.back()->id : found_message_ids.back();
        g_app_data_model.search_in_chat(g_app_data_model.get_currently_opened_chat(), messages_view.search_substring, filter, starting_from_id, callback);
    }
}

void ChatMainUI::handle_scroll_y(int scroll_y, int visible_height) {
//    Logger::debug("ChatMainUI::handle_scroll_y scroll_y:%d", scroll_y);
    int ext_h = messages_view.win.extent().height();
    if (g_app_data_model.get_currently_opened_chat()) {
        if ((scroll_y > -160 && prev_scroll_y < scroll_y) || (scroll_y == 0 && ext_h > visible_height)) {
            messages_view.maintain_scroll_position(ScrollPosition::FIXED_FROM_BOTTOM);
//            Logger::debug("ScrollPosition::FIXED_FROM_BOTTOM");
            g_app_data_model.load_more_messages_in_chat(messages_view.get_chat(), true);
        } else if ((-scroll_y + visible_height + 160) > ext_h) {
            if (scroll_y < 0 && messages_view.get_chat()->has_newer_messages()) {
                messages_view.maintain_scroll_position(ScrollPosition::UNCHANGED);
                g_app_data_model.load_more_messages_in_chat(messages_view.get_chat(), false);
            } else {
                messages_view.maintain_scroll_position(ScrollPosition::GO_TO_BOTTOM);
                setup_messages_down_arrow(0);
            }
        } else {
//        Logger::debug("ScrollPosition::UNCHANGED");
            messages_view.maintain_scroll_position(ScrollPosition::UNCHANGED);
            setup_messages_down_arrow(-1);
        }
        prev_scroll_y = scroll_y;
    }
}
