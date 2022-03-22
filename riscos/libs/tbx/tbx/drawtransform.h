/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010-2012 Alan Buckley   All Rights Reserved.
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
 * drawtransform.h
 *
 *  Created on: 2 Nov 2010
 *      Author: alanb
 */

#ifndef TBX_DRAWTRANSFORM_H_
#define TBX_DRAWTRANSFORM_H_

#include "fixed16.h"

namespace tbx
{
/**
 * Class to represent Drawing transforms
 *
 * This is a three by three matrix that can be used to rotate, scale or translate a path
 * in a single operation. It is laid out like this:
 * a  b  0
 * c  d  0
 * e  f  1
 *
 * This matrix transforms a coordinate (x, y) into another coordinate (x', y') as follows:
 *
 * x' = ax + cy + e
 * y' = bx + dy + f
 *
 * Translation by a given displacement is done by e for the x axis and f for the y axis.
 * Scaling the x axis uses a, while the y axis uses d.
 * Rotation can be performed by setting
 *   a = cos(angle), b= sin(angle), c = -sin(angle) and d = cos(angle)
 *   a, b, c and d given in 16.16 bit fixed point.
 *   e and f are as 256th of an OS unit.
 */
	class DrawTransform
	{
	public:
		Fixed16 a; //!< Top left of transform matrix
		Fixed16 b; //!< Top middle of transform matrix
		Fixed16 c; //!< Middle left of transform matrix
		Fixed16 d; //!< Middle of transform matrix
		int e; //!< Translation in x direction in 256th of an OS unit
		int f; //!< Translation in y direction in 256th of an OS unit

		/**
		 * Create the identity draw transform.
		 * i.e. a transformed that does not move anything
		 */
		DrawTransform() {a = d = 1; b = c = e = f = 0;}

		/**
		 * Set transform translation in os units
		 */
		void translate_os(int x, int y) {e = x << 8;f = y << 8;}
		/**
		 * Set transform translation in os units
		 */
		void translate_os_x(int x) {e = x << 8;}
		/**
		 * Set transform translation in os units
		 */
		void translate_os_y(int y) {f = y << 8;}

		/**
		 * Set matrix to scale without rotation
		 *
		 * @param scale integer to scale by
		 */
		void scale(int scale) {a=d=scale;b=c=0;}

		/**
		 * Set matrix to scale without rotation
		 *
		 * @param scale Fixed16 to scale by
		 */
		void scale(const Fixed16 &scale) {a=d=scale;b=c=0;}

		/**
		 * Set matrix scale so 1 user unit = 1 os unit
		 */
		void scale_os() {a=d=256;b=c=0;}
	};
}

#endif /* TBX_DRAWTRANSFORM_H_ */
