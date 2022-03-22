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

#ifndef TBX_RES_RESWRITABLEFIELD_H
#define TBX_RES_RESWRITABLEFIELD_H

#include "resgadget.h"
#include "resjustification.h"
#include "../handles.h"

namespace tbx {
namespace res {

/**
 * Class for WritableField gadget template
 */
class ResWritableField : public ResGadget
{

public:
	enum {TYPE_ID = 512 };

	/**
	 * Construct an writable field gadget resource
	 *
	 * @param other ResGadget to copy resource from
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an writable field
	 */
	ResWritableField(const ResGadget &other) : ResGadget(other)
	{
     check_type(TYPE_ID);
	}

	/**
	 * Construct an writable field gadget resource
	 *
	 * @param other writable field to copy resource from
	 */
	ResWritableField(const ResWritableField &other) : ResGadget(other)
	{
	}

	virtual ~ResWritableField() {}

	/**
	 * Assign from a ResGadget
	 *
	 * @param other ResGadget to copy
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an writable field
	 */
	ResWritableField &operator=(const ResGadget &other)
	{
		other.check_type(TYPE_ID);
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Assign from another writable field gadget resource
	 *
	 * @param other writable field gadget resource to copy
	 */
	ResWritableField &operator=(const ResWritableField &other)
	{
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Construct a writable field gadget resource.
	 *
	 * All options are false, events unset and messages are null.
	 */
	ResWritableField()
	  : ResGadget(512,60)
	{
		init_message(36,0); // text
		init_message(44,0); // allowable
		before(-1);
		after(-1);
	}

	/**
	 * Check if the value changed event will be generated
	 *
	 * @returns true if the event will be generated
	 */
	bool generate_value_changed() const {return flag(0, 1<<0);}
	/**
	 * Set if the value changed event will be generated
	 *
	 * @param value set to true to generate the event
	 */
	void generate_value_changed(bool value) {flag(0,1<<0,value);}
	/**
	 * Get justification of the value
	 *
	 * @returns ResJustification value specifying justification
	 */
	ResJustification justification() const {return ResJustification(flag_value(0, 12)>>2);}
	/**
	 * Set justification of the value
	 *
	 * @param value ResJustification value specifying justification
	 */
	void justification(ResJustification value) {flag_value(0,12,(int)value<<2);}
	/**
	 * Check if the text should not be displayed
	 *
	 * This is used for a password type field
	 *
	 * @returns true if the text should not be displayed
	 */
	bool do_not_display_text() const {return flag(0, 1<<4);}
	/**
	 * Set if the text should not be displayed
	 *
	 * This is used for a password type field
	 *
	 * @param value set to true if the text should not be displayed
	 */
	void do_not_display_text(bool value) {flag(0,1<<4,value);}

	/**
	 * Get the initially text for the writable field
	 *
	 * @returns pointer to zero terminated string or 0 if none
	 */
	const char *text() const {return message(36);}
	/**
	 * Set the initially text for the writable field
	 *
	 * @param value pointer to zero terminated string or 0 if none
	 * @param max_length The maximum length the text can be increased
	 * to when using the string set or -1 (the default) to use the length of value.
	 */
	void text(const char *value, int max_length = -1) {message_with_length(36, value, max_length);}
	/**
	 * Set the initially text for the writable field
	 *
	 * @param value initial text
	 * @param max_length The maximum length the text can be increased
	 * to when using the string set or -1 (the default) to use the length of value.
	 */
	void text(const std::string &value, int max_length = -1) {message_with_length(36, value, max_length);}
	/**
	 * Get the maximum length for the writable text
	 */
	int max_text_len() const {return int_value(40);}

	/**
	 * Get the allowable characters in a writable field
	 *
	 * @see tbx::WritableField::allowable()
	 *
	 * @returns pointer to zero terminated string of allowable characters or 0
	 * if there is no restriction
	 */
	const char *allowable() const {return message(44);}
	/**
	 * Set the allowable characters in a writable field
	 *
	 * @see tbx::WritableField::allowable()
	 *
	 * @param value pointer to zero terminated string of allowable characters or 0
	 * if there is no restriction
	 * @param max_length maximum length the allowable character can be changed
	 * to when the gadget is shown or -1 (the default) for the length of
	 * value.
	 */
	void allowable(const char *value, int max_length = -1) {message_with_length(44, value, max_length);}
	/**
	 * Set the allowable characters in a writable field
	 *
	 * @see tbx::WritableField::allowable()
	 *
	 * @param value string of allowable characters
	 * @param max_length maximum length the allowable character can be changed
	 * to when the gadget is shown or -1 (the default) for the length of
	 * value.
	 */
	void allowable(const std::string &value, int max_length = -1) {message_with_length(44, value, max_length);}
	/**
	 * Get the maximum length the allowable character can be changed
	 * to when the gadget is shown.
	 */
	int max_allowable_len() const {return int_value(48);}
	/**
	 * Get the component ID of the gadget before this one in the Tab order
	 *
	 * @returns component ID of previous gadget
	 */
	ComponentId before() const {return ComponentId(int_value(52));}
	/**
	 * Set the component ID of the gadget before this one in the Tab order
	 *
	 * @param value component ID of previous gadget
	 */
	void before(ComponentId value) {int_value(52,(int)value);}
	/**
	 * Get the component ID of the gadget after this one in the Tab order
	 *
	 * @returns component ID of next gadget
	 */
	ComponentId after() const {return ComponentId(int_value(56));}
	/**
	 * Set the component ID of the gadget after this one in the Tab order
	 *
	 * @param value component ID of next gadget
	 */
	void after(ComponentId value) {int_value(56,(int)value);}

};

}
}

#endif // TBX_RES_RESWRITABLEFIELD_H

