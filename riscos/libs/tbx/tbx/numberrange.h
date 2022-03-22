/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010-2011 Alan Buckley   All Rights Reserved.
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

#ifndef TBX_NUMBERRANGE_H_
#define TBX_NUMBERRANGE_H_

#include "gadget.h"
#include "adjuster.h"
#include "slider.h"

namespace tbx {

class ValueChangedListener;

/**
 * NumberRange wrapper for an underlying toolbox NumberRange gadget.
 *
 * A NumberRange is a gadget that shows a number in a Window that has
 * arrows next to it that can be used to adjust its value.
 */
class NumberRange: public tbx::Gadget {
public:
	enum {TOOLBOX_CLASS = 832}; //!< Toolbox class for this gadget.

	NumberRange() {} //!< Construct an uninitialised number range.
	/**
	 * Destroy a number range gadget.
	 *
	 * This does not delete the underlying toolbox gadget.
	 */
	~NumberRange() {}

	/**
	 * Construct a number range from another number range.
	 *
	 * Both NumberRange will refer to the same underlying toolbox gadget.
	 */
	NumberRange(const NumberRange &other) : Gadget(other) {}

	/**
	 * Construct a number range from another gadget.
	 *
	 * The NumberRange and the Gadget will refer to the same
	 * underlying toolbox gadget.
	 *
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a number range
	 */
	NumberRange(const Gadget &other) : Gadget(other)	{check_toolbox_class(NumberRange::TOOLBOX_CLASS);}

	/**
	 * Construct a number range from a gadget.
	 *
	 * The NumberRange and the Component will refer to the same
	 * underlying toolbox component.
	 */
	NumberRange(const Component &other) : Gadget(other) {Window check(other.handle()); check_toolbox_class(NumberRange::TOOLBOX_CLASS);}

	/**
	 * Assign a number range to refer to the same underlying toolbox gadget
	 * as another.
	 */
	NumberRange &operator=(const NumberRange &other) {_handle = other._handle; _id = other._id; return *this;}

	/**
	 * Assign a number range to refer to the same underlying toolbox gadget
	 * as an existing Gadget.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a number range
	 */
	NumberRange &operator=(const Gadget &other) {_handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

	/**
	 * Assign a number range to refer to the same underlying toolbox component
	 * as an existing Gadget.
	 * @throws ObjectClassError if the component is not in a window.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a number range
	 */
	NumberRange &operator=(const Component &other) {Window check(other.handle()); _handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

	/**
	 * Check if this number range refers to the same underlying toolbox gadget
	 * as another gadget.
	 * @returns true if they refer to the same underlying toolbox gadget.
	 */
	bool operator==(const Gadget &other) const {return (_handle == other.handle() && _id == other.id());}

	/**
	 * Check if this number range refers to the same underlying toolbox gadget
	 * as another gadget.
	 * @returns true if they do not refer to the same underlying toolbox gadget.
	 */
	bool operator!=(const Gadget &other) const {return (_handle != other.handle() || _id != other.id());}

	/**
	 * Set the value of the number range.
	 *
	 * The value will be displayed taking into account it's precision.
	 */
	void value(int value) {int_property(832, value);}

	/**
	 * Get the value of the number range.
	 *
	 * This is the integer version of what is displayed and does not take into
	 * account the number range precision.
	 */
	int value() const {return int_property(833);}

    void set_bounds(int lower, int upper, int step_size, int precision);
	void get_bounds(int &lower, int &upper, int &step_size, int &precision) const;
    void set_bounds(int lower, int upper, int step_size);
	void get_bounds(int &lower, int &upper, int &step_size) const;
	void set_bounds(int lower, int upper);
	void get_bounds(int &lower, int &upper) const;
	void lower_bound(int value);
	int lower_bound() const;
	void upper_bound(int value);
	int upper_bound() const;
	void step_size(int value);
	int step_size() const;
	void precision(int value);
	int precision() const;

    Gadget numeric_gadget() const;
    Adjuster left_adjuster() const;
    Adjuster right_adjuster() const;
    Slider slider() const;

	void add_value_changed_listener(ValueChangedListener *listener);
	void remove_value_changed_listener(ValueChangedListener *listener);
};

}

#endif /* NUMBERRANGE_H_ */
