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


#ifndef TBX_DISPLAYFIELD_H_
#define TBX_DISPLAYFIELD_H_

#include "gadget.h"

namespace tbx {

/**
 * DisplayField wrapper for an underlying toolbox DisplayField gadget.
 *
 * A DisplayField is a gadget that shows text in a Window.
 */
class DisplayField: public tbx::Gadget {
public:
	enum {TOOLBOX_CLASS = 448}; //!< Toolbox class for this gadget.

	DisplayField() {} //!< Construct an uninitialised display field.
	/**
	 * Destroy a Display Field gadget.
	 *
	 * This does not delete the underlying toolbox gadget.
	 */
	~DisplayField() {}

	/**
	 * Construct a display field from another display field.
	 *
	 * Both DisplayField will refer to the same underlying toolbox gadget.
	 */
	DisplayField(const DisplayField &other) : Gadget(other) {}

	/**
	 * Construct a display field from another gadget.
	 *
	 * The DisplayField and the Gadget will refer to the same
	 * underlying toolbox gadget.
	 *
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a display field
	 */
	DisplayField(const Gadget &other) : Gadget(other)	{check_toolbox_class(DisplayField::TOOLBOX_CLASS);}

	/**
	 * Construct a display field from a gadget.
	 *
	 * The DisplayField and the Component will refer to the same
	 * underlying toolbox component.
	 */
	DisplayField(const Component &other) : Gadget(other) {Window check(other.handle()); check_toolbox_class(DisplayField::TOOLBOX_CLASS);}

	/**
	 * Assign a display field to refer to the same underlying toolbox gadget
	 * as another.
	 */
	DisplayField &operator=(const DisplayField &other) {_handle = other._handle; _id = other._id; return *this;}

	/**
	 * Assign a display field to refer to the same underlying toolbox gadget
	 * as an existing Gadget.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a display field
	 */
	DisplayField &operator=(const Gadget &other) {_handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

	/**
	 * Assign a display field to refer to the same underlying toolbox component
	 * as an existing Gadget.
	 * @throws ObjectClassError if the component is not in a window.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a display field
	 */
	DisplayField &operator=(const Component &other) {Window check(other.handle()); _handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

	/**
	 * Check if this display field refers to the same underlying toolbox gadget
	 * as another gadget.
	 * @returns true if they refer to the same underlying toolbox gadget.
	 */
	bool operator==(const Gadget &other) const {return (_handle == other.handle() && _id == other.id());}

	/**
	 * Check if this display field refers to the same underlying toolbox gadget
	 * as another gadget.
	 * @returns true if they do not refer to the same underlying toolbox gadget.
	 */
	bool operator!=(const Gadget &other) const {return (_handle != other.handle() || _id != other.id());}

	/**
	 *  Set the the text to display
	 *
	 *  @param value new value for text
	 */
	void text(const std::string &value) {string_property(448, value);}

	/**
	 * Get the the text this is being displayed
	 *
	 * @returns text of display field
	 */
	std::string text() const {return string_property(449);}

	/**
	 *   Get the length of the text property
	 *
	 *@returns Number of characters for this property
	 */
	int text_length() const {return string_property_length(449);}

	void font(const std::string &name, int width, int height);
	void system_font(int width, int height);
};

}

#endif /* TBX_DISPLAYFIELD_H_ */
