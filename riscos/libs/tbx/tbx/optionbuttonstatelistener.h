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
 * optionbuttonstatelistener.h
 *
 *  Created on: 20-Aug-2009
 *      Author: alanb
 */

#ifndef TBX_OPTIONBUTTONSTATELISTENER_H
#define TBX_OPTIONBUTTONSTATELISTENER_H

#include "listener.h"
#include "eventinfo.h"

namespace tbx
{
/**
 * Event for Option button state changed
 */
class OptionButtonStateEvent : public EventInfo
{
public:
	/**
	 * Construct the event
	 */
	OptionButtonStateEvent(IdBlock &id_block, PollBlock &data) :
		EventInfo(id_block, data) {}

	/**
	 * Return true is now on else false
	 */
	bool turned_on() const {return (_data.word[4]!=0);}

	/**
	 * Return true if state changed with adjust
	 */
	bool adjust() const {return (_data.word[3] & 1) != 0;}

	/**
	 * Return true if state changed with select
	 */
	bool select() const {return (_data.word[3] & 4) != 0;}
};

/**
 * Listener for changes to the state of an option button.
 */
class OptionButtonStateListener : public Listener
{
public:
	OptionButtonStateListener() {}
	virtual ~OptionButtonStateListener() {}

	/**
	 * Option button state has changed
	 */
	virtual void option_button_state_changed(OptionButtonStateEvent &event) = 0;
};

}

#endif /* TBX_OPTIONBUTTONSTATELISTENER_H */
