//
// Created by lenz on 7/8/21.
//

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <oslib/osfile.h>
#include <oslib/osfind.h>
#include <oslib/osargs.h>
#include <oslib/osgbpb.h>
#include <Wimptypes.h>
#include <tbx/application.h>
#include "CLClipboard.h"
#include "Logger.h"

CLClipboard* CLClipboard::_instance = nullptr;

CLClipboard::CLClipboard() {
    auto app = tbx::Application::instance();
    app->add_user_message_listener(message_CLAIM_ENTITY, this);
    app->add_user_message_listener(message_DATA_SAVE_ACK, this);
    app->add_user_message_listener(message_DATA_REQUEST, this);
    app->add_user_message_listener(message_RAM_FETCH, this);
    app->add_user_message_listener(message_DATA_SAVE, this);
    app->add_user_message_listener(message_RAM_TRANSMIT, this);
    app->add_user_message_listener(message_DATA_LOAD, this);

    app->add_recorded_message_listener(message_CLAIM_ENTITY, this);
    app->add_recorded_message_listener(message_DATA_SAVE_ACK, this);
    app->add_recorded_message_listener(message_DATA_REQUEST, this);
    app->add_recorded_message_listener(message_RAM_FETCH, this);
    app->add_recorded_message_listener(message_DATA_SAVE, this);
    app->add_recorded_message_listener(message_RAM_TRANSMIT, this);
    app->add_recorded_message_listener(message_DATA_LOAD, this);

    app->add_acknowledge_message_listener(message_CLAIM_ENTITY, this);
    app->add_acknowledge_message_listener(message_DATA_SAVE_ACK, this);
    app->add_acknowledge_message_listener(message_DATA_REQUEST, this);
    app->add_acknowledge_message_listener(message_RAM_FETCH, this);
    app->add_acknowledge_message_listener(message_DATA_SAVE, this);
    app->add_acknowledge_message_listener(message_RAM_TRANSMIT, this);
    app->add_acknowledge_message_listener(message_DATA_LOAD, this);
}
CLClipboard::~CLClipboard() {
    auto app = tbx::Application::instance();
    app->remove_user_message_listener(message_CLAIM_ENTITY, this);
    app->remove_user_message_listener(message_DATA_SAVE_ACK, this);
    app->remove_user_message_listener(message_DATA_REQUEST, this);
    app->remove_user_message_listener(message_RAM_FETCH, this);
    app->remove_user_message_listener(message_DATA_SAVE, this);
    app->remove_user_message_listener(message_RAM_TRANSMIT, this);
    app->remove_user_message_listener(message_DATA_LOAD, this);

    app->remove_recorded_message_listener(message_CLAIM_ENTITY, this);
    app->remove_recorded_message_listener(message_DATA_SAVE_ACK, this);
    app->remove_recorded_message_listener(message_DATA_REQUEST, this);
    app->remove_recorded_message_listener(message_RAM_FETCH, this);
    app->remove_recorded_message_listener(message_DATA_SAVE, this);
    app->remove_recorded_message_listener(message_RAM_TRANSMIT, this);
    app->remove_recorded_message_listener(message_DATA_LOAD, this);

    app->remove_acknowledge_message_listener(message_CLAIM_ENTITY, this);
    app->remove_acknowledge_message_listener(message_DATA_SAVE_ACK, this);
    app->remove_acknowledge_message_listener(message_DATA_REQUEST, this);
    app->remove_acknowledge_message_listener(message_RAM_FETCH, this);
    app->remove_acknowledge_message_listener(message_DATA_SAVE, this);
    app->remove_acknowledge_message_listener(message_RAM_TRANSMIT, this);
    app->remove_acknowledge_message_listener(message_DATA_LOAD, this);
}

CLClipboard* CLClipboard::instance() {
    if (!_instance) {
        _instance = new CLClipboard();
    }
    return _instance;
}

void CLClipboard::buffer_free() {
    if (clip_text) {
        free(clip_text);
        clip_text = nullptr;
    }
}

clipboard_text* CLClipboard::buffer_alloc(const char *text, int len, int remain) {
    clipboard_text *ret;
    ret = (clipboard_text*)malloc(sizeof(clipboard_text) + len + remain + 2);
    if (!ret) {
        return NULL;
    }
    ret->write_offset = len;
    ret->len = len + remain;
    if (len) {
        memcpy(ret->text, text, len);
    }
    ret->text[len + remain] = 0;
    //Logger::info("CLClipboard: Allocated buffer addr:%p len:%d text:%s", ret, sizeof(clipboard_text) + len + remain + 2, ret->text);
    return ret;
}

