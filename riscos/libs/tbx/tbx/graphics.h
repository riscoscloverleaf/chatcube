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

#ifndef TBX_GRAPHICS_H
#define TBX_GRAPHICS_H

#include "colour.h"
#include "font.h"
#include "bbox.h"
#include "image.h"
#include "drawpath.h"
#include <string>

namespace tbx
{
	/**
	 * Interface to drawing graphics to the screen.
	 *
	 * This provides a standard interface that can be used with all
	 * the graphics classes.
	 */
	class Graphics
	{
	public:
		Graphics() {}
		virtual ~Graphics() {}

		// coordinate conversion
		/**
		 * Convert from logical x value to OS units
		 *
		 * @param logical_x value to convert
		 * @returns equivalent value in OS units
		 */
		virtual int os_x(int logical_x) const = 0;
		/**
		 * Convert from logical y value to OS units
		 *
		 * @param logical_y value to convert
		 * @returns equivalent value in OS units
		 */
		virtual int os_y(int logical_y) const = 0;
		/**
		 * Convert from OS units to logical x value
		 *
		 * @param os_x value to convert
		 * @returns equivalent value in logical units
		 */
		virtual int logical_x(int os_x) const = 0;
		/**
		 * Convert from OS units to logical y value
		 *
		 * @param os_y value to convert
		 * @returns equivalent value in logical units
		 */
		virtual int logical_y(int os_y) const = 0;

		/**
		 * Convert from logical coordinates to OS units
		 *
		 * @param pt value to convert
		 * @returns equivalent value in OS units
		 */
		virtual Point os(const Point &pt) const {Point o;o.x=os_x(pt.x);o.y=os_y(pt.y);return o;}
		/**
		 * Convert from OS units to logical coordinates
		 *
		 * @param pt value to convert
		 * @returns equivalent value in logical units
		 */
		virtual Point logical(const Point &pt) const {Point o;o.x=logical_x(pt.x);o.y=logical_y(pt.y);return o;}
		/**
		 * Convert from logical coordinates to OS units
		 *
		 * @param b value to convert
		 * @returns equivalent value in OS units
		 */
		virtual BBox os(const BBox &b) const {BBox o;o.min = os(b.min);o.max = os(b.max);return o;}
		/**
		 * Convert from OS units to logical coordinates
		 *
		 * @param b value to convert
		 * @returns equivalent value in logical units
		 */
		virtual BBox logical(const BBox &b) const {BBox o;o.min = logical(b.min);o.max = logical(b.max);return o;}

		// Colours
		/**
		 * Set the foreground colour for graphics
		 *
		 * @param colour new colour for the foreground
		 */
		virtual void foreground(Colour colour) = 0;
		/**
		 * Set the background colour for graphics
		 *
		 * @param colour new colour for the background
		 */
		virtual void background(Colour colour) = 0;

		/**
		 * Set the foreground colour for graphics to one of the standard
		 * WIMP colours.
		 *
		 * @param colour new colour for the foreground
		 */
		virtual void wimp_foreground(WimpColour colour) = 0;
		/**
		 * Set the background colour for graphics to one of the standard
		 * WIMP colours.
		 *
		 * @param colour new colour for the background
		 */
		virtual void wimp_background(WimpColour colour) = 0;

		// Drawing
		/**
		 * Move the graphics cursor to the given point.
		 *
		 * @param x x coordinate
		 * @param y y coordinate
		 */
		virtual void move(int x, int y) = 0;
		/**
		 * Draw a point at the given location.
		 *
		 * @param x x coordinate
		 * @param y y coordinate
		 */
		virtual void point(int x, int y) = 0;

		/**
		 * Draw a line from the last point visited to
		 * the given location
		 *
		 * @param tx coordinate of end of line
		 * @param ty coordinate of end of line
		 */
		virtual void line(int tx, int ty) = 0;

		/**
		 * Draw a line between the given two points
		 *
		 * @param fx x coordinate of start of line
		 * @param fy y coordinate of start of line
		 * @param tx x coordinate of end of line
		 * @param ty y coordinate of end of line
		 */
		virtual void line(int fx, int fy, int tx, int ty) {move(fx,ty);line(tx,ty);}
		/**
		 * Draw the outline of a rectangle
		 *
		 * @param xmin minimum x coordinate of rectangle
		 * @param ymin minimum y coordinate of rectangle
		 * @param xmax maximum x coordinate of rectangle
		 * @param ymax maximum y coordinate of rectangle
		 */
		virtual void rectangle(int xmin, int ymin, int xmax, int ymax) = 0;
		/**
		 * Draw a filled rectangle
		 *
		 * @param xmin minimum x coordinate of rectangle
		 * @param ymin minimum y coordinate of rectangle
		 * @param xmax maximum x coordinate of rectangle
		 * @param ymax maximum y coordinate of rectangle
		 */
		virtual void fill_rectangle(int xmin, int ymin, int xmax, int ymax) = 0;

