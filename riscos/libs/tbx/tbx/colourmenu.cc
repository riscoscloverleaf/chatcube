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
 * colourmenu.cc
 *
 *  Created on: 01-Jul-2010
 *      Author: alanb
 */

#include "colourmenu.h"
#include "abouttobeshownlistener.h"
#include "hasbeenhiddenlistener.h"
#include "res/rescolourmenu.h"

namespace tbx {

ColourMenu::ColourMenu(const res::ResColourMenu &object_template) : ShowPointObject(object_template) {}

/**
 * This event is raised just before the ColourMenu underlying window is
 * about to be shown.
 *
 * @param listener about to be shown listener to add
 */
void ColourMenu::add_about_to_be_shown_listener(AboutToBeShownListener *listener)
{
	add_listener(0x82980, listener, about_to_be_shown_router);
}

/**
 * Remove about to be shown listener
 *
 * @param listener about to be shown listener to remove
 */
void ColourMenu::remove_about_to_be_shown_listener(AboutToBeShownListener *listener)
{
	remove_listener(0x82980, listener);
}

/**
 * This event is raised after the ColourMenu dialog has been completed
 *
 * @param listener has been hidden listener to add
 */
void ColourMenu::add_has_been_hidden_listener(HasBeenHiddenListener *listener)
{
	add_listener(0x82981, listener, has_been_hidden_router);
}

/**
 * Remove has been hidden listener
 *
 * @param listener has been hidden listener to remove
 */
void ColourMenu::remove_has_been_hidden_listener(HasBeenHiddenListener *listener)
{
	remove_listener(0x82981, listener);
}

//! @cond INTERNAL
static void colourmenu_selection_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	ColourMenu cm(id_block.self_object());
    static_cast<ColourMenuSelectionListener*>(listener)->colourmenu_selection(cm, WimpColour(data.word[4]));
}
//! @endcond

/**
 * Add listener to be called when the a colour has been selected from the menu.
 *
 * @param listener colour selected listener to add
 */
void ColourMenu::add_selection_listener(ColourMenuSelectionListener *listener)
{
	add_listener(0x82982, listener, colourmenu_selection_router);
}

/**
 * Remove listener for menu colour selection
 *
 * @param listener colour selected listener to remove
 */
void ColourMenu::remove_selection_listener(ColourMenuSelectionListener *listener)
{
	remove_listener(0x82982, listener);
}


}