void CLClipboard::handle_event(tbx::WimpMessageEvent &event) {
    const tbx::WimpMessage &wimpmsg = event.message();
    wimp_block *block = (wimp_block *)event.message().str(0);
    //Logger::debug("CLClipboard::handle_event: %d", block->message.action);
    switch(block->message.action) {
        case message_CLAIM_ENTITY: {
            if (wimpmsg.sender_task_handle() != tbx::Application::instance()->task_handle()
                && (block->message.data.claim_entity.flags & wimp_CLAIM_CLIPBOARD)) {
                own_clipboard = false;
                buffer_free();
                event.claim();
            }
            break;
        }
        case message_DATA_REQUEST: {
            if (block->message.data.data_request.flags & wimp_DATA_REQUEST_CLIPBOARD
                && own_clipboard
                && clip_text) {

                wimp_t target = block->message.sender;
                Logger::info("CLClipboard: Received data request from %08x, offering data save", target);
                sprintf(block->message.data.data_xfer.file_name, "clipboard-%d", tbx::Application::instance()->task_handle());
                block->message.size =
                        sizeof(wimp_full_message_data_xfer) -
                        sizeof(block->message.data.data_xfer.file_name) +
                        ((strlen(block->message.data.data_xfer.file_name) + 4) & ~3);
                block->message.your_ref = block->message.my_ref;
                block->message.action = message_DATA_SAVE;
                /* window, icon, pos preserved */
                block->message.data.data_xfer.est_size = clip_text->len;
                block->message.data.data_xfer.file_type = 0xfff;
                if (xwimp_send_message(wimp_USER_MESSAGE, &block->message, target)) {
                    Logger::error("CLClipboard: message_DATA_SAVE failed!");
                } else {
                    Logger::info("CLClipboard: message_DATA_SAVE my_ref = %08x", block->message.my_ref);
                    paste_msg = block->message.my_ref;
                    paste_offset = 0;
                }
                event.claim();
            }
            break;
        }
        case message_DATA_SAVE_ACK: {
            Logger::info("CLClipboard: message_DATA_SAVE_ACK, your_ref = %08x, filename = \"%s\"",
                    block->message.your_ref,
                    block->message.data.data_xfer.file_name);
            if (block->message.your_ref == paste_msg) {
                paste_msg = 0;
                if (xosfile_save_stamped(block->message.data.data_xfer.file_name,
                                         block->message.data.data_xfer.file_type,
                                         clip_text->text,
                                         clip_text->text + clip_text->len)) {
                    Logger::error("CLClipboard: Clipboard save failed!");
                } else {
                    Logger::info("CLClipboard: Save OK, sending load message");
                    block->message.your_ref = block->message.my_ref;
                    block->message.action = message_DATA_LOAD;
                    xwimp_send_message(wimp_USER_MESSAGE_RECORDED, &block->message, block->message.sender);
                }
                event.claim();
            }
            break;
        }
        case message_RAM_FETCH: {
            Logger::info("CLClipboard: message_DATA_RAM_FETCH, your_ref = %08x", block->message.your_ref);
            if (block->message.your_ref == paste_msg) {
                /* We're attempting to paste text */
                int len = block->message.data.ram_xfer.size;
                if (len >= clip_text->len - paste_offset) {
                    len = clip_text->len - paste_offset;
                }
                if (len &&
                    xwimp_transfer_block((wimp_t) tbx::Application::instance()->task_handle(),
                            clip_text->text + paste_offset, block->message.sender,
                            block->message.data.ram_xfer.addr, len)) {
                    Logger::info("CLClipboard: RAM transfer failed!");
                    paste_msg = 0;
                } else {
                    Logger::info("CLClipboard: RAM transfer OK, sending ACK message");
                    block->message.your_ref = block->message.my_ref;
                    block->message.action = message_RAM_TRANSMIT;
                    wimp_event_no evt = (block->message.data.ram_xfer.size == len
                                         ? wimp_USER_MESSAGE_RECORDED
                                         : wimp_USER_MESSAGE); /* Send recorded messages until we transfer less data than was requested */
                    block->message.data.ram_xfer.size = len;
                    xwimp_send_message(evt, &block->message, block->message.sender);
                    paste_offset += len;
                    paste_msg = (evt == wimp_USER_MESSAGE ? 0 : block->message.my_ref);
                }
                event.claim();
            } else if (block->message.your_ref == fetch_msg) {
                /* We're attempting to get text */
                Logger::info("CLClipboard: RAM fetch bounced, trying data save instead");
                fetch_msg = 0;

                sprintf(block->message.data.data_xfer.file_name, "<Wimp$Scrap>");
                block->message.size =
                        sizeof(wimp_full_message_data_xfer) -
                        sizeof(block->message.data.data_xfer.file_name) +
                        ((strlen(block->message.data.data_xfer.file_name) + 4) & ~3);
                block->message.your_ref = check_msg;
                block->message.action = message_DATA_SAVE_ACK;
                block->message.data.data_xfer.w = wimp_NO_ICON;
                block->message.data.data_xfer.i = 0;
                block->message.data.data_xfer.pos.x = 0;
                block->message.data.data_xfer.pos.y = 0;
                block->message.data.data_xfer.est_size = clip_text->len;
                block->message.data.data_xfer.file_type = 0xfff;
                if (xwimp_send_message(wimp_USER_MESSAGE_RECORDED, &block->message, block->message.sender)) {
                    Logger::error("CLClipboard: message_DATA_SAVE_ACK failed!");
                    check_msg = 0;
                } else {
                    /* Wait for the data load */
                    check_msg = block->message.my_ref;
                }
                event.claim();
            }
            break;
        }

        /* Messages for when we're polling the clipboard */
        case message_DATA_SAVE: {
            if (block->message.your_ref == check_msg) {
                Logger::info("CLClipboard: Task %08x owns clipboard data of type %03x size %d", block->message.sender,
                        block->message.data.data_xfer.file_type, block->message.data.data_xfer.est_size);
                check_msg = 0;
                if ((block->message.data.data_xfer.file_type == 0xfff) &&
                    (block->message.data.data_xfer.est_size > 0)) {
                    if (clip_text) {
                        Logger::info("CLClipboard: Free old clipboard buffer, addr: %p", clip_text);
                        buffer_free();
                    }
                    Logger::info("CLClipboard: Allocating new clipboard buffer, est_size: %d", block->message.data.data_xfer.est_size);
                    clip_text = buffer_alloc(NULL, 0, block->message.data.data_xfer.est_size);
                    if (clip_text) {
                        check_msg = block->message.my_ref;
                        Logger::info("CLClipboard: Trying RAM fetch");
                        block->message.size = sizeof(wimp_full_message_ram_xfer);
                        block->message.your_ref = block->message.my_ref;
                        block->message.action = message_RAM_FETCH;
                        block->message.data.ram_xfer.addr = clip_text->text;
                        block->message.data.ram_xfer.size = clip_text->len +
                                                           1; /* RAM transmit terminates when we receive less than we requested, so always request one more byte than necessary (which, if received, will overwrite our null terminator) */
                        if (xwimp_send_message(wimp_USER_MESSAGE_RECORDED, &block->message,
                                               block->message.sender)) {
                            Logger::error("CLClipboard: message_RAM_FETCH failed!");
                            check_msg = 0;
                        } else {
                            fetch_msg = block->message.my_ref;
                        }
                    }
                }
                event.claim();
            }
            break;
        }
        case message_RAM_TRANSMIT: {
            if (block->message.your_ref == fetch_msg) {
                Logger::info("CLClipboard: Received %d/%d bytes via RAM transfer", block->message.data.ram_xfer.size,
                        clip_text->len - clip_text->write_offset);
                check_msg = 0;
                clip_text->write_offset += block->message.data.ram_xfer.size;
                if (clip_text->write_offset > clip_text->len) {
                    /* Oh dear, the estimated length was bogus
                       Just reset our position and go round again
                       (will result in us only getting the end of the clipboard,
                       but it will avoid the RAM transfer protocol stalling) */
                    clip_text->write_offset = 0;
                    block->message.size = sizeof(wimp_full_message_ram_xfer);
                    block->message.your_ref = block->message.my_ref;
                    block->message.action = message_RAM_FETCH;
                    block->message.data.ram_xfer.addr = clip_text->text;
                    block->message.data.ram_xfer.size = clip_text->len + 1;
                    if (xwimp_send_message(wimp_USER_MESSAGE_RECORDED, &block->message,
                                           block->message.sender)) {
                        Logger::info("CLClipboard: message_RAM_FETCH failed!");
                        fetch_msg = 0;
                    } else {
                        fetch_msg = block->message.my_ref;
                    }
                } else {
                    /* Transaction complete, push to server */
                    fetch_msg = 0;
                    clip_text->len = clip_text->write_offset; /* Just in case it was less than we were originall told */
                    clip_text->text[clip_text->len] = 0; /* And make sure we put the terminator back, in case it was overwritten */
                    if (has_get_clipboard_success_callback) {
                        has_get_clipboard_success_callback = false;
                        clipboard_get_success_callback(clip_text->text);
                    }
                }
                event.claim();
            } else if (block->message.your_ref == paste_msg) {
                Logger::error("CLClipboard: message_RAM_TRANSMIT failed!");
                /* Just give up then */
                paste_msg = 0;
                event.claim();
            }
            break;
        }
        case message_DATA_LOAD: {
            if (block->message.your_ref == check_msg) {
                Logger::info("CLClipboard: Data save worked OK, file '%s'", block->message.data.data_xfer.file_name);
                check_msg = 0;
                os_fw f = 0;
                xosfind_openinw(osfind_NO_PATH, block->message.data.data_xfer.file_name, NULL, &f);
                bool ok = (f != 0);
                int unread;
                if (ok && xosgbpb_readw(f, clip_text->text, clip_text->len, &unread)) ok = false;
                if (ok && unread) ok = false;
                osbool eof;
                if (ok && xosargs_read_eof_statusw(f, &eof)) ok = false;
                if (ok && !eof) ok = false;
                if (f) xosfind_closew(f);
                if (ok) {
                    Logger::info("CLClipboard: Data read OK");
                    /* Ack the message */
                    block->message.your_ref = block->message.my_ref;
                    block->message.action = message_DATA_LOAD_ACK;
                    xwimp_send_message(wimp_USER_MESSAGE, &block->message, block->message.sender);

                    if (has_get_clipboard_success_callback) {
                        has_get_clipboard_success_callback = false;
                        clipboard_get_success_callback(clip_text->text);
                    }
                } else {
                    Logger::error("CLClipboard: Data load failed!");
                }
                event.claim();
            }
            break;
        }
    }
}

