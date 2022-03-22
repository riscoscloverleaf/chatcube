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

#ifndef TBX_ICONBARCLICKLISTENER_H_
#define TBX_ICONBARCLICKLISTENER_H_

#include "listener.h"
#include "eventinfo.h"
#include "iconbar.h"

namespace tbx {

/**
 * Iconbar as been clicked event details
 */
class IconbarClickEvent : public EventInfo
{
public:
	/**
	 * Construct the event.
	 */
	IconbarClickEvent(IdBlock &id_block, PollBlock &data) : EventInfo(id_block, data) {};

	/**
	 * Check if click was with adjust
	 */
	bool adjust() const {return (_data.word[3] & 1)!=0;}

	/**
	 * Check if click was with select
	 */
	bool select() const {return (_data.word[3] & 4)!=0;}
};

/**
 * Listener for button selected events
 */
class IconbarClickListener: public tbx::Listener {
public:
	/**
	 * Method called when a click occurs on the icon bar
	 *
	 * @param event Information on icon bar and which mouse button
	 *              was used for the click.
	 */
	virtual void iconbar_clicked(IconbarClickEvent &event) = 0;
};

}

#endif
