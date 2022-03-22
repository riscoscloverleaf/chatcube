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
 * modechangelistener.h
 *
 *  Created on: 25 Nov 2010
 *      Author: alanb
 */

#ifndef TBX_MODECHANGEDLISTENER_H_
#define TBX_MODECHANGEDLISTENER_H_

#include "wimpmessagelistener.h"

namespace tbx {

class Application;
/**
 * Class to simplify handling of Quit WIMP message.
 *
 * Normally used internally by Application::add_mode_changed_listener
 */
class ModeChangedListener : public Listener
{
private:
	class MessageModeChangedListener : public WimpRecordedMessageListener
	{
		ModeChangedListener *_mode_changed_listener;
	public:
		MessageModeChangedListener(ModeChangedListener *mcl) : _mode_changed_listener(mcl) {};
		virtual ~MessageModeChangedListener() {};
		// Just forward all messages to the ModeChangedListener mode_changed
		virtual void recorded_message(WimpMessageEvent &event) {_mode_changed_listener->mode_changed();}

	} _message_listener;
	friend class tbx::Application;
public:
	ModeChangedListener() : _message_listener(this) {};
	virtual ~ModeChangedListener() {};

	/**
	 * Called when the application has be told to Mode has changed by the desktop.
	 */
	virtual void mode_changed() = 0;
};

}

#endif /* TBX_MODECHANGEDLISTENER_H_ */
