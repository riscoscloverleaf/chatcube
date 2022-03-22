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
 * hourglass.cc
 *
 *  Created on: 01-Jun-2009
 *      Author: alanb
 */

#include "hourglass.h"
#include "swis.h"

namespace tbx {

/**
 * Construct an hour glass.
 *
 * By default this turns the RISC OS hourglass on.
 * Pass false as it's parameter to stop the hourglass starting immediately.
 *
 * The destructor automatically turns it off
 *
 * @param on true to turn RISC OS hourglass on, false if hourglass isn't
 * required immediatly;
 */
Hourglass::Hourglass(bool on /*= true*/)
{
	_on = on;
	if (on) _swix(Hourglass_On, 0);
}

/**
 * Hourglass destructor. This will automatically turn the hourglass off
 * if it is on
 */
Hourglass::~Hourglass()
{
	if (_on) _swix(Hourglass_Off, 0);
}

/**
 * Turns hourglass on
 *
 * Has no effect if hourglass is already on
 */
void Hourglass::on()
{
	if (!_on)
	{
		_on = true;
		_swix(Hourglass_On, 0);
	}
}

/**
 * Turns the hourglass off
 *
 * Has no effect if it is not on
 */
void Hourglass::off()
{
	if (_on)
	{
		_on = false;
		 _swix(Hourglass_Off, 0);
	}
}

/**
 * Starts the hourglass making it visible after the specified
 * delay.
 *
 * @param delay delay in centiseconds before hourglass appears.
 * If delay is 0 it will suppress the hourglass.
 */
void Hourglass::start(int delay)
{
	_on = true;
	_swix(Hourglass_Start, _IN(0), delay);
}

/**
 * Show a percentage below the hourglass
 *
 * @param pc percentage to show from 0 to 99.
 */
void Hourglass::percentage(int pc)
{
	_swix(Hourglass_Percentage, _IN(0), pc);
}

}