		/**
		 * Draw a path of lines connecting the given points
		 *
		 * @param points array of points to connect
		 * @param num number of points to connect
		 */
		virtual void path(const Point *points, int num) = 0;
		/**
		 * Draw the outline of a polygon.
		 *
		 * This draws a line connecting all the points and an
		 * additional line connecting the last and first points.
		 *
		 * @param points array of vertices of the polygon
		 * @param num number of vertices
		 */
		virtual void polygon(const Point *points, int num) = 0;

		/**
		 * Draw a filled polygon.
		 *
		 * The algorithm that fills the polygon is not guaranteed
		 * to be accurate unless it can fill it by using triangles
		 * swept from the first point.
		 *
		 * @param points array of vertices of the polygon
		 * @param num number of vertices
		 */
		virtual void fill_polygon(const Point *points, int num) = 0;

		/**
		 * Draw the outline of a circle
		 *
		 * @param centre_x x coordinate of centre of the circle
		 * @param centre_y y coordinate of centre of the circle
		 * @param radius radius of the circle
		 */
		virtual void circle(int centre_x, int centre_y, int radius) = 0;
		/**
		 * Draw a filled circle
		 *
		 * @param centre_x x coordinate of centre of the circle
		 * @param centre_y y coordinate of centre of the circle
		 * @param radius radius of the circle
		 */
		virtual void fill_circle(int centre_x, int centre_y, int radius) = 0;
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
		virtual void arc(int centre_x, int centre_y, int start_x, int start_y, int end_x, int end_y) = 0;
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
		virtual void segment(int centre_x, int centre_y, int start_x, int start_y, int end_x, int end_y) = 0;
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
		virtual void sector(int centre_x, int centre_y, int start_x, int start_y, int end_x, int end_y) = 0;
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
		virtual void ellipse(int centre_x, int centre_y, int intersect_x, int intersect_y, int high_x, int high_y) = 0;
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
		virtual void fill_ellipse(int centre_x, int centre_y, int intersect_x, int intersect_y, int high_x, int high_y) = 0;

