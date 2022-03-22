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
 * pointerinfo.h
 *
 *  Created on: 4 Nov 2010
 *      Author: alanb
 */

#ifndef TBX_POINTERINFO_H_
#define TBX_POINTERINFO_H_

#include "gadget.h"

namespace tbx {

/**
 * Class to store and fetch information about the
 * WIMP mouse pointer
 */
class PointerInfo
{
	int _block[5];

public:
	PointerInfo(bool update_now = true, bool get_objects = true);
	PointerInfo(WindowHandle window_handle, IconHandle icon_handle, int dest_x, int dest_y, int buttons);
	virtual ~PointerInfo();

	void Update(bool get_objects = true);

	// Properties
	/**
	 * Get the x mouse coordinate
	 *
	 * @return x mouse position on screen in OS units
	 */
	int mouse_x() const		{return _block[0];}
	/**
	 * Get the y mouse coordinate
	 *
	 * @return y mouse position on screen in OS units
	 */
	int mouse_y() const		{return _block[1];}

	/**
	 * Get the button state.
	 *
	 * @returns the state of the buttons when this was updated
	 *    The bits of the button state are:
	 *       0 adjust button pressed
	 *       1 menu button pressed
	 *       2 select button pressed
	 */
	int button_state() const	{return _block[2];}

	// More details
	/**
	 * Check if the adjust button is pressed
	 *
	 * @returns true if adjust is pressed
	 */
	bool adjust_down() const	{return (_block[2] & 1)!=0;}
	/**
	 * Check if then menu button is pressed
	 *
	 * @returns true if menu is pressed
	 */
	bool menu_down() const		{return (_block[2] & 2)!=0;}
	/**
	 * Check if the select button is pressed
	 *
	 * @returns true if select is pressed
	 */
	bool select_down() const	{return (_block[2] & 4)!=0;}

	// Object/component hands
	/**
	 * Check if pointer is returning object information
	 *
	 * @returns true if pointer is return object information
	 */
	bool on_object() const	{return (_block[2] & 256)!=0;}
	Object object() const;
	Gadget gadget() const;

	// WIMP window/icon handles
	/**
	 * Get the WIMP window handle the pointer was over
	 *
	 * This is only valid if get_objects was false when this was
	 * constructed or last updated
	 *
	 * @returns WIMP window handle
	 *  can also be:
	 *    -1 for desktop background
	 *    -2 for icon bar
	 */
	WindowHandle window_handle() const {return _block[3];}
	/**
	 * Get the WIMP icon handle the pointer was over
	 *
	 * This is only valid if get_objects was false when this was
	 * constructed or last updated
	 *
	 * @returns WIMP icon handle
	 */
	IconHandle icon_handle() const	{return _block[4];}
	/**
	 * Check if the window returned is a user window
	 *
	 * This is only valid if get_objects was false when this was
	 * constructed or last updated
	 *
	 * @returns true if the window is a user window
	 */
	bool user_window() const	{return (_block[3] >= 0);}
	/**
	 * Check if the window returned is the desktop background
	 *
	 * This is only valid if get_objects was false when this was
	 * constructed or last updated
	 *
	 * @returns true if the window return represents the background
	 */
	bool on_background() const {return (_block[3] == -1);}
	/**
	 * Check if the window returned is the iconbar
	 *
	 * This is only valid if get_objects was false when this was
	 * constructed or last updated
	 *
	 * @returns true if the window return represents the iconbar
	 */
	bool on_iconbar() const	{return (_block[3] == -2);}

	/**
	 * Check if this is a system icon

	 * This is only valid if get_objects was false when this was
	 * constructed or last updated
	 *
	 * @returns true if the icon handle represents a system icon
	 */
	bool system_icon() const	{return (_block[4] < 0);}

	// TODO: define enum to give system icon numbers

};

}

#endif /* TBX_POINTERINFO_H_ */
