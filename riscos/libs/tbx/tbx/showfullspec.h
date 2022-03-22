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

/*
 * showfullspec.h
 *
 *  Created on: 12-Sep-2008
 *      Author: alanb
 */

#ifndef SHOWFULLSPEC_H_
#define SHOWFULLSPEC_H_

#include "bbox.h"
#include "handles.h"
#include "visiblearea.h"

namespace tbx
{
	/**
	 * Structure used to specify exact position and size for an
	 * object derived from tbx::ShowFullObject.
	 */
	struct ShowFullSpec
	{
	public:
		/**
		 * Location of window on screen and scroll offsets
		 */
		VisibleArea visible_area;
		/**
		 * Window handle of window to show this above.
		 * Can also be one of the constants below
		 */
		WindowHandle wimp_window;

		/**
		 * Special window handles to specify where a window should be
		 * shown in the window stack.
		 */
		enum {
			WINDOW_SHOW_TOP = -1, //!< Show the window at the top of the window stack
			WINDOW_SHOW_BOTTOM = -2, //!< Show the window at the bottom of the window stack
			WINDOW_SHOW_BEHIND = -3  //!< Show the window below the desktop background
		};
	};
}

#endif /* SHOWFULLSPEC_H_ */
