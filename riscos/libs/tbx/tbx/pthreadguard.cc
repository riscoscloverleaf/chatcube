/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2014 Alan Buckley   All Rights Reserved.
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
 * pthreadguard.cc
 *
 *  Created on: 6 Jun 2014
 *      Author: alanb
 */

#include "pthreadguard.h"
#include "application.h"
#include <pthread.h>

namespace tbx
{
PThreadGuard *PThreadGuard::_instance = 0;

PThreadGuard::PThreadGuard() {
}

PThreadGuard::~PThreadGuard() {
}

/**
 * Start the pthread guard if it is not already started
 *
 * This should be called before the running thread is created.
 *
 * Once started it stops threading during event processing as this
 * is likely to cause a crash and calls pthread_yield after event
 * processing to ensure running threads get a call on each event
 */
void PThreadGuard::start()
{
	if (_instance == 0)
	{
		_instance = new PThreadGuard();
		tbx::app()->set_pre_poll_listener(_instance);
		tbx::app()->set_post_event_listener(_instance);
	}
}

/**
 * Turns off the thread guard.
 *
 * Should only be called when there are no thread running
 */
void PThreadGuard::stop()
{
	if (_instance)
	{
		delete _instance;
		_instance = 0;
		tbx::app()->set_pre_poll_listener(0);
		tbx::app()->set_post_event_listener(0);
	}
}


/**
 * Called just before the wimp poll
 */
bool PThreadGuard::pre_poll()
{
	// Stop ticker over poll/event processing
	__pthread_stop_ticker();

	return true;
}

void PThreadGuard::post_event(int reason_code, const tbx::PollBlock &poll_block, const tbx::IdBlock &id_block, int reply_to)
{
	// Restart thread ticker
	__pthread_start_ticker();

	// Allow other threads to get a chance to run
	pthread_yield();
}

}
