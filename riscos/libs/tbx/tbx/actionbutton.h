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

#ifndef TBX_ACTIONBUTTON_H
#define TBX_ACTIONBUTTON_H

#include "gadget.h"

namespace tbx
{
	class ButtonSelectedListener;
	class Command;

	/**
	 * ActionButton wrapper for an underlying toolbox ActionButton gadget.
	 *
	 * An ActionButton a gadget that shows a button with text in a Window.
	 */
	class ActionButton : public Gadget
	{
	public:
		enum { TOOLBOX_CLASS = 128 };

		ActionButton() {} //!< Construct an uninitialised display field.

		/**
		 * Construct a action button from another action button.
		 *
		 * Both ActionButton will refer to the same underlying toolbox gadget.
		 */
		ActionButton(const ActionButton &other) : Gadget(other) {}

		/**
		 * Construct a action button from another gadget.
		 *
		 * The ActionButton and the Gadget will refer to the same
		 * underlying toolbox gadget.
		 *
		 * @throws GadgetClassError if the underlying toolbox gadget
		 *         isn't a action button
		 */
		ActionButton(const Gadget &other) : Gadget(other)	{check_toolbox_class(ActionButton::TOOLBOX_CLASS);}

		/**
		 * Construct a action button from a gadget.
		 *
		 * The ActionButton and the Component will refer to the same
		 * underlying toolbox component.
		 */
		ActionButton(const Component &other) : Gadget(other) {Window check(other.handle()); check_toolbox_class(ActionButton::TOOLBOX_CLASS);}

		/**
		 * Assign a action button to refer to the same underlying toolbox gadget
		 * as another.
		 */
		ActionButton &operator=(const ActionButton &other) {_handle = other._handle; _id = other._id; return *this;}

		/**
		 * Assign a action button to refer to the same underlying toolbox gadget
		 * as an existing Gadget.
		 * @throws GadgetClassError if the underlying toolbox gadget
		 *         isn't a action button
		 */
		ActionButton &operator=(const Gadget &other) {_handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

		/**
		 * Assign a action button to refer to the same underlying toolbox component
		 * as an existing Gadget.
		 * @throws ObjectClassError if the component is not in a window.
		 * @throws GadgetClassError if the underlying toolbox gadget
		 *         isn't a action button
		 */
		ActionButton &operator=(const Component &other) {Window check(other.handle()); _handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

		/**
		 * Check if this action button refers to the same underlying toolbox gadget
		 * as another gadget.
		 * @returns true if they refer to the same underlying toolbox gadget.
		 */
		bool operator==(const Gadget &other) const {return (_handle == other.handle() && _id == other.id());}

		/**
		 * Check if this action button refers to the same underlying toolbox gadget
		 * as another gadget.
		 * @returns true if they do not refer to the same underlying toolbox gadget.
		 */
		bool operator!=(const Gadget &other) const {return (_handle != other.handle() || _id != other.id());}

		/**
		 *  Set the the text to display
		 */
		void text(const std::string &value) {string_property(128, value);}

		/**
		 *   Get the the text this is being displayed
		 */
		std::string text() const {return string_property(129);}

		/**
		 *   Get the length of the text property
		 *
		 * @returns Number of characters for this property
		 */
		int text_length() const {return string_property_length(129);}

		/**
		 * Set the event to be raised when this action button is clicked.
		 */
		void event(int code) {int_property(130, code);}

		/**
		 * Get the event that will be raised when this action button is clicked.
		 */
		int event() const {return int_property(130);}

		void click_show(const Object &object, bool transient = false);
		void clear_click_show();
		bool has_click_show() const;
		Object click_show(bool *transient = 0);

		void add_selected_listener(ButtonSelectedListener *listener);
		void remove_selected_listener(ButtonSelectedListener *listener);

		void add_selected_command(Command *command);
		void remove_selected_command(Command *command);

		void add_select_command(Command *command);
		void remove_select_command(Command *command);

		void add_adjust_command(Command *command);
		void remove_adjust_command(Command *command);
	};
}

#endif
