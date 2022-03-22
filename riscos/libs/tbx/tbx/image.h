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
 * image.h
 *
 *  Created on: 2 Nov 2010
 *      Author: alanb
 */

#ifndef TBX_IMAGE_H_
#define TBX_IMAGE_H_

#include "point.h"

namespace tbx
{

/**
 * Base class image classes providing a consistent interface
 * to plot the to the screen.
 */
class Image
{
public:
	Image() {};
	virtual ~Image() {};

	/**
	 * Plot the image to the screen
	 *
	 * @param x x coordinate in os units
	 * @param y y coordinate in os units
	 */
	virtual void plot(int x, int y) const = 0;

	/**
	 * Plot the image to the screen
	 *
	 * By default this method just calls Image::plot(x,y)
	 *
	 * @param pt location to plot in os units.
	 */
	virtual void plot(const tbx::Point &pt) const {plot(pt.x,pt.y);}
};

}

#endif /* IMAGE_H_ */
