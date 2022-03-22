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

#ifndef TBX_POLLINFO_H_
#define TBX_POLLINFO_H_

#include "handles.h"

#undef self_component
#undef parent_component
#undef ancestor_component

namespace tbx
{
	class EventRouter;
	class Listener;
	class Object;
	class Component;

	/**
	 * Information passed back by the Toolbox with each toolbox event
	 * providing information on where the event occurred.
	 */
	class IdBlock
	{
	private:
		ObjectId    ancestor_object_id;
		ComponentId ancestor_component_id;
		ObjectId    parent_object_id;
		ComponentId parent_component_id;
		ObjectId    self_object_id;
		ComponentId self_component_id;

		friend class EventRouter;

	public:
		/**
		 * Construct uninitialised IdBlock
		 */
		IdBlock() {}

		IdBlock(Object obj);
		Object self_object() const;
		Component self_component() const;
		Object parent_object() const;
		Component parent_component() const;
		Object ancestor_object() const;
		Component ancestor_component() const;
	};

	/**
	 * Structure holding the raw data from a call to Wimp_Poll.
	 */
	struct PollBlock
	{
		/**
		 * Array containing information return from Wimp_Poll
		 */
		int word[64];

		PollBlock() {}
		PollBlock(const PollBlock &other);
		PollBlock &operator=(const PollBlock &other);
	};

	/**
	 * Function type for handlers of raw (unprocessed) Toolbox events.
	 */
	typedef void (*RawToolboxEventHandler)(IdBlock &id_block, PollBlock &data, Listener *listener);
}

#endif /* TBX_POLLINFO_H_ */
