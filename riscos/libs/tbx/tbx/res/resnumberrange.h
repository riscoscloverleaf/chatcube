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

#ifndef TBX_RES_RESNUMBERRANGE_H
#define TBX_RES_RESNUMBERRANGE_H

#include "resgadget.h"
#include "resjustification.h"
#include "../colour.h"
#include "../handles.h"

namespace tbx {
namespace res {

/**
 * Class for NumberRange gadget template
 */
class ResNumberRange : public ResGadget
{

public:
	enum {TYPE_ID = 832 };

	/**
	 * Construct an number range gadget resource
	 *
	 * @param other ResGadget to copy resource from
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an number range
	 */
	ResNumberRange(const ResGadget &other) : ResGadget(other)
	{
     check_type(TYPE_ID);
	}

	/**
	 * Construct an number range gadget resource
	 *
	 * @param other number range to copy resource from
	 */
	ResNumberRange(const ResNumberRange &other) : ResGadget(other)
	{
	}

	virtual ~ResNumberRange() {}

	/**
	 * Assign from a ResGadget
	 *
	 * @param other ResGadget to copy
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an number range
	 */
	ResNumberRange &operator=(const ResGadget &other)
	{
		other.check_type(TYPE_ID);
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Assign from another number range gadget resource
	 *
	 * @param other number range gadget resource to copy
	 */
	ResNumberRange &operator=(const ResNumberRange &other)
	{
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Construct a number range gadget resource.
	 *
	 * All options are false, events unset and messages are null.
	 */
	ResNumberRange()
	  : ResGadget(832,68)
	{
		slider_colour(4);
		step_size(1);
		before(-1);
		after(-1);
	}

	/**
	 * Check if the value changed event will be generated
	 *
	 * @returns true if the value changed event will be generated
	 */
	bool generate_value_changed() const {return flag(0, 1<<0);}
	/**
	 * Set if the value changed event will be generated
	 *
	 * @param value set to true to generate the value changed event
	 */
	void generate_value_changed(bool value) {flag(0,1<<0,value);}
	/**
	 * Check if number range is writable
	 *
	 * @returns true if the number range is writable
	 */
	bool writable() const {return flag(0, 1<<2);}
	/**
	 * Set if number range is writable
	 *
	 * @param value set to true to make the number range writable
	 */
	void writable(bool value) {flag(0,1<<2,value);}
	/**
	 * Check if display area is omitted from the number range
	 *
	 * @returns true if there is no display area
	 */
	bool no_display_area() const {return flag(0, 1<<3);}
	/**
	 * Set if display area is omitted from the number range
	 *
	 * @param value set to true to omit the display area
	 */
	void no_display_area(bool value) {flag(0,1<<3,value);}
	/**
	 * Check if the number range shows adjuster arrows
	 *
	 * @returns true if the number range shows adjuster arrows
	 */
	bool has_adjuster_arrows() const {return flag(0, 1<<4);}
	/**
	 * Set if the number range shows adjuster arrows
	 *
	 * @param value set to true to shows adjuster arrows
	 */
	void has_adjuster_arrows(bool value) {flag(0,1<<4,value);}
	/**
	 * Enumaration for how slider is shown
	 */
	enum SliderType
	{
		NO_SLIDER,		//!< no slider
		SLIDER_RIGHT,	//!< slider to the right of the display area
		SLIDER_LEFT		//!< slider to the left of the display area
	};
	/**
	 * Get how the slider is displayed
	 *
	 * @returns SliderType enumeration value specifying how the slider is displayed
	 */
	SliderType slider_type() const {return SliderType(flag_value(0, 224)>>5);}
	/**
	 * Set how the slider is displayed
	 *
	 * @param value set to the SliderType enumeration value for the display required
	 */
	void slider_type(SliderType value) {flag_value(0,224,((int)value)<<5);}

	/**
	 * Get the horizontal position of the number range in its bounding box
	 *
	 * @return ResJustification enumeration specifying the position
	 */
	ResJustification justification() const {return ResJustification(flag_value(0, 768)>>8);}
	/**
	 * Set the horizontal position of the number range in its bounding box
	 *
	 * @param value ResJustification enumeration specifying the position
	 */
	void justification(ResJustification value) {flag_value(0,768,(int)value<<8);}
	/**
	 * Get slider foreground colour
	 *
	 * @returns WimpColour with slider colour
 	 */
	WimpColour slider_colour() const {return WimpColour(flag_value(0, 61440)>>12);}
	/**
	 * Set slider foreground colour
	 *
	 * @param value WimpColour with slider colour
 	 */
	void slider_colour(WimpColour value) {flag_value(0,61440,((int)value)<<12);}
	/**
	 * Get slider background colour
	 *
	 * @returns WimpColour with slider colour
 	 */
	WimpColour slider_background_colour() const {return WimpColour(flag_value(0, 983040)>>16);}
	/**
	 * Set slider background colour
	 *
	 * @param value WimpColour with slider colour
 	 */
	void slider_background_colour(WimpColour value) {flag_value(0,983040,((int)value)<<16);}

	/**
	 * Get the minimum value for the number range
	 *
	 * @returns minimum value (this is the value scaled by the precision)
	 */
	int lower_bound() const {return int_value(36);}
	/**
	 * Set the minimum value for the number range
	 *
	 * @param value the minimum value (this is the value scaled by the precision)
	 */
	void lower_bound(int value) {int_value(36,value);}
	/**
	 * Get the maximum value for the number range
	 *
	 * @returns maximum value (this is the value scaled by the precision)
	 */
	int upper_bound() const {return int_value(40);}
	/**
	 * Set the maximum value for the number range
	 *
	 * @param value the maximum value (this is the value scaled by the precision)
	 */
	void upper_bound(int value) {int_value(40,value);}
	/**
	 * Get the step size value for the number range
	 *
	 * @returns step size value (this is the value scaled by the precision)
	 */
	int step_size() const {return int_value(44);}
	/**
	 * Set the step size value for the number range
	 *
	 * @param value the step size value (this is the value scaled by the precision)
	 */
	void step_size(int value) {int_value(44,value);}
	/**
	 * Get the initial value for the number range
	 *
	 * @returns initial value (this is the value scaled by the precision)
	 */
	int initial_value() const {return int_value(48);}
	/**
	 * Set the initial value for the number range
	 *
	 * @param value the initial value (this is the value scaled by the precision)
	 */
	void initial_value(int value) {int_value(48,value);}
	/**
	 * Get the precision of the number range.
	 *
	 * The precision sets the number of decimal places displayed on the number range.
	 * All values are passed to and retrieved from the number range scaled
	 * by the power of ten of this value.
	 *
	 * e.g. If the precision is 2, 123 is passed to the methods and shown
	 * as 1.23.
	 *
	 * @returns the number of decimal places to display
	 */
	int precision() const {return int_value(52);}
	/**
	 * Set the precision of the number range.
	 *
	 * This sets the number of decimal places displayed on the number range.
	 * All values are passed to and retrieved from the number range scaled
	 * by the power of ten of this value.
	 *
	 * e.g. If the precision is 2, 123 is passed to the methods and shown
	 * as 1.23.
	 *
	 * @param value the number of decimal places to display
	 */
	void precision(int value) {int_value(52,value);}
	/**
	 * Get the component ID of the gadget before this one in the Tab order
	 *
	 * @returns component ID of previous gadget
	 */
	ComponentId before() const {return ComponentId(int_value(56));}
	/**
	 * Set the component ID of the gadget before this one in the Tab order
	 *
	 * @param value component ID of previous gadget
	 */
	void before(ComponentId value) {int_value(56,value);}
	/**
	 * Get the component ID of the gadget after this one in the Tab order
	 *
	 * @returns component ID of next gadget
	 */
	ComponentId after() const {return ComponentId(int_value(60));}
	/**
	 * Set the component ID of the gadget after this one in the Tab order
	 *
	 * @param value component ID of next gadget
	 */
	void after(ComponentId value) {int_value(60,value);}
	/**
	 * Get the length of the display field
	 *
	 * @return length of the display field
	 */
	int display_length() const {return int_value(64);}
	/**
	 * Set the length of the display field
	 *
	 * @param value length of the display field
	 */
	void display_length(int value) {int_value(64,value);}

};

}
}

#endif // TBX_RES_RESNUMBERRANGE_H

