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
 * stringset.h
 *
 *  Created on: 28-Nov-2008
 *      Author: alanb
 */

#ifndef TBX_STRINGSET_H_
#define TBX_STRINGSET_H_

#include "gadget.h"
#include "menu.h"
#include "listener.h"
#include "eventinfo.h"

namespace tbx {

class TextChangedListener;
class StringSetAboutToBeShownListener;


/**
 * StringSet wrapper class for an underlying toolbox StringSet gadget.
 *
 * A StringSet is a gadget that a value to be entered and/or chosen from
 * a list.
 *
 * The list is a shown in a popup menu.
 */
class StringSet: public tbx::Gadget
{
public:
	enum {TOOLBOX_CLASS = 896}; //!< Toolbox class for this gadget.

	StringSet() {} //!< Construct an uninitialised display field.

	/**
	 * Destroy a string set  gadget.
	 *
	 * This does not delete the underlying toolbox gadget.
	 */
	~StringSet() {}

	/**
	 * Construct a string set from another string set.
	 *
	 * Both string sets will refer to the same underlying toolbox gadget.
	 */
	StringSet(const StringSet &other) : Gadget(other) {}

	/**
	 * Construct a string set from another gadget.
	 *
	 * The StringSet and the Gadget will refer to the same
	 * underlying toolbox gadget.
	 *
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a string set
	 */
	StringSet(const Gadget &other) : Gadget(other)	{check_toolbox_class(StringSet::TOOLBOX_CLASS);}

	/**
	 * Construct a string set from a Component.
	 *
	 * @throws ObjectClassError if the component is not in a window.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a string set
	 */
	StringSet(const Component &other) : Gadget(other) {}

	/**
	 * Assign a string set to refer to the same underlying toolbox gadget
	 * as another.
	 */
	StringSet &operator=(const StringSet &other) {_handle = other.handle(); _id = other._id; return *this;}

	/**
	 * Assign a string set to refer to the same underlying toolbox gadget
	 * as an existing Gadget.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a string set
	 */
	StringSet &operator=(const Gadget &other) {_handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

	/**
	 * Assign a string set field to refer to the same underlying toolbox component
	 * as an existing Gadget.
	 * @throws ObjectClassError if the component is not in a window.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a string set
	 */
	StringSet &operator=(const Component &other) {Window check(other.handle()); _handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

	/**
	 * Check if this string set refers to the same underlying toolbox gadget
	 * as another gadget.
	 * @returns true if they refer to the same underlying toolbox gadget.
	 */
	bool operator==(const Gadget &other) const {return (_handle == other.handle() && _id == other.id());}

	/**
	 * Check if this string set refers to the same underlying toolbox gadget
	 * as another gadget.
	 * @returns true if they do not refer to the same underlying toolbox gadget.
	 */
	bool operator!=(const Gadget &other) const {return (_handle != other.handle() || _id != other.id());}

	/**
	 * Set the list of strings available.
	 *
	 * The list is separated by comma's. If a comma is required in the data
	 * it must be escaped out by proceeding by a "\".
	 */
	void available(const std::string &value) {string_property(896, value);}

	/**
	 * Set the string to be selected.
	 */
	void selected(const std::string &value) {string_property(898, value);}

	/**
	 * Get the currently selected string.
	 */
	std::string selected() const {return string_property(899);}

	/**
	 * Set the string to be selected by it's position in
	 * the available list.
	 */
	void selected_index(int index);

	/**
	 * Get the index of the selected string in
	 * the available list.
	 */
	int selected_index() const;

	/**
	 * Set the characters that are allowed to be entered into a string set.
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
	void allowable(const std::string &value) {string_property(900, value);}

	Gadget alphanumeric_gadget() const;
	Menu popup_menu() const;

	// Listeners
	void add_text_changed_listener(TextChangedListener *listener);
	void remove_text_changed_listener(TextChangedListener *listener);

	void add_about_to_be_shown_listener(StringSetAboutToBeShownListener *listener);
	void remove_about_to_be_shown_listener(StringSetAboutToBeShownListener *listener);

};

/**
 * Listener for the string set about to be shown event
 */
class StringSetAboutToBeShownListener : public Listener
{
public:
	StringSetAboutToBeShownListener() {}
	virtual ~StringSetAboutToBeShownListener() {}

	/**
	 * Method called just before the popup menu for a string set is shown
	 *
	 * @param event information on the objects/components for this event.
	 */
	virtual void stringset_about_to_be_shown(const EventInfo &event) = 0;
};

}

#endif /* TBX_STRINGSET_H_ */
