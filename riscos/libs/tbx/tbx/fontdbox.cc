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
 * fontdbox.cc
 *
 *  Created on: 01-Jul-2010
 *      Author: alanb
 */

#include "fontdbox.h"
#include "abouttobeshownlistener.h"
#include "hasbeenhiddenlistener.h"
#include "swixcheck.h"
#include "tbxexcept.h"
#include "res/resfontdbox.h"

#include <swis.h>

namespace tbx {

FontDbox::FontDbox(const res::ResFontDbox &object_template) : ShowFullObject(object_template) {}

/**
 * Set size of font
 */
void FontDbox::height(int size)
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	// Run Toolbox_ObjectMiscOp
	swix_check(_swix(0x44ec6, _INR(0,3), 1, _handle, 3, size));
}


/**
 * Get size of font
 */
int FontDbox::height() const
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	int value;

	// Run Toolbox_ObjectMiscOp
	swix_check(_swix(0x44ec6, _INR(0,2) | _OUT(0), 1, _handle, 4, &value));

	return value;
}

/**
 * Set aspect ratio of font
 */
void FontDbox::aspect_ratio(int value)
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	// Run Toolbox_ObjectMiscOp
	swix_check(_swix(0x44ec6, _INR(0,3), 2, _handle, 3, value));
}

/**
 * Get aspect ration of font
 */
int FontDbox::aspect_ratio() const
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	int value;

	// Run Toolbox_ObjectMiscOp
	swix_check(_swix(0x44ec6, _INR(0,2) | _OUT(0), 2, _handle, 4, &value));

	return value;
}

/**
 * This event is raised just before the FontDbox underlying window is
 * about to be shown.
 */
void FontDbox::add_about_to_be_shown_listener(AboutToBeShownListener *listener)
{
	add_listener(0x82a00, listener, about_to_be_shown_router);
}

/**
 * Remove about to be shown listener
 */
void FontDbox::remove_about_to_be_shown_listener(AboutToBeShownListener *listener)
{
	remove_listener(0x82a00, listener);
}

/**
 * This event is raised after the FontDbox dialog has been completed
 */
void FontDbox::add_has_been_hidden_listener(HasBeenHiddenListener *listener)
{
	add_listener(0x82a01, listener, has_been_hidden_router);
}

/**
 * Remove has been hidden listener
 */
void FontDbox::remove_has_been_hidden_listener(HasBeenHiddenListener *listener)
{
	remove_listener(0x82a01, listener);
}

static void fontdbox_apply_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	FontDboxApplyEvent ev(id_block, data);
    static_cast<FontDboxApplyListener*>(listener)->fontdbox_apply(ev);
}

/**
 * Add listener for when a font is chosen
 */
void FontDbox::add_apply_listener(FontDboxApplyListener *listener)
{
	add_listener(0x82a02, listener, fontdbox_apply_router);
}

/**
 * Remove listener for when a font is chosen
 */
void FontDbox::remove_apply_listener(FontDboxApplyListener *listener)
{
	remove_listener(0x82a02, listener);
}

}
