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

#include "showpointobject.h"
#include "swixcheck.h"
#include <swis.h>

using namespace tbx;
/**
 * Show object with given parent object
 */
void ShowPointObject::show(const Object &parent)
{
	swix_check(_swix(0x44EC3, _INR(0,5),
			0,
			_handle,
			0,
			0,
			parent.handle(),
			-1));
}

/**
 * Show object with given parent component
 */
void ShowPointObject::show(const Component &parent)
{
	swix_check(_swix(0x44EC3, _INR(0,5),
			0,
			_handle,
			0,
			0,
			parent.handle(),
			parent.id()));

}

/**
 * Show object with top left at the given position
 */
void ShowPointObject::show(const Point &pos)
{
	swix_check(_swix(0x44EC3, _INR(0,5),
			0,
			_handle,
			2,
			&(pos.x),
			0,
			-1));
}
/**
 * Show object with top left at the given position
 */
void ShowPointObject::show(const Point &pos, const Object &parent)
{
	swix_check(_swix(0x44EC3, _INR(0,5),
			0,
			_handle,
			2,
			&(pos.x),
			parent.handle(),
			-1));
}
/**
 * Show object with top left at the given position
 */
void ShowPointObject::show(const Point &pos, const Component &parent)
{
	swix_check(_swix(0x44EC3, _INR(0,5),
			0,
			_handle,
			2,
			&(pos.x),
			parent.handle(),
			parent.id()));
}

/**
 * Show object as a menu at default position.
 */
void ShowPointObject::show_as_menu()
{
	swix_check(_swix(0x44EC3, _INR(0,5),
		1,
		_handle,
		0,
		0,
		0,
		-1));
}

/**
 * Show object as a menu with the given parent.
 */
void ShowPointObject::show_as_menu(const Object &parent)
{
	swix_check(_swix(0x44EC3, _INR(0,5),
		1,
		_handle,
		0,
		0,
		parent.handle(),
		-1));
}

/**
 * Show object as a menu with the given parent.
 */
void ShowPointObject::show_as_menu(const Component &parent)
{
	swix_check(_swix(0x44EC3, _INR(0,5),
			1,
			_handle,
			0,
			0,
			parent.handle(),
			parent.id()));
}


/**
 * Show object as a menu at the given point with the given parent.
 */
void ShowPointObject::show_as_menu(const Point &pos)
{
	swix_check(_swix(0x44EC3, _INR(0,5),
		1,
		_handle,
		2,
		&(pos.x),
		0,
		-1));
}

/**
 * Show object as a menu at the given point with the given parent.
 */
void ShowPointObject::show_as_menu(const Point &pos, const Object &parent)
{
	swix_check(_swix(0x44EC3, _INR(0,5),
		1,
		_handle,
		2,
		&(pos.x),
		parent.handle(),
		-1));
}

/**
 * Show object as a menu at the given point with the given parent.
 */
void ShowPointObject::show_as_menu(const Point &pos, const Component &parent)
{
	swix_check(_swix(0x44EC3, _INR(0,5),
		1,
		_handle,
		2,
		&(pos.x),
		parent.handle(),
		parent.id()));
}

/**
 * Show object as a sub menu from the given menu item
 */
void ShowPointObject::show_as_submenu(const MenuItem &parent)
{
	swix_check(_swix(0x44EC3, _INR(0,5),
			2,
			_handle,
			0,
			0,
			parent.handle(),
			parent.id()));
}

/**
 * Show object as a sub menu at the given point from the given menu item
 */
void ShowPointObject::show_as_submenu(const Point &pos, const MenuItem &parent)
{
	swix_check(_swix(0x44EC3, _INR(0,5),
		2,
		_handle,
		2,
		&(pos.x),
		parent.handle(),
		parent.id()));
}
