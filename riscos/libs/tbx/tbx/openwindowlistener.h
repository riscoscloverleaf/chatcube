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

#ifndef OPENWINDOWLISTENER_H_
#define OPENWINDOWLISTENER_H_

#include "bbox.h"
#include "handles.h"
#include "listener.h"
#include "eventinfo.h"
#include "window.h"

namespace tbx
{
	/**
	 * Event data for open window request listener
	 */
	class OpenWindowEvent : EventInfo
	{
	public:
		/**
		 * Constructs the event from the information returned by the WIMP
		 *
		 * @param id_block Toolbox ID block
		 * @param poll_block WIMP poll information
		 */
		OpenWindowEvent(IdBlock &id_block, PollBlock &poll_block) :
			EventInfo(id_block, poll_block) {}

		/**
		 * Get Window that generated this event
		 */
		Window window() const {return id_block().self_object();}

		/**
		 * Wimp window handle of window being opened
		 */
		WindowHandle handle() const {return _data.word[0];}

		/**
		 * Get visible area on screen
		 */
		const BBox &visible_area() const {return *reinterpret_cast<const BBox *>(&_data.word[1]);}

		/**
		 * Set visible area on screen
		 */
		void visible_area(const BBox &area) {_data.word[1] = area.min.x; _data.word[2] = area.min.y; _data.word[3] = area.max.x; _data.word[4] = area.max.y;}

		/**
		 * Get scroll offsets in work area
		 */
		const Point &scroll() const {return *reinterpret_cast<const Point *>(&_data.word[5]);}

		/**
		 * Set scroll offsets in work area
		 */
		void scroll(const Point &offsets) {_data.word[5] = offsets.x; _data.word[6] = offsets.y;}

		/**
		 * Get WIMP Window handle opened behind
		 */
		WindowHandle behind() const {return _data.word[7];}

		/**
		 * Set WIMP Window handle to open behind
		 */
		void behind(WindowHandle h) {_data.word[7] = h;}

		//TODO: Methods or constants for specific places like on top.
	};

	/**
	 * Listener for Wimp Open Window Request events
	 */
	class OpenWindowListener : public Listener
	{
	public:
		/**
		 * virtual function called when the application receives the open window
		 * request for the window.
		 *
		 * Note: The framework calls Wimp_OpenWindow automatically after
		 * all OpenWindowListeners are called for a window so do no call
		 * it in this routine.
		 *
		 * @param event details of the open request
		 */
		virtual void open_window(OpenWindowEvent &event) = 0;
	};
}

#endif /* WINDOWOPENLISTENER_H_ */
