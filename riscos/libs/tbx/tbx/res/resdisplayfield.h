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

#ifndef TBX_RES_RESDISPLAYFIELD_H
#define TBX_RES_RESDISPLAYFIELD_H

#include "resgadget.h"
#include "resjustification.h"

namespace tbx {
namespace res {

/**
 * Class for DisplayField gadget template
 */
class ResDisplayField : public ResGadget
{

public:
	enum {TYPE_ID = 448 };

	/**
	 * Construct an display field gadget resource
	 *
	 * @param other ResGadget to copy resource from
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an display field
	 */
	ResDisplayField(const ResGadget &other) : ResGadget(other)
	{
     check_type(TYPE_ID);
	}

	/**
	 * Construct an display field gadget resource
	 *
	 * @param other display field to copy resource from
	 */
	ResDisplayField(const ResDisplayField &other) : ResGadget(other)
	{
	}

	virtual ~ResDisplayField() {}

	/**
	 * Assign from a ResGadget
	 *
	 * @param other ResGadget to copy
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an display field
	 */
	ResDisplayField &operator=(const ResGadget &other)
	{
		other.check_type(TYPE_ID);
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Assign from another display field gadget resource
	 *
	 * @param other display field gadget resource to copy
	 */
	ResDisplayField &operator=(const ResDisplayField &other)
	{
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Construct a display field gadget resource.
	 *
	 * All options are false, events unset and messages are null.
	 */
	ResDisplayField()
	  : ResGadget(448,44)
	{
		init_message(36,0); // text
	}

	/**
	 * Get the justification for the display field
	 *
	 * @return ResJustication enumeration value
	 */
	ResJustification justification() const {return ResJustification(flag_value(0,6)>>1);}
	/**
	 * Set the justification for the display field
	 *
	 * @param value ResJustication enumeration value
	 */
	void justification(ResJustification value) {flag_value(0,6, (int)value << 1);}

	/**
	 * Get the initial text for the display field
	 *
	 * @returns initial text or 0 for none
	 */
	const char *text() const {return message(36);}
	/**
	 * Set the initial text for the display field
	 *
	 * @param value initial text or 0 for none
	 * @param max_length the maximum length the text will be changed to after
	 * the gadget is created. The default (-1) will use the length of value.
	 */
	void text(const char *value, int max_length = -1) {message_with_length(36, value, max_length);}
	/**
	 * Set the initial text for the display field
	 *
	 * @param value initial text or 0 for none
	 * @param max_length the maximum length the text will be changed to after
	 * the gadget is created. The default (-1) will use the length of value.
	 */
	void text(const std::string &value, int max_length = -1) {message_with_length(36, value, max_length);}
	/**
	 * Get the maximum length for the text
	 *
	 * @returns maximum buffer size for the text.
	 */
	int max_text_len() const {return int_value(40);}
};

}
}

#endif // TBX_RES_RESDISPLAYFIELD_H

