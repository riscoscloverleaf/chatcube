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

#ifndef TBX_DRAWPATH_H_
#define TBX_DRAWPATH_H_

#include "drawtransform.h"

namespace tbx
{
	class DrawPath;

	/**
	 * Enumeration to set the fill style when filling shapes.
	 *
	 * It should consist of one of the winding rules ored with
	 * the other values as required
	 */
	enum DrawFillStyle
	{
		WINDING_NON_ZERO, //!< non-zero winding number rule.
		WINDING_NEGATIVE, //!< negative winding number rule.
		WINDING_EVEN_ODD, //!<	even-odd winding number rule.
		WINDING_POSITIVE, //!<	positive winding number rule.
		PLOT_NON_BOUNDARY_EXTERIOR = 4, //!< plot non-boundary exterior pixels.
		PLOT_BOUNDARY_EXTERIOR = 8, //!< plot boundary exterior pixels.
		PLOT_BOUNDARY_INTERIOR = 16, //!< plot boundary interior pixels.
		PLOT_NON_BOUNDARY_INTERIOR = 32, //!< plot non-boundary interior pixels.
		PLOT_ALL_AT_ONCE = 0x80000000 //!< Used with DrawPath::stroke, will only plot each pixel once at the cost of more temporary workspace.
	};

	/**
	 * Class to set the cap and joins style for lines that are greater
	 * than a single pixel wide.
	 */
	class DrawCapAndJoin
	{
	public:
		enum JoinStyle {MITRED_JOINS, ROUND_JOINS, BEVELLED_JOINS};
		enum CapStyle {BUTT_CAPS, ROUND_CAPS, SQUARE_CAPS, TRIANGULAR_CAPS};

	private:
		JoinStyle _join_style;
		CapStyle _leading_cap_style;
		CapStyle _trailing_cap_style;
		int _reserved;
		Fixed16 _mitre_limit;
		int _leading_tri_cap;
		int _trailing_tri_cap;

		friend class DrawPath;

	public:
		/**
		 * Construct with ROUND_JOINS and ROUND_CAPS at both ends
		 */
		DrawCapAndJoin()
		{
			_join_style = ROUND_JOINS;
			_leading_cap_style = ROUND_CAPS;
			_trailing_cap_style = ROUND_CAPS;
			_reserved = 0;
			_mitre_limit = 1;
			_leading_tri_cap = 0x1010;
			_trailing_tri_cap = 0x1010;
		}

		/**
		 * Set the join style.
		 *
		 * If the style is set to MITRED_JOINS the mitre must be set as well.
		 *
		 * @param style the new join style
		 */
		void join(JoinStyle style) {_join_style = style;}
		/**
		 * Get the join style
		 *
		 * @returns the join style
		 */
		JoinStyle join() const {return _join_style;}
		/**
		 * Sets the mitre limit.
		 *
		 * The mitre limit must be set if the join style
		 * is set to MITRED_JOINS.
		 *
		 * At any given corner, the mitre length is the distance from the
		 * point at which the inner edges of the stroke meet, to the point
		 * where the outer edges of the stroke meet. This distance increases
		 * as the angle between the lines decreases. If the ratio of the mitre
		 * length to the line width exceeds the mitre limit, stroke treats the
		 * corner with a bevel join instead of a mitre join.
		 *
		 * @param limit new mitre limit
		 */
		void mitre_limit(const Fixed16 &limit) {_mitre_limit = limit;}
		/**
		 * Get the mitre limit
		 *
		 * @return mitre limit
		 */
		int mitre_limit() const {return _mitre_limit;}
		/**
		 * Set the leading cap style.
		 *
		 * If TRIANGULAR_CAPS is set, the leading cap width and length
		 * should also be set.
		 * @param cap_style new leading cap style
		 */
		void leading_cap(CapStyle cap_style) { _leading_cap_style = cap_style;}
		/**
		 * Get the leading cap style
		 *
		 * @returns CapStyle with leading cap style
		 */
		CapStyle leading_cap() const {return _leading_cap_style;}
		/**
		 * Set the trailing cap style.
		 *
		 * If TRIANGULAR_CAPS is set, the trailing cap width and length
		 * should also be set.
		 * @param cap_style new trailing cap style
		 */
		void trailing_cap(CapStyle cap_style) { _leading_cap_style = cap_style;}
		/**
		 * Get the trailing cap style
		 *
		 * @returns CapStyle with trailing cap style
		 */
		CapStyle trailing_cap() const {return _trailing_cap_style;}

