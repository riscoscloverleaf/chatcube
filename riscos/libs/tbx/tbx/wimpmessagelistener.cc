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

#include "wimpmessagelistener.h"
#include "swixcheck.h"
#include "swis.h"

namespace tbx {

WimpMessage::WimpMessage(PollBlock &poll_block) :
	  _message_block(&(poll_block.word[0])),
	  _owns_block(false)
{
}

WimpMessage::WimpMessage(int message_id, int size)
{
	_message_block = new int[size];
	_message_block[0] = size * 4;
	_message_block[1] = _message_block[2] = _message_block[3] = 0;
	_message_block[4] = message_id;
	_owns_block = true;
}

WimpMessage::WimpMessage(const WimpMessage &other, int size_override /* = 0*/)
{
	int size = other._message_block[0]/4;
	if (size_override >= 4)
	{
		if (size > size_override) size = size_override;
	} else
		size_override = size;
	_message_block = new int[size_override];
	_owns_block = true;
	for (int j = 0; j < size; j++) _message_block[j] = other._message_block[j];
	_message_block[0] = size_override * 4;
}

WimpMessage::WimpMessage(const WimpMessage &other, bool copy)
{
	if (copy)
	{
		int size = other._message_block[0]/4;
		_message_block = new int[size];
		_owns_block = true;
		for (int j = 0; j < size; j++) _message_block[j] = other._message_block[j];
	} else
	{
		_message_block = other._message_block;
		_owns_block = false;
	}
}

WimpMessage &WimpMessage::operator=(const WimpMessage &other)
{
	int size = other._message_block[0]/4;
	if (_message_block[0] < other._message_block[0] || !_owns_block)
	{
		if (_owns_block) delete [] _message_block;
		_message_block = new int[size];
		_owns_block = true;
	}
	for (int j = 0; j < size; j++) _message_block[j] = other._message_block[j];

	return *this;
}


WimpMessage::~WimpMessage(void)
{
	if (_owns_block) delete _message_block;
}

/**
 *  Send a message to another application.
 *
 *  On exit sender task handle and my ref fields have been updated.
 *
 * @param type type of message (User, Recorded, Acknowledge)
 * @param destination target for the message. task handle, window handle, -2 for icon bar, 0 for broadcast
 * @param icon_handle icon handle to send to if destination is -2.
 *
 * @returns task handle of the destination.
 * @throws OsError if send fails
 */
int WimpMessage::send(SendType type, int destination, int icon_handle /*= 0*/)
{
	_kernel_swi_regs regs;

	regs.r[0] = type;
	regs.r[1] = (int)_message_block;
	regs.r[2] = destination;
	regs.r[3] = icon_handle;

	swix_check(_kernel_swi(0x400e7, &regs, &regs));

	return regs.r[2];
}


}
