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
 * prequitlistener.cc
 *
 *  Created on: 09-Jun-2009
 *      Author: alanb
 */

#include "prequitlistener.h"
#include "application.h"
#include "swis.h"
#include <algorithm>

namespace tbx
{

//! @cond INTERNAL
// Instance variable for pre quit listener managment
PreQuitManager *PreQuitManager::_instance = 0;
//! @endcond

/**
 * Construct a quit restarter that will just close the
 * current application when restart_quit is called.
 */
QuitRestarter::QuitRestarter() : _close_only(true)
{

}

/**
 * Copy constructor
 */
QuitRestarter::QuitRestarter(const QuitRestarter &other)
{
	_prequit_sender = other._prequit_sender;
	_close_only = other._close_only;
}

/**
 * Assignment operator
 */
QuitRestarter &QuitRestarter::operator=(const QuitRestarter &other)
{
	_prequit_sender = other._prequit_sender;
	_close_only = other._close_only;
	return *this;
}

/**
 * Construct a Quit restarter which will either restart
 * the desktop quit or just close the current application
 * depending on the parameters.
 *
 * @param sender task handle from the sender of the pre-quit message
 * @param close_only Close this application only don't restart desktop quit
 */
QuitRestarter::QuitRestarter(int sender, bool close_only) :
	_prequit_sender(sender), _close_only(close_only)
{
}



/**
 * Restart a quit application cancelled during a PreQuit message
 */
void QuitRestarter::restart_quit()
{
	if (_close_only) app()->quit();
	else
	{
		int block[7];
		_swix(Wimp_GetCaretPosition, _IN(1), block);
		block[6] = 0x1FC;
		_swix(Wimp_SendMessage, _INR(0,2), 8, block, _prequit_sender);
	}
}

/**
 * Construct PreQuitEvent from prequit message received from the WIMP
 *
 * @param m WimpMessage from prequit event
 * @param reply_to task to reply to to prevent the quit
 */
PreQuitEvent::PreQuitEvent(const WimpMessage &m, int reply_to) :
	_message(m),
	_reply_to(reply_to),
	_cancelled(false),
	_restarter(m.sender_task_handle(), ((m.block_size() >= 24) ? (m.word(5) & 1) : 0)!= 0)
{
}

/**
 * Cancel the quit preceeded by this pre-quit message
 */
void PreQuitEvent::cancel_quit()
{
	if (_cancelled) return; // Don't do it twice
	_cancelled = true;

	_message.your_ref(_message.my_ref());

	_message.send(WimpMessage::Acknowledge, _reply_to);
}

// Internal class handling prequit events
//! @cond INTERNAL

PreQuitManager::PreQuitManager()
{
	_instance = this;
}

void PreQuitManager::add_listener(PreQuitListener *listener)
{
	_listeners.push_back(listener);
}

void PreQuitManager::remove_listener(PreQuitListener *listener)
{
	std::vector<PreQuitListener *>::iterator found = std::find(_listeners.begin(), _listeners.end(), listener);
	if (found != _listeners.end())_listeners.erase(found);
}

void PreQuitManager::recorded_message(WimpMessageEvent &event, int reply_to)
{
	PreQuitEvent pqe(event.message(), reply_to);
	for (std::vector<PreQuitListener *>::iterator i = _listeners.begin();
			i != _listeners.end(); ++i)
	{
		(*i)->pre_quit(pqe);
	}
}

//! @endcond

}
