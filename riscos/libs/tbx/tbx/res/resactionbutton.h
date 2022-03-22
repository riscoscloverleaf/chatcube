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

#ifndef TBX_RES_RESACTIONBUTTON_H
#define TBX_RES_RESACTIONBUTTON_H

#include "resgadget.h"

namespace tbx {
namespace res {

/**
 * Class for ActionButton gadget template
 */
class ResActionButton : public ResGadget
{

public:
	enum {TYPE_ID = 128 };

	/**
	 * Construct an action button gadget resource
	 *
	 * @param other ResGadget to copy resource from
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an action button
	 */
	ResActionButton(const ResGadget &other) : ResGadget(other)
	{
     check_type(TYPE_ID);
	}

	/**
	 * Construct an action button gadget resource
	 *
	 * @param other action button to copy resource from
	 */
	ResActionButton(const ResActionButton &other) : ResGadget(other)
	{
	}

	virtual ~ResActionButton() {}

	/**
	 * Assign from a ResGadget
	 *
	 * @param other ResGadget to copy
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an action button
	 */
	ResActionButton &operator=(const ResGadget &other)
	{
		other.check_type(TYPE_ID);
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Assign from another action button gadget resource
	 *
	 * @param other action button gadget resource to copy
	 */
	ResActionButton &operator=(const ResActionButton &other)
	{
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Construct a action button gadget resource.
	 *
	 * All options are false, events unset and messages are null.
	 */
	ResActionButton()
	  : ResGadget(128,52)
	{
		init_message(36,0); // text
		init_string(44,0); // click_show
	}

	/**
	 * Check if this is the default button.
	 *
	 * The default button is displayed slightly differently and
	 * is activated when return is pressed.
	 *
	 * @returns true if this will create a default button
	 */
	bool is_default() const {return flag(0, 1<<0);}
	/**
	 * Set if this is a default button
	 *
	 * The default button is displayed slightly differently and
	 * is activated when return is pressed in the window.
	 *
	 * @param value true to make this button the default button
	 */
	void is_default(bool value) {flag(0,1<<0,value);}
	/**
	 * Check if this is the cancel button
	 *
	 * The cancel button is also activated by pressing escape
	 * in the window.
	 *
	 * @returns true if this is the cancel button
	 */
	bool cancel() const {return flag(0, 1<<1);}
	/**
	 * Set if this is the cancel button
	 *
	 * The cancel button is also activated by pressing escape
	 * in the window.
	 *
	 * @param value true to make this the cancel button
	 */
	void cancel(bool value) {flag(0,1<<1,value);}
	/**
	 * Check if this is a local button
	 *
	 * A local button does not close the window when it is selected,
	 * whereas a non-local button will close the window when selected.
	 *
	 * @return true if this is a local button
	 */
	bool local() const {return flag(0, 1<<2);}
	/**
	 * Set if this is a local button
	 *
	 * A local button does not close the window when it is selected,
	 * whereas a non-local button will close the window when selected.
	 *
	 * @param value true to make this a local button
	 */
	void local(bool value) {flag(0,1<<2,value);}

	/**
	 * Check if the object shown on a click is transient
	 *
	 * @returns true if the shown object is transient.
	 */
	bool click_show_transient() const {return flag(0, 1<<3);}
	/**
	 * Set if the object shown on a click is transient
	 *
	 * @param value true to make the shown object is transient.
	 */
	void click_show_transient(bool value) {flag(0,1<<3,value);}

	/**
	 * Get the text for the button
	 *
	 * @returns zero terminated string.
	 */
	const char *text() const {return message(36);}
	/**
	 * Set the text for the button.
	 *
	 * @param value the text for the button
	 * @param max_length the maximum length the text will be changed to after
	 * the gadget is created. The default (-1) will use the length of value
	 */
	void text(const char *value, int max_length = -1) {message_with_length(36, value, max_length);}
	/**
	 * Set the text for the button.
	 *
	 * @param value the text for the button
	 * @param max_length the maximum length the text will be changed to when
	 * the gadget is created. The default (-1) will use the length of value
	 */
	void text(const std::string &value, int max_length = -1) {message_with_length(36, value, max_length);}
	/**
	 * Get the maximum length for the text
	 *
	 * @returns maximum buffer size for the text.
	 */
	int max_text_len() const {return int_value(40);}
	/**
	 * Get the name of object to show on click
	 *
	 * @returns pointer to zero terminated name of the object to show or 0
	 * if no object will be shown.
	 */
	const char *click_show() const {return string(44);}
	/**
	 * Set the name of the object to show on click
	 *
	 * @param value name of object to show or 0 for no object
	 */
	void click_show(const char *value) {string(44, value);}
	/**
	 * Set the name of the object to show on click
	 *
	 * @param value name of object to show
	 */
	void click_show(const std::string &value) {string(44, value);}
	/**
	 * Get the event number generated by this button
	 *
	 * @returns event number or 0 to generate the default event
	 */
	int event() const {return int_value(48);}
	/**
	 * Set the event number to be generated by this button
	 *
	 * @param value event number or 0 to generate the default event
	 */
	void event(int value) {int_value(48,value);}
};

}
}

#endif // TBX_RES_RESACTIONBUTTON_H

