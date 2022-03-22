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

#ifndef TBX_RES_RESRADIOBUTTON_H
#define TBX_RES_RESRADIOBUTTON_H

#include "resgadget.h"

namespace tbx {
namespace res {

/**
 * Class for RadioButton gadget template
 */
class ResRadioButton : public ResGadget
{

public:
	enum {TYPE_ID = 384 };

	/**
	 * Construct an radio button gadget resource
	 *
	 * @param other ResGadget to copy resource from
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an radio button
	 */
	ResRadioButton(const ResGadget &other) : ResGadget(other)
	{
     check_type(TYPE_ID);
	}

	/**
	 * Construct an radio button gadget resource
	 *
	 * @param other radio button to copy resource from
	 */
	ResRadioButton(const ResRadioButton &other) : ResGadget(other)
	{
	}

	virtual ~ResRadioButton() {}

	/**
	 * Assign from a ResGadget
	 *
	 * @param other ResGadget to copy
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an radio button
	 */
	ResRadioButton &operator=(const ResGadget &other)
	{
		other.check_type(TYPE_ID);
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Assign from another radio button gadget resource
	 *
	 * @param other radio button gadget resource to copy
	 */
	ResRadioButton &operator=(const ResRadioButton &other)
	{
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Construct a radio button gadget resource.
	 *
	 * All options are false, events unset and messages are null.
	 */
	ResRadioButton()
	  : ResGadget(384,52)
	{
		init_message(40,0); // label
	}

	/**
	 * Check if state changed event will be generated
	 *
	 * @returns true if the event will be generated
	 */
	bool generate_state_changed() const {return flag(0, 1<<0);}
	/**
	 * Set if state changed event will be generated
	 *
	 * @param value set to true if the event will be generated
	 */
	void generate_state_changed(bool value) {flag(0,1<<0,value);}
	/**
	 * Check if the initial state of the radion button is on
	 *
	 * @return true if the initial state is on
	 */
	bool on() const {return flag(0, 1<<2);}
	/**
	 * Set if the initial state of the radion button is on
	 *
	 * @param value set to true if the initial state should be on
	 */
	void on(bool value) {flag(0,1<<2,value);}

	/**
	 * Get the group number for this radio buttons
	 *
	 * Other buttons in the same group are automatically deselected when
	 * a button in the group is selected.
	 *
	 * @returns group number
	 */
	int group_number() const {return int_value(36);}
	/**
	 * Set the group number for this radio buttons
	 *
	 * Other buttons in the same group are automatically deselected when
	 * a button in the group is selected.
	 *
	 * @param value group number
	 */
	void group_number(int value) {int_value(36,value);}
	/**
	 * Get the label for the radio button
	 *
	 * @returns zero terminated pointer to the message or 0 if none.
	 */
	const char *label() const {return message(40);}
	/**
	 * Set the label for the radio button
	 *
	 * @param value zero terminated pointer to the label or 0 if none.
	 * @param max_length the maximum length the label can be changed to.
	 * -1 (the default) means use the length of value.
	 */
	void label(const char *value, int max_length = -1) {message_with_length(40, value, max_length);}
	/**
	 * Set the label for the radio button
	 *
	 * @param value new label
	 * @param max_length the maximum length the label can be changed to.
	 * -1 (the default) means use the length of value.
	 */
	void label(const std::string &value, int max_length = -1) {message_with_length(40, value, max_length);}
	/**
	 * Get the maximum length allowed for the label
	 */
	int max_label_len() const {return int_value(44);}
	/**
	 * Get the event ID raised when the radio button is selected
	 *
	 * @returns event ID or 0 for the default event
	 */
	int event() const {return int_value(48);}
	/**
	 * Set the event ID raised when the radio button is selected
	 *
	 * @param value event ID or 0 for the default event
	 */
	void event(int value) {int_value(48,value);}

};

}
}

#endif // TBX_RES_RESRADIOBUTTON_H

