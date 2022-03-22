/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2020 Alan Buckley   All Rights Reserved.
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

#ifndef TBX_RES_RESTABS_H
#define TBX_RES_RESTABS_H

#include "tbx/res/resgadget.h"

namespace tbx {
namespace ext {
namespace res {

/**
 * Class for Tabs gadget template
 */
class ResTabs : public tbx::res::ResGadget
{

public:
	enum {TYPE_ID = 0x402c };

	/**
	 * Construct a Tabs gadget resource
	 *
	 * @param other ResGadget to copy resource from
	 * @throws ResGadgetTypeMismatch if the ResGadget is not a tabs
	 */
	ResTabs(const tbx::res::ResGadget &other) : tbx::res::ResGadget(other)
	{
     check_type(TYPE_ID);
	}

	/**
	 * Construct a Tabs gadget resource
	 *
	 * @param other Tabs to copy resource from
	 */
	ResTabs(const ResTabs &other) : tbx::res::ResGadget(other)
	{
	}

	virtual ~ResTabs() {}

	/**
	 * Assign from a ResGadget
	 *
	 * @param other ResGadget to copy
	 * @throws ResGadgetTypeMismatch if the ResGadget is not a Tabs
	 */
	ResTabs &operator=(const tbx::res::ResGadget &other)
	{
		other.check_type(TYPE_ID);
		tbx::res::ResBase::operator=(other);
		return *this;
	}

	/**
	 * Assign from another Tabs gadget resource
	 *
	 * @param other Tabs gadget resource to copy
	 */
	ResTabs &operator=(const ResTabs &other)
	{
		tbx::res::ResBase::operator=(other);
		return *this;
	}

	/**
	 * Construct a Tabs gadget resource.
	 *
	 * All options are false, default wimp colours set and messages are null.
	 */
	ResTabs()
	  : tbx::res::ResGadget(0x402c,48)
	{
		init_message(44,0); // text
		foreground(7); // Black
		background(1); // grey
		flags(0);
	}

	/**
	 * Check if Tabs allows scroll arrows
	 *
	 * @returns true if scroll arrows are allowed
	 */
	bool allow_scroll_arrows() const {return flag(0, 1<<0);}
	/**
	 * Set if Tabs allows scroll arrows
	 *
	 * @param value set to true to allow the scroll arrows to be shown
	 */
	void allow_scroll_arrows(bool value) {flag(0,1<<0,value);}
	/**
	 * Get the names of the windows nested in the tabs gagdet
	 *
	 * @returns pointer to zero terminated string or 0 if none
	 */
	const char *nested_windows() const {return message(44);}
	/**
	 * Set the names of the windows nested in the tabs gagdet
	 *
	 * @param value pointer to zero terminated string or 0 if none
	 */
	void nested_windows(const char *value) {message(44, value);}
	/**
	 * Get foreground/text colour for the Tabs
	 *
	 * @returns foreground/text colour
	 */
	int foreground() const {return int_value(36);}
	/**
	 * Set foreground/text colour for the Tabs
	 *
	 * @param value foreground/text wimp colour
	 */
	void foreground(int value) {int_value(36,value);}
	/**
	 * Get background colour for the Tabs
	 *
	 * @returns background
	 */
	int background() const {return int_value(40);}
	/**
	 * Set background colour for the Tabs
	 *
	 * @param value background wimp colour
	 */
	void background(int value) {int_value(40,value);}

};

}
}
}

#endif // TBX_RES_RESTABS_H

