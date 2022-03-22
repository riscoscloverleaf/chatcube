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
 * optionbutton.h
 *
 *  Created on: 20-Aug-2009
 *      Author: alanb
 */

#ifndef TBX_OPTIONBUTTON_H
#define TBX_OPTIONBUTTON_H

#include "gadget.h"

namespace tbx {

class OptionButtonStateListener;

/**
 * OptionButton wrapper for an underlying toolbox OptionButton gadget.
 *
 * An OptionButton a gadget that shows a check mark and is either on or off.
 */
class OptionButton : public Gadget
{
public:
	enum { TOOLBOX_CLASS = 192 };

	OptionButton() {} //!< Construct an uninitialised option button.

	/**
	 * Construct a option button from another option button.
	 *
	 * Both OptionButton will refer to the same underlying toolbox gadget.
	 */
	OptionButton(const OptionButton &other) : Gadget(other) {}

	/**
	 * Construct a option button from another gadget.
	 *
	 * The OptionButton and the Gadget will refer to the same
	 * underlying toolbox gadget.
	 *
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a option button
	 */
	OptionButton(const Gadget &other) : Gadget(other)	{check_toolbox_class(OptionButton::TOOLBOX_CLASS);}

	/**
	 * Construct a option button from a gadget.
	 *
	 * The OptionButton and the Component will refer to the same
	 * underlying toolbox component.
	 */
	OptionButton(const Component &other) : Gadget(other) {Window check(other.handle()); check_toolbox_class(OptionButton::TOOLBOX_CLASS);}

	/**
	 * Assign a option button to refer to the same underlying toolbox gadget
	 * as another.
	 */
	OptionButton &operator=(const OptionButton &other) {_handle = other._handle; _id = other._id; return *this;}

	/**
	 * Assign a option button to refer to the same underlying toolbox gadget
	 * as an existing Gadget.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a option button
	 */
	OptionButton &operator=(const Gadget &other) {_handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

	/**
	 * Assign a option button to refer to the same underlying toolbox component
	 * as an existing Gadget.
	 * @throws ObjectClassError if the component is not in a window.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a option button
	 */
	OptionButton &operator=(const Component &other) {Window check(other.handle()); _handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

	/**
	 * Check if this option button refers to the same underlying toolbox gadget
	 * as another gadget.
	 * @returns true if they refer to the same underlying toolbox gadget.
	 */
	bool operator==(const Gadget &other) const {return (_handle == other.handle() && _id == other.id());}

	/**
	 * Check if this option button refers to the same underlying toolbox gadget
	 * as another gadget.
	 * @returns true if they do not refer to the same underlying toolbox gadget.
	 */
	bool operator!=(const Gadget &other) const {return (_handle != other.handle() || _id != other.id());}

	/**
	 * Set the the label
	 *
	 * @param value new value for the label
	 */
	void label(const std::string &value) {string_property(192, value);}

	/**
	 * Get the the label
	 *
	 * @returns current value for the label
	 */
	std::string label() const {return string_property(193);}

	/**
	 *  Get the length of the label
	 *
	 * @returns Number of characters for the label buffer
	 */
	int label_length() const {return string_property_length(193);}

	/**
	 * Set the event to be raised when this option button is clicked.
	 *
	 * @param code New event code
	 */
	void event(int code) {int_property(194, code);}

	/**
	 * Get the event that will be raised when this option button is clicked.
	 *
	 * @returns event code
	 */
	int event() const {return int_property(195);}

	/**
	 * Turn option button on or off
	 *
	 * @param value true to turn the option button on
	 */
	void on(bool value) {bool_property(196, value);}

	/**
	 * Check if option button is on
	 *
	 * @returns true if the option button is on
	 */
	bool on() const {return bool_property(197); }

	void add_state_listener(OptionButtonStateListener *listener);
	void remove_state_listener(OptionButtonStateListener *listener);
};

}


#endif /* TBX_OPTIONBUTTON_H_ */
