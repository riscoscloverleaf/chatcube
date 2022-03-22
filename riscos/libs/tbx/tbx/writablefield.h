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
 * writablefield.h
 *
 *  Created on: 18-Sep-2008
 *      Author: alanb
 */

#ifndef TBX_WRITEABLEFIELD_H_
#define TBX_WRITEABLEFIELD_H_

#include "gadget.h"

namespace tbx {

class TextChangedListener;
class KeyListener;

/**
 * WritableField wrapper class for an underlying toolbox WritableField gadget.
 *
 * A WritableField is a gadget that allows a line of text to be entered
 * in a Window.
 */

class WritableField: public tbx::Gadget {
public:
	enum {TOOLBOX_CLASS = 512}; //!< Toolbox class for this gadget.

	WritableField() {} //!< Construct an uninitialised display field.

	/**
	 * Destroy a Writable Field gadget.
	 *
	 * This does not delete the underlying toolbox gadget.
	 */
	~WritableField() {}

	/**
	 * Construct a writable field from another writableField.
	 *
	 * Both WritableField will refer to the same underlying toolbox gadget.
	 */
	WritableField(const WritableField &other) : Gadget(other) {}

	/**
	 * Construct a writable field from another gadget.
	 *
	 * The WritableField and the Gadget will refer to the same
	 * underlying toolbox gadget.
	 *
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a writable field
	 */
	WritableField(const Gadget &other) : Gadget(other)	{check_toolbox_class(WritableField::TOOLBOX_CLASS);}

	/**
	 * Construct a writable field from a Component.
	 *
	 * @throws ObjectClassError if the component is not in a window.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a writable field
	 */
	WritableField(const Component &other) : Gadget(other) {}

	/**
	 * Assign a writable field to refer to the same underlying toolbox gadget
	 * as another.
	 */
	WritableField &operator=(const WritableField &other) {_handle = other.handle(); _id = other._id; return *this;}

	/**
	 * Assign a writable field to refer to the same underlying toolbox gadget
	 * as an existing Gadget.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a display field
	 */
	WritableField &operator=(const Gadget &other) {_handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

	/**
	 * Assign a writable field to refer to the same underlying toolbox component
	 * as an existing Gadget.
	 * @throws ObjectClassError if the component is not in a window.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a writable field
	 */
	WritableField &operator=(const Component &other) {Window check(other.handle()); _handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

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
	 * @param value new text to show in the writable field
	 */
	void text(const std::string &value) {string_property(512, value);}

	/**
	 * Get the the text that is being displayed
	 */
	std::string text() const {return string_property(513);}

	/**
	 *   Get the length of the text property
	 *
	 * @returns Number of characters for this property
	 */
	int text_length() const {return string_property_length(513);}

	/**
	 * Set the characters that are allowed to be entered into a writable field.
	 *
	 * allow-spec ::= { char-spec }* { ~ { char-spec }* }*
     * char-spec ::= char | char-char
     * char ::= \- | \; | \\ | \~ | any character other than - ;
     *
	 * Each char-spec in the 'allow' string specifies a character or range
	 * of characters; the ~ character toggles whether they are included or
	 * excluded from the icon text string:
	 *
	 * e.g. 0-9a-z~dpu   allows the digits 0 - 9 and the lower-case letters
	 * a - z, except for 'd', 'p' and 'u'
	 *
	 * If the first character following the A command is a ~ all normal
	 * characters are initially included:
	 *
	 * e.g. ~0-9   allows all characters except for the digits 0 - 9
	 *
	 * If you use any of the four special characters - ; ~ \ in a char-spec
	 * you must precede them with a backslash \:
	 *
	 * e.g. ~\-\;\~\\ allows all characters except the four special ones
	 * - ; ~ \
	 *
	 * @param value string specifying what character can be entered into the list box.
	 */
	void allowable(const std::string &value) {string_property(514, value);}
	/**
	 * Get the pattern that specifies the character that can be entered into this field
	 *
	 * @return string specification of characters allowed to be entered or "" if no restriction.
	 */
	std::string allowable() const {return string_property(515);}
	/**
	 * Get the length of the allowable pattern
	 */
	int allowable_length() const {return string_property_length(515);}

	void font(const std::string &name, int width, int height);
	void system_font(int width, int height);

	// Listeners
	void add_text_changed_listener(TextChangedListener *listener);
	void remove_text_changed_listener(TextChangedListener *listener);

    void add_key_listener(KeyListener *listener);
	void remove_key_listener(KeyListener *listener);
};

}

#endif /* TBX_WRITEABLEFIELD_H_ */
