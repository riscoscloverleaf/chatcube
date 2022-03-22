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

#ifndef TBX_RES_RESSCROLLLIST_H
#define TBX_RES_RESSCROLLLIST_H

#include "resgadget.h"

namespace tbx {
namespace res {

/**
 * Class for ScrollList gadget template
 */
class ResScrollList : public ResGadget
{

public:
	enum {TYPE_ID = 0x401A };

	/**
	 * Construct an scroll list gadget resource
	 *
	 * @param other ResGadget to copy resource from
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an scroll list
	 */
	ResScrollList(const ResGadget &other) : ResGadget(other)
	{
     check_type(TYPE_ID);
	}

	/**
	 * Construct an scroll list gadget resource
	 *
	 * @param other scroll list to copy resource from
	 */
	ResScrollList(const ResScrollList &other) : ResGadget(other)
	{
	}

	virtual ~ResScrollList() {}

	/**
	 * Assign from a ResGadget
	 *
	 * @param other ResGadget to copy
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an scroll list
	 */
	ResScrollList &operator=(const ResGadget &other)
	{
		other.check_type(TYPE_ID);
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Assign from another scroll list gadget resource
	 *
	 * @param other scroll list gadget resource to copy
	 */
	ResScrollList &operator=(const ResScrollList &other)
	{
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Construct a scroll list gadget resource.
	 *
	 * All options are false, events unset and messages are null.
	 */
	ResScrollList()
	  : ResGadget(0x401A,48)
	{
		background(0xffffff00); // White background
	}


	/**
	 * Get the event returned when an item is selected
	 *
	 * @returns event ID or 0 for the default
	 */
	int event() const {return int_value(36);}
	/**
	 * Set the event returned when an item is selected
	 *
	 * @param value event ID or 0 for the default
	 */
	void event(int value) {int_value(36,value);}
	/**
	 * Get the foreground colour of the scroll list
	 *
	 * @returns foreground/text colour
	 */
	int foreground() const {return int_value(40);}
	/**
	 * Set the foreground colour of the scroll list
	 *
	 * @param value sets the foreground/text colour
	 */
	void foreground(int value) {int_value(40,value);}
	/**
	 * Get the background colour of the scroll list
	 *
	 * @returns background colour
	 */
	int background() const {return int_value(44);}
	/**
	 * Set the background colour of the scroll list
	 *
	 * @param value background colour
	 */
	void background(int value) {int_value(44,value);}

};

}
}

#endif // TBX_RES_RESSCROLLLIST_H

