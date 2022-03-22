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

#ifndef TBX_SHOWSUBWINDOWSPEC
#define TBX_SHOWSUBWINDOWSPEC

#include "showfullspec.h"

namespace tbx
{
	/**
	 * Flags specifying how to align a subwindow in it's parent.
	 * It also can specify new furniture flags for a window.
	 */
	enum SubWindowFlags
	{
	   NEW_WINDOW_FLAGS = 1, //!< Use the window flags from the ShowSubWindowSpec structure
	   ALIGN_LEFT_VISIBLE_LEFT = 0x0010000, //!< Left edge linked to left of visible area
	   ALIGN_LEFT_VISIBLE_RIGHT = 0x0020000, //!< Left edge linked to right of visible area
	   ALIGN_BOTTOM_VISIBLE_BOTTOM = 0x0040000, //!< Bottom edge linked to bottom of visible area
	   ALIGN_BOTTOM_VISIBLE_TOP = 0x0080000, //!< Bottom edge linked to right of visible area
	   ALIGN_RIGHT_VISIBLE_LEFT = 0x0100000, //!< Right edge linked to left of visible area
	   ALIGN_RIGHT_VISIBLE_RIGHT = 0x0200000, //!< Right edge linked to right of visible area
	   ALIGN_TOP_VISIBLE_BOTTOM  = 0x0400000, //!< Top edge linked to bottom of visible area
	   ALIGN_TOP_VISIBLE_TOP = 0x0800000, //!< Top edge linked to top of visible area
	   ALIGN_X_SCROLL_VISIBLE_LEFT = 0x1000000, //!< X scroll linked to left of visible area
	   ALIGN_X_SCROLL_VISIBLE_RIGHT = 0x2000000, //!< X scroll linked to right of visible area
	   ALIGN_Y_SCROLL_VISIBLE_BOTTOM = 0x4000000, //!< Y scroll linked to bottom of visible area
	   ALIGN_Y_SCROLL_VISIBLE_TOP= 0x8000000  //!< Y scroll linked to top of visible area
	};

	/**
	 * Structure used to show a window as a subwindow.
	 *
	 * i.e. The window is a nested child of the parent.
	 */
	struct ShowSubWindowSpec : public ShowFullSpec
	{
		/**
		 * Window flags to use for the window.
		 *
		 * The flags parameter must include the NEW_WINDOW_FLAGS bit
		 * for this to be used.
		 */
		unsigned int window_flags;

		/**
		 * The parent window handle to make this window a subwindow of
		 *
		 * To get the window handle from a toolbox window use the
		 * Window::window_handle() method.
		 */
		WindowHandle wimp_parent;

		/**
		 * Control the use of the window_flags member and the
		 * alignment of the sub window inside its parent.
		 *
		 * Combine multiple flags using the "|" operator.
		 *
		 * @see SubWindowFlags for a list of flags available
		 */
		unsigned int flags;
	};
}

#endif
