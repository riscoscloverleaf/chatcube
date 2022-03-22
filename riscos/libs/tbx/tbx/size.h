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

#ifndef TBX_SIZE_H_
#define TBX_SIZE_H_

namespace tbx
{

/**
 * Class to represent a two-dimensional size
 */
class Size
{
public:
	/**
	 * Construct an uninitialised size object.
	 *
	 * The width and height have undefined values
	 */
	Size() {}
	/**
	 * Construct a size object with the given dimensions
	 *
	 * @param w width for size object
	 * @param h height for size object
	 */
	Size(int w, int h) : width(w), height(h) {}
	/**
	 * Copy constructor
	 *
	 * @param other size object to copy
	 */
	Size(const Size &other) : width(other.width), height(other.height) {}

	int width;	//!< Width of size object
	int height; //!< Height of size object

	/**
	 * Assign size to be equal to another
	 *
	 * @param other Size to copy
	 * @returns *this
	 */
	Size &operator=(const Size &other) {width=other.width; height=other.height; return *this;}
	/**
	 * Check if two size have the same values
	 *
	 * @param other Size to compare with
	 * @returns true if sizes match
	 */
	bool operator==(const Size &other) const {return (width==other.width) && (height==other.height);}
	/**
	 * Check if two size have a different width or height
	 *
	 * @param other Size to compare with
	 * @returns true if sizes do not match
	 */
	bool operator!=(const Size &other) const {return (width!=other.width) || (height!=other.height);}
};

}

#endif /* TBX_SIZE_H_ */
