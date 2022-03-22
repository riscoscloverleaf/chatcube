/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2012 Alan Buckley   All Rights Reserved.
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
 * textdisplaywindow.h
 *
 *  Created on: 2 Mar 2012
 *      Author: alanb
 */

#ifndef TBX_TEXTDISPLAYWINDOW_H_
#define TBX_TEXTDISPLAYWINDOW_H_

#include <string>
#include <vector>

#include "redrawlistener.h"
#include "hasbeenhiddenlistener.h"
#include "window.h"
#include "showfullspec.h"

namespace tbx {

/**
 * Class to show some text in a window in the centre of the screen,
 * with an icon to the left and one or buttons at the bottom.
 *
 * The window is created to fit the text.
 *
 * The buttons are created with the component ids 10 and up.
 *
 * Subclasses can use the _window protected member to modify
 * the window and add event handlers.
 *
 * The MessageWindow and QuestionWindow both derived from
 * this class.
 */
class TextDisplayWindow  :
	public tbx::RedrawListener,
	public tbx::HasBeenHiddenListener
{
protected:
	tbx::Window _window;
private:
	std::string _text;
	std::vector<int> _line_end;
	tbx::BBox _text_bounds;
	bool _delete_on_hide;
	tbx::Command *_close_command;
	bool _delete_close_command;

	virtual void redraw (const tbx::RedrawEvent &e);
	virtual void has_been_hidden (const tbx::EventInfo &hidden_event);

    void create_window(const char *buttons,
       int default_button, int cancel_button,
	   int button_width);
	bool calc_line_ends(int max_width);

public:
	TextDisplayWindow(const std::string &text, const char *buttons,
	   int default_button = -1, int cancel_button = -1,
	   int button_width = 200);
	virtual ~TextDisplayWindow();

    void title(const std::string &title);
    /**
     * Get underlying window used for the message window
     */
    tbx::Window window() {return _window;}

    void delete_on_hide();
    void close_command(tbx::Command *close_command, bool delete_command = false);

    void show();
    void show_as_menu();
};

} /* namespace tbx */
#endif /* TEXTDISPLAYWINDOW_H_ */
