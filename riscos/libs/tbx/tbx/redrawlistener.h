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

#ifndef REDRAWLISTENER_H_
#define REDRAWLISTENER_H_

#include "listener.h"
#include "eventinfo.h"
#include "bbox.h"
#include "pollinfo.h"
#include "visiblearea.h"

namespace tbx
{
    /**
     * Event passed to redraw listener to give details on
     * the area that needs a redraw.
     *
     * Also contains helper functions to convert between screen
     * area coordinates and work area coordinates.
     */
	class RedrawEvent : EventInfo
	{
	public:
		/**
		 * Construct the event from Toolbox and WIMP event data
		 *
		 * @param id_block Toolbox IDs for this event
		 * @param poll_block Information returned from the WIMP for this event
		 */
		RedrawEvent(IdBlock &id_block, PollBlock &poll_block) :
			EventInfo(id_block, poll_block)
			{
			}

		/**
		 * The visible area class giving details or screen area and scroll
		 * offset.
		 */
		const VisibleArea &visible_area() const {return *reinterpret_cast<const VisibleArea *>(&_data.word[1]);}

		/**
		 * The graphics clip in screen coordinates
		 */
		const BBox &clip() const {return *reinterpret_cast<const BBox *>(&_data.word[7]);}
	};

	/**
	 * Redraw event listener.
	 *
	 * Add to a window to be notified of each rectangle of the window
	 * that requires a redraw.
	 */
	class RedrawListener : public Listener
	{
	public:
		/**
		 * Override this function and repaint the area of
		 * window required.
		 *
		 * The area is clipped so you could redraw the whole
		 * window each time, but it is more efficient if you
		 * look the the events clip member to see what part
		 * of the window needs to be redrawn.
		 */
		virtual void redraw(const RedrawEvent &e) = 0;
	};
}

#endif /* REDRAWLISTENER_H_ */
