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
 * scale.cc
 *
 *  Created on: 01-Jul-2010
 *      Author: alanb
 */

#include "scale.h"
#include "abouttobeshownlistener.h"
#include "hasbeenhiddenlistener.h"
#include "swixcheck.h"
#include "tbxexcept.h"
#include "res/resscale.h"

#include <swis.h>

namespace tbx {

Scale::Scale(const res::ResScale &object_template) : ShowFullObject(object_template) {}

/**
 * Set the lower bound for the scale
 *
 * @param value lowest value the scale can be set to
 */
void Scale::lower_bound(int value)
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	// Run Toolbox_ObjectMiscOp
	swix_check(_swix(0x44ec6, _INR(0,3), 1, _handle, 3, value));
}

/**
 * Get the lower bound for the scale
 *
 * @returns lowest value ths scale can be set to
 */
int Scale::lower_bound() const
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	int value;
	// Run Toolbox_ObjectMiscOp
	swix_check(_swix(0x44ec6, _INR(0,2)|_OUT(0), 1, _handle, 4, &value));

	return value;
}

/**
 * Set the upper bound for the scale
 *
 * @param value maximum value for the scale
 */
void Scale::upper_bound(int value)
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	// Run Toolbox_ObjectMiscOp
	swix_check(_swix(0x44ec6, _INR(0,2)|_IN(4), 2, _handle, 3, value));
}

/**
 * Get the upper bound for the scale
 *
 * @returns maximum value for the scale
 */
int Scale::upper_bound() const
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	int value;
	// Run Toolbox_ObjectMiscOp
	swix_check(_swix(0x44ec6, _INR(0,2)|_OUT(1), 2, _handle, 4, &value));

	return value;
}

/**
 * Set the step size for the scale
 *
 * @param value increment for the scale
 */
void Scale::step_size(int value)
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	// Run Toolbox_ObjectMiscOp
	swix_check(_swix(0x44ec6, _INR(0,2)|_IN(5), 4, _handle, 3, value));
}

/**
 * Get the step size for the scale
 *
 * @returns increments used for the scale
 */
int Scale::step_size() const
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	int value;
	// Run Toolbox_ObjectMiscOp
	swix_check(_swix(0x44ec6, _INR(0,2)|_OUT(2), 4, _handle, 4, &value));

	return value;
}

/**
 * This event is raised just before the Scale underlying window is
 * about to be shown.
 *
 * @param listener listener to add
 */
void Scale::add_about_to_be_shown_listener(AboutToBeShownListener *listener)
{
	add_listener(0x82c00, listener, about_to_be_shown_router);
}

/**
 * Remove about to be shown listener
 *
 * @param listener listener to remove
 */
void Scale::remove_about_to_be_shown_listener(AboutToBeShownListener *listener)
{
	remove_listener(0x82c00, listener);
}

/**
 * This event is raised after the Scale dialog has been completed
 *
 * @param listener listener to add
 */
void Scale::add_has_been_hidden_listener(HasBeenHiddenListener *listener)
{
	add_listener(0x82c01, listener, has_been_hidden_router);
}

/**
 * Remove has been hidden listener
 *
 * @param listener listener to remove
 */
void Scale::remove_has_been_hidden_listener(HasBeenHiddenListener *listener)
{
	remove_listener(0x82c01, listener);
}

static void scale_apply_factor_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	ScaleApplyFactorEvent ev(id_block, data);
    static_cast<ScaleApplyFactorListener*>(listener)->scale_apply_factor(ev);
}


/**
 * Add listener for save button on print dialogue
 *
 * @param listener listener to add
 */
void Scale::add_apply_factor_listener(ScaleApplyFactorListener *listener)
{
	add_listener(0x82c02, listener, scale_apply_factor_router);
}

/**
 * Remove listener for save button on print dialogue
 *
 * @param listener listener to remove
 */
void Scale::remove_apply_factor_listener(ScaleApplyFactorListener *listener)
{
	remove_listener(0x82c02, listener);
}

}
