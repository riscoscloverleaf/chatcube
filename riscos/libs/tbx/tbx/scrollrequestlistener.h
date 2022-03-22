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
 * scrollrequestlistener.h
 *
 *  Created on: 24 Nov 2010
 *      Author: alanb
 */

#ifndef TBX_SCROLLREQUESTLISTENER_H_
#define TBX_SCROLLREQUESTLISTENER_H_

#include "listener.h"
#include "eventinfo.h"
#include "window.h"

namespace tbx
{

/**
 * Information for ScrollRequest listener
 */
class ScrollRequestEvent : public EventInfo
{
public:
	/**
	 * Construct the event from Toolbox and WIMP event data
	 *
	 * @param id_block Toolbox IDs for this event
	 * @param poll_block Information returned from the WIMP for this event
	 */
	ScrollRequestEvent(IdBlock &id_block, PollBlock &poll_block) :
		EventInfo(id_block, poll_block)
		{}

	/**
	 * Window open information with details of visible area and current scroll setttings
	 */
	const WindowOpenInfo &open_info() const {return *reinterpret_cast<WindowOpenInfo *>(&_data.word[0]);}

	/**
	 * Enumeration of how the window should be scrolled in the X direction
	 */
	enum ScrollXDirection
	{
		PAGE_LEFT = -2, //!< Scroll left by one page
		LEFT = -1,		//!< Scroll left by one
		NONE_X = 0,     //!< No scrolling in X direction
		RIGHT = 1,      //!< Scroll right by one
		PAGE_RIGHT = 2  //!< Scroll right be one page
	};
	/**
	 * Enumeration of how the window should be scrolled in the y direction
	 */
	enum ScrollYDirection
	{
		PAGE_DOWN = -2, //!< Scroll down by one page
		DOWN = -1,		//!< Scroll down by one
		NONE_Y = 0, 	//!< No scrolling in Y direction
		UP = 1, 		//!< Scroll up by one
		PAGE_UP = 2		//!< Scroll up by one page
	};

	/**
	 * Direction requested for scroll in x direction
	 */
	ScrollXDirection x_scroll() const {return (ScrollXDirection)_data.word[8];}

	/**
	 * Direction requested for scroll in y direction
	 */
	ScrollYDirection y_scroll() const {return (ScrollYDirection)_data.word[9];}
};

/**
 * Scroll request listener.
 *
 * This listener is used to handle a scroll request event on a window
 * that has the flags set to generate this event.
 */
class ScrollRequestListener : public Listener
{
public:
	virtual ~ScrollRequestListener() {}

	/**
	 * Called when a scroll request event is generated.
	 *
	 * To process you should take a copy of the WindowOpenInfo, update
	 * the scroll fields and call Window::open.
	 *
	 * @param event details of scroll request
	 */
	virtual void scroll_request(const ScrollRequestEvent &event) = 0;
};

}

#endif /* TBX_SCROLLREQUESTLISTENER_H_ */
