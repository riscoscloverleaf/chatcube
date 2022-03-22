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

#ifndef TBX_MARGIN_H_
#define TBX_MARGIN_H_

namespace tbx
{

/**
 * Class to represent a margin around an area
 */
class Margin
{
public:
	/**
	 * Construct a margin with 0 for all measurements
	 */
	Margin() : left(0), top(0), right(0), bottom(0) {}
	/**
	 * Construct a margin with all measurements equal
	 *
	 * @param all value for all measurements
	 */
	Margin(int all) : left(all), top(all), right(all), bottom(all) {}
	/**
	 * Construct a margin with the left = right and top = bottom
	 *
	 * @param x size for left and right margins
	 * @param y size for top and bottom margins
	 */
	Margin(int x, int y)  : left(x), top(y), right(x), bottom(y) {}
	/**
	 * Construct a margin with all measurements specified
	 *
	 * @param l left margin
	 * @param t top margin
	 * @param r right margin
	 * @param b bottom margin
	 */
	Margin(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b) {}

	/**
	 * Copy constructor
	 *
	 * @param other Margin to copy
	 */
	Margin(const Margin &other) : left(other.left), top(other.top),
								  right(other.right), bottom(other.bottom) {}

	/**
	 * Left margin
	 */
	int left;
	/**
	 * Top margin
	 */
	int top;
	/**
	 * Right margin
	 */
	int right;
	/**
	 * Bottom margin
	 */
	int bottom;

	/**
	 * Assign this margin with the values of another
	 *
	 * @param other Margin to copy
	 */
	Margin &operator=(const Margin &other) {left=other.left; top=other.top; right=other.right; bottom=other.bottom; return *this;}
	/**
	 * Check if this margin contains the same values as another margin
	 *
	 * @param other margin to compare with
	 * @returns true if all measurements are equal
	 */
	bool operator==(const Margin &other) const {return (left==other.left) && (top==other.top) && (right==other.right) && (bottom==other.bottom);}
	/**
	 * Check if this margin contains differing values from another margin
	 *
	 * @param other margin to compare with
	 * @returns true if one or more measurements are not equal
	 */
	bool operator!=(const Margin &other) const {return (left!=other.left) || (top!=other.top) || (right!=other.right) || (bottom!=other.bottom);}
};

}

#endif /* MARGIN_H_ */
