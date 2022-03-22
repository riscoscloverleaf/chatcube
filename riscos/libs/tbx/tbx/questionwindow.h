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
 *//*
 * questionwindow.h
 *
 *  Created on: 8 Mar 2012
 *      Author: alanb
 */

#ifndef TBX_QUESTIONWINDOW_H_
#define TBX_QUESTIONWINDOW_H_

#include "textdisplaywindow.h"
#include "command.h"
#include <vector>

namespace tbx {

/**
 * Class to show a question in a window in the centre of the screen,
 * resizing the window to fit the question if necessary.
 *
 * To use this window you must have a template called "Question" in
 * your resources with the following characteristics.
 *
 * The visible area shown is the minimum size of the window.
 * The extent of the window sets the maximum width and height of the
 * window.
 *
 * Gadgets:
 *    0 - Button with the needs help flag set. This is used as a guide
 *        to position the message and is resized if necessary.
 *    1 - Yes action button
 *    2 - No action button
 *
 * A window you can copy to provide this is provided in TbxRes in the !Tbx
 * directory.
 *
 * If the depth or width of the window is changed due to the size of the
 * text the buttons are moved to stay at the same relative position to the
 * bottom right of the window.
 */
class QuestionWindow : public TextDisplayWindow
{
	std::vector<tbx::Command *> *_commands_to_delete;
public:
	QuestionWindow(const std::string &question);
	virtual ~QuestionWindow();

	void delete_commands();

	void add_yes_command(tbx::Command *yes_command);
	void add_no_command(tbx::Command *no_command);
};

void show_question(const std::string &question, const std::string &title, tbx::Command *yes_command, tbx::Command *no_command = 0, bool delete_commands = false);
void show_question_as_menu(const std::string &question, const std::string &title, tbx::Command *yes_command, tbx::Command *no_command = 0, bool delete_commands = false);

} /* namespace tbx */

#endif /* TBX_QUESTIONWINDOW_H_ */
