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

#ifndef TBX_RES_RESLABELLEDBOX_H
#define TBX_RES_RESLABELLEDBOX_H

#include "resgadget.h"

namespace tbx {
namespace res {

/**
 * Class for LabelledBox gadget template
 */
class ResLabelledBox : public ResGadget
{

public:
	enum {TYPE_ID = 256 };

	/**
	 * Construct an labelled box gadget resource
	 *
	 * @param other ResGadget to copy resource from
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an labelled box
	 */
	ResLabelledBox(const ResGadget &other) : ResGadget(other)
	{
     check_type(TYPE_ID);
	}

	/**
	 * Construct an labelled box gadget resource
	 *
	 * @param other labelled box to copy resource from
	 */
	ResLabelledBox(const ResLabelledBox &other) : ResGadget(other)
	{
	}

	virtual ~ResLabelledBox() {}

	/**
	 * Assign from a ResGadget
	 *
	 * @param other ResGadget to copy
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an labelled box
	 */
	ResLabelledBox &operator=(const ResGadget &other)
	{
		other.check_type(TYPE_ID);
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Assign from another labelled box gadget resource
	 *
	 * @param other labelled box gadget resource to copy
	 */
	ResLabelledBox &operator=(const ResLabelledBox &other)
	{
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Construct a labelled box gadget resource.
	 *
	 * All options are false except the back option and messages are null.
	 */
	ResLabelledBox()
	  : ResGadget(256,40)
	{
		back(true);
		init_message(36,0); // label
	}

	/**
	 * Get the text displayed in the labelled box
	 *
	 * @returns pointer to zero terminated label text or 0 if none
	 */
	const char *label() const {return message(36);}
	/**
	 * Set the text displayed in the labelled box
	 *
	 * @param value pointer to zero terminated label text or 0 for none
	 */
	void label(const char *value) {message(36, value);}
	/**
	 * Set the text displayed in the labelled box
	 *
	 * @param value label text
	 */
	void label(const std::string &value) {message(36, value);}

};

}
}

#endif // TBX_RES_RESLABELLEDBOX_H

