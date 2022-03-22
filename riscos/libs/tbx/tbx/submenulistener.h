/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2015 Alan Buckley   All Rights Reserved.
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
 * submenulistener.h
 *
 *  Created on: 22 Jun 2015
 *      Author: alanb
 */

#ifndef SUBMENULISTENER_H_
#define SUBMENULISTENER_H_

#include "listener.h"
#include "point.h"

namespace tbx {

/**
 * Event details for submenu event
 */
class SubMenuEvent : public EventInfo
{
public:
	/**
	 * Construct the event.
	 */
	SubMenuEvent(IdBlock &id_block, PollBlock &data) :
		EventInfo(id_block, data) {};

	/**
	 * x coordinate where sub menu will be shown
	 */
	int top_left_x() const {return _data.word[0];}
	/**
	 * y coordinate where sub menu will be shown
	 */
	int top_left_y() const {return _data.word[1];}
	/**
	 * point where the sub menu will be shown
	 */
	Point top_left() const {return Point(_data.word[0],_data.word[1]);}
};

/**
 * Listener for event when mouse moves over the submenu arrow if no
 * other toolbox event has been associated with it.
 */
class SubMenuListener : public Listener
{
public:
	SubMenuListener() {}
	virtual ~SubMenuListener() {}

	/**
	 * Called when the user moves the mouse over the submenu arrow when
	 * no other toolbox event has been associated with the event.
	 *
	 * @param event Details of the event.
	 *              id_block().self_component() is the menu item,
	 *              id_block().self_object() is the menu
	 */
	virtual void submenu(const SubMenuEvent &event) = 0;
};

} /* namespace tbx */

#endif /* SUBMENULISTENER_H_ */
