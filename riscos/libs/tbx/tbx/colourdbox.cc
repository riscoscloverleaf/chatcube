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
 * colourdbox.cc
 *
 *  Created on: 01-Jul-2010
 *      Author: alanb
 */

#include "colourdbox.h"
#include "abouttobeshownlistener.h"
#include "hasbeenhiddenlistener.h"
#include "res/rescolourdbox.h"
#include <cstring>

namespace tbx {

ColourDbox::ColourDbox(const res::ResColourDbox &object_template) : ShowPointObject(object_template) {}

/**
 * This event is raised just before the ColourDbox underlying window is
 * about to be shown.
 *
 * @param listener listener for about to be shown events
 */
void ColourDbox::add_about_to_be_shown_listener(AboutToBeShownListener *listener)
{
	add_listener(0x829C0, listener, about_to_be_shown_router);
}

/**
 * Remove about to be shown listener
 *
 * @param listener listener for about to be shown events
 */
void ColourDbox::remove_about_to_be_shown_listener(AboutToBeShownListener *listener)
{
	remove_listener(0x829C0, listener);
}

//! @cond INTERNAL
static void colourdbox_dialog_completed_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	ColourDbox dbox(id_block.self_object());
    static_cast<ColourDboxDialogueCompletedListener*>(listener)->colourdbox_dialogue_completed(dbox, (data.word[3]&1)!=0);
}
//! @endcond

/**
 * Add listener to be called when the dialogue has been completed
 *
 * @param listener listener for dialogue completed events
 */
void ColourDbox::add_dialogue_completed_listener(ColourDboxDialogueCompletedListener *listener)
{
	add_listener(0x829c1, listener, colourdbox_dialog_completed_router);
}

/**
 * Remove listener to be called when the dialogue has been completed
 *
 * @param listener listener for dialogue completed events
 */
void ColourDbox::remove_dialogue_completed_listener(ColourDboxDialogueCompletedListener *listener)
{
	remove_listener(0x829c1, listener);
}

//! @cond INTERNAL
static void colourdbox_colour_selected_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	ColourSelectedEvent ev(id_block.self_object(),
			(data.word[3] & 1) != 0,
			reinterpret_cast<const unsigned char *>(&data.word[4])
			);
    static_cast<ColourSelectedListener*>(listener)->colour_selected(ev);
}
//! @endcond

/**
 * Add listener to report colour select when dialog is finished
 * with OK.
 *
 * @param listener listener for colour selected events
 */
void ColourDbox::add_colour_selected_listener(ColourSelectedListener *listener)
{
	add_listener(0x829c2, listener, colourdbox_colour_selected_router);
}

/**
 * Remove the colour selected listener
 *
 * @param listener listener for colour selected events
 */
void ColourDbox::remove_colour_selected_listener(ColourSelectedListener *listener)
{
	remove_listener(0x829c2, listener);
}

/**
 * Construct a colour selected event from event data
 *
 * @param cbox ColourDbox event occurred upon
 * @param none true if no colour option is provided
 * @param block selected colour details
 */
ColourSelectedEvent::ColourSelectedEvent(ColourDbox cbox, bool none, const unsigned char *block)
   : _colour_dbox(cbox), _none(none)
{
	int remainder_size = *((int *)(block + 4));
	_block = 0;
	_block = new unsigned char[12 + remainder_size];
	//TODO: Should this be 8 + remainder size - in which case what happens to the model field
	// if it's 0.
	std::memcpy(_block, block, 12 + remainder_size);
}

/**
 * Construct from anothe ColourSelectedEvent
 *
 * @param other ColourSelectedEvent to copy
 */
ColourSelectedEvent::ColourSelectedEvent(const ColourSelectedEvent &other)
{
	_colour_dbox = other._colour_dbox;
	_none = other._none;
	delete _block;
	_block = 0;
	int remainder_size = *((int *)(other._block + 4));
	_block = new unsigned char[12 + remainder_size];
	//TODO: Should this be 8 + remainder size - in which case what happens to the model field
	// if it's 0.
	std::memcpy(_block, other._block, 12 + remainder_size);
}

/**
 * Make this object to another ColourSelectedEvent object
 *
 * @param other ColourSelectedEvent to copy
 * @returns this object reference
 */
ColourSelectedEvent &ColourSelectedEvent::operator=(const ColourSelectedEvent &other)
{
	_colour_dbox = other._colour_dbox;
	_none = other._none;
	delete _block;
	_block = 0;
	int remainder_size = *((int *)(other._block + 4));
	_block = new unsigned char[12 + remainder_size];
	//TODO: Should this be 8 + remainder size - in which case what happens to the model field
	// if it's 0.
	std::memcpy(_block, other._block, 12 + remainder_size);

    return *this;
}

}
