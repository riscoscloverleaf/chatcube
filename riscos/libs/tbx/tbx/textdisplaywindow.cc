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
/*
 * textdisplaywindow.cc
 *
 *  Created on: 2 Mar 2012
 *      Author: alanb
 */

#include "textdisplaywindow.h"
#include "window.h"
#include "gadget.h"
#include "font.h"
#include "modeinfo.h"
#include "button.h"
#include "res/reswindow.h"
#include "res/resbutton.h"
#include "res/resactionbutton.h"
#include "application.h"
#include "margin.h"
#include "command.h"
#include <cstring>

namespace tbx
{

/**
 * Construct a text display window with the given text and buttons
 *
 * If the default button or cancel button are given a value other than
 * -1 the window will be given the input focus when shown.
 *
 * @param text The message for the window
 * @param buttons string contains semi-colon seperated list of button text
 * @param default_button index (from 0) of button that is default button
 *        or -1 if there is not a default button
 * @param cancel_button index (from 0) of button that is cancel button
 *        or -1 if there is not a cancel button
 * @param button_width width of buttons (default 200 OS units)
 */
TextDisplayWindow::TextDisplayWindow(const std::string &text,
        const char *buttons,
	    int default_button /*= -1*/, int cancel_button /*= -1*/,
	    int button_width /*= 200*/) :
		_text(text),
		_delete_on_hide(false),
		_close_command(0),
		_delete_close_command(false)
{
    create_window(buttons, default_button, cancel_button, button_width);

	_window.add_redraw_listener(this);
}

/**
 * Destructor, delete close command if necessary
 */
TextDisplayWindow::~TextDisplayWindow()
{
	if (_delete_close_command) delete _close_command;
}


/**
 * Set the title for the text display window
 *
 * @param new_title New caption for the text display window
 */
void TextDisplayWindow::title(const std::string &new_title)
{
    _window.title(new_title);
}

/**
 * Set up text display window to delete itself when it is hidden.
 *
 * The text display window should have been created using new.
 */
void TextDisplayWindow::delete_on_hide()
{
	if (_delete_on_hide) return;
	_delete_on_hide = true;
	if (!_close_command) _window.add_has_been_hidden_listener(this);
}

/**
 * Set command to be run when message window is closed.
 *
 * Closing includes using the close button or clicking outside
 * it if it has been shown as a menu
 *
 * @param close_command command to run on close
 * @param delete_close_command delete the close command when the window is deleted
 */
void TextDisplayWindow::close_command(tbx::Command *close_command, bool delete_command /*= false*/)
{
	if (!_delete_on_hide && !_close_command) _window.add_has_been_hidden_listener(this);
	_close_command = close_command;
	_delete_close_command = delete_command;
}

/**
 * Show the text display window
 *
 * The size of the message is calculated and the window resized and gadgets
 * moved accordingly at this point.
 */
void TextDisplayWindow::show()
{
    _window.show();
}

/**
 * Show the text display window as a menu
 *
 * When shown as a menu clicking outside of the window will close it.
 *
 * The size of the message is calculated and the window resized and gadgets
 * moved accordingly at this point.
 */
void TextDisplayWindow::show_as_menu()
{
    _window.show_as_menu();
}


/**
 * Helper function to create the window
 *
 * @param buttons semi-colon separated list of button text
 * @param default_button index (from 0) of button that is default button
 *        or -1 if there is not a default button
 * @param cancel_button index (from 0) of button that is cancel button
 *        or -1 if there is not a cancel button
 * @param button_width width of buttons (default 200 OS units)
 */
void TextDisplayWindow::create_window(const char *buttons,
       int default_button, int cancel_button,
	   int button_width)
{
    res::ResWindow twindow("TbxMessage");
    const int gap = 16; // Gap between edges and elements
    const int right_margin = gap + gap; // Space on right
    const int icon_size = 96;
    const int button_height = 68;
    const int rule_height = 8;
    int text_height;
    int window_width;
    int window_height;
    int num_buttons = 1;
    const char *button_text = buttons;

    while ((button_text = std::strchr(button_text, ';')) != 0)
    {
       num_buttons++;
       button_text++;
    }

    tbx::ModeInfo mode_info;
    tbx::Size screen_size = mode_info.screen_size();

    int min_width = (button_width + gap) * num_buttons + right_margin;
    int max_width = screen_size.width / 2;

    if (min_width < 256) min_width = 256;
    if (max_width < min_width) max_width = min_width;

    _text_bounds = BBox(gap * 2 + icon_size,
                        -gap - 40,
                        gap * 2 + icon_size + min_width,
                        -gap);
    calc_line_ends(max_width);

    if (_text_bounds.height() < icon_size)
    {
       text_height = icon_size;
       _text_bounds.move(0, (_text_bounds.height() - text_height) / 2);
    } else text_height = _text_bounds.height();

    window_width = _text_bounds.width();
    window_width += icon_size + gap  * 2 + right_margin;
    if (window_width < min_width) window_width = min_width;
    window_height = text_height;
    window_height += gap * 4 + button_height;

    // Set window flags
    twindow.title_text("Message");
    twindow.title_buflen(64);
    twindow.auto_redraw(false);
    twindow.hscrollbar(false);
    twindow.vscrollbar(false);
    twindow.back_icon(false);
    twindow.close_icon(false);
    twindow.toggle_size_icon(false);
    twindow.generate_has_been_hidden(true);
    twindow.hide_event(0x82890); // Default has been hidden event

    // size window and centre on screen
    int wx = (screen_size.width - window_width)/2;
    int wy = (screen_size.height - window_height)/2;
    twindow.visible_xmin(wx);
    twindow.visible_xmax(wx + window_width);
    twindow.visible_ymin(wy);
    twindow.visible_ymax(wy + window_height);

    twindow.work_xmin(0);
    twindow.work_xmax(window_width);
    twindow.work_ymin(-window_height);
    twindow.work_ymax(0);

    // Add Icon to left - actual icon used set in derived classes
    res::ResButton icon;
    icon.component_id(0);
    icon.is_sprite(true);
    icon.hcentred(true);
    icon.vcentred(true);
    icon.value("information");
    icon.xmin(gap);
    icon.xmax(icon_size + gap);
    icon.ymin(-text_height - gap);
    icon.ymax(-gap);

    twindow.add_gadget(icon);

    // Add Icon as rule off
    res::ResButton rule;
    rule.component_id(99);
    rule.has_text(true);
    rule.hcentred(true);
    rule.vcentred(true);
    rule.has_border(true);
    rule.validation("R2");
    rule.xmin(0);
    rule.xmax(window_width);
    rule.ymax(-gap * 2 - text_height);
    rule.ymin(rule.ymax() - rule_height);
    twindow.add_gadget(rule);

    res::ResActionButton button;
    bool get_focus = false;
    char *text_end;

    button_text = buttons;
    button.xmin(window_width - (button_width + gap) * (num_buttons+1) + gap - right_margin);
    button.ymin(-window_height + gap);
    button.ymax(-window_height + button_height + gap);

    for (int j = 0; j < num_buttons; j++)
    {
        text_end = std::strchr(button_text, ';');
        if (text_end)
        {
           button.text(std::string(button_text, text_end - button_text));
           button_text = text_end + 1;
        } else button.text(button_text);

        button.component_id(10+j);
        if (j == default_button)
        {
           button.is_default(true);
           get_focus = true;
        } else
        {
           button.is_default(false);
        }

        if (j == cancel_button)
        {
           button.cancel(true);
           get_focus = true;
        } else
        {
           button.cancel(false);
        }

        button.xmin(button.xmin() + button_width + gap);
        button.xmax(button.xmin() + button_width);

        twindow.add_gadget(button);
    }

    if (get_focus) twindow.default_focus(res::FOCUS_TO_WINDOW);

    _window = Window(twindow);
}

/**
 * Redraw the message text
 *
 * @param e Redraw information
 */
void TextDisplayWindow::redraw (const tbx::RedrawEvent &e)
{
    int x = e.visible_area().screen_x(_text_bounds.min.x)+2;
    int y = e.visible_area().screen_y(_text_bounds.max.y)-32;

    tbx::WimpFont font;
    font.set_colours(tbx::WimpColour::black, tbx::WimpColour::no_colour);
    const char *text = _text.c_str();
    int start = 0;

    for (unsigned int row = 0; row < _line_end.size(); row++)
	{
		int end = _line_end[row];
		if (text[start] == '\n') start++;
		else if (text[start] == ' ' && start > 0 && text[start-1] != '\n') start++;
		if (start < end)
		{
			font.paint(x, y, text + start, end-start);
		}
		start = end;
		y -= 40;
	}
}

/**
 * Calculate the line end positions in the string.
 *
 * The message bounds will be increased up to max width and as
 * high as necessary to fit the text.
 *
 * @param max_width - maximum width for expansion
 * @returns true if message bounds were expanded
 */
bool TextDisplayWindow::calc_line_ends(int max_width)
{
   int width = _text_bounds.width(); // Minimum width set by template
   int height = 8;
   tbx::Font font;
   font.desktop_font();
   const char *text = _text.c_str();
   int pos = 0;
   int start;
   int line_width;

   while (pos < (int)_text.size())
   {
        height += 40;
		start = pos;
		pos = font.find_split_os(text + start, -1, max_width, ' ') + start;
		if (pos == start)
		{
			pos = font.find_index_xy_os(text + start, -1, max_width, 0) + start;
			if (pos == start) pos++;
		}
		_line_end.push_back(pos);
		if (pos > start)
		{
		   line_width = font.string_width_os(text+start, pos - start);
		   if (line_width > width) width = line_width;
		}
		if (text[pos] == '\n') pos++;
	}

    // Update message box bounds if necessary
    bool updated = false;
    if (width > _text_bounds.width())
    {
        _text_bounds.max.x = _text_bounds.min.x + width;
        updated = true;
    }
    if (height > _text_bounds.height())
    {
        _text_bounds.min.y = _text_bounds.max.y - height;
        updated = true;
    }

    return updated;
}


/**
 * Self destruct when message window is hidden
 *
 * Set delete_on_hide before showing the window.
 */
void TextDisplayWindow::has_been_hidden (const tbx::EventInfo &hidden_event)
{
	if (_close_command) _close_command->execute();
	if (_delete_on_hide)
	{
	   hidden_event.id_block().self_object().delete_object();
	   delete this;
	}
}


} /* namespace tbx */