		/**
		 * Leading triangular cap width on each side
		 *
		 * Set this if the leading cap style is TRIANGULAR_CAPS
		 *
		 * @param width cap width (in 256ths of line widths, so 0x100 is 1 linewidth)
		 */
		void leading_cap_width(short width) {_leading_tri_cap = (_leading_tri_cap & 0xFFFF0000) | width;};
		/**
		 * Get the leading cap width
		 *
		 * @returns the leading caps width in 256ths of a line width
		 */
		short leading_cap_width() const {return (short)(_leading_tri_cap & 0xFFFF);}
		/**
		 * Leading triangular cap length away from the line
		 *
		 * Set this if the leading cap style is TRIANGULAR_CAPS
		 *
		 * @param length cap length (in 256ths of line widths, so 0x100 is 1 linewidth)
		 */
		void leading_cap_length(short length) {_leading_tri_cap = (_leading_tri_cap & 0xFFFF) | (((int)length) << 16);}
		/**
		 * Get the leading cap length
		 *
		 * @returns the leading caps length in 256ths of a line width
		 */
		short leading_cap_length() const {return (short)(_leading_tri_cap >> 16);}
		/**
		 * Trailing triangular cap width on each side
		 *
		 * Set this if the trailing cap style is TRIANGULAR_CAPS
		 *
		 * @param width cap width (in 256ths of line widths, so 0x100 is 1 linewidth)
		 */
		void trailing_cap_width(short width) {_trailing_tri_cap = (_trailing_tri_cap & 0xFFFF0000) | width;};
		/**
		 * Get the trailing cap width
		 *
		 * @returns the trailing caps width in 256ths of a line width
		 */
		short trailing_cap_width() const {return (short)(_trailing_tri_cap & 0xFFFF);}
		/**
		 * Trailing triangular cap length away from the line
		 *
		 * Set this if the trailing cap style is TRIANGULAR_CAPS
		 *
		 * @param length cap length (in 256ths of line widths, so 0x100 is 1 linewidth)
		 */
		void trailing_cap_length(short length) {_trailing_tri_cap = (_trailing_tri_cap & 0xFFFF) | (((int)length) << 16);}
		/**
		 * Get the trailing cap length
		 *
		 * @returns the trailing caps length in 256ths of a line width
		 */
		short trailing_cap_length() const {return (short)(_trailing_tri_cap >> 16);}
	};

	/**
	 * Class to represent a dash pattern for lines
	 *
	 * Each element specifies a distance to draw in the present state. 
	 * The pattern starts with the draw on, and alternates off and on
	 * for each successive element. If it reaches the end of the pattern
	 * while drawing the line, then it will restart at the beginning.
	 *
	 * If the number of elements is odd, then the elements will alternate
	 * on or off with each pass through the pattern: so the first element
	 * will be on the first pass, off the second pass, on the third pass,
	 * and so on.
	 */
	class DrawDashPattern
	{
		int *_data;
		friend class DrawPath;

	public:
		/**
		 * Construct a dash pattern from an array of dashes
		 *
		 * @param start distance into dash pattern to start in user coordinates
		 * @param dashes array of integers specify the length of each on/off.
		 * @param count number of elements in the dash pattern
		 */
		DrawDashPattern(int start, int *dashes, int count)
		{
			_data = new int[count+2];
			_data[0] = start;
			_data[1] = count;
			for (int j = 0; j < count; j++) _data[j+2] = dashes[j];
		}

		/**
		 * Construct a dash pattern initialised to equal sized dashes
		 *
		 * @param start distance into dash pattern to start in user coordinates
		 * @param dash integer specifying the length of each on/off. Default 1.
		 * @param count number of elements in the dash pattern. Default 2.
		 */
		DrawDashPattern(int start, int dash = 1, int count = 2)
		{
			_data = new int[count+2];
			_data[0] = start;
			_data[1] = count;
			for (int j = 0; j < count; j++) _data[j+2] = dash;
		}

		/**
		 * Set the distance into the dash pattern of start
		 *
		 * @param value distance into the dash pattern of the start in user units
		 */
		void start(int value) {_data[0] = value;}
		/**
		 * Get the distance into the dash pattern of start
		 *
		 * @returns distance into the dash pattern of the start in user units
		 */
		int start() const {return _data[0];}
		/**
		 * Get the number of elements in the dash pattern
		 */
		int count() const {return _data[1];}
		/**
		 * Get reference to a dash element
		 *
		 * @param index 0 based index of dash element
		 * @returns reference to a dash element
		 */
		int &operator[](int index) {return _data[index+2];}
		/**
		 * Get value of a dash element
		 *
		 * @param index 0 based index of dash element
		 * @returns dash element size
		 */
		int operator[](int index) const {return _data[index+2];}
	};

	/**
	 * Base class for elements added to a draw path
	 */
	class DrawElement
	{
	public:
		/**
		 * Draw element types
		 */
		enum ElementType { END, CONTINUATION, MOVE, MOVE_INTERNAL, CLOSE_GAP, CLOSE_LINE, BEZIER, GAP, LINE };
	private:
		ElementType _type;
		friend class DrawPath;
	protected:
		DrawElement(ElementType type) : _type(type) {};
	public:
		ElementType type() const {return _type;}

		static int size_in_words(ElementType type);
	};

	/**
	 * End of path.
	 */
	class DrawElementEnd : public DrawElement
	{
		int _left;
		friend class DrawPath;

	public:
		DrawElementEnd() : DrawElement(DrawElement::END), _left(0) {}
	};

