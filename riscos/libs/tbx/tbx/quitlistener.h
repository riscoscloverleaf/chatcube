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
 * quitlistener.h
 *
 *  Created on: 09-Jun-2009
 *      Author: alanb
 */

#ifndef TBX_QUITLISTENER_H_
#define TBX_QUITLISTENER_H_

#include "wimpmessagelistener.h"

namespace tbx {

class Application;
/**
 * Class to simplify handling of Quit WIMP message.
 *
 * Normally used internally by Application::add_quit_listener
 */
class QuitListener : public Listener
{
private:
	class MessageQuitListener : public WimpRecordedMessageListener
	{
		QuitListener *_quit_listener;
	public:
		MessageQuitListener(QuitListener *ql) : _quit_listener(ql) {};
		virtual ~MessageQuitListener() {};
		// Just forward all messages to the QuitListener quit
		virtual void recorded_message(WimpMessageEvent &event) {_quit_listener->quit();}

	} _message_listener;
	friend class tbx::Application;
public:
	QuitListener() : _message_listener(this) {};
	virtual ~QuitListener() {};

	/**
	 * Called when the application has be told to quit by the desktop.
	 */
	virtual void quit() = 0;
};

}

#endif /* TBX_QUITLISTENER_H_ */
