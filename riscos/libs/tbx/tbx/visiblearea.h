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

#ifndef VISIBLEAREA_H_
#define VISIBLEAREA_H_

#include "bbox.h"

namespace tbx
{
	/**
	 * Class to store information for the Visible area of a window
	 * and provide work area to screen conversion.
	 */
	class VisibleArea
	{
	private:
		BBox _bounds;
		Point _scroll;

	public:
		/**
		 * Construct an uninitialised visible area
		 */
		VisibleArea() {};

		/**
		 * Construct a visible area from an array of 6 integers as
		 * provided by Wimp calls.
		 */
		VisibleArea(int *block) :
			_bounds(block[0], block[1], block[2], block[3]),
			_scroll(block[4], block[5])
		{};

		/**
		 * The visible area of the window on the screen.
		 * (screen coordinates)
		 */
		const BBox &bounds() const {return _bounds;}

		/**
		 * The visible area of the window on the screen.
		 * (screen coordinates)
		 */
		BBox &bounds() {return _bounds;}

		/**
		 * The scroll offset of the work area within the window
		 */
		const Point &scroll() const {return _scroll;}

		/**
		 * The scroll offset of the work area within the window
		 */
		Point &scroll() {return _scroll;}

		 /**
		  * Convert screen x coordinate to work area
		  */
		 int work_x(int scr_x) const {return scr_x - (_bounds.min.x - _scroll.x);}

		 /**
		  * Convert screen y coordinate to work area
		  */
		 int work_y(int scr_y) const {return scr_y - (_bounds.max.y - _scroll.y);}

		 /**
		  * Convert point from screen coordinates to work area coordinates
		  *
		  * @param scr_pt Screen point to convert
		  * @param work_pt Work area point to store result
		  * @returns reference to work_pt
		  */
		 Point &work(const Point &scr_pt, Point &work_pt) const
		 {
			 work_pt.x = scr_pt.x - (_bounds.min.x - _scroll.x);
			 work_pt.y = scr_pt.y - (_bounds.max.y - _scroll.y);

			return work_pt;
		 }

		 /**
		  * Convert point from screen coordinates to work area coordinates
		  *
		  * @param scr_pt Screen point to convert
		  * @returns point converted to work area coordinates
		  */
		 Point work(const Point &scr_pt) const {Point pt; work(scr_pt, pt); return pt;}


		 /**
		  * Convert bounding box from screen coordinates to work area coordinates
		  *
		  * @param scr_box Screen BBox to convert
		  * @param work_box Work area BBox to store result
		  * @returns reference to work_box
		  */
		 BBox &work(const BBox &scr_box, BBox &work_box) const
		 {
			 work(scr_box.min, work_box.min);
			 work(scr_box.max, work_box.max);
			 return work_box;
		 }

		 /**
		  * Convert bounding box from screen coordinates to work area coordinates
		  *
		  * @param scr_box Screen BBox to convert
		  * @returns copy of box converted to work area coordinates
		  */
		 BBox work(const BBox &scr_box) const
		 {
			 BBox work_box;
			 work(scr_box.min, work_box.min);
			 work(scr_box.max, work_box.max);
			 return work_box;
		 }

		 /**
		  * Convert work area x coordinate to screen.
		  */
		 int screen_x(int work_x) const {return work_x + (_bounds.min.x - _scroll.x);}

		 /**
		  * Convert work area y coordinate to screen area
		  */
		 int screen_y(int work_y) const {return work_y + (_bounds.max.y - _scroll.y);}

		 /**
		  * Convert point from work coordinates to screen area coordinates
		  *
		  * @param work_pt Work area point to convertt
		  * @param scr_pt Screen point to store resul
		  * @returns reference to scr_pt
		  */
		 Point &screen(const Point &work_pt, Point &scr_pt) const
		 {
			 scr_pt.x = work_pt.x + (_bounds.min.x - _scroll.x);
			 scr_pt.y = work_pt.y + (_bounds.max.y - _scroll.y);

			 return scr_pt;
		 }

		 /**
		  * Convert point from work area coordinates to screen coordinates
		  *
		  * @param work_pt work area point to convert
		  * @returns point converted to screen coordinates
		  */
		 Point screen(const Point &work_pt) const {Point pt; screen(work_pt, pt); return pt;}


		 /**
		  * Convert bounding box from work area coordinates to screen coordinates
		  *
		  * @param work_box Work area BBox to convert
		  * @param scr_box Screen BBox to store result
		  * @returns reference to scr_box
		  */
		 BBox &screen(const BBox &work_box, BBox &scr_box) const
		 {
			 screen(work_box.min, scr_box.min);
			 screen(work_box.max, scr_box.max);
			 return scr_box;
		 }

		 /**
		  * Convert bounding box from work area coordinates to screen coordinates
		  *
		  * @param work_box work area BBox to convert
		  * @returns copy of box converted to screen coordinates
		  */
		 BBox screen(const BBox &work_box) const
		 {
			 BBox scr_box;
			 screen(work_box.min, scr_box.min);
			 screen(work_box.max, scr_box.max);
			 return scr_box;
		 }
	};
}

#endif /* VISIBLEAREA_H_ */
