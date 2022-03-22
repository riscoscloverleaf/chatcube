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
 * prequitlistener.h
 *
 *  Created on: 09-Jun-2009
 *      Author: alanb
 */

#ifndef TBX_PREQUITLISTENER_H_
#define TBX_PREQUITLISTENER_H_

#include "wimpmessagelistener.h"
#include "listener.h"
#include <vector>

namespace tbx
{
	/**
	 * Class to allow a quit cancelled during the prequit message
	 * to be restarted.
	 */
	class QuitRestarter
	{
		int _prequit_sender;
		bool _close_only;

	public:
		QuitRestarter();
		QuitRestarter(int sender, bool close_only);
		QuitRestarter(const QuitRestarter &other);

		QuitRestarter &operator=(const QuitRestarter &other);

		/**
		 * Set the QuitRestarter to close this application and not continue
		 * a desktop shutdown.
		 *
		 * @param close_only true to close this application only. It should only be set
		 * to false if the QuitRestarter was created as part of a prequit
		 * message.
		 */
		void close_application_only(bool close_only) {_close_only = close_only;}

		/**
		 * Check if this QuitRestarter will close the current application
		 * only.
		 * @return true if it will only close this application.
		 * 		   false if it will restart a desktop closedown
		 */
		bool close_application_only() const {return _close_only;}

		void restart_quit();

	};

	/**
	 * Event passed to pre quit listeners
	 */
	class PreQuitEvent
	{
	private:
		WimpMessage _message;
		int _reply_to;
		bool _cancelled;
		QuitRestarter _restarter;

	public:
		PreQuitEvent(const WimpMessage &m, int reply_to);
		void cancel_quit();

		/**
		 * Quit has already been cancelled by a previous
		 * pre-quit handler.
		 */
		bool cancelled() const { return _cancelled;}

		/**
		 * Return a class that can be used to restart the quit.
		 */
		QuitRestarter quit_restarter() const {return _restarter;}
	};

	/**
	 * Listen for the pre quit message from the desktop.
	 */
	class PreQuitListener : public Listener
	{
	public:
		/**
		 * Called when the prequit message has been received from the
		 * desktop.
		 *
		 * To stop the application being quitted call the cancel_quit
		 * method on the passed event.
		 *
		 * To restart the quit later take a copy of the QuitRestarter
		 * class from the event and call restart_quit on it when
		 * ready.
		 */
		virtual void pre_quit(PreQuitEvent &event) = 0;
	};

//! @cond INTERNAL
	/**
	 * Internal class to handle prequit listeners
	 */
	class PreQuitManager : public WimpRecordedMessageListener
	{
		static PreQuitManager *_instance;
		std::vector<PreQuitListener *> _listeners;

	public:
		PreQuitManager();
		static PreQuitManager *instance() {return _instance;}

		void add_listener(PreQuitListener *listener);
		void remove_listener(PreQuitListener *listener);

		virtual void recorded_message(WimpMessageEvent &event, int reply_to);
	};
//! @endcond

}

#endif /* TBX_PREQUITLISTENER_H_ */
