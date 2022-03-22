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

#ifndef TBX_RES_RESADJUSTER_H
#define TBX_RES_RESADJUSTER_H

#include "resgadget.h"

namespace tbx {
namespace res {

/**
 * Class for Adjuster gadget template
 */
class ResAdjuster : public ResGadget
{
public:
  enum {TYPE_ID = 768 };
	/**
	 * Construct an adjuster gadget resource
	 *
	 * @param other ResGadget to copy resource from
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an adjuster
	 */
	ResAdjuster(const ResGadget &other) : ResGadget(other)
	{
   check_type(TYPE_ID);
	}

	/**
	 * Construct an adjuster gadget resource
	 *
	 * @param other adjuster to copy resource from
	 */
	ResAdjuster(const ResAdjuster &other) : ResGadget(other)
	{
	}

	virtual ~ResAdjuster() {}

	/**
	 * Assign from a ResGadget
	 *
	 * @param other ResGadget to copy
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an adjuster
	 */
	ResAdjuster &operator=(const ResGadget &other)
	{
		other.check_type(TYPE_ID);
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Assign from another adjuster gadget resource
	 *
	 * @param other adjuster gadget resource to copy
	 */
	ResAdjuster &operator=(const ResAdjuster &other)
	{
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Construct a adjuster gadget resource.
	 *
	 * All options are false, events unset and messages are null.
	 */
	ResAdjuster()
    : ResGadget(768,40)
    {
		// seems to have an unused reserved word at 36
	}

	/**
	 * Check if adjustor is increment adjustor
	 *
	 * @returns true if increment, false if decrement
	 */
	bool increment() const {return flag(0,1);}
	/**
	 * Set if adjustor is increment adjustor
	 *
	 * @param value true for increment, false for decrement
	 */
	void increment(bool value) {flag(0,1, value);}

	/**
	 * Check adjuster orientation.
	 *
	 * @returns true if vertical (i.e. one of an up/down pair)
	 * false if horizontal (i.e. one of a left/right pair)
	 */
	bool vertical() const {return flag(0,2);}
	/**
	 * Set adjuster orientation.
	 *
	 * @param value true to make a vertical adjuster (i.e. one of an up/down pair)
	 * false to make a horizontal adjuster (i.e. one of a left/right pair)
	 */
	void vertical(bool value) {flag(0,2, value);}
};

}
}

#endif // TBX_RES_RESADJUSTER_H

