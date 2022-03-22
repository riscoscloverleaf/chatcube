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
 * optionbutton.cc
 *
 *  Created on: 20-Aug-2009
 *      Author: alanb
 */

#include "optionbutton.h"
#include "pollinfo.h"

#include "optionbuttonstatelistener.h"

namespace tbx {

static void option_button_state_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	OptionButtonStateEvent event(id_block, data);
	static_cast<OptionButtonStateListener *>(listener)->option_button_state_changed(event);
}

/**
 * Add listener for option button state changed.
 * i.e. goes from on to off or off to on
 *
 * @param listener listener to add
 */
void OptionButton::add_state_listener(OptionButtonStateListener *listener)
{
	add_listener(0x82882, listener, option_button_state_router);
}

/**
 * Remove listener for option button state changed
 *
 * @param listener listener to remove
 */
void OptionButton::remove_state_listener(OptionButtonStateListener *listener)
{
	remove_listener(0x82882, listener);
}

}
