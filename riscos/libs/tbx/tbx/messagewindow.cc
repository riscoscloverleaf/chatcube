/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2012-2015 Alan Buckley   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "messagewindow.h"

#include "window.h"
#include "gadget.h"
#include "font.h"
#include "showfullspec.h"
#include "modeinfo.h"
#include "button.h"
#include "message.h"

namespace tbx
{

/**
 * Show a message in a window centred in the screen.
 *
 * This shows the window and returns immediately. It is appropriate to use
 * instead of report_error as it allows the current and all other applications
 * to multi-task while the message is shown.
 *
 * @param msg The text of the message. This can be multi-line and the window
 *  will be resized to fit it.
 * @param title The title of the message box. Use "" (the default) for
 * the default title.
 * @param sprite_name The name of the sprite for the window. Use "" (the
 * default) to keep the sprite name in the Message resource. Useful sprites for
 * this provided with RISC OS include "information", "warning" and "error".
 * @param close_command - command to run when message window is closed
 * @param delete_command - true to delete the command after it has been run
 */
void show_message(const std::string &msg, const std::string &title /*=""*/, const std::string &sprite_name/*=""*/, tbx::Command *close_command /*= 0*/, bool delete_command /*= false*/)
{
   MessageWindow *mw = new MessageWindow(msg);
   if (!sprite_name.empty()) mw->sprite(sprite_name);
   if (!title.empty()) mw->title(title);
   if (close_command) mw->close_command(close_command, delete_command);
   mw->delete_on_hide();
   mw->show();
}

/**
 * Show a message in a window centred in the screen in the same way as
 * a menu is shown.
 *
 * This shows the window and returns immediately. It is appropriate to use
 * instead of report_error as it allows the current and all other applications
 * to multi-task while the message is shown.
 *
 * The window is closed if the OK button is selected or the mouse is clicked
 * outside the window. If a close command is set it is run in both of these
 * cases.
 *
 * @param msg The text of the message. This can be multi-line and the window
 *  will be resized to fit it.
 * @param title The title of the message box. Use "" (the default) to use
 * the default.
 * @param sprite_name The name of the sprite for the window. Use "" (the
 * default) to keep the sprite name in the Message resource. Useful sprites for
 * this provided with RISC OS include "information", "warning" and "error".
 * @param close_command - command to run when message window is closed
 * @param delete_command - true to delete the command after it has been run
 */
void show_message_as_menu(const std::string &msg, const std::string &title /*=""*/, const std::string &sprite_name/*=""*/,
	tbx::Command *close_command /*= 0*/, bool delete_command /*= false*/)
{
   MessageWindow *mw = new MessageWindow(msg);
   if (!sprite_name.empty()) mw->sprite(sprite_name);
   if (!title.empty()) mw->title(title);
   if (close_command) mw->close_command(close_command, delete_command);
   mw->delete_on_hide();
   mw->show_as_menu();
}

/**
 * Construct a message window with the give message
 *
 * @param msg The message for the window
 */
MessageWindow::MessageWindow(const std::string &msg) :
    TextDisplayWindow(msg, "OK", 0, 0)
{
   title("Message from " + message("_TaskName"));
}

MessageWindow::~MessageWindow()
{
}

/**
 * Set the name of the sprite to show in the message box
 *
 * @param sprite_name name of sprite shown in the message box.
 */
void MessageWindow::sprite(const std::string &sprite_name)
{
    tbx::Button sprite_gadget = _window.gadget(0);
    sprite_gadget.value(sprite_name);
}

};
