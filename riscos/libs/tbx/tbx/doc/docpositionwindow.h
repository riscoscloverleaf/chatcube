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
 * DocPositionWindow.h
 *
 *  Created on: 14 Oct 2010
 *      Author: alanb
 */

#ifndef TBX_DOCPOSITIONWINDOW_H_
#define TBX_DOCPOSITIONWINDOW_H_

#include "../window.h"

namespace tbx
{
namespace doc
{

/**
 * Class to reposition each newly opened document window so
 * the titles don't overlap.
 *
 * To use it add a static Member to your Window class, make sure
 * the document window isn't automatically shown and call
 * the show method to show the window in the correct place.
 *
 */
class DocPositionWindow
{
private:
	tbx::Point _position;
	int _bottom;

public:
	DocPositionWindow();
	virtual ~DocPositionWindow() {};

	void show(tbx::Window window);
};


}
}

#endif /* TBX_DOCPOSITIONWINDOW_H_ */
