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
 * quit.cc
 *
 *  Created on: 09-Jun-2009
 *      Author: alanb
 */

#include "quit.h"
#include "abouttobeshownlistener.h"
#include "hasbeenhiddenlistener.h"
#include "res/resquit.h"

namespace tbx {

Quit::Quit(const res::ResQuit &object_template) : ShowFullObject(object_template) {}

/**
 * This event is raised just before the Quit underlying window is
 * about to be shown.
 *
 * @param listener listener to add
 */
void Quit::add_about_to_be_shown_listener(AboutToBeShownListener *listener)
{
	add_listener(0x82a90, listener, about_to_be_shown_router);
}

/**
 * Remove about to be shown listener
 *
 * @param listener listener to remove
 */
void Quit::remove_about_to_be_shown_listener(AboutToBeShownListener *listener)
{
	remove_listener(0x82a90, listener);
}

/**
 * This event is raised after the Quit dialog has been completed
 *
 * @param listener listener to add
 */
void Quit::add_has_been_hidden_listener(HasBeenHiddenListener *listener)
{
	add_listener(0x82a92, listener, has_been_hidden_router);
}

/**
 * Remove has been hidden listener
 *
 * @param listener listener to remove
 */
void Quit::remove_has_been_hidden_listener(HasBeenHiddenListener *listener)
{
	remove_listener(0x82a92, listener);
}

static void quit_quit_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	EventInfo ev(id_block, data);
    static_cast<QuitQuitListener*>(listener)->quit_quit(ev);
}

/**
 * Add listener to be called when quit is selected from the dialogue
 *
 * @param listener listener to add
 */
void Quit::add_quit_listener(QuitQuitListener *listener)
{
	add_listener(0x82a91, listener, quit_quit_router);
}

/**
 * Remove listener called when quit is selected from the dialogue
 *
 * @param listener listener to remove
 */
void Quit::remove_quit_listener(QuitQuitListener *listener)
{
	remove_listener(0x82a91, listener);
}

static void quit_cancel_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	EventInfo ev(id_block, data);
    static_cast<QuitCancelListener*>(listener)->quit_cancel(ev);
}

/**
 * Add listener to be called when cancel is selected from the dialogue
 *
 * @param listener listener to add
 */
void Quit::add_cancel_listener(QuitCancelListener *listener)
{
	add_listener(0x82a93, listener, quit_cancel_router);
}

/**
 * Remove listener called when cancel is selected from the dialogue
 *
 * @param listener listener to remove
 */
void Quit::remove_cancel_listener(QuitCancelListener *listener)
{
	remove_listener(0x82a93, listener);
}


}
