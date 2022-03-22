/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2012 Alan Buckley   All Rights Reserved.
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

#include "drawpath.h"
#include "swixcheck.h"
#include <swis.h>
#include <stdexcept>
#include <memory>
#include <cstring>

namespace tbx
{

/**
 * Return the size of each Draw element type
 */
int DrawElement::size_in_words(ElementType type)
{
	static int sizes[9] = {2,2,3,3,1,1,7,3,3};
	return sizes[(int)type];
}

/**
 * Create a new draw path object
 *
 * A move, line or gap takes 3 words.
 * A curve takes 7 words
 * Closing a path takes 1 word.
 * The end element and draw continuation take 2 words
 *
 * @param initial capacity in words. 1 word is 4 bytes. Default is 64.
 */
DrawPath::DrawPath(int capacity /*= 64*/)
{
	_capacity = capacity;
	_data = new int[_capacity];
	_size = 0;
}

DrawPath::~DrawPath()
{
	delete [] _data;
}


/**
 * Add an element to the path
 *
 * @param element DrawElement to add
 */
void DrawPath::add(const DrawElement &element)
{
	int el_size = DrawElement::size_in_words(element.type());
	ensure_space(_size + el_size);
	std::memcpy(_data + _size, &element._type, el_size * 4);
	_size += el_size;
	if (element.type() == DrawElement::END)
	{
		_data[_size-1] = (_capacity - _size - 1) * 4;
	}
}

/**
 * End the path
 */
void DrawPath::end_path()
{
	ensure_space(2);
	_data[_size++] = DrawElement::END;
	_data[_size] = (_capacity - _size) * 4;
	_size++;
}

/**
 * Move to (x, y) starting new subpath.
 *
 * The new subpath does affect winding numbers and so is filled normally.
 */
void DrawPath::move(int x, int y)
{
	ensure_space(3);
	_data[_size++] = DrawElement::MOVE;
	_data[_size++] = x;
	_data[_size++] = y;
}

/**
 * Close path with a gap
 */
void DrawPath::close_gap()
{
	ensure_space(1);
	_data[_size++] = DrawElement::CLOSE_GAP;
}

/**
 * Close path with a line
 */
void DrawPath::close_line()
{
	ensure_space(1);
	_data[_size++] = DrawElement::CLOSE_LINE;
}

/**
 * Bezier curve to
 */
void DrawPath::bezier(int cx1, int cy1, int cx2, int cy2, int x, int y)
{
	ensure_space(7);
	_data[_size++] = DrawElement::BEZIER;
	_data[_size++] = cx1;
	_data[_size++] = cy1;
	_data[_size++] = cx2;
	_data[_size++] = cy2;
	_data[_size++] = x;
	_data[_size++] = y;
}

/**
 * Add a gap to a path
 */
void DrawPath::gap(int x, int y)
{
	ensure_space(3);
	_data[_size++] = DrawElement::GAP;
	_data[_size++] = x;
	_data[_size++] = y;
}

/**
 * Add a line to a path
 */
void DrawPath::line(int x, int y)
{
	ensure_space(3);
	_data[_size++] = DrawElement::LINE;
	_data[_size++] = x;
	_data[_size++] = y;
}


/**
 * Adds a circle subpath to the drawing.
 *
 * The circle is an approximation using bezier curves.
 *
 * The circle is made of 4 bezier curves, a move and a close
 * so requires 32 words.
 *
 * @param x user x coordinate of centre
 * @param y user y coordinate of centre
 * @param radius radius of the circle (maximum 32767).
 */
void DrawPath::circle(int x, int y, int radius)
{
	/*
	int d = (radius * 46341) >> 16; // 46341 = sin( 45 degrees ) * 65536
	int d2 = d << 1;

	ensure_space(32);
	move(x-d,y+d);
	bezier(x,d2,   x+d2,y, x+d, y+d);
	bezier(x+d2,y, x,y-d2, x+d, y-d);
	bezier(x,y-d2, x-d2,y, x-d, y-d);
	bezier(x-d2,y, x,y+d2, x-d, y+d);
	close_line();
	*/

	int bx1, by1, bx2, by2, xs, ys, xe, ye;
	int rkappa = (radius * 36914) >> 16; // radius * kappa (0.5522847498307933984022516322796)
	ensure_space(32);

	// First curve
	xs=x; 			ys=y+radius;
	move(xs, ys);
	xe=x+radius;	ye=y;
	bx1=xs+rkappa; 	by1=ys;
	bx2=xe; 		by2=ye+rkappa;
	bezier(bx1,by1, bx2, by2, xe, ye);
	/* Second curve */
	xs=xe; 			ys=ye;
	xe=x; 			ye=y-radius;
	bx1=xs;			by1=ys-rkappa;
	bx2=xe+rkappa;	by2=ye;
	bezier(bx1,by1, bx2, by2, xe, ye);
	/* Third curve */
	xs=xe; ys=ye;
	xe=x-radius; 	ye=y;
	bx1=xs-rkappa;	by1=ys;
	bx2=xe;			by2=ye-rkappa;
	bezier(bx1,by1, bx2, by2, xe, ye);
	/* Final curve */
	xs=xe; 			ys=ye;
	xe=x; 			ye=y+radius;
	bx1=xs;			by1=ys+rkappa;
	bx2=xe-rkappa; 	by2=ye;
	bezier(bx1,by1, bx2, by2, xe, ye);

	/* Close subpath */
	close_line();
}


/**
 * Set the capacity of the path
 *
 * @param new_cap new capacity in words (1 word = 4 bytes)
 */
void DrawPath::capacity(int new_cap)
{
	if (new_cap < _size) return;
	
	int *new_data = new int[new_cap];
	std::memcpy(new_data, _data, _size * 4);
	delete [] _data;
	_data = new_data;
}


/**
 * Fill the interior of a path.
 *
 * This method emulates the PostScript 'fill' operator. It performs the following actions:
 *  - closes open subpaths
 *  - flattens the path
 *  - transforms it to standard coordinates
 *  - fills the resulting path and draws to the VDU.
 *
 * The transform needs to map the user units into internal draw units which
 * are 1/256th of an OS unit.
 *
 * @param style DrawFillStyle flags for filling. Default is WINDING_NON_ZERO.
 * @param transform transformation matrix of 0 for none
 * @param flatness maximum distance allowed from beizer curve when flattening it in user units (0 use default)
 */
void DrawPath::fill(DrawFillStyle fill_style /*= WINDING_NON_ZERO*/, DrawTransform *transform /*= 0*/, int flatness /*= 0*/) const
{
	swix_check(_swix(Draw_Fill, _INR(0,3), _data, fill_style, transform, flatness));
}

/**
 * Sends a path to the VDU
 *
 * This command emulates the PostScript 'stroke' operator. It performs the following actions:
 * flattens the path
 * - applies a dash pattern to the path, if dashes != 0
 * - thickens the path, using the specified joins and caps
 * - re-flattens the path, to flatten round caps and joins, so that they can be filled.
 * - transforms the path to standard coordinates
 * - fills the resulting path and draws to the VDU.
 *
 * The transform needs to map the user units into internal draw units which
 * are 1/256th of an OS unit.
 *
 * @param fill_style DrawFillStyle. Default is WINDING_NON_ZERO.
 * @param transform pointer to transformation matrix, or 0 for identity matrix
 * @param flatness flatness, or 0 for default
 * @param thickness line thickness, or 0 for default.
 * If the thickness is zero then the line is drawn with the minimum width that can be used, given the limitations of the pixel size (so lines are a single pixel wide).
 * If the thickness is n, then the line will be drawn with a thickness of n/2 user coordinates translated to pixels on either side of the theoretical line position.
 * If the line thickness is non-zero, then the cap and join parameter must also be passed.
 * @param cap_and_join pointer to line cap and join specification (if required)
 * @param pointer to dash pattern, or 0 for no dashes
 */
void DrawPath::stroke(DrawFillStyle fill_style /*= WINDING_NON_ZERO*/, DrawTransform *transform /*= 0*/, int flatness /*= 0*/,
					  int thickness /*= 0*/, DrawCapAndJoin *cap_and_join /*= 0*/, DrawDashPattern *dashes /*= 0*/) const
{
	swix_check(_swix(Draw_Stroke, _INR(0,6),
		_data,
		fill_style,
		transform,
		flatness,
		thickness,
		cap_and_join,
		dashes));
}


};
