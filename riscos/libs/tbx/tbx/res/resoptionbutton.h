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

#ifndef TBX_RES_RESOPTIONBUTTON_H
#define TBX_RES_RESOPTIONBUTTON_H

#include "resgadget.h"

namespace tbx {
namespace res {

/**
 * Class for OptionButton gadget template
 */
class ResOptionButton : public ResGadget
{

public:
	enum {TYPE_ID = 192 };

	/**
	 * Construct an option button gadget resource
	 *
	 * @param other ResGadget to copy resource from
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an option button
	 */
	ResOptionButton(const ResGadget &other) : ResGadget(other)
	{
     check_type(TYPE_ID);
	}

	/**
	 * Construct an option button gadget resource
	 *
	 * @param other option button to copy resource from
	 */
	ResOptionButton(const ResOptionButton &other) : ResGadget(other)
	{
	}

	virtual ~ResOptionButton() {}

	/**
	 * Assign from a ResGadget
	 *
	 * @param other ResGadget to copy
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an option button
	 */
	ResOptionButton &operator=(const ResGadget &other)
	{
		other.check_type(TYPE_ID);
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Assign from another option button gadget resource
	 *
	 * @param other option button gadget resource to copy
	 */
	ResOptionButton &operator=(const ResOptionButton &other)
	{
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Construct a option button gadget resource.
	 *
	 * All options are false, events unset and messages are null.
	 */
	ResOptionButton()
	  : ResGadget(192,48)
	{
		init_message(36,0); // label
	}

	/**
	 * Check if state change message will be generated
	 *
	 * @returns true if state change message will be generated
	 */
	bool generate_state_changed() const {return flag(0, 1<<0);}
	/**
	 * Set if state change message will be generated
	 *
	 * @param value set to true if state change message should be generated
	 */
	void generate_state_changed(bool value) {flag(0,1<<0,value);}
	/**
	 * Check if option is on
	 *
	 * @returns true if option is on
	 */
	bool on() const {return flag(0, 1<<2);}
	/**
	 * Set state of option button
	 *
	 * @param value true to turn the option on
	 */
	void on(bool value) {flag(0,1<<2,value);}

	/**
	 * Get the label for the option button
	 *
	 * @returns pointer to zero terminated label text or 0 if none
	 */
	const char *label() const {return message(36);}
	/**
	 * Set the label for the option button
	 *
	 * @param value pointer to zero terminated label text or 0 if none
	 * @param max_length maximum length the label text can be changed to
	 * or -1 (the default) to use the length of value.
	 */
	void label(const char *value, int max_length = -1) {message_with_length(36, value, max_length);}
	/**
	 * Set the label for the option button
	 *
	 * @param value pointer to zero terminated label text
	 * @param max_length maximum length the label text can be changed to
	 * or -1 (the default) to use the length of value.
	 */
	void label(const std::string &value, int max_length = -1) {message_with_length(36, value, max_length);}
	/**
	 * Get the maximum length of the label text
	 *
	 * @returns maximum label text length
	 */
	int max_label_len() const {return int_value(40);}
	/**
	 * Get the event ID generated when the state changes
	 *
	 * @returns event ID set by user or 0 for the default event
	 */
	int event_id() const {return int_value(44);}
	/**
	 * Set the event ID generated when the state changes
	 *
	 * @param value event ID set by user or 0 for the default event
	 */
	void event_id(int value) {int_value(44,value);}
};

}
}

#endif // TBX_RES_RESOPTIONBUTTON_H

