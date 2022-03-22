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

#ifndef TBX_POINTERLISTENER_H_
#define TBX_POINTERLISTENER_H_

#include "listener.h"
#include "eventinfo.h"
#include "window.h"

namespace tbx
{

/**
 * Listener for the pointer leaving the window
 *
 * This event doesn't only occur when the pointer leaves the window's
 * visible work area, but whenever the window stops being the most visible
 * thing under the pointer. So, for example, popping up a menu at the
 * pointer position would cause this event.
 */

class PointerLeavingListener : public Listener
{
public:
	virtual ~PointerLeavingListener() {};

	/**
	 * Called when the pointer leaves the windows visible work area
	 */
	virtual void pointer_leaving(const EventInfo &ev) = 0;
};

/**
 * Listener for the pointer entering the window
 *
 * This event doesn't just happen when the pointer is physically moved
 * into a window's visible work area. It could occur because a menu is
 * removed or a window is closed, revealing a new uppermost window.
 */
class PointerEnteringListener : public Listener
{
public:
	virtual ~PointerEnteringListener() {};

	/**
	 * Called when the pointer enters the windows workspace
	 */
	virtual void pointer_entering(const EventInfo &ev) = 0;
};

}

#endif /* TBX_POINTERLISTENER_H_ */
