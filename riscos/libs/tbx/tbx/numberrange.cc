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

#include "numberrange.h"
#include "valuechangedlistener.h"
#include "pollinfo.h"

#include "kernel.h"
#include "swixcheck.h"

namespace tbx {

/**
 * Set bounds, step size and precision
 *
 * The non-precision parameters are divided by 10^precision for use
 * in the number range. i.e. set_bound(100,499,40,2) would create
 * a number range from 1.00 to 4.99 with step size 0.4.
 *
 * @param lower lower bound before transformation by precision
 * @param upper upper bound before transformation by precision
 * @param step_size step size before transformation by precision
 * @param precision precision of number range i.e. number of decimal places.
 */
void NumberRange::set_bounds(int lower, int upper, int step_size, int precision)
{
	_kernel_swi_regs regs;
	regs.r[0] = 15;
	regs.r[2] = 834;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = upper;
	regs.r[5] = lower;
	regs.r[6] = step_size;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Get the bounds, step size and precision.
 *
 * The values returned are multiplied by 10^precision
 *
 * @param lower updated to lower bound before transformation by precision
 * @param upper updated to upper bound before transformation by precision
 * @param step_size updated to step size before transformation by precision
 * @param precision updated to precision of number range i.e. number of decimal places.
 */
void NumberRange::get_bounds(int &lower, int &upper, int &step_size, int &precision) const
{
	_kernel_swi_regs regs;
	regs.r[0] = 15;
	regs.r[2] = 835;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

	upper = regs.r[0];
	lower = regs.r[1];
	step_size = regs.r[2];
	precision = regs.r[3];
}

/**
 * Set bounds and step size.
 *
 * The given parameters are divided by 10^current precision
 *
 * @param lower lower bound before transformation by precision
 * @param upper upper bound before transformation by precision
 * @param step_size step size before transformation by precision
 */
void NumberRange::set_bounds(int lower, int upper, int step_size)
{
	_kernel_swi_regs regs;
	regs.r[0] = 7;
	regs.r[2] = 834;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = upper;
	regs.r[5] = lower;
	regs.r[6] = step_size;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Get the bounds and step size
 *
 * The returned parameters are multiplied by 10^precision
 *
 * @param lower updated to lower bound before transformation by precision
 * @param upper updated to upper bound before transformation by precision
 * @param step_size updated to step size before transformation by precision
 */
void NumberRange::get_bounds(int &lower, int &upper, int &step_size) const
{
	_kernel_swi_regs regs;
	regs.r[0] = 835;
	regs.r[2] = 579;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

	upper = regs.r[0];
	lower = regs.r[1];
	step_size = regs.r[2];
}

/**
 * Set lower and upper bounds
 *
 * The given parameters are divided by 10^current precision
 *
 * @param lower lower bound before transformation by precision
 * @param upper upper bound before transformation by precision
 */
void NumberRange::set_bounds(int lower, int upper)
{
	_kernel_swi_regs regs;
	regs.r[0] = 3;
	regs.r[2] = 834;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = upper;
	regs.r[5] = lower;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Get the lower and upper bounds
 *
 * The returned parameters are multiplied by 10^precision
 *
 * @param lower updated to lower bound before transformation by precision
 * @param upper updated to upper bound before transformation by precision
 */
void NumberRange::get_bounds(int &lower, int &upper) const
{
	_kernel_swi_regs regs;
	regs.r[0] = 3;
	regs.r[2] = 835;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

	upper = regs.r[0];
	lower = regs.r[1];
}

/**
 * Set the Lower bound of NumberRange
 *
 */
void NumberRange::lower_bound(int value)
{
	_kernel_swi_regs regs;
	regs.r[0] = 1;
	regs.r[2] = 834;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = 0;
	regs.r[5] = value;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}
/**
 * Get Lower bound of NumberRange
 *
 * @returns lower bound multiplied by 10^current precision
 */
int NumberRange::lower_bound() const
{
	_kernel_swi_regs regs;
	regs.r[0] = 1;
	regs.r[2] = 835;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

	return regs.r[0];
}

/**
 * Set Upper bound of NumberRange
 *
 * @param value new upper bound before transformation by precision
 */
void NumberRange::upper_bound(int value)
{
	_kernel_swi_regs regs;
	regs.r[0] = 2;
	regs.r[2] = 834;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = value;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}
/**
 * Get Upper bound of NumberRange
 *
 * @returns upper bound multiplied by 10^current precision
 */
int NumberRange::upper_bound() const
{
	_kernel_swi_regs regs;
	regs.r[0] = 2;
	regs.r[2] = 835;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

	return regs.r[0];
}

/**
 * Set step size of NumberRange
 *
 * @param value new step size before transformation by precision
 */
void NumberRange::step_size(int value)
{
	_kernel_swi_regs regs;
	regs.r[0] = 4;
	regs.r[2] = 834;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = 0;
	regs.r[5] = 0;
	regs.r[6] = value;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}
/**
 * Get step size of NumberRange
 *
 * @returns step size multiplied by 10^current precision
 */
int NumberRange::step_size() const
{
	_kernel_swi_regs regs;
	regs.r[0] = 4;
	regs.r[2] = 835;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

	return regs.r[2];
}
/**
 * Set the precision of the NumberRange
 *
 * The precision effects how all other parameters are set. They
 * are all set as an integer and then internally divided by
 * 10 ^ precision.
 *
 * @param value new precision
 */
void NumberRange::precision(int value)
{
	_kernel_swi_regs regs;
	regs.r[0] = 8;
	regs.r[2] = 834;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = 0;
	regs.r[5] = 0;
	regs.r[7] = value;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}
/**
 * Get the precision of the NumberRange
 *
 * The precision effects how all other parameters are set. They
 * are all set as an integer and then internally divided by
 * 10 ^ precision.
 *
 * @returns the new precision
 */
int NumberRange::precision() const
{
	_kernel_swi_regs regs;
	regs.r[0] = 4;
	regs.r[2] = 835;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

	return regs.r[3];
}

/**
 * Get the gadget that displays the value of the NumberRange
 *
 * @returns display gadget. This will be a WritableField or a DisplayField
 * depending on if the NumberRange allows a value to be typed.
 */
Gadget NumberRange::numeric_gadget() const
{
	_kernel_swi_regs regs;
	regs.r[0] = 1;
	regs.r[2] = 836;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    return window().gadget(regs.r[0]);
}

/**
 * Get the left adjuster
 *
 * @returns the Adjuster used for reducing the value of the Number range
 */
Adjuster NumberRange::left_adjuster() const
{
	_kernel_swi_regs regs;
	regs.r[0] = 2;
	regs.r[2] = 836;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    return window().gadget(regs.r[1]);
}

/**
 * Get the right adjuster
 *
 * @returns the Adjuster used for increasing the value of the Number range
 */
Adjuster NumberRange::right_adjuster() const
{
	_kernel_swi_regs regs;
	regs.r[0] = 4;
	regs.r[2] = 836;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    return window().gadget(regs.r[2]);
}

/**
 * Get the slider
 *
 * @returns the Slider used by the number range
 */
Slider NumberRange::slider() const
{
	_kernel_swi_regs regs;
	regs.r[0] = 8;
	regs.r[2] = 836;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    return window().gadget(regs.r[3]);
}

//! @cond INTERNAL
/**
 * Router for value changed events
 */
void value_changed_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
 	 ValueChangedEvent event(id_block, data);
 	 static_cast<ValueChangedListener *>(listener)->value_changed(event);
}
//! @endcond

/**
 * Add listener for when the value of the number range changes
 *
 * The default value change event must have been enabled in the resources
 * for this number range.
 */
void NumberRange::add_value_changed_listener(ValueChangedListener *listener)
{
	add_listener(0x8288d, listener, value_changed_router);
}

/**
 * Remove listener for when the value of the number range changes
 */
void NumberRange::remove_value_changed_listener(ValueChangedListener *listener)
{
	remove_listener(0x8288d, listener);
}

}