		// Drawing using points and boxes
		/**
		 * Move the graphics cursor to the given location.
		 *
		 * @param point coordinates to move to
		 */
	    virtual void move(const Point &point) {move(point.x,point.y);}
		/**
		 * Draw a point at the given location.
		 *
		 * @param pt coordinates to draw point
		 */
	    virtual void point(const Point &pt) {point(pt.x,pt.y);}
		/**
		 * Draw a line from the last point visited to
		 * the given location
		 *
		 * @param to_point coordinates to draw line to
		 */
		virtual void line(const Point &to_point) {line(to_point.x,to_point.y);}
		/**
		 * Draw a line between the given two points
		 *
		 * @param from_point coordinates of start of line
		 * @param to_point coordinates of end of line
		 */
		virtual void line(const Point &from_point, const Point &to_point) {move(from_point);line(to_point);}
		/**
		 * Draw the outline of a rectangle
		 *
		 * @param min_point minimum coordinates of rectangle
		 * @param max_point maximum coordinates of rectangle
		 */
		virtual void rectangle(const Point &min_point, const Point &max_point) {rectangle(min_point.x, min_point.y, max_point.x, max_point.y);}
		/**
		 * Draw the outline of a rectangle
		 *
		 * @param r coordinates of rectangle
		 */
		virtual void rectangle(const BBox &r) {rectangle(r.min.x,r.min.y, r.max.x, r.max.y);}
		/**
		 * Draw the outline of a rectangle
		 *
		 * @param min_point minimum coordinates of rectangle
		 * @param max_point maximum coordinates of rectangle
		 */
		virtual void fill_rectangle(const Point &min_point, const Point &max_point) {fill_rectangle(min_point.x, min_point.y, max_point.x, max_point.y);}
		/**
		 * Draw a filled rectangle
		 *
		 * @param r coordinates of rectangle
		 */
		virtual void fill_rectangle(const BBox &r) {fill_rectangle(r.min.x,r.min.y, r.max.x, r.max.y);}
		/**
		 * Draw the outline of a circle
		 *
		 * @param centre_point coordinates of centre of the circle
		 * @param radius radius of the circle
		 */
		virtual void circle(const Point &centre_point, int radius) {circle(centre_point.x, centre_point.y, radius);}
		/**
		 * Draw a filled circle
		 *
		 * @param centre_point coordinates of centre of the circle
		 * @param radius radius of the circle
		 */
		virtual void fill_circle(const Point &centre_point, int radius) {fill_circle(centre_point.x, centre_point.y, radius);}
		/**
		 * Draw an arc
		 *
		 * @param centre_point centre of circle arc is on
		 * @param start_point start of arc
		 * @param end_point point on line from centre to the end of the arc
		 */
		virtual void arc(const Point &centre_point, const Point &start_point, const Point &end_point) {arc(centre_point.x, centre_point.y, start_point.x, start_point.y, end_point.x, end_point.y);}
		/**
		 * Draw a segment of a circle
		 *
		 * @param centre_point centre of circle segment is on
		 * @param start_point start of segment
		 * @param end_point point on line from centre to the end of the segment
		 */
		virtual void segment(const Point &centre_point, const Point &start_point, const Point &end_point) {segment(centre_point.x, centre_point.y, start_point.x, start_point.y, end_point.x, end_point.y);}
		/**
		 * Draw a sector of a circle
		 *
		 * @param centre_point centre of circle sector is on
		 * @param start_point start of sector
		 * @param end_point point on line from centre to the end of the sector
		 */
		virtual void sector(const Point &centre_point, const Point &start_point, const Point &end_point) {sector(centre_point.x, centre_point.y, start_point.x, start_point.y, end_point.x, end_point.y);}
		/**
		 * Draw the outline of an ellipse
		 *
		 * @param centre_point centre of the ellipse
		 * @param intersect_point intersection of centre and edge of ellipse
		 * @param high_point highest (or lowest) point on the ellipse
		 */
		virtual void ellipse(const Point &centre_point, const Point &intersect_point, const Point &high_point) {ellipse(centre_point.x, centre_point.y, intersect_point.x, intersect_point.y, high_point.x, high_point.y);}
		/**
		 * Draw a filled ellipse
		 *
		 * @param centre_point centre of the ellipse
		 * @param intersect_point intersection of centre and edge of ellipse
		 * @param high_point highest (or lowest) point on the ellipse
		 */
		virtual void fill_ellipse(const Point &centre_point, const Point &intersect_point, const Point &high_point) {fill_ellipse(centre_point.x, centre_point.y, intersect_point.x, intersect_point.y, high_point.x, high_point.y);}

		// Text
		/**
		 * Draw text at the given location in the current WIMP font
		 *
		 * @param x x coordinate for base line of text
		 * @param y y coordinate for left of text
		 * @param text text to draw
		 */
		virtual void text(int x, int y, const std::string &text) = 0;
		/**
		 * Draw text at the given location in the given font
		 *
		 * @param x x coordinate for base line of text
		 * @param y y coordinate for left of text
		 * @param text text to draw
		 * @param font font to use to draw the text
		 */
		virtual void text(int x, int y, const std::string &text, const Font &font) = 0;
		/**
		 * Set the text colours for the current WIMP font
		 *
		 * @param foreground text foreground colour
		 * @param background text background colour
		 */
		virtual void text_colours(Colour foreground, Colour background) = 0;
		/**
		 * Set text colours for the given font
		 *
		 * @param font to set colours for
		 * @param foreground text foreground colour
		 * @param background text background colour
		 */
		virtual void text_colours(Font &font, Colour foreground, Colour background) = 0;
		/**
		 * Draw text at the given location in the current WIMP font
		 *
		 * @param pt coordinates for base line and left of text
		 * @param str text to draw
		 */
		virtual void text(const Point &pt, const std::string &str) {text(pt.x,pt.y,str);}
		/**
		 * Draw text at the given location in the given font
		 *
		 * @param pt coordinates for base line and left of text
		 * @param str text to draw
		 * @param font font to use to draw the text
		 */
		virtual void text(const Point &pt, const std::string &str, const Font &font) {text(pt.x,pt.y,str,font);}

