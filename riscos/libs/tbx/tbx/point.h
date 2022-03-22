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

#ifndef TBX_POINT_H_
#define TBX_POINT_H_

#include "size.h"

namespace tbx
{

/**
 * Class to represent a position in two dimensional space.
 */
class Point
{
public:
	/**
	 * Construct an uninitialised point
	 *
	 * The x and y values are undefined until set
	 */
	Point() {}
	/**
	 * Construct a point from the give coordinates
	 *
	 * @param ix x coordinate
	 * @param iy y coordinate
	 */
	Point(int ix, int iy) : x(ix), y(iy) {}
	/**
	 * Construct a point as a copy of another point
	 *
	 * @param other point to copy
	 */
	Point(const Point &other) : x(other.x), y(other.y) {}

	int x; /*!< x coordinate of the point */
	int y; /*!< y coordinate of the point */

	/**
	 * Assign this point to the same value as another
	 *
	 * @param other point to copy
	 * @returns *this
	 */
	Point &operator=(const Point &other) {x=other.x; y=other.y; return *this;}
	/**
	 * Check if this point is the same as another
	 *
	 * @param other point to compare with
	 * @return true if this point is the same
	 */
	bool operator==(const Point &other) const {return (x==other.x) && (y==other.y);}
	/**
	 * Check if this point is different from another
	 *
	 * @param other point to compare with
	 * @return true if this point is different
	 */
	bool operator!=(const Point &other) const {return (x!=other.x) || (y!=other.y);}

	/**
	 * Subtract another point from this one
	 *
	 * @param other point to subtract
	 * @returns Size with x difference as width, y difference has height
	 */
	Size operator-(const Point &other) const {return Size(x - other.x, y - other.y);}

	/**
	 * Check if this points is less than another
	 *
	 * Check is x < other.x and y < other.y
	 *
	 * @param other point to compare with
	 * @return true if x & y coordinates are both less
	 */
	bool operator<(const Point &other) const {return x < other.x && y < other.y;}
	/**
	 * Check if this points is less than or equal another
	 *
	 * Check is x <= other.x and y <= other.y
	 *
	 * @param other point to compare with
	 * @return true if x & y coordinates are both less than or equal
	 */
	bool operator<=(const Point &other) const {return x <= other.x && y <= other.y;}
	/**
	 * Check if this points is greater than another
	 *
	 * Check is x > other.x and y > other.y
	 *
	 * @param other point to compare with
	 * @return true if x & y coordinates are both greater
	 */
	bool operator>(const Point &other) const {return x > other.x && y > other.y;}
	/**
	 * Check if this points is greater than or equal to another
	 *
	 * Check is x >= other.x and y >= other.y
	 *
	 * @param other point to compare with
	 * @return true if x & y coordinates are both greater than or equal
	 */
	bool operator>=(const Point &other) const {return x >= other.x && y >= other.y;}
};

}

#endif /* POINT_H_ */