void CLClipboard::clipboard_set(const std::string& text) {
    buffer_free();
    if (!text.empty()) {
        paste_msg = 0; /* Abort any current paste op */
        check_msg = 0;
        fetch_msg = 0;
        // allocate memory for host clipboard content
        //Logger::info("Allocating buffer for clipboard content, len:%d text:%s", text.size(), text.c_str());
        clip_text = buffer_alloc(text.c_str(), text.size() + 1, 0);
        Logger::info("CLClipboard: Clipboard text set: %s", clip_text->text);
        if (!own_clipboard) {
            wimp_block block;
            /* Claim the clipboard */
            Logger::debug("CLClipboard: Clipboard text set, claiming clipboard...");
            block.message.size = sizeof(wimp_full_message_claim_entity);
            block.message.your_ref = 0;
            block.message.action = message_CLAIM_ENTITY;
            block.message.data.claim_entity.flags = wimp_CLAIM_CLIPBOARD;
            own_clipboard = true;
            if (xwimp_send_message(wimp_USER_MESSAGE, &block.message, wimp_BROADCAST)) {
                Logger::error("CLClipboard: Claim failed!");
                own_clipboard = false;
            }
        } else {
            Logger::info("CLClipboard: clipboard_set(text), clipboard already owned");
        }
    }
}

void CLClipboard::clipboard_get(std::function<void(char*)> on_success) {
    has_get_clipboard_success_callback = false;
    if (!own_clipboard) {
        wimp_block block;

        buffer_free();
        block.message.size = sizeof(wimp_full_message_data_request) -
                             sizeof(block.message.data.data_request.file_types) + 8;
        block.message.your_ref = 0;
        block.message.action = message_DATA_REQUEST;
        block.message.data.data_request.w = wimp_NO_ICON;
        block.message.data.data_request.i = 0;
        block.message.data.data_request.pos.x = 0;
        block.message.data.data_request.pos.y = 0;
        block.message.data.data_request.flags = wimp_DATA_REQUEST_CLIPBOARD;
        block.message.data.data_request.file_types[0] = 0xfff;
        block.message.data.data_request.file_types[1] = -1;
        if (xwimp_send_message(wimp_USER_MESSAGE_RECORDED, &block.message, wimp_BROADCAST)) {
            Logger::error("CLClipboard: Data request failed");
        } else {
            check_msg = block.message.my_ref;
            clipboard_get_success_callback = on_success;
            has_get_clipboard_success_callback = true;
        }
    } else {
        on_success(clip_text->text);
    }
}