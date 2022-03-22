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
 * menuselectionlistener.h
 *
 *  Created on: 22 Jun 2015
 *      Author: alanb
 */

#ifndef MENUSELECTIONLISTENER_H_
#define MENUSELECTIONLISTENER_H_

#include "listener.h"

namespace tbx {

/**
 * Listener for selection of menu items that do not have another event
 * associated with them
 */
class MenuSelectionListener : public Listener
{
public:
	MenuSelectionListener() {}
	virtual ~MenuSelectionListener() {}

	/**
	 * Called when a menu item with no other event is selected
	 *
	 * @param event event info. id_block().self_component() is the menu item,
	 *                          id_block().self_object() is the menu
	 */
	virtual void menu_selection(const tbx::EventInfo &event) = 0;
};

} /* namespace tbx */

#endif /* MENUSELECTEDLISTENER_H_ */
