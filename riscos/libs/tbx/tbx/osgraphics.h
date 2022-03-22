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

#ifndef TBX_OSGRAPHICS_H
#define TBX_OSGRAPHICS_H

#include "graphics.h"

namespace tbx
{
	/**
	 * Class to draw to graphics to the screen using standard OS routines
	 * with no translation or scaling
	 */
	class OSGraphics : public Graphics
	{
	public:
		OSGraphics();

		virtual void plot(int code, int x, int y);
		/*! Enum for how to combine plotted graphics with the screen */
		enum PlotAction {
			ACTION_OVERWRITE, 	/*!< overwrite the screen */
			ACTION_OR, 			/*!< Or the colour with the screen */
			ACTION_AND, 		/*!< And the colour with the screen */
			ACTION_XOR,			/*!< Exclusive Or the colour with the screen */
			ACTION_INVERT, 		/*!< invert the screen colour*/
			ACTION_NONE, 		/*!< Do not change the screen */
			ACTION_AND_NOT, 	/*!< And Not the colour with the screen */
			ACTION_OR_NOT}; 	/*!< Or Not the colour with the screen */

		void plot_action(PlotAction action);

		// Graphic interface

		// coordinate conversion
		virtual int os_x(int logical_x) const {return logical_x;}
		virtual int os_y(int logical_y) const {return logical_y;}
		virtual int logical_x(int os_x) const {return os_x;}
		virtual int logical_y(int os_y) const {return os_y;}

		virtual Point os(const Point &pt)  const {return pt;}
		virtual Point logical(const Point &pt) const {return pt;}
		virtual BBox os(const BBox &b) const {return b;}
		virtual BBox logical(const BBox &b) const {return b;}

		virtual void foreground(Colour colour);
		virtual void background(Colour colour);

		virtual void wimp_foreground(WimpColour colour);
		virtual void wimp_background(WimpColour colour);

		// Drawing
		virtual void move(int x, int y);
		virtual void point(int x, int y);
		virtual void line(int fx, int fy, int tx, int ty);
		virtual void line(int x, int y);
		virtual void rectangle(int xmin, int ymin, int xmax, int ymax);
		virtual void fill_rectangle(int xmin, int ymin, int xmax, int ymax);
		virtual void circle(int centre_x, int centre_y, int radius);
		virtual void fill_circle(int centre_x, int centre_y, int radius);
		virtual void arc(int centre_x, int centre_y, int start_x, int start_y, int end_x, int end_y);
		virtual void segment(int centre_x, int centre_y, int start_x, int start_y, int end_x, int end_y);
		virtual void sector(int centre_x, int centre_y, int start_x, int start_y, int end_x, int end_y);
		virtual void ellipse(int centre_x, int centre_y, int intersect_x, int intersect_y, int high_x, int high_y);
		virtual void fill_ellipse(int centre_x, int centre_y, int intersect_x, int intersect_y, int high_x, int high_y);

		virtual void path(const Point *points, int num);
		virtual void polygon(const Point *points, int num);
		virtual void fill_polygon(const Point *points, int num);

		virtual void text(int x, int y, const std::string &text);
		virtual void text(int x, int y, const std::string &text, const Font &font);
		virtual void text_colours(Colour foreground, Colour background);
		virtual void text_colours(Font &font, Colour foreground, Colour background);


		// Images
		virtual void image(int x, int y, const Image &image);

		// Draw paths
		virtual void fill(int x, int y, const DrawPath &path, DrawFillStyle fill_style = WINDING_NON_ZERO, int flatness = 1);
		virtual void stroke(int x, int y, const DrawPath &path,DrawFillStyle fill_style = WINDING_NON_ZERO, int flatness = 1,
					  int thickness = 0, DrawCapAndJoin *cap_and_join = 0, DrawDashPattern *dashes = 0);

		// OS Graphics specific functions
		void clear();
		BBox clip() const;
		void clip(const BBox &clip_to);

	protected:
		/**
		 * Stores the current Plot action
		 */
		int _plot_action;
	};
}

#endif
