/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010-2021 Alan Buckley   All Rights Reserved.
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

#ifndef TBX_OFFSETGRAPHICS_H
#define TBX_OFFSETGRAPHICS_H

#include "osgraphics.h"
#include "visiblearea.h"

namespace tbx
{
	/**
	 * Class to draw to graphics to the screen using standard OS routines
	 * off setting the coordinates given.
	 */
	class OffsetGraphics : public OSGraphics
	{
	private:
		int _offset_x;
		int _offset_y;
	public:
		/**
		 * Construct with no offset
		 */
		OffsetGraphics() {_offset_x = 0; _offset_y = 0;}
		/**
		 * Construct with given offsets
		 *
		 * @param offset_x x offset in OS units
		 * @param offset_y y offset in OS units
		 */
		OffsetGraphics(int offset_x, int offset_y) {_offset_x = offset_x; _offset_y = offset_y;}

		/**
		 * Construct from a visible area.
		 *
		 * This constructor calculates its offsets required to plot
		 * in a visible area.
		 *
		 * @param area visible area to retrieve offsets from
		 */
		OffsetGraphics(const VisibleArea &area) {_offset_x = area.screen_x(0); _offset_y = area.screen_y(0);}

		/**
		 * Set the horizontal offset
		 *
		 * @param new_x new value for horizontal offset in OS units
		 */
		void offset_x(int new_x) {_offset_x = new_x;}
		/**
		 * Get the horizontal offset
		 *
		 * @returns horizontal offset in OS units
		 */
		int offset_x() const {return _offset_x;}
		/**
		 * Set the vertical offset
		 *
		 * @param new_y new value for vertical offset in OS units
		 */
		void offset_y(int new_y) {_offset_y = new_y;}
		/**
		 * Get the vertical offset
		 *
		 * @returns vertical offset in OS units
		 */
		int offset_y() const {return _offset_y;}

		/**
		 * Get the offsets as a point
		 *
		 * @returns current offsets in a Point
		 */
		Point offset() const {return Point(_offset_x, _offset_y);}
		/**
		 * Set both offsets from a point
		 *
		 * @param new_offset Point containing the new horizontal and vertical offset in OS units
		 */
		void offset(const Point &new_offset) {_offset_x = new_offset.x; _offset_y = new_offset.y;}

		// Override plot to take into account offset
		virtual void plot(int code, int x, int y) {OSGraphics::plot(code, x + _offset_x, y + _offset_y);}

		// Graphic interface

		// coordinate conversion
		virtual int os_x(int logical_x) const {return logical_x + _offset_x;}
		virtual int os_y(int logical_y) const {return logical_y + _offset_y;}
		virtual int logical_x(int os_x) const {return os_x - _offset_x;}
		virtual int logical_y(int os_y) const {return os_y - _offset_y;}
		virtual Point os(const Point &pt) const {return Point(pt.x+_offset_x, pt.x+_offset_y);}
		virtual Point logical(const Point &pt) const {return Point(pt.x-_offset_x, pt.x-_offset_y);}
		virtual BBox os(const BBox &b) const {return BBox(b.min.x + _offset_x, b.min.y + _offset_y, b.max.x + _offset_x, b.max.y + _offset_y);}
		virtual BBox logical(const BBox &b) const {return BBox(b.min.x - _offset_x, b.min.y - _offset_y, b.max.x - _offset_x, b.max.y - _offset_y);}

		virtual void text(int x, int y, const std::string &text) {OSGraphics::text(x + _offset_x, y + _offset_y, text);}
		virtual void text(int x, int y, const std::string &text, const Font &font) {OSGraphics::text(x + _offset_x, y + _offset_y, text, font);}

		// Images
		virtual void image(int x, int y, const Image &image) {OSGraphics::image(x + _offset_x, y + _offset_y, image);}
		virtual void image(const Point &pt, const Image &im) {OSGraphics::image(pt.x + _offset_x, pt.y + _offset_y, im);}

		// Draw paths
		virtual void fill(int x, int y, const DrawPath &path, DrawFillStyle fill_style = WINDING_NON_ZERO, int flatness = 1) {OSGraphics::fill(x + _offset_x, y + _offset_y, path, fill_style, flatness);}
		virtual void fill(const Point &pt, const DrawPath &path, DrawFillStyle fill_style = WINDING_NON_ZERO, int flatness = 1) {OSGraphics::fill(pt.x + _offset_x, pt.y + _offset_y, path, fill_style, flatness);}
		virtual void stroke(int x, int y, const DrawPath &path,DrawFillStyle fill_style = WINDING_NON_ZERO, int flatness = 1,
					  int thickness = 0, DrawCapAndJoin *cap_and_join = 0, DrawDashPattern *dashes = 0)
		{
			OSGraphics::stroke(x + _offset_x, y + _offset_y, path, fill_style, flatness,
					  thickness, cap_and_join, dashes);
		}
		virtual void stroke(const Point &pt, const DrawPath &path,DrawFillStyle fill_style = WINDING_NON_ZERO, int flatness = 1,
					  int thickness = 0, DrawCapAndJoin *cap_and_join = 0, DrawDashPattern *dashes = 0)
		{
			OSGraphics::stroke(pt.x + _offset_x, pt.y + _offset_y, path, fill_style, flatness,
					  thickness, cap_and_join, dashes);
		}
	};
}

#endif /* TBX_OFFSETGRAPHICS_H_ */
