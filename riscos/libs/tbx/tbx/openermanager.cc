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
 * openermanager.cc
 *
 *  Created on: 23 Nov 2010
 *      Author: alanb
 */

#include "openermanager.h"
#include "application.h"

namespace tbx {

OpenerManager *OpenerManager::_instance = 0;

OpenerManager::OpenerManager() : _first(0)
{
	_instance = 0;

	// Register for messages we handle
	app()->add_recorded_message_listener(5, this); // DataOpen
}

OpenerManager::~OpenerManager()
{
	if (_first)
	{
		OpenerItem *next;
		while (_first)
		{
			next = _first->next;
			delete _first;
			_first = next;
		}
	}
}

/**
 * Process the data open event
 */
void OpenerManager::recorded_message(WimpMessageEvent &event, int reply_to)
{
	int file_type = event.message().word(10);
	OpenerItem *check = _first;
	bool accepted = false;
	LoadEvent load_event(Object(), Gadget(), 0, 0, // Not used for opener
			  -1, // Size not specified
			  file_type,
			  event.message().str(11), // File name
			  true); // Event is always from filer

	while(check && !accepted)
	{
		if (check->file_type == file_type
				|| check->file_type == -2)
		{
			accepted = check->loader->accept_file(load_event);
		}
		if (!accepted) check = check->next;
	}

	if (accepted)
	{
		// It's what we are interested in
		tbx::WimpMessage reply(event.message());
		reply.message_id(4); // DataLoadAck
		reply.your_ref(event.message().my_ref());

		reply.send(tbx::WimpMessage::Acknowledge, reply_to);
		event.claim();

		// Now load the file
		check->loader->load_file(load_event);
	}
}

/**
 * Add an opener to the manager.
 */
void OpenerManager::add_opener(int file_type, Loader *loader)
{
	OpenerItem *item = new OpenerItem;
	item->file_type = file_type;
	item->loader = loader;
	item->next = 0;
	if (_first == 0)
	{
		_first = item;
	} else
	{
		OpenerItem *check = _first;
		while (check->next != 0) check = check->next;
		check->next = item;
	}
}

/**
 * Remove an opener from the manager
 */
void OpenerManager::remove_opener(int file_type, Loader *loader)
{
	OpenerItem *check = _first, *prev = 0;
	while (check && (check->file_type != file_type || check->loader != loader))
	{
		prev = check;
		check = check->next;
	}

	if (check)
	{
		if (prev) prev->next = check->next;
		else _first = check->next;
		delete check;
	}
}

}
