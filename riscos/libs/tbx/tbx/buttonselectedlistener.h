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

#ifndef TBX_BUTTONSELECTEDLISTENER_H_
#define TBX_BUTTONSELECTEDLISTENER_H_

#include "listener.h"
#include "eventinfo.h"
#include "actionbutton.h"

namespace tbx {

/**
 * Button has been selected event
 */
class ButtonSelectedEvent : public EventInfo
{
public:
	/**
	 * Construct the event.
	 */
	ButtonSelectedEvent(IdBlock &id_block, PollBlock &data) :
		EventInfo(id_block, data) {};

	/**
	 * Check if adjust was held down.
	 */
	bool adjust() const {return (_data.word[3] & 1)!=0;}

	/**
	 * Check if select was held down
	 */
	bool select() const {return (_data.word[3] & 4)!=0;}

	/**
	 * Check if the button was the default button activated by return
	 * or the cancel button executed by escape.
	 */
	bool return_or_escape() const {return (_data.word[3] & 7)==0;}

	/**
	 * Button clicked was the default button.
	 */
	bool default_button() const {return (_data.word[3] & 8)!=0;}

	/**
	 * Button clicked was the cancel button.
	 */
	bool cancel_button() const {return (_data.word[3] & 16)!=0;}

	/**
	 * Button clicked was a local button so the window was not
	 * automatically closed.
	 */
	bool local_button() const {return (_data.word[3] & 32)!=0;}
};

/**
 * Listener for button selected events
 */
class ButtonSelectedListener: public tbx::Listener {
public:
	/**
	 * Called when the button is selected.
	 */
	virtual void button_selected(ButtonSelectedEvent &event) = 0;
};

}
#endif /* BUTTONSELECTEDLISTENER_H_ */
