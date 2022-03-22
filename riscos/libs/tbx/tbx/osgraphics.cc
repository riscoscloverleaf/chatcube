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

#include "osgraphics.h"
#include "swixcheck.h"

#include "swis.h"

using namespace tbx;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * Create the graphics class with the default plot
 * action of overwrite.
 */
OSGraphics::OSGraphics()
{
   _plot_action = ACTION_OVERWRITE;
}


/**
 * Sets the plot action.
 *
 * The plot action. The plot action doesn't take effect until
 * the next colour change.
 */
void OSGraphics::plot_action(PlotAction action)
{
	_plot_action = action;
}

/**
 * Execute the OS_Plot swi.
 *
 * For details of the plot codes see the RISC OS PRM.
 *
 * For clarity use and protability to other Graphics derived
 * classes use the move, line, etc. codes
 */
void OSGraphics::plot(int code, int x, int y)
{
	_swix(OS_Plot, _INR(0,2), code, x, y);
}

/**
 * Set foreground graphics colour to the nearest match
 * to the given colour for the current screen mode.
 *
 * It will use an ECF pattern to get a better match.
 */
void OSGraphics::foreground(Colour colour)
{
	swix_check(_swix(ColourTrans_SetGCOL, _IN(0)|_INR(3,4),
			(unsigned)colour,
			(1 << 8), // use ECF
			_plot_action));
}

/**
 * Set background graphics colour to the nearest match
 * to the given colour for the current screen mode.
 *
 * It will use an ECF pattern to get a better match.
 */
void OSGraphics::background(Colour colour)
{
	swix_check(_swix(ColourTrans_SetGCOL, _IN(0)|_INR(3,4),
			(unsigned)colour,
			(1 << 7) | (1 << 8), // set background and use ECF
			_plot_action));
}

/**
 * Set the graphics foreground colour to the given WIMP colour
 */
void OSGraphics::wimp_foreground(WimpColour colour)
{
	swix_check(_swix(Wimp_SetColour, _IN(0),
			((int)colour)
			| (_plot_action << 4)
			));
}

/**
 * Set the graphics background colour to the given WIMP colour
 */
void OSGraphics::wimp_background(WimpColour colour)
{
	swix_check(_swix(Wimp_SetColour, _IN(0),
			((int)colour)
			| (_plot_action << 4)
			| (1 << 7) // Background flag
			));

}

/**
 * Move graphics position
 */
void OSGraphics::move(int x, int y)
{
	plot( 4, x, y);
}

/**
 * Draw a single pixel point
 */
void OSGraphics::point(int x, int y)
{
	plot( 69, x, y);
}

/**
 * Draw a line from the current graphics position
 * to the given coordinates.
 */
void OSGraphics::line(int x, int y)
{
	plot(5, x, y);
}

/**
 * Draw a line betweenn the given coordinates.
 */
void OSGraphics::line(int from_x, int from_y, int to_x, int to_y)
{
	move(from_x, from_y);
	plot(5, to_x, to_y);
}

/**
 * Draw outline of a rectangle
 */
void OSGraphics::rectangle(int xmin, int ymin, int xmax, int ymax)
{
	move(xmin,ymin);
	// plot 13 is line not including end point
	plot(13 ,xmin, ymax);
	plot(13, xmax, ymax);
	plot(13, xmax, ymin);
	plot(13, xmin, ymin);
}

/**
 * Fill a rectangle the current foreground colour
 */
void OSGraphics::fill_rectangle(int xmin, int ymin, int xmax, int ymax)
{
	move(xmin, ymin);
	plot(101, xmax, ymax);
}

/**
 * Draw outline of a circle
 *
 * @param centre_x centre of circle
 * @param centre_y centre of circle
 * @param radius radius of the circle
 */
void OSGraphics::circle(int centre_x, int centre_y, int radius)
{
	move(centre_x,centre_y);
	plot(149,centre_x+radius,centre_y); // outline
}

/**
 * Draw a filled circle
 *
 * @param centre_x centre of circle
 * @param centre_y centre of circle
 * @param radius radius of the circle
 */
void OSGraphics::fill_circle(int centre_x, int centre_y, int radius)
{
	move(centre_x,centre_y);
	plot(157, centre_x+radius,centre_y);
}

