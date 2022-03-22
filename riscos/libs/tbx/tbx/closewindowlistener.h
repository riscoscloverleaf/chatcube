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


#ifndef TBX_CLOSEWINDOWLISTENER_H_
#define TBX_CLOSEWINDOWLISTENER_H_

#include "listener.h"
#include "eventinfo.h"
#include "window.h"

namespace tbx
{

/**
 * Listener for the close window request message.
 *
 * This message is received if the auto close option is not
 * set in a window.
 *
 * If it is OK to close the handle then window.hide() should
 * be called to actually close it.
 */
class CloseWindowListener : public Listener
{
public:
	virtual ~CloseWindowListener() {};

	/**
	 * Called when the close window request event is received
	 * by the window.
	 *
	 * Call window.hide() to actually close the window if that
	 * is required.
	 */
	virtual void close_window(const EventInfo &ev) = 0;
};

}

#endif /* TBX_CLOSEWINDOWLISTENER_H_ */
