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

#ifndef TBX_USEREVENTLISTENER_H_
#define TBX_USEREVENTLISTENER_H_

#include "listener.h"
#include "eventinfo.h"

namespace tbx {

/**
 * UserEvent class holds details for a UserEventListener.
 *
 */
class UserEvent : public EventInfo
{
public:
	/**
	 * Construct a user event listener with references to the underlying
	 * toolbox event.
	 */
	UserEvent(IdBlock &id_block, PollBlock &data) : EventInfo(id_block, data) {}

	/**
	 * Return a PollBlock with the raw data of the event.
	 */
	PollBlock &data() {return _data;}
};

/**
 * A UserEventListener can be used to capture any event on an object or
 * component.
 *
 * It allows a lower level access to the underlying toolbox event objects
 * and components and the data for the event.
 *
 * It is mainly used when the context of an user defined event id is
 * required.
 */
class UserEventListener: public tbx::Listener {
public:
	virtual ~UserEventListener() {}; //!< Destructor

	/**
	 * Called when the user event is occurs.
	 */
	virtual void user_event(UserEvent &event) = 0;
};

//! @cond INTERNAL
void user_event_router(IdBlock &id_block, PollBlock &data, Listener *listener);
//! @endcond

}

#endif /* USEREVENTLISTENER_H_ */
