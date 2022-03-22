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

/*
 * slider.cc
 *
 *  Created on: 27-Mar-2009
 *      Author: alanb
 */

#include "slider.h"
#include "swixcheck.h"
#include "kernel.h"

namespace tbx
{

/**
 * Set all three bounds at once
 */
void Slider::set_bounds(int lower, int upper, int step_size)
{
	_kernel_swi_regs regs;
	regs.r[0] = 7;
	regs.r[2] = 578;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = lower;
	regs.r[5] = upper;
	regs.r[6] = step_size;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Get all three bounds at once
 */
void Slider::get_bounds(int &lower, int &upper, int &step_size) const
{
	_kernel_swi_regs regs;
	regs.r[0] = 7;
	regs.r[2] = 579;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

	lower = regs.r[0];
	upper = regs.r[1];
	step_size = regs.r[2];
}

/**
 * Set lower and upper bounds without changing the step size
 */
void Slider::set_bounds(int lower, int upper)
{
	_kernel_swi_regs regs;
	regs.r[0] = 3;
	regs.r[2] = 578;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = lower;
	regs.r[5] = upper;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Get all lower and upper bounds at once
 */
void Slider::get_bounds(int &lower, int &upper) const
{
	_kernel_swi_regs regs;
	regs.r[0] = 3;
	regs.r[2] = 579;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

	lower = regs.r[0];
	upper = regs.r[1];
}

/**
 * Set Lower bound of slider
 */
void Slider::lower_bound(int value)
{
	_kernel_swi_regs regs;
	regs.r[0] = 1;
	regs.r[2] = 578;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = value;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}
/**
 * Get Lower bound of slider
 */
int Slider::lower_bound() const
{
	_kernel_swi_regs regs;
	regs.r[0] = 1;
	regs.r[2] = 579;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

	return regs.r[0];
}

/**
 * Set Upper bound of slider
 */
void Slider::upper_bound(int value)
{
	_kernel_swi_regs regs;
	regs.r[0] = 2;
	regs.r[2] = 578;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = 0;
	regs.r[5] = value;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}
/**
 * Get Upper bound of slider
 */
int Slider::upper_bound() const
{
	_kernel_swi_regs regs;
	regs.r[0] = 2;
	regs.r[2] = 579;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

	return regs.r[1];
}

/**
 * Set step sizeof slider
 */
void Slider::step_size(int value)
{
	_kernel_swi_regs regs;
	regs.r[0] = 4;
	regs.r[2] = 578;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = 0;
	regs.r[5] = 0;
	regs.r[6] = value;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}
/**
 * Get step size of slider
 */
int Slider::step_size() const
{
	_kernel_swi_regs regs;
	regs.r[0] = 4;
	regs.r[2] = 579;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

	return regs.r[2];
}

/**
 * Set colours for the bar and background of the slider
 *
 * @param bar bar desktop colour
 * @param background background desktop colour
 */
void Slider::set_colour(WimpColour bar, WimpColour background)
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 580;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = (int)bar;
	regs.r[5] = (int)background;

	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}
/**
 * Get colours for the bar and background of the slider
 *
 * @param bar Updated to the bar desktop colour
 * @param background Update to the background desktop colour
 */
void Slider::get_colour(WimpColour &bar, WimpColour &background)
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 581;
	regs.r[1] = _handle;
	regs.r[3] = _id;

	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
	bar = regs.r[0];
	background = regs.r[1];
}

/**
 * Change the bar colour of the slider
 *
 * @param value new WimpColour for the bar
 */
void Slider::bar_colour(WimpColour value)
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 581;
	regs.r[1] = _handle;
	regs.r[3] = _id;

	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
    regs.r[4] = (int)value;
	regs.r[5] = regs.r[1];
	regs.r[0] = 0;
	regs.r[2] = 580;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Get the bar colour of the slider
 *
 * @returns WimpColour of the bar
 */
WimpColour Slider::bar_colour() const
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 581;
	regs.r[1] = _handle;
	regs.r[3] = _id;

	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    return WimpColour(regs.r[0]);
}

/**
 * Change the background colour of the slider
 *
 * @param value new WimpColour for the background
 */
void Slider::background_colour(WimpColour value)
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 581;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
    regs.r[5] = (int)value;
	regs.r[4] = regs.r[0];
	regs.r[0] = 0;
	regs.r[2] = 580;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Get the background colour of the slider
 *
 * @returns WimpColour of the background
 */
WimpColour Slider::background_colour() const
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 581;
	regs.r[1] = _handle;
	regs.r[3] = _id;

	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    return WimpColour(regs.r[1]);
}

/*
 * handle Slider value changed event
 */
static void slider_changed_handler(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	SliderValueChangedEvent event(id_block, data);
	static_cast<SliderValueChangedListener *>(listener)->slider_value_changed(event);
}

/*
 * handle Slider value changed event as a standard value changed event
 */
static void slider_standard_changed_handler(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	ValueChangedEvent event(id_block, data);
	static_cast<ValueChangedListener *>(listener)->value_changed(event);
}


/**
 * Add Listener for changes in the slider
 *
 * @param listener listener to add
 */
void Slider::add_value_changed_listener(SliderValueChangedListener *listener)
{
	add_listener(0x82886, listener, slider_changed_handler);
}
/**
 * Remove Listener for changes in the slider
 *
 * @param listener listener to remove
 */
void Slider::remove_value_changed_listener(SliderValueChangedListener *listener)
{
	remove_listener(0x82886, listener);
}

/**
 * Add Listener for changes in the slider.
 *
 * This listens for the same event as the SliderValueChangedListener but
 * uses a standard value changed event listener to report the value.
 *
 * @param listener listener to add
 */
void Slider::add_value_changed_listener(ValueChangedListener *listener)
{
	add_listener(0x82886, listener, slider_standard_changed_handler);
}
/**
 * Remove Listener for changes in the slider.
 *
 * This listens for the same event as the SliderValueChangedListener but
 * uses a standard value changed event listener to report the value.
 *
 * @param listener listener to remove
 */
void Slider::remove_value_changed_listener(ValueChangedListener *listener)
{
	remove_listener(0x82886, listener);
}




}