	/**
	 * Pointer to continuation of path.
	 */
	class DrawElementContinuation : public DrawElement
	{
	public:
		/**
		 * Construct Continuation path element
		 * @param iptr pointer to DrawElement starting the path
		 */
		DrawElementContinuation(DrawElement *iptr) : DrawElement(DrawElement::CONTINUATION), ptr(iptr) {};

		DrawElement *ptr;
	};

	/**
	 * Move to (x, y) starting new subpath. The new subpath does affect winding numbers and so is filled normally.
	 * This is the normal way to start a new subpath.
	 */
	class DrawElementMove : public DrawElement
	{
	public:
		/**
		 * Construct a new move element
		 *
		 * @param ix x coordinate in user coordinates
		 * @param iy y coordinate in user coordinates
		 */
		DrawElementMove(int ix, int iy) : DrawElement(DrawElement::MOVE), x(ix), y(iy) {};

		int x;
		int y;
	};

	/**
	 * Move to (x, y) starting new subpath. The new subpath does not affect winding numbers when filling.
	 * This is mainly for internal use and rarely used by applications.	 
	 */
	class DrawElementMoveInternal : public DrawElement
	{
	public:
		/**
		 * Construct a new move element
		 *
		 * @param ix x coordinate in user coordinates
		 * @param iy y coordinate in user coordinates
		 */
		DrawElementMoveInternal(int ix, int iy) : DrawElement(DrawElement::MOVE_INTERNAL), x(ix), y(iy) {};

		int x;
		int y;
	};

	/**
	 * Close path with a gap element
	 */
	class DrawElementCloseGap : public DrawElement
	{
	public:
		DrawElementCloseGap() : DrawElement(DrawElement::CLOSE_GAP) {};
	};

	/**
	 * Close path with a line element.
	 * It is better to use one of these two to close a subpath than 2 or 3, because this guarantees a closed subpath.
	 */
	class DrawElementCloseLine : public DrawElement
	{
	public:
		DrawElementCloseLine() : DrawElement(DrawElement::CLOSE_LINE) {};
	};

	/**
	 * Bezier curve to element
	 */
	class DrawElementBezier : public DrawElement
	{
	public:
		DrawElementBezier(int icx1, int icy1, int icx2, int icy2, int ix, int iy) : DrawElement(DrawElement::BEZIER), cx1(icx1), cy1(icy1), cx2(icx2), cy2(icy2), x(ix), y(iy) {}
		int cx1; //!< First control x coordinate
		int cy1; //!< First control y coordinate
		int cx2; //!< Second control x coordinate
		int cy2; //!< Second control y coordinate
		int x;   //!< End point x coordinate
		int y;   //!< End point y coordinate
	};

	/**
	 * Gap to (x, y) Element. Do not start a new subpath. 
	 *
	 * Mainly for internal use in dot-dash sequences.
	 */
	class DrawElementGap : public DrawElement
	{
	public:
		/*
		 * Construct a new gap element
		 *
		 * @param ix x coordinate in user coordinates
		 * @param iy y coordinate in user coordinates
		 */
		DrawElementGap(int ix, int iy) : DrawElement(DrawElement::GAP), x(ix), y(iy) {};

		int x;
		int y;
	};

	/**
	 * Line to (x, y) Element.
	 */
	class DrawElementLine : public DrawElement
	{
	public:
		/*
		 * Construct a new line element
		 *
		 * @param ix x coordinate in user coordinates
		 * @param iy y coordinate in user coordinates
		 */
		DrawElementLine(int ix, int iy) : DrawElement(DrawElement::LINE), x(ix), y(iy) {};

		int x;
		int y;
	};

	/**
	 * Class to represent, display and manipulate a graphical
	 * path used by the Draw RISC OS module.
	 *
	 * A path is created by adding DrawElements to the path.
	 *
	 * Convenience methods are included to add the common draw elements
	 * without need to construct a draw element first.
	 *
	 * The units used for coordinates in a path are user defined, they
	 * are converted to screen coordinates by a transform when the path
	 * it filled or stroked.
	 */
	class DrawPath
	{
	private:
		int *_data;
		int _size;
		int _capacity;

		/**
		 * Helper to ensure an element can be added
		 */
		void ensure_space(int needed) { if (_size + needed > _capacity) capacity(_size + needed + 8);}

	public:
		DrawPath(int capacity = 64);
		~DrawPath();

		void add(const DrawElement &element);

		void capacity(int capacity);
		/**
		 * Get capacity in words (1 word = 4 bytes)
		 */
		int capacity() const {return _capacity;}

		void end_path();
		void move(int x, int y);
		void close_gap();
		void close_line();
		void bezier(int cx1, int cy1, int cx2, int cy2, int x, int y);
		void gap(int x, int y);
		void line(int x, int y);

		void circle(int x, int y, int radius);

		void fill(DrawFillStyle fill_style = WINDING_NON_ZERO, DrawTransform *transform = 0, int flatness = 0) const;
		void stroke(DrawFillStyle fill_style = WINDING_NON_ZERO, DrawTransform *transform = 0, int flatness = 0,
					  int thickness = 0, DrawCapAndJoin *cap_and_join = 0, DrawDashPattern *dashes = 0) const;

		//TODO: Output to path functions

	};

};

#endif