		// Image
		/**
		 * Draw an image.
		 *
		 * @param x x coordinate for bottom left of image
		 * @param y x coordinate for bottom left of image
		 * @param image Image to draw
		 */
		virtual void image(int x, int y, const Image &image) = 0;
		/**
		 * Draw an image.
		 *
		 * @param pt coordinates for bottom left of image
		 * @param im Image to draw
		 */
		virtual void image(const Point &pt, const Image &im) {image(pt.x, pt.y, im);}

		/**
		 * Fill a draw path
		 *
		 * The path is drawn by transforming it so the user units used in it
		 * are treated as OS coordinates.
		 * 
		 * @param x coordinate of bottom left to place the path
		 * @param y coordinate of bottom left to place the path
		 * @param path the DrawPath to fill
		 * @param style style flags for filling. Default WINDING_NON_ZERO.
		 * @param flatness maximum distance allowed from beizer curve when flattening it in user units.
		 *  Defaults to 1 as this gives a good curve with the transform used.
		 */
		virtual void fill(int x, int y, const DrawPath &path, DrawFillStyle fill_style = WINDING_NON_ZERO, int flatness = 1) = 0;
		/**
		 * Fill a draw path
		 *
		 * The path is drawn by transforming it so the user units used in it
		 * are treated as OS coordinates.
		 * 
		 * @param pt coordinates of bottom left to place the path
		 * @param path the DrawPath to fill
		 * @param style style flags for filling. Default WINDING_NON_ZERO.
		 * @param flatness maximum distance allowed from beizer curve when flattening it in user units.
		 *  Defaults to 1 as this gives a good curve with the transform used.
		 */
		virtual void fill(const Point &pt,  const DrawPath &path, DrawFillStyle fill_style = WINDING_NON_ZERO, int flatness = 1) {fill(pt.x, pt.y, path, fill_style, flatness);}

		/**
		 * Plot the lines in a path
		 *
		 * The path is drawn by transforming it so the user units used in it
		 * are treated as OS coordinates.
		 *
		 * @param x coordinate of bottom left to place the path
		 * @param y coordinate of bottom left to place the path
		 * @param path the DrawPath to draw
		 * @param fill_style DrawFillStyle. Default WINDING_NON_ZERO.
		 * @param transform pointer to transformation matrix, or 0 for identity matrix
		 * @param flatness flatness.  Defaults to 1 as this gives a good curve with the transform used.
		 * @param thickness line thickness, or 0 for default.
		 * If the thickness is zero then the line is drawn with the minimum width that can be used, given the limitations of the pixel size (so lines are a single pixel wide).
		 * If the thickness is n, then the line will be drawn with a thickness of n/2 user coordinates translated to pixels on either side of the theoretical line position.
		 * If the line thickness is non-zero, then the cap and join parameter must also be passed.
		 * @param cap_and_join pointer to line cap and join specification (if required)
		 * @param pointer to dash pattern, or 0 for no dashes
		 */
		virtual void stroke(int x, int y, const DrawPath &path,DrawFillStyle fill_style = WINDING_NON_ZERO, int flatness = 1,
					  int thickness = 0, DrawCapAndJoin *cap_and_join = 0, DrawDashPattern *dashes = 0) = 0;

		/**
		 * Plot the lines in a path
		 *
		 * The path is drawn by transforming it so the user units used in it
		 * are treated as OS coordinates.
		 *
		 * @param pt coordinates of bottom left to place the path
		 * @param path the DrawPath to draw
		 * @param fill_style DrawFillStyle. Default WINDING_NON_ZERO.
		 * @param transform pointer to transformation matrix, or 0 for identity matrix
		 * @param flatness flatness.  Defaults to 1 as this gives a good curve with the transform used.
		 * @param thickness line thickness, or 0 for default.
		 * If the thickness is zero then the line is drawn with the minimum width that can be used, given the limitations of the pixel size (so lines are a single pixel wide).
		 * If the thickness is n, then the line will be drawn with a thickness of n/2 user coordinates translated to pixels on either side of the theoretical line position.
		 * If the line thickness is non-zero, then the cap and join parameter must also be passed.
		 * @param cap_and_join pointer to line cap and join specification (if required)
		 * @param pointer to dash pattern, or 0 for no dashes
		 */
		virtual void stroke(const Point &pt, const DrawPath &path, DrawFillStyle fill_style = WINDING_NON_ZERO, int flatness = 1,
			int thickness = 0, DrawCapAndJoin *cap_and_join = 0, DrawDashPattern *dashes = 0)
		{
			stroke(pt.x, pt.y, path, fill_style, flatness, thickness, cap_and_join, dashes);
		}
	};
}

#endif
