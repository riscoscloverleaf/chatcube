/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2021 Alan Buckley   All Rights Reserved.
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

#include "caret.h"
#include "kernel.h"

namespace tbx
{

/**
 * Create an uninitialised caret.
 *
 * Do not call set until get is called or the fields are set.
 *
 * @params get_current true to read in the current caret data, false to leave uninitialised (default false).
 */
Caret::Caret(bool get_current /*= false*/)
{
	if (get_current) get();
	else
	{
		_window = -1;
		_icon = -1;
		_x_offset = 0;
		_y_offset = 0;
		_height_and_flags = 0;
		_index = 0;
	}
}

/**
 * Construct from an existing caret
 */
Caret::Caret(const Caret &other)
{
	_window = other._window;
	_icon = other._icon;
	_x_offset = other._x_offset;
	_y_offset = other._y_offset;
	_height_and_flags = other._height_and_flags;
	_index = other._index;
}

/**
 * Assigning using the raw fields.
 *
 * This is provided to allow everything to be set. Use one of the
 * other constructors for a simpler interfaces.
 *
 * Call the set() method to update the WIMP caret.
 *
 * @param window WIMP window handle (or -1 to disown the caret)
 * @param icon WIMP icon handle (or -1 for focus to the window not an icon)
 * @param x x work area offset of the caret.
 * @param y y work area offset of the caret.
 * @param height_and_flags The bottom 16 bits are the height (see the Programmers Reference Manual for the flags).
 * @param index index into the string on a writable field (or -1 if not on a writable field)
 */
Caret::Caret(WindowHandle window, IconHandle icon, int x, int y, int height_and_flags, int index) :
  _window(window),
  _icon(icon),
  _x_offset(x),
  _y_offset(y),
  _height_and_flags(height_and_flags),
  _index(index)
{
}

/**
 * Construct an invisible caret for the given toolbox window.
 *
 * Call the set() method to update the WIMP caret.
 * @param window Window the caret will be shown in
 */
Caret::Caret(tbx::Window window) :
  _window(window.window_handle()),
  _icon(-1),
  _x_offset(0),
  _y_offset(0),
  _height_and_flags(0 | (1 << 25)), // invisible flag
  _index(0)
{
	
}

/**
 * Construct an invisible caret for the given toolbox window.
 *
 * Call the set() method to update the WIMP caret.
 * @param window Window the caret will be shown in
 * @param x x work area offset of the caret.
 * @param y y work area offset of the caret.
 */
Caret::Caret(tbx::Window window, int x, int y) :
  _window(window.window_handle()),
  _icon(-1),
  _x_offset(x),
  _y_offset(y),
  _height_and_flags(0 | (1 << 25)), // invisible flag
  _index(0)
{
}

/**
 * Construct a visible caret of the given height using the standard wimp colours.
 *
 * Call the set() method to update the WIMP caret.
 * @param window Window the caret will be shown in
 * @param x x work area offset of the caret.
 * @param y y work area offset of the caret.
 * @param height height of the caret in OS units
 * @param vdu true to use a VDU 5 style caret, false (the default) to use the standard anti-aliased caret.
 */
Caret::Caret(tbx::Window window, int x, int y, int height, bool vdu /*= false*/) :
  _window(window.window_handle()),
  _icon(-1),
  _x_offset(x),
  _y_offset(y),
  _height_and_flags(height),
  _index(0)
{
	if (vdu) _height_and_flags |= (1<<24);
}

/**
 * Construct a visible caret of the given height using the given colours.
 *
 * Call the set() method to update the WIMP caret.
 * @param window Window the caret will be shown in
 * @param x x work area offset of the caret.
 * @param y y work area offset of the caret.
 * @param height height of the caret in OS units.
 * @param colour the colour for the cursor.
 * @param untranslated true to use the colour number directly, false (the default) to use the equivalent WIMP colour.
 * @param vdu true to use a VDU 5 style caret, false (the default) to use the standard anti-aliased caret.
 */
Caret::Caret(tbx::Window window, int x, int y, int height, int colour, bool untranslated /*= false*/, bool vdu /*= false*/) :
  _window(window.window_handle()),
  _icon(-1),
  _x_offset(x),
  _y_offset(y),
  _height_and_flags(height),
  _index(0)
{
	_height_and_flags = (colour &0xFF) << 16;
	if (vdu) _height_and_flags |= (1<<24);
	_height_and_flags |= (1<<26); // Use colour flag
	if (untranslated) _height_and_flags |= (1<<27);
}

/**
 * Assignment operator
 */
Caret& Caret::operator=(const Caret &other)
{
	_window = other._window;
	_icon = other._icon;
	_x_offset = other._x_offset;
	_y_offset = other._y_offset;
	_height_and_flags = other._height_and_flags;
	_index = other._index;
	return *this;
}

/**
 * Update this object with the current location and state of the WIMP caret.
 */
void Caret::get()
{
	_kernel_swi_regs regs;
	regs.r[1] = reinterpret_cast<int>(&_window);

	_kernel_swi(0x400D3, &regs, &regs);
}

/**
 * Update the WIMP caret location and appearance to the settings in this class.
 */
void Caret::set()
{
	_kernel_swi_regs regs;
	regs.r[0] = _window;
	regs.r[1] = _icon;
	regs.r[2] = _x_offset;
	regs.r[3] = _y_offset;
	regs.r[4] = _height_and_flags;
	regs.r[5] = _index;
	
	_kernel_swi(0x400D2, &regs, &regs);
}

/**
 * Move the caret class and the WIMP caret to a new location
 *
 * @param x new work area x-offset
 * @param y new work area y-offset
 */
void Caret::move(int x, int y)
{
	_x_offset = x;
	_y_offset = y;
	set();
}

/**
 * Turn off and disown the caret.
 */
void Caret::turn_off()
{
	_kernel_swi_regs regs;
	regs.r[0] = -1;	
	_kernel_swi(0x400D2, &regs, &regs);
}
		

}
