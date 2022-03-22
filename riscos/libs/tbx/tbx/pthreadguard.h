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
 * PThreadGuard.h
 *
 *  Created on: 6 Jun 2014
 *      Author: alanb
 */

#ifndef TBX_PTHREADGUARD_H_
#define TBX_PTHREADGUARD_H_

#include "prepolllistener.h"
#include "posteventlistener.h"

namespace tbx
{
/**
 * Helper class to stop pthreads from executing during event
 * processing which is unsafe
 */
class PThreadGuard :
		private tbx::PrePollListener,
		private tbx::PostEventListener
{

	PThreadGuard();
	~PThreadGuard();

	static PThreadGuard *_instance;

    virtual bool pre_poll();
    virtual void post_event(int reason_code, const tbx::PollBlock &poll_block, const tbx::IdBlock &id_block, int reply_to);

public:
	static void start();
	static void stop();
};

}

#endif /* TBX_PTHREADGUARD_H_ */
