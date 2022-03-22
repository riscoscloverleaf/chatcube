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
 * fontmenu.cc
 *
 *  Created on: 01-Jul-2010
 *      Author: alanb
 */

#include "fontmenu.h"
#include "abouttobeshownlistener.h"
#include "hasbeenhiddenlistener.h"
#include "res/resfontmenu.h"

namespace tbx {

FontMenu::FontMenu(const res::ResFontMenu &object_template) : ShowPointObject(object_template) {}

/**
 * This event is raised just before the FontMenu underlying window is
 * about to be shown.
 */
void FontMenu::add_about_to_be_shown_listener(AboutToBeShownListener *listener)
{
	add_listener(0x82a40, listener, about_to_be_shown_router);
}

/**
 * Remove about to be shown listener
 */
void FontMenu::remove_about_to_be_shown_listener(AboutToBeShownListener *listener)
{
	remove_listener(0x82a40, listener);
}

/**
 * This event is raised after the FontMenu dialog has been completed
 */
void FontMenu::add_has_been_hidden_listener(HasBeenHiddenListener *listener)
{
	add_listener(0x82a41, listener, has_been_hidden_router);
}

/**
 * Remove has been hidden listener
 */
void FontMenu::remove_has_been_hidden_listener(HasBeenHiddenListener *listener)
{
	remove_listener(0x82a41, listener);
}

static void fontmenu_selection_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	FontMenuSelectionEvent ev(id_block, data);
    static_cast<FontMenuSelectionListener*>(listener)->fontmenu_selection(ev);
}

/**
 * Add listener to be called when a font is selected in the menu
 */
void FontMenu::add_selection_listener(FontMenuSelectionListener *listener)
{
	add_listener(0x82a42, listener, fontmenu_selection_router);
}

/**
 * Remove listener to be called when a font is selected in the menu
 */
void FontMenu::remove_selection_listener(FontMenuSelectionListener *listener)
{
	remove_listener(0x82a42, listener);
}


}
