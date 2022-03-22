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

#ifndef TBX_RES_RESSLIDER_H
#define TBX_RES_RESSLIDER_H

#include "resgadget.h"
#include "../colour.h"

namespace tbx {
namespace res {

/**
 * Class for Slider gadget template
 */
class ResSlider : public ResGadget
{

public:
	enum {TYPE_ID = 576 };

	/**
	 * Construct an slider gadget resource
	 *
	 * @param other ResGadget to copy resource from
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an slider
	 */
	ResSlider(const ResGadget &other) : ResGadget(other)
	{
     check_type(TYPE_ID);
	}

	/**
	 * Construct an slider gadget resource
	 *
	 * @param other slider to copy resource from
	 */
	ResSlider(const ResSlider &other) : ResGadget(other)
	{
	}

	virtual ~ResSlider() {}

	/**
	 * Assign from a ResGadget
	 *
	 * @param other ResGadget to copy
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an slider
	 */
	ResSlider &operator=(const ResGadget &other)
	{
		other.check_type(TYPE_ID);
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Assign from another slider gadget resource
	 *
	 * @param other slider gadget resource to copy
	 */
	ResSlider &operator=(const ResSlider &other)
	{
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Construct a slider gadget resource.
	 *
	 * All options are false, events unset and messages are null.
	 */
	ResSlider()
	  : ResGadget(576,52)
	{
		int_value(0, 0x4010); // bar colour 4, is dragggable
		upper_bound(100);
		step_size(1);
	}

	/**
	 * Check if value changed event will be generated
	 *
	 * @returns true if the value changed event will be generated
	 */
	bool generate_value_changed() const {return flag(0, 1<<0);}
	/**
	 * Set if value changed event will be generated
	 *
	 * @param value set to if the value changed event will be generated
	 */
	void generate_value_changed(bool value) {flag(0,1<<0,value);}
	/**
	 * Check if the value changed event will be generated when the slider is
	 * dragged.
	 *
	 * @returns true to generate the value changed while dragging
	 */
	bool generate_value_changed_while_dragging() const {return flag(0, 1<<1);}
	/**
	 * Set if the value changed event will be generated when the slider is
	 * dragged.
	 *
	 * @param value set to true to generate the value changed while dragging
	 */
	void generate_value_changed_while_dragging(bool value) {flag(0,1<<1,value);}
	/**
	 * Check if the slider is vertical instead of horizontal
	 *
	 * @returns true if the slider is vertical
	 */
	bool vertical() const {return flag(0, 1<<3);}
	/**
	 * Set if the slider is vertical instead of horizontal
	 *
	 * @param value set to true for a vertical slider
	 */
	void vertical(bool value) {flag(0,1<<3,value);}
	/**
	 * Check if the bar can be dragged
	 *
	 * @returns true if the bar can be dragged
	 */
	bool bar_is_draggable() const {return flag(0, 1<<4);}
	/**
	 * Set if the bar can be dragged
	 *
	 * @param value set to true to allow the bar to be dragged
	 */
	void bar_is_draggable(bool value) {flag(0,1<<4,value);}
	/**
	 * Get the bar colour.
	 *
	 * @returns WIMP colour for the bar
	 */
	WimpColour bar_colour() const {return WimpColour(flag_value(0, 61440)>>12);}
	/**
	 * Set the bar colour.
	 *
	 * @param value WIMP colour for the bar
	*/
	void bar_colour(WimpColour value) {flag_value(0,61440,((int)value)<<12);}
	/**
	 * Get the background colour
	 *
	 * @returns WIMP colour for the background
	 */
	WimpColour background_colour() const {return WimpColour(flag_value(0, 983040)>>16);}
	/**
	 * Set the background colour.
	 *
	 * @param value WIMP colour for the background
	*/
	void background_colour(WimpColour value) {flag_value(0,983040,((int)value)<<16);}

	/**
	 * Get the minimum value for the slider
	 *
	 * @returns minimum value for the slider
	 */
	int lower_bound() const {return int_value(36);}
	/**
	 * Set the minimum value for the slider
	 *
	 * @param value minimum value for the slider
	 */
	void lower_bound(int value) {int_value(36,value);}
	/**
	 * Get the maximum value for the slider
	 *
	 * @returns maximum value for the slider
	 */
	int upper_bound() const {return int_value(40);}
	/**
	 * Set the maximum value for the slider
	 *
	 * @param value maximum value for the slider
	 */
	void upper_bound(int value) {int_value(40,value);}
	/**
	 * Get the step size for the slider
	 *
	 * @returns step size for the slider
	 */
	int step_size() const {return int_value(44);}
	/**
	 * Set the step size for the slider
	 *
	 * @param value step size for the slider
	 */
	void step_size(int value) {int_value(44,value);}
	/**
	 * Get the initial value for the slider
	 *
	 * @returns initial value for the slider
	 */
	int initial_value() const {return int_value(48);}
	/**
	 * Set the initial value for the slider
	 *
	 * @param value initial value for the slider
	 */
	void initial_value(int value) {int_value(48,value);}

};

}
}

#endif // TBX_RES_RESSLIDER_H

