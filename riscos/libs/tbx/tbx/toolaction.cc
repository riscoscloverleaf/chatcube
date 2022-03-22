/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010 Alan Buckley   All Rights Reserved.
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
 * toolaction.cc
 *
 *  Created on: 6-Jul-2010
 *      Author: alanb
 */

#include "toolaction.h"
#include "swixcheck.h"
#include "kernel.h"
#include "command.h"
#include "commandrouter.h"
#include <swis.h>

namespace tbx
{

/**
 * Set text or sprite to show when button is on
 */
void ToolAction::on_ident(std::string text)
{
	swix_check(_swix(0x44EC6, _INR(0,4), 1, _handle, 0x140140, _id,
			reinterpret_cast<int>(text.c_str())));
}
/**
 * Get text or sprite to show when button is on
 */
std::string ToolAction::on_ident() const
{
    _kernel_swi_regs regs;
    regs.r[0] = 1;
    regs.r[1] = _handle;
    regs.r[2] = 0x140141;
    regs.r[3] = _id;
    regs.r[4] = 0;
    regs.r[5] = 0;

    // Run Toolbox_ObjectMiscOp to get the size of the buffer
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    std::string value;
    int len = regs.r[5];
    if (len)
    {
       char buffer[len];
       regs.r[4] = reinterpret_cast<int>(buffer);
       regs.r[5] = len;

       // Run Toolbox_ObjectMiscOp to get the buffer
       swix_check(_kernel_swi(0x44ec6, &regs, &regs));

       value = buffer;
    }

    return value;
}

/**
 * Set text or sprite to show when button is off
 */
void ToolAction::off_ident(std::string text)
{
	swix_check(_swix(0x44EC6, _INR(0,4), 0, _handle, 0x140140, _id,
			reinterpret_cast<int>(text.c_str())));
}

/**
 * Get text or sprite to show when button is off
 */
std::string ToolAction::off_ident() const
{
    _kernel_swi_regs regs;
    regs.r[0] = 0;
    regs.r[1] = _handle;
    regs.r[2] = 0x140141;
    regs.r[3] = _id;
    regs.r[4] = 0;
    regs.r[5] = 0;

    // Run Toolbox_ObjectMiscOp to get the size of the buffer
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    std::string value;
    int len = regs.r[5];
    if (len)
    {
       char buffer[len];
       regs.r[4] = reinterpret_cast<int>(buffer);
       regs.r[5] = len;

       // Run Toolbox_ObjectMiscOp to get the buffer
       swix_check(_kernel_swi(0x44ec6, &regs, &regs));

       value = buffer;
    }

    return value;
}


/**
 * Set text or sprite to show when button is faded
 */
void ToolAction::fade_ident(std::string text)
{
	swix_check(_swix(0x44EC6, _INR(0,4), 2, _handle, 0x140140, _id,
			reinterpret_cast<int>(text.c_str())));
}

/**
 * Get text or sprite to show when button is faded
 */
std::string ToolAction::fade_ident() const
{
    _kernel_swi_regs regs;
    regs.r[0] = 2;
    regs.r[1] = _handle;
    regs.r[2] = 0x140141;
    regs.r[3] = _id;
    regs.r[4] = 0;
    regs.r[5] = 0;

    // Run Toolbox_ObjectMiscOp to get the size of the buffer
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    std::string value;
    int len = regs.r[5];
    if (len)
    {
       char buffer[len];
       regs.r[4] = reinterpret_cast<int>(buffer);
       regs.r[5] = len;

       // Run Toolbox_ObjectMiscOp to get the buffer
       swix_check(_kernel_swi(0x44ec6, &regs, &regs));

       value = buffer;
    }

    return value;
}

/**
 * Set action to take on select and adjust clicks
 */
void ToolAction::set_action(int select_action, int adjust_action)
{
	swix_check(_swix(0x44EC6, _INR(0,5), 0, _handle, 0x140142, _id,
			select_action, adjust_action));
}

/**
 * Get the actions on select or adjust clicks
 */
void ToolAction::get_action(int &select_action, int &adjust_action) const
{
	swix_check(_swix(0x44EC6, _INR(0,3)|_OUTR(0,1), 0, _handle, 0x140143, _id,
			&select_action, &adjust_action));
}

/**
 * Set objects to show on select or adjust clicks
 */
void ToolAction::set_click_show(Object select_object, Object adjust_object)
{
	swix_check(_swix(0x44EC6, _INR(0,5), 0, _handle, 0x140144, _id,
			select_object.handle(), adjust_object.handle()));
}

/**
 * Get the objects shown on select or adjust clicks
 */
void ToolAction::get_click_show(Object &select_object, Object &adjust_object) const
{
	int sel_obj, adj_obj;

	swix_check(_swix(0x44EC6, _INR(0,3)|_OUTR(0,1), 0, _handle, 0x140145, _id,
			&sel_obj, &adj_obj));

	select_object = Object((ObjectId)sel_obj);
	adjust_object = Object((ObjectId)adj_obj);
}

static void toolaction_selected_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	ToolActionSelectedEvent ev(id_block, data);
	static_cast<ToolActionSelectedListener *>(listener)->toolaction_selected(ev);
}

/**
 * Add listener for tool action button selected
 */
void ToolAction::add_selected_listener(ToolActionSelectedListener *listener)
{
	add_listener(0x140140, listener, toolaction_selected_router);
}

/**
 * Remove listener for tool action button selected
 */
void ToolAction::remove_selected_listener(ToolActionSelectedListener *listener)
{
	remove_listener(0x140140, listener);
}

static void toolaction_select_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	if ((data.word[3] & 1) == 0)
		static_cast<Command *>(listener)->execute();
}

static void toolaction_adjust_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	if ((data.word[3] & 1) != 0)
		static_cast<Command *>(listener)->execute();
}

/**
 * Add command to be run if the button is selected with the select or adjust.
 */
void ToolAction::add_selected_command(Command *command)
{
	add_listener(0x140140, command, command_router);
}

/**
 * Remove command to be run if the button is selected with the select or adjust.
 */
void ToolAction::remove_selected_command(Command *command)
{
	remove_listener(0x140140, command);
}

/**
 * Add command to be run if the button is selected with the select mouse button.
 */

void ToolAction::add_select_command(Command *command)
{
	add_listener(0x140140, command, toolaction_select_router);
}

/**
 * Remove command to be run if the button is selected with the select mouse button.
 */
void ToolAction::remove_select_command(Command *command)
{
	remove_listener(0x140140, command);
}

/**
 * Add command to be run if the button is selected with the adjust mouse button
 * only
 */
void ToolAction::add_adjust_command(Command *command)
{
	add_listener(0x140140, command, toolaction_adjust_router);
}

/**
 * Remove command to be run if the button is selected with the adjust mouse button
 * only
 */
void ToolAction::remove_adjust_command(Command *command)
{
	remove_listener(0x140140, command);
}

}
