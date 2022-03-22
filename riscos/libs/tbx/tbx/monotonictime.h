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
 * montonictime.h
 *
 *  Created on: 20 Oct 2010
 *      Author: alanb
 */

#ifndef TBX_MONTONICTIME_H_
#define TBX_MONTONICTIME_H_

#include "swis.h"
namespace tbx
{
    /**
     * Read the time in centiseconds since the system was last reset.
     *
     * As the result it stored in an unsigned integer it will wrap around
     * after approximately 497 days.
     *
     * @returns the number of centiseconds since the system was last reset
     */
	inline unsigned int monotonic_time()
	{
		unsigned int mt;
		_swix(OS_ReadMonotonicTime, _OUT(0), &mt);
		return mt;
	}

	/**
	 * Return the elapsed time taking into account
	 * the wrap around for unsigned integers.
	 *
	 * @param from time to measure from
	 * @param to time to measure to
	 */
	inline unsigned int monotonic_elapsed(unsigned int from, unsigned int to)
	{
		if (to >= from) return to - from;
		return to + (0xFFFFFFFF ^ from) + 1;
	}

	/**
	 * Compare if one time is less than another taking into account
	 * wrap around.
	 *
	 * @param compare value to check to see if it is less than
	 * @param to value to compare to
	 * @returns true if compare < to.
	 */
	inline bool monotonic_lt(unsigned int compare, unsigned int to)
	{
		if (compare < to) return true;
		return (compare - to) > 0x7FFFFFFF;
	}

	/**
	 * Compare if one time is less than or equals to another taking into
	 * account wrap around.
	 *
	 * @param compare value to check to see if it is less than
	 * @param to value to compare to
	 * @returns true if compare <= to.
	 */
	inline bool monotonic_le(unsigned int compare, unsigned int to)
	{
		if (compare <= to) return true;
		return (compare - to) > 0x7FFFFFFF;
	}

	/**
	 * Compare if one time is greater than another taking into account
	 * wrap around.
	 *
	 * @param compare value to check to see if it is less than
	 * @param to value to compare to
	 * @returns true if compare > to.
	 */
	inline bool monotonic_gt(unsigned int compare, unsigned int to)
	{
		if (compare <= to) return false;
		return (compare - to) <= 0x7FFFFFFF;
	}

	/**
	 * Compare if one time is greater than or equal to another taking into
	 * account wrap around.
	 *
	 * @param compare value to check to see if it is less than
	 * @param to value to compare to
	 * @returns true if compare >= to.
	 */
	inline bool monotonic_ge(unsigned int compare, unsigned int to)
	{
		if (compare < to) return false;
		if (compare == to) return true;
		return (compare - to) <= 0x7FFFFFFF;
	}
}

#endif /* TBX_MONTONICTIME_H_ */