/**
 * Draw arc around a circle
 *
 * @param centre_x centre of circle arc is on
 * @param centre_y centre of circle arc is on
 * @param start_x start of arc
 * @param start_y start of arc
 * @param end_x point on line from centre to the end of the arc
 * @param end_y point on line from centre to the end of the arc
 */
void OSGraphics::arc(int centre_x, int centre_y, int start_x, int start_y, int end_x, int end_y)
{
	move(centre_x, centre_y);
	move(start_x, start_y);
	plot(165, end_x, end_y);
}

/**
 * Draw a segment of a circle
 *
 * @param centre_x centre of circle arc is on
 * @param centre_y centre of circle arc is on
 * @param start_x start of arc
 * @param start_y start of arc
 * @param end_x point on line from centre to the end of the arc
 * @param end_y point on line from centre to the end of the arc
 */
void OSGraphics::segment(int centre_x, int centre_y, int start_x, int start_y, int end_x, int end_y)
{
	move(centre_x, centre_y);
	move(start_x, start_y);
	plot(173, end_x, end_y);
}

/**
 * Draw a sector of a circle
 *
 * @param centre_x centre of circle arc is on
 * @param centre_y centre of circle arc is on
 * @param start_x start of arc
 * @param start_y start of arc
 * @param end_x point on line from centre to the end of the arc
 * @param end_y point on line from centre to the end of the arc
 */
void OSGraphics::sector(int centre_x, int centre_y, int start_x, int start_y, int end_x, int end_y)
{
	move(centre_x, centre_y);
	move(start_x, start_y);
	plot(181, end_x, end_y);
}

/**
 * Draw the outline of an ellipse
 *
 * @param centre_x centre of the ellipse
 * @param centre_y centre of the ellipse
 * @param intersect_x intersection of centre_y and edge of ellipse
 * @param intersect_y intersection of centre_y and edge of ellipse
 * @param high_x highest (or lowest) point on the ellipse
 * @param high_y highest (or lowest) point on the ellipse
 */
void OSGraphics::ellipse(int centre_x, int centre_y, int intersect_x, int intersect_y, int high_x, int high_y)
{
	move(centre_x, centre_y);
	move(intersect_x, intersect_y);
	plot(197, high_x, high_y);
}

/**
 * Draw a filled ellipse
 *
 * @param centre_x centre of the ellipse
 * @param centre_y centre of the ellipse
 * @param intersect_x intersection of centre_y and edge of ellipse
 * @param intersect_y intersection of centre_y and edge of ellipse
 * @param high_x highest (or lowest) point on the ellipse
 * @param high_y highest (or lowest) point on the ellipse
 */

void OSGraphics::fill_ellipse(int centre_x, int centre_y, int intersect_x, int intersect_y, int high_x, int high_y)
{
	move(centre_x, centre_y);
	move(intersect_x, intersect_y);
	plot(205, high_x, high_y);
}

/**
 * Draw a line through the specified points
 *
 * @param points array of points to draw through
 * @param num number of points
 */
void OSGraphics::path(const Point *points, int num)
{
	move(points->x, points->y);
	while (--num > 1)
	{
		points++;
		// plot 13 is line not including end point
		plot(13, points->x, points->y);
	}
	if (num > 0)
	{
		// Last segment of line includes the end.
		points++;
		line(points->x, points->y);
	}
}

/**
 * Draw outline of a polygon
 *
 * @param points vertices of the polygon
 * @param num number of vertices (minimum of 3)
 * @throws std::domain_error if num is less than three
 */
void OSGraphics::polygon(const Point *points, int num)
{
	if (num < 3) throw std::domain_error("Too few points for polygon");
	const Point *start = points;
	move(points->x, points->y);
	while (--num > 0)
	{
		points++;
		// plot 13 is line not including end point
		plot(13, points->x, points->y);
	}
	plot(13, start->x, start->y);
}

/**
 * Fill a polygon.
 *
 * The polygon is filled using triangles swept from the
 * first point so the line from the first point to any
 * vertex should not cross any edges of the polygon.
 *
 * @param points vertices of the polygon
 * @param num number of vertices (minimum of 3)
 * @throws std::domain_error if num is less than three
 */
