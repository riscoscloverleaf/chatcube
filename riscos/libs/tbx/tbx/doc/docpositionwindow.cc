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
 * DocPositionWindow.cpp
 *
 *  Created on: 14 Oct 2010
 *      Author: alanb
 */

#include "docpositionwindow.h"
#include "../modeinfo.h"
#include <iostream>

namespace tbx
{
namespace doc
{

/**
 * Constructor, sets it up to read the position
 * for the first call
 */
DocPositionWindow::DocPositionWindow()
{
	_position.y = -999;
}

/**
 * Show the given window at the next position
 *
 * On the first call it reads the position from the window and
 * shows it in it's default position.
 * Subsequent calls show the window moved down by xx units
 *
 * When the window position gets close to the bottom of the screen
 * it starts again near the top of the screen.
 */
void DocPositionWindow::show(tbx::Window window)
{
	if (_position.y == -999)
	{
		tbx::BBox wbox = window.bounds();
		_position = wbox.top_left();
		_bottom = 64 + wbox.height();
		window.show();
	} else
	{
		_position.y -= 32;
		if (_position.y < _bottom)
		{
			// Reached bottom so move to near top
			tbx::ModeInfo mi;
			_position.y = mi.screen_size().height - 64;
		}
		window.show(_position);
	}
}

}
}