void OSGraphics::fill_polygon(const Point *points, int num)
{
	if (num < 3) throw std::domain_error("Too few points for polygon");
	const Point *start = points;
	move(points->x, points->y);
	points++;
	move(points->x, points->y);
	while (--num > 1)
	{
		points++;
		plot(85, points->x, points->y);
		move(start->x, start->y);
	}
}

/**
 * Draw text in current desktop font
 */
void OSGraphics::text(int x, int y, const std::string &text)
{
	WimpFont wf;
	wf.paint(x,y,text);
}

/**
 * Draw text in given font
 */
void OSGraphics::text(int x, int y, const std::string &text, const Font &font)
{
	font.paint(x,y, text, Font::FPF_OSUNITS);
}

/**
 * Set the colours for the desktop font.
 */
void OSGraphics::text_colours(Colour foreground, Colour background)
{
	WimpFont wf;
	wf.set_colours(foreground, background);
}

/**
 * Set the colours for the given font
 */
void OSGraphics::text_colours(Font &font, Colour foreground, Colour background)
{
	font.set_colours(foreground, background);
}

/**
 * Draw an image at given location
 */
void OSGraphics::image(int x, int y, const Image &image)
{
	image.plot(x, y);
}

/**
 * Fill an draw path at the given location
 */
void OSGraphics::fill(int x, int y, const DrawPath &path, DrawFillStyle fill_style /*= WINDING_NON_ZERO*/, int flatness /*= 1*/)
{
	DrawTransform trans;
	trans.translate_os(x,y);
	trans.scale_os();
	path.fill(fill_style, &trans, flatness);
}

/**
 * Draw lines of a draw path at the given location
 */
void OSGraphics::stroke(int x, int y, const DrawPath &path,DrawFillStyle fill_style /* = WINDING_NON_ZERO*/, int flatness /* = 1*/,
					  int thickness /* = 0*/, DrawCapAndJoin *cap_and_join /* = 0*/, DrawDashPattern *dashes /* = 0*/)
{
	DrawTransform trans;
	trans.translate_os(x,y);
	trans.scale_os();
	path.stroke(fill_style, &trans, flatness, thickness, cap_and_join, dashes);
}

/**
 * Clear the current graphics window to the background colour
 */
void OSGraphics::clear()
{
	_kernel_oswrch(16);
}

/**
 * Get the current Graphics clipping region.
 *
 * @returns bounding box of the cliping region in logical co-ordinates.
 * The coordinates are inclusive. The egdes of the bounding box lie within the window.
 */
BBox OSGraphics::clip() const
{
	int vars[7], vals[6];
	vars[0] = 4; // X-Eig
	vars[1] = 5; // Y-Eig
	vars[2] = 128; // Left-hand column of the graphics window (ic)
	vars[3] = 129; // Bottom row of the graphics window (ic)
	vars[4] = 130; // Right-hand column of the graphics window (ic)
	vars[5] = 131; // Top row of the graphics window (ic)
	vars[6] = -1;

	_swix(OS_ReadVduVariables, _INR(0,1), vars, vals);

	// Create result in OS coordinates
	BBox result(vals[2] << vals[0], vals[3] << vals[1], vals[4] << vals[0], vals[5] << vals[1]);
	return logical(result);
}

/**
 * Set the clipping region.
 *
 * This call will fail if the clip_to parameter is outside the screen once transformed
 * from logical co-ordinates to screen co-ordinates.
 *
 * @param clip_to area to clip the graphics to in logical units.
 * The coordinates are inclusive. The egdes of the bounding box lie within the window.
 */
void OSGraphics::clip(const BBox &clip_to)
{
	BBox lclip = os(clip_to);
	_kernel_oswrch(24);
	_kernel_oswrch(lclip.min.x & 0xFF);
	_kernel_oswrch(lclip.min.x >> 8);
	_kernel_oswrch(lclip.min.y & 0xFF);
	_kernel_oswrch(lclip.min.y >> 8);
	_kernel_oswrch(lclip.max.x & 0xFF);
	_kernel_oswrch(lclip.max.x >> 8);
	_kernel_oswrch(lclip.max.y & 0xFF);
	_kernel_oswrch(lclip.max.y >> 8);
}
